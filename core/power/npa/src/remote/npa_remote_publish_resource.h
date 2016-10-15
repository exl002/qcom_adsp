/*==========================================================================
@file npa_remote_publish_resource.h

NPA Remote API to publish one or multiple resources to a different address space

Copyright (c) 2010-2012 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.adsp/2.2/power/npa/src/remote/npa_remote_publish_resource.h#1 $
============================================================================*/
#ifndef NPA_REMOTE_PUBLISH_RESOURCE_H
#define NPA_REMOTE_PUBLISH_RESOURCE_H

#include "npa_resource.h"
#include "npa_remote_resource.h"

typedef struct npa_remote_published_resource_link
{
  struct npa_remote_published_resource_link *next, *prev;
  npa_resource_definition                   *definition;
  npa_remote_domain_id                       remote_domain;
} npa_remote_published_resource_link;

extern npa_remote_published_resource_link *npa_remote_published_resource_list;

#endif /* NPA_REMOTE_PUBLISH_RESOURCE_H */
