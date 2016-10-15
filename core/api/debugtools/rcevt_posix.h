#ifndef _RCEVT_POSIX_H
#define _RCEVT_POSIX_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcevt_posix.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

////////////////////////////////////////
// REGISTER
////////////////////////////////////////

// pthread_mutex_t posix_mutex;
// pthread_mutexattr_t posix_mutexattr;
// pthread_cond_t posix_cond;
// pthread_condattr_t posix_condattr;
// RCEVT_SIGEX_SIGPOSIX rcevt_sigex;
//
// pthread_mutexattr_init(&posix_mutexattr);
// pthread_mutex_init(&posix_mutex, &posix_mutexattr);
// pthread_mutexattr_destroy(&posix_mutexattr);
//
// pthread_condattr_init(&posix_condattr);
// pthread_cond_init(&posix_cond, &posix_condattr);
// pthread_condattr_destroy(&posix_condattr);
//
// rcevt_sigex.mutex_p = &posix_mutex;
// rcevt_sigex.cond_p = &posix_cond;
//
// if (RCEVT_NULL == rcevt_register_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGPOSIX, &rcevt_sigex))
// {
// MSG("event register"); // handle error
// }

////////////////////////////////////////
// BLOCKING WAIT
////////////////////////////////////////

// pthread_mutex_lock(&posix_mutex);
// pthread_cond_wait(&posix_cond, &posix_mutex);
// pthread_mutex_unlock(&posix_mutex);
//
// MSG("event handler");

////////////////////////////////////////
// UNREGISTER
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_unregister_sigex_name(RCEVT_EVENTNAME, RCEVT_SIGEX_TYPE_SIGPOSIX, &rcevt_sigex))
// {
// MSG("event unregister"); // handle error
// }
//
// pthread_mutex_destroy(&posix_mutex);
// pthread_cond_destroy(&posix_cond);

////////////////////////////////////////
// SIGNAL
////////////////////////////////////////

// if (RCEVT_NULL == rcevt_signal_name(RCEVT_EVENTNAME))
// {
// MSG("event signal"); // handle error
// }

#include "pthread.h"
#include "rcevt.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
   pthread_mutex_t* mutex_p;
   pthread_cond_t* cond_p;

} RCEVT_SIGEX_SIGPOSIX;

#if defined(__cplusplus)
}
#endif

#endif
