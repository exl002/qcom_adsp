#ifndef IPC_ROUTER_PACKET_H
#define IPC_ROUTER_PACKET_H
/*===========================================================================

                    I P C    R O U T E R   P A C K E T

        This file provides an abstraction to the packet buffer manager

 Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/inc/ipc_router_packet.h#2 $
$DateTime: 2013/07/11 18:00:48 $
$Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"

#define IPC_ROUTER_NATIVE_PACKET 1

#define ipc_router_packet_read(msg, payload, len)\
  ipc_router_packet_read_payload(msg, payload, len)


typedef struct ipc_router_packet_s ipc_router_packet_type;

typedef struct ipc_router_packet_queue_s ipc_router_packet_queue_type;

struct ipc_router_packet_queue_s {
  ipc_router_os_sem      lock;
  uint32                 cnt;
  ipc_router_packet_type *head;
  ipc_router_packet_type *tail;
};


/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/

void ipc_router_packet_init(void);

ipc_router_packet_type *ipc_router_packet_new(void);

ipc_router_packet_type *ipc_router_packet_new_headroom(uint32 headroom);

uint32 ipc_router_packet_expand(ipc_router_packet_type **pkt, uint32 bytes);

uint32 ipc_router_packet_copy_header(ipc_router_packet_type **pkt, void *buf, uint32 len);

uint32 ipc_router_packet_copy_payload(ipc_router_packet_type **pkt, void *buf, uint32 len);

uint32 ipc_router_packet_copy_tail(ipc_router_packet_type **pkt, void *buf, uint32 len);

uint32 ipc_router_packet_read_header(ipc_router_packet_type **pkt, void *buf, uint32 len);

uint32 ipc_router_packet_read_payload(ipc_router_packet_type **pkt, void *buf, uint32 len);

void *ipc_router_packet_peek_header(ipc_router_packet_type *pkt);

uint32 ipc_router_packet_length(ipc_router_packet_type *pkt);

uint32 ipc_router_payload_length(ipc_router_packet_type *pkt);

void ipc_router_packet_trim(ipc_router_packet_type **pkt, uint32 len);

void ipc_router_packet_free(ipc_router_packet_type **pkt);

void ipc_router_packet_queue_init(ipc_router_packet_queue_type *queue);
void ipc_router_packet_queue_deinit(ipc_router_packet_queue_type *queue);
void ipc_router_packet_queue_put(ipc_router_packet_queue_type *queue, ipc_router_packet_type *pkt);
ipc_router_packet_type *ipc_router_packet_queue_get(ipc_router_packet_queue_type *queue);


#endif
