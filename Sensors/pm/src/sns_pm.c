/*============================================================================

  @file sns_pm.c

  @brief
  This file contains the implementation of the Sensors Power Manager

  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/pm/src/sns_pm.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
2013-11-12   dc      Enable SPaRe on 8084
2013-10-03   pk      Add hysteresis back to AP state change notification
2013-09-26   ps      Add BRINGUP_8084 flag to disable SPARE on apq8084
2013-08-09   ps      Eliminate compiler warnings when SNS_EXCLUDE_POWER is defined
2013-07-11   pk      Remove handling of temporary processor monitor API
2013-07-09   pk      Remove hysteresis in AP state change notification
2013-06-06   pn      Informs SMGR on AP state change
2013-06-05   dc      Increase resume hysteresis to reliably handle suspend scenarios for 
                     200Hz clients.
2013-05-20   br      Removed lines for PNOC control
2013-05-07   sc      Fix klockwork errors; check for NULL points
2013-05-07   dc      Add resume time hysteresis to prevent rapid suspend/resume due to 
                     high frequency data.
2013-05-06   hw      Change MIPs and bus bandwidth voting for ACTIVE HIGH state
2013-04-11   dc      Add back SMSM dependency to enable SPaRe
2013-04-09   hw      Set PERIODIC CLIENT flag in Sensors registration with ADSPPM 
2013-03-26   hw      Add bw voting for turning on PNOC clocks from sensors PM
2013-03-25   dc      Remove SMSM dependency to fix mutual depenedency problem.
2013-03-20   hw
             hb      Change MIPs and bandwidth voting for ACTIVE HIGH, ACTIVE LOW, 
                     and INACTIVE respectively
2013-03-21   gju     Use ISO standard bool
2013-02-19   jhh     Replace ulog with QXDM message
2013-02-07   dc      Register for SMSM notifications to handle filtering
                     of indications during kernel suspend.
2013-01-29   gju     Include different file for SMR utlity functions.
2013-01-03   jhh     Remove pmic control
2012-12-20   jhh     Remove time delay between two smdl write and add three different
                     memory segments for ocmem
2012-12-04   jhh     Add flushing the memory written to smdl
2012-12-10   ag      Initialize ext_signal to NULL, remove macro OLD_QCCI
2012-11-02   vh      Change the task create to sns_os_task_create_ext for SNS_DSPS_BUILD
2012-10-31   jhh     Add INACTIVE power state to have MIPs and BW off for Audio only usecase
2012-10-24   vh      Processing the messages in task loop
2012-09-13   vh      Eliminated compiler warnings
2012-08-29   ag      Updated temporary workaround to distinguish between PM clients
2012-08-08   jhh     Merge ADSPPM feature into QMI code base and Re-enable ULOG
2012-08-01   sc      Moved struct definition client_info_type to common header
2012-07-31   ag      Fix indications, clean up
2012-07-02   ps      Added QCSI support
2011-11-14   jhh     Updated alloc and free function calls to meet new API
2011-06-09   sj      Vote for DSPS high power at init time until USB is able
                     able to vote with RPM (this change is a temp fix)
06-02-2011   sj      Improve ULOG readability
05-20-2011   sj      Add more ULOG messages for target bringup
05-13-2011   sj      Fix signal handling, added checks for ULOG return values
05-13-2011   dc      Fixed compilation errors from sns_pm_init.
05-06-2011   agk     Moved npa_init() outside SNS_EXCLUDE_POWER
05-04-2011   sj      Featurize power related calls temporarily for 8960 bringup
04-29-2011   jh      Removed compiler warning
04-29-2011   jh      Added call to mpmint_init()
04-27-2011   sj      Compilation fix
04-22-2011   sj      Update processor frequency for busywait
04-20-2011   sj      Optimizations, include sleep.h for proper compilation,
                     fixed default power state.
04-19-2011   sj      Adding more debug messages,
                     featurizing correctly for PC-SIM compilation
04-18-2011   sj      Consolidating all power related inits &
                     Fixed compilation problems after RPM driver integration
04-18-2011   sj      Fill in RPM API for sending power state transition
                     messages to RPM.
04-15-2011   sj      Support for power log packet
04-15-2011   sj      Fix for compilation warnings
04-13-2011   sj      Added support for logging through ULOG
03-30-2011   sj      First version of PM

============================================================================*/

/*=====================================================================
                               INCLUDES
=======================================================================*/
#include <stdbool.h>
#include "sns_memmgr.h"
#include "sns_common_v01.h"
#include "sns_init.h"
#include "sns_common.h"
#include "sns_smr_util.h"
#include "sns_pm_priv.h"
#include "sns_em.h"
#include "sns_debug_str.h"
#if !defined SNS_PCSIM && !defined QDSP6
#include "npa_init.h"  /* For NPA init */
#include "mpmint.h"
#include "rpm_messaging.h"
#include "sleep.h" /* For Sleep init */
#include "busywait.h" /* For configuring busywait with CPU frequency */
#include "DALStdErr.h" /* For DAL_SUCCESS */
#endif /* SNS_PCSIM */
#ifdef DSPS_RPM_SUPPORT
#include "sns_log_types.h" /* For power log packet defintion */
#include "sns_log_api.h"
#include "sns_em.h" /* For getting timestamp for power log packet */
#endif /* DSPS_RPM_SUPPORT */

#ifndef SNS_PCSIM
#ifdef FEATURE_SMSM
#include "smsm.h"
#else
#warning SMSM is required, do not expect correct behavior
#endif /* FEATURE_SMSM */
#endif /* #ifndef SNS_PCSIM */

#include "pm.h"
#include "npa.h"

#ifdef SNS_QMI_ENABLE
#include "qmi_csi.h"
#include "qmi_csi_target_ext.h"
#endif

#include "mmpm.h"
#include "sns_smgr_internal_api_v01.h"

#ifdef OCMEM_ENABLE
#include "qmi_csi_common.h"
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "sns_smgr_api_v01.h"
#include "sns_queue.h"
#include "sns_ocmem_v01.h"
#include "smd_lite.h"
#include "sns_ocmem_v01.h"

// TODO: for testing only, remove once fix crash
#define BUF_SIZE 65536 // 4*65536=256K, actual allocation should be 384kb, this is just to test if ocmem allocation is working fine.
static uint32_t sns_ocmem_test_buffer[BUF_SIZE] __attribute__ ((aligned (4096)));
uint64_t error_word_msg_for_ocmem_test= (uint64_t) 0xFFFFFFFFFFFFFFFFULL;

static uint32_t sns_code_start    __attribute__((__section__(".txt.sns_code_start")));
static uint32_t sns_code_end      __attribute__((__section__(".txt.sns_code_end")));
static uint32_t sns_data_start    __attribute__((__section__(".bss.sns_data_start")));
static uint32_t sns_data_end      __attribute__((__section__(".bss.sns_data_end")));

#endif
/*=======================================================================
                  INTERNAL DEFINITIONS AND TYPES
========================================================================*/

/*========================================================================
                        GLOBAL VARIABLES
========================================================================*/

/* Signal for PM when any message is received in its SMR message queue*/
static OS_FLAG_GRP *sns_pm_sig_grp;

/* Stores the Sensors Active Power State */
static uint8_t sns_pm_curr_active_st_g;

/* Stores active state vote values of SAM and SMGR */
static uint8_t pm_vote_array_g[SNS_PM_NUM_VOTING_CLIENTS];

#ifndef SNS_EXCLUDE_POWER
/* Clients of power manager who vote for active power state
 * These clients will be sent indication on DSPS active state change
 */
static uint8_t sns_pm_voting_clients_g[SNS_PM_NUM_VOTING_CLIENTS];

/* Processor frequency in active states used for configuring busywait */
static uint32_t sns_pm_active_freq_g[SNS_PM_NUM_ACTIVE_STATES];
#endif /* SNS_EXCLUDE_POWER */

bool linux_kernel_suspend = FALSE;

#define  RESUME_HYSTERESIS_TICKS   (uint32_t)(200000/30.51)  /* 200 mill sec resume delay */
volatile bool resume_from_suspend = false;


//ADSPPM Client ID
uint32 adsppm_sns_id = 0;
uint32 adsppm_sns_sram_id = 0;

volatile int wait_for_break = 1;

#ifdef OCMEM_ENABLE
static qmi_client_os_params   pm_smgr_cl_os_params;
static qmi_client_type        pm_smgr_cl_user_handle;
uint8_t enable_client_ind;

/* Definition of event index which is used for converting to a signal number */
typedef enum
{
  /** Called when the remote processor opens the port.
   * See smdl_open() */
  PM_SMDL_EVENT_OPEN = 0,
  /** Called when data is available to be read from the FIFO.
   * See smdl_read() */
  PM_SMDL_EVENT_READ,
  /** Called when space is available in the FIFO after a failed write.
   * See smdl_write() */
  PM_SMDL_EVENT_WRITE,
  /** Called when the remote processor closes the port.
   * See smdl_close() */
  PM_SMDL_EVENT_CLOSE,
  PM_SMDL_EVENT_LAST
} pm_smdl_event_idx_e;

typedef struct pm_smdl_port_s
{
  smd_channel_type type;                /* SMD_APPS_DSPS or SMD_MDM_DSPS */
  smdl_handle_type hndl;
  OS_FLAGS         sig_flag[PM_SMDL_EVENT_LAST];
} pm_smdl_port_s;

smdl_handle_type smdl_hndl_qdsp;
pm_smdl_port_s smdl_port_qdsp;
#endif

static sns_q_s                pm_queue;
static OS_EVENT               *pm_que_mutex_ptr;
static OS_EVENT               *pm_suspend_resume_mutex_ptr;

static sns_em_timer_obj_t     resume_tmr_obj;

typedef struct pm_q_item_s
{
  sns_q_link_s                  q_link;
  qmi_idl_type_of_message_type  msg_type;
  unsigned int                  msg_id;
  unsigned int                  msg_len;
  qmi_req_handle                req_handle;
  void                          *connection_handle;
  void                          *body_ptr;
} pm_q_item_s;

static OS_STK sns_pm_stack[4096];

#ifdef SNS_QMI_ENABLE
static qmi_csi_service_handle sns_pm_qcsi_service_handler_s;
static qmi_csi_os_params      sns_pm_qcsi_os_params;
#endif

static OS_FLAGS               sns_pm_sig_mask;

uint32 cb_adsppm_core_adsp(MmpmCallbackParamType *pCbParam);
uint32 cb_adsppm_core_sram(MmpmCallbackParamType *pCbParam);

#ifdef OCMEM_ENABLE
void adsp_smdl_cb (smdl_handle_type hndl, smdl_event_type event, const pm_smdl_port_s *smdl_port_ptr);

#endif

extern void sns_smgr_app_state_change(bool new_suspend_state);
extern void sns_sam_ap_state_change_cb( bool suspendState );

/*===========================================================================
                               FUNCTIONS
===========================================================================*/

#ifdef SNS_QMI_ENABLE
#ifdef OCMEM_ENABLE
/*=============================================================================
  CALLBACK FUNCTION sns_pm_client_ind_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an indication for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to
                 identify different services.

@param[in]   msg_id              Message ID of the indication

@param[in]  ind_buf              Buffer holding the encoded indication

@param[in]  ind_buf_len          Length of the encoded indication

@param[in]  ind_cb_data          Cookie value supplied by the client during registration

*/
/*=========================================================================*/
static void sns_pm_client_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *ind_buf,
  unsigned int                   ind_buf_len,
  void                           *ind_cb_data
)
{
  uint8_t      os_err;
  pm_q_item_s* msg_ptr;

  if (user_handle != pm_smgr_cl_user_handle)
  {
    return;
  }

  msg_ptr = (pm_q_item_s*)SNS_OS_MALLOC(0, sizeof(pm_q_item_s));

  msg_ptr->body_ptr = SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, ind_buf_len);

  SNS_OS_MEMCOPY(msg_ptr->body_ptr, ind_buf, ind_buf_len);
  msg_ptr->msg_len = ind_buf_len;
  msg_ptr->msg_id = msg_id;
  msg_ptr->msg_type = QMI_IDL_INDICATION;

  sns_q_link(msg_ptr, &msg_ptr->q_link);

  sns_os_mutex_pend(pm_que_mutex_ptr, 0, &os_err);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  sns_q_put(&pm_queue, &msg_ptr->q_link);

  os_err = sns_os_mutex_post(pm_que_mutex_ptr);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  sns_os_sigs_post ( sns_pm_sig_grp, SNS_PM_MSG_SIG, OS_FLAG_SET, &os_err );
}

