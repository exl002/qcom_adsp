#ifndef SNS_PM_PRIV_H
#define SNS_PM_PRIV_H

/*============================================================================

  @file sns_pm_priv.h

  @brief
  This file contains definitions for use by only Sensors Power Manager

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

============================================================================*/


/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 
//source/qcom/qct/core/sensors/dsps/dsps/core/dev/pcsim/ucos-ii_pc/dsps/smr/inc/sns_pm.h#1 
$ 


when         who     what, where, why
----------   ---     ---------------------------------------------------------
02-19-2013   jhh     Add macro for logging
10-31-2012   jhh     Add new definition SNS_PM_INACTIVE
08-08-2012   jhh     Redirect PM_LOG to ULOG and cleanup signals between SMR and QMI
07-31-2012   ag      Cleanup macros
07-27-2012   ps      Added QMI related signal 
07-07-2011   sj      Making test thread work for power measurements
05-20-2011   sj      Featurize on SNS_PM_TEST as this is a possible 
                     test thread on target
05-19-2011   sj      Added checks before writing to ULOG
04-25-2011   sj      Added define for num of active states
04-19-2011   sj      Fixed PC SIM compile time flag name
04-13-2011   sj      Added support for logging through ULOG
03-31-2011   sj      Initial version
============================================================================*/
 
/*=======================================================================
                  EXTERNAL DEFINITIONS AND TYPES
========================================================================*/
#include "sns_memmgr.h"
#include "sns_common.h"
#include "sns_pm_api_v01.h"

#ifdef OCMEM_ENABLE
#include "ocmem_sensors.h"
#endif

#ifndef SNS_PCSIM
#include "ULogFront.h"  /* For logging through ULOG */
#endif


/*=======================================================================
                  INTERNAL DEFINITIONS AND TYPES
========================================================================*/
#define SNS_PM_NUM_ACTIVE_STATES 2
#define SNS_PM_NUM_VOTING_CLIENTS 2
#define SNS_PM_SRC_MODULE_SMGR 0
#define SNS_PM_SRC_MODULE_SAM  1
#define SNS_PM_ACTIVE_LOW  SNS_PM_ST_ACTIVE_LOW_V01
#define SNS_PM_ACTIVE_HIGH SNS_PM_ST_ACTIVE_HIGH_V01
#define SNS_PM_INACTIVE    SNS_PM_ST_INACTIVE_V01

/* PM Signal and Signal Mask */
#define SNS_PM_MSG_SIG              0x1
#define SNS_PM_SIGNAL_MASK          (SNS_PM_MSG_SIG)

#ifdef SNS_QMI_ENABLE
#define PM_QMI_WAIT_SIG             (0x1 << 1)
#endif

/* PM Signal and Signal Mask */
#define SNS_PM_SMR_MSG_SIG          (0x1 << 0)
#define SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON          (0x1 << 4)
#define SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF         (0x1 << 5)
#define SNS_PM_ADSPPM_POWER_VOTE_CB_SIG     (0x1 << 6)
#define SNS_PM_SMR_SIGNAL_MASK  (SNS_PM_SMR_MSG_SIG + SNS_PM_ADSPPM_OCMEM_CB_SIG + SNS_PM_ADSPPM_POWER_VOTE_CB_SIG)

#ifdef OCMEM_ENABLE
/* SMDL READ/WRITE LENGTH */
#define OCMEM_MAX_MSG_LEN ((uint32_t)(850 + sizeof(sns_ocmem_hdr_s)))

/* SMDL and QMI flgas */
#define SNS_SMDL_READ_SIG                   (0x1 << 7)
#define SNS_SMDL_OPEN_SIG                   (0x1 << 8)
#define SNS_SMDL_WRITE_SIG                  (0x1 << 9)
#define SNS_SMDL_CLOSE_SIG                  (0x1 << 10)
#define SNS_PM_QMI_SMGR_CL_MSG_SIG    (0x1 << 2)
#define SNS_PM_QMI_SMGR_CL_TIMER_SIG  (0x1 << 3)


typedef struct {
  sns_ocmem_hdr_s hdr;  
  char body[1];     
} __attribute__((packed,aligned (1))) sns_smdl_msg_s;


