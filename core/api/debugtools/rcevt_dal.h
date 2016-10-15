#ifndef _RCEVT_DAL_H
#define _RCEVT_DAL_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcevt_dal.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

////////////////////////////////////////
// REGISTER
////////////////////////////////////////

// DALSYSEventHandle hEvent;
// RCEVT_SIGEX_SIGDAL rcevt_sigex;
//
// if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent, NULL))
// {
// MSG("event create"); // handle error
// }
//
// rcevt_sigex.signal = hEvent;
//
// if (RCEVT_NULL == rcevt_register_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGDAL, &rcevt_sigex))
// {
// MSG("event register"); // handle error
// }

////////////////////////////////////////
// BLOCKING WAIT
////////////////////////////////////////

// if (DAL_SUCCESS != DALSYS_EventWait(hEvent))
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

// if (RCEVT_NULL == rcevt_unregister_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGDAL, &rcevt_sigex))
// {
// MSG("event unregister"); // handle error
// }
//
// if (DAL_SUCCESS != DALSYS_DestroyObject(hEvent))
// {
// MSG("event destroy"); // handle error
// }

////////////////////////////////////////
// SIGNAL
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_signal_name(RCEVT_EVENTNAME))
// {
// MSG("event signal"); // handle error

#include "DALSys.h"
#include "rcevt.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
   DALSYSEventHandle signal;  // context for delivery

} RCEVT_SIGEX_SIGDAL;

#if defined(__cplusplus)
}
#endif

#endif
