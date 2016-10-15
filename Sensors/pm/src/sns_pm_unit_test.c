/*============================================================================

   @file sns_pm_unit_test.c

   @brief
   This file contains the implementation of the Sensors Power Manager
   Unit Tests

   @detail
   How to write Unit Tests for Power Manager:
   The input to Sensors power manager are the votes for active state change. 
   So the test framework can send vote messages on behalf of the voting modules. 

   The tests are fixed at compile time. The global array main_test_trigger
   contains the list of tests to run i.e a list of vote messages to send and
   when to send them. The vote messages can be sent after the power vote 
   response/indications comes back or at a random time. To mimick when to
   send the next vote message, main_test_trigger array contains vote messages
   seperated by instructions: SNS_PM_UT_WAIT_FOR_IND, SNS_PM_UT_WAIT_FOR_RESP,
   SNS_PM_UT_SETUP_TIMER. When the criteria is satisfied i.e vote resp comes back/
   vote ind comes back/ timer expires the next vote message is sent to the 
   Sensors Power Manager. So a sequence of tests in the main_test_trigger
   array can look like this:
   Send Vote Msg from SAM |  Wait for response | Send vote msg from SMGR |
   Send vote msg from SAM | Timer delay | Send vote msg from SAM |
   Wait for Indication

  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 

============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/pm/src/sns_pm_unit_test.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
01-29-2013   gju     Include different file for SMR utlity functions.
12-10-2012   ag      Initialize ext_signal to NULL
07-31-2012   ag      Cleanup macros
07-17-2012   ps      Added QCCI support
11-14-2011   jhh     Updated alloc and free function calls to meet new API
06-13-2011   sj      Making PM Unit test work with centralized test thread (SNS_UNIT_TEST)
05-20-2011   sj      Featurize tests on SNS_PM_TEST (for use in target bringup)
                     Introduce logging macro so that Test thread does not write to PM ULOG
04-25-2011   sj      Create unit test thread for sns power manager
03-30-2011   sj      First version of PM Unit Test

============================================================================*/

/*=====================================================================
                               INCLUDES
=======================================================================*/
#ifdef SNS_UNIT_TEST

#include "stdbool.h"   /* For true or false defn */
#include "sns_common_v01.h"
#include "sns_pm_api_v01.h"
#include "sns_init.h"
#include "sns_common.h"
#include "sns_smr_util.h"
//#include "sns_test.h"
#include "sns_pm_priv.h"

#ifdef SNS_QMI_ENABLE
#include "qmi_client.h"
#endif

/*=======================================================================
                   INTERNAL DATA TYPES
========================================================================*/
/* Possible test triggers */
typedef enum
{
  SNS_PM_UT_SEND_MESSAGE,
  SNS_PM_UT_WAIT_FOR_IND,
  SNS_PM_UT_WAIT_FOR_RESP,
  SNS_PM_UT_SETUP_TIMER
} sns_pm_ut_test_triggers;

typedef struct {
  sns_pm_ut_test_triggers trigger_type;
  union 
  {
    sns_pm_active_pwr_st_change_req_msg_v01 msg_trig;    
  } trigger_details;
} triggers;

/*========================================================================
                        GLOBAL VARIABLES
========================================================================*/
/* PM test sequence array */
static triggers main_test_trigger[10];

/* How many entries in the main_test_trigger array? */
static uint8_t num_tests = 0;

/* Current test that is being executed */
static uint8_t curr_test_id = 0;

static OS_STK sns_pm_test_stk[SNS_MODULE_STK_SIZE_DSPS_TEST];

static OS_FLAG_GRP *pm_test_sig_event;

#ifdef SNS_QMI_ENABLE
static qmi_client_os_params   pm_ut_os_params;
static qmi_client_type        pm_ut_handle;
#endif

/*===========================================================================
                               FUNCTIONS
===========================================================================*/
#ifdef SNS_QMI_ENABLE

