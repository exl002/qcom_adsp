#ifndef IPC_ROUTER_LOG_H
#define IPC_ROUTER_LOG_H
/*===========================================================================

                      I P C    R O U T E R    L O G
                          H E A D E R    F I L E

   This file describes the logging interface

 Copyright (c) 2007,2009 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_log.h#1 $ $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_database.h"
#include "ipc_router_packet.h"

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
 );

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
 );

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
 );


#endif
