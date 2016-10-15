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

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcevt_common.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcevt.h"
#include "rcevt_internal.h"

/*===========================================================================

 FUNCTION rce_nme

 DESCRIPTION
 internal name handling routine to map a string to a hash; this specific
 routine is originally specified in the System V ABI Application Binary
 Interface dealing with symbol tables in ELF images.

 DEPENDENCIES
 none

 RETURN VALUE
 hashtable offest modulo the number of buckets in the hashtable

 SIDE EFFECTS
 none

 ===========================================================================*/
rce_hash_t rce_nmehash(RCEVT_NAME name)
{
   rce_hash_t hash = 0;

   while ((rce_nme_t)'\0' != *name)
   {
      rce_hash_t temp;

      hash = (hash << 4) + (rce_hash_t)(*name);

      if (0 != (temp = hash & 0xf0000000))
      {
         hash ^= (temp >> 24);
      }

      hash &= (~temp);

      name++;
   }

   return (hash % (sizeof(rce_hashtab)/sizeof(rce_nde_p)));
}

int rce_nmelen(RCEVT_NAME name)
{
   RCEVT_NAME s;

   for (s = name; (rce_nme_t)'\0' != *s; ++s)
      /* NULL */;

   return (s - name);
}

int rce_nmecmp(RCEVT_NAME name_1, RCEVT_NAME name_2, int len)
{
   if (0 == len)                                                                 // no work; return
   {
      return (0);                                                                // lexical compare identical
   }

   while (len-- > 0 && *name_1 == *name_2)                                       // iterate comparing name strings
   {
      if (0 == len || (rce_nme_t)'\0' == *name_1)                                // len hits zero or NULL terminator
      {
         return (0);                                                             // lexical compare identical
      }

      name_1++, name_2++;                                                        // setup for next comparision
   }

   return ((*name_1 < *name_2) ? -1 : (*name_1 > *name_2));                      // lexical analysis and return result (-1, 0, 1)
}

/*===========================================================================

 FUNCTION rce_nfy_pool

 DESCRIPTION
 internal storage initializr for notify structures queue pool

 DEPENDENCIES
 requires prior lock of internal mutex during execution

 RETURN VALUE
 notify pool allocated

 SIDE EFFECTS
 none

 ===========================================================================*/
rce_nfy_p rce_nfy_pool_init(void)
{
   rce_nfy_pool_p next_pool = RCE_NULL;                                          // return result

   if (RCE_NULL == rce_internal.nfy_pool_head_p)                                 // first call use static pool
   {
      next_pool = &rce_nfy_pool_static;                                          // first pool allocation static
   }

   else if (RCEVT_TRUE == rce_internal.dynamic_use)                              // else dynamic allocation allowed; attempt pool expansion
   {
      next_pool = (rce_nfy_pool_p)rce_malloc(sizeof(rce_nfy_pool_t));            // allocate
   }

   if (RCE_NULL != next_pool)                                                    // populate
   {
      int i;

      for (i = 0; i < RCEVT_NOTIFY_POOL_SIZ; i++)                                // initialize pool internal
      {
         if (i != (RCEVT_NOTIFY_POOL_SIZ - 1))
         {
            next_pool->nfy_pool[i].next = &(next_pool->nfy_pool[i + 1]);         // next entry
         }

         else
         {
            next_pool->nfy_pool[i].next = rce_internal.nfy_pool_free_p;          // last entry of pool list
         }

         memset(&(next_pool->nfy_pool[i].sigex), 0, sizeof(next_pool->nfy_pool[i].sigex));

         next_pool->nfy_pool[i].sigex_type = RCEVT_SIGEX_TYPE_NONE;              // initialize member

         next_pool->nfy_pool[i].threshold = 0;                                   // initialize member

         next_pool->nfy_pool[i].nfy_cookie = RCE_NFY_COOKIE;                     // initialize member (set cookie last)
      }

      rce_internal.nfy_pool_free_p = &(next_pool->nfy_pool[0]);                  // initialize free list

      next_pool->next = rce_internal.nfy_pool_head_p;                            // link this pool to the head of the chain

      rce_internal.nfy_pool_head_p = next_pool;                                  // point to the new pool as the head
   }

   else                                                                          // failure for pool expansion
   {
      RCE_ERR_FATAL("no resource, pool expansion exhausted", 0, 0, 0);
   }

   return (rce_internal.nfy_pool_free_p);                                        // return result
}

