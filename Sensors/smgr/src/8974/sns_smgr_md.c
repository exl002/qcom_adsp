/*=============================================================================
  @file sns_smgr_md.c

  This file contains handles motion detect and power vote related features.

*******************************************************************************
* Copyright (c) 2011-2013 QUALCOMM Technologies, Inc.  All Rights Reserved
* All Rights Reserved.	  Qualcomm Technologies Confidential and Proprietary
********************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/8974/sns_smgr_md.c#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-09-23  pn  - Only counts active reports in power voting consideration
                  - Considers MD when selecting power rail state to set
  2013-08-30  ad  Fix SMD wakeup failure from stationary state
  2013-08-23  jhh Update client check and requested frequency for power voting
                  and remove sns_smgr_is_external_client()
  2013-06-12  agk Specify client type when voting for latency node
  2013-06-06  pn  sns_smgr_send_indication() signature changed 
  2013-06-02  pn  Added support for back-to-back Buffering indications
  2013-05-22  lka Re-factored internal_process_message() function
  2013-05-21  br  Inserted validity checking in set_hw_md_int()
  2013-05-16  asr Removed unused log types.
  2013-05-01  dc  Update right usage of "true" constant.
  2013-3-20   hw  Change the cutoff frequecy value for pm voting to 66
  2013-03-11  vh  Added NULL condition check in QMI callback functions' memory allocation
  2013-02-07  dc  Allow MD indications to be delivered even when kernel has suspended.
  2013-02-04  pn  Uses matching malloc/free calls
  2013-01-08  sd  removed compare power rail state when calling sns_hw_power_rail_config
  2012-12-05  pn  Powers off Accel sensor when MD interrup is disabled and
                  no Accel reports exist
  2012-12-03  pn  Added Buffering feature
  2012-11-26  vh  Replaced SNS_SMGR_MODULE with SNS_DBG_MOD_DSPS_SMGR
  2012-10-31  jhh Removed unnecessary code section under OCMEM_ENABLE which doesn't apply anymore
  2012-10-23  vh  Processing the messages in task loop
  2012-10-19  ag  Bringup MD on 8974
  2012-10-02  ag  Restructured to look similar to SMR based targets
  2012-09-21  sc  Completed code that sends back response to NUMBER_OF_CLIENTS req.
  2012-09-13  vh  Eliminated compiler warnings
  2012-09-14  sc  Enable SMGR as a client of PM (dependency: sns_pm_init must be done)
  2012-09-13  sc  Disable SMGR being a client of PM, until PM is tested and enabled
  2012-08-29  ag  Added SMGR as client of PM via QCCI; registered SMGR int service via QCSI;
                  disabled NPA calls until they are fixed
  2012-06-05  ag  Before turning off power rail check if a sensor test is ongoing
  2012-04-18  br  Optimized few functions including sns_smgr_set_hw_md_int()
  2012-03-30  sd  Moved changing accel ODR when enabling/disabling MD from SMGR to DD.
  2011-03-16  sd  stop/start accel sampling when MD is enabled/disabled for DRI enabled accel
  2011-03-05  br  deleted controlling GPIO for MD
  2011-11-14  jhh Updated alloc and free function calls to meet new API
  2011-11-04  sd  removed 1ms delay thought needed for the LSM303DLHC, but no need for it
  2011-11-01  sd  only send IND if MD is set successfully
  2011-10-31  agk Hack to always vote for HIGH_POWER in SMGR. To be removed later, see comments in the code.
  2011-10-25  sd  use smr macro SNS_SMR_IS_INTERNAL_MODULE to determine if a module is external
  2011-10-11  sd  changed return false to NULL
  2011-09-21  sd  added back log of SNS_LOG_DRV_MD_IRQ
  2011-09-16  sd  comment out log of SNS_LOG_MD_IRQ before app support
  2011-09-13  sd  removed a condition check to turn off power rail
  2011-08-13  sd  disable MD int if a del request is received, not disable it if DD already disabled after motion detected
  2011-08-13  sd  added some delay between enable qup clock and set sttribute for MD int
                  added some debug strings
  2011-08-10  yk  Added logging when motion detection occurs
  2011-07-28  sd  override old MD request if the source module and report ID are the same
  2011-06-17  sd   smgr code clean up
  2011-06-02  sd  Use DalTlmm_ConfigGpio to config gpio low/high power
                  Use sns_smgr_set_attr to set DD attribute
  2011-05-18  sd  Check power vote and power rail update after MD int happens and suspended reports are de-suspended
  2011-05-16  sd  Remove schedule after all reports are checked and are moved to suspend queue
  2011-05-16  DC  Changed DDITLMM.h to DDITlmm.h to compile on linux(case sensitivity)
  2011-05-10  sd   keep items in report linked in sensor lead when the report is moved to suspended que
                   delete scheduler from schedule que before freeing it.
                   don't update power vote status if voting msg is not sent successfully.
  2011-05-09  SD  compile error fix on target
  2011-05-05  sc  Commented out DAL-related functions for PCSIM build
  2011-05-03  SD  updated power rail update check,
                  moved some functions to sns_smgr_hw.c,
                  seperated MD and power fields in sns_smgr.md,
                  added comments and updated function headers
  2011-04-29  SD   Added sns_smgr_find_same_rpt_in_suspend_que
  2011-04-27  SD   Initial version for 8960
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sensor1.h"
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_init.h"
#include "fixed_point.h"

#include "sns_log_api.h"
#include "sns_log_types.h"

#include <sns_smgr_api_v01.h>  /* Auto-generated by IDL */
#include <sns_pm_api_v01.h>
#include <sns_sam_qmd_v01.h>
#include <sns_sam_amd_v01.h>
#include <sns_common_v01.h>    /* Auto-generated by IDL */
#include "sns_smgr_define.h"
#include "sns_smgr.h"
#include "sns_dd.h"

