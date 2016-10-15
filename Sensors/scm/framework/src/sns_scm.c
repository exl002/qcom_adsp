/*============================================================================
  FILE: sns_scm.c

  This file contains the Sensors Calibration Manager implementation

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 ============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/scm/framework/src/sns_scm.c#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-03-28  ks   Gyro Cal stream early termination fix  
  2014-03-12  lka  Added change to prevent disabling QMD when there is a client left.
  2014-02-26  lka  Added support for multi-client support to calibration services.
  2013-12-16  rt   Fixed memory leak issues on QMI client errors
  2013-12-11  ks   Introduced AMD gating support for QMag Cal
  2013-10-20  ad   Fixed klocwork errors
  2013-10-04  sc   Release QMI client notifier handler after being notified for service registration
  2013-09-12  ad   Fix unintended apps wakeup issue when calibration algorithms are active
  2013-09-09  pk   Clean up F3 logs in processing buffering indications
  2013-09-04  pk   Fix NULL pointer dereferencing when processing buffering indications
  2013-08-09  ps   Eliminate compilier warnings when ADSP_STANDALONE is defined
  2013-07-31  hw   Fix Klocwork warning error
  2013-07-26  vh   Eliminated compiler warnings
  2013-06-11  ae   Added QDSP SIM playback support
  2013-06-05  ag   Fixed debug macro to support diag api change
  2013-02-25  ps   Added macros to support ADSP_STANDALONE mode
  2013-03-05  vh   Replaced previous debug message MACROs with standard debug message MACROs
  2013-02-07  dc   Allow AMD and SCM calibration routines within ADSP to get SMGR data
                   even when kernel has suspended.
  2013-02-13  pn   Fixed compiler warning regarding unused stack variables.
  2013-01-30  ad   Move factory calibration to sensors manager
  2012-12-10  ag   Initialize ext_signal to NULL
  2012-12-03  pn   Uses uint16 instead of uint8 for message size.
  2012-11-08  vh   Assigned proper value for Item_len of SNS_SMGR_REPORT_REQ_V01
  2012-11-05  vh   Added malloc for message request
  2012-11-02  vh   Change the task create to sns_os_task_create_ext for SNS_DSPS_BUILD
  2012-10-30  vh   Passing the decoded message in sns_scm_process_sensor_status_ind
  2012-10-24  vh   Replaced printf with macros
  2012-10-17  vh   Added QCCI calls for registry code
  2012-10-14  sc   Use AMD enable request structure instead of common QMD struct
  2012-10-14  sc   Added SCM as client of registry service
  2012-10-02  ag   SMR cleanup
  2012-09-12  vh   Modified callbacks to process data in task loop
  2012-08-12  vh   Allocated memory for QMI Response Callback data
  2012-08-02  ag   Fix compilation errors when SNS_QMI_ENABLE is not defined
  2012-07-26  vh   Added QCCI calls
  2012-07-03  rp   change error to low debug message if the SCM log packets are disabled
  2012-03-07  ad   Support gyro autocal sample rate config via registry
  2012-02-02  ad   Consolidate all registry requests during initialization
  2012-01-19  ks   Reenabling Debug Msgs disabled for PCSim
  2012-01-18  ks   Disabling debug msgs for PCSim, reenabling for On Target
  2012-01-06  ad   Cleanup tabs
  2011-12-19  ks   PCSIM workaround for Calibration Cycle 1 minute timeout
  2011-11-14  jhh  Updated alloc and free function calls to meet new API
  2011-10-10  ad   Do not disable QMD on report from SMGR when in motion
  2011-08-30  ad   Add registry support for gyro autocal params
  2011-09-09  sc   Update with registry service V02
  2011-08-12  ad   Activate QMD only for gyro active use case
  2011-08-09  ad   Added check on gyro absolute value for bias calibration
  2011-08-04  rb   Added factory calibration initialization
  2011-08-03  rb   Added bias value storage during shutdown
  2011-07-29  rb   Added checking for allowance of algorithm instantiation
  2011-07-20  ad   Refactor SCM for memory optimization
  2011-07-13  rb   Added support for calibration via registry
  2011-07-11  ad   request filtered data from SMGR for better noise performance
  2011-07-06  ad   handle back-to-back idle/active messages from sensors manager
  2011-07-05  sc   Re-formated with UNIX newline ending
  2011-06-28  ad   incorporate SMGR update to sensor status indication behavior
  2011-06-21  ad   updated gyro bias calibration algorithm
  2011-06-08  ad   changed the gyro sampling rate for offset cal to 25Hz
  2011-04-08  ad   register for QMD only if calibration algorithm is active
  2011-03-17  ad   add debug messages
  2011-03-14  ad   add gyro cal log support
  2011-03-04  ad   integrate SMGR calibration interface
  2011-02-04  ad   initial version

 ============================================================================*/

#define __SNS_MODULE__ SNS_SCM

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_scm_priv.h"
#include "gyro_cal.h"
#include "qmag_cal.h"

#include "sns_reg_common.h"
#include "sns_reg_api_v02.h"

#include "sns_init.h"
#include "sns_memmgr.h"
#include "sns_smgr_api_v01.h"
#include "sns_sam_amd_v01.h"
#include "sns_debug_str.h"
#include "fixed_point.h"
#include "sns_log_api.h"
#include "sns_log_types.h"
#include "sns_smgr_define.h"
#include "sns_smgr_sensor_config.h"

#include <qmi_client.h>
#include <qmi_idl_lib_internal.h>

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

//Gryo sampling rate for gyro bias calibration
#define SNS_SCM_GYRO_CAL_SAMP_RATE_HZ   (25)
//Gyro maximum absolute bias
#define SNS_SCM_GYRO_CAL_MAX_ABS_BIAS   FX_FLTTOFIX_Q16(0.4363)

//QMD instance id when request is pending
#define SNS_SCM_QMD_PEND_ID             (0xFE)

#define SNS_SCM_MAG_CAL

/*---------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
sns_q_s scm_reg_q;
OS_EVENT *scm_mutex;
qmi_client_type sns_scm_smgr_user_handle;
qmi_client_type sns_scm_sam_user_handle;
#ifndef ADSP_STANDALONE
qmi_client_type sns_scm_reg_user_handle;
#endif /* ADSP_STANDALONE */

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
#if defined(SNS_PLAYBACK_SKIP_SMGR) || defined(SNS_QDSP_SIM)
static int sns_scm_pb_gyro_cal_algo_svc_id=0;
static uint32_t sns_scm_pb_report_timeout=1966600; // dsps clock ticks corresponding to 1 min timeout of SCM
static uint32_t sns_scm_pb_next_report_time=0;
static bool sns_scm_pb_update_next_report_time=false;
#endif

//SCM task stack
static OS_STK sns_scm_task_stk[SNS_SCM_MODULE_STK_SIZE];

//SCM event signal
static OS_FLAG_GRP *sns_scm_sig_event;

//algorithm database
static sns_scm_algo_s* sns_scm_algo_dbase[SNS_SCM_NUM_ALGO_SVCS];

//algorithm instance database
static sns_scm_algo_inst_s* sns_scm_algo_inst_dbase[SNS_SCM_MAX_ALGO_INSTS];
static uint8_t sns_scm_algo_inst_count;

//sensor data request database
static sns_scm_data_req_s* sns_scm_data_req_dbase[SNS_SCM_MAX_DATA_REQS];
static uint8_t sns_scm_data_req_count;

//sensor database
static sns_scm_sensor_s sns_scm_sensor_dbase[SNS_SCM_MAX_SNS_MON];

static uint8_t sns_scm_qmd_inst_id = SNS_SCM_INVALID_ID;
static uint8_t sns_scm_qmd_state = SNS_SAM_MOTION_UNKNOWN_V01;

static qmi_client_os_params       scm_smgr_os_params;
static qmi_client_os_params       scm_sam_os_params;
#ifndef ADSP_STANDALONE
static qmi_client_os_params       scm_reg_os_params;
#endif /* ADSP_STANDALONE */

static sns_q_s                    scm_sam_smgr_q;

//SMGR Buffering Support flag
static bool sns_scm_smgr_buffering_flag = false;
static bool sns_scm_sensor_report_rate_available = false;

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/*=========================================================================
  FUNCTION:  sns_scm_qmi_client_send_msg_async
  =========================================================================*/
/*!
  @brief    Wrapper function to qmi_client_send_msg_async to prevent memory
            leaks

  @param[i]   user_handle:         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id:              Message ID
  @param[i]   req_c_struct:        Pointer to the request parameters
  @param[i]   req_c_struct_len:    Length of the request buffer
  @param[i]   resp_c_struct:       Pointer to the response parameters
  @param[i]   resp_c_struct_len:   Length of the response buffer
  @param[i]   resp_cb:             Callback function to handle response parameters
  @param[i]   resp_cb_data:        Callback user data
  @param[i]   txn_handle:          Handle used to identify the transaction  

  @return   QMI error type
*/
/*=======================================================================*/
qmi_client_error_type sns_scm_qmi_client_send_msg_async (
   qmi_client_type                 user_handle,
   unsigned int                    msg_id,
   void                            *req_c_struct,
   unsigned int                    req_c_struct_len,
   void                            *resp_c_struct,
   unsigned int                    resp_c_struct_len,
   qmi_client_recv_msg_async_cb    resp_cb,
   void                            *resp_cb_data,
   qmi_txn_handle                  *txn_handle )
{
   qmi_client_error_type qmi_err;

   qmi_err = qmi_client_send_msg_async(user_handle, msg_id,
                                      req_c_struct, req_c_struct_len,
                                      resp_c_struct, resp_c_struct_len,
                                      resp_cb, resp_cb_data, txn_handle);

   if (QMI_NO_ERR != qmi_err)
   {
      /* If QMI returns an error, free response struct and callback data to prevent memory leaks */
      if (resp_c_struct)
      {
         SNS_OS_FREE(resp_c_struct);
      }

      if (resp_cb_data)
      {
         SNS_OS_FREE(resp_cb_data);
      }

      SNS_SCM_PRINTF1(ERROR, "error in sns_scm_qmi_client_send_msg_async: QMI error:%d", qmi_err);
   }

   if (req_c_struct)
   {
      SNS_OS_FREE(req_c_struct);
   }

   return qmi_err;
}

/*=========================================================================
  FUNCTION:  sns_scm_sensor_report_rate_available
  =========================================================================*/
/*!
  @brief    Detects sensor report rate availability in Registry

  @return   'true' if sensor report rate is expected to be available;
            'false' otherwise.
*/
/*=======================================================================*/
bool sns_scm_is_sensor_report_rate_available()
{
   return sns_scm_sensor_report_rate_available;
}

/*=========================================================================
  FUNCTION:  sns_scm_sensor_status_hyst_timer_cb
  =========================================================================*/
/*!
  @brief Callback registered for sensor status timer expiry

  @param[i] argPtr: pointer to argument for callback function

  @return None
*/
/*=======================================================================*/
static void sns_scm_sensor_status_hyst_timer_cb(
   void *argPtr)
{
   uint8_t err;
   uint32_t sensorIndex = (uint32_t)(uintptr_t)argPtr;

   if (sensorIndex < SNS_SCM_MAX_SNS_MON)
   {
      sns_scm_sensor_dbase[sensorIndex].timeout = true;

      sns_os_sigs_post(sns_scm_sig_event,
                       SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG,
                       OS_FLAG_SET,
                       &err);
      if (err != OS_ERR_NONE)
      {
         SNS_SCM_PRINTF1(ERROR, "sensor status timer callback err:%d",err);
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_timer_cb
  =========================================================================*/
/*!
  @brief Callback registered for timer expiry

  @param[i] argPtr: pointer to argument for callback function

  @return None
*/
/*=======================================================================*/
static void sns_scm_timer_cb(
   void *argPtr)
{
   uint8_t err;
   uint32_t algoIndex = (uint32_t)(uintptr_t)argPtr;

   if (algoIndex < SNS_SCM_NUM_ALGO_SVCS &&
       sns_scm_algo_dbase[algoIndex] != NULL)
   {
      sns_scm_algo_dbase[algoIndex]->timeout = true;

      sns_os_sigs_post(sns_scm_sig_event,
                       SNS_SCM_REPORT_TIMER_SIG,
                       OS_FLAG_SET,
                       &err);
      if (err != OS_ERR_NONE)
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_TIMER_CB_SIGNALERR, err);
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_sensor_status_hyst_timer
  =========================================================================*/
/*!
  @brief Register timer for sensor status hysteresis

  @param[i] sensorIndex: Id of sensor in database

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_reg_sensor_status_hyst_timer(
   uint32_t sensorIndex)
{
   sns_err_code_e err;
   sns_scm_sensor_s* sensorPtr = &sns_scm_sensor_dbase[sensorIndex];

   if (sensorPtr->timer == NULL)
   {
      err = sns_em_create_timer_obj(sns_scm_sensor_status_hyst_timer_cb,
                                    (void*)(intptr_t)sensorIndex,
                                    SNS_EM_TIMER_TYPE_ONESHOT,
                                    &(sensorPtr->timer));
   }
   else
   {
      err = sns_em_cancel_timer(sensorPtr->timer);
   }

#if !defined(SNS_PLAYBACK_SKIP_SMGR) && !defined(SNS_QDSP_SIM)
   if (err == SNS_SUCCESS)
   {
      err = sns_em_register_timer(sensorPtr->timer,
                                  sns_em_convert_usec_to_localtick(SNS_SCM_SENSOR_STATUS_HYST_PERIOD_USEC));
      if (err == SNS_SUCCESS)
      {
         SNS_SCM_PRINTF2(MEDIUM, "sensor status hysteresis timer started sensorId:%d, period:%d",
                         sensorPtr->sensorId,SNS_SCM_SENSOR_STATUS_HYST_PERIOD_USEC);
      }
      else
      {
         SNS_SCM_PRINTF3(ERROR, "sensor status hysteresis timer start error sensorId:%d, period:%d, err:%d",
                         sensorPtr->sensorId,SNS_SCM_SENSOR_STATUS_HYST_PERIOD_USEC,err);
      }
   }
#endif

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_timer
  =========================================================================*/
/*!
  @brief Register timer for client reports

  @param[i] algoIndex: algorithm instance id

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_reg_timer(
   uint32_t algoIndex)
{
   sns_err_code_e err;
   sns_scm_algo_s* algoPtr = sns_scm_algo_dbase[algoIndex];

   if(algoPtr->period == 0)
   {
      // timer disabled
      return SNS_SUCCESS;
   }

   if (algoPtr->timer == NULL)
   {
      err = sns_em_create_timer_obj(sns_scm_timer_cb,
                                    (void*)(intptr_t)algoIndex,
                                    SNS_EM_TIMER_TYPE_PERIODIC,
                                    &(algoPtr->timer));
   }
   else
   {
      err = sns_em_cancel_timer(algoPtr->timer);
   }

#if !defined(SNS_PLAYBACK_SKIP_SMGR) && !defined(SNS_QDSP_SIM)
   if (err == SNS_SUCCESS)
   {
      err = sns_em_register_timer(algoPtr->timer,
                                  algoPtr->period);
      if (err == SNS_SUCCESS)
      {
         SNS_SCM_DEBUG2(MEDIUM, DBG_SCM_REG_TIMER_STARTED,
                        algoIndex, algoPtr->period);
      }
      else
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_REG_TIMER_FAILED, err);
      }
   }
#endif

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_dereg_timer
  =========================================================================*/
/*!
  @brief Deregister timer

  @param[i] algoIndex: client request id

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_dereg_timer(
   uint8_t algoIndex)
{
   sns_err_code_e err;

   if (sns_scm_algo_dbase[algoIndex]->timer == NULL)
   {
      return SNS_SUCCESS;
   }

#if !defined(SNS_PLAYBACK_SKIP_SMGR) && !defined(SNS_QDSP_SIM)
   err = sns_em_cancel_timer(sns_scm_algo_dbase[algoIndex]->timer);
   if (err != SNS_SUCCESS)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_DEREG_TIMER_FAILED, err);
   }
#endif

   err = sns_em_delete_timer_obj(sns_scm_algo_dbase[algoIndex]->timer);
   if (err == SNS_SUCCESS)
   {
      sns_scm_algo_dbase[algoIndex]->timer = NULL;
      SNS_SCM_DEBUG1(MEDIUM, DBG_SCM_DEREG_TIMER_DELETED, sns_em_get_timestamp());
   }
   else
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_DEREG_TIMER_FAILED, err);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_find_data_req
  =========================================================================*/
/*!
  @brief Searches the active sensor data request database for an instance
  with the same sensor id and report rate

  @param[i] algoInstId: algorithm instance id
  @param[i] reportRate: report generation rate
  @param[i] sensorReqCnt: number of sensors in data request
  @param[i] sensorReq: sensors for which data is requested

  @return  Index to algorithm instance matching the specified configuration
  SNS_SCM_INVALID_ID if match is not found
*/
/*=======================================================================*/
static uint8_t sns_scm_find_data_req(
   uint8_t algoInstId,
   uint32_t reportRate,
   uint8_t sensorReqCnt,
   sns_scm_sensor_req_s sensorReq[])
{
   uint8_t dataReqId, dataReqCnt;

   for (dataReqId = 0, dataReqCnt = 0;
       dataReqCnt < sns_scm_data_req_count &&
       dataReqId < SNS_SCM_MAX_DATA_REQS;
       dataReqId++)
   {
      if (sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         sns_scm_data_req_s *dataReq = sns_scm_data_req_dbase[dataReqId];

         if (dataReq->sensorCount == sensorReqCnt &&
             dataReq->reportRate == reportRate)
         {
            uint8_t i, j;
            for (j = 0; j < sensorReqCnt; j++)
            {
               //ordered match done on requests
               if (dataReq->sensorDbase[j].sensorId != sensorReq[j].sensorId ||
                   dataReq->sensorDbase[j].dataType != sensorReq[j].dataType ||
                   dataReq->sensorDbase[j].sampleRate != sensorReq[j].sampleRate ||
                   dataReq->sensorDbase[j].sampleQual != sensorReq[j].sampleQual)
               {
                  break;
               }
            }

            //Found matching request
            if (j >= sensorReqCnt)
            {
               //Avoid duplicate
               for (i = 0; i < dataReq->algoInstCount; i++)
               {
                  if (dataReq->algoInstDbase[i] == algoInstId)
                  {
                     return dataReqId;
                  }
               }

               //Add request
               if (i < SNS_SCM_MAX_ALGO_INSTS_PER_DATA_REQ)
               {
                  dataReq->algoInstDbase[i] = algoInstId;
                  dataReq->algoInstCount++;
                  return dataReqId;
               }
            }
         }

         dataReqCnt++;
      }
   }

   return SNS_SCM_INVALID_ID;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_qmd_start_req
  =========================================================================*/
/*!
  @brief Send a request to sensors algorithm manager for QMD

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_send_qmd_start_req(void)
{
   qmi_client_error_type           err;
   sns_sam_qmd_enable_req_msg_v01  *msgReqP;
   sns_sam_qmd_enable_resp_msg_v01 *msgRespP;
   qmi_txn_handle                  txn_handle;

   msgReqP = (sns_sam_qmd_enable_req_msg_v01 *)
             SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_enable_req_msg_v01));
   SNS_ASSERT(msgReqP != NULL);

   msgRespP = (sns_sam_qmd_enable_resp_msg_v01 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_enable_resp_msg_v01));
   SNS_ASSERT(msgRespP != NULL);

   msgReqP->report_period = 0;  //async report
   msgReqP->config_valid = 0;
   msgReqP->notify_suspend_valid = true;
   msgReqP->notify_suspend.proc_type = SNS_PROC_SSC_V01;
   msgReqP->notify_suspend.send_indications_during_suspend = true;

   err = sns_scm_qmi_client_send_msg_async (sns_scm_sam_user_handle, SNS_SAM_AMD_ENABLE_REQ_V01,
                                    msgReqP, sizeof(sns_sam_qmd_enable_req_msg_v01),
                                    msgRespP, sizeof(sns_sam_qmd_enable_resp_msg_v01),
                                    sns_scm_sam_resp_cb, NULL, &txn_handle);

   //avoid duplicate qmd start requests
   sns_scm_qmd_inst_id = SNS_SCM_QMD_PEND_ID;

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_REQ_ALGO_SVC,
                  SNS_SAM_AMD_SVC_ID_V01,
                  SNS_SAM_AMD_ENABLE_REQ_V01,
                  err);

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_qmd_stop_req
  =========================================================================*/
/*!
  @brief Send a request to sensors algorithm manager for QMD

  @return None
*/
/*=======================================================================*/
static void sns_scm_send_qmd_stop_req(void)
{
   qmi_client_error_type            err;
   sns_sam_qmd_disable_req_msg_v01  *msgReqP;
   sns_sam_qmd_disable_resp_msg_v01 *msgRespP;
   qmi_txn_handle                   txn_handle;

   msgReqP = (sns_sam_qmd_disable_req_msg_v01 *)
             SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_disable_req_msg_v01));
   SNS_ASSERT(msgReqP != NULL);

   msgRespP = (sns_sam_qmd_disable_resp_msg_v01 *)
              SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_sam_qmd_disable_resp_msg_v01));
   SNS_ASSERT(msgRespP != NULL);

   msgReqP->instance_id = sns_scm_qmd_inst_id;

   err = sns_scm_qmi_client_send_msg_async (sns_scm_sam_user_handle, SNS_SAM_AMD_DISABLE_REQ_V01,
                                    msgReqP, sizeof(sns_sam_qmd_disable_req_msg_v01),
                                    msgRespP, sizeof(sns_sam_qmd_disable_resp_msg_v01),
                                    sns_scm_sam_resp_cb, NULL, &txn_handle);

   //reset QMD instance id and state
   sns_scm_qmd_inst_id = SNS_SCM_INVALID_ID;
   sns_scm_qmd_state = SNS_SAM_MOTION_UNKNOWN_V01;

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_REQ_ALGO_SVC,
                  SNS_SAM_AMD_SVC_ID_V01,
                  SNS_SAM_AMD_DISABLE_REQ_V01,
                  err);
}

