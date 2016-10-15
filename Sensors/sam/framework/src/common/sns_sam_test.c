/*============================================================================
  FILE: sns_sam_test.c

  This file contains the Sensors Algorithm Manager implementation

  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-01-29  gju  Include different file for SMR utlity functions.
  2012-12-07  gju  Use updated SAM framework API
  2011-11-14  jhh  Updated alloc and free function calls to meet new API
  2011-07-05  sc   Re-formated with UNIX newline ending
  2011-06-24  ad   Cleanup deprecated SMGR API
  2011-06-10  sc   Moved SAM test suite to centralized unit test thread
  2011-05-25  ad   Updates for unit test on target
  2010-11-04  jtl  Removing GCC compiler warnings on LA_SIM.
  2010-10-06  br   Changed SMR API from sns_smr_q_register() to sns_smr_register()
  2010-09-02  ad   Fixed SAM test to account for mutex-task priority dependency
  2010-08-13  ad   Added support for test algorithm query 
  2010-08-01  ad   Added support for test algorithm disable 
  2010-07-20  ad   Added support for test algorithm enable 
  2010-07-13  ad   Added support for test manager indication report
  2010-07-09  ad   Added support for test manager response
  2010-07-02  ad   Added test manager task
  2010-06-17  ad   Initial version

============================================================================*/

#ifdef SNS_UNIT_TEST

#ifdef SNS_PCSIM
  #define SNS_SAM_LOG printf
#else
  #define SNS_SAM_LOG(_Format, ...)  
#endif

//#define SAM_TEST_SENSOR_DATA
#define SAM_TEST_ALGO_ENABLE
#define SAM_TEST_ALGO_TOGGLE
#define SAM_TEST_ALGO_REPORT
#define SAM_TEST_ALGO_VERSION
#define SAM_TEST_ALGO_CANCEL

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "fixed_point.h"

#include "sns_init.h"
#include "sns_common.h"
#include "sns_em.h"
#include "sns_smr_util.h"

#include "sns_osa.h"
#include "sns_init.h"
#include "sns_test.h"

#include "sns_sam_priv.h"
#include "sns_memmgr.h"

#include "sns_common_v01.h"
#include "sns_smgr_api_v01.h"
#include "sns_sam_amd_v01.h"
#include "sns_sam_vmd_v01.h"
#include "sns_sam_rmd_v01.h"

/*---------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/

#define SNS_SAM_DEF_CLIENT_REPORT_PERIOD (FX_FLTTOFIX_Q16(1.0)) /*in secs*/

#define SNS_SAM_DEF_SVC_ID   SNS_SAM_AMD_SVC_ID_V01

/*---------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
* Global Data Definitions
* -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
* Static Variable Definitions
* -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
* Static Function Definitions
* -------------------------------------------------------------------------*/

#ifdef SAM_TEST_SENSOR_DATA
/*=========================================================================
  FUNCTION:  sns_sam_send_sensor_req
=========================================================================*/
/*!
    @brief
    send sensor request message to SMGR
 
    @param
    action: add or delete
    reportID: report ID
    sensorID: sensor ID
 
    @return
    None
*/
/*=======================================================================*/
void sns_sam_send_sensor_req(
   uint8_t action, 
   uint8_t reportID, 
   uint8_t sensorID)
{
   sns_smr_header_s msgHdr;
   
   uint8_t msgSize = sizeof(sns_smgr_periodic_report_req_msg_v01);
   
   sns_smgr_periodic_report_req_msg_v01 *msgPtr =
      (sns_smgr_periodic_report_req_msg_v01 *)sns_sam_msg_alloc(SNS_DBG_MOD_DSPS_SAM,msgSize);
   
   if (msgPtr)
   {
      msgPtr->ReportId = reportID;
      msgPtr->Action = action;
      msgPtr->ReportRate = 1;
      msgPtr->BufferFactor = 2;
      msgPtr->Item_len = 1;
      
      msgPtr->Item[0].SensorId = sensorID;
      msgPtr->Item[0].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      
      msgHdr.src_module = SNS_MODULE_DSPS_TEST;
      msgHdr.dst_module = SNS_MODULE_DSPS_SMGR;
      msgHdr.priority = SNS_SMR_MSG_PRI_LOW;
      
      msgHdr.txn_id = 0x11;
      msgHdr.ext_clnt_id = 0x22;
      msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
      msgHdr.svc_num = SNS_SMGR_SVC_ID_V01;
      msgHdr.msg_id = SNS_SMGR_REPORT_REQ_V01;
      msgHdr.body_len = msgSize;
      sns_smr_set_hdr(&msgHdr, msgPtr);
      sns_smr_send(msgPtr);
   }
}
#endif

