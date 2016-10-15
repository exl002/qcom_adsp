/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 RCINIT SOURCE MODULE

 GENERAL DESCRIPTION
 this module contains the source implementation for the rcinit framework

 EXTERNALIZED FUNCTIONS
 yes

 INITIALIZATION AND SEQUENCING REQUIREMENTS
 yes

 Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.adsp/2.2/debugtools/rcinit/src/rcinit.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "err.h"
#include "rciniti.h"
#include "rcevt_dal.h"
#include "sys_m_messages.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

#define dwMaxNumEvents                          2                                // DALSys Worker Loop Event Queue Depth for DALTASK

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/* static */ struct // clients do not need to access this
{
   // processing state
   // trace32: v.v rcinit_internal

   RCINIT_GROUP current_group;                                                   // current group being processed

   boolean process_initfn;                                                       // current group being processed initfn
   boolean process_task;                                                         // current group being processed task

   unsigned long stacks_size;                                                    // total stacks allotment (set after process_stacks)

   // internal events

   // trace32: v.v (rce_nde_t*)rcinit_internal.define : to view the list of blockers
   // trace32: v.v (rce_nde_t*)rcinit_internal.defineack : to view the list of blockers (concurrent)

   RCEVT_HANDLE define;                                                          // rcinit_task <-> client task define coordination, internal management
   RCEVT_HANDLE defineack;                                                       // rcinit_task <-> client task defineack coordination, internal management

   // internal worker

   const rcinit_info_p* worker_argv;                                             // worker argv (coordinated by rcevt initfn)

   DALSYSEventHandle hEventWorkLoop;                                             // worker event
   DALSYSEventHandle hEventWorkLoopAck;                                          // worker event
   DALSYSWorkLoopHandle hWorkLoop;                                               // worker loop

   // internal initfnspawn

   void* InitFnEntry;                                                            // diagnostic instrument
   DALSYSEventHandle hEventInitFnSpawn;                                          // initfnspawn event

   // internal rcinit

   DALSYSEventHandle hEventShutdownTask;                                         // worker event
   rcinit_stack_p stacks;                                                        // base to block of stacks allotment
   unsigned long count;                                                          // unit test initfn

} rcinit_internal;                                                               // trace32: v.v rcinit_internal

static DALSYSEventHandle hEventCoreStarted;                                            // internal event to unblock core_main

/*===========================================================================

 FUNCTION rcinit_internal_nmelen

 DESCRIPTION
 internal key handling routine to obtain length

 DEPENDENCIES
 none

 RETURN VALUE
 length of result sans terminator

 SIDE EFFECTS
 none

 ===========================================================================*/
int rcinit_internal_nmelen(rcinit_name_p name)
{
   rcinit_name_p ptr = name;                                                     // return result

   while ((rcinit_name_t)'\0' != *ptr)                                           // iterate to locate NULL terminator
   {
      ptr++;                                                                     // setup for next comparision
   }

   return(ptr - name);                                                           // return length of name (sans NULL terminator)
}

/*===========================================================================

 FUNCTION rcevt_internal_nmecmp

 DESCRIPTION
 internal key handling routine to lexical compare two names

 DEPENDENCIES
 none

 RETURN VALUE
 lexical comparision result

 SIDE EFFECTS
 will not exceed length passed in, should be sizeof() destination buffer

 ===========================================================================*/
int rcinit_internal_nmecmp(rcinit_name_p name1, rcinit_name_p name2, int len)
{
   if (0 == len)                                                                 // no work; return
   {
      return(0);                                                                 // lexical compare identical
   }

   while (len-- > 0 && *name1 == *name2)                                         // iterate comparing names
   {
      if (0 == len || (rcinit_name_t)'\0' == *name1)                             // len hits zero or NULL terminator
      {
         return(0);                                                              // lexical compare identical
      }

      name1++, name2++;                                                          // setup for next comparision
   }

   return(((rcinit_name_t)*name1 < (rcinit_name_t)*name2) ? -1 : ((rcinit_name_t)*name1 > (rcinit_name_t)*name2)); // lexical analysis and return result (-1, 0, 1)
}

/*===========================================================================

 FUNCTION rcinit_internal_name_map_search

 DESCRIPTION
 internal name map search to locate rcinit_info offset in the internal
 rcinit_info database

 DEPENDENCIES
 none

 RETURN VALUE
 zero based offset on success
 -1 on failure

 SIDE EFFECTS
 none

 ===========================================================================*/
int rcinit_internal_name_map_search(RCINIT_NAME value, int len, int low, int high)
{
   while (low <= high)
   {
      int compare;
      int mid = (low + high) / 2;

      compare = rcinit_internal_nmecmp(rcinit_internal_name_map[mid].name, value, len);   // compare array[mid] to value

      if (0 == compare)                                                          // array[mid] == value
      {
         return(mid);
      }
      else if (compare > 0)                                                      // array[mid] > value
      {
         high = mid - 1;
      }
      else                                                                       // array[mid] < value
      {
         low = mid + 1;
      }
   }
   return(-1);
}

/*===========================================================================

 FUNCTION rcinit_lookup

 DESCRIPTION
 rcinit task database lookup accessor functions

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none

 ===========================================================================*/
RCINIT_INFO rcinit_lookup(RCINIT_NAME name)
{
   int offset;

   offset = rcinit_internal_name_map_search(name, rcinit_internal_nmelen(name) + 1, 0, rcinit_internal_name_map_size - 2);

   if (-1 != offset)
   {
      return(rcinit_internal_name_map[offset].info);
   }

   return(RCINIT_NULL);
}

RCINIT_NAME rcinit_lookup_name_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   return(rcinit_p->name);
}

void* rcinit_lookup_entry_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   return(rcinit_p->entry);
}

void* rcinit_lookup_entry(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_entry_info(info));
   }

   return(RCINIT_NULL);
}

RCINIT_PRIO rcinit_lookup_prio_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   return(rcinit_p->prio);
}

RCINIT_PRIO rcinit_lookup_prio(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_prio_info(info));
   }

   return(RCINIT_ZERO);
}

unsigned long rcinit_lookup_stksz_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   return(rcinit_p->stksz);
}

unsigned long rcinit_lookup_stksz(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_stksz_info(info));
   }

   return(RCINIT_ZERO);
}

unsigned long rcinit_lookup_cpu_affinity_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   return(rcinit_p->cpu_affinity);
}

unsigned long rcinit_lookup_cpu_affinity(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_cpu_affinity_info(info));
   }

   return(RCINIT_ZERO);
}

int rcinit_lookup_group_count(void)
{
   int rc = 0;
   const rcinit_info_p** rcinit_sequence_group;

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      rc++;
      rcinit_sequence_group++;
   }

   return (rc);
}

RCINIT_GROUP rcinit_lookup_group(RCINIT_NAME name)
{
   RCINIT_GROUP rc = RCINIT_GROUP_0;
   const rcinit_info_p** rcinit_sequence_group;

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_p* rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         int compare = rcinit_internal_nmecmp(rcinit_p->name, name, rcinit_internal_nmelen(rcinit_p->name) + 1);

         if (0 == compare)
         {
            return (rc);
         }

         rcinit_group++; // next; this group
      }

      rc++; // processing next group

      rcinit_sequence_group++; // next; sequence group
   }

   return (RCINIT_GROUP_NONE); // no task context in framework processing
}

