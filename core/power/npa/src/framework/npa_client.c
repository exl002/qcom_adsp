/*============================================================================
@file npa_client.c

Client functions in NPA

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.adsp/2.2/power/npa/src/framework/npa_client.c#2 $
============================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
#include "npa_scheduler_internal.h"
#endif /* NPA_SCHEDULED_REQUESTS_SUPPORTED */

#ifdef __cplusplus
extern "C" {
#endif


/**
 * <!-- npa_issue_sync_request_unconditional -->
 *
 * @brief An unconditional version of the regular issue request function.
 *
 * Used to ensure that a client's first request is passed through to
 * the resource, without being checked for redundancy. This is important
 * in cases where the first request is a 0, because client structures are
 * initialized to 0  on creation and this can cause the real 0 request
 * to be incorrectly short-circuited.
 *
 * @param client : Handle to client to apply the request.
 * @param new_request : Ignored, a value of 1 will be sent to the client request. 
 */
static void npa_issue_sync_request_unconditional( npa_client_handle client,
                                                  int new_request )
{
  /* Issue the request as new */
  npa_issue_sync_request( client, 1 );
  
#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
  /* Reset the issue_request function pointer to the conditional version */
  if ( client->request_ptr &&
       client->issue_request != npa_issue_sync_request_unconditional )
  { /* If this client is scheduled... */
    client->request_ptr->issue_request = npa_issue_sync_request;
  }
  else
#endif /* NPA_SCHEDULED_REQUESTS_SUPPORTED */
  {
    client->issue_request = npa_issue_sync_request;
  }
}


/**
 * <!-- npa_enqueue_async_request_unconditional -->
 *
 * @brief An unconditional version of the regular issue request function.
 *
 * Used to ensure that a client's first request is passed through to
 * the resource, without being checked for redundancy. This is important
 * in cases where the first request is a 0, because client structures are
 * initialized to 0  on creation and this can cause the real 0 request
 * to be incorrectly short-circuited.
 *
 * @param client : Handle to client to apply the request.
 * @param new_request : Ignored, a value of 1 will be sent to the client request. 
 */
static void npa_enqueue_async_request_unconditional( npa_client_handle client,
                                                     int new_request )
{
  /* Enqueue the request as new */
  npa_enqueue_async_request( client, 1 );
  
#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
  /* Reset the issue_request function pointer to the conditional version */
  if ( client->request_ptr &&
       client->issue_request != npa_enqueue_async_request_unconditional )
  { /* If this client is scheduled... */
    client->request_ptr->issue_request = npa_enqueue_async_request;
  }
  else
#endif /* NPA_SCHEDULED_REQUESTS_SUPPORTED */
  {
    client->issue_request = npa_enqueue_async_request;
  }
}


/**
 * <!-- npa_new_client -->
 *
 * @brief Allocate a new client on the named resource.
 *
 * Called by the public API client create functions npa_create_sync_client_ex 
 * and npa_create_async_client_cb_ex.
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created
 * @param client_type : The work model that this client will use for work 
                        requests. npa_client_type enum values like
                        NPA_CLIENT_REQUIRED 
 * @param client_value : resource-specific value argument
 * @param client_ref : resource-specific pointer argument
 *
 * @return Handle to new client, NULL if client could not be created
 */
static npa_client_handle npa_new_client( const char       *resource_name,
                                         const char       *client_name, 
                                         npa_client_type   client_type,
                                         unsigned int      client_value,
                                         void             *client_ref,
                                         void (*request_fcn)( 
                                         npa_client_handle client,
                                         int  new_request ) )
{
  npa_link      *link;
  npa_resource  *resource;
  npa_client    *client;

  CORE_VERIFY_PTR( client_name );
  CORE_VERIFY( strlen( client_name ) <= NPA_MAX_STR_LEN );

  link = npa_find_resource_link( resource_name );

  if ( !npa_is_link_valid( link ) )
  {
    /* TODO: need to create a reference copy of the resource_name here */
    ULOG_RT_PRINTF_2( npa.log, 
                      "\tFAILED npa_new_client \"%s\": resource \"%s\" does not exist", 
                      client_name, 
                      resource_name );
    return NULL; 
  }

  resource      = link->resource;
  resource_name = link->name;

  if ( (resource->active_plugin->supported_clients & client_type) != 
                                                     (unsigned) client_type )
  { 
    ULOG_RT_PRINTF_3( npa.log, 
                      "\tFAILED npa_new_client \"%s\": resource %s does not support client type %s", 
                      client_name, 
                      resource_name, 
                      npa_client_type_name(client_type));
    return NULL;
  }

  client = (npa_client *)Core_PoolAlloc( &npa.client_pool );
  CORE_VERIFY_PTR( client );

  memset( client, 0, sizeof( npa_client ) );

  client->name          = client_name;
  client->resource_name = resource_name;
  client->resource      = resource;
  client->type          = client_type;
  client->log_handle    = resource->log_handle;
  client->issue_request = request_fcn;

  if ( ( npa_enqueue_async_request_unconditional == request_fcn ) ||
       ( NPA_CLIENT_VECTOR & client_type ) )
  {
    client->continuation = npa_new_continuation(NULL, NULL);
    if ( NULL == client->continuation )
    {
      Core_PoolFree( &npa.client_pool, client );
      return NULL;
    }
  }

  Core_MutexLock( resource->node_lock );

  /* Add client to list */
  if ( npa_resource_has_attribute( resource, 
                                   NPA_RESOURCE_SINGLE_CLIENT ) &&
       resource->clients != NULL )
  {
    /* iterate through the client list and check if there is already a client 
     * of type "client_type" */
    npa_client_handle _client = resource->clients;
    do
    {
      if ( _client->type == client_type )
      {
        ULOG_RT_PRINTF_3( npa.log, 
                          "\tFAILED npa_new_client \"%s\": "
                          "resource \"%s\" only supports SINGLE client of each type "
                          "and already has one of type %s",
                          client_name, resource_name,
                          npa_client_type_name(client_type) );
        goto client_error;
      }
      _client = _client->next;
    } while ( _client );
  }

  /* Call client create function if defined */
  if ( NULL != resource->active_plugin->create_client_ex_fcn )
  {
    unsigned int code = 
      resource->active_plugin->create_client_ex_fcn( client,
                                                     client_value,
                                                     client_ref );

    if ( 0 != code )
    {
      ULOG_RT_PRINTF_3( npa.log, 
                        "\tFAILED npa_new_client \"%s\": "
                        "resource \"%s\" failed client create "
                        "(error_code: %d)",
                        client_name, 
                        resource_name,
                        code );
      
      goto client_error;
    } 
  }
  else if ( NULL != resource->active_plugin->create_client_fcn )
  {
    /* Should I verify extended args are 0/NULL?
     * Perhaps log a warning message? */
    client->resource->definition->plugin->create_client_fcn( client );
  }
  
  /* add client to client list */
  client->next = resource->clients;
  if ( NULL != resource->clients ) 
  {
    resource->clients->prev = client;
  }
  resource->clients = client;

  Core_MutexUnlock( resource->node_lock );
  return client;

  /* Error handling path - free up everything and return NULL */
client_error:
  Core_MutexUnlock( resource->node_lock );
  npa_destroy_continuation( client->continuation );
  Core_PoolFree( &npa.client_pool, client );
  return NULL;
}


/**
 * <!-- npa_client_type_name -->
 *
 * @brief Return a human readable ascii string type for the client type.
 *
 * @todo Move to non-static area
 *
 * @param client_type : The npa_client_type value to decode.
 *
 * @return A string pointer for the ascii type string. 
 */
const char *npa_client_type_name( npa_client_type client_type )
{ 
  switch (client_type) 
  {
  case NPA_CLIENT_RESERVED1:    return "NPA_CLIENT_RESERVED1";
  case NPA_CLIENT_RESERVED2:    return "NPA_CLIENT_RESERVED2";
  case NPA_CLIENT_CUSTOM1:      return "NPA_CLIENT_CUSTOM1";
  case NPA_CLIENT_CUSTOM2:      return "NPA_CLIENT_CUSTOM2";
  case NPA_CLIENT_CUSTOM3:      return "NPA_CLIENT_CUSTOM3";
  case NPA_CLIENT_CUSTOM4:      return "NPA_CLIENT_CUSTOM4";
  case NPA_CLIENT_REQUIRED:     return "NPA_CLIENT_REQUIRED";
  case NPA_CLIENT_ISOCHRONOUS:  return "NPA_CLIENT_ISOCHRONOUS";
  case NPA_CLIENT_IMPULSE:      return "NPA_CLIENT_IMPULSE";
  case NPA_CLIENT_LIMIT_MAX:    return "NPA_CLIENT_LIMIT_MAX";
  case NPA_CLIENT_VECTOR:       return "NPA_CLIENT_VECTOR";
  case NPA_CLIENT_SUPPRESSIBLE: return "NPA_CLIENT_SUPPRESSIBLE";
  case NPA_CLIENT_SUPPRESSIBLE_VECTOR:
    return "NPA_CLIENT_SUPPRESSIBLE_VECTOR";
  default:                      return "unknown client type";
  }
}


/** 
 * <!-- npa_create_sync_client_ex -->
 *
 * @brief Creates a synchronous client of a resource
 *
 * Create a synchronous client of the given resource with the given
 * work model.  The new client will not be created with have an
 * initial request. Work requests made via syncronous clients will be
 * fully processed before returning.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * There are two resource-optional arguments, client_value (an
 * unsigned int) and client_ref( a void *). The meaning of these
 * arguments, if any, is defined by the resource. If unnecessary, 0 &
 * NULL can be passed as these arguments.  Note that the macro
 * npa_create_sync_client can be used for this case.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created
 * @param client_type : The work model that this client will use for 
 *                      work requests
 * @param client_value : Resource-specific value argument
 * @param client_ref : Resource-specific pointer argument
 *
 * @return Handle to new client, NULL if client could not be created
 */
npa_client_handle npa_create_sync_client_ex( const char       *resource_name,
                                             const char       *client_name, 
                                             npa_client_type   client_type,
                                             unsigned int      client_value,
                                             void             *client_ref )

{
  npa_client *client;

  ULOG_RT_PRINTF_3( npa.log,
                    "npa_create_sync_client (resource: \"%s\") "
                    "(client: \"%s\") (client_type: %s)",
                    resource_name,
                    client_name, 
                    npa_client_type_name(client_type) );

  client = npa_new_client( resource_name, 
                           client_name, 
                           client_type,
                           client_value,
                           client_ref,                            
                           npa_issue_sync_request_unconditional );

  if ( NULL != client )
  {
    ULOG_RT_PRINTF_4( npa.log,
                      "\tend npa_create_sync_client "
                      "(handle: 0x%08x) "
                      "(resource: \"%s\") "
                      "(client: \"%s\") "
                      "(client_type: %s) ",
                      client, 
                      client->resource_name,
                      client->name, 
                      npa_client_type_name(client_type) );
  }
  else
  {
    ULOG_RT_PRINTF_3( npa.log,
                      "\tFAILED npa_create_sync_client (resource: \"%s\") "
                      "(client: \"%s\") (client_type: %s)",
                      resource_name,
                      client_name, 
                      npa_client_type_name(client_type) );
  }

  return client;
}


/** 
 * <!-- npa_create_async_client_cb_ex -->
 *
 * @brief Creates an asynchronous client of a resource
 *
 * Create an asynchronous client of the given resource with the given
 * work model.  Requests made via an asynchronous client will be
 * processed asynchronously. The new client will not be created with an
 * initial request.
 *
 * If the named resource does not exist or it exists but doesn't
 * support the requested client_type, a NULL handle will be
 * returned. Callers are expected to check for NULL handles.
 *  
 * A client must never attempt to use a NULL handle to update a
 * resource.
 *
 * There are two resource-optional arguments, client_value (an
 * unsigned int) and client_ref( a void *). The meaning of these
 * arguments, if any, is defined by the resource. If unnecessary, 0 &
 * NULL can be passed as these arguments.  Note that the macro
 * npa_create_sync_client can be used for this case.
 *
 * The client_name must be < 64 characters in length.
 *
 * The client_name string is not copied.  Any string pointer passed to
 * the framework must be immutable. It is hoped that future versions
 * will relax this restriction.  
 *
 * If non-NULL, the npa_callback will be triggered when the
 * asynchronous request has completed.  when invoked, the arguments
 * will be (context, 0, client_handle, sizeof(client_handle) );
 *
 * Restrictions: 
 *
 * In general, the same client can not have multiple simultaneous
 * requests issued.  It is the responsibility of the user to make sure
 * the previous request has completed before issuing a new request via
 * the same client.
 * 
 * Due to current limitations in DALSYS, async client requests can not
 * be issued while INTLOCK'd.1
 *
 * It is hoped that both these restrictions will be removed in future
 * versions.
 *
 * @param resource_name : Name of the resource that is to be used
 * @param client_name : Name of the client that's being created 
 * @param client_type : The work model that this client will use for
 *                      work requests
 * @param callback : Callback to be invoked when work
 *                   request is complete
 * @param context : Context to be passed back in callback invocation
 * @param client_value : resource-specific value argument
 * @param client_ref : resource-specific pointer argument
 *
 * @return Handle to new client, NULL if client could not be created
 */
npa_client_handle npa_create_async_client_cb_ex( const char      *resource_name,
                                                 const char      *client_name, 
                                                 npa_client_type  client_type,
                                                 npa_callback     callback,
                                                 void            *context,
                                                 unsigned int     client_value,
                                                 void             *client_ref )

{
  npa_client *client;

  if ( NULL == npa.async_request_handle )
  {
    ULOG_RT_PRINTF_3( npa.log,
                      "FAILED npa_create_async_client (resource: \"%s\") "
                      "(client: \"%s\") (client_type: %s):"
                      "NPA target configuration does not support async clients",
                      resource_name,
                      client_name, 
                      npa_client_type_name(client_type) );
    
    return NULL;
  }

  ULOG_RT_PRINTF_3( npa.log,
                    "npa_create_async_client (resource: \"%s\") "
                    "(client: \"%s\") (client_type: %s)",
                    resource_name,
                    client_name, 
                    npa_client_type_name(client_type) );

  client = npa_new_client( resource_name, 
                           client_name, 
                           client_type,
                           client_value,
                           client_ref,
                           npa_enqueue_async_request_unconditional );

  if ( NULL != client )
  {    
    npa_continuation *continuation = client->continuation;

    CORE_DAL_VERIFY( DALSYS_EventCreate( DALSYS_EVENT_ATTR_WORKLOOP_EVENT, 
                                         &continuation->data.client.enqueue_event,
                                         NULL ) );
    
    /* TODO: Need sync object?  Can ignore? */
    CORE_DAL_VERIFY( DALSYS_AddEventToWorkLoop( npa.async_request_handle,
                                                npa_issue_async_request,
                                                client,
                                                continuation->data.client.enqueue_event,
                                                NULL ) );
    
    /* Set up callback */
    continuation->callback.callback = callback;
    continuation->callback.context  = context;

    ULOG_RT_PRINTF_4( npa.log,
                      "\tend npa_create_async_client "
                      "(handle: 0x%08x) "
                      "(resource: \"%s\") "
                      "(client: \"%s\") "
                      "(client_type: %s) ",
                      client, 
                      client->resource_name,
                      client->name, 
                      npa_client_type_name(client_type) );
  }
  else
  {
    ULOG_RT_PRINTF_3( npa.log,
                      "\tFAILED npa_create_async_client (resource: \"%s\") "
                      "(client: \"%s\") (client_type: %s)",
                      resource_name,
                      client_name, 
                      npa_client_type_name(client_type) );
  }
  return client;
}


/** 
 * <!-- npa_issue_limit_max_request -->
 *
 * @brief Issue the LIMIT_MAX  request
 *
 * If there are multiple concurrent max requests, the minimum of the
 * requests will be used.
 *
 * The client must be of LIMIT_MAX work model.
 *
 * Note: The behavior of concurrent LIMIT_MAX and FIXED request or
 * LIMIT_MAX < LIMIT_MIN requests is undefined
 *
 * @param client : Handle to client being used
 * @param max : Maximum state of resource
 */
void npa_issue_limit_max_request( npa_client_handle   client,
                                  npa_resource_state  max )
{
  npa_resource       *resource;
  npa_resource_state  active_max;

  CORE_VERIFY_PTR( client );

  resource = client->resource;
  CORE_VERIFY_PTR( resource );

  active_max = resource->definition->max;

  NPA_PENDING_REQUEST(client).state = max;
  
  Core_MutexLock( resource->node_lock );
  if ( ( max != 0 ) && ( max < resource->active_max ) )
  {
    resource->active_max = max;
  }
  /* TODO: Reinvestigate this logic when requests are more than double
   * buffered */
  else if ( NPA_ACTIVE_REQUEST(client).state == resource->active_max )
  {
    /* There are ways to optimize this path, but it would involve
     * making the limit max requests be even more of a special case.
     * Not bothering with that unless this becomes a performance
     * problem */
    npa_client *max_client = resource->clients;

    while ( NULL != max_client )
    {
      if ( NPA_CLIENT_LIMIT_MAX == max_client->type )
      {
        npa_resource_state req = (max_client == client) ? 
          max : NPA_ACTIVE_REQUEST(max_client).state;

        if ( 0 != req && req < active_max )
        {
          active_max = req;
        }
      }
      max_client = max_client->next;
    }
    resource->active_max = active_max;
  }
  Core_MutexUnlock( resource->node_lock );

  ULOG_RT_PRINTF_5( client->log_handle,
                    "npa_issue_limit_max_request "
                    "(handle: 0x%08x) "
                    "(client: \"%s\") "
                    "(max: %d) "
                    "(resource: \"%s\") "
                    "(active_max: %d)",
                    client,
                    client->name, 
                    max, 
                    resource->definition->name,
                    active_max );

  client->issue_request( client, 
                         ( NPA_PENDING_REQUEST(client).state != 
                           NPA_ACTIVE_REQUEST(client).state ) );

  return;
}


/** 
 * <!-- npa_issue_scalar_request -->
 *
 * @brief Issue a scalar client work request
 *
 * Resource state should be updated to account for new request. 
 *  
 * @param client : Handle to client being used
 * @param state : Requested resource state
 */
void npa_issue_scalar_request( npa_client_handle   client, 
                               npa_resource_state  state )
{
  char *client_type = "custom";
  CORE_VERIFY_PTR( client );

  if ( NPA_CLIENT_REQUIRED == client->type )
  {
    client_type = "required";
  }
  else if ( NPA_CLIENT_SUPPRESSIBLE == client->type )
  {
    client_type = "suppressible";
  }

  ULOG_RT_PRINTF_5( client->log_handle,
                    "npa_issue_%s_request "
                    "(handle: 0x%08x) "
                    "(client: \"%s\") "
                    "(request: %u) "
                    "(resource: \"%s\")",
                    client_type,
                    client,
                    client->name, 
                    state, 
                    client->resource_name );

  NPA_PENDING_REQUEST(client).state = state;
  client->issue_request( client, NPA_ACTIVE_REQUEST(client).state != state );

  return;
}


/** 
 * <!-- npa_modify_required_request -->
 *
 * @brief Modify the existing REQUIRED client work request
 *
 * This is intended to be used in a feedback loop, so the client can
 * request incrementally more (or less) resource 
 *
 * Resource state should be updated to account for new request. 
 *  
 * The client must be of REQUIRED work model
 * 
 * @param client : Handle to client being used
 * @param delta : Modifier to resource state
*/
void npa_modify_required_request( npa_client_handle         client, 
                                  npa_resource_state_delta  delta )
{
  CORE_VERIFY_PTR( client );
  
  ULOG_RT_PRINTF_4( client->log_handle,
                    "npa_modify_request "
                    "(handle: 0x%08x) "
                    "(client: \"%s\") "
                    "(delta: %d) "
                    "(resource: \"%s\")",
                    client,
                    client->name, 
                    delta, 
                    client->resource_name );

  NPA_PENDING_REQUEST(client).state = NPA_ACTIVE_REQUEST(client).state + delta;
  client->issue_request( client, delta != 0 );

  return;
}


/** 
 * <!-- npa_issue_impulse_request -->
 *
 * @brief Issue an IMPULSE work request 
 *
 * The client must be of IMPULSE work model.
 * The resource defines the impulse response
 * Resource state should be updated to account for new request
 * 
 * @param client : Handle to client being used
 */
void npa_issue_impulse_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );


  ULOG_RT_PRINTF_3( client->log_handle,
                    "npa_issue_impulse_request "
                    "(handle: 0x%08x)"
                    "(client: \"%s\") "
                    "(resource: \"%s\")",
                    client,
                    client->name, 
                    client->resource_name );

  client->issue_request( client, 1 );
}


