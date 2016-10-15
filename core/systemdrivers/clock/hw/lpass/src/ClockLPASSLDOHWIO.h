#ifndef __CLOCKLPASSLDOHWIO_H__
#define __CLOCKLPASSLDOHWIO_H__
/*
===========================================================================
*/
/**
  @file ClockLPASSLDOHWIO.h 
  
  The file contains the resource definitions for LDO voltage scaling on
  the Low Power Audio Subsystem processor for 9x25.
*/
/*  
  ====================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.adsp/2.2/systemdrivers/clock/hw/lpass/src/ClockLPASSLDOHWIO.h#3 $
  $DateTime: 2013/08/14 17:27:17 $
  $Author: coresvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  06/12/12   dcf     Created for LPASS on 8974.

  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include <HALhwio.h>

/*=========================================================================
      Type Definitions
==========================================================================*/

extern uint32                      Clock_nHWIOBaseLPASS;
#define LPASS_BASE                 Clock_nHWIOBaseLPASS

extern uint32                      Clock_nHWIOBaseSecurity;
#define SECURITY_CONTROL_BASE      Clock_nHWIOBaseSecurity


/*=========================================================================
      Macros
==========================================================================*/

#define LPASS_QDSP6SS_PUB_REG_BASE                                      (LPASS_BASE      + 0x00200000)
#define LPASS_QDSP6SS_PUB_REG_BASE_OFFS                                 0x00200000

#define HWIO_LPASS_QDSP6SS_LDO_CFG0_ADDR                                (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000050)
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_OFFS                                (LPASS_QDSP6SS_PUB_REG_BASE_OFFS + 0x00000050)
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_RMSK                                 0x7ffffff
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_CFG0_ADDR, HWIO_LPASS_QDSP6SS_LDO_CFG0_RMSK)
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_CFG0_ADDR, m)
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_LDO_CFG0_ADDR,v)
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_LDO_CFG0_ADDR,m,v,HWIO_LPASS_QDSP6SS_LDO_CFG0_IN)
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_TEST_BMSK                        0x4000000
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_TEST_SHFT                             0x1a
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_BYP_LDO_EXT_BMSK                     0x2000000
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_BYP_LDO_EXT_SHFT                          0x19
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_IDAC_EN_BMSK                         0x1000000
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_IDAC_EN_SHFT                              0x18
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_CTL2_BMSK                         0xff0000
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_CTL2_SHFT                             0x10
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_CTL1_BMSK                           0xff00
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_CTL1_SHFT                              0x8
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_CTL0_BMSK                             0xff
#define HWIO_LPASS_QDSP6SS_LDO_CFG0_LDO_CTL0_SHFT                              0x0

#define HWIO_LPASS_QDSP6SS_LDO_CFG1_ADDR                                (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000054)
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_OFFS                                (LPASS_QDSP6SS_PUB_REG_BASE_OFFS + 0x00000054)
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_RMSK                                  0xffffff
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_CFG1_ADDR, HWIO_LPASS_QDSP6SS_LDO_CFG1_RMSK)
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_CFG1_ADDR, m)
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_LDO_CFG1_ADDR,v)
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_LDO_CFG1_ADDR,m,v,HWIO_LPASS_QDSP6SS_LDO_CFG1_IN)
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_DIG_RSV2_BMSK                         0xff0000
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_DIG_RSV2_SHFT                             0x10
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_DIG_RSV1_BMSK                           0xff00
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_DIG_RSV1_SHFT                              0x8
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_DIG_CTL_BMSK                              0xff
#define HWIO_LPASS_QDSP6SS_LDO_CFG1_DIG_CTL_SHFT                               0x0

#define HWIO_LPASS_QDSP6SS_LDO_CFG2_ADDR                                (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000058)
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_OFFS                                (LPASS_QDSP6SS_PUB_REG_BASE_OFFS + 0x00000058)
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_RMSK                                    0xffff
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_CFG2_ADDR, HWIO_LPASS_QDSP6SS_LDO_CFG2_RMSK)
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_CFG2_ADDR, m)
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_LDO_CFG2_ADDR,v)
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_LDO_CFG2_ADDR,m,v,HWIO_LPASS_QDSP6SS_LDO_CFG2_IN)
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_RDAC_CTL2_BMSK                          0xff00
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_RDAC_CTL2_SHFT                             0x8
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_RDAC_CTL1_BMSK                            0xff
#define HWIO_LPASS_QDSP6SS_LDO_CFG2_RDAC_CTL1_SHFT                             0x0

#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_ADDR                            (LPASS_QDSP6SS_PUB_REG_BASE      + 0x0000005c)
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_OFFS                            (LPASS_QDSP6SS_PUB_REG_BASE_OFFS + 0x0000005c)
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_RMSK                                0x7f7f
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_VREF_SET_ADDR, HWIO_LPASS_QDSP6SS_LDO_VREF_SET_RMSK)
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_LDO_VREF_SET_ADDR, m)
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_LDO_VREF_SET_ADDR,v)
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_LDO_VREF_SET_ADDR,m,v,HWIO_LPASS_QDSP6SS_LDO_VREF_SET_IN)
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_VREF_RET_BMSK                       0x7f00
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_VREF_RET_SHFT                          0x8
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_VREF_LDO_BMSK                         0x7f
#define HWIO_LPASS_QDSP6SS_LDO_VREF_SET_VREF_LDO_SHFT                          0x0