#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_smgr_hw.h"

#include "npa.h"

#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_SMGR_MAX_FRQ_TO_VOTE_LOW_POWER 66
#define SNS_SMGR_MAX_NUM_CLIENT_TO_VOTE_LOW_POWER 3

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Macro Definition
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
extern  boolean              enable_num_clnt_ind;
extern  npa_client_handle    reqWakeUpClient;
extern  npa_client_handle    reqLatencyClient;
extern  qmi_client_type      smgr_pm_cl_user_handle;
extern  sns_q_s              smgr_cl_queue;
extern  OS_EVENT             *smgr_cl_que_mutex_ptr;

/*----------------------------------------------------------------------------
 *  Functions
 *
 * -------------------------------------------------------------------------*/

void sns_smgr_del_md_all_clients(void);
void sns_smgr_cancel_internal_service(void *connection_handle);

/*===========================================================================

  FUNCTION:   smgr_int_handle_req_cb

===========================================================================*/
/*!
  @brief Callback registered with QCSI to receive service requests
 */
/*=========================================================================*/
qmi_csi_cb_error smgr_int_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
)
{
  return sns_smgr_q_put(connection_handle,
                        req_handle,
                        msg_id,
                        req_c_struct,
                        req_c_struct_len,
                        QMI_IDL_REQUEST,
                        SNS_SMGR_MSG_INTERNAL,
                        0);
}

/*===========================================================================

  FUNCTION:   sns_smgr_log_md_irq

===========================================================================*/
/*!
  @brief Log an occurence of a motion detect interrupt.

  @detail Logs the last recorded timestamp, saved by SMGR at the time the
          last motion detect interrupt time ocurred.
 */
/*=========================================================================*/
void sns_smgr_log_md_irq(uint32_t md_timestamp)
{
  // Currently the threshold used for motion detect is neither configurable nor
  // retrievable. It is simply hard-coded in the driver as 48mG.
  const uint8_t md_accel_threshold = 48;

  sns_err_code_e status;
  sns_log_md_irq_s* log;

  status = sns_logpkt_malloc(SNS_LOG_DRV_MD_IRQ, sizeof(sns_log_md_irq_s), (void**)&log);
  if(status != SNS_SUCCESS)
  {
     // Can't allocate memory for this log packet.
     return;
  }

  log->version = SNS_LOG_MD_IRQ_VERSION;
  log->timestamp = md_timestamp;
  log->threshold = md_accel_threshold;
  sns_logpkt_commit(SNS_LOG_DRV_MD_IRQ, log);

}