/*=========================================================================
  CALLBACK FUNCTION:  sns_pm_client_resp_cb
  =========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an asynchronous response for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to
                 identify different services.

@param[in]   msg_id              Message ID of the response

@param[in]   buf                 Buffer holding the decoded response

@param[in]   len                 Length of the decoded response

@param[in]   resp_cb_data        Cookie value supplied by the client

@param[in]   transp_err          Error value

*/
/*=======================================================================*/
void sns_pm_client_resp_cb
(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void                    *resp_c_struct,
  unsigned int            resp_c_struct_len,
  void                    *resp_cb_data,
  qmi_client_error_type   transp_err
)
{
  uint8_t      os_err;
  pm_q_item_s  *msg_ptr;

  if ((resp_c_struct == NULL) || (transp_err !=  QMI_NO_ERR))
  {
    return;
  }

  if (user_handle != pm_smgr_cl_user_handle)
  {
    SNS_OS_FREE(resp_c_struct);
    return;
  }

  msg_ptr = (pm_q_item_s*)SNS_OS_MALLOC(0, sizeof(pm_q_item_s));

  msg_ptr->body_ptr = resp_c_struct;
  msg_ptr->msg_len = resp_c_struct_len;
  msg_ptr->msg_id = msg_id;
  msg_ptr->msg_type = QMI_IDL_RESPONSE;

  sns_q_link(msg_ptr, &msg_ptr->q_link);

  sns_os_mutex_pend(pm_que_mutex_ptr, 0, &os_err);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  sns_q_put(&pm_queue, &msg_ptr->q_link);

  os_err = sns_os_mutex_post(pm_que_mutex_ptr);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  sns_os_sigs_post ( sns_pm_sig_grp, SNS_PM_MSG_SIG, OS_FLAG_SET, &os_err );
}
#endif /* OCMEM_ENABLE */

