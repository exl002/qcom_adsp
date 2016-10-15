/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 RCEVT SOURCE MODULE

 GENERAL DESCRIPTION
 this module contains the source implementation for the event notification
 service. supports incrementing events, static value events, masked value
 events. mix and match combinations are possible, but it is anticipated that
 signaling and blocking calls over a specific event name will be uniform
 in nature.

 EXTERNALIZED FUNCTIONS
 yes

 INITIALIZATION AND SEQUENCING REQUIREMENTS
 yes

 Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcevt_internal.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcevt.h"
#include "rcevt_internal.h"

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

struct rce_internal_s rce_internal;
rce_nfy_pool_t rce_nfy_pool_static;                                              // first pool static storage
rce_nde_pool_t rce_nde_pool_static;                                              // first pool static storage
rce_nde_p rce_hashtab[RCEVT_HASHTABLE_BUCKETS];                                  // hashtable lookup buckets
