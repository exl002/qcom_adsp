/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Legacy Service Mapping layer implementation for Events

GENERAL DESCRIPTION
  Contains main implementation of Legacy Service Mapping layer for Diagnostic 
  Event Services.

EXTERNALIZED FUNCTIONS
  event_report
  event_report_payload
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2007-2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.adsp/2.2/services/diag/LSM/qurt/src/Diag_LSM_Event.c#1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
07/05/12   sg     Changes to bringup Diag MultiPD
08/25/10   sg      Fixed compiler warnings
04/12/10   sg     Merging both DCM rtos and WM files 
06/22/09   mad    Removed inclusion of diagdiag.h and diagpkti.h
01/19/09   mad    Removed inclusion of diagtune.h, updated copyright
09/30/08   mad    Changes to take out Component services, and use 
                  filesystem calls to access windiag driver.
05/01/08   JV     Added support to update the copy of event_mask in this process
                  during initialization and also on mask change
11/12/07   mad    Created      

===========================================================================*/
                     
                      
/* ==========================================================================
   Include Files
========================================================================== */

#include "comdef.h"
#include "event.h" 
#include "Diag_LSMi.h" /* for declaration of windiag Handle */
#include "DiagSvc_Malloc.h"
#include "event_defs.h"
#include "queue.h"
#include "eventi.h"
#include "Diag_LSM_Event_i.h"
#include "diag_shared_i.h" /* for definition of diag_data struct. */
#include "diagstub.h"
#include "Diag_LSM_Qdi_wrapper.h"
/*this keeps track of number of failures to WriteFile().
This will currently be used only internally.*/
static unsigned int gEvent_commit_fail = 0;

/*Local Function declarations*/
static byte *event_alloc (event_id_enum_type id, uint8 payload_length, int* pAlloc_Len);

/* The event mask. */
static unsigned char event_mask[EVENT_MASK_SIZE];

#define EVENT_MASK_BIT_SET(id) \
  (event_mask[(id)/8] & (1 << ((id) & 0x07)))
  

/* Externalized functions */
/*===========================================================================

FUNCTION EVENT_REPORT

DESCRIPTION
  Report an event. Published Diag API.

DEPENDENCIES
   Diag Event service must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void event_report (event_id_enum_type Event_Id)
{
   if(diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pEvent;
      int Alloc_Len = 0;
      pEvent = event_alloc (Event_Id, 0, &Alloc_Len);
      if(pEvent)
      {
         if(diag_lsm_write_data(pEvent, Alloc_Len) < Alloc_Len )
         {
            gEvent_commit_fail++;
         }
         DiagSvc_Free(pEvent, GEN_SVC_ID);
      }
   }
   return;
}/* event_report */

/*===========================================================================

FUNCTION EVENT_REPORT_PAYLOAD

DESCRIPTION
  Report an event with payload data.

DEPENDENCIES
  Diag Event service must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
event_report_payload (event_id_enum_type Event_Id, uint8 Length, void *pPayload)
{
   if( diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pEvent = NULL;
      int Alloc_Len = 0;
      if (Length > 0 && pPayload)
      {
         pEvent = event_alloc (Event_Id, Length, &Alloc_Len);
         if (pEvent)
         {
            event_store_type* temp = (event_store_type*) (pEvent + FPOS(diag_data, rest_of_data));
            memcpy (temp->payload.payload, pPayload, Length);
           if(diag_lsm_write_data(pEvent, Alloc_Len) < Alloc_Len )
            {
               gEvent_commit_fail++;
            }
            DiagSvc_Free(pEvent, GEN_SVC_ID);
         }
         
      }
      else
      {
         event_report (Event_Id);
      }
   }
   return;
}/* event_report_payload */

    
/*===========================================================================
FUNCTION   Diag_LSM_Event_Init

DESCRIPTION
  Initializes the event service. Gets a copy of the event_mask from DCM.

DEPENDENCIES
  None.

RETURN VALUE
  FALSE = failure, else TRUE

SIDE EFFECTS
  None

===========================================================================*/

boolean Diag_LSM_Event_Init(void)
{
   boolean status = TRUE;
   status = event_update_mask();
   return status;
} /* Diag_LSM_Event_Init */

/*===========================================================================

FUNCTION    Diag_LSM_Event_DeInit

DESCRIPTION
  De-Initialize the Diag Event service.

DEPENDENCIES
  None.

RETURN VALUE
  boolean: returns TRUE; currently does nothing.

SIDE EFFECTS
  None

===========================================================================*/
boolean Diag_LSM_Event_DeInit(void)
{
  return TRUE;
} /* Diag_LSM_Event_DeInit */

/*===========================================================================

FUNCTION    event_update_mask

DESCRIPTION
  Updates the event mask for this user process

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean event_update_mask(void)
{
 	boolean status = TRUE;
   if(diag_qdi_handle>=DIAG_FD_MIN_VALUE)
   {
      dword maskLen_Recvd = 0;
      if(!qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_IOCTL,DIAG_IOCTL_GETEVENTMASK, NULL,0, event_mask, sizeof(event_mask), &maskLen_Recvd) 
        || maskLen_Recvd != EVENT_MASK_SIZE)
      {
         status = FALSE;
      }
   }
   else
   {
      status = FALSE;
   }
	return status;
} /* event_update_mask */


/*==========================================================================

FUNCTION event_alloc

DESCRIPTION
  This routine allocates an event item from the pre-mallocated memory pool
   and fills in the following information:
  
  Event ID
  Time stamp
  Payload length field
  
  //TODO :This routine also detects dropped events and handles the reporting of
  //dropped events.
    
RETURN VALUE
  A pointer to the allocated  event is returned.  
  NULL if the event cannot be allocated.
  The memory should be freed by the calling function, using DiagSvc_Free().
  pAlloc_Len is an output value, indicating the number of bytes allocated.

===========================================================================*/
static byte *
event_alloc (event_id_enum_type id, uint8 payload_length, int* pAlloc_Len)
{  
   byte *pEvent = NULL;
   int alloc_len = 0;
   
   /* Verify that the event id is in the right range and that the
    corresponding bit is turned on in the event mask. */
    if ((id > EVENT_LAST_ID) || !EVENT_MASK_BIT_SET (id))
    {
       return NULL;
    }

   alloc_len =  FPOS(diag_data, rest_of_data) + FPOS (event_store_type, payload.payload) + payload_length ;
   pEvent = (byte *) DiagSvc_Malloc(alloc_len, GEN_SVC_ID);
   if (pEvent)
   {
      event_store_type* temp = NULL;
      diag_data* pdiag_data = (diag_data*) pEvent;
      /* Fill in the fact that this is an event. */
      pdiag_data->diag_data_type = DIAG_DATA_TYPE_EVENT;
      /* Advance the pointer to point to the event_store_type part */
      temp = (event_store_type*) (pEvent + FPOS(diag_data, rest_of_data)); 
      if(pAlloc_Len)
      {
         *pAlloc_Len = alloc_len;
      }
      time_get (temp->ts);
      temp->event_id.id = id;
      temp->payload.length = payload_length;
   }
   return pEvent;
} /* event_alloc */
