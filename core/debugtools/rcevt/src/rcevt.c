/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 RCEVT SOURCE MODULE

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

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcevt.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 ===========================================================================*/

#include "rcevt_dal.h"
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
RCEVT_NAME rcevt_map_handle(RCEVT_HANDLE handle)
{
   RCEVT_NAME rc = RCE_NULL;                                                     // return result
   rce_nde_p nde_p;

   nde_p = rcevt2nde(handle);

   rcevt_init();

   rc = nde_p->name;                                                             // return result

   return (rc);                                                                  // return status or failure
}

RCEVT_HANDLE rcevt_map_name(RCEVT_NAME name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p;

   rcevt_init();

   nde_p = rce_nde_hashtab_get(rce_nmehash(name));

   while (RCE_NULL != nde_p)                                                     // search for existing node
   {
      if (nde_p->name != name)                                                   // same address compare
      {
         int len = rce_nmelen(nde_p->name);                                      // string compare required

         if (0 == rce_nmecmp(nde_p->name, name, len))                            // lexical comparision
         {
            rc = nde2rcevt(nde_p);                                               // return result

            break;                                                               // break loop for return
         }
      }

      else                                                                       // same address
      {
         rc = nde2rcevt(nde_p);                                                  // return result

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
RCEVT_HANDLE rcevt_create_name(RCEVT_NAME name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_map_name(name);

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

         rc = nde2rcevt(nde_p);                                                  // return result
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
RCEVT_BOOL rcevt_search_handle(RCEVT_HANDLE handle)
{
   RCEVT_BOOL rc = RCEVT_FALSE;                                                  // return result

   if (RCE_NULL != rcevt_map_handle(handle))
   {
      rc = RCEVT_TRUE;                                                           // return result
   }

   return (rc);                                                                  // return status or failure
}

RCEVT_BOOL rcevt_search_name(RCEVT_NAME name)
{
   RCEVT_BOOL rc = RCEVT_FALSE;                                                  // return result

   if  (RCE_NULL != rcevt_map_name(name))
   {
      rc = RCEVT_TRUE;                                                           // return result
   }

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
int rcevt_getcontexts_handle(RCEVT_HANDLE handle)
{
   int rc = 0;                                                                   // return result
   rce_nde_p nde_p = rcevt2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&(nde_p->mutex));

      nfy_p = nde_p->notify_head;                                                // locate notify context

      while (RCE_NULL != nfy_p)                                                  // loop for check
      {
         if ((RCEVT_SIGEX_TYPE_SIGDAL == nfy_p->sigex_type) ||                   // types utilized by rcevt
             (RCEVT_SIGEX_TYPE_SIGPOSIX == nfy_p->sigex_type) ||
             (RCEVT_SIGEX_TYPE_SIGQURT == nfy_p->sigex_type) ||
             (RCEVT_SIGEX_TYPE_SIGREX == nfy_p->sigex_type))
         {
            rc++;                                                                // return result
         }

         nfy_p = nfy_p->next;                                                    // iterate to next
      }

      rce_mutex_unlock_dal(&(nde_p->mutex));
   }

   return (rc);                                                                  // return status or failure
}

int rcevt_getcontexts_name(RCEVT_NAME name)
{
   int rc = 0;                                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_getcontexts_handle(handle);
   }

   return (rc);                                                                  // return status or failure
}

/*===========================================================================

 FUNCTION getcount

 DESCRIPTION
 searches for a handle based on name
 returns the current threshold associated to caller if requested

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution

 ===========================================================================*/
RCEVT_THRESHOLD rcevt_getcount_handle(RCEVT_HANDLE handle)
{
   RCEVT_THRESHOLD rc = RCE_THRESHOLD_INIT;                                      // default
   rce_nde_p nde_p = rcevt2nde(handle);

   rce_mutex_lock_dal(&(nde_p->mutex));

   rc = nde_p->threshold;

   rce_mutex_unlock_dal(&(nde_p->mutex));

   return (rc);                                                                  // return result
}

RCEVT_THRESHOLD rcevt_getcount_name(RCEVT_NAME name)
{
   RCEVT_THRESHOLD rc = RCE_THRESHOLD_INIT;                                      // default
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rce_nde_p nde_p = rcevt2nde(handle);

      rce_mutex_lock_dal(&(nde_p->mutex));

      rc = nde_p->threshold;

      rce_mutex_unlock_dal(&(nde_p->mutex));
   }

   return (rc);                                                                  // return result
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
 requires locks internal mutex during execution
 caller provides the signal value for the hlos implementation; this
 allows the caller to manage what signal is utilized for notification

 ===========================================================================*/
rce_nfy_p rcevt_internal_register_sigex_update(RCEVT_HANDLE handle, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nde_p nde_p;
   rce_nfy_p nfy_p;

   nde_p =  rcevt2nde(handle);
   nfy_p = nde_p->notify_head;                                                   // locate notify context

   while (RCE_NULL != nfy_p)                                                     // loop for check
   {
      if (type == nfy_p->sigex_type)                                             // comparision based on type
      {
         if (RCEVT_SIGEX_TYPE_SIGDAL == nfy_p->sigex_type && RCEVT_TRUE == rce_nfy_eq_dal(nfy_p, sigex))
         {
            rce_nfy_init_dal(nfy_p, sigex);
            nfy_p->threshold = threshold;                                        // update member
            rc = nfy_p;                                                          // return result
            break;
         }

         else if (RCEVT_SIGEX_TYPE_SIGPOSIX == nfy_p->sigex_type && RCEVT_TRUE == rce_nfy_eq_posix(nfy_p, sigex))
         {
            rce_nfy_init_posix(nfy_p, sigex);
            nfy_p->threshold = threshold;                                        // update member
            rc = nfy_p;                                                          // return result
            break;
         }

         else if (RCEVT_SIGEX_TYPE_SIGQURT == nfy_p->sigex_type && RCEVT_TRUE == rce_nfy_eq_qurt(nfy_p, sigex))
         {
            rce_nfy_init_qurt(nfy_p, sigex);
            nfy_p->threshold = threshold;                                        // update member
            rc = nfy_p;                                                          // return result
            break;
         }

         else if (RCEVT_SIGEX_TYPE_SIGREX == nfy_p->sigex_type && RCEVT_TRUE == rce_nfy_eq_rex(nfy_p, sigex))
         {
            rce_nfy_init_rex(nfy_p, sigex);
            nfy_p->threshold = threshold;                                        // update member
            rc = nfy_p;                                                          // return result
            break;
         }
      }

      nfy_p = nfy_p->next;                                                       // iterate to next
   }

   return (rc);
}

rce_nfy_p rcevt_internal_register_sigex_add(RCEVT_HANDLE handle, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rce_nfy_pool_alloc();

   if (RCE_NULL != nfy_p)                                                        // populate
   {
      rce_nde_p nde_p = rcevt2nde(handle);

      nfy_p->next = nde_p->notify_head;                                          // insert to head of list

      nde_p->notify_head = nfy_p;                                                // update head

      nfy_p->sigex_type = type;                                                  // update member

      nfy_p->threshold = threshold;                                              // update member

      if (RCEVT_SIGEX_TYPE_SIGDAL == type)   { rce_nfy_init_dal(nfy_p, sigex); }

      else if (RCEVT_SIGEX_TYPE_SIGPOSIX == type)  { rce_nfy_init_posix(nfy_p, sigex); }

      else if (RCEVT_SIGEX_TYPE_SIGQURT == type)  { rce_nfy_init_qurt(nfy_p, sigex); }

      else if (RCEVT_SIGEX_TYPE_SIGREX == type)  { rce_nfy_init_rex(nfy_p, sigex); }

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

**AVAILABILITY BASED ON UNDERLYING HLOS INFRASTRUCTURE**

 ===========================================================================*/
RCEVT_HANDLE rcevt_register_sigex_handle(RCEVT_HANDLE handle, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_THRESHOLD threshold = RCE_THRESHOLD_DEFAULT;                            // default
   rce_nde_p nde_p = rcevt2nde(handle);

   if (RCE_NULL != handle)
   {
      rce_nfy_p nfy_p = RCE_NULL;

      rce_mutex_lock_dal(&(nde_p->mutex));

      nfy_p = rcevt_internal_register_sigex_update(handle, threshold, type, sigex); // update an existing

      if (RCE_NULL != nfy_p)
      {
         rc = handle;                                                            // return result
      }
      else
      {
         nfy_p = rcevt_internal_register_sigex_add(handle, threshold, type, sigex); // add a new

         if (RCE_NULL != nfy_p)
         {
            rc = handle;                                                         // return result
         }
         else
         {
            RCE_ERR_FATAL("cannot update/add event in register", 0, 0, 0);
         }
      }

      rce_mutex_unlock_dal(&(nde_p->mutex));
   }

   return (rc);                                                                  // return result
}

RCEVT_HANDLE rcevt_register_sigex_name(RCEVT_NAME name, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_register_sigex_handle(handle, type, sigex);
   }

   return (rc);                                                                  // return result
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
 requires locks internal mutex during execution

 ===========================================================================*/
RCEVT_HANDLE rcevt_internal_unregister_sigex(RCEVT_HANDLE handle, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nfy_p nfy_prev = RCE_NULL;
   rce_nde_p nde_p = rcevt2nde(handle);
   rce_nfy_p nfy_p;

   nfy_p = nde_p->notify_head;                                                   // search out on notify queue

   while (RCE_NULL != nfy_p)                                                     // loop for next
   {
      if (type == nfy_p->sigex_type)                                             // check based on type
      {
         if (RCEVT_SIGEX_TYPE_SIGDAL == type && RCEVT_TRUE == rce_nfy_eq_dal(nfy_p, sigex))
         {
            if (RCE_NULL == nfy_prev)                                            // head of list check
            {
               nde_p->notify_head = nfy_p->next;                                 // remove from head
            }
            else
            {
               nfy_prev->next = nfy_p->next;                                     // remove from list
            }

            nfy_p->next = RCE_NULL;                                              // depopulate

            rce_nfy_term_dal(nfy_p);                                             // signal internals

            rce_nfy_pool_free(nfy_p);                                            // restore to pool

            rc = nde2rcevt(nde_p);                                               // return result

            break;
         }

         else if (RCEVT_SIGEX_TYPE_SIGPOSIX == type && RCEVT_TRUE == rce_nfy_eq_posix(nfy_p, sigex))
         {
            if (RCE_NULL == nfy_prev)                                            // head of list check
            {
               nde_p->notify_head = nfy_p->next;                                 // remove from head
            }
            else
            {
               nfy_prev->next = nfy_p->next;                                     // remove from list
            }

            nfy_p->next = RCE_NULL;                                              // depopulate

            rce_nfy_term_posix(nfy_p);                                           // signal internals

            rce_nfy_pool_free(nfy_p);                                            // restore to pool

            rc = nde2rcevt(nde_p);                                               // return result

            break;
         }

         else if (RCEVT_SIGEX_TYPE_SIGQURT == type && RCEVT_TRUE == rce_nfy_eq_qurt(nfy_p, sigex))
         {
            if (RCE_NULL == nfy_prev)                                            // head of list check
            {
               nde_p->notify_head = nfy_p->next;                                 // remove from head
            }
            else
            {
               nfy_prev->next = nfy_p->next;                                     // remove from list
            }

            nfy_p->next = RCE_NULL;                                              // depopulate

            rce_nfy_term_qurt(nfy_p);                                            // signal internals

            rce_nfy_pool_free(nfy_p);                                            // restore to pool

            rc = nde2rcevt(nde_p);                                               // return result

            break;
         }

         else if (RCEVT_SIGEX_TYPE_SIGREX == type && RCEVT_TRUE == rce_nfy_eq_rex(nfy_p, sigex))
         {
            if (RCE_NULL == nfy_prev)                                            // head of list check
            {
               nde_p->notify_head = nfy_p->next;                                 // remove from head
            }
            else
            {
               nfy_prev->next = nfy_p->next;                                     // remove from list
            }

            nfy_p->next = RCE_NULL;                                              // depopulate

            rce_nfy_term_rex(nfy_p);                                             // signal internals

            rce_nfy_pool_free(nfy_p);                                            // restore to pool

            rc = nde2rcevt(nde_p);                                               // return result

            break;
         }
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
RCEVT_HANDLE rcevt_unregister_sigex_handle(RCEVT_HANDLE handle, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcevt2nde(handle);

   rce_mutex_lock_dal(&(nde_p->mutex));

   rc = rcevt_internal_unregister_sigex(handle, type, sigex);                    // existing

   rce_mutex_unlock_dal(&(nde_p->mutex));

   return (rc);                                                                  // return result
}

RCEVT_HANDLE rcevt_unregister_sigex_name(RCEVT_NAME name, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_unregister_sigex_handle(handle, type, sigex);                   // existing
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
RCEVT_HANDLE rcevt_signal_handle(RCEVT_HANDLE handle)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcevt2nde(handle);

   if (RCE_NULL != nde_p)
   {
      if (RCE_NDE_COOKIE == nde_p->nde_cookie)
      {
         rce_nfy_p nfy_p;

         rce_mutex_lock_dal(&(nde_p->mutex));

         nde_p->threshold += 1;                                                  // increment threshold

         nfy_p = nde_p->notify_head;                                             // traverse notify queue

         while (RCE_NULL != nfy_p)                                               // iterate
         {
            if (RCE_NFY_COOKIE == nfy_p->nfy_cookie)                             // corruption check before use
            {
               if (nde_p->threshold >= nfy_p->threshold)                         // signal on condition
               {
                  if (RCEVT_SIGEX_TYPE_SIGDAL == nfy_p->sigex_type)  { rce_nfy_sig_dal(nde_p, nfy_p); }

                  else if (RCEVT_SIGEX_TYPE_SIGPOSIX == nfy_p->sigex_type) { rce_nfy_sig_posix(nde_p, nfy_p); }

                  else if (RCEVT_SIGEX_TYPE_SIGQURT == nfy_p->sigex_type) { rce_nfy_sig_qurt(nde_p, nfy_p); }

                  else if (RCEVT_SIGEX_TYPE_SIGREX == nfy_p->sigex_type) { rce_nfy_sig_rex(nde_p, nfy_p); }
               }
            }
            else
            {
               RCE_ERR_FATAL("cookie mismatch in signal", 0, 0, 0);              // corruption check fails
            }

            nfy_p = nfy_p->next;                                                 // iterate looking for context
         }

         rce_mutex_unlock_dal(&(nde_p->mutex));

         rc = nde2rcevt(nde_p);
      }
      else
      {
         RCE_ERR_FATAL("cookie mismatch in signal", 0, 0, 0);                    // corruption check fails
      }
   }

   return (rc);                                                                  // return result
}

RCEVT_HANDLE rcevt_signal_name(RCEVT_NAME name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_signal_handle(handle);                                          // signal handle
   }

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION wait (internal)

 DESCRIPTION
 internal block mechanism

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 locks internal mutex during execution
 caller does not block if the signal condition has already been met

 ===========================================================================*/
RCEVT_HANDLE rcevt_wait_common(RCEVT_HANDLE handle, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcevt2nde(handle);
   rce_nfy_p nfy_p = RCE_NULL;

   rce_mutex_lock_dal(&(nde_p->mutex));

   if (nde_p->threshold < threshold)                                             // implicit register
   {
      nfy_p = rcevt_internal_register_sigex_update(handle, threshold, type, sigex); // update an existing

      if (RCE_NULL != nfy_p)
      {
         rc = handle;                                                            // return result
      }
      else
      {
         nfy_p = rcevt_internal_register_sigex_add(handle, threshold, type, sigex); // add a new

         if (RCE_NULL != nfy_p)
         {
            rc = handle;                                                         // return result
         }
         else
         {
            RCE_ERR_FATAL("cannot update/add event in wait", 0, 0, 0);
         }
      }

      if (RCE_NFY_COOKIE == nfy_p->nfy_cookie)                                   // corruption check before use
      {
         rce_mutex_unlock_dal(&(nde_p->mutex));

         if (RCEVT_SIGEX_TYPE_SIGDAL == nfy_p->sigex_type)  { rce_nfy_wait_dal(nde_p, nfy_p); }

         else if (RCEVT_SIGEX_TYPE_SIGPOSIX == nfy_p->sigex_type) { rce_nfy_wait_posix(nde_p, nfy_p); }

         else if (RCEVT_SIGEX_TYPE_SIGQURT == nfy_p->sigex_type) { rce_nfy_wait_qurt(nde_p, nfy_p); }

         else if (RCEVT_SIGEX_TYPE_SIGREX == nfy_p->sigex_type) { rce_nfy_wait_rex(nde_p, nfy_p); }

         rce_mutex_lock_dal(&(nde_p->mutex));
      }
      else
      {
         RCE_ERR_FATAL("cookie mismatch in wait", 0, 0, 0);                      // corruption check fails
      }

      rcevt_internal_unregister_sigex(handle, type, sigex);                      // implicit unregister
   }

   rce_mutex_unlock_dal(&(nde_p->mutex));

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION wait

 DESCRIPTION
 caller blocks for specific handle to be signaled

 DEPENDENCIES
 none

 RETURN VALUE
 success of operation

 SIDE EFFECTS
 caller does not block if the signal condition has already been met

 ===========================================================================*/
RCEVT_HANDLE rcevt_wait_sigex_handle(RCEVT_HANDLE handle, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   if (RCE_NULL != handle)
   {
      rc = rcevt_wait_common(handle, threshold, type, sigex);                    // wait
   }

   return (rc);
}

RCEVT_HANDLE rcevt_wait_sigex_name(RCEVT_NAME name, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_wait_common(handle, threshold, type, sigex);                    // wait
   }

   return (rc);
}

RCEVT_HANDLE rcevt_wait_count_handle(RCEVT_HANDLE handle, RCEVT_THRESHOLD threshold)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   DALResult rcdal;
   DALSYSEventObj signalObject;
   RCEVT_SIGEX_SIGDAL sig;

   // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
   if (DAL_SUCCESS != (rcdal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &(sig.signal), &(signalObject))))
   {
      RCE_ERR_FATAL("event create", 0, 0, 0);
   }

   rc = rcevt_wait_common(handle, threshold, RCEVT_SIGEX_TYPE_SIGDAL, &sig);

   // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
   if (DAL_SUCCESS != (rcdal = DALSYS_DestroyObject(sig.signal)))
   {
      RCE_ERR_FATAL("event destroy", 0, 0, 0);
   }

   return (rc);                                                                  // return result
}

RCEVT_HANDLE rcevt_wait_handle(RCEVT_HANDLE handle)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   rc = rcevt_wait_count_handle(handle, RCE_THRESHOLD_DEFAULT);

   return (rc);                                                                  // return result
}

RCEVT_HANDLE rcevt_wait_count_name(RCEVT_NAME name, RCEVT_THRESHOLD threshold)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_wait_count_handle(handle, threshold);
   }

   return (rc);                                                                  // return result
}

RCEVT_HANDLE rcevt_wait_name(RCEVT_NAME name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   rc = rcevt_wait_count_name(name, RCE_THRESHOLD_DEFAULT);

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
void rcevt_init(void)
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
void rcevt_term(void)
{
   rce_term();
}
