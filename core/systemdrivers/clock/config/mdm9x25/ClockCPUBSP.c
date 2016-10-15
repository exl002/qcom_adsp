/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock regime bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/systemdrivers/clock/config/mdm9x25/ClockCPUBSP.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
10/19/11   dcf     Created. 

==============================================================================
            Copyright (c) 2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBSP.h"
#include "ClockLPASSCPU.h"


/*=========================================================================
      Type Definitions and Macros
==========================================================================*/


/*
 * Q6 Voltage levels in uv.
 */
#define CLOCK_CPU_VREG_LEVEL_LOW_UV      812500
#define CLOCK_CPU_VREG_LEVEL_NOMINAL_UV  900000
#define CLOCK_CPU_VREG_LEVEL_HIGH_UV    1050000



/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Enumeration of QDSP6 configurations.
 */
enum
{
  CLOCK_CPU_CONFIG_9P6_MHZ_XO,
  CLOCK_CPU_CONFIG_19P2_MHZ_XO,
  CLOCK_CPU_CONFIG_49P152_MHZ_PLL8,
  CLOCK_CPU_CONFIG_65P536_MHZ_PLL8,
  CLOCK_CPU_CONFIG_78P643_MHZ_PLL8,
  CLOCK_CPU_CONFIG_98P304_MHZ_PLL8,
  CLOCK_CPU_CONFIG_131P072_MHZ_PLL8,
  CLOCK_CPU_CONFIG_157P286_MHZ_PLL8,
  CLOCK_CPU_CONFIG_196P608_MHZ_PLL8,
  CLOCK_CPU_CONFIG_393P216_MHZ_PLL8,

  CLOCK_CPU_CONFIG_TOTAL
};



/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 * Mux configuration for different CPU frequencies.
 */
static ClockCPUConfigType Clock_QDSP6Config [] =
{
/* HAL CLOCK MUX                        Freq. Hz        Source                 2*div        Voltage                                                LDO Vdd     Headroom */
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, {   9600 * 1000, {HAL_CLK_SOURCE_XO,       4}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      725 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, {  19200 * 1000, {HAL_CLK_SOURCE_XO,       2}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      725 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, {  49152 * 1000, {HAL_CLK_SOURCE_LPAPLL0, 16}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      725 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, {  65536 * 1000, {HAL_CLK_SOURCE_LPAPLL0, 12}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      725 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, {  78643 * 1000, {HAL_CLK_SOURCE_LPAPLL0, 10}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      725 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, {  98304 * 1000, {HAL_CLK_SOURCE_LPAPLL0,  8}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      725 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, { 131072 * 1000, {HAL_CLK_SOURCE_LPAPLL0,  6}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      725 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, { 157286 * 1000, {HAL_CLK_SOURCE_LPAPLL0,  5}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      0 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, { 196608 * 1000, {HAL_CLK_SOURCE_LPAPLL0,  4}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,      0 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX, { 393216 * 1000, {HAL_CLK_SOURCE_LPAPLL0,  2}, CLOCK_VREG_LEVEL_NOMINAL}, CLOCK_CPU_VREG_LEVEL_NOMINAL_UV , 0 }, 
};


/*
 * Enumeration of CPU performance levels.  More performance 
 * levels exist here than are actually implemented.
 */
enum
{
  CLOCK_CPU_PERF_LEVEL_0,
  CLOCK_CPU_PERF_LEVEL_1,
  CLOCK_CPU_PERF_LEVEL_2,
  CLOCK_CPU_PERF_LEVEL_3,
  CLOCK_CPU_PERF_LEVEL_4,
  CLOCK_CPU_PERF_LEVEL_5,
  CLOCK_CPU_PERF_LEVEL_6,
  CLOCK_CPU_PERF_LEVEL_7,
  CLOCK_CPU_PERF_LEVEL_8,
  CLOCK_CPU_PERF_LEVEL_9,

  CLOCK_CPU_PERF_LEVEL_TOTAL
};

/*
 * Enumeration of generic CPU performance levels.  More performance 
 * levels exist here than are actually implemented.
 */
static const uint32 QDSP6PerfList[] =
{
  CLOCK_CPU_CONFIG_19P2_MHZ_XO,
  CLOCK_CPU_CONFIG_49P152_MHZ_PLL8,
  CLOCK_CPU_CONFIG_65P536_MHZ_PLL8,
  CLOCK_CPU_CONFIG_78P643_MHZ_PLL8,
  CLOCK_CPU_CONFIG_98P304_MHZ_PLL8,
  CLOCK_CPU_CONFIG_131P072_MHZ_PLL8,
  CLOCK_CPU_CONFIG_157P286_MHZ_PLL8,
  CLOCK_CPU_CONFIG_196P608_MHZ_PLL8,
  CLOCK_CPU_CONFIG_393P216_MHZ_PLL8
};


/*
 * Performance level configuration data for the Q6LPASS (CPU).
 */
static const ClockCPUPerfConfigType Clock_QDSP6LPASSPerfConfig[] =
{
  {
    /*
     * Define performance levels.
     */
   
    /*
     * min level
     */
    CLOCK_CPU_PERF_LEVEL_0,

    /*
     * max level
     */
    CLOCK_CPU_PERF_LEVEL_8,
   
    /*
     * Init performance level
     */
    CLOCK_CPU_PERF_LEVEL_7,
   
    /*
     * Total number of CPU configurations.
     */
    sizeof(Clock_QDSP6Config)/sizeof(ClockCPUConfigType),
   
    /*
     * Pointer to an array of supported performance level indices.
     */
    (uint32*)QDSP6PerfList
  }
};


/*
 * Main Image data structure.
 */
const ClockImageConfigType Clock_ImageConfig =
{
  /*
   * Pointer to the CPU frequency plan settings.
   */
   (ClockCPUConfigType*)Clock_QDSP6Config,

  /*
   * Pointer to the performance level configurations.
   */
   (ClockCPUPerfConfigType*)Clock_QDSP6LPASSPerfConfig,

   /*
    * Global enable flag for eLDO.
    */
   TRUE,

   /*
    * Flag specifying if this is an MDM which may support Fusion.
    */
   TRUE

};


/*
 * Clock LDO Configuration Data.
 */
const ClockLDODataType ClockLDODataConfig[] =
{
  {
    /* nLDOCFG0 */           0x01004000,
    /* nLDOCFG1 */           0x00000050,
    /* nLDOCFG2 */           0x00000048,
    /* nRetentionVoltage */  500 * 1000,
    /* nOperatingVoltage */  775 * 1000
  }
};


/*
 * OCMEM is not used on this chipset.  Add additional flags to this as needed.
 */
const ClockNPARemoteNodeSupportType ClockNPARemoteNodeSupport = 
{
   /* bOCMEM  = */ FALSE,
};


const HAL_clk_HWIOBaseType ClockLDOHWIOBases = 
{
   /* nPhysAddress */ 0xfe000000,
   /* nSize        */ 0x00300000
};



const HAL_clk_HWIOBaseType ClockSECHWIOBases = 
{
   /* nPhysAddress */ 0xfc4b8000,
   /* nSize        */ 0x00008000
};



const HAL_clk_SourceType ClockSourcesToInit[] =
{
   HAL_CLK_SOURCE_NULL
};

