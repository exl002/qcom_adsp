/*==============================================================================
@file npa_remote_qdi_protocol.c

NPA QDI Remote protocol

Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.adsp/2.2/power/npa/src/remote/npa_remote_qdi_protocol.c#1 $
==============================================================================*/
#include <qurt.h>

#include "CoreVerify.h"
#include "CoreHeap.h"
#include "npa.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"
#include "npa_remote_qdi_protocol.h"

#define NPA_QDI_REMOTE_PROTOCOL_NAME "/protocol/qurt/qdi"

typedef struct npa_qdi_remote_resource_data
{
  char name[NPA_MAX_STR_LEN + 1];
  char units[NPA_QDI_REMOTE_MAX_RESOURCE_UNITS_LEN + 1];
} npa_qdi_remote_resource_data;

static int npa_qdi_drv_hdl;

static npa_resource_state 
npa_qdi_remote_update_fcn(npa_resource      *resource,
                          npa_client_handle  client)
{
  int ret;
  npa_resource_state state;

  CORE_VERIFY((NULL != resource) && (NULL != client));
  state  = NPA_PENDING_REQUEST(client).state;

  switch (client->type)
  {
  case NPA_CLIENT_REQUIRED:
  case NPA_CLIENT_SUPPRESSIBLE:
    {
      ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl, 
                                   NPA_QDI_ISSUE_SCALAR_REQUEST, 
                                   client->resource_data, state);
      CORE_VERIFY(0 == ret);
      break;
    }
  case NPA_CLIENT_LIMIT_MAX:
    {
      ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl, 
                                   NPA_QDI_ISSUE_LIMIT_MAX_REQUEST, 
                                   client->resource_data, state);
      CORE_VERIFY(0 == ret);
      state = resource->request_state;
      break;
    }
  default:
    {
      CORE_VERIFY( 0 );
    }
  }

  return state;
}

static unsigned int 
npa_qdi_remote_create_client_ex_fcn(npa_client   *client,
                                    unsigned int  client_value, 
                                    void         *client_ref)
{
  int ret;
  npa_client_handle rmt_clt;

  CORE_VERIFY_PTR(client);
  
  ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl, 
                               NPA_QDI_CREATE_SYNC_CLIENT_EX, 
                               client->resource->definition->name,
                               client->name, 
                               client->type,
                               client_value,
                               client_ref,
                               &rmt_clt);
  if(0 != ret)
  { 
    return NPA_ERROR;
  }
  
  client->resource_data = (npa_user_data)rmt_clt;
  return NPA_SUCCESS;
}

static 
void npa_qdi_remote_destroy_client_fcn(npa_client *client)
{
  int ret;
  CORE_VERIFY_PTR(client);

  ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl, 
                               NPA_QDI_DESTROY_CLIENT, 
                               client->resource_data);
  CORE_VERIFY(0 == ret);
}

const npa_resource_plugin npa_qdi_remote_plugin =
{
  npa_qdi_remote_update_fcn,
  0xffffffff,
  NULL,
  npa_qdi_remote_destroy_client_fcn,
  npa_qdi_remote_create_client_ex_fcn,
  NULL
};

static npa_resource_state 
npa_qdi_remote_driver_fcn(struct npa_resource *resource,
                          npa_client_handle    client,
                          npa_resource_state   state)
{
  CORE_VERIFY((NULL != resource) && (NULL != client));

  if(NPA_CLIENT_INITIALIZE == client->type)
  {
    if(0 == strncmp(NPA_QDI_REMOTE_PROTOCOL_NAME,
                    resource->definition->name,
                    sizeof(NPA_QDI_REMOTE_PROTOCOL_NAME)))
    {
      npa_qdi_drv_hdl = qurt_qdi_open("/dev/npa");
      CORE_VERIFY(0 <= npa_qdi_drv_hdl);
    }
    else
    {
      int ret;
      npa_query_handle handle;
      CORE_VERIFY_PTR(resource->definition);
      ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl, 
                                   NPA_QDI_CREATE_QUERY_HANDLE, 
                                   resource->definition->name,
                                   &handle);
      CORE_VERIFY(0 == ret);
      resource->definition->data = (npa_user_data)handle;
      resource->active_plugin = &npa_qdi_remote_plugin;
    }
  }

  return state;
}