/** 
 * <!-- npa_issue_isoc_request -->
 *
 * @brief Issue an ISOCHRONOUS work request 
 *
 * The client must be of ISOCHRONOUS work model
 *
 * The client must issue npa_complete_request() when the work is completed
 *  
 * Resource state should be updated to accound for new work
 * Note: The hint may be overriden/ignored by the resource
 *
 * @param client : Handle to client being used
 * @param deadline : The interval (in ms) when the work must be completed
 * @param level_hint : A hint as to what an appropriate level might be. May
 *                     be overridden by resource.
 */
void npa_issue_isoc_request( npa_client_handle   client,
                             npa_resource_time   deadline,
                             npa_resource_state  level_hint )
{
  CORE_UNIMPLEMENTED();
}


/** 
 * <!-- npa_issue_vector_request -->
 *
 * @brief Issue the VECTOR client work request
 *
 * Resource state should be updated to account for new request.
 *
 * The vector pointer must remain valid for the duration of the
 * request, but not beyond.  This includes scheduled requests 
 * where the vector information must remain valid until the 
 * scheduled request is been issued and the callback has 
 * occurred. If the resource needs the data to persist, it is 
 * the repsonsibility of the resource to copy the data. 
 *
 * The interpretation of the vector elements is defined by the
 * resource.
 *  
 * The client must be of VECTOR (either normal or SUPRESSIBLE) work
 * model
 * 
 * @param client : Handle to client being used
 * @param num_elems : Number of elements in the vector
 * @param vector : Array of num_elems
 */
