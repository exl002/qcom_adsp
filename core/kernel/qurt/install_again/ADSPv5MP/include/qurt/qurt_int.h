#ifndef QURT_INT_H
#define QURT_INT_H
/**
  @file  qurt_int.h

  @brief  Prototypes of kernel interrupt API functions      

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

	$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/qurt/include/public/qurt_int.h#8 $ 
	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $


when 				who 		what, where, why
---------- 	--- 		------------------------------------------------
2011-02-25 	op			Add Header file
2012-12-14  cm          (Tech Pubs) Edited/added Doxygen comments and markup.
======================================================================*/

/*=====================================================================
 Constants and macros
======================================================================*/
/** @ingroup interrupts_constants */
#define SIG_INT_ABORT 0x80000000


/*=====================================================================
 Functions
======================================================================*/

/**@ingroup func_qurt_interrupt_register
  @xreflabel{sec:interrupt_register}
  Registers the interrupt.\n
  Enables the specified interrupt and associates it with the specified QuRT signal object and
  signal mask.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1
  indicates that a signal is to be waited on, and 0 that it is not to be waited on.

  When the interrupt occurs, the signal specified in the signal mask is set in the signal
  object. An interrupt service thread (IST) conventionally waits on that signal in order to
  handle the interrupt.
								 
  Up to 31 separate interrupts can be registered to a single signal object, as determined by
  the number of individual signals the object can store. (Note that signal 31 is reserved by
  QuRT.) Thus a single IST can handle several different interrupts.

  QuRT reserves some interrupts for internal use -- the remainder are available for use by
  applications, and thus are valid interrupt numbers. If the specified interrupt number is
  outside the valid range, the register operation returns the status value QURT_EINT.

  Only one thread can be registered at a time to a specific interrupt. Attempting to register
  an already-registered interrupt returns the status value QURT_EVAL.

  Only one signal bit in a signal object can be registered at a time to a specific interrupt.
  Attempting to register multiple signal bits to an interrupt returns the status value
  QURT_ESIG.
								 
  @note1hang The valid range for an interrupt number may differ on target execution
             environments other than the simulator. For more information see the
             appropriate hardware document.
								 
  Second-level interrupt numbers are calculated as follows: \n
  (L1_interrupt_number x 32) + L2_interrupt_number \n
  where the L1 interrupt number is the level-one IRQ number that the SIRCx (secondlevel
  interrupt controller: x=0..1) connects to, and the L2 interrupt number is the level-two
  interrupt number of SIRCx that the interrupting device connects to.

  For example, consider a second-level interrupt source which is assigned to interrupt
  number 15 of SIRC0. If SIRC0 connects to IRQ 23 of the Hexagon processor, then to
  register this interrupt in QuRT, use the interrupt number 751 (23 x 32 + 15).

  @note1hang First-level interrupts are still registered using a level-one interrupt number (1,
             10-31).

  For more information on second-level interrupts see @xhyperref{Q3,[Q3]}.

  @datatypes
  #qurt_anysignal_t

  @param[in] int_num      Second-level interrupt to deregister. Valid range is 1, 10-1023 (simulator only)
  @param[in] int_signal   Any-signal object to wait on (Section @xref{dox:any_signals}).
  @param[in] signal_mask  Signal mask value indicating signal to receive the interrupt.

   @return
   QURT_EOK -- Interrupt successfully registered.\n
   QURT_EINT -- Invalid interrupt number: \n
                - L1 interrupt > 31 \n
                - L2 interrupt does not exist \n
                - Interrupt reserved for QuRT \n
   QURT_ESIG -- Invalid signal bitmask (cannot set more than one
                signal at a time) \n
   QURT_EVAL -- Interrupt already registered

   @dependencies
   None.
*/
 unsigned int qurt_interrupt_register(int int_num, qurt_anysignal_t *int_signal, int signal_mask);