static npa_query_status 
npa_qdi_remote_query_fcn(struct npa_resource *resource, 
                         unsigned int         query_id, 
                         npa_query_type      *query_result)
{
  int ret;

  CORE_VERIFY((NULL != resource) && (NULL != query_result));

  if(NPA_QUERY_REMOTE_RESOURCE_AVAILABLE == query_id)
  {
    /* The QuRT OS boots before all user PDs, so all of its resources are
       always available. */
    npa_remote_resource_available( query_result->data.reference );
    return NPA_QUERY_SUCCESS;
  }

  CORE_VERIFY((NULL != resource->definition) && 
              (NULL != resource->definition->data));
  ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl,
                               NPA_QDI_QUERY,
                               resource->definition->data,
                               query_id,
                               query_result);
  CORE_VERIFY(0 <= ret);
  return ((npa_query_status)ret);
}

static npa_resource_definition npa_qdi_remote_protocol_node_resource[] = 
{ 
  {
    NPA_QDI_REMOTE_PROTOCOL_NAME,
    "na",
    0,
    &npa_no_client_plugin,
    NPA_RESOURCE_DEFAULT,
    NULL,
    npa_qdi_remote_query_fcn
  }
};

static npa_node_definition npa_qdi_remote_protocol_node = 
{ 
  "/node/protocol/qurt/qdi",
  npa_qdi_remote_driver_fcn, 
  NPA_NODE_DEFAULT,
  NULL,
  NPA_EMPTY_ARRAY,
  NPA_ARRAY(npa_qdi_remote_protocol_node_resource)
};

void npa_qdi_remote_define_proxy_resources(void)
{
  int pid;
  unsigned int ret, count, i;
  npa_remote_resource_definition *resources = NULL;
  npa_qdi_remote_resource_data *resource_data = NULL;

  /* The driver handle should be initialized when the protocol node is defined. */
  CORE_VERIFY(0 <= npa_qdi_drv_hdl);
  
  pid = qurt_getpid();
  CORE_VERIFY(0 < pid && 32 >= pid);
  
  /* The assumption is that all kernel resources are already created and
     published. */
  ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl,
                               NPA_QDI_GET_PUBLISHED_RESOURCE_COUNT,
                               pid,
                               &count);
  CORE_VERIFY(0 == ret);
  
  if(count > 0)
  {
    resources = (npa_remote_resource_definition *)
                Core_Malloc(sizeof(npa_remote_resource_definition) * count);
    CORE_VERIFY_PTR(resources);
    resource_data = (npa_qdi_remote_resource_data *)
                    Core_Malloc(sizeof(npa_qdi_remote_resource_data) * count);
    CORE_VERIFY_PTR(resource_data);
    
    for(i = 0; i < count; i++)
    {
      resources[i].local_resource_name = resource_data[i].name;
      resources[i].remote_resource_name = resource_data[i].name;
      resources[i].protocol_type = NPA_QDI_REMOTE_PROTOCOL_NAME;
      resources[i].plugin = &npa_qdi_remote_plugin;
      resources[i].units = resource_data[i].units;
    }
  
    ret = qurt_qdi_handle_invoke(npa_qdi_drv_hdl,
                                 NPA_QDI_GET_PUBLISHED_RESOURCE_DEFINITION,
                                 pid,
                                 count,
                                 resources);
    CORE_VERIFY(0 == ret);
    
    for(i = 0; i < count; i++)
    {
      npa_remote_define_resource_cb(&resources[i], 0, NULL, NULL);
    }
  }
}

void npa_qdi_remote_init(void)
{
  npa_define_node_cb(&npa_qdi_remote_protocol_node, NULL, NULL, NULL);
  npa_qdi_remote_define_proxy_resources();
}