void npa_issue_vector_request( npa_client_handle   client, 
                               unsigned int        num_elems, 
                               npa_resource_state *vector )
{
  char *client_type = "_custom_";

  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( vector );
  CORE_LOG_VERIFY_PTR( client->continuation,
                       ULOG_RT_PRINTF_3(npa.log,
                              "npa_issue_vector_request() on wrong client type"
                              " (handle:0x%08x)"
                              " (type:\"%s\")"
                              " (continuation: 0x%08x)",
                              client,
                              npa_client_type_name(client->type),
                              client->continuation));

  if ( NPA_CLIENT_VECTOR == client->type )
  {
    client_type = "_";
  }
  else if ( NPA_CLIENT_SUPPRESSIBLE_VECTOR == client->type )
  {
    client_type = "_suppressible_";
  }

  if ( ULogFront_RealTimeMultipartMsgBegin( client->log_handle ) )
  {
	unsigned int log_vector_len = MIN( num_elems, npa_vector_logging_limit );

    ULOG_RT_PRINTF_5( client->log_handle,
                      "npa_issue%svector_request "
                      "(handle: 0x%08x) "
                      "(client: \"%s\") "
                      "(resource: \"%s\") "
                      "(num_elems: %d) "
                      "(vector: %m)",
                      client_type,
                      client, 
                      client->name, 
                      client->resource_name,
                      num_elems );
    ULogFront_RealTimeVector( client->log_handle,
                              "0x%08x ",
                              ( unsigned short )sizeof(npa_resource_state),
                              ( unsigned short )log_vector_len, 
                              vector );
    ULogFront_RealTimeMultipartMsgEnd( client->log_handle );
  }

  NPA_PENDING_REQUEST(client).state = num_elems;
  NPA_PENDING_REQUEST(client).pointer.vector = vector;

  /* Save the stack pointer so we can check the client's version for
   * validity */
  client->continuation->data.client.vector = vector; 

  client->issue_request( client, 1 );
}


