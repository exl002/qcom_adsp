/*===========================================================================

                I P C    R O U T E R   P R O T O C O L

   This file describes the OS-independent interface to to write and
   extract the header in the respective protocol wire format.
   This file must be compatible with all operating systems on which the 
   router will be used.

 Copyright (c) 2013 QUALCOMM Technology Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_protocol.c#1 $
$DateTime: 2013/07/11 18:00:48 $
$Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_protocol.h"
#include "ipc_router_types.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/* Flags in the V3 header control flags field */
#define IPC_ROUTER_V3_CTRL_FLAG_CONF_RX       0x1
#define IPC_ROUTER_V3_CTRL_FLAG_OPT_HDR       0x2

/* Flags in the V3 optional header optional flags field */
#define IPC_ROUTER_V3_OPT_CTRL_FLAG_OPT_HDR   0x1

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                               VERSION 1

  HEADER:
      0         1         2       3
  |--------|--------|--------|--------|
  |VERSION |     RESERVED             | Word 1
  |--------|--------|--------|--------|
  |MSG_TYPE|     RESERVED             | Word 2
  |--------|--------|--------|--------|
  |       SOURCE PROCESSOR ID         | Word 3
  |--------|--------|--------|--------|
  |         SOURCE PORT ID            | Word 4
  |--------|--------|--------|--------|
  |CONF RX |     RESERVED             | Word 5
  |--------|--------|--------|--------|
  |           MESSAGE SIZE            | Word 6
  |--------|--------|--------|--------|
  |     DESTINATION PROCESSOR ID      | Word 7
  |--------|--------|--------|--------|
  |       DESTINATION PORT ID         | Word 8
  |--------|--------|--------|--------|
===========================================================================*/
typedef ipc_router_header_type ipc_router_v1_header_type;


/*===========================================================================
                               VERSION 3
  HEADER: 
      0         1         2       3
  |--------|--------|--------|--------|
  |VERSION |MSG_TYPE|  CONTROL FLAGS  | Word 1 
  |--------|--------|--------|--------|
  |              MSG_SIZE             | Word 2
  |--------|--------|--------|--------|
  |   SRC PROC ID   |   SRC PORT ID   | Word 3
  |--------|--------|--------|--------|
  |   DST PROC ID   |   DST PORT ID   | Word 3
  |--------|--------|--------|--------|

  CONTROL FLAGS:
         Bit 0    : Confirm RX
         Bit 1    : Optional Header
         Bits 2-15: Reserved
 
  OPTIONAL HEADER
  (Present if Optional Header bit is set in the control flags)
 
      0         1         2       3
  |--------|--------|--------|--------|
  |OPT_LEN |OPT_TYPE|    OPT FLAGS    | Word 1 
  |--------|--------|--------|--------|
  .........OPT_LEN - 1 Words...........
 
  OPT_LEN:
  Length of the optional header in words
  (Including self, so it is 1 at the minimum)
 
  OPT_TYPE:
  Type of the optional header. 
 
  OPT FLAGS:
  Optional flags. 
       Bit 0    : Optional Header. If set, there is another 
                  optional header after this.
       Bit 1-15 : Reserved
===========================================================================*/
typedef struct
{
  uint16 processor_id;
  uint16 port_id;
} ipc_router_v3_address_type;

typedef struct
{
  uint8                      version;
  uint8                      type;
  uint16                     flags;
  uint32                     msg_size;
  ipc_router_v3_address_type src_addr;
  ipc_router_v3_address_type dest_addr;
} ipc_router_v3_header_type;

typedef struct
{
  uint8  len;
  uint8  type;
  uint16 flags;
} ipc_router_v3_optional_header_type;

/*===========================================================================
                        FORWARD DECLARATIONS
===========================================================================*/
static int ipc_router_inject_header_v1
(
  ipc_router_header_type *header, 
  ipc_router_packet_type **msg
);
static int ipc_router_extract_header_v1
(
  ipc_router_packet_type **msg, 
  ipc_router_header_type *header
);
static int ipc_router_inject_header_v3
(
  ipc_router_header_type *header, 
  ipc_router_packet_type **msg
);
static int ipc_router_extract_header_v3
(
  ipc_router_packet_type **msg, 
  ipc_router_header_type *header
);

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
ipc_router_protocol_ops_type ipc_router_protocol_ops[IPC_ROUTER_VERSION_TBL_SIZE] =
{
  /* Version 0 - Invalid */
  {NULL, NULL}, 

  /* Version 1 - Legacy IPC Router (BW Compatible with RPC Router) */
  {ipc_router_inject_header_v1, ipc_router_extract_header_v1}, 

  /* Version 2 - Skipped (IPC Router V2 Dev version) */
  {NULL, NULL},

  /* Version 3 - IPC Router V3 */
  {ipc_router_inject_header_v3, ipc_router_extract_header_v3}
};

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_peek_version

DESCRIPTION   Peeks into the message to get to the version

RETURN VALUE  Version number
===========================================================================*/
static uint32 ipc_router_peek_version(ipc_router_packet_type *msg)
{
  uint8 *version_peek;

  version_peek = (uint8 *)ipc_router_packet_peek_header(msg);
  if(!version_peek)
  {
    return 0;
  }
  return (uint32)(*version_peek);
}