/*============================================================================

  CALLBACK FUNCTION sns_pm_ut_ind_cb

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
void sns_pm_ut_ind_cb
(
  qmi_client_type pm_ut_handle,
  unsigned int msg_id,
  void *ind_buf,
  unsigned int ind_buf_len,
  void *ind_cb_data)
{
  switch(msg_id)
  {
   case SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01:
     {
        sns_pm_active_pwr_st_change_ind_msg_v01 vote_ind_msg;
        qmi_client_error_type err;

        err = qmi_client_message_decode(pm_ut_handle,
                                        QMI_IDL_INDICATION,
                                        SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01,
                                        ind_buf, ind_buf_len,
                                        &vote_ind_msg,
                                        sizeof(sns_pm_active_pwr_st_change_ind_msg_v01));
        if (err == QMI_NO_ERR)
        {
          PM_LOG_1("\nIndication msg header details: msg_id %d\n", msg_id);
          PM_LOG_1("\nPM State Indication value: %d ",vote_ind_msg.curr_active_state);
        }
	else
        {
          PM_LOG_0("\nError decoding Indication msgs");
        }

        /* Check if the next message needs to be sent */
        if (main_test_trigger[curr_test_id].trigger_type == SNS_PM_UT_WAIT_FOR_IND)
        {
          curr_test_id++;
          sns_pm_ut_send_msg();
        }
     }
     break;

   default:
     {
        PM_LOG_0("Unknown indication\n");
     }
     break;
  } // End of switch
}

/*=========================================================================
 
  CALLBACK FUNCTION:  sns_pm_ut_resp_cb

  =========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when 
  infrastructure receives an asynchronous response for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to 
                                 identify different services.
@param[in]   msg_id              Message ID of the response
@param[in]   resp_c_struct       Buffer holding the decoded response
@param[in]   resp_c_struct_len   Length of the decoded response
@param[in]   resp_cb_data        Cookie value supplied by the client
@param[in]   transp_err          Error value
 
*/
/*=======================================================================*/
void sns_pm_ut_resp_cb
(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void                    *resp_c_struct,
  unsigned int            resp_c_struct_len,
  void                    *resp_cb_data,
  qmi_client_error_type   transp_err
)
{

  switch (msg_id)
  {
   case SNS_PM_VERSION_RESP_V01:
    {
      PM_LOG_0("\nPM Version Response Received\n");
    }
    break;

   case SNS_PM_ACTIVE_PWR_ST_CHANGE_RESP_V01:
    { 
      sns_pm_active_pwr_st_change_resp_msg_v01  *vote_resp_ptr;
      vote_resp_ptr = (sns_pm_active_pwr_st_change_resp_msg_v01 *)resp_c_struct;

      PM_LOG_1("PM Vote response value: %d\n", vote_resp_ptr->resp.sns_result_t);
      /* Check if the next message needs to be sent */
      if (main_test_trigger[curr_test_id].trigger_type == SNS_PM_UT_WAIT_FOR_RESP)
      {
        curr_test_id++;
         sns_pm_ut_send_msg();
      }
    }
    break;

   default:
     break;
  }
}

/*===========================================================================

  FUNCTION:   sns_pm_ut_send_vote_msg

===========================================================================*/
/*!
  @brief
  Sends a vote message to Power Manager
   
  @param[i] vote_val     : Vote value
  @param[i] voting_module: Module making the vote

  @detail
  - Creates a message packet to send to power manager
  - Vote value and Voting module are parameters to the function. 
  Based on the parameters the same function can be reused for sending data 
  to power manager on behalf of both SAM and SMGR.
  
  @return
  Returns error code
  SNS_SUCCESS: If successful 
  Error code:  In case of any errors
 
*/
/*=========================================================================*/

static sns_err_code_e sns_pm_ut_send_vote_msg (sns_pm_active_pwr_st_change_req_msg_v01* req_msg,
                                               uint8_t voting_module)
{
   sns_pm_active_pwr_st_change_req_msg_v01   *req_vote_msg_ptr;
   sns_pm_active_pwr_st_change_resp_msg_v01  *vote_resp_ptr;
   uint8_t req_vote_msg_size = 0;
   uint8_t vote_resp_size = 0;
   void *resp_cb_data;
   qmi_client_error_type err;
   qmi_txn_handle txn_handle;

   req_vote_msg_size = sizeof(sns_pm_active_pwr_st_change_req_msg_v01);
   vote_resp_size = sizeof(sns_pm_active_pwr_st_change_resp_msg_v01);

   vote_resp_ptr = (sns_pm_active_pwr_st_change_resp_msg_v01 *)SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_PWR, vote_resp_size);
   SNS_ASSERT(vote_resp_ptr != NULL);

   req_vote_msg_ptr = req_msg;

   err = qmi_client_send_msg_async (pm_ut_handle,
                               SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01,
                               (void *)req_vote_msg_ptr, req_vote_msg_size,
                               (void *)vote_resp_ptr, vote_resp_size,
                               sns_pm_ut_resp_cb,
                               resp_cb_data, &txn_handle);
   if(err != QMI_NO_ERR)
   {
      printf("qmi_client_send_msg_async() failed, %d\n", err);
   }

  return err;
}

