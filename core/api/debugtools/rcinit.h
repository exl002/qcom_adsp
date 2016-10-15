#ifndef _RCINIT_H
#define _RCINIT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 EVENT NOTIFICATION API

 GENERAL DESCRIPTION
 this module contains the source implementation for the event notification
 service

 EXTERNALIZED FUNCTIONS
 yes

 INITIALIZATION AND SEQUENCING REQUIREMENTS
 yes

 Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcinit.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcecb.h"
#include "rcevt.h"

#if defined(__cplusplus)
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////
// Exported API Details
/////////////////////////////////////////////////////////////////////

// External Exposed Events : Observers Ok

#define RCINIT_RCEVT_PREFIX                  "rcinit:"                           // prefix for rcinit events (observers ok)
#define RCINIT_RCEVT_INITGROUPS              RCINIT_RCEVT_PREFIX "initgroups"    // observers ok
#define RCINIT_RCEVT_TERMGROUPS              RCINIT_RCEVT_PREFIX "termgroups"    // observers ok

// Exported Manifest Constants

#define RCINIT_NULL           ((void*)0)                                         // return result
#define RCINIT_ZERO           (0)                                                // return result

typedef enum
{
   RCINIT_GROUP_0,                                                               // internal representation group 0
   RCINIT_GROUP_1,                                                               // internal representation group 1
   RCINIT_GROUP_2,                                                               // internal representation group 2
   RCINIT_GROUP_3,                                                               // internal representation group 3
   RCINIT_GROUP_4,                                                               // internal representation group 4
   RCINIT_GROUP_5,                                                               // internal representation group 5
   RCINIT_GROUP_6,                                                               // internal representation group 6
   RCINIT_GROUP_7,                                                               // internal representation group 7
   RCINIT_GROUP_NONE                                                             // end of list

} RCINIT_GROUP;

typedef enum
{
   RCINIT_TASK_INITFN         = 1,                                               // initialization function callbacks (not task)
   RCINIT_TASK_TERMFN         = 2,                                               // termination function callbacks (not task)
   RCINIT_TASK_DALTASK        = 3,                                               // dalsys tasks
   RCINIT_TASK_PTHREAD        = 4,                                               // pthread tasks
   RCINIT_TASK_QURTTASK       = 5,                                               // qurt tasks
   RCINIT_TASK_REXTASK        = 6,                                               // rex tasks
   RCINIT_TASK_LEGACY         = 7,                                               // rex tasks with feature guard
   RCINIT_TASK_NONE                                                              // end of list

} RCINIT_TASK;

typedef enum
{
   RCINIT_PRIO_MAX            = 255,                                             // Priority HIGH = 255, to LOW = 1, mapping performed internally for nonhlos use
   RCINIT_PRIO_MIN            = 1,                                               // Priority HIGH = 255, to LOW = 1, mapping performed internally for nonhlos use
   RCINIT_PRIO_INITFN         = 0,                                               // Place Holder (not actual priority initfn run at)
   RCINIT_PRIO_NONE           = -1                                               // Not Assigned

} RCINIT_PRIO;

/////////////////////////////////////////////////////////////////////
// Exported Types and Function Prototypes
/////////////////////////////////////////////////////////////////////

typedef void* RCINIT_INFO;                                                       // Opaque Handle
typedef const char *RCINIT_NAME;                                                 // Opaque Handle
typedef void* RCINIT_HANDLE;                                                     // Opaque Handle

/*===========================================================================

 FUNCTION rcinit_lookup

 DESCRIPTION
 name to rcinit database lookups

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none

 ===========================================================================*/
RCINIT_INFO rcinit_lookup(RCINIT_NAME name);
RCINIT_NAME rcinit_lookup_name_info(RCINIT_INFO info);

RCINIT_PRIO rcinit_lookup_prio(RCINIT_NAME name);
RCINIT_PRIO rcinit_lookup_prio_info(RCINIT_INFO info);

int rcinit_lookup_group_count(void);
RCINIT_GROUP rcinit_lookup_group(RCINIT_NAME name);
RCINIT_GROUP rcinit_lookup_group_entry(void* entry);

unsigned long rcinit_lookup_stksz(RCINIT_NAME name);
unsigned long rcinit_lookup_stksz_info(RCINIT_INFO info);

void* rcinit_lookup_entry(RCINIT_NAME name);
void* rcinit_lookup_entry_info(RCINIT_INFO info);

/*===========================================================================

 FUNCTION rcinit_register/rcinit_handshake

 DESCRIPTION
 required task handshake to rcinit framework following task define

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_handshake_init(void);
void rcinit_handshake_startup(void);         // alias to rcinit_handshake_init()

void rcinit_handshake_term(void);
void rcinit_handshake_shutdown(void);        // alias to rcinit_handshake_term()

void rcinit_register_term(RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);
void rcinit_register_term_group(RCINIT_GROUP group, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);
void rcinit_unregister_term_group(RCINIT_GROUP group, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);

void rcinit_register_termfn(void(*fn)(void));
void rcinit_register_termfn_group(RCINIT_GROUP group, void(*fn)(void));
void rcinit_unregister_termfn_group(RCINIT_GROUP group, void(*fn)(void));

/*===========================================================================

 FUNCTION rcinit_initfn_spawn_task

 DESCRIPTION
 required wrapper for an init function to spawn a task (very specialized edge
 case found during sequence group 0, this *is* an exception)

 DEPENDENCIES
 Requires existing RCINIT task database information where the task entry
 point is NULL. The static data is preallocated, and this call binds the
 entry point to the context and starts it. Edge case; only allowed from
 Group 0 initfn processing. All other use cases are not supported and
 are considered a bug.

 RETURN VALUE
 none

 SIDE EFFECTS
 task must call rcinit_handshake_startup(); else startup process will block
 as designed

 ===========================================================================*/
void rcinit_initfn_spawn_task(RCINIT_INFO info_p, void *entry);

#if defined(__cplusplus)
}
#endif

// this dependency is present *only* to support compatiblity to rcinit api 1.1;
// it is to be removed for rcinit api 2.0; the impact to a client is to directly
// include rcinit_rex.h instead of rcinit.h

//#include "rcinit_rex.h" // required for compatiblity to rcinit api 1.1

#endif // _RCINIT_H