/*
 * Waits for registered interrupt signal

 * Suspend the current thread until one of its registered interrupts occurs. The second input, "mask", 
 * contains the interrupt signals the IST expects to receive. The interrupt signals are registered 
 * with interrupts via qurt_register_interrupt API.
 *
 * The signals returned in "signal" variable indicate which interrupts occurred. Use function 
 * qurt_anysignal_get to read the signals. IST shall locally maintain a table that maps a signal to 
 * a specific interrupt. IST shall also check if signal SIG_INT_ABORT is received. If so, the IST 
 * shall quit from interrupt receiving loop.
 *
 * For detail information on this API, see Qurt User Manual Section 4.2.5
 *
 * Prototype
 *
 * unsigned int qurt_anysignal_wait(qurt_anysignal_t *int_signal, unsigned int mask)
 */

/**@ingroup func_qurt_interrupt_acknowledge
  Acknowledges an interrupt after it has been processed.\n
  Re-enables an interrupt and clears its pending status. This is done after an interrupt has been
  processed by an interrupt service thread (IST).

  Interrupts are automatically disabled after they occur. To re-enable an interrupt, an IST
  performs the acknowledge operation after it has finished processing the interrupt and
  just before suspending itself (i.e., by waiting on the interrupt signal).

  @note1hang To prevent subsequent occurrences of the interrupt from being lost or reprocessed,
           an IST must clear the interrupt signal (Section @xref{sec:anysignal_clear}) before
           acknowledging the interrupt.
	 
  @param[in] int_num Interrupt that is being re-enabled (0-31).

  @return 
  QURT_EOK -- Interrupt acknowledge was successful. \n
  QURT_EDEREGISTERED -- Interrupt has already been deregistered.
	
  @dependencies
  None.	
*/
int qurt_interrupt_acknowledge(int int_num);

/**@ingroup func_qurt_interrupt_deregister
  Disables the specified interrupt and disassociate it from any QuRT signal object.
  If the specified interrupt was never registered (Section @xref{sec:interrupt_register}), the deregister operation
  returns the status value QURT_EINT.

  @note1hang If an interrupt is deregistered while an interrupt service thread (IST) is waiting
             to receive it, the IST may wait indefinitely for the interrupt to occur. To avoid
             this problem, the QuRT kernel sends the signal SIG_INT_ABORT to awaken an
             IST after determining that it has no interrupts registered.

  @param[in] int_num Second-level interrupt to deregister.
                     Valid range 1, 10-1023 (simulator only).

  @return
  QURT_EOK -- Interrupt deregister was successful.\n
  QURT_EINT -- Invalid interrupt number (not registered).

  @dependencies
  None.

*/
unsigned int qurt_interrupt_deregister(int int_num);

/**@ingroup func_qurt_interrupt_enable
  Enables the interrupt.\n
  The interrupt for the int_num needs to be registered 
  beforehand. After calling qurt_interrupt_disable(), the 
  corresponding interrupt cannot come to the Hexagon processor anymore until 
  qurt_interrupt_enable() is called with the same int_num. 

  @param[in] int_num Interrupt number.

  @return
  QURT_EOK -- Interrupt successfully enabled.\n 
  QURT_EINT -- Invalid interrupt number.\n
  QURT_EVAL -- Interrupt has not been registered.

  @dependencies
  None.

*/
unsigned int qurt_interrupt_enable(int int_num);

/**@ingroup func_qurt_interrupt_disable
  Disables the interrupt.\n
  The interrupt for the int_num needs to be registered 
  beforehand. After calling qurt_interrupt_disable(), the 
  corresponding interrupt cannot come to the Hexagon processor anymore until 
  qurt_interrupt_enable() is called with the same int_num. 

  @param[in] int_num Interrupt number.

  @return
  QURT_EOK -- Interrupt successfully disabled.\n 
  QURT_EINT -- Invalid interrupt number.\n
  QURT_EVAL -- Interrupt has not been registered. 

  @dependencies
  None.
*/
unsigned int qurt_interrupt_disable(int int_num);


