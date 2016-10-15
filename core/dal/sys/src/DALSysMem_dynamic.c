/*==============================================================================
@file  DALSYSMem_dynamic.c

DALSYS dynamic memory implementation

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "DALSysInt.h"
#include <qurt.h>

/*==============================================================================
          Structures
 ==============================================================================*/

/*==============================================================================
                    DEFINITIONS AND DECLARATIONS
==============================================================================*/
#ifndef DAL_USE_64BIT_PHYS_ADDR

#define QMEM_ATTR_SET_PHYSADDR( attr, paddr ) \
   qurt_mem_region_attr_set_physaddr(  attr, (qurt_paddr_t)paddr )
#define QMEM_ATTR_GET_PHYSADDR( attr, ppaddr ) \
   qurt_mem_region_attr_get_physaddr(  attr, (qurt_paddr_t *)ppaddr )
#define QMEM_MAP_STATIC_QUERY( vaddr, paddr, len, cache, perm )\
   qurt_mem_map_static_query( vaddr, (qurt_paddr_t)paddr, len, cache, perm )
#define QMEM_MAPPING_CREATE( vaddr, paddr, len, cache, perm ) \
   qurt_mapping_create( vaddr, (qurt_paddr_t)paddr, len, cache, perm )
#define QMEM_MAPPING_REMOVE( vaddr, paddr, len ) \
   qurt_mapping_remove( vaddr, (qurt_paddr_t)paddr, len )

#else

#define QMEM_ATTR_SET_PHYSADDR( attr, paddr ) \
   qurt_mem_region_attr_set_physaddr_64(  attr, paddr )
#define QMEM_ATTR_GET_PHYSADDR( attr, ppaddr ) \
   qurt_mem_region_attr_get_physaddr_64(  attr, ppaddr )
#define QMEM_MAP_STATIC_QUERY( vaddr, paddr, len, cache, perm )\
   qurt_mem_map_static_query_64( vaddr, paddr, len, cache, perm )
#define QMEM_MAPPING_CREATE( vaddr, paddr, len, cache, perm ) \
   qurt_mapping_create_64( vaddr, paddr, len, cache, perm )
#define QMEM_MAPPING_REMOVE( vaddr, paddr, len ) \
   qurt_mapping_remove_64( vaddr, paddr, len )

#endif

/*============================================================================n
      Constants and Macros
==============================================================================*/

