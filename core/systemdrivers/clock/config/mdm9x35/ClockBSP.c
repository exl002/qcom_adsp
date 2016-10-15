/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock regime bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/systemdrivers/clock/config/mdm9x35/ClockBSP.c#4 $

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


/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 * Clock source configuration data.
 */
const ClockSourceConfigType SourceConfig[] =
{

  /*-----------------------------------------------------------------------*/
  /* XO                                                                    */
  /*-----------------------------------------------------------------------*/  

  {
    /* .eSource            = */ HAL_CLK_SOURCE_XO,
    /* .HALConfig          = */ { HAL_CLK_SOURCE_NULL },
    /* .nConfigMask        = */ 0,
    /* .nFreqHz            = */ 19200 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* GPLL0 - General purpose PLL0                                          */
  /*-----------------------------------------------------------------------*/  

  {
    /* .eSource            = */ HAL_CLK_SOURCE_GPLL0,
    /* .HALConfig            */ {                               
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO1,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   1,
    /* .HALConfig.nL       = */   31,
    /* .HALConfig.nM       = */   1,
    /* .HALConfig.nN       = */   4,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 600000 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* LPAPLL0 (Tesla) - LPAAUDIO PLL                                        */
  /*-----------------------------------------------------------------------*/  

  {
    /* .eSource            = */ HAL_CLK_SOURCE_LPAPLL0,
    /* .HALConfig            */ {                               
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO1,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   1,
    /* .HALConfig.nL       = */   32,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   0,
    /* .HALConfig.nAlpha   = */   0,
    /* .HALConfig.nAlpha   = */   0,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 614400 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* LPAPLL1 - QDSP6SS PLL                                                 */
  /*-----------------------------------------------------------------------*/  

  {
    /* .eSource            = */ HAL_CLK_SOURCE_LPAPLL1,
    /* .HALConfig            */ {                               
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO1,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   2,
    /* .HALConfig.nL       = */   42,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   0,
    /* .HALConfig.nAlpha   = */   0,
    /* .HALConfig.nAlpha   = */   0,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 403200 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* LPAAUDIO (Prius) PLL - LPAAUDIO_PRIUS PLL                             */
  /*-----------------------------------------------------------------------*/  

  {
    /* .eSource            = */ HAL_CLK_SOURCE_LPAPLL2,
    /* .HALConfig            */ {                               
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO1,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   1,
    /* .HALConfig.nL       = */   32,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   0,
    /* .HALConfig.nAlpha   = */   0,
    /* .HALConfig.nAlpha   = */   0,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 614400 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /* last entry */
  { HAL_CLK_SOURCE_NULL }
};

/* =========================================================================
**    nFreqHz       { eSource, nDiv2x, nM, nN, n2D },      eVRegLevel         
** =========================================================================*/


/*
 * QUP I2C clock configuration.
 */
const ClockMuxConfigType  QUPI2CClockConfig[] =
{
  {  19200 * 1000, { HAL_CLK_SOURCE_XO,     2,    0,      0,     0 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};


/*
 * LPASS AHB clock configurations.
 */
const ClockMuxConfigType  AHBClockConfig[] =
{
  {   3200 * 1000, { HAL_CLK_SOURCE_XO,      12, 0,  0,  0 }, CLOCK_VREG_LEVEL_LOW },
  {   6400 * 1000, { HAL_CLK_SOURCE_XO,       6, 0,  0,  0 }, CLOCK_VREG_LEVEL_LOW },
  {   9600 * 1000, { HAL_CLK_SOURCE_XO,       4, 0,  0,  0 }, CLOCK_VREG_LEVEL_LOW },
  {  19200 * 1000, { HAL_CLK_SOURCE_XO,       2, 0,  0,  0 }, CLOCK_VREG_LEVEL_LOW },
  {  24576 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  2, 3, 25, 25 }, CLOCK_VREG_LEVEL_LOW },
  {  34133 * 1000, { HAL_CLK_SOURCE_LPAPLL2, 12, 0,  0,  0 }, CLOCK_VREG_LEVEL_LOW },
  {  40960 * 1000, { HAL_CLK_SOURCE_LPAPLL2, 10, 0,  0,  0 }, CLOCK_VREG_LEVEL_LOW },
  {  51200 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  8, 0,  0,  0 }, CLOCK_VREG_LEVEL_NOMINAL },
  {  68267 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  6, 0 , 0,  0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 136533 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  3, 0,  0,  0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};


/*
 * Low Power Audio Interface (LPAIF) clocks
 */
const ClockMuxConfigType LPAIFOSRClockConfig[] =
{
  {    128 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1, 384, 384 }, CLOCK_VREG_LEVEL_LOW },
  {    512 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  48,  48 }, CLOCK_VREG_LEVEL_LOW },
  {    768 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  32,  32 }, CLOCK_VREG_LEVEL_LOW },
  {   1024 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  24,  24 }, CLOCK_VREG_LEVEL_LOW },
  {   1536 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  16,  16 }, CLOCK_VREG_LEVEL_LOW },
  {   2048 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  12,  12 }, CLOCK_VREG_LEVEL_LOW },
  {   3072 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   8,   8 }, CLOCK_VREG_LEVEL_LOW },
  {   4096 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   6,   6 }, CLOCK_VREG_LEVEL_LOW },
  {   6144 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   4,   4 }, CLOCK_VREG_LEVEL_LOW },
  {   8192 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   3,   3 }, CLOCK_VREG_LEVEL_LOW },
  {  12288 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   2,   2 }, CLOCK_VREG_LEVEL_LOW }
};

