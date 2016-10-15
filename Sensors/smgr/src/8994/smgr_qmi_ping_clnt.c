/******************************************************************************
  @file    sam_qmi_ping_clnt.c
  @brief   SAM QMI ping client

  DESCRIPTION
  Test client based on QMI ping client. Used to test SAM in ADSP only
  test scenario.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 
  ---------------------------------------------------------------------------
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-12-17  ps   Branched out from 8974 directory
  2013-08-09  ps   Eliminate compilier warnings
*******************************************************************************/
#include "comdef.h"
#include "msg.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "sns_smgr_api_v01.h"
#include "qurt.h"
#include "qurt_sclk.h"
#include "sns_smr_util.h"
#include "sns_common.h"
#include "sns_debug_api.h"
//#include "sns_smr_priv.h"
#include "sns_debug_str.h"
#include "sns_init.h"
#include "fixed_point.h"

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG 0x00000001
#define QMI_CLNT_IND_DONE_SIG 0x00000002
#define QMI_CLNT_ASYNC_DONE_SIG 0x00000004

#define QMI_PING_CLIENT_STACK_SIZE  (2048)
static uint8 sensor_id[] =
{
  SNS_SMGR_ID_ACCEL_V01,
  SNS_SMGR_ID_GYRO_V01,
  SNS_SMGR_ID_MAG_V01,
  SNS_SMGR_ID_PRESSURE_V01,
  SNS_SMGR_ID_PROX_LIGHT_V01,
  SNS_SMGR_ID_HUMIDITY_V01,  
  SNS_SMGR_ID_RGB_V01
};
#define NUMBER_OF_SENSOR_IDS	sizeof(sensor_id)/sizeof(sensor_id[0])

static qurt_thread_attr_t  smgr_qmi_ping_client_thread_attr;
static qurt_thread_t       smgr_qmi_ping_client_tcb;
static char                smgr_qmi_ping_client_stack[QMI_PING_CLIENT_STACK_SIZE];
static qurt_anysignal_t    smgr_qmi_ping_client_sig;
uint8_t TestSensorID=0;
uint8_t TestSensorsConcurrent=0;
uint32_t smgr_ping_ind_num=0;
uint32_t smgr_ping_clnt_freq=30;
/*=============================================================================
  CALLBACK FUNCTION smgr_ping_ind_cb
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
void smgr_ping_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  smgr_ping_ind_num++;
  switch (msg_id)
  {
    case SNS_SMGR_REPORT_IND_V01:
      {
        qmi_client_error_type err;
        sns_smgr_periodic_report_ind_msg_v01 ind_msg;
        uint32_t index;

        err = qmi_client_message_decode(user_handle,
                                        QMI_IDL_INDICATION,
                                        SNS_SMGR_REPORT_IND_V01,
                                        ind_buf, ind_buf_len,
                                        &ind_msg, sizeof(sns_smgr_periodic_report_ind_msg_v01));

        if (err == QMI_NO_ERR)
        {
          qurt_printf("\n SMGR Report IND: ReportID: %d, status: %d, CurrentRate: %d", ind_msg.ReportId,
                      ind_msg.status,ind_msg.CurrentRate);
          qurt_printf("\n SMGR Report IND: Item_len: %lu",ind_msg.Item_len);

          for (index = 0; index < ind_msg.Item_len; index++)
          {
            printf ("\n SMGR Report IND: Item: %d DataType: %d Ts: %lu\n", ind_msg.Item[index].SensorId,
                     ind_msg.Item[index].DataType, ind_msg.Item[index].TimeStamp);

            printf ("\n SMGR Report IND: ItemData[0]: %f ItemData[1]: %f ItemData[2]: %f\n",
			    FX_FIXTOFLT_Q16(ind_msg.Item[index].ItemData[0]),
                            FX_FIXTOFLT_Q16(ind_msg.Item[index].ItemData[1]),
			    FX_FIXTOFLT_Q16(ind_msg.Item[index].ItemData[2]));
          }
        }

      }
      break;

    default:
      break;
  }
}

/*=============================================================================
  CALLBACK FUNCTION smgr_ping_rx_cb
=============================================================================*/
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
/*=========================================================================*/
static void smgr_ping_rx_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *buf,
 unsigned int                   len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
 )
{
  /* Print the appropriate message based on the message ID */
  switch (msg_id)
  {
    case SNS_SMGR_REPORT_RESP_V01:
    {
          sns_smgr_periodic_report_resp_msg_v01 *resp_msg = (sns_smgr_periodic_report_resp_msg_v01*)buf;

          qurt_printf("\n SMGR Report Resp: Result: %d   Err: %d", resp_msg->Resp.sns_result_t, resp_msg->Resp.sns_err_t);
          qurt_printf("\n SMGR Report Resp: ReportID: %d   AckNak: %d ReasonPair_len: %lu", resp_msg->ReportId, resp_msg->AckNak, resp_msg->ReasonPair_len);
    }
      break;
    default:
      break;
  }
}

