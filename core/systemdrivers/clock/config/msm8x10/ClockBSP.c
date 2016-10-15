/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock regime bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/systemdrivers/clock/config/msm8x10/ClockBSP.c#5 $

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
  /* LPAPLL0 - LPASS PLL0                                                  */
  /*-----------------------------------------------------------------------*/  

  {
    /* .eSource            = */ HAL_CLK_SOURCE_LPAPLL0,
    /* .HALConfig            */ {                               
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO1,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   2,
    /* .HALConfig.nL       = */   51,
    /* .HALConfig.nM       = */   1,
    /* .HALConfig.nN       = */   5,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 491520 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_LOW,
  },

  /*-----------------------------------------------------------------------*/
  /* LPAPLL1 - LPASS Q6 PLL                                                */
  /*-----------------------------------------------------------------------*/  

  {
    /* .eSource            = */ HAL_CLK_SOURCE_LPAPLL1,
    /* .HALConfig            */ {                               
    /* .HALConfig.eSource  = */   HAL_CLK_SOURCE_XO,
    /* .HALConfig.eVCO     = */   HAL_CLK_PLL_VCO1,
    /* .HALConfig.nPreDiv  = */   1,
    /* .HALConfig.nPostDiv = */   2,
    /* .HALConfig.nL       = */   72,
    /* .HALConfig.nM       = */   0,
    /* .HALConfig.nN       = */   1,
                                },
    /* .nConfigMask        = */ CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .nFreqHz            = */ 691200 * 1000,
    /* .eVRegLevel         = */ CLOCK_VREG_LEVEL_NOMINAL,
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
  {  19200000,     { HAL_CLK_SOURCE_NULL,     2,    0,      0,     0 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};

/*
 * QUP SPI clock configurations.
 */
const ClockMuxConfigType  QUPSPIClockConfig[] =
{
  {    960000,     { HAL_CLK_SOURCE_XO,    20,  1,   2,   2 }, CLOCK_VREG_LEVEL_LOW},
  {   4800000,     { HAL_CLK_SOURCE_XO,     8,  0,   0,   0 }, CLOCK_VREG_LEVEL_LOW},
  {   9600000,     { HAL_CLK_SOURCE_XO,     4,  0,   0,   0 }, CLOCK_VREG_LEVEL_LOW },
  {  15000000,     { HAL_CLK_SOURCE_GPLL0, 20,  1,   4,   4 }, CLOCK_VREG_LEVEL_LOW },
  {  19200000,     { HAL_CLK_SOURCE_XO,     2,  0,   0,   0 }, CLOCK_VREG_LEVEL_LOW },
  {  25000000,     { HAL_CLK_SOURCE_GPLL0, 24,  1,   2,   2 }, CLOCK_VREG_LEVEL_LOW },
  {  50000000,     { HAL_CLK_SOURCE_GPLL0, 24,  0,   0,   0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};


/*
 * LPASS AHB clock configurations.
 */
const ClockMuxConfigType  AHBClockConfig[] =
{
  {   3200 * 1000, { HAL_CLK_SOURCE_XO,      12, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {   6400 * 1000, { HAL_CLK_SOURCE_XO,       6, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {   9600 * 1000, { HAL_CLK_SOURCE_XO,       4, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  19200 * 1000, { HAL_CLK_SOURCE_XO,       2, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  24576 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 10, 1, 4, 4 }, CLOCK_VREG_LEVEL_LOW },
  {  30720 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 32, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  40960 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 24, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  61440 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 16, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  {  70217 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 14, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  {  81920 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 12, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  {  98304 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 10, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 122880 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  8, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 140434 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  7, 0, 0, 0 }, CLOCK_VREG_LEVEL_NOMINAL },
  { 0 }
};

/*
 * Low Power Audio Interface (LPAIF) clocks
 */
const ClockMuxConfigType LPAIFOSRClockConfig[] =
{
  {    512 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1, 60, 60 }, CLOCK_VREG_LEVEL_LOW},
  {    768 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1, 40, 40 }, CLOCK_VREG_LEVEL_LOW},
  {   1024 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1, 30, 30 }, CLOCK_VREG_LEVEL_LOW},
  {   1536 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1, 20, 20 }, CLOCK_VREG_LEVEL_LOW},
  {   2048 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1, 15, 15 }, CLOCK_VREG_LEVEL_LOW},
  {   3072 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1, 10, 10 }, CLOCK_VREG_LEVEL_LOW},
  {   4096 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  30, 1,  8,  8 }, CLOCK_VREG_LEVEL_LOW},
  {   6144 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  20, 1,  8,  8 }, CLOCK_VREG_LEVEL_LOW},
  {   8192 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  30, 1,  4,  4 }, CLOCK_VREG_LEVEL_LOW},
  {  12288 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  20, 1,  4,  4 }, CLOCK_VREG_LEVEL_LOW}
};


/*
 * Low Power Audio Interface (LPAIF) clocks
 */
const ClockMuxConfigType LPAIFPCMClockConfig[] =
{
   {    128 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1, 240, 240 }, CLOCK_VREG_LEVEL_LOW},
   {    512 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1,  60,  60 }, CLOCK_VREG_LEVEL_LOW},
   {    768 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1,  40,  40 }, CLOCK_VREG_LEVEL_LOW},
   {   1024 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1,  30,  30 }, CLOCK_VREG_LEVEL_LOW},
   {   1536 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1,  20,  20 }, CLOCK_VREG_LEVEL_LOW},
   {   2048 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1,  15,  15 }, CLOCK_VREG_LEVEL_LOW},
   {   3072 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  32, 1,  10,  10 }, CLOCK_VREG_LEVEL_LOW},
   {   4096 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  30, 1,   8,   8 }, CLOCK_VREG_LEVEL_LOW},
   {   6144 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  20, 1,   8,   8 }, CLOCK_VREG_LEVEL_LOW},
   {   8192 * 1000, { HAL_CLK_SOURCE_LPAPLL0,  30, 1,   4,   4 }, CLOCK_VREG_LEVEL_LOW}
};


/*
 * Slimbus clock configurations. 
 *  
 * Note:  Slimbus requires nominal voltage, but is making requests
 *        directly to the PMIC driver so set the vote here for low. 
 */
const ClockMuxConfigType SlimbusClockConfig[] =
{
  {  24576 * 1000, { HAL_CLK_SOURCE_LPAPLL0, 20, 1, 2, 2 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};


/*
 * Digital Codec clock configurations. 
 */
const ClockMuxConfigType DigCodecClockConfig[] =
{
  {   9600 * 1000, { HAL_CLK_SOURCE_XO,       4, 0, 0, 0 }, CLOCK_VREG_LEVEL_LOW },
  { 0 }
};


/*
 * Clock Log Default Configuration.
 *
 * NOTE: A .nGlobalLogFlags value of 0x12 will log only clock frequency
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
  { CLOCK_FLAG_NODE_TYPE_CLOCK,        (void*)"digcodec_ahb_clk",                CLOCK_FLAG_SUPPRESSIBLE },
  { CLOCK_FLAG_NODE_TYPE_NONE,         (void*)0,                                 0                       } 
};


/*
 * Resources that need to be published to domains outside GuestOS.
 */
static const char *ClockPubResource[] =
{
   CLOCK_NPA_RESOURCE_QDSS
};

ClockNPAResourcePubType ClockResourcePub = 
{
   SENSOR_PD,
   ClockPubResource,
   1
};