#if defined(SAM_TEST_ALGO_ENABLE) || defined(SAM_TEST_ALGO_TOGGLE)
/*=========================================================================
  FUNCTION:  sns_sam_send_enable_req
=========================================================================*/
/*!
    @brief
    Sends message enabling specified service

    @return
    None
*/
/*=======================================================================*/
static void sns_sam_send_enable_req(
   uint32_t svcNum,
   uint8_t srcMod,
   uint32_t repPeriod)
{
   uint8_t *msgPtr;
   sns_smr_header_s msgHdr;
   
   msgHdr.src_module = srcMod;
   msgHdr.svc_num = (uint8_t)svcNum;

   msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
   msgHdr.msg_id = SNS_SAM_ALGO_ENABLE_REQ;
   msgHdr.priority = SNS_SMR_MSG_PRI_LOW;
   msgHdr.txn_id = 0;
   msgHdr.ext_clnt_id = 0;

   switch(svcNum)
   {
   case SNS_SAM_AMD_SVC_ID_V01:
      msgHdr.body_len = sizeof(sns_sam_amd_enable_req_msg_v01);

      msgPtr = (uint8_t *)sns_sam_msg_alloc(SNS_DBG_MOD_DSPS_SAM,sizeof(sns_sam_amd_enable_req_msg_v01));
      if (msgPtr != NULL)
      {
         ((sns_sam_amd_enable_req_msg_v01 *)msgPtr)->report_period = repPeriod;
      }
      break;

   case SNS_SAM_VMD_SVC_ID_V01:
   case SNS_SAM_RMD_SVC_ID_V01:
      msgHdr.body_len = sizeof(sns_sam_qmd_enable_req_msg_v01);

      msgPtr = (uint8_t *)sns_sam_msg_alloc(SNS_DBG_MOD_DSPS_SAM,sizeof(sns_sam_qmd_enable_req_msg_v01));
      if (msgPtr != NULL)
      {
         ((sns_sam_qmd_enable_req_msg_v01 *)msgPtr)->report_period = repPeriod;
      }
      break;

   default:
      SNS_SAM_LOG("received unsupported service id %d", svcNum);
      return;
   }

   sns_smr_set_hdr(&msgHdr, msgPtr);

   sns_smr_send(msgPtr);

   SNS_SAM_LOG("Sent enable request message id %d from module %d for service %d\n", 
      msgHdr.msg_id, msgHdr.src_module, msgHdr.svc_num);
}
#endif

#ifdef SAM_TEST_ALGO_TOGGLE
/*=========================================================================
  FUNCTION:  sns_sam_send_disable_req
=========================================================================*/
/*!
    @brief
    Sends message enabling specified service

    @return
    None
*/
/*=======================================================================*/
static void sns_sam_send_disable_req(
   uint32_t svcNum,
   uint8_t srcMod)
{
   uint8_t *msgPtr;
   sns_smr_header_s msgHdr;
   
   msgHdr.src_module = srcMod;
   msgHdr.svc_num = (uint8_t)svcNum;

   msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
   msgHdr.msg_id = SNS_SAM_ALGO_DISABLE_REQ;
   msgHdr.priority = SNS_SMR_MSG_PRI_LOW;
   msgHdr.txn_id = 1;
   msgHdr.ext_clnt_id = 0;

   switch(svcNum)
   {
   case SNS_SAM_AMD_SVC_ID_V01:
   case SNS_SAM_VMD_SVC_ID_V01:
   case SNS_SAM_RMD_SVC_ID_V01:
      msgHdr.body_len = sizeof(sns_sam_qmd_disable_req_msg_v01);

      msgPtr = (uint8_t *)sns_sam_msg_alloc(SNS_DBG_MOD_DSPS_SAM,sizeof(sns_sam_qmd_disable_req_msg_v01));
      if (msgPtr != NULL)
      {
         ((sns_sam_qmd_disable_req_msg_v01 *)msgPtr)->instance_id = 0;
      }
      break;

   default:
      SNS_SAM_LOG("received unsupported service id %d", svcNum);
      return;
   }

   sns_smr_set_hdr(&msgHdr, msgPtr);

   sns_smr_send(msgPtr);

   SNS_SAM_LOG("Sent disable request message id %d from module %d for service %d\n", 
      msgHdr.msg_id, msgHdr.src_module, msgHdr.svc_num);
}
#endif /* SAM_TEST_ALGO_TOGGLE */

