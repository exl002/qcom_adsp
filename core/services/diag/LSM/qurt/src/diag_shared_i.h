#ifndef DIAG_SHARED_I_H
#define DIAG_SHARED_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                GENERAL DESCRIPTION

  Diag-internal declarations  and definitions common to API layer
  (Diag_LSM) and diag driver (windiag).

Copyright (c) 2008-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE
$Header: //components/rel/core.adsp/2.2/services/diag/LSM/qurt/src/diag_shared_i.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/10/12   sg      Added client_id arg to mask chnage req functions to
                   use qurt_qdi_copy_to_user() function
07/05/12   sg      Changes to bring up Diag MultiPD
10/12/10   mad     Modified diag_modality_ctrl_params structure
09/15/10   mad     New mode of on-device logging: log to named MsgQ
07/23/10   mad     Added DIAG_IOCTL_ONDEV_QUERY and DIAG_CFG_UPDATE_SYNC_EVT_NAME
06/20/10   sg      Moved event_q_alloc, event_q_put to eventi.h and removed 
                   event_q_pending as we donot need it 
05/27/10   JV      Added prototype for diagdiag_get_diag_task_pri()
03/17/10   mad     Included comdef.h and windows.h
02/02/10   mad     Added Registry entries for on-device logging
11/19/09   mad     Added IOCTL codes for on-device logging
04/24/09   mad     Added IOCTL code to read delayed response id from windiag.
04/23/09   mad     Added structures etc for delayed response
01/16/09   mad     Added constant values for Diag Registry entries.
01/15/09   mad     Removed declaration of g_rx_buffer. Added prefix names for
                   synch. events.
10/03/08   mad     Created file.
===========================================================================*/


#include "comdef.h"
#include "qurt_qdi.h"
#include "qurt_signal.h"
/*Diag 1.5 Stream-driver model:
Adding a uint32 to the beginning of the diag packet,
so windiag driver can identify what this is. This will be stripped out in the 
WDG_Write() function, and only the rest of the data will
be copied to diagbuf. */
typedef struct
{
  uint32 diag_data_type; /* This will be used to identify whether the data passed to DCM is an event, log, F3 or response.*/
  uint8 rest_of_data;
}
diag_data;
#define DIAG_REST_OF_DATA_POS (FPOS(diag_data, rest_of_data))

/* Reasoning for an extra structure:
diagpkt_delay_alloc allocates a certain length, to send out a delayed response.
On WM, diagpkt_delay_commit needs to know the length of the response, to call WriteFile(), 
and the header doesn't have any length field.

(On AMSS, diagpkt_delay_alloc directly allocates to diagbuf, and length is stored in the 
diagbuf header. When diagpkt_delay_commit() (that results in diagbuf_commit()) is called,
we know how much to commit.)

*/
typedef struct
{
   uint32 length; /* length of delayed response pkt */
   diag_data diagdata;
}diag_data_delayed_response;
#define DIAG_DEL_RESP_REST_OF_DATA_POS (FPOS(diag_data_delayed_response,diagdata.rest_of_data))

/* different values that go in for diag_data_type */
#define DIAG_DATA_TYPE_EVENT         0
#define DIAG_DATA_TYPE_F3            1
#define DIAG_DATA_TYPE_LOG           2
#define DIAG_DATA_TYPE_RESPONSE      3
#define DIAG_DATA_TYPE_DELAYED_RESPONSE   4


/* The various IOCTLs */
#define DIAG_IOCTL_ERROR             0 /* To have a default value for IOCTL Code DWORD */
#define DIAG_IOCTL_COMMAND_REG       1 /* IOCTL for packet registration.
                                       Clients can use this to register to 
                                       respond to packets from host tool */
#define DIAG_IOCTL_COMMAND_DEREG     2 /* IOCTL for de-registration */
                                       /* Client process uses this to 
                                       de-register itself, while 
                                       unloading gracefully. */
#define DIAG_IOCTL_MASK_REG          3 /* IOCTL for registration for mask-change */
#define DIAG_IOCTL_MASK_DEREG        4 
#define DIAG_IOCTL_GETEVENTMASK      5 /* For Client process to get event mask from DCM */
#define DIAG_IOCTL_GETLOGMASK        6
#define DIAG_IOCTL_GETMSGMASK        7
#define DIAG_IOCTL_GET_DELAYED_RSP_ID  8 /* Diag_LSM uses this IOCTL to get the next delayed response id
                                          in the system. */