/*===========================================================================

  FUNCTION:   sns_smgr_suspend_rpt_request_from_client_registering_hw_md_int

===========================================================================*/
/*!
  @brief Suspends all Accel reports that are coupled with MD int registration requests.

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
static void sns_smgr_suspend_rpt_request_from_client_registering_hw_md_int(void)
{
  smgr_sensor_s* sensor_ptr = sns_smgr_find_sensor(SNS_SMGR_ID_ACCEL_V01);
  smgr_rpt_item_s*  item_ptr;
  smgr_rpt_item_s** item_ptr_ptr;
  smgr_rpt_spec_s* rpt_list[SNS_SMGR_MAX_REPORT_CNT];

  if ( NULL != sensor_ptr )
  {
    uint8_t num_report = 0;
    SMGR_FOR_EACH_ASSOC_ITEM( sensor_ptr->ddf_sensor_ptr[0], item_ptr, item_ptr_ptr )
    {
      smgr_rpt_spec_s* rpt_ptr = item_ptr->parent_report_ptr;
      if ( sns_smgr_check_accel_rpt_coupled_with_md(
           rpt_ptr->rpt_id, rpt_ptr->header_abstract.src_module) != FALSE )
      {
        rpt_list[num_report++] = rpt_ptr;
      }
    }
    while ( num_report > 0 )
    {
      smgr_rpt_spec_s* rpt_ptr = rpt_list[--num_report];
      sns_smgr_deactivate_report(rpt_ptr);
    }
  }
}
/*===========================================================================

  FUNCTION:   sns_smgr_de_suspend_rpt_request_from_client_registering_hw_md_int

===========================================================================*/
/*!
  @brief Resumes all suspended reports.

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
static void sns_smgr_resume_rpt_request_from_client_registering_hw_md_int(void)
{
  smgr_rpt_spec_s *rpt_ptr;
  SMGR_FOR_EACH_Q_ITEM( &sns_smgr.report_queue, rpt_ptr, rpt_link )
  {
    if ( rpt_ptr->state == SMGR_RPT_STATE_INACTIVE )
    {
      sns_smgr_activate_report(rpt_ptr);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_de_suspend_one_rpt_request

===========================================================================*/
/*!
  @brief Searches for and unsuspends the matching report

  @detail

  @param[i]  rpt_id:  report ID of the client request
  @param[i]  src_module: source module
  @return none
 */
