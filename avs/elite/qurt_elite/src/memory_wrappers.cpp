/*========================================================================
memory_wrappers.cpp

This file contains wrapper functions for malloc, free, calloc, realloc

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/elite/qurt_elite/src/memory_wrappers.cpp#13 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/03/11   rkc      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "memory_wrappers.h"
#include "qurt_elite.h"
#include "memheap.h"
#include <stdlib.h>
#include <stdio.h>
#include <stringl/stringl.h>

//#define DBG_MEM_LEAK

/* Variable for tracking heap initialization. Heap will
   be initialized in the very first malloc call. */

static uint32_t is_heap_init = 0;

/* Default heap status structure for memheap apis */
static mem_heap_type default_heap_state;

/* MemHeap block header information (as in amssheap.c) */
union header {                  /* block header */
    struct {
        unsigned size;          /* size of this block */
        union header *ptr;      /* next block if on free list */
    } s;
    long long align;            /* force alignment of blocks */
};
typedef union header Header;

/* Variable for tracking heap usage for non-elite code */
mem_wrap_heap_stats_t mem_wrap_heap_stats;

/* Structures and variables for debugging memory leaks */
typedef struct
{
   int thread_id;
   char thread_name[QURT_THREAD_ATTR_NAME_MAXLEN];
   uint32_t ptr;
   uint32_t size;
} heap_params_t;

/* This must be a power of 2 */
#define NUM_DBG_CTR         (1024)

#ifdef DBG_MEM_LEAK
static qurt_elite_mutex_t mem_wrap_memleak_lock;
static heap_params_t dbg_malloc[NUM_DBG_CTR];
static heap_params_t temp_dbg_malloc[NUM_DBG_CTR];
static uint32_t ctr_malloc = 0;
static uint32_t ctr_malloc_wrap = 0;
static heap_params_t dbg_free[NUM_DBG_CTR];
static heap_params_t temp_dbg_free[NUM_DBG_CTR];
static uint32_t ctr_free = 0;
static uint32_t ctr_free_wrap = 0;
#endif //DBG_MEM_LEAK