/*===========================================================================

  FUNCTION:   sns_pm_chk_active_state_transition

===========================================================================*/
/*!
  @brief
  Check for active state transition of DSPS

  @param[i]
  Connection_handle to determine the client

  @detail
  This function is called after all the active state votes from the sensor
  modules are processed.
  - Checks if the DSPS active state needs to change based on the received
    votes.
  - Triggers state transition vote with RPM Driver.
  - Sends power state indication to clients (SAM, SMGR) upon completion
    of the state transition.

  @return
  No return code

*/
/*=========================================================================*/
static void sns_pm_chk_active_state_transition
(
  void *qcci_handle_ptr
)
{
  MMPM_STATUS req_result;
#ifdef DSPS_RPM_SUPPORT
  sns_log_dsps_pwr_s                      *log_struct_ptr;
  sns_err_code_e                           ret_err_code;
#endif /* DSPS_RPM_SUPPORT */
#if !defined SNS_PCSIM && !defined QDSP6
  DALResult                               send_msg_err_code;
  RPM_Message                             active_st_msg;
#endif /* SNS_PCSIM */

  uint8_t total_vote;
  total_vote = pm_vote_array_g[SNS_PM_SRC_MODULE_SAM] | pm_vote_array_g[SNS_PM_SRC_MODULE_SMGR];

  if((total_vote & SNS_PM_ACTIVE_HIGH) && (total_vote & SNS_PM_ACTIVE_LOW))
  {
    total_vote = SNS_PM_ACTIVE_HIGH;
  }

  if (sns_pm_curr_active_st_g != total_vote)
  {
#if !defined SNS_PCSIM
    /* Set Busy wait frequency before sending vote to RPM
     * When you need to call this API depends on whether you are increasing or
     * decreasing the processor frequency:
     * 1.If you are increasing the processor frequency, then you need to update busywait before the switch.
     * 2.If you are decreasing the processor frequency, then you need to update busywait after the switch.
     * The reason for this is to make sure that if a busywait call were to come in during
     * the period of time between when the frequency change takes place and when
     * busywait is updated, we will err on the side of waiting too long.
     * We never want to wait too short. When are "going" to transition to high
     * power state set the CPU frequency before making the transition
     */

    // Vote for appropriate power state to ADSPPM
    sns_pm_curr_active_st_g = total_vote;

    if(sns_pm_curr_active_st_g == SNS_PM_ACTIVE_HIGH)
    {
      PM_MSG_0(DBG_MED_PRIO, "PM : ACTIVE_HIGH");

      MmpmRscExtParamType req_rsc_param;
      MMPM_STATUS result_status[2];
      MmpmRscParamType req_param[2];
      MmpmGenBwValType bw_value[1];
      MmpmBwReqType bw_param;
      MmpmMipsReqType mips_param;

      bw_value[0].bwValue.busBwValue.bwBytePerSec =  6000000; // Bus bandwidth requirements based on profiling for max concurrency with margin of 20%
      bw_value[0].bwValue.busBwValue.usagePercentage = 100;
      bw_value[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
      bw_value[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
      bw_value[0].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
      bw_param.numOfBw = 1;
      bw_param.pBandWidthArray = (MmpmBwValType *)bw_value;

      req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
      req_param[0].rscParam.pBwReq = &bw_param;

      mips_param.mipsTotal = 30; //For the highest MIPs usecase, max concurrency, max sampling rate
      mips_param.mipsPerThread = 21; // Based on thread loading seen in profiling
      mips_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
      mips_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

      req_param[1].rscId = MMPM_RSC_ID_MIPS_EXT;
      req_param[1].rscParam.pMipsExt = &mips_param;

      req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
      req_rsc_param.numOfReq = 2;
      req_rsc_param.reqTag = 0;
      req_rsc_param.pStsArray = result_status;
      req_rsc_param.pReqArray = req_param;
      PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP PM Vote: BW(%d) MIPS(%d)", bw_value[0].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);

      req_result = MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param);
      if(MMPM_STATUS_SUCCESS != req_result)
      {
        PM_MSG_1(DBG_ERROR_PRIO, "PM : MMPM MIPS high active request failed due to %d\n", req_result);
      }
    }
    else if(sns_pm_curr_active_st_g == SNS_PM_ACTIVE_LOW )
    {
      PM_MSG_0(DBG_MED_PRIO, "PM : ACTIVE_LOW");

      MmpmRscExtParamType req_rsc_param;
      MMPM_STATUS result_status[2];
      MmpmRscParamType req_param[2];
      MmpmGenBwValType bw_value[1];
      MmpmBwReqType bw_param;
      MmpmMipsReqType mips_param;

      bw_value[0].bwValue.busBwValue.bwBytePerSec = 1728381; //Based on profiling for up to 66 Hz usecase
      bw_value[0].bwValue.busBwValue.usagePercentage = 100;
      bw_value[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
      bw_value[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
      bw_value[0].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
      bw_param.numOfBw = 1;
      bw_param.pBandWidthArray = (MmpmBwValType *)bw_value;

      req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
      req_param[0].rscParam.pBwReq = &bw_param;

      mips_param.mipsTotal = 13; //Based on profiling for up to 66 Hz usecase
      mips_param.mipsPerThread = 10;
      mips_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
      mips_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

      req_param[1].rscId = MMPM_RSC_ID_MIPS_EXT;
      req_param[1].rscParam.pMipsExt = &mips_param;

      req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
      req_rsc_param.numOfReq = 2;
      req_rsc_param.reqTag = 0;
      req_rsc_param.pStsArray = result_status;
      req_rsc_param.pReqArray = req_param;
      PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP PM Vote: BW(%d) MIPS(%d)", bw_value[0].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);

      req_result = MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param);
      if(MMPM_STATUS_SUCCESS != req_result)
      {
        PM_MSG_1(DBG_ERROR_PRIO, "PM : MMPM MIPS low active request failed due to %d\n", req_result);
      }
    }
    else if(sns_pm_curr_active_st_g == SNS_PM_INACTIVE)
    {
      MmpmRscExtParamType req_rsc_param;
      MMPM_STATUS result_status[2];
      MmpmRscParamType req_param[2];
      MmpmGenBwValType bw_value[1];
      MmpmBwReqType bw_param;
      MmpmMipsReqType mips_param;

      PM_MSG_0(DBG_MED_PRIO, "PM : INACTIVE");
      bw_value[0].bwValue.busBwValue.bwBytePerSec = 0; // minimum request for processing commands
      bw_value[0].bwValue.busBwValue.usagePercentage = 50;
      bw_value[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
      bw_value[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
      bw_value[0].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
      bw_param.numOfBw = 1;
      bw_param.pBandWidthArray = (MmpmBwValType *)bw_value;

      req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
      req_param[0].rscParam.pBwReq = &bw_param;

      mips_param.mipsTotal = 0;     // Because sensors' processing is stopping, no MIPs vote is required
      mips_param.mipsPerThread = 0; // ADSP PM has in-built mechanism for allocation of enough bandwidth to process commands
      mips_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
      mips_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

      req_param[1].rscId = MMPM_RSC_ID_MIPS_EXT;
      req_param[1].rscParam.pMipsExt = &mips_param;

      req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
      req_rsc_param.numOfReq = 2;
      req_rsc_param.reqTag = 0;
      req_rsc_param.pStsArray = result_status;
      req_rsc_param.pReqArray = req_param;
      PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP PM Vote: BW(%d) MIPS(%d)", bw_value[0].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);

      req_result = MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param);
      if(MMPM_STATUS_SUCCESS != req_result)
      {
        PM_MSG_1(DBG_ERROR_PRIO, "PM : MMPM MIPS inactive request failed due to %d\n", req_result);
      }

    }

#endif /* SNS_PCSIM */
  }
  else
  {
    /* No transition required */
    PM_MSG_0(DBG_MED_PRIO, "PM : No Active State Transition required");
    return;
  }

#if !defined SNS_PCSIM
  /* NOTE: This code will be executed for both high power and low power
   * transitions. This API will be called twice for a high power state transition
   * one time before the transition and second time is below call.
   * Calling the API twice does not matter. Wanted to avoid additional "if then"
   * checks in code
   */
//  busywait_config_cpu_freq(sns_pm_active_freq_g[sns_pm_curr_active_st_g]);
#endif

  //moved indication for loop into the cb sig

#ifdef DSPS_RPM_SUPPORT
  /* Send log packet to apps processor indicating current active state
   * This code should be commented out during power measurement testing
   */

  //Allocate log packet
  ret_err_code = sns_logpkt_malloc(SNS_LOG_DSPS_PWR,
                                   sizeof(sns_log_dsps_pwr_s),
                                   (void**)&log_struct_ptr);

  if ((ret_err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version    = SNS_LOG_DSPS_PWR_VERSION;
    log_struct_ptr->timestamp  = sns_em_get_timestamp();
    if (sns_pm_curr_active_st_g == SNS_PM_ACTIVE_HIGH)
    {
      log_struct_ptr->powerstate = SNS_LOG_DSPS_PWR_ST_HIGH;
    }
    else
    {
      log_struct_ptr->powerstate = SNS_LOG_DSPS_PWR_ST_LOW;
    }

    //Commit log (also frees up the log packet memory)
    sns_logpkt_commit(SNS_LOG_DSPS_PWR, log_struct_ptr);
  }
#endif  /* DSPS_RPM_SUPPORT */

} // end of sns_pm_chk_active_state_transition


/*=========================================================================
  CALLBACK FUNCTION:  sns_pm_connect_cb
  =========================================================================*/
/*!
  @brief Callback registered with QCSI to receive connect requests
*/
/*=======================================================================*/
static qmi_csi_cb_error sns_pm_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
)
{
  client_info_type *pm_client_info;

 /* Assign client_handle pointer to connection_handle so that it can be
    used in the handle_req_cb to send indications if necessary */
  if(!connection_handle)
    return QMI_CSI_CB_CONN_REFUSED;

  pm_client_info = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_PWR,sizeof(client_info_type)); /* Freed in disconnect_cb */
  if(!pm_client_info)
    return QMI_CSI_CB_CONN_REFUSED;
  pm_client_info->client_handle = client_handle;

  *connection_handle = (void*)pm_client_info;

  return QMI_CSI_NO_ERR;
}


/*=========================================================================
  CALLBACK FUNCTION:  sns_pm_disconnect_cb
  =========================================================================*/
/*!
  @brief Callback registered with QCSI to receive disconnect requests
*/
/*=======================================================================*/
static void sns_pm_disconnect_cb
(
  void                      *connection_handle,
  void                      *service_cookie
)
{
  /* Free up memory for the client */
  if(connection_handle)
    SNS_OS_FREE(connection_handle); /* Malloc in connect_cb */

  return;
}


/*=========================================================================
  CALLBACK FUNCTION:  sns_pm_process_req_cb
  =========================================================================*/
/*!
  @brief Callback registered with QCSI to receive service requests
*/
/*=======================================================================*/
static qmi_csi_cb_error sns_pm_process_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
)
{
  pm_q_item_s         *msg_ptr;
  uint8_t             os_err;

  msg_ptr = (pm_q_item_s*)SNS_OS_MALLOC(0, sizeof(pm_q_item_s));
  if (msg_ptr == NULL)
  {
    return QMI_CSI_NO_MEM;
  }

  msg_ptr->body_ptr = SNS_OS_MALLOC (SNS_DBG_NUM_MOD_IDS, req_c_struct_len);
  if (msg_ptr->body_ptr == NULL)
  {
    SNS_OS_FREE(msg_ptr);
    return QMI_CSI_NO_MEM;
  }

  SNS_OS_MEMCOPY(msg_ptr->body_ptr, req_c_struct, req_c_struct_len);
  msg_ptr->msg_len = req_c_struct_len;
  msg_ptr->msg_id = msg_id;
  msg_ptr->msg_type = QMI_IDL_REQUEST;
  msg_ptr->connection_handle = connection_handle;
  msg_ptr->req_handle = req_handle;

  sns_q_link(msg_ptr, &msg_ptr->q_link);

  sns_os_mutex_pend(pm_que_mutex_ptr, 0, &os_err);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  sns_q_put(&pm_queue, &msg_ptr->q_link);

  os_err = sns_os_mutex_post(pm_que_mutex_ptr);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  sns_os_sigs_post ( sns_pm_sig_grp, SNS_PM_MSG_SIG, OS_FLAG_SET, &os_err );

  return QMI_CSI_CB_NO_ERR;
}

#else

/*===========================================================================

  FUNCTION:   sns_pm_send_msg

===========================================================================*/
/*!
  @brief
  Send message to other sensor modules using SMR API

  @param[i] hdr_ptr: Pointer to SMR Header
  @param[i] msg_ptr: Pointer to Message


  @detail
  This function is called to send a message to other sensor modules.
  - Set Header for message
  - Send Message, in case of error free the message

  @return
  No return code

*/
/*=========================================================================*/
static void sns_pm_send_msg(sns_smr_header_s* hdr_ptr,
                                void* msg_ptr)
{
  sns_err_code_e     ret_err_code;

  sns_smr_set_hdr(hdr_ptr, msg_ptr);
  ret_err_code = sns_smr_send(msg_ptr);
  if (ret_err_code != SNS_SUCCESS)
  {
    // Free the memory
    sns_smr_msg_free(msg_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_pm_chk_active_state_transition

===========================================================================*/
/*!
  @brief
  Check for active state transition of DSPS

  @param[i]
  No input parameters

  @detail
  This function is called after all the active state votes from the sensor
  modules are processed.
  - Checks if the DSPS active state needs to change based on the received
    votes.
  - Triggers state transition vote with RPM Driver.
  - Sends power state indication to clients (SAM, SMGR) upon completion
    of the state transition.

  @return
  No return code

*/
/*=========================================================================*/
static void sns_pm_chk_active_state_transition(void)
{

  //sns_pm_active_pwr_st_change_ind_msg_v01 *ind_msg_ptr;
  //sns_smr_header_s                        ind_msg_hdr;
  //uint8_t                                  loop_var;
#ifdef DSPS_RPM_SUPPORT
  sns_log_dsps_pwr_s                      *log_struct_ptr;
  sns_err_code_e                           ret_err_code;
#endif /* DSPS_RPM_SUPPORT */

#if !defined SNS_PCSIM && !defined QDSP6
  DALResult                               send_msg_err_code;
  RPM_Message                             active_st_msg;
#endif /* SNS_PCSIM */

  if (sns_pm_curr_active_st_g != (pm_vote_array_g[0] | pm_vote_array_g[1]))
  {
#if !defined SNS_PCSIM
    /* Set Busy wait frequency before sending vote to RPM
     * When you need to call this API depends on whether you are increasing or
     * decreasing the processor frequency:
     * 1.If you are increasing the processor frequency, then you need to update busywait before the switch.
     * 2.If you are decreasing the processor frequency, then you need to update busywait after the switch.
     * The reason for this is to make sure that if a busywait call were to come in during
     * the period of time between when the frequency change takes place and when
     * busywait is updated, we will err on the side of waiting too long.
     * We never want to wait too short. When are "going" to transition to high
     * power state set the CPU frequency before making the transition
     */
    if (!(sns_pm_curr_active_st_g) == SNS_PM_ACTIVE_HIGH)
    {
//      busywait_config_cpu_freq(sns_pm_active_freq_g[SNS_PM_ACTIVE_HIGH]);
    }

    // Vote for appropriate power state to ADSPPM
    if(sns_pm_curr_active_st_g == SNS_PM_ACTIVE_HIGH)
    {
      MmpmRscExtParamType req_rsc_param;
      MMPM_STATUS result_status[1];
      MmpmRscParamType req_param[1];
      //MmpmGenBwValType bw_param[1];

      //bw_param[0].bwValue.busBwValue.bwBytePerSec = 10; // TBD
      //bw_param[0].bwValue.busBwValue.usagePercentage = 100; // TBD
      //bw_param[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
      //bw_param[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
      //bw_param[0].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;

      //req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
      //req_param[0].rscParam.pBwReq->numOfBw = 1;
      //req_param[0].rscParam.pBwReq->pBandWidthArray = bw_param;


      req_param[0].rscId = MMPM_RSC_ID_MIPS_EXT;
      req_param[0].rscParam.pMipsExt->mipsTotal = 30; // TBD
      req_param[0].rscParam.pMipsExt->mipsPerThread = 21; // TBD
      req_param[0].rscParam.pMipsExt->codeLocation = MMPM_BW_PORT_ID_DDR_SLAVE;
      req_param[0].rscParam.pMipsExt->reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

      req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
      req_rsc_param.numOfReq = 1;
      req_rsc_param.reqTag = 0;
      req_rsc_param.pStsArray = result_status;
      req_rsc_param.pReqArray = req_param;

      PM_MSG_1(DBG_MED_PRIO, "Sensors PM->ADSP PM Vote:  MIPS(%d)", req_param[0].rscParam.pMipsExt->mipsTotal);
      if(MMPM_STATUS_SUCCESS != MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param))
      {
        PM_MSG_0(DBG_ERROR_PRIO, "PM : MMPM bw request failed");
    }
    }
    else
    {
      MmpmRscExtParamType req_rsc_param;
      MMPM_STATUS result_status[1];
      MmpmRscParamType req_param[1];
      //MmpmGenBwValType bw_param[1];

      //bw_param[0].bwValue.busBwValue.bwBytePerSec = 50; // TBD
      //bw_param[0].bwValue.busBwValue.usagePercentage = 50; // TBD
      //bw_param[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
      //bw_param[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
      //bw_param[0].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;

      //req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
      //req_param[0].rscParam.pBwReq->numOfBw = 1;
      //req_param[0].rscParam.pBwReq->pBandWidthArray = bw_param;


      req_param[0].rscId = MMPM_RSC_ID_MIPS_EXT;
      req_param[0].rscParam.pMipsExt->mipsTotal = 13; // Based on profiling for low-power usecase
      req_param[0].rscParam.pMipsExt->mipsPerThread = 10; // Based on profiling for low-power usecase
      req_param[0].rscParam.pMipsExt->codeLocation = MMPM_BW_PORT_ID_DDR_SLAVE;
      req_param[0].rscParam.pMipsExt->reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

      req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
      req_rsc_param.numOfReq = 1;
      req_rsc_param.reqTag = 0;
      req_rsc_param.pStsArray = result_status;
      req_rsc_param.pReqArray = req_param;
      PM_MSG_1(DBG_MED_PRIO, "Sensors PM->ADSP PM Vote:  MIPS(%d)",  req_param[0].rscParam.pMipsExt->mipsTotal);

      if(MMPM_STATUS_SUCCESS != MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param))
    {
        PM_MSG_0(DBG_ERROR_PRIO, "PM : MMPM bw request failed");
      }
    }

#endif /* SNS_PCSIM */
  }
  else
  {
    /* No transition required */
    PM_MSG_0(DBG_MED_PRIO, "PM : No Active State Transition required");
    return;
  }
  sns_pm_curr_active_st_g = !(sns_pm_curr_active_st_g);
#if !defined SNS_PCSIM
  /* NOTE: This code will be executed for both high power and low power
   * transitions. This API will be called twice for a high power state transition
   * one time before the transition and second time is below call.
   * Calling the API twice does not matter. Wanted to avoid additional "if then"
   * checks in code
   */
//  busywait_config_cpu_freq(sns_pm_active_freq_g[sns_pm_curr_active_st_g]);
#endif

  // moved indication for loop into call back

#ifdef DSPS_RPM_SUPPORT
  /* Send log packet to apps processor indicating current active state
   * This code should be commented out during power measurement testing
   */

  //Allocate log packet
/*
  ret_err_code = sns_logpkt_malloc(SNS_LOG_DSPS_PWR,
                                   sizeof(sns_log_dsps_pwr_s),
                                   (void**)&log_struct_ptr);

  if ((ret_err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version    = SNS_LOG_DSPS_PWR_VERSION;
    log_struct_ptr->timestamp  = sns_em_get_timestamp();
    if (sns_pm_curr_active_st_g == SNS_PM_ACTIVE_HIGH)
    {
      log_struct_ptr->powerstate = SNS_LOG_DSPS_PWR_ST_HIGH;
    }
    else
    {
      log_struct_ptr->powerstate = SNS_LOG_DSPS_PWR_ST_LOW;
    }

    //Commit log (also frees up the log packet memory)
    sns_logpkt_commit(SNS_LOG_DSPS_PWR,
                      log_struct_ptr);
  }
*/
#endif  /* DSPS_RPM_SUPPORT */

} // end of sns_pm_chk_active_state_transition

/*===========================================================================

  FUNCTION:   sns_pm_proc_active_vote

===========================================================================*/
/*!
  @brief
  Processes the active state change vote request

  @param[i] pm_in_hdr : Pointer to message SMR header
  @param[i] msg_in_ptr: Ptr to the message

  @detail
  - Records the active state vote of the module voting for it
  - Allocates memory for the response msg
  - Calls the appropriate smr functions to send response to the requesting
    module

  @return
  Returns error code
  SNS_SUCCESS: If successful
  Error code:  In case of any errors

*/
/*=========================================================================*/
static sns_err_code_e sns_pm_proc_active_vote(sns_smr_header_s *pm_in_hdr,
                                              void *msg_in_ptr)
{
  sns_smr_header_s                           pm_out_hdr;
  sns_pm_active_pwr_st_change_req_msg_v01   *vote_msg_ptr;
  sns_pm_active_pwr_st_change_resp_msg_v01  *msg_out_ptr;
  sns_err_code_e                            err_code = SNS_SUCCESS;

  vote_msg_ptr = (sns_pm_active_pwr_st_change_req_msg_v01*) msg_in_ptr;
  if (pm_in_hdr->src_module == SNS_MODULE_DSPS_SAM)
  {
    PM_MSG_1(DBG_MED_PRIO, "PM : SAM Voted %d", vote_msg_ptr->vote_value);
    pm_vote_array_g[SNS_PM_SRC_MODULE_SAM] = vote_msg_ptr->vote_value;
  }
  else if (pm_in_hdr->src_module == SNS_MODULE_DSPS_SMGR)
  {
    PM_MSG_1(DBG_MED_PRIO, "PM : SMGR Voted %d", vote_msg_ptr->vote_value);
    pm_vote_array_g[SNS_PM_SRC_MODULE_SMGR] =vote_msg_ptr->vote_value;
  }

#ifdef SNS_UNIT_TEST
  else if (pm_in_hdr->src_module == SNS_MODULE_DSPS_TEST)
  {
    PM_MSG_1(DBG_MED_PRIO, "PM : UNIT_TEST Voted %d",vote_msg_ptr->vote_value);
    pm_vote_array_g[SNS_PM_SRC_MODULE_SMGR] =vote_msg_ptr->vote_value;
  }
#endif

  // Filling the QMI response msg
  msg_out_ptr = (sns_pm_active_pwr_st_change_resp_msg_v01*)
                sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_PWR,sizeof(sns_pm_active_pwr_st_change_resp_msg_v01));

  if (msg_out_ptr == NULL)
  {
    /* Reuse the incoming message to send the response back */
    pm_in_hdr->dst_module = pm_in_hdr->src_module;
    pm_in_hdr->src_module = SNS_MODULE_DSPS_PM;
    pm_in_hdr->msg_type   = SNS_SMR_MSG_TYPE_RESP_INT_ERR;
    pm_in_hdr->msg_id     = SNS_PM_ACTIVE_PWR_ST_CHANGE_RESP_V01;
    sns_smr_set_hdr(pm_in_hdr,msg_in_ptr);
    err_code = sns_smr_send(msg_in_ptr);
    if (err_code != SNS_SUCCESS)
    {
      PM_MSG_1(DBG_ERROR_PRIO, "PM : SMR send failure in vote req proc (%d)", err_code);
      /* Free the memory */
      sns_smr_msg_free(msg_in_ptr);
    }

    return err_code;
  }

  msg_out_ptr->resp.sns_result_t = SNS_SUCCESS;

  // Filling the SMR Response Header
  pm_out_hdr.dst_module  = pm_in_hdr->src_module;
  pm_out_hdr.src_module  = pm_in_hdr->dst_module;
  pm_out_hdr.svc_num     = SNS_PM_SVC_ID_V01;
  pm_out_hdr.msg_id      = SNS_PM_ACTIVE_PWR_ST_CHANGE_RESP_V01;
  pm_out_hdr.msg_type    = SNS_SMR_MSG_TYPE_RESP;
  pm_out_hdr.body_len    = sizeof(sns_pm_active_pwr_st_change_resp_msg_v01);
  pm_out_hdr.ext_clnt_id = pm_in_hdr->ext_clnt_id;
  pm_out_hdr.txn_id      = pm_in_hdr->txn_id;
  pm_out_hdr.priority    = pm_in_hdr->priority;

  // Send the Response back
  sns_pm_send_msg(&pm_out_hdr,(void*)msg_out_ptr);

  // Free the incoming message as well
  sns_smr_msg_free(msg_in_ptr);

  PM_MSG_0(DBG_MED_PRIO, "PM : Vote req msg processed");
  return err_code;

} // end of sns_pm_proc_active_vote

/*===========================================================================

  FUNCTION:   sns_pm_proc_version_req

===========================================================================*/
/*!
  @brief
  Provides the version number of the PM service

  @param[i] pm_in_hdr : Pointer to message SMR header
  @param[i] msg_in_ptr: Ptr to the message body

  @detail
  - Responds with response message containing the version number
    of the PM service
  - Allocates memory for the response msg
  - Calls the appropriate smr functions to send response to the requesting
    module

  @return
  Returns error code
  SNS_SUCCESS: If successful
  Error code:  In case of any errors

*/
/*=========================================================================*/
static sns_err_code_e sns_pm_proc_version_req(sns_smr_header_s *pm_in_hdr,
                                              void *msg_in_ptr)
{
  sns_smr_header_s                  pm_out_hdr;
  sns_common_version_resp_msg_v01   *msg_out_ptr;
  sns_err_code_e                    err_code = SNS_SUCCESS;

  // Filling the QMI response msg
  msg_out_ptr = (sns_common_version_resp_msg_v01*)
                sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_PWR,sizeof(sns_common_version_resp_msg_v01));

  if (msg_out_ptr == NULL)
  {
    /* Reuse the incoming message to send the response back */
    pm_in_hdr->dst_module = pm_in_hdr->src_module;
    pm_in_hdr->src_module = SNS_MODULE_DSPS_PM;
    pm_in_hdr->msg_type   = SNS_SMR_MSG_TYPE_RESP_INT_ERR;
    pm_in_hdr->msg_id     = SNS_PM_VERSION_RESP_V01;
    sns_smr_set_hdr(pm_in_hdr,msg_in_ptr);
    err_code = sns_smr_send(msg_in_ptr);
    if (err_code != SNS_SUCCESS)
    {
      PM_MSG_1(DBG_ERROR_PRIO, "PM : SMR send failure in version req proc (%d)", err_code);
      /* Free the memory */
      sns_smr_msg_free(msg_in_ptr);
    }

    return err_code;
  }

  msg_out_ptr->resp.sns_result_t        = SNS_SUCCESS;
  msg_out_ptr->interface_version_number = SNS_PM_SVC_V01_IDL_MAJOR_VERS;
  msg_out_ptr->max_message_id           = SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01;

  // Filling the SMR Response Header
  pm_out_hdr.dst_module  = pm_in_hdr->src_module;
  pm_out_hdr.src_module  = pm_in_hdr->dst_module;
  pm_out_hdr.svc_num     = SNS_PM_SVC_ID_V01;
  pm_out_hdr.msg_id      = SNS_PM_VERSION_RESP_V01;
  pm_out_hdr.msg_type    = SNS_SMR_MSG_TYPE_RESP;
  pm_out_hdr.body_len    = sizeof(sns_common_version_resp_msg_v01);
  pm_out_hdr.ext_clnt_id = pm_in_hdr->ext_clnt_id;
  pm_out_hdr.txn_id      = pm_in_hdr->txn_id;
  pm_out_hdr.priority    = pm_in_hdr->priority;

  // Send the Response back
  sns_pm_send_msg(&pm_out_hdr,(void*)msg_out_ptr);

  // Free the incoming message as well
  sns_smr_msg_free(msg_in_ptr);

  PM_MSG_0(DBG_MED_PRIO, "PM : Version request msg processed");
  return err_code;
} // end of sns_pm_proc_version_req