/**
 * <!-- npa_issue_internal_request -->
 *
 * @brief  Issue the INTERNAL client work request
 *
 * Note: The semantics of an internal client request are
 * resource/plugin specific, there's no general API for setting the
 * work request fields - it's the responsibility of the user to set the
 * internal client state appropriately outside of the framework.  This
 * function is used to issue the request after the internal fields have
 * been established.
 *
 * The client must be of INTERNAL work model. Clients with internal
 * client types are created/destroyed via the standard client api.
 *
 * @param client : handle to client being used
 */
void npa_issue_internal_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );

  ULOG_RT_PRINTF_3( client->log_handle,
                    "npa_issue_internal_request "
                    "(handle: 0x%08x)"
                    "(client: \"%s\") "
                    "(resource: \"%s\")",
                    client, 
                    client->name, 
                    client->resource_name );

  client->issue_request( client, 1 );
}


/** 
 * <!-- npa_complete_request -->
 *
 * @brief Complete the work request
 *
 *  This is used to notify the resource that the work request is
 *  complete and the client no longer needs the resource
 *
 *  Any request state that has been learned will persist
 *  
 *  Resource state should be updated to account for completed work
 *
 *  @param client : Handle to client being used
 */
void npa_complete_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );
  
  ULOG_RT_PRINTF_3( client->log_handle,
                    "npa_complete_request "
                    "(handle: 0x%08x) "
                    "(client: \"%s\") "
                    "(resource: \"%s\") ",
                    client, 
                    client->name, 
                    client->resource_name );

  npa_terminate_request( client );
}