RCINIT_GROUP rcinit_lookup_group_entry(void* entry)
{
   RCINIT_GROUP rc = RCINIT_GROUP_0;
   const rcinit_info_p** rcinit_sequence_group;

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_p* rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         if ((RCINIT_NULL != rcinit_p->entry))
         {
            if (rcinit_p->entry == entry) // entry pointer associates with group
            {
               return (rc);
            }
         }

         rcinit_group++; // next; this group
      }

      rc++; // processing next group

      rcinit_sequence_group++; // next; sequence group
   }

   return (RCINIT_GROUP_NONE); // no task context in framework processing
}

////////////////////////////////////////
// INITFN SUPPORT
////////////////////////////////////////

void rcinit_trampoline_initfn(void* entry)
{
   // TECHNOLOGY TEAM PROVIDED CALLBACK; RESTRICTIONS INCLUDE: NOT BLOCKING,
   // NO EXCESSIVE STACK RESOURCE USE, LIMITED EXECUTION TIME (ALL CODE EXECUTED
   // AT THIS POINT WILL CONTRIBUTE TO STARTUP LATENCY), LOGIC RESTRICTIONS
   // ASSOCIATED TO INTERRUPT HANDLERS, ...

   void (*initfunc)(void) = (void(*)(void))entry; // init func prototype

   rcinit_internal.InitFnEntry = entry; // diagnostic instrument

   initfunc();
}

#ifndef RCINIT_EXCLUDE_EXAMPLES

void rcinit_example_initfn(void)
{
   rcinit_internal.count++;
}

#endif

////////////////////////////////////////
// TASK SUPPORT DALTASK
////////////////////////////////////////

#ifndef RCINIT_EXCLUDE_STARTUP_DALTASK

//#if defined(__ARMCC_VERSION)
//#message "RCINIT Framework Contains Support For DALTASK"
//#elif defined(__GNUC__)
//#pragma message "RCINIT Framework Contains Support For DALTASK"
//#endif

RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle && RCINIT_TASK_DALTASK == rcinit_p->handle->type)
   {
      return(rcinit_p->handle->context.tid_dal);
   }

   return(RCINIT_NULL);
}

RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_daltask_info(info));
   }

   return(RCINIT_NULL);
}

void rcinit_internal_start_daltask(rcinit_info_p rcinit_p, void* entry)
{
   DALResult rcdal = DAL_ERROR;
   if (REX_ANY_CPU_AFFINITY_MASK != rcinit_p->cpu_affinity)
   {
      RCINIT_ERR_FATAL("daltask work loop must have any_cpu_affinity", 0, 0, 0);
   }
   if (NULL == (rcinit_p->handle->context.tid_dal = (RCINIT_TASK_DALTASK_CTX*)rcinit_internal_malloc(sizeof(RCINIT_TASK_DALTASK_CTX))))
   {
      RCINIT_ERR_FATAL("daltask context creation", 0, 0, 0);
   }
   memset(rcinit_p->handle->context.tid_dal, 0, sizeof(RCINIT_TASK_DALTASK_CTX));
   if (DAL_SUCCESS != (rcdal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &(rcinit_p->handle->context.tid_dal->hEventStart), NULL)))
   {
      RCINIT_ERR_FATAL("daltask event creation", 0, 0, 0);
   }
   if (DAL_SUCCESS != (rcdal = DALSYS_RegisterWorkLoopEx((char*)rcinit_p->name, rcinit_p->stksz, RCINIT_MAP_PRIO_DAL(rcinit_p->prio), dwMaxNumEvents, &(rcinit_p->handle->context.tid_dal->hWorkLoop), NULL)))
   {
      RCINIT_ERR_FATAL("daltask work loop registration", 0, 0, 0);
   }
   if (DAL_SUCCESS != (rcdal = DALSYS_AddEventToWorkLoop(rcinit_p->handle->context.tid_dal->hWorkLoop, (DALSYSWorkLoopExecute)entry, RCINIT_NULL, rcinit_p->handle->context.tid_dal->hEventStart, NULL)))
   {
      RCINIT_ERR_FATAL("daltask work loop event addition", 0, 0, 0);
   }
   if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(rcinit_p->handle->context.tid_dal->hEventStart, DALSYS_EVENT_CTRL_TRIGGER)))
   {
      RCINIT_ERR_FATAL("daltask work loop start", 0, 0, 0);
   }
   //release resources (not currently done for postmortem)
   //if (DAL_SUCCESS != (rcdal = DALSYS_DeleteEventFromWorkLoop(rcinit_p->handle->context.tid_dal->hWorkLoop, rcinit_p->handle->context.tid_dal->hEventStart)))
   //{
   //RCINIT_ERR_FATAL("daltask work loop event addition", 0, 0, 0);
   //}
   //if (DAL_SUCCESS != (rcdal = DALSYS_DestroyObject(rcinit_p->handle->context.tid_dal->hEventStart)))
   //{
   //RCINIT_ERR_FATAL("daltask event creation", 0, 0, 0);
   //}
   //rcinit_p->handle->context.tid_dal->hEventStart = RCINIT_NULL;
   rcdal = rcdal; // eliminate compiler warning
}

#endif

////////////////////////////////////////
// TASK SUPPORT PTHREAD
////////////////////////////////////////

#ifndef RCINIT_EXCLUDE_STARTUP_PTHREAD

//#if defined(__ARMCC_VERSION)
//#message "RCINIT Framework Contains Support For PTHREAD"
//#elif defined(__GNUC__)
//#pragma message "RCINIT Framework Contains Support For PTHREAD"
//#endif

pthread_t rcinit_lookup_pthread_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle && RCINIT_TASK_PTHREAD == rcinit_p->handle->type)
   {
      return(rcinit_p->handle->context.tid_pthread);
   }

   return(0);
}

pthread_t rcinit_lookup_pthread(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_pthread_info(info));
   }

   return(0);
}

void rcinit_internal_start_pthread(rcinit_info_p rcinit_p, void* entry)
{
   pthread_attr_t attr;
   struct sched_param sched;
   int policy;
   cpu_set_t cpuset;
   memset(rcinit_p->stack, RCINIT_STACK_INIT, rcinit_p->stksz);
   pthread_attr_init(&attr);
   pthread_attr_setthreadname(&attr, rcinit_p->name);
   pthread_attr_setstacksize(&attr, rcinit_p->stksz);
   pthread_attr_setstackaddr(&attr, rcinit_p->stack);
   cpuset = rcinit_p->cpu_affinity;
   pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
   pthread_create(&(rcinit_p->handle->context.tid_pthread), &attr, (void*(*)(void*))entry, RCINIT_NULL);
   pthread_attr_destroy(&attr);
   pthread_getschedparam(rcinit_p->handle->context.tid_pthread, &policy, &sched);
   sched.sched_priority = RCINIT_MAP_PRIO_PTHREAD(rcinit_p->prio);
   pthread_setschedparam(rcinit_p->handle->context.tid_pthread, policy, &sched);
}

