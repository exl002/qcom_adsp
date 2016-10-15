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

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcevt_qurt_void.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcevt.h"
#include "rcevt_internal.h"

RCEVT_BOOL rce_nfy_eq_qurt(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
      return (RCEVT_FALSE);
   }
   else
   {
      return (RCEVT_FALSE);
   }
}

void rce_nfy_init_qurt(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
   }
   else
   {
   }
}

void rce_nfy_term_qurt(rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
   }
   else
   {
   }
}

rce_nde_p rce_nfy_sig_qurt(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      nde_p = nde_p;
      return (RCE_NULL);
   }
   else
   {
      return (RCE_NULL);
   }
}

rce_nde_p rce_nfy_wait_qurt(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      nde_p = nde_p;
      return (RCE_NULL);
   }
   else
   {
      return (RCE_NULL);
   }
}
