#ifndef __ABTIMEOUT_HWIO_H__
#define __ABTIMEOUT_HWIO_H__
/*
===========================================================================
*/
/**
  @file ABTimeout_HWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    LPASS_AHBE_TIME
    LPASS_AHBI_TIME
    TCSR_TCSR_REGS

  'Include' filters applied: BASE[LPASS_AHBE_TIME] BASE[LPASS_AHBI_TIME] TIMEOUT_INTR[TCSR_TCSR_REGS] 
  'Exclude' filters applied: RESERVED DUMMY RESERVED DUMMY [LPASS_AHBE_TIME] RESERVED DUMMY [LPASS_AHBI_TIME] RESERVED DUMMY[TCSR_TCSR_REGS] 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.adsp/2.2/buses/icb/src/9x25/ABTimeout_HWIO.h#1 $
  $DateTime: 2013/04/03 17:22:53 $
  $Author: coresvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: LPASS_AHBE_TIME
 *--------------------------------------------------------------------------*/

#define LPASS_AHBE_TIME_REG_BASE                                 (LPASS_BASE      + 0x0003a000)

/*----------------------------------------------------------------------------
 * MODULE: LPASS_AHBI_TIME
 *--------------------------------------------------------------------------*/

#define LPASS_AHBI_TIME_REG_BASE                                 (LPASS_BASE      + 0x00052000)

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                           (CORE_TOP_CSR_BASE      + 0x00020000)

#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x00000800)

#define HWIO_TCSR_TIMEOUT_INTR_PENDING_ADDR                                                                               (TCSR_TCSR_REGS_REG_BASE      + 0x00008020)
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_RMSK                                                                                 0x7efdff
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_IN          \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_PENDING_ADDR, HWIO_TCSR_TIMEOUT_INTR_PENDING_RMSK)
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_PENDING_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_LPASS_IRQ_OUT_AHB_TIMEOUT_1_BMSK                                                     0x400000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_LPASS_IRQ_OUT_AHB_TIMEOUT_1_SHFT                                                         0x16
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_LPASS_IRQ_OUT_AHB_TIMEOUT_0_BMSK                                                     0x200000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_LPASS_IRQ_OUT_AHB_TIMEOUT_0_SHFT                                                         0x15
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_BMSK                                                    0x100000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_MSS_CONFIG_TIMEOUT_SLAVE_IRQ_SHFT                                                        0x14
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_QHS_MMSS2_BUS_TIMEOUT_IRQ_BMSK                                                        0x80000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_QHS_MMSS2_BUS_TIMEOUT_IRQ_SHFT                                                           0x13
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_QHS_MMSS1_BUS_TIMEOUT_IRQ_BMSK                                                        0x40000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_QHS_MMSS1_BUS_TIMEOUT_IRQ_SHFT                                                           0x12
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_QHS_MMSS_BUS_TIMEOUT_IRQ_BMSK                                                         0x20000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_QHS_MMSS_BUS_TIMEOUT_IRQ_SHFT                                                            0x11
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_4_IRQ_BMSK                                                             0x8000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_4_IRQ_SHFT                                                                0xf
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_3_IRQ_BMSK                                                             0x4000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_3_IRQ_SHFT                                                                0xe
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_2_IRQ_BMSK                                                             0x2000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_2_IRQ_SHFT                                                                0xd
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_1_IRQ_BMSK                                                             0x1000
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_1_IRQ_SHFT                                                                0xc
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_0_IRQ_BMSK                                                              0x800
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_PSS_BUS_TIMEOUT_0_IRQ_SHFT                                                                0xb
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_SNOC_S0_IRQ_BMSK                                                            0x400
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_SNOC_S0_IRQ_SHFT                                                              0xa
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_SNOC_S2_IRQ_BMSK                                                            0x100
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_SNOC_S2_IRQ_SHFT                                                              0x8
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_SNOC_S3_IRQ_BMSK                                                             0x80
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_SNOC_S3_IRQ_SHFT                                                              0x7
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S6_IRQ_BMSK                                                             0x40
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S6_IRQ_SHFT                                                              0x6
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S5_IRQ_BMSK                                                             0x20
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S5_IRQ_SHFT                                                              0x5
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S4_IRQ_BMSK                                                             0x10
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S4_IRQ_SHFT                                                              0x4
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S3_IRQ_BMSK                                                              0x8
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S3_IRQ_SHFT                                                              0x3
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S2_IRQ_BMSK                                                              0x4
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S2_IRQ_SHFT                                                              0x2
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S1_IRQ_BMSK                                                              0x2
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S1_IRQ_SHFT                                                              0x1
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S0_IRQ_BMSK                                                              0x1
#define HWIO_TCSR_TIMEOUT_INTR_PENDING_BUS_TIMEOUT_CNOC_S0_IRQ_SHFT                                                              0x0

#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR                                                                          (TCSR_TCSR_REGS_REG_BASE      + 0x00008050)
#define HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_RMSK                                                                            0x7efdff

#endif /* __ABTIMEOUT_HWIO_H__ */