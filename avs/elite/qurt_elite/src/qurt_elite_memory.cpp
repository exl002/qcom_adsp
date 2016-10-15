/*========================================================================
qurt_elite_memory.cpp

This file contains a utility for memory allocation.

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/elite/qurt_elite/src/qurt_elite_memory.cpp#14 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/10   mwc      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "memory_wrappers.h"
#include "memheap.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
#define QURT_ELITE_HEAP_MGR_HEAP_INDEX_START  1
#define QURT_ELITE_HEAP_MGR_HEAP_INDEX_END  QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS

/* QuRT_Elite heap manager heap table structure */
typedef struct qurt_elite_heap_table_t {
   uint32_t          used_flag;        /* Heap table entry status (used=1, unused=0). */
   uint32_t          start_addr;       /* Start address of the heap. */
   uint32_t          end_addr;         /* End address of the heap. */
}qurt_elite_heap_table_t;

/*add_section wrapper externed*/
extern "C" {
   void mem_wrap_add_heap_section(mem_heap_type *heap_ptr, void *section_start, unsigned long section_size);
}

/*Heap manager table*/
static qurt_elite_heap_table_t qurt_elite_heap_table[QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS + 1];

/* State of the heaps. */
static mem_heap_type     heap_state[QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS + 1];

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/****************************************************************************
** Memory
*****************************************************************************/

#ifdef DEBUG_QURT_ELITE_MALLOC_CALLSTACK

static const uint32_t g_SkipStackFrames = 0;
static const uint32_t g_MaxStackFrames  = 6;

typedef struct StackFrameType
{
   StackFrameType* pPrevFrame;
   void* pReturnAddress;
} StackFrameType;

void PrintBacktrace ( void )
{
   StackFrameType* pCurFrame;
   uint32_t numSkipped = 0;
   uint32_t numStored = 0;
   static void *pReturnAddresses[6] = {0};

   __asm__ __volatile(
      "memw(%0) = fp\n"
      : /* Output variables */
         "=m" ( pCurFrame )
      : /* Input variables */
      : /* Clobbered registers */
   );


   // Skip the first couple stack entries because they aren't very useful
   while ( ( pCurFrame != NULL ) && ( numSkipped < g_SkipStackFrames ) )
   {
      pCurFrame = pCurFrame->pPrevFrame;
      numSkipped += 1;
   }


   // Save a sequence of stack entries
   while ( ( pCurFrame != NULL ) && ( numStored < g_MaxStackFrames ) )
   {
      pReturnAddresses[numStored++] = pCurFrame->pReturnAddress;
      pCurFrame = pCurFrame->pPrevFrame;
   }
   MSG_6(MSG_SSID_QDSP6, DBG_LOW_PRIO, "QURT_ELITE_MALLOC_FREE_CALLSTK: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                                       pReturnAddresses[0],
                                       pReturnAddresses[1],
                                       pReturnAddresses[2],
                                       pReturnAddresses[3],
                                       pReturnAddresses[4],
                                       pReturnAddresses[5]);

}
#endif

void *qurt_elite_memory_malloc(uint32_t unBytes, QURT_ELITE_HEAP_ID heapId)
{
   void* ptr;

#ifdef DEBUG_QURT_ELITE_MEMORY
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
      "Entering qurt_elite_memory_malloc: thread 0x%x; size %d; heap %d; count: %d",
         qurt_thread_get_id(), unBytes, heapId, qurt_elite_globalstate.nSimulatedMallocFailCount);
