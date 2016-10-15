
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            Event Service

General Description
  This file containsthe implementation for the event reporting service.

  An event consists of a 12 bit event ID and a timestamp.

  The requirement is that the DMSS will never drop events as long as it does
  not exceed bandwidth limitations with event reporting.  Event reporting
  has the highest priority of all unsolicited diagnostic data.

  To minimize impact of the caller's task, the event is generated and placed
  in a queue.  The DIAG task will build the final packet structure and send
  to the external device.

  Just in case an event is dropped, a "drop event" will be reported and a
  count of dropped events will be reproted as payload.

Initialization and Sequencing Requirements
  Call event_init() before reporting any events.

Copyright (c) 2011-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                              Edit History

$Header: //components/rel/core.adsp/2.2/services/diag/DCM/rtos/src/event_api.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/22/13   ph       Moved listeners allocation to system heap.
04/18/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove() 
02/01/13   sg      Fixed Klock work warnings
01/25/13   rh      Removed deprecated event trace code 
06/23/11   vs      changes to make event_listeners accessible
04/27/11   is      Resolve compiler warnings
10/29/10   vs      Changes by debugtools for lock-less implementation
09/28/10   sg     Moved diag_time_get to diag_cfg.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
06/30/10   sg      Moved the inclusion of diagi_v.h up for DIAG_QDSP6_APPS_PROC
                   to be defined earlier before using it on adsp
04/20/10   sg     Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "event.h"
#include "eventi.h"
#include "msg.h"
#include "osal.h"
#include "stringl.h"
#include "diagtarget.h"
#include "diagdiag_v.h" 
#include "diag_cfg.h"
#include "diagi_v.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#ifndef FEATURE_WINCE
#include "dog.h"
#endif 
#endif


#define EVENT_CONFIG_LISTENER_MASK 0x02

void event_service_listener (event_store_type *event_store);

#if defined (DIAG_MV_LISTENER_RPC)
extern boolean diag_add_event_listener_on_modem(
    const unsigned int event_id,  
    const diag_event_listener listener,  
    void *param);

extern boolean diag_remove_event_listener_on_modem(
    const unsigned int event_id,  
    const diag_event_listener listener,  
    void *param);
#endif
extern byte event_read_stream_id(event_id_enum_type event_id);
typedef struct
{
  diag_event_listener listener;
  void *param; /* User's parameter */
}
event_listener_entry_type;

/* This data member is used for the event listener functinality.
   The API is in diag.h. */
/*lint -save -e{785} */
diag_searchlist_type event_listeners = {FALSE, {0}, {{0}}, 0, 0, 0, NULL};
extern uint8 event_config;
#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 event_listener_match_and_free_cnt =0;
uint32 diag_add_event_listener_malloc_cnt = 0;
uint32 diag_add_event_listener_free_cnt = 0;
#endif 

/*lint -restore */

/*=========================================================================*/
void
event_report (event_id_enum_type event_id)
{
  event_store_type *event= NULL;
  event_store_type *event_ctrl = NULL;
  byte stream_id = 0;

  stream_id = event_read_stream_id(event_id);
   
  event = (event_store_type *) event_q_alloc (event_id, 0,DIAG_STREAM_1);
  if (event)
  {
    event_q_put (event,DIAG_STREAM_1);
  }

  event_ctrl = (event_store_type *) event_q_alloc (event_id, 0,DIAG_STREAM_2);
  
  if (event_ctrl)
  {
    event_q_put (event_ctrl,DIAG_STREAM_2);
  }

  return;
}                               /* event_report */

/*=========================================================================*/
void
event_report_payload (event_id_enum_type event_id, uint8 length, void *payload)
{
  event_store_type *event = NULL;
  event_store_type *event_ctrl = NULL;
  byte stream_id = 0;
   
  if (length > 0 && payload)
  {
    stream_id = event_read_stream_id(event_id);
    event = (event_store_type *) event_q_alloc (event_id, length ,DIAG_STREAM_1);

    if (event)
    {
      memscpy (event->payload.payload, event->payload.length, payload, length);
      event_q_put(event,DIAG_STREAM_1);
    }

    event_ctrl = (event_store_type *) event_q_alloc (event_id, length ,DIAG_STREAM_2);

    if (event_ctrl)
    {
      memscpy (event_ctrl->payload.payload, event_ctrl->payload.length, payload, length);
      event_q_put(event_ctrl,DIAG_STREAM_2);
    }

  }
  else
  {
    event_report (event_id);
  }
  return;
}                               /* event_report_payload */


/*===========================================================================

FUNCTION EVENT_LISTENER_NOTIFY

DESCRIPTION
  This routine is called by the searchlist when a matching listener node is
  found.  It is called once per entry.  
  
  This routine is a wrapper to call the registered listeners.
  
===========================================================================*/
static void
event_listener_notify (uint32 seq_num, void *event, diag_searchlist_node_type *node)
{
  event_listener_entry_type *entry = NULL;
  diag_event_type *event_notify = (diag_event_type *) event;
  
  if (node && event)
  {
    entry = (event_listener_entry_type *) node->goo;

    if (entry->listener)
    {
      entry->listener (seq_num, event_notify, entry->param);
    }    
  }
}

