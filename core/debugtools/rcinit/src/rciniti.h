#ifndef _RCINITI_H
#define _RCINITI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 GENERAL DESCRIPTION

 EXTERNALIZED FUNCTIONS

 INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.adsp/2.2/debugtools/rcinit/src/rciniti.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#if defined(__GNUC__)
#include <stdlib.h>
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_DALTASK
#include "rcinit_dal.h"
#endif
#ifndef RCINIT_EXCLUDE_STARTUP_PTHREAD
#include "rcinit_pthread.h"
#endif
#ifndef RCINIT_EXCLUDE_STARTUP_QURTTASK
#include "rcinit_qurt.h"
#endif
#ifndef RCINIT_EXCLUDE_STARTUP_REXTASK
#include "rcinit_rex.h"
#endif

#include "err.h"

#define RCINIT_ERR(str, a, b, c)             ERR("rcinit: " str, a, b, c)
#define RCINIT_ERR_FATAL(str, a, b, c)       ERR_FATAL("rcinit: " str ,a, b, c)

#if defined(__cplusplus)
extern "C" {
#endif

// SCONS CPU AFFINITY MAPPING (LEGACY SUPPORT)

#ifndef REX_SINGLE_CPU_AFFINITY_MASK
#define REX_SINGLE_CPU_AFFINITY_MASK (0)
#endif
#ifndef REX_ANY_CPU_AFFINITY_MASK
#define REX_ANY_CPU_AFFINITY_MASK (~0)
#endif
#ifndef REX_COMMON_SMT_MASK
#define REX_COMMON_SMT_MASK (~0)
#endif
#ifndef REX_ANY_SMT_MASK
#define REX_ANY_SMT_MASK (~0)
#endif

/////////////////////////////////////////////////////////////////////
// Tuneable Controls
/////////////////////////////////////////////////////////////////////

#define RCINIT_STATIC               static
#define rcinit_internal_malloc(x)   malloc(x)                                    // internal allocator implementation wrapper
#define rcinit_internal_free(x)     free(x)                                      // internal allocator implementation wrapper

/////////////////////////////////////////////////////////////////////
// Localized Manifest Constants
/////////////////////////////////////////////////////////////////////

// These are the "recommended" by policy RCINIT task priority aliases to use. The
// legacy design principle of one unique task requires one unique priority
// is no longer valid. Software must be written in a manner that it is
// hardware thread safe and does not utilize priority alone as the exclusive
// method to arbitrate resource contention.

// AVAILABLE FOR USE IN SCONS BUILD ADDRCINITTASK() TASK CALLS, THIS LIST *IS NOT*
// EXPANDED FOR TECH TEAMS TO HAVE THEIR OWN PRIVATE ALIAS.

#define SHARED_IST_PRI_ORDER                          239                        // priority alias from rcinit_task_prio database
#define SHARED_IST_SERVICE_PRI_ORDER                  230                        // priority alias from rcinit_task_prio database
#define SHARED_DRIVER_PRI_ORDER                       210                        // priority alias from rcinit_task_prio database
#define SHARED_DRIVER_SERVICE_PRI_ORDER               190                        // priority alias from rcinit_task_prio database
#define SHARED_FOREGROUND_PRI_ORDER                   150                        // priority alias from rcinit_task_prio database
#define SHARED_FOREGROUND_SERVICE_PRI_ORDER           120                        // priority alias from rcinit_task_prio database
#define SHARED_BACKGROUND_PRI_ORDER                   100                        // priority alias from rcinit_task_prio database
#define SHARED_BACKGROUND_SERVICE_PRI_ORDER           80                         // priority alias from rcinit_task_prio database
#define SHARED_IDLE_PRI_ORDER                         30                         // priority alias from rcinit_task_prio database
#define SHARED_IDLE_SERVICE_PRI_ORDER                 2                          // priority alias from rcinit_task_prio database

#define RCINIT_TASK_DALTASK_STKSZ_MAX                 4096                       // obtain from kernel runtime accessor or manifest constant

#define RCINIT_STKSZ_ZERO                             (0)
#define RCINIT_STACK_NULL                             ((rcinit_stack_p)0)
#define RCINIT_STACK_NONE                             ((rcinit_stack_p)-1)
#define RCINIT_STACK_INIT                             (0xa5)

// MAP Different Kernel Interfaces to RCINIT Priority Concept
// Rcinit priority database is 0 = low, 255 = high

#define RCINIT_MAP_PRIO_DAL(x)                        (x)
#define RCINIT_MAP_PRIO_PTHREAD(x)                    (x)
#define RCINIT_MAP_PRIO_QURT(x)                       ((((int)255-(int)(x))>=0)?((int)255-(int)(x)):255)
#define RCINIT_MAP_PRIO_REX(x)                        (x)

// REX API use on NONHLOS
#if defined(FEATURE_BLAST) || defined(FEATURE_QURT)
#define RCINIT_MAP_REX_DEF_TASK(a, b, c, d, e, f, g, h, i, j) rex_def_task_ext5(a, b, c, d, e, f, g, h, i, j)
#else // defined(FEATURE_L4) || defined(FEATURE_REX)
#define RCINIT_MAP_REX_DEF_TASK(a, b, c, d, e, f, g, h, i, j) rex_def_task_ext2(b, c, d, e, f, g, h, i, j)
#endif

// Logic Used to Determine if RCINIT Database Entry is INITFN
// Internal Macro : IS_INITFN, entry, no handle, no priority, no stack size, no stack pointer, not rcinit

#define IS_INITFN(rcinit_p) \
   ((RCINIT_NULL != rcinit_p->entry) && \
    (RCINIT_PRIO_INITFN == rcinit_p->prio) && \
    (RCINIT_NULL == rcinit_p->handle) && \
    (RCINIT_STKSZ_ZERO == rcinit_p->stksz) && \
    (RCINIT_STACK_NULL == rcinit_p->stack) && \
    (&rcinit_info_rcinit != rcinit_p))

// Logic Used to Determine if RCINIT Database Entry is TASK
// Internal Macro : IS_TASK, entry, handle, priority, stack size, stack pointer, not rcinit

#define IS_TASK(rcinit_p) \
   ((RCINIT_NULL != rcinit_p->entry) && \
    (RCINIT_PRIO_INITFN != rcinit_p->prio) && \
    (RCINIT_NULL != rcinit_p->handle) && \
    (RCINIT_STKSZ_ZERO != rcinit_p->stksz) && \
    (RCINIT_STACK_NULL != rcinit_p->stack) && \
    (&rcinit_info_rcinit != rcinit_p))

// internally managed events (no observers, these are policy based with ack)

#define RCINIT_RCEVT_INTERNAL_PREFIX         "rci:"

#define RCINIT_RCEVT_DEFINE                  RCINIT_RCEVT_INTERNAL_PREFIX "define"
#define RCINIT_RCEVT_DEFINEACK               RCINIT_RCEVT_INTERNAL_PREFIX "defack"
#define RCINIT_RCEVT_INITFN                  RCINIT_RCEVT_INTERNAL_PREFIX "initfn"
#define RCINIT_RCEVT_TERMACK                 RCINIT_RCEVT_INTERNAL_PREFIX "trmack"

#define RCINIT_RCECB_TERM_GROUP_0            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:0"
#define RCINIT_RCECB_TERM_GROUP_1            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:1"
#define RCINIT_RCECB_TERM_GROUP_2            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:2"
#define RCINIT_RCECB_TERM_GROUP_3            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:3"
#define RCINIT_RCECB_TERM_GROUP_4            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:4"
#define RCINIT_RCECB_TERM_GROUP_5            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:5"
#define RCINIT_RCECB_TERM_GROUP_6            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:6"
#define RCINIT_RCECB_TERM_GROUP_7            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:7"

#define RCINIT_RCEVT_TERM_GROUP_0            RCINIT_RCEVT_INTERNAL_PREFIX "term:0"
#define RCINIT_RCEVT_TERM_GROUP_1            RCINIT_RCEVT_INTERNAL_PREFIX "term:1"
#define RCINIT_RCEVT_TERM_GROUP_2            RCINIT_RCEVT_INTERNAL_PREFIX "term:2"
#define RCINIT_RCEVT_TERM_GROUP_3            RCINIT_RCEVT_INTERNAL_PREFIX "term:3"
#define RCINIT_RCEVT_TERM_GROUP_4            RCINIT_RCEVT_INTERNAL_PREFIX "term:4"
#define RCINIT_RCEVT_TERM_GROUP_5            RCINIT_RCEVT_INTERNAL_PREFIX "term:5"
#define RCINIT_RCEVT_TERM_GROUP_6            RCINIT_RCEVT_INTERNAL_PREFIX "term:6"
#define RCINIT_RCEVT_TERM_GROUP_7            RCINIT_RCEVT_INTERNAL_PREFIX "term:7"

// Private Internal Type

typedef const char rcinit_name_t, *rcinit_name_p;                                // NULL terminate string referring to item information

// Private Internal Type Mapping to NONHLOS

typedef void* rcinit_tid_p;
typedef unsigned long rcinit_stack_t, *rcinit_stack_p;                           // native stack
typedef void* rcinit_entry_p;

typedef struct rcinit_handle_s                                                   // Private Implementation Details of Database
{
   RCINIT_TASK                  type;                                            // private

   union
   {
      rcinit_tid_p   tid_p;                                                      // private place holder (void*, must be first)

#ifndef RCINIT_EXCLUDE_STARTUP_DALTASK
      RCINIT_TASK_DALTASK_CTX* tid_dal;                                          // rcinit daltask "workloop"
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_PTHREAD
      pthread_t      tid_pthread;                                                // pthread id
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_QURTTASK
      qurt_thread_t  tid_qurt;                                                   // qurt id
#endif

#ifndef RCINIT_EXCLUDE_STARTUP_REXTASK
      rex_tcb_type*  tid_rex;                                                    // rex id
#endif

   } context;                                                                    // private

} rcinit_handle, *rcinit_handle_p;                                               // private

// Private Internal Type : rcinit task information database

typedef struct rcinit_info_s                                                     // Private Implementation Details of Database
{
   RCINIT_NAME name;                                                             // string identifier
   rcinit_entry_p              entry;                                            // entry point of task or init_function
   RCINIT_PRIO                 prio;                                             // priority of task
   rcinit_handle_p             handle;                                           // context handle (i.e., *rex_tcb)
   unsigned long               stksz;                                            // stack size
   rcinit_stack_p              stack;                                            // stack pointer
   unsigned long               cpu_affinity;                                     // processor affinity

} rcinit_info, *rcinit_info_p;                                                   // private

// Private Internal Type : rcinit task information database map name to data

typedef const struct rcinit_internal_name_map_s                                  // Private Implementation Details of Database
{
   RCINIT_NAME name;                                                             // string identifier
   rcinit_info_p info;                                                           // internal pointer to rcinit_info structure associated with identifier

} rcinit_internal_name_map_t;                                                    // private

/////////////////////////////////////////////////////////////////////
// Internal Function Prototypes
/////////////////////////////////////////////////////////////////////

extern rcinit_info rcinit_info_rcinit;                                           // access to bootstrap information
extern const int rcinit_internal_name_map_size;                                  // compile time name to database entries
extern const rcinit_internal_name_map_t rcinit_internal_name_map[];              // compile time name to database map
extern const rcinit_info_p* rcinit_internal_sequence_groups[];                   // compile time sequence group map

void rcinit_bootstrap_rcinit_task(void);                                         // private direct access, bootstrap only
DALResult rcinit_task(DALSYSEventHandle hEvent, void* arg_p);                    // private direct access, bootstrap only

#if defined(__cplusplus)
}
#endif

#endif // _RCINITI_H
