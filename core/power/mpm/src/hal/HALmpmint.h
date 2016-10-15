#ifndef HALMPMINT_H
#define HALMPMINT_H

/*
===========================================================================

FILE:         HALmpmint.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the MPM interrupt
  controller block.

===========================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/power/mpm/src/hal/HALmpmint.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/18/11   nbg     General code clean up based on earlier code review.
11/19/10   tau     Completed the table for 8660 interrupts.
09/15/09   ajf     Expanded functionality to support MPM interrupt controllers
                   with more interrupts than 32.  Doxygen-ized function
                   documentation.
08/06/09   ajf     Rename mao --> mpm.
07/21/09   taw     Added support for SCMM interrupts.
06/16/09   taw     Refine the interrupt list to 1H09.
06/16/09   taw     Refine the interrupt list to 2H08.
01/09/09   taw     Added the full list of supported MPM interrupts, as well
                   as APIs for checking the properties of the interrupts.
10/06/08   taw     Added prototypes for enabling/disabling/clearing all,
                   a single, or a mask, of MAO interrupts.
03/20/08   kr      Created.
05/27/08   kr      Modifications based on code review

===========================================================================
          Copyright © 2008-2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <HALcomdef.h>
/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/**
 * Binary Compatibility Notes:
 * If any of the following macros and enums need change for a target within
 * same family, it may affect binary compatibility of vMPM driver.
 */

/*                                                       
 * HAL_MPMINT_NOT_DEFINED                                
 *                                                       
 * Used to specify an invalid MPM interrupt.             
 */                                                      
#define HAL_MPMINT_NOT_DEFINED  HAL_MPMINT_NUM

/*
 * HAL_MPMINT_INVALID_GPIO
 *
 * Used to specify an invalid GPIO interrupt.
 */
#define HAL_MPMINT_INVALID_GPIO 0xFF

/*
 * HAL_MPMINT_INVALID_BIT_POS
 *
 * Used to specify an invalid GPIO interrupt.
 */
#define HAL_MPMINT_INVALID_BIT_POS 0xFF

/*
 * HAL_MPMINT_NONE
 *
 * Value returned by GetPending to indicate no more interrupts pending.
 */
#define HAL_MPMINT_NONE  0xFFFF

/*
 * HAL_MPMINT_GIC_IRQ_NONE
 *
 * Used to indicate invalid interrupt id at master's GIC in HAL table
 * for reverse look up.
 */
#define HAL_MPMINT_GIC_IRQ_NONE (uint16)(-1)

/* 
 * HAL_TT_EXTRACT_*_BIT 
 *
 * These MACROS are used to convert HAL_mpmint_TriggerType values into
 * bit values for vmpm based Polarity,Detect3 and Detect1 registers
 */

#define HAL_TT_EXTRACT_POL_BIT(hal_tt)  ((hal_tt & 0x4) >> 2)
#define HAL_TT_EXTRACT_DET3_BIT(hal_tt) ((hal_tt & 0x2) >> 1)
#define HAL_TT_EXTRACT_DET1_BIT(hal_tt) ((hal_tt & 0x1) >> 0)

/*
 * HAL_mpmint_TriggerType
 *
 * Enumeration of possible trigger types for a MPM interrupt.
 *
 * HAL_MPM_TRIGGER_HIGH    : Interrupt is level sensitive, positive polarity.
 * HAL_MPM_TRIGGER_LOW     : Interrupt is level sensitive, negative polarity.
 * HAL_MPM_TRIGGER_RISING  : Interrupt is edge sensitive, positive polarity.
 * HAL_MPM_TRIGGER_FALLING : Interrupt is edge sensitive, negative polarity.
 * HAL_MPM_TRIGGER_DUAL    : Interrupt is dual edge sensitive.
 *
 * @Note
 * Change in this enumeration may require change in mpmint_config_info_type
 * and HAL_mpmint_ConfigInfoType.
 *
 * @Note 
 * The bit encoding of the this trigger type corresponds to: 
 *
 * POLARITY: Bit2
 * DETECT3 : Bit1
 * DETECT  : Bit0
 *
 * This information is used to encode/decode vmpm registers in HALmpmint.c 
 */
