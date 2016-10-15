#ifndef _RCEVT_H
#define _RCEVT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 RCE - RUN CONTROL EVENT API

 GENERAL DESCRIPTION
 this module contains the source implementation for the event notification
 service

 EXTERNALIZED FUNCTIONS
 yes

 INITIALIZATION AND SEQUENCING REQUIREMENTS
 yes

 Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcevt.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

// USE APPROPRIATE HEADER FILE TO ACCESS THE RCEVT API AS DEPENDENCY. DIRECT INCLUSION
// OF THIS HEADER MODULE IS SUPPORTED ONLY WHEN NHLOS SIGNAL INFORMATION IS NOT
// REQUIRED BY A CLIENT IMPLEMENTATION.

// #include "rcevt_dal.h"     // NHLOS DAL
// #include "rcevt_posix.h"   // NHLOS POSIX
// #include "rcevt_qurt.h"    // NHLOS QURT
// #include "rcevt_rex.h"     // NHLOS REX

#if defined(__cplusplus)
extern "C" {
#endif

#define RCEVT_DEPRECATED_ATTRIBUTE  __attribute__((deprecated))

/////////////////////////////////////////////////////////////////////
// Exported Type Declarations
/////////////////////////////////////////////////////////////////////

// RCEVT - Events w/ Signal Notification

typedef const char* RCEVT_NAME;                                                  // Name (RCEVT, RCECB)
typedef void* RCEVT_HANDLE;                                                      // Opaque Handle (RCEVT)
typedef enum { RCEVT_FALSE, RCEVT_TRUE } RCEVT_BOOL;                             // Query Return (RCEVT)
typedef void(*RCEVT_FNSIG_VOID_VOID)(void);                                      // Specific Function Signature
typedef unsigned long RCEVT_THRESHOLD;                                           // Threshold (RCEVT, RCECB)
typedef unsigned long RCEVT_EXPIRY;                                              // Expiry (RCEVT)
typedef unsigned long RCEVT_ORDER;                                               // Ordering (RCEVT)

typedef unsigned long RCEVT_SIGNAL; // REX ONLY; RCEVT API1.0; USE RCEVT_REX.H
// typedef rex_sigs_type RCEVT_SIGNAL; // REX ONLY; RCEVT API1.0; USE RCEVT_REX.H

typedef enum
{
   RCEVT_SIGEX_TYPE_CALLBACK,                                                    // Type RCECB (Internal Utilization, Use RCECB API)
   RCEVT_SIGEX_TYPE_SIGDAL,                                                      // Type RCEVT/DAL
   RCEVT_SIGEX_TYPE_SIGPOSIX,                                                    // Type RCEVT/POSIX
   RCEVT_SIGEX_TYPE_SIGQURT,                                                     // Type RCEVT/QURT
   RCEVT_SIGEX_TYPE_SIGREX,                                                      // Type RCEVT/REX
   RCEVT_SIGEX_TYPE_NONE                                                         // None

} RCEVT_SIGEX_TYPE;

typedef void* RCEVT_SIGEX;                                                       // Opaque Handle

#define RCEVT_NULL ((void*)0)

/////////////////////////////////////////////////////////////////////
// Exported Function Prototypes
/////////////////////////////////////////////////////////////////////

/*===========================================================================

FUNCTION init/term

DESCRIPTION
initialize all internal data structures

DEPENDENCIES
none

RETURN VALUE
none

SIDE EFFECTS
none

===========================================================================*/
void rcevt_init(void);
void rcevt_term(void);

/*===========================================================================

FUNCTION map

DESCRIPTION
map name to handle

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/
RCEVT_HANDLE rcevt_map_name(RCEVT_NAME name);
RCEVT_NAME rcevt_map_handle(RCEVT_HANDLE handle);

/*===========================================================================

FUNCTION create

DESCRIPTION
create an event with name

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/
RCEVT_HANDLE rcevt_create_name(RCEVT_NAME name);

/*===========================================================================

FUNCTION search

DESCRIPTION
searches for a presense of name
searches for a presense of context associated to name

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/
//RCEVT_BOOL rcevt_search_sigex_handle(RCEVT_HANDLE handle, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);
//RCEVT_BOOL rcevt_search_sigex_name(RCEVT_NAME name, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);

RCEVT_BOOL rcevt_search_handle(RCEVT_HANDLE handle);
RCEVT_BOOL rcevt_search_name(RCEVT_NAME name);

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
int rcevt_getcontexts_handle(RCEVT_HANDLE handle);
int rcevt_getcontexts_name(RCEVT_NAME name);

/*===========================================================================

FUNCTION getcount

DESCRIPTION
searches for a handle based on key
returns the current value associated to caller if requested

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/
RCEVT_THRESHOLD rcevt_getcount_handle(RCEVT_HANDLE handle);
RCEVT_THRESHOLD rcevt_getcount_name(RCEVT_NAME name);

/*===========================================================================

FUNCTION register

DESCRIPTION
caller registered to signal notify queue by key

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
caller provides the signal value for the hlos implementation; this
allows the caller to manage what signal is utilized for notification

===========================================================================*/
RCEVT_HANDLE rcevt_register_sigex_handle(RCEVT_HANDLE handle, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX signal);
RCEVT_HANDLE rcevt_register_sigex_name(RCEVT_NAME name, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX signal);

RCEVT_HANDLE rcevt_register_handle(RCEVT_HANDLE handle, RCEVT_SIGNAL signal); // REX ONLY; RCEVT API1.0; USE RCEVT_REX.H
RCEVT_HANDLE rcevt_register_name(RCEVT_NAME name, RCEVT_SIGNAL signal); // REX ONLY; RCEVT API1.0; USE RCEVT_REX.H

/*===========================================================================

FUNCTION unregister

DESCRIPTION
caller unregistered from signal notify queue by key

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/
RCEVT_HANDLE rcevt_unregister_sigex_handle(RCEVT_HANDLE handle, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);
RCEVT_HANDLE rcevt_unregister_sigex_name(RCEVT_NAME name, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);

RCEVT_HANDLE rcevt_unregister_handle(RCEVT_HANDLE handle, RCEVT_SIGNAL signal); // REX ONLY; RCEVT API1.0; USE RCEVT_REX.H
RCEVT_HANDLE rcevt_unregister_name(RCEVT_NAME name, RCEVT_SIGNAL signal); // REX ONLY; RCEVT API10.; USE RCEVT_REX.H

/*===========================================================================

FUNCTION signal

DESCRIPTION
caller signals specific key

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
none

===========================================================================*/
RCEVT_HANDLE rcevt_signal_handle(RCEVT_HANDLE handle);
RCEVT_HANDLE rcevt_signal_name(RCEVT_NAME name);

/*===========================================================================

FUNCTION wait

DESCRIPTION
caller optionally blocks for specific name to be signalled

DEPENDENCIES
none

RETURN VALUE
success of operation

SIDE EFFECTS
caller does not block if the signal condition has already been met

===========================================================================*/
RCEVT_HANDLE rcevt_wait_sigex_handle(RCEVT_HANDLE handle, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);
RCEVT_HANDLE rcevt_wait_sigex_name(RCEVT_NAME name, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex);

RCEVT_HANDLE rcevt_wait_handle(RCEVT_HANDLE handle);
RCEVT_HANDLE rcevt_wait_name(RCEVT_NAME name);

RCEVT_HANDLE rcevt_wait_count_handle(RCEVT_HANDLE handle, RCEVT_THRESHOLD threshold);
RCEVT_HANDLE rcevt_wait_count_name(RCEVT_NAME name, RCEVT_THRESHOLD threshold);

#if defined(__cplusplus)
}
#endif

#endif

