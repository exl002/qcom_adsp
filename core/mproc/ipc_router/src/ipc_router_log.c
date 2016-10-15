/*===========================================================================

                      I P C    R O U T E R    L O G

   This file describes the logging interface

   Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
   QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_log.c#2 $ $DateTime: 2013/07/11 18:00:48 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_types.h"
#include "ipc_router_log.h"
#include "ipc_router_packet.h"
#include "ipc_router_protocol.h"
#include "msg.h"
#include "smem_log.h"

size_t strlcpy(char *dst, const char *src, size_t siz);

/* IPC router events */
#define IPC_ROUTER_LOG_EVENT_ERROR      0x00
#define IPC_ROUTER_LOG_EVENT_TX         0x01
#define IPC_ROUTER_LOG_EVENT_RX         0x02

#define LOG_EVENT3(event, d1, d2, d3) \
  SMEM_LOG_EVENT(SMEM_LOG_IPC_ROUTER_EVENT_BASE | event, d1, d2, d3)

#define LOG_EVENT6(event, d1, d2, d3, d4, d5, d6) \
  SMEM_LOG_EVENT6(SMEM_LOG_IPC_ROUTER_EVENT_BASE | event, d1, d2, d3, \
      d4, d5, d6)

/*===========================================================================
FUNCTION      ipc_router_peek_header

DESCRIPTION   Peeks into the message to get to the header
              This function is highly dependent on the implementation
              of the ipc_router_packet interface

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS - on success
              IPC_ROUTER_STATUS_*       - on failure
===========================================================================*/
static int ipc_router_peek_header
(
  ipc_router_packet_type *msg, 
  ipc_router_header_type *header, 
  void **body_ptr
)
{
  int rc = IPC_ROUTER_STATUS_SUCCESS;
  uint8 *hdr;
  uint32 version;
  struct {
    uint8  version;
    uint8  msg_type;
    uint16 ctrl_flags;
    uint32 msg_size;
    uint16 s_proc_id;
    uint16 s_port_id;
    uint16 d_proc_id;
    uint16 d_port_id;
  } *v3_hdr;

  if(!msg || !header || !body_ptr) 
    return IPC_ROUTER_STATUS_FAILURE;

  hdr = (uint8 *)ipc_router_packet_peek_header(msg);
  if(!hdr)
    return IPC_ROUTER_STATUS_FAILURE;

  version = (uint32)(*hdr);

  switch(version)
  {
    case IPC_ROUTER_V1_VERSION:
      ipc_router_os_mem_copy(header, hdr, sizeof(*header));
      *body_ptr = (void *)&hdr[sizeof(*header)];
      break;
    case IPC_ROUTER_V3_VERSION:
      v3_hdr = (void *)hdr;
      header->version                = (uint32)v3_hdr->version;
      header->msg_type               = (uint32)v3_hdr->msg_type;
      header->confirm_rx             = (uint32)v3_hdr->ctrl_flags & 0x1 ? 1 : 0;
      header->msg_size               = (uint32)v3_hdr->msg_size;
      header->src_addr.processor_id  = (uint32)v3_hdr->s_proc_id;
      header->src_addr.port_id       = (uint32)v3_hdr->s_port_id;
      header->dest_addr.processor_id = (uint32)v3_hdr->d_proc_id;
      header->dest_addr.port_id      = (uint32)v3_hdr->d_port_id;
      *body_ptr = &hdr[16];
      break;
    default:
      rc = IPC_ROUTER_STATUS_FAILURE;
      break;
  }
  return rc;
}