typedef enum
{
  HAL_MPMINT_TRIGGER_LOW       = 0,
  HAL_MPMINT_TRIGGER_FALLING   = 1,
  HAL_MPMINT_TRIGGER_RISING    = 2,
  HAL_MPMINT_TRIGGER_DUAL      = 3,
  HAL_MPMINT_TRIGGER_HIGH      = 4,
  HAL_MPMINT_TRIGGER_INVALID   = 8
} HAL_mpmint_TriggerType;

/*
 * HAL_mpmint_StatusType
 *
 * Enumeration of possible interrupt status
 *
 * @Note
 * Change in this enumeration may require change in mpmint_config_info_type
 * and HAL_mpmint_ConfigInfoType.
 */
typedef enum
{
  HAL_MPMINT_STATUS_DISABLE = 0,
  HAL_MPMINT_STATUS_ENABLE  = 1,
}HAL_mpmint_StatusType;

/*
 * HAL_mpmint_ConfigInfoType
 *
 * Stores various information about an interrupt. This structure is mainly
 * used when there is a need to configure/enable/disable more than one
 * mpm mapped interrupts.
 *
 * @Note
 * This structure must be same as the one defined in mpm header file as
 * main purpose for decoupling them is avoid including HAL mpm files
 * outside mpm code and also avoid including any driver level file at
 * HW level.
 */
typedef struct HAL_mpmint_ConfigInfoType
{
  uint32 mpm_hw_int_id;            /* Actual interrupt id at MPM hardware */
  uint32 master_hw_int_id;         /* Interrupt id at master hardware 
                                      (could be Tramp if available) */
  uint8 trigger;                   /* Trigger for the interrupt at mpm */
  uint8 status;                    /* Interrupt status */
}HAL_mpmint_ConfigInfoType;

/* -----------------------------------------------------------------------
** Variable Declarations
** ----------------------------------------------------------------------- */

/**
 * Number of interrupts/gpio mapped to MPM hardware (usually remains same
 * within a particular chipset family)
 *
 * In general this variable will be the length of HAL config table.
 */
extern uint32 HAL_MPMINT_NUM;

/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** HAL standard APIs
** ----------------------------------------------------------------------- */
                                 
/**
 * This is the init function for the MPM interrupt controller HAL layer.
 *
 * It should be noted that in case of OS where we cannot directly write into
 * some physical memory address, the memory must be mapped while passing
 * the second parameter as virtual address.
 *
 * @param ppszVersion  The HAL version string for this HAL.
 * @param master_msg_ram_base_ptr: Base address (physical/virtual based on OS)
 *                                 of master's shared message ram with rpm for 
 *                                 MPM.
 * @param init_vmpm_regs: If true, initializes the vmpm registers that rpm
 *                        uses else keep it the way they were. It can be false
 *                        for the OSes in which mpm driver is used in multiple
 *                        process space and once initialized, we don't want to
 *                        disturb vmpm configuration registers.
 * @return None
 */
void HAL_mpmint_Init
( 
  char **ppszVersion,
  uint8 *master_msg_ram_base_ptr, 
  bool32 init_vmpm_regs
);

/**
 * This is the reset function for the MPM interrupt controller HAL layer.
 * 
 * @return None
 */
void HAL_mpmint_Reset( void );

/**
 * This is the save function for the MPM interrupt controller HAL layer.  It
 * does nothing of particular value, since the MPM hardware is always on.
 * 
 * @return None
 */
void HAL_mpmint_Save( void );

/**
 * This is the restore function for the MPM interrupt controller HAL layer.  It
 * does nothing of particular value, since the MPM hardware is always on.
 * 
 * @return None
 */
void HAL_mpmint_Restore( void );
                                 

