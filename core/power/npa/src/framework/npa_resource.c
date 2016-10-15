/*============================================================================
@file npa_resource.c

Routines that constitute NPA's resource request pipeline

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.adsp/2.2/power/npa/src/framework/npa_resource.c#2 $
============================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * <!-- _npa_update_resource_state -->
 */
static NPA_INLINE void _npa_update_resource_state( npa_resource      *resource,
                                                   npa_resource_state new_state )
{
  npa_resource_state       old_state    = resource->active_state;
  npa_resource_state_delta old_headroom = resource->active_headroom;

  resource->active_state    = new_state;
  resource->active_headroom = resource->request_state - resource->active_max;

  if ( ( resource->events.event_header ) &&
       ( resource->events.event_header->reg_count > 0 ) &&
       ( (resource->active_state    != old_state) || 
         (resource->active_headroom != old_headroom) ) )
  {
    npa_event_action *action;
    struct npa_event_trigger *trigger;
    CORE_DAL_VERIFY( Core_WorkQueueAlloc(npa.npa_event_queue_handle, (void **)&action) );

    action->function = npa_event_action_trigger;
    trigger = &action->state.trigger;

    trigger->resource     = resource;
    trigger->old_state    = old_state;
    trigger->new_state    = resource->active_state;
    trigger->old_headroom = old_headroom;
    trigger->new_headroom = resource->active_headroom;

    CORE_DAL_VERIFY( Core_WorkQueuePut(npa.npa_event_queue_handle, action) );
  }
}


/**
 * <!-- npa_update_resource_state -->
 */ 
void npa_update_resource_state( npa_resource      *resource,
                                npa_resource_state new_state )
{
  _npa_update_resource_state( resource, new_state );
}


/**
 * <!-- npa_invoke_update_function -->
 *
 * @brief Call the resource's active plugin's update function.
 *
 * @param resource : The resource to update.
 * @param client : The client causing the update. 
 */
static NPA_INLINE void npa_invoke_update_function( npa_resource *resource,
                                                   npa_client   *client )
{
  resource->request_state = 
    resource->active_plugin->update_fcn( resource, client );
}


/**
 * <!-- npa_invoke_driver_function -->
 */
static NPA_INLINE void npa_invoke_driver_function( npa_resource *resource,
                                                   npa_client   *client )
{
  npa_resource_state new_state = 
    ( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) ) ?
    resource->request_state : MIN( resource->request_state, 
                                   resource->active_max );

  if ( npa_resource_has_attribute( resource, NPA_RESOURCE_DRIVER_UNCONDITIONAL ) ||
       ( new_state != resource->active_state ) ||
       ( resource->required_state != NPA_RESOURCE_REQUIRED_REQUEST(resource) ) )
  {
    uint64 start_time = CoreTimetick_Get64();
    new_state = resource->_internal->active_driver_fcn( resource, 
                                                        client, 
                                                        new_state );
    /* update stats */
    npa_calc_duration( start_time, CoreTimetick_Get64(),
                       &resource->_internal->stats.driver_duration );
    /* update the required requests aggregation for scalars */                       
    resource->required_state = NPA_RESOURCE_REQUIRED_REQUEST(resource);
  }

  /* Do this anyway - active max may have changed without
   * hitting driver function/changing resource state */
  _npa_update_resource_state( resource, new_state );
}


/**
 * <!-- _npa_request_serviced -->
 */
static NPA_INLINE void _npa_request_serviced( npa_client *client )
{
  npa_resource *resource = client->resource;

  /* Clear request attributes - they only apply to the first issue_request
     after npa_set_request_attribute */
  client->request_attr = NPA_REQUEST_DEFAULT;

  /* If a vector client or vector resource and we've enabled vector logging */
  if ( ( client->type & NPA_CLIENT_VECTOR || resource->state_vector ) &&
       npa_vector_logging_limit != 0 )
  {
    if ( ULogFront_RealTimeMultipartMsgBegin( client->log_handle ) )
    {
      unsigned int log_vector_len = MIN( resource->definition->max,
                                         npa_vector_logging_limit );
      ULOG_RT_PRINTF_2( client->log_handle,
                        "\trequest complete "
                        "(handle: 0x%08x) (sequence: 0x%08x) "
                        "(resource state vector: %m)",
                        client, client->sequence );
      ULogFront_RealTimeVector( client->log_handle,
                                "0x%08x ",
                                ( unsigned short )sizeof(npa_resource_state),
                                ( unsigned short )log_vector_len,
                                resource->state_vector );
      ULogFront_RealTimeMultipartMsgEnd( client->log_handle );
    }
  }
  else
  {
    ULOG_RT_PRINTF_4( client->log_handle,
                      "\trequest complete "
                      "(handle: 0x%08x) (sequence: 0x%08x) "
                      "(request state:%d) (active state:%d)",
                      client, client->sequence,
                      resource->request_state, resource->active_state );
  }

  /* Invoke Async Client Completion Event */
  if ( NULL != client->continuation && 
       NULL != client->continuation->callback.callback )
  {
    npa_trigger_callback_event( &client->continuation->callback,
                                0,
                                client, 
                                sizeof( npa_client_handle ) );
  }
}


