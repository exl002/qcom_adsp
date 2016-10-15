#ifndef ALLOC_H
#define ALLOC_H
/*=============================================================================

                alloc.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  QuRT OS memory region/section allocation functions  
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/qurtos/include/alloc.h#8 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/
#include <qurtos_memory.h>

#define size_round_up(size,alignment) ((((size) + ((alignment)-1)) & (~((alignment)-1))))

#define INVALID_ADDR 0xFFFFFFFFu

mem_t *qurtos_mem_alloc_pages(mem_pool_t *pool,
                              uint32_t page_count,
                              uint32_t pageno_c,
                              unsigned flags);
#define QURTOS_ALLOC_FIXED_ONLY     1
#define QURTOS_ALLOC_FIXED          2
#define QURTOS_ALLOC_RANDOM         4

void qurtos_mem_free(struct mem_pool *pool, mem_t *mem);

/*
 * Specialised functions.
 */
int mem_add_pages(mem_pool_t *pool,
                  uint32_t pageno,
                  uint32_t page_count);

void mem_pool_split(struct mem_pool *src_pool, struct mem_pool *dest_pool, uint32_t pageno_start, uint32_t pageno_end);

#endif /* ALLOC_H */