/* -----------------------------------------------------------------------
** Single interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to enable a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  A valid index in to HAL table to enable interrupt/gpio 
 *              corresponding to that index.
 * 
 * @return None
 */
void HAL_mpmint_Enable( uint32 nMPM );

/**
 * This function is used to disable a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  A valid index in to HAL table to disable interrupt/gpio 
 *              corresponding to that index.
 * 
 * @return None
 */
void HAL_mpmint_Disable( uint32 nMPM ); 

/**
 * This function is used to clear a specific interrupt in the MPM interrupt
 * controller HW.  If the interrupt is not supported on this platform, this
 * function does nothing.
 *
 * @param nMPM  A valid index in to HAL table to clear interrupt/gpio 
 *              represented by that index.
 * 
 * @return None
 */
void HAL_mpmint_Clear( uint32 nMPM );


/* -----------------------------------------------------------------------
** Mass interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function enables every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void HAL_mpmint_All_Enable( void );

/**
 * This function disables every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void HAL_mpmint_All_Disable( void ); 

/**
 * This function clears every interrupt supported by this platform in the MPM
 * interrupt controller HW.
 * 
 * @return None
 */
void HAL_mpmint_All_Clear( void );


/* -----------------------------------------------------------------------
** Masked interrupt APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to determine the number of 32-bit masks needed to
 * represent all of the physical interrupts supported by the MPM interrupt
 * controller on this platform.
 *
 * @param[out] pnNumber  The number of 32-bit masks needed to represent all
 *                       interrupts supported by this platform.
 * 
 * @return None
 */
void HAL_mpmint_GetNumberMasks( uint32 *pnNumber );

/**
 * This function is used to enable multiple interrupts at once.
 * 
 * @param nMaskIndex  Which 32-bit mask is being set.  The mask that houses
 *                    physical interrupt number 'n' is floor(n / 32).
 * @param nMask       The bits of the mask to be enabled in HW.  The bit to set
 *                    for physical interrupt 'n' is (1 << (n % 32)).
 *
 * @return None
 *
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_Mask_Enable( uint32 nMaskIndex, uint32 nMask );

/**
 * This function is used to disable multiple interrupts at once.
 * 
 * @param nMaskIndex  Which 32-bit mask is being set.  The mask that houses
 *                    physical interrupt number 'n' is floor(n / 32).
 * @param nMask       The bits of the mask to be disabled in HW.  The bit to set
 *                    for physical interrupt 'n' is (1 << (n % 32)).
 *
 * @return None
 *
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_Mask_Disable( uint32 nMaskIndex, uint32 nMask );

/**
 * This function is used to clear multiple interrupts at once.
 * 
 * @param nMaskIndex  Which 32-bit mask is being set.  The mask that houses
 *                    physical interrupt number 'n' is floor(n / 32).
 * @param nMask       The bits of the mask to be cleared in HW.  The bit to set
 *                    for physical interrupt 'n' is (1 << (n % 32)).
 *
 * @return None
 *
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_Mask_Clear( uint32 nMaskIndex, uint32 nMask );

/**
 * This function is used to retrieve a mask of pending interrupts.
 * 
 * @param nMaskIndex   Which 32-bit mask to retrieve.  The mask that houses
 *                     physical interrupt number 'n' is floor(n / 32).
 * @param pnMask[out]  The mask representing pending interrupts.  The bit that
 *                     that will be set for physical interrupt 'n' is
 *                     (1 << (n % 32)).
 *
 * @return None
 *
 * @see HAL_mpmint_GetNumberMasks
 * @see HAL_mpmint_GetPhysNumber
 */
void HAL_mpmint_GetPending( uint32 nMaskIndex, uint32 *pnMask );


/* -----------------------------------------------------------------------
** Interrupt configuration APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used retrieve the current configuration of an interrupt as
 * it is programmed into the MPM HW.
 *
 * @param      nMPM       The interrupt which needs its configuration queried.
 * @param[out] peTrigger  The configuration of the interrupt requested.
 * 
 * @return None
 */
