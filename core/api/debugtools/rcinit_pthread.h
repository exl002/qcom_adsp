#ifndef _RCINIT_PTHREAD_H
#define _RCINIT_PTHREAD_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcinit_pthread.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcecb.h"
#include "rcevt.h"
#include "rcinit.h"
#include "pthread.h"
#include "sched.h"

#if defined(__cplusplus)
extern "C" {
#endif

RCINIT_INFO rcinit_lookup_info_pthread(pthread_t tid);
RCINIT_GROUP rcinit_lookup_group_pthread(pthread_t tid);
pthread_t rcinit_lookup_pthread(RCINIT_NAME name);
pthread_t rcinit_lookup_pthread_info(RCINIT_INFO info);

#if defined(__cplusplus)
}
#endif

#endif
