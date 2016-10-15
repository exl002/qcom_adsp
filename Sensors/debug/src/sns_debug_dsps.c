#define __SNS_MODULE__ SNS_DEBUG

#ifndef SNS_DEBUG_DSPS_C
#define SNS_DEBUG_DSPS_C

/*============================================================================

  @file sns_debug_dsps.c

  @brief
  This file contains the implementation of sensors debug and DIAG services.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/debug/src/sns_debug_dsps.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
2013-03-18   gju     Removed debug internal references and processing
2013-02-12   vh      Processing the QMI callback data in task loop
2013-01-29   gju     Include different file for SMR utlity functions.
2012-10-17   sc      Assigned sns_err_t value in response field
2012-09-13   vh      Eliminated compiler warnings
2012-08-08   ag      Initialize client handle to NULL; use right req_cb
2012-08-01   sc      Moved struct definition client_info_type to common header
2012-07-30   ag      Port debug (not internal) service
2012-06-27   ag      Initial Release

============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/

#include "sns_common.h"
#include "smd_lite.h"
#include "sns_em.h"
#include "sns_init.h"
#include "sns_memmgr.h"
#include "sns_osa.h"
#include "sns_queue.h"
#include "sns_common_v01.h"
#include "sns_diag_dsps_v01.h"
#include "sns_debug_interface_v01.h"
#include "sns_log_api.h"
#include "sns_debug_api.h"
#include "qmi_csi.h"
#include "qmi_csi_common.h"
#include "qmi_csi_target_ext.h"
#include "sns_debug_str.h"
#include <stdbool.h>
#include "sns_smr_util.h"

/*===========================================================================

                            INTERNAL DEFINITION AND TYPES

===========================================================================*/
#define SNS_DEBUG_CLBK_MSG_SIG       0x1
#define SNS_DIAG_MSG_SIG             0x2
#define SNS_DEBUG_MSG_SIG            0x4

#define SNS_DEBUG_SIG_ANY            (SNS_DIAG_MSG_SIG | \
                                      SNS_DEBUG_MSG_SIG | SNS_DEBUG_CLBK_MSG_SIG)

typedef enum
{
  SNS_DEBUG_DIAG_HANDLE = 0,
  SNS_DEBUG_DEBUG_HANDLE
} sns_debug_msg_e;

typedef struct debug_q_item_s
{
  sns_q_link_s    q_link;
  qmi_req_handle  req_handle;
  unsigned int    msg_id;
  sns_debug_msg_e debug_msg_type;
  void            *body_ptr;
} debug_q_item_s;

/*===========================================================================

                            STATIC VARIABLES

===========================================================================*/
qmi_client_handle debug_ind_client_handle;

//DEBUG task stack
static OS_STK sns_debug_task_stk[SNS_MODULE_STK_SIZE_DSPS_DEBUG];

/* signal event for debug task */
static OS_FLAG_GRP *sns_debug_sig_event;

/* Queue for QMI messages of Debug task */
sns_q_s debug_q;

/* Mutex for QMI messages of Debug task */
OS_EVENT *debug_mutex;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION sns_debug_put_msg