/*===========================================================================

 FUNCTION rcinit_example_pthread

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#ifndef RCINIT_EXCLUDE_EXAMPLES

/* static */ struct // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   pthread_cond_t cond;
   pthread_mutex_t mutex;

} rcinit_example_pthread_events;

void rcinit_example_pthread_client_api_signal(void)
{
   if (TRUE != rcinit_example_pthread_events.init)
   {
      ERR_FATAL("rcinit_example_pthread_client_api_signal_cond event trigger", 0, 0, 0);
   }
   else
   {
      pthread_mutex_lock(&rcinit_example_pthread_event.mutex);
      pthread_cond_signal(&rcinit_example_pthread_event.cond);
      pthread_mutex_unlock(&rcinit_example_pthread_event.mutex);
   }
}

void rcinit_example_pthread(void* arg_p)
{
   {
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutex_init(&rcinit_example_pthread_event.mutex, &attr);
      pthread_mutexattr_destroy(&attr);
   }

   {
      pthread_condattr_t attr;
      pthread_condattr_init(&attr);
      pthread_cond_init(&rcinit_example_pthread_event.cond, &attr);
      pthread_condattr_destroy(&attr);
   }

   rcinit_example_pthread_events.init = TRUE;

   rcinit_handshake_startup(); // *REQUIRED*

   for (;;)
   {
      pthread_mutex_lock(&rcinit_example_pthread_event.mutex);
      pthread_cond_wait(&rcinit_example_pthread_event.cond, &rcinit_example_pthread_event.mutex);
      pthread_mutex_unlock(&rcinit_example_pthread_event.mutex);
   }

   arg_p = arg_p; // unused
}

#endif

#endif

////////////////////////////////////////
// TASK SUPPORT QURTTASK
////////////////////////////////////////

#ifndef RCINIT_EXCLUDE_STARTUP_QURTTASK

//#if defined(__ARMCC_VERSION)
//#message "RCINIT Framework Contains Support For QURTTASK"
//#elif defined(__GNUC__)
//#pragma message "RCINIT Framework Contains Support For QURTTASK"
//#endif

qurt_thread_t rcinit_lookup_qurttask_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle && RCINIT_TASK_QURTTASK == rcinit_p->handle->type)
   {
      return(rcinit_p->handle->context.tid_qurt);
   }

   return(RCINIT_ZERO);
}

qurt_thread_t rcinit_lookup_qurttask(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_qurttask_info(info));
   }

   return(RCINIT_ZERO);
}

void rcinit_internal_start_qurttask(rcinit_info_p rcinit_p, void* entry)
{
   qurt_thread_attr_t qurt_attr;
   memset(rcinit_p->stack, RCINIT_STACK_INIT, rcinit_p->stksz);
   qurt_thread_attr_init(&qurt_attr);
   qurt_thread_attr_set_name(&qurt_attr, (char*)rcinit_p->name);
   qurt_thread_attr_set_stack_size(&qurt_attr, rcinit_p->stksz);
   qurt_thread_attr_set_stack_addr(&qurt_attr, rcinit_p->stack);
   qurt_thread_attr_set_priority(&qurt_attr, RCINIT_MAP_PRIO_QURT(rcinit_p->prio)); // qurt priority is 0 = high, invert to match 0 = low
   qurt_thread_attr_set_affinity(&qurt_attr, rcinit_p->cpu_affinity);
   qurt_thread_create(&(rcinit_p->handle->context.tid_qurt), &qurt_attr, (void(*)(void*))entry, RCINIT_NULL);
}

/*===========================================================================

 FUNCTION rcinit_example_qurttask

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#ifndef RCINIT_EXCLUDE_EXAMPLES

#define RCINIT_EXAMPLE_QURTTASK_MASK0  0x00000001
#define RCINIT_EXAMPLE_QURTTASK_MASK1  0x00000002
#define RCINIT_EXAMPLE_QURTTASK_MASKS  (RCINIT_EXAMPLE_QURTTASK_MASK0|RCINIT_EXAMPLE_QURTTASK_MASK1)

/* static */ struct // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   qurt_anysignal_t Events;

} rcinit_example_qurttask_events;

void rcinit_example_qurttask_client_api_signal_event0(void)
{
   if (TRUE != rcinit_example_qurttask_events.init)
   {
      ERR_FATAL("rcinit_example_qurttask_client_api_signal_event0 event trigger", 0, 0, 0);
   }
   else
   {
      qurt_anysignal_set(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK0);
   }
}

void rcinit_example_qurttask_client_api_signal_event1(void)
{
   if (TRUE != rcinit_example_qurttask_events.init)
   {
      ERR_FATAL("rcinit_example_qurttask_client_api_signal_event1 event trigger", 0, 0, 0);
   }
   else
   {
      qurt_anysignal_set(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK0);
   }
}

void rcinit_example_qurttask(void* arg_p)
{
   qurt_anysignal_init(&rcinit_example_qurttask_events.Events);

   rcinit_example_qurttask_events.init = TRUE;

   rcinit_handshake_startup(); // *REQUIRED*

   for (;;)
   {
      unsigned long mask = qurt_anysignal_wait(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASKS);

      if (RCINIT_EXAMPLE_QURTTASK_MASK0 & mask)
      {
         unsigned long rc = qurt_anysignal_clear(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK0);
         mask &= ~RCINIT_EXAMPLE_QURTTASK_MASK0;
         rc = rc; // unused
      }

      if (RCINIT_EXAMPLE_QURTTASK_MASK1 & mask)
      {
         unsigned long rc = qurt_anysignal_clear(&rcinit_example_qurttask_events.Events, RCINIT_EXAMPLE_QURTTASK_MASK1);
         mask &= ~RCINIT_EXAMPLE_QURTTASK_MASK1;
         rc = rc; // unused
      }

      if (0 != mask)
      {
         ERR_FATAL("rcinit_example_qurttask unknown event mask", 0, 0, 0);
      }
   }

   rcinit_example_qurttask_events.init = FALSE;

   qurt_anysignal_destroy(&rcinit_example_qurttask_events.Events);

   arg_p = arg_p; // unused
}

#endif

#endif

////////////////////////////////////////
// TASK SUPPORT REXTASK
////////////////////////////////////////

#ifndef RCINIT_EXCLUDE_STARTUP_REXTASK

//#if defined(__ARMCC_VERSION)
//#message "RCINIT Framework Contains Support For REXTASK"
//#elif defined(__GNUC__)
//#pragma message "RCINIT Framework Contains Support For REXTASK"
//#endif

rex_tcb_type* rcinit_lookup_rextask_info(RCINIT_INFO info)
{
   rcinit_info_p rcinit_p = (rcinit_info_p)info;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle &&
       ((RCINIT_TASK_REXTASK == rcinit_p->handle->type) || (RCINIT_TASK_LEGACY == rcinit_p->handle->type)))
   {
      return(rcinit_p->handle->context.tid_rex);
   }

   return(RCINIT_NULL);
}

rex_tcb_type* rcinit_lookup_rextask(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return(rcinit_lookup_rextask_info(info));
   }

   return(RCINIT_NULL);
}

