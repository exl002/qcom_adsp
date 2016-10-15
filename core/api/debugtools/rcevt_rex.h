#ifndef _RCEVT_REX_H
#define _RCEVT_REX_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcevt_rex.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

////////////////////////////////////////
// REGISTER
////////////////////////////////////////

// RCEVT_SIGEX_SIGREX rcevt_sigex;
//
// rcevt_sigex.signal = rex_self();
// rcevt_sigex.mask = RCEVT_REX_SIGNAL_MASK;
//
// if (RCEVT_NULL == rcevt_register_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGREX, &rcevt_sigex))
// {
// MSG("event register"); // handle error
// }

////////////////////////////////////////
// BLOCKING WAIT
////////////////////////////////////////

// rex_sigs_type recv = rex_wait(RCEVT_REX_SIGNAL_MASK);
// if (0 == (recv & RCEVT_REX_SIGNAL_MASK))
// {
// MSG("event wait"); // handle error
// }
// else
// {
// MSG("event handler");
// }

////////////////////////////////////////
// UNREGISTER
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_unregister_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGREX, &rcevt_sigex))
// {
// MSG("event unregister"); // handle error
// }

////////////////////////////////////////
// SIGNAL
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_signal_name(RCEVT_EVENTNAME))
// {
// MSG("event signal"); // handle error


#include "rex.h"
#include "rcevt.h"

#if defined(__cplusplus)
extern "C" {
#endif

// remove definition from RCEVT.H; use following
// typedef rex_sigs_type RCEVT_SIGNAL;

typedef struct
{
   rex_tcb_type* signal;   // context for delivery
   rex_sigs_type mask;     // payload for delivery

} RCEVT_SIGEX_SIGREX;

RCEVT_HANDLE rcevt_register_handle(RCEVT_HANDLE handle, RCEVT_SIGNAL signal);
RCEVT_HANDLE rcevt_register_name(RCEVT_NAME name, RCEVT_SIGNAL signal);

RCEVT_HANDLE rcevt_unregister_handle(RCEVT_HANDLE handle, RCEVT_SIGNAL signal);
RCEVT_HANDLE rcevt_unregister_name(RCEVT_NAME name, RCEVT_SIGNAL signal);

#if defined(__cplusplus)
}
#endif

#endif // _RCEVT_H