extern "C" {
/*add_section API externed*/
void mem_heap_add_section(mem_heap_type *heap_ptr, void *section_start, unsigned long section_size);

/* Updates malloc statistics (malloc counters and heap usage) */
static void mem_wrap_update_alloc_stats(void *ptr, uint32_t size, mem_wrap_heap_stats_t *stats);

/* Updates free statistics (malloc counters and heap usage) */
static void mem_wrap_update_free_stats(void *ptr, uint32_t size, mem_wrap_heap_stats_t *stats);

/* Function call to give default heap information */
void * sys_sbrk(int more);

void *__real_malloc (size_t size);
void *__real_realloc (void *ptr, size_t size);
void *__real_calloc (size_t num_elem, size_t elem_size);
void __real_free (void *ptr);

/*
 *Function Name:__wrap_malloc
 *
 *Parameters: size_t size: Size of memory to allocate in bytes
 *
 *Description: Wrapper function for the C stdlib function malloc
 *             Allocates extra memory for header, tail guard words
 *             to detect memory corruption and extra memory
 *             to hold the size of memory allocated. Also keeps track
 *             of total number of memory blocks allocated, current heap usage
 *             and peak heap usage
 *
 *Returns: Pointer to allocated memory block
 *
 */

void *__wrap_malloc (size_t size)
{
   return mem_wrap_alloc_track((uint32_t)size, &mem_wrap_heap_stats);
}

/*
 *Function Name:__wrap_free
 *
 *Parameters: void *ptr: Pointer to deallocate
 *
 *Description: Wrapper function for the C stdlib function free.
 *             Frees a  memory block allocated by __wrap_malloc
 *             (except for the first call to this function).
 *             Keeps track of the total number of memory blocks
 *             freed and the current heap usage.
 *
 *             For the first ever call to this function, a pointer not
 *             allocated by __wrap_malloc is given. This is done
 *             during heap initialization. We must therefore treat
 *             this pointer as had been allocated by real malloc
 *             and just call __real_free on this pointer without
 *             changing it.
 *
 *Returns: None
 *
 */

void __wrap_free (void *ptr)
{
   mem_wrap_free_track(ptr, &mem_wrap_heap_stats);
}

/*
 *Function Name:__wrap_calloc
 *
 *Parameters: size_t nelements: Number of elements to allocate
 *            size_t elementSize: Size of each element in bytes
 *
 *Description: Wrapper function for the C stdlib function calloc
 *             Same as __wrap_malloc but the allocated memory block
 *             is initialized to all zeros.
 *
 *Returns: Pointer to allocated memory block
 *
 */

void *__wrap_calloc (size_t num_elem, size_t elem_size)
{
   size_t total_size = num_elem * elem_size; //multiplication can overflow
   void *ptr = __wrap_malloc(total_size);

   if(!ptr) return NULL;

   memset(ptr, 0, total_size);
   return ptr;
}

/*
 *Function Name:__wrap_realloc
 *
 *Parameters: void *pointer: Pointer to reallocate
 *            size_t size: New size of the memory block in bytes
 *
 *Description: Wrapper function for the C stdlib function realloc
 *             Same functionality as realloc but implemented through
 *             __wrap_malloc and __wrap_free to track memory usage
 *             and detect memory corruption.
 *
 *Returns: Pointer to allocated memory block
 *
 */

void *__wrap_realloc (void *ptr, size_t size)
{
   if(!ptr)
   {
      return __wrap_malloc(size);
   }

   if(0 == size)
   {
      __wrap_free(ptr);
      return NULL;
   }

   //new memory is always disjoint from old memory, could be inefficient
   void *_ptr = __wrap_malloc(size);

   if(!_ptr) return NULL;

   // size of old block.
   uint32_t orig_size = mem_get_block_size(&default_heap_state, ptr);

   // calculate copy size
   uint32_t copy_size = orig_size;
   if(size < copy_size)
   {
      copy_size = size;
   }

   memscpy(_ptr, size,  ptr, copy_size);

   __wrap_free(ptr);

   return _ptr;
}

/*
 *Function Name:__wrap_memalign
 *
 *Parameters: size_t blocksize: Alignment size
 *Parameters: size_t size: Size of memory to allocate in bytes
 *
 *Description: This function returns a block of memory of size "size"
 *             aligned to blocksize. The blocksize must be given as
 *             a power of two. It returns NULL pointer upon failure.
 *
 *Returns: Pointer to allocated memory block
 *
 */

void *__wrap_memalign (size_t blocksize, size_t size)
{
   void *ptr = mem_memalign(&default_heap_state, blocksize, size);

   if(!ptr) return NULL;

   // update maloc statistics
   mem_wrap_update_alloc_stats(ptr, size, &mem_wrap_heap_stats);

   return ptr;
}

/*
 *Function Name:mem_wrap_alloc_track
 *
 *Parameters: uint32_t size: size of memory to allocate in bytes
 *            mem_wrap_heap_stats_t *stats: pointer to
 *            hep statistics structure
 *
 *Description: Helper function to allocate memory and keep track of
 *             heap usage
 *
 *Returns: Pointer to allocated memory block
 *
 */

void *mem_wrap_alloc_track(uint32_t size, mem_wrap_heap_stats_t *stats)
{
   if(0 == size)
   {
      return NULL;
   }

   // Initialize default memory pool. This will happen for very first malloc
   if (is_heap_init == 0)
   {
      uint32_t heap_size;
      void *heap_start, *heap_end;

      // Get default heap attributes
      Header *heap_ptr=(Header *)sys_sbrk(1);

      // Get start and end address of heaps
      heap_start = (void *)heap_ptr;
      QURT_ELITE_ASSERT(NULL != heap_start);
      heap_end = (void *)((int8_t *)heap_ptr + ((heap_ptr->s.size) - 1));

      // Calculate the size of the default heap
      heap_size = (uint32)heap_end-(uint32)heap_start;
      QURT_ELITE_ASSERT(0 != heap_size);

      // Clear the heap control struct
      memset(&default_heap_state, 0, sizeof(default_heap_state));

      // Initialize heap
      mem_init_heap( &default_heap_state, heap_start, heap_size, NULL);

      // update initialization flag
      is_heap_init = 1;

      mem_wrap_init_heap_stats(&mem_wrap_heap_stats);
      mem_wrap_init_memleak_data();
   }

   // Allocate memory
   void *ptr = mem_malloc( &default_heap_state, size );

   if (!ptr)
   {
      return NULL;
   }

   // update malloc statistics
   mem_wrap_update_alloc_stats(ptr, size, stats);

   return (void*) (ptr);
}

/*
 *Function Name:mem_wrap_free_track
 *
 *Parameters: void* ptr: pointer to deallocate
 *            mem_wrap_heap_stats_t *stats: pointer
 *            to heap statistics structure
 *
 *Description: Helper function to free memory and keep track of
 *             heap usage
 *
 *Returns: None
 *
 */

void mem_wrap_free_track(void *ptr, mem_wrap_heap_stats_t *stats)
{
   if(!ptr) return;

   // find size of freed block
   uint32_t size = mem_get_block_size(&default_heap_state, ptr);

   // free using memheap
   mem_free(&default_heap_state, ptr);

   mem_wrap_update_free_stats(ptr, size, stats);
}

/*
 *Function Name:mem_wrap_reset_peak_heap
 *
 *Parameters: mem_wrap_heap_stats_t *stats: pointer to heap
 *            statistics structure
 *
 *Description: Sets peak heap usage equal to current heap usage
 *
 *Returns: None
 *
 */

void mem_wrap_reset_peak_heap(mem_wrap_heap_stats_t *stats)
{
   qurt_elite_mutex_lock(&stats->lock);

   stats->peak_heap = stats->curr_heap;

   qurt_elite_mutex_unlock(&stats->lock);
}

/*
 *Function Name:mem_wrap_init_heap_stats
 *
 *Parameters: mem_wrap_heap_stats_t *stats: pointer to heap
 *            statistics structure
 *
 *Description: Initializes the heap statistics to zero.
 *
 *Returns: None
 *
 */

void mem_wrap_init_heap_stats(mem_wrap_heap_stats_t *stats)
{
   qurt_elite_mutex_init(&stats->lock);
   stats->num_mallocs = 0;
   stats->num_frees = 0;
   stats->curr_heap = 0;
   stats->peak_heap = 0;
}

/*
 *Function Name:mem_wrap_deinit_heap_stats
 *
 *Parameters: mem_wrap_heap_stats_t *stats: pointer to heap
 *            statistics structure
 *
 *Description: De-initializes the heap statistics.
 *
 *Returns: None
 *
 */

void mem_wrap_deinit_heap_stats(mem_wrap_heap_stats_t *stats)
{
   qurt_elite_mutex_destroy(&stats->lock);
   stats->num_mallocs = 0;
   stats->num_frees = 0;
   stats->curr_heap = 0;
   stats->peak_heap = 0;
}

/*
 *Function Name:mem_wrap_print_memleak_data
 *
 *Parameters: None
 *
 *Description: When DBG_MEM_LEAK is defined, prints
 *             the data contained in memleak data structures.
 *
 *Returns: None
 *
 */

void mem_wrap_print_memleak_data(void)
{
#ifdef DBG_MEM_LEAK
   /* Copy the memleak info into temporary memory. This is required because
      the printf's below also allocate memory which can go through the wrappers
      if MALLOC_WRAPPER is defined. Thus it is possible to corrupt the memleak
      info while printing it.
    */

   qurt_elite_mutex_lock(&mem_wrap_memleak_lock);
   uint32_t num_mallocs = ctr_malloc;
   uint32_t num_frees = ctr_free;
   memscpy(temp_dbg_malloc, sizeof(temp_dbg_malloc), dbg_malloc, sizeof(dbg_malloc));
   memscpy(temp_dbg_free, sizeof(temp_dbg_free), dbg_free, sizeof(dbg_free));
   qurt_elite_mutex_unlock(&mem_wrap_memleak_lock);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "num mallocs = %lu, num frees = %lu\n", num_mallocs, num_frees);
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "num malloc wraparound = %lu, num free wraparound= %lu\n", ctr_malloc_wrap, ctr_free_wrap);

   //print the most recent memory allocations
   uint32_t idx = num_mallocs;
   for(uint32_t i = 0; i < NUM_DBG_CTR; i++)
   {
      if(temp_dbg_malloc[idx].size != 0)
      {
	     /*Using printf in case this code is used with cosim test fwk.
		 With cosim test fwk, if there is no diag cosim, the MSG macros
		 would simply not print. Whereas, printf's simply appear on the console.
		 */
         printf("QURT_ELITE_MALLOC_FREE_TRACER: Malloc 0x%x    %lu     %s\n",
         temp_dbg_malloc[idx].ptr, temp_dbg_malloc[idx].size, temp_dbg_malloc[idx].thread_name);
      }

      idx = (idx + 1) & (NUM_DBG_CTR - 1);
   }

   //print the most recent memory frees
   idx = num_frees;
   for(uint32_t i = 0; i < NUM_DBG_CTR; i++)
   {
      if(temp_dbg_free[idx].size != 0)
      {
	     /*Using printf in case this code is used with cosim test fwk.
		 With cosim test fwk, if there is no diag cosim, the MSG macros
		 would simply not print. Whereas, printf's simply appear on the console.
		 */
         printf("QURT_ELITE_MALLOC_FREE_TRACER: Free 0x%x    %lu     %s\n",
         temp_dbg_free[idx].ptr, temp_dbg_free[idx].size, temp_dbg_free[idx].thread_name);
      }

      idx = (idx + 1) & (NUM_DBG_CTR - 1);
   }