/*=========================================================================
  FUNCTION:  sns_scm_get_smgr_msg_req_type
  =========================================================================*/
/*!
  @brief  Determines the type of message request used to communicate with
          Sensor Manager.

  @param[i] algoSvcId: algorithm service ID

  @return  message request ID
*/
/*=======================================================================*/
uint8_t sns_scm_get_smgr_msg_req_type(
   uint8_t algoSvcId)
{
   uint8_t buff_flag = SNS_SMGR_REPORT_REQ_V01;  // default

   if(!sns_scm_smgr_buffering_flag)
   {
      SNS_SCM_PRINTF0(MED, "SMGR does not support buffering" );
   }
   else
   {
      if(algoSvcId == SNS_SCM_GYRO_CAL_SVC )
      {
         sns_scm_algo_s* algoPtr = sns_scm_get_algo_handle(algoSvcId);
         if( algoPtr != NULL && algoPtr->defSensorReportRate != 0 )
         {
            buff_flag =  SNS_SMGR_BUFFERING_REQ_V01;
         }
      }
      else if(algoSvcId == SNS_SCM_MAG_CAL_SVC )
      {
         /* Always use buffering */
         buff_flag =  SNS_SMGR_BUFFERING_REQ_V01;
      }
   }

   SNS_SCM_PRINTF1(MED, "Returning buff flag %d", buff_flag);
   return (buff_flag);
}

/*=========================================================================
  FUNCTION:  sns_scm_send_smgr_start_req
  =========================================================================*/
/*!
  @brief Send a request to sensors manager for sensor data

  @param[i] dataReqId: Index of data request in database
  @param[i] svcId: Service Id

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_send_smgr_start_req(
   uint8_t dataReqId,
   uint8_t svcId)
{
   if(sns_scm_get_smgr_msg_req_type(svcId) == SNS_SMGR_BUFFERING_REQ_V01)
   {
      sns_smgr_buffering_req_msg_v01 *msgPtr = NULL;
      uint8_t i;
      qmi_client_error_type err;
      sns_smgr_buffering_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle txn_handle;

      msgPtr   = (sns_smgr_buffering_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgRespP = (sns_smgr_buffering_resp_msg_v01 *)
                 SNS_OS_MALLOC (SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr->ReportId = dataReqId;
      msgPtr->ReportRate = sns_scm_data_req_dbase[dataReqId]->reportRate;
      msgPtr->Action = SNS_SMGR_BUFFERING_ACTION_ADD_V01;
      msgPtr->Item_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;
      for(i = 0; i < msgPtr->Item_len; i++)
      {
         msgPtr->Item[i].SensorId =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sensorId;
         msgPtr->Item[i].DataType =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].dataType;
         msgPtr->Item[i].SamplingRate =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate;
         msgPtr->Item[i].SampleQuality =
            sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleQual;
         msgPtr->Item[i].Decimation = SNS_SMGR_DECIMATION_FILTER_V01;
         msgPtr->Item[i].Calibration = SNS_SMGR_CAL_SEL_FACTORY_CAL_V01;
      }

      msgPtr->notify_suspend_valid = false;
      msgPtr->SrcModule_valid = true;
      msgPtr->SrcModule = SNS_SCM_MODULE;

      err = sns_scm_qmi_client_send_msg_async (sns_scm_smgr_user_handle, SNS_SMGR_BUFFERING_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_buffering_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_buffering_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_PRINTF3(MEDIUM, "Sensor Buffering data req %d to SMGR for %d sensors for algo SVC %d",
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     svcId);

      return err;
   }
   else
   {
      sns_smgr_periodic_report_req_msg_v01 *msgPtr = NULL;
      uint8_t i;
      qmi_client_error_type                 err;
      sns_smgr_periodic_report_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle                        txn_handle;

      msgPtr   = (sns_smgr_periodic_report_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgRespP = (sns_smgr_periodic_report_resp_msg_v01 *)
                 SNS_OS_MALLOC (SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr->BufferFactor = 1;

      msgPtr->ReportId = dataReqId;
      msgPtr->Action = SNS_SMGR_REPORT_ACTION_ADD_V01;

      msgPtr->ReportRate = sns_scm_data_req_dbase[dataReqId]->reportRate;

      msgPtr->cal_sel_valid = true;
      msgPtr->cal_sel_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;

      msgPtr->Item_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;
      for (i = 0; i < sns_scm_data_req_dbase[dataReqId]->sensorCount; i++)
      {
         msgPtr->Item[i].SensorId =
         sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sensorId;
         msgPtr->Item[i].DataType =
         sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].dataType;

         msgPtr->Item[i].Decimation = SNS_SMGR_DECIMATION_FILTER_V01;

         msgPtr->cal_sel[i] = SNS_SMGR_CAL_SEL_FACTORY_CAL_V01;
      }

      msgPtr->notify_suspend_valid = false;
      msgPtr->SrcModule_valid = true;
      msgPtr->SrcModule = SNS_SCM_MODULE;

      err = sns_scm_qmi_client_send_msg_async (sns_scm_smgr_user_handle, SNS_SMGR_REPORT_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_periodic_report_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_periodic_report_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_PRINTF3(MEDIUM, "Sensor Periodic data req %d to SMGR for %d sensors for algo SVC %d",
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     sns_scm_data_req_dbase[dataReqId]->sensorDbase[0].sensorId);

      return err;
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_find_max_sample_rate
  =========================================================================*/
/*!
  @brief Finds max sample rate in a data request

  @param[i] dataReqId: Index of data request in data request database

  @return sample rate in Hz (Q16 format)
*/
/*=======================================================================*/
static int32_t sns_scm_find_max_sample_rate(
   const uint8_t dataReqId)
{
   int32_t maxSampleRate = 0;

   if( dataReqId < SNS_SCM_MAX_DATA_REQS &&
       sns_scm_data_req_dbase[dataReqId] != NULL )
   {
      int i;
      for( i = 0; i < sns_scm_data_req_dbase[dataReqId]->sensorCount; i++ )
      {
         if( sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate >
             maxSampleRate )
         {
            maxSampleRate =
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate;
         }
      }
   }
   return FX_CONV_Q16(maxSampleRate,0);
}

/*=========================================================================
  FUNCTION:  sns_scm_switch_smgr_data_req
  =========================================================================*/
/*!
  @brief Switch between buffering and periodic data requests to SMGR

  @param[i] dataReqId: Index of data request in data request database

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_switch_smgr_data_req(
   const uint8_t dataReqId)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   if( dataReqId < SNS_SCM_MAX_DATA_REQS &&
       sns_scm_data_req_dbase[dataReqId] != NULL &&
       sns_scm_data_req_dbase[dataReqId]->algoInstCount > 0 )
   {
      uint8_t algoInstId, svcId = 0;

      sns_scm_data_req_dbase[dataReqId]->reportRate =
          sns_scm_find_max_sample_rate(dataReqId);

      // Find one service associated with this data request
      algoInstId = sns_scm_data_req_dbase[dataReqId]->algoInstDbase[0];
      if( algoInstId < SNS_SCM_MAX_ALGO_INSTS &&
          sns_scm_algo_inst_dbase[algoInstId] != NULL )
      {
         sns_scm_algo_inst_s * algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];
         svcId = sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId;

         // Send request to switch to new message type
         SNS_SCM_PRINTF2(MED, "Switching data req %d for service %d", dataReqId, svcId);
         err = sns_scm_send_smgr_start_req( dataReqId, svcId );
      }
   }
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_send_smgr_stop_req
  =========================================================================*/
