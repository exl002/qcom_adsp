#ifndef SNS_MEMMGR_H
#define SNS_MEMMGR_H

/*============================================================================
  @file sns_memmgr.h

  @brief
  Defines the sensors memory manager interface.

  <br><br>

  DEPENDENCIES: 

Copyright (c) 2010, 2012-2013 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/common/util/memmgr/inc/sns_memmgr.h#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who    what, where, why 
  ---------- --- -----------------------------------------------------------
  2014-02-26 MW  Added SNS_OS_MEMSCPY
  2012-11-21 vh  Added SNS_OS_MEMSET for SNS_OS_MALLOC
  2012-11-15 ps  Replaced default heap id with sensors head id for SNS_OS_MALLOC
  2011-11-30 jtl Updating macors for USE_NATIVE_MALLOC to work with heap
                 analysis changes
  2011-11-22 br  Differenciated between USE_NATIVE_MEMCPY and USE_NATIVE_MALLOC
  2011-11-14 jhh Added heap memory analysis feature
  2010-11-03 jtl Implementing USE_NATIVE_MALLOC macro handling
  2010-08-30 JTL Moving init function decl into sns_init.h
  2010-08-26 jtl Added header comments

  ============================================================================*/
/*=====================================================================
                       INCLUDES
=======================================================================*/
#include "sns_common.h"
#if (defined USE_NATIVE_MEMCPY || defined USE_NATIVE_MALLOC)
#  include "stdlib.h"
#  include "string.h"
#endif
#if !(defined USE_NATIVE_MEMCPY && defined USE_NATIVE_MALLOC)
#  include "mem_cfg.h"
#endif /* USE_NATIVE_MALLOC */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus  
  

/*=====================================================================
                    INTERNAL DEFINITIONS AND TYPES
=======================================================================*/
/* used to set debugging level (0~2) */
#define SNS_DEBUG   0
//#define SNS_MEMMGR_PROFILE_ON

/* verbose level set up */
#ifdef SNS_DEBUG
 #if SNS_DEBUG == 0
  #define DBG_PRINT1(x) (void)0
  #define DBG_PRINT2(x) (void)0
 #else
  #if SNS_DEBUG == 1
#include <stdio.h>
#ifdef _WIN32
   #define DBG_PRINT1(x) (void)(sns_trace x)
   #define DBG_PRINT2(x) (void)0
#else
   #define DBG_PRINT1(x) (void)(printf x)
   #define DBG_PRINT2(x) (void)0
#endif /* _WIN32 */
   #define OI_DEBUG
  #elif SNS_DEBUG == 2
#include <stdio.h>
#ifdef _WIN32
   #define DBG_PRINT1(x) (void)(sns_trace x)
   #define DBG_PRINT2(x) (void)0
#else
   #define DBG_PRINT1(x) (void)(printf x)
   #define DBG_PRINT2(x) (void)(printf x)
#endif /* _WIN32 */
   #define OI_DEBUG
  #endif
 #endif
#endif

#ifdef SNS_MEMMGR_PROFILE_ON
#define SNS_OS_MEMSTAT_SUMMARY_POOLS(pool_id)     SUMMARY_DUMP(pool_id)
#define SNS_OS_MEMSTAT_DETAIL_POOLS(pool_id)      DETAIL_DUMP(pool_id)

typedef enum {
  POOL_16_BYTE,
  POOL_24_BYTE,
  POOL_32_BYTE,
  POOL_64_BYTE,
  POOL_128_BYTE,
  POOL_256_BYTE,
  POOL_512_BYTE,
  POOL_1024_BYTE,
  POOL_ALL
} mem_pool_e;
#else
#define SNS_OS_MEMSTAT_SUMMARY_POOLS(pool_id)     (void)0
#define SNS_OS_MEMSTAT_DETAIL_POOLS(pool_id)      (void)0
#endif

#ifdef USE_NATIVE_MEMCPY
#  define SNS_OS_MEMCOPY(to,from,size)  memcpy(to, from, size)
#  define SNS_OS_MEMSET(block,val,size) memset(block, val, size)
#  define SNS_OS_MEMZERO(block,size)    memset(block, 0, size)
#  define SNS_OS_MEMCMP(s1,s2,n)        memcmp(s1, s2, n)
#  define SNS_OS_MEMSCPY(dst, dst_size, src, copy_size)  memscpy(dst, dst_size, src, copy_size)
#else
#  define SNS_OS_MEMCOPY(to,from,size)  OI_MemCopy(to, from, size)
#  define SNS_OS_MEMSET(block,val,size) OI_MemSet(block, val, size)
#  define SNS_OS_MEMZERO(block,size)    OI_MemZero(block, size)
#  define SNS_OS_MEMCMP(s1,s2,n)        OI_MemCmp(s1, s2, n)
#  define SNS_OS_MEMSCPY(dst, dst_size, src, copy_size)  OI_MemCopy(dst, src, copy_size)
#endif

#ifdef USE_NATIVE_MALLOC
#  define SNS_OS_FREE(block)            free(block)
#  define SNS_OS_MALLOC(module,size)    malloc(size)
#  define SNS_OS_FREE(block)            free(block)
#  define SNS_OS_CHGLLOC(new_mod,block)
#elif defined(QDSP6)
// TODO: decide the memory allocation type for QMI messages
#ifndef SNS_QMI_ENABLE
#define SNS_OS_MALLOC(module,size)    qurt_elite_memory_malloc(size, sns_heap_id)
#else //SNS_QMI_ENABLE
static inline void *SNS_OS_MALLOC(uint8_t module, uint32_t size)
{
  void *buffer = NULL;
  buffer = qurt_elite_memory_malloc(size, sns_heap_id);
  if (buffer != NULL)
     SNS_OS_MEMSET(buffer, 0, size);
  return buffer;
}
#endif//SNS_QMI_ENABLE
#  define SNS_OS_FREE(block)            qurt_elite_memory_free(block)
#  define SNS_OS_CHGLLOC(new_mod,block)
#else
#  define SNS_OS_MALLOC(module, size)    OI_Malloc(module, size)
#  define SNS_OS_FREE(block)             OI_Free(block)
#  define SNS_OS_CHGALLOC(new_mod,block) OI_Realloc(new_mod,block)
#endif /* USE_NATIVE_MALLOC */


/*=====================================================================
                          FUNCTIONS
=======================================================================*/

/*===========================================================================

  FUNCTION:   sns_heap_init

  ===========================================================================*/
/*!
  @brief Initializes separate heap for Sensors

  This function will create a separate Heap for Sensors.
  
  'sns_heap_id' is heap ID for memory allocations from Sensors code.

*/
/*=========================================================================*/
sns_err_code_e sns_heap_init(void);


/*===========================================================================

  FUNCTION:   sns_heap_destroy

  ===========================================================================*/
/*!
    Destorys the Sensors heap manager created by sns_heap_init
*/
/*=========================================================================*/
sns_err_code_e sns_heap_destroy(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* SNS_MEMMGR_H */
