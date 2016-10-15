#ifndef DIAG_V_H
#define DIAG_V_H
/*==========================================================================

                     Diagnostic Task Header File

Description
  Global Data declarations of the diag_task.

Copyright (c) 1991-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.adsp/2.2/services/diag/LSM/common/src/diag_v.h#4 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/09/13   sr      Added support for diag mask change notification for 
                   registered users  
04/22/13   ph       Moved listeners allocation to system heap.
02/11/13   sr      Mainling Diag central routing  
09/26/12   is      Support for preset masks
07/05/12   sg      Changes to bring up diag Multi PD
03/10/12   sg      Fixed Klock work warning 
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
04/18/11   is      Update feature flag for SMD-Lite
03/04/11   is      Support for Diag over SMD-Lite
02/04/11   vs      Removed check for MASTER to not have RSP_SEND
11/26/10   vs      if a9 on 9k - have proc ID be modem_proc
11/22/10   hm      diag_v.h clean up. Moved the defines to respective custdiag.h files  
10/25/10   is      Diag feature flag cleanup
07/23/10   mad     Added prototype for diagpkt_check_validity_request_ondevice() 
07/12/10   mad     Changes for apps-only diag bring-up 
06/30/10   sg      diag_client_maskchange() is made static 
04/20/10   sg      Moved diag_client_maskchange() here form diag_shared_i.h
06/07/10   is      Replace external IMAGE_* feature flags with internal Diag 
                   versions
04/21/10   JV      Added prototypes for functions that manage the new heap for
                   events.
12/14/09   sg      Added diag_rx_rsp_notify()
07/14/09   vs      Added diag_set_drain_timer
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Added new internal feature to single out the Q6 on 8x50
                   DIAG_QDSP6_APPS_PROC
07/31/09   JV      Merged Q6 diag code back to mainline
07/28/09   as      Removed public API's from internal file.
07/17/09   mad     Featurized includion of customer.h for WM.
07/10/09   as      Decoupled feature FEATURE_DIAG_NON_STREAMING
05/07/09   vk      moved declerations into diag.h that were being referred from there
01/15/09   as      Fixed corner case in proc ID
12/12/08   vg      split public and private information diag.h and diag_v.h
10/03/08   vg      Update configuration to enable msm 8200, 2 modem processors
12/22/06   as      Moved proc ID macros to diag.h
12/05/06   as      Added signal for Diag drain timer
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command.
09/23/03    gr     Function prototypes related to changes that enable more
                   efficient detection of the condition where diag is out of
                   heap space.
08/20/01   jal     Changes to support more Diag packets.  Support for powerup
                   online/offline state, service programming lock state.
04/06/01   lad     Cosmetic changes.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  This file now
                   contains an API.  No other information or coupling remains
                   except for featurized legacy code that impacts existing 
                   targets.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "comdef.h"
#ifndef FEATURE_WINCE
#include "customer.h"    
#endif

#include "qw.h"
#include "diag.h"


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
/* Diagnostics version (protocol revision).
*/
/* DIAG_DIAGVER                                                            
   Diagnostics version, used to ensure compatibility of the DM and the DMSS.
   1   Original
   2   Changes to status packet and verno packet. Removed verstr packet
   3   for release to factory. Sends RF mode in status packet
   4   Adds release directory to verno packet and dipswitch packets
   5   Many changes in DM.
   6   Added Vocoder PCM and PKT loopback, and Analog IS-55 tests
   7   New FER data in tagraph packet
   8   Streaming protocol enhancements and event reporting.
*/

#define DIAG_DIAGVER 8

/* -------------------------------------------------------------------------
** Diag Internal features
** ------------------------------------------------------------------------- */

/* Proc ID definitions */
#if defined(DIAG_IMAGE_APPS_PROC)
#if defined (DIAG_IMAGE_QDSP6_PROC)
#define DIAG_MY_PROC_ID DIAG_QDSP6_PROC
#else
#define DIAG_MY_PROC_ID DIAG_APP_PROC
#endif
#elif defined(DIAG_IMAGE_MODEM_PROC)
 #define DIAG_MY_PROC_ID DIAG_MODEM_PROC
#elif defined(DIAG_IMAGE_RIVA_PROC)
  #define DIAG_MY_PROC_ID DIAG_RIVA_PROC
#else
// Not defined 
#endif

/* Runtime Device Map port selection for diag */
#if defined (FEATURE_RUNTIME_DEVMAP) && !defined(DIAG_APPS_ONLY_BRINGUP)
#if defined (DIAG_MP)
  #if defined (DIAG_MP_MASTER)|| defined(FEATURE_DIAG_MODEM_ONLY)
#ifndef FEATURE_WINCE
    #define DIAG_RUNTIME_DEVMAP
#endif
  #endif
#else
   #define DIAG_RUNTIME_DEVMAP
#endif
#endif

#if defined(FEATURE_DATA_SERIALIZER) && defined(DIAG_RUNTIME_DEVMAP)
  #define FEATURE_DIAG_ON_DATA_PORT
#endif /* FEATURE_DATA_SERIALIZER */

/* Error cases */
#ifndef FEATURE_WINCE
#if defined (DIAG_MP) && !defined(FEATURE_SMD) && !defined(FEATURE_SMD_LITE)
  #error "Error: FEATURE_SMD or FEATURE_SMD_LITE is required for DIAG_MP" 