/**
 * <!-- npa_request_serviced -->
 */
void npa_request_serviced( npa_client_handle client )
{
  _npa_request_serviced( client );
}


/**
 * <!-- npa_resource_has_client -->
 *
 * @brief Check if the given client is a client of the given resource
 *
 * @param resource : The resource in question
 * @param client : The client to check
 * 
 * @return : 1 if client is a client of the resource, else 0
 */
unsigned int npa_resource_has_client( npa_resource     *resource,
                                      npa_client_handle client )
{
  npa_client_handle c;
  
  CORE_VERIFY_PTR( resource );
  CORE_VERIFY_PTR( client );

  for ( c = resource->clients; c != NULL; c = c->next )
  {
    if ( client == c ) break;
  }

  return c ? 1 : 0;
}


/**
 * <!-- npa_process_request -->
 */
static NPA_INLINE void npa_process_request( npa_client_handle client)
{
  npa_transaction_handle transaction;
  npa_resource *resource;
  
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( resource = client->resource );
  
  transaction = npa_get_current_transaction();
  if ( transaction )
  {
    npa_request_enter_transaction( transaction, client );
  }
  else
  {
    Core_MutexLock( resource->node_lock );
  }

  /* Set the current_client so it's available for use and debugging. */
  resource->current_client = client;

  /* Aggregate */
  npa_invoke_update_function( resource, client );

  /* Apply */
  npa_invoke_driver_function( resource, client );

  /* Increment sequence number */
  client->sequence = (resource->sequence += NPA_SEQUENCE_INCREMENT);
  
  /* Swap PENDING and ACTIVE work requests */
  client->index ^= 1;
  
  /* If resource has not updated active request vector, clear it out so we
     don't retain reference to invalid data */
  if ( NPA_CLIENT_VECTOR & client->type &&
       NULL != client->continuation &&
       NPA_ACTIVE_REQUEST(client).pointer.vector == client->continuation->data.client.vector )
  {
    NPA_ACTIVE_REQUEST(client).pointer.vector = NULL; 
  }

  /* Clear the current_client. */
  resource->current_client = NULL;

  if ( transaction )
  {
    /* Remove the resource from the transaction's active resource stack and
     * unlock, if it or one of its dependencies in the current request
     * path is NOT marked forked */
    npa_request_exit_transaction( transaction, client );
  }
  else
  {
    Core_MutexUnlock( resource->node_lock );
    _npa_request_serviced( client );
  }
}


/**
 * <!-- npa_issue_sync_request -->
 */
void npa_issue_sync_request( npa_client_handle client, int new_request )
{ 
  if ( new_request )
  {
    npa_process_request( client );
  }
  else 
  {
    client->sequence |= NPA_REDUNDANT_REQUEST;
    _npa_request_serviced( client );
  }
}


/**
 * <!-- npa_issue_async_request -->
 *
 * @brief This function runs in the async client service workloop 
 */
DALResult npa_issue_async_request( DALSYSEventHandle event, void *arg )
{
  npa_client_handle client = (npa_client_handle)arg;

  npa_process_request( client );
  
  return DAL_SUCCESS;
}


/**
 * <!-- npa_enqueue_async_request -->
 */
void npa_enqueue_async_request( npa_client_handle client, int new_request )
{
  if ( new_request )
  {
    CORE_DAL_VERIFY( 
         DALSYS_EventCtrl( client->continuation->data.client.enqueue_event,
                           DALSYS_EVENT_CTRL_TRIGGER ) );
  }
  else
  {
    /* Complete synchronously */
    client->sequence |= NPA_REDUNDANT_REQUEST;
    _npa_request_serviced( client );
  }
}


/**
 * <!-- npa_terminate_request -->
 */