#else

/*===========================================================================

  FUNCTION:   sns_pm_ut_check_pm_indications

===========================================================================*/
/*!
  @brief
  Checks indications and responses from power manager.
   
  @param[i] vote_val     : Vote value
  @param[i] voting_module: Module making the vote

  @detail
  Checks the indications and responses returned from Power Manager.
  
  @return
  No return value
 
*/
/*=========================================================================*/
void sns_pm_ut_check_pm_indications(sns_smr_header_s *msg_in_hdr,
                                    void* msg_ind_ptr)
{
  sns_pm_active_pwr_st_change_ind_msg_v01   *vote_ind_ptr;
  sns_pm_active_pwr_st_change_resp_msg_v01  *vote_resp_ptr;

  SNS_ASSERT(msg_ind_ptr != NULL);

  if (msg_in_hdr->msg_id == SNS_PM_VERSION_RESP_V01)
  {
    // Check the response message
    PM_LOG_0("\nPM Version Response Received");
  }
  else if ( (msg_in_hdr->msg_id == SNS_PM_ACTIVE_PWR_ST_CHANGE_RESP_V01) &&
            (msg_in_hdr->msg_type == SNS_SMR_MSG_TYPE_RESP) )
  {
    vote_resp_ptr = msg_ind_ptr;
    // Check the response message
    PM_LOG_1("\nPM Vote response value: %d ",vote_resp_ptr->resp.sns_result_t);
    /* Check if the next message needs to be sent */
    if (main_test_trigger[curr_test_id].trigger_type == SNS_PM_UT_WAIT_FOR_RESP)
    {
      curr_test_id++;
      sns_pm_ut_send_msg();
    }
  }
  else if ((msg_in_hdr->msg_id == SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01) &&
           (msg_in_hdr->msg_type == SNS_SMR_MSG_TYPE_IND) )
  {
    vote_ind_ptr = msg_ind_ptr;
    // Check the indication message
    PM_LOG_1("\nIndication msg header details: msg_id %d",msg_in_hdr->msg_id);
    PM_LOG_1("\nPM State Indication value: %d ",vote_ind_ptr->curr_active_state);
    /* Check if the next message needs to be sent */
    if (main_test_trigger[curr_test_id].trigger_type == SNS_PM_UT_WAIT_FOR_IND)
    {
      curr_test_id++;
      sns_pm_ut_send_msg();
    }
  }

}

/*===========================================================================

  FUNCTION:   sns_pm_ut_send_vote_msg

===========================================================================*/
/*!
  @brief
  Sends a vote message to Power Manager
   
  @param[i] vote_val     : Vote value
  @param[i] voting_module: Module making the vote

  @detail
  - Creates a message packet to send to power manager
  - Vote value and Voting module are parameters to the function. 
  Based on the parameters the same function can be reused for sending data 
  to power manager on behalf of both SAM and SMGR.
  
  @return
  Returns error code
  SNS_SUCCESS: If successful 
  Error code:  In case of any errors
 
*/
/*=========================================================================*/
static sns_err_code_e sns_pm_ut_send_vote_msg(sns_pm_active_pwr_st_change_req_msg_v01* req_msg,
                                              uint8_t voting_module)
{
  sns_smr_header_s                           vote_req_out_hdr;
  sns_pm_active_pwr_st_change_req_msg_v01   *vote_msg_ptr;
  sns_err_code_e                            err_code = SNS_SUCCESS;


  // Filling the vote request message
  vote_msg_ptr = (sns_pm_active_pwr_st_change_req_msg_v01*) 
                 sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_PWR,sizeof(sns_pm_active_pwr_st_change_req_msg_v01));

  if (vote_msg_ptr == NULL)
  { 
    PM_LOG_0("Could not allocate memory for vote request message \n");
    return SNS_ERR_NOMEM;
  }

  vote_msg_ptr->vote_value    = req_msg->vote_value;  

  // Filling the SMR Request Header
  vote_req_out_hdr.dst_module  = SNS_MODULE_DSPS_PM;
  vote_req_out_hdr.src_module  = voting_module;
  vote_req_out_hdr.svc_num     = SNS_PM_SVC_ID_V01;
  vote_req_out_hdr.msg_id      = SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01;
  vote_req_out_hdr.msg_type    = SNS_SMR_MSG_TYPE_REQ;
  vote_req_out_hdr.body_len    = sizeof(sns_pm_active_pwr_st_change_req_msg_v01);
  vote_req_out_hdr.ext_clnt_id = 0;
  vote_req_out_hdr.txn_id      = 0;
  vote_req_out_hdr.priority    = SNS_SMR_MSG_PRI_LOW;
  
  // Send the Response back
  sns_smr_set_hdr(&vote_req_out_hdr, (void*)vote_msg_ptr);
  err_code = sns_smr_send((void*)vote_msg_ptr);
  if (err_code != SNS_SUCCESS)
  {  
    PM_LOG_1("Failed in sns_pm_proc_version_req due to following error code (%d)\n", err_code);
    // Free the memory
    sns_smr_msg_free((void*)vote_msg_ptr);
  }

  return err_code;

}

