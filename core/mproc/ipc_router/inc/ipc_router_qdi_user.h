#ifndef IPC_ROUTER_QDI_USER_H_
#define IPC_ROUTER_QDI_USER_H_

#include "ipc_router_types.h"

typedef struct ipc_router_qdi_struct ipc_router_qdi_handle;

/* Buf has to be freed by the user 
 * addr has to be copied if it is needed after returning from this callback */
typedef void (*ipc_router_qdi_rx_cb_type)
(
  ipc_router_qdi_handle *handle,
  void *cb_data,
  void *buf,
  ipc_router_address_type *src_addr,
  uint32_t len,
  uint32_t confirm_rx
);

typedef void (*ipc_router_qdi_event_cb_type)
(
 ipc_router_qdi_handle      *handle,
 void                       *cb_data,
 ipc_router_event_type      event,
 ipc_router_event_info_type *info
);


/* Init and de-init functions, once per process only */
void ipc_router_qdi_init(void);
void ipc_router_qdi_deinit(void);

int ipc_router_qdi_open
(
  ipc_router_qdi_handle **out_handle,
  uint32_t port_id,
  ipc_router_qdi_rx_cb_type rx_cb,
  void *rx_cb_data,
  ipc_router_qdi_event_cb_type event_cb,
  void *event_cb_data
);

void ipc_router_qdi_close
(
  ipc_router_qdi_handle *handle
);

int ipc_router_qdi_send
(
  ipc_router_qdi_handle *handle,
  ipc_router_address_type *dest_addr,
  void *buf,
  uint32_t len
);

int ipc_router_qdi_send_conf
(
  ipc_router_qdi_handle *handle,
  ipc_router_address_type *dest_addr
);


int ipc_router_qdi_reg_server
(
 ipc_router_qdi_handle *handle,
 ipc_router_service_type *service
);

int ipc_router_qdi_dereg_server
(
 ipc_router_qdi_handle *handle,
 ipc_router_service_type *service
);

int ipc_router_qdi_find_all_servers
(
  ipc_router_qdi_handle *handle,
  ipc_router_service_type *service,
  ipc_router_server_type *servers,
  uint32_t *num_entries,
  uint32_t *num_servers,
  uint32_t mask
);

#endif