RCINIT_GROUP rcinit_lookup_group_rextask(rex_tcb_type* tid)
{
   RCINIT_GROUP rc = RCINIT_GROUP_0;
   const rcinit_info_p** rcinit_sequence_group;

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_p* rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         if ((RCINIT_NULL != rcinit_p->handle) &&                 // must have a context handle
             ((RCINIT_TASK_REXTASK == rcinit_p->handle->type) || (RCINIT_TASK_LEGACY == rcinit_p->handle->type)) &&
             (rcinit_p->handle->context.tid_rex == tid))        // tcb pointer associates with group
         {
            return (rc);
         }

         rcinit_group++; // next; this group
      }

      rc++; // processing next group

      rcinit_sequence_group++; // next; sequence group
   }

   return (RCINIT_GROUP_NONE); // no task context in framework processing
}

RCINIT_INFO rcinit_lookup_info_rextask(rex_tcb_type* tid)
{
   RCINIT_INFO rc = RCINIT_NULL;
   const rcinit_info_p** rcinit_sequence_group;

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_p* rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         if ((RCINIT_NULL != rcinit_p->handle) &&                 // must have a context handle
             ((RCINIT_TASK_REXTASK == rcinit_p->handle->type) || (RCINIT_TASK_LEGACY == rcinit_p->handle->type)) &&
             (rcinit_p->handle->context.tid_rex == tid))        // tcb pointer associates with group
         {
            return (rcinit_p);
         }

         rcinit_group++; // next; this group
      }

      rcinit_sequence_group++; // next; sequence group
   }

   return (rc); // no task context in framework processing
}

void rcinit_internal_start_rextask(rcinit_info_p rcinit_p, void* entry)
{
   if (RCINIT_NULL == rcinit_p->handle->context.tid_rex)
   {
      if (NULL == (rcinit_p->handle->context.tid_rex = (rex_tcb_type*)rcinit_internal_malloc(sizeof(rex_tcb_type))))
      {
         RCINIT_ERR_FATAL("rextask context creation", 0, 0, 0);
      }
   }
   memset(rcinit_p->handle->context.tid_rex, 0, sizeof(rex_tcb_type));
   memset(rcinit_p->stack, 0, rcinit_p->stksz); // rex trace32 extensions requires stack to be zero

   RCINIT_MAP_REX_DEF_TASK(rcinit_p->cpu_affinity, rcinit_p->handle->context.tid_rex,
                           (unsigned char*)rcinit_p->stack, rcinit_p->stksz,
                           RCINIT_MAP_PRIO_REX(rcinit_p->prio), entry, RCINIT_ZERO, (char*)rcinit_p->name,
                           FALSE, -1);
}

/*===========================================================================

 FUNCTION rcinit_example_rextask

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#ifndef RCINIT_EXCLUDE_EXAMPLES

#define RCINIT_EXAMPLE_REXTASK_MASK0   0x00000001
#define RCINIT_EXAMPLE_REXTASK_MASK1   0x00000002
#define RCINIT_EXAMPLE_REXTASK_MASKS   (RCINIT_EXAMPLE_REXTASK_MASK0|RCINIT_EXAMPLE_REXTASK_MASK1)

/* static */ struct // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   rex_tcb_type* myself;

} rcinit_example_rextask_events;

void rcinit_example_rextask_client_api_signal_event0(void)
{
   if (TRUE != rcinit_example_rextask_events.init)
   {
      ERR_FATAL("rcinit_example_rextask_client_api_signal_event0 event trigger", 0, 0, 0);
   }
   else
   {
      rex_set_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK0);
   }
}

void rcinit_example_rextask_client_api_signal_event1(void)
{
   if (TRUE != rcinit_example_rextask_events.init)
   {
      ERR_FATAL("rcinit_example_rextask_client_api_signal_event1 event trigger", 0, 0, 0);
   }
   else
   {
      rex_set_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK1);
   }
}

void rcinit_example_rextask(unsigned long arg_p)
{
   rcinit_example_rextask_events.myself = rex_self();

   rcinit_example_rextask_events.init = TRUE;

   rcinit_handshake_startup(); // *REQUIRED*

   for (;;)
   {
      unsigned long mask = rex_wait(RCINIT_EXAMPLE_REXTASK_MASKS);

      if (RCINIT_EXAMPLE_REXTASK_MASK0 & mask)
      {
         rex_clr_sigs(rex_self(), RCINIT_EXAMPLE_REXTASK_MASK0);
         mask &= ~RCINIT_EXAMPLE_REXTASK_MASK0;
      }

      if (RCINIT_EXAMPLE_REXTASK_MASK1 & mask)
      {
         rex_clr_sigs(rex_self(), RCINIT_EXAMPLE_REXTASK_MASK1);
         mask &= ~RCINIT_EXAMPLE_REXTASK_MASK1;
      }

      if (0 != mask)
      {
         ERR_FATAL("rcinit_example_rextask unknown event mask", 0, 0, 0);
      }
   }

   rcinit_example_rextask_events.init = FALSE;

   rcinit_example_rextask_events.myself = NULL;

   arg_p = arg_p; // unused
}

#endif

#endif

/*===========================================================================

 FUNCTION worker

 DESCRIPTION
 internal shell functions

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 shuts down by registration to system monitor
 ===========================================================================*/
DALResult rcinit_dal_loop_worker(DALSYSEventHandle hEvent, void* tid)
{
   DALResult rcdal = DAL_ERROR;
   const rcinit_info_p* rcinit_group = rcinit_internal.worker_argv;

   if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET)))
   {
      RCINIT_ERR_FATAL("worker start", 0, 0, 0);
   }

   while (RCINIT_NULL != *rcinit_group)
   {
      rcinit_info_p rcinit_p = *rcinit_group;

      if (IS_INITFN(rcinit_p))
      {
         // STALLING HERE? BLOCKING HERE OCCURS ONLY WHEN THE INITFN IS USING
         // KERNEL BLOCKING MECHANISMS. CHECK WITH THE TECHNOLOGY TEAM.

         rcinit_trampoline_initfn(rcinit_p->entry);
      }

      rcinit_group++;
   }

   if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(rcinit_internal.hEventWorkLoopAck, DALSYS_EVENT_CTRL_TRIGGER)))
   {
      RCINIT_ERR_FATAL("worker ack", 0, 0, 0);
   }

   rcdal = rcdal; // eliminate compiler warning

   return(DAL_SUCCESS);
}

