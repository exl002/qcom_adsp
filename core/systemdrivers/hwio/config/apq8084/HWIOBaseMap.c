
/*
===========================================================================
*/
/**
  @file HWIOBaseMap.c
  @brief Auto-generated HWIO Device Configuration base file.

  DESCRIPTION:
    This file contains Device Configuration data structures for mapping
    physical and virtual memory for HWIO blocks.
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.adsp/2.2/systemdrivers/hwio/config/apq8084/HWIOBaseMap.c#2 $
  $DateTime: 2013/05/06 17:55:25 $
  $Author: coresvc $

  ===========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DalHWIO.h"
#include "msmhwiobase.h"


/*=========================================================================
      Data Definitions
==========================================================================*/

static HWIOModuleType HWIOModules_RPM_SS_MSG_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PERIPH_SS[] =
{
  { "PERIPH_SS_SMMUV2_APU0132_14_QR",              0x003c1000, 0x000008d4 },
  { "PERIPH_SS_SMMUV2_SMMUV2_MM_MMU_GLOBAL0",      0x003c4000, 0x00000c7c },
  { "PERIPH_SS_SMMUV2_SMMUV2_MM_MMU_GLOBAL1",      0x003c5000, 0x0000041c },
  { "PERIPH_SS_SMMUV2_SMMUV2_MM_MMU_IMPL_DEF",     0x003c6000, 0x00000c7c },
  { "PERIPH_SS_SMMUV2_SMMUV2_MM_MMU_PERF_MON",     0x003c7000, 0x00000fcc },
  { "PERIPH_SS_SMMUV2_SMMUV2_MM_MMU_SSD",          0x003c8000, 0x00000000 },
  { "PERIPH_SS_SMMUV2_CB0_CB0_MM_MMU_CONTEXT_BANK", 0x003cc000, 0x00000fb8 },
  { "PERIPH_SS_SMMUV2_CB1_CB1_MM_MMU_CONTEXT_BANK", 0x003cd000, 0x00000fb8 },
  { "PERIPH_SS_SMMUV2_CB2_CB2_MM_MMU_CONTEXT_BANK", 0x003ce000, 0x00000fb8 },
  { "PERIPH_SS_SMMUV2_CB3_CB3_MM_MMU_CONTEXT_BANK", 0x003cf000, 0x00000fb8 },
  { "PERIPH_SS_SMMUV2_CB4_CB4_MM_MMU_CONTEXT_BANK", 0x003d0000, 0x00000fb8 },
  { "PERIPH_SS_SMMUV2_CB5_CB5_MM_MMU_CONTEXT_BANK", 0x003d1000, 0x00000fb8 },
  { "PERIPH_SS_SMMUV2_CB6_CB6_MM_MMU_CONTEXT_BANK", 0x003d2000, 0x00000fb8 },
  { "PERIPH_SS_SMMUV2_CB7_CB7_MM_MMU_CONTEXT_BANK", 0x003d3000, 0x00000fb8 },
  { "PERIPH_SS_SDC3_SDCC_SDCC4",                   0x00064000, 0x000007fc },
  { "PERIPH_SS_SDC3_SDCC_DML",                     0x00064800, 0x0000004c },
  { "PERIPH_SS_SDC3_SDCC_SDCC4_HC",                0x00064900, 0x00000198 },
  { "PERIPH_SS_SDC3_SDCC_SDCC4_BAM",               0x00044000, 0x00006934 },
  { "PERIPH_SS_SDC3_SDCC_XPU2",                    0x00042000, 0x00000254 },
  { "PERIPH_SS_SDC3_SDCC_VMIDMT",                  0x00040000, 0x00000e00 },
  { "PERIPH_SS_SDC2_SDCC_SDCC4",                   0x000a4000, 0x000007fc },
  { "PERIPH_SS_SDC2_SDCC_DML",                     0x000a4800, 0x0000004c },
  { "PERIPH_SS_SDC2_SDCC_SDCC4_HC",                0x000a4900, 0x00000198 },
  { "PERIPH_SS_SDC2_SDCC_SDCC4_BAM",               0x00084000, 0x00006934 },
  { "PERIPH_SS_SDC2_SDCC_XPU2",                    0x00082000, 0x00000254 },
  { "PERIPH_SS_SDC2_SDCC_VMIDMT",                  0x00080000, 0x00000e00 },
  { "PERIPH_SS_SDC1_SDCC_SDCC5",                   0x00024000, 0x000007fc },
  { "PERIPH_SS_SDC1_SDCC_SDCC5_DML",               0x00024800, 0x0000004c },
  { "PERIPH_SS_SDC1_SDCC_SDCC5_HC",                0x00024900, 0x0000019c },
  { "PERIPH_SS_SDC1_SDCC_SDCC5_BAM",               0x00004000, 0x00006934 },
  { "PERIPH_SS_SDC1_SDCC_XPU2",                    0x00002000, 0x00000254 },
  { "PERIPH_SS_SDC1_SDCC_VMIDMT",                  0x00000000, 0x00000e00 },
  { "PERIPH_SS_SDC4_SDCC_SDCC4",                   0x000e4000, 0x000007fc },
  { "PERIPH_SS_SDC4_SDCC_DML",                     0x000e4800, 0x0000004c },
  { "PERIPH_SS_SDC4_SDCC_SDCC4_HC",                0x000e4900, 0x00000198 },
  { "PERIPH_SS_SDC4_SDCC_SDCC4_BAM",               0x000c4000, 0x00006934 },
  { "PERIPH_SS_SDC4_SDCC_XPU2",                    0x000c2000, 0x00000254 },
  { "PERIPH_SS_SDC4_SDCC_VMIDMT",                  0x000c0000, 0x00000e00 },
  { "PERIPH_SS_BLSP1_BLSP_BAM",                    0x00104000, 0x00018934 },
  { "PERIPH_SS_BLSP1_BLSP_BAM_XPU2",               0x00102000, 0x00001754 },
  { "PERIPH_SS_BLSP1_BLSP_BAM_VMIDMT",             0x00100000, 0x00000e60 },
  { "PERIPH_SS_BLSP1_BLSP_UART0_UART0_DM",         0x0011d000, 0x000001a0 },
  { "PERIPH_SS_BLSP1_BLSP_UART1_UART1_DM",         0x0011e000, 0x000001a0 },
  { "PERIPH_SS_BLSP1_BLSP_UART2_UART2_DM",         0x0011f000, 0x000001a0 },
  { "PERIPH_SS_BLSP1_BLSP_UART3_UART3_DM",         0x00120000, 0x000001a0 },
  { "PERIPH_SS_BLSP1_BLSP_UART4_UART4_DM",         0x00121000, 0x000001a0 },
  { "PERIPH_SS_BLSP1_BLSP_UART5_UART5_DM",         0x00122000, 0x000001a0 },
  { "PERIPH_SS_BLSP1_BLSP_QUP0",                   0x00123000, 0x00000414 },
  { "PERIPH_SS_BLSP1_BLSP_QUP1",                   0x00124000, 0x00000414 },
  { "PERIPH_SS_BLSP1_BLSP_QUP2",                   0x00125000, 0x00000414 },
  { "PERIPH_SS_BLSP1_BLSP_QUP3",                   0x00126000, 0x00000414 },
  { "PERIPH_SS_BLSP1_BLSP_QUP4",                   0x00127000, 0x00000414 },
  { "PERIPH_SS_BLSP1_BLSP_QUP5",                   0x00128000, 0x00000414 },
  { "PERIPH_SS_BLSP2_BLSP_BAM",                    0x00144000, 0x00018934 },
  { "PERIPH_SS_BLSP2_BLSP_BAM_XPU2",               0x00142000, 0x00001754 },
  { "PERIPH_SS_BLSP2_BLSP_BAM_VMIDMT",             0x00140000, 0x00000e60 },
  { "PERIPH_SS_BLSP2_BLSP_UART0_UART0_DM",         0x0015d000, 0x000001a0 },
  { "PERIPH_SS_BLSP2_BLSP_UART1_UART1_DM",         0x0015e000, 0x000001a0 },
  { "PERIPH_SS_BLSP2_BLSP_UART2_UART2_DM",         0x0015f000, 0x000001a0 },
  { "PERIPH_SS_BLSP2_BLSP_UART3_UART3_DM",         0x00160000, 0x000001a0 },
  { "PERIPH_SS_BLSP2_BLSP_UART4_UART4_DM",         0x00161000, 0x000001a0 },
  { "PERIPH_SS_BLSP2_BLSP_UART5_UART5_DM",         0x00162000, 0x000001a0 },
  { "PERIPH_SS_BLSP2_BLSP_QUP0",                   0x00163000, 0x00000414 },
  { "PERIPH_SS_BLSP2_BLSP_QUP1",                   0x00164000, 0x00000414 },
  { "PERIPH_SS_BLSP2_BLSP_QUP2",                   0x00165000, 0x00000414 },
  { "PERIPH_SS_BLSP2_BLSP_QUP3",                   0x00166000, 0x00000414 },
  { "PERIPH_SS_BLSP2_BLSP_QUP4",                   0x00167000, 0x00000414 },
  { "PERIPH_SS_BLSP2_BLSP_QUP5",                   0x00168000, 0x00000414 },
  { "PERIPH_SS_BAM_DMA_BAM",                       0x00184000, 0x00014934 },
  { "PERIPH_SS_BAM_DMA_XPU2",                      0x00182000, 0x00001754 },
  { "PERIPH_SS_BAM_DMA_VMIDMT",                    0x00180000, 0x00000e50 },
  { "PERIPH_SS_BAM_DMA",                           0x00199000, 0x0000a000 },
  { "PERIPH_SS_USB3_HSIC_DWC_USB3",                0x00400000, 0x0000c630 },
  { "PERIPH_SS_USB3_HSIC_USB30_GENERAL_PURPOSE_REGS", 0x004f8400, 0x000000fc },
  { "PERIPH_SS_USB3_HSIC_USB30_QSCRATCH",          0x004f8800, 0x000000ec },
  { "PERIPH_SS_USB3_HSIC_USB30_DBM_REGFILE",       0x004f8000, 0x000000bc },
  { "PERIPH_SS_USB3_HSIC_USB30_VMIDMT",            0x004f9000, 0x00000e00 },
  { "PERIPH_SS_USB3_HSIC_USB30_XPU2",              0x004fa000, 0x00000254 },
  { "PERIPH_SS_USB1_HS_USB_OTG_HS_BASE",           0x00255000, 0x00000348 },
  { "PERIPH_SS_USB1_HS_VMIDMT",                    0x00240000, 0x00000e00 },
  { "PERIPH_SS_USB1_HS_USB_OTG_HS_XPU2",           0x00242000, 0x00000254 },
  { "PERIPH_SS_TSIF_TSIF_BAM",                     0x001c4000, 0x00010934 },
  { "PERIPH_SS_TSIF_TSIF_XPU2",                    0x001c2000, 0x00000c54 },
  { "PERIPH_SS_TSIF_TSIF_VMIDMT",                  0x001c0000, 0x00000e40 },
  { "PERIPH_SS_TSIF_TSIF_0_TSIF_0_TSIF",           0x001d8000, 0x000001c0 },
  { "PERIPH_SS_TSIF_TSIF_1_TSIF_1_TSIF",           0x001d9000, 0x000001c0 },
  { "PERIPH_SS_TSIF_TSIF_TSIF_TSPP",               0x001da000, 0x00000d4c },
  { "PERIPH_SS_BUS_TIMEOUT_0_BUS_TIMEOUT",         0x003f7000, 0x00000034 },
  { "PERIPH_SS_BUS_TIMEOUT_1_BUS_TIMEOUT",         0x003f8000, 0x00000034 },
  { "PERIPH_SS_BUS_TIMEOUT_2_BUS_TIMEOUT",         0x003f9000, 0x00000034 },
  { "PERIPH_SS_BUS_TIMEOUT_3_BUS_TIMEOUT",         0x003fa000, 0x00000034 },
  { "PERIPH_SS_BUS_TIMEOUT_4_BUS_TIMEOUT",         0x003fb000, 0x00000034 },
  { "PERIPH_SS_MPU_MPU1132A_4_M15L7_AHB",          0x003fc000, 0x000003dc },
  { "PERIPH_SS_APU_APU0132_2",                     0x003fd000, 0x000002d4 },
  { "PERIPH_SS_PDM_WEB_TCXO4",                     0x00310000, 0x00003008 },
  { "PERIPH_SS_PRNG_PRNG",                         0x003ff000, 0x00000120 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_RPM[] =
{
  { "RPM_DEC",                                     0x00080000, 0x00000070 },
  { "RPM_QTMR_AC",                                 0x00082000, 0x00000fd0 },
  { "RPM_F0_QTMR_V1_F0",                           0x00083000, 0x00000fd0 },
  { "RPM_F1_QTMR_V1_F1",                           0x00084000, 0x00000fd0 },
  { "RPM_MPU",                                     0x00086000, 0x000004dc },
  { "RPM_APU",                                     0x00087000, 0x00000254 },
  { "RPM_VMIDMT",                                  0x00088000, 0x00000e00 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QDSS_QDSS[] =
{
  { "QDSS_DAPROM",                                 0x00000000, 0x00000ffc },
  { "QDSS_QDSSCSR",                                0x00001000, 0x00000ffc },
  { "QDSS_CXSTM_2_32_32_TRUE",                     0x00002000, 0x00000ffc },
  { "QDSS_CTI0_CTI0_CSCTI",                        0x00010000, 0x00000ffc },
  { "QDSS_CTI1_CTI1_CSCTI",                        0x00011000, 0x00000ffc },
  { "QDSS_CTI2_CTI2_CSCTI",                        0x00012000, 0x00000ffc },
  { "QDSS_CTI3_CTI3_CSCTI",                        0x00013000, 0x00000ffc },
  { "QDSS_CTI4_CTI4_CSCTI",                        0x00014000, 0x00000ffc },
  { "QDSS_CTI5_CTI5_CSCTI",                        0x00015000, 0x00000ffc },
  { "QDSS_CTI6_CTI6_CSCTI",                        0x00016000, 0x00000ffc },
  { "QDSS_CTI7_CTI7_CSCTI",                        0x00017000, 0x00000ffc },
  { "QDSS_CTI8_CTI8_CSCTI",                        0x00018000, 0x00000ffc },
  { "QDSS_CSTPIU_CSTPIU_CSTPIU",                   0x00020000, 0x00000ffc },
  { "QDSS_IN_FUN0_IN_FUN0_CXATBFUNNEL_128W8SP",    0x00021000, 0x00000ffc },
  { "QDSS_IN_FUN1_IN_FUN1_CXATBFUNNEL_128W8SP",    0x00022000, 0x00000ffc },
  { "QDSS_MERG_FUN_MERG_FUN_CXATBFUNNEL_128W2SP",  0x00023000, 0x00000ffc },
  { "QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP",     0x00024000, 0x00000ffc },
  { "QDSS_ETFETB_ETFETB_CXTMC_F128W64K",           0x00025000, 0x00000ffc },
  { "QDSS_ETR_ETR_CXTMC_R64W32D",                  0x00026000, 0x00000ffc },
  { "QDSS_VMIDDAP_VMIDDAP_VMIDMT_IDX_2_SSD1",      0x00076000, 0x00000e04 },
  { "QDSS_VMIDETR_VMIDETR_VMIDMT_IDX_2_SSD1",      0x00077000, 0x00000e04 },
  { "QDSS_NDPBAM_BAM",                             0x0007c000, 0x00002934 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CLK_CTL[] =
{
  { "GCC_CLK_CTL_REG",                             0x00000000, 0x00001fc0 },
  { "GCC_RPU_RPU0032_128_L6",                      0x00010000, 0x000041d4 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MPM2_MPM[] =
{
  { "MPM2_MPM",                                    0x00000000, 0x000000f8 },
  { "MPM2_G_CTRL_CNTR",                            0x00001000, 0x00000fd0 },
  { "MPM2_G_RD_CNTR",                              0x00002000, 0x00000fd0 },
  { "MPM2_SLP_CNTR",                               0x00003000, 0x00000000 },
  { "MPM2_QTIMR_AC",                               0x00004000, 0x00000fd0 },
  { "MPM2_QTIMR_V1",                               0x00005000, 0x00000fd0 },
  { "MPM2_TSYNC",                                  0x00006000, 0x00000020 },
  { "MPM2_APU",                                    0x00007000, 0x00000754 },
  { "MPM2_TSENS",                                  0x00008000, 0x00000064 },
  { "MPM2_TSENS_TM",                               0x00009000, 0x0000005c },
  { "MPM2_WDOG",                                   0x0000a000, 0x00000010 },
  { "MPM2_PSHOLD",                                 0x0000b000, 0x00000004 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SPDM_WRAPPER_TOP[] =
{
  { "SPDM_SPDM_CREG",                              0x00000000, 0x00000100 },
  { "SPDM_SPDM_OLEM",                              0x00001000, 0x0000013c },
  { "SPDM_SPDM_RTEM",                              0x00002000, 0x000002f8 },
  { "SPDM_APU0132_1",                              0x00003000, 0x00000254 },
  { "SPDM_SPDM_SREG",                              0x00004000, 0x00000100 },
  { "SPDM_VMIDMT_IDX_1_SSD0",                      0x00005000, 0x00000e00 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SECURITY_CONTROL[] =
{
  { "SECURITY_CONTROL_CORE",                       0x00000000, 0x00006100 },
  { "SEC_CTRL_APU_APU1132_3",                      0x00007000, 0x00000354 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PMIC_ARB[] =
{
  { "PMIC_ARB_SPMI_SPMI_GENI_CFG",                 0x0000a000, 0x000005fc },
  { "PMIC_ARB_SPMI_CFG",                           0x0000a700, 0x00000874 },
  { "PMIC_ARB_SPMI_SPMI_XPU2",                     0x00000000, 0x00008554 },
  { "PMIC_ARB_SPMI_PIC",                           0x0000b000, 0x00000dfc },
  { "PMIC_ARB_XPU2",                               0x0000e000, 0x000005d4 },
  { "PMIC_ARB_CORE",                               0x0000f000, 0x00000aa0 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CORE_TOP_CSR[] =
{
  { "TCSR_MUTEX_RPU1132_64_L7",                    0x00000000, 0x000021d4 },
  { "TCSR_TCSR_MUTEX",                             0x00004000, 0x00001788 },
  { "TCSR_REGS_RPU1132_16_L12",                    0x00018000, 0x000009d4 },
  { "TCSR_TCSR_REGS",                              0x00020000, 0x0000f000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_TLMM[] =
{
  { "TLMM_APU1032_165",                            0x00000000, 0x00005454 },
  { "TLMM_CSR",                                    0x00010000, 0x00003130 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_LPASS[] =
{
  { "LPASS_LPASS_CC_REG",                          0x00000000, 0x0003f004 },
  { "LPASS_LPASS_TCSR",                            0x0004f000, 0x00000008 },
  { "LPASS_AHBE_CSR",                              0x0004f100, 0x00000010 },
  { "LPASS_QDSP6_APU0132_14_QR",                   0x00051000, 0x000008d4 },
  { "LPASS_QDSP6_SMMUV2_MM_MMU_GLOBAL0",           0x00054000, 0x00000c3c },
  { "LPASS_QDSP6_SMMUV2_MM_MMU_GLOBAL1",           0x00055000, 0x0000040c },
  { "LPASS_QDSP6_SMMUV2_MM_MMU_IMPL_DEF",          0x00056000, 0x00000c3c },
  { "LPASS_QDSP6_SMMUV2_MM_MMU_PERF_MON",          0x00057000, 0x00000fcc },
  { "LPASS_QDSP6_SMMUV2_MM_MMU_SSD",               0x00058000, 0x00000000 },
  { "LPASS_QDSP6_CB0_CB0_MM_MMU_CONTEXT_BANK",     0x0005c000, 0x00000fb8 },
  { "LPASS_QDSP6_CB1_CB1_MM_MMU_CONTEXT_BANK",     0x0005d000, 0x00000fb8 },
  { "LPASS_QDSP6_CB2_CB2_MM_MMU_CONTEXT_BANK",     0x0005e000, 0x00000fb8 },
  { "LPASS_QDSP6_CB3_CB3_MM_MMU_CONTEXT_BANK",     0x0005f000, 0x00000fb8 },
  { "LPASS_CORE_APU0132_14_QR",                    0x00061000, 0x000008d4 },
  { "LPASS_CORE_SMMUV2_MM_MMU_GLOBAL0",            0x00064000, 0x00000c08 },
  { "LPASS_CORE_SMMUV2_MM_MMU_GLOBAL1",            0x00065000, 0x00000408 },
  { "LPASS_CORE_SMMUV2_MM_MMU_IMPL_DEF",           0x00066000, 0x00000c08 },
  { "LPASS_CORE_SMMUV2_MM_MMU_PERF_MON",           0x00067000, 0x00000fcc },
  { "LPASS_CORE_SMMUV2_MM_MMU_SSD",                0x00068000, 0x00000000 },
  { "LPASS_CORE_CB0_CB0_MM_MMU_CONTEXT_BANK",      0x0006c000, 0x00000fb8 },
  { "LPASS_CORE_CB1_CB1_MM_MMU_CONTEXT_BANK",      0x0006d000, 0x00000fb8 },
  { "LPASS_CORE_CB2_CB2_MM_MMU_CONTEXT_BANK",      0x0006e000, 0x00000fb8 },
  { "LPASS_BR",                                    0x00070000, 0x00000338 },
  { "LPASS_AHBE_TIME",                             0x00071000, 0x00000034 },
  { "LPASS_Q6AHB_VMT",                             0x00072000, 0x00000e00 },
  { "LPASS_Q6AXI_VMT",                             0x00073000, 0x00000e0c },
  { "LPASS_LCC_XPU",                               0x00084000, 0x000021d4 },
  { "LPASS_TOP_XPU",                               0x00088000, 0x00000454 },
  { "LPASS_SENSOR",                                0x000a0000, 0x00000000 },
  { "LPASS_LPASS_CSR",                             0x000b0000, 0x00008004 },
  { "LPASS_SPDM",                                  0x000c7000, 0x00000118 },
  { "LPASS_AHBI_TIME",                             0x000c8000, 0x00000034 },
  { "LPASS_AVTIMER",                               0x000c9000, 0x00000090 },
  { "LPASS_AUDSYNC_WRAPPER",                       0x000c9200, 0x0000004c },
  { "LPASS_ATIMER",                                0x000c9400, 0x00000090 },
  { "LPASS_RESMP_VMT",                             0x000ca000, 0x00000e00 },
  { "LPASS_DM_VMT",                                0x000cc000, 0x00000e00 },
  { "LPASS_LPAIF_VMT",                             0x000cd000, 0x00000e20 },
  { "LPASS_RESAMPLER",                             0x000d0000, 0x00000054 },
  { "LPASS_LPM_XPU",                               0x000db000, 0x000009dc },
  { "LPASS_CSR_XPU",                               0x000dc000, 0x00000654 },
  { "LPASS_CMN_XPU",                               0x000dd000, 0x00000554 },
  { "LPASS_RSMPLR_XPU",                            0x000e0000, 0x00000554 },
  { "LPASS_LPAIF_XPU",                             0x000ec000, 0x000011d4 },
  { "LPASS_LPASS_LPM",                             0x00100000, 0x00000000 },
  { "LPASS_SPDIF_VMT",                             0x00120000, 0x00000e00 },
  { "LPASS_SPDIFTX",                               0x00133000, 0x000000d8 },
  { "LPASS_LPA_IF",                                0x00140000, 0x0001e100 },
  { "LPASS_SB_BAM",                                0x00184000, 0x0001f934 },
  { "LPASS_SB_XPU2",                               0x00182000, 0x00001a54 },
  { "LPASS_SB_VMIDMT",                             0x00180000, 0x00000e7c },
  { "LPASS_SLIMBUS",                               0x001af000, 0x0002b018 },
  { "LPASS_QDSP6SS_PUB",                           0x00200000, 0x00002020 },
  { "LPASS_QDSP6SS_QDSP6SS_APU0132_2",             0x00270000, 0x000002d4 },
  { "LPASS_QDSP6SS_CSR",                           0x00280000, 0x0000703c },
  { "LPASS_QDSP6SS_L2VIC",                         0x00290000, 0x0000057c },
  { "LPASS_QDSP6SS_QDSP6SS_QTMR_AC",               0x002a0000, 0x00000fd0 },
  { "LPASS_QDSP6SS_QTMR_F0_0",                     0x002a1000, 0x00000fd0 },
  { "LPASS_QDSP6SS_QTMR_F1_1",                     0x002a2000, 0x00000fd0 },
  { "LPASS_QDSP6SS_QTMR_F2_2",                     0x002a3000, 0x00000fd0 },
  { "LPASS_QDSP6SS_QDSP6SS_SAW2",                  0x002b0000, 0x00000fd0 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SYSTEM_NOC[] =
{
  { "SYSTEM_NOC",                                  0x00000000, 0x00004090 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CONFIG_NOC[] =
{
  { "CONFIG_NOC",                                  0x00000000, 0x00000028 },
  { NULL, 0, 0 }
};

HWIOPhysRegionType HWIOBaseMap[] =
{
  {
    "RPM_SS_MSG_RAM_START_ADDRESS",
    (DALSYSMemAddr)0xfc428000,
    0x00004000,
    (DALSYSMemAddr)RPM_SS_MSG_RAM_START_ADDRESS_BASE,
    HWIOModules_RPM_SS_MSG_RAM_START_ADDRESS
  },
  {
    "PERIPH_SS",
    (DALSYSMemAddr)0xf9800000,
    0x00500000,
    (DALSYSMemAddr)PERIPH_SS_BASE,
    HWIOModules_PERIPH_SS
  },
  {
    "RPM",
    (DALSYSMemAddr)0xfc100000,
    0x00090000,
    (DALSYSMemAddr)RPM_BASE,
    HWIOModules_RPM
  },
  {
    "QDSS_QDSS",
    (DALSYSMemAddr)0xfc300000,
    0x00080000,
    (DALSYSMemAddr)QDSS_QDSS_BASE,
    HWIOModules_QDSS_QDSS
  },
  {
    "CLK_CTL",
    (DALSYSMemAddr)0xfc400000,
    0x00020000,
    (DALSYSMemAddr)CLK_CTL_BASE,
    HWIOModules_CLK_CTL
  },
  {
    "MPM2_MPM",
    (DALSYSMemAddr)0xfc4a0000,
    0x0000c000,
    (DALSYSMemAddr)MPM2_MPM_BASE,
    HWIOModules_MPM2_MPM
  },
  {
    "SPDM_WRAPPER_TOP",
    (DALSYSMemAddr)0xfc4b0000,
    0x00006000,
    (DALSYSMemAddr)SPDM_WRAPPER_TOP_BASE,
    HWIOModules_SPDM_WRAPPER_TOP
  },
  {
    "SECURITY_CONTROL",
    (DALSYSMemAddr)0xfc4b8000,
    0x00008000,
    (DALSYSMemAddr)SECURITY_CONTROL_BASE,
    HWIOModules_SECURITY_CONTROL
  },
  {
    "PMIC_ARB",
    (DALSYSMemAddr)0xfc4c0000,
    0x00010000,
    (DALSYSMemAddr)PMIC_ARB_BASE,
    HWIOModules_PMIC_ARB
  },
  {
    "CORE_TOP_CSR",
    (DALSYSMemAddr)0xfd480000,
    0x00030000,
    (DALSYSMemAddr)CORE_TOP_CSR_BASE,
    HWIOModules_CORE_TOP_CSR
  },
  {
    "TLMM",
    (DALSYSMemAddr)0xfd500000,
    0x00020000,
    (DALSYSMemAddr)TLMM_BASE,
    HWIOModules_TLMM
  },
  {
    "LPASS",
    (DALSYSMemAddr)0xfe000000,
    0x00300000,
    (DALSYSMemAddr)LPASS_BASE,
    HWIOModules_LPASS
  },
  {
    "SYSTEM_NOC",
    (DALSYSMemAddr)0xfc460000,
    0x00005000,
    (DALSYSMemAddr)SYSTEM_NOC_BASE,
    HWIOModules_SYSTEM_NOC
  },
  {
    "CONFIG_NOC",
    (DALSYSMemAddr)0xfc480000,
    0x00001000,
    (DALSYSMemAddr)CONFIG_NOC_BASE,
    HWIOModules_CONFIG_NOC
  },
  { NULL, 0, 0, 0, NULL }
};