/*=========================================================================*/
static void sns_smgr_de_suspend_one_rpt_request(uint8_t rpt_id, uint8_t src_module)
{
  smgr_rpt_spec_s* rpt_spec_ptr;
  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.report_queue, rpt_spec_ptr, rpt_link)
  {
    if ( (rpt_spec_ptr->rpt_id == rpt_id) &&
         (rpt_spec_ptr->header_abstract.src_module == src_module) &&
         (rpt_spec_ptr->state == SMGR_RPT_STATE_INACTIVE) )
    {
      sns_smgr_activate_report(rpt_spec_ptr);
      break;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_set_hw_md_int

===========================================================================*/
/*!
  @brief set HW MD interupt

  @detail  GPIO is powered only when expecting interrupt.

  @param  enable_flag - TRUE to enable MD interrupt, FALSE disable

  @return true  succesffully enabled/disabled MD int
          false failed enabling/disabling MD int
 */
/*=========================================================================*/
static boolean sns_smgr_set_hw_md_int(boolean enable_flag)
{
  boolean success = TRUE;

  smgr_sensor_s* sensor_ptr = sns_smgr_find_sensor(SNS_SMGR_ID_ACCEL_V01);
  if ( (enable_flag != sns_smgr.md.is_md_int_enabled) && (NULL != sensor_ptr) )
  {
    uint32_t set_data = (uint32_t)enable_flag;
    sns_ddf_status_e status = sns_smgr_set_attr(sensor_ptr, SNS_DDF_SENSOR_ACCEL,
                                                SNS_DDF_ATTRIB_MOTION_DETECT,
                                                &set_data);

    SNS_SMGR_PRINTF2(HIGH, "MD int - Enable=%d status=%d", enable_flag, status);
    if ( SNS_DDF_SUCCESS == status )
    {
      sns_smgr.md.is_md_int_enabled = enable_flag;
      if (enable_flag)
      {
        /* stop sampling accel */
        sns_smgr_suspend_rpt_request_from_client_registering_hw_md_int();
      }
      else
      {
        /* resume suspended report, enable accel sampling */
        sns_smgr_resume_rpt_request_from_client_registering_hw_md_int();

        if ( sensor_ptr->ddf_sensor_ptr[0]->num_rpt_items == 0 )
        {
          set_data = (uint32_t)SNS_DDF_POWERSTATE_LOWPOWER;
          status = sns_smgr_set_attr(sensor_ptr,
                                     SNS_DDF_SENSOR__ALL,
                                     SNS_DDF_ATTRIB_POWER_STATE,
                                     &set_data);
          SNS_SMGR_PRINTF1(HIGH, "MD int - power state set result=%d", status);
        }
      }
    }
    success = (SNS_DDF_SUCCESS == status) ? TRUE : FALSE;
  }
  return success;
}


/*===========================================================================

  FUNCTION:   sns_smgr_send_hw_md_int_ind

===========================================================================*/
/*!
  @brief send HW MD interupt indication msg

  @detail

  @param  ind  indication field in the indication message needs to be sent
  @return none
 */
/*=========================================================================*/
static void sns_smgr_send_hw_md_int_ind(uint8_t ind)
{
  sns_smgr_reg_hw_md_int_ind_msg_v01 *msg_p=NULL;
  smgr_md_client_s                   *md_client_p=sns_smgr.md.md_client;

  while ( md_client_p  )
  {
   /* send indication to each client */
    msg_p = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR,sizeof(sns_smgr_reg_hw_md_int_ind_msg_v01));

    if ( msg_p== NULL )
    {
       SNS_SMGR_PRINTF0(ERROR, "MD int ind - alloc failed");
       return;
    }
    msg_p->indication = ind;
    msg_p->ReportId = md_client_p->rpt_id;

    sns_smgr_send_indication(msg_p,
                             SNS_SMGR_REG_HW_MD_INT_IND_V01,
                             sizeof(sns_smgr_reg_hw_md_int_ind_msg_v01),
                             md_client_p->connection_handle);
    md_client_p = md_client_p->md_client;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_handle_md_int

===========================================================================*/
/*!
  @brief MD int handler

  @detail Disable MD interrupt, send indication etc.

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_smgr_handle_md_int(uint32_t md_timestamp)
{
  sns_smgr_log_md_irq(md_timestamp);
  SNS_SMGR_PRINTF2(HIGH, "MD int handler - ts=%d enabled=%d",
                   md_timestamp, sns_smgr.md.is_md_int_enabled);
  /* still need to disable gpio and update suspend que */
  sns_smgr_set_hw_md_int(false);

  sns_smgr_send_hw_md_int_ind(SNS_SMGR_REG_HW_MD_INT_OCCURRED_V01);
  sns_smgr_del_md_all_clients();
  /* check if need to to power vote and turn on power rail  after reports de-suspended*/
  sns_smgr_check_rpts_for_md_update();
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_power_vote

===========================================================================*/
/*!
  @brief send power vote msg

  @detail

  @param  vote : the vote ( high power or low power ) to be sent
  @return none
 */
/*=========================================================================*/
void sns_smgr_send_power_vote(uint8_t vote)
{
#ifndef SNS_EXCLUDE_POWER
  sns_smr_header_s                         msg_header;
  sns_pm_active_pwr_st_change_req_msg_v01 *vote_msg_ptr=NULL;
  sns_pm_active_pwr_st_change_resp_msg_v01 *vote_resp_ptr;

  SNS_SMGR_PRINTF1(HIGH, "send_power_vote - vote=%d", vote);
  
  vote_msg_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR,sizeof(sns_pm_active_pwr_st_change_req_msg_v01));
  vote_resp_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, sizeof(sns_pm_active_pwr_st_change_resp_msg_v01));

  if ( (vote_msg_ptr == NULL) || (vote_resp_ptr == NULL) )
  {
     SNS_SMGR_PRINTF0(ERROR, "MD send_power_vote - alloc failed");
     return;
  }

  vote_msg_ptr->vote_value = vote;
  vote_msg_ptr->module_id = SNS_MODULE_DSPS_SMGR;

#if 0
  msg_header.dst_module = SNS_MODULE_DSPS_PM;
  msg_header.ext_clnt_id = 0;
  msg_header.src_module = SNS_MODULE_DSPS_SMGR;
  msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  msg_header.msg_type = SNS_SMR_MSG_TYPE_REQ;
  msg_header.svc_num = SNS_PM_SVC_ID_V01;
#endif
  msg_header.msg_id = SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01;
  msg_header.body_len = sizeof(sns_pm_active_pwr_st_change_req_msg_v01);
  sns_smgr_send_req(&msg_header, vote_msg_ptr, vote_resp_ptr,
                    smgr_pm_cl_user_handle);
  sns_smgr.last_power_vote = vote;

  /* if the vote is INACTIVE, there's no client for smgr. remove the latency vote */
  if(vote == SNS_PM_ST_INACTIVE_V01)
  {
    SNS_SMGR_PRINTF0(HIGH, "Remove latency vote");
    sns_hw_power_npa_vote_latency(SNS_SMGR_NPA_CLIENT_SYNC, 0);
  }
#endif /* SNS_EXCLUDE_POWER */
}

/*===========================================================================

  FUNCTION:   sns_smgr_md_init

===========================================================================*/
/*!
  @brief SMGR init QMD related variables

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_smgr_md_init(void)
{
  SNS_OS_MEMZERO( &sns_smgr.md, sizeof(sns_smgr.md) );
}

/*===========================================================================

  FUNCTION:   sns_smgr_power_init

===========================================================================*/
/*!
  @brief SMGR init power vote related variables

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_smgr_power_init(void)
{
  /*inital default by Power Manager Module is SNS_PM_ST_ACTIVE_HIGH_V01 */
  sns_smgr.last_power_vote = SNS_PM_ST_ACTIVE_HIGH_V01;
}