/*===========================================================================
                               VERSION 1
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_inject_header_v1

DESCRIPTION   Injects a V1 header to the start of a packet with the contents
              from the generic ipc_router_header_type header

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_inject_header_v1
(
  ipc_router_header_type *header,
  ipc_router_packet_type **msg
)
{
  if(!header || !msg || !*msg)
    return IPC_ROUTER_STATUS_FAILURE;

  header->version = IPC_ROUTER_V1_VERSION;

  if(ipc_router_packet_copy_header(msg, header, sizeof(*header)) != sizeof(*header))
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_extract_header_v1

DESCRIPTION   Extracts a V1 header (If present at the start of the packet) 
              and puts the contents in the generic ipc_router_header_type
              if successful.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_extract_header_v1
(
  ipc_router_packet_type **msg, 
  ipc_router_header_type *header
)
{
  if(!header || !msg || !*msg)
    return IPC_ROUTER_STATUS_FAILURE;

  if(IPC_ROUTER_V1_VERSION != ipc_router_peek_version(*msg))
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }

  if(ipc_router_packet_read_header(msg, header, sizeof(*header)) != sizeof(*header))
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  header->version = IPC_ROUTER_V1_VERSION;

  return IPC_ROUTER_STATUS_SUCCESS;
}


/*===========================================================================
                               VERSION 3
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_inject_header_v3

DESCRIPTION   Injects a V3 header to the start of a packet with the contents
              from the generic ipc_router_header_type header

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_inject_header_v3
(
  ipc_router_header_type *header, 
  ipc_router_packet_type **msg
)
{
  ipc_router_v3_header_type w_header;

  if(!header || !msg || !*msg)
    return IPC_ROUTER_STATUS_FAILURE;

  w_header.version                = (uint8)IPC_ROUTER_V3_VERSION;
  w_header.type                   = (uint8)header->msg_type;
  w_header.flags                  = header->confirm_rx ? IPC_ROUTER_V3_CTRL_FLAG_CONF_RX : 0x0;
  w_header.msg_size               = header->msg_size;
  w_header.src_addr.processor_id  = (uint16)header->src_addr.processor_id;
  w_header.src_addr.port_id       = (uint16)header->src_addr.port_id;
  w_header.dest_addr.processor_id = (uint16)header->dest_addr.processor_id;
  w_header.dest_addr.port_id      = (uint16)header->dest_addr.port_id;

  if(ipc_router_packet_copy_header(msg, &w_header, sizeof(w_header)) != sizeof(w_header))
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }
  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_extract_header_v3

DESCRIPTION   Extracts a V3 header (If present at the start of the packet) 
              and puts the contents in the generic ipc_router_header_type
              if successful.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCES on success
              IPC_ROUTER_STATUS_*      on failure
===========================================================================*/
static int ipc_router_extract_header_v3
(
  ipc_router_packet_type **msg, 
  ipc_router_header_type *header
)
{
  ipc_router_v3_header_type w_header;
  ipc_router_v3_optional_header_type opt_header;
  boolean optional_present = FALSE;
  uint32 junk;

  if(!header || !msg || !*msg)
    return IPC_ROUTER_STATUS_FAILURE;

  if(IPC_ROUTER_V3_VERSION != ipc_router_peek_version(*msg))
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }

  if(ipc_router_packet_read_header(msg, &w_header, sizeof(w_header)) != sizeof(w_header))
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  header->version                = IPC_ROUTER_V3_VERSION;
  header->confirm_rx             = w_header.flags & IPC_ROUTER_V3_CTRL_FLAG_CONF_RX ? 1 : 0;
  header->msg_type               = (uint32)w_header.type;
  header->msg_size               = w_header.msg_size;
  header->src_addr.processor_id  = (uint32)w_header.src_addr.processor_id;
  header->src_addr.port_id       = (uint32)w_header.src_addr.port_id;
  header->dest_addr.processor_id = (uint32)w_header.dest_addr.processor_id;
  header->dest_addr.port_id      = (uint32)w_header.dest_addr.port_id;
  optional_present               = w_header.flags & IPC_ROUTER_V3_CTRL_FLAG_OPT_HDR ? TRUE : FALSE;

  /* Discard optional header */
  while(optional_present)
  {
    if(ipc_router_packet_read_payload(msg, &opt_header, sizeof(opt_header)) 
        != sizeof(opt_header))
    {
      return IPC_ROUTER_STATUS_NO_MEM;
    }
    if(opt_header.len < 1)
    {
      return IPC_ROUTER_STATUS_INVALID_MEM;
    }

    optional_present = opt_header.flags & IPC_ROUTER_V3_OPT_CTRL_FLAG_OPT_HDR ? TRUE : FALSE;
    opt_header.len--; /* We have already extracted 1 word */

    /* Discard the whole optional header */
    while(opt_header.len > 0)
    {
      if(ipc_router_packet_read_payload(msg, &junk, sizeof(junk)) != sizeof(junk))
      {
        return IPC_ROUTER_STATUS_NO_MEM;
      }
      opt_header.len--;
    }
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