===========================================================================*/
/*!
@brief  This function puts the QMI messages in Queue.

@param[i]  connection_handle  Handle used by the infrastructure to identify different connections.
@param[i]  user_handle           Opaque handle used by the infrastructure to identify different services.
@param[i]  msg_id                  Message ID of the request
@param[i]  req_c_struct           Buffer holding the request data
@param[i]  req_c_struct_len     Length of the reuest data
@param[i]  service_cookie       Cookie value supplied by the client

@return QMI_CSI_CB_NO_MEM or QMI_CSI_CB_NO_ERR

*/
/*=========================================================================*/
static qmi_csi_cb_error sns_debug_put_msg
(
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  sns_debug_msg_e          debug_msg_type
)
{
  debug_q_item_s *debug_req_q_ptr = NULL;
  uint8_t        os_err = 0;

  debug_req_q_ptr = (debug_q_item_s *) SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_DIAG, sizeof(debug_q_item_s));

  if (debug_req_q_ptr == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : malloc fail");
    return QMI_CSI_CB_NO_MEM;
  }

  debug_req_q_ptr->req_handle = req_handle;
  debug_req_q_ptr->msg_id = msg_id;
  debug_req_q_ptr->debug_msg_type = debug_msg_type;

  if (req_c_struct_len != 0)
  {
    debug_req_q_ptr->body_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_DIAG, req_c_struct_len);

    if (debug_req_q_ptr->body_ptr == NULL)
    {
      SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : malloc fail");
      SNS_OS_FREE(debug_req_q_ptr);

      return QMI_CSI_CB_NO_MEM;
    }
    SNS_OS_MEMCOPY(debug_req_q_ptr->body_ptr, req_c_struct, req_c_struct_len);
  }
  else
  {
    debug_req_q_ptr->body_ptr = NULL;
  }

  sns_q_link(debug_req_q_ptr, &debug_req_q_ptr->q_link);

  sns_os_mutex_pend(debug_mutex, 0, &os_err);
  SNS_ASSERT( os_err == OS_ERR_NONE );

  sns_q_put(&debug_q, &debug_req_q_ptr->q_link);

  os_err = sns_os_mutex_post(debug_mutex);
  SNS_ASSERT( os_err == OS_ERR_NONE );

  sns_os_sigs_post (sns_debug_sig_event, SNS_DEBUG_CLBK_MSG_SIG, OS_FLAG_SET, &os_err );
  SNS_ASSERT( os_err == OS_ERR_NONE );

  return QMI_CSI_CB_NO_ERR;
}

/*===========================================================================

  CALLBACK FUNCTION debug_connect_cb

===========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives connect request for this client

@param[i]  client_handle         QMI client handle
@param[i]  service_cookie       Cookie value supplied by the client
@param[i]  connection_handle  Handle used by the infrastructure to identify different connections.

@return QMI_CSI_CB_CONN_REFUSED or QMI_CSI_NO_ERR

*/
/*=========================================================================*/
static qmi_csi_cb_error debug_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
)
{
  client_info_type *debug_client_info;

 /* Assign client_handle pointer to connection_handle so that it can be
    used in the handle_req_cb to send indications if necessary */
  if(!connection_handle)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : conncetion handle is null");
    return QMI_CSI_CB_CONN_REFUSED;
  }

  debug_client_info = SNS_OS_MALLOC(0, sizeof(client_info_type)); /* Freed in disconnect_cb */

  if(!debug_client_info)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : malloc fail");
    return QMI_CSI_CB_CONN_REFUSED;
  }
  debug_client_info->client_handle = client_handle;

  *connection_handle = (void*)debug_client_info;

  return QMI_CSI_NO_ERR;
}

/*===========================================================================

  CALLBACK FUNCTION debug_disconnect_cb

===========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives disconnect request for this client

@param[i]  connection_handle  Handle used by the infrastructure to identify different connections.
@param[i]  service_cookie       Cookie value supplied by the client

@return None

*/
/*=========================================================================*/
static void debug_disconnect_cb
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

/*===========================================================================

  CALLBACK FUNCTION diag_handle_req_cb

===========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives diag request for this client

@param[i]  connection_handle  Handle used by the infrastructure to identify different connections.
@param[i]  user_handle           Opaque handle used by the infrastructure to identify different services.
@param[i]  msg_id                  Message ID of the request
@param[i]  req_c_struct           Buffer holding the request data
@param[i]  req_c_struct_len     Length of the reuest data
@param[i]  service_cookie       Cookie value supplied by the client

@return returned value from sns_debug_put_msg()

*/
/*=========================================================================*/
static qmi_csi_cb_error diag_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
)
{
  return sns_debug_put_msg(req_handle,
                           msg_id,
                           req_c_struct,
                           req_c_struct_len,
                           SNS_DEBUG_DIAG_HANDLE);
}