#define DIAG_IOCTL_ONDEVICE_PORT_OPEN  9 
#define DIAG_IOCTL_USB_PORT_OPEN       10
#define DIAG_IOCTL_PORT_CLOSE          11
#define DIAG_IOCTL_CFG_READ            12  /* Read mask-configuration file for on-device */
#define DIAG_IOCTL_ONDEV_QUERY         13  /* Query On-device info, e.g. current log file */
#define DIAG_IOCTL_CTRL_DIAG           14  /* Control diag sleep-vote, buffering modality etc */


#define DIAG_QDI_OPEN  (0+QDI_PRIVATE)
#define DIAG_QDI_READ  (1+QDI_PRIVATE)

#define DIAG_QDI_WRITE (2+QDI_PRIVATE)

#define DIAG_QDI_CLOSE (3+QDI_PRIVATE)
#define DIAG_QDI_IOCTL (4+QDI_PRIVATE)

#define DIAG_QDI_BLOCK_FOR_NOTIFICATION (5+QDI_PRIVATE)
#define DIAG_QDI_SEND_PKT_READ_COMPLETE_NOTIFICATION (6+QDI_PRIVATE)
#define DIAG_QDI_KILL_RX_THREAD (7+QDI_PRIVATE)

#define DIAG_EVENTSVC_MASK_CHANGE 1
#define DIAG_LOGSVC_MASK_CHANGE 2
#define DIAG_MSGSVC_MASK_CHANGE 4
#define SYNC_EVENT_DIAG_LSM_PKT_IDX    8
#define DIAG_KILL_RX_THREAD 16
/* Max. size of a synch. object. 
This is defined to be MAX_PATH (260), but
we don't need that much.
*/
#define MAX_SYNC_OBJ_NAME_SIZE      32



/* 
Changing the structure, for WM6.
We cannot pass "embedded" pointers, in WM6.
count and sync_obj_name will be repeated for each entry,
but command registration is not a frequent occurrence, so we can take the hit.
*/
typedef struct
{
   uint32 count; /* Number of entries in this bind */
   uint16 cmd_code;
   uint16 subsys_id;
   uint16 cmd_code_lo;
   uint16 cmd_code_hi;
   uint16 proc_id;
   uint32 event_id;
   uint32 log_code;
   uint32 client_id;
   qurt_signal_t * signal;
}
bindpkt_params_per_process;



#define MAX_CONCURRENT_REQS      1

/* Name of synch. event,
to signal diag thread that master processor
has processed the packet currently in global Rx buffer */
#define DIAG_PKT_MSTR_READ_EVT_NAME    TEXT("DIAG_PKT_MASTER_READ")

/* Prefixes for Synch. event names, to notify diag clients.
ClientID (Process ID) is appended later, to
 make these names unique in the system. */
#define DIAG_LSM_PKT_EVENT_PREFIX _T("DIAG_SYNC_EVENT_PKT")
#define DIAG_LSM_MASK_EVENT_PREFIX _T("DIAG_SYNC_EVENT_MSK")

/* functions used to communicate between 
API layer and diag driver. */
boolean diagpkt_tbl_reg_dcm (const byte * tbl_ptr, unsigned int count);
boolean diagpkt_tbl_dereg_dcm(uint32 client_id);
boolean event_process_LSM_mask_req (int client_handle,unsigned char* mask, int maskLen, int * maskLenReq);
boolean log_process_LSM_mask_req (int client_handle,unsigned char* mask, int maskLen, int * maskLenReq);
boolean msg_process_LSM_mask_req (int client_handle,unsigned char* mask, int maskLen, int * maskLenReq);
boolean diagpkt_mask_tbl_reg (uint32 client_id, qurt_signal_t * mask);
boolean diagpkt_mask_tbl_dereg (uint32 client_id);

/* prototype for function used to obtain the priority of
diag task */
uint16 diagdiag_get_diag_task_pri( void );

#endif /* DIAG_SHARED_I_H */
