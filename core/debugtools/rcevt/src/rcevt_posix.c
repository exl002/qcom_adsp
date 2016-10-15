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

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcevt_posix.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "pthread.h"
#include "rcevt.h"
#include "rcevt_posix.h"
#include "rcevt_internal.h"

RCEVT_BOOL rce_nfy_eq_posix(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGPOSIX* sigex_p = (RCEVT_SIGEX_SIGPOSIX*)(&(nfy_p->sigex));
      if (sigex_p->mutex_p == ((RCEVT_SIGEX_SIGPOSIX*)sigex)->mutex_p) return (RCEVT_TRUE);
      return (RCEVT_FALSE);
   }
   else
   {
      return (RCEVT_FALSE);
   }
}

void rce_nfy_init_posix(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGPOSIX* sigex_p = (RCEVT_SIGEX_SIGPOSIX*)(&(nfy_p->sigex));
      memset((void*)(&nfy_p->sigex), 0, sizeof(struct rce_nfy_sigex_s));
      sigex_p->mutex_p = ((RCEVT_SIGEX_SIGPOSIX*)sigex)->mutex_p;
      sigex_p->cond_p = ((RCEVT_SIGEX_SIGPOSIX*)sigex)->cond_p;
   }
}

void rce_nfy_term_posix(rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      memset((void*)(&nfy_p->sigex), 0, sizeof(struct rce_nfy_sigex_s));
   }
}

rce_nde_p rce_nfy_sig_posix(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGPOSIX* sigex_p = (RCEVT_SIGEX_SIGPOSIX*)(&(nfy_p->sigex));
      rce_nde_p rc = RCE_NULL;
      pthread_mutex_lock(sigex_p->mutex_p);
      pthread_cond_broadcast(sigex_p->cond_p);
      pthread_mutex_unlock(sigex_p->mutex_p);
      rc = nde_p;
      return (rc);
   }
   else
   {
      return (RCE_NULL);
   }
}

rce_nde_p rce_nfy_wait_posix(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGPOSIX* sigex_p = (RCEVT_SIGEX_SIGPOSIX*)(&(nfy_p->sigex));
      rce_nde_p rc = RCE_NULL;
      pthread_mutex_lock(sigex_p->mutex_p);
      pthread_cond_wait(sigex_p->cond_p, sigex_p->mutex_p);
      pthread_mutex_unlock(sigex_p->mutex_p);
      rc = nde_p;
      return (rc);
   }
   else
   {
      return (RCE_NULL);
   }
}
