#ifndef _RCINIT_REX_H
#define _RCINIT_REX_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcinit_rex.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcecb.h"
#include "rcevt.h"
#include "rcinit.h"
#include "rex.h"

#if defined(__cplusplus)
extern "C" {
#endif

RCINIT_INFO rcinit_lookup_info_rextask(rex_tcb_type* tid);
RCINIT_GROUP rcinit_lookup_group_rextask(rex_tcb_type* tid);
rex_tcb_type* rcinit_lookup_rextask(RCINIT_NAME name);
rex_tcb_type* rcinit_lookup_rextask_info(RCINIT_INFO info);

#if defined(__cplusplus)
}
#endif

#endif
