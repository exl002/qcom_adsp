/*===========================================================================

                    I P C    R O U T E R   Q D I

   This file describes the QDI Driver implementation of IPC Router.

 Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_qdi_driver.c#1 $
$DateTime: 2013/04/03 17:22:53 $
$Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/12   aep     Initial Creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include <stddef.h>
#include "qurt_qdi_driver.h"
#include "ipc_router_core.h"
#include "ipc_router_qdi_driver.h"


/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

#define LIST_HEAD(type) struct {\
  type   *head;           \
  type   *tail;           \
  uint32 count;           \
  ipc_router_os_sem lock; \
}

#define LIST_INIT(ptr) do { \
  (ptr)->head = (ptr)->tail = NULL;\
  (ptr)->count = 0; \
  ipc_router_os_sem_init(&(ptr)->lock); \
} while(0)

#define LIST_LOCK(ptr) ipc_router_os_sem_lock(&ptr->lock)
#define LIST_UNLOCK(ptr)  ipc_router_os_sem_unlock(&ptr->lock)

/*===========================================================================
                      FORWARD TYPE DECLARATIONS
===========================================================================*/
typedef struct data_buf_s data_buf_t;
typedef struct event_buf_s event_buf_t;
typedef struct ipc_router_user_ep_s ipc_router_user_ep_t;
typedef struct ipc_router_qdi_handle_s ipc_router_qdi_handle_t;

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/** Data buffer type (Queue entry) */
struct data_buf_s {
  void                    *cookie;
  int                     fd;
  ipc_router_packet_type  *pkt;
  uint32                  len;
  ipc_router_address_type src_addr;
  uint32                  confirm_rx;
  data_buf_t              *next;
};

/** Event buffer type (Queue entry) */
struct event_buf_s {
  void                       *cookie;
  int                        fd;
  uint32                     event;
  ipc_router_event_info_type info;
  event_buf_t                *next;
};

typedef struct {
  int local;
  int remote;
} ipc_router_qdi_sig_type;

/** QDI Device handle (One for each client process) */
struct ipc_router_qdi_handle_s {
  qurt_qdi_obj_t            obj; /* Has to be the first element */
  int                       qdi_handle;

  LIST_HEAD(event_buf_t)    event_q;
  LIST_HEAD(data_buf_t)     rx_q;

  struct {
    ipc_router_qdi_sig_type group;
    ipc_router_qdi_sig_type event;
    ipc_router_qdi_sig_type data;
  } signal;

  ipc_router_user_ep_t      *clients;
  unsigned int              clients_count;
  int                       next_fd;
  ipc_router_os_sem         clients_lock;
  qurt_anysignal_t          cleanup_sig;
};

/** QDI User Endpoint handle (One for each endpoint (Router client)) */
struct ipc_router_user_ep_s 
{
  ipc_router_client_type  *handle;
  ipc_router_qdi_handle_t *dev;
  int                     fd;
  void                    *cookie;
  unsigned int            ref_count;
  ipc_router_user_ep_t    *next;
};

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
/* For Debug */
static ipc_router_qdi_handle_t *ipc_router_qdi_latest_handle = NULL;

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      ipc_router_qdi_get_obj

DESCRIPTION   Search through the device's (handle) endpoint list and returns
              the object associated with the particular endpoint file 
              descriptor. 

ARGUMENTS     handle - Handle associated with this endpoint
              fd - The endpoint's file descriptor. 

RETURN VALUE  EP Handle if successful (Found) NULL otherwise.

SIDE EFFECTS  The endpoint's reference count is incremented upon success.

FUTURE OPT.    This can be optimized to be a search in a hash table 
              (Rather than a list to reduce the average search time)
===========================================================================*/
static ipc_router_user_ep_t *ipc_router_qdi_get_obj(ipc_router_qdi_handle_t *handle, int fd)
{
  ipc_router_user_ep_t *ep = NULL;
  if(!handle)
  {
    return NULL;
  }
  ipc_router_os_sem_lock(&handle->clients_lock);
  ep = handle->clients;
  while(ep)
  {
    if(ep->fd == fd)
    {
      ep->ref_count++;
      break;
    }
    ep = ep->next;
  }
  ipc_router_os_sem_unlock(&handle->clients_lock);
  return ep;
}

