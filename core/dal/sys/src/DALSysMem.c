/*==============================================================================
@file  DALSYSMem.c

DALSYS memory region implementation

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
#include <stdlib.h>
#include <string.h>
#include <qurt.h>
#include "stringl/stringl.h"

/*==============================================================================
          Structures
 ==============================================================================*/
typedef struct _DALSYSMemObj _DALSYSMemObj;
struct _DALSYSMemObj
{
   INHERIT_DALSYS_BASE_OBJ();
   qurt_mem_region_t qmem_region;
   DALSYSMemInfoEx memInfo;
};

/*==============================================================================
                    DEFINITIONS AND DECLARATIONS
==============================================================================*/
extern DALResult DALSYS_MemAllocQurtRegion( DALSYSMemInfoEx *pMemInfo,
      qurt_mem_region_t *p_qmem_region );

extern void DALSYS_MemFreeQurtRegion( DALSYSMemInfoEx const *pMemInfo, 
      qurt_mem_region_t qmem_region );

/*============================================================================n
      Constants and Macros
==============================================================================*/

/*==============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
static _DALSYSMemObj *
AllocAndInitMemObj( DALSYSMemInfoEx *pMemInfo, _DALSYSMemObj *pMemObj,
   qurt_mem_region_t qmem_region )
{
   if (NULL == pMemObj)
   {
      if (DAL_SUCCESS != DALSYS_Malloc (DALSYS_MEM_OBJ_SIZE_BYTES, 
               (void **)&pMemObj))
      {
         // ensure the advertised memory object size is not smaller
         // than the actual size
         DALSYSCMN_COMPILER_ASSERT(
               (sizeof(_DALSYSMemObj) > DALSYS_MEM_OBJ_SIZE_BYTES));
         return NULL;
      }
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
      DALSYSCMN_SET_DYNAMIC_ALLOC_BIT(pMemObj->dwObjInfo); 
   }
   else
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
   }

   DALSYSCMN_SET_OBJECT_ID(pMemObj->dwObjInfo, DALSYSCMN_MEM_OBJECT_ID);
   DALSYSCMN_SET_ATTRIB(pMemObj->dwObjInfo, pMemInfo->dwProps);
   DALSYSCMN_RESET_MEM_COPY_BIT(pMemObj->dwObjInfo);
   pMemObj->thisVirtualAddr = (DALSYSMemAddr)pMemObj;
   pMemObj->memInfo = *pMemInfo;
   pMemObj->qmem_region = qmem_region; 
   return pMemObj;
}

/*------------------------------------------------------------------------------
* Users must invoke this API to construct a Mem Object. Users have the option
  of pre-allocating memor: required for this object, or rely upon dynamic
  memory allocation.
 -----------------------------------------------------------------------------*/
DALResult
DALSYS_MemRegionAllocEx( DALSYSMemInfoEx *pMemInfo, DALSYSMemHandle *phMem,
      DALSYSMemObj *pObj)
{
    _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)pObj;
    qurt_mem_region_t qmem_region = 0;
    *phMem = NULL;

    if ((pMemInfo->dwProps & DALSYS_MEM_PROPS_PHYS_PREALLOC) == 0)
    {
        // create underlying kernel memory region
        // QURT will check and update the virtual addr, phys addr, and size
        // to ensure they match the alignment and size of memory mappings
        if (DAL_SUCCESS != DALSYS_MemAllocQurtRegion( pMemInfo, &qmem_region ))   
            return DAL_ERROR;
    }

    // Alloc memory for the dalsys memobject
    pMemObj = AllocAndInitMemObj( pMemInfo, pMemObj, qmem_region );
    if( NULL != pMemObj )
    {
        // return handle
        *phMem = (DALSYSMemHandle)pMemObj;
        return DAL_SUCCESS;
    }
    else
    {
        // delete memory region
        DALSYS_MemFreeQurtRegion( pMemInfo, qmem_region );
        return DAL_ERROR;
    }
}

DALResult
DALSYS_MemRegionAlloc(uint32 dwAttribs, DALSYSMemAddr virtualAddr,
                      DALSYSMemAddr physicalAddr, uint32 dwLen, 
                      DALSYSMemHandle *phMem, DALSYSMemObj *pObj)
{
   DALSYSMemInfoEx memInfo;

   memInfo.PhysicalAddr = (DALSYSMemAddr64)physicalAddr;
   memInfo.VirtualAddr = virtualAddr;
   memInfo.dwLen = dwLen;
   memInfo.dwProps = dwAttribs;

   return DALSYS_MemRegionAllocEx( &memInfo, phMem, pObj);
}

DALResult
DALSYS_MemInfoEx(DALSYSMemHandle hMem, DALSYSMemInfoEx *pMemInfo)
{
    _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)DALSYSCMN_HANDLE_TO_OBJECT(hMem);
    if(pMemObj)
    {
       *pMemInfo = pMemObj->memInfo;
       return DAL_SUCCESS;
    }
    DALSYS_LOG_ERROR_EVENT("Failed- pMemInfo:0x%x", pMemInfo);
    return DAL_ERROR;
} 