#endif //DBG_MEM_LEAK
}

/*
 *Function Name:mem_wrap_empty_memleak_data
 *
 *Parameters: None
 *
 *Description: When DBG_MEM_LEAK is defined, empties the
 *             data contained in memleak data structures.
 *
 *Returns: None
 *
 */

void mem_wrap_empty_memleak_data(void)
{
#ifdef DBG_MEM_LEAK
   qurt_elite_mutex_lock(&mem_wrap_memleak_lock);

   ctr_malloc = 0;
   ctr_free = 0;
   ctr_malloc_wrap = 0;
   ctr_free_wrap  = 0;
   memset(dbg_malloc, 0, sizeof(dbg_malloc));
   memset(dbg_free, 0, sizeof(dbg_free));

   qurt_elite_mutex_unlock(&mem_wrap_memleak_lock);
#endif //DBG_MEM_LEAK
}

/*
 *Function Name:mem_wrap_init_memleak_data
 *
 *Parameters: None
 *
 *Description: When DBG_MEM_LEAK is defined, initializes
 *             memleak data structures.
 *
 *Returns: None
 *
 */

void mem_wrap_init_memleak_data(void)
{
#ifdef DBG_MEM_LEAK
   qurt_elite_mutex_init(&mem_wrap_memleak_lock);

   ctr_malloc = 0;
   ctr_free = 0;
   ctr_malloc_wrap = 0;
   ctr_free_wrap = 0;
   memset(dbg_malloc, 0, sizeof(dbg_malloc));
   memset(dbg_free, 0, sizeof(dbg_free));
#endif //DBG_MEM_LEAK
}