/*===========================================================================

  CALLBACK FUNCTION debug_handle_req_cb

===========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an external request for this client

@param[i]  connection_handle  Handle used by the infrastructure to identify different connections.
@param[i]  user_handle           Opaque handle used by the infrastructure to identify different services.
@param[i]  msg_id                  Message ID of the request
@param[i]  req_c_struct           Buffer holding the request data
@param[i]  req_c_struct_len     Length of the reuest data
@param[i]  service_cookie       Cookie value supplied by the client

@return returned value from sns_debug_put_msg()

*/
/*=========================================================================*/
static qmi_csi_cb_error debug_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
)
{
  debug_ind_client_handle = ((client_info_type*)connection_handle)->client_handle;

  return sns_debug_put_msg(req_handle,
                           msg_id,
                           req_c_struct,
                           req_c_struct_len,
                           SNS_DEBUG_DEBUG_HANDLE);
}

/*=========================================================================

  FUNCTION:  sns_debug_diag_handle_req

  =========================================================================*/
/*!
  @brief Process the DIAG handle requests of Debug Task

  @param[i]   req_handle             Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                   Message ID
  @param[i]   req_c_struct           Pointer to the request buffer

  @return None
*/
/*=========================================================================*/
static void sns_debug_diag_handle_req
(
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct
)
{
  qmi_csi_error rc = QMI_CSI_CB_INTERNAL_ERR;

  if (SNS_DIAG_SET_LOG_MASK_REQ_V01 == msg_id)
  {
    sns_diag_set_log_mask_resp_msg_v01 resp;
    sns_diag_dsps_set_log_mask((sns_diag_set_log_mask_req_msg_v01*)req_c_struct);

    resp.resp.sns_result_t=0;          /*  0 == SUCCESS; 1 == FAILURE */
    resp.resp.sns_err_t = SENSOR1_SUCCESS;
    rc = qmi_csi_send_resp(req_handle, msg_id,
                           (void *)&resp, sizeof(sns_diag_set_log_mask_resp_msg_v01));
  }
  else if (SNS_DIAG_SET_DEBUG_MASK_REQ_V01 == msg_id)
  {
    sns_diag_set_debug_mask_resp_msg_v01 resp;
    sns_diag_dsps_set_debug_mask((sns_diag_set_debug_mask_req_msg_v01*)req_c_struct);

    resp.resp.sns_result_t=0;          /*  0 == SUCCESS; 1 == FAILURE */
    resp.resp.sns_err_t = SENSOR1_SUCCESS;
    rc = qmi_csi_send_resp(req_handle, msg_id,
                           (void *)&resp, sizeof(sns_diag_set_debug_mask_resp_msg_v01));
  }
  else if (SNS_DIAG_DSPS_CANCEL_REQ_V01 == msg_id)
  {
    sns_common_cancel_resp_msg_v01 resp;

    resp.resp.sns_result_t=0;          /*  0 == SUCCESS; 1 == FAILURE */
    resp.resp.sns_err_t = SENSOR1_SUCCESS;
    rc = qmi_csi_send_resp(req_handle, msg_id,
                           (void *)&resp, sizeof(sns_common_cancel_resp_msg_v01));
  }
  else if (SNS_DIAG_DSPS_VERSION_REQ_V01 == msg_id)
  {
    sns_common_version_resp_msg_v01 resp;

    resp.interface_version_number = SNS_DIAG_DSPS_SVC_V01_IDL_MINOR_VERS;
    resp.max_message_id = SNS_DIAG_SET_DEBUG_MASK_REQ_V01;
    resp.resp.sns_result_t=0;          /*  0 == SUCCESS; 1 == FAILURE */
    resp.resp.sns_err_t = SENSOR1_SUCCESS;
    rc = qmi_csi_send_resp(req_handle, msg_id,
                           (void *)&resp, sizeof(sns_common_version_resp_msg_v01));
  }

  if (rc != QMI_CSI_NO_ERR)
  {
    SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : qmi_csi_error value = %d", rc);
  }
}