#endif


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
void sns_pm_ut_send_msg()
{

  while (curr_test_id <= num_tests)
  {
    switch (main_test_trigger[curr_test_id].trigger_type)
    {
      case SNS_PM_UT_SEND_MESSAGE:
        sns_pm_ut_send_vote_msg(&main_test_trigger[curr_test_id].trigger_details.msg_trig,
                                SNS_MODULE_DSPS_TEST);
        break;

      case SNS_PM_UT_WAIT_FOR_IND:
      case SNS_PM_UT_WAIT_FOR_RESP:
        /* Do nothing. Need to wait for response or indication from power manager before
         * sending the next message 
         */
        return;

      case SNS_PM_UT_SETUP_TIMER:

      break;

      default:
        break;

    } //end of switch
    curr_test_id++;
  } // end of while

}

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
void sns_pm_ut_init_tests()
{

   /* First test to trigger high power vote from SAM */
   main_test_trigger[num_tests].trigger_type = SNS_PM_UT_SEND_MESSAGE;
   main_test_trigger[num_tests].trigger_details.msg_trig.vote_value = SNS_PM_ACTIVE_HIGH;   
   num_tests++;

  /* Wait for the indication before proceeding */
  main_test_trigger[num_tests].trigger_type = SNS_PM_UT_WAIT_FOR_RESP;
  num_tests++;

   /* Second test to trigger high power vote from SMGR */
   main_test_trigger[num_tests].trigger_type = SNS_PM_UT_SEND_MESSAGE;
   main_test_trigger[num_tests].trigger_details.msg_trig.vote_value = SNS_PM_ACTIVE_HIGH;
   num_tests++;

  /* Wait for the indication before proceeding */
  main_test_trigger[num_tests].trigger_type = SNS_PM_UT_WAIT_FOR_IND;
  num_tests++;

  /* Third test to trigger low power vote from SMGR and SAM */
  main_test_trigger[num_tests].trigger_type = SNS_PM_UT_SEND_MESSAGE;
  main_test_trigger[num_tests].trigger_details.msg_trig.vote_value = SNS_PM_ACTIVE_LOW;
  num_tests++;
  main_test_trigger[num_tests].trigger_type = SNS_PM_UT_SEND_MESSAGE;
  main_test_trigger[num_tests].trigger_details.msg_trig.vote_value = SNS_PM_ACTIVE_LOW;
  num_tests++;

  /* Wait for the indication before proceeding */
  main_test_trigger[num_tests].trigger_type = SNS_PM_UT_WAIT_FOR_IND;

}