rce_nfy_p rce_nfy_pool_alloc(void)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   rce_mutex_lock_dal(&(rce_internal.mutex));                                    // lock mutex

   if (RCE_NULL == rce_internal.nfy_pool_free_p)
   {
      nfy_p = rce_nfy_pool_init();                                               // obtain reference from pool, possible pool expansion
   }

   else
   {
      nfy_p = rce_internal.nfy_pool_free_p;
   }

   if (RCE_NULL != nfy_p)
   {
      rce_internal.nfy_pool_free_p = nfy_p->next;                                // unlink from free list

      nfy_p->next = RCE_NULL;                                                    // initialize member

      nfy_p->sigex_type = RCEVT_SIGEX_TYPE_NONE;                                 // initialize member

      rc = nfy_p;                                                                // return result

      rce_mutex_unlock_dal(&(rce_internal.mutex));                               // unlock mutex

      return (rc);                                                               // return result
   }

   else
   {
      return(RCE_NULL);
   }
}

rce_nfy_p rce_nfy_pool_free(rce_nfy_p nfy_p)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result

   rce_mutex_lock_dal(&(rce_internal.mutex));                                    // lock mutex

   nfy_p->next = rce_internal.nfy_pool_free_p;                                   // return to head of free list

   rce_internal.nfy_pool_free_p = nfy_p;

   rc = nfy_p;                                                                   // return result

   rce_mutex_unlock_dal(&(rce_internal.mutex));                                  // unlock mutex

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION rce_nde_pool

 DESCRIPTION
 internal storage initializr for node structures pool (handles)

 DEPENDENCIES
 requires prior lock of internal mutex during execution

 RETURN VALUE
 node pool allocated

 SIDE EFFECTS
 none

 ===========================================================================*/
rce_nde_p rce_nde_pool_init(void)
{
   rce_nde_pool_p next_pool = RCE_NULL;                                          // return result

   if (RCE_NULL == rce_internal.nde_pool_head_p)                                 // first call use static pool
   {
      next_pool = &rce_nde_pool_static;                                          // first pool allocation static
   }

   else if (RCEVT_TRUE == rce_internal.dynamic_use)                              // else dynamic allocation allowed; attempt pool expansion
   {
      next_pool = (rce_nde_pool_p)rce_malloc(sizeof(rce_nde_pool_t));            // allocate
   }

   if (RCE_NULL != next_pool)                                                    // populate
   {
      int i;

      for (i = 0; i < RCEVT_NODE_POOL_SIZ; i++)                                  // initialize pool internal
      {
         if (i != (RCEVT_NODE_POOL_SIZ - 1))
         {
            next_pool->nde_pool[i].next = &(next_pool->nde_pool[i + 1]);         // next entry
         }

         else
         {
            next_pool->nde_pool[i].next = rce_internal.nde_pool_free_p;          // last entry of pool list
         }

         memset(&(next_pool->nde_pool[i].mutex), 0, sizeof(next_pool->nde_pool[i].mutex));

         rce_mutex_init_dal(&(next_pool->nde_pool[i].mutex));                    // initialize member

         next_pool->nde_pool[i].notify_head = RCE_NULL;                          // initialize member

         next_pool->nde_pool[i].name = RCE_NULL;                                 // initialize member

         next_pool->nde_pool[i].threshold = 0;                                   // initialize member

         next_pool->nde_pool[i].nde_cookie = RCE_NDE_COOKIE;                     // initialize member (set cookie last)
      }

      rce_internal.nde_pool_free_p = &(next_pool->nde_pool[0]);                  // initialize free list

      next_pool->next = rce_internal.nde_pool_head_p;                            // link this pool to the head of the chain

      rce_internal.nde_pool_head_p = next_pool;                                  // point to the new pool as the head
   }

   else                                                                          // failure for pool expansion
   {
      RCE_ERR_FATAL("no resource, pool expansion exhausted", 0, 0, 0);
   }

   return (rce_internal.nde_pool_free_p);                                        // return result
}

