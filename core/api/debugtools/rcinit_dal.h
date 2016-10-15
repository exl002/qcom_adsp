#ifndef _RCINIT_DAL_H
#define _RCINIT_DAL_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcinit_dal.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcecb.h"
#include "rcevt.h"
#include "rcinit.h"
#include "DALSys.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct { DALSYSEventHandle hEventStart; DALSYSWorkLoopHandle hWorkLoop; } RCINIT_TASK_DALTASK_CTX;
RCINIT_INFO rcinit_lookup_info_daltask(RCINIT_TASK_DALTASK_CTX* tid);
RCINIT_GROUP rcinit_lookup_group_daltask(RCINIT_TASK_DALTASK_CTX* tid);
RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask(RCINIT_NAME name);
RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask_info(RCINIT_INFO info);

#if defined(__cplusplus)
}
#endif

#endif