#ifdef SAM_TEST_ALGO_REPORT
/*=========================================================================
  FUNCTION:  sns_sam_send_get_report_req
=========================================================================*/
/*!
    @brief
    Sends message enabling specified service

    @return
    None
*/
/*=======================================================================*/
static void sns_sam_send_get_report_req(
   uint32_t svcNum,
   uint8_t srcMod)
{
   uint8_t *msgPtr;
   sns_smr_header_s msgHdr;
   
   msgHdr.src_module = srcMod;
   msgHdr.svc_num = (uint8_t)svcNum;

   msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
   msgHdr.msg_id = SNS_SAM_ALGO_GET_REPORT_REQ;
   msgHdr.priority = SNS_SMR_MSG_PRI_LOW;
   msgHdr.txn_id = 1;
   msgHdr.ext_clnt_id = 0;

   switch(svcNum)
   {
   case SNS_SAM_AMD_SVC_ID_V01:
   case SNS_SAM_VMD_SVC_ID_V01:
   case SNS_SAM_RMD_SVC_ID_V01:
      msgHdr.body_len = sizeof(sns_sam_qmd_get_report_req_msg_v01);

      msgPtr = (uint8_t *)sns_sam_msg_alloc(SNS_DBG_MOD_DSPS_SAM,sizeof(sns_sam_qmd_get_report_req_msg_v01));
      if (msgPtr != NULL)
      {
         ((sns_sam_qmd_get_report_req_msg_v01 *)msgPtr)->instance_id = 0;
      }
      break;

   default:
      SNS_SAM_LOG("received unsupported service id %d", svcNum);
      return;
   }

   sns_smr_set_hdr(&msgHdr, msgPtr);

   sns_smr_send(msgPtr);

   SNS_SAM_LOG("Sent get_report request message id %d from module %d for service %d\n", 
      msgHdr.msg_id, msgHdr.src_module, msgHdr.svc_num);
}
#endif /* SAM_TEST_ALGO_REPORT */

#if defined(SAM_TEST_ALGO_VERSION) || defined(SAM_TEST_ALGO_CANCEL)
/*=========================================================================
  FUNCTION:  sns_sam_send_null_req
=========================================================================*/
/*!
    @brief
    Sends null message request with specified message id

    @return
    None
*/
/*=======================================================================*/
static void sns_sam_send_null_req(
   uint32_t svcNum,
   uint8_t srcMod,
   uint16_t msgId)
{
   uint8_t *msgPtr;
   sns_smr_header_s msgHdr;
   
   msgHdr.src_module = srcMod;
   msgHdr.svc_num = (uint8_t)svcNum;
   msgHdr.msg_id = msgId;

   msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
   msgHdr.priority = SNS_SMR_MSG_PRI_LOW;
   msgHdr.txn_id = 1;
   msgHdr.ext_clnt_id = 0;

   msgHdr.body_len = 0;
   msgPtr = (uint8_t *)sns_sam_msg_alloc(SNS_DBG_MOD_DSPS_SAM,0);

   sns_smr_set_hdr(&msgHdr, msgPtr);

   sns_smr_send(msgPtr);

   SNS_SAM_LOG("Sent null request message id %d from module %d for service %d\n", 
      msgHdr.msg_id, msgHdr.src_module, msgHdr.svc_num);
}
#endif /* defined(SAM_TEST_ALGO_VERSION) || defined(SAM_TEST_ALGO_CANCEL) */

static sns_common_resp_s_v01 sns_sam_qmd_result_buf[2];
static sns_sam_qmd_report_ind_msg_v01 sns_sam_qmd_report_buf[2];
static uint32_t sns_sam_qmd_report_count;
static uint32_t sns_sam_qmd_report_period;
 
