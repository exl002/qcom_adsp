/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 RCECB SOURCE MODULE

 GENERAL DESCRIPTION
 this module contains the source implementation for the event notification
 service. supports incrementing events, static value events, masked value
 events. mix and match combinations are possible, but it is anticipated that
 signaling and blocking calls over a specific event name will be uniform
 in nature.

 EXTERNALIZED FUNCTIONS
 yes

 INITIALIZATION AND SEQUENCING REQUIREMENTS
 yes

 Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcecb.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcecb.h"
#include "rcevt_internal.h"

/*===========================================================================

 FUNCTION map

 DESCRIPTION
 none

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution

 ===========================================================================*/
RCEVT_NAME rcecb_map_handle(RCECB_HANDLE handle)
{
   RCEVT_NAME rc = RCE_NULL;                                                     // return result

      rce_init();

   rc = rcecb2nde(handle)->name;                                                 // return result

   return (rc);                                                                  // return status or failure
}

RCECB_HANDLE rcecb_map_name(RCEVT_NAME name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p;

      rce_init();

   nde_p = rce_nde_hashtab_get(rce_nmehash(name));

   while (RCE_NULL != nde_p)                                                     // search for existing node
   {
      if (nde_p->name != name)                                                   // same address compare
      {
         int len = rce_nmelen(nde_p->name);                                      // string compare required

         if (0 == rce_nmecmp(nde_p->name, name, len))                            // lexical comparision
         {
            rc = nde2rcecb(nde_p);                                               // return result

            break;                                                               // break loop for return
         }
      }

      else                                                                       // same address
      {
         rc = nde2rcecb(nde_p);                                                  // return result

         break;                                                                  // break loop for return
      }

      nde_p = nde_p->next;                                                       // iterate to locate
   }

   return (rc);                                                                  // return status or failure
}

extern rce_nde_p rce_hashtab[RCEVT_HASHTABLE_BUCKETS];                           // forward reference

RCECB_HANDLE rcecb_map_name_nolocks(RCEVT_NAME name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_hash_t hash = rce_nmehash(name);                                          // map name to hash
   rce_nde_p nde_p;

   nde_p = rce_hashtab[hash];                                                    // hashtable offset

   while (RCE_NULL != nde_p)                                                     // search for existing node
   {
      if (nde_p->name != name)                                                   // same address compare
      {
         int len = rce_nmelen(nde_p->name);                                      // string compare required

         if (0 == rce_nmecmp(nde_p->name, name, len))                            // lexical comparision
         {
            rc = nde2rcecb(nde_p);                                               // return result

            break;                                                               // break loop for return
         }
      }

      else                                                                       // same address
      {
         rc = nde2rcecb(nde_p);                                                  // return result

         break;                                                                  // break loop for return
      }

      nde_p = nde_p->next;                                                       // iterate to locate
   }

   return (rc);                                                                  // return status or failure
}

/*===========================================================================

 FUNCTION create

 DESCRIPTION
 creates an internal en structure to manage signaling and blocking by name
 caller has opportunity to obtain a handle on success

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution
 successive calls with same name do nothing but return success of operation

 ===========================================================================*/
