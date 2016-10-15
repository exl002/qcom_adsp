#ifndef _RCEVT_QURT_H
#define _RCEVT_QURT_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcevt_qurt.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

////////////////////////////////////////
// REGISTER
////////////////////////////////////////

// qurt_anysignal_t qurt_signal;
// RCEVT_SIGEX_SIGQURT rcevt_sigex;
//
// qurt_anysignal_init(&qurt_signal);
//
// rcevt_sigex.signal = &qurt_signal;
// rcevt_sigex.mask = RCEVT_QURT_SIGNAL_MASK;
//
// if (RCEVT_NULL == rcevt_register_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGQURT, &rcevt_sigex))
// {
// MSG("event register"); // handle error
// }

////////////////////////////////////////
// BLOCKING WAIT
////////////////////////////////////////

// unsigned int recv = qurt_anysignal_wait(&qurt_signal, RCEVT_QURT_SIGNAL_MASK);
// if (0 == (recv & RCEVT_QURT_SIGNAL_MASK))
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

// if (RCEVT_NULL == rcevt_unregister_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGQURT, &rcevt_sigex))
// {
// MSG("event unregister"); // handle error
// }
//
// qurt_anysignal_destroy(&qurt_signal);

////////////////////////////////////////
// SIGNAL
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_signal_name(RCEVT_EVENTNAME))
// {
// MSG("event signal"); // handle error
// }

#include "qurt.h"
#include "rcevt.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
   qurt_anysignal_t* signal;    // context for delivery
   unsigned int mask;            // payload for delivery

} RCEVT_SIGEX_SIGQURT;

#if defined(__cplusplus)
}
#endif

#endif