#endif

/* Sensors Processor Frequency in low power 27MHz */
#define SNS_PM_LOW_PWR_FREQ_MHZ 27
/* Sensors Processor Frequency in high power 64MHz */
#define SNS_PM_HIGH_PWR_FREQ_MHZ 64


/* Sensors Processor Frequency in low power 27MHz */
#define SNS_PM_LOW_PWR_FREQ_MHZ     27
/* Sensors Processor Frequency in high power 64MHz */
#define SNS_PM_HIGH_PWR_FREQ_MHZ    64

/*========================================================================
                        MACROS
========================================================================*/
/* SNS_UNIT_TEST is defined in sns_common.h */
#if defined(SNS_PCSIM) || defined(SNS_UNIT_TEST)
#define PM_LOG_0(a) printf(a)
#define PM_LOG_1(a,b) printf(a,b)
#define PM_LOG_2(a,b,c) printf(a,b,c)
#define PM_LOG_3(a,b,c,d) printf(a,b,c,d)
#else
extern ULogHandle     sns_pm_ulog_hdl;
extern uint8_t        ulog_init_success;
/* Use ULog for logging */
#define PM_LOG_0(a)   if (ulog_init_success) { ULOG_RT_PRINTF_0(sns_pm_ulog_hdl,a); }
#define PM_LOG_1(a,b) if (ulog_init_success) { ULOG_RT_PRINTF_1(sns_pm_ulog_hdl,a,b); }
#define PM_LOG_2(a,b,c) if (ulog_init_success) { ULOG_RT_PRINTF_2(sns_pm_ulog_hdl,a,b,c); }
#define PM_LOG_3(a,b,c,d) if (ulog_init_success) { ULOG_RT_PRINTF_3(sns_pm_ulog_hdl,a,b,c,d); }
#endif

#if defined(QDSP6)
#define PM_MSG_0(xx_pri, xx_fmt)                            MSG(MSG_SSID_SNS, xx_pri, xx_fmt)
#define PM_MSG_1(xx_pri, xx_fmt, xx_arg1)                   MSG_1(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1)
#define PM_MSG_2(xx_pri, xx_fmt, xx_arg1, xx_arg2)          MSG_2(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2)
#define PM_MSG_3(xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3) MSG_3(MSG_SSID_SNS, xx_pri, xx_fmt, xx_arg1, xx_arg2, xx_arg3)
#endif

/*=======================================================================
                        GLOBAL VARIABLES
========================================================================*/


/*========================================================================
                        LOCAL VARIABLES
========================================================================*/



/*=====================================================================
                        UNIT TEST RELATED FUNCTION PROTOTYPES
=======================================================================*/
/* Unit Test Related prototypes */
#ifdef SNS_UNIT_TEST
/*===========================================================================

  FUNCTION:   sns_pm_ut_init_tests

===========================================================================*/
/*!
  @brief
  Initializes the tests that need to be performed.
   
  @param[i] 
  No input parameters

  @detail
  This function initializes the main_test_trigger array with the tests to 
  perform in this run.
  
  @return
  No return code
 
*/
/*=========================================================================*/
void sns_pm_ut_init_tests(void);

/*===========================================================================

  FUNCTION:   sns_pm_ut_send_msg

===========================================================================*/
/*!
  @brief
  Send message to power manager (faking the votes of SAM and SMGR)
   
  @param[i] 
  No input parameters

  @detail
  This function fakes votes coming from SAM and SMGR.  
  
  @return
  No return code
 
*/
/*=========================================================================*/
void sns_pm_ut_send_msg(void);

/*===========================================================================

  FUNCTION:   sns_pm_ut_check_pm_indications

===========================================================================*/
/*!
  @brief
  Checks indications from power manager.
   
  @param[i] vote_val     : Vote value
  @param[i] voting_module: Module making the vote

  @detail
  Checks the indications returned from Power Manager.
  
  @return
  No return value
 
*/
/*=========================================================================*/
void sns_pm_ut_check_pm_indications(sns_smr_header_s *msg_in_hdr,
                                    void* msg_ind_ptr);
#endif /* SNS_UNIT_TEST */

#endif /* SNS_PM_PRIV_H */