/*===========================================================================

  FUNCTION:   sns_smgr_add_md_client

===========================================================================*/
/*!
  @brief Appends new MD client to the MD client list.

  @param  none

  @return the newly added client pointer
 */
/*=========================================================================*/
smgr_md_client_s* sns_smgr_add_md_client(void)
{
  smgr_md_client_s* new_md_client_ptr =
    SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof (smgr_md_client_s));

  if ( !new_md_client_ptr  )
  {
     SNS_SMGR_PRINTF0(ERROR, "MD add_md_client - alloc failed");
  }
  else
  {
    smgr_md_client_s** md_client_ptr_ptr = &sns_smgr.md.md_client;
    while ( *md_client_ptr_ptr != NULL )
    {
      md_client_ptr_ptr = &(*md_client_ptr_ptr)->md_client;
    }
    *md_client_ptr_ptr = new_md_client_ptr;
    new_md_client_ptr->md_client = NULL;
  }
  return new_md_client_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_del_md_client

===========================================================================*/
/*!
  @brief Searches for and deletes MD client from MD client list

  @param [in]   rpt_id:  report ID of the client request
         [in]   src_module: source module
  @return none
 */
/*=========================================================================*/
void sns_smgr_del_md_client(uint8_t rpt_id, void *connection_handle)
{
  smgr_md_client_s** md_client_ptr_ptr = &sns_smgr.md.md_client;
  smgr_md_client_s*  md_client_ptr = NULL;

  while ( (*md_client_ptr_ptr != NULL) && (md_client_ptr == NULL) )
  {
    if ( ((*md_client_ptr_ptr)->rpt_id == rpt_id) &&
          ((*md_client_ptr_ptr)->connection_handle == connection_handle) )
    {
      md_client_ptr = *md_client_ptr_ptr;
      *md_client_ptr_ptr = md_client_ptr->md_client;
      SNS_OS_FREE(md_client_ptr);
      break;
    }
    else
    {
      md_client_ptr_ptr = &(*md_client_ptr_ptr)->md_client;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_del_md_all_clients

===========================================================================*/
/*!
  @brief Deletes all MD clients;

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void  sns_smgr_del_md_all_clients(void)
{
  smgr_md_client_s *client_list_ptr =sns_smgr.md.md_client,
                   *tmp_client_ptr;

  while ( client_list_ptr )
  {
     tmp_client_ptr = client_list_ptr->md_client;
     SNS_OS_FREE(client_list_ptr);
     client_list_ptr = tmp_client_ptr;
  }
  sns_smgr.md.md_client = NULL;
}

/*===========================================================================

  FUNCTION:   sns_smgr_check_rpts_coupled_with_md

===========================================================================*/
/*!
  @brief Check if smgr should vote for high /low power, or no vote , or check
         if power railstate needs to be updated and motion detect interrupt needs
         to be set and indication needs to be sent.

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
static void sns_smgr_check_rpts_coupled_with_md(uint8_t* power_vote_ptr)
{
  smgr_rpt_spec_s* rpt_ptr;

  if ( power_vote_ptr != NULL )
  {
    *power_vote_ptr = SNS_PM_ST_INACTIVE_V01;
  }

  sns_smgr.md.total_rpt_num = 0;
  sns_smgr.md.num_accel_rpt_not_coupled_with_md = 0;
  sns_smgr.md.num_non_accel_items = 0;

  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.report_queue, rpt_ptr, rpt_link)
  {
    uint8_t i;
    if ( rpt_ptr->state == SMGR_RPT_STATE_INACTIVE )
    {
      continue;
    }

    sns_smgr.md.total_rpt_num++;
    for ( i=0; i<rpt_ptr->num_items; i++ )
    {
      smgr_rpt_item_s* item_ptr = rpt_ptr->item_list[i];

      if ( (power_vote_ptr != NULL) &&
           (*power_vote_ptr != SNS_PM_ST_ACTIVE_HIGH_V01) )
      {
#ifdef SNS_PM_TEST
        /* Hack until RPM turns PLL8 OFF. Since PLL8 is never turned off now, there is no gain in SPS lowering the clock
        the clock to 27Mhz. It will increase the processing timeline and hence make the power numbers worse. This hack
        needs to be removed when RPM makes the change to turn PLL8 OFF when all votes for PLL8 are taken off */
        *power_vote_ptr =  SNS_PM_ST_ACTIVE_HIGH_V01;
#else
        if ( sns_q_cnt(&sns_smgr.report_queue) <= SNS_SMGR_MAX_NUM_CLIENT_TO_VOTE_LOW_POWER &&
             (item_ptr->sampling_rate_hz <= SNS_SMGR_MAX_FRQ_TO_VOTE_LOW_POWER) )
        {
          *power_vote_ptr =  SNS_PM_ST_ACTIVE_LOW_V01;
        }
        else
        {
          *power_vote_ptr =  SNS_PM_ST_ACTIVE_HIGH_V01;
        }
#endif
      }

      if ( ! SMGR_SENSOR_IS_ACCEL(item_ptr->ddf_sensor_ptr->sensor_ptr) )
      {
        sns_smgr.md.num_non_accel_items++;
      }
      else if ( sns_smgr_check_accel_rpt_coupled_with_md(
                  rpt_ptr->rpt_id,
                  rpt_ptr->header_abstract.src_module) == FALSE )
      {
        sns_smgr.md.num_accel_rpt_not_coupled_with_md++;
      }
    }
  }
  SNS_SMGR_PRINTF2(LOW, "check_rpts_coupled_w_md - accel_wo_md=%d non_accel=%d",
                   sns_smgr.md.num_accel_rpt_not_coupled_with_md,
                   sns_smgr.md.num_non_accel_items);
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_reg_hw_md_int_req_msg