void rcinit_dal_loop_worker_create(void)
{
   DALResult rcdal = DAL_ERROR;
   if (DAL_SUCCESS != (rcdal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &rcinit_internal.hEventWorkLoop, NULL)))
   {
      RCINIT_ERR_FATAL("worker event creation", 0, 0, 0);
   }
   if (DAL_SUCCESS != (rcdal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &rcinit_internal.hEventWorkLoopAck, NULL)))
   {
      RCINIT_ERR_FATAL("worker create event initfn", 0, 0, 0);
   }
   if (DAL_SUCCESS != (rcdal = DALSYS_RegisterWorkLoopEx("rcinit_worker", RCINIT_TASK_DALTASK_STKSZ_MAX, RCINIT_MAP_PRIO_DAL(SHARED_DRIVER_SERVICE_PRI_ORDER), dwMaxNumEvents, &rcinit_internal.hWorkLoop, NULL)))
   {
      RCINIT_ERR_FATAL("worker work loop registration", 0, 0, 0);
   }
   if (DAL_SUCCESS != (rcdal = DALSYS_AddEventToWorkLoop(rcinit_internal.hWorkLoop, rcinit_dal_loop_worker, RCINIT_NULL, rcinit_internal.hEventWorkLoop, NULL)))
   {
      RCINIT_ERR_FATAL("worker work loop event addition", 0, 0, 0);
   }
   //release resources (not currently done for postmortem)
   //if (DAL_SUCCESS != (rcdal = DALSYS_DestroyObject(rcinit_internal.hEventWorkLoopAck)))
   //{
   //RCINIT_ERR_FATAL("worker event creation", 0, 0, 0);
   //}
   //if (DAL_SUCCESS != (rcdal = DALSYS_DeleteEventFromWorkLoop(rcinit_internal.hWorkLoop, rcinit_internal.hEventWorkLoop)))
   //{
   //RCINIT_ERR_FATAL("worker work loop event addition", 0, 0, 0);
   //}
   //if (DAL_SUCCESS != (rcdal = DALSYS_DestroyObject(rcinit_internal.hEventWorkLoop)))
   //{
   //RCINIT_ERR_FATAL("worker event creation", 0, 0, 0);
   //}
   rcdal = rcdal; // eliminate compiler warning
}

/*===========================================================================

 FUNCTION rcinit_handshake_init

 DESCRIPTION
 *required* task handshake to rcinit; all tasks managed by the framework *must*
 *call this api or they will block startup. this is by design.

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/
void rcinit_handshake_init(void) // preferred API
{
   DALResult rcdal = DAL_ERROR;

   // edge case group 0, initfn starting a task, must call rcinit_handshake

   if ((RCINIT_GROUP_0 == rcinit_internal.current_group) &&                      // processing group 0 (only)
       (TRUE == rcinit_internal.process_initfn))                               // processing initfns (only)
   {
      if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(rcinit_internal.hEventInitFnSpawn, DALSYS_EVENT_CTRL_TRIGGER)))
      {
         RCINIT_ERR_FATAL("initfnspawn event", 0, 0, 0);
      }
   }

   // all other use case collect and wait for the defineack (concurrent)

   else
   {
      RCEVT_THRESHOLD count = rcevt_getcount_handle(rcinit_internal.defineack) + 1;

      rcevt_signal_handle(rcinit_internal.define); // signals rcinit_task this task init complete

      rcevt_wait_count_handle(rcinit_internal.defineack, count);
   }

   rcdal = rcdal; // eliminate compiler warning
}

void rcinit_handshake_startup(void)
{
   rcinit_handshake_init(); // use preferred API
}

/*===========================================================================

 FUNCTION rcinit_register_term_group

 DESCRIPTION
 register for termination notification and handshake back expectations

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none
 ===========================================================================*/
void rcinit_register_term_group(RCINIT_GROUP group, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   switch (group)
   {
      case RCINIT_GROUP_0:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_0, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_1:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_1, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_2:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_2, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_3:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_3, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_4:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_4, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_5:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_5, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_6:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_6, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_7:
         rcevt_register_sigex_name(RCINIT_RCEVT_TERM_GROUP_7, type, sigex);
         RCINIT_ERR("client registers term signal group %d", group, 0, 0);
         break;

      default:
         RCINIT_ERR_FATAL("client does not meet requirements", 0, 0, 0);
         break;
   }
}

void rcinit_unregister_term_group(RCINIT_GROUP group, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   switch (group)
   {
      case RCINIT_GROUP_0:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_0, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_1:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_1, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_2:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_2, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_3:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_3, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_4:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_4, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_5:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_5, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_6:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_6, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_7:
         rcevt_unregister_sigex_name(RCINIT_RCEVT_TERM_GROUP_7, type, sigex);
         RCINIT_ERR("client unregisters term signal group %d", group, 0, 0);
         break;

      default:
         RCINIT_ERR_FATAL("client does not meet requirements", 0, 0, 0);
         break;
   }
}

/*===========================================================================

 FUNCTION rcinit_register_termfn_group

 DESCRIPTION
 register for termination notification and no handshake back expectations

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none
 ===========================================================================*/
void rcinit_register_termfn_group(RCINIT_GROUP group, void(*fn)(void))
{
   switch (group)
   {
      case RCINIT_GROUP_0:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_0, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_1:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_1, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_2:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_2, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_3:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_3, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_4:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_4, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_5:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_5, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_6:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_6, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      case RCINIT_GROUP_7:
         rcecb_register_context_name(RCINIT_RCECB_TERM_GROUP_7, fn);
         RCINIT_ERR("client registers termfn callback group %d", group, 0, 0);
         break;

      default:
         RCINIT_ERR_FATAL("client does not meet requirements", 0, 0, 0);
         break;
   }
}

// client call without group (GROUP_7)

void rcinit_register_term(RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rcinit_register_term_group(RCINIT_GROUP_7, type, sigex);
}

void rcinit_unregister_term(RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rcinit_unregister_term_group(RCINIT_GROUP_7, type, sigex);
}

// client call without group (GROUP_7)

void rcinit_register_termfn(void(*fn)(void))
{
   rcinit_register_termfn_group(RCINIT_GROUP_7, fn);
}

/*===========================================================================

 FUNCTION rcinit_handshake_term

 DESCRIPTION
 client called api to handshake during termination

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none
 ===========================================================================*/
void rcinit_handshake_term(void)
{
   rcevt_signal_name(RCINIT_RCEVT_TERMACK);
}

void rcinit_handshake_shutdown(void)
{
   rcinit_handshake_term();
}

/*===========================================================================

 FUNCTION rcinit_initfn_spawn_task

 DESCRIPTION
 required wrapper for an init function to spawn a task (very specialized edge
 case found during sequence group 0, this should be the exception)

 DEPENDENCIES
 Requires existing RCINIT task database information where the task entry
 point is NULL. The static data is preallocated, and this call binds the
 entry point to the context and starts it. Edge case; only allowed from
 Group 0 initfn processing. all other use cases are not supported and
 are considered a bug.

 RETURN VALUE
 none

 SIDE EFFECTS
 task must call rcinit_handshake_startup(); else startup process will block
 as designed

 ===========================================================================*/
