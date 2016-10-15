#ifndef QURT_POWER_H
#define QURT_POWER_H
/**
  @file qurt_power.h 
  @brief  Prototypes of power API  

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


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file 
12/12/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/


/**@ingroup func_qurt_power_shutdown_prepare
  Prepares for power collapse.\n
  This function determines if ISDB is disabled and all other hardware threads in the QuRT 
  system are in Wait or Stop mode.
  If so, mask the global interrupt and return the result value QURT_EOK.
 
  @return
  QURT_EOK -- Operation was successfully performed. 
           This value indicates that the processor is ready for
           Power-collapse mode. \n
  QURT_EISDB -- ISDB is not disabled. \n
  QURT_ESTM -- Processor is not in Single-threaded mode.

  @dependencies
  None.
 */
int qurt_power_shutdown_prepare (void);

/**@ingroup func_qurt_power_shutdown_fail_exit  
  Returns from Power Collapse mode when power collapse cannot proceed.

  This function unmasks the global interrupt. This operation is used only when the thread is 
  recovering from a failed power collapse operation (Section @xref{sec:powerShutdownEnter}).
 
  @return
  QURT_EOK -- Operation was successfully performed.

  @dependencies
  None.
 */
int qurt_power_shutdown_fail_exit (void);

/**@ingroup func_qurt_power_shutdown_enter
  @xreflabel{sec:powerShutdownEnter}   
  Triggers a power collapse. \n
  This function determines if no valid interrupts are pending. Valid interrupts are determined using the
  interrupt registration status. If so, put the current hardware thread into Wait mode.

  When all hardware threads have entered Wait mode, the Hexagon processor can enter
  Power-collapse mode.

  The type indicates whether the L2 cache is retained during the power collapse.

  @param[in]  type    The L2 cache retention type. Values: \n
                      - QURT_POWER_SHUTDOWN_TYPE_L2NORET \n
                      - QURT_POWER_SHUTDOWN_TYPE_L2RET  @tablebulletend 
  
  @return
  QURT_EOK -- Operation was successfully performed. This value indicates that the processor has
              returned from Power-collapse mode. \n
  Nonzero -- IPEND mask value indicating pending interrupts.

  @dependencies
  None.
 */
int qurt_power_shutdown_enter (int type);

/**@ingroup func_qurt_power_shutdown_exit
  Undoes state changes made preparing for power collapse.\n
  This function unmasks the global interrupts.
 
  @return
  QURT_EOK --Operation was successfully performed.

  @dependencies
  None.
 */
int qurt_power_shutdown_exit (void);

/* 
  Determines if all other hardware threads in the QuRT system are in Wait or
  Stop mode. 

  If so, mask the global interrupt and return the result value QURT_EOK.
  If any hardware thread is not in Wait or Stop mode, then do nothing and return the
  result value QURT_ESTM.
 
  @return
  QURT_EOK -- Operation was successfully performed.\n
  QURT_ESTM -- Processor is not in Single-threaded mode.

  @dependencies
  None.
 */
int qurt_power_tcxo_prepare (void);

/* 
  Unmasks the global interrupt.
  This operation is used only when the thread is recovering from a failed TCXO shutdown
  operation
 
  @return
  QURT_EOK -- Operation was successfully performed.

  @dependencies
  None.
 */
int qurt_power_tcxo_fail_exit (void);

/*  
  Checks for pending interrupts.
  If any active (i.e., registered) interrupts are pending, return a bitmask value indicating the
  active pending interrupts.

  If no pending interrupts exist, change the interrupt mask to the value read from symbol
  QURTK_tcxo_intmask (which is specified as part of QuRT configuration). \n
  @note1hang The default value of QURTK_tcxo_intmask is 0xffffffff, which specifies
  that no interrupts are enabled.
  
  @return
  QURT_EOK -- Operation was successfully performed. \n
  Non-zero -- IPEND value masking out invalid interrupts.

  @dependencies
  None.
 */
