#ifndef __MSMHWIOREG_QDSP6SS_WDT__
#define __MSMHWIOREG_QDSP6SS_WDT__
/*
===========================================================================
*/
/**
  @file msmhwioreg_qdsp6ss_csr
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    LPASS_QDSP6SS_CSR
    LPASS_QDSP6SS_PUB

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/dspcore.adsp/2.2/wdt/inc/msmhwioreg_qdsp6ss_wdt.h#1 $
  $DateTime: 2013/05/23 14:56:28 $
  $Author: coresvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: LPASS_QDSP6SS_CSR
 *--------------------------------------------------------------------------*/

#define LPASS_QDSP6SS_CSR_REG_BASE                                          (LPASS_BASE      + 0x00280000)

#define HWIO_LPASS_QDSP6SS_WDOG_RESET_ADDR                                  (LPASS_QDSP6SS_CSR_REG_BASE      + 0x00002000)
#define HWIO_LPASS_QDSP6SS_WDOG_RESET_RMSK                                         0x1
#define HWIO_LPASS_QDSP6SS_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_WDOG_RESET_ADDR,v)
#define HWIO_LPASS_QDSP6SS_WDOG_RESET_WDOG_RESET_BMSK                              0x1
#define HWIO_LPASS_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT                              0x0

#define HWIO_LPASS_QDSP6SS_WDOG_CTL_ADDR                                    (LPASS_QDSP6SS_CSR_REG_BASE      + 0x00002004)
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_RMSK                                           0x7
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_CTL_ADDR, HWIO_LPASS_QDSP6SS_WDOG_CTL_RMSK)
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_CTL_ADDR, m)
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_WDOG_CTL_ADDR,v)
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_WDOG_CTL_ADDR,m,v,HWIO_LPASS_QDSP6SS_WDOG_CTL_IN)
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK                            0x4
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT                            0x2
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_BMSK                           0x2
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_SHFT                           0x1
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_ENABLE_BMSK                                    0x1
#define HWIO_LPASS_QDSP6SS_WDOG_CTL_ENABLE_SHFT                                    0x0

#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_ADDR                                 (LPASS_QDSP6SS_CSR_REG_BASE      + 0x00002008)
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_RMSK                                 0xf81fffff
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_STATUS_ADDR, HWIO_LPASS_QDSP6SS_WDOG_STATUS_RMSK)
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_STATUS_ADDR, m)
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_BMSK               0x80000000
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_SHFT                     0x1f
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_BMSK           0x40000000
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_SHFT                 0x1e
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_BMSK           0x20000000
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_SHFT                 0x1d
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_BMSK            0x10000000
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_SHFT                  0x1c
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_BMSK            0x8000000
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_SHFT                 0x1b
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_WDOG_COUNT_BMSK                        0x1ffffe
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_WDOG_COUNT_SHFT                             0x1
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_BMSK                    0x1
#define HWIO_LPASS_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_SHFT                    0x0

#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_ADDR                              (LPASS_QDSP6SS_CSR_REG_BASE      + 0x0000200c)
#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_RMSK                                 0xfffff
#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_ADDR, HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_RMSK)
#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_ADDR, m)
#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_ADDR,v)
#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_ADDR,m,v,HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_IN)
#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK                            0xfffff
#define HWIO_LPASS_QDSP6SS_WDOG_BARK_TIME_DATA_SHFT                                0x0

#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_ADDR                              (LPASS_QDSP6SS_CSR_REG_BASE      + 0x00002010)
#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_RMSK                                 0xfffff
#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_ADDR, HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_RMSK)
#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_ADDR, m)
#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_ADDR,v)
#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_ADDR,m,v,HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_IN)
#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK                            0xfffff
#define HWIO_LPASS_QDSP6SS_WDOG_BITE_TIME_DATA_SHFT                                0x0

#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_ADDR                               (LPASS_QDSP6SS_CSR_REG_BASE      + 0x00002014)
#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_RMSK                                  0xfffff
#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_ADDR, HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_RMSK)
#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_ADDR, m)
#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_ADDR,v)
#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_ADDR,m,v,HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_IN)
#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_DATA_BMSK                             0xfffff
#define HWIO_LPASS_QDSP6SS_WDOG_NMI_TIME_DATA_SHFT                                 0x0

#define HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_ADDR                              (LPASS_QDSP6SS_CSR_REG_BASE      + 0x00002018)
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_RMSK                                     0x1
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_LOAD_BMSK                                0x1
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_LOAD_SHFT                                0x0

#define HWIO_LPASS_QDSP6SS_WDOG_TEST_ADDR                                   (LPASS_QDSP6SS_CSR_REG_BASE      + 0x0000201c)
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_RMSK                                     0x1fffff
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_TEST_ADDR, HWIO_LPASS_QDSP6SS_WDOG_TEST_RMSK)
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_WDOG_TEST_ADDR, m)
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_WDOG_TEST_ADDR,v)
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_QDSP6SS_WDOG_TEST_ADDR,m,v,HWIO_LPASS_QDSP6SS_WDOG_TEST_IN)
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_SYNC_STATUS_BMSK                         0x100000
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_SYNC_STATUS_SHFT                             0x14
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_VALUE_BMSK                           0xfffff
#define HWIO_LPASS_QDSP6SS_WDOG_TEST_LOAD_VALUE_SHFT                               0x0


/*----------------------------------------------------------------------------
 * MODULE: LPASS_QDSP6SS_PUB
 *--------------------------------------------------------------------------*/

#define LPASS_QDSP6SS_PUB_REG_BASE                                      (LPASS_BASE      + 0x00200000)

#define HWIO_LPASS_QDSP6SS_NMI_ADDR                                     (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000040)
#define HWIO_LPASS_QDSP6SS_NMI_RMSK                                            0x3
#define HWIO_LPASS_QDSP6SS_NMI_OUT(v)      \
        out_dword(HWIO_LPASS_QDSP6SS_NMI_ADDR,v)
#define HWIO_LPASS_QDSP6SS_NMI_CLEAR_STATUS_BMSK                               0x2
#define HWIO_LPASS_QDSP6SS_NMI_CLEAR_STATUS_SHFT                               0x1
#define HWIO_LPASS_QDSP6SS_NMI_SET_NMI_BMSK                                    0x1
#define HWIO_LPASS_QDSP6SS_NMI_SET_NMI_SHFT                                    0x0

#define HWIO_LPASS_QDSP6SS_NMI_STATUS_ADDR                              (LPASS_QDSP6SS_PUB_REG_BASE      + 0x00000044)
#define HWIO_LPASS_QDSP6SS_NMI_STATUS_RMSK                                     0x3
#define HWIO_LPASS_QDSP6SS_NMI_STATUS_IN          \
        in_dword_masked(HWIO_LPASS_QDSP6SS_NMI_STATUS_ADDR, HWIO_LPASS_QDSP6SS_NMI_STATUS_RMSK)
#define HWIO_LPASS_QDSP6SS_NMI_STATUS_INM(m)      \
        in_dword_masked(HWIO_LPASS_QDSP6SS_NMI_STATUS_ADDR, m)
#define HWIO_LPASS_QDSP6SS_NMI_STATUS_WDOG_TRIG_BMSK                           0x2
#define HWIO_LPASS_QDSP6SS_NMI_STATUS_WDOG_TRIG_SHFT                           0x1
#define HWIO_LPASS_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_BMSK                         0x1
#define HWIO_LPASS_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_SHFT                         0x0

#endif /* __MSMHWIOREG_QDSP6SS_WDT__ */
