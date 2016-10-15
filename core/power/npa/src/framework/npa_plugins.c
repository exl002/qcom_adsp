/*===========================================================================
@file npa_plugins.c

NPA standard plugin library

Copyright © 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.adsp/2.2/power/npa/src/framework/npa_plugins.c#1 $
============================================================================*/

#include "npa_internal.h"
#include "npa_inlines.h"

#ifdef WINSIM
#include "npa_offline.h"
#endif /* WINSIM */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IMPORTANT: If you add a new update function to this file and that function
 * supports SUPPRESSIBLE requests, make sure that you add the function to
 * the npa_update_fcn_list in npa_graph.c
 */

static NPA_INLINE
unsigned int _internal_index( npa_client_handle client )
{
  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:     return NPA_REQUIRED_INDEX;
  case NPA_CLIENT_SUPPRESSIBLE: return NPA_SUPPRESSIBLE_INDEX;
  default: CORE_VERIFY( 0 );
  }
  return 0;
}

npa_resource_state npa_min_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client)
{
  npa_resource_state   request, active_min, pending_min;
  unsigned int         index;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE: 
    index = _internal_index( client );
    pending_min = active_min = resource->internal_state[index];

    NPA_REMAP_MAX_REQUEST( resource, client );
    request = NPA_PENDING_REQUEST(client).state;

    if ( ( request != 0 ) &&
         ( ( pending_min == 0 ) || request < pending_min ) )
    {
      pending_min = request;
    } 
    else if ( active_min == NPA_ACTIVE_REQUEST(client).state )
    {
      npa_client_handle active = resource->clients;
      pending_min = request;
    
      /* Scan the pending list for a new minimum */
      while ( NULL != active )
      {
        if ( active != client && client->type == active->type ) 
        {
          request = NPA_ACTIVE_REQUEST(active).state;
          if ( ( request != 0 ) &&
               ( ( pending_min == 0 ) || request < pending_min ) )
          {
            pending_min = request;
          }
        }
        active = active->next;
      }
    }
    resource->internal_state[index] = pending_min;
  default:
    break;
  }

  if ( 0 == resource->internal_state[NPA_SUPPRESSIBLE_INDEX] )
  {
    return resource->internal_state[NPA_REQUIRED_INDEX];
  }
  else if ( 0 == resource->internal_state[NPA_REQUIRED_INDEX] )
  {
    return resource->internal_state[NPA_SUPPRESSIBLE_INDEX];
  }
  else 
  {
    return MIN( resource->internal_state[NPA_REQUIRED_INDEX],
                resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
  }
}

npa_resource_state npa_max_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client )
{
  npa_resource_state request, active_max, pending_max;
  unsigned int index;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE:
    index = _internal_index( client );
    pending_max = active_max = resource->internal_state[index];
  
    NPA_REMAP_MAX_REQUEST( resource, client );
    request = NPA_PENDING_REQUEST(client).state;
    if ( request > pending_max )
    {
      pending_max = request;
    } 
    else if ( active_max == NPA_ACTIVE_REQUEST(client).state )
    {
      npa_client_handle active = resource->clients;
      pending_max = NPA_PENDING_REQUEST(client).state;
    
      /* Scan the pending list for a new maximum */
      while ( NULL != active )
      {
        if ( client != active && client->type == active->type )
        {
          request = NPA_ACTIVE_REQUEST(active).state;
          if ( request > pending_max )
          {
            pending_max = request;
          }
        }
        active = active->next;
      }
    }
    resource->internal_state[index] = pending_max;
    break;
  default:
    break;
  }

  return MAX( resource->internal_state[NPA_REQUIRED_INDEX],
              resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
}

npa_resource_state npa_sum_update_fcn( npa_resource      *resource,
                                       npa_client_handle  client )
{
  unsigned int index;

  if ( NULL != client )
  {
    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      index = _internal_index( client );
      NPA_REMAP_MAX_REQUEST( resource, client );
      resource->internal_state[index] -= NPA_ACTIVE_REQUEST(client).state;
      resource->internal_state[index] += NPA_PENDING_REQUEST(client).state;
      break;
    default:
      break;
    }
  }
  return resource->internal_state[NPA_REQUIRED_INDEX] +
         resource->internal_state[NPA_SUPPRESSIBLE_INDEX];
}

