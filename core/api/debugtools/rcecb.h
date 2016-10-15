#ifndef _RCECB_H
#define _RCECB_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcecb.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

#define RCECB_DEPRECATED_ATTRIBUTE  __attribute__((deprecated))

/////////////////////////////////////////////////////////////////////
// Exported Type Declarations
/////////////////////////////////////////////////////////////////////

// RCECB - Events w/ Callback Utilization

typedef const char* RCECB_NAME;                                                  // Name (RCEVT, RCECB)
typedef void* RCECB_HANDLE;                                                      // Opaque Handle (RCECB)
typedef void* RCECB_CONTEXT;                                                     // Opaque handle (RCECB) Unique Function Context ID
typedef enum { RCECB_FALSE, RCECB_TRUE } RCECB_BOOL;                             // Query Return (RCECB)
typedef void(*RCECB_FNSIG_VOID_VOID)(void);                                      // Specific Function Signature

#define RCECB_NULL ((void*)0)

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
void rcecb_init(void);
void rcecb_term(void);

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
RCECB_HANDLE rcecb_map_name(RCECB_NAME name);
RCECB_NAME rcecb_map_handle(RCECB_HANDLE handle);

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
RCECB_HANDLE rcecb_create_name(RCECB_NAME name);

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
RCECB_BOOL rcecb_search_context_handle(RCECB_HANDLE handle, RCECB_CONTEXT context);
RCECB_BOOL rcecb_search_context_name(RCECB_NAME name, RCECB_CONTEXT context);

RCECB_BOOL rcecb_search_handle(RCECB_HANDLE handle);
RCECB_BOOL rcecb_search_name(RCECB_NAME name);

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
int rcecb_getcontexts_handle(RCECB_HANDLE handle);
int rcecb_getcontexts_name(RCECB_NAME name);

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
//RCEVT_THRESHOLD rcecb_getcount_handle(RCECB_HANDLE handle);
//RCEVT_THRESHOLD rcecb_getcount_name(RCECB_NAME name);

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
RCECB_HANDLE rcecb_register_handle(RCECB_HANDLE handle, RCECB_CONTEXT context);
RCECB_HANDLE rcecb_register_name(RCECB_NAME name, RCECB_CONTEXT context);

/*RCECB_DEPRECATED_ATTRIBUTE*/ RCECB_HANDLE rcecb_register_context_handle(RCECB_HANDLE handle, RCECB_CONTEXT context);
/*RCECB_DEPRECATED_ATTRIBUTE*/ RCECB_HANDLE rcecb_register_context_name(RCECB_NAME name, RCECB_CONTEXT context);

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
RCECB_HANDLE rcecb_unregister_handle(RCECB_HANDLE handle, RCECB_CONTEXT context);
RCECB_HANDLE rcecb_unregister_name(RCECB_NAME name, RCECB_CONTEXT context);

/*RCECB_DEPRECATED_ATTRIBUTE*/ RCECB_HANDLE rcecb_unregister_context_handle(RCECB_HANDLE handle, RCECB_CONTEXT context);
/*RCECB_DEPRECATED_ATTRIBUTE*/ RCECB_HANDLE rcecb_unregister_context_name(RCECB_NAME name, RCECB_CONTEXT context);

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
RCECB_HANDLE rcecb_signal_handle(RCECB_HANDLE handle);
RCECB_HANDLE rcecb_signal_name(RCECB_NAME name);

RCECB_HANDLE rcecb_signal_handle_nolocks(RCECB_HANDLE handle); // specialized, no mutex protections, single thread only, *not for general use*
RCECB_HANDLE rcecb_signal_name_nolocks(RCECB_NAME name);       // specialized, no mutex protections, single thread only, *not for general use*

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
// NONE

#if defined(__cplusplus)
}
#endif

#endif