/*
 *Function Name:mem_wrap_init_memleak_data
 *
 *Parameters: None
 *
 *Description: When DBG_MEM_LEAK is defined, de-initializes
 *             memleak data structures.
 *
 *Returns: None
 *
 */

void mem_wrap_deinit_memleak_data(void)
{
#ifdef DBG_MEM_LEAK
   qurt_elite_mutex_destroy(&mem_wrap_memleak_lock);
#endif //DBG_MEM_LEAK
}

/*
 *Function Name:mem_wrap_get_block_size
 *
 *Parameters: ptr: Pointer to the memory
 *
 *Description: Returns the size of memory of a given pointer
 *
 *Returns: Size of memory
 *
 */
uint32_t mem_wrap_get_block_size(void *ptr)
{
  return(mem_get_block_size(&default_heap_state, ptr));
}

/*
*Function Name:mem_wrap_add_heap_section
 *
 *Parameters: 	heap_ptr : Pointer to statically allocated heap structure
						section_start : Pointer to contiguous block of memory used for this section
 *          			section_size  : Size of the section
 *
 *Description: Add a section to an existing heap.
 *
 *Returns: None
 *
*/
void mem_wrap_add_heap_section(mem_heap_type *heap_ptr, void *section_start, uint32_t section_size)
{
	if (heap_ptr == 0)
	{
		mem_heap_add_section( &default_heap_state, section_start, (unsigned long)section_size);
	}
	else
	{
		mem_heap_add_section( heap_ptr, section_start, (unsigned long)section_size);
	}
	return;
}

/*
 *Function Name: mem_wrap_update_alloc_stats
 *
 *Parameters: ptr: Pointer to the memory
 *
 *Description: Updates malloc statistics (malloc counters and heap usage)
 *
 *Returns: None
 *
 */