/*
 * Low Power Audio Interface (LPAIF) clocks
 */
const ClockMuxConfigType LPAIFPCMOEClockConfig[] =
{
  {    128 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1, 384, 384 }, CLOCK_VREG_LEVEL_LOW },
  {    512 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  48,  48 }, CLOCK_VREG_LEVEL_LOW },
  {    768 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  32,  32 }, CLOCK_VREG_LEVEL_LOW },
  {   1024 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  24,  24 }, CLOCK_VREG_LEVEL_LOW },
  {   1536 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  16,  16 }, CLOCK_VREG_LEVEL_LOW },
  {   2048 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  12,  12 }, CLOCK_VREG_LEVEL_LOW },
  {   3072 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   8,   8 }, CLOCK_VREG_LEVEL_LOW },
  {   4096 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   6,   6 }, CLOCK_VREG_LEVEL_LOW },
  {   6144 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   4,   4 }, CLOCK_VREG_LEVEL_LOW },
  {   8192 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   3,   3 }, CLOCK_VREG_LEVEL_LOW },
  {  12288 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   2,   2 }, CLOCK_VREG_LEVEL_LOW }
};


/*
 * Low Power Audio Interface (LPAIF) clocks 
 *  
 * Note:  LPAAUDIO PRIUS PLL output from AUX1 has a CDIV-5 on the PLL output which sources 
 *        the RCG with a 122.88 MHz frequency, requiring a different RCG divider.
 */
const ClockMuxConfigType LPAIFPCMClockConfig[] =
{
  {    128 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1, 384, 384 }, CLOCK_VREG_LEVEL_LOW },
  {    512 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  48,  48 }, CLOCK_VREG_LEVEL_LOW },
  {    768 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  32,  32 }, CLOCK_VREG_LEVEL_LOW },
  {   1024 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  24,  24 }, CLOCK_VREG_LEVEL_LOW },
  {   1536 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  16,  16 }, CLOCK_VREG_LEVEL_LOW },
  {   2048 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,  12,  12 }, CLOCK_VREG_LEVEL_LOW },
  {   3072 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   8,   8 }, CLOCK_VREG_LEVEL_LOW },
  {   4096 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   6,   6 }, CLOCK_VREG_LEVEL_LOW },
  {   6144 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   4,   4 }, CLOCK_VREG_LEVEL_LOW },
  {   8192 * 1000, { HAL_CLK_SOURCE_LPAPLL2,  10, 1,   3,   3 }, CLOCK_VREG_LEVEL_LOW }
};


/*
 * Slimbus clock configurations. 
 *  
 * Note:  Slimbus requires nominal voltage, but is making requests
 *        directly to the PMIC driver so set the vote here for low.
 *  
 * Note:  There is a CDIV-5 on the output of the LPAAUDIO PRIUS PLL 
 *        AUX2 selection which results in a 122.88 MHz frequency to
 *        supply the Slimbus core clock.  Further division reflects
 *        the proper frequency.
 */
const ClockMuxConfigType SlimbusClockConfig[] =
{
  {  24576 * 1000, { HAL_CLK_SOURCE_LPAPLL2, 1, 1, 5, 5 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};


/*
 * Clock Log Default Configuration.
 *
 * NOTE: An .nGlobalLogFlags value of 0x12 will log only clock frequency
 *       changes and source state changes by default.
 */
const ClockLogType ClockLogDefaultConfig[] =
{
  {
    /* .nLogSize        = */ 4096,
    /* .nGlobalLogFlags = */ 0x12
  }
};


/*
 * Clock Flag Init Config.
 */
const ClockFlagInitType ClockFlagInitConfig[] =
{
  { CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN, (void*)"lpass_q6core",                    CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_wrapper_timeout_clk",       CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_timeout_clk",          CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_ixfabric_clk",         CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"q6ss_ahbm_clk",                   CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_dml_clk",              CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_lpaif_dma_clk",        CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_lpaif_csr_clk",        CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_slimbus_lfabif_clk",   CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_avsync_csr_clk",       CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_lpm_clk",              CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_wrapper_smem_clk",          CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_wrapper_lcc_csr_clk",       CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_wrapper_efabric_clk",       CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_csr_clk",              CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_core_security_clk",         CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_wrapper_security_clk",      CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"audio_wrapper_br_clk",            CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_NONE,         (void*)0,                                 0                       } 
};