npa_resource_state npa_binary_update_fcn( npa_resource      *resource,
                                          npa_client_handle  client )
{
  unsigned int index;

  if ( NULL != client )
  {
    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      index = _internal_index( client );
      resource->internal_state[index+2] -= (NPA_ACTIVE_REQUEST(client).state  == 0) ? 0 : 1;
      resource->internal_state[index+2] += (NPA_PENDING_REQUEST(client).state == 0) ? 0 : 1;
      resource->internal_state[index] = 
        ( resource->internal_state[index+2] ) ? resource->active_max : 0;
      break;
    default:
      break;
    }
  }
  
  return MAX( resource->internal_state[NPA_REQUIRED_INDEX],
              resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
}

npa_resource_state npa_or_update_fcn( npa_resource       *resource,
                                      npa_client_handle   client)
{
  if ( NULL != client )
  {
    npa_resource_state request = NPA_PENDING_REQUEST(client).state;
    npa_client_handle  active  = resource->clients;
    unsigned int       index   = _internal_index( client );

    switch (client->type) 
    {
    case NPA_CLIENT_REQUIRED:
    case NPA_CLIENT_SUPPRESSIBLE: 
      while ( NULL != active )
      {
        if ( ( active != client ) && ( client->type == active->type ) )
        {
          request |= NPA_ACTIVE_REQUEST(active).state;
        }
        active = active->next;
      }
      break;
    default:
      break;
    }
    resource->internal_state[index] = request;
  }
  
  return ( resource->internal_state[NPA_REQUIRED_INDEX] |
           resource->internal_state[NPA_SUPPRESSIBLE_INDEX] );
}

npa_resource_state npa_identity_update_fcn( npa_resource      *resource,
                                            npa_client_handle  client )
{
  if ( npa_resource_has_attribute( resource, NPA_RESOURCE_VECTOR_STATE ) )
  {
    unsigned int vector_size;

    CORE_VERIFY( 
         NPA_PENDING_REQUEST(client).state <= resource->definition->max );
    resource->active_max = NPA_PENDING_REQUEST(client).state;
    
    resource->request_state = NPA_PENDING_REQUEST(client).state;
    vector_size = resource->request_state * sizeof( npa_resource_state );
      
    memcpy( resource->state_vector, 
            NPA_PENDING_REQUEST(client).pointer.vector,
            vector_size );

    resource->required_state_vector = \
      resource->suppressible_state_vector = resource->state_vector;
      
    return resource->request_state;
  }
  else
  {
    unsigned int index = _internal_index( client );
    resource->internal_state[index] = NPA_PENDING_REQUEST(client).state;
    return NPA_PENDING_REQUEST(client).state;
  }
}

npa_resource_state npa_always_on_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client )
{
  resource->internal_state[NPA_REQUIRED_INDEX] = \
    resource->internal_state[NPA_SUPPRESSIBLE_INDEX] = resource->active_max;
  return resource->active_max;
}

npa_resource_state npa_impulse_update_fcn( npa_resource      *resource,
                                           npa_client_handle  client )
{
  return resource->request_state;
}

npa_resource_state npa_no_client_update_fcn( npa_resource      *resource,
                                             npa_client_handle  client )
{
  /* This function should never be invoked */
  CORE_VERIFY(0);
  return 0;
}

/*
  NPA plugin library
*/

const npa_resource_plugin npa_binary_plugin =
{
  npa_binary_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_min_plugin = 
{
  npa_min_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_max_plugin = 
{
  npa_max_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_sum_plugin = 
{
  npa_sum_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_identity_plugin = 
{
  npa_identity_update_fcn,
  0xffffffff,                   /* All client types supported */
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_always_on_plugin = 
{
  npa_always_on_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};


const npa_resource_plugin npa_impulse_plugin = 
{
  npa_impulse_update_fcn,
  /* Supported client types */
  NPA_CLIENT_IMPULSE,
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

const npa_resource_plugin npa_or_plugin = 
{
  npa_or_update_fcn,
  /* Supported client types */
  NPA_CLIENT_INTERNAL | NPA_CLIENT_REQUIRED | NPA_CLIENT_SUPPRESSIBLE | 
  NPA_CLIENT_LIMIT_MAX | NPA_CLIENT_IMPULSE, 
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};

/* Resource supports no clients - used for markers */
static void npa_no_client_create_fcn ( npa_client *client )
{
  client->resource = NULL; /* Signal that this client can't be created */
  client->resource_data = 0;    /* Set error code */
}

const npa_resource_plugin npa_no_client_plugin =
{
  npa_no_client_update_fcn,
  /* Supported client types */
  0,
  npa_no_client_create_fcn,     /* Create client function */
  NULL                          /* Destroy client function */
};

#ifdef __cplusplus
}
#endif