/*===========================================================================
FUNCTION      cleanup_client_queues

DESCRIPTION   Removes all data and event packets which are destined to a
              particular endpoint.

ARGUMENTS     handle - Handle associated with this endpoint

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void cleanup_client_queues(ipc_router_user_ep_t *handle)
{
  ipc_router_qdi_handle_t *dev = handle->dev;
  data_buf_t *data_buf, *data_prev = NULL;
  event_buf_t *event_buf, *event_prev = NULL;

  /* Remove all queued packets */
  ipc_router_os_sem_lock(&dev->rx_q.lock);
  data_buf = dev->rx_q.head;
  while(data_buf)
  {
    data_buf_t *to_free = NULL;
    if(data_buf->fd == handle->fd)
    {
      if(data_prev)
      {
        data_prev->next = data_buf->next;
      }
      else
      {
        dev->rx_q.head = data_buf->next;
      }
      dev->rx_q.count--;
      to_free = data_buf;
      data_buf = data_buf->next;
      ipc_router_packet_free(&to_free->pkt);
      ipc_router_os_free(to_free);
    }
    else 
    {
      data_prev = data_buf;
      data_buf = data_buf->next;
    }
  }
  dev->rx_q.tail = data_prev;
  ipc_router_os_sem_unlock(&dev->rx_q.lock);
  
  /* Remove all queued events */
  ipc_router_os_sem_lock(&dev->event_q.lock);
  event_buf = dev->event_q.head;
  while(event_buf)
  {
    event_buf_t *to_free = NULL;
    if(event_buf->fd == handle->fd)
    {
      if(event_prev)
      {
        event_prev->next = event_buf->next;
      }
      else
      {
        dev->event_q.head = event_buf->next;
      }
      dev->event_q.count--;
      to_free = event_buf;
      event_buf = event_buf->next;
      ipc_router_os_free(to_free);
    }
    else 
    {
      event_prev = event_buf;
      event_buf = event_buf->next;
    }
  }
  dev->event_q.tail = event_prev;
  ipc_router_os_sem_unlock(&dev->event_q.lock);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_put_obj

DESCRIPTION   Decrements the reference count, and releases the endpoint
              if the reference is 0.

ARGUMENTS     fd - The endpoint's handle.

RETURN VALUE  None

SIDE EFFECTS  None

FUTURE OPT.   This can be optimized to be a search in a hash table 
             (Rather than a list to reduce the average search time)