/*=========================================================================

  FUNCTION:  sns_debug_debug_handle_req

  =========================================================================*/
/*!
  @brief Process the extrenal handle requests of Debug Task

  @param[i]   req_handle             Handle used by the infrastructure to identify different clients.
  @param[i]   msg_id                   Message ID
  @param[i]   req_c_struct           Pointer to the request buffer

  @return None
*/
/*=========================================================================*/
static void sns_debug_debug_handle_req
(
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct
)
{
  qmi_csi_error rc = QMI_CSI_CB_INTERNAL_ERR;

  if (SNS_DEBUG_VERSION_REQ_V01 == msg_id)
  {
    sns_common_version_resp_msg_v01 resp;

    resp.interface_version_number = SNS_DEBUG_SVC_V01_IDL_MINOR_VERS;
    resp.max_message_id = SNS_DEBUG_LOG_IND_V01;
    resp.resp.sns_result_t=0;          /*  0 == SUCCESS; 1 == FAILURE */
    resp.resp.sns_err_t = SENSOR1_SUCCESS;
    rc = qmi_csi_send_resp(req_handle, msg_id,
                           (void *)&resp, sizeof(sns_common_version_resp_msg_v01));

    if (rc != QMI_CSI_NO_ERR)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : qmi_csi_error value = %d", rc);
    }
  }
}


/*=========================================================================

  FUNCTION:  sns_debug_process_msg

  =========================================================================*/
