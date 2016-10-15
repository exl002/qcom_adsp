#ifndef __HALCLKGENERIC_H__
#define __HALCLKGENERIC_H__
/*
==============================================================================

FILE:         HALclkGeneric.h

DESCRIPTION:
  Internal, chipset specific definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/systemdrivers/hal/clk/hw/v1/HALclkGeneric.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
10/04/12   ll      Corrected CLK_DIV_FMSK value
07/11/11   vs      Created.

==============================================================================
            Copyright (c) 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALclkInternal.h"



/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * Common root clock configuration registers and their relative offset to 
 * the first register (*_CMD_CGR)
 */
#define HAL_CLK_CMD_REG_OFFSET                                         0
#define HAL_CLK_CFG_REG_OFFSET                                         0x4
#define HAL_CLK_M_REG_OFFSET                                           0x8
#define HAL_CLK_N_REG_OFFSET                                           0xC
#define HAL_CLK_D_REG_OFFSET                                           0x10


/*
 * Common block fields/masks
 */
#define HAL_CLK_BLOCK_CTRL_REG_BLK_ARES_FMSK                           0x00000001


/*
 * Common root clock command fields/masks (*_CMD_RCGR)
 */
#define HAL_CLK_CMD_CGR_ROOT_OFF_FMSK                                  0x80000000
#define HAL_CLK_CMD_CGR_ROOT_EN_FMSK                                   0x00000002
#define HAL_CLK_CMD_CFG_UPDATE_FMSK                                    0x00000001
#define HAL_CLK_CMD_CFG_UPDATE_SHFT                                    0


/*
 * Common root clock config fields/masks (*_CFG_RCGR)
 */
#define HAL_CLK_CFG_CGR_MODE_FMSK                                      0x00003000
#define HAL_CLK_CFG_CGR_MODE_SHFT                                      0xc
#define HAL_CLK_CFG_CGR_SRC_SEL_FMSK                                   0x00000700
#define HAL_CLK_CFG_CGR_SRC_SEL_SHFT                                   0x8
#define HAL_CLK_CFG_CGR_SRC_DIV_FMSK                                   0x0000001F
#define HAL_CLK_CFG_CGR_SRC_DIV_SHFT                                   0

#define HAL_CLK_CFG_CFG_DUAL_EDGE_MODE_VAL                             0x2
#define HAL_CLK_CFG_CFG_BYPASS_MODE_VAL                                0


/*
 * Common clock branch fields/masks (*_CBCR register)
 */
#define HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK                           0xF0000000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_SHFT                           28
#define HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_FMSK                           0x01FF0000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_DIV_SHFT                           16
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_CORE_ON_FMSK             0x00004000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_ON_FMSK           0x00002000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_FORCE_MEM_PERIPH_OFF_FMSK          0x00001000
#define HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK                        0x00000001
#define HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_SHFT                        0

#define HAL_CLK_BRANCH_CTRL_REG_CLK_ON_VAL_1                           0x0
#define HAL_CLK_BRANCH_CTRL_REG_CLK_ON_VAL_2                           0x2


/*
 * Common distributed switch control fields (*_GDSCR register)
 */
#define HAL_CLK_GDSCR_PWR_ON_FMSK                                      0x80000000
#define HAL_CLK_GDSCR_PWR_ON_SHFT                                      31
#define HAL_CLK_GDSCR_EN_ALL_FMSK                                      0x000000C0
#define HAL_CLK_GDSCR_CLAMP_IO_FMSK                                    0x00000030
#define HAL_CLK_GDSCR_SW_COLLAPSE_FMSK                                 0x00000001                                     
#define HAL_CLK_GDSCR_SW_COLLAPSE_SHFT                                 0


/*
 * externs
 */
extern HAL_clk_ClockControlType       HAL_clk_mGenericClockControl;
extern HAL_clk_ClockDomainControlType HAL_clk_mGenericClockDomainControl;
extern HAL_clk_PowerDomainControlType HAL_clk_mGenericPowerDomainControl;


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/

/*
 * Clock control functions
 */
void    HAL_clk_GenericEnable           ( HAL_clk_ClockDescType  *pmClockDesc );
void    HAL_clk_GenericDisable          ( HAL_clk_ClockDescType  *pmClockDesc );
boolean HAL_clk_GenericIsEnabled        ( HAL_clk_ClockDescType  *pmClockDesc );
boolean HAL_clk_GenericIsOn             ( HAL_clk_ClockDescType  *pmClockDesc );
void    HAL_clk_GenericInvert           ( HAL_clk_ClockDescType  *pmClockDesc, boolean bInvert );
void    HAL_clk_GenericReset            ( HAL_clk_ClockDescType  *pmClockDesc, boolean bAssert );
boolean HAL_clk_GenericIsReset          ( HAL_clk_ClockDescType  *pmClockDesc );
void    HAL_clk_GenericConfig           ( HAL_clk_ClockDescType  *pmClockDesc, HAL_clk_ClockConfigType eConfig );
void    HAL_clk_GenericDetectConfig     ( HAL_clk_ClockDescType  *pmClockDesc, uint32 *pnConfig, boolean *pbValid );
void    HAL_clk_GenericConfigDivider    ( HAL_clk_ClockDescType  *pmClockDesc, uint32 nDiv );
void    HAL_clk_GenericDetectDivider    ( HAL_clk_ClockDescType  *pmClockDesc, uint32 *pnDiv );
void    HAL_clk_GenericConfigFootswitch ( HAL_clk_ClockDescType  *pmClockDesc, HAL_clk_FootswitchConfigType eConfig );


/*
 * Clock domain control functions
 */
void               HAL_clk_GenericConfigMux       ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, const HAL_clk_ClockMuxConfigType * );
void               HAL_clk_GenericDetectMuxConfig ( HAL_clk_ClockDomainDescType *pmClockDomainDesc, HAL_clk_ClockMuxConfigType *pmConfig );
uint32             HAL_clk_GenericSourceMapToHW   ( HAL_clk_SourceMapType  *pmSourceMap, HAL_clk_SourceType eSource );
HAL_clk_SourceType HAL_clk_GenericSourceMapFromHW ( HAL_clk_SourceMapType  *pmSourceMap, uint32 nSourceIndex );


/*
 * Clock power domain control functions
 */
void    HAL_clk_GenericPowerDomainEnable     ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
void    HAL_clk_GenericPowerDomainDisable    ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
boolean HAL_clk_GenericPowerDomainIsEnabled  ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );
boolean HAL_clk_GenericPowerDomainIsOn       ( HAL_clk_PowerDomainDescType *pmPowerDomainDesc );


#endif  /* __HALCLKGENERIC_H__ */

