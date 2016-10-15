#ifndef _IPC_ROUTER_XAL_SMD_COMMON_H_
#define _IPC_ROUTER_XAL_SMD_COMMON_H_
/*===========================================================================

                      I P C    R O U T E R    X A L
                           S M D L  C O M M O N


 Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_xal_smdl_common.h#2 $ 
$DateTime: 2013/07/11 18:00:48 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/11   aep     First revision
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_packet.h"
#include "smd_lite.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/** SMDL Specific port structur definitions */
typedef struct {
  void                         *task_handle;    /** OS Private task member */
  void                         *cb_handle;      /** Handle to call up the stack */
  boolean                      paused;          /** Flag indicating that a resume is required */
  boolean                      rx_start;        /** Flag indicating that rx open */
  boolean                      tx_start;        /** Flag indicating that tx open */
  uint32                       tx_ref;          /** Number of references to the TX logic */
  uint32                       rx_ref;          /** Number of references to the RX logic */
  smdl_handle_type             handle;          /** SMDL Handle returned by open */
  ipc_router_os_sem            lock;            /** Lock of this structure */
  ipc_router_packet_queue_type tx_queue;        /** TX Queue */
  ipc_router_packet_type       *rx_leftover;    /** Pending RX Packet */
  ipc_router_packet_type       *tx_leftover;    /** Pending TX Packet */
  smdl_iovec_type              *rx_leftover_iov;/** Pending RX IO Vector */
  smdl_iovec_type              *tx_leftover_iov;/** Pending TX IO Vector */
} ipc_router_xal_smdl_port_type;

/** Event types being signalled to the worker thread */
typedef enum
{
  IPC_ROUTER_XAL_SMDL_READ_WRITE,
  IPC_ROUTER_XAL_SMDL_REMOTE_CLOSED,
  IPC_ROUTER_XAL_SMDL_REMOTE_RESUME,
} ipc_router_xal_smdl_event_type;

/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_init

DESCRIPTION   Initializes the OS Private structure and spawns a worker 
              thread to handle this port.

              port - The port structure for this SMDL interface.

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success,
              error code on failure.
===========================================================================*/
int ipc_router_xal_smdl_os_init(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_deinit

DESCRIPTION   De-initialize the OS specific elements associated with this port.

              port - The port structure for this SMDL interface.

RETURN VALUE  None

SIDE EFFECTS  The worker thread associated with this port might be killed.
===========================================================================*/
void ipc_router_xal_smdl_os_deinit(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_os_signal_worker

DESCRIPTION   Signal the worker to handle an event

              port - The port structure for this SMDL interface.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_xal_smdl_os_signal_worker(ipc_router_xal_smdl_port_type *port, ipc_router_xal_smdl_event_type event);

/*===========================================================================
FUNCTION      ipc_router_xal_smdl_convert_packet

DESCRIPTION   Converts an IPC Router packet type into an smd_lite IO Vector.
              the IO Vector is allocated from the heap.

              pkt - pointer to packet

RETURN VALUE  Pointer to the IO Vector's head, NULL on failure.

SIDE EFFECTS  None
===========================================================================*/
smdl_iovec_type *ipc_router_xal_smdl_convert_packet(ipc_router_packet_type *pkt);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_rx

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle RX

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_rx(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_tx

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle TX

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_tx(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_remote_close

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle remote
  close

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_remote_close(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_handle_resume

DESCRIPTION
  Function called by the IPC Router's SMDL worker thread to handle resume

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_handle_remote_resume(ipc_router_xal_smdl_port_type *port);

/*===========================================================================
FUNCTION ipc_router_xal_smdl_free_iov

DESCRIPTION
  Frees the passed IO Vector

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ipc_router_xal_smdl_free_iov(smdl_iovec_type *iov);

#endif