#endif //DEBUG_QURT_ELITE_MEMORY

   // check for zero size memory requests.
   if (0 == unBytes)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Thread 0x%x trying to allocate zero size memory!", qurt_thread_get_id());
      return NULL;
   }

   // check for requested simulated malloc failure (for robustness testing)
   // Decrement counter only if
   // a) It is not steady state (Value = -1) (i.e. non-test mode) AND
   // b) Malloc Fail Count is non zero, i.e. Mem failure has not yet been simulated

   if ((0 != qurt_elite_globalstate.nSimulatedMallocFailCount) && (-1 != qurt_elite_globalstate.nSimulatedMallocFailCount))
   {
      if (0 == --(qurt_elite_globalstate.nSimulatedMallocFailCount))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Simulated out of memory failure!!");
         return NULL;
      }
   }

   //check if the heap-id is valid heap-id
   if (heapId >= QURT_ELITE_HEAP_OUT_OF_RANGE)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HeapID provided is not valid!!");
      return NULL;
   }

   // check if the memory allocation is from non-default heap.
   if ((heapId >= QURT_ELITE_HEAP_MGR_HEAP_INDEX_START) && (heapId <= QURT_ELITE_HEAP_MGR_HEAP_INDEX_END))
   {
       if (NULL != (ptr = qurt_elite_memory_heapmgr_malloc(unBytes, heapId)))
       {
           // malloc's from non-default heaps will not get counted in nMallocs/nFree's in global state.
           return ptr;
       }
       else
       {
           MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "memory requested by thread 0x%x of size %lu is not available in heap %d, allocating in default heap",
               qurt_thread_get_id(), unBytes, heapId);
       }
   }

   if (0 == qurt_elite_globalstate.nSimulatedMallocFailCount)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Already simulated memory failure! Returning NULL!");
      return NULL;
   }

   ptr = mem_wrap_alloc_track(unBytes, &qurt_elite_globalstate.heap_stats);

#ifdef DEBUG_QURT_ELITE_MEMORY
   MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "QURT_ELITE_MALLOC_FREE_TRACER: Thread 0x%x mallocs %u bytes, gets ptr 0x%x",
      qurt_thread_get_id(), unBytes, ptr);
#endif //DEBUG_QURT_ELITE_MEMORY

#ifdef DEBUG_QURT_ELITE_MALLOC_CALLSTACK
   PrintBacktrace();
#endif

   return ptr;

}

void qurt_elite_memory_free(void *ptr)
{
   uint32_t heap_idx;

   // check for NULL.
   if (!ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Thread 0x%x trying to free null pointer!", qurt_thread_get_id());
      return;
   }

   /* check if the given address falls in heap manager heaps and free the address        */
   for (heap_idx = QURT_ELITE_HEAP_MGR_HEAP_INDEX_START; heap_idx <= QURT_ELITE_HEAP_MGR_HEAP_INDEX_END; heap_idx++)
   {
      if (( 1 == (qurt_elite_heap_table[heap_idx].used_flag))
         && (ptr >= (void*)(qurt_elite_heap_table[heap_idx].start_addr))
         && (ptr < (void*)(qurt_elite_heap_table[heap_idx].end_addr)))
      {
         // free the memory
         qurt_elite_memory_heapmgr_free(ptr, (QURT_ELITE_HEAP_ID)heap_idx);
         return;
      }
   }

#ifdef DEBUG_QURT_ELITE_MEMORY
   MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "QURT_ELITE_MALLOC_FREE_TRACER: Thread 0x%x frees %u bytes, from ptr 0x%x",
      qurt_thread_get_id(), unBytes, pPtr);
#endif //DEBUG_QURT_ELITE_MEMORY

   mem_wrap_free_track(ptr, &qurt_elite_globalstate.heap_stats);
}

void *qurt_elite_memory_aligned_malloc(uint32_t unBytes, uint32_t unAlignSize, QURT_ELITE_HEAP_ID heapId)
{

   if (heapId >= QURT_ELITE_HEAP_OUT_OF_RANGE) return NULL;

   // only allow alignment to 4 or more bytes.
   if (unAlignSize <= 4) unAlignSize = 4;

   // keep out the crazy stuff.
   if (unAlignSize > (1 << 30)) return NULL;

   char *ptr, *ptr2, *aligned_ptr;
   uint32_t align_mask = ~(unAlignSize - 1);

   // allocate enough for requested bytes + alignment wasteage + 1 word for storing offset
   // (which will be just before the aligned ptr)
   ptr = (char*) qurt_elite_memory_malloc(unBytes + unAlignSize + sizeof(int), heapId);
   if(ptr == NULL) return (NULL);

   //allocate enough for requested bytes + alignment wasteage + 1 word for storing offset
   ptr2 = ptr + sizeof(int);
   aligned_ptr = (char*)((uint32_t)(ptr2 - 1) & align_mask) + unAlignSize;

   // save offset to raw pointer from aligned pointer
   ptr2 = aligned_ptr - sizeof(int);
   *((int *)ptr2)=(int)(aligned_ptr - ptr);

   return(aligned_ptr);
}