void HAL_mpmint_GetTrigger( uint32 nMPM, HAL_mpmint_TriggerType *peTrigger );

/**
 * This function is used set the current configuration of an interrupt in the
 * MPM interrupt controller HW.
 *
 * @param nMPM      The interrupt which should be reconfigured.
 * @param eTrigger  The new configuration for the interrupt requested.
 * 
 * @return None
 */
void HAL_mpmint_SetTrigger( uint32 nMPM, HAL_mpmint_TriggerType eTrigger );

/**
 * This function is used to add master level GIC irq id to HAL table for 
 * table entries which are of interrupt (not gpios) type and which are marked
 * as wakeup interrupts (not all mapped interrupts are used as wakeup irqs).
 * During initialization, when we inform interrupt controller about wakeup
 * interrupts, we should be calling this function as well to add entry in
 * to HAL table.
 *
 * We need this additional entry in the HAL table to expediate reverse look 
 * up while trigger interrupts locally.
 *
 * @param nMPM     Entry in to HAL table that is of interrupt type (and
 *                 marked as wakeup in interrupt mapping table at driver
 *                 level).
 * @param nGicIrq  Interrupt id at master's own GIC.
 *
 * @return none
 */
void HAL_mpmint_MapWakeupIRQ( uint32 nMPM, uint16 nGicIrq );                                    

/**
 * This function is used to configure and enable/disable multiple interrupts
 * at once in the MPM interrupt controller HW.
 *
 * @param intrs:       Array containing interrupts' information like id,
 *                     trigger type and status.
 * @param intrs_count: Size of the above array.
 *
 * @return none
 */
void HAL_mpmint_setup_interrupts
( 
  HAL_mpmint_ConfigInfoType *intrs, 
  uint32 intrs_count 
);

/* -----------------------------------------------------------------------
** Query APIs
** ----------------------------------------------------------------------- */

/**
 * This function is used to determine the number of interrupts supported by
 * this version of the MPM HAL. It is same as number of interrupt lines that
 * go to MPM. 
 *
 * @param[out] pnNumber  The number interrupts supported by this version of the
 *                       MPM HAL.
 * 
 * @return None
 *
 * @see HAL_mpmint_IsSupported
 */
void HAL_mpmint_GetNumber( uint32 *pnNumber );

/**
 * This function is used to determine the number of physical interrupts
 * supported by this HW platform. Right now this function should return same
 * value as above.
 *
 * @param[out] pnNumber  The number of physical interrupts supported by this HW
 *                       platform.
 * 
 * @return None
 */
void HAL_mpmint_GetNumberPhysical( uint32 *pnNumber );

/**
 * This function is used to determine the physical assignment of an interrupt
 * defined in HAL_mpmint_IsrType for this HW platform.
 *
 * @Note
 * Right now there is always one to one mappine between HAL table and actual
 * MPM HW.
 *
 * @param      nMPM    Index for the entry in to HAL config table that represents
 *                     interrupt/gpio of interest.
 * @param[out] pnPhys  The physical interrupt number of nMPM if it is supported
 *                     by this HW platform, otherwise untouched.
 * 
 * @return FALSE if nMPM is not supported on this HW platform, otherwise TRUE.
 *
 * @see HAL_mpmint_IsSupported
 */
bool32 HAL_mpmint_GetPhysNumber( uint32 nMPM, uint32 *pnPhys );

/**
 * This function is used to determine which HAL_mpmint_IsrType interrupt is
 * assigned to a given physical interrupt number on this HW platform.
 *
 * @Note
 * Right now there is always one to one mappine between HAL table and actual
 * MPM HW.
 *
 * @param      nPhys  The physical interrupt number to retrive the
 *                    HAL_mpmint_IsrType for.
 * @param[out] pnMPM  Index in to HAL config table representing input physical
 *                    interrupt if it is valid else untouched.
 * 
 * @return FALSE if nPhys is not a valid physical interrupt number for this HW
 *         platform, otherwise TRUE.
 *
 * @see HAL_mpmint_GetNumberPhysical
 */
