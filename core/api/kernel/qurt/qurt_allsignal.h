
#ifndef QURT_ALLSIGNAL_H
#define QURT_ALLSIGNAL_H

/**
  @file  qurt_allsignal.h
  @brief  Prototypes of Kernel signal API functions      

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
 
 	$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_allsignal.h#11 $ 
 	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
  2012-12-16    cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/
#include <qurt_futex.h>

/** @addtogroup all_signal_types
@{ */
/*=====================================================================
 Typedefs
 ======================================================================*/

/** qurt_allsignal type.         
 The caller of signal_wait can only be woken up when all bits in the mask are set. */
typedef union {
    /** @cond */
	unsigned long long int raw;
	struct {
		unsigned int waiting;      /**< */
		unsigned int signals_in;   /**< */
		unsigned int queue;        /**< */
		unsigned int reserved;     /**< */
	}X;
    /** @endcond */
} qurt_allsignal_t;
/** @} */ /* end_addtogroup chapter_all_signal */

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**@ingroup func_qurt_allsignal_init
  Initializes an all-signal object.\n
  The all-signal object is initially cleared.

  @datatypes
  #qurt_allsignal_t

  @param[out] signal Pointer to the all-signal object to initialize. 
  
  @return         
  None.

  @dependencies    
  None.
 */
/* ======================================================================*/
void qurt_allsignal_init(qurt_allsignal_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_allsignal_destroy
  Destroys the specified all-signal object.\n
  @note1hang All-signal objects must be destroyed when they are no longer in use. 
             Failure to do this causes resource leaks in the QuRT kernel.  \n
  @note1cont All-signal objects must not be destroyed while they are still in use. 
             If this happens the behavior of QuRT is undefined.
  
  @datatypes
  #qurt_allsignal_t

  @param[in] signal Pointer to the all-signal object to destroy.

  @return         
  None.
 
  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_allsignal_destroy(qurt_allsignal_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_allsignal_get
  Gets signal values from the all-signal object.

  Returns the current signal values of the specified all-signal object.

  @datatypes
  #qurt_allsignal_t

  @param[in] signal Pointer to the all-signal object to access.

  @return         
  Bitmask with current signal values.
    
  @dependencies
  None.
*/
/* ======================================================================*/
static inline unsigned int qurt_allsignal_get(qurt_allsignal_t *signal)
{ return signal->X.signals_in; };
    
/*======================================================================*/
/**@ingroup func_qurt_allsignal_wait
  Waits on the all-signal object.n
  Suspends the current thread until all of the specified signals are set.
  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1
  indicates that a signal is to be waited on, and 0 that it is not to be waited on.

  If a signal is set in an all-signal object, and a thread is waiting on the all-signal object for
  that signal, then the thread is awakened. If the awakened thread has higher priority than
  the current thread, a context switch may occur.

  Unlike any-signals, all-signals do not need to explicitly clear any set signals in an all-signal
  object before waiting on them again -- clearing is done automatically by the wait
  operation.
	
  @note1hang At most one thread can wait on an all-signal object at any given time.
             Because signal clearing is done by the wait operation, no clear operation is
             defined for all-signals.

  @datatypes
  #qurt_allsignal_t
  
  @param[in] signal Pointer to the all-signal object to wait on.
  @param[in] mask	Signal mask value identifying the individual signals in the all-signal object
                    to be waited on.
 
  @return
  None.
 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_allsignal_wait(qurt_allsignal_t *signal, unsigned int mask);

/*======================================================================*/
/**@ingroup func_qurt_allsignal_set
  Set signals in the specified all-signal object.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit 
  value of 1 indicates that a signal is to be set, and 0 that it is not to be set.

  @datatypes
  #qurt_allsignal_t

  @param[in]	signal  Pointer to the all-signal object to be modified. 
  @param[in]	mask 	Signal mask value identifying the individual signals to be 
                        set in the all-signal object.
	
  @return
  None.
 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_allsignal_set(qurt_allsignal_t *signal, unsigned int mask);


#endif /* QURT_ALLSIGNAL_H */

