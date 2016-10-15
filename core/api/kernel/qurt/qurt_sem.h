#ifndef QURT_SEM_H
#define QURT_SEM_H 
/**
  @file  qurt_sem.h 
  @brief  Prototypes of Semaphore API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2010 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_sem.h#11 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file 
12/17/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/
#include <qurt_futex.h>

/*=============================================================================
												TYPEDEFS
=============================================================================*/
/** @addtogroup semaphore_types
@{ */

/** QuRT semaphore type.   */
typedef union {
    /** @cond */
	unsigned int raw[2];
	struct {
		unsigned short val;        /**< */
		unsigned short n_waiting;  /**< */
        unsigned int reserved1;    /**< */
        unsigned int queue;        /**< */
        unsigned int reserved2;    /**< */
	}X; /** @endcond */   
} qurt_sem_t;
/** @} */ /* end_addtogroup semaphore_types */
/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_sem_add
  Releases access to a shared resource (incrementing the semaphore count value by the
  specified amount).\n
  When a thread performs an add operation on a semaphore, the semaphore count value is
  incremented by the specified value. The result depends on the number of threads waiting
  on the semaphore: \n
  - If no threads are waiting, the current thread releases access to the shared resource
     and continues executing. \n
  - If one or more threads are waiting and the semaphore count value is nonzero, then
     the kernel repeatedly awakens the highest-priority waiting thread and decrements
     the semaphore count value until either no waiting threads remain or the
     semaphore count value is zero. If any of the awakened threads has higher priority
     than the current thread, a context switch may occur.

   @datatypes
   #qurt_sem_t
   
   @param[in]  sem  Pointer to the semaphore object to access. 
   @param[in]  amt  Amount to increment the semaphore count value. 

   @return 
   Unused integer value.

   @dependencies 
   None.
  
 */
int qurt_sem_add(qurt_sem_t *sem, unsigned int amt);

/**@ingroup func_qurt_sem_up
  When a thread performs an up operation on a semaphore, the semaphore count value is
  incremented. The result depends on the number of threads waiting on the semaphore: \n
  - If no threads are waiting the current thread releases access to the shared resource
     and continues executing.\n
  - If one or more threads are waiting and the semaphore count value is nonzero, then
     the kernel awakens the highest-priority waiting thread and decrements the
     semaphore count value. If the awakened thread has higher priority than the current
     thread, a context switch may occur.

   @datatypes
   #qurt_sem_t
   
   @param[in]  sem  Pointer to the semaphore object to access.

   @return 
   Unused integer value.

   @dependencies
   None.  
 */
static inline int qurt_sem_up(qurt_sem_t *sem) { return qurt_sem_add(sem,1); };

/**@ingroup func_qurt_sem_down
  When a thread performs a down operation on a semaphore, the result depends on the
  semaphore count value: \n
  - If the count value is nonzero it is decremented, and the thread gains access to the
     shared resource and continues executing.\n
  - If the count value is zero it is not decremented, and the thread is suspended on the
     semaphore. When the count value becomes nonzero (because another thread
     released the semaphore) it is decremented, and the suspended thread is awakened
     and gains access to the shared resource.
  
   @datatypes
   #qurt_sem_t
   
   @param[in]  sem  Pointer to the semaphore object to access.

   @return 
   Unused integer value.

   @dependencies
   None.
 */
int qurt_sem_down(qurt_sem_t *sem);

/**@ingroup func_qurt_sem_try_down
  When a thread performs a try down operation on a semaphore, the result depends on the
  semaphore count value: \n
  - If the count value is nonzero it is decremented. The down operation returns 0 as
     the function result, and the thread gains access to the shared resource and is free to
     continue executing.\n
  - If the count value is zero it is not decremented. The down operation returns -1
     as the function result, and the thread does not gain access to the shared resource
     and should not continue executing.
 
   @datatypes
   #qurt_sem_t
   
   @param[in]  sem  Pointer to the semaphore object to access. 

   @return 
   0 -- Success. \n
   -1 -- Failure. 

   @dependencies
   None.
   
 */
int qurt_sem_try_down(qurt_sem_t *sem);

/**@ingroup func_qurt_sem_init
  Initializes a semaphore object.
  The default initial value of the semaphore count value is 1.

  @param[out]  sem  Pointer to the initialized semaphore object.

  @return 
  None.

  @dependencies
  None.
  
 */
void qurt_sem_init(qurt_sem_t *sem);

/**@ingroup func_qurt_sem_destroy
  Destroys the specified semaphore.\n
  @note1hang Semaphores must be destroyed when they are no longer in use. Failure to do
             this causes resource leaks in the QuRT kernel.\n
  @note1cont Semaphores must not be destroyed while they are still in use. If this happens
             the behavior of QuRT is undefined.

  @datatypes
  #qurt_sem_t

  @param[in]  sem  Pointer to the semaphore object to destroy. 
 
  @return
  None.

  @dependencies
  None.
 */
void qurt_sem_destroy(qurt_sem_t *sem);

/**@ingroup func_qurt_sem_init_val
  Initializes a semaphore object.

   @datatypes
   #qurt_sem_t
   
   @param[out]  sem  Pointer to the initialized semaphore object. 
   @param[in]  val   Initial value of the semaphore count value.

   @return
   None.

   @dependencies
   None.
  
 */
void qurt_sem_init_val(qurt_sem_t *sem, unsigned short val);

/**@ingroup func_qurt_sem_get_val
  Gets the semaphore count value.\n
  Returns the current count value of the specified semaphore.

  @datatypes
  #qurt_sem_t
  
  @param[in]   sem Pointer to the semaphore object to access.

  @return
  Integer semaphore count value

  @dependencies
  None.
 */
static inline unsigned short qurt_sem_get_val(qurt_sem_t *sem ){return sem->X.val;}


#endif /* QURT_SEM_H  */