===========================================================================*/
void ipc_router_qdi_put_obj(ipc_router_user_ep_t *ep)
{
  ipc_router_qdi_handle_t *handle = NULL;

  if(!ep || !ep->dev)
  {
    return;
  }

  handle = ep->dev;

  ipc_router_os_sem_lock(&handle->clients_lock);
  ep->ref_count--;
  
  if(ep->ref_count == 0)
  {
    ipc_router_user_ep_t *i = handle->clients;
    ipc_router_user_ep_t *prev = NULL;
    while(i)
    {
      if(i->fd == ep->fd)
      {
        if(prev)
        {
          prev->next = i->next;
        }
        else
        {
          handle->clients = i->next;
        }
        handle->clients_count--;
        break;
      }
      prev = i;
      i = i->next;
    }
    if(i && i == ep)
    {
      cleanup_client_queues(ep);
      ipc_router_os_free(ep);
    }
    else
    {
      /* Log error */
    }
  }
  ipc_router_os_sem_unlock(&handle->clients_lock);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_event_cb

DESCRIPTION   Callback function called by IPC Router Core when there is a
              new event for an endpoint.

ARGUMENTS     unused -the router core client handle (Not used)
              cb_data - the cookie provided on open (the endpoint handle)
              event - the event
              info - the data associated with the event.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_event_cb
(
    ipc_router_client_type *unused,
    void *cb_data,
    ipc_router_event_type event,
    ipc_router_event_info_type *info)
{
  ipc_router_user_ep_t *clnt = (ipc_router_user_ep_t *)cb_data;
  ipc_router_qdi_handle_t *handle = NULL;
  event_buf_t *buf;

  if(!clnt || !clnt->dev)
  {
    /* TODO Log error */
    return;
  }
  handle = clnt->dev;

  buf = ipc_router_os_malloc(sizeof(*buf));
  if(!buf)
  {
    return;
  }
  buf->next  = NULL;
  buf->event = (uint32)event;
  buf->cookie= clnt->cookie;
  buf->fd    = clnt->fd;
  buf->info  = *info;

  if(event == IPC_ROUTER_EVENT_CLOSED)
  {
    /* IPC Router Core releases the reference */
    ipc_router_qdi_put_obj(clnt);
    qurt_anysignal_set(&handle->cleanup_sig, 0x1);
  }

  /* Queue up the event. */
  ipc_router_os_sem_lock(&handle->event_q.lock);
  if(handle->event_q.head)
  {
    handle->event_q.tail->next = buf;
    handle->event_q.tail = buf;
  }
  else
  {
    handle->event_q.head = handle->event_q.tail = buf;
  }
  handle->event_q.count++;
  ipc_router_os_sem_unlock(&handle->event_q.lock);

  qurt_qdi_signal_set(handle->signal.event.local);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_rx_cb

DESCRIPTION   Callback function called from IPC Router Core when there is a
              packet to be read.

ARGUMENTS     client - The router core client.
              cb_data - the cookie provided on open (endpoint handle).
              msg - IPC router packet  type.
              src_addr - the address of the source.
              len - the length of the packet.
              confirm_rx - flag if true implies that the source is requesting
                           confirmation.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_rx_cb
(
    ipc_router_client_type *client,
    void *cb_data,
    ipc_router_packet_type *msg,
    ipc_router_address_type *src_addr,
    uint32 len,
    boolean confirm_rx)
{
  ipc_router_user_ep_t *clnt = (ipc_router_user_ep_t *)cb_data;
  ipc_router_qdi_handle_t *handle;
  data_buf_t *buf;

  if(!clnt || !clnt->dev)
  {
    return;
  }
  handle = clnt->dev;

  buf = ipc_router_os_malloc(sizeof(*buf));
  if(!buf)
  {
    /* TODO Log event */
    ipc_router_packet_free(&msg);
    return;
  }

  buf->cookie     = clnt->cookie;
  buf->fd         = clnt->fd;
  buf->src_addr   = *src_addr;
  buf->confirm_rx = (uint32)confirm_rx;
  buf->len        = len;
  buf->next       = NULL;
  buf->pkt        = msg;

  ipc_router_os_sem_lock(&handle->rx_q.lock);
  if(handle->rx_q.tail)
  {
    handle->rx_q.tail->next = buf;
    handle->rx_q.tail = buf;
  }
  else
  {
    handle->rx_q.tail = handle->rx_q.head = buf;
  }
  handle->rx_q.count++;
  ipc_router_os_sem_unlock(&handle->rx_q.lock);

  qurt_qdi_signal_set(handle->signal.data.local);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_compare

DESCRIPTION   Compare utility function passed to ipc_router core to
              perform the compare operation when looking for services.

ARGUMENTS     inst1 - The instance ID of service A
              inst2 - The instance ID of service B
              compare_data - cookie, which is the mask.

RETURN VALUE  TRUE when the two instances are the same, FALSE otherwise.

SIDE EFFECTS  None
===========================================================================*/
static boolean ipc_router_qdi_compare
(
  uint32 inst1,
  uint32 inst2,
  void *compare_data
)
{
  uint32 mask = (uint32)(uintptr_t)compare_data;

  return (inst1 & mask) == (inst2 & mask) ? TRUE : FALSE;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_event_read_invocation

DESCRIPTION   Allows an endpoint to read a broadcasted event.

ARGUMENTS     client_handle - The calling client's handle
              handle - Handle for this device
              out_event - out event ID.
              out_info  - Out info of this event.
              out_cookie - The cookie associated with the endpoint.

RETURN VALUE  Negative error code on failure, positive bytes read into
              out_info. 0 if there are no events to be read.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_event_read_invocation
(
    int client_handle,
    ipc_router_qdi_handle_t *handle,
    uint32 *out_event,
    ipc_router_event_info_type *out_info,
    void *out_cookie
)
{
  event_buf_t *buf;
  int rc;

  if(!handle || !out_event || !out_info || !out_cookie)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  ipc_router_os_sem_lock(&handle->event_q.lock);
  buf = handle->event_q.head;
  if(!buf)
  {
    ipc_router_os_sem_unlock(&handle->event_q.lock);
    return 0;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_event, &buf->event, sizeof(uint32));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_info, &buf->info, sizeof(ipc_router_event_info_type));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_cookie, &buf->cookie, sizeof(void *));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  handle->event_q.head = handle->event_q.head->next;
  handle->event_q.count--;
  if(!handle->event_q.head)
  {
    handle->event_q.tail  = NULL;
  }

  ipc_router_os_free(buf);
  rc = (int)sizeof(ipc_router_event_info_type);
bail:
  ipc_router_os_sem_unlock(&handle->event_q.lock);

  return rc;
}
/*===========================================================================
FUNCTION      ipc_router_qdi_peek

DESCRIPTION   returns the size of the next packet in the RX queue

ARGUMENTS     client_handle - Handle identifying the client
              handle - The handle for this device.

RETURN VALUE  If there are packets in the RX Queue, the size of the
              next packet. 0 otherwise.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_peek_invocation
(
  int client_handle, 
  ipc_router_qdi_handle_t *handle 
)
{
  int ret = 0;

  ipc_router_os_sem_lock(&handle->rx_q.lock);
  if(handle->rx_q.head)
  {
    ret = handle->rx_q.head->len;
  }
  ipc_router_os_sem_unlock(&handle->rx_q.lock);
  return ret;
}


/*===========================================================================
FUNCTION      ipc_router_qdi_read_invocation

DESCRIPTION   returns a buffer sent to this endpoint.

ARGUMENTS     client_handle - Handle identifying the client
              handle - The handle for this device.
              out_buf - the buffer to place the packet
              in_max_len - the size of the provided buffer.
              out_proc_id - the source processor ID.
              out_port_id - the source port ID.
              out_confirm_rx - flag if set requesting the endpoint to 
                              send a confirmation (send_conf)
              cookie - the cookie associated with this endpoint
                       (Provided by the client on  open)

RETURN VALUE  Negative error code on failure, positive number of bytes
              read into out_buf. 0 if there is no packet to be read.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_read_invocation
(
    int client_handle,
    ipc_router_qdi_handle_t *handle,
    void *out_buf,
    uint32 in_max_len,
    ipc_router_address_type *out_addr,
    uint32 *out_confirm_rx,
    void *cookie
)
{
  data_buf_t *buf;
  int rc;
  void *return_buf = NULL;
  if(!handle || !out_buf || !in_max_len || 
      !out_addr || !out_confirm_rx || !cookie)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  ipc_router_os_sem_lock(&handle->rx_q.lock);
  buf = handle->rx_q.head;
  if(!buf)
  {
    ipc_router_os_sem_unlock(&handle->rx_q.lock);
    return 0;
  }

  if(buf->len > in_max_len)
  {
    ipc_router_os_sem_unlock(&handle->rx_q.lock);
    return IPC_ROUTER_QDI_NO_SPACE_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, out_buf, buf->len, QDI_PERM_W, (void **)&return_buf);
  if(rc || !return_buf)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_lock_buffer(client_handle, out_addr, sizeof(ipc_router_address_type), QDI_PERM_W, (void **)&out_addr);
  if(rc || !out_addr)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_addr, &buf->src_addr, sizeof(ipc_router_address_type));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_confirm_rx, &buf->confirm_rx, sizeof(uint32));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_copy_to_user(client_handle, cookie, &buf->cookie, sizeof(void *));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = ipc_router_packet_read(&buf->pkt, return_buf, buf->len);

  /* Remove the head buffer from the queue */
  handle->rx_q.head = handle->rx_q.head->next;
  handle->rx_q.count--;
  if(!handle->rx_q.head)
  {
    handle->rx_q.tail = NULL;
  }

  if((unsigned int)rc != buf->len)
  {
    rc = IPC_ROUTER_QDI_INTERNAL_ERR;
  }

  ipc_router_packet_free(&buf->pkt);
  ipc_router_os_free(buf);

bail:
  ipc_router_os_sem_unlock(&handle->rx_q.lock);
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_find_servers_invocation

DESCRIPTION   Query the number and location of services of a particular
              type with (instance & mask) being the same.

ARGUMENTS     client_handle - Handle identifying the client
              handle - The handle of this endpoint.
              service - The service discretion (ID and Instance).
              servers - (Optional) pointers to place the address of the found servers.
              num_entries - (Optional) In: Number of servers the above array can hold
                                       Out: Total populated servers.
              num_servers - Total number of found services.
              mask - The mask to be used during compare operations on instance ID.
                      (mask = 0xffffffff implies that only services with the same
                      instance ID will be returned).

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_find_servers_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_service_type *service,
    ipc_router_server_type *servers, 
    uint32 *num_entries, 
    uint32 *num_servers,
    uint32 mask
)
{
  ipc_router_client_type *client = NULL;
  int rc;

  if(!num_servers)
    return IPC_ROUTER_QDI_PARAM_ERR;

  if(handle)
  {
    client = handle->handle;
  }

  if((servers && !num_entries) ||
      (!servers && num_entries))
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, service, sizeof(ipc_router_service_type), QDI_PERM_R, (void **)&service);
  if(rc || !service)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, num_servers, sizeof(uint32), QDI_PERM_RW, (void **)&num_servers);
  if(rc || !num_servers)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(servers)
  {
    /* If servers is valid, the so will be num_entries */
    rc = qurt_qdi_lock_buffer(client_handle, num_entries, sizeof(uint32), QDI_PERM_RW, (void **)&num_entries);
    if(rc || !num_entries)
    {
      return IPC_ROUTER_QDI_PERM_ERR;
    }

    /* num_entries was locked above for read and write, so we can directly access it here 
     * servers might be read by the ipc_router_core function even though it is 
     * strictly an out parameter, just make sure we have permissions for read */
    rc = qurt_qdi_lock_buffer(client_handle, servers, *num_entries * sizeof(ipc_router_server_type),
            QDI_PERM_RW, (void **)&servers);
    if(rc || !servers)
    {
      return IPC_ROUTER_QDI_PERM_ERR;
    }
  }

  return ipc_router_core_find_all_servers(
      client, service, servers, num_entries, num_servers,
      ipc_router_qdi_compare, (void *)(uintptr_t)mask);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_unregister_invocation

