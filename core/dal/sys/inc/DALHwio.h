/*
===========================================================================
*/
/**
  @file DALHwio.h
  @brief Semi-auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TCSR_TCSR_MUTEX
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.adsp/2.2/dal/sys/inc/DALHwio.h#1 $
  $DateTime: 2013/05/03 17:59:48 $
  $Author: coresvc $

  ===========================================================================
*/

#ifndef __DAL_HWIO_H__
#define __DAL_HWIO_H__

#ifdef CORE_TOP_CSR_BASE
#undef CORE_TOP_CSR_BASE
#endif
extern uint8 *HWMutexBases;
#define CORE_TOP_CSR_BASE                           HWMutexBase

/* Needed only for the DAL MapRegion function, on procs that use DAL. */
#define HW_MUTEX_BASE_NAME ("CORE_TOP_CSR")

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_MUTEX
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_MUTEX_REG_BASE                                            ( CORE_TOP_CSR_BASE      + 0x00004000 )
#define TCSR_TCSR_MUTEX_REG_BASE_PHYS                                       ( CORE_TOP_CSR_BASE_PHYS + 0x00004000 )
#define TCSR_TCSR_MUTEX_REG_BASE_SIZE                                       0x00005380

#define HWIO_TCSR_MUTEX_REG_n_ADDR(n)                                       ( TCSR_TCSR_MUTEX_REG_BASE      + 0x00000000 + 0x80 * (n) )
#define HWIO_TCSR_MUTEX_REG_n_RMSK                                                0xff
#define HWIO_TCSR_MUTEX_REG_n_MAXn                                                  31
#define HWIO_TCSR_MUTEX_REG_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), HWIO_TCSR_MUTEX_REG_n_RMSK)
#define HWIO_TCSR_MUTEX_REG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), (mask))
#define HWIO_TCSR_MUTEX_REG_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_MUTEX_REG_n_ADDR(n),val)
#define HWIO_TCSR_MUTEX_REG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_MUTEX_REG_n_ADDR(n),mask,val,HWIO_TCSR_MUTEX_REG_n_INI(n))
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_BMSK                                          0xff
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_SHFT                                           0x0

#endif /* __DAL_HWIO_H__ */
