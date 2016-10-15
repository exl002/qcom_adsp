/*============================================================================
@file npa_remote_publish_resource.c

NPA Remote API to publish one or multiple resources to a different address space

Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.adsp/2.2/power/npa/src/remote/npa_remote_publish_resource.c#2 $
============================================================================*/

#include "ULogFront.h"
#include "npa_internal.h"
#include "npa_inlines.h"
#include "npa_remote_publish_resource.h"

#define npa_remote_published_resource_link_pool  (npa.link_pool)
CORE_STATIC_VERIFY( sizeof( npa_remote_published_resource_link ) <= 
                    sizeof( npa_link ) );

/** List of all published resources */
npa_remote_published_resource_link *npa_remote_published_resource_list = NULL;

npa_remote_published_resource_link *
npa_remote_find_published_resource_link(const char *resource_name)
{
  npa_remote_published_resource_link *link = npa_remote_published_resource_list;
  if (NULL == resource_name) {return NULL;}

  while (NULL != link)
  {
    if (0 == strncmp(resource_name, link->definition->name, NPA_MAX_STR_LEN + 1))
    {
      return link;
    }
    link = link->next;
  }
  return NULL;
}


/**
 * <!-- npa_remote_publish_resource_cb -->
 */
void npa_remote_publish_resource_cb( void         *context,
                                     unsigned int  event_type,
                                     void         *data,
                                     unsigned int  data_size )
{
  npa_link *resource_link;
  npa_remote_published_resource_link *published_link, *new_link = 
    (npa_remote_published_resource_link *)context;
  const char *resource_name = (const char *)data;

  CORE_VERIFY((NULL != new_link) && (0 != new_link->remote_domain) &&
              (NULL != resource_name));

  /* Use the graph lock. Split if lock contention is a concern */
  npa_lock_graph();

  resource_link = npa_find_resource_link(resource_name);
  CORE_VERIFY(npa_is_link_valid(resource_link));
  
  published_link = npa_remote_find_published_resource_link(resource_name);
  if(NULL == published_link)
  {
    /* This resource has never been published before. */
    published_link = new_link;
    published_link->definition = resource_link->resource->definition;
    published_link->next = npa_remote_published_resource_list;
    published_link->prev = NULL;

    if( NULL != npa_remote_published_resource_list )
    {
      npa_remote_published_resource_list->prev = published_link;
    }
    npa_remote_published_resource_list = published_link;
  }
  else
  {
    /* The resource was published before, so perform a bitwise OR on the domain
       flags, and release the new link buffer */
    published_link->remote_domain |= new_link->remote_domain;
    Core_PoolFree(&npa_remote_published_resource_link_pool, new_link);
  }

  npa_unlock_graph();

  ULOG_RT_PRINTF_1( npa.log, 
                    "\tpublish complete (resource name: \"%s\")",
                    resource_name);
}


/**
 * <!-- npa_remote_publish_resource -->
 *
 * @brief Publish a resource to one or multiple domains
 *
 * This function may defer publishing the named resource till
 * the resource is available. If a resource is published for
 * multiple times, it will perform a bitwise OR on the domain
 * flags. NPA Remoting will then create a proxy for the named
 * resource with remote aggregation in the specified remote
 * domain(s).
 *
 * @param remote_domain_id: bit flag(s) to indicate the domain(s)
 * @param resource_name: name of the NPA resource to be published
 *
 * @return 0 on success
 */
int npa_remote_publish_resource( npa_remote_domain_id remote_domain_id,
                                 const char *resource_name )
{
  npa_remote_published_resource_link *published_link;
  
  if ((NULL == resource_name) || (0 == remote_domain_id)) {return -1;}
  
  ULOG_RT_PRINTF_2( npa.log, 
                    "npa_remote_publish_resource (resource name: \"%s\")"
                    "(domain: 0x%08x)", resource_name, remote_domain_id );

  /* Use the graph lock. Split if lock contention is a concern */
  npa_lock_graph();
  published_link = npa_remote_find_published_resource_link( resource_name );
  if(NULL != published_link)
  {  /* The resource was published before, so perform a bitwise OR on the domain
        flags */
    published_link->remote_domain |= remote_domain_id;
    npa_unlock_graph();

    ULOG_RT_PRINTF_1( npa.log, 
                      "\tpublish complete (resource name: \"%s\")",
                      resource_name);
  }
  else
  {
    /* This resource has never been published before. */
    npa_unlock_graph();

    published_link = Core_PoolAlloc( &npa_remote_published_resource_link_pool );
    CORE_VERIFY_PTR(published_link);
    memset(published_link, 0, sizeof(npa_remote_published_resource_link));
    published_link->remote_domain = remote_domain_id;

    npa_resource_available_cb(resource_name, 
                              npa_remote_publish_resource_cb,
                             (void *)published_link);
  }

  return 0; 
}


/**
 * <!-- npa_remote_publish_resources -->
 *
 * @brief Publish an array of resources to one or multiple domains
 *
 * This function may defer publishing the named resources till
 * resources are available. If a resource is published for
 * multiple times, it will perform a bitwise OR on the domain
 * flags. NPA Remoting will then create proxies for the named
 * resources with remote aggregation in the specified remote domain(s).
 *
 * @param remote_domain_id: bit flag(s) to indicate the domain(s)
 * @param num_resources: number of NPA resources to be published
 * @param resources: array of resources to be published
 *
 * @return 0 on success
 */
int npa_remote_publish_resources( npa_remote_domain_id remote_domain_id,
                                  unsigned int num_resources,
                                  const char *resources[] )
{
  int i;

  CORE_VERIFY((0 != num_resources) && (NULL != resources));

  for(i = 0; i < num_resources; i++)
  {
    if(0 > npa_remote_publish_resource(remote_domain_id, resources[i]))
    {
      return -1;
    }
  }

  return 0;
}
