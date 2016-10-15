/*============================================================================
@file npa_remote_rpm_protocol.c

NPA Remote RPM Protocol

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
 
$Header: //components/rel/core.adsp/2.2/power/npa/src/remote/npa_remote_rpm_protocol.c#2 $
============================================================================*/

#include "npa_remote_resource.h"
#include "npa_remote_rpm_protocol.h"
#include "npa_internal.h"
#include "rpmclient.h"
#include "npa_inlines.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Local client context for remote resources */
typedef struct npa_rpm_client_context
{
  /* Identity of remote resources on the RPM */
  struct
  {
    rpm_resource_type type;
    unsigned          id;
  } remote_name;

  unsigned            wrote_sleep;
  unsigned            units;
  kvp_t              *new_kvps;
} npa_rpm_client_context;

typedef struct npa_rpm_client_context *npa_rpm_client_handle;

static struct
{
  CorePoolConfigType pool_config;
} npa_rpm_config =
{
  { sizeof(npa_rpm_client_context), 30, 10 }
};

static CorePoolType npa_rpm_client_handle_pool;

/* Custom event and action for deferred barrier() calls on fire-and-forgets */
static npa_custom_event *npa_rpm_deferred_barrier_event = NULL;
static npa_event_action *npa_rpm_deferred_barrier_action = NULL;

/* Pointer to the function we use to flush msgs to the RPM.
 * Initialized to rpm_barrier, this may be toggled to rpm_churn_queue by
 * issuing a non-zero request to the "/protocol/rpm/polling_mode" resource.
 */
static void (*npa_rpm_sync)( unsigned int ) = rpm_barrier;

static void 
npa_rpm_end_transaction( npa_transaction_handle transaction,
                         npa_transaction_extension *extension );

static npa_transaction_extension npa_rpm_transaction_extension = 
{
  { NULL, NPA_EXTENSION_RPM_TRANSACTION }, /* extension header */
  npa_rpm_end_transaction,
};

static
npa_query_status npa_rpm_query_function( npa_resource    *resource,
                                         unsigned int     query_id,
                                         npa_query_type  *result )
{
  switch ( query_id )
  {
  case NPA_QUERY_REMOTE_RESOURCE_AVAILABLE:
  {
    /* The RPM boots first, so all of its resources are always available.
     * Call npa_remote_resource_availble with the given context
     */
    npa_remote_resource_available( result->data.reference ); 
    break;
  }
  
  /* If the RPM supported remote queries and the REMOTE_PROXY attribute
     was set, we would do something like this -
  case NPA_QUERY_CURRENT_STATE:
    if ( npa_resource_has_attribute( resource, NPA_RESOURCE_REMOTE_PROXY ) )
    {
      // query the RPM for remote state and return
    }
    else, fall-through to default for resources that don't have that attribute
   */

  default:
    /* Let npa_query handle the query */
    return NPA_QUERY_UNSUPPORTED_QUERY_ID;
  }

  return NPA_QUERY_SUCCESS;
}

static void npa_rpm_do_deferred_barrier( void *context, unsigned int evt_type,
                                         void *data, unsigned int size )
{
  /* 'context' is the unsigned msg_id cast as void* */
  unsigned int msg_id = (unsigned int)(context);
  
  ULOG_RT_PRINTF_1( npa.log, "npa_rpm_do_deferred_barrier (msg_id: 0x%08x)",
                    msg_id );

  /* Call rpm_barrier directly, rather than npa_rpm_sync, because this callback
     can only be called from the event workloop */
  rpm_barrier( msg_id );
}

static NPA_INLINE void npa_rpm_init( void )
{
  Core_PoolConfig( &npa_rpm_client_handle_pool, &npa_rpm_config.pool_config );
                   
  /* Create a custom event for deferred barrier() calls */
  npa_rpm_deferred_barrier_event = \
          npa_create_custom_event( "npa_rpm_deferred_barrier_event",
                                   npa_rpm_do_deferred_barrier, NULL );
}

