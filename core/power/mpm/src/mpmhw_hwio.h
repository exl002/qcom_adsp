#ifndef __MPMHW_HWIO_H__
#define __MPMHW_HWIO_H__
/*
===========================================================================
*/
/**
  @file mpmhw_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    MPM2_SLP_CNTR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.adsp/2.2/power/mpm/src/mpmhw_hwio.h#1 $
  $DateTime: 2013/04/03 17:22:53 $
  $Author: coresvc $

  ===========================================================================
*/
#include "HALcomdef.h"

extern uint8* mpm_hw_base_addr;
#define MPM2_MPM_BASE (uint32)mpm_hw_base_addr

/*----------------------------------------------------------------------------
 * MODULE: MPM2_SLP_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_SLP_CNTR_REG_BASE                                    (MPM2_MPM_BASE      + 0x00003000)
#define MPM2_SLP_CNTR_REG_BASE_OFFS                               0x00003000

#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR               (MPM2_SLP_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_OFFS               (MPM2_SLP_CNTR_REG_BASE_OFFS + 0x00000000)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK               0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_IN          \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_RMSK)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_ADDR, m)
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_BMSK          0xffffffff
#define HWIO_MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL_DATA_SHFT                 0x0


#endif /* __MPMHW_HWIO_H__ */

