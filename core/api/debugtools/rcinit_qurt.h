#ifndef _RCINIT_QURT_H
#define _RCINIT_QURT_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcinit_qurt.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcecb.h"
#include "rcevt.h"
#include "rcinit.h"
#include "qurt.h"

#if defined(__cplusplus)
extern "C" {
#endif

RCINIT_INFO rcinit_lookup_info_qurttask(qurt_thread_t tid);
RCINIT_GROUP rcinit_lookup_group_qurttask(qurt_thread_t tid);
qurt_thread_t rcinit_lookup_qurttask(RCINIT_NAME name);
qurt_thread_t rcinit_lookup_qurttask_info(RCINIT_INFO info);

#if defined(__cplusplus)
}
#endif

#endif