/*===========================================================================

  FUNCTION:   sns_pm_perform_ut

===========================================================================*/
/*!
  @brief
  Performs unit tests for power manager. 
   
  @param[i] p_arg : Pointer to arguments

  @detail
  This function does the following:
  - initializes the tests
  - triggers the first test
  - checks for indications received

  To enable the Power Manager Unit tests do the following:
  - Define feature SNS_UNIT_TEST in sns_common.h
  
  @return
  No return code
 
*/
/*=========================================================================*/
void sns_pm_perform_ut(void *p_arg)
{
#ifdef SNS_QMI_ENABLE
   qmi_idl_service_object_type sns_pm_service_object;
   qmi_service_info            sns_pm_service_info;
   qmi_client_error_type       err;
   qmi_client_type             sns_pm_ut_notifier;
   unsigned int                num_entries = 1;
   unsigned int                num_services;
#else
   int8_t   i;
   OS_FLAGS sigFlags;
   void     *msgPtr;
   sns_smr_header_s msgHdr;
   uint8_t err;
#endif

#ifdef SNS_QMI_ENABLE
   sns_pm_service_object = sns_smr_get_svc_obj(SNS_PM_SVC_ID_V01);
   pm_ut_os_params.ext_signal = NULL;
   pm_ut_os_params.sig = PM_QMI_WAIT_SIG;

   PM_LOG_0("PM TEST : qmi_client_notifier_init()\n");
   err = qmi_client_notifier_init(sns_pm_service_object,
                             &pm_ut_os_params,
                             &sns_pm_ut_notifier);
   if(err != QMI_NO_ERR)
   {
      PM_LOG_0("qmi_client_notifier_init() failed\n");
      return;
   }

   QMI_CCI_OS_SIGNAL_WAIT(&pm_ut_os_params, 0);
   QMI_CCI_OS_SIGNAL_CLEAR(&pm_ut_os_params);

   err = qmi_client_get_service_list(sns_pm_service_object, &sns_pm_service_info, 
                                      &num_entries, &num_services);

   if (err == QMI_NO_ERR)
   {
      err = qmi_client_init(&sns_pm_service_info, sns_pm_service_object,
                           sns_pm_ut_ind_cb, NULL, &pm_ut_os_params,
                           &pm_ut_handle);
      if(err != QMI_NO_ERR)
      {
         PM_LOG_0("qmi_client_init() failed\n");
         return;
      }
   }
#else
   PM_LOG_0("PM TEST : SMR sig registers\n");
   sns_smr_register(SNS_MODULE_DSPS_TEST, pm_test_sig_event, SNS_PM_MSG_SIG);
#endif

   /* Initialize the Power Manager Unit Test Suite */
   sns_pm_ut_init_tests();
   sns_pm_ut_send_msg();

#ifdef SNS_QMI_ENABLE
     /* TODO: Do QMI handle the signals/indications and call CBs */
#else
   for (i=0; ;i++)
   {
      sigFlags = sns_os_sigs_pend(pm_test_sig_event,
                                  0x1,
                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 
                                  0,
                                  &err);
      if (sigFlags == 0)
      {
         PM_LOG_0("\nERROR: PM Test Task Stopped");
         return;
      }

      while ( (msgPtr = (uint8_t*)sns_smr_rcv(SNS_MODULE_DSPS_TEST)) )
      {
         err = sns_smr_get_hdr(&msgHdr, msgPtr);
         /* Check Message */
         sns_pm_ut_check_pm_indications(&msgHdr,msgPtr);
         
         sns_smr_msg_free(msgPtr);
      } //end of while
   } // end of for loop
#endif
} //end of function sns_pm_perform_ut

/*===========================================================================

  FUNCTION:   sns_pm_ut_task_init

===========================================================================*/
/*!
  @brief
  Sensors Power Manager Unit Test task initialization. 
   
  @param[i]
  No input parameters

  @detail
  This function initializes the sensors power manager unit test
  i.e a new task for power measurement purposes is created.
  
  @return
  Returns sensors error code
  SNS_SUCCESS: If successful 
  Error code:  In case of any errors
 
*/
/*=========================================================================*/
sns_err_code_e sns_pm_ut_task_init(void)
{
   int8_t err;

   pm_test_sig_event = sns_os_sigs_create((OS_FLAGS)SNS_PM_MSG_SIG, &err);
   SNS_ASSERT(pm_test_sig_event != NULL);

   //create the PM Unit Test
   sns_os_task_create_ext(sns_pm_perform_ut,
                                NULL, 
                                &sns_pm_test_stk[SNS_MODULE_STK_SIZE_DSPS_TEST-1], 
                                SNS_MODULE_DSPS_TEST,
                                SNS_MODULE_DSPS_TEST,
                                &sns_pm_test_stk[0],
                                SNS_MODULE_STK_SIZE_DSPS_TEST,
                                (void *)0,
                                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                                "SNS_PowerTest");

   sns_init_done();
   return SNS_SUCCESS;
} //end of sns_pm_ut_task_init

#endif /* SNS_UNIT_TEST */