void qurt_elite_memory_aligned_free(void *ptr)
{
   uint32_t* pTemp = (uint32_t*)ptr;
   uint32_t* ptr2 = pTemp - 1;

   if (NULL == ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Thread 0x%x trying to free null pointer!", qurt_thread_get_id());
      return;
   }
   pTemp -= *ptr2 / sizeof(uint32_t);
   qurt_elite_memory_free(pTemp);
}

/**
Allocate memory in the heap

@param[in] heap_id  Heap id
@param[in] size  Number of bytes to be allocated
@return
A pointer to the allocated buffer
*/
void* qurt_elite_memory_heapmgr_malloc(uint32_t size, QURT_ELITE_HEAP_ID heap_id)
{
   #ifdef DEBUG_QURT_ELITE_MEMORY
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Entering qurt_elite_memory_heapmgr_malloc: size %d; heap %d",
         size, heap_id);
   #endif //DEBUG_QURT_ELITE_MEMORY

   // error check for heap_id
   if ( (heap_id < QURT_ELITE_HEAP_MGR_HEAP_INDEX_START) || (heap_id > QURT_ELITE_HEAP_MGR_HEAP_INDEX_END))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "heap id %d out of range!", heap_id);
      return NULL;
   }

   // check if there is an existing heap with given heap-id
   if ( qurt_elite_heap_table[heap_id].used_flag != 1)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "trying to allocate memory from non-existing heap %d !!", heap_id);
      return NULL;
   }

   //Allocate using memheap
   return mem_malloc(&heap_state[heap_id], size);
}

/**
Free allocated memory

@param[in] ptr Pointer to the buffer to be freed
@param[in] heap_id Heap id
@return
 None
*/
void qurt_elite_memory_heapmgr_free(void *ptr, QURT_ELITE_HEAP_ID heap_id)
{
   if (!ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Thread 0x%x trying to free null pointer!", qurt_thread_get_id());
      return;
   }
   if ( (heap_id < QURT_ELITE_HEAP_MGR_HEAP_INDEX_START) || (heap_id > QURT_ELITE_HEAP_MGR_HEAP_INDEX_END))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory at pointer 0x%lx is not managed by heap manager!", (uint32_t)ptr);
      return;
   }

   // check if there is an existing heap with given heap-id
   if ( qurt_elite_heap_table[heap_id].used_flag != 1)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "trying to free memory from non-existing heap %d !!", heap_id);
      return;
   }

   //Dallocate using memheap
   mem_free(&heap_state[heap_id], ptr);
}

