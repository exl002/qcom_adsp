/*===========================================================================
Copyright (c) 2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */
/**
@file memory_wrappers.h

@brief This file contains wrapper functions for malloc, free, calloc, realloc
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/elite/qurt_elite/inc/memory_wrappers.h#12 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw      (Tech Pubs) Edited Doxygen comments and markup for 2.0.
11/03/11   rkc      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "mmdefs.h"
#include "qurt_elite_mutex.h"

#ifndef __MEMORY_WRAPPERS_H__
#define __MEMORY_WRAPPERS_H__

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup qurt_elite_memory_wrappers
@{ */

/** Structure for heap memory statistics.
*/
typedef struct
{
   qurt_elite_mutex_t            lock;
   /**< Mutex used to serialize access to the other variables in this
        structure. */

   uint32_t                      num_mallocs;
   /**< Number of mallocs that happened from the beginning. */

   uint32_t                      num_frees;
   /**< Number of frees that happened from the beginning. */

   uint32_t                      curr_heap;
   /**< Current heap consumption in bytes. */

   uint32_t                      peak_heap;
   /**< Peak heap consumption in bytes since the beginning 
        or since the last reset of peak heap. */
} mem_wrap_heap_stats_t;

/** Structure that holds heap statistics for all memory allocations and
    deallocations that go through the wrapper functions:
    - __wrap_malloc()
    - __wrap_calloc()
    - __wrap_realloc()
    - __wrap_free()
 */
extern mem_wrap_heap_stats_t mem_wrap_heap_stats;

/**
  Helper function used to allocate memory and keep track of heap usage.

  @datatypes
  mem_wrap_heap_stats_t

  @param[in]     size   Size in bytes of memory to allocate.
  @param[in,out] stats  Pointer to the heap statistics structure.

  @return
  Pointer to the allocated memory block. @newpage
 */
void *mem_wrap_alloc_track(uint32_t size, mem_wrap_heap_stats_t *stats);

/**
  Helper function used to free memory and keep track of heap usage.

  @datatypes
  mem_wrap_heap_stats_t

  @param[in] ptr        Pointer to the memory to deallocate. 
  @param[in,out] stats  Pointer to mem_wrap_heap_stats_t.

  @return
  None.

  @dependencies
  None.
 */
void mem_wrap_free_track(void *ptr, mem_wrap_heap_stats_t *stats);

/**
  Sets the peak heap usage equal to the current heap usage.

  @datatypes
  mem_wrap_heap_stats_t

  @param[in,out] stats  Pointer to mem_wrap_heap_stats_t.

  @return
  None.

  @dependencies
  None. @newpage
 */
void mem_wrap_reset_peak_heap(mem_wrap_heap_stats_t *stats);

/**
  Initializes heap use statistics to zero.

  @datatypes
  mem_wrap_heap_stats_t

  @param[in,out] stats  Pointer to mem_wrap_heap_stats_t.

  @return
  None.

  @dependencies
  None.
 */
void mem_wrap_init_heap_stats(mem_wrap_heap_stats_t *stats);

/**
  Deinitializes heap use statistics.

  @datatypes
  mem_wrap_heap_stats_t

  @param[in,out] stats  Pointer to mem_wrap_heap_stats_t.

  @return
  None.

  @dependencies
  None. @newpage
 */
void mem_wrap_deinit_heap_stats(mem_wrap_heap_stats_t *stats);

/**
  When DBG_MEM_LEAK is defined, prints the data contained in memleak data
  structures. 

  @return
  None.

  @dependencies
  None.
 */
void mem_wrap_print_memleak_data(void);

/**
  When DBG_MEM_LEAK is defined, empties the data contained in memleak data
  structures. 

  @return
  None.

  @dependencies
  None.
 */
void mem_wrap_empty_memleak_data(void);

/**
  When DBG_MEM_LEAK is defined, initializes memleak data structures. 

  @return
  None.

  @dependencies
  None. @newpage
 */
void mem_wrap_init_memleak_data(void);

/**
  When DBG_MEM_LEAK is defined, deinitializes memleak data structures. 

  @return
  None.

  @dependencies
  None.
 */
void mem_wrap_deinit_memleak_data(void);

/**
  Returns the memory size of a given pointer. This pointer must be the same
  pointer that is returned by malloc, calloc, or realloc without any
  modifications.

  @param[in] ptr    Pointer to the memory. 
  
  @return
  Size of memory.

  @dependencies
  None. @newpage
*/
uint32_t mem_wrap_get_block_size(void *ptr);

/** @} */ /* end_addtogroup qurt_elite_memory_wrappers */

#ifdef __cplusplus
} //extern "C"
#endif //__cplusplus

#endif //__MEMORY_WRAPPERS_H__

