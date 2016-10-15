#ifndef HAL_MPMINT_INTERNAL_H
#define HAL_MPMINT_INTERNAL_H

/*
===========================================================================

FILE:         HALmpmintInternal.h

DESCRIPTION:  
  This is the hardware abstraction layer internal interface for the MPM
  interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/power/mpm/src/hal/HALmpmintInternal.h#2 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/18/11   nbg     General code clean up based on earlier code review.
08/06/09   ajf     Rename mao --> mpm.
06/30/09   taw     Added a field for the TLMM pad enable register.
01/09/09   taw     Added HAL_maoint_PlatformIntType, which contains the
                   properties for each interrupt.
03/20/08   kr      Created.
05/27/08   kr      Modifications based on code review

===========================================================================
             Copyright © 2008-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HALmpmint.h"

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* Macro to indicate how many physical interrupt/gpio lines are mapped to
 * MPM hardware. The value is calculated during init time. */
#define HAL_MPMINT_PHYS_INTS  HAL_MPMINT_NUM


/*
 * HAL_MPMINT_DEFINE_TABLE_ENTRY
 *
 * Macro to fill entries of type HAL_mpmint_PlatformIntType in the HAL table.
 * 'gic_irq' field is initialied with invalid gic interrupt id for backward
 * compatibility. Valid values are filled in during init time.
 */
#define HAL_MPMINT_DEFINE_TABLE_ENTRY( trigger, gpio ) \
  {                                                 \
    HAL_MPMINT_TRIGGER_##trigger,                   \
    gpio,                                           \
    HAL_MPMINT_GIC_IRQ_NONE                         \
  }
/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * HAL_mpmint_PlatformType
 *
 * Platform (asic/processor) specific information filled in by the platform
 * code in HAL_pic_PlatformEarlyInit.
 *
 *  WakeupReg     : The base of the interrupt enable register array.
 *  nEnableReg    : The base of the interrupt enable register array.
 *  nDetectReg    : The base of the interrupt detect ctl register array.
 *  nDetect3Reg   : The base of the interrupt detect ctl register array.
 *  nPolarityReg  : The base of the interrupt polarity register array.
 *  nStatusReg    : The base of the interrupt status register array.
 *  nEnableRegPad : The base of TLMM's wakeup interrupt enable register array.
 *  nClearReg     : The base of the interrupt clear register array.
 */
typedef struct
{
  uint32 WakeupReg;
  uint32 nEnableReg;
  uint32 nDetectReg;
  uint32 nDetect3Reg;
  uint32 nPolarityReg;
  uint32 nStatusReg;
  uint32 nEnableRegPad;
  uint32 nClearReg;
} HAL_mpmint_PlatformType;

/*
 * HAL_mpmint_PlatformIntType
 *
 * The description of an mpm interrupt.
 *
 *  eTrigger:      trigger type (edge, level, positive, negative)
 *  gpio:          which GPIO this interrupt is associated with
 *  nPadMask:      mask to use for this interrupt in the TLMM pad enables
 */
typedef struct
{
  HAL_mpmint_TriggerType   eTrigger;
  uint8                    gpio;        /* Actual GPIO number for master. 
                                           -1 if not entry is not a GPIO. */
  uint16                   gic_irq;     /* IRQ id at master level for wakeup
                                           entry. -1 for non wakeup entry */
} HAL_mpmint_PlatformIntType;


/* -----------------------------------------------------------------------
**                        VARIABLE DECLARATIONS
** ----------------------------------------------------------------------- */

/*
 * This object contains addresses of various vMPM configuration register
 * for the master. Various fields of this objects must be initialized before
 * using any mpm functionalities. 
 *
 * @see HAL_mpmint_InitializeRegisterLayout
 */
extern HAL_mpmint_PlatformType registerLayout;

/*
 * Pointer to HAL table containing mpm mapped interrupt/gpio configuration 
 * for the particular target it is running on. Depending on the target, this
 * pointer can be set either directly or via querying DevCfg.
 *
 * @see HAL_mpmint_InitTargetData
 */
extern HAL_mpmint_PlatformIntType *aInterruptTable;

/*
 * Variable indicating size of the above table. Depending on the target,
 * it can be set by either iterating above table or directly using sizeof
 * operator. In general this should be same as number of interrupts mapped
 * to MPM hardware.
 *
 * @see HAL_mpmint_InitTargetData
 */
extern uint32 HAL_MPMINT_NUM;


/* -----------------------------------------------------------------------
**                        FUNCTION DECLARATIONS
** ----------------------------------------------------------------------- */

/**
 * Intializes the register layour for writing to shared memory. Its 
 * implementation will vary based on OS. For Apps (QNX or EA) we will
 * need virtual address for the base of shared memory address for mpm.
 *
 * @param master_msg_ram_base_ptr: Address of the memory region for this
 *                                 master reserved in shared message ram.
 *                                 It is determined by RPM team. Address
 *                                 can be virtual or physical based on
 *                                 target.
 */
extern void HAL_mpmint_InitializeRegisterLayout
(
  uint8 *master_msg_ram_base_ptr
);

/**
 * Prepares mpm driver to perform IPC with RPM afterwards. This is just
 * a setup function and it does not perform any actual IPC. 
 *
 * @Note
 * This OS dependent function. For example,
 *  - in case where we use DAL IPC, it would be attaching to DAL IPC device.
 *  - On some target this function will map the addresses of IPC registers 
 *    for the process they are running in.
 */
extern void HAL_mpmint_SetupIPC( void );

/**
 * Obtains the target specific data for HAL layer of MPM driver and uses them
 * to initialize various structures/variables used by hal layer. It can use
 * DevCfg api or direct declaration and assignment.
 *
 * Currently it is expected to initialize following variables
 *   - aInterruptTable
 *   - HAL_MPMINT_NUM
 */
extern void HAL_mpmint_InitTargetData( void );

/**
 * Sends an IPC interrupt to RPM whenver vMPM configuration for this
 * master has been changed. This happens when any interrupt/gpio 
 * configuration (detection, polarity) is changed.
 *
 * @Note:
 * This function may have different implementation on different OSes.
 * - The default implementation is using DAL IPC interrupt interface.
 * - Alternative one would be to write directly to IPC registers.
 */
extern void vmpm_send_interrupt( void );

#endif /* HAL_MPMINT_INTERNAL_H */