/*!
  @brief Send a request to sensors manager to stop sensor data received by
  specified algorithm

  @param[i] dataReqId: Index of data request in database
  @param[i] svcId: Service Id

  @return None
*/
/*=======================================================================*/
static void sns_scm_send_smgr_stop_req(
   uint8_t dataReqId,
   uint8_t svcId)
{
   if(sns_scm_get_smgr_msg_req_type(svcId) == SNS_SMGR_BUFFERING_REQ_V01)
   {
      qmi_client_error_type            err;
      sns_smgr_buffering_req_msg_v01  *msgReqP = NULL;
      sns_smgr_buffering_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle                   txn_handle;

      msgReqP  = (sns_smgr_buffering_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_req_msg_v01));
      SNS_ASSERT(msgReqP != NULL);

      msgRespP = (sns_smgr_buffering_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_buffering_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgReqP->ReportId = dataReqId;
      msgReqP->Action = SNS_SMGR_BUFFERING_ACTION_DELETE_V01;
      msgReqP->Item_len = 0;
      msgReqP->notify_suspend_valid = false;
      msgReqP->SrcModule_valid = true;
      msgReqP->SrcModule = SNS_SCM_MODULE;

      err = sns_scm_qmi_client_send_msg_async (sns_scm_smgr_user_handle, SNS_SMGR_BUFFERING_REQ_V01,
                                       msgReqP, sizeof(sns_smgr_buffering_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_buffering_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_STOP_SNSR_DATA,
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     sns_scm_data_req_dbase[dataReqId]->sensorDbase[0].sensorId);
   }
   else
   {
      qmi_client_error_type                 err;
      sns_smgr_periodic_report_req_msg_v01  *msgReqP = NULL;
      sns_smgr_periodic_report_resp_msg_v01 *msgRespP = NULL;
      qmi_txn_handle                        txn_handle;

      msgReqP  = (sns_smgr_periodic_report_req_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_req_msg_v01));
      SNS_ASSERT(msgReqP != NULL);

      msgRespP = (sns_smgr_periodic_report_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_periodic_report_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgReqP->Item_len = sns_scm_data_req_dbase[dataReqId]->sensorCount;
      msgReqP->BufferFactor = 1;

      msgReqP->ReportId = dataReqId;
      msgReqP->Action = SNS_SMGR_REPORT_ACTION_DELETE_V01;
      msgReqP->SrcModule_valid = true;
      msgReqP->SrcModule = SNS_SCM_MODULE;

      err = sns_scm_qmi_client_send_msg_async (sns_scm_smgr_user_handle, SNS_SMGR_REPORT_REQ_V01,
                                       msgReqP, sizeof(sns_smgr_periodic_report_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_periodic_report_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_STOP_SNSR_DATA,
                     dataReqId,
                     sns_scm_data_req_dbase[dataReqId]->sensorCount,
                     sns_scm_data_req_dbase[dataReqId]->sensorDbase[0].sensorId);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_start_sensor_data
  =========================================================================*/
/*!
  @brief Request sensor data according to the algorithm needs

  @param[i] algoInstId: algorithm instance id
  @param[i] reportRate: report generation rate
  @param[i] sensorReqCnt: number of sensors in data request
  @param[i] sensorReq: sensors for which data is requested

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_start_sensor_data(
   uint8_t algoInstId,
   uint32_t reportRate,
   uint8_t sensorReqCnt,
   sns_scm_sensor_req_s sensorIndex[])
{
   sns_err_code_e err = SNS_ERR_FAILED;
   uint8_t dataReqId, i;
   sns_scm_algo_inst_s * algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

   //check if there is an identical existing request
   dataReqId = sns_scm_find_data_req(algoInstId,
                                     reportRate,
                                     sensorReqCnt,
                                     sensorIndex);

   //add request if none exists
   if (dataReqId >= SNS_SCM_MAX_DATA_REQS)
   {
      if (sns_scm_data_req_count >= SNS_SCM_MAX_DATA_REQS)
      {
         return err;
      }

      for (dataReqId = 0; dataReqId < SNS_SCM_MAX_DATA_REQS; dataReqId++)
      {
         if (sns_scm_data_req_dbase[dataReqId] == NULL)
         {
            sns_scm_data_req_dbase[dataReqId] =
            SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_scm_data_req_s));
            SNS_ASSERT(sns_scm_data_req_dbase[dataReqId] != NULL);

            for (i = 0; i < SNS_SCM_MAX_ALGO_INSTS_PER_DATA_REQ; i++)
            {
               sns_scm_data_req_dbase[dataReqId]->algoInstDbase[i] =
               SNS_SCM_INVALID_ID;
            }
            sns_scm_data_req_dbase[dataReqId]->reportRate = reportRate;
            for (i = 0; i < sensorReqCnt; i++)
            {
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sensorId = sensorIndex[i].sensorId;
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].dataType = sensorIndex[i].dataType;
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleRate = sensorIndex[i].sampleRate;
               sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sampleQual = sensorIndex[i].sampleQual;
            }
            sns_scm_data_req_dbase[dataReqId]->sensorCount = sensorReqCnt;
            sns_scm_data_req_dbase[dataReqId]->algoInstDbase[0] = algoInstId;
            sns_scm_data_req_dbase[dataReqId]->algoInstCount = 1;

            //send message to sensors manager requesting required sensor data
            err = sns_scm_send_smgr_start_req(dataReqId,
                sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId);

            sns_scm_data_req_count++;

            break;
         }
      }
   }
   else
   {
      SNS_SCM_PRINTF0(LOW, "Request already exists, not adding a new one");
      err = SNS_SUCCESS;
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_stop_sensor_data
  =========================================================================*/
/*!
  @brief Stop sensor data received for specified algorithm instance

  @param[i] algoInstId: Index of algorithm instance in database

  @return None
*/
/*=======================================================================*/
static void sns_scm_stop_sensor_data(
   uint8_t algoInstId)
{
   uint8_t dataReqId, dataReqCnt, i;

   for (dataReqId = 0, dataReqCnt = 0;
       dataReqCnt < sns_scm_data_req_count &&
       dataReqId < SNS_SCM_MAX_DATA_REQS;
       dataReqId++)
   {
      if (sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         sns_scm_data_req_s *dataReq = sns_scm_data_req_dbase[dataReqId];
         for (i = 0; i < dataReq->algoInstCount; i++)
         {
            if (dataReq->algoInstDbase[i] == algoInstId)
            {
               dataReq->algoInstCount--;
               dataReq->algoInstDbase[i] =
               dataReq->algoInstDbase[dataReq->algoInstCount];
               dataReq->algoInstDbase[dataReq->algoInstCount] =
               SNS_SCM_INVALID_ID;

               if (dataReq->algoInstCount == 0)
               {
                  sns_scm_algo_inst_s * algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];
                  sns_scm_send_smgr_stop_req(dataReqId,sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId);
                  SNS_OS_FREE(sns_scm_data_req_dbase[dataReqId]);
                  sns_scm_data_req_dbase[dataReqId] = NULL;

                  sns_scm_data_req_count--;
                  break;
               }
            }
         }
         dataReqCnt++;
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_req_sensor_data
  =========================================================================*/
/*!
  @brief Request sensor data for algorithm

  @param[i] algoInstId: algorithm instance id

  @return None
*/
/*=======================================================================*/
static void sns_scm_req_sensor_data(
   uint8_t algoInstId)
{
   uint8_t sensorCnt = 0;
   uint32_t reportRate = 0;
   sns_scm_sensor_req_s sensorReq[SNS_SCM_MAX_SENSORS_PER_DATA_REQ];
   uint8_t algoIndex = sns_scm_algo_inst_dbase[algoInstId]->algoIndex;
   int i;

   SNS_SCM_PRINTF2(LOW, "Requesting sensor data for algoInst %d, %d",
                   algoInstId, 0);

   for( i = 0; i < SNS_SCM_MAX_SENSORS_PER_DATA_REQ; i++ )
   {
      sensorReq[i].sampleRate = SNS_SCM_INVALID_SAMPLE_RATE;
      sensorReq[i].sampleQual = SNS_SCM_DEFAULT_SAMPLE_QUALITY;
   }

   if (sns_scm_algo_dbase[algoIndex]->serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      gyro_cal_config_s *cfgPtr = (gyro_cal_config_s *)
         (sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr);

       sensorReq[0].sensorId =
          sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].sensorId;
      sensorReq[0].dataType =
          sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].dataType;
      sensorCnt = 1;
      if( sns_scm_get_smgr_msg_req_type(SNS_SCM_GYRO_CAL_SVC) !=
          SNS_SMGR_BUFFERING_REQ_V01 )
      {
         reportRate = FX_CONV(cfgPtr->sampleRate, 16, 0);
      }
      else
      {
         reportRate = sns_scm_algo_dbase[algoIndex]->defSensorReportRate;
         sensorReq[0].sampleRate = FX_CONV(cfgPtr->sampleRate, 16, 0);
      }
   }
   else if (sns_scm_algo_dbase[algoIndex]->serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      qmag_cal_config_s *cfgPtr = (qmag_cal_config_s *)
         (sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr);

      sensorReq[0].sensorId = SNS_SMGR_ID_MAG_V01;
      sensorReq[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      sensorReq[0].sampleRate = FX_CONV(cfgPtr->sample_rate_mag, 16, 0);
          
      sensorReq[1].sensorId = SNS_SMGR_ID_ACCEL_V01;
      sensorReq[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      sensorReq[1].sampleRate = FX_CONV(cfgPtr->sample_rate_accel, 16, 0);

      sensorCnt = 2;
      if( sns_scm_get_smgr_msg_req_type(SNS_SCM_MAG_CAL_SVC) !=
          SNS_SMGR_BUFFERING_REQ_V01 )
      {
         reportRate = FX_CONV(cfgPtr->sample_rate_mag, 16, 0);
      }
      else
      {
         reportRate = sns_scm_algo_dbase[algoIndex]->defSensorReportRate;
         sensorReq[0].sampleRate = FX_CONV(cfgPtr->sample_rate_mag, 16, 0);
      }
   }
   else
   {
      SNS_SCM_PRINTF2(LOW, "Cannot Handle sensor algo %d, %d",
                      sns_scm_algo_dbase[algoIndex]->serviceId, 0);

   }

   //send requests for sensor data to sensor manager
   if (sensorCnt > 0)
   {
      SNS_SCM_PRINTF2(LOW, "Requesting sensor data at %d Hz (Q16) for svc %d",
          reportRate, sns_scm_algo_dbase[algoIndex]->serviceId );
      sns_scm_start_sensor_data(algoInstId, reportRate, sensorCnt, sensorReq);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_log_gyro_cal_config
  =========================================================================*/
/*!
  @brief Log gyro calibration configuration

  @param[i] algoInstId: algorithm instance id

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_log_gyro_cal_config(
   uint8_t algoInstId)
{
   sns_err_code_e err;
   sns_log_gyro_cal_config_s* logPtr;
   gyro_cal_config_s* algoCfgPtr =
   sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_CONFIG,
                           sizeof(sns_log_gyro_cal_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_GYRO_CAL_CONFIG_VERSION;

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sampleRate;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GYRO_CAL_CONFIG, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_gyro_cal_result
  =========================================================================*/
/*!
  @brief Log gyro calibration result

  @param[i] algoInstId: algorithm instance id
  @param[i] timestamp: timestamp on the algorithm input data

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_log_gyro_cal_result(
   uint8_t algoInstId,
   uint32_t timestamp)
{
   sns_err_code_e err;
   sns_log_gyro_cal_result_s* logPtr;
   gyro_cal_input_s* algoInPtr =
   sns_scm_algo_inst_dbase[algoInstId]->inputData.memPtr;
   gyro_cal_output_s* algoOutPtr =
   sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_RESULT,
                           sizeof(sns_log_gyro_cal_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_GYRO_CAL_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      logPtr->gyro_x = algoInPtr->data[0];
      logPtr->gyro_y = algoInPtr->data[1];
      logPtr->gyro_z = algoInPtr->data[2];
      logPtr->gyro_zero_bias_x = algoOutPtr->bias[0];
      logPtr->gyro_zero_bias_y = algoOutPtr->bias[1];
      logPtr->gyro_zero_bias_z = algoOutPtr->bias[2];

      //Timestamp
      logPtr->timestamp = timestamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GYRO_CAL_RESULT, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_qmag_cal_config
  =========================================================================*/
/*!
  @brief Log mag calibration configuration

  @param[i] algoInstId: algorithm instance id

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_log_qmag_cal_config(
   uint8_t algoInstId)
{
   sns_err_code_e err;
   sns_log_qmag_cal_config_s* logPtr;
   qmag_cal_config_s* algoCfgPtr = 
   sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_CONFIG,
                           sizeof(sns_log_qmag_cal_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_QMAG_CAL_CONFIG_VERSION;
      logPtr->sample_rate_mag = algoCfgPtr->sample_rate_mag;
	  logPtr->sample_rate_accel = algoCfgPtr->sample_rate_accel;

	  logPtr->cal_algo_type = 0;
	  logPtr->cal_cycle_rate = 2.0;
      //Timestamp
	  logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = sns_em_get_timestamp();

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GYRO_CAL_CONFIG, logPtr);
   }
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_qmag_cal_result
  =========================================================================*/
/*!
  @brief Log mag calibration result

  @param[i] algoInstId: algorithm instance id
  @param[i] timestamp: timestamp on the algorithm input data

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_log_qmag_cal_result(
   uint8_t algoInstId,
   uint32_t timestamp)
{

   sns_err_code_e err;
   sns_log_qmag_cal_result_s* logPtr; 

   qmag_cal_input_s* algoInPtr =
   sns_scm_algo_inst_dbase[algoInstId]->inputData.memPtr;

   qmag_cal_state_s* algoStatPtr =
   sns_scm_algo_inst_dbase[algoInstId]->stateData.memPtr;

   qmag_cal_output_s* algoOutPtr =
   sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_RESULT,
                           sizeof(sns_log_qmag_cal_result_s),
                           (void**)&logPtr);


   //below is yet to be changed
   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_QMAG_CAL_RESULT_VERSION;
     
      logPtr->input_type =  algoInPtr->input_type;

      if (algoInPtr->input_type == QMAG_CAL_INPUT_TYPE_MAG) 
      {
         logPtr->input_mag_x = algoInPtr->sensor_data[0]; 
         logPtr->input_mag_y = algoInPtr->sensor_data[1];
         logPtr->input_mag_z = algoInPtr->sensor_data[2];
      }else if(algoInPtr->input_type == QMAG_CAL_INPUT_TYPE_ACCEL) 
      {
         logPtr->input_accel_x = algoInPtr->sensor_data[0]; 
         logPtr->input_accel_y = algoInPtr->sensor_data[1];
         logPtr->input_accel_z = algoInPtr->sensor_data[2];
      }

      logPtr-> accel_input_variance = algoStatPtr->accel_raw_variance;
      logPtr-> mag_input_variance = algoStatPtr->mag_raw_variance;
      logPtr-> offset_HI_unfiltered_x = algoStatPtr->offset_HI[0];
      logPtr-> offset_HI_unfiltered_y = algoStatPtr->offset_HI[1];
      logPtr-> offset_HI_unfiltered_z = algoStatPtr->offset_HI[2];
      logPtr-> radius_HI  = algoStatPtr->radius_HI;
      logPtr-> unfiltered_accuracy = algoStatPtr ->accuracy_HI;
      logPtr->accuracy_published = algoStatPtr->persistent_state.published_accuracy_HI;

      logPtr->cal_algo_type = 0;
      logPtr->mag_zero_bias_x = algoOutPtr->offset_HI[0];
      logPtr->mag_zero_bias_y = algoOutPtr->offset_HI[1];
      logPtr->mag_zero_bias_z = algoOutPtr->offset_HI[2];

      logPtr->offset_time = algoOutPtr->offset_time;

      //Timestamp
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = timestamp;

      logPtr -> soft_reset = 0;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GYRO_CAL_RESULT, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_log_algo_config
  =========================================================================*/
/*!
  @brief Log algorithm configuration

  @param[i] algoInstId: algorithm instance id

  @return None
*/
/*=======================================================================*/
static void sns_scm_log_algo_config(
   uint8_t algoInstId)
{
   sns_err_code_e err = SNS_SUCCESS;
   uint8_t serviceId =
   sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
   serviceId;

   if (serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      err = sns_scm_log_gyro_cal_config(algoInstId);
   }else if (serviceId == SNS_SCM_MAG_CAL_SVC) 
   {
      err = sns_scm_log_qmag_cal_config(algoInstId);
   }

   if (err != SNS_SUCCESS && err != SNS_ERR_NOTALLOWED)
   {
      SNS_SCM_DEBUG3(ERROR,DBG_SCM_LOG_CONFIG_ERR, serviceId, algoInstId, err);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_log_algo_result
  =========================================================================*/
/*!
  @brief Log algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] timestamp: timestamp on the algorithm input data

  @return None
*/
/*=======================================================================*/
static void sns_scm_log_algo_result(
   uint8_t algoInstId,
   uint32_t timestamp)
{
   sns_err_code_e err = SNS_SUCCESS;
   uint8_t serviceId =
   sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
   serviceId;

   if (serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      err = sns_scm_log_gyro_cal_result(algoInstId, timestamp);
   }else if(serviceId == SNS_SCM_MAG_CAL_SVC) 
   {
	  
      err = sns_scm_log_qmag_cal_result(algoInstId, timestamp);
   }

   if (err != SNS_SUCCESS && err != SNS_ERR_NOTALLOWED)
   {
      SNS_SCM_DEBUG3(ERROR,DBG_SCM_LOG_RESULT_ERR, serviceId, algoInstId, err);
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_find_algo_inst
  =========================================================================*/
/*!
  @brief Searches the active algorithm database for an instance
  of an algorithm with specified configuration

  @param[i] algoSvcId: algorithm service id
  @param[i] algoCfgPtr: pointer to configuration of specified algorithm

  @return  Index to algorithm instance matching the specified configuration
  if successful, SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
static uint8_t sns_scm_find_algo_inst(
   uint8_t algoSvcId,
   void *algoCfgPtr)
{
   uint8_t algoInstId, algoInstCnt;

   for (algoInstId = 0, algoInstCnt = 0;
       algoInstCnt < sns_scm_algo_inst_count &&
       algoInstId < SNS_SCM_MAX_ALGO_INSTS;
       algoInstId++)
   {
      sns_scm_algo_inst_s *algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];
      if (algoInstPtr == NULL)
      {
         continue;
      }

      if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == algoSvcId)
      {
         if (!SNS_OS_MEMCMP(algoCfgPtr,
                            algoInstPtr->configData.memPtr,
                            algoInstPtr->configData.memSize))
         {
            return algoInstId;
         }
      }
      algoInstCnt++;
   }
   return SNS_SCM_INVALID_ID;
}

/*=========================================================================
  FUNCTION:  sns_scm_get_algo_inst
  =========================================================================*/
/*!
  @brief
  If an instance of the specified algorithm doesnt exist,
  creates an instance and initializes the reference count.
  If an instance with identical configuration exists,
  updates the reference count.

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return index of the algorithm instance in database if successful,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
static uint8_t sns_scm_get_algo_inst(
   uint8_t algoIndex)
{
   uint8_t algoSvcId = sns_scm_algo_dbase[algoIndex]->serviceId;

   void *algoCfgPtr = sns_scm_algo_dbase[algoIndex]->defConfigData.memPtr;

   uint8_t algoInstId = sns_scm_find_algo_inst(algoSvcId, algoCfgPtr);

   if (algoInstId >= SNS_SCM_MAX_ALGO_INSTS)
   {
      uint8_t instId;

      //check if number of algo instances already reaches max
      if (sns_scm_algo_inst_count >= SNS_SCM_MAX_ALGO_INSTS)
      {
         return SNS_SCM_INVALID_ID;
      }

      for (instId = 0; instId < SNS_SCM_MAX_ALGO_INSTS; instId++)
      {
         if (sns_scm_algo_inst_dbase[instId] == NULL)
         {
            sns_scm_algo_inst_s* algoInstPtr;
            void *statePtr = NULL;
            uint32_t memSize = sizeof(sns_scm_algo_inst_s);

            //create algorithm instance
            sns_scm_algo_inst_dbase[instId] = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
            SNS_ASSERT(sns_scm_algo_inst_dbase[instId] != NULL);
            algoInstPtr = sns_scm_algo_inst_dbase[instId];

            //initialize the algorithm instance
            algoInstPtr->configData.memPtr = NULL;
            algoInstPtr->stateData.memPtr = NULL;
            algoInstPtr->inputData.memPtr = NULL;
            algoInstPtr->outputData.memPtr = NULL;

            memSize = sns_scm_algo_dbase[algoIndex]->defConfigData.memSize;
            if (memSize > 0)
            {
               algoInstPtr->configData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
               SNS_ASSERT(algoInstPtr->configData.memPtr != NULL);
               SNS_OS_MEMCOPY(algoInstPtr->configData.memPtr,
                              algoCfgPtr, memSize);
            }
            algoInstPtr->configData.memSize = memSize;

            memSize = sns_scm_algo_dbase[algoIndex]->algoApi.
                      sns_scm_algo_mem_req(algoCfgPtr);

            //Algorithm indicates configuration error with size 0
            if (memSize == 0)
            {
               SNS_OS_FREE(algoInstPtr->configData.memPtr);
               algoInstPtr->configData.memPtr = NULL;
               SNS_OS_FREE(sns_scm_algo_inst_dbase[instId]);
               sns_scm_algo_inst_dbase[instId] = NULL;
               return SNS_SCM_INVALID_ID;
            }

            algoInstPtr->stateData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
            SNS_ASSERT(algoInstPtr->stateData.memPtr != NULL);
            SNS_OS_MEMZERO(algoInstPtr->stateData.memPtr, memSize);
            algoInstPtr->stateData.memSize = memSize;

            memSize = sns_scm_algo_dbase[algoIndex]->defInputDataSize;
            if (memSize > 0)
            {
               algoInstPtr->inputData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
               SNS_ASSERT(algoInstPtr->inputData.memPtr != NULL);
               SNS_OS_MEMZERO(algoInstPtr->inputData.memPtr, memSize);
            }
            algoInstPtr->inputData.memSize = memSize;

            memSize = sns_scm_algo_dbase[algoIndex]->defOutputDataSize;
            if (memSize > 0)
            {
               algoInstPtr->outputData.memPtr = SNS_OS_MALLOC(SNS_SCM_DBG_MOD, memSize);
               SNS_ASSERT(algoInstPtr->outputData.memPtr != NULL);
               SNS_OS_MEMZERO(algoInstPtr->outputData.memPtr, memSize);
            }
            algoInstPtr->outputData.memSize = memSize;

            statePtr = sns_scm_algo_dbase[algoIndex]->algoApi.
                       sns_scm_algo_reset(algoInstPtr->configData.memPtr,
                                          algoInstPtr->stateData.memPtr);
            if (statePtr == NULL)
            {
               SNS_SCM_DEBUG0(ERROR, DBG_SCM_ENABLE_ALGO_STATE_NULL);
            }

            algoInstPtr->algoIndex = algoIndex;

            algoInstId = instId;

            sns_scm_algo_inst_count++;

            //request sensor data
            sns_scm_req_sensor_data(algoInstId);

            //Log algorithm configuration
            sns_scm_log_algo_config(algoInstId);

            SNS_SCM_DEBUG3(LOW, DBG_SCM_ALGO_MEM_INFO,
                           sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId,
                           sizeof(sns_scm_algo_inst_s),
                           algoInstPtr->configData.memSize);
            SNS_SCM_DEBUG3(LOW, DBG_SCM_ALGO_STATE_MEM_INFO,
                           algoInstPtr->stateData.memSize,
                           algoInstPtr->inputData.memSize,
                           algoInstPtr->outputData.memSize);

            break;
         }
      }
   }

   return algoInstId;
}

/*=========================================================================
  FUNCTION:  sns_scm_delete_algo_inst
  =========================================================================*/
/*!
  @brief Deletes the specified algorithm instance

  @param[i] algoInstId: index to algorithm instance in database

  @return None
*/
/*=======================================================================*/
static void sns_scm_delete_algo_inst(
   uint8_t algoInstId)
{
   sns_scm_algo_inst_s* algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

   //free memory reserved for this algorithm instance
   SNS_OS_FREE(algoInstPtr->configData.memPtr);
   algoInstPtr->configData.memPtr = NULL;
   algoInstPtr->configData.memSize = 0;

   SNS_OS_FREE(algoInstPtr->stateData.memPtr);
   algoInstPtr->stateData.memPtr = NULL;
   algoInstPtr->stateData.memSize = 0;

   SNS_OS_FREE(algoInstPtr->inputData.memPtr);
   algoInstPtr->inputData.memPtr = NULL;
   algoInstPtr->inputData.memSize = 0;

   SNS_OS_FREE(algoInstPtr->outputData.memPtr);
   algoInstPtr->outputData.memPtr = NULL;
   algoInstPtr->outputData.memSize = 0;

   SNS_OS_FREE(sns_scm_algo_inst_dbase[algoInstId]);
   sns_scm_algo_inst_dbase[algoInstId] = NULL;

   sns_scm_algo_inst_count--;
}

/*=========================================================================
  FUNCTION:  sns_scm_enable_algo
  =========================================================================*/
/*!
  @brief
  enables specified algorithm with the specified configuration

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return index of the algorithm instance in database if successful,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
static uint8_t sns_scm_enable_algo(
   uint8_t algoIndex)
{
   uint8_t algoInstId;

   algoInstId = sns_scm_get_algo_inst(algoIndex);
   if (algoInstId >= SNS_SCM_MAX_ALGO_INSTS ||
       sns_scm_algo_inst_dbase[algoInstId] == NULL)
   {
      return SNS_SCM_INVALID_ID;
   }

   if (sns_scm_algo_dbase[algoIndex]->timer == NULL)
   {
      sns_scm_reg_timer(algoIndex);
   }

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_ENABLE_ALGO,
                  sns_scm_algo_dbase[algoIndex]->serviceId,
                  sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].sensorId,
                  algoInstId);

   return algoInstId;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_auto_cal_params_registry
  =========================================================================*/
/*!
  @brief Stores last estimated auto cal parameters from SCM algo dbase to registry

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_store_auto_cal_params_registry(
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_scm_algo_s* algoPtr;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoPtr = sns_scm_algo_dbase[algoIndex];

   if (algoPtr->serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      err = sns_scm_store_gyro_auto_cal_params_registry(algoPtr->persistAutoCalParam.memPtr);
   }
   else if (algoPtr->serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      err = sns_scm_store_qmag_auto_cal_params_registry(algoPtr->persistAutoCalParam.memPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_auto_cal_state_registry
  =========================================================================*/
/*!
  @brief Store auto cal algorithm state in registry
 
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_store_auto_cal_state_registry(
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_scm_algo_s* algoPtr;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoPtr = sns_scm_algo_dbase[algoIndex];
   
   if (algoPtr->serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      err = sns_scm_store_qmag_auto_cal_state_registry(algoPtr->defConfigData.memPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_store_auto_cal_params_algodbase
  =========================================================================*/
/*!
  @brief Stores the auto cal param and state to scm algo database

  @param[i] algoIndex: index to algorithm in the algorithm database
  @param[i] algoInstId: index to algorithm instance in the database

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_store_auto_cal_params_algodbase(
   uint8_t algoIndex, uint8_t algoInstId)
{
   sns_scm_algo_s *algoPtr;
   sns_scm_algo_inst_s * algoInstPtr;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoPtr = sns_scm_algo_dbase[algoIndex];

   if (algoInstId >= SNS_SCM_MAX_ALGO_INSTS ||
       sns_scm_algo_inst_dbase[algoInstId] == NULL)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_DISABLE_ALGO_INSTANCE_ERR, algoInstId);
      return SNS_ERR_FAILED;
   }

   algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

   if (algoPtr->persistAutoCalParam.memPtr != NULL)
   {
      if ( algoPtr->serviceId == SNS_SCM_MAG_CAL_SVC)
      {
         qmag_cal_output_s *qmagOpAlgoInstPtr = (qmag_cal_output_s *)(algoInstPtr->outputData.memPtr);
         qmag_cal_output_s *qmagOpAlgoDBPtr = (qmag_cal_output_s *)(algoPtr->persistAutoCalParam.memPtr);

         //skip storing to algo dbase if calibration did not occur
         if (qmagOpAlgoInstPtr->offset_HI[0] != 0 && qmagOpAlgoInstPtr->offset_HI[1] != 0 && qmagOpAlgoInstPtr->offset_HI[2] != 0)
         {
            SNS_OS_MEMCOPY(algoPtr->persistAutoCalParam.memPtr,
                           algoInstPtr->outputData.memPtr,
                           algoPtr->persistAutoCalParam.memSize);

            SNS_SCM_PRINTF3(MEDIUM, " stored qmag auto cal params to scm algo dbase offsets: %d, %d, %d",
                            qmagOpAlgoDBPtr->offset_HI[0],
                            qmagOpAlgoDBPtr->offset_HI[1],
                            qmagOpAlgoDBPtr->offset_HI[2]);

            sns_scm_store_qmag_auto_cal_state_algodbase(algoInstPtr->stateData.memPtr,
                                                        algoPtr->defConfigData.memPtr);
         }
      }
      else if ( algoPtr->serviceId == SNS_SCM_GYRO_CAL_SVC)
      {
         gyro_cal_output_s *gyroOpAlgoInstPtr = (gyro_cal_output_s *)(algoInstPtr->outputData.memPtr);
         gyro_cal_output_s *gyroOpAlgoDBPtr = (gyro_cal_output_s *)(algoPtr->persistAutoCalParam.memPtr);

         //skip storing to algo dbase if calibration did not occur
         if (gyroOpAlgoInstPtr->bias[0] != 0 && gyroOpAlgoInstPtr->bias[1] != 0 && gyroOpAlgoInstPtr->bias[2] != 0)
         {
            SNS_OS_MEMCOPY(algoPtr->persistAutoCalParam.memPtr,
                           algoInstPtr->outputData.memPtr,
                           algoPtr->persistAutoCalParam.memSize);

            SNS_SCM_PRINTF3(HIGH, " stored gyro auto cal params to persistent algo dbase offset: %d, %d, %d",
                            gyroOpAlgoDBPtr->bias[0],
                            gyroOpAlgoDBPtr->bias[1],
                            gyroOpAlgoDBPtr->bias[2]);
         }
      }
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_disable_algo
  =========================================================================*/
/*!
  @brief Decrements the reference count of the algorithm instance.
         If count is zero, deletes an instance and frees up its resources

  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_disable_algo(
   uint8_t algoIndex)
{
   uint8_t algoInstId;

   if (sns_scm_algo_dbase[algoIndex] == NULL)
   {
      SNS_SCM_PRINTF1(ERROR, "algoIndex: %d has NULL entry in database", algoIndex);
      return SNS_ERR_FAILED;
   }

   algoInstId = sns_scm_find_algo_inst(sns_scm_algo_dbase[algoIndex]->serviceId,
                                       sns_scm_algo_dbase[algoIndex]->defConfigData.memPtr);

   if (algoInstId == SNS_SCM_INVALID_ID)
   {
      return SNS_ERR_BAD_PARM;
   }
   // store auto cal params to scm's algo database
   sns_scm_store_auto_cal_params_algodbase(algoIndex, algoInstId);

   //Deregister timer
   sns_scm_dereg_timer(algoIndex);

   //stop sensor data
   sns_scm_stop_sensor_data(algoInstId);

   //delete the algo instance
   sns_scm_delete_algo_inst(algoInstId);

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_DISABLE_ALGO,
                  sns_scm_algo_dbase[algoIndex]->serviceId,
                  sns_scm_sensor_dbase[sns_scm_algo_dbase[algoIndex]->sensorIndex].sensorId,
                  algoInstId);

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_update_sensor_status
  =========================================================================*/
/*!
  @brief    Updates overall status of sensor calibration service based on
            sensor status of each sensor ID.

  @param[i] algoPtr: pointer to algo database

  @return   Overall status
*/
/*=======================================================================*/
static uint8_t sns_scm_update_sensor_status(
   sns_scm_algo_s *algoPtr)
{
   unsigned int one_client_cnt = 0;
   unsigned int active_cnt = 0;
   uint8_t sensorStatus0 = SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01;
   uint8_t status = SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01, sensorIndex = 0;
   int k;

   for(k = 0; k < algoPtr->monitorSensorCount; k++)
   {
      sensorIndex = sns_scm_get_sensor_index(algoPtr->monitorSensorId[k]);
      if(sensorIndex != SNS_SCM_INVALID_ID && sensorIndex < SNS_SCM_MAX_SNS_MON)
      {
         switch(sns_scm_sensor_dbase[sensorIndex].status)
         {
            case SNS_SMGR_SENSOR_STATUS_ACTIVE_V01:
               active_cnt++;
               break;
            case SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01:
               one_client_cnt++;
               break;
            default:
               break;
         }

         if(k == 0)
         {
            sensorStatus0 = sns_scm_sensor_dbase[sensorIndex].status;
         }
      }
   }

   status = (active_cnt >= 1)     ? SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 :
            (one_client_cnt > 1)  ? SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 :
            (sensorStatus0 ==
             SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01)
                                  ? SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01 :
            (sensorStatus0 ==
             SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01)
                                  ? SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01 :
                                    SNS_SMGR_SENSOR_STATUS_IDLE_V01 ;

   SNS_SCM_PRINTF3(MED, "active=%d 1client=%d status=%d",
                   active_cnt, one_client_cnt, status);

   algoPtr->monitorSensorStatus = status;

   return status;
}

/*=========================================================================
  FUNCTION:  sns_scm_active_qmd_client
  =========================================================================*/
/*!
  @brief Determines if any algorithm requires QMD.

  @return true if at least one algorithm requires QMD
          false otherwise
*/
/*=======================================================================*/
static bool sns_scm_active_qmd_client(void)
{
   uint8_t i;
   bool active = false;

   for(i = 0; i < SNS_SCM_NUM_ALGO_SVCS; i++)
   {
      if(sns_scm_algo_dbase[i] != NULL)
      {
         if(sns_scm_algo_dbase[i]->monitorSensorStatus != SNS_SMGR_SENSOR_STATUS_IDLE_V01 &&
            sns_scm_algo_dbase[i]->monitorSensorStatus != SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01)
         {
            active = true;
            break;
         }
      }
   }

   return active;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_algo_result
  =========================================================================*/
/*!
  @brief Process specified algorithm result

  @param[i] clientReqId: index to client request in the database

  @return None
*/
/*=======================================================================*/
static void sns_scm_process_algo_result(
   uint8_t algoInstId)
{
   if (sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
       serviceId == SNS_SCM_GYRO_CAL_SVC)
   {
      gyro_cal_output_s* gyroCalResult;
      sns_smgr_sensor_cal_req_msg_v01 *msgPtr = NULL;

      qmi_client_error_type            err;
      sns_smgr_sensor_cal_resp_msg_v01 *msgRespP;
      qmi_txn_handle                   txn_handle;

      msgRespP = (sns_smgr_sensor_cal_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr = (sns_smgr_sensor_cal_req_msg_v01 *)
               SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      gyroCalResult = (gyro_cal_output_s*)
                      (sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr);

      msgPtr->usage = SNS_SMGR_CAL_DYNAMIC_V01;
      msgPtr->SensorId = SNS_SMGR_ID_GYRO_V01;
      msgPtr->DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;

      //Send bias correction values to SMGR
      msgPtr->ZeroBias_len = 3;
      msgPtr->ZeroBias[0] = -(gyroCalResult->bias[0]);
      msgPtr->ZeroBias[1] = -(gyroCalResult->bias[1]);
      msgPtr->ZeroBias[2] = -(gyroCalResult->bias[2]);

      msgPtr->ScaleFactor_len = 0;
      err = sns_scm_qmi_client_send_msg_async (sns_scm_smgr_user_handle, SNS_SMGR_CAL_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_sensor_cal_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_sensor_cal_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_GYRO_CAL_REQ_INFO,
                     -(gyroCalResult->bias[0]),
                     -(gyroCalResult->bias[1]),
                     -(gyroCalResult->bias[2]));
   }
   else if (sns_scm_algo_dbase[sns_scm_algo_inst_dbase[algoInstId]->algoIndex]->
       serviceId == SNS_SCM_MAG_CAL_SVC)
   {
      qmag_cal_output_s* qmagCalResult;
      sns_smgr_sensor_cal_req_msg_v01 *msgPtr = NULL;

      qmi_client_error_type            err;
      sns_smgr_sensor_cal_resp_msg_v01 *msgRespP;
      qmi_txn_handle                   txn_handle;

      msgRespP = (sns_smgr_sensor_cal_resp_msg_v01 *)
                 SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_resp_msg_v01));
      SNS_ASSERT(msgRespP != NULL);

      msgPtr = (sns_smgr_sensor_cal_req_msg_v01 *)
               SNS_OS_MALLOC(SNS_SCM_DBG_MOD, sizeof(sns_smgr_sensor_cal_req_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      qmagCalResult = (qmag_cal_output_s*)
                      (sns_scm_algo_inst_dbase[algoInstId]->outputData.memPtr);

      msgPtr->usage = SNS_SMGR_CAL_DYNAMIC_V01;
      msgPtr->SensorId = SNS_SMGR_ID_MAG_V01;
      msgPtr->DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;

      //Send bias correction values to SMGR
      msgPtr->ZeroBias_len = 3;
      msgPtr->ZeroBias[0] = -(qmagCalResult->offset_HI[0]);
      msgPtr->ZeroBias[1] = -(qmagCalResult->offset_HI[1]);
      msgPtr->ZeroBias[2] = -(qmagCalResult->offset_HI[2]);

      msgPtr->ScaleFactor_len = 0;
      msgPtr->CompensationMatrix_valid = false;
      msgPtr->CalibrationAccuracy_valid = true;
      msgPtr->CalibrationAccuracy = qmagCalResult->accuracy_HI;
      err = sns_scm_qmi_client_send_msg_async (sns_scm_smgr_user_handle, SNS_SMGR_CAL_REQ_V01,
                                       msgPtr, sizeof(sns_smgr_sensor_cal_req_msg_v01),
                                       msgRespP, sizeof(sns_smgr_sensor_cal_resp_msg_v01),
                                       sns_scm_smgr_resp_cb, NULL, &txn_handle);

      SNS_SCM_PRINTF3(MEDIUM, "Sending correction values to Smgr %d %d %d",
          msgPtr->ZeroBias[0],
          msgPtr->ZeroBias[1],
          msgPtr->ZeroBias[2]);
      SNS_SCM_PRINTF1(MEDIUM, "Sending Cal Accuracy Smgr %d ", msgPtr->CalibrationAccuracy);
   }
}


/*=========================================================================
  FUNCTION:  sns_scm_process_sam_resp
  =========================================================================*/
/*!
  @brief Processes the response received from sensors manager

  @param[i] smgrRespPtr: Pointer to sensors manager response message
  @param[i] msgId: Message ID

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_sam_resp(
   const void *samRespPtr,
   int32_t msgId)
{
   if (msgId == SNS_SAM_AMD_ENABLE_RESP_V01)
   {
      sns_sam_qmd_enable_resp_msg_v01* respPtr =
      (sns_sam_qmd_enable_resp_msg_v01*)samRespPtr;

      if (respPtr->resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF0(ERROR, "SCM : error in SNS_SAM_AMD_ENABLE_RESP_V01");

         return((sns_err_code_e)(respPtr->resp.sns_err_t));
      }

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SAM_RESP_INFO,
                     msgId, respPtr->instance_id, sns_scm_qmd_inst_id);

      //Update the qmd instance id
      sns_scm_qmd_inst_id = respPtr->instance_id;
   }
   else if (msgId == SNS_SAM_AMD_DISABLE_RESP_V01)
   {
      sns_sam_qmd_disable_resp_msg_v01* respPtr =
      (sns_sam_qmd_disable_resp_msg_v01*)samRespPtr;

      if (respPtr->resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF0(ERROR, "SCM : error in SNS_SAM_AMD_DISABLE_RESP_V01");

         return((sns_err_code_e)(respPtr->resp.sns_err_t));
      }

      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SAM_RESP_INFO,
                     msgId, respPtr->instance_id, sns_scm_qmd_inst_id);
   }
   else
   {
      SNS_SCM_DEBUG1(HIGH, DBG_SCM_SAM_RESP_DROPPED, msgId);
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_sam_ind
  =========================================================================*/
/*!
  @brief Processes sensor status indication received from sensors manager

  @param[i] smgrIndPtr: Pointer to sensors manager indication message

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_sam_ind(
   const void *samIndPtr)
{
   sns_sam_qmd_report_ind_msg_v01* indPtr;
   uint8_t i;
   uint8_t algoInstCnt = sns_scm_algo_inst_count;

   indPtr = (sns_sam_qmd_report_ind_msg_v01*)samIndPtr;

   if (indPtr->instance_id == sns_scm_qmd_inst_id)
   {
      sns_scm_qmd_state = indPtr->state;
   }

   SNS_SCM_PRINTF1(MEDIUM, "scm qmd state change: %d",sns_scm_qmd_state);

   //enable algos
   for (i = 0;
        (i < SNS_SCM_NUM_ALGO_SVCS) && (sns_scm_algo_dbase[i] != NULL);
        i++)
   {
      if (sns_scm_algo_dbase[i]->enableAlgo)
      {
         if (sns_scm_algo_dbase[i]->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 ||
             sns_scm_algo_dbase[i]->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01)
         {
            if ( (sns_scm_qmd_state == SNS_SAM_MOTION_REST_V01 &&
                 sns_scm_algo_dbase[i]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_REST) ||
                 (sns_scm_qmd_state == SNS_SAM_MOTION_MOVE_V01 &&
                 sns_scm_algo_dbase[i]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_MOTION)
                )
            {
               sns_scm_enable_algo(i);
               SNS_SCM_PRINTF1(MEDIUM, "scm enable algo svc: %d",sns_scm_algo_dbase[i]->serviceId);
            }
         }
      }
   }

   algoInstCnt = sns_scm_algo_inst_count; // reinitialize to account for any new enables
   //disable algos
   for (i=0; i < SNS_SCM_MAX_ALGO_INSTS && algoInstCnt > 0; i++)
   {
      if (sns_scm_algo_inst_dbase[i] != NULL) 
      {
         uint8_t algoIndex = sns_scm_algo_inst_dbase[i]->algoIndex;

         SNS_SCM_PRINTF3(HIGH, "scm algo dbase scan i: %d, algo index: %d, algo svc: %d",
                         i,algoIndex,sns_scm_algo_dbase[algoIndex]->serviceId);
         SNS_SCM_PRINTF1(HIGH, "scm motionTrigFlags: %d",sns_scm_algo_dbase[algoIndex]->motionTrigFlags);

         if( (sns_scm_algo_dbase[algoIndex]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_REST &&
              sns_scm_qmd_state == SNS_SAM_MOTION_MOVE_V01) ||
             (sns_scm_algo_dbase[algoIndex]->motionTrigFlags == MOTION_DETECT_FLAG_ACTIVE_IN_MOTION &&
              sns_scm_qmd_state == SNS_SAM_MOTION_REST_V01))
         {
            sns_scm_disable_algo(algoIndex);
            algoInstCnt --;
            SNS_SCM_PRINTF1(MEDIUM, "scm disable algo svc: %d",sns_scm_algo_dbase[algoIndex]->serviceId);
         }
      }
   }

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SAM_IND_INFO,
                  sns_scm_qmd_inst_id, indPtr->instance_id, indPtr->state);

#if defined(SNS_PLAYBACK_SKIP_SMGR) || defined(SNS_QDSP_SIM)
   if (sns_scm_qmd_state == SNS_SAM_MOTION_REST_V01)
   {
      sns_scm_pb_next_report_time=indPtr->timestamp + sns_scm_pb_report_timeout;
      sns_scm_pb_update_next_report_time=true;
   }
   else if (sns_scm_qmd_state == SNS_SAM_MOTION_MOVE_V01)
   {
      sns_scm_pb_update_next_report_time=false;
   }
#endif

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_resp
  =========================================================================*/
/*!
  @brief Processes the response received from sensors manager

  @param[i] smgrRespPtr: Pointer to sensors manager response message
  @param[i] msgId: Message ID

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_smgr_resp(
   const void *smgrRespPtr,
   int32_t msgId)
{
   uint8_t dataReqId = SNS_SCM_INVALID_ID;
   uint32_t i;

   if (msgId == SNS_SMGR_BUFFERING_RESP_V01)
   {
      sns_smgr_buffering_resp_msg_v01* respPtr =
         (sns_smgr_buffering_resp_msg_v01*) smgrRespPtr;

      if (respPtr->ReportId_valid)
      {
         dataReqId = respPtr->ReportId;
      }

      if( respPtr->Resp.sns_result_t != SNS_RESULT_SUCCESS_V01 &&
          respPtr->Resp.sns_err_t != SENSOR1_SUCCESS )
      {
        SNS_SCM_PRINTF2(ERROR, "SCM : error in SNS_SMGR_BUFFERING_RESP_V01,err:%d, acknack:%d",
            respPtr->Resp.sns_err_t, respPtr->AckNak);
        if( respPtr->Resp.sns_err_t == SENSOR1_EBAD_MSG_ID )
        {
           sns_scm_smgr_buffering_flag = false;
           if( sns_scm_switch_smgr_data_req( dataReqId ) == SNS_SUCCESS )
           {
              // Retrying with periodic data request
              return SNS_ERR_WOULDBLOCK;
           }
        }
        else if( respPtr->AckNak_valid &&
                 respPtr->AckNak == SNS_SMGR_RESPONSE_NAK_REPORT_RATE_V01 )
        {
           if( sns_scm_switch_smgr_data_req( dataReqId ) == SNS_SUCCESS )
           {
              // Retrying with periodic data request
              return SNS_ERR_WOULDBLOCK;
           }
        }
      }

      if (respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ||
          respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01)
      {
         SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SMGR_RESP_SUCCESS,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);

         return SNS_SUCCESS;
      }

      if (dataReqId < SNS_SCM_MAX_DATA_REQS &&
          sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_ACK_VAL,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }
      else
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_DATA_DROP,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }

      if (respPtr->ReasonPair_len < SNS_SMGR_MAX_NUM_REASONS_V01)
      {
         for (i = 0; i < respPtr->ReasonPair_len; i++)
         {
            SNS_SCM_DEBUG2(MEDIUM, DBG_SCM_SMGR_RESP_INFO,
                           respPtr->ReasonPair[i].ItemNum,
                           respPtr->ReasonPair[i].Reason);
         }
      }
      return((sns_err_code_e)(respPtr->Resp.sns_err_t));
   }
   else if (msgId == SNS_SMGR_REPORT_RESP_V01)
   {
      sns_smgr_periodic_report_resp_msg_v01* respPtr =
      (sns_smgr_periodic_report_resp_msg_v01*)smgrRespPtr;

      if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF0(ERROR, "SCM : error in SNS_SMGR_REPORT_RESP_V01");

         return((sns_err_code_e)(respPtr->Resp.sns_err_t));
      }

      dataReqId = respPtr->ReportId;

      if (respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ||
          respPtr->AckNak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01)
      {
         SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SMGR_RESP_SUCCESS,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);

         return SNS_SUCCESS;
      }

      if (dataReqId < SNS_SCM_MAX_DATA_REQS &&
          sns_scm_data_req_dbase[dataReqId] != NULL)
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_ACK_VAL,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }
      else
      {
         SNS_SCM_DEBUG3(HIGH, DBG_SCM_SMGR_RESP_DATA_DROP,
                        dataReqId, respPtr->AckNak, respPtr->ReasonPair_len);
      }

      if (respPtr->ReasonPair_len < SNS_SMGR_MAX_NUM_REASONS_V01)
      {
         for (i = 0; i < respPtr->ReasonPair_len; i++)
         {
            SNS_SCM_DEBUG2(MEDIUM, DBG_SCM_SMGR_RESP_INFO,
                           respPtr->ReasonPair[i].ItemNum,
                           respPtr->ReasonPair[i].Reason);
         }
      }
   }
   else if (msgId == SNS_SMGR_SENSOR_STATUS_RESP_V01)
   {
      sns_smgr_sensor_status_resp_msg_v01* respPtr =
      (sns_smgr_sensor_status_resp_msg_v01 *)smgrRespPtr;

      if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF1(ERROR,
                         "error in SNS_SMGR_SENSOR_STATUS_RESP_V01 = %d",
                         respPtr->Resp.sns_result_t);

         return((sns_err_code_e)(respPtr->Resp.sns_err_t));
      }

      for (i=0; i < SNS_SCM_MAX_SNS_MON &&
          sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
      {
         if (sns_scm_sensor_dbase[i].sensorId == respPtr->SensorID
             && sns_scm_sensor_dbase[i].status == SNS_SCM_INVALID_ID)
         {
            sns_scm_sensor_dbase[i].status = SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01;
            break;
         }
      }
   }
   else if (msgId == SNS_SMGR_CAL_RESP_V01)
   {
      sns_smgr_sensor_cal_resp_msg_v01* respPtr =
      (sns_smgr_sensor_cal_resp_msg_v01*)smgrRespPtr;

      if (respPtr->Resp.sns_result_t != SNS_SUCCESS)
      {
         SNS_SCM_PRINTF1(ERROR, "error in SNS_SMGR_CAL_RESP_V01 = %d",
                         respPtr->Resp.sns_result_t);

         return((sns_err_code_e)(respPtr->Resp.sns_err_t));
      }
   }
   else
   {
      SNS_SCM_DEBUG1(HIGH, DBG_SCM_SMGR_RESP_DROPPED, msgId);
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_item
  =========================================================================*/
/*!
  @brief    Processes a sample in a SMGR buffering indication message

  @param[i] dataPtr: pointer to sample in buffering indication message
  @param[i] timestamp:  timestamp of the current sample
  @param[i] dataReqId:  index of data request entry in dbase

  @return   Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_smgr_item(
   const void* dataPtr,
   uint32_t timestamp,
   uint8_t dataReqId, 
   uint32_t sensorId)
{
   uint32_t i;

   sns_smgr_buffering_sample_s_v01 * samplePtr =
      (sns_smgr_buffering_sample_s_v01*)dataPtr;
   uint8_t itemSize = sizeof(samplePtr->Data);

   if (dataReqId >= SNS_SCM_MAX_DATA_REQS ||
       sns_scm_data_req_dbase[dataReqId] == NULL)
   {
      return SNS_ERR_FAILED;
   }

   //execute algorithms waiting for this sensor data
   for (i = 0; i < sns_scm_data_req_dbase[dataReqId]->algoInstCount; i++)
   {
      uint8_t algoInstId = sns_scm_data_req_dbase[dataReqId]->algoInstDbase[i];
      sns_scm_algo_inst_s *algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];

      if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_GYRO_CAL_SVC)
      {
         uint8_t algoInpSize, *algoInpPtr;
         gyro_cal_output_s* gyroCalResult = algoInstPtr->outputData.memPtr;
         gyro_cal_state_s* gyroCalState = algoInstPtr->stateData.memPtr;
         int32_t *gyroData = samplePtr->Data;
         bool skipGyroCal = false;
         uint8_t j;

         //Skip gyro calibration if absolute bias is exceeded on any axis
         for (j=0; j < 3; j++)
         {
            if (FX_ABS(gyroData[j]) > SNS_SCM_GYRO_CAL_MAX_ABS_BIAS)
            {
               skipGyroCal = true;
               break;
            }
         }
         if (skipGyroCal)
         {
            sns_scm_stop_sensor_data(algoInstId);
            if (sns_scm_data_req_dbase[dataReqId] == NULL)
            {
               break;
            }
         }

         //copy sensor data to algorithm input
         algoInpPtr = algoInstPtr->inputData.memPtr;
         algoInpSize = algoInstPtr->inputData.memSize;
         if (itemSize > algoInpSize)
         {
            SNS_SCM_DEBUG2(ERROR, DBG_SCM_SMGR_IND_INVALID, algoInpSize, itemSize);
            continue;
         }
         SNS_OS_MEMCOPY(algoInpPtr, samplePtr->Data, itemSize);

         //execute the algorithm
         sns_scm_algo_dbase[algoInstPtr->algoIndex]->algoApi.
         sns_scm_algo_update(algoInstPtr->stateData.memPtr,
                             algoInstPtr->inputData.memPtr,
                             algoInstPtr->outputData.memPtr);

         if (gyroCalResult->motionState == GYRO_CAL_MOT_STATE_REST)
         {
            sns_scm_process_algo_result(algoInstId);
         }

         sns_scm_log_algo_result(algoInstId, timestamp);

         if (gyroCalResult->motionState == GYRO_CAL_MOT_STATE_UNKNOWN)
         {
            continue;
         }

         SNS_SCM_DEBUG3(LOW, DBG_SCM_GYRO_CAL_REQ_INFO,
                        (int32_t)(gyroCalState->variance[0] >> 32),
                        (int32_t)(gyroCalState->variance[1] >> 32),
                        (int32_t)(gyroCalState->variance[2] >> 32));
         SNS_SCM_DEBUG3(LOW, DBG_SCM_GYRO_CAL_REQ_INFO,
                        (int32_t)(gyroCalState->variance[0]),
                        (int32_t)(gyroCalState->variance[1]),
                        (int32_t)(gyroCalState->variance[2]));

         //stop sensor data
         sns_scm_stop_sensor_data(algoInstId);
         if (sns_scm_data_req_dbase[dataReqId] == NULL)
         {
            break;
         }
      }
      else if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_MAG_CAL_SVC)
      {
         bool hard_reset = false;
         qmag_cal_input_s *inp = (qmag_cal_input_s *)(algoInstPtr->inputData.memPtr);
         qmag_cal_output_s *outp = (qmag_cal_output_s *)(algoInstPtr->outputData.memPtr);

         // Peak Detector
         if (outp->accuracy_HI >= QMAG_CAL_MIN_HI_GOOD_ACCURACY && 
          sensorId == SNS_SMGR_ID_MAG_V01)
         {
            // convert raw -> calibrated sample and compute its norm squared
            uint32_t col;
            float cal_mag_norm_sq = 0.0;
            void *statePtr = NULL;

            for(col=0; col<QMAG_CAL_COL; col++)
            {
               cal_mag_norm_sq += (float)FX_FIXTOFLT_Q16(samplePtr->Data[col] - outp->offset_HI[col]) *
                              (float)FX_FIXTOFLT_Q16(samplePtr->Data[col] - outp->offset_HI[col]);
            }

            if (cal_mag_norm_sq > QMAG_CAL_HARD_RESET_THRESH_GAUSS_SQ)
            {
              hard_reset = true;
              SNS_SCM_PRINTF0(ERROR, "QMag Cal Hard Reset");

              statePtr = qmag_cal_scm_state_reset(algoInstPtr->configData.memPtr,
                               algoInstPtr->stateData.memPtr);

              if (statePtr == NULL)
              {
                SNS_SCM_PRINTF0(ERROR, "QMag Cal State Reset Error");
              }

               outp->accuracy_HI = QMAG_CAL_ACCURACY_FAILED; 
               outp->offset_time = timestamp + samplePtr->TimeStampOffset;
               outp->offset_updated = true;
             }
         }

         if (!hard_reset)
         {
            inp->sensor_time   = timestamp + samplePtr->TimeStampOffset;
            inp->sensor_data[0] = samplePtr->Data[0];
            inp->sensor_data[1] = samplePtr->Data[1];
            inp->sensor_data[2] = samplePtr->Data[2];

             if (sensorId == SNS_SMGR_ID_ACCEL_V01)
             {
                /* Update with Accel */
                inp->input_type = QMAG_CAL_INPUT_TYPE_ACCEL;
                //  SNS_SCM_PRINTF3(MED, "ACCEL INPUTS : %d, %d, ts, %u ",
                //            inp->sensor_data[0],
                //             inp->sensor_data[1],
                //            inp->sensor_time);
             }
             else if (sensorId == SNS_SMGR_ID_MAG_V01)
             {
               /* Update with Mag */
               inp->input_type = QMAG_CAL_INPUT_TYPE_MAG;
               // SNS_SCM_PRINTF3(MED, "MAG INPUTS : %d, %d,ts , %u",
               //            inp->sensor_data[0],
               //            inp->sensor_data[1],
               //            inp->sensor_time);
             }
             else
             {
               SNS_SCM_PRINTF0(ERROR, "Cannot understand sensor id - bail !");
               return(SNS_ERR_FAILED);
             }
         }
         else
         {
           SNS_SCM_PRINTF0(ERROR, "Reset Mag cal!");
         }

         if ( (inp->sensor_time > 0) && (!hard_reset) )
         {
            //execute the algorithm
            sns_scm_algo_dbase[algoInstPtr->algoIndex]->algoApi.
            sns_scm_algo_update(algoInstPtr->stateData.memPtr,
                     algoInstPtr->inputData.memPtr,
                     algoInstPtr->outputData.memPtr);

            SNS_SCM_PRINTF3(MED, "OUTPUT OFFSETS: %d %d %d",
              outp->offset_HI[0],
              outp->offset_HI[1],
              outp->offset_HI[2]);

            SNS_SCM_PRINTF2(MED, "after input with sns_type %d, accuracy : %d ",
                      inp->input_type,
                      outp->accuracy_HI);
         }

         if (outp->offset_updated)
         {
            sns_scm_process_algo_result(algoInstId);
         }
         sns_scm_log_algo_result(algoInstId, timestamp);
      }
      else
      {
         SNS_SCM_PRINTF0(ERROR, "Skipping sample");

      }
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_buffer
  =========================================================================*/
/*!
  @brief Processes samples in the buffering indication received from SMGR

  @param[i] indPtr: Pointer to buffering indication message
  @param[i] dataReqId: Index of entry in data request database

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_smgr_buffer(
   const sns_smgr_buffering_ind_msg_v01 *indPtr,
   uint8_t dataReqId)
{
   uint32_t i;
   sns_err_code_e err = SNS_SUCCESS;
   const sns_smgr_buffering_sample_s_v01 * samplePtr[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01];
   uint32_t sampleCount[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01] = { 0 };
   uint32_t timestamp[SNS_SMGR_BUFFERING_REQUEST_MAX_ITEMS_V01] = { 0 };
   uint32_t samplesProcessed = 0;

   SNS_OS_MEMSET( samplePtr, 0, sizeof(samplePtr) );

   for (i = 0; i < indPtr->Indices_len; i++)
   {
      samplePtr[i] = &indPtr->Samples[indPtr->Indices[i].FirstSampleIdx];
      sampleCount[i] = indPtr->Indices[i].SampleCount;
      timestamp[i] = indPtr->Indices[i].FirstSampleTimestamp;
   }

   while( samplesProcessed < indPtr->Samples_len )
   {
      uint32_t minTimestamp = 0;
      uint32_t oldestSampleIdx = 0;
      const sns_smgr_buffering_sample_s_v01 * oldestSamplePtr = NULL;

      samplesProcessed++;
      for (i = 0; i < indPtr->Indices_len; i++)
      {
         // Find first valid sample to initialize min timestamp
         if( samplePtr[i] != NULL )
         {
            minTimestamp = timestamp[i];
            oldestSampleIdx = i;
            break;
         }
      }

      for (i = oldestSampleIdx + 1; i < indPtr->Indices_len; i++)
      {
         // Find first minimum based on timestamp
         if( samplePtr[i] != NULL &&
             ((timestamp[i] < minTimestamp && (minTimestamp - timestamp[i] < UINT32_MAX / 2)) ||
              (timestamp[i] > minTimestamp && (timestamp[i] - minTimestamp > UINT32_MAX / 2)) ))
         {
            minTimestamp = timestamp[i];
            oldestSampleIdx = i;
         }
      }

      oldestSamplePtr = samplePtr[oldestSampleIdx];
      if( !oldestSamplePtr )
      {
         SNS_SCM_PRINTF1( ERROR, "SCM bad sample ptr for index %d", oldestSampleIdx );
         err = SNS_ERR_BAD_PTR;
         break;
      }

      if( sampleCount[oldestSampleIdx] > 1 )
      {
         samplePtr[oldestSampleIdx]++;
         timestamp[oldestSampleIdx] += (samplePtr[oldestSampleIdx]->TimeStampOffset);
         sampleCount[oldestSampleIdx]--;
      }
      else
      {
         samplePtr[oldestSampleIdx] =  NULL;
         timestamp[oldestSampleIdx] = UINT32_MAX;
      }

      if (oldestSamplePtr->Flags == SNS_SMGR_ITEM_FLAG_INVALID_V01 ||
          oldestSamplePtr->Quality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_FAILED_SENSOR_V01 ||
          oldestSamplePtr->Quality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY_V01 ||
          oldestSamplePtr->Quality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_SUSPENDED_V01)
      {
         SNS_SCM_DEBUG1(MEDIUM, DBG_SCM_SMGR_IND_DROPPED, oldestSampleIdx);
         continue;
      }

      //execute algorithms waiting for this sensor data
      err = sns_scm_process_smgr_item(oldestSamplePtr, minTimestamp,
                    dataReqId, indPtr->Indices[oldestSampleIdx].SensorId);
   }
   SNS_SCM_PRINTF1( LOW, "SCM processed %d samples from buffering indication", samplesProcessed );
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_buffering_ind
  =========================================================================*/
/*!
  @brief Processes the buffering indication received from sensors manager

  @param[i] smgrIndPtr: Pointer to sensors manager buffering indication msg

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_smgr_buffering_ind(const void *smgrIndPtr)
{
   sns_smgr_buffering_ind_msg_v01* indPtr;
   uint8_t dataReqId = SNS_SCM_INVALID_ID;
   sns_scm_data_req_s * dataReqPtr = NULL;
   uint32_t i, j;
   sns_err_code_e err;
   bool foundType = false;

   indPtr = (sns_smgr_buffering_ind_msg_v01*)smgrIndPtr;
   dataReqId = indPtr->ReportId;
   if (dataReqId >= SNS_SCM_MAX_DATA_REQS ||
       sns_scm_data_req_dbase[dataReqId] == NULL)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);
      return SNS_ERR_FAILED;
   }

   if( indPtr->Indices_len == 0 || indPtr->Samples_len == 0 )
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);
      return SNS_ERR_FAILED;
   }

   //Validate sensor data
   dataReqPtr = sns_scm_data_req_dbase[dataReqId];
   for( i = 0; !foundType && i < indPtr->Indices_len; i++ )
   {
      // Check if report has at least one of the types that was requested
      for( j = 0; !foundType && j < dataReqPtr->sensorCount; j++ )
      {
         if( dataReqPtr->sensorDbase[j].sensorId == indPtr->Indices[i].SensorId &&
             dataReqPtr->sensorDbase[j].dataType == indPtr->Indices[i].DataType )
         {
            foundType = true;
         }
      }
   }
   SNS_SCM_PRINTF3(LOW, "SCM: got buffering ind with %d samples and %d types, found_type %d",
                   indPtr->Samples_len, indPtr->Indices_len, foundType);

   //TODO: If report has types that were not requested, bail
   if( !foundType )
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);
      return SNS_ERR_FAILED;
   }

#if defined(SNS_PLAYBACK_SKIP_SMGR)
   if(sns_scm_pb_update_next_report_time==true)
   {
      if(sns_scm_pb_next_report_time < indPtr->Item[0].TimeStamp)
      {
         //Simulate timeout
         sns_scm_timer_cb((void*)sns_scm_pb_gyro_cal_algo_svc_id);
         sns_scm_pb_next_report_time += sns_scm_pb_report_timeout;
      }
   }
#endif

   //execute algorithms waiting for this sensor data
   err = sns_scm_process_smgr_buffer( indPtr, dataReqId);
   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_smgr_ind
  =========================================================================*/
/*!
  @brief Processes the indication received from sensors manager

  @param[i] smgrIndPtr: Pointer to sensors manager indication message

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_smgr_ind(const void *smgrIndPtr)
{
   sns_smgr_periodic_report_ind_msg_v01* indPtr;
   uint8_t dataReqId = SNS_SCM_INVALID_ID, i;
   uint32_t timestamp;

   indPtr = (sns_smgr_periodic_report_ind_msg_v01*)smgrIndPtr;

   if (indPtr->status != SNS_SMGR_REPORT_OK_V01)
   {
      SNS_SCM_DEBUG2(HIGH, DBG_SCM_SMGR_IND_INFO, dataReqId, indPtr->status);

      return SNS_ERR_FAILED;
   }

#if defined(SNS_PLAYBACK_SKIP_SMGR) || defined(SNS_QDSP_SIM)
   if(sns_scm_pb_update_next_report_time==true)
   {
      if(sns_scm_pb_next_report_time < indPtr->Item[0].TimeStamp)
      {
         //Simulate timeout
         sns_scm_timer_cb((void*)sns_scm_pb_gyro_cal_algo_svc_id);
         sns_scm_pb_next_report_time += sns_scm_pb_report_timeout;
      }
   }
#endif

   dataReqId = indPtr->ReportId;
   if (dataReqId >= SNS_SCM_MAX_DATA_REQS ||
       sns_scm_data_req_dbase[dataReqId] == NULL)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);

      return SNS_ERR_FAILED;
   }

   if (indPtr->Item_len != sns_scm_data_req_dbase[dataReqId]->sensorCount)
   {
      SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);

      return SNS_ERR_FAILED;
   }

   if (indPtr->CurrentRate != sns_scm_data_req_dbase[dataReqId]->reportRate)
   {
      SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_SMGR_IND_RATE_INFO,
                     dataReqId, indPtr->CurrentRate,
                     sns_scm_data_req_dbase[dataReqId]->reportRate);
   }


   //SNS_SCM_PRINTF0(ERROR, "Validating SMGR data");

   //Validate sensor data
   for (i = 0; i < indPtr->Item_len; i++)
   {
      //SNS_SCM_PRINTF2(ERROR, "IDX %d, Sensor %d", i, indPtr->Item[i].SensorId);
      if (indPtr->Item[i].SensorId !=
          sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].sensorId ||
          indPtr->Item[i].DataType !=
          sns_scm_data_req_dbase[dataReqId]->sensorDbase[i].dataType)
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);

         return SNS_ERR_FAILED;
      }
      if (indPtr->Item[i].ItemFlags == SNS_SMGR_ITEM_FLAG_INVALID_V01 ||
          indPtr->Item[i].ItemQuality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_FAILED_SENSOR_V01 ||
          indPtr->Item[i].ItemQuality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY_V01 ||
          indPtr->Item[i].ItemQuality ==
          SNS_SMGR_ITEM_QUALITY_INVALID_SUSPENDED_V01)
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_SMGR_IND_DROPPED, dataReqId);

         return SNS_ERR_FAILED;
      }
   }

   //use the timestamp of the first item i.e. the sensor data being calibrated
   timestamp = indPtr->Item[0].TimeStamp;

   //execute algorithms waiting for this sensor data
   for (i = 0; i < sns_scm_data_req_dbase[dataReqId]->algoInstCount; i++)
   {
      uint8_t algoInpSize, *algoInpPtr;
      uint8_t algoInstId = sns_scm_data_req_dbase[dataReqId]->algoInstDbase[i];
      sns_scm_algo_inst_s *algoInstPtr = sns_scm_algo_inst_dbase[algoInstId];
      uint8_t j;

      if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_GYRO_CAL_SVC)
      {
         //Skip gyro calibration if absolute bias is exceeded on any axis
         if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_GYRO_CAL_SVC)
         {
            int32_t *gyroData = indPtr->Item[0].ItemData;
            bool skipGyroCal = false;
            for (j=0; j < 3; j++)
            {
               if (FX_ABS(gyroData[j]) > SNS_SCM_GYRO_CAL_MAX_ABS_BIAS)
               {
                  skipGyroCal = true;
                  break;
               }
            }
            if (skipGyroCal)
            {
               sns_scm_stop_sensor_data(algoInstId);
               if (sns_scm_data_req_dbase[dataReqId] == NULL)
               {
                  break;
               }
            }
         }

         //copy sensor data to algorithm input
         for (j = 0,
              algoInpPtr = algoInstPtr->inputData.memPtr,
              algoInpSize = algoInstPtr->inputData.memSize;
             j < indPtr->Item_len;
             j++)
         {
            uint8_t itemSize = sizeof(indPtr->Item[j].ItemData);

            if (itemSize > algoInpSize)
            {
               SNS_SCM_DEBUG2(ERROR, DBG_SCM_SMGR_IND_INVALID,
                              algoInstPtr->inputData.memSize, indPtr->Item_len);
               return SNS_ERR_FAILED;
            }
            SNS_OS_MEMCOPY(algoInpPtr, indPtr->Item[j].ItemData, itemSize);

            algoInpPtr += itemSize;
            algoInpSize -= itemSize;
         }

         //execute the algorithm
         sns_scm_algo_dbase[algoInstPtr->algoIndex]->algoApi.
         sns_scm_algo_update(algoInstPtr->stateData.memPtr,
                             algoInstPtr->inputData.memPtr,
                             algoInstPtr->outputData.memPtr);

         if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_GYRO_CAL_SVC)
         {
            gyro_cal_output_s* gyroCalResult = algoInstPtr->outputData.memPtr;
            gyro_cal_state_s* gyroCalState = algoInstPtr->stateData.memPtr;

            if (gyroCalResult->motionState == GYRO_CAL_MOT_STATE_REST)
            {
               sns_scm_process_algo_result(algoInstId);
            }

            sns_scm_log_algo_result(algoInstId, timestamp);

            if (gyroCalResult->motionState == GYRO_CAL_MOT_STATE_UNKNOWN)
            {
               continue;
            }

            SNS_SCM_DEBUG3(LOW, DBG_SCM_GYRO_CAL_REQ_INFO,
                           (int32_t)(gyroCalState->variance[0] >> 32),
                           (int32_t)(gyroCalState->variance[1] >> 32),
                           (int32_t)(gyroCalState->variance[2] >> 32));
            SNS_SCM_DEBUG3(LOW, DBG_SCM_GYRO_CAL_REQ_INFO,
                           (int32_t)(gyroCalState->variance[0]),
                           (int32_t)(gyroCalState->variance[1]),
                           (int32_t)(gyroCalState->variance[2]));

            //stop sensor data
            sns_scm_stop_sensor_data(algoInstId);
            if (sns_scm_data_req_dbase[dataReqId] == NULL)
            {
               break;
            }
         }
      }
      else if (sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId == SNS_SCM_MAG_CAL_SVC)
      {
         bool hard_reset = false;
         qmag_cal_input_s *inp = (qmag_cal_input_s *)(algoInstPtr->inputData.memPtr);
         qmag_cal_output_s *outp = (qmag_cal_output_s *)(algoInstPtr->outputData.memPtr);

         // Peak Detector
         if (outp->accuracy_HI >= QMAG_CAL_MIN_HI_GOOD_ACCURACY)
         {
            // convert raw -> calibrated sample and compute its norm squared
            uint32_t col;
            float cal_mag_norm_sq = 0.0;
            void *statePtr = NULL;

            for(col=0; col<QMAG_CAL_COL; col++)
            {
               cal_mag_norm_sq += (float)FX_FIXTOFLT_Q16(indPtr->Item[0].ItemData[col] - outp->offset_HI[col]) *
                  (float)FX_FIXTOFLT_Q16(indPtr->Item[0].ItemData[col] - outp->offset_HI[col]);
            }

            if (cal_mag_norm_sq > QMAG_CAL_HARD_RESET_THRESH_GAUSS_SQ)
            {
               hard_reset = true;
               SNS_SCM_PRINTF0(ERROR, "QMag Cal Hard Reset");

               statePtr = qmag_cal_scm_state_reset(algoInstPtr->configData.memPtr,
                                                   algoInstPtr->stateData.memPtr);

               if (statePtr == NULL)
               {
                  SNS_SCM_PRINTF0(ERROR, "QMag Cal State Reset Error");
               }

               outp->accuracy_HI = QMAG_CAL_ACCURACY_FAILED; 
               outp->offset_time = indPtr->Item[0].TimeStamp; // mag sample time
               outp->offset_updated = true;
            }
         }

         if (!hard_reset)
         {
            /* Update with Accel first */
            inp->input_type = QMAG_CAL_INPUT_TYPE_ACCEL;
            inp->sensor_time   = indPtr->Item[0].TimeStamp;
            inp->sensor_data[0] = indPtr->Item[1].ItemData[0];
            inp->sensor_data[1] = indPtr->Item[1].ItemData[1];
            inp->sensor_data[2] = indPtr->Item[1].ItemData[2];

            //SNS_SCM_PRINTF3(ERROR, "Executing MagCal: Accel Update %d %d %d",
            //                inp->sensor_data[0],
            //                inp->sensor_data[1],
            //                inp->sensor_data[2]);
            //execute the algorithm
            sns_scm_algo_dbase[algoInstPtr->algoIndex]->algoApi.
            sns_scm_algo_update(algoInstPtr->stateData.memPtr,
                                algoInstPtr->inputData.memPtr,
                                algoInstPtr->outputData.memPtr);

            /* Update with Mag second*/
            inp->input_type = QMAG_CAL_INPUT_TYPE_MAG;
            inp->sensor_time   = indPtr->Item[0].TimeStamp;
            inp->sensor_data[0] = indPtr->Item[0].ItemData[0];
            inp->sensor_data[1] = indPtr->Item[0].ItemData[1];
            inp->sensor_data[2] = indPtr->Item[0].ItemData[2];

            //SNS_SCM_PRINTF3(ERROR, "Executing MagCal: Mag Update %d %d %d",
            //                inp->sensor_data[0],
            //                inp->sensor_data[1],
            //                inp->sensor_data[2]);
            //execute the algorithm
            sns_scm_algo_dbase[algoInstPtr->algoIndex]->algoApi.
            sns_scm_algo_update(algoInstPtr->stateData.memPtr,
                                algoInstPtr->inputData.memPtr,
                                algoInstPtr->outputData.memPtr);
         }

         SNS_SCM_PRINTF3(MED, "OUTPUT OFFSETS: %d %d %d",
                outp->offset_HI[0],
                outp->offset_HI[1],
                outp->offset_HI[2]);

         SNS_SCM_PRINTF1(MED, "ACCURACY: %d",outp->accuracy_HI);

         if (outp->offset_updated)
         {
            sns_scm_process_algo_result(algoInstId);
         }

         sns_scm_log_algo_result(algoInstId, timestamp);
      }
      else
      {
         SNS_SCM_PRINTF2(ERROR, "No Execution, cannot determine algo", 0, 0);
      }
   }

   return SNS_SUCCESS;
}

bool sns_scm_enable_algo_in_qmd_state(uint8_t motion_state, 
                                      const sns_scm_algo_s* algo)
{
   switch (algo->motionTrigFlags)
   {
      case  MOTION_DETECT_FLAG_DONT_CARE:
      {
         return (true); // Algo does not depend on motion state, enable it
         break;
      }

      case  MOTION_DETECT_FLAG_ACTIVE_IN_REST:
      {
         return (motion_state == SNS_SAM_MOTION_REST_V01); // Algo wants to be in rest
         break;
      }

      case  MOTION_DETECT_FLAG_ACTIVE_IN_MOTION:
      {
         return (motion_state == SNS_SAM_MOTION_MOVE_V01); // Algo wants to be in rest
         break;
      }

      default:
      {
         // All possible values have been captured above - this is a 
         // catch all for any future expansions
         return(true);
      }

   }
}
/*=========================================================================
  FUNCTION:  sns_scm_process_sensor_status_ind
  =========================================================================*/
/*!
  @brief Processes sensor status indication received from sensors manager

  @param[i] smgrIndPtr: Pointer to sensor status indication message

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_sensor_status_ind(
   const void *smgrIndPtr)
{
   uint8_t i;
   bool found = false;
   sns_smgr_sensor_status_ind_msg_v01* indPtr
   = (sns_smgr_sensor_status_ind_msg_v01*)smgrIndPtr;

   SNS_SCM_PRINTF2(LOW, "SCM: sensor %d, status %d",
                  indPtr->SensorID, indPtr->SensorState);

   /*Update sensor state*/
   for (i=0; i<SNS_SCM_MAX_SNS_MON &&
       sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
   {
      if (sns_scm_sensor_dbase[i].sensorId == indPtr->SensorID)
      {
         sns_scm_sensor_dbase[i].status = indPtr->SensorState;
         found = true;
         sns_scm_reg_sensor_status_hyst_timer(i);
         break;
      }
   }

   if (!found)
   {
      SNS_SCM_PRINTF1(LOW, "SCM: Was not monitoring sensor %d, bailing",
                     indPtr->SensorID);
      return SNS_ERR_FAILED;
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_get_algo_handle
  =========================================================================*/
/*!
  @brief Get the handle to the specified algorithm in the algorithm database

  @param[i] algoSvcId: algorithm service id

  @return handle to the specified algorithm if found,
          NULL otherwise
*/
/*=======================================================================*/
sns_scm_algo_s* sns_scm_get_algo_handle(
   uint8_t algoSvcId)
{
   uint8_t i;

   for (i = 0; i < SNS_SCM_NUM_ALGO_SVCS && sns_scm_algo_dbase[i] != NULL; i++)
   {
      if (sns_scm_algo_dbase[i]->serviceId == algoSvcId)
      {
         return sns_scm_algo_dbase[i];
      }
   }
   SNS_SCM_DEBUG1(ERROR, DBG_SCM_GET_ALGO_INDX_ERR, algoSvcId);

   return NULL;
}

/*=========================================================================
  FUNCTION:  sns_scm_process_msg
  =========================================================================*/
/*!
  @brief Process the messages from SCM input message queue

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_process_msg(void)
{
  scm_q_item_s           *msgPtr      = NULL;
  sns_err_code_e         error_code   = SNS_SUCCESS;

  while (NULL != (msgPtr = (scm_q_item_s *)sns_scm_q_get(SCM_SAM_SMGR_MSG_SIG)))
  {
    error_code   = SNS_SUCCESS;

    SNS_SCM_PRINTF2(LOW, "SCM: got msg_id %d, scm_msg_type %d",
                   msgPtr->msg_id, msgPtr->scm_msg_type);

    /* process SCM - SMGR indications */
    if (msgPtr->scm_msg_type == SCM_SMGR_IND_MSG)
    {
      if (msgPtr->msg_id == SNS_SMGR_BUFFERING_IND_V01)
      {
        error_code = sns_scm_process_smgr_buffering_ind( msgPtr->body_ptr);
      }
      else if (msgPtr->msg_id == SNS_SMGR_REPORT_IND_V01)
      {
        error_code = sns_scm_process_smgr_ind( msgPtr->body_ptr);
      }
      else if (msgPtr->msg_id == SNS_SMGR_SENSOR_STATUS_IND_V01)
      {
        error_code = sns_scm_process_sensor_status_ind( msgPtr->body_ptr);
      }
      else
      {
        SNS_SCM_PRINTF0(ERROR, "data in SCM_SMGR_IND_MSG_SIG is invalid");
      }
    }
    /* process SCM - SAM indications */
    else if (msgPtr->scm_msg_type == SCM_SAM_IND_MSG)
    {
      error_code = sns_scm_process_sam_ind(msgPtr->body_ptr);
    }
    /* process SCM - SMGR responses */
    else if (msgPtr->scm_msg_type == SCM_SMGR_RESP_MSG)
    {
      error_code = sns_scm_process_smgr_resp(msgPtr->body_ptr, msgPtr->msg_id);
    }
    /* process SCM - SAM responses */
    else if (msgPtr->scm_msg_type == SCM_SAM_RESP_MSG)
    {
      error_code = sns_scm_process_sam_resp(msgPtr->body_ptr, msgPtr->msg_id);
    }
    else
    {
      SNS_SCM_PRINTF1(ERROR, "Invalid SCM message type %d", msgPtr->scm_msg_type);
    }

    if (error_code != SNS_SUCCESS)
    {
      SNS_SCM_PRINTF3(ERROR,
                      "SCM: msg processing error %d, msg_id %d, scm_msg_type %d",
                      error_code, msgPtr->msg_id, msgPtr->scm_msg_type);
    }

    SNS_OS_FREE(msgPtr->body_ptr);
    SNS_OS_FREE(msgPtr);
    msgPtr = NULL;
  }
   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_algo_svc
  =========================================================================*/
/*!
  @brief Register the algorithm with SCM. This is expected to be done
         at SCM initialization for all algorithms to be handled by SCM

  @param[i] algoSvcId: Algorithm service id

  @return Sensors error code
*/
/*=======================================================================*/
static sns_err_code_e sns_scm_reg_algo_svc(
   uint8_t algoSvcId)
{
   uint8_t algoIndex,j;
   for (algoIndex = 0;
       algoIndex < SNS_SCM_NUM_ALGO_SVCS && sns_scm_algo_dbase[algoIndex] != NULL;
       algoIndex++)
   {
      if (sns_scm_algo_dbase[algoIndex]->serviceId == algoSvcId)
      {
         SNS_SCM_DEBUG1(ERROR, DBG_SCM_REG_ALGO_ERR, algoSvcId);
         return SNS_SUCCESS;
      }
   }

   if (algoIndex >= SNS_SCM_NUM_ALGO_SVCS)
   {
      return SNS_ERR_FAILED;
   }

   sns_scm_algo_dbase[algoIndex] = SNS_OS_MALLOC(SNS_SCM_DBG_MOD,
                                                 sizeof(sns_scm_algo_s));
   SNS_ASSERT(sns_scm_algo_dbase[algoIndex] != NULL);

   sns_scm_algo_dbase[algoIndex]->serviceId = algoSvcId;
   sns_scm_algo_dbase[algoIndex]->timer = NULL;
   sns_scm_algo_dbase[algoIndex]->timeout = false;
   sns_scm_algo_dbase[algoIndex]->defSensorReportRate = 0;

   for (j=0; j<SNS_SCM_MAX_REG_ITEMS_PER_ALGO; j++)
   {
      sns_scm_algo_dbase[algoIndex]->regItemType[j]=SNS_SCM_REG_ITEM_TYPE_NONE;
   }

   return(sns_scm_reg_algo(sns_scm_algo_dbase[algoIndex]));
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_algos
  =========================================================================*/
/*!
  @brief Register all algorithms

  @return None
*/
/*=======================================================================*/
static void sns_scm_reg_algos(void)
{
   //initialise the sensor status database with sensors to be monitored
   int k = 0;
   uint8_t snsIdx, algoIdx, IdIdx, algoCount = 0;

   /* Monitor Accel... */
   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_ACCEL_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_ACCEL_DYN_CAL_PARAMS_V02;
   k++;

   /* and Gyro...*/
   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_GYRO_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_GYRO_DYN_CAL_PARAMS_V02;
   k++;

   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_GYRO_DYN_CAL_PARAMS_V02;
   k++;

   sns_scm_reg_algo_svc(SNS_SCM_GYRO_CAL_SVC);
   algoCount++;

#ifdef SNS_SCM_MAG_CAL
   /* And Mag */
   sns_scm_sensor_dbase[k].sensorId = SNS_SMGR_ID_MAG_V01;
   sns_scm_sensor_dbase[k].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
   sns_scm_sensor_dbase[k].auto_cal_reg_id = SNS_REG_SCM_GROUP_MAG_DYN_CAL_PARAMS_V02;
   k++;

   sns_scm_reg_algo_svc(SNS_SCM_MAG_CAL_SVC);
   algoCount++;
#endif

   // Cross-checks and locates all sensors that should monitor this service
   for (snsIdx = 0; snsIdx < SNS_SCM_MAX_SNS_MON && snsIdx < k; snsIdx++)
   {
      sns_scm_sensor_dbase[snsIdx].monitorAlgoIndex = SNS_SCM_INVALID_ID;
      for (algoIdx = 0; algoIdx < algoCount; algoIdx++)
      {
         for (IdIdx = 0; IdIdx < sns_scm_algo_dbase[algoIdx]->monitorSensorCount; IdIdx++)
         {
            if (sns_scm_sensor_dbase[snsIdx].sensorId ==
                sns_scm_algo_dbase[algoIdx]->monitorSensorId[IdIdx])
            {
               sns_scm_sensor_dbase[snsIdx].monitorAlgoIndex = algoIdx;
            }
         }
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_smgr
  =========================================================================*/
/*!
  @brief Register sensors that are registered to at least one service.

  @return None
*/
/*=======================================================================*/
static void sns_scm_reg_smgr(void)
{
#ifndef SNS_QDSP_SIM
   uint8_t k;

   for (k = 0;
        k < SNS_SCM_MAX_SNS_MON &&
          sns_scm_sensor_dbase[k].sensorId != SNS_SCM_INVALID_ID;
        k++)
   {
      if(sns_scm_sensor_dbase[k].monitorAlgoIndex != SNS_SCM_INVALID_ID)
      {
         sns_scm_reg_sensor_status(sns_scm_sensor_dbase[k].sensorId);
      }
   }
#endif
}

/*=========================================================================
  FUNCTION:  sns_scm_handle_sensor_status_hyst_timeout
  =========================================================================*/
/*!
  @brief Handle the timeout indicating sensor status hysteresis period is complete

  @return None
*/
/*=======================================================================*/
static void sns_scm_handle_sensor_status_hyst_timeout(void)
{
   uint8_t sensorIndex,i,monitorAlgoIndex;
   sns_scm_sensor_s* sensorPtr;
   bool enable_algo;

   for (sensorIndex = 0; sensorIndex < SNS_SCM_MAX_SNS_MON; sensorIndex++)
   {
      sensorPtr = &sns_scm_sensor_dbase[sensorIndex];

      if (sensorPtr != NULL && sensorPtr->timer != NULL)
      {
         if (sensorPtr->timeout == true)
         {
            sensorPtr->timeout = false;
            monitorAlgoIndex = sensorPtr->monitorAlgoIndex;

            /* Look for the corresponding algo service */
            sns_scm_update_sensor_status(sns_scm_algo_dbase[monitorAlgoIndex]);

            /* The code below is custom designed for algos that depend on QMD state,
            we must bypass QMD specific logic for some algos */

            SNS_SCM_PRINTF2(LOW, "sensor status hysteresis timeout sensorId=%d overall_status=%d",
                            sensorPtr->sensorId, sns_scm_algo_dbase[monitorAlgoIndex]->monitorSensorStatus);

            switch (sns_scm_algo_dbase[monitorAlgoIndex]->monitorSensorStatus)
            {
               case SNS_SMGR_SENSOR_STATUS_ACTIVE_V01:
               {
                  if (sns_scm_qmd_inst_id == SNS_SCM_INVALID_ID)
                  {
                     for (i = 0; (i < SNS_SCM_NUM_ALGO_SVCS) && (sns_scm_algo_dbase[i] != NULL); i++)
                     {
                        if(sns_scm_algo_dbase[i]->enableAlgo   /* algo can be enabled */
                           && sns_scm_algo_dbase[i]->motionTrigFlags != MOTION_DETECT_FLAG_DONT_CARE) /* and needs AMD information */
                        {
                           sns_scm_send_qmd_start_req();
                           break;
                        }
                     }
                  }

                  enable_algo = sns_scm_enable_algo_in_qmd_state(sns_scm_qmd_state, sns_scm_algo_dbase[monitorAlgoIndex]);

                  SNS_SCM_PRINTF3(LOW, "SCM: Enable algo %d, %d, %d",
                                       sns_scm_algo_dbase[monitorAlgoIndex]->enableAlgo, 
                                       sensorPtr->sensorId, 
                                       enable_algo);
                  if ( sns_scm_algo_dbase[monitorAlgoIndex]->enableAlgo && enable_algo)
                  {
                     SNS_SCM_PRINTF2(LOW, "SCM: Enabling algo %d, for sensor %d", monitorAlgoIndex, sensorPtr->sensorId);
                     sns_scm_enable_algo(monitorAlgoIndex);
                  }
                  break; // end case SMGR_SENSOR_STATUS_ACTIVE
               }

               case SNS_SMGR_SENSOR_STATUS_IDLE_V01:
               {
                  uint8_t algoIndex, qmdAlgoCnt = sns_scm_algo_inst_count;

                  /* Checks all algo instances and disables algo if needed */
                  for (i=0; i < SNS_SCM_MAX_ALGO_INSTS && qmdAlgoCnt > 0; i++)
                  {
                     if (sns_scm_algo_inst_dbase[i] != NULL)
                     {
                        if (sns_scm_algo_inst_dbase[i]->algoIndex == monitorAlgoIndex)
                        {
                           qmdAlgoCnt--;
                           sns_scm_disable_algo(monitorAlgoIndex);
                           break;
                        }
                        else
                        {
                           algoIndex = sns_scm_algo_inst_dbase[i]->algoIndex;
                           if(sns_scm_algo_dbase[algoIndex]->motionTrigFlags == MOTION_DETECT_FLAG_DONT_CARE)
                           {
                              qmdAlgoCnt--;
                           }
                        }
                     }
                  }

                  if (sns_scm_qmd_inst_id != SNS_SCM_INVALID_ID)
                  {
                     if (qmdAlgoCnt == 0 &&
                      sns_scm_qmd_inst_id < SNS_SCM_QMD_PEND_ID)
                     {
                        if ( !sns_scm_active_qmd_client() )
                        {
                           sns_scm_send_qmd_stop_req();
                        }
                     }
                  }

                  // store auto cal param, algo state in registry
                  if (sns_scm_algo_dbase[monitorAlgoIndex] != NULL && sns_scm_algo_dbase[monitorAlgoIndex]->enableAlgo)
                  {
                     sns_scm_store_auto_cal_params_registry(monitorAlgoIndex);
                     sns_scm_store_auto_cal_state_registry(monitorAlgoIndex);
                  }
                  break; // end case SMGR_SENSOR_STATUS_IDLE
               }

               case SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01:
               {
                  if (sns_scm_algo_inst_count == 0 &&
                      sns_scm_qmd_inst_id < SNS_SCM_QMD_PEND_ID &&
                      sns_scm_qmd_state != SNS_SAM_MOTION_MOVE_V01 && 
                      sensorPtr->sensorId != SNS_SMGR_ID_MAG_V01)
                  {
                     if ( !sns_scm_active_qmd_client() )
                     {                   
                        sns_scm_send_qmd_stop_req();
                     }
                  }

                  /* Forcing the loop below only for the Mag */
                  for (i=0; i < SNS_SCM_MAX_ALGO_INSTS; i++)
                  {
                     if (sns_scm_algo_inst_dbase[i] != NULL)
                     {
                        if (sns_scm_algo_inst_dbase[i]->algoIndex == monitorAlgoIndex)
                        {
                           if(sensorPtr->sensorId == SNS_SMGR_ID_MAG_V01)
                           {
                              SNS_SCM_PRINTF2(LOW, "Disabling Mag Cal %d, %d",
                                                   0, 0);
                              sns_scm_disable_algo(monitorAlgoIndex);
                              break;
                           }
                           else if (sensorPtr->sensorId == SNS_SMGR_ID_GYRO_V01)
                           {
                              sns_scm_stop_sensor_data(i);
                              break;
                           }
                        }
                     }
                  }

                  break; // end case SMGR_SENSOR_STATUS_ONE_CLIENT
               }
               case SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01:
               default:
                  break;

            }

         }
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_handle_report_timeout
  =========================================================================*/
/*!
  @brief Handle the timeout indicating periodic report to client is due

  @return None
*/
/*=======================================================================*/
static void sns_scm_handle_report_timeout(void)
{
   uint8_t algoIndex;
   sns_scm_algo_s* algoPtr;

   for (algoIndex = 0; algoIndex < SNS_SCM_NUM_ALGO_SVCS; algoIndex++)
   {
      algoPtr = sns_scm_algo_dbase[algoIndex];

      if (algoPtr != NULL && algoPtr->timer != NULL)
      {
         if (algoPtr->timeout == true)
         {
            algoPtr->timeout = false;

            if (algoPtr->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 ||
                algoPtr->monitorSensorStatus == SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01)
            {
               uint8_t algoInstId = sns_scm_find_algo_inst(
                  algoPtr->serviceId,
                  algoPtr->defConfigData.memPtr);

               if (algoInstId < SNS_SCM_MAX_ALGO_INSTS)
               {
                  //reset algorithm state
                  algoPtr->algoApi.sns_scm_algo_reset(
                     sns_scm_algo_inst_dbase[algoInstId]->configData.memPtr,
                     sns_scm_algo_inst_dbase[algoInstId]->stateData.memPtr);

                  //start sensor data
                  sns_scm_req_sensor_data(algoInstId);
               }
            }
         }
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_init_cal_algos
  =========================================================================*/
/*!
  @brief Initialize calibration algorithms

  @return none
*/
/*=======================================================================*/
static void sns_scm_init_cal_algos(void)
{
   uint8_t i, j, numRegQuery = 0;
   sns_err_code_e err;

   /* Send registry requests for initial dynamic calibration parameters */
   for (i = 0; i < SNS_SCM_MAX_SNS_MON &&
        sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
   {
      if (sns_scm_sensor_dbase[i].auto_cal_reg_id != SNS_SCM_INVALID_ID &&
          ((sns_scm_sensor_dbase[i].sensorId == SNS_SMGR_ID_GYRO_V01) ||
           (sns_scm_sensor_dbase[i].sensorId == SNS_SMGR_ID_MAG_V01)))
      {
         err = sns_scm_req_reg_data(SNS_SCM_REG_ITEM_TYPE_GROUP,
                                    sns_scm_sensor_dbase[i].auto_cal_reg_id);

         if ( SNS_SUCCESS != err )
         {
            /* Could not send request message. Use default cal. Already set for
               default at start of init process */
            SNS_SCM_DEBUG3(ERROR, DBG_SCM_REG_REQ_FAIL,
                           sns_scm_sensor_dbase[i].sensorId,
                           sns_scm_sensor_dbase[i].auto_cal_reg_id,
                           err);
         }
         else
         {
            numRegQuery++;
         }
      }
   }

   /* Send registry requests for algorithm configuration parameters */
   for (i = 0;
       i < SNS_SCM_NUM_ALGO_SVCS && sns_scm_algo_dbase[i] != NULL; i++)
   {
      for ( j = 0;
            j < SNS_SCM_MAX_REG_ITEMS_PER_ALGO && sns_scm_algo_dbase[i]->regItemType[j] != SNS_SCM_REG_ITEM_TYPE_NONE;
            j++)
      {
         if (sns_scm_algo_dbase[i]->regItemType[j] != SNS_SCM_REG_ITEM_TYPE_NONE)
         {
            err = sns_scm_req_reg_data(sns_scm_algo_dbase[i]->regItemType[j],
                                       sns_scm_algo_dbase[i]->regItemId[j]);

            SNS_SCM_PRINTF2(LOW, "SCM Algo Init Registry Req Type:%d, Id:%d",
                            sns_scm_algo_dbase[i]->regItemType[j],
                            sns_scm_algo_dbase[i]->regItemId[j]);

            if ( SNS_SUCCESS != err )
            {
               /* Could not send request message. Use default configuration. */
               SNS_SCM_DEBUG3(ERROR, DBG_SCM_REG_REQ_FAIL,
                              sns_scm_algo_dbase[i]->serviceId,
                              sns_scm_algo_dbase[i]->regItemId[j],
                              err);
            }
            else
            {
               numRegQuery++;
            }
         }
      }
   }

   /* Process registry responses */
   if (numRegQuery)
   {
      if (sns_scm_reg_init_timer(sns_scm_sig_event) == SNS_SUCCESS)
      {
         sns_scm_process_init_events(numRegQuery, sns_scm_sensor_dbase);
      }

      sns_scm_dereg_init_timer();
   }
}

/*=========================================================================
  FUNCTION:  sns_scm_smgr_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR response callback

  @detail
  Handles the responses from SMGR to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_resp_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err)
{
  sns_scm_q_put(msg_id, resp_buf, SCM_SMGR_RESP_MSG, transp_err, 0, 0);
}

/*=========================================================================
  FUNCTION:  sns_scm_sam_resp_cb
  =========================================================================*/
/*!
  @brief SCM - SAM response callback

  @detail
  Handles the responses from SAM to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   resp_buf              Pointer to the response buffer
  @param[i]   resp_buf_len        Length of the response buffer
  @param[i]   user_cb_data       Pointer to the User-data
  @param[i]   transp_err           Error code

    @return None
*/
/*=======================================================================*/
void sns_scm_sam_resp_cb (
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *resp_buf,
   unsigned int resp_buf_len,
   void *user_cb_data,
   qmi_client_error_type transp_err)
{
  sns_scm_q_put(msg_id, resp_buf, SCM_SAM_RESP_MSG, transp_err, 0, 0);
}

/*=========================================================================
  FUNCTION:  sns_scm_smgr_ind_cb
  =========================================================================*/
/*!
  @brief SCM - SMGR indication callback

  @detail
  Handles the indications from SMGR to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_smgr_ind_cb(
   qmi_client_type user_handle,
   unsigned int msg_id,
   void *ind_buf,
   unsigned int ind_buf_len,
   void *user_cb_data)
{
   int32  decode_msg_len = 0;

   if (msg_id == SNS_SMGR_BUFFERING_IND_V01)
   {
      decode_msg_len = sizeof (sns_smgr_buffering_ind_msg_v01);
   }
   else if (msg_id == SNS_SMGR_REPORT_IND_V01)
   {
      decode_msg_len = sizeof (sns_smgr_periodic_report_ind_msg_v01);
   }
   else if (msg_id == SNS_SMGR_SENSOR_STATUS_IND_V01)
   {
      decode_msg_len = sizeof (sns_smgr_sensor_status_ind_msg_v01);
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "error in msg_id = %d", msg_id);

      return;
   }

   sns_scm_q_put(msg_id, ind_buf, SCM_SMGR_IND_MSG, QMI_NO_ERR, ind_buf_len, decode_msg_len);
}

/*=========================================================================
  FUNCTION:  sns_scm_sam_ind_cb
  =========================================================================*/
/*!
  @brief SCM - SAM indication callback

  @detail
  Handles the indications from SAM to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_sam_ind_cb(
  qmi_client_type user_handle,
  unsigned int msg_id,
  void *ind_buf,
  unsigned int ind_buf_len,
  void *user_cb_data)
{
   int32  decode_msg_len = 0;

   if (msg_id == SNS_SAM_AMD_REPORT_IND_V01)
   {
      decode_msg_len = sizeof(sns_sam_qmd_report_ind_msg_v01);
   }
   else
   {
      SNS_SCM_PRINTF1(ERROR, "error in msg_id = %d", msg_id);

      return;
   }

   sns_scm_q_put(msg_id, ind_buf, SCM_SAM_IND_MSG, QMI_NO_ERR, ind_buf_len, decode_msg_len);
}

/*=========================================================================
  FUNCTION:  sns_scm_reg_ind_cb
  =========================================================================*/
/*!
  @brief SCM - REG indication callback

  @detail
  Handles the indications from REG to SCM.

  @param[i]   user_handle         Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                Message ID
  @param[i]   ind_buf                Pointer to the indication buffer
  @param[i]   ind_buf_len          Length of the indication buffer
  @param[i]   user_cb_data       Pointer to the User-data

  @return None
*/
/*=======================================================================*/
void sns_scm_reg_ind_cb(
  qmi_client_type user_handle,
  unsigned int msg_id,
  void *ind_buf,
  unsigned int ind_buf_len,
  void *user_cb_data)
{
  /* Not used currently */
}

/*=========================================================================
  FUNCTION:  sns_scm_q_get
  =========================================================================*/
/*!
  @brief Get the SCM data from QUEUE

  @param[i] qflag: queue flag that is to be picked

  @return Queue data pointer
          NULL otherwise
*/
/*=======================================================================*/
void* sns_scm_q_get (OS_FLAGS qflag)
{
  void    *msg_ptr = NULL;
  uint8_t os_err   = 0;

  sns_os_mutex_pend(scm_mutex, 0, &os_err);
  if (qflag == SCM_SAM_SMGR_MSG_SIG)
  {
    msg_ptr = sns_q_get(&scm_sam_smgr_q);
  }
  else
  {
    msg_ptr = sns_q_get(&scm_reg_q);
  }
  os_err = sns_os_mutex_post(scm_mutex);

  return msg_ptr;
}

/*=========================================================================
  FUNCTION:  sns_scm_q_put
  =========================================================================*/
/*!
  @brief Put the SCM data in QUEUE

  @detail
  Puts the SCM adat in QUEUE

  @param[i]   msg_id                Message ID
  @param[i]   buffer                  Pointer to the message buffer
  @param[i]   scm_msg_type     SCM message type
  @param[i]   transp_err           Error code
  @param[i]   buf_len                Length of the message buffer
  @param[i]   decode_msg_len   Length of the message to be decoded

  @return None
*/
/*=======================================================================*/
void sns_scm_q_put(
   unsigned int msg_id,
   void *buffer,
   sns_scm_msg_type_e scm_msg_type,
   qmi_client_error_type transp_err,
   int32 buf_len,
   int32 decode_msg_len)
{
  uint8_t               os_err = 0;
  scm_q_item_s          *scm_q_ptr = NULL;
  void                  *decode_buf_ptr = NULL;
  void                  *scm_msg_buf_ptr = NULL;
  qmi_client_error_type qmi_err = 0;

  if (buffer == NULL)
  {
    SNS_SCM_PRINTF2(ERROR, "Null Buffer, scm_msg_type = %d, msg_id = %d",
                    scm_msg_type, msg_id);

    return;
  }

  switch (scm_msg_type)
  {
    case SCM_SAM_RESP_MSG:
    case SCM_SMGR_RESP_MSG:
    case SCM_REG_RESP_MSG:
      if (transp_err != QMI_NO_ERR)
      {
        SNS_OS_FREE(buffer);

        SNS_SCM_PRINTF3(ERROR,
                        "Transporatation Error = %d, scm_msg_type = %d, msg_id = %d",
                        transp_err, scm_msg_type, msg_id);
        return;
      }

      scm_msg_buf_ptr = buffer;
      break;
    case SCM_SAM_IND_MSG:
    case SCM_SMGR_IND_MSG:
      decode_buf_ptr = SNS_OS_MALLOC (SNS_SCM_DBG_MOD, decode_msg_len);

      if (decode_buf_ptr ==  NULL)
      {
        SNS_SCM_PRINTF1(ERROR, "Malloc Fail for requested size = %d",
                        decode_msg_len);
        return;
      }

      if (scm_msg_type == SCM_SMGR_IND_MSG)
      {
         qmi_err = qmi_client_message_decode(sns_scm_smgr_user_handle, QMI_IDL_INDICATION,
                                             msg_id, buffer, buf_len,
                                             decode_buf_ptr, decode_msg_len);
      }
      else
      {
        qmi_err = qmi_client_message_decode(sns_scm_sam_user_handle, QMI_IDL_INDICATION,
                                            msg_id, buffer, buf_len,
                                            decode_buf_ptr, decode_msg_len);
      }

      if (qmi_err != QMI_NO_ERR)
      {
         SNS_OS_FREE(decode_buf_ptr);

         SNS_SCM_PRINTF2(ERROR,
                         "qmi_client_message_decode fail error = %d, scm_msg_type = %d",
                         qmi_err, scm_msg_type);

         return;
      }

      scm_msg_buf_ptr = decode_buf_ptr;
      break;
    default:
      SNS_SCM_PRINTF1(ERROR, "Invalid scm_msg_type = %d", scm_msg_type);
      return;
  }

  scm_q_ptr = (scm_q_item_s *)
                 SNS_OS_MALLOC (SNS_SCM_DBG_MOD, sizeof(scm_q_item_s));
  if (scm_q_ptr == NULL)
  {
    SNS_OS_FREE(scm_msg_buf_ptr);

    SNS_SCM_PRINTF1(ERROR, "Malloc Fail for requested size = %d",
                    sizeof(scm_q_item_s));

    return;
  }

  scm_q_ptr->msg_id = msg_id;
  scm_q_ptr->body_ptr = scm_msg_buf_ptr;
  scm_q_ptr->scm_msg_type = scm_msg_type;
  sns_q_link(scm_q_ptr, &scm_q_ptr->q_link);

  sns_os_mutex_pend(scm_mutex, 0, &os_err);
  if (os_err != OS_ERR_NONE)
  {
    SNS_OS_FREE(scm_q_ptr->body_ptr);
    SNS_OS_FREE(scm_q_ptr);

    SNS_SCM_PRINTF1(ERROR, "Error acquiring mutex = %d", os_err);

    return ;
  }

  if (SCM_REG_RESP_MSG == scm_msg_type)
  {
    sns_q_put(&scm_reg_q, &scm_q_ptr->q_link);
  }
  else
  {
    sns_q_put(&scm_sam_smgr_q, &scm_q_ptr->q_link);
  }

  os_err = sns_os_mutex_post(scm_mutex);
  SNS_ASSERT( os_err == OS_ERR_NONE );

  if (SCM_REG_RESP_MSG == scm_msg_type)
  {
    sns_os_sigs_post (sns_scm_sig_event, SCM_REG_MSG_SIG, OS_FLAG_SET, &os_err);
  }
  else
  {
    sns_os_sigs_post (sns_scm_sig_event, SCM_SAM_SMGR_MSG_SIG, OS_FLAG_SET, &os_err);
  }
}

/*=========================================================================
  FUNCTION:  sns_scm_task
  =========================================================================*/
/*!
  @brief Sensors calibration manager task

  @detail
  All algorithms are executed in this task context.
  Waits on events primarily from sensors manager or client.

  @param[i] argPtr: pointer to task argument

  @return None
*/
/*=======================================================================*/
static void sns_scm_task(
   void *argPtr)
{
   int8_t                            i;
   OS_FLAGS                          sigFlags;
   uint8_t                           err;
   qmi_client_error_type             qmi_err;
   qmi_service_info                  scm_service_info_array = {{0}};
   unsigned int                      scm_num_entries;
   unsigned int                      scm_num_services;

   /* mutex create */
   scm_mutex = sns_os_mutex_create (SNS_SCM_MODULE_PRIORITY, &err);

   /* Queue Init */
   sns_q_init(&scm_sam_smgr_q);
   sns_q_init(&scm_reg_q);

   /* SCM - SAM & SMGR messages signal */
   sns_os_sigs_add(sns_scm_sig_event, SCM_SAM_SMGR_MSG_SIG);

   /* SCM - REG messages signal */
   sns_os_sigs_add(sns_scm_sig_event, SCM_REG_MSG_SIG);

   /* SCM - hysteresis timeout signal */
   sns_os_sigs_add(sns_scm_sig_event, SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG);

   /* SCM - SMGR */
   sns_os_sigs_add(sns_scm_sig_event, SCM_SMGR_QCSI_WAIT_SIG);
   sns_os_sigs_add(sns_scm_sig_event, SCM_SMGR_QCSI_TIMER_SIG);

   scm_smgr_os_params.ext_signal = NULL;
   scm_smgr_os_params.sig = SCM_SMGR_QCSI_WAIT_SIG;
   scm_smgr_os_params.timer_sig = SCM_SMGR_QCSI_TIMER_SIG;

   qmi_err = qmi_client_notifier_init(sns_smr_get_svc_obj(SNS_SMGR_SVC_ID_V01),
                                      &scm_smgr_os_params,
                                      &sns_scm_smgr_user_handle);
   if (QMI_NO_ERR != qmi_err)
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_notifier_init %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - SMGR qmi_client_notifier_init done");

   /* SCM - SAM */
   sns_os_sigs_add(sns_scm_sig_event, SCM_SAM_QCSI_WAIT_SIG);
   sns_os_sigs_add(sns_scm_sig_event, SCM_SAM_QCSI_TIMER_SIG);

   scm_sam_os_params.ext_signal = NULL;
   scm_sam_os_params.sig = SCM_SAM_QCSI_WAIT_SIG;
   scm_sam_os_params.timer_sig = SCM_SAM_QCSI_TIMER_SIG;

   qmi_err = qmi_client_notifier_init(sns_smr_get_svc_obj(SNS_SAM_AMD_SVC_ID_V01),
                                      &scm_sam_os_params,
                                      &sns_scm_sam_user_handle);
   if (QMI_NO_ERR != qmi_err)
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_notifier_init %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - SAM qmi_client_notifier_init done");

#ifndef ADSP_STANDALONE
   /* SCM - REG */
   sns_os_sigs_add(sns_scm_sig_event, SCM_REG_QCSI_WAIT_SIG);
   sns_os_sigs_add(sns_scm_sig_event, SCM_REG_QCSI_TIMER_SIG);

   scm_reg_os_params.ext_signal = NULL;
   scm_reg_os_params.sig = SCM_REG_QCSI_WAIT_SIG;
   scm_reg_os_params.timer_sig = SCM_REG_QCSI_TIMER_SIG;

   qmi_err = qmi_client_notifier_init(sns_smr_get_svc_obj(SNS_REG2_SVC_ID_V01),
                                      &scm_reg_os_params,
                                      &sns_scm_reg_user_handle);
   if (QMI_NO_ERR != qmi_err)
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_notifier_init %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - REG qmi_client_notifier_init done");
#endif /* ADSP_STANDALONE */

   //initialize algorithm database
   for (i = SNS_SCM_NUM_ALGO_SVCS-1; i >= 0; i--)
   {
      sns_scm_algo_dbase[i] = NULL;
   }

   //initialize algorithm instance database
   for (i = SNS_SCM_MAX_ALGO_INSTS-1; i >= 0; i--)
   {
      sns_scm_algo_inst_dbase[i] = NULL;
   }
   sns_scm_algo_inst_count = 0;

   //initialize sensor data request database
   for (i = SNS_SCM_MAX_DATA_REQS-1; i >= 0; i--)
   {
      sns_scm_data_req_dbase[i] = NULL;
   }
   sns_scm_data_req_count = 0;

   for (i = SNS_SCM_MAX_SNS_MON-1; i >= 0; i--)
   {
      sns_scm_sensor_dbase[i].sensorId = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].dataType = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].status = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].auto_cal_reg_id = SNS_SCM_INVALID_ID;
      sns_scm_sensor_dbase[i].timer = NULL;
      sns_scm_sensor_dbase[i].timeout = false;
   }

   //register algorithms
   sns_scm_reg_algos();

   /* SMGR services */
   QMI_CCI_OS_SIGNAL_WAIT(&scm_smgr_os_params, 0);
   QMI_CCI_OS_SIGNAL_CLEAR(&scm_smgr_os_params);
   qmi_client_release(sns_scm_smgr_user_handle);

   scm_num_entries = SNS_SCM_SMGR_NUM_ENTRIES;

   qmi_err = qmi_client_get_service_list(sns_smr_get_svc_obj(SNS_SMGR_SVC_ID_V01),
                                         &scm_service_info_array,
                                         &scm_num_entries,
                                         &scm_num_services);
   if ((QMI_NO_ERR != qmi_err) && (scm_num_entries != scm_num_services))
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_get_service_list %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - SMGR qmi_client_get_service_list done");

   qmi_err = qmi_client_init (&scm_service_info_array,
                              sns_smr_get_svc_obj(SNS_SMGR_SVC_ID_V01),
                              sns_scm_smgr_ind_cb, NULL, NULL,
                              &sns_scm_smgr_user_handle);
   if (QMI_NO_ERR != qmi_err)
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_init %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - SMGR qmi_client_init done");

   /* SAM services */
   QMI_CCI_OS_SIGNAL_WAIT(&scm_sam_os_params, 0);
   QMI_CCI_OS_SIGNAL_CLEAR(&scm_sam_os_params);
   qmi_client_release(sns_scm_sam_user_handle);

   scm_num_entries = SNS_SCM_SAM_NUM_ENTRIES;

   qmi_err = qmi_client_get_service_list(sns_smr_get_svc_obj(SNS_SAM_AMD_SVC_ID_V01),
                                         &scm_service_info_array,
                                         &scm_num_entries,
                                         &scm_num_services);
   if ((QMI_NO_ERR != qmi_err) && (scm_num_entries != scm_num_services))
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_get_service_list %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - SAM qmi_client_get_service_list done");

   qmi_err = qmi_client_init (&scm_service_info_array,
                              sns_smr_get_svc_obj(SNS_SAM_AMD_SVC_ID_V01),
                              sns_scm_sam_ind_cb, NULL, NULL,
                              &sns_scm_sam_user_handle);
   if (QMI_NO_ERR != qmi_err)
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_init %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - SAM qmi_client_init done");

#ifndef ADSP_STANDALONE
   /* REG services */
   QMI_CCI_OS_SIGNAL_WAIT(&scm_reg_os_params, 0);
   QMI_CCI_OS_SIGNAL_CLEAR(&scm_reg_os_params);
   qmi_client_release(sns_scm_reg_user_handle);

   scm_num_entries = SNS_SCM_REG_NUM_ENTRIES;

   qmi_err = qmi_client_get_service_list(sns_smr_get_svc_obj(SNS_REG2_SVC_ID_V01),
                                         &scm_service_info_array,
                                         &scm_num_entries,
                                         &scm_num_services);
   if ((QMI_NO_ERR != qmi_err) && (scm_num_entries != scm_num_services))
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_get_service_list %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - REG qmi_client_get_service_list done");

   qmi_err = qmi_client_init (&scm_service_info_array,
                              sns_smr_get_svc_obj(SNS_REG2_SVC_ID_V01),
                              sns_scm_reg_ind_cb, NULL, NULL,
                              &sns_scm_reg_user_handle);
   if (QMI_NO_ERR != qmi_err)
   {
      SNS_SCM_PRINTF1(ERROR, "error in qmi_client_init %d", qmi_err);
      return;
   }

   SNS_SCM_PRINTF0(LOW, "SCM - REG qmi_client_init done");
#endif /* ADSP_STANDALONE */

   // Detect SMGR Buffering support
   sns_scm_smgr_buffering_flag = sns_scm_detect_smgr_buffering( sns_scm_sig_event );

   if( sns_scm_smgr_buffering_flag )
   {
      // If buffering is supported, check if default sensor report rates are present in registry
      // A registry version check is sufficient to get this info.
      sns_scm_sensor_report_rate_available = sns_scm_check_sensor_report_rate( sns_scm_sig_event );
   }
   SNS_SCM_PRINTF2(LOW, "SCM: SMGR Buffering support:%d, Sensor report rates in registry:%d",
      sns_scm_smgr_buffering_flag, sns_scm_sensor_report_rate_available);

   //initialize calibration algorithms SCM to REG
   sns_scm_init_cal_algos();
   SNS_SCM_PRINTF0(LOW, "sns_scm_init_cal_algos done");

   //register SCM to SMGR for sensor status reports
   sns_scm_reg_smgr();
   SNS_SCM_PRINTF0(LOW, "sns_scm_reg_smgr done");

   sns_init_done();
   SNS_SCM_PRINTF0(LOW, "SCM init done");

#ifdef SNS_SCM_TEST_ENABLE
   SNS_SCM_PRINTF0(LOW, "start SCM uinit Tests");

   sns_scm_ext_test();

   SNS_SCM_PRINTF0(LOW, "sns_scm_ext_test test done");

   sns_scm_send_qmd_start_req();

   SNS_SCM_PRINTF0(LOW, "sns_scm_send_qmd_start_req test done");

   sns_scm_send_qmd_stop_req();

   SNS_SCM_PRINTF0(LOW, "sns_scm_send_qmd_stop_req test done");
#endif

   while (1)
   {
     //wait for event
     sigFlags = sns_os_sigs_pend(sns_scm_sig_event,
                                 SNS_SCM_SAM_SMGR_FLAGS,
                                 OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                 0,
                                 &err);

     SNS_ASSERT(err == 0);

     //timer event check
     if (sigFlags & SNS_SCM_REPORT_TIMER_SIG)
     {
        SNS_SCM_PRINTF0(LOW, "SCM : got the SNS_SCM_REPORT_TIMER_SIG signal");

        sns_scm_handle_report_timeout();
     }

     if (sigFlags & SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG)
     {
        SNS_SCM_PRINTF0(LOW, "SCM : got the SNS_SCM_SENSOR_STATUS_HYST_TIMER_SIG signal");

        sns_scm_handle_sensor_status_hyst_timeout();
     }


     if (sigFlags & SCM_SAM_SMGR_MSG_SIG)
     {
       sns_scm_process_msg();
     }

     if (sigFlags & SCM_REG_MSG_SIG)
     {
       sns_scm_process_reg_msg();
     }

     sigFlags &= (~SNS_SCM_SAM_SMGR_FLAGS);

     if (sigFlags != 0)
     {
        SNS_SCM_PRINTF1(ERROR, "error signal flag %d", sigFlags);
     }
   }
}

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*=========================================================================
  FUNCTION:  sns_scm_init
  =========================================================================*/
/*!
  @brief Sensors calibration manager initialization.
         Creates the SCM task and internal databases.

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_scm_init(void)
{
   uint8_t err;

   //initialize events
   sns_scm_sig_event = sns_os_sigs_create(SNS_SCM_REPORT_TIMER_SIG, &err);

   SNS_ASSERT(sns_scm_sig_event != NULL);

   //create the SCM task
   err = sns_os_task_create_ext(sns_scm_task,
                                NULL,
                                &sns_scm_task_stk[SNS_SCM_MODULE_STK_SIZE-1],
                                SNS_SCM_MODULE_PRIORITY,
                                SNS_SCM_MODULE_PRIORITY,
                                &sns_scm_task_stk[0],
                                SNS_SCM_MODULE_STK_SIZE,
                                (void *)0,
                                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                                (uint8_t *)"SNS_SCM");
   SNS_ASSERT(err == 0);

   return err;
}

/*=========================================================================
  FUNCTION:  sns_scm_get_sensor_index
  =========================================================================*/
/*!
  @brief Get the index into the sensor status database for specified sensor

  @param[i] sensorId: algorithm service id

  @return index for the specified sensor in database if found,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
uint8_t sns_scm_get_sensor_index(
   uint8_t sensorId)
{
   uint8_t i;

   for (i=0; i < SNS_SCM_MAX_SNS_MON &&
       sns_scm_sensor_dbase[i].sensorId != SNS_SCM_INVALID_ID; i++)
   {
      if (sns_scm_sensor_dbase[i].sensorId == sensorId)
      {
         return i;
      }
   }

   return SNS_SCM_INVALID_ID;
}

//retained for future use
#if 0

/*=========================================================================
  FUNCTION:  sns_scm_get_algo_index
  =========================================================================*/
/*!
  @brief Get the index into the algorithm database for the specified algorithm

  @param[i] algoSvcId: algorithm service id

  @return algorithm index for the specified algorithm if found,
          SNS_SCM_INVALID_ID otherwise
*/
/*=======================================================================*/
static uint8_t sns_scm_get_algo_index(
   uint8_t algoSvcId)
{
   uint8_t i;

   for (i = 0; i < SNS_SCM_NUM_ALGO_SVCS && sns_scm_algo_dbase[i] != NULL; i++)
   {
      if (sns_scm_algo_dbase[i]->serviceId == algoSvcId)
      {
         return i;
      }
   }
   SNS_SCM_DEBUG1(ERROR, DBG_SCM_GET_ALGO_INDX_ERR, algoSvcId);

   return SNS_SCM_INVALID_ID;
}

/*=========================================================================
  FUNCTION:  sns_scm_dereg_sensor_status
  =========================================================================*/
/*!
  @brief Send a request to sensors manager to stop sensor data received by
  specified algorithm

  @param[i] sensorId: sensor id

  @return None
*/
/*=======================================================================*/
static void sns_scm_dereg_sensor_status(
   uint8_t sensorId)
{
   sns_smr_header_s msgHdr;

   uint8_t msgSize = sizeof(sns_smgr_sensor_status_req_msg_v01);
   sns_smgr_sensor_status_req_msg_v01* msgPtr =
      (sns_smgr_sensor_status_req_msg_v01 *)sns_smr_msg_alloc(SNS_SCM_DBG_MOD,msgSize);
   SNS_ASSERT(msgPtr != NULL);

   msgHdr.src_module = SNS_SCM_MODULE;
   msgHdr.svc_num = SNS_SMGR_SVC_ID_V01;
   msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
   msgHdr.priority = SNS_SMR_MSG_PRI_LOW;
   msgHdr.msg_id = SNS_SMGR_SENSOR_STATUS_REQ_V01;
   msgHdr.body_len = msgSize;

   msgHdr.txn_id = 0;
   msgHdr.ext_clnt_id = 0;

   msgPtr->SensorID = sensorId;
   msgPtr->Action = SNS_SMGR_SENSOR_STATUS_DEL_V01;
   msgPtr->ReqDataTypeNum = 1;

   sns_smr_set_hdr(&msgHdr, msgPtr);
   if (sns_smr_send(msgPtr) != SNS_SUCCESS)
   {
      //free the message
      sns_smr_msg_free(msgPtr);
   }

   SNS_SCM_DEBUG3(MEDIUM, DBG_SCM_REQ_SNSR_STATUS_INFO,
                  SNS_SMGR_SENSOR_STATUS_DEL_V01,
                  sensorId,
                  1);
}

/*=========================================================================
  FUNCTION:  sns_scm_send_resp_msg
  =========================================================================*/
/*!
  @brief Sends response message for the specified request

  @param[i] reqMsgPtr: Pointer to request message for which
            response needs to be sent
  @param[i] respMsgPtr: Pointer to response message body,
            to be sent with header
  @param[i] respMsgBodyLen: Response message body length
            (excluding the header part)

  @return None
*/
/*=======================================================================*/
static void sns_scm_send_resp_msg(
   const uint8_t *reqMsgPtr,
   void *respMsgPtr,
   uint16_t respMsgBodyLen)
{
   sns_smr_header_s reqMsgHdr, respMsgHdr;

   sns_smr_get_hdr(&reqMsgHdr, reqMsgPtr);
   if (reqMsgHdr.msg_type == SNS_SMR_MSG_TYPE_REQ)
   {
      respMsgHdr.dst_module = reqMsgHdr.src_module;
      respMsgHdr.src_module = reqMsgHdr.dst_module;
      respMsgHdr.svc_num = reqMsgHdr.svc_num;
      respMsgHdr.msg_type = SNS_SMR_MSG_TYPE_RESP;
      respMsgHdr.priority = SNS_SMR_MSG_PRI_LOW;
      respMsgHdr.body_len = respMsgBodyLen;

      respMsgHdr.txn_id = reqMsgHdr.txn_id;
      respMsgHdr.ext_clnt_id = reqMsgHdr.ext_clnt_id;

      //request and response messages are assumed to have same message ids
      respMsgHdr.msg_id = reqMsgHdr.msg_id;

      sns_smr_set_hdr(&respMsgHdr, respMsgPtr);
      if (sns_smr_send(respMsgPtr) != SNS_SUCCESS)
      {
         //free the message
         sns_smr_msg_free(respMsgPtr);
      }

      SNS_SCM_DEBUG3(HIGH, DBG_SCM_SEND_RSP_INFOMSG,
                     reqMsgHdr.msg_id, reqMsgHdr.src_module, reqMsgHdr.svc_num);
   }
}
/*=========================================================================
  FUNCTION:  sns_scm_process_shutdown_msg
  =========================================================================*/
/*!
  @brief Process a shutdown message from SCM input message queue

  @return none
*/
/*=======================================================================*/
void sns_scm_process_shutdown_msg(void)
{
   uint8_t i;
   sns_smr_header_s msgHdr;
   sns_reg_group_write_req_msg_v02 *msgPtr;
   uint8_t msgSize = 0;
   sns_err_code_e err;
   int32_t * dataPtr;
   /*for all instantiated algorithms*/
   for ( i = 0; i < SNS_SCM_MAX_ALGO_INSTS && sns_scm_algo_inst_dbase[i] != NULL; i++ )
   {
      sns_scm_algo_inst_s * algoInstPtr = sns_scm_algo_inst_dbase[i];
      /*If the algorithm is gyro_cal */
      if ( SNS_SCM_GYRO_CAL_SVC == sns_scm_algo_dbase[algoInstPtr->algoIndex]->serviceId )
      {
         gyro_cal_output_s * gyroCalOutput = algoInstPtr->outputData.memPtr;
         msgSize = sizeof(sns_smgr_sensor_cal_req_msg_v01);
         msgPtr = (sns_reg_group_write_req_msg_v02 *) sns_smr_msg_alloc(SNS_SCM_DBG_MOD,msgSize);

         SNS_ASSERT(msgPtr != NULL);

         msgHdr.src_module = SNS_SCM_MODULE;
         msgHdr.svc_num = SNS_REG2_SVC_ID_V01;
         msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
         msgHdr.priority = SNS_SMR_MSG_PRI_LOW;
         msgHdr.msg_id = SNS_REG_GROUP_WRITE_REQ_V02;
         msgHdr.body_len = msgSize;

         /*Important Note: If not all 6 bias&scale values are set by a single algorithm, then they must
         all be sent as single write messages, not a group write*/

         msgPtr->group_id = SNS_REG_SCM_GROUP_GYRO_DYN_CAL_PARAMS_V02;
         msgPtr->data_len = 6 * sizeof(int32_t);
         dataPtr = (int32_t*)msgPtr->data;

         dataPtr[0] = gyroCalOutput->bias[0];
         dataPtr[1] = gyroCalOutput->bias[1];
         dataPtr[2] = gyroCalOutput->bias[2];
         dataPtr[3] = 65536;
         dataPtr[4] = 65536;
         dataPtr[5] = 65536;

         sns_smr_set_hdr(&msgHdr, msgPtr);
         err = sns_smr_send(msgPtr);
         if (err != SNS_SUCCESS)
         {
            //free the message
            sns_smr_msg_free(msgPtr);
         }
      }
   }
}

#endif

