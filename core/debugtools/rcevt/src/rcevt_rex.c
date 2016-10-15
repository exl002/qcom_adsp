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

 $Header: //components/rel/core.adsp/2.2/debugtools/rcevt/src/rcevt_rex.c#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#include "rcevt_rex.h"
#include "rcevt_internal.h"

RCEVT_HANDLE rcevt_register_handle(RCEVT_HANDLE handle, RCEVT_SIGNAL signal)
{
   RCEVT_HANDLE rc = RCE_NULL;
   RCEVT_SIGEX_SIGREX sig;
   sig.signal = rex_self();
   sig.mask = signal;
   rc = rcevt_register_sigex_handle(handle, RCEVT_SIGEX_TYPE_SIGREX, &sig);
   return (rc);
}

RCEVT_HANDLE rcevt_register_name(RCEVT_NAME name, RCEVT_SIGNAL signal)
{
   RCEVT_HANDLE rc = RCE_NULL;
   RCEVT_HANDLE handle = rcevt_create_name(name);
   if (RCE_NULL != handle)
   {
      rc = rcevt_register_handle(handle, signal);
   }
   return (rc);
}

RCEVT_BOOL rce_nfy_eq_rex(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGREX* sigex_p = (RCEVT_SIGEX_SIGREX*)(&(nfy_p->sigex));
      if (sigex_p->signal == ((RCEVT_SIGEX_SIGREX*)sigex)->signal) return (RCEVT_TRUE);
      return (RCEVT_FALSE);
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
      return (RCEVT_FALSE);
   }
}

void rce_nfy_init_rex(rce_nfy_p nfy_p, RCEVT_SIGEX sigex)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGREX* sigex_p = (RCEVT_SIGEX_SIGREX*)(&(nfy_p->sigex));
      if (sizeof(struct rce_nfy_sigex_s) < sizeof(RCEVT_SIGEX_SIGREX))
      {
         RCE_ERR_FATAL("payload size configuration", 0, 0, 0);
      }
      memset((void*)(&nfy_p->sigex), 0, sizeof(struct rce_nfy_sigex_s));
      sigex_p->mask = ((RCEVT_SIGEX_SIGREX*)sigex)->mask;
      sigex_p->signal = ((RCEVT_SIGEX_SIGREX*)sigex)->signal;
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
   }
}

void rce_nfy_term_rex(rce_nfy_p nfy_p)
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

rce_nde_p rce_nfy_sig_rex(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGREX* sigex_p = (RCEVT_SIGEX_SIGREX*)(&(nfy_p->sigex));
      rce_nde_p rc = RCE_NULL;
      rex_set_sigs(sigex_p->signal, sigex_p->mask);
      rc = nde_p;
      return (rc);
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
      return (RCE_NULL);
   }
}

rce_nde_p rce_nfy_wait_rex(rce_nde_p nde_p, rce_nfy_p nfy_p)
{
   if (RCE_NULL != nfy_p)
   {
      RCEVT_SIGEX_SIGREX* sigex_p = (RCEVT_SIGEX_SIGREX*)(&(nfy_p->sigex));
      rce_nde_p rc = RCE_NULL;
      if (rex_self() == sigex_p->signal)
      {
         rex_wait(sigex_p->mask);
         rex_clr_sigs(sigex_p->signal, sigex_p->mask);
         rc = nde_p;
      }
      else
      {
         RCE_ERR_FATAL("cannot wait event", 0, 0, 0);
      }
      return (rc);
   }
   else
   {
      RCE_ERR_FATAL("null handle use", 0, 0, 0);
      return (RCE_NULL);
   }
}