/*=============================================================================
  FUNCTION ping_data_test
=============================================================================*/
/*!
@brief
  This function sends a number of data ping messages asynchronously

@param[in]   clnt                Client handle needed to send messages

@param[in]   txn                 Transaction handle

@param[in]   num_pings           Number of data messages to send

@param[in]   msg_size            Size of data messages to send

*/
/*=========================================================================*/
int smgr_ping_data_test
(
 qmi_client_type *clnt,
 qmi_txn_handle *txn,
 int num_msgs,
 int msg_size
 )
{

  sns_smgr_periodic_report_req_msg_v01 *data_req;
  sns_smgr_periodic_report_resp_msg_v01 *data_resp;

  {
    int i,rc;

    data_req = (sns_smgr_periodic_report_req_msg_v01*)MALLOC(sizeof(sns_smgr_periodic_report_req_msg_v01));
    if(!data_req) {
      return -1;
    }
    data_resp = (sns_smgr_periodic_report_resp_msg_v01*)MALLOC(sizeof(sns_smgr_periodic_report_resp_msg_v01));
    if(!data_resp) {
      FREE(data_req);
      return -1;
    }
    memset( data_req, 0, sizeof(sns_smgr_periodic_report_req_msg_v01) );
    memset( data_resp, 0, sizeof(sns_smgr_periodic_report_resp_msg_v01) );
    data_req->Action = SNS_SMGR_REPORT_ACTION_ADD_V01;
    data_req->ReportId = 10;
    data_req->ReportRate = smgr_ping_clnt_freq;
    data_req->BufferFactor = 0;
    if (TestSensorsConcurrent)
    {
      uint8_t item_len=TestSensorsConcurrent;

      for (i = 0; i < NUMBER_OF_SENSOR_IDS; i++)
      {
        data_req->Item[i].SensorId = sensor_id[i];
        /*
        - 00 - SNS_SMGR_ID_ACCEL
        - 10 - SNS_SMGR_ID_GYRO
        - 20 - SNS_SMGR_ID_MAG
        - 30 - SNS_SMGR_ID_PRESSURE
        - 40 - SNS_SMGR_ID_PROX_LIGHT
        - 50 - SNS_SMGR_ID_HUMIDITY
		- 60 - SNS_SMGR_ID_RGB
        */
        data_req->Item[i].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
        data_req->Item[i].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
      }
      if (TestSensorsConcurrent == 5)
      {
        data_req->Item[5].SensorId = sensor_id[4];   /* PROX_LIGHT sensor */
        data_req->Item[5].DataType = SNS_SMGR_DATA_TYPE_SECONDARY_V01; /* LIGHT data*/
        data_req->Item[5].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
        item_len++;
      }

      data_req->Item_len = item_len;
    }
    else
    {
      data_req->Item[0].SensorId = TestSensorID;
      data_req->Item[0].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      data_req->Item[0].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
      data_req->Item_len = 1;
    }
    qurt_anysignal_clear(&smgr_qmi_ping_client_sig, QMI_CLNT_ASYNC_DONE_SIG);
    qurt_anysignal_clear(&smgr_qmi_ping_client_sig, QMI_CLNT_IND_DONE_SIG);

    for (i=0;i<num_msgs;++i)
    {
        rc = qmi_client_send_msg_async(*clnt, SNS_SMGR_REPORT_REQ_V01, data_req, sizeof(sns_smgr_periodic_report_req_msg_v01),
                                       data_resp, sizeof(sns_smgr_periodic_report_req_msg_v01), smgr_ping_rx_cb, (void *)2, txn);
        if (rc != 0){
          return -1;
        }
    }
  }

  /* Wait until all pending async messages have been received */
  qurt_anysignal_wait(&smgr_qmi_ping_client_sig, QMI_CLNT_IND_DONE_SIG); //wait forever
  qurt_anysignal_clear(&smgr_qmi_ping_client_sig, QMI_CLNT_IND_DONE_SIG);

  FREE(data_req);
  FREE(data_resp);
  return 0;
}