rce_nde_p rce_nde_pool_alloc(void)
{
   rce_nde_p rc = RCE_NULL;                                                      // return result
   rce_nde_p nde_p;

   rce_mutex_lock_dal(&(rce_internal.mutex));                                    // lock mutex

   if (RCE_NULL == rce_internal.nde_pool_free_p)
   {
      nde_p = rce_nde_pool_init();                                               // obtain reference from pool, possible pool expansion
   }
   else
   {
      nde_p = rce_internal.nde_pool_free_p;
   }

   if (RCE_NULL != nde_p)
   {
      rce_internal.nde_pool_free_p = nde_p->next;                                // unlink from free list

      nde_p->next = RCE_NULL;                                                    // initialize member

      nde_p->notify_head = RCE_NULL;                                             // initialize member

      rc = nde_p;                                                                // return result

      rce_mutex_unlock_dal(&(rce_internal.mutex));                               // unlock mutex

      return (rc);                                                               // return result
   }
   else
   {
      return (RCE_NULL);
   }
}

rce_nde_p rce_nde_pool_free(rce_nde_p nde_p)
{
   rce_nde_p rc = RCE_NULL;                                                      // return result

   nde_p->next = rce_internal.nde_pool_free_p;                                   // return to head of free list

   rce_internal.nde_pool_free_p = nde_p;

   rc = nde_p;                                                                   // return result

   return (rc);                                                                  // return result
}

/*===========================================================================

 FUNCTION rce_hashtab

 DESCRIPTION
 internal hash table storage allocator

 DEPENDENCIES
 requires prior lock of internal mutex during execution

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rce_hashtab_init(void)
{
   memset(&rce_hashtab, 0, sizeof(rce_hashtab));
}

rce_nde_p rce_nde_hashtab_get(rce_hash_t hash)
{
   rce_nde_p rc = RCE_NULL;

   rce_mutex_lock_dal(&(rce_internal.mutex));                                    // lock mutex

   rc = rce_hashtab[hash];                                                       // hashtable offset

   rce_mutex_unlock_dal(&(rce_internal.mutex));                                  // unlock mutex

   return (rc);
}

void rce_nde_hashtab_put(rce_nde_p nde_p, rce_hash_t hash)
{
   rce_mutex_lock_dal(&(rce_internal.mutex));                                    // lock mutex

   nde_p->next = rce_hashtab[hash];                                              // initalize next node pointer to current bucket list head

   rce_hashtab[hash] = nde_p;                                                    // insert node pointer to hash bucket list

   rce_mutex_unlock_dal(&(rce_internal.mutex));                                  // unlock mutex
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
void rce_init(void)
{
   if (RCE_NDE_COOKIE != rce_internal.init_flag)                                 // first call to insure service init
   {
      DALSYS_InitMod(NULL);                                                      // prerequsite dependency

      rce_mutex_init_dal(&(rce_internal.mutex));                                 // initialize mutex

      rce_mutex_lock_dal(&(rce_internal.mutex));                                 // lock mutex

      rce_internal.dynamic_use = TRUE;                                           // allow pools to grow at runtime

      rce_hashtab_init();                                                        // allocate initial hash table buckets

      rce_nde_pool_init();                                                       // allocate initial pool storage

      rce_nfy_pool_init();                                                       // allocate initial pool storage

      rce_mutex_unlock_dal(&(rce_internal.mutex));                               // unlock mutex

      rce_internal.init_flag = RCE_NDE_COOKIE;                                   // mark service init
   }
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
void rce_term(void)
{
   if (RCE_NDE_COOKIE != rce_internal.init_flag)                                 // first call to insure service init
   {
      rce_mutex_lock_dal(&(rce_internal.mutex));                                 // lock mutex

      // return structures to heap? diagnostic postmortem precludes a proper
      // cleanup. it is anticipated that this service once started does not go
      // away.

      //rce_term_hashtable();

      //rce_term_nfy_pool();

      //rce_term_node_pool();

      //rce_term_name_pool();

      rce_mutex_unlock_dal(&(rce_internal.mutex));                               // unlock mutex

      //rce_mutex_term(&(rce_internal.mutex));                                   // terminate mutex

      // DALSYS DEINIT;                                                          // prerequsite dependency

      rce_internal.init_flag = 0;                                                // mark service init
   }
}