int qurt_power_tcxo_enter (void);

/*  
  Restores the interrupt mask to the value defined when the operation
  qurt_power_tcxo_enter() was previously called.

  @return
  QURT_EOK -- Operation was successfully performed.

  @dependencies
  None.
 */
int qurt_power_tcxo_exit (void);

/**@ingroup func_qurt_power_wait_for_idle
  Suspends the current caller thread until all other threads in the QuRT system are
  also suspended. When this happens the caller thread is awakened.

  If all other threads are suspended when this function is called, the caller thread continues
  executing. \n
  @note1hang This function is used to perform power management.
 
  @return
  None.

  @dependencies
  None.
 */
void qurt_power_wait_for_idle (void);

/**@ingroup func_qurt_power_wait_for_active
  Suspends the current caller thread until any other thread in the QuRT system is
  awakened. When this happens the caller thread is awakened.

  If any other thread is ready when this function is called, the caller thread is immediately
  scheduled. \n
 
  @return
  None.

  @dependencies
  This function is used to perform power management (Section @xref{sec:power_management}). It is useful
             only when paired with qurt_power_wait_for_idle().
 */
void qurt_power_wait_for_active (void);

/**@ingroup func_qurt_system_ipend_clear
  Clears pending interrupts from the Hexagon processor IPEND register.

  The bit order of the mask value is identical to the order defined for the IPEND register. A
  mask bit value of 1 indicates that the corresponding pending interrupt should be cleared,
  and 0 indicates that the corresponding pending interrupt should not be cleared.

  @param[in] ipend Interrupt mask value identifying the individual interrupts to be cleared.

  @return
  None.

  @dependencies
  None.
 */
void qurt_system_ipend_clear (unsigned int ipend);


/**@ingroup func_qurt_system_ipend_get
  Gets the IPEND register.\n
  Returns the current value of the Hexagon processor IPEND register.

  The return value is a mask value which identifies the individual interrupts that are
  pending.

  The bit order of the mask value is identical to the order defined for the IPEND register. A
  mask bit value of&nbsp;1 indicates that the corresponding interrupt is pending, and 0 that the
  corresponding interrupt is not pending.
 
  @return
  Integer -- IPEND register value; the pending interrupt.

  @dependencies
  None.
 */
unsigned int qurt_system_ipend_get (void);

/*
   Sets the AVS configuration register. 
 
   @param avscfg_value    AVS configuration value to be set.
  
   @return          
   None.

   @dependencies
   None.
 */
void qurt_system_avscfg_set(unsigned int avscfg_value);

/*
   Clears the IPEND register.
   Gets the AVS configuration register. 
   
   @return
   AVS configuration register value.

   @dependencies
   None.
 */
unsigned int qurt_system_avscfg_get(void);


/**@ingroup func_qurt_system_vid_get  
  Gets the VID register. \n
  Returns the current value of the Hexagon processor VID register.

  The return value is the vector number of a second-level interrupt that has been accepted by
  the Hexagon processor core.  
 
  @return 
  Integer -- VID register value; a vector number of L2 interrupt is accepted by the processor.
          Valid range is 0-1023.

  @dependencies
  None.
 */
unsigned int qurt_system_vid_get(void);

/*
   Gets the number of power collapses and counts of processor cycles 
   for entering and exiting most recent power collapse. If there 
   no power collapse has occured yet, processor cycles numbers will 
   be zero. 

   @param[out] enter_pcycles  Number of processor cycles for entering most
                              recent power collapse.
   @param[out] exit_pcycles  Number of processor cycles for exiting most
                             recent power collapse.
   @return      
   Zero -- No power collapses have occurred.
   Non-zero -- Number of power collapses that have occurred since
                the processor was reset

   @dependencies
   None.
 */ 
int qurt_get_shutdown_pcycles( unsigned long long *enter_pcycles,  unsigned long long *exit_pcycles );

#endif /* QURT_POWER_H */
