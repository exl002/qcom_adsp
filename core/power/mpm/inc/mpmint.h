#ifndef MPMINT_H
#define MPMINT_H

/*==============================================================================
  FILE:         mpmint.h

  OVERVIEW:     This file contains function prototypes to configure mpm mapped
                interrupts and gpios from master (non-rpm) side.

  DEPENDENCIES: None

                Copyright (c) 2005-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.adsp/2.2/power/mpm/inc/mpmint.h#3 $
$DateTime: 2013/10/01 18:40:25 $
==============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "mpmint_target.h"
#include "mpmint_types.h"
                                         
/*===========================================================================   
                      PUBLIC FUNCTION DECLARATIONS                              
===========================================================================*/   
                                                                                
/**
 * @brief Initialize the MPM interrupt controller subsystem.
 *
 * This function initializes the MPM interrupt controller to a known state (all
 * interrupts masked off).  The MPM interrupt controller is the piece of
 * hardware that remains on in even the deepest sleep modes, allowing the
 * system to wake for interrupts no matter what power saving measures have been
 * taken.  An ISR is also registered, so that this driver can soft-trigger the
 * local equivalents of any interrupt that the MPM catches during a sleep mode
 * (thus removing any requirement to monitor an interrupt in both the MPM and
 * the local interrupt controller).
 */
extern void mpmint_init( void );


/**
 * @brief Configure the polarity (ACTIVE HIGH/ACTIVE LOW) and detection type
 *        (EDGE/LEVEL) of an MPM fixed-function interrupt.
 *
 * If you want to configure an MPM-routed GPIO interrupt, you probably want to
 * use mpmint_config_gpio_wakeup instead.  Also, if you simply want to enable
 * an interrupt as a wakeup source, see mpmint_config_wakeup.
 *
 * Since fixed-function interrupts generally have only a single configuration
 * that makes sense, this driver populates each one with a default setting at
 * init time.  In the unlikely event you want to override the default you can
 * call this function--but it is not expected to be used in general.
 *
 * @param int_num   The interrupt that you wish to configure.
 * @param detection The desired detection method (edge/level).
 * @param polarity  The desired polarity (high/rising vs. low/falling).
 *
 * @see mpmint_config_gpio_wakeup
 * @see mpmint_config_wakeup
 */
extern void mpmint_config_int
(
  mpmint_isr_type       int_num,     
  mpmint_detect_type    detection,
  mpmint_polarity_type  polarity
);


/**
 * @brief Enable an MPM-routed interrupt as a wakeup source during deep sleep.
 *
 * If the interrupt occurs during deep sleep, the MPM will wake the system and
 * this driver will soft-trigger the interrupt into the local interrupt
 * controller (that is, you do not have to handle the MPM interrupt seperate
 * from the interrupt's regular incarnation).
 *
 * @param int_num Which interrupt to enable.
 * @param ignored No longer used--reatined for legacy compatibility.
 */
extern void mpmint_config_wakeup
(
  mpmint_isr_type       int_num,
  mpmint_processor_type ignored
);


/**
 * @brief Disable an MPM-routed interrupt as a wakeup source during deep sleep.
 *
 * @note When an interrupt is disabled in the MPM, that interrupt may be
 *       completely discarded during deep sleep (not even latched for later
 *       processing).  However, it can still cause a wakeup from "shallower"
 *       forms of sleep (like simple halt, etc.).
 *
 * @param int_num Which interrupt to disable.
 */
extern void mpmint_disable_wakeup
(
  mpmint_isr_type int_num
);


/**
 * @brief Configure a GPIO interrupt as a wakeup source.
 *
 * Configure the polarity (ACTIVE HIGH/ACTIVE LOW) and detection type
 * (EDGE/LEVEL) of an MPM-routed GPIO interrupt, and enable that GPIO as a
 * wakeup source during deep sleep.
 *
 * @note If the GPIO is not supported by the MPM hardware this function does
 *       nothing and returns.
 *
 * @param which_gpio The number of the GPIO to configure.
 * @param detection  The desired detection method (edge/level).
 * @param polarity   The desired polarity (high/rising vs. low/falling).
 * @param ignored    No longer used--reatined for legacy compatibility.
 */
extern void mpmint_config_gpio_wakeup
(
  uint32                  which_gpio,
  mpmint_detect_type      detection,
  mpmint_polarity_type    polarity,
  mpmint_processor_type   ignored
);


/**
 * @brief Disable an MPM-routed GPIO as a wakeup source during deep sleep.
 *
 * @note When an interrupt is disabled in the MPM, that interrupt may be
 *       completely discarded during deep sleep (not even latched for later
 *       processing).  However, it can still cause a wakeup from "shallower"
 *       forms of sleep (like simple halt, etc.).
 *
 * @param which_gpio The number of the GPIO to disable.
 */
extern void mpmint_disable_gpio_wakeup
(
  uint32 which_gpio
);

/**
 * @brief triggers the interrupts caught on MPM during vdd minimization 
 *        on master's interrupt controller to simulate if they had
 *        occurred normally.
 *
 * @note  On modem, this function should loop through all the latched MPM 
 *        interrupts and perform an associated soft-trigger operation to 
 *        simulate above situation.
 */
extern void mpmint_trigger_interrupts( void );

/**
 * @brief Returns the number of interrupts (IRQs) that are mapped to mpm for
 *        a master it's running (i.e. modem, apps, q6).
 *
 * @return As above
 */
extern uint32 mpmint_get_num_mapped_interrupts( void );

/**
 * @brief Fills in the input arrays with the mapping of mpm interrupt id
 *        and corresponding master interrupt id.
 *
 * It should be noted that the first two fields of mpmint_config_info_type
 * must not be changed by caller or it can result into configuring other
 * interrupts which are not meant to be.
 *
 * @param intrs: Array in which above mapping will be filled in.
 * @param intrs_count: Size of the array (Usually it should be value returned
 *                     by mpmint_get_num_mapped_interrupts function).
 *
 * @return Number of mappings actually filled in (should be same as
 *         intrs_count in normal scenario).
 */
extern uint32 mpmint_map_interrupts_with_master
(
  mpmint_config_info_type *intrs,
  uint32 intrs_count
);

/**
 * @brief Sets up various fields of more than one interrupts at MPM level
 *        like trigger type and status.
 *
 * @note  As of now this function is intended to use for interrupts only 
 *        (no GPIO). 
 *
 * @param intrs: Input array containing various interrupt information 
 * @param intrs_count: Number of interrupts in the above array.
 */
extern void mpmint_setup_interrupts
( 
  mpmint_config_info_type *intrs,
  uint32 intrs_count 
);

/**
 * @brief Returns the virtual address for the processor's MPM
 *        RAM corresponding to wakeup_time.  This virtual
 *        address value is determined at runtime and is target
 *        specific
 *  
 * @note This function has dependency on prior initialization 
 *       of MPM driver as the virtual address is determined by
 *       MPM driver during runtime and target specific
 *  
 * @return Returns the virtual address for the processor's MPM 
 *         RAM. 
 */
uint32 mpm_get_proc_mpm_base_va( void );

#endif  /* MPMINT_H */

