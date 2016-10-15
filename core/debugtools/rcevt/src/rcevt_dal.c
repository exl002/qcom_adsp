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

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcevt_dal.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcevt_dal.h"
#include "rcevt_internal.h"

void rce_mutex_init_dal(rce_mutex_p mutex_p)
{
   if (RCE_NULL != mutex_p)
   {
      memset((void*)mutex_p, 0, sizeof(rce_mutex_t));
      if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &(mutex_p->mutex), &(mutex_p->mutexObject)))
      {
         RCE_ERR_FATAL("cannot init mutex", 0, 0, 0);
      }
   }
}

void rce_mutex_lock_dal(rce_mutex_p mutex_p)
{
   if (RCE_NULL != mutex_p)
   {
      DALSYS_SyncEnter(mutex_p->mutex);
   }
}

void rce_mutex_unlock_dal(rce_mutex_p mutex_p)
{
   if (RCE_NULL != mutex_p)
   {
      DALSYS_SyncLeave(mutex_p->mutex);
   }
}

RCEVT_BOOL rce_nfy_eq_dal(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGDAL* sigex_p = (RCEVT_SIGEX_SIGDAL*)(&(nfy_p->sigex));
      if (sigex_p->signal == ((RCEVT_SIGEX_SIGDAL*)sigex)->signal) return (RCEVT_TRUE);
      return (RCEVT_FALSE);
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
      return (RCEVT_FALSE);
   }
}

void rce_nfy_init_dal(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGDAL* sigex_p = (RCEVT_SIGEX_SIGDAL*)(&(nfy_p->sigex));
      if (sizeof(struct rce_nfy_sigex_s) < sizeof(RCEVT_SIGEX_SIGDAL))
      {
         RCE_ERR_FATAL("payload size configuration", 0, 0, 0);
      }
      memset((void*)(&nfy_p->sigex), 0, sizeof(struct rce_nfy_sigex_s));
      sigex_p->signal = ((RCEVT_SIGEX_SIGDAL*)sigex)->signal;
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
   }
}

void rce_nfy_term_dal(rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      memset((void*)(&nfy_p->sigex), 0, sizeof(struct rce_nfy_sigex_s));
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
   }
}

rce_nde_p rce_nfy_sig_dal(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGDAL* sigex_p = (RCEVT_SIGEX_SIGDAL*)(&(nfy_p->sigex));
      rce_nde_p rc = RCE_NULL;
      if (DAL_SUCCESS != DALSYS_EventCtrl(sigex_p->signal, DALSYS_EVENT_CTRL_TRIGGER))
      {
         RCE_ERR_FATAL("cannot trigger event", 0, 0, 0);
      }
      rc = nde_p;
      return (rc);
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
      return (RCE_NULL);
   }
}

rce_nde_p rce_nfy_wait_dal(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGDAL* sigex_p = (RCEVT_SIGEX_SIGDAL*)(&(nfy_p->sigex));
      rce_nde_p rc = RCE_NULL;
      if (DAL_SUCCESS != DALSYS_EventWait(sigex_p->signal))
      {
         RCE_ERR_FATAL("cannot wait event", 0, 0, 0);
      }
      if (DAL_SUCCESS != DALSYS_EventCtrl(sigex_p->signal, DALSYS_EVENT_CTRL_RESET))
      {
         RCE_ERR_FATAL("cannot reset event", 0, 0, 0);
      }
      rc = nde_p;
      return (rc);
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
      return (RCE_NULL);
   }
}