#ifdef SNS_SMGR_QMI_UNIT_TEST
static int smgr_qmi_ping_test_passed[] = {
0, 0, 0, 0, 0, 0
};
#endif

void smgr_qmi_ping_client_thread(void *unused)
{
  qmi_client_type clnt;
#ifdef SNS_SMGR_QMI_UNIT_TEST
  qmi_txn_handle txn;
#endif
  qmi_client_type notifier;
  unsigned int num_services, num_entries=10, i=0, num_services_old=0;
  int rc;
  qmi_cci_os_signal_type os_params;
  qmi_service_info info[10];
  qmi_idl_service_object_type ping_service_object = sns_smr_get_svc_obj(SNS_SMGR_SVC_ID_V01);

  os_params.ext_signal = NULL;
  os_params.sig = QMI_CLNT_WAIT_SIG;
  os_params.timer_sig = QMI_CLNT_TIMER_SIG;

  if (!ping_service_object)
  {
    SNS_PRINTF_STRING_ID_ERROR_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,0,0,0);
  }

  rc = qmi_client_notifier_init(ping_service_object, &os_params, &notifier);

  sns_init_done();

  /* Check if the service is up, if not wait on a signal */
  while(1)
  {
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
    QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

    /* The server has come up, store the information in info variable */
    num_entries=10;
    rc = qmi_client_get_service_list( ping_service_object, info, &num_entries, &num_services);

    if(rc != QMI_NO_ERR || num_services == num_services_old)
      continue;

    num_services_old = num_services;

    for(i = 0; i < num_services; i++)
    {
      rc = qmi_client_init(&info[i], ping_service_object, smgr_ping_ind_cb, NULL, &os_params, &clnt);

#ifdef SNS_SMGR_QMI_UNIT_TEST
      {
        /*
        qurt_elite_timer_t   tmr;
        qurt_elite_signal_t  *pSignal;
        int status;
        status = qurt_elite_timer_create(&tmr, QURT_ELITE_TIMER_ONESHOT_DURATION, QURT_ELITE_TIMER_USER, pSignal);
        */
        qurt_timer_sleep(100000); //100ms - let SMGR come up completely
        rc = smgr_ping_data_test(&clnt, &txn, 1, 1024);
        if(rc)
        {
          goto bail;
        }
        smgr_qmi_ping_test_passed[2] = 1;
      }
      bail:
#endif

      rc = qmi_client_release(clnt);
    }
  }
}

sns_err_code_e smgr_qmi_ping_client_start(void)
{
  sns_err_code_e err_code;

  qurt_anysignal_init(&smgr_qmi_ping_client_sig);
  qurt_thread_attr_init(&smgr_qmi_ping_client_thread_attr);
  qurt_thread_attr_set_name(&smgr_qmi_ping_client_thread_attr, "smgr_QMI_CLNT");
  qurt_thread_attr_set_priority(&smgr_qmi_ping_client_thread_attr, 10);
  qurt_thread_attr_set_stack_size(&smgr_qmi_ping_client_thread_attr, QMI_PING_CLIENT_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&smgr_qmi_ping_client_thread_attr, smgr_qmi_ping_client_stack);
  err_code = qurt_thread_create(&smgr_qmi_ping_client_tcb, &smgr_qmi_ping_client_thread_attr,
      smgr_qmi_ping_client_thread, NULL);
  return err_code;

}
