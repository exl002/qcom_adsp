/*============================================================================
@file npa_query.c

Routines to create and destroy NPA query handles and query [for] resources
using said handles.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.adsp/2.2/power/npa/src/framework/npa_query.c#1 $
============================================================================*/

#include "npa_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

npa_query_handle npa_create_query_handle( const char * resource_name )
{
  CORE_VERIFY_PTR( resource_name );

  /* Return the resource handle as the query handle. Nothing is alloc-d */
  return (npa_query_handle)npa_find_resource( resource_name );
}

void npa_destroy_query_handle( npa_query_handle query )
{
  /* Since the query handle returned from npa_create_query_handle is nothing
     but the resource handle, there is nothing to destroy. */
  return;
}

npa_query_status npa_query( npa_query_handle handle, 
                            uint32           id, 
                            npa_query_type  *result )
{
  npa_query_status  status  = NPA_QUERY_UNSUPPORTED_QUERY_ID;
  npa_resource     *resource = (npa_resource *) handle;

  /* Validate resource */
  if ( NULL == resource ) { return NPA_QUERY_UNKNOWN_RESOURCE; }
  if ( NPA_IS_RESOURCE_MARKER(resource) ) { return status; }

  if ( NULL == result ) { return NPA_QUERY_NULL_POINTER; }
  
  /* Give the resource a chance to handle the query */
  if ( NULL != resource->definition->query_fcn )
  {
    status = resource->definition->query_fcn( resource, 
                                              id, 
                                              result );
  }

  /* If the query has not been handled yet, process it */
  if ( NPA_QUERY_UNSUPPORTED_QUERY_ID == status )
  {
    status = NPA_QUERY_SUCCESS;

    switch ( id )
    {
    case NPA_QUERY_CURRENT_STATE:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->active_state;
      break;
    case NPA_QUERY_ACTIVE_MAX:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->active_max;
      break;
    case NPA_QUERY_RESOURCE_MAX:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->definition->max;
      break;
    case NPA_QUERY_RESOURCE_DISABLED:
      result->type = NPA_QUERY_TYPE_STATE;
      result->data.state = resource->sequence & NPA_RESOURCE_DISABLED;
      break;
    case NPA_QUERY_RESOURCE_ATTRIBUTES:
      result->type = NPA_QUERY_TYPE_VALUE;
      result->data.state = resource->definition->attributes;
      break;
    case NPA_QUERY_NODE_ATTRIBUTES:
      result->type = NPA_QUERY_TYPE_VALUE;
      result->data.state = resource->node->attributes;
      break;
    default:
      result->type = NPA_QUERY_TYPE_REFERENCE;
      result->data.reference = NULL;
      status = NPA_QUERY_UNSUPPORTED_QUERY_ID;
      break;
    }
  }

  return status;
}

npa_query_status npa_query_by_name( const char      *resource_name, 
                                    uint32           query_id, 
                                    npa_query_type  *query_result )
{
  return npa_query( (npa_query_handle)npa_find_resource( resource_name ), 
                    query_id, 
                    query_result );
}

npa_query_status npa_query_by_client( npa_client_handle client, 
                                      uint32            query_id, 
                                      npa_query_type  *query_result )
{
  CORE_VERIFY_PTR( client );
  return npa_query( (npa_query_handle)client->resource,
                    query_id, 
                    query_result );
}

npa_query_status npa_query_by_event( npa_event_handle event, 
                                     uint32           query_id, 
                                     npa_query_type  *query_result )
{
  CORE_VERIFY_PTR( event );
  return npa_query( (npa_query_handle)event->resource,
                    query_id, 
                    query_result );
}

npa_resource *npa_query_get_resource( npa_query_handle query_handle )
{
  return (npa_resource *)query_handle;
}

npa_query_status npa_query_resource_available( const char *resource_name )
{
  if ( npa_find_resource( resource_name ) )
  {
    return NPA_QUERY_SUCCESS;
  }
  else
  {
    return NPA_QUERY_UNKNOWN_RESOURCE;
  }
}

#ifdef __cplusplus
}
#endif