DESCRIPTION   Unregisters a previously registered service on this endpoint

ARGUMENTS     client_handle - Handle identifying the client
              handle - client handle for this endpoint
              service - The service discretion (ID and Instance).

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_unregister_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_service_type *service
)
{
  int rc;

  if(!handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, service, sizeof(ipc_router_service_type), QDI_PERM_R, (void **)&service);
  if(rc || !service)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_dereg_server(handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_register_invocation

DESCRIPTION   Register a service on this endpoint.

ARGUMENTS     client_handle - Handle identifying the client
              handle - Client handle for this endpoint.
              service - The service discretion (ID and Instance).

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_register_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_service_type *service
)
{
  int rc;

  if(!handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, service, sizeof(ipc_router_service_type), QDI_PERM_R, (void **)&service);
  if(rc || !service)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_reg_server(handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send_conf_invocation

DESCRIPTION   Send a TX confirmation control message to a destination port.

ARGUMENTS     client_handle - Handle identifying the client
              handle - the client handle provided by open,
              addr - Address of the destination

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_send_conf_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_address_type *addr
)
{
  int rc;

  if(!handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, addr, sizeof(ipc_router_address_type), QDI_PERM_R, (void **)&addr);
  if(rc || !addr)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_send_conf(handle->handle, addr);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send_invocation

DESCRIPTION   Invocation to handle client request to send a buffer to a 
              remote client.

ARGUMENTS     client_handle - Handle identifying the client
              handle - Client handle provided on open
              addr - Address of the destination
              user_buf - The buffer to send to the destination,
              len - the length of the buffer

RETURN VALUE  0 on success, error code on failure. 
              IPC_ROUTER_STATUS_BUSY if the particular port is busy.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_send_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_address_type *addr,
    void *user_buf,
    uint32 len
)
{
  int rc;

  if(!user_buf || !len || !handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }
  
  rc = qurt_qdi_lock_buffer(client_handle, addr, sizeof(ipc_router_address_type), QDI_PERM_R, (void **)&addr);
  if(rc || !addr)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(qurt_qdi_lock_buffer(client_handle, user_buf, len, QDI_PERM_R, (void **)&user_buf))
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_send(handle->handle, addr, user_buf, len);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_close_invocation

DESCRIPTION   handles the request from a client to release a router handle.
              (Needs the clients_lock to be held by the caller).

ARGUMENTS     client_handle - Handle identifying the client
              handle - handle to release

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  Unlocks the clients_lock
===========================================================================*/
static inline int ipc_router_qdi_close_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle
)
{
  ipc_router_qdi_handle_t *dev;

  if(!handle || !handle->handle || !handle->dev)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  dev = handle->dev;

  /* Signal any user thread which might be waiting so, it has a chance to 
   * shutdown. */
  qurt_qdi_signal_set(dev->signal.event.local);
  qurt_qdi_signal_set(dev->signal.data.local);

  /* This handle will be freed in the close callback */
  ipc_router_core_close(handle->handle);

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_open_invocation

DESCRIPTION   invocation called when the remote client requests for an 
              IPC router handle. 

ARGUMENTS     client_handle - Identify the caller
              handle - The handle to this device.
              out_clnt_handle - The pointer to place the output handle.
              port_id - port ID required.
              cookie - Cookie to be associated with this opening handle.

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_qdi_open_invocation
(
  int client_handle,
  ipc_router_qdi_handle_t *handle,
  int *out_clnt_handle,
  uint32 port_id,
  void *cookie
)
{
  int rc;
  ipc_router_user_ep_t *clnt = NULL;

  if(!handle || !out_clnt_handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  clnt = ipc_router_os_malloc(sizeof(*clnt));
  if(!clnt)
  {
    return IPC_ROUTER_QDI_NO_MEM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, out_clnt_handle, sizeof(int), QDI_PERM_W, (void **)&out_clnt_handle);
  if(rc || !out_clnt_handle)
  {
    ipc_router_os_free(clnt);
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  clnt->dev = handle;
  clnt->cookie = cookie;
  clnt->ref_count = 1; /* One reference to ipc_router_core */

  rc = ipc_router_core_open(&clnt->handle, port_id, 
      ipc_router_qdi_rx_cb, clnt, 
      ipc_router_qdi_event_cb, clnt);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_os_free(clnt);
    return rc;
  }

  ipc_router_os_sem_lock(&handle->clients_lock);
  if(handle->next_fd < 1)
  {
    handle->next_fd = 1;
  }
  *out_clnt_handle = clnt->fd = handle->next_fd++;

  clnt->next = handle->clients;
  handle->clients = clnt;
  handle->clients_count++;
  ipc_router_os_sem_unlock(&handle->clients_lock);

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_handle_release

DESCRIPTION   Cleanup everything associated with a handle. Called when a
              process calls deinit or when it exits (Hopefully QuRT closes
              pending handles when a process exits).

ARGUMENTS     handle - The handle associated with this open object.

RETURN VALUE  None

SIDE EFFECTS  All clients will be closed on the behalf of the remote clients.
===========================================================================*/
static inline void ipc_router_qdi_handle_release
(
  ipc_router_qdi_handle_t *handle 
)
{
  data_buf_t *buf, *buf_to_free;
  event_buf_t *event, *event_to_free;
  ipc_router_user_ep_t *i;

  /* Auto close all clients on this device */
  ipc_router_os_sem_lock(&handle->clients_lock);
  i = handle->clients;
  /* Assumption, when this device is closed, then
   * we are not expecting any more "Additions" to the list 
   * so it is safe to unlock the list, as there will
   * be no more additions */
  ipc_router_os_sem_unlock(&handle->clients_lock);

  while(i)
  {
    ipc_router_user_ep_t *to_free = i;
    i = i->next;
    ipc_router_core_close(to_free->handle);
    /* i will be freed & unlinked when its ref goes to 0. */
  }

  /* Wait till all clients are freed (reference count of handle
   * goes to 0) so we can continue to destroy it */
  ipc_router_os_sem_lock(&handle->clients_lock);
  while(handle->clients_count > 0)
  {
    ipc_router_os_sem_unlock(&handle->clients_lock);
    qurt_anysignal_wait(&handle->cleanup_sig, 0x1);
    qurt_anysignal_clear(&handle->cleanup_sig, 0x1);
    ipc_router_os_sem_lock(&handle->clients_lock);
  }
  ipc_router_os_sem_unlock(&handle->clients_lock);

  /* Remove all queued buffers */
  ipc_router_os_sem_lock(&handle->rx_q.lock);
  buf = handle->rx_q.head;
  handle->rx_q.head = handle->rx_q.tail = NULL;
  handle->rx_q.count = 0;
  while(buf)
  {
    ipc_router_packet_free(&buf->pkt);
    buf_to_free = buf;
    buf = buf->next;
    ipc_router_os_free(buf_to_free);
  }
  ipc_router_os_sem_unlock(&handle->rx_q.lock);

  /* Remove all queued events */
  ipc_router_os_sem_lock(&handle->event_q.lock);
  event = handle->event_q.head;
  handle->event_q.head = handle->event_q.tail = NULL;
  handle->event_q.count = 0;
  while(event)
  {
    event_to_free = event;
    event = event->next;
    ipc_router_os_free(event_to_free);
  }
  ipc_router_os_sem_unlock(&handle->event_q.lock);

  /* Deinit all locks */
  ipc_router_os_sem_deinit(&handle->event_q.lock);
  ipc_router_os_sem_deinit(&handle->rx_q.lock);
  ipc_router_os_sem_deinit(&handle->clients_lock);
  qurt_anysignal_destroy(&handle->cleanup_sig);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_main_invocation

DESCRIPTION   funtion which will be invoked when any calls are made to the
              QDI driver. This is the starting point on handling all 
              router methods.

ARGUMENTS     ptr - Usually the open handle.
              method - The method of the invocation.
              a1 - a8 - The arguments passed in the invocation

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_qdi_main_invocation
(
 int client_handle,
 qurt_qdi_obj_t *ptr,
 int method,
 qurt_qdi_arg_t a1,
 qurt_qdi_arg_t a2,
 qurt_qdi_arg_t a3,
 qurt_qdi_arg_t a4,
 qurt_qdi_arg_t a5,
 qurt_qdi_arg_t a6,
 qurt_qdi_arg_t a7,
 qurt_qdi_arg_t a8,
 qurt_qdi_arg_t a9
)
{
  ipc_router_qdi_handle_t *handle = (ipc_router_qdi_handle_t *)ptr;
  /* For most calls, the first parameter is the client handle */
  ipc_router_user_ep_t *client = NULL;

  int ret = IPC_ROUTER_QDI_INTERNAL_ERR;

  if(!handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  switch(method)
  {
    case IPC_ROUTER_QDI_OPEN:
      ret = ipc_router_qdi_open_invocation(client_handle, handle, a1.ptr, (uint32)a2.num, a3.ptr);
      break;

    case IPC_ROUTER_QDI_CLOSE:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_close_invocation(client_handle, client);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_SEND:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_send_invocation(client_handle, client, a2.ptr, a3.ptr, (uint32)a4.num);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_SEND_CONF:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_send_conf_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_REGISTER_SERVER:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_register_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_UNREGISTER_SERVER:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_unregister_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_FIND_ALL_SERVERS:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_find_servers_invocation(client_handle, client, a2.ptr, a3.ptr, a4.ptr, 
                                                            a5.ptr, (uint32)a6.num);
      ipc_router_qdi_put_obj(client);
      break;

    /* The following are for the common process reader thread, and hence do not 
     * require the client handle provided */

    case IPC_ROUTER_QDI_PEEK:
      ret = ipc_router_qdi_peek_invocation(client_handle, handle);
      break;

    case IPC_ROUTER_QDI_READ:
      ret = ipc_router_qdi_read_invocation(client_handle, handle, a1.ptr, (uint32)a2.num, 
                                           a3.ptr, a4.ptr, a5.ptr);
      break;

    case IPC_ROUTER_QDI_EVENT_READ:
      ret = ipc_router_qdi_event_read_invocation(client_handle, handle, a1.ptr, a2.ptr, a3.ptr);
      break;

    default:
      ret = qurt_qdi_method_default(client_handle, ptr, method, a1, a2, a3, a4, a5, a6, a7, a8, a9);
      break;
  }

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_deinit_invocation

DESCRIPTION   Handles an deinit call (Technically this is the invocation for
              qurt_qdi_close(), but named deinit to avoid confusion with the
              actual IPC Router close call) which initializes a process's
              connection to the QDI driver.

ARGUMENTS     ptr - object to release

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_deinit_invocation
(
  qurt_qdi_obj_t *ptr
)
{
  ipc_router_qdi_handle_t *handle = (ipc_router_qdi_handle_t *)ptr;

  if(!handle)
  {
    return;
  }

  ipc_router_qdi_handle_release(handle);
  ipc_router_os_free(handle);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_init_invocation

DESCRIPTION   Handles an init call (Technically this is the invocation for
              qurt_qdi_open(), but named init to avoid confusion with the
              actual IPC Router open call) which initializes a process's
              connection to the QDI driver.

ARGUMENTS     client_handle - Handle of the caller
              ptr - unused
              method - this is the method ID.
              a1:a9 - The init arguments.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_qdi_init_invocation
(
 int client_handle, 
 qurt_qdi_obj_t *ptr,
 int method, 
 qurt_qdi_arg_t a1, /* Dev name */
 qurt_qdi_arg_t a2, /* Out: Group Sig */
 qurt_qdi_arg_t a3, /* Out: Event Sig */
 qurt_qdi_arg_t a4, /* Out: Data Sig */
 qurt_qdi_arg_t a5,
 qurt_qdi_arg_t a6,
 qurt_qdi_arg_t a7,
 qurt_qdi_arg_t a8,
 qurt_qdi_arg_t a9
)
{
  ipc_router_qdi_handle_t *handle;
  int rc;
  int *remote_group_sig = NULL;
  int *remote_event_sig = NULL;
  int *remote_data_sig = NULL;

  if(method != IPC_ROUTER_QDI_INIT)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  handle = ipc_router_os_malloc(sizeof(*handle));
  if(!handle)
  {
    return IPC_ROUTER_QDI_NO_MEM_ERR;
  }

  LIST_INIT(&handle->rx_q);
  LIST_INIT(&handle->event_q);

  handle->clients = NULL;
  handle->clients_count = 0;
  ipc_router_os_sem_init(&handle->clients_lock);
  handle->next_fd = 1;

  qurt_anysignal_init(&handle->cleanup_sig);

  handle->obj.invoke = ipc_router_qdi_main_invocation;
  handle->obj.release = ipc_router_qdi_deinit_invocation;
  handle->obj.refcnt = QDI_REFCNT_INIT;

  rc = qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t *)handle);
  if(rc < 0)
  {
    rc = IPC_ROUTER_QDI_INTERNAL_ERR;
    goto err_bail;
  }
  handle->qdi_handle = rc;

  /* User passes in 3 pointers to place the output signals. Get permissions to write */

  rc = qurt_qdi_lock_buffer(client_handle, a2.ptr, sizeof(int), QDI_PERM_W, (void **)&remote_group_sig);
  if(rc || !remote_group_sig)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto release_handle_bail;
  }

  rc = qurt_qdi_lock_buffer(client_handle, a3.ptr, sizeof(int), QDI_PERM_W, (void **)&remote_event_sig);
  if(rc || !remote_event_sig)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto release_handle_bail;
  }

  rc = qurt_qdi_lock_buffer(client_handle, a4.ptr, sizeof(int), QDI_PERM_W, (void **)&remote_data_sig);
  if(rc || !remote_data_sig)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto release_handle_bail;
  }

  rc = qurt_qdi_signal_group_create(client_handle, 
             &handle->signal.group.local, &handle->signal.group.remote);
  if(rc)
  {
    rc = IPC_ROUTER_QDI_INTERNAL_ERR;
    goto release_handle_bail;
  }
  *remote_group_sig = handle->signal.group.remote;

  rc = qurt_qdi_signal_create(handle->signal.group.local, &handle->signal.event.local,
                              &handle->signal.event.remote);
  if(rc)
  {
    rc = IPC_ROUTER_QDI_INTERNAL_ERR;
    goto release_handle_bail;
  }
  *remote_event_sig = handle->signal.event.remote;

  rc = qurt_qdi_signal_create(handle->signal.group.local, &handle->signal.data.local,
                              &handle->signal.data.remote);
  if(rc)
  {
    rc = IPC_ROUTER_QDI_INTERNAL_ERR;
    goto release_handle_bail;
  }
  *remote_data_sig = handle->signal.data.remote;

  /* For debug */
  ipc_router_qdi_latest_handle = handle;

  return handle->qdi_handle;

release_handle_bail:
  qurt_qdi_handle_release(client_handle, handle->qdi_handle);
err_bail:
  ipc_router_os_sem_deinit(&handle->clients_lock);
  ipc_router_os_sem_deinit(&handle->event_q.lock);
  ipc_router_os_sem_deinit(&handle->rx_q.lock);
  qurt_anysignal_destroy(&handle->cleanup_sig);
  ipc_router_os_free(handle);
  return rc;
}

/*===========================================================================
                        PUBLIC FUNCTIONS
===========================================================================*/


/*===========================================================================
FUNCTION      ipc_router_qdi_init

DESCRIPTION   Initializes the IPC Router QDI driver

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_qdi_init(void)
{
  static qurt_qdi_obj_t opener = {
    ipc_router_qdi_init_invocation,
    QDI_REFCNT_PERM,
    NULL
  };

  qurt_qdi_register_devname(IPC_ROUTER_QDI_DEV_NAME, &opener);
}