/**
Initialize heap manager for a given heap.

@param[in] heap_id_ptr pointer to heap-id
@param[in] heap_start_ptr  Starting address of the heap
@param[in] heap_size Size of the heap
@return
 status of heap manager creation
*/
ADSPResult qurt_elite_memory_heapmgr_create(QURT_ELITE_HEAP_ID *heap_id_ptr, void *heap_start_ptr, uint32_t heap_size)
{
   uint32_t   heap_idx;

   // check if the heap_start_add is vaild pointer
   if (NULL == heap_start_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "trying to create heap manager with null pointer!!");
      return ADSP_EFAILED;
   }

   /*allocate unique heap id to the heap managed by heap manager*/
   /* and update heap manager entries accordingly               */

   // acquire lock for qurt_elite global state mutex to ensure thread saftey of qurt_elite_heap_table
   qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);

   // check for heap table entry that is unused.
   for (heap_idx = QURT_ELITE_HEAP_MGR_HEAP_INDEX_START; heap_idx <= QURT_ELITE_HEAP_MGR_HEAP_INDEX_END; heap_idx++)
   {
      if (0 == qurt_elite_heap_table[heap_idx].used_flag)
      {
         break;
      }
   }

   // check if heap manager is already managing max number of heaps
   if (heap_idx > QURT_ELITE_HEAP_MGR_HEAP_INDEX_END)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite heap manager can only maintain %d heaps!",
         QURT_ELITE_HEAP_MGR_HEAP_INDEX_END);
      // unlock qurt_elite global state mutex
      qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);
      return ADSP_EFAILED;
   }

   //allocate heap-id
   *heap_id_ptr = (QURT_ELITE_HEAP_ID)heap_idx;

   //update heap table entries
   qurt_elite_heap_table[heap_idx].used_flag = 1;
   qurt_elite_heap_table[heap_idx].start_addr = (uint32_t)heap_start_ptr;
   qurt_elite_heap_table[heap_idx].end_addr = (uint32_t)heap_start_ptr + heap_size;

   //Initialize the heap using memheap
   //Memheap can handle any alignment of heap_start_ptr
   //heap_size need not be a multiple of any power of 2.
   mem_init_heap(&heap_state[heap_idx],
                 heap_start_ptr,
                 heap_size,
                 NULL);

   // unlock qurt_elite global state mutex
   qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

   return ADSP_EOK;
}

/**
UnInitialize the heap manager of a given heap-id

@param[in]  heap_id  Heap id
@return
 status of heap manager deletion
*/
ADSPResult qurt_elite_memory_heapmgr_delete(QURT_ELITE_HEAP_ID heap_id)
{
   // error check for heap_id
   if ((heap_id < QURT_ELITE_HEAP_MGR_HEAP_INDEX_START) || (heap_id > QURT_ELITE_HEAP_MGR_HEAP_INDEX_END))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "heap id %d out of range!", heap_id);
      return ADSP_EFAILED;
   }

   // acquire lock for qurt_elite global state mutex to ensure thread saftey of qurt_elite_heap_table
   qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);

   //Deinit the heap
   mem_deinit_heap(&heap_state[heap_id]);

   // reset heap table entries
   memset(&qurt_elite_heap_table[heap_id], 0, 
          sizeof(qurt_elite_heap_table[heap_id]));

   // unlock qurt_elite global state mutex
   qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

   return ADSP_EOK;
}


/*
 *Function Name: qurt_elite_heap_table_init
 *
 *Parameters: None 
 *
 *Description: Initialize heap manager heap table entries.
 *
 *Returns: None
 *
 */

void qurt_elite_heap_table_init(void)
{
   //initialize heap manager heap table entries
   memset(qurt_elite_heap_table, 0, sizeof(qurt_elite_heap_table));
}

/*
 *Function Name:qurt_elite_mem_heap_add_section
 *
 *Parameters: section_start : Pointer to contiguous block of memory used for this section
 *            section_size  : Size of the section 	
 *            heap_id	    : Heap ID to which the section must be added to
 *
 *Description: Adds a section of memory to the specified heap ID
 *
 *Returns: None
 *
 */

void qurt_elite_mem_heap_add_section(void *section_start, uint32_t section_size,QURT_ELITE_HEAP_ID heap_id)
{
	//Check and add for default heap
	if(QURT_ELITE_HEAP_DEFAULT == heap_id)
	{
		mem_wrap_add_heap_section( 0, section_start, section_size);
	}
	//Check for other heaps 
	else
	{
		// error check for heap_id
		if ((heap_id < QURT_ELITE_HEAP_MGR_HEAP_INDEX_START) || (heap_id > QURT_ELITE_HEAP_MGR_HEAP_INDEX_END))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "heap id %d out of range!", heap_id);
			return;
		}
		// check if there is an existing heap with given heap-id
		if ( 1 != qurt_elite_heap_table[heap_id].used_flag)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "trying to add section for a non-existing heap %d !!", heap_id);
			return;
		}
		mem_wrap_add_heap_section(&heap_state[heap_id], section_start, section_size);
	}	
	return;

}