/*===========================================================================

  FUNCTION:   sns_pm_smr_msg_parser

===========================================================================*/
/*!
  @brief
  This function processes messages for the Sensors Power manager Service.

  @param[i]
  None

  @detail
  - Retreives the QMI message from SMR queue (msg body and msg header)
  - Processes the messages received
  - Performs power state changes if required
  Note: All the messages in SMR queue are looked at before acting on
  power state changes

  @return
  None

*/
/*=========================================================================*/
static void sns_pm_smr_msg_parser(void)
{
  void             *msg_in_ptr = NULL;
  sns_smr_header_s pm_in_hdr;
  sns_err_code_e   err_code;
  uint8_t           chk_active_transition;

  chk_active_transition = 0;
  while ( (msg_in_ptr = sns_smr_rcv(SNS_MODULE_DSPS_PM)) != NULL )
  {
    if ( (sns_smr_get_hdr(&pm_in_hdr, msg_in_ptr) != SNS_SUCCESS) &&
         (pm_in_hdr.svc_num != SNS_PM_SVC_ID_V01) )
    {
      PM_MSG_0(DBG_ERROR_PRIO, "PM : Error - Due to SMR Hdr OR Wrong srvc id");
      continue;
    }

    PM_MSG_2(DBG_MED_PRIO, "PM : Header info - MSG_ID=%d MSG_TYPE=%d",pm_in_hdr.msg_id, pm_in_hdr.msg_type);

    if(pm_in_hdr.svc_num == SNS_PM_SVC_ID_V01)
    {
    switch(pm_in_hdr.msg_id)
    {
      case SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01:

        PM_MSG_0(DBG_MED_PRIO, "PM : SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01");
        err_code = sns_pm_proc_active_vote(&pm_in_hdr, msg_in_ptr);
        if( err_code != SNS_SUCCESS)
        {
          /* We dont want to return as we need to process
           * rest of SMR queue.
           */
        }
        chk_active_transition = 1;
        break;

      case SNS_PM_CANCEL_REQ_V01:

        /* Do nothing. Cancel request is not applicable
           for sensors power service
        */
        break;

      case SNS_PM_VERSION_REQ_V01:

        err_code = sns_pm_proc_version_req(&pm_in_hdr, msg_in_ptr);
        if( err_code != SNS_SUCCESS)
        {
          /* We dont want to return as we need to process
           * rest of SMR queue
           */
        }
        break;
        default:
          PM_MSG_0(DBG_MED_PRIO, "PM : Invalid Message request");
          break;
      } // end of switch
    }

  } // end of while loop

  /* If we received active state votes check if the DSPS power state
   * needs to change
   */
  if (chk_active_transition)
  {
    sns_pm_chk_active_state_transition();
  }

} // end of sns_pm_smr_msg_parser
#endif

#if defined(SNS_QMI_ENABLE)
void* sns_pm_q_get (void)
{
  uint8_t os_err;
  void    *msg_ptr = NULL;

  sns_os_mutex_pend(pm_que_mutex_ptr, 0, &os_err );
  SNS_ASSERT ( os_err == OS_ERR_NONE );

  msg_ptr = sns_q_get(&pm_queue);

  os_err = sns_os_mutex_post(pm_que_mutex_ptr );
  SNS_ASSERT ( os_err == OS_ERR_NONE );

  return msg_ptr;
}
#endif /* OCMEM_ENABLE */

/*===========================================================================

  FUNCTION:   sns_pm_process_msg

===========================================================================*/
/*!
  @brief This function processes incoming messages for PM

  @detail

  @param[i]  none
  @return  none
 */
