#ifndef QURT_SIGNAL2_H
#define QURT_SIGNAL2_H

/**
  @file qurt_signal2.h
  @brief  Prototypes of kernel signal2 API functions.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2012 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_signal2.h#7 $ 
 	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
 
  when         who     what, where, why
  ----------   ---     ------------------------------------------------
  2012-03-29   sho     Creation
 ======================================================================*/

#define QURT_SIGNAL_ATTR_WAIT_ANY 0x00000000
#define QURT_SIGNAL_ATTR_WAIT_ALL 0x00000001

/*=====================================================================
 Typedefs
 ======================================================================*/

/** @addtogroup signals_types
@{ */
/** qurt_signal2 type                                           
 */
typedef struct {
   /** @cond */
   unsigned int cur_mask __attribute__((aligned(8)));  /* Current set of signal bits that are set */
   unsigned int sig_state;                             /* Current state */
                                                       /*   Bit 0 = currently in "anysignal" wait */
                                                       /*   Bit 1 = currently in "allsignal" wait */
                                                       /*   Bit 2 = currently in "interrupt" wait */
                                                       /*   Bits 31-3 = reference count field */
   unsigned int queue;                                 /* Kernel-maintained "futex queue" value */
   unsigned int wait_mask;                             /* If sig_state indicates a waiter is present, this is the wait mask */
   /** @endcond */
} qurt_signal2_t;
/** @} */ /* end_addtogroup signals_types */

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**@ingroup func_qurt_signal2_init
  Initializes a signal2 object.
  Signal returns the initialized object.
  The signal object is initially cleared.
  
  Objects of type signal2 solve a potential race condition between
  set() and destroy() operations.

  @datatypes
  #qurt_signal2_t

  @param[in] *signal Pointer to the initialized object.

  @return         
  None.
     
  @dependencies
  Each mutex-based object has an associated 
       kernel resource(s), therefore users should make sure to call qurt_signal2_destroy()
       when this object is not used anymore 
 */
/* ======================================================================*/
void qurt_signal2_init(qurt_signal2_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_signal2_destroy
  Destroys the specified signal object.

  @note1cont Signal objects must not be destroyed while they are still in use. If this
  happens the behavior of QuRT is undefined.
  @note1cont In general, application code should "destroy" a signal2 object prior to
             deallocating it; calling qurt_signal2_destroy() before deallocating it ensures
             that all qurt_signal2_set() calls have completed.
  
  @datatypes
  #qurt_signal2_t

  @param[in] *signal  Pointer to the signal object to destroy.

  @return
  None.
 
  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_signal2_destroy(qurt_signal2_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_signal2_wait    
  Suspends the current thread until the specified signals are set.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates 
  that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for any of the specified set of signals to be set, 
  and one or more of those signals is set in the signal object, then the thread is awakened.

  If a thread is waiting on a signal object for all of the specified set of signals to be set, 
  and all of those signals are set in the signal object, then the thread is awakened.

  @note1hang At most one thread can wait on a signal object at any given time.
	
  @datatypes
  #qurt_signal2_t

  @param[in] signal      Pointer to the signal object to wait on.
  @param[in] mask        Mask value which identifying the individual signals in the signal object to be 
                         waited on.
  @param[in] attribute   Specifies whether the thread waits for any of the signals to be set, or for all of 
                         them to be set. Values:\n
                         QURT_SIGNAL_ATTR_WAIT_ANY \n
                         QURT_SIGNAL_ATTR_WAIT_ALL
  @return     	
  A 32-bit word with current signals
 
  @dependencies
  None.
*/
/* ======================================================================*/
unsigned int qurt_signal2_wait(qurt_signal2_t *signal, unsigned int mask, 
                unsigned int attribute);

/*======================================================================*/
/**@ingroup func_qurt_signal2_wait_any
  Suspends the current thread until any of the specified signals are set.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates
  that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for any of the specified set of signals to be set, 
  and one or more of those signals is set in the signal object, then the thread is awakened.

  @note1hang At most one thread can wait on a signal object at any given time.

  @datatypes
  #qurt_signal2_t
	
  @param[in] signal      Pointer to the signal object to wait on.
  @param[in] mask        Mask value identifying the individual signals in the signal object to be 
                         waited on.

  @return     	
  A 32 bit word with current signals.
 
  @dependencies
  None.
*/
/* ======================================================================*/
static inline unsigned int qurt_signal2_wait_any(qurt_signal2_t *signal, unsigned int mask)
{
  return qurt_signal2_wait(signal, mask, QURT_SIGNAL_ATTR_WAIT_ANY);
}

/*======================================================================*/
/**@ingroup func_qurt_signal2_wait_all
  Suspends the current thread until all of the specified signals are set.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates 
  that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for all of the specified set of signals to be set, 
  and all of those signals are set in the signal object, then the thread is awakened.

  @note1hang At most one thread can wait on a signal object at any given time.

  @datatypes
  #qurt_signal2_t
	
  @param[in] signal      Pointer to the signal object to wait on. 
  @param[in] mask        Mask value which identifies the individual signals in the signal object to be 
                         waited on.

  @return      	
  A 32 bit word with current signals.
 
  @dependencies
  None.
*/
/* ======================================================================*/
static inline unsigned int qurt_signal2_wait_all(qurt_signal2_t *signal, unsigned int mask)
{
  return qurt_signal2_wait(signal, mask, QURT_SIGNAL_ATTR_WAIT_ALL);
}

/*======================================================================*/
/**@ingroup func_qurt_signal2_set
  Sets signals in the specified signal object.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates 
  that a signal is to be set, and 0 that it is not to be set.
	
  @datatypes
  #qurt_signal2_t

  @param[in]	signal  Pointer to the signal object to be modified.
  @param[in]	mask 	Mask value identifying the individual signals to be set in the signal 
                        object.

  @return 
  None.
 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_signal2_set(qurt_signal2_t *signal, unsigned int mask);

/*======================================================================*/
/**@ingroup func_qurt_signal2_get
   Gets a signal from a signal object.

   Returns the current signal values of the specified signal object.

  @datatypes
  #qurt_signal2_t

  @param[in] *signal Pointer to the signal object to access.

  @return         
  A 32 bit word with current signals
    
  @dependencies
  None.
*/
/* ======================================================================*/
unsigned int qurt_signal2_get(qurt_signal2_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_signal2_clear
  Clear signals in the specified signal object.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 
  indicates that a signal is to be cleared, and 0 that it is not to be cleared.

  @note1hang Signals must be explicitly cleared by a thread when it is awakened -- the wait 
           operations do not automatically clear them.

  @datatypes
  #qurt_signal2_t

  @param[in] signal   Pointer to the signal object to modify.
  @param[in] mask     Mask value identifying the individual signals to be cleared in the signal object.
	
  @return 		  
  None.

  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_signal2_clear(qurt_signal2_t *signal, unsigned int mask);

#endif /* QURT_SIGNAL2_H */
