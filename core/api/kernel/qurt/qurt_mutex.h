#ifndef QURT_MUTEX_H
#define QURT_MUTEX_H
/**
  @file qurt_mutex.h 
  @brief   Prototypes of qtypes API.  
   This is mostly a user space mutex, but calls the 
   kernel to block if the mutex is taken 

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


$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_mutex.h#11 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
12/17/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup. 
=============================================================================*/
#include <qurt_futex.h>

/** @addtogroup mutex_types
@{ */
/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** QuRT mutex type.                                       
  
   Both non-recursive mutex lock/unlock and recursive
   mutex lock/unlock can be applied to this type.
 */
typedef union qurt_mutex_aligned8{
    /** @cond */
    struct {
        unsigned int holder; 
        unsigned int count;
        unsigned int queue;
        unsigned int wait_count;
    };
    unsigned long long int raw;
    /** @endcond */
} qurt_mutex_t;
/** @} */ /* end_addtogroup mutex_types */
/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
/** @addtogroup mutex_const_macros
@{ */
#define MUTEX_MAGIC 0xfe                             /**< */
#define QURTK_FUTEX_FREE_MAGIC     0x1F   // 11111   /**< */
#define QURT_MUTEX_INIT {{MUTEX_MAGIC, 0, QURTK_FUTEX_FREE_MAGIC, 0}}   /**< Suitable as an initializer for a
                                                                        variable of type qurt_mutex_t. */
/** @} */ /* end_addtogroup mutex_const_macros */
/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_mutex_init
  Initializes a mutex object.  
  The mutex is initially unlocked.
  
  @note1hang Each mutex-based object has an associated kernel
             resource(s); be sure to call the destroy function when this object
             is not used anymore 
  
  @datatypes
  #qurt_mutex_t
  
  @param[out]  lock  Pointer to the mutex object. Returns the initialized object.

  @return
  None.

  @dependencies
  None.
  
 */
void qurt_mutex_init(qurt_mutex_t *lock);

/**@ingroup func_qurt_mutex_destroy
   Destroys the specified mutex. 

   @note1hang Mutexes must be destroyed when they are no longer in use. Failure to do this
              causes resource leaks in the QuRT kernel.\n
   @note1cont Mutexes must not be destroyed while they are still in use. If this happens the
              behavior of QuRT is undefined. 
  
   @datatypes
   #qurt_mutex_t
   
   @param[in]  lock  Pointer to the mutex object to destroy.

   @return 
   None.

   @dependencies
   None.
  
 */
void qurt_mutex_destroy(qurt_mutex_t *lock); 

/**@ingroup func_qurt_mutex_lock
   Locks the specified mutex.  
   If a thread performs a lock operation on a mutex that is not being used, the thread gains
   access to the shared resource that is protected by the mutex, and continues executing.

   If a thread performs a lock operation on a mutex that is already being used by another
   thread, the thread is suspended. When the mutex becomes available again (because the
   other thread has unlocked it), the thread is awakened and given access to the shared
   resource.

   @note1hang A thread is suspended indefinitely if it locks a mutex that it has already
           locked. This can be avoided by using recursive mutexes (Section @xref{dox:recursive_mutexes}).  
  
   @datatypes
   #qurt_mutex_t
   
   @param[in]  lock  Pointer to the mutex object. Specifies the mutex to lock.

   @return 
   None.

   @dependencies
   None.  
 */
void qurt_mutex_lock(qurt_mutex_t *lock);		/* blocking */

/**@ingroup func_qurt_mutex_unlock
  Unlocks the specified mutex.  \n
  More than one thread can be suspended on a mutex. When the mutex is unlocked, only the
  highest-priority thread waiting on the mutex is awakened. If the awakened thread has
  higher priority than the current thread, a context switch occurs.

  @note1hang The behavior of QuRT is undefined if a thread unlocks a mutex it did not first
              lock.  
  
   @datatypes
   #qurt_mutex_t
   
   @param[in]  lock  Pointer to the mutex object. Specifies the mutex to unlock. 

   @return
   None.

   @dependencies
   None.  
 */
void qurt_mutex_unlock(qurt_mutex_t *lock);	/* unlock */

/**@ingroup func_qurt_mutex_try_lock
   Attempts to lock the specified mutex. 
   If a thread performs a try_lock operation on a mutex that is not being used, the thread gains
   access to the shared resource that is protected by the mutex, and continues executing.

   @note1hang If a thread performs a try_lock operation on a mutex that it has already locked 
              or is in use by another thread, qurt_mutex_try_lock immediately returns with a 
              nonzero result value.
   
  
   @datatypes
   #qurt_mutex_t
   
   @param[in]  lock  Pointer to the mutex object. Specifies the mutex to lock.

   @return 
   0 -- Success. \n 
   Nonzero -- Fail.
  
  @dependencies
  None.
 */
int qurt_mutex_try_lock(qurt_mutex_t *lock);	

#endif /* QURT_MUTEX_H */