DALResult
DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo)
{
    _DALSYSMemObj *pMemObj = (_DALSYSMemObj *) DALSYSCMN_HANDLE_TO_OBJECT(hMem);
    if(pMemObj)
    {
        pMemInfo->VirtualAddr  = pMemObj->memInfo.VirtualAddr; 
        pMemInfo->PhysicalAddr = (DALSYSMemAddr)pMemObj->memInfo.PhysicalAddr;
        pMemInfo->dwLen        = pMemObj->memInfo.dwLen;
        pMemInfo->dwMappedLen  = pMemObj->memInfo.dwLen;
        pMemInfo->dwProps      = pMemObj->memInfo.dwProps;
        return DAL_SUCCESS;
    }
    DALSYS_LOG_ERROR_EVENT("Failed- pMemInfo:0x%x", pMemInfo);
    return DAL_ERROR;
} 

DALResult
DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen)
{
   qurt_mem_cache_op_t opCode;
    if (0 == dwLen)
    {
        DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR,
            "line length cannot be empty in DALSYS_CacheCommand- VirtualAddr:0x%x",
            VirtualAddr);
        return DAL_ERROR;
    }
     
    switch(CacheCmd)
    {
      case DALSYS_CACHE_CMD_INVALIDATE:
         opCode = QURT_MEM_CACHE_INVALIDATE;
         break;

      case DALSYS_CACHE_CMD_FLUSH:
         opCode = QURT_MEM_CACHE_FLUSH_INVALIDATE;
         break;

      case DALSYS_CACHE_CMD_CLEAN:
         opCode = QURT_MEM_CACHE_FLUSH;
         break;

      default: 
        DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR,
            "Unsupported cache command 0x%x in DALSYS_CacheCommand- VirtualAddr:0x%x",
            CacheCmd, VirtualAddr);
         return DAL_ERROR;
   }

   if (QURT_EOK == qurt_mem_cache_clean( (qurt_addr_t)VirtualAddr, 
            (qurt_size_t)dwLen, opCode, QURT_MEM_DCACHE ))
   {
      return DAL_SUCCESS;
   }
   else
   {
      DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR,
         "qurt_mem_cache_clean failed in DALSYS_CacheCommand- VirtualAddr:0x%x",
            VirtualAddr);
      return DAL_ERROR;
   }
}

DALSYSMemHandle
DALSYSCMN_MemObjCopy(DALSYSMemObj * pInObj, DALSYSMemObj * pOutObj)
{
   _DALSYSMemObj * pInMemObj = (_DALSYSMemObj *)pInObj;

   if (!pInObj)
   {
      return NULL;
   }

   // qmem_region below is NULL
   // the underlying qurt memory region can only be deallocated
   // by destroying the parent DAL memory object
   return (DALSYSMemHandle)AllocAndInitMemObj( &pInMemObj->memInfo, 
         (_DALSYSMemObj *)pOutObj, NULL );
}

DALResult DALSYS_DestroyMemObject(DALSYSMemHandle hMem)
{
   _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)DALSYSCMN_HANDLE_TO_OBJECT(hMem);

   if(DALSYSCMN_HANDLE_IS_COPY(hMem)) return DAL_ERROR;

   if( !pMemObj || (DALSYSCMN_MEM_OBJECT_ID != 
            DALSYSCMN_GET_OBJECT_ID(pMemObj->dwObjInfo))) return DAL_ERROR;

   DALSYS_MemFreeQurtRegion( &pMemObj->memInfo, pMemObj->qmem_region );

   /*----------------------------------------------------------------------
    If DALSYS Mem object dynamically alloced free structure memory,
    otherwise reset the memory area to ZERO
    ----------------------------------------------------------------------*/
   if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pMemObj->dwObjInfo))
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
      return DALSYS_Free(pMemObj);
   }
   else
   {
      DALSYSCMN_RESET_ALL_BITS(pMemObj->dwObjInfo);
      return DAL_SUCCESS;
   }
}

DALResult
DALSYS_Malloc(uint32 dwSize, void **ppMem)
{
    if (0 == dwSize)
    {
        //treat zero size as fatal error
        DALSYS_LOG_FATAL_EVENT("DALSYS_Malloc: size cannot be zero");
        return DAL_ERROR;
    }

    *ppMem = qurt_malloc(dwSize);
    if (NULL != *ppMem)
    {
        return DAL_SUCCESS;
    }
    // under RTOS environments treat this as fatal error
    DALSYS_LOG_FATAL_EVENT("OUT OF MEMORY");
    return DAL_ERROR;
}

DALResult
DALSYS_Free(void *pMem)
{
    qurt_free(pMem);
    return DAL_SUCCESS;
}

uint32 _DALSYS_memscpy(void * pDest, uint32 iDestSz, 
      const void * pSrc, uint32 iSrcSize)
{
   return memscpy(pDest, iDestSz, pSrc, iSrcSize);
}