static void npa_rpm_create_client_context( npa_resource *resource )
{
  npa_rpm_client_handle handle;
  const char *remote_name = resource->node->name;

  CORE_VERIFY_PTR( handle = (npa_rpm_client_handle)
                            Core_PoolAlloc( &npa_rpm_client_handle_pool ) );
  memset( handle, 0, sizeof(npa_rpm_client_context) );

  /* Reinterpret resource->definition->units as an unsigned int */
  {
    const char *units = resource->definition->units;
    unsigned int i, len = strlen( units );
    
    if ( len > 4 ) len = 4;
    for ( i = 0; i < len; ++i )
    {
      handle->units |= units[i] << 8*i;
    }
  }

  /* Reinterpret remote_resource_name as (4 bytes of type & 4 bytes of id)
   * Resource authors must pad both type and id with NULLs, if either is
   * less than 4 bytes in length. The type is a 4-byte character array and
   * the id is an unsigned int encoded little-endian.
   *
   * Correct names look like - "clk1\x01\0\0\0", "reg\0\x03\0\0\0",
   * "ldo\0\0\0\0\0" or "vreg\x01\x01\0\0"
   *
   * Assumes 8 bytes of data and will crash and burn if not the case!
   * TODO: Don't require the id part to be NULL-padded?
   */
  handle->remote_name.type 
    = (rpm_resource_type) (remote_name[0] | remote_name[1] << 8 |
                           remote_name[2] << 16 | remote_name[3] << 24);

  handle->remote_name.id = remote_name[4] | remote_name[5] << 8 |
                           remote_name[6] << 16 | remote_name[7] << 24;

  if ( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    /* Vector resources manage KVP buffers on their own. At each request, we
     * are given a KVP buffer in resource->state_vector. We don't need to
     * pre-allocate a buffer, but still need to create a kvp_t object.
     */
    handle->new_kvps = kvp_create( 0 );
  }
  else
  {
    /* We build a KVP buffer for scalar resources on the fly. Preallocate
     * the memory used for that now to prevent malloc'ing from an
     * uncomfortable location later (e.g., an IST). The 24 bytes will hold
     * 2 KVPs (type, len, value triples); the first is the state and the
     * second the active_max.
     */
    handle->new_kvps = kvp_create( 24 );
  }

  /* Associate client context with resource */
  resource->node->data = handle;                                          
}

/* The MAX key */
#define STR_MAX_AS_KEY 0x78616d

static NPA_INLINE
unsigned int npa_rpm_post_request( npa_resource *resource, 
                                   rpm_set_type  set, 
                                   npa_resource_state state )
{
  unsigned int msg_id;
  npa_rpm_client_handle handle = (npa_rpm_client_handle)resource->node->data;
  npa_resource_state max = resource->active_max;

  kvp_clear( handle->new_kvps );
  kvp_put( handle->new_kvps, handle->units, 4, (const char *)&state );
  kvp_put( handle->new_kvps, STR_MAX_AS_KEY, 4, (const char *)&max );

  msg_id = rpm_post_request( set,
                             handle->remote_name.type, handle->remote_name.id,
                             handle->new_kvps );
  
  ULOG_RT_PRINTF_6( npa.log, "npa_rpm_resource_driver:post (set: %d) "
                    "(resource: \"%s\") (handle: 0x%08x) "
                    "(state: %u) (max: %u) (msg_id: 0x%08x)", set,
                    resource->definition->name, resource, state, max, msg_id );
  return msg_id;
}