/*=========================================================================
  FUNCTION:  sns_sam_test_main
=========================================================================*/
/*!
    @brief
    SAM test main

    @return
    None
*/
/*=======================================================================*/
void sns_sam_test_main(void)
{
   uint8_t err, i;
   OS_FLAGS sigFlags;
   uint8_t *msgPtr;
   sns_smr_header_s msgHdr;

#ifdef SAM_TEST_SENSOR_DATA
   sns_sam_send_sensor_req(SNS_SMGR_REPORT_ACTION_ADD_V01, 20, SNS_SMGR_ID_ACCEL_V01);
#endif

#ifdef SAM_TEST_ALGO_ENABLE
   sns_sam_send_enable_req(SNS_SAM_DEF_SVC_ID, 
                           SNS_MODULE_DSPS_TEST,
                           SNS_SAM_DEF_CLIENT_REPORT_PERIOD);
#endif

   for (i=0; ;i++)
   {
      sigFlags = sns_os_sigs_pend(sns_test_sig_grp, sns_test_sig_flag, OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
      if (sigFlags == 0)
      {
         SNS_SAM_LOG("ERROR: SAM test task stopped\n");
         return;
      }

      while (NULL != (msgPtr = (uint8_t*)sns_smr_rcv(SNS_MODULE_DSPS_TEST)))
      {
         err = sns_smr_get_hdr(&msgHdr, msgPtr);

         if (err == SNS_SUCCESS && msgHdr.src_module == SNS_SAM_MODULE)
         {
            switch (msgHdr.msg_id)
            {
            case SNS_SAM_ALGO_ENABLE_RESP:
               SNS_SAM_LOG("SAM TEST received algo enable response from SAM for service %d\n", 
                           msgHdr.svc_num);
               break;
            case SNS_SAM_ALGO_DISABLE_RESP:
               SNS_SAM_LOG("SAM TEST received algo disable response from SAM for service %d\n", 
                           msgHdr.svc_num);
               break;
            case SNS_SAM_ALGO_CANCEL_RESP:
               SNS_SAM_LOG("SAM TEST received algo cancel response from SAM for service %d\n", 
                           msgHdr.svc_num);
               break;
            case SNS_SAM_ALGO_VERSION_RESP:
               SNS_SAM_LOG("SAM TEST received algo version response from SAM for service %d\n", 
                           msgHdr.svc_num);
               break;
            case SNS_SAM_ALGO_GET_REPORT_RESP:
               {
                  switch (msgHdr.svc_num)
                  {
                  case SNS_SAM_AMD_SVC_ID_V01:
                  case SNS_SAM_VMD_SVC_ID_V01:
                  case SNS_SAM_RMD_SVC_ID_V01:
                     {
                        sns_sam_qmd_get_report_resp_msg_v01 *respPtr = 
                           (sns_sam_qmd_get_report_resp_msg_v01 *)msgPtr;

                        sns_sam_qmd_result_buf[1] = sns_sam_qmd_result_buf[0];
                        sns_sam_qmd_result_buf[0].sns_result_t = respPtr->resp.sns_result_t;
                        sns_sam_qmd_result_buf[0].sns_err_t = respPtr->resp.sns_err_t;

                        sns_sam_qmd_report_buf[1] = sns_sam_qmd_report_buf[0];
                        sns_sam_qmd_report_buf[0].instance_id = respPtr->instance_id;
                        sns_sam_qmd_report_buf[0].state = respPtr->state;
                        sns_sam_qmd_report_buf[0].timestamp = respPtr->timestamp;
                     }
                     SNS_SAM_LOG("algo instance id %d, algo state %d, report timestamp %d\n",
                                 sns_sam_qmd_report_buf[0].instance_id,
                                 sns_sam_qmd_report_buf[0].state,
                                 sns_sam_qmd_report_buf[0].timestamp);
                     SNS_SAM_LOG("result %d, error code %d\n",
                                 sns_sam_qmd_result_buf[0].sns_result_t,
                                 sns_sam_qmd_result_buf[0].sns_err_t);
                     break;
                  default:
                     break;
                  }
                  SNS_SAM_LOG("SAM TEST received algo get report response from SAM for service %d\n", 
                              msgHdr.svc_num);
               }
               break;
            case SNS_SAM_ALGO_ERROR_IND:
               SNS_SAM_LOG("SAM TEST received algo error indication from SAM for service %d\n", 
                           msgHdr.svc_num);
               break;
            case SNS_SAM_ALGO_REPORT_IND:
               {
                  SNS_SAM_LOG("SAM TEST received algo report indication from SAM for service %d\n", 
                              msgHdr.svc_num);
                  switch (msgHdr.svc_num)
                  {
                  case SNS_SAM_AMD_SVC_ID_V01:
                  case SNS_SAM_VMD_SVC_ID_V01:
                  case SNS_SAM_RMD_SVC_ID_V01:
                     {
                        sns_sam_qmd_report_ind_msg_v01 *indPtr =
                           (sns_sam_qmd_report_ind_msg_v01 *)msgPtr;

                        if (sns_sam_qmd_report_count & 1)
                        {
                           sns_sam_qmd_report_buf[1].instance_id = indPtr->instance_id;
                           sns_sam_qmd_report_buf[1].state = indPtr->state;
                           sns_sam_qmd_report_buf[1].timestamp = indPtr->timestamp;
                           sns_sam_qmd_report_period = (sns_sam_qmd_report_buf[1].timestamp -
                                                        sns_sam_qmd_report_buf[0].timestamp);
                        }
                        else
                        {
                           sns_sam_qmd_report_buf[0].instance_id = indPtr->instance_id;
                           sns_sam_qmd_report_buf[0].state = indPtr->state;
                           sns_sam_qmd_report_buf[0].timestamp = indPtr->timestamp;
                           sns_sam_qmd_report_period = (sns_sam_qmd_report_buf[0].timestamp -
                                                        sns_sam_qmd_report_buf[1].timestamp);
                        }

                        sns_sam_qmd_report_count++;
                     }
                     SNS_SAM_LOG("algo instance id %d, algo state %d, report timestamp %d\n",
                                 sns_sam_qmd_report_buf[0].instance_id,
                                 sns_sam_qmd_report_buf[0].state,
                                 sns_sam_qmd_report_buf[0].timestamp);
                     SNS_SAM_LOG("report count %d, report period %d\n",
                                 sns_sam_qmd_report_count, sns_sam_qmd_report_period);
                     break;
                  default:
                     break;
                  }
               }
               break;
            }
         }

		   SNS_SAM_LOG("SAM TEST received message id %d, from %d for service %d\n", 
            msgHdr.msg_id, msgHdr.src_module, msgHdr.svc_num);

         sns_sam_msg_free(msgPtr);
      }

      if (i >= 10)
      {
         SNS_SAM_LOG("\n===== SAM test finished =====\n\n");
         break;
      }

#ifdef SAM_TEST_ALGO_TOGGLE
      if (i & 1)
      {
         sns_sam_send_enable_req(SNS_SAM_DEF_SVC_ID, 
                                 SNS_MODULE_DSPS_TEST,
                                 SNS_SAM_DEF_CLIENT_REPORT_PERIOD);
      }
      else
      {
         sns_sam_send_disable_req(SNS_SAM_DEF_SVC_ID, SNS_MODULE_DSPS_TEST);
      }
#endif

#ifdef SAM_TEST_ALGO_REPORT
      if (i & 1)
      {
         sns_sam_send_get_report_req(SNS_SAM_DEF_SVC_ID, SNS_MODULE_DSPS_TEST);
      }
#endif

#ifdef SAM_TEST_ALGO_VERSION
      sns_sam_send_null_req(SNS_SAM_DEF_SVC_ID, 
                            SNS_MODULE_DSPS_TEST, 
                            SNS_SAM_ALGO_VERSION_REQ);
#endif
      
#ifdef SAM_TEST_ALGO_CANCEL
      sns_sam_send_null_req(SNS_SAM_DEF_SVC_ID, 
                            SNS_MODULE_DSPS_TEST, 
                            SNS_SAM_ALGO_CANCEL_REQ);
#endif 
 
#ifdef SNS_PCSIM
      OSTimeDlyHMSM(0, 0, 2, 0);
#endif
   }
}

#endif /* SNS_UNIT_TEST */