static void mem_wrap_update_alloc_stats(void *ptr, uint32_t size, mem_wrap_heap_stats_t *stats)
{
   if (NULL != stats)
   {
     // check if peak heap consumption has been surpassed, and record it.
      qurt_elite_mutex_lock(&stats->lock);

      stats->curr_heap += size;
      if (stats->curr_heap > stats->peak_heap)
      {
         stats->peak_heap = stats->curr_heap;
      }

      // increment the counter of mallocs.
      stats->num_mallocs ++;

      qurt_elite_mutex_unlock(&stats->lock);
   }

#ifdef DBG_MEM_LEAK
   qurt_elite_mutex_lock(&mem_wrap_memleak_lock);
   dbg_malloc[ctr_malloc].ptr = (uint32_t) ptr;
   dbg_malloc[ctr_malloc].thread_id = qurt_thread_get_id();

   /* The root task's name cannot be obtained through qurt_thread_get_name
   because in the beginning the UGP for that thread is not yet initialized when
   we call qurt_thread_get_name but qurt_thread_get_name seems to require it which
   leads to a crash. Hence manually copy "ROOT_TASK" as the thread name.
   */
   if(dbg_malloc[ctr_malloc].thread_id != 0)
      qurt_thread_get_name(dbg_malloc[ctr_malloc].thread_name, QURT_THREAD_ATTR_NAME_MAXLEN);
   else
      qurt_elite_strl_cpy(dbg_malloc[ctr_malloc].thread_name, "ROOT_TASK", QURT_THREAD_ATTR_NAME_MAXLEN);

   dbg_malloc[ctr_malloc].size = size;
   ctr_malloc = (ctr_malloc + 1) & (NUM_DBG_CTR - 1);
   // check if wraparound occured.
   if (0 == ctr_malloc)
   {
      ctr_malloc_wrap += 1;
   }
   qurt_elite_mutex_unlock(&mem_wrap_memleak_lock);
#endif //DBG_MEM_LEAK
   return;
}

/*
 *Function Name: mem_wrap_update_free_stats
 *
 *Parameters: ptr: Pointer to the memory
 *
 *Description: Updates free statistics (free counters and heap usage)
 *
 *Returns: None
 *
 */
static void mem_wrap_update_free_stats(void *ptr, uint32_t size, mem_wrap_heap_stats_t *stats)
{
   if (NULL != stats)
   {
      qurt_elite_mutex_lock(&stats->lock);

      stats->curr_heap -= size;
      stats->num_frees ++;

      qurt_elite_mutex_unlock(&stats->lock);
   }

#ifdef DBG_MEM_LEAK
   qurt_elite_mutex_lock(&mem_wrap_memleak_lock);
   dbg_free[ctr_free].ptr = (uint32_t) ptr;
   dbg_free[ctr_free].thread_id = qurt_thread_get_id();

   /* The root task's name cannot be obtained through qurt_thread_get_name
   because in the beginning the UGP for that thread is not yet initialized when
   we call qurt_thread_get_name but qurt_thread_get_name seems to require it which
   leads to a crash. Hence manually copy "ROOT_TASK" as the thread name.
   */
   if (dbg_free[ctr_free].thread_id != 0)
      qurt_thread_get_name(dbg_free[ctr_free].thread_name, QURT_THREAD_ATTR_NAME_MAXLEN);
   else
      qurt_elite_strl_cpy(dbg_free[ctr_free].thread_name, "ROOT_TASK", QURT_THREAD_ATTR_NAME_MAXLEN);

   dbg_free[ctr_free].size = size;
   ctr_free = (ctr_free + 1) & (NUM_DBG_CTR - 1);
   // check if wraparound occured.
   if (0 == ctr_free)
   {
      ctr_free_wrap += 1;
   }
   qurt_elite_mutex_unlock(&mem_wrap_memleak_lock);
#endif //DBG_MEM_LEAK

   return;
}

#ifndef MALLOC_WRAPPER

/* These definitions are required to compile qurt_elite if
   MALLOC_WRAPPER is not used */
void *__real_malloc (size_t size)
{
   return malloc(size);
}

void *__real_realloc (void *ptr, size_t size)
{
   return realloc(ptr, size);
}

void *__real_calloc (size_t num_elem, size_t elem_size)
{
   return calloc(num_elem, elem_size);
}

void __real_free (void *ptr)
{
   free(ptr);
}

#endif //MALLOC_WRAPPER

} //extern "C"