/*=========================================================================*/
void sns_pm_process_msg(void)
{
  pm_q_item_s *msgPtr;

  PM_MSG_0(DBG_MED_PRIO, "PM : sns_pm_process_msg");
  while ((msgPtr = (pm_q_item_s *)sns_pm_q_get()) != NULL)
  {
    PM_MSG_1(DBG_MED_PRIO, "PM : PM : msg type in queue is : %d", msgPtr->msg_type);
    if (msgPtr->msg_type == QMI_IDL_RESPONSE)
    {
      switch(msgPtr->msg_id)
      {
#ifdef OCMEM_ENABLE
        case SNS_SMGR_NUMBER_OF_CLIENTS_RESP_V01:
          // do nothing for now
          //body_size = sizeof(sns_smgr_number_of_clients_resp_msg_v01);
          break;
#endif
        default:
          break;
      }
    }
    else if (msgPtr->msg_type == QMI_IDL_INDICATION)
    {
      PM_MSG_1(DBG_MED_PRIO, "PM : msg id in queue is : %d", msgPtr->msg_id);
    }
    else if (msgPtr->msg_type == QMI_IDL_REQUEST)
    {
      void                *msg_in_ptr;
      uint8_t             chk_active_transition;
      uint32_t            pm_client;
      void                *connection_handle;
      qmi_req_handle      req_handle;
      unsigned int        msg_id;
      qmi_csi_cb_error    rc;

      connection_handle = msgPtr->connection_handle;
      req_handle = msgPtr->req_handle;
      msg_in_ptr = msgPtr->body_ptr;
      msg_id = msgPtr->msg_id;
      chk_active_transition = 0;

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"PM : QMI_IDL_REQUEST");

      switch ( msg_id )
      {
        case SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01:
        {
           PM_MSG_0(DBG_MED_PRIO, "PM : SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01");
           sns_pm_active_pwr_st_change_req_msg_v01   *vote_msg_ptr;
           sns_pm_active_pwr_st_change_resp_msg_v01  out_msg;
           uint16_t                                  msg_body_size = 0;

           msg_body_size = sizeof(sns_pm_active_pwr_st_change_resp_msg_v01);

           vote_msg_ptr = (sns_pm_active_pwr_st_change_req_msg_v01*) msg_in_ptr;

           /* Temp function to identify client from connection_handle */
           pm_client = vote_msg_ptr->module_id;

           PM_MSG_2(DBG_MED_PRIO, "PM : PM received power vote: %x from client:%lx", vote_msg_ptr->vote_value, pm_client);

           if (pm_client == SNS_MODULE_DSPS_SAM)
           {
             PM_MSG_1(DBG_MED_PRIO, "PM : SAM Voted: %d",vote_msg_ptr->vote_value);
             pm_vote_array_g[SNS_PM_SRC_MODULE_SAM] = vote_msg_ptr->vote_value;
           }
           else if (pm_client == SNS_MODULE_DSPS_SMGR)
           {
#ifdef OCMEM_ENABLE
              PM_MSG_1(DBG_MED_PRIO, "PM : enable_client_ind %d",enable_client_ind);
              if(enable_client_ind)
              {
                uint32 byte_written;
                if(vote_msg_ptr->vote_value == SNS_PM_INACTIVE && pm_vote_array_g[SNS_PM_SRC_MODULE_SMGR] != SNS_PM_INACTIVE)
                {
                  sns_ocmem_has_client_ind_msg_v01 *num_clnt_ind_ptr;
                  sns_smdl_msg_s *smdl_msg = (sns_smdl_msg_s *)SNS_OS_MALLOC(0, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_has_client_ind_msg_v01) );

                  num_clnt_ind_ptr = (sns_ocmem_has_client_ind_msg_v01 *) smdl_msg->body;

                  smdl_msg->hdr.dst_module = SNS_OCMEM_MODULE_KERNEL;
                  smdl_msg->hdr.src_module = SNS_OCMEM_MODULE_ADSP;
                  smdl_msg->hdr.msg_id = SNS_OCMEM_HAS_CLIENT_IND_V01;
                  smdl_msg->hdr.msg_type = SNS_OCMEM_MSG_TYPE_IND;
                  smdl_msg->hdr.msg_size = sizeof(sns_ocmem_has_client_ind_msg_v01);
                  num_clnt_ind_ptr->num_clients = 0;
                         

                  if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)smdl_msg),
                         ((qurt_size_t) (sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_has_client_ind_msg_v01))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                            ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
                  {
                    PM_MSG_0(DBG_MED_PRIO, "PM : cache flush success");
                  }

                  //write to SMDL
                  PM_MSG_0(DBG_MED_PRIO, "PM : write to SMDL for has_client_ind 0");
                  smdl_write(smdl_hndl_qdsp, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_has_client_ind_msg_v01), smdl_msg, 0);
                  SNS_OS_FREE(smdl_msg);
                }
                else if(vote_msg_ptr->vote_value != SNS_PM_INACTIVE && pm_vote_array_g[SNS_PM_SRC_MODULE_SMGR] == SNS_PM_INACTIVE)
                {
                  sns_ocmem_has_client_ind_msg_v01 *num_clnt_ind_ptr;
                  sns_smdl_msg_s *smdl_msg = (sns_smdl_msg_s *)SNS_OS_MALLOC(0, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_has_client_ind_msg_v01) );

                  num_clnt_ind_ptr = (sns_ocmem_has_client_ind_msg_v01 *) smdl_msg->body;

                  smdl_msg->hdr.dst_module = SNS_OCMEM_MODULE_KERNEL;
                  smdl_msg->hdr.src_module = SNS_OCMEM_MODULE_ADSP;
                  smdl_msg->hdr.msg_id = SNS_OCMEM_HAS_CLIENT_IND_V01;
                  smdl_msg->hdr.msg_type = SNS_OCMEM_MSG_TYPE_IND;
                  smdl_msg->hdr.msg_size = sizeof(sns_ocmem_has_client_ind_msg_v01);
                  num_clnt_ind_ptr->num_clients = 1;
                         

                  if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)smdl_msg),
                         ((qurt_size_t) (sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_has_client_ind_msg_v01))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                            ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
                  {
                    PM_MSG_0(DBG_MED_PRIO, "PM : cache flush success");
                  }

                  //write to SMDL
                  PM_MSG_0(DBG_MED_PRIO, "PM : write to SMDL for has_client_ind 1");
                  smdl_write(smdl_hndl_qdsp, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_has_client_ind_msg_v01), smdl_msg, 0);



                  SNS_OS_FREE(smdl_msg);
                }
              }
#endif /* OCMEM_ENABLE */
              PM_MSG_1(DBG_MED_PRIO, "PM : SMGR Voted: %d", vote_msg_ptr->vote_value);
              pm_vote_array_g[SNS_PM_SRC_MODULE_SMGR] =vote_msg_ptr->vote_value;

           }

           out_msg.resp.sns_result_t = SNS_SUCCESS;

           rc = qmi_csi_send_resp(req_handle, msg_id, (void *)&out_msg, msg_body_size);

           PM_MSG_0(DBG_MED_PRIO, "PM : Vote req msg processed");

           chk_active_transition = 1;
        }
        break;

        case SNS_PM_CANCEL_REQ_V01:
        {
          /* Do nothing. Cancel request is not applicable
                   * for sensors power service
                   */
        }
        break;

        case SNS_PM_VERSION_REQ_V01:
        {
           sns_common_version_resp_msg_v01   out_msg;
           uint16_t                          msg_body_size = 0;

           msg_body_size = sizeof(sns_common_version_resp_msg_v01);

           out_msg.resp.sns_result_t        = SNS_SUCCESS;
           out_msg.interface_version_number = SNS_PM_SVC_V01_IDL_MAJOR_VERS;
           out_msg.max_message_id           = SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01;

           // Send the Response back
           rc = qmi_csi_send_resp(req_handle, msg_id, (void *)&out_msg, msg_body_size);
        }
        break;

        default:
           PM_MSG_0(DBG_MED_PRIO, "PM : Invalid Message request");
        break;
      }

      if ( chk_active_transition )
      {
         sns_pm_chk_active_state_transition(connection_handle);
      }
    }
    else
    {
      PM_MSG_1(DBG_MED_PRIO, "PM : INVALID MSG TYPE %d",msgPtr->msg_type);
    }

    SNS_OS_FREE(msgPtr->body_ptr);
    SNS_OS_FREE(msgPtr);
  }
}


#ifndef SNS_PCSIM
#ifdef FEATURE_SMSM
/*===========================================================================

FUNCTION sns_pm_resume_timer_cb

DESCRIPTION
  Callback that gets called after resume time hysteresis

===========================================================================*/
void sns_pm_resume_timer_cb(void *arg)
{
  uint8_t os_err;
  PM_MSG_0(DBG_MED_PRIO, "PM : resume notification received");
  sns_os_mutex_pend(pm_suspend_resume_mutex_ptr, 0, &os_err );
  SNS_ASSERT ( os_err == OS_ERR_NONE );

  if(resume_from_suspend)
  {
    linux_kernel_suspend = false;
    sns_smgr_app_state_change(linux_kernel_suspend);
    sns_sam_ap_state_change_cb(linux_kernel_suspend);
  }

  os_err = sns_os_mutex_post(pm_suspend_resume_mutex_ptr );
  SNS_ASSERT ( os_err == OS_ERR_NONE );
}

/*===========================================================================

FUNCTION pm_smsm_cb

DESCRIPTION
  Call back registered with SMSM to be called when the SMSM_PROC_AWAKE bit is set
  on the other processor. This signals when the processor is either suspended or awake

===========================================================================*/
static void pm_smsm_cb (
  smsm_entry_type entry,
  smsm_state_type prevstate,
  smsm_state_type currstate,
  void * userData)
{
  uint32 bitsSet = (currstate ^ prevstate) & currstate;
  uint8_t os_err;
  bool state = bitsSet & SMSM_PROC_AWAKE;

  sns_os_mutex_pend(pm_suspend_resume_mutex_ptr, 0, &os_err );
  SNS_ASSERT ( os_err == OS_ERR_NONE );

  if( state )
  {
    resume_from_suspend = true;
    if ( SNS_ERR_FAILED == sns_em_register_timer(resume_tmr_obj, RESUME_HYSTERESIS_TICKS))
    {
      linux_kernel_suspend = false; 
      sns_smgr_app_state_change(linux_kernel_suspend);
      sns_sam_ap_state_change_cb(linux_kernel_suspend);
    }
  }
  else
  {
    PM_MSG_0(DBG_MED_PRIO, "PM : suspend notification received");

    if(resume_from_suspend)
    {
      sns_em_cancel_timer(resume_tmr_obj);
    }

    resume_from_suspend = false;
    linux_kernel_suspend = true;

    sns_smgr_app_state_change(linux_kernel_suspend);
    sns_sam_ap_state_change_cb(linux_kernel_suspend);
  }

  os_err = sns_os_mutex_post(pm_suspend_resume_mutex_ptr );
  SNS_ASSERT ( os_err == OS_ERR_NONE );
}

#endif /* FEATURE_SMSM */
#endif /* #ifndef SNS_PCSIM */

/*===========================================================================*/
/**
  @brief
   Initialize data structures for managing SMSM port

  @detail
   - Initialize signal flag table for each SMDL callback
   - Open SMDL port

  @return None

*/
/*=========================================================================*/
static void pm_smsm_init(void)
{
#ifndef BRINGUP_8X26
#ifndef SNS_PCSIM
  uint8_t err;
  pm_suspend_resume_mutex_ptr = sns_os_mutex_create(SNS_PM_QUE_MUTEX, &err);
  SNS_ASSERT (err == OS_ERR_NONE );

  sns_em_create_timer_obj( &sns_pm_resume_timer_cb, NULL, SNS_EM_TIMER_TYPE_ONESHOT, &resume_tmr_obj );
    
  smsm_cb_register( SMSM_APPS_STATE, SMSM_PROC_AWAKE, pm_smsm_cb, NULL);
#endif
#endif /* BRINGUP_8X26 */
}