#endif 
#endif
#if defined (FEATURE_DIAG_MODEM_ONLY) &&  \
    !(defined (DIAG_MP) || defined (DIAG_IMAGE_MODEM_PROC))
  #error "Error: Invalid configuration for DIAG MP"
#endif 


#if !defined (DIAG_MP_MASTER)
 #if defined (DIAG_REQ_FWD) 
   #error "Error: DIAG_REQ_FWD defined on Slave"
 #endif
 #if defined(DIAG_MP) && defined (DIAG_FRAMING)
   #error "Error: DIAG_FRAMING defined on Slave"
 #endif
#endif


#define DIAG_FTM_SWITCH_VAL 2

/* -------------------------------------------------------------------------
** Function Definitions
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef FEATURE_WINCE
/* Needs to be moved to Windiag driver file */
void startdiag();
#endif

/*These define the bit postion in diag_mask_notify 
  for mask change notification*/
#define DIAG_EVENT_MASK_NOTIFY_BIT 1
#define DIAG_LOG_MASK_NOTIFY_BIT   2
#define DIAG_MSG_MASK_NOTIFY_BIT   4

/* ==================================================================
FUNCTION DIAG_MALLOC

DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
  void *diag_malloc (unsigned long num_bytes);


/* ==================================================================
FUNCTION DIAG_EVENT_MALLOC

DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
  void *diag_event_malloc (unsigned long num_bytes);

  int diag_event_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes);

/* ==================================================================
FUNCTION DIAG_EVENT_FREE

DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc.
===================================================================== */
  void diag_event_free (void *mem_ptr);
/* ==================================================================
FUNCTION DIAG_EVENT_MALLOC_SEC

DESCRIPTION
   Dynamically allocates a specified number of bytes from diag secondary
   event heap and returns a   pointer to this chunk of memory.
===================================================================== */
  void *diag_event_malloc_sec (unsigned long num_bytes);

  int diag_event_alloc_check_sec (unsigned int num_allocs, uint32 *alloc_sizes);

/* ==================================================================
FUNCTION DIAG_EVENT_FREE

DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc from diag_event_sec_heap.
===================================================================== */
  void diag_event_free_sec (void *mem_ptr);

  
#ifdef FEATURE_WINCE
/* Needs to be moved to Windiag driver file */
void startdiag();
#endif

/*===========================================================================
FUNCTION DIAG_SUSPEND_TASK

DESCRIPTION
  This procedure suspends the DIAG task for 'duration' milliseconds.  This
  routine has no impact on the flow of diagnostic data other than 
  suspending the DIAG task.
   
===========================================================================*/
  void diag_suspend (unsigned int duration);

/* ==================================================================
FUNCTION DIAG_BLOCK

DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG 
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_block (void);

/* ==================================================================
FUNCTION DIAG_LISTENERS_FREE

DESCRIPTION
   This function frees the listener 
===================================================================== */
  void diag_listeners_free(void *mem_ptr);

/* ==================================================================
FUNCTION DIAG_LISTENERS_MALLOC

DESCRIPTION
   This function allocates a listener
===================================================================== */
  void * diag_listeners_malloc (unsigned long num_bytes);


/* ==================================================================
FUNCTION DIAG_RELEASE

DESCRIPTION
   Blocks the DIAG task until diag_release() is called.  The DIAG 
   task will be suspended completely until diag_release() is called.
===================================================================== */
  void diag_release (void);

/*===========================================================================
FUNCTION DIAG_TX_NOTIFY

DESCRIPTION
  This routine notifies DIAG that data is ready to be sent without causing
  a context switch immediately.
  
  This eliminates concern that INTLOCK's may be violated from a context 
  switch.
  
RETURN VALUE
  None.

===========================================================================*/
  void diag_tx_notify (void);


/*===========================================================================
FUNCTION DIAG_RX_RSP_NOTIFY

DESCRIPTION
  This function notifies DIAG that RX response data is ready to be sent .

===========================================================================*/
  void diag_rx_rsp_notify(void);


/*===========================================================================
FUNCTION DIAG_SET_DRAIN_TIMER

DESCRIPTION
  This function sets the drain timer.

===========================================================================*/
  void diag_set_drain_timer(void);

/* Data associated with synch. event for mask-change. */
#define DIAG_EVENTSVC_MASK_CHANGE   1
#define DIAG_LOGSVC_MASK_CHANGE     2
#define DIAG_MSGSVC_MASK_CHANGE     4
#define DIAG_PKTSVC_READ_COMPLETE   16
/*===========================================================================
FUNCTION DIAG_CLIENT_MASKCHANGE

DESCRIPTION
  Notifies all diag clients (Diag_LSMs) of mask change.
============================================================================*/
boolean diag_client_maskchange(int Svc);

//ToDo: Move to diagpkti_v.h
boolean diagpkt_check_validity_request_ondevice (void * buf);

#if !defined(DIAG_MP_MASTER) && defined (DIAG_MP)
void diagpkt_fwd_cmd_reg( void );
#endif

#ifdef __cplusplus
}
#endif
#endif /*DIAG_V_H*/
