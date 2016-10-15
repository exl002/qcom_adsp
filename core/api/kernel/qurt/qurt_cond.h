#ifndef QURT_COND_H
#define QURT_COND_H 
/**
  @file qurt_cond.h
  @brief Prototypes of Kernel condition variable object  API functions      

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
 
 	$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_cond.h#11 $ 
 	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
  2012*12-15    cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/
#include <qurt_mutex.h>
#include <qurt_rmutex2.h>

/** @addtogroup condition_variables_types
@{ */
/*=====================================================================
 Typedefs
 ======================================================================*/

/** QuRT condition variable type.  */
typedef union {
    /** @cond */
	unsigned long long raw;
	struct {
		unsigned int count;
		unsigned int n_waiting;
        unsigned int queue;
        unsigned int reserved;
	}X;
    /** @endcond */
} qurt_cond_t;

/** @} */ /* end_addtogroup condition_variables_types */

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**@ingroup func_qurt_cond_init
  Initializes a conditional variable object.

  @datatypes
  #qurt_cond_t
	
  @param[out] cond Pointer to the initialized condition variable object. 

  @return
  None.

  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_cond_init(qurt_cond_t *cond);

/*======================================================================*/
/**@ingroup func_qurt_cond_destroy
  Destroys the specified condition variable.

  @note1hang Conditions must be destroyed when they are no longer in use. Failure to do
             this causes resource leaks in the QuRT kernel.\n
  @note1cont Conditions must not be destroyed while they are still in use. If this happens
             the behavior of QuRT is undefined. 
	
  @datatypes
  #qurt_cond_t

  @param[in] cond Pointer to the condition variable object to destroy.

  @return
  None.
		 
 */
/* ======================================================================*/
void qurt_cond_destroy(qurt_cond_t *cond);

/*======================================================================*/
/**@ingroup func_qurt_cond_signal
  Signals a waiting thread that the specified condition is true. \n

  When a thread wishes to signal that a condition is true on a shared data item, it must
  perform the following procedure: \n
  1. Lock the mutex that controls access to the data item. \n
  2. Perform the signal condition operation. \n
  3. Unlock the mutex.

  @note1hang Failure to properly lock and unlock a condition variable's mutex may cause
             the threads to never be suspended (or suspended but never awakened).
	
  @datatypes
  #qurt_cond_t

  @param[in] cond Pointer to the condition variable object to signal.

  @return
  None.
		 
  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_cond_signal(qurt_cond_t *cond);

/*======================================================================*/
/**@ingroup func_qurt_cond_broadcast
  Signals multiple waiting threads that the specified condition is true.\n
  When a thread wishes to broadcast that a condition is true on a shared data item, it must
  perform the following procedure: \n
  1. Lock the mutex that controls access to the data item. \n
  2. Perform the broadcast condition operation. \n
  3. Unlock the mutex.\n

  @note1hang Failure to properly lock and unlock a condition variable's mutex may cause
             the threads to never be suspended (or suspended but never awakened).

  @datatypes
  #qurt_cond_t

  @param[in] cond Pointer to the condition variable object to signal.

  @return
  None.
		 
  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_cond_broadcast(qurt_cond_t *cond);

/*======================================================================*/
/**@ingroup func_qurt_cond_wait
  Suspends the current thread until the specified condition is true.
  When a thread wishes to wait for a specific condition on a shared data item, it must
  perform the following procedure: \n
  1. Lock the mutex that controls access to the data item. \n
  2. If the condition is not satisfied, perform the wait condition operation on the
  condition variable (which both suspends the thread and unlocks the mutex).
	
  @note1hang Failure to properly lock and unlock a condition variable's mutex may cause
             the threads to never be suspended (or suspended but never awakened).

  @datatypes
  #qurt_cond_t \n
  #qurt_mutex_t
  
  @param[in] cond     Pointer to the condition variable object to wait on.
  @param[in] mutex    Pointer to the mutex associated with condition variable to wait on.

  @return
  None.
		 
  @dependencies 
  None.
 */
/* ======================================================================*/
void qurt_cond_wait(qurt_cond_t *cond, qurt_mutex_t *mutex);

/*======================================================================*/
/**@ingroup func_qurt_cond_wait2
  Suspends the current thread until the specified condition is true.
  When a thread wishes to wait for a specific condition on a shared data item, it must
  perform the following procedure: \n
  1. Lock the mutex that controls access to the data item. \n
  2. If the condition is not satisfied, perform the wait condition operation on the
  condition variable (which both suspends the thread and unlocks the mutex).
	
  @note1hang Failure to properly lock and unlock a condition variable's mutex may cause
             the threads to never be suspended (or suspended but never awakened).
  @note1cont This is the same API as qurt_cond_wait(), but this version should be used
             if the mutex being used is of type qurt_rmutex2_t.

  @datatypes
  #qurt_cond_t \n
  #qurt_rmutex2_t
  
  @param[in] cond     Pointer to the condition variable object to wait on.
  @param[in] mutex    Pointer to the mutex associated with condition variable to wait on.

  @return
  None.
		 
  @dependencies 
  None.
 */
/* ======================================================================*/
void qurt_cond_wait2(qurt_cond_t *cond, qurt_rmutex2_t *mutex);

#endif /* QURT_COND_H */