/*===========================================================================

  FUNCTION:   sns_pm_main

===========================================================================*/
/*!
  @brief
  This function is the main pm thread function that pends on signals

  @param[i]
  No input arguments

  @detail
  Implements the power manager main function.
  - Pends on signals
  - Processes signals (if received) accordingly

  @return
  No return value

*/
/*=========================================================================*/
void sns_pm_main( void *arg)
{
#ifdef SNS_QMI_ENABLE
  uint8_t  os_err_code;
#endif

#ifdef OCMEM_ENABLE
  enable_client_ind = 0;
#endif /* OCMEM_ENABLE */

  uint8_t  os_error_code;
  OS_FLAGS signal_type;
  MMPM_STATUS req_result;

  PM_MSG_0(DBG_MED_PRIO, "PM : ADSPPM initialization");
  // Register with ADSPPM for ADSP core
  MmpmRegParamType  mmpmRegParam;
  char  clientName[] = "SENSORS PM";
  mmpmRegParam.rev = MMPM_REVISION;
  mmpmRegParam.coreId = MMPM_CORE_ID_LPASS_ADSP;
  mmpmRegParam.instanceId = MMPM_CORE_INSTANCE_0;
  mmpmRegParam.MMPM_Callback = cb_adsppm_core_adsp;
  mmpmRegParam.pClientName = clientName;
  mmpmRegParam.pwrCtrlFlag = PWR_CTRL_PERIODIC_CLIENT;
  mmpmRegParam.callBackFlag = CALLBACK_REQUEST_COMPLETE;
  mmpmRegParam.cbFcnStackSize = 512;
  adsppm_sns_id = MMPM_Register_Ext(&mmpmRegParam);
  if (0 == adsppm_sns_id)
  {
    /* TODO : error handle case */
    PM_MSG_0(DBG_ERROR_PRIO, "PM : ADSPPM registration for MMPM_CORE_ID_LPASS_ADSP failed");
  }

  // Register with ADSPPM for SRAM
  MmpmRegParamType  mmpmRegParamForSRAM;
  char  clientNameSRAM[] = "SENSORS PM (SRAM)";
  mmpmRegParamForSRAM.rev = MMPM_REVISION;
  mmpmRegParamForSRAM.coreId = MMPM_CORE_ID_LPASS_SRAM;
  mmpmRegParamForSRAM.instanceId = MMPM_CORE_INSTANCE_0;
  mmpmRegParamForSRAM.MMPM_Callback = cb_adsppm_core_sram;
  mmpmRegParamForSRAM.pClientName = clientNameSRAM;
  mmpmRegParamForSRAM.pwrCtrlFlag = PWR_CTRL_PERIODIC_CLIENT;
  mmpmRegParamForSRAM.callBackFlag = CALLBACK_REQUEST_COMPLETE;
  mmpmRegParamForSRAM.cbFcnStackSize = 512;
  adsppm_sns_sram_id = MMPM_Register_Ext(&mmpmRegParamForSRAM);
  if (0 == adsppm_sns_sram_id)
  {
    /* TODO : error handle case */
    PM_MSG_0(DBG_ERROR_PRIO, "PM : ADSPPM registration for MMPM_CORE_ID_LPASS_SRAM failed");
  }

  PM_MSG_0(DBG_MED_PRIO, "PM : ADSPPM register done");

  // Vote for Active Low case for ADSP, DDR, and SRAM (SRAM to be added later)
  MmpmRscExtParamType req_rsc_param;
  MMPM_STATUS result_status[2];
  MmpmRscParamType req_param[2];
  MmpmGenBwValType bw_value[1];
  MmpmBwReqType bw_param;
  MmpmMipsReqType mips_param;

  bw_value[0].bwValue.busBwValue.bwBytePerSec = 6000000; // Vote for high bw during initialization 
  bw_value[0].bwValue.busBwValue.usagePercentage = 100;
  bw_value[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
  bw_value[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
  bw_value[0].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
  bw_param.numOfBw = 1;
  bw_param.pBandWidthArray = (MmpmBwValType *) bw_value;

  req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
  req_param[0].rscParam.pBwReq = &bw_param;

  mips_param.mipsTotal = 30; // Vote for high clock rate during initialization
  mips_param.mipsPerThread = 21;
  mips_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
  mips_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

  req_param[1].rscId = MMPM_RSC_ID_MIPS_EXT;
  req_param[1].rscParam.pMipsExt = &mips_param;

  req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
  req_rsc_param.numOfReq = 2;
  req_rsc_param.reqTag = 2;
  req_rsc_param.pStsArray = result_status;
  req_rsc_param.pReqArray = req_param;
  PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP PM Vote: BW(%d) MIPS(%d)", bw_value[0].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);

  req_result = MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param);
  if(MMPM_STATUS_SUCCESS != req_result)
  {
    PM_MSG_1(DBG_ERROR_PRIO, "PM : mmpm request failed due to %d\n", req_result);
  }


  PM_MSG_0(DBG_MED_PRIO, "PM : QMI_CSI_REGISTER");
#ifdef SNS_QMI_ENABLE
  qmi_csi_error   sns_pm_qcsi_error;
  void            *pm_qcsi_service_cookie_ptr=NULL;

  sns_os_set_qmi_csi_params(sns_pm_sig_grp,(PM_QMI_WAIT_SIG),
                                   &sns_pm_qcsi_os_params,&os_err_code);
  SNS_ASSERT(os_err_code == OS_ERR_NONE);

  sns_pm_qcsi_error = qmi_csi_register(sns_smr_get_svc_obj(SNS_PM_SVC_ID_V01),
                        sns_pm_connect_cb, sns_pm_disconnect_cb, sns_pm_process_req_cb,
                        pm_qcsi_service_cookie_ptr, &sns_pm_qcsi_os_params,
                        &sns_pm_qcsi_service_handler_s);
  if(sns_pm_qcsi_error != QMI_CSI_NO_ERR)
   {
    PM_MSG_1(DBG_ERROR_PRIO, "PM : Error initializing qmi_csi_register, %d\n", sns_pm_qcsi_error);
   }
#endif



#ifdef OCMEM_ENABLE
  PM_MSG_0(DBG_MED_PRIO, "PM : SMDL OPEN");

  // SMDL initialize
  pm_smdl_port_s *smdl_port;

  smdl_port = &smdl_port_qdsp;
  smdl_port->type = SMD_APPS_QDSP;
  smdl_port->sig_flag[PM_SMDL_EVENT_OPEN] = SNS_SMDL_OPEN_SIG;
  smdl_port->sig_flag[PM_SMDL_EVENT_READ] = SNS_SMDL_READ_SIG;
  smdl_port->sig_flag[PM_SMDL_EVENT_WRITE] = SNS_SMDL_WRITE_SIG;
  smdl_port->sig_flag[PM_SMDL_EVENT_CLOSE] = SNS_SMDL_CLOSE_SIG;

  // Define the FIFO SIZE
  smdl_port_qdsp.hndl = smdl_open("SENSOR", smdl_port->type, 0,
                (4096*2), (smdl_callback_t)adsp_smdl_cb, &smdl_port_qdsp );
  smdl_hndl_qdsp = smdl_port_qdsp.hndl;
  if(smdl_hndl_qdsp == NULL) {
    /* TODO : disable smdl transaction when failed */
    PM_MSG_0(DBG_ERROR_PRIO, "PM : SMDL OPEN fail");
  }

  PM_MSG_0(DBG_MED_PRIO, "PM : Register client with SMGR service");
  /* Register as client with QCCI for SMGR service */
  {
    qmi_client_type smgr_user_handle;
    qmi_idl_service_object_type smgr_qmi_service;

    sns_os_sigs_add(sns_pm_sig_grp, SNS_PM_QMI_SMGR_CL_MSG_SIG);
    pm_smgr_cl_os_params.ext_signal = NULL;
    pm_smgr_cl_os_params.sig = SNS_PM_QMI_SMGR_CL_MSG_SIG;
    pm_smgr_cl_os_params.timer_sig = SNS_PM_QMI_SMGR_CL_TIMER_SIG;

    smgr_qmi_service = sns_smr_get_svc_obj(SNS_SMGR_SVC_ID_V01);
    qmi_client_notifier_init(smgr_qmi_service,
                             &pm_smgr_cl_os_params,
                             &smgr_user_handle);
  }

#endif /* OCMEM_ENABLE */

#if !defined BRINGUP_8X26
  PM_MSG_0(DBG_MED_PRIO, "PM Init: SMSM Register");
  pm_smsm_init();
#endif /* BRINGUP_8X26 */

  sns_init_done();

  while(1)
  {
    /* IMPORTANT Note:
         * The idle task should never be suspended or deleted otherwise there will be a crash.
         * There for the sns_os_flags_accept function is called which has the same functionality
         * as sns_os_sigs_pend but does not block/suspend the caller
         */
#ifdef SNS_QMI_ENABLE
    /* receive the internal QCSI signal here
         */
    PM_MSG_0(DBG_MED_PRIO, "PM : waiting for MSG");
    signal_type = sns_os_sigs_pend(sns_pm_sig_grp, sns_pm_sig_mask,
                                   OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                   0,
                                   &os_error_code);

    /* qmi_csi_handle_event() calls the corresponding callback function */
    PM_MSG_1(DBG_MED_PRIO, "PM : signal received : %d", signal_type);
    if ( (os_error_code == 0) && (signal_type & PM_QMI_WAIT_SIG ) )
    {
      qmi_csi_handle_event(sns_pm_qcsi_service_handler_s, &sns_pm_qcsi_os_params);
      signal_type &= (~PM_QMI_WAIT_SIG);
    }
    if((os_error_code == 0) && (signal_type & SNS_PM_ADSPPM_POWER_VOTE_CB_SIG))
    {
      sns_pm_active_pwr_st_change_ind_msg_v01 *ind_msg_ptr;

      // Send indication to all clients of power manager
      PM_MSG_0(DBG_MED_PRIO, "PM : Wake up from ADSPPM CB - send indication to all clients");
      ind_msg_ptr = (sns_pm_active_pwr_st_change_ind_msg_v01*)
                    sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_PWR,sizeof(sns_pm_active_pwr_st_change_ind_msg_v01));

      if (ind_msg_ptr == NULL)
      {
        /* Print error message */
      }
      else
      {
        ind_msg_ptr->curr_active_state = sns_pm_curr_active_st_g;

        qmi_csi_send_broadcast_ind(sns_pm_qcsi_service_handler_s,
                          SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01,
                          ind_msg_ptr,
                          sizeof(sns_pm_active_pwr_st_change_ind_msg_v01));

        sns_smr_msg_free( ind_msg_ptr );
      }
      signal_type &= (~SNS_PM_ADSPPM_POWER_VOTE_CB_SIG);
    }
    if((os_error_code == 0) && (signal_type & SNS_PM_MSG_SIG))
    {
      //process the items in the queue
      PM_MSG_0(DBG_MED_PRIO, "PM : received SNS_PM_MSG_SIG");
      sns_pm_process_msg();
      signal_type &= (~SNS_PM_MSG_SIG);
    }
#ifdef OCMEM_ENABLE
    else if((os_error_code == 0) && (signal_type & SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON))
    {
      PM_MSG_0(DBG_MED_PRIO, "PM : Wake up from ADSPPM CB - SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON");

      int i;
      int32 smdl_result;
      sns_ocmem_bw_vote_ind_msg_v01 *ind_ptr;
      sns_smdl_msg_s *smdl_msg = (sns_smdl_msg_s *)SNS_OS_MALLOC(0, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_ind_msg_v01) );

      ind_ptr = (sns_ocmem_bw_vote_ind_msg_v01 *) smdl_msg->body;

      smdl_msg->hdr.dst_module = SNS_OCMEM_MODULE_KERNEL;
      smdl_msg->hdr.src_module = SNS_OCMEM_MODULE_ADSP;
      smdl_msg->hdr.msg_id = SNS_OCMEM_BW_VOTE_IND_V01;
      smdl_msg->hdr.msg_type = SNS_OCMEM_MSG_TYPE_IND;
      smdl_msg->hdr.msg_size = sizeof(sns_ocmem_bw_vote_ind_msg_v01);
      ind_ptr->is_vote_on = 1;


      if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)smdl_msg),
             ((qurt_size_t) (sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_ind_msg_v01))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
      {
        PM_MSG_0(DBG_MED_PRIO, "PM : cache flush success");
      }

      //write to SMDL
      PM_MSG_0(DBG_MED_PRIO, "PM : Wake up from ADSPPM CB - sending MAP_ON indication");
      smdl_result = smdl_write(smdl_hndl_qdsp, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_ind_msg_v01), smdl_msg, 0);
      PM_MSG_1(DBG_MED_PRIO, "PM : smdl result %d", smdl_result);

      //memset(sns_ocmem_test_buffer, 0x67896789, BUF_SIZE);
      for (i=0;i<512;i++)
            sns_ocmem_test_buffer[i] = sns_ocmem_test_buffer[i] - (uint32_t) 0x99999999;

      if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)sns_ocmem_test_buffer),
             ((qurt_size_t) (512 * sizeof(uint32_t))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
      {
        PM_MSG_0(DBG_MED_PRIO, "PM : cache flush success");
      }

      SNS_OS_FREE(smdl_msg);
      signal_type &= (~SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON);

    }
    else if((os_error_code == 0) && (signal_type & SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF))
    {
      PM_MSG_0(DBG_MED_PRIO, "PM : Wake up from ADSPPM CB - SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF");

      int32 smdl_result;
      sns_ocmem_bw_vote_ind_msg_v01 *ind_ptr;
      sns_smdl_msg_s *smdl_msg = (sns_smdl_msg_s *)SNS_OS_MALLOC(0, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_ind_msg_v01) );

      ind_ptr = (sns_ocmem_bw_vote_ind_msg_v01 *) smdl_msg->body;

      smdl_msg->hdr.dst_module = SNS_OCMEM_MODULE_KERNEL;
      smdl_msg->hdr.src_module = SNS_OCMEM_MODULE_ADSP;
      smdl_msg->hdr.msg_id = SNS_OCMEM_BW_VOTE_IND_V01;
      smdl_msg->hdr.msg_type = SNS_OCMEM_MSG_TYPE_IND;
      smdl_msg->hdr.msg_size = sizeof(sns_ocmem_bw_vote_ind_msg_v01);
      ind_ptr->is_vote_on = 0;


      if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)smdl_msg),
             ((qurt_size_t) (sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_ind_msg_v01))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
      {
        PM_MSG_0(DBG_MED_PRIO, "PM : cache flush success");
      }

      //write to SMDL
      PM_MSG_0(DBG_MED_PRIO, "PM : Wake up from ADSPPM CB - sending MAP_OFF indication");
      smdl_result = smdl_write(smdl_hndl_qdsp, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_ind_msg_v01), smdl_msg, 0);
      PM_MSG_1(DBG_MED_PRIO, "PM : smdl result %d", smdl_result);

      SNS_OS_FREE(smdl_msg);
      signal_type &= (~SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF);
    }
    else if((os_error_code == 0) && (signal_type & SNS_SMDL_READ_SIG))
    {
      uint16 byte_read;
      PM_MSG_0(DBG_MED_PRIO, "PM : SNS_SMDL_READ_SIG");

      sns_smdl_msg_s *rcv_msg = (sns_smdl_msg_s *)SNS_OS_MALLOC(0, OCMEM_MAX_MSG_LEN );


      // read from SMDL
      byte_read = smdl_read(smdl_hndl_qdsp, OCMEM_MAX_MSG_LEN, rcv_msg, 0);
      PM_MSG_1(DBG_MED_PRIO, "PM : SMDL READ bytes read : %d", byte_read);

      if(byte_read == 0)
      {
        PM_MSG_0(DBG_MED_PRIO, "PM : no data in SMD");
      }


      PM_MSG_1(DBG_MED_PRIO, "PM : SMDL READ msg_id is : %d", rcv_msg->hdr.msg_id);
      uint32 byte_written;


      switch(rcv_msg->hdr.msg_id)
      {
        case SNS_OCMEM_PHYS_ADDR_REQ_V01:
        {
          PM_MSG_0(DBG_MED_PRIO, "PM : OCMEM - OCMEM_PHY_ADDR_REQ received");
          sns_ocmem_phys_addr_resp_msg_v01 *msg_body;
          sns_smdl_msg_s *smdl_msg;

          smdl_msg = (sns_smdl_msg_s *)SNS_OS_MALLOC(0, (sizeof(sns_ocmem_hdr_s)+sizeof(sns_ocmem_phys_addr_resp_msg_v01)) );

          if (smdl_msg == NULL ) {
            PM_MSG_0(DBG_ERROR_PRIO, "PM : sensors_ocmem_test - SNS_OS_MALLOC returned error");
            return(1);
          } else {
            PM_MSG_0(DBG_MED_PRIO, "PM : sensors_ocmem_test - successfully allocated memory for smdl_msg");
          }

          smdl_msg->hdr.dst_module = SNS_OCMEM_MODULE_KERNEL;
          smdl_msg->hdr.src_module = SNS_OCMEM_MODULE_ADSP;
          smdl_msg->hdr.msg_id = SNS_OCMEM_PHYS_ADDR_RESP_V01;
          smdl_msg->hdr.msg_type = SNS_OCMEM_MSG_TYPE_RESP;
          smdl_msg->hdr.msg_size = sizeof(sns_ocmem_phys_addr_resp_msg_v01);

          // TODO : add routine to find the physical addr
          {
			 
            /////////// for testing only ////////////
            char *test_phys;
            int i;

            test_phys = (char *) qurt_lookup_physaddr((qurt_addr_t) sns_ocmem_test_buffer);

            for (i=0;i<2048;i++)
                  sns_ocmem_test_buffer[i]= (uint32_t) 0xABCDABCD;
            

            // flush the buffer from cache into DDR or OCMEM
            if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)sns_ocmem_test_buffer),
                   ((qurt_size_t) (2048*(sizeof(uint32_t)))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                      ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
            {
              error_word_msg_for_ocmem_test = 0xBABEBABE;
            }
            else
            {
              error_word_msg_for_ocmem_test = 0xDEADDEAD;
            }

            msg_body =  (sns_ocmem_phys_addr_resp_msg_v01 *) (smdl_msg->body);

            msg_body->segments_len = 3;
            msg_body->segments_valid = 1;
            msg_body->segments[0].type = 0;
            msg_body->segments[0].size = 1024*sizeof(uint32_t);
            msg_body->segments[0].start_address = (uint64_t) test_phys;

            msg_body->segments[1].type = 1;
            msg_body->segments[1].size = ((uint32_t)(&sns_code_end) - (uint32_t)(&sns_code_start));
            msg_body->segments[1].start_address = (uint64_t)qurt_lookup_physaddr((uint32_t)(&sns_code_start));

            msg_body->segments[2].type = 0;
            msg_body->segments[2].size = ((uint32_t)(&sns_data_end) - (uint32_t)(&sns_data_start));
            msg_body->segments[2].start_address = (uint64_t)qurt_lookup_physaddr((uint32_t)(&sns_data_start));
            /////////// end of for testing only ////////////
          }
          /*
          // commented out until OCMEM access is verified
          msg_body =  (sns_ocmem_phys_addr_resp_msg_v01 *) (smdl_msg->body);
          msg_body->segments_len = 1;
          msg_body->segments_valid = 1;
          msg_body->segments[0].type = 1;
          msg_body->segments[0].size = ((uint32_t)(&sns_code_end) - (uint32_t)(&sns_code_start));
          msg_body->segments[0].start_address = (uint64_t)qurt_lookup_physaddr((uint32_t)(&sns_code_start));
*/

          PM_MSG_2(DBG_MED_PRIO, "PM : Sensors read only (code) segment size = %d, start address = 0x%x!\n",
          msg_body->segments[0].size, msg_body->segments[0].start_address);

          PM_MSG_0(DBG_MED_PRIO, "PM : set enable_client_ind = 1");
          enable_client_ind = 1;

          if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)smdl_msg),
                 ((qurt_size_t) (sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_phys_addr_resp_msg_v01))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                    ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
          {
            PM_MSG_0(DBG_MED_PRIO, "PM : cache flush success");
          }

          PM_MSG_0(DBG_MED_PRIO, "PM : write to SMDL for phy addr resp");
          smdl_write(smdl_hndl_qdsp, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_phys_addr_resp_msg_v01), smdl_msg, 0);
          SNS_OS_FREE(smdl_msg);

          break;
        }
        case SNS_OCMEM_BW_VOTE_REQ_V01:
        {
          uint8_t is_map = 0;
          sns_ocmem_bw_vote_resp_msg_v01 *resp_msg;
          sns_smdl_msg_s *smdl_msg = (sns_smdl_msg_s *)SNS_OS_MALLOC(0, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_resp_msg_v01) );

          PM_MSG_0(DBG_MED_PRIO, "PM : OCMEM_BW_VOTE_REQ received");

          smdl_msg->hdr.dst_module = SNS_OCMEM_MODULE_KERNEL;
          smdl_msg->hdr.src_module = SNS_OCMEM_MODULE_ADSP;
          smdl_msg->hdr.msg_id = SNS_OCMEM_BW_VOTE_RESP_V01;
          smdl_msg->hdr.msg_type = SNS_OCMEM_MSG_TYPE_RESP;
          smdl_msg->hdr.msg_size = sizeof(sns_ocmem_bw_vote_resp_msg_v01);
          sns_ocmem_bw_vote_req_msg_v01 *req_msg;
          req_msg = (sns_ocmem_bw_vote_req_msg_v01 *)(rcv_msg->body);

          is_map = req_msg->is_map;
          PM_MSG_2(DBG_MED_PRIO, "PM : is_map=%d, vectors_valid=%d, vectors_len=%d", req_msg->is_map, req_msg->vectors_valid, req_msg->vectors_len);

          resp_msg = (sns_ocmem_bw_vote_resp_msg_v01 *)(smdl_msg->body);

          if (QURT_EOK == (qurt_mem_cache_clean(((qurt_addr_t)smdl_msg),
                 ((qurt_size_t) (sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_resp_msg_v01))), ((qurt_mem_cache_op_t) (QURT_MEM_CACHE_FLUSH)),
                    ((qurt_mem_cache_type_t) (QURT_MEM_DCACHE) ))))
          {
            PM_MSG_0(DBG_MED_PRIO, "PM : cache flush success");
          }

          PM_MSG_0(DBG_MED_PRIO, "PM : write to SMDL for bw vote resp");
          byte_written = smdl_write(smdl_hndl_qdsp, sizeof(sns_ocmem_hdr_s) + sizeof(sns_ocmem_bw_vote_resp_msg_v01), smdl_msg, 0);


          PM_MSG_1(DBG_MED_PRIO, "PM : %d bytes have been written", byte_written);
          SNS_OS_FREE(smdl_msg);

          if(is_map)
          {
              PM_MSG_0(DBG_MED_PRIO, "PM : is_map is true");
              MmpmRscExtParamType req_rsc_param;
              MMPM_STATUS result_status[2];
              MmpmRscParamType req_param[2];
              MmpmGenBwValType bw_value[2];
              MmpmBwReqType bw_param;
              MmpmMipsReqType mips_param;


              bw_value[0].bwValue.busBwValue.bwBytePerSec = 1417273; // Based on OCMEM profiling, this value for OCMEM bandwidth needed for low-power usecase needs to be changed; 
              bw_value[0].bwValue.busBwValue.usagePercentage = 100; 
              bw_value[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
              bw_value[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
              bw_value[0].busRoute.slavePort = MMPM_BW_PORT_ID_OCMEM_SLAVE;

              bw_value[1].bwValue.busBwValue.bwBytePerSec = 343318; // Based on profiling, this value for DDR bandwidth needed for low-power usecase needs to be changed
              bw_value[1].bwValue.busBwValue.usagePercentage = 100; 
              bw_value[1].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
              bw_value[1].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
              bw_value[1].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
			  
              bw_param.numOfBw = 2;
              bw_param.pBandWidthArray = bw_value;

              req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
              req_param[0].rscParam.pBwReq = &bw_param;

              mips_param.mipsTotal = 13;  // MIPs needed for low-power usecase, should be changed based on OCMEM profiling results 
              mips_param.mipsPerThread = 10; 
              mips_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
              mips_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

              req_param[1].rscId = MMPM_RSC_ID_MIPS_EXT;
              req_param[1].rscParam.pMipsExt = &mips_param;

              req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
              req_rsc_param.numOfReq = 2;
              req_rsc_param.reqTag = 1;
              req_rsc_param.pStsArray = result_status;
              req_rsc_param.pReqArray = req_param;

              PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP OCMEM Vote: BW(%d) MIPS(%d)", bw_value[0].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);
			  PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP DDR PM Vote: BW(%d) MIPS(%d)", bw_value[1].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);
			  PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP for PNOC PM Vote: BW(%d) MIPS(%d)", bw_value[2].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);
              if(MMPM_STATUS_SUCCESS != MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param))
              {
                PM_MSG_0(DBG_ERROR_PRIO, "PM : MMPM BW request failed");
              }
          }
          else
          {
              PM_MSG_0(DBG_MED_PRIO, "PM : is_map is false");
              MmpmRscExtParamType req_rsc_param;
              MMPM_STATUS result_status[2];
              MmpmRscParamType req_param[2];
              MmpmGenBwValType bw_value[2];
              MmpmBwReqType bw_param;
              MmpmMipsReqType mips_param;


              bw_value[0].bwValue.busBwValue.bwBytePerSec = 0; // No OCMEM bandwidth needed any more
              bw_value[0].bwValue.busBwValue.usagePercentage = 0; 
              bw_value[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
              bw_value[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
              bw_value[0].busRoute.slavePort = MMPM_BW_PORT_ID_OCMEM_SLAVE;

              bw_value[1].bwValue.busBwValue.bwBytePerSec = 1728381; // DDR Bandwidth needed for low-power usecase 
              bw_value[1].bwValue.busBwValue.usagePercentage = 100; 
              bw_value[1].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
              bw_value[1].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
              bw_value[1].busRoute.slavePort = MMPM_BW_PORT_ID_DDR_SLAVE;
			  
              bw_param.numOfBw = 2;
              bw_param.pBandWidthArray = bw_value;

              req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
              req_param[0].rscParam.pBwReq = &bw_param;

              mips_param.mipsTotal = 13;  // MIPs needed for low-power usecase
              mips_param.mipsPerThread = 10; 
              mips_param.codeLocation = MMPM_BW_PORT_ID_ADSP_MASTER;
              mips_param.reqOperation = MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY;

              req_param[1].rscId = MMPM_RSC_ID_MIPS_EXT;
              req_param[1].rscParam.pMipsExt = &mips_param;

              req_rsc_param.apiType = MMPM_API_TYPE_ASYNC;
              req_rsc_param.numOfReq = 2;
              req_rsc_param.reqTag = 3;
              req_rsc_param.pStsArray = result_status;
              req_rsc_param.pReqArray = req_param;

              PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP OCMEM Vote: BW(%d) MIPS(%d)", bw_value[0].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);
			  PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP DDR PM Vote: BW(%d) MIPS(%d)", bw_value[1].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);
			  PM_MSG_2(DBG_MED_PRIO, "Sensors PM->ADSP for PNOC PM Vote: BW(%d) MIPS(%d)", bw_value[2].bwValue.busBwValue.bwBytePerSec, mips_param.mipsTotal);
              if(MMPM_STATUS_SUCCESS != MMPM_Request_Ext(adsppm_sns_id, &req_rsc_param))
              {
                PM_MSG_0(DBG_ERROR_PRIO, "PM : MMPM BW request failed");
              }
          }
          break;
        }
        default:
        break;
      }

      SNS_OS_FREE(rcv_msg);

      signal_type &= (~SNS_SMDL_READ_SIG);
    }
