#ifndef QURT_BARRIER_H
#define QURT_BARRIER_H

/**
  @file qurt_barrier.h
  @brief  Prototypes of Kernel barrier  API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/qurt/include/public/qurt_barrier.h#8 $ 
 	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
  2012-12-14    cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/
/** @addtogroup barrier_types
@{ */
/*=====================================================================
 Constants and macros
======================================================================*/
#define QURT_BARRIER_SERIAL_THREAD 1
#define QURT_BARRIER_OTHER 0

#ifndef ASM
#include <qurt_mutex.h>

/*=====================================================================
Typedefs
======================================================================*/

/** qurt_barrier type.                                                 
 
  All threads are woken up only when all threads have reached the
  qurt_barrier_wait call.
 */
typedef union {
    /** @cond */
	struct {
		unsigned short threads_left;
		unsigned short count;
		unsigned int threads_total;
        unsigned int queue;
        unsigned int reserved;
	};
	unsigned long long int raw;
    /** @endcond */
} qurt_barrier_t;

/** @} */ /* end_addtogroup barrier_types */

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**@ingroup func_qurt_barrier_init
  Initializes a barrier object.

  @datatypes
  #qurt_barrier_t
	
  @param[out] barrier       Pointer to the barrier object to initialize.
  @param[in]  threads_total Total number of threads to synchronize on the barrier.


  @return
  Unused integer value.

  @dependencies
  None.
*/
/* ======================================================================*/
int qurt_barrier_init(qurt_barrier_t *barrier, unsigned int threads_total);

/*======================================================================*/
/**@ingroup func_qurt_barrier_destroy
  Destroys the specified barrier.

  @note1hang Barriers must be destroyed when they are no longer in use. Failure
             to do this causes resource leaks in the QuRT kernel.\n
  @note1cont Barriers must not be destroyed while they are still in use. If this
             happens the behavior of QuRT is undefined.
	
  @datatypes
  #qurt_barrier_t
 
  @param[in] barrier Pointer to the barrier object to destroy.

  @return     		
  Unused integer value.

  @dependencies
  None.
*/
/* ======================================================================*/
int qurt_barrier_destroy(qurt_barrier_t *barrier);

/*======================================================================*/
/**@ingroup func_qurt_barrier_wait
  Waits on the barrier.\n
  Suspends the current thread on the specified barrier. \n
  The function return value indicates whether or not the thread was the last one to
  synchronize on the barrier.
  When a thread waits on a barrier, it is suspended on the barrier: \n
  - If the total number of threads waiting on the barrier is less than the barrier's
     assigned value, no other action occurs.\n
  - If the total number of threads waiting on the barrier equals the barrier's assigned
     value, all threads currently waiting on the barrier are awakened, allowing them to
     execute past the barrier.

  @note1hang After its waiting threads are awakened, a barrier is automatically reset 
            and can be used again in the program without the need for re-initialization.
	                
  @datatypes
  #qurt_barrier_t
	                
  @param[in] barrier Pointer to the barrier object to wait on.

	@return 				
  QURT_BARRIER_OTHER -- Current thread awakened from barrier. \n 
  QURT_BARRIER_SERIAL_THREAD -- Current thread is last caller of barrier.

  @dependencies
  None.
*/
/* ======================================================================*/
int qurt_barrier_wait(qurt_barrier_t *barrier);


#endif

#endif /* QURT_BARRIER_H */

