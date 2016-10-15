/*============================================================================
  @file sns_memmgr.c

  @brief
  Implements SNS memory manager interfaces.

  <br><br>

  DEPENDENCIES:  Uses BM3 memory manager.

Copyright (c) 2010-2014 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/common/util/memmgr/src/sns_memmgr.c#2 $
  $DateTime: 2014/05/28 12:36:49 $

  when       who    what, where, why 
  ---------- --- -----------------------------------------------------------
  2014-05-13  pk Isolate enhanced batching to SSC_BATCH_ENHANCE switch
  2014-05-12  ad Support enhanced batching
  2013-08-22  pk Increased SNS heap size for Hammerhead
  2013-06-12 agk Do not use OI_Memmgr for adsp (Q6) baselines.
  2010-11-03 jtl Implementing USE_NATIVE_MALLOC macro handling.
  2010-08-26 jtl Initial version

  ============================================================================*/
/*=====================================================================
  INCLUDES
=======================================================================*/

#include "sns_common.h"
#include "sns_memmgr.h"
#include "sns_init.h"

#include "oi_status.h"
#include "oi_support_init.h"

/* Determine the Sensors Heap as required */
#ifdef SSC_BATCH_ENHANCE
   #define SNS_HEAP_SIZE   (1024*1024)
#else
   #define SNS_HEAP_SIZE   (256*1024)
#endif

//extern unsigned int heapSize;

/*============================================================================
  Global Data Definitions
  ===========================================================================*/

/* Sensors memory allocation requests uses this heap ID */
QURT_ELITE_HEAP_ID sns_heap_id;

/* Sensors heap of SNS_HEAP_SIZE bytes. All dynamically allocated buffers are allocated from heap */
static char sns_heap[SNS_HEAP_SIZE] __attribute__((__section__(".bss.sns_heap")));

/*=====================================================================
  EXTERNAL FUNCTIONS
=======================================================================*/

/*===========================================================================

  FUNCTION:   sns_heap_init
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e sns_heap_init(void)
{
   ADSPResult result;

   /*Initializing heap manager for Sensors' dynamic memory allocations */
   if (ADSP_FAILED(result = qurt_elite_memory_heapmgr_create(
                            &sns_heap_id, sns_heap, sizeof(sns_heap)))) {
      MSG_1(MSG_SSID_SNS, DBG_ERROR_PRIO,
                  "Failed to initialize heap manager!! Error %d\n", result);
      return SNS_ERR_FAILED;
   }

   MSG_3(MSG_SSID_SNS, DBG_MED_PRIO,
               "Sensors Heap ID %d, BUFFERADDR 64K aligned page: 0x%8x, size %d",
               sns_heap_id, sns_heap, sizeof(sns_heap));

   return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_memmgr_init
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e sns_heap_destroy(void)
{
   if(ADSP_EOK != qurt_elite_memory_heapmgr_delete(sns_heap_id)) {
	   return SNS_ERR_FAILED;
   }

   return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_memmgr_init
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e
sns_memmgr_init( void )
{
#if !defined(USE_NATIVE_MALLOC) && !defined (QDSP6)
  OI_STATUS err;
  
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MEMMGR INIT");

  err = OI_MEMMGR_Init( &oi_default_config_MEMMGR );

  sns_init_done();

  if( OI_OK == err ) {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MEMMGR INIT SUCCESS");
    return SNS_SUCCESS;
  } else {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MEMMGR INIT FAIL");
    return SNS_ERR_FAILED;
  }
#else
  sns_init_done();
  return SNS_SUCCESS;
#endif /* USE_NATIVE_MALLOC */
  
}

/*===========================================================================

  FUNCTION:   sns_memmgr_deinit
  - Documented in sns_memmgr.h
  ===========================================================================*/
sns_err_code_e
sns_memmgr_deinit( void )
{
#if !defined(USE_NATIVE_MALLOC) && !defined (QDSP6)
  OI_STATUS err;

  err = OI_MEMMGR_DeInit( );
#else
    //todo, find any/all memory used via native calls ?!?
#endif
    return SNS_SUCCESS;
}