#endif /* OCMEM_ENABLE */
#else  /* SNS_QMI_ENABLE */
    signal_type = sns_os_sigs_pend(sns_pm_sig_grp,
                                   SNS_PM_SMR_SIGNAL_MASK,
                                   OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                   10,
                                   &os_error_code);

    if ( (os_error_code == 0) && (signal_type & SNS_PM_SMR_MSG_SIG) )
    {
      PM_MSG_0(DBG_MED_PRIO, "PM : Accpeted msg calling sns_pm_smr_msg_parser");
      sns_pm_smr_msg_parser();
    }
    else if((os_error_code == 0) && (signal_type & SNS_PM_ADSPPM_POWER_VOTE_CB_SIG))
    {
      uint8_t                                  loop_var;
      sns_pm_active_pwr_st_change_ind_msg_v01 *ind_msg_ptr;
      sns_smr_header_s                        ind_msg_hdr;

      // Send indication to all clients of power manager
      PM_MSG_0(DBG_MED_PRIO, "PM : send indication to all clients");
      for(loop_var = 0; loop_var < SNS_PM_NUM_VOTING_CLIENTS; loop_var++)
      {
        ind_msg_ptr = (sns_pm_active_pwr_st_change_ind_msg_v01*)
                      sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_PWR,sizeof(sns_pm_active_pwr_st_change_ind_msg_v01));

        if (ind_msg_ptr == NULL)
        {
          //Print error message
          continue;
        }

        ind_msg_ptr->curr_active_state = sns_pm_curr_active_st_g;

        // Filling the SMR Response Header
        ind_msg_hdr.dst_module  = sns_pm_voting_clients_g[loop_var];
        ind_msg_hdr.src_module  = SNS_MODULE_DSPS_PM;
        ind_msg_hdr.svc_num     = SNS_PM_SVC_ID_V01;
        ind_msg_hdr.msg_id      = SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01;
        ind_msg_hdr.msg_type    = SNS_SMR_MSG_TYPE_IND;
        ind_msg_hdr.body_len    = sizeof(sns_pm_active_pwr_st_change_ind_msg_v01);
        ind_msg_hdr.ext_clnt_id = 0;
        ind_msg_hdr.txn_id      = 0;
        ind_msg_hdr.priority    = SNS_SMR_MSG_PRI_LOW;

        PM_MSG_0(DBG_MED_PRIO, "PM : sending indication to client %d", sns_pm_voting_clients_g[loop_var]);
        sns_pm_send_msg(&ind_msg_hdr,(void*)ind_msg_ptr);
      } //end of for
    }

