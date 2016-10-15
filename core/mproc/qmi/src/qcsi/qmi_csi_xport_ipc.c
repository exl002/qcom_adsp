/******************************************************************************
  @file    qmi_csi_xport_xxx.c
  @brief   The QMI Common Service Interface (CSI) transport adapter

  DESCRIPTION
  QMI Common Service Interface transport adapter module for IPC router
   
  ---------------------------------------------------------------------------
  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi_target.h"
#include "qmi_csi.h"
#include "qmi_csi_common.h"
#include "ipc_router_core.h"

#define MAX_TX_BUFS (30)

/* Tx queues for handling flow control:
 * +--------+  +--------+  +--------+
 * | q head |->| dest 1 |->| dest 2 |->...
 * +--------+  +--------+  +--------+
 *                 |            |
 *                buf 1        buf 1
 *                 |            |
 *                buf 2        buf 2
 *                 |            |
 *                ...          ...
 */
typedef struct buf_s
{
  LINK(struct buf_s, link);
  void *msg;
  uint32 len;
  ipc_router_address_type src_addr;
  boolean confirm_rx;
  boolean disconnect;
} buf_type;

typedef struct dest_s
{
  ipc_router_address_type *addr;
  LINK(struct dest_s, link);
  LIST(struct buf_s, bufs);
} dest_type;

struct qmi_csi_xport_handle
{
  qmi_csi_xport_type *xport;
  ipc_router_client_type *handle;
  qmi_csi_os_params sig;
  qmi_csi_lock_type rx_q_lock;
  LIST(struct buf_s, rx_q);
  qmi_csi_lock_type tx_q_lock;
  LIST(struct dest_s, tx_q);
  qmi_csi_xport_options_type *xport_options;
};



static dest_type *find_tx_q(struct qmi_csi_xport_handle *xp, ipc_router_address_type *addr)
{
  dest_type *dest = LIST_HEAD(xp->tx_q);
  while(dest)
  {
    if(!memcmp(addr, dest->addr, sizeof(ipc_router_address_type)))
      break;
    dest = dest->link.next;
  }
  return dest;
}

static dest_type *get_tx_q(struct qmi_csi_xport_handle *xp, ipc_router_address_type *addr)
{
  dest_type *dest = find_tx_q(xp, addr);
  if(!dest)
  {
    dest = CALLOC(1, sizeof(dest_type));
    if(dest)
    {
      dest->addr = addr;
      LIST_ADD(xp->tx_q, dest, link);
    }
  }
  return dest;
}

static void purge_tx_q(struct qmi_csi_xport_handle *xp, dest_type *dest)
{
  buf_type *buf = LIST_HEAD(dest->bufs);
  LIST_REMOVE(xp->tx_q, dest, link);
  while(buf)
  {
    buf_type *to_free = buf;
    FREE(buf->msg);
    buf = buf->link.next;
    FREE(to_free);
  }
  FREE(dest);
}

static void qmi_csi_rx_cb
(
 ipc_router_client_type     *client,
 void                       *cb_data,
 ipc_router_packet_type     *msg,
 ipc_router_address_type    *src_addr,
 uint32                      len,
 boolean                     confirm_rx
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)cb_data;
  buf_type *buf;

  buf = CALLOC(1, sizeof(buf_type));
  if(!buf)
  {
    ipc_router_packet_free(&msg);
    QMI_CSI_OS_LOG_ERROR();
    return;
  }

  /* linearize data */
  buf->msg = MALLOC(len);
  if(!buf->msg)
  {
    /* message too large for memory, try to allocate just the header and
     * hand it up the stack for error response
     */
    QMI_CSI_OS_LOG_ERROR();
    buf->msg = MALLOC(QMI_HEADER_SIZE);
    if(!buf->msg)
    {
      /* unrecoverable error at this point -- dropping msg */
      QMI_CSI_OS_LOG_ERROR();
      FREE(buf);
      ipc_router_packet_free(&msg);
      return;
    }
    ipc_router_packet_read(&msg, buf->msg, QMI_HEADER_SIZE);
    buf->len = QMI_HEADER_SIZE;
  }
  else
  {
    ipc_router_packet_read(&msg, buf->msg, len);
    buf->len = len;
  }
  buf->src_addr = *src_addr;
  buf->confirm_rx = confirm_rx;

  ipc_router_packet_free(&msg);

  LOCK(&xp->rx_q_lock);
  LIST_ADD(xp->rx_q, buf, link);
  UNLOCK(&xp->rx_q_lock);
  QMI_CSI_OS_SIGNAL_SET(&xp->sig);

}