static
npa_resource_state npa_rpm_resource_driver( npa_resource      *resource,
                                            npa_client_handle  client,
                                            npa_resource_state state )
{
  unsigned int msg_id = 0;
  npa_transaction_handle transaction = resource->transaction;
  npa_rpm_client_handle handle = (npa_rpm_client_handle)resource->node->data;
  
  rpm_set_type set = \
    npa_request_has_attribute( client, NPA_REQUEST_NEXT_AWAKE ) ? 
    RPM_NEXT_ACTIVE_SET : RPM_ACTIVE_SET;

  if ( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    kvp_swapbuffer( handle->new_kvps, (const char *)resource->state_vector,
                    resource->active_max * sizeof(npa_resource_state) );

    msg_id = rpm_post_request( set, handle->remote_name.type,
                               handle->remote_name.id, handle->new_kvps );
                                    
    ULOG_RT_PRINTF_6( npa.log, "npa_rpm_resource_driver:post (set: %d) "
                      "(resource: \"%s\") (handle: 0x%08x) (vector: 0x%08x) "
                      "(size: %d) (msg_id: 0x%08x)",
                      set, resource->definition->name, resource,
                      resource->state_vector, resource->active_max, msg_id );

    if ( npa_resource_has_attribute( resource, 
                                     NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ) )
    {
      if ( 0 != memcmp( resource->required_state_vector, resource->state_vector,
                        resource->active_max * sizeof(npa_resource_state) ) )
      {
        if ( client->type == NPA_CLIENT_VECTOR || handle->wrote_sleep == 0 )
        {
          kvp_swapbuffer( handle->new_kvps,
                          (const char *)resource->required_state_vector,
                          resource->active_max * sizeof(npa_resource_state) );

          rpm_post_request( RPM_SLEEP_SET, handle->remote_name.type,
                            handle->remote_name.id, handle->new_kvps );
                                        
          ULOG_RT_PRINTF_3( npa.log, "npa_rpm_resource_driver:post_ss "
                            "(resource: \"%s\") (vector: 0x%08x) (size: %d)",
                            resource->definition->name,
                            resource->required_state_vector, resource->active_max );
          handle->wrote_sleep = 1;
        }
      }
      else if ( handle->wrote_sleep )
      {
        rpm_post_request( RPM_SLEEP_SET, handle->remote_name.type,
                          handle->remote_name.id, NULL );
        ULOG_RT_PRINTF_2( npa.log, "npa_rpm_resource_driver:invalidate_ss "
                          "(resource: \"%s\") (handle: 0x%08x)",
                          resource->definition->name, resource );
        handle->wrote_sleep = 0;
      }
    }
  }
  else
  {
    if ( state != resource->active_state )
    {
      msg_id = npa_rpm_post_request( resource, set, state );
    }
  
    /* In the absence of an explicit sleep set request, the sleep set for the
     * particular resource inherits state from the active set. So, we only
     * need to write to the sleep set if the required aggregation is different
     * from overall. If the required aggregation is the same as overall and 
     * we've previously written a sleep set request, we invalidate that.
     */

    if ( npa_resource_has_attribute( resource, 
                                     NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ) )
    {
      if ( state != NPA_RESOURCE_REQUIRED_REQUEST(resource) )
      {
        if ( client->type == NPA_CLIENT_REQUIRED || handle->wrote_sleep == 0 )
        {
          npa_rpm_post_request( resource, RPM_SLEEP_SET, 
                                NPA_RESOURCE_REQUIRED_REQUEST(resource) );
          handle->wrote_sleep = 1;
        }
      }
      else if ( handle->wrote_sleep )
      {
        /* Invalidate any previous sleep set request */
        rpm_post_request( RPM_SLEEP_SET, handle->remote_name.type,
                          handle->remote_name.id, NULL );
        ULOG_RT_PRINTF_2( npa.log, "npa_rpm_resource_driver:invalidate_ss "
                          "(resource: \"%s\") (handle: 0x%08x)",
                          resource->definition->name, resource );
        handle->wrote_sleep = 0;
      }
    }
  }

  /* A msg_id of 0 is returned when the request posted is to the Sleep or NAS
     sets OR if the request is "KVP short-circuited" by the RPM driver */
  if ( msg_id )
  {
    if ( transaction )
    {
      /* We use transaction->data.u to track the last msg_id posted in this
         transaction and barrier() on this value at end */
      if ( transaction->data.u == 0 )
      { /* first request in the transaction */
        npa_add_transaction_extension( transaction, 
                                       &npa_rpm_transaction_extension );  
      }
      /* Update last msg_id */
      transaction->data.u = msg_id;

      /* Request is only posted; mark resource forked */
      npa_mark_resource_forked( resource );
    }
    else if ( npa_request_has_attribute( client,
                                         NPA_REQUEST_FIRE_AND_FORGET ) )
    {
      /* Do this only if the current sync method is barrier(). Prevents
       * context switches out of sleep when fire-and-forget requests are 
       * issued from within the scheduler LPR
       */
      if ( npa_rpm_sync == rpm_barrier )
      {
        /* Post a custom event with 'msg_id' as data. In the callback, barrier()
         * on this msg_id. The idea is to provide a fail-safe for the unlikely
         * case that nobody on the system calls rpm_barrier() after this point.
         */
        npa_post_custom_event_nodups( npa_rpm_deferred_barrier_event,
                                      (void *)msg_id,
                                      &npa_rpm_deferred_barrier_action );
      }
    }
    else
    {
      /* synchronous request */
      ULOG_RT_PRINTF_1( npa.log, "npa_rpm_resource_driver:barrier "
                        "(msg_id: 0x%08x)", msg_id );
      npa_rpm_sync( msg_id );
    }
  }

  return state;
}

/* All RPM remoted resources have their driver functions set to the below
 * routine at init. This function handles CLIENT_INITIALIZE and then switches
 * the resource over to the regular driver - the routine above.
 */