===========================================================================*/
/*!
  @brief Processing HW MD interupt request

  @detail

  @param  [in]  Hdr_p,  message header info
          [in]  Msg_p,  the request message body pointer
          [out] Resp_p, response message pointert
  @return none
 */
/*=========================================================================*/
static void sns_smgr_proc_reg_hw_md_int_req_msg(sns_smr_header_s *Hdr_p,
                                   sns_smgr_reg_hw_md_int_req_msg_v01  *Msg_p,
                                   sns_smgr_reg_hw_md_int_resp_msg_v01 *Resp_p )
{
  Hdr_p->src_module = 0;
  if ( Msg_p->SrcModule_valid )
  {
    Hdr_p->src_module = Msg_p->SrcModule;
  }

  SNS_SMGR_PRINTF3(MED, "reg_hw_md_int_req - src_module=%d rpt_id=%d action=%d",
                   Msg_p->SrcModule, Msg_p->ReportId, Msg_p->Action);

  Resp_p->ReportId = Msg_p->ReportId;
  if ( Msg_p->Action== SNS_SMGR_REG_HW_MD_INT_ADD_V01 )
  {
     smgr_md_client_s *new_client_p;
     sns_smgr_del_md_client(Msg_p->ReportId, Hdr_p->connection_handle);
     new_client_p = sns_smgr_add_md_client();
     if (new_client_p )
     {
        new_client_p->rpt_id = Msg_p->ReportId;
        new_client_p->connection_handle = Hdr_p->connection_handle;
        new_client_p->src_module = Hdr_p->src_module;
        new_client_p->ext_clnt_id = Hdr_p->ext_clnt_id;
        sns_smgr_check_rpts_coupled_with_md(NULL);
        if ( sns_smgr.md.num_accel_rpt_not_coupled_with_md == 0 )
        {
          sns_smgr_set_hw_md_int(true);
        }
     }
     else
     {
        Resp_p->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
        Resp_p->Resp.sns_err_t = SENSOR1_ENOMEM;
     }
  }
  else if ( Msg_p->Action== SNS_SMGR_REG_HW_MD_INT_DEL_V01  )
  {
     sns_smgr_del_md_client(Msg_p->ReportId, Hdr_p->connection_handle);
     sns_smgr_de_suspend_one_rpt_request(Msg_p->ReportId, Hdr_p->src_module);
     if ( sns_smgr.md.md_client == NULL )
     {
       sns_smgr_set_hw_md_int(false);
     }
  }

  if ( sns_smgr.md.is_md_int_enabled == FALSE )
  {
    Resp_p->result = SNS_SMGR_REG_HW_MD_INT_DISABLED_V01;
  }
  else
  {
    Resp_p->result = SNS_SMGR_REG_HW_MD_INT_ENABLED_V01;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_check_accel_rpt_coupled_with_md

===========================================================================*/
/*!
  @brief check if the report is coupled with one of the motion detection request

  @detail

  @param [in] rpt_id, report ID
         [in] src_module, source module
  @return true/false
 */
/*=========================================================================*/
bool sns_smgr_check_accel_rpt_coupled_with_md(uint8_t rpt_id, uint8_t src_module)
{
  smgr_md_client_s*  tmp_client_ptr = sns_smgr.md.md_client;
  while ( tmp_client_ptr )
  {
     if ( (tmp_client_ptr->rpt_id == rpt_id) &&
          (tmp_client_ptr->src_module == src_module)  )
     {
        return true;
     }
     tmp_client_ptr = tmp_client_ptr->md_client;
  }
  return false;
}

/*===========================================================================

  FUNCTION:   sns_smgr_check_rpts_for_md_update

===========================================================================*/
/*!
  @brief Check if smgr should vote for high /low power, or no vote , or check
         if power railstate needs to be updated and motion detect interrupt needs
         to be set and indication needs to be sent.

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_smgr_check_rpts_for_md_update(void)
{
  uint8_t new_vote;
  sns_smgr_check_rpts_coupled_with_md(&new_vote);
#ifndef SNS_EXCLUDE_POWER
 {
  smgr_power_state_type_e power_rail;


  SNS_SMGR_PRINTF3(
     MED, "check_rpts_4_md_update - #rpts=%d md=%d testing=%d",
     sns_smgr.md.total_rpt_num, (sns_smgr.md.md_client == NULL) ? 0 : 1,
     !sns_smgr.sensor_test_info.test_done);

  if ( (sns_smgr.md.num_non_accel_items != 0) || !sns_smgr.sensor_test_info.test_done )
  {
    power_rail = SNS_SMGR_POWER_HIGH;
  }
  else if ( (sns_smgr.md.total_rpt_num != 0) || (sns_smgr.md.md_client != NULL) )
  {
    power_rail = SNS_SMGR_POWER_LOW;
  }
  else
  {
    power_rail = SNS_SMGR_POWER_OFF;
  }
  SNS_SMGR_PRINTF1(MED, "check_rpts_4_md_update - power_rail=%d", power_rail);
  sns_hw_power_rail_config(power_rail);
 }
 #endif

  if ( sns_smgr.last_power_vote != new_vote )
  {
    sns_smgr_send_power_vote(new_vote);
  }
  if (sns_smgr.md.md_client != NULL )
  {
    if ( sns_smgr.md.num_accel_rpt_not_coupled_with_md == 0 )
    {
      if ( !sns_smgr.md.is_md_int_enabled )
      {
        if( FALSE != sns_smgr_set_hw_md_int(TRUE) )
        {
          sns_smgr_send_hw_md_int_ind(SNS_SMGR_REG_HW_MD_INT_ENABLED_V01);
        }
      }
    }
    else
    {
      if (sns_smgr.md.is_md_int_enabled)
      {
        if( FALSE != sns_smgr_set_hw_md_int(FALSE))
        {
          sns_smgr_send_hw_md_int_ind(SNS_SMGR_REG_HW_MD_INT_DISABLED_V01);
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_cancel_internal_service

===========================================================================*/
/*!
  @brief This function cancels all requests to the service from this client

  @detail

  @param[in]  src_module client source module
        [in]  client_id client ID
  @return none
 */
/*=========================================================================*/
void sns_smgr_cancel_internal_service(void *connection_handle)
{
  smgr_md_client_s** md_client_ptr_ptr = &sns_smgr.md.md_client;
  smgr_md_client_s*  md_client_ptr = NULL;

  while ( (*md_client_ptr_ptr != NULL) && (md_client_ptr == NULL) )
  {
    if ((*md_client_ptr_ptr)->connection_handle == connection_handle)
    {
      md_client_ptr = *md_client_ptr_ptr;
      *md_client_ptr_ptr = md_client_ptr->md_client;
      SNS_OS_FREE(md_client_ptr);
      return;
    }
    else
    {
      md_client_ptr_ptr = &(*md_client_ptr_ptr)->md_client;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_internal_process_md

===========================================================================*/
/*!
  @brief This function processes incoming motion detect messages.

  @param[in] msg_header: Message header
  @param[in] item_ptr: Message items pointer
  @param[in] body_ptr: Message body pointer

  @return   Response message ID
 */
/*=========================================================================*/
uint16_t sns_smgr_internal_process_md(
  sns_smr_header_s msg_header,
  void *item_ptr,
  void *body_ptr)
{
  uint16_t         resp_msg_id = 0xFFFF;
  SNS_SMGR_PRINTF2(MED, "internal_msg - conn=0x%x msg_id=%d",
                   msg_header.connection_handle, msg_header.msg_id);

  sns_smr_header_s                      resp_msg_header;
  sns_smgr_reg_hw_md_int_req_msg_v01*   req_ptr =
    (sns_smgr_reg_hw_md_int_req_msg_v01*) body_ptr;
  sns_smgr_reg_hw_md_int_resp_msg_v01*  resp_ptr =
    SMGR_MSG_ALLOC(sns_smgr_reg_hw_md_int_resp_msg_v01);
  if (resp_ptr)
  {
     resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
     resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;
     sns_smgr_proc_reg_hw_md_int_req_msg(&msg_header, req_ptr, resp_ptr);
     resp_msg_header.msg_id = SNS_SMGR_REG_HW_MD_INT_RESP_V01;
     resp_msg_header.body_len = sizeof(sns_smgr_reg_hw_md_int_resp_msg_v01);
     sns_smgr_send_resp(&resp_msg_header, resp_ptr, msg_header);
     sns_smgr_check_rpts_for_md_update();
  }
  else
  {
     resp_msg_id = SNS_SMGR_REG_HW_MD_INT_RESP_V01;
  }

  return resp_msg_id;
}