/**@ingroup func_qurt_interrupt_status
  Returns a value indicating the pending status of the specified interrupt.

  @param[in]  int_num  Interrupt that is being checked (0-31).
  @param[out] status   Interrupt status, with 1 indicating that an interrupt is
                       pending, and 0 that it is not pending.
 
  @return 
  QURT_EOK -- Status successfully returned. \n
  QURT_EINT -- Invalid interrupt number.

  @dependencies
  None.
 */
unsigned int qurt_interrupt_status(int int_num, int *status);

/**@ingroup func_qurt_interrupt_clear
  Clears the pending status of the specified interrupt.

  @note1hang This operation is intended for system-level use, and must be used with care.

  @param[in] int_num Interrupt that is being re-enabled (0-31).
 
  @return 
  QURT_EOK -- Interrupt clear was successful.\n
  QURT_EINT -- Invalid interrupt number.
  
  @dependencies
  None.
 */
unsigned int qurt_interrupt_clear(int int_num);

/**@ingroup func_qurt_interrupt_get_registered
  Gets the registered L1 interrupts. \n
  Returns a bitmask indicating which L1 interrupts have been registered.\n
  Interrupts are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1
  indicates that an interrupt is registered, and 0 that it is not registered.

  @note1hang This operation is intended for system-level use, and must be used with care.

  @return 
  Bitmask -- Registered L1 interrupts. 

  @dependencies
  None.
 */
unsigned int qurt_interrupt_get_registered(void);

/**@ingroup func_qurt_interrupt_get_config
  Gets the L2VIC interrupt configuration. \n
  Returns the type and polarity of the specified L2VIC interrupt.

  @note1hang L2VIC interrupts are supported only on the Hexagon V4 and V5 processors.
 
  @param[in]   int_num       L2VIC interrupt that is being re-enabled (0-31).
  @param[out]  int_type      Pointer to an interrupt type. 0 indicates a level-triggered interrupt,
                             1 indicates an edge-triggered interrupt.
  @param[out]  int_polarity  Pointer to interrupt polarity. 0 indicates an active-high
                             interrupt, and 1 indicates an active-low interrupt.
 
  @return 
  QURT_EOK -- Configuration successfully returned.\n
  QURT_EINT -- Invalid interrupt number. 

  @dependencies
  None.
 */
unsigned int qurt_interrupt_get_config(unsigned int int_num, unsigned int *int_type, unsigned int *int_polarity);

/**@ingroup func_qurt_interrupt_set_config
  Sets the type and polarity of the specified L2VIC interrupt.

  @note1hang L2VIC interrupts are supported only on the V4 and V5 Hexagon processors. L2VIC 
                  interrupts must be deregistered before they can be reconfigured.

  @param[in] int_num        L2VIC interrupt that is being re-enabled (0-31).
  @param[in] int_type       Interrupt type, with 0 indicating a level-triggered interrupt, and 1
                            an edge-triggered interrupt.
  @param[in] int_polarity   Interrupt polarity, with 0 indicating an active-high interrupt,
                            and 1 an active-low interrupt.
 
  @return
  QURT_EOK -- Success. \n
  QURT_ENOTALLOWED -- Not allowed; the interrupt is being registered.\n
  QURT_EINT -- Invalid interrupt number.
  
  @dependencies
  None.
 */
unsigned int qurt_interrupt_set_config(unsigned int int_num, unsigned int int_type, unsigned int int_polarity);

/**@ingroup func_qurt_interrupt_raise
  Raises the interrupt. \n
  Triggers the specified interrupt from software.\n
  On the V5 Hexagon processor, this function triggers a level-triggered L2VIC
  interrupt, and thus accepts interrupt numbers in the range 0 to 1023.

   @param[in] interrupt_num Interrupt number.
  
   @return
   None.

   @dependencies
   None.
 */
void qurt_interrupt_raise(unsigned int interrupt_num);



/**
  Check if the current function is called from the callback function of fastint or ISR

  @param[in] void
 
  @return
  QURT_EOK -- it is called from callback function of fastint or ISR. \n
  QURT_EVAL -- it is not called from callback function of fastint or ISR.
  
  @dependencies
  None.
 */

int qurt_isr_subcall(void);


#endif /* QURT_INT_H */