/*===========================================================================

FUNCTION EVENT_SERVICE_LISTENER

DESCRIPTION
  This routine is to be called once per event before the event is formatted
  for the outbound comm packet.
  
ASSUMPTIONS
  'event_store_type' and 'diag_event_type' are assumed to be formatted 
  the same, with the exception of the q_link at the beginning of the store
  type.  Changing either types could result in undesired behavior.

===========================================================================*/
void
event_service_listener (event_store_type *event_store)
{
  if (event_store && (event_config & EVENT_CONFIG_LISTENER_MASK))
  {
    /* NOTE: diag_event_type and event_store_type purposely use the same
       format, except that event_store_type is preceeded by a Q link
       and the event ID field has a union for internal packet formatting.
       If either types are changed, the service will not function properly.
       &event_store->event_id is cast to diag_event_type * in 
       event_listener_notify(). */
    (void) diag_searchlist_search_all (event_store->event_id.id, 
                                &event_listeners, 
                                (void *) &event_store->event_id, /* cast!!! */
                                event_listener_notify);

  }
}

/*===========================================================================

FUNCTION EVENT_LISTENER_MATCH_AND_FREE

DESCRIPTION
  This routine is passed to and called by diag_searchlist_delete() for each 
  node of the search until this routine returns TRUE or the search is 
  exhausted.  This routine is responsible for freeing the 'entry' which 
  was allocated in diag_add_event_listener().

===========================================================================*/
static boolean
event_listener_match_and_free (diag_searchlist_node_type *match_node, 
                               diag_searchlist_node_type *list_node)
{
  boolean found = FALSE;
  event_listener_entry_type *match = NULL;
  event_listener_entry_type *entry = NULL;

  if (match_node && list_node)
  {
    match = (event_listener_entry_type *) match_node->goo;
    entry = (event_listener_entry_type *) list_node->goo;

    if (match && entry)
    {
      if (match->listener == entry->listener &&
          match->param == entry->param)
      {
        found = TRUE;
        diag_listeners_free (entry);

#ifdef FEATURE_DEBUG_DIAG_HEAP
        event_listener_match_and_free_cnt++;
        MSG_FATAL("FREE event_listener_match_and_free = %d ",
            event_listener_match_and_free_cnt,0,0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
      }
    }
  }

  return found;
}

/*=========================================================================*/
boolean
diag_add_event_listener (const unsigned int event_id,
             const diag_event_listener listener, void *param)
{
  event_listener_entry_type *entry = NULL;
  boolean success = FALSE;
  
  entry = (event_listener_entry_type *) 
    diag_listeners_malloc (sizeof (event_listener_entry_type));
  
  
#if defined (DIAG_MV_LISTENER_RPC)
  diag_add_event_listener_on_modem(event_id, listener, param);
#endif

  if (entry)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_add_event_listener_malloc_cnt++;
    MSG_FATAL("MALLOC diag_add_event_listener_malloc = %d Bytes allocated %d",
        diag_add_event_listener_malloc_cnt, sizeof (event_listener_entry_type),0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    entry->listener = listener;
    entry->param = param;

    success = diag_searchlist_add (&event_listeners, event_id, entry);

    if (success)
    {
      /* Assume the critical section is initialized (success, right?). */
      osal_enter_crit_sect (&event_listeners.crit_sect);

      DIAG_SET_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);

      event_control (
        (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
        EVENT_CONFIG_LISTENER_MASK);

      osal_exit_crit_sect (&event_listeners.crit_sect);
    }
    else
    {
      /* unable to add, free 'entry' */
      diag_listeners_free ((void *) entry);
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_add_event_listener_free_cnt++;
      MSG_FATAL("FREE diag_add_event_listener_free_cnt = %d ",
          diag_add_event_listener_free_cnt,0,0);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    }
  }

  return success;
}

/*===========================================================================

FUNCTION EVENT_REMOVE_EVENT_LISTENER

DESCRIPTION
  This is the packet handler for the event service configuration packet.

===========================================================================*/
boolean
diag_remove_event_listener (const unsigned int event_id,
                const diag_event_listener listener, void *param)
{
  event_listener_entry_type entry;
  diag_searchlist_node_type node;
  boolean success = FALSE;
  
  entry.listener = listener;
  entry.param = param;
  
  node.key = event_id;
  node.goo = (void *) &entry;

#if defined (DIAG_MV_LISTENER_RPC)
  diag_remove_event_listener_on_modem(event_id, listener, param);
#endif

  success = diag_searchlist_delete (&event_listeners, node,
                                    event_listener_match_and_free);

  if (success)
  {
    /* Assume the critical section is initialized (success, right?). */
    osal_enter_crit_sect (&event_listeners.crit_sect);

    event_control (
      (event_listeners.count > 0) ? EVENT_CONFIG_LISTENER_MASK : 0, 
      EVENT_CONFIG_LISTENER_MASK);

    if (event_listeners.count == 0)
    {
      DIAG_CLR_MASK (diag_cx_state, DIAG_CX_EVENT_LISTEN_S);
    }

    osal_exit_crit_sect (&event_listeners.crit_sect);
  }

  return success;
}