static void parse_cntl_msg
(
 unsigned int type,
 ipc_router_control_msg_body *cntl_msg,
 uint32 event
 )
{
  uint32 addr;
  switch(type)
  {
    case IPC_ROUTER_CONTROL_NEW_SERVER:
    case IPC_ROUTER_CONTROL_REMOVE_SERVER:
      addr = (cntl_msg->arguments.server_arg.addr.processor_id << 24)
        | (cntl_msg->arguments.server_arg.addr.port_id & 0xffffff);
      LOG_EVENT3(event | (type << 8), 
          addr, 
          cntl_msg->arguments.server_arg.service.type,
          cntl_msg->arguments.server_arg.service.instance);
      break;
    case IPC_ROUTER_CONTROL_REMOVE_CLIENT:
      LOG_EVENT3(event | (type << 8), 
          cntl_msg->arguments.client_arg.processor_id,
          cntl_msg->arguments.client_arg.port_id, type);
      break;
    case IPC_ROUTER_CONTROL_RESUME_TX:
      LOG_EVENT3(event | (type << 8), 
          cntl_msg->arguments.client_arg.processor_id,
          cntl_msg->arguments.client_arg.port_id, type);
      break;
    default:
      break;
  }
}
static void log_txrx(char *xp, ipc_router_header_type *header, void *body, uint32 event)
{
  uint32 desc = 0, name = 0;
  char thread_name[16];

  if(!header)
    return;

  qurt_thread_get_name(thread_name, 16);
  thread_name[15] = '\0';
  memcpy(&name, thread_name, sizeof(uint32));

  memcpy(&desc, (xp ? xp : "LOCL"), sizeof(uint32));
  
LOG_EVENT6(event,
      (header->src_addr.processor_id << 24)  
      | (header->src_addr.port_id & 0xffffff),  /* 1 */
      (header->dest_addr.processor_id << 24)
      | (header->dest_addr.port_id & 0xffffff), /* 2 */
      (header->msg_type << 24) | (header->confirm_rx << 16) 
      | (header->msg_size & 0xffff),            /* 3 */
      desc,                                     /* 4 */
      (uint32)qurt_thread_get_id(),             /* 5 */
      name);
  parse_cntl_msg(header->msg_type, (ipc_router_control_msg_body *)body,
      event);

}

/*===========================================================================
FUNCTION      ipc_router_core_log_tx

DESCRIPTION   Log the outgoing packet

ARGUMENTS     xport - pointer to xport
              msg - pointer to packet

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_tx
(
 ipc_router_xport_type *xport,
 ipc_router_packet_type *msg
 )
{
  ipc_router_header_type header;
  void *body_ptr = NULL;

  if(ipc_router_peek_header(msg, &header, &body_ptr) == IPC_ROUTER_STATUS_SUCCESS)
  {
    log_txrx(xport ? xport->desc : "LOCL", &header, body_ptr,
        IPC_ROUTER_LOG_EVENT_TX);
  }
}

/*===========================================================================
FUNCTION      ipc_router_core_log_rx

DESCRIPTION   Log the incoming packet

ARGUMENTS     xport - pointer to xport
              msg - pointer to packet

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_rx
(
 ipc_router_xport_type *xport,
 ipc_router_packet_type *msg
 )
{
  ipc_router_header_type header;
  void *body_ptr = NULL;

  if(ipc_router_peek_header(msg, &header, &body_ptr) == IPC_ROUTER_STATUS_SUCCESS)
  {
    log_txrx(xport ? xport->desc : "LOCL", &header, body_ptr,
        IPC_ROUTER_LOG_EVENT_RX);
  }
}

/*===========================================================================
FUNCTION      ipc_router_core_log_error

DESCRIPTION   Log runtime error

ARGUMENTS     filename - filename
              line - line number

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_error
(
 char *filename,
 unsigned int line
 )
{
  uint32 name[5];
  char *last;
  last = strrchr(filename, '/');
  if(!last)
    last = strrchr(filename, '\\');
  last = last ? (last+1) : filename;
  strlcpy((char *)name, last, sizeof(name));
  LOG_EVENT6(IPC_ROUTER_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
  MSG_ERROR("Runtime error. File 0x%s, Line: %d", filename, line, 0);
}
