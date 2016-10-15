#ifndef SNS_SMGR_HW_H
#define SNS_SMGR_HW_H
/*=============================================================================
  @file sns_smgr_hw.h
 
  This header file contains the interface between  HW  and
  Sensor Manager (SMGR)
 
******************************************************************************   
* Copyright (c) 2011-2012 QUALCOMM Technologies, Inc.  All Rights Reserved
* All Rights Reserved.	  Qualcomm Technologies Confidential and Proprietary
******************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_hw.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-10-18  pn   Removed sns_hw_init()
  2013-06-23  lka  Added function prototypes for SSI.
  2013-06-12  agk  Added client type for NPA latency node voting
  2013-04-11  dc   Read GPIO information from registry and use them for hw init.
  2012-12-03  pn   Added Buffering feature
  2012-03-02  pn   added sns_hw_gpio_out_config() and sns_hw_gpio_out()
  2012-01-18  br   inserted latency node interface 
  2011-12-27  sd   added sns_hw_qup_clck_status
  2011-08-13  sd   added sns_hw_send_powerrail_msg_tmr_cb_proc
  2011-06-17  sd   smgr code clean up
  2011-05-02  SD   Initial version
  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#if !defined(SNS_PCSIM)
#include "npa.h"
#include "npa_resource.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
  SNS_SMGR_POWER_HIGH    =0,
  SNS_SMGR_POWER_LOW     =1,
  SNS_SMGR_POWER_OFF    = 2,
} smgr_power_state_type_e;

typedef enum
{
  SNS_SMGR_NPA_CLIENT_SYNC = 0,
  SNS_SMGR_NPA_CLIENT_ASYNC = 1,
} smgr_npa_client_type_e;

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

void sns_smgr_md_int_gpio_config(void);

void sns_hw_set_qup_clk(bool  enable_flag);

void sns_hw_power_rail_config(smgr_power_state_type_e enable_flag);

void sns_hw_bd_config(void);

smgr_power_state_type_e sns_hw_powerrail_status(void);

boolean sns_hw_qup_clck_status(void);

void sns_hw_send_powerrail_msg_tmr_cb_proc(void);

void sns_hw_power_set_latency_node_us (uint32_t latency_us);

void sns_hw_power_npa_vote_vdd(uint32_t vdd_state);
void sns_hw_power_npa_vote_wakeup(uint32_t wake_state);
void sns_hw_power_npa_vote_latency(smgr_npa_client_type_e latencyClientType, uint32_t latency_state);

int sns_hw_gpio_out_config(uint16_t gpio_num);

int sns_hw_gpio_out(uint16_t gpio_num, boolean b_to_high);

void sns_hw_update_ssi_reg_items(uint16_t Id, uint8_t* data_ptr);

void sns_hw_set_smgr_ssi_recv(bool state);

#endif /* #ifndef SNS_SMGR_HW_H */