static npa_resource_state
npa_rpm_resource_driver_initial( npa_resource      *resource,
                                 npa_client_handle  client,
                                 npa_resource_state state )
{
  /* Should only be invoked with this client type */
  CORE_VERIFY( client->type == NPA_CLIENT_INITIALIZE );
  
  ULOG_RT_PRINTF_3( npa.log, "npa_rpm_resource_driver:initial "
                    "(resource: \"%s\") (handle: 0x%08x) (no_init: %d)",
                    resource->definition->name, resource,
                    npa_resource_has_attribute( resource, NPA_RESOURCE_REMOTE_NO_INIT ) );

  /* This driver is shared between the protocol node and remote resources */
  if ( 0 == strncmp( resource->definition->name, RPM_PROTOCOL_RESOURCE_NAME,
                     NPA_MAX_STR_LEN + 1 ) )
  {
    npa_rpm_init();
  }
  else
  {
    npa_rpm_create_client_context( resource );

    /* Switch the resource over to the regular driver */
    resource->node->driver_fcn = npa_rpm_resource_driver;

    /* If not NO_INIT, call the regular driver function with initial_state */
    if ( !npa_resource_has_attribute( resource, NPA_RESOURCE_REMOTE_NO_INIT ) )
    {
      /* Mangle active_state such that the driver does not short-circuit the
       * initial request. active_state will be set right upon return from this
       * function.
       */
      if ( !npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
      {
        resource->active_state = ~state;
      }
      state = npa_rpm_resource_driver( resource, client, state );
    }
  }
  
  return state;
}

static void 
npa_rpm_end_transaction( npa_transaction_handle transaction,
                         npa_transaction_extension *extension )
{
  unsigned int msg_id = transaction->data.u;
  
  if ( transaction->fire_and_forget )
  {
    /* Post a custom event with 'msg_id' as data. In the callback, barrier()
     * on this msg_id. The idea is to provide a fail-safe for the unlikely
     * case that nobody on the system calls rpm_barrier() after this point.
     */
    npa_post_custom_event_nodups( npa_rpm_deferred_barrier_event,
                                  (void *)msg_id,
                                  &npa_rpm_deferred_barrier_action );
  }
  else
  {
    /* Synchronous transaction; barrier */
    ULOG_RT_PRINTF_1( npa.log, "npa_rpm_end_transaction:barrier "
                      "(msg_id: 0x%08x)", msg_id );
    npa_rpm_sync( msg_id );
  }

  /* Reset last msg id */
  transaction->data.u = 0;
}

static
npa_resource_state npa_rpm_polling_mode_toggle( npa_resource      *resource,
                                                npa_client        *client,
                                                npa_resource_state state )
{
  /* We don't bother with an NPA_CLIENT_INITIALIZE section because we
   * pass in 0 as initial value in npa_define_node and the logic below
   * handles the case.
   */
  npa_rpm_sync = ( state == 0 ? rpm_barrier : rpm_churn_queue );
  return state;
}

/* The RPM polling mode toggle resource */
static npa_resource_definition npa_rpm_polling_mode_resource[] =
{
  {
    "/protocol/rpm/polling_mode",
    "toggle",
    1,
    &npa_identity_plugin,
    NPA_RESOURCE_DEFAULT,
  }
};

static npa_node_dependency npa_rpm_polling_mode_deps[] = 
{
  { RPM_PROTOCOL_RESOURCE_NAME, NPA_NO_CLIENT }
};

/* The RPM polling mode toggle node; lockless, because requests to this node
   must only be issued from sleep's context */
static npa_node_definition npa_rpm_polling_mode_node = 
{ 
  "/protocol/rpm/polling_mode",
  npa_rpm_polling_mode_toggle, 
  NPA_NODE_NO_LOCK,
  NULL,
  NPA_ARRAY(npa_rpm_polling_mode_deps),
  NPA_ARRAY(npa_rpm_polling_mode_resource)
};

/* The RPM protocol resource */
static npa_resource_definition npa_rpm_resource[] =
{
  {
    RPM_PROTOCOL_RESOURCE_NAME,
    "na",
    0,
    &npa_no_client_plugin,
    NPA_RESOURCE_DEFAULT,
    NULL,
    npa_rpm_query_function
  }
};

/* The RPM protocol node */
static npa_node_definition npa_rpm_node = 
{ 
  RPM_PROTOCOL_RESOURCE_NAME,
  npa_rpm_resource_driver_initial, 
  NPA_NODE_NO_LOCK,
  NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(npa_rpm_resource)
};

void npa_rpm_available_cb( void         *context,
                           unsigned int  event_type, 
                           void         *data,       
                           unsigned int  data_size )
{
  npa_resource_state dummy = 0;
  npa_define_node( &npa_rpm_node, &dummy, NULL );
  npa_define_node( &npa_rpm_polling_mode_node, &dummy, NULL );
} 

#ifdef __cplusplus
}
#endif