void rcinit_initfn_spawn_task(RCINIT_INFO info_p, void *entry)
{
   boolean started = FALSE;
   rcinit_info_p rcinit_p = (rcinit_info_p)info_p;

   if ((RCINIT_GROUP_0 == rcinit_lookup_group(rcinit_p->name)) &&                // must be in group 0 (only)
       (RCINIT_GROUP_0 == rcinit_internal.current_group) &&                      // processing group 0 (only)
       (TRUE == rcinit_internal.process_initfn) &&                               // processing initfns (only)
       (RCINIT_NULL == rcinit_p->entry) &&                                       // must not have an entry (specialized)
       (RCINIT_NULL != entry))                                                   // must have an entry point argument
   {
      DALResult rcdal = DAL_ERROR;

      // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
      if (DAL_SUCCESS != (rcdal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &rcinit_internal.hEventInitFnSpawn, NULL)))
      {
         RCINIT_ERR_FATAL("initfnspawn event", 0, 0, 0);
      }

      switch (rcinit_p->handle->type)
      {
         case RCINIT_TASK_INITFN:
         case RCINIT_TASK_TERMFN:
         case RCINIT_TASK_NONE:
#ifdef RCINIT_EXCLUDE_STARTUP_DALTASK
         case RCINIT_TASK_DALTASK:                                               // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_PTHREAD
         case RCINIT_TASK_PTHREAD:                                               // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_QURTTASK
         case RCINIT_TASK_QURTTASK:                                              // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_REXTASK
         case RCINIT_TASK_REXTASK:                                               // eliminate compiler warning
         case RCINIT_TASK_LEGACY:                                                // eliminate compiler warning
#endif
            started = FALSE;
            break;

#ifndef RCINIT_EXCLUDE_STARTUP_DALTASK
         case RCINIT_TASK_DALTASK:
            rcinit_internal_start_daltask(rcinit_p, entry);
            started = TRUE;
            break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_PTHREAD
         case RCINIT_TASK_PTHREAD:
            rcinit_internal_start_pthread(rcinit_p, entry);
            started = TRUE;
            break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_QURTTASK
         case RCINIT_TASK_QURTTASK:
            rcinit_internal_start_qurttask(rcinit_p, entry);
            started = TRUE;
            break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_REXTASK
         case RCINIT_TASK_REXTASK:
         case RCINIT_TASK_LEGACY:
            rcinit_internal_start_rextask(rcinit_p, entry);
            started = TRUE;
            break;
#endif
      }

      // STALLING HERE? BLOCKING HERE OCCURS UNTIL THE SINGLE TASK STARTED
      // PERFORMS ITS HANDSHAKE. THIS IS THE NORMAL MECHANISM. EXTENDED
      // BLOCKING HERE WILL BE FROM A TASK NOT CALLING THE REQUIRED HANDSHAKE.

      if (TRUE == started)
      {
         // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
         if (DAL_SUCCESS != (rcdal = DALSYS_EventWait(rcinit_internal.hEventInitFnSpawn)))
         {
            RCINIT_ERR_FATAL("initfnspawn event", 0, 0, 0);
         }
      }
      // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
      if (DAL_SUCCESS != (rcdal = DALSYS_DestroyObject(rcinit_internal.hEventInitFnSpawn)))
      {
         RCINIT_ERR_FATAL("initfnspawn event", 0, 0, 0);
      }

      rcdal = rcdal; // eliminate compiler warning
   }
   else
   {
      RCINIT_ERR_FATAL("client does not meet requirements", 0, 0, 0);
   }
}