RCECB_HANDLE rcecb_create_name(RCEVT_NAME name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = rcecb_map_name(name);

   if (RCE_NULL != handle)                                                       // check succes of loop for found node
   {
      rc = handle;                                                               // return result
   }

   else                                                                          // create new
   {
      rce_nde_p nde_p;

      nde_p = rce_nde_pool_alloc();                                              // allocate new node

      if (RCE_NULL != nde_p)
      {
         nde_p->name = name;                                                     // memorize name address

         nde_p->notify_head = RCE_NULL;                                          // initialize notify queue

         rce_nde_hashtab_put(nde_p, rce_nmehash(name));

         rc = nde2rcecb(nde_p);                                                  // return result
      }
   }

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION search

 DESCRIPTION
 searches for a presense of event
 returns success of query

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 none

 ===========================================================================*/
RCECB_BOOL rcecb_search_handle(RCECB_HANDLE handle)
{
   RCECB_BOOL rc = RCECB_FALSE;                                                  // return result

   if (RCE_NULL != rcecb_map_handle(handle))
   {
      rc = RCECB_TRUE;                                                           // return result
   }

   return (rc);                                                                  // return status or failure
}

RCECB_BOOL rcecb_search_name(RCEVT_NAME name)
{
   RCECB_BOOL rc = RCECB_FALSE;                                                  // return result

   if (RCE_NULL != rcecb_map_name(name))
   {
      rc = RCECB_TRUE;                                                           // return result
   }

   return (rc);                                                                  // return status or failure
}

/*===========================================================================

 FUNCTION search_context

 DESCRIPTION
 searches for a presense of context associated to event
 returns success of query

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 none

 ===========================================================================*/
RCECB_BOOL rcecb_search_context_handle(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   RCECB_BOOL rc = RCECB_FALSE;                                                  // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&(nde_p->mutex));                                       // lock mutex

      nfy_p = nde_p->notify_head;                                                // locate notify context

      while (RCE_NULL != nfy_p)                                                  // loop for check
      {
         if ((RCEVT_SIGEX_TYPE_CALLBACK == nfy_p->sigex_type) &&
             (context == (RCECB_CONTEXT)nfy_p->sigex.parm[0]))
         {
            rc = RCECB_TRUE;                                                     // return result

            break;                                                               // break loop
         }

         nfy_p = nfy_p->next;                                                    // iterate to next
      }

      rce_mutex_unlock_dal(&(nde_p->mutex));                                     // unlock mutex
   }

   return (rc);                                                                  // return status or failure
}

RCECB_BOOL rcecb_search_context_name(RCEVT_NAME name, RCECB_CONTEXT context)
{
   RCECB_BOOL rc = RCECB_FALSE;                                                  // return result
   RCECB_HANDLE handle = rcecb_map_name(name);

   rc = rcecb_search_context_handle(handle, context);

   return (rc);                                                                  // return status or failure
}

/*===========================================================================

FUNCTION getcontexts

DESCRIPTION
searches for a handle based on key
returns the current value associated to number of contexts registered

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/
int rcecb_getcontexts_handle(RCECB_HANDLE handle)
{
   int rc = 0;                                                                   // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&(nde_p->mutex));                                       // lock mutex

      nfy_p = nde_p->notify_head;                                                // locate notify context

      while (RCE_NULL != nfy_p)                                                  // loop for check
      {
         if (RCEVT_SIGEX_TYPE_CALLBACK == nfy_p->sigex_type)
         {
            rc++;                                                                // return result
         }

         nfy_p = nfy_p->next;                                                    // iterate to next
      }

      rce_mutex_unlock_dal(&(nde_p->mutex));                                     // unlock mutex
   }

   return (rc);                                                                  // return status or failure
}

int rcecb_getcontexts_name(RCEVT_NAME name)
{
   int rc = 0;                                                                   // return result
   RCECB_HANDLE handle = rcecb_map_name(name);

   rc = rcecb_getcontexts_handle(handle);

   return (rc);                                                                  // return status or failure
}

/*===========================================================================

 FUNCTION register (internal)

 DESCRIPTION
 caller registered to signal notify queue by handle

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution
 caller provides the signal value for the hlos implementation; this
 allows the caller to manage what signal is utilized for notification

 ===========================================================================*/
rce_nfy_p rcecb_register_upd(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rcecb2nde(handle)->notify_head;                                       // locate notify context

   while (RCE_NULL != nfy_p)                                                     // loop for check
   {
      if ((RCEVT_SIGEX_TYPE_CALLBACK == nfy_p->sigex_type) &&
          (context == (RCECB_CONTEXT)nfy_p->sigex.parm[0]))
      {
         rc = nfy_p;                                                             // return result

         break;                                                                  // break loop
      }

      nfy_p = nfy_p->next;                                                       // iterate to next
   }

   return (rc);
}