/*==============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
static qurt_perm_t convert_dal_to_qurt_perm_attribs(uint32 dwAttribs)
{
   // DAL currently does not support executable permission
   switch (dwAttribs & DALSYS_MEM_PROPS_EXT_PERM_MASK)
   {
      case DALSYS_MEM_PROPS_EXT_PERM_RD:
         return QURT_PERM_READ;
      case DALSYS_MEM_PROPS_EXT_PERM_FULL:
         return QURT_PERM_READ | QURT_PERM_WRITE;
      default:
      case DALSYS_MEM_PROPS_EXT_PERM_NONE:
         return 0;
   }
}

static uint32 convert_dal_to_qurt_cache_attribs (uint32 dwAttribs)
{
    if (dwAttribs & DALSYS_MEM_PROPS_EXT)
    {
      switch (dwAttribs & DALSYS_MEM_PROPS_EXT_ATTRIBS_MASK)
      {

         case DALSYS_MEM_PROPS_EXT_OUTER_INNER_UNCACHED:
         case (DALSYS_MEM_PROPS_EXT_NORM_MEM | DALSYS_MEM_PROPS_EXT_OUTER_MEM_NONCACHED):
            return QURT_MEM_CACHE_NONE_SHARED;  

	     case DALSYS_MEM_PROPS_EXT_STRONGLY_ORDERED:
         case DALSYS_MEM_PROPS_EXT_SHARED_DEVICE:
         case DALSYS_MEM_PROPS_EXT_NON_SHARED_DEVICE:       
           return QURT_MEM_CACHE_NONE; //device-type?
 
         case DALSYS_MEM_PROPS_EXT_OUTER_INNER_WB_NO_ALLOC:
         case DALSYS_MEM_PROPS_EXT_OUTER_INNER_WB_ALLOC:
         case (DALSYS_MEM_PROPS_EXT_NORM_MEM | DALSYS_MEM_PROPS_EXT_OUTER_MEM_WB_ALLOC):
         case (DALSYS_MEM_PROPS_EXT_NORM_MEM | DALSYS_MEM_PROPS_EXT_OUTER_MEM_WB_NO_ALLOC):
           return QURT_MEM_CACHE_WRITEBACK;   

         case DALSYS_MEM_PROPS_EXT_OUTER_INNER_WT_NO_ALLOC:
         case (DALSYS_MEM_PROPS_EXT_NORM_MEM | DALSYS_MEM_PROPS_EXT_OUTER_MEM_WT_NO_ALLOC):
           return QURT_MEM_CACHE_WRITETHROUGH;

         default:
           break;  
       }
    }
    return QURT_MEM_CACHE_NONE;
}

static DALResult create_qurt_mem_region( qurt_mem_pool_t mem_pool, 
        qurt_mem_cache_type_t cache_attribs, qurt_mem_mapping_t mapping,
        qurt_mem_region_t *p_qmem_region, DALSYSMemInfoEx *pMemInfo )
{
   qurt_mem_region_attr_t mem_attribs;
   qurt_mem_region_attr_init (&mem_attribs); 
   qurt_mem_region_attr_set_cache_mode(&mem_attribs, cache_attribs);
   qurt_mem_region_attr_set_mapping (&mem_attribs, mapping);

   if ( DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemInfo->PhysicalAddr )
   {
      QMEM_ATTR_SET_PHYSADDR(&mem_attribs, pMemInfo->PhysicalAddr);
   }

   if(QURT_EOK == qurt_mem_region_create(p_qmem_region, 
      pMemInfo->dwLen, mem_pool, &mem_attribs))
   {
      // we need to read addresses and size attributes because memory region
      // addresses and size can be modified to match alignment
      // and size of memory mappings
      qurt_mem_region_attr_get(*(p_qmem_region), &mem_attribs );
      qurt_mem_region_attr_get_virtaddr(&mem_attribs, 
         (qurt_addr_t *)&(pMemInfo->VirtualAddr)); 
      qurt_mem_region_attr_get_size(&mem_attribs, 
         (qurt_size_t *)&(pMemInfo->dwLen));

      // if memory region with virt-to-phys mapping is created,
      // physical address is updated from the memory region, otherwise
      // physical address is rounded-down according to the region size
      if (QURT_MEM_MAPPING_NONE != mapping)
      {
         QMEM_ATTR_GET_PHYSADDR(&mem_attribs, &(pMemInfo->PhysicalAddr)); 
      }
      return DAL_SUCCESS;
   }

   DALSYS_LOG_WARNING_EVENT("Failed- pMemInfo:0x%x mem_pool:0x%x cache_attribs:0x%x",
      pMemInfo, mem_pool, cache_attribs);
   return DAL_ERROR;
}


DALResult DALSYS_MemAllocQurtRegion( DALSYSMemInfoEx *pMemInfo,
      qurt_mem_region_t *p_qmem_region )
{
   static qurt_mem_pool_t hwio_phys_pool = NULL;
   DALResult result;
   qurt_mem_cache_type_t qmem_cache;

   if (DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemInfo->VirtualAddr)
   {
      // no support for memory region creation with a 
      // specified virtual address
      DALSYS_LOG_ERROR_EVENT("Cannot specify virtual address- pMemInfo:0x%x", 
            pMemInfo);
      return DAL_ERROR;
   }

   // allocate from HWIO pool
   if (NULL == hwio_phys_pool)
   {
      // need to attach to HWIO pool if this is the first allocation
      if (QURT_EOK != qurt_mem_pool_attach ("HWIO_POOL", &hwio_phys_pool))
      {
         return DAL_ERROR;
      }
   }

   qmem_cache = convert_dal_to_qurt_cache_attribs(pMemInfo->dwProps);

   if (DALSYS_MEM_ADDR_NOT_SPECIFIED != pMemInfo->PhysicalAddr)
   {
      int qurt_ret;
      qurt_addr_t qmemVirtualAddr;
      qurt_perm_t qmem_perms = convert_dal_to_qurt_perm_attribs(pMemInfo->dwProps);

      // attempt to find memory from statically defined memory regions
      qurt_ret = QMEM_MAP_STATIC_QUERY(&qmemVirtualAddr,
                 (pMemInfo->PhysicalAddr), pMemInfo->dwLen, 
                 qmem_cache, qmem_perms);

      switch(qurt_ret)
      {
         case QURT_EOK: // memory region found
            *(p_qmem_region) = 0;  //no underlying qmem_region, memory mapped statically
            pMemInfo->VirtualAddr = qmemVirtualAddr;
            result = DAL_SUCCESS; //have correct vaddr returned in second argument
            break;
         case QURT_EMEM: // memory region found but attributes don't match
            DALSYS_LOG_WARNING_EVENT("memory region found but attributes not match- pMemInfo:0x%x",
               pMemInfo );
            result = DAL_ERROR;
            break;
         default: // memory region is not found - create it
            result = create_qurt_mem_region( hwio_phys_pool, qmem_cache, 
               QURT_MEM_MAPPING_PHYS_CONTIGUOUS, p_qmem_region, pMemInfo );
            break;
      }

      if (DAL_SUCCESS != result)
      {
         // unable to find or allocate from HWIO pool
         // allocate virtual memory and map to specified physical memory region
         // below pool parameter is basically ignored by QuRT as no 
         // physical memory is mapped, however it must still be a valid pool handle
         result = create_qurt_mem_region( hwio_phys_pool, qmem_cache, 
            QURT_MEM_MAPPING_NONE, p_qmem_region, pMemInfo );

         if (DAL_SUCCESS == result)
         {
            qurt_ret = QMEM_MAPPING_CREATE( pMemInfo->VirtualAddr, 
                  pMemInfo->PhysicalAddr, pMemInfo->dwLen, qmem_cache, 
                  qmem_perms);
            if (QURT_EOK != qurt_ret)
            {
               qurt_mem_region_delete(*p_qmem_region);
               result = DAL_ERROR;
            }
         }
      }
   }
   else
   {
      // physical address is not specified
      // allocate from DEFAULT pool
      result = create_qurt_mem_region( qurt_mem_default_pool, qmem_cache,
         QURT_MEM_MAPPING_VIRTUAL, p_qmem_region, pMemInfo );
   }
   return result;
}

void DALSYS_MemFreeQurtRegion( DALSYSMemInfoEx const *pMemInfo, 
      qurt_mem_region_t qmem_region )
{
   qurt_mem_region_attr_t attr;

   if (NULL == qmem_region) return;

   if (QURT_EOK != qurt_mem_region_attr_get(qmem_region, &attr)) return;

   qurt_mem_region_delete(qmem_region);

   if (QURT_MEM_MAPPING_NONE == attr.mapping_type)
   {
      QMEM_MAPPING_REMOVE( pMemInfo->VirtualAddr,
         (qurt_addr_t)pMemInfo->PhysicalAddr, pMemInfo->dwLen);
   }
}
