/*===========================================================================

                    I P C    R O U T E R   Q D I

   This file describes the QDI Driver implementation of IPC Router.

 Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_qdi_user.c#1 $
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
#include "ipc_router_qdi_user.h"
#include "ipc_router_qdi_driver.h"
#include "qurt_qdi_driver.h"
#include "ipc_router_core.h"
#include "stdint.h"
#include "qurt_timer.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/* GLOBAL TODO:
 *
 * Add meaningful return codes. Currently it is the RC codes from IPC
 * Router Core or -1 */

#define WORKER_STACK_SIZE (2048)

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
struct ipc_router_qdi_struct {
  int                          handle;
  uint32_t                     port_id;
  ipc_router_qdi_rx_cb_type    rx_cb;
  void                         *rx_cb_data;
  ipc_router_qdi_event_cb_type event_cb;
  void                         *event_cb_data;
  /* TODO: Priority not supported at the moment */
};

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/

/* NOTE: The fact that these are globals means that we currently 
 * can support only 1 rdr thread touple per process. Thus can probably
 * be changed in the future and make the users get a handle when
 * they call init (Obviously then the APIs change to take in 1 extra
 * handle) */

static volatile int ipc_router_qdi_device = -1;
static int ipc_router_qdi_group_sig = -1;
static int ipc_router_qdi_data_sig = -1;
static int ipc_router_qdi_event_sig = -1;

static qurt_thread_t worker_tid;

static qurt_thread_attr_t worker_tattr;

static uint8_t worker_stack[WORKER_STACK_SIZE];

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      ipc_router_qdi_handle_event

DESCRIPTION   Handles a event signal

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_handle_event(void)
{
  int rc;
  ipc_router_event_info_type info;
  uint32 event;
  void *cookie;
  ipc_router_qdi_handle *handle;
  
    while(1)
    {
      rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_EVENT_READ,
                                 &event, &info, &cookie);
      if(rc <= 0)
      {
        break;
      }
      handle = (ipc_router_qdi_handle *)cookie;
      if(handle && handle->event_cb)
      {
        handle->event_cb(handle, handle->event_cb_data, event, &info);
      }

      /* Now it is safe to free up the handle */
      if(event == IPC_ROUTER_EVENT_CLOSED)
      {
        ipc_router_os_free(handle);
      }
    }
  }

/*===========================================================================
FUNCTION      ipc_router_qdi_handle_data

DESCRIPTION   Handles a data signal

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_handle_data(void)
{
  int rc;
  void *cookie;
  void *buf = NULL;
  unsigned int len = 0;
  ipc_router_qdi_handle *handle;
  ipc_router_address_type src_addr;
  unsigned int confirm_rx;

    while(1)
    {
    rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_PEEK);
      if(rc <= 0)
      {
        break;
      }
      len = (unsigned int)rc;
      buf = ipc_router_os_malloc(len);
      if(!buf)
      {
        /* TODO: Log error */
        break;
      }

    rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_READ,
                        buf, len, &src_addr, &confirm_rx, &cookie);
      if(rc <= 0)
      {
        ipc_router_os_free(buf);
      /* The head has changed, peek again */
      if(rc == IPC_ROUTER_QDI_NO_SPACE_ERR)
      {
        continue;
      }
        break;
      }

      /* Driver could have changed the head in-between peek and read */
      len = (unsigned int)rc;
      handle = (ipc_router_qdi_handle *)cookie;

      if(handle && handle->rx_cb)
      {
        handle->rx_cb(handle, handle->rx_cb_data, buf, &src_addr, len, confirm_rx);
        /* buf is freed by the rx to mimic exactly what ipc_router does */
      }
      else
      {
        /* TODO: Log error */
        ipc_router_os_free(buf);
      }
    }
  }
/*===========================================================================
FUNCTION      ipc_router_qdi_worker

DESCRIPTION   IPC Router worker thread

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_worker(void *unused)
{
  while(ipc_router_qdi_device >= 0)
  {
    qurt_qdi_signal_group_wait(ipc_router_qdi_group_sig);

    if(ipc_router_qdi_device >= 0 && qurt_qdi_signal_poll(ipc_router_qdi_event_sig))
    {
      qurt_qdi_signal_clear(ipc_router_qdi_event_sig);
      ipc_router_qdi_handle_event();
    }

    if(ipc_router_qdi_device >= 0 && qurt_qdi_signal_poll(ipc_router_qdi_data_sig))
    {
      qurt_qdi_signal_clear(ipc_router_qdi_data_sig);
      ipc_router_qdi_handle_data();
    }
  }
  /* TEMP: Currently MP QuRT cannot handle thread exits */
  while(1)
    qurt_timer_sleep(1000 * 1000);
  /* TODO: Log exiting from reader thread */
}