bool32 HAL_mpmint_GetEnumNumber( uint32 nPhys, uint32 *pnMPM );

/**
 * This function is used to determine if a HAL_mpmint_IsrType interrupt is
 * supported on this HW platform.
 *
 * @param nMPM  A value which would be used to index HAL config table.
 * 
 * @return TRUE if value represent valid HAL config table entry else FALSE.
 *
 * @see HAL_mpmint_GetNumber
 */
bool32 HAL_mpmint_IsSupported( uint32 nMPM );

/**
 * This function is used to determine if a particular GPIO is supported as an
 * MPM wakeup interrupt.
 *
 * @param      nGPIO  The GPIO number to check for wakeup interrupt support.
 * @param[out] pnMPM  Index in to HAL config table if the input GPIO is
 *                    supported by the MPM on this HW platform, otherwise
 *                    untouched.
 * 
 * @return TRUE if this HW platform supports nGPIO as a wakeup interrupt,
 *         otherwise FALSE.
 */
bool32 HAL_mpmint_IsGpioSupported( uint32 nGPIO, uint32 *pnMPM );

/**
 * This function is used to determine if a particular MPM interrupt is mapped
 * to a GPIO on this HW platform.
 *
 * @param      nMPM    MPM interrupt which we want to check if it is GPIO.
 * @param[out] pnGPIO  The GPIO number of the interrupt line if it is mapped to
 *                     a GPIO on this HW platform, otherwise untouched.
 * 
 * @return TRUE if this HW platform supports this wakeup via a GPIO, otherwise
 *         FALSE.
 */
bool32 HAL_mpmint_IsGpio( uint32 nMPM, uint32 *pnGPIO );

/**
 * This function is used to determine if a particular MPM line is of
 * interrupt type and if it is a wakeup source.
 *
 * @param      nMPM    Index of the entry in to HAL interrupt configuration 
 *                     table (ICT) for the MPM line we are interested in.
 * @param[out] pnIRQ   Interrupt id on the master GIC that corresponds to this
 *                     entry in the HAL ICT if the entry is a wakeup source else
 *                     untouched (valid only if this function returns TRUE).
 *
 * @return TRUE if nMPM entry in HAL table is wakeup interrupt else FALSE.
 */
bool32 HAL_mpmint_IsWakeupIRQ( uint32 nMPM, uint16 *pnIRQ );

/**
 * This function is used to determine if a particular MPM interrupt is pending
 * in HW.
 *
 * @param nMPM  Index in to HAL config table for the interrupt to be checked
 *              for pending status.
 * 
 * @return TRUE if the HW indicates this interrupt is pending, otherwise FALSE.
 */
bool32 HAL_mpmint_IsPending( uint32 nMPM );

/**
 * This function is used to determine if a particular MPM interrupt is enabled
 * in HW.
 *
 * @param nMPM  Index in to HAL config table for the interrupt to be checked
 *              if enabled.
 * 
 * @return TRUE if the HW indicates this interrupt is currently enabled in HW,
 *         otherwise FALSE.
 */
bool32 HAL_mpmint_IsEnabled( uint32 nMPM );

/**
 * This function is used to determine if a particular MPM interrupt is cleared
 * before its associated ISR is called or not.  If it is not a 'clear before'
 * interrupt it is assumed the interrupt is cleared after the ISR is called.
 *
 * @param nMPM  Index in to HAL config table for the interrupt to be checked
 *              if Clear Before.
 * 
 * @return TRUE if the this interrupt is 'clear before', FALSE if it is 'clear
 *         after.'
 */
bool32 HAL_mpmint_IsClearBefore( uint32 nMPM );

#ifdef __cplusplus
}
#endif

#endif /* HAL_MPMINT_H */


