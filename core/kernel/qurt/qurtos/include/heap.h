#ifndef HEAP_H
#define HEAP_H
/*=============================================================================
                heap.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  Kernel heap functions
			
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


$Header: //components/rel/core.adsp/2.2/kernel/qurt/qurtos/include/heap.h#6 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/

void qurtos_heap_init(void );
void * qurtos_heap_alloc(void);
void qurtos_heap_free( void * buf );

#endif /* HEAP_H */
