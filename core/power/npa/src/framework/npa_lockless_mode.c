/*============================================================================
@file npa_lockless_mode.c

NPA lockless mode

This code implements an NPA node which switches all instances of CoreMutex
to lockless mode, where the lock and unlock operations do nothing.  Lockless
mode is entered by a non-zero request to the code; lockless mode is exited by
completing that request.

No checking is done to ensure that it is a safe time to enter lockless mode; 
it is assumed that the client has that knowledge.

Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
 
$Header: //components/rel/core.adsp/2.2/power/npa/src/framework/npa_lockless_mode.c#1 $
============================================================================*/

#include "npa_resource.h"
#include "CoreMutex.h"

#ifdef __cplusplus
extern "C" {
#endif

static
npa_resource_state coremutex_lockless_mode_toggle( npa_resource      *resource,
                                                   npa_client        *client,
                                                   npa_resource_state state )
{
  /* We don't bother with an NPA_CLIENT_INITIALIZE section because we
   * pass in 0 as initial value in npa_define_node and the logic below
   * handles the case.
   */
  if( 0 == state )
  {
    // (Re)enable all CoreMutex locks
    Core_MutexEnable();
  }
  else
  {
    // Disable all CoreMutex locks
    Core_MutexDisable();
  }
  return state;
}

/* The lockless mode toggle resource */
static npa_resource_definition coremutex_lockless_mode_resource[] =
{
  {
    "/core/mutex/lockless_mode",
    "toggle",
    1,
    &npa_identity_plugin,
    NPA_RESOURCE_DEFAULT,
  }
};

/* The lockless mode toggle node */
npa_node_definition coremutex_lockless_node = 
{ 
  "/core/mutex/lockless_mode",
  coremutex_lockless_mode_toggle, 
  NPA_NODE_NO_LOCK,
  NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(coremutex_lockless_mode_resource)
};

#ifdef __cplusplus
}
#endif