/** 
 * <!-- npa_cancel_request -->
 *
 * @brief  Cancel the work request
 *
 * This can be used by any work model to terminate the work request.
 * This internally invokes npa_complete_request and then clears any
 * adapted state
 *
 * This function is used when future client requests will be
 * qualitatively different than previous requests made via this client
 *
 * Resource state should be updated to account for terminated work
 *
 * @param client : Handle to client being used
 */
void npa_cancel_request( npa_client_handle client )
{
  CORE_VERIFY_PTR( client );

  ULOG_RT_PRINTF_3( client->log_handle,
                    "npa_cancel_request "
                    "(handle: 0x%08x) "
                    "(client: \"%s\") "
                    "(resource: \"%s\")",
                    client,
                    client->name, 
                    client->resource_name );

  npa_terminate_request( client );

  /* Execute the plugin cancel function (if defined)
   *
   * Note: This function is executed in a locked context.  The lock is
   * outside the check for the plugin's cancel function being non-NULL
   * to avoid a race condition on changing plugins */
  Core_MutexLock( client->resource->node_lock );
  if ( NULL != client->resource->active_plugin->cancel_client_fcn )
  {
    client->resource->active_plugin->cancel_client_fcn( client );
  }
  Core_MutexUnlock( client->resource->node_lock );
}