static void qmi_csi_event_cb
(
 ipc_router_client_type *client,
 void *cb_data,
 ipc_router_event_type event,
 ipc_router_event_info_type *info
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)cb_data;

  if(!xp)
    return;

  switch(event)
  {
    case IPC_ROUTER_EVENT_RESUME_TX:
    {
      dest_type *dest;
      buf_type *buf;
      LOCK(&xp->tx_q_lock);
      dest = find_tx_q(xp, &info->client);
      if(dest)
      {
        buf = LIST_HEAD(dest->bufs);
        while(buf)
        {
          /* try transmitting */
          if(ipc_router_core_send(client, dest->addr, buf->msg, buf->len) 
              == IPC_ROUTER_STATUS_SUCCESS)
          {
            buf_type *to_free = buf;
            LIST_REMOVE(dest->bufs, buf, link);
            buf = buf->link.next;
            FREE(to_free->msg);
            FREE(to_free);
          }
          else
            break;
        }
      }
      UNLOCK(&xp->tx_q_lock);
      break;
    }
    case IPC_ROUTER_EVENT_NEW_SERVER:
      break;
    case IPC_ROUTER_EVENT_REMOVE_SERVER:
      break;
    case IPC_ROUTER_EVENT_REMOVE_CLIENT:
    {
      buf_type *buf;
      dest_type *dest;
      LOCK(&xp->tx_q_lock);
      dest = find_tx_q(xp, &info->client);
      if(dest)
        purge_tx_q(xp, dest);
      UNLOCK(&xp->tx_q_lock);

      /* queue an event to Rx queue */
      buf = CALLOC(1, sizeof(buf_type));
      if(!buf)
      {
        return;
      }

      buf->src_addr = info->client;
      buf->disconnect = TRUE;

      LOCK(&xp->rx_q_lock);
      LIST_ADD(xp->rx_q, buf, link);
      UNLOCK(&xp->rx_q_lock);
      QMI_CSI_OS_SIGNAL_SET(&xp->sig);

      break;
    }
    case IPC_ROUTER_EVENT_CLOSED:
    {
      dest_type *dest, *dest_to_free;

      qmi_csi_xport_closed(xp->xport);
      LOCK_DEINIT(&xp->rx_q_lock);

      LOCK(&xp->tx_q_lock);
      dest = LIST_HEAD(xp->tx_q);
      while(dest)
      {
        dest_to_free = dest;
        dest = dest->link.next;
        purge_tx_q(xp, dest_to_free);
      }
      UNLOCK(&xp->tx_q_lock);

      LOCK_DEINIT(&xp->tx_q_lock);
      FREE(xp);
      break;
    }
    default:
      break;
  }
}

static void *xport_open
(
 void *xport_data,
 qmi_csi_xport_type *xport,
 uint32_t max_rx_len,
 qmi_csi_os_params *os_params,
 qmi_csi_xport_options_type *options
 )
{
  int rc;
  struct qmi_csi_xport_handle *xp = CALLOC(1, sizeof(struct qmi_csi_xport_handle));

  /* Be backwards compatible with old IPC Router interface */
#ifdef IPC_ROUTER_CORE_PORT_OPTIONS_INIT
  ipc_router_core_options_type ipc_options;

  IPC_ROUTER_CORE_PORT_OPTIONS_INIT(ipc_options);
#endif

#ifdef IPC_ROUTER_CORE_PORT_OPTIONS_SET_SCOPE
  if(options && options->scope_valid)
  {
    IPC_ROUTER_CORE_PORT_OPTIONS_SET_SCOPE(ipc_options, options->scope);
  }
#endif

  if(xp)
  {
    LOCK_INIT(&xp->tx_q_lock);
    LOCK_INIT(&xp->rx_q_lock);
    xp->xport = xport;
    xp->xport_options = options;
    xp->sig = *os_params;
#ifdef IPC_ROUTER_CORE_PORT_OPTIONS_INIT
    rc = ipc_router_core_open_with_options(&xp->handle, 0, qmi_csi_rx_cb, xp, 
                                           qmi_csi_event_cb, xp, &ipc_options);
#else
    rc = ipc_router_core_open(&xp->handle, 0, qmi_csi_rx_cb, xp, qmi_csi_event_cb, xp);
#endif

    if(rc != IPC_ROUTER_STATUS_SUCCESS)
    {
      LOCK_DEINIT(&xp->tx_q_lock);
      LOCK_DEINIT(&xp->rx_q_lock);
      FREE(xp);
      return NULL;
    }
  }

  return xp;
}