#endif /* SNS_QMI_ENABLE */
  }

} // end of function sns_pm_main

/*===========================================================================

  FUNCTION:   sns_pm_init

===========================================================================*/
/*!
  @brief
  This function initializes the sensors power manager

  @param[i]
  None

  @detail
  - Creates and initializes event flag group
  - registers for SMR Queue
  - Initializes global variables

  @return
  Returns error code
  SNS_SUCCESS     : Init went through fine
  Any other value : Errors were encountered during init
*/
/*=========================================================================*/
sns_err_code_e sns_pm_init(void)
{
  uint8_t err;
#ifndef SNS_EXCLUDE_POWER
  uint8_t        os_err_code;
#if !defined SNS_PCSIM && !defined QDSP6
  DALResult      send_msg_err_code;
  RPM_Message    active_st_msg;
#endif /* SNS_PCSIM */
#endif /* SNS_EXCLUDE_POWER */

#if defined(OCMEM_ENABLE) || defined(SNS_QMI_ENABLE)
  pm_que_mutex_ptr = sns_os_mutex_create(SNS_PM_QUE_MUTEX, &err);
  SNS_ASSERT (err == OS_ERR_NONE );

  sns_q_init(&pm_queue);
#endif /* OCMEM_ENABLE */

#if !defined SNS_PCSIM && !defined QDSP6
  npa_init();
#endif

#ifndef SNS_EXCLUDE_POWER
#if !defined SNS_PCSIM && !defined QDSP6

  /* Do MPM and sleep driver init */
  mpmint_init();
  sleep_init();
#endif /* SNS_PCSIM */

  /* Creating the signal/event flags */
  sns_pm_sig_grp = sns_os_sigs_create((OS_FLAGS)SNS_PM_MSG_SIG, &os_err_code);
  sns_os_sigs_add(sns_pm_sig_grp, SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON);
  sns_os_sigs_add(sns_pm_sig_grp, SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF);
  sns_os_sigs_add(sns_pm_sig_grp, SNS_PM_ADSPPM_POWER_VOTE_CB_SIG);

#ifdef OCMEM_ENABLE
  sns_os_sigs_add(sns_pm_sig_grp, SNS_SMDL_READ_SIG);
  sns_os_sigs_add(sns_pm_sig_grp, SNS_SMDL_WRITE_SIG);
  sns_os_sigs_add(sns_pm_sig_grp, SNS_SMDL_OPEN_SIG);
  sns_os_sigs_add(sns_pm_sig_grp, SNS_SMDL_CLOSE_SIG);
#endif /* OCMEM_ENABLE */
  SNS_ASSERT(sns_pm_sig_grp != NULL);

#ifdef SNS_QMI_ENABLE
  sns_os_sigs_add(sns_pm_sig_grp, PM_QMI_WAIT_SIG);
  sns_pm_sig_mask |= (PM_QMI_WAIT_SIG);
  sns_pm_sig_mask |= (SNS_PM_ADSPPM_POWER_VOTE_CB_SIG);
  sns_pm_sig_mask |= (SNS_PM_MSG_SIG);

#ifdef OCMEM_ENABLE
  sns_pm_sig_mask |= (SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON);
  sns_pm_sig_mask |= (SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF);
  sns_pm_sig_mask |= SNS_SMDL_READ_SIG;
  sns_pm_sig_mask |= SNS_SMDL_WRITE_SIG;
  sns_pm_sig_mask |= SNS_SMDL_OPEN_SIG;
  sns_pm_sig_mask |= SNS_SMDL_CLOSE_SIG;
  sns_pm_sig_mask |= SNS_PM_QMI_SMGR_CL_MSG_SIG;

#endif /* OCMEM_ENABLE */
#else /* SNS_QMI_ENABLE */
  /* Registering queue with SMR (To receive votes from other modules) */
  ret_err_code = sns_smr_register(SNS_MODULE_DSPS_PM,
                            sns_pm_sig_grp,
                              SNS_PM_SMR_MSG_SIG);

   SNS_ASSERT(ret_err_code == SNS_SUCCESS);
#endif /* SNS_QMI_ENABLE */

  /* Initialize Sensors Power Manager Global Variables */
  pm_vote_array_g[SNS_PM_SRC_MODULE_SMGR] = SNS_PM_INACTIVE;
  pm_vote_array_g[SNS_PM_SRC_MODULE_SAM]  = SNS_PM_INACTIVE;
  sns_pm_voting_clients_g[0] = SNS_MODULE_DSPS_SMGR;
  sns_pm_voting_clients_g[1] = SNS_MODULE_DSPS_SAM;
  /* Note for configuring the busywait we need to provide frequency values in
   * KHz. Hence we are storing the values in Khz in the global variables to
   * feed into busywait API directly
   */
  sns_pm_active_freq_g[0]    = SNS_PM_LOW_PWR_FREQ_MHZ*1000; // Low power frequency 27000khz
  sns_pm_active_freq_g[1]    = SNS_PM_HIGH_PWR_FREQ_MHZ*1000; // High power frequency 64000khz


  //MMPM_vote_low_power
  sns_pm_curr_active_st_g = SNS_PM_ACTIVE_HIGH;

#if !defined SNS_PCSIM
  /* Set Busywait frequency */
//  busywait_config_cpu_freq(sns_pm_active_freq_g[SNS_PM_ACTIVE_LOW]);
  /* Send active state of DSPS at power up to RPM */

  //active_st_msg.msg_type = Active_Low;


#endif /* PC_SIM */

  PM_MSG_0(DBG_MED_PRIO, "PM : Init SUCCESS");
#endif /* SNS_EXCLUDE_POWER */

  PM_MSG_1(DBG_MED_PRIO, "PM : PM starting.... id(%d)", SNS_MODULE_DSPS_PM);

  err = sns_os_task_create_ext(sns_pm_main,
                               NULL,
                               &sns_pm_stack[SNS_MODULE_STK_SIZE_DSPS_PM-1],
                               SNS_MODULE_PRI_DSPS_PM,
                               SNS_MODULE_PRI_DSPS_PM,
                               &sns_pm_stack[0],
                               SNS_MODULE_STK_SIZE_DSPS_PM,
                               (void *)0,
                               OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                               (uint8_t *)"SNS_PM");
  return err;
} // end of sns_pm_init


uint32 cb_adsppm_core_adsp(MmpmCallbackParamType *pCbParam)
{
  uint8_t err;
  MmpmCompletionCallbackDataType *p_cb_data;

  p_cb_data = (MmpmCompletionCallbackDataType *) pCbParam->callbackData;

  //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"adsppm call back from adsp core\n");

  if(p_cb_data->result == MMPM_STATUS_SUCCESS)
  {
    // reqTag 0 for power vote
    // reqTag 1 for OCMEM map on
    // reqTag 2 for power vote init
    //PM_LOG_1("adsp core MMPM_STATUS_SUCCESS on %lu\n", p_cb_data->reqTag);
    if(p_cb_data->reqTag == 1)
    {
      //PM_LOG_0("PM : ping SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON");
      sns_os_sigs_post(sns_pm_sig_grp, SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_ON, OS_FLAG_SET, &err);
    }
    else if(p_cb_data->reqTag == 3)
    {
      //PM_LOG_0("PM : ping SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF");
      sns_os_sigs_post(sns_pm_sig_grp, SNS_PM_ADSPPM_OCMEM_CB_SIG_MAP_OFF, OS_FLAG_SET, &err);
    }
    else if(p_cb_data->reqTag == 0)
    {
      //printf("ping SNS_PM_ADSPPM_POWER_VOTE_CB_SIG\n");
      //PM_LOG_0("ping SNS_PM_ADSPPM_POWER_VOTE_CB_SIG\n");
      //PM_LOG_0("PM : ping SNS_PM_ADSPPM_POWER_VOTE_CB_SIG");
      //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ping SNS_PM_ADSPPM_POWER_VOTE_CB_SIG\n");
      sns_os_sigs_post(sns_pm_sig_grp, SNS_PM_ADSPPM_POWER_VOTE_CB_SIG, OS_FLAG_SET, &err);
    }
  }
  else
  {
    // CB failed, print out the reason
  }

  return 0;
}
uint32 cb_adsppm_core_sram(MmpmCallbackParamType *pCbParam)
{
  //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"adsppm call back from sram core");
  return 0;
}

#ifdef OCMEM_ENABLE
void adsp_smdl_cb (smdl_handle_type hndl, smdl_event_type event, const pm_smdl_port_s *smdl_port_ptr)
{
  uint8_t   os_err;
  OS_FLAGS  sig_flag;

  //SNS_ASSERT(hndl == smdl_port_qdsp.hndl);
  PM_MSG_0(DBG_MED_PRIO, "PM : adsp_smdl_cb");
  switch ( event )
  {
    case SMDL_EVENT_READ:
      sig_flag = smdl_port_ptr->sig_flag[PM_SMDL_EVENT_READ];
      break;
    case SMDL_EVENT_OPEN:
      sig_flag = smdl_port_ptr->sig_flag[PM_SMDL_EVENT_OPEN];
      break;
    case SMDL_EVENT_WRITE:
      sig_flag = smdl_port_ptr->sig_flag[PM_SMDL_EVENT_WRITE];
      break;
    case SMDL_EVENT_REMOTE_CLOSE:
      sig_flag = smdl_port_ptr->sig_flag[PM_SMDL_EVENT_CLOSE];
      break;
    default:
      return;         /* silently ignore all other events */
  }

  sns_os_sigs_post(sns_pm_sig_grp, sig_flag, OS_FLAG_SET, &os_err);
  SNS_ASSERT (OS_ERR_NONE == os_err);
}
#endif /* OCMEM_ENABLE */