/*!
  @brief This function processes the QMI messages of debug task

  @param[i]   None

  @return None
*/
/*=========================================================================*/
static void sns_debug_process_msg(void)
{
  debug_q_item_s *debug_req_msg = NULL;
  uint8_t        os_err   = 0;
  bool           loopit = true;

  while (loopit == true)
  {
    loopit = false;
    debug_req_msg = NULL;

    sns_os_mutex_pend(debug_mutex, 0, &os_err);
    SNS_ASSERT ( os_err == OS_ERR_NONE );

    debug_req_msg = sns_q_get(&debug_q);

    os_err = sns_os_mutex_post(debug_mutex);
    SNS_ASSERT ( os_err == OS_ERR_NONE );

    if (debug_req_msg != NULL)
    {
      if (debug_req_msg->debug_msg_type == SNS_DEBUG_DIAG_HANDLE)
      {
         sns_debug_diag_handle_req(debug_req_msg->req_handle,
                                   debug_req_msg->msg_id,
                                   debug_req_msg->body_ptr);
      }
      else if (debug_req_msg->debug_msg_type == SNS_DEBUG_DEBUG_HANDLE)
      {
        sns_debug_debug_handle_req(//debug_req_msg->connection_handle,
                                   debug_req_msg->req_handle,
                                   debug_req_msg->msg_id,
                                   debug_req_msg->body_ptr);
      }
      else
      {
        SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : Invalid message");
      }

      if (debug_req_msg->body_ptr != NULL)
      {
        SNS_OS_FREE(debug_req_msg->body_ptr);
      }
      SNS_OS_FREE(debug_req_msg);
      loopit = true;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_debug_task

===========================================================================*/
/*!
  @brief This function is DSPS Debug thread start routine.

  @param[i] arg_ptr A pointer to the argument

  @return
   None
*/
/*=========================================================================*/
void sns_debug_task (void *arg_ptr)
{
  static uint8_t                os_err;
  static qmi_csi_service_handle debug_service_handle;
  static qmi_csi_os_params      debug_os_params;
  static qmi_csi_service_handle diag_service_handle;
  static qmi_csi_os_params      diag_os_params;
  static OS_FLAGS               sig_flags;

  uint32_t debug_qcsi_service_ptr;
  uint32_t diag_qcsi_service_ptr;

  /* mutex create */
  debug_mutex = sns_os_mutex_create (SNS_MODULE_PRI_DSPS_DEBUG, &os_err);
  SNS_ASSERT ( os_err == OS_ERR_NONE );

  /* Queue Init */
  sns_q_init(&debug_q);

  sns_os_set_qmi_csi_params(sns_debug_sig_event, SNS_DEBUG_MSG_SIG,
                            &debug_os_params, &os_err);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  debug_qcsi_service_ptr = (uint32_t) qmi_csi_register(sns_smr_get_svc_obj(SNS_DEBUG_SVC_ID_V01),
                                                       debug_connect_cb,
                                                       debug_disconnect_cb,
                                                       debug_handle_req_cb,
                                                       NULL,
                                                       &debug_os_params,
                                                       &debug_service_handle);

  sns_os_set_qmi_csi_params(sns_debug_sig_event, SNS_DIAG_MSG_SIG,
                            &diag_os_params, &os_err);
  SNS_ASSERT(os_err == OS_ERR_NONE);

  diag_qcsi_service_ptr = (uint32_t) qmi_csi_register(sns_smr_get_svc_obj(SNS_DIAG_DSPS_SVC_ID_V01),
                                                       debug_connect_cb,
                                                       debug_disconnect_cb,
                                                       diag_handle_req_cb,
                                                       NULL,
                                                       &diag_os_params,
                                                       &diag_service_handle);

  sns_init_done();

  while (1)
  {
    sig_flags = sns_os_sigs_pend (sns_debug_sig_event,
                                  SNS_DEBUG_SIG_ANY,
                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
                                  0,
                                  &os_err);
    SNS_ASSERT(os_err == OS_ERR_NONE);

    if (sig_flags & SNS_DEBUG_MSG_SIG)
    {
      SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : SIG HANDLE THREAD - SNS_DEBUG_MSG_SIG");
      qmi_csi_handle_event(debug_service_handle, &debug_os_params);
    }

    if (sig_flags & SNS_DIAG_MSG_SIG)
    {
      SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : SIG HANDLE THREAD - SNS_DIAG_MSG_SIG");
      qmi_csi_handle_event(diag_service_handle, &diag_os_params);
    }

    if (sig_flags & SNS_DEBUG_CLBK_MSG_SIG)
    {
      SNS_PRINTF_STRING_LOW_0(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : SIG HANDLE THREAD - SNS_DEBUG_CLBK_MSG_SIG");
      sns_debug_process_msg();
    }
  }
}
/*=========================================================================
  FUNCTION:  sns_debug_init
  =========================================================================*/
/*!
  @brief Sensors debug module initialization.
         Creates the debug task.

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_debug_init(void)
{
   uint8_t err;

   SNS_PRINTF_STRING_LOW_1(SNS_DBG_MOD_DSPS_DIAG, "DEBUG : debug starting.... id(%d)", SNS_MODULE_DSPS_DEBUG);

   //initialize events
   //add two signals plz
   sns_debug_sig_event = sns_os_sigs_create((OS_FLAGS)SNS_DEBUG_CLBK_MSG_SIG, &err);
   sns_os_sigs_add(sns_debug_sig_event, SNS_DIAG_MSG_SIG);
   sns_os_sigs_add(sns_debug_sig_event, SNS_DEBUG_MSG_SIG);
   SNS_ASSERT(sns_debug_sig_event != NULL);

   //create the DEBUG task
   err = sns_os_task_create_ext(sns_debug_task,
                            NULL,
                            &sns_debug_task_stk[SNS_MODULE_STK_SIZE_DSPS_DEBUG-1],
                            SNS_MODULE_PRI_DSPS_DEBUG,
                            SNS_MODULE_PRI_DSPS_DEBUG,
                            &sns_debug_task_stk[0],
                            SNS_MODULE_STK_SIZE_DSPS_DEBUG,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                            (uint8_t *)"SNS_DEBUG");
   SNS_ASSERT(err == 0);

   return SNS_SUCCESS;
}
#endif  /* SNS_DEBUG_DSPS_C */
