/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock regime bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/systemdrivers/clock/config/mdm9x35/ClockCPUBSP.c#3 $

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
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_0,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_1,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_2,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_3,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_4,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_5,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_6,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_7,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_8,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_9,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_10,
//  CLOCK_QDSP6_CONFIG_TABLE_INDEX_11,
//  CLOCK_QDSP6_CONFIG_TABLE_INDEX_12,

  CLOCK_CPU_CONFIG_TOTAL
};



/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 * Mux configuration for different CPU frequencies. 
 *  
 * Note:  LPAPLL2_AUX2 has a CDIV=2 coming out of the PLL output so the source 
 *        source coming into the RCG is 614.4/2 = 307.2 MHz.
 */
static ClockCPUConfigType Clock_QDSP6Config [] =
{
/* HAL CLOCK MUX                            Freq. Hz         Source                        2*div       Voltage                                                      LDO Vdd     Headroom */
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, {   9600 * 1000, {HAL_CLK_SOURCE_XO,       4, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, {  19200 * 1000, {HAL_CLK_SOURCE_XO,       2, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, {  38400 * 1000, {HAL_CLK_SOURCE_LPAPLL2, 16, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, {  51200 * 1000, {HAL_CLK_SOURCE_LPAPLL2, 12, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, {  61440 * 1000, {HAL_CLK_SOURCE_LPAPLL2, 10, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, {  76800 * 1000, {HAL_CLK_SOURCE_LPAPLL2,  8, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, { 102400 * 1000, {HAL_CLK_SOURCE_LPAPLL2,  6, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, { 122880 * 1000, {HAL_CLK_SOURCE_LPAPLL2,  5, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     700 * 1000, 78500 },     
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, { 153600 * 1000, {HAL_CLK_SOURCE_LPAPLL2,  4, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV,     0 },
//  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCA, { 268800 * 1000, {HAL_CLK_SOURCE_LPAPLL1,  4, 0, 0, 0}, CLOCK_VREG_LEVEL_LOW},     CLOCK_CPU_VREG_LEVEL_LOW_UV, 0 },
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCC, { 307200 * 1000, {HAL_CLK_SOURCE_LPAPLL2,  2, 0, 0, 0}, CLOCK_VREG_LEVEL_NOMINAL}, CLOCK_CPU_VREG_LEVEL_NOMINAL_UV, 0 },
  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCA, { 403600 * 1000, {HAL_CLK_SOURCE_LPAPLL1,  2, 0, 0, 0}, CLOCK_VREG_LEVEL_NOMINAL}, CLOCK_CPU_VREG_LEVEL_NOMINAL_UV, 0 }, 
//  {HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX_SRCA, { 537600 * 1000, {HAL_CLK_SOURCE_LPAPLL1,  2, 0, 0, 0}, CLOCK_VREG_LEVEL_HIGH},    CLOCK_CPU_VREG_LEVEL_HIGH_UV, 0 }, 
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
  CLOCK_CPU_PERF_LEVEL_10,
//  CLOCK_CPU_PERF_LEVEL_11,
//  CLOCK_CPU_PERF_LEVEL_12,

  CLOCK_CPU_PERF_LEVEL_TOTAL
};

/*
 * Enumeration of generic CPU performance levels.  More performance 
 * levels exist here than are actually implemented.
 */
static const uint32 QDSP6PerfList[] =
{
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_1,  /* Start at XO */
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_2,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_3,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_4,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_5,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_6,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_7,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_8,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_9,
  CLOCK_QDSP6_CONFIG_TABLE_INDEX_10,
//  CLOCK_QDSP6_CONFIG_TABLE_INDEX_11,
//  CLOCK_QDSP6_CONFIG_TABLE_INDEX_12,
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
    CLOCK_CPU_PERF_LEVEL_9,
   
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
   FALSE,

   /*
    * Fusion MDM support.
    */
   FALSE,


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
   HAL_CLK_SOURCE_LPAPLL0,
   HAL_CLK_SOURCE_LPAPLL1,
   HAL_CLK_SOURCE_NULL

};

const boolean ClockLDOVersion[] =
{
   1
};