#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_ADDR                                 (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000030)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_OFFS                                 (LPASS_QDSP6SS_PUB_REG_BASE_OFFS + 0x00000030)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_RMSK                                  0xfff0007
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PWR_CTL_V1_ADDR, HWIO_LPASS_QDSP6SS_PWR_CTL_V1_RMSK)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PWR_CTL_V1_ADDR, m)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_PWR_CTL_V1_ADDR,v)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_PWR_CTL_V1_ADDR,m,v,HWIO_LPASS_QDSP6SS_PWR_CTL_V1_IN)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_BG_PU_BMSK                        0x8000000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_BG_PU_SHFT                             0x1b
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_VREF_SEL_BMSK                     0x4000000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_VREF_SEL_SHFT                          0x1a
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_PWR_UP_BMSK                       0x3000000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_PWR_UP_SHFT                            0x18
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_BYP_BMSK                           0xc00000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_LDO_BYP_SHFT                               0x16
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_BHS_ON_BMSK                            0x200000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_BHS_ON_SHFT                                0x15
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_CLAMP_IO_BMSK                          0x100000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_CLAMP_IO_SHFT                              0x14
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_SLP_RET_N_BMSK                          0x80000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_SLP_RET_N_SHFT                             0x13
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_L2DATA_STBY_N_BMSK                      0x40000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_L2DATA_STBY_N_SHFT                         0x12
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_ETB_SLP_NRET_N_BMSK                     0x20000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_ETB_SLP_NRET_N_SHFT                        0x11
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_L2TAG_SLP_NRET_N_BMSK                   0x10000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_L2TAG_SLP_NRET_N_SHFT                      0x10
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_L2DATA_SLP_NRET_N_BMSK                      0x7
#define HWIO_LPASS_QDSP6SS_PWR_CTL_V1_L2DATA_SLP_NRET_N_SHFT                      0x0


#define HWIO_LPASS_QDSP6SS_PWR_CTL_ADDR                                         (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000030)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_RMSK                                          0x77fe0ff
#define HWIO_LPASS_QDSP6SS_PWR_CTL_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PWR_CTL_ADDR, HWIO_LPASS_QDSP6SS_PWR_CTL_RMSK)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_PWR_CTL_ADDR, m)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_PWR_CTL_ADDR,v)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_PWR_CTL_ADDR,m,v,HWIO_LPASS_QDSP6SS_PWR_CTL_IN)
#define HWIO_LPASS_QDSP6SS_PWR_CTL_LDO_PWR_UP_BMSK                               0x4000000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_LDO_PWR_UP_SHFT                                    0x1a
#define HWIO_LPASS_QDSP6SS_PWR_CTL_LDO_BYP_BMSK                                  0x2000000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_LDO_BYP_SHFT                                       0x19
#define HWIO_LPASS_QDSP6SS_PWR_CTL_BHS_ON_BMSK                                   0x1000000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_BHS_ON_SHFT                                        0x18
#define HWIO_LPASS_QDSP6SS_PWR_CTL_CLAMP_QMC_MEM_BMSK                             0x400000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_CLAMP_QMC_MEM_SHFT                                 0x16
#define HWIO_LPASS_QDSP6SS_PWR_CTL_CLAMP_WL_BMSK                                  0x200000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_CLAMP_WL_SHFT                                      0x15
#define HWIO_LPASS_QDSP6SS_PWR_CTL_CLAMP_IO_BMSK                                  0x100000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_CLAMP_IO_SHFT                                      0x14
#define HWIO_LPASS_QDSP6SS_PWR_CTL_SLP_RET_N_BMSK                                  0x80000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_SLP_RET_N_SHFT                                     0x13
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2DATA_STBY_N_BMSK                              0x40000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2DATA_STBY_N_SHFT                                 0x12
#define HWIO_LPASS_QDSP6SS_PWR_CTL_ETB_SLP_NRET_N_BMSK                             0x20000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_ETB_SLP_NRET_N_SHFT                                0x11
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2TAG_SLP_NRET_N_BMSK                           0x10000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2TAG_SLP_NRET_N_SHFT                              0x10
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L1IU_SLP_NRET_N_BMSK                             0x8000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L1IU_SLP_NRET_N_SHFT                                0xf
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L1DU_SLP_NRET_N_BMSK                             0x4000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L1DU_SLP_NRET_N_SHFT                                0xe
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2PLRU_SLP_NRET_N_BMSK                           0x2000
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2PLRU_SLP_NRET_N_SHFT                              0xd
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2DATA_SLP_NRET_N_BMSK                             0xff
#define HWIO_LPASS_QDSP6SS_PWR_CTL_L2DATA_SLP_NRET_N_SHFT                              0x0

#define HWIO_LPASS_QDSP6SS_STRAP_ACC_ADDR                               (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000110)
#define HWIO_LPASS_QDSP6SS_STRAP_ACC_RMSK                               0xffffffff
#define HWIO_LPASS_QDSP6SS_STRAP_ACC_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_STRAP_ACC_ADDR, HWIO_LPASS_QDSP6SS_STRAP_ACC_RMSK)
#define HWIO_LPASS_QDSP6SS_STRAP_ACC_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_STRAP_ACC_ADDR, m)
#define HWIO_LPASS_QDSP6SS_STRAP_ACC_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_STRAP_ACC_ADDR,v)
#define HWIO_LPASS_QDSP6SS_STRAP_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_STRAP_ACC_ADDR,m,v,HWIO_LPASS_QDSP6SS_STRAP_ACC_IN)
#define HWIO_LPASS_QDSP6SS_STRAP_ACC_DATA_BMSK                          0xffffffff
#define HWIO_LPASS_QDSP6SS_STRAP_ACC_DATA_SHFT                                 0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                                             (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                                        0x00000000

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_CAL_REDUN_96_73_BMSK                                                 0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_CAL_REDUN_96_73_SHFT                                                      0x0


#endif /* __CLOCKLPASSLDOHWIO_H__ */

