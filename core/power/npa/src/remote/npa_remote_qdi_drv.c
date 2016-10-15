/*==============================================================================
@file npa_remote_qdi_drv.c

NPA QDI Remote driver functions

Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.adsp/2.2/power/npa/src/remote/npa_remote_qdi_drv.c#1 $
==============================================================================*/
#include <string.h>
#include <qurt_qdi_driver.h>
#include "CoreVerify.h"
#include "ULogFront.h"
#include "npa.h"
#include "npa_internal.h"
#include "npa_remote_publish_resource.h"
#include "npa_remote_qdi_protocol.h"

#ifdef PD_IMAGE_NAME
/* TODO: change this to a QuRT macro */
#define NPA_QURT_PD_IMAGE_NAME_MAX  64
static char npa_qurt_pd_image_name[NPA_QURT_PD_IMAGE_NAME_MAX];
#endif /* PD_IMAGE_NAME*/


int npa_opener_invocation(int client_handle, qurt_qdi_obj_t *ptr, int method,
                          qurt_qdi_arg_t a1, qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3, qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7, qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
   switch (method)
   {
   case QDI_OPEN:
     {
       return qurt_qdi_new_handle_from_obj_t(client_handle, ptr);
     }
   case NPA_QDI_GET_PUBLISHED_RESOURCE_COUNT:
     {
       npa_remote_domain_id domain_id = (npa_remote_domain_id)a1.num;
       unsigned int count = 0;
       npa_remote_published_resource_link *published_link = npa_remote_published_resource_list;
       if(NULL != a2.ptr)
       {
         while(NULL != published_link)
         {
           if(domain_id & published_link->remote_domain) count++;
           published_link = published_link->next;
         }
         *((unsigned int *)a2.ptr) = count;
         return 0;
       }
       return -1;
     }
   case NPA_QDI_GET_PUBLISHED_RESOURCE_DEFINITION:
     {
       npa_remote_domain_id domain_id = (npa_remote_domain_id)a1.num;
       unsigned int i = 0, count = (unsigned int)a2.num;
       npa_remote_resource_definition *definition = (npa_remote_resource_definition *)a3.ptr;
       npa_remote_published_resource_link *published_link = npa_remote_published_resource_list;

       if(0 == count){ return 0; }

       while(NULL != published_link)
       {
         if(domain_id & published_link->remote_domain)
         {
           if(i >= count)
           {
             /* Resources published to this domain are more than expected */
             ULOG_RT_PRINTF_1(npa.log, 
                              "ERROR: npa_opener_invocation() finds more "
                              "resources published to Domain %u than "
                              "expected.", domain_id);
             return -1;
           }
           strlcpy(definition[i].remote_resource_name, 
                   published_link->definition->name, NPA_MAX_STR_LEN + 1);
           strlcpy((char *)definition[i].units, published_link->definition->units, 
                   NPA_QDI_REMOTE_MAX_RESOURCE_UNITS_LEN + 1);
           definition[i].max = published_link->definition->max;
           definition[i].attributes = published_link->definition->attributes;
           i++;
         }
         published_link = published_link->next;
       }
       if(i != count)
       {
         ULOG_RT_PRINTF_1(npa.log, 
                          "ERROR: npa_opener_invocation() cannot find all the "
                          "resources published to Domain %u ", domain_id);
         return -1;
       }
       
       return 0;
     }
   case NPA_QDI_CREATE_SYNC_CLIENT_EX:
     {
       npa_client_handle handle;
       handle = npa_create_sync_client_ex(a1.ptr, 
                                          a2.ptr, 
                                          (npa_client_type)a3.num,
                                          (unsigned int)a4.num,
                                           a5.ptr);
       *((npa_client_handle *)a6.ptr) = handle;
       if(NULL != handle)
       {
         return 0;
       }
       return -1;
     }
   case NPA_QDI_DESTROY_CLIENT:
     {
       npa_destroy_client(a1.ptr);
       return 0;
     }
   case NPA_QDI_ISSUE_SCALAR_REQUEST:
     {
       npa_issue_scalar_request(a1.ptr, (npa_resource_state)a2.num);
       return 0;
     }
   case NPA_QDI_ISSUE_LIMIT_MAX_REQUEST:
     {
       npa_issue_limit_max_request(a1.ptr, (npa_resource_state)a2.num);
       return 0;
     }
   case NPA_QDI_CREATE_QUERY_HANDLE:
     {
       npa_query_handle handle = npa_create_query_handle(a1.ptr);
       *((npa_query_handle *)a2.ptr) = handle;
       if(NULL != handle)
       {
         return 0;
       }
       return -1;
     }
   case NPA_QDI_QUERY:
     {
       return (int) npa_query(a1.ptr, (uint32)a2.num, a3.ptr);
     }
   default:
     {
       return qurt_qdi_method_default(client_handle, ptr, method,
                                      a1, a2, a3, a4, a5, a6, a7, a8, a9);
     }
   }
}

struct npa_opener {
   qurt_qdi_obj_t qdiobj;
};

static const struct npa_opener npa_opener = {
   {
      npa_opener_invocation,
      QDI_REFCNT_PERM,
      0
   }
};

void npa_qdi_init(void)
{
#ifdef PD_IMAGE_NAME
  int len = strlcpy(npa_qurt_pd_image_name,
                    PD_IMAGE_NAME, 
                    NPA_QURT_PD_IMAGE_NAME_MAX);
  CORE_VERIFY(len == strlen(PD_IMAGE_NAME));
#endif  

  CORE_VERIFY(0 == qurt_qdi_register_devname("/dev/npa",
                                             (void *)&npa_opener));
}
