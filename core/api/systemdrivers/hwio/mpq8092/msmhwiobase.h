#ifndef __MSMHWIOBASE_H__
#define __MSMHWIOBASE_H__
/*
===========================================================================
*/
/**
  @file msmhwiobase.h
  @brief Auto-generated HWIO base include file.
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.adsp/2.2/api/systemdrivers/hwio/mpq8092/msmhwiobase.h#2 $
  $DateTime: 2013/08/12 06:17:02 $
  $Author: coresvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * BASE: PERIPH_SS
 *--------------------------------------------------------------------------*/

#define PERIPH_SS_BASE                                              0xe0000000
#define PERIPH_SS_BASE_SIZE                                         0x00400000
#define PERIPH_SS_BASE_PHYS                                         0xf9800000

/*----------------------------------------------------------------------------
 * BASE: RPM
 *--------------------------------------------------------------------------*/

#define RPM_BASE                                                    0xe0400000
#define RPM_BASE_SIZE                                               0x00090000
#define RPM_BASE_PHYS                                               0xfc100000

/*----------------------------------------------------------------------------
 * BASE: QDSS_APB
 *--------------------------------------------------------------------------*/

#define QDSS_APB_BASE                                               0xe0500000
#define QDSS_APB_BASE_SIZE                                          0x00028000
#define QDSS_APB_BASE_PHYS                                          0xfc300000

/*----------------------------------------------------------------------------
 * BASE: QDSS_AHB
 *--------------------------------------------------------------------------*/

#define QDSS_AHB_BASE                                               0xe0670000
#define QDSS_AHB_BASE_SIZE                                          0x00018000
#define QDSS_AHB_BASE_PHYS                                          0xfc370000

/*----------------------------------------------------------------------------
 * BASE: CLK_CTL
 *--------------------------------------------------------------------------*/

#define CLK_CTL_BASE                                                0xe0700000
#define CLK_CTL_BASE_SIZE                                           0x00020000
#define CLK_CTL_BASE_PHYS                                           0xfc400000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_START_ADDRESS_BASE                           0xe0828000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE                      0x00004000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS                      0xfc428000

/*----------------------------------------------------------------------------
 * BASE: RPM_SS_MSG_RAM_END_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_SS_MSG_RAM_END_ADDRESS_BASE_PHYS                        0xfc42ffff

/*----------------------------------------------------------------------------
 * BASE: SYSTEM_NOC
 *--------------------------------------------------------------------------*/

#define SYSTEM_NOC_BASE                                             0xe0960000
#define SYSTEM_NOC_BASE_SIZE                                        0x00005000
#define SYSTEM_NOC_BASE_PHYS                                        0xfc460000

/*----------------------------------------------------------------------------
 * BASE: CONFIG_NOC
 *--------------------------------------------------------------------------*/

#define CONFIG_NOC_BASE                                             0xe0a80000
#define CONFIG_NOC_BASE_SIZE                                        0x00001000
#define CONFIG_NOC_BASE_PHYS                                        0xfc480000

/*----------------------------------------------------------------------------
 * BASE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_BASE                                               0xe0ba0000
#define MPM2_MPM_BASE_SIZE                                          0x0000c000
#define MPM2_MPM_BASE_PHYS                                          0xfc4a0000

/*----------------------------------------------------------------------------
 * BASE: SPDM_WRAPPER_TOP
 *--------------------------------------------------------------------------*/

#define SPDM_WRAPPER_TOP_BASE                                       0xe0cb0000
#define SPDM_WRAPPER_TOP_BASE_SIZE                                  0x00006000
#define SPDM_WRAPPER_TOP_BASE_PHYS                                  0xfc4b0000

/*----------------------------------------------------------------------------
 * BASE: SECURITY_CONTROL
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_BASE                                       0xe0db8000
#define SECURITY_CONTROL_BASE_SIZE                                  0x00008000
#define SECURITY_CONTROL_BASE_PHYS                                  0xfc4b8000

/*----------------------------------------------------------------------------
 * BASE: PMIC_ARB
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_BASE                                               0xe0ec0000
#define PMIC_ARB_BASE_SIZE                                          0x00010000
#define PMIC_ARB_BASE_PHYS                                          0xfc4c0000

/*----------------------------------------------------------------------------
 * BASE: CORE_TOP_CSR
 *--------------------------------------------------------------------------*/

#define CORE_TOP_CSR_BASE                                           0xe0f80000
#define CORE_TOP_CSR_BASE_SIZE                                      0x00030000
#define CORE_TOP_CSR_BASE_PHYS                                      0xfd480000

/*----------------------------------------------------------------------------
 * BASE: TLMM
 *--------------------------------------------------------------------------*/

#define TLMM_BASE                                                   0xe1000000
#define TLMM_BASE_SIZE                                              0x00020000
#define TLMM_BASE_PHYS                                              0xfd500000

/*----------------------------------------------------------------------------
 * BASE: LPASS
 *--------------------------------------------------------------------------*/

#define LPASS_BASE                                                  0xee000000
#define LPASS_BASE_SIZE                                             0x00300000
#define LPASS_BASE_PHYS                                             0xfe000000


#endif /* __MSMHWIOBASE_H__ */