rce_nfy_p rcecb_register_add(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rce_nfy_pool_alloc();

   if (RCE_NULL != nfy_p)                                                        // populate
   {
      rce_nde_p nde_p = rcecb2nde(handle);

      nfy_p->next = nde_p->notify_head;                                          // insert to head of list

      nde_p->notify_head = nfy_p;                                                // update head

      nfy_p->sigex_type = RCEVT_SIGEX_TYPE_CALLBACK;

      memset(nfy_p->sigex.parm, 0, sizeof(nfy_p->sigex.parm));

      nfy_p->sigex.parm[0] = context;

      rc = nfy_p;                                                                // return result
   }

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION register

 DESCRIPTION
 caller registered to signal notify queue by handle

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution
 caller provides the signal value for the hlos implementation; this
 allows the caller to manage what signal is utilized for notification

 ===========================================================================*/
RCECB_HANDLE rcecb_register_handle(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&(nde_p->mutex));                                          // lock mutex

   if (RCE_NULL != rcecb_register_upd(handle, context)                           // update existing registration
       || RCE_NULL != rcecb_register_add(handle, context))                       // add new registration
   {
      rc = handle;                                                               // return result
   }

   rce_mutex_unlock_dal(&(nde_p->mutex));                                        // unlock mutex

   return (rc);                                                                  // return result
}

RCECB_HANDLE rcecb_register_context_handle(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   return (rcecb_register_context_handle(handle, context));
}

RCECB_HANDLE rcecb_register_name(RCEVT_NAME name, RCECB_CONTEXT context)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = (RCECB_HANDLE)rcevt_map_name(name);
   rce_nde_p nde_p;

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcevt_create_name(name);                                          // implicit creation
   }

   nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&(nde_p->mutex));                                          // lock mutex

   if (RCE_NULL != rcecb_register_upd(handle, context)                           // update existing registration
       || RCE_NULL != rcecb_register_add(handle, context))                       // add new registration
   {
      rc = handle;                                                               // return result
   }

   rce_mutex_unlock_dal(&(nde_p->mutex));                                        // unlock mutex

   return (rc);                                                                  // return result
}

RCECB_HANDLE rcecb_register_context_name(RCEVT_NAME name, RCECB_CONTEXT context)
{
   return (rcecb_register_name(name, context));
}

/*===========================================================================

 FUNCTION unregister (internal)

 DESCRIPTION
 caller unregistered from signal notify queue by handle

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution

 ===========================================================================*/
RCECB_HANDLE rcecb_internal_unregister(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nfy_p nfy_prev = RCE_NULL;
   rce_nfy_p nfy_p;
   rce_nde_p nde_p = rcecb2nde(handle);

   nfy_p = nde_p->notify_head;                                                   // search out on notify queue

   while (RCE_NULL != nfy_p)                                                     // loop for next
   {
      if ((RCEVT_SIGEX_TYPE_CALLBACK == nfy_p->sigex_type) &&
          (context == (RCECB_CONTEXT)nfy_p->sigex.parm[0]))
      {
         if (RCE_NULL == nfy_prev)                                               // head of list check
         {
            nde_p->notify_head = nfy_p->next;                                    // remove from head
         }
         else
         {
            nfy_prev->next = nfy_p->next;                                        // remove from list
         }

         nfy_p->next = RCE_NULL;                                                 // depopulate

         rce_nfy_pool_free(nfy_p);                                               // restore to pool

         rc = nde2rcecb(nde_p);                                                  // return result

         break;                                                                  // exit loop
      }

      nfy_prev = nfy_p;

      nfy_p = nfy_p->next;                                                       // iterate to check next
   }

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION unregister

 DESCRIPTION
 caller unregistered from signal notify queue by handle

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution

 ===========================================================================*/
RCECB_HANDLE rcecb_unregister_handle(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&(nde_p->mutex));                                          // lock mutex

   rc = rcecb_internal_unregister(handle, context);                              // existing

   rce_mutex_unlock_dal(&(nde_p->mutex));                                        // unlock mutex

   return (rc);                                                                  // return result
}