void npa_terminate_request( npa_client_handle client )
{
  npa_set_request_attribute( client, NPA_REQUEST_FIRE_AND_FORGET );

  /* TODO: May want to use a complete_request function pointer */
  if ( NPA_CLIENT_LIMIT_MAX == client->type ) 
  {
    npa_issue_limit_max_request( client, 0 );
  }
  else 
  {
    NPA_PENDING_REQUEST(client).state = 0;
    client->issue_request( client, NPA_ACTIVE_REQUEST(client).state != 0 );
  }
}


/**
 * <!-- npa_assign_resource_state -->
 *
 * @brief Assign the resource state directly, bypassing the update and
 * driver functions.  This will trigger events as necessary.
 *
 * This function is intended for use in a compound node when one
 * resource's state change can cause another coupled resource's state
 * to change. This function must be called from within the node's
 * atomic section - which means it can be invoked from within the
 * update or driver function.  It require the npa_resource *, which
 * can be retrived for coupled resources from the current resource
 * by the NPA_RESOURCE macro.
 *
 * @param resource : Resource whose state is changing
 * @param state : New state of the resource
 */
void npa_assign_resource_state( npa_resource       *resource,
                                npa_resource_state  new_state )
{
  CORE_VERIFY_PTR( resource );
  CORE_VERIFY( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) ?
               1 : ( new_state <= resource->active_max ) );

  ULOG_RT_PRINTF_3( resource->log_handle, 
                    "npa_assign_resource_state "
                    "(handle: 0x%08x) "
                    "(resource: \"%s\") "
                    "(active state: %d)", 
                    resource,
                    resource->definition->name,
                    new_state );

  Core_MutexLock( resource->node_lock );
  npa_update_resource_state( resource, new_state );
  Core_MutexUnlock( resource->node_lock );
}


/**
 * <!-- npa_request_has_request -->
 *
 * @brief Returns true if the current request  has the given attribute set.
 * 
 * Attributes may be set on client handles and this is cached in the resource
 * structure for the duration of the request.
 *
 * @param client : Handle to the client to check
 * @param attr : The attribute to test for
 *
 * @return TRUE if the attribute is present
 */
unsigned int npa_request_has_attribute( npa_client_handle     client,
                                        npa_request_attribute attr )
{
  CORE_VERIFY_PTR( client );
  return client->request_attr & attr;
}


/**
 * <!-- npa_get_request_attributes -->
 *
 * @brief  Returns a bitset representing all  request attributes set on this client.
 *
 * @param client : Handle of the npa_client to read
 */
unsigned int npa_get_request_attributes( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );
  return client->request_attr;
}


/**
 * <!-- npa_pass_request_attributes -->
 *
 * @brief Add request attributes of the 'current' client to the 'dependency' client.
 *
 * Does not unset any attributes set previously on the 'dependency' client.
 *
 * @param current : Handle of a npa_client whose attributes will be applied
 *                  to the other.
 * @param dependency : Handle of a npa_client whose attributes will be updated. 
 *
 * @return The dependency client handle passed in
 */
npa_client_handle npa_pass_request_attributes( npa_client_handle current,
                                               npa_client_handle dependency )
{
  CORE_VERIFY_PTR( current ); CORE_VERIFY_PTR( dependency );
  dependency->request_attr |= current->request_attr;
  return dependency;
}


/**
 * <!-- npa_resource_lock -->
 *
 * @brief  Lock the resource.
 *
 * If the resource is forked, it will be joined before this function returns.
 * Note that the resource shares its lock with other resources in the node.
 * So this will, in effect, lock the node.
 *
 *  @param resource : Handle to the resource
 */
void npa_resource_lock( npa_resource *resource )
{
  CORE_VERIFY_PTR( resource );

  Core_MutexLock( resource->node_lock );
  ULOG_RT_PRINTF_2( npa.log, "npa_resource_lock "
                    "(resource: \"%s\") (sequence: 0x%08x)",
                    resource->definition->name, resource->sequence );
}


/**
 * <!-- npa_resource_lock -->
 *
 * @brief  Lock the resource.
 *
 * If the resource is forked, it will be joined before this function returns.
 * Note that the resource shares its lock with other resources in the node.
 * So this will, in effect, lock the node.
 *
 * @param resource : Handle to the resource
 */
void npa_resource_unlock( npa_resource *resource )
{
  CORE_VERIFY_PTR( resource );
  Core_MutexUnlock( resource->node_lock );

  ULOG_RT_PRINTF_1( npa.log, "npa_resource_unlock "
                    "(resource: \"%s\")",
                    resource->definition->name );
}


#ifdef __cplusplus
}
#endif