static qmi_csi_error xport_reg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 )
{
  int rc;
  ipc_router_service_type service;
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;

  service.type = service_id;
  service.instance = version;

  rc = ipc_router_core_reg_server(xp->handle, &service);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  return QMI_CSI_NO_ERR;
}

static qmi_csi_error xport_unreg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 )
{
  int rc;
  ipc_router_service_type service;
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;

  service.type = service_id;
  service.instance = version;

  rc = ipc_router_core_dereg_server(xp->handle, &service);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  return QMI_CSI_NO_ERR;
}


static qmi_csi_error xport_send
(
 void *handle,
 void *addr,
 uint8_t *msg,
 uint32_t len,
 uint32_t flags,
 void **client_data
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;
  dest_type *dest;
  int rc;
  uint32_t max_queue_len = 0;

  if(0 != (flags & QMI_CSI_SEND_FLAG_RATE_LIMITED) && NULL != xp->xport_options)
  {
    max_queue_len = xp->xport_options->rate_limited_queue_size;
  }

  LOCK(&xp->tx_q_lock);

  dest = find_tx_q(xp, addr);
  if(dest && LIST_CNT(dest->bufs))
  {
    /* queue to the end so message is not sent out of order */
    rc = IPC_ROUTER_STATUS_BUSY;
  }
  else
  {
    rc = ipc_router_core_send(xp->handle, addr, msg, len);
    if(rc != IPC_ROUTER_STATUS_SUCCESS && rc != IPC_ROUTER_STATUS_BUSY)
    {
      /* unrecoverable error */
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_INTERNAL_ERR;
    }
  }

  if(rc == IPC_ROUTER_STATUS_BUSY)
  {
    /* queue to tx queue */
    buf_type *buf;

    dest = get_tx_q(xp, addr);
    if(!dest)
    {
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_INVALID_HANDLE;
    }
    if(max_queue_len > 0 && LIST_CNT(dest->bufs) >= max_queue_len)
    {
      /* too many in the queue, return error */
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_CONN_BUSY;
    }
    buf = CALLOC(1, sizeof(buf_type));
    if(!buf)
    {
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_NO_MEM;
    }
    buf->len = len;
    buf->msg = MALLOC(len);
    if(!buf->msg)
    {
      FREE(buf);
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_NO_MEM;
    }
    memcpy(buf->msg, msg, len);
    LIST_ADD(dest->bufs, buf, link);
  }
  UNLOCK(&xp->tx_q_lock);
  return QMI_CSI_NO_ERR;
}

static void xport_close
(
 void *handle
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;
  ipc_router_core_close(xp->handle);
}

static uint32_t xport_addr_len
(
 void
 )
{
  return sizeof(ipc_router_address_type);
}

static buf_type *dequeue_rx(struct qmi_csi_xport_handle *xp)
{
  buf_type *buf;
  LOCK(&xp->rx_q_lock);
  buf = LIST_HEAD(xp->rx_q);
  if(buf)
    LIST_REMOVE(xp->rx_q, buf, link);
  UNLOCK(&xp->rx_q_lock);
  return buf;
}

static void xport_handle_event
(
 void *handle,
 qmi_csi_os_params *os_params
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;
  buf_type *buf;

  while((buf = dequeue_rx(xp)) != NULL)
  {
    if(buf->disconnect)
    {
      qmi_csi_xport_disconnect(xp->xport, &buf->src_addr);
    }
    else
    {
      if(buf->confirm_rx)
      {
        ipc_router_core_send_conf(xp->handle, &buf->src_addr);
      }
      qmi_csi_xport_recv(xp->xport, &buf->src_addr, buf->msg, buf->len);
      FREE(buf->msg);
    }
    FREE(buf);
  }
}

qmi_csi_xport_ops_type qcsi_ipc_ops = {
  NULL, /* Legacy Open */
  xport_reg,
  xport_unreg,
  NULL, /* Legacy Send */
  xport_handle_event,
  xport_close,
  xport_addr_len,
  xport_open,
  xport_send
};