RCECB_HANDLE rcecb_unregister_context_handle(RCECB_HANDLE handle, RCECB_CONTEXT context)
{
   return (rcecb_unregister_context_handle(handle, context));
}

RCECB_HANDLE rcecb_unregister_name(RCEVT_NAME name, RCECB_CONTEXT context)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = rcecb_map_name(name);
   rce_nde_p nde_p;

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&(nde_p->mutex));                                          // lock mutex

   rc = rcecb_internal_unregister(handle, context);                              // existing

   rce_mutex_unlock_dal(&(nde_p->mutex));                                        // unlock mutex

   return (rc);                                                                  // return result
}

RCECB_HANDLE rcecb_unregister_context_name(RCEVT_NAME name, RCECB_CONTEXT context)
{
   return (rcecb_unregister_name(name, context));
}

/*===========================================================================

 FUNCTION signal (internal)

 DESCRIPTION
 caller signals specific handle

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution

 ===========================================================================*/
rce_nde_p rcecb_signal_common(rce_nde_p nde_p)
{
   rce_nde_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   rce_mutex_lock_dal(&(nde_p->mutex));                                          // lock mutex

   nfy_p = nde_p->notify_head;                                                   // traverse notify queue

   while (RCE_NULL != nfy_p)                                                     // iterate
   {
      if (RCEVT_SIGEX_TYPE_CALLBACK == nfy_p->sigex_type)                        // only callback
      {
         void (*callback)(void) = (void(*)(void))(nfy_p->sigex.parm[0]);

         callback(); // <<- other tech team callbacks running this context

         rc = nde_p;                                                             // return result
      }

      nfy_p = nfy_p->next;                                                       // iterate looking for context
   }

   rce_mutex_unlock_dal(&(nde_p->mutex));                                        // unlock mutex

   return (rc);                                                                  // return result
}

rce_nde_p rcecb_signal_common_nolocks(rce_nde_p nde_p)
{
   rce_nde_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = nde_p->notify_head;                                                   // traverse notify queue

   while (RCE_NULL != nfy_p)                                                     // iterate
   {
      if (RCEVT_SIGEX_TYPE_CALLBACK == nfy_p->sigex_type)
      {
         void (*callback)(void) = (void(*)(void))(nfy_p->sigex.parm[0]);

         callback(); // <<- other tech team callbacks running this context

         rc = nde_p;                                                             // return result
      }

      nfy_p = nfy_p->next;                                                       // iterate looking for context
   }

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION signal

 DESCRIPTION
 caller signals specific handle

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 none

 ===========================================================================*/
RCECB_HANDLE rcecb_signal_handle(RCECB_HANDLE handle)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result

   rc = nde2rcecb(rcecb_signal_common(rcecb2nde(handle)));                       // signal handle

   return (rc);                                                                  // return result
}

RCECB_HANDLE rcecb_signal_handle_nolocks(RCECB_HANDLE handle)                    // specialized, no mutex protections, single thread only
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result

   rc = nde2rcecb(rcecb_signal_common_nolocks(rcecb2nde(handle)));               // signal handle

   return (rc);                                                                  // return result
}

RCECB_HANDLE rcecb_signal_name(RCEVT_NAME name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = rcecb_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   rc = rcecb_signal_handle(handle);                                             // signal handle

   return (rc);                                                                  // return result
}

RCECB_HANDLE rcecb_signal_name_nolocks(RCEVT_NAME name)                          // specialized, no mutex protections, single thread only
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = rcecb_map_name_nolocks(name);

   if (RCE_NULL != handle)                                                       // check for existing
   {
      rc = rcecb_signal_handle_nolocks(handle);                                  // signal handle
   }

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION init

 DESCRIPTION
 initialize all internal data structures

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 locks internal mutex during execution

 ===========================================================================*/
void rcecb_init(void)
{
      rce_init();
}

/*===========================================================================

 FUNCTION terminate

 DESCRIPTION
 destroy all internal data structures

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 structures are not actually destroyed for diagnostic reasons; this
 is by design

 ===========================================================================*/
void rcecb_term(void)
{
      rce_term();
}