/*===========================================================================

 FUNCTION rcinit_internal_process_stacks

 DESCRIPTION
 stack pool allocation
 stack pool portioning

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_internal_process_stacks(void)
{
   const rcinit_info_p** rcinit_sequence_group;

   ////////////////////////////////////////
   // Loop and process the sequence groups array for stksz information
   // allocate as one big block pool to prevent fragmentation that occurs
   // when allocated as individual blocks; then populate the internal
   // database structures with each individual stack address
   ////////////////////////////////////////

   ////////////////////////////////////////
   // Determine size for the stack pool
   ////////////////////////////////////////

   rcinit_internal.stacks_size = 0;

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_p* rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         if ((RCINIT_NULL != rcinit_p->handle) &&                                // must have a context handle
             (RCINIT_STKSZ_ZERO != rcinit_p->stksz) &&                           // must have a stack size
             (&rcinit_info_rcinit != rcinit_p))                                  // must not be rcinit, rcinit is the bootstrap
         {
            switch (rcinit_p->handle->type)
            {
               case RCINIT_TASK_INITFN:
               case RCINIT_TASK_TERMFN:
               case RCINIT_TASK_NONE:
#ifdef RCINIT_EXCLUDE_STARTUP_DALTASK
               case RCINIT_TASK_DALTASK:                                         // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_PTHREAD
               case RCINIT_TASK_PTHREAD:                                         // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_QURTTASK
               case RCINIT_TASK_QURTTASK:                                        // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_REXTASK
               case RCINIT_TASK_REXTASK:                                         // eliminate compiler warning
               case RCINIT_TASK_LEGACY:                                          // eliminate compiler warning
#endif
                  // NULL
                  break;

#ifndef RCINIT_EXCLUDE_STARTUP_DALTASK
               case RCINIT_TASK_DALTASK:
                  if (RCINIT_NULL == rcinit_p->handle->context.tid_dal)
                  {
                     rcinit_internal.stacks_size += (sizeof(RCINIT_TASK_DALTASK_CTX)/sizeof(rcinit_stack_t));
                  }
                  rcinit_internal.stacks_size += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_PTHREAD
               case RCINIT_TASK_PTHREAD:
                  rcinit_internal.stacks_size += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_QURTTASK
               case RCINIT_TASK_QURTTASK:
                  rcinit_internal.stacks_size += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_REXTASK
               case RCINIT_TASK_REXTASK:
               case RCINIT_TASK_LEGACY:
                  if (RCINIT_NULL == rcinit_p->handle->context.tid_rex)
                  {
                     rcinit_internal.stacks_size += (sizeof(rex_tcb_type)/sizeof(rcinit_stack_t));
                  }
                  rcinit_internal.stacks_size += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif
            }
         }

         rcinit_group++;                                                         // next; this group
      }

      rcinit_sequence_group++;                                                   // next; sequence group
   }

   ////////////////////////////////////////
   // Allocate the stack pool
   ////////////////////////////////////////

   rcinit_internal.stacks = (rcinit_stack_t*)rcinit_internal_malloc(rcinit_internal.stacks_size * sizeof(rcinit_stack_t));

   ////////////////////////////////////////
   // Allocate portions from the stack pool
   ////////////////////////////////////////

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_p* rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         if ((RCINIT_NULL != rcinit_p->handle) &&                                // must have a context handle
             (RCINIT_STKSZ_ZERO != rcinit_p->stksz) &&                           // must have a stack size
             (&rcinit_info_rcinit != rcinit_p))                                  // must not be rcinit, rcinit is the bootstrap
         {
            switch (rcinit_p->handle->type)
            {
               case RCINIT_TASK_INITFN:
               case RCINIT_TASK_TERMFN:
               case RCINIT_TASK_NONE:
#ifdef RCINIT_EXCLUDE_STARTUP_DALTASK
               case RCINIT_TASK_DALTASK:                                         // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_PTHREAD
               case RCINIT_TASK_PTHREAD:                                         // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_QURTTASK
               case RCINIT_TASK_QURTTASK:                                        // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_REXTASK
               case RCINIT_TASK_REXTASK:                                         // eliminate compiler warning
               case RCINIT_TASK_LEGACY:                                          // eliminate compiler warning
#endif
                  // NULL
                  break;

#ifndef RCINIT_EXCLUDE_STARTUP_DALTASK
               case RCINIT_TASK_DALTASK:
                  if (RCINIT_NULL == rcinit_p->handle->context.tid_dal)
                  {
                     rcinit_p->handle->context.tid_dal = (RCINIT_TASK_DALTASK_CTX*)rcinit_internal.stacks;
                     rcinit_internal.stacks += (sizeof(RCINIT_TASK_DALTASK_CTX)/sizeof(rcinit_stack_t));
                  }
                  rcinit_p->stack = rcinit_internal.stacks;
                  rcinit_internal.stacks += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_PTHREAD
               case RCINIT_TASK_PTHREAD:
                  rcinit_p->stack = rcinit_internal.stacks;
                  rcinit_internal.stacks += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_QURTTASK
               case RCINIT_TASK_QURTTASK:
                  rcinit_p->stack = rcinit_internal.stacks;
                  rcinit_internal.stacks += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_REXTASK
               case RCINIT_TASK_REXTASK:
               case RCINIT_TASK_LEGACY:
                  if (RCINIT_NULL == rcinit_p->handle->context.tid_rex)
                  {
                     rcinit_p->handle->context.tid_rex = (rex_tcb_type*)rcinit_internal.stacks;
                     rcinit_internal.stacks += (sizeof(rex_tcb_type)/sizeof(rcinit_stack_t));
                  }
                  rcinit_p->stack = rcinit_internal.stacks;
                  rcinit_internal.stacks += (rcinit_p->stksz/sizeof(rcinit_stack_t));
                  break;
#endif
            }
         }

         rcinit_group++;                                                         // next; this group
      }

      rcinit_sequence_group++;                                                   // next; sequence group
   }
}

/*===========================================================================

 FUNCTION rcinit_internal_process_groups

 DESCRIPTION
 init function processing
 task define and start signaling

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_internal_process_groups(void)
{
   DALResult rcdal = DAL_ERROR;
   const rcinit_info_p** rcinit_sequence_group;

   ////////////////////////////////////////
   // Process Init Functions and Tasks
   ////////////////////////////////////////

   rcinit_sequence_group = rcinit_internal_sequence_groups;

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_p* rcinit_group;
      int grp_define; // counter of defined tasks; this group
      RCEVT_THRESHOLD current_define;

      ////////////////////////////////////////
      // Process Init Functions and Tasks
      ////////////////////////////////////////

      rcinit_internal.process_initfn = TRUE; // instrument

      rcinit_group = *rcinit_sequence_group; // process this group

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         ////////////////////////////////////////
         // Init Function Check
         ////////////////////////////////////////

         if (IS_INITFN(rcinit_p))
         {
            ////////////////////////////////////////
            // Init Function Process
            ////////////////////////////////////////

            rcinit_internal.worker_argv = rcinit_group;

            if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(rcinit_internal.hEventWorkLoop, DALSYS_EVENT_CTRL_TRIGGER)))
            {
               RCINIT_ERR_FATAL("workloop trigger", 0, 0, 0);
            }

            if (DAL_SUCCESS != (rcdal = DALSYS_EventWait(rcinit_internal.hEventWorkLoopAck)))
            {
               RCINIT_ERR_FATAL("workloop response", 0, 0, 0);
            }

            if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(rcinit_internal.hEventWorkLoopAck, DALSYS_EVENT_CTRL_RESET)))
            {
               RCINIT_ERR_FATAL("workloop response reset", 0, 0, 0);
            }

            break;
         }

         rcinit_group++; // next; this group
      }

      rcinit_internal.process_initfn = FALSE;

      ////////////////////////////////////////
      // Process Tasks
      ////////////////////////////////////////

      rcinit_internal.process_task = TRUE;

      grp_define = 0; // counter of defined tasks; this group
      current_define = rcevt_getcount_handle(rcinit_internal.define);

      rcinit_group = *rcinit_sequence_group; // process this group

      while (RCINIT_NULL != *rcinit_group)
      {
         rcinit_info_p rcinit_p = *rcinit_group;

         ////////////////////////////////////////
         // Task Check
         ////////////////////////////////////////

         if (IS_TASK(rcinit_p))
         {
            ////////////////////////////////////////
            // Task Process
            ////////////////////////////////////////

            switch (rcinit_p->handle->type)
            {
               case RCINIT_TASK_INITFN:
               case RCINIT_TASK_TERMFN:
               case RCINIT_TASK_NONE:
#ifdef RCINIT_EXCLUDE_STARTUP_DALTASK
               case RCINIT_TASK_DALTASK:                                         // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_PTHREAD
               case RCINIT_TASK_PTHREAD:                                         // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_QURTTASK
               case RCINIT_TASK_QURTTASK:                                        // eliminate compiler warning
#endif
#ifdef RCINIT_EXCLUDE_STARTUP_REXTASK
               case RCINIT_TASK_REXTASK:                                         // eliminate compiler warning
               case RCINIT_TASK_LEGACY:                                          // eliminate compiler warning
#endif
                  // NULL
                  break;

#ifndef RCINIT_EXCLUDE_STARTUP_DALTASK
               case RCINIT_TASK_DALTASK:
                  rcinit_internal_start_daltask(rcinit_p, rcinit_p->entry);
                  grp_define++;                                                  // counter of defined tasks; this group
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_PTHREAD
               case RCINIT_TASK_PTHREAD:
                  rcinit_internal_start_pthread(rcinit_p, rcinit_p->entry);
                  grp_define++;                                                  // counter of defined tasks; this group
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_QURTTASK
               case RCINIT_TASK_QURTTASK:
                  rcinit_internal_start_qurttask(rcinit_p, rcinit_p->entry);
                  grp_define++;                                                  // counter of defined tasks; this group
                  break;
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_REXTASK
               case RCINIT_TASK_REXTASK:
               case RCINIT_TASK_LEGACY:
                  rcinit_internal_start_rextask(rcinit_p, rcinit_p->entry);
                  grp_define++;                                                  // counter of defined tasks; this group
                  break;
#endif
            }
         }

         rcinit_group++;                                                         // next; this group
      }

      if (0 != grp_define)                                                       // only wait when tasks were started
      {
         // async order; wait for all to complete that were started

         // STALLING HERE? BLOCKING HERE OCCURS UNTIL ALL TASKS STARTED IN
         // THE GROUP HAVE HANDSHAKE. THIS IS THE NORMAL MECHANISM. EXTENDED
         // BLOCKING HERE WILL BE FROM A TASK NOT CALLING THE REQUIRED HANDSHAKE.

         rcevt_wait_count_handle(rcinit_internal.define, current_define + grp_define); // wait for all defined tasks handshake; this group
      }
      else
      {
         // NULL ; // No blocking required, no tasks started
      }

      rcevt_signal_handle(rcinit_internal.defineack);                            // issue start signal to all defined tasks; this group

      rcinit_internal.process_task = FALSE;

      // extension: ssr during init, polling check would be inserted here

      rcinit_sequence_group++;                                                   // next; sequence group

      rcinit_internal.current_group++;
   }

   if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(hEventCoreStarted, DALSYS_EVENT_CTRL_TRIGGER)))   // signal event core started (private handshake to core_start routine)
   {
      RCINIT_ERR_FATAL("fail corestarted trigger", 0, 0, 0);
   }

   rcdal = rcdal; // eliminate compiler warning
}

/*===========================================================================

 FUNCTION rcinit_internal_unprocess_groups

 DESCRIPTION
 task and termfn processing

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_internal_unprocess_groups(void)
{
   int count;                                                                    // registered count
   RCEVT_THRESHOLD ackcnt;                                                       // current ack count

   ////////////////////////////////////////
   // RCINIT_GROUP_7
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_7;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_7);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_7);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_7);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn

   ////////////////////////////////////////
   // RCINIT_GROUP_6
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_6;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_6);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_6);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_6);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn

   ////////////////////////////////////////
   // RCINIT_GROUP_5
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_5;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_5);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_5);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_5);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn

   ////////////////////////////////////////
   // RCINIT_GROUP_4
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_4;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_4);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_4);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_4);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn

   ////////////////////////////////////////
   // RCINIT_GROUP_3
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_3;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_3);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_3);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_3);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn

   ////////////////////////////////////////
   // RCINIT_GROUP_2
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_2;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_2);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_3);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_3);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn

   ////////////////////////////////////////
   // RCINIT_GROUP_1
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_1;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_1);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_1);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_1);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn

   ////////////////////////////////////////
   // RCINIT_GROUP_0
   ////////////////////////////////////////

   rcinit_internal.current_group = RCINIT_GROUP_0;                               // processing group
   rcinit_internal.process_task = TRUE;                                          // processing tasks
   count = rcevt_getcontexts_name(RCINIT_RCEVT_TERM_GROUP_0);                    // get registered count
   ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                           // get current ack count
   rcevt_signal_name(RCINIT_RCEVT_TERM_GROUP_0);                                 // signal registered
   if (0 != count) { rcevt_wait_count_name(RCINIT_RCEVT_TERMACK, ackcnt + count); } // wait for acks from registered
   rcinit_internal.process_task = FALSE;                                         // processing tasks
   rcinit_internal.process_initfn = TRUE;                                        // processing termfn
   rcecb_signal_name(RCINIT_RCECB_TERM_GROUP_0);                                 // signal registered
   rcinit_internal.process_initfn = FALSE;                                       // processing termfn
}

/*===========================================================================

 FUNCTION rcinit_init

 DESCRIPTION
 prepare internal data storage setup

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_init(void)
{
   DALResult rcdal = DAL_ERROR;

   rcevt_init();                                                                 // rcevt events service

   rcinit_dal_loop_worker_create();                                              // internal worker thread

   // internal data structures

   rcinit_internal.current_group = RCINIT_GROUP_0;                               // current group
   rcinit_internal.process_initfn = FALSE;                                       // processing initfns
   rcinit_internal.process_task = FALSE;                                         // processing rextasks

   // internal events

   if (DAL_SUCCESS != (rcdal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &rcinit_internal.hEventShutdownTask, NULL)))
   {
      RCINIT_ERR_FATAL("create event shutdown", 0, 0, 0);
   }

   rcinit_internal.define     = rcevt_create_name(RCINIT_RCEVT_DEFINE);          // handle to define event, internal
   rcinit_internal.defineack  = rcevt_create_name(RCINIT_RCEVT_DEFINEACK);       // handle to defineack event, internal

   if ((RCINIT_NULL == rcinit_internal.define) ||                                // must have rcevt allocated
       (RCINIT_NULL == rcinit_internal.defineack))                               // must have rcevt allocated
   {
      RCINIT_ERR_FATAL("rcinit_init unable to initialize", 0, 0, 0);
   }

   rcdal = rcdal; // eliminate compiler warning
}

/*===========================================================================

 FUNCTION rcinit_term

 DESCRIPTION
 prepare internal data storage cleanup

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_term(void)
{
   // DESIGN DECISION TO *NOT* CLEANUP ANY INFORMATION STRUCTURES
}

/*===========================================================================

 FUNCTION rcinit_task

 DESCRIPTION
 hlos task thread to process sequnece groups

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/

DALResult rcinit_task(DALSYSEventHandle hEvent, void* arg_p)
{
   DALResult rcdal = DAL_ERROR;
   RCEVT_SIGEX_SIGDAL sig;

   if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_RESET)))
   {
      RCINIT_ERR_FATAL("task create", 0, 0, 0);
   }

   rcinit_init();                                                                // rcinit init

   rcinit_internal_process_stacks();                                             // stack pools

   rcinit_internal_process_groups();                                             // sequence groups

   sig.signal = rcinit_internal.hEventShutdownTask;

   rcevt_register_sigex_name(SYS_M_SHUTDOWN, RCEVT_SIGEX_TYPE_SIGDAL, &sig);

   rcevt_signal_name(RCINIT_RCEVT_INITGROUPS);                                   // observers

   if (DAL_SUCCESS != (rcdal = DALSYS_EventWait(sig.signal)))
   {
      RCINIT_ERR_FATAL("task wait event shutdown", 0, 0, 0);
   }

   rcevt_unregister_sigex_name(SYS_M_SHUTDOWN, RCEVT_SIGEX_TYPE_SIGDAL, &sig);

   if (DAL_SUCCESS != (rcdal = DALSYS_EventCtrl(sig.signal, DALSYS_EVENT_CTRL_RESET)))
   {
      RCINIT_ERR_FATAL("task wait event shutdown", 0, 0, 0);
   }

   rcevt_signal_name(RCINIT_RCEVT_TERMGROUPS);                                   // observers

   rcinit_internal_unprocess_groups();                                           // sequence groups

   rcinit_term();                                                                // rcinit termination

   rcevt_signal_name(SYS_M_SHUTDOWN_ACK);

   arg_p = arg_p; // eliminate compiler warning

   rcdal = rcdal; // eliminate compiler warning

   return(DAL_SUCCESS);
}

/*===========================================================================
FUNCTION rcinit_bootstrap_rcinit_task

DESCRIPTION
  Create a context for rcinit framework

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define dwMaxNumEvents                          2                                // DALSys Worker Loop Event Queue Depth for DALTASK
#define wlPriority                              190                              // SHARED_DRIVER_SERVICE_PRI_ORDER

void rcinit_bootstrap_rcinit_task(void)
{
   const unsigned long wlStackSize = 6144;   // moderate for rcinit (KB)
   static DALSYSEventHandle hEventStart;     // persistent workloop context
   static DALSYSWorkLoopHandle hWorkLoop;    // persistent workloop context

   if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEventCoreStarted, NULL))
   {
      ERR_FATAL("RCInit task initialization failure",0,0,0);
   }

   if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &hEventStart, NULL))
   {
      ERR_FATAL("RCInit task initialization failure",0,0,0);
   }
   if (DAL_SUCCESS != DALSYS_RegisterWorkLoopEx("rcinit", wlStackSize, wlPriority, dwMaxNumEvents, &hWorkLoop, NULL))
   {
      ERR_FATAL("RCInit task initialization failure",0,0,0);
   }
   if (DAL_SUCCESS != DALSYS_AddEventToWorkLoop(hWorkLoop, rcinit_task, NULL, hEventStart, NULL))
   {
      ERR_FATAL("RCInit task initialization failure",0,0,0);
   }
   if (DAL_SUCCESS != DALSYS_EventCtrl(hEventStart, DALSYS_EVENT_CTRL_TRIGGER))
   {
      ERR_FATAL("RCInit task initialization failure",0,0,0);
   }
   if (DAL_SUCCESS != DALSYS_EventWait(hEventCoreStarted))
   {
      ERR_FATAL("RCInit task initialization failure",0,0,0);
   }
}
