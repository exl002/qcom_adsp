#ifndef SPM_H
#define SPM_H

/*=============================================================================
   FILE:           spm.h

   OVERVIEW:       This module contains routines for the subsystem power 
                   manager (SPM).

   DEPENDENCIES:   None

                   Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary

   $Header: //components/rel/core.adsp/2.2/power/spm/inc/spm.h#1 $
==============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"

/*=============================================================================
                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

=============================================================================*/

/*
 * Variable indicating how many spm cores the given master has. Technically it
 * is a constant but value is obtained through different methods. But once 
 * assigned, it should never be changed.
 *
 * Initial value will be zero and then later assigned a value during 
 * initialization.
 */
extern unsigned int SPM_NUM_CORES;

/** This enum reflects the low power modes that are supported by the SPM driver.
 *  Not all of these low power modes may be supported on every target.
 *  Supported low-power modes are listen in the BSP configuration for the target
 */
typedef enum
{
  SPM_MODE_CLK_GATE,         /** Gate Clock to Processor Core                */
  SPM_MODE_VDD_MIN,          /** Minimize the Processor Core Voltage         */
  SPM_MODE_PWRC,             /** PC Processor Core using PMIC                */
  SPM_MODE_PWRC_L2RET,       /** PC with L2 retention                        */
  SPM_MODE_L2_RETENTION,     /** Retain Power to L2 Cache Memory             */
  SPM_MODE_L2_GDHS,          /** Power Off L2 Cache using Local Switch       */
  SPM_MODE_L2_OFF,           /** Power Off L2 Cache                          */
  SPM_NUM_LOW_POWER_MODES
} spm_low_power_mode_type;

/*
   This enum is for bypassing RPM to expediate some of SPM functions for an 
    individual subsystem like Scorpion.
 */
typedef enum
{
  SPM_RPM_PERFORM_HANDSHAKE,
  SPM_RPM_BYPASS_HANDSHAKE,
} spm_rpm_bypass_type;

typedef enum
{
  SPM_CORE_NUM_0,
  SPM_CORE_NUM_1,
  SPM_CORE_NUM_2,
  SPM_CORE_NUM_L2 = SPM_CORE_NUM_2
} spm_core_num_type;

/*=============================================================================

                    EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*============================================================================
 FUNCTION SPM_SET_LOW_POWER_MODE
  
  DESCRIPTION
  This function is used to set the low power mode of the SPM HW.  The low
  power mode will be valid the next time the SPM HW receives a SW_DONE_EN
  signal from the processor.  This function should be used to set the low
  power mode before every halt in order to guarantee that the SW_DONE_EN
  signal is regarded in the desired way.
  
  PARAMETERS
  low_power_mode  Desired low power mode during the next halt.
  core_num        This tells the SPM driver which core to program the
                        SPM for.  The first core starts at 0, and the core
                        numbers go up from there.  This function will do
                        nothing if given an invalid core number.  
 
 DEPENDENCIES
  None
 
 RETURN VALUE
  None
 
 SIDE EFFECTS
  None
 ============================================================================*/
void spm_set_low_power_mode
( 
  uint32 core_num,
  spm_low_power_mode_type low_power_mode
);

/*============================================================================
 FUNCTION SPM_SET_RPM_BYPASS
 
 DESCRIPTION
 This function is used to tell the SPM driver whether or not the RPM
 handshaking should be performed during the next power savings sequence.
 This function should be called before halting the processor.
 
 PARAMETERS
 rpm_bypass  Indicates whether or not the RPM handshaking should be
                    performed.  The choices are:
                    SPM_RPM_PERFORM_HANDSHAKE  - Perform the RPM handshake.
                    SPM_RPM_BYPASS_HANDSHAKE -    Bypass the RPM handshake.

 core_num    This tells the SPM driver which core to program the
                    SPM for.  The first core starts at 0, and the core
                    numbers go up from there.  This function will do
                    nothing if given an invalid core number.  

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
 ============================================================================*/
void spm_set_rpm_bypass
( 
  uint32 core_num,
  spm_rpm_bypass_type rpm_bypass
);

/*============================================================================
  FUNCTION spm_enable
  
 DESCRIPTION
 Enable/Disable the SPM HW.
 
 PARAMETERS
 core_num   : Which core's SPM (0,1,2,...)
 spm_enable : TRUE to enable; FALSE to disable.

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
 ============================================================================*/
void spm_enable( uint32 core_num, boolean spm_enable );

/*============================================================================
  FUNCTION spm_initialize
  
 DESCRIPTION
 This is the init function for the SPM driver.  It initializes the HAL 
 layer as well as the driver layer.
 
 PARAMETERS
 None

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
 ============================================================================*/
void spm_initialize( void );


/*=============================================================================

                    INTERNAL FUNCTION DEFINITIONS

=============================================================================*/
/**
 * SPM_CURR_CORE
 *
 * @brief Function call to obtain core id on which this instruction is 
 *        getting executed based on platform.
 *
 * @Note:
 * ---------------------------------------------------------------------------
 * Power utils does not have getCurrCpuID for NON-HLOSes which is used for
 * multicore targets. Since it is not even present, if we have that here, it 
 * can result in to compile time error even if it is not ever being called for
 * single core targets.
 * ---------------------------------------------------------------------------
 */
#define SPM_CURR_CORE (SPM_NUM_CORES > 1 ? /* getCurrCpuID() */ 0 : 0)


#endif /* !SPM_H */