/** 
 * <!-- npa_destroy_client -->
 *
 * @brief Cancel the client request (if any) and free the client structure. 
 *
 * The client handle can not be used to issue requests after being destroyed
 *
 * @param client : Handle to client being destroyed
 *
 */
void npa_destroy_client( npa_client_handle client )
{
  npa_resource *resource;
  const char *client_name, *resource_name;

  if( NULL == client ) {return;}

  resource = client->resource;
  CORE_VERIFY_PTR( resource );

  client_name   = client->name;
  resource_name = client->resource_name;

  ULOG_RT_PRINTF_3( npa.log,
                    "npa_destroy_client "
                    "(handle: 0x%08x) "
                    "(client: \"%s\") "
                    "(resource: \"%s\")",
                    client,
                    client_name,
                    resource_name );

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
  npa_destroy_schedulable_client(client);
#endif  /* NPA_SCHEDULED_REQUESTS_SUPPORTED */

  /* Handle this request synchronously */
  client->issue_request = npa_issue_sync_request;
  
  /* Be sure to complete any pending work request */
  npa_complete_request( client );

  /* Remove client from inactive list */
  Core_MutexLock( resource->node_lock );
  if ( NULL != client->next ) 
  {
    client->next->prev = client->prev;
  }

  if ( NULL == client->prev ) 
  {
    CORE_VERIFY( resource->clients == client );
    /* first element of the list */
    resource->clients = client->next;
  } 
  else 
  {
    client->prev->next = client->next;
  }

  if ( NULL != client->continuation )
  {
    npa_continuation *continuation = client->continuation;
    DALSYSEventHandle event = continuation->data.client.enqueue_event;

    if ( NULL != event )
    {
      CORE_DAL_VERIFY( DALSYS_DeleteEventFromWorkLoop( npa.async_request_handle, 
                                                       event ) );
      CORE_DAL_VERIFY( DALSYS_DestroyObject( event ) );
    }
    npa_destroy_continuation( continuation );
  }

  if ( NULL != resource->active_plugin->destroy_client_fcn )
  {
    resource->active_plugin->destroy_client_fcn( client );
  }
  Core_MutexUnlock( resource->node_lock );

  ULOG_RT_PRINTF_3( npa.log,
                    "\tend npa_destroy_client "
                    "(handle: 0x%08x) (client: \"%s\") (resource: \"%s\")",
                    client, client_name, resource_name );

  Core_PoolFree( &npa.client_pool, client );
}


/** 
 * <!-- npa_set_request_attribute -->
 *
 * @brief Associates the given attribute to the next request from this client.
 *  
 * Request attributes only apply to the first issue_request after
 * npa_set_request_attribute 
 */
void npa_set_request_attribute( npa_client_handle    client,
                                npa_request_attribute attr )
{
  /* Request attributes may be set on async clients, but if client code does
     not wait until the previous request is complete before issuing a second
     request on the same handle, incorrect behavior may result */

  CORE_VERIFY_PTR( client );
  client->request_attr |= attr;
}


#ifdef __cplusplus
}
#endif