/*===========================================================================
                        PUBLIC FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_qdi_init

DESCRIPTION   Initialize the client framework.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  A reader thread will be started. This function may sleep
              till a successful open on the ipc_router device is performed.
===========================================================================*/
void ipc_router_qdi_init(void)
{
  int rc = ipc_router_qdi_device;

  while(rc < 0)
  {
    rc = qurt_qdi_open(IPC_ROUTER_QDI_DEV_NAME,
        &ipc_router_qdi_group_sig, &ipc_router_qdi_event_sig, &ipc_router_qdi_data_sig);
    if(rc < 0)
    {
      qurt_timer_sleep(1000 * 10); 
    }
  }
  ipc_router_qdi_device = rc;

  qurt_thread_attr_init(&worker_tattr);
  qurt_thread_attr_set_name(&worker_tattr, "IPCRTR_RDR");
  qurt_thread_attr_set_priority(&worker_tattr, 204);
  qurt_thread_attr_set_stack_size(&worker_tattr, WORKER_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&worker_tattr, (void *)worker_stack);
  qurt_thread_create(&worker_tid, &worker_tattr, ipc_router_qdi_worker, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_deinit

DESCRIPTION   De-initialize the client framework.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  This function will sleep/block till the reader thread is
              fully torn down.
===========================================================================*/
void ipc_router_qdi_deinit(void)
{
  int handle = ipc_router_qdi_device;
  int status;

  ipc_router_qdi_device = -1;

  if(handle >= 0)
  {
    qurt_qdi_close(handle);
    qurt_thread_join(worker_tid, &status);
  }
}

/*===========================================================================
FUNCTION      ipc_router_qdi_open

DESCRIPTION   Open an endpoint (ep) on the router.

ARGUMENTS     out_handle - Return handle if the open is successful.
              port_id - Optional port ID.
              rx_cb - Callback to be called on incoming packets in this ep
              rx_cb_data - Cookie to be used on the RX callback.
              event_cb - Callback to be called on incoming events on this ep
              event_cb_data - Cookie to be used on the Event callback.

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_qdi_open
(
  ipc_router_qdi_handle **out_handle,
  uint32_t port_id,
  ipc_router_qdi_rx_cb_type rx_cb,
  void *rx_cb_data,
  ipc_router_qdi_event_cb_type event_cb,
  void *event_cb_data
)
{
  ipc_router_qdi_handle *handle;
  int rc;

  if(!out_handle || !rx_cb || !event_cb)
  {
    return -1;
  }

  if(ipc_router_qdi_device < 0)
  {
    return -2;
  }

  handle = ipc_router_os_malloc(sizeof(*handle));
  if(!handle)
  {
    return -3;
  }
  handle->rx_cb         = rx_cb;
  handle->rx_cb_data    = rx_cb_data;
  handle->event_cb      = event_cb;
  handle->event_cb_data = event_cb_data;
  handle->port_id       = port_id;
  handle->handle        = -1;

  rc = qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_OPEN, 
                              &handle->handle, port_id, handle);
  if(rc)
  {
    ipc_router_os_free(handle);
    return rc;
  }

  *out_handle = handle;

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_close

DESCRIPTION   Close a previously opened endpoint.

ARGUMENTS     handle - Handle provided for this endpoint.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_qdi_close
(
  ipc_router_qdi_handle *handle
)
{
  if(ipc_router_qdi_device < 0 || !handle)
  {
    return;
  }

  (void)qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_CLOSE, handle->handle);

  /* Memory freed when we get a CLOSED event */
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send

DESCRIPTION   Send a packet to a remote endpoint.

ARGUMENTS     handle - handle for this endpoint.
              dest_addr - address for the remote endpoint.
              buf - the buffer to send.
              len - the length of the buffer.

RETURN VALUE  0 on success, error code on failure. 
              If IPC_ROUTER_STATUS_BUSY is returned, then the caller
              must wait till a resume_tx event is received for this EP.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_qdi_send
(
  ipc_router_qdi_handle *handle,
  ipc_router_address_type *dest_addr,
  void *buf,
  uint32_t len
)
{
  if(!handle || !dest_addr || !buf || !len)
  {
    return -1;
  }
  
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_SEND,
      handle->handle, dest_addr, buf, len);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send_conf

DESCRIPTION   Sends a confirm TX control packet to a destination EP.

ARGUMENTS     handle - The handle for this endpoint.
              dest_addr - The address of the destination endpoint.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_qdi_send_conf
(
  ipc_router_qdi_handle *handle,
  ipc_router_address_type *dest_addr
)
{
  if(!handle || !dest_addr)
  {
    return -1;
  }
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_SEND_CONF,
      handle->handle, dest_addr);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_reg_server

DESCRIPTION   Register a service on this endpoint.

ARGUMENTS     handle - handle for this endpoint.
              service - The service description.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_qdi_reg_server
(
 ipc_router_qdi_handle *handle,
 ipc_router_service_type *service
)
{
  if(!handle || !service)
  {
    return -1;
  }
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_REGISTER_SERVER,
      handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_dereg_server

DESCRIPTION   De-register a previously registered service on this EP.

ARGUMENTS     handle - handle for this EP.
              service - the service description

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_qdi_dereg_server
(
 ipc_router_qdi_handle *handle,
 ipc_router_service_type *service
)
{
  if(!handle || !service)
  {
    return -1;
  }
  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_UNREGISTER_SERVER,
      handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_find_all_servers

DESCRIPTION   Query the router for services matching the required description

ARGUMENTS     handle - the handle for this EP.
              service - the service description to find.
              servers - (Optional) The array to place the found services.
              num_entries - (Optional) In: The number of services 'servers' can hold
                                       Out: The number of servers populated in 'servers'
              num_servers - Total number of servers which exist.
              mask - The mask to apply on the instance ID of the service before
                     matching. A mask of 0xffffffff forces the router to find
                     services with the same instance ID.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_qdi_find_all_servers
(
  ipc_router_qdi_handle *handle,
  ipc_router_service_type *service,
  ipc_router_server_type *servers,
  uint32_t *num_entries,
  uint32_t *num_servers,
  uint32_t mask
)
{
  int req_handle = -1;

  if(!num_servers || !mask)
  {
    return -1;
  }

  /* We support calls to this API without providing a handle, so 
   * dereference only if possible. The driver can handle
   * without this (only for this API) */

  if(handle)
  {
    /* Default to invalid handle. */
    req_handle = handle->handle;
  }

  if((num_entries && !servers) || (!num_entries && servers))
  {
    return -1;
  }

  return qurt_qdi_handle_invoke(ipc_router_qdi_device, IPC_ROUTER_QDI_FIND_ALL_SERVERS,
      req_handle, service, servers, num_entries, num_servers, mask);
}

