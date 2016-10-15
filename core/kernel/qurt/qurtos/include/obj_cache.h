#ifndef OBJ_CACHE_H
#define OBJ_CACHE_H
/*=============================================================================
                obj_cache.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  Kernel obj cache functions
			
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


$Header: //components/rel/core.adsp/2.2/kernel/qurt/qurtos/include/obj_cache.h#6 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/

#include <qurtk_stddef.h>
#include <qurt_mutex.h>
#include <buf.h>

typedef struct {
    qurt_mutex_t lock;    // lock is used when allocating from kernel heap
    void *free_obj_list;// free object LIFO list
    volatile unsigned short pgnum;  // total number of pages allocated
    unsigned short iskernel;        // objects are in kernel memory if this is set
    void (*init)(void **, void *);  // init objects of specific type in new page
}qurtos_obj_cache_t;


void qurtos_obj_cache_init(qurtos_obj_cache_t *cache, void (*func_ptr)(void **, void *)); 

void * qurtos_obj_cache_alloc(qurtos_obj_cache_t *cache); 

void qurtos_obj_cache_free(qurtos_obj_cache_t *cache, void *obj);

#endif /* OBJ_CACHE_H */

