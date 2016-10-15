/*============================================================================

   @file sns_pm_testtrigger.c

   @brief
   This file contains the implementation of the Sensors Power Manager
   Test trigger thread

   @detail
   The Test trigger thread is used for receiving responses and indications
   from requests triggered through DIAG for measuring power.
   The scenarios can be to ask for SMGR data from a particular sensor at a 
   particular rate OR Trigger QMD by placing a request to SAM etc.
   Its important to have an application resident on the DSPS to receive indications
   because we would like to measure the power consumed by the DSPS only
   when such requests are triggered.
   

Copyright (c) 2011,2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.


============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/pm/src/sns_pm_testtrigger.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
01-29-2013   gju     Include different file for SMR utlity functions.
11-18-2011   sc      Fix compilation warnings
11-14-2011   jhh     Updated alloc and free function calls to meet new API
09-16-2011   agk     Added ind_count_limit and made it static global.
07-12-2011   sj      Added code in function so that its not compiled out
06-14-2011   sj      First version of PM Test Trigger thread

============================================================================*/

/*=====================================================================
                               INCLUDES
=======================================================================*/
#include "sns_common.h"
#ifdef SNS_PM_TEST
#include "sns_osa.h"
#include "stdbool.h"   /* For true or false defn */
#include "sns_memmgr.h"
#include "sns_init.h"
#include "sns_common_v01.h"
#include "sns_smgr_api_v01.h" 
#include "sns_smr_util.h"
#ifndef SNS_PCSIM
#include "ULogFront.h"
#endif

/*=======================================================================
                   INTERNAL DATA TYPES
========================================================================*/
/* PM Signal and Signal Mask */
#define SNS_PM_TEST_SMR_MSG_SIG          0x1

/* ULOG Size */
#define SNS_PM_TEST_ULOG_SIZE 64

/*========================================================================
                        GLOBAL VARIABLES
========================================================================*/
/* Ulog handle */
#ifndef SNS_PCSIM
static ULogHandle    sns_pm_test_ulog_hdl;
static uint8_t       ulog_test_init_success;
#endif

/* PM Test Trigger Task Stack 
 * Use a small stack size ~100*4(4 is size of OS_STK type) bytes
 * We would want to use the test stack for on target power measurement test
 */
static OS_STK sns_pm_test_stk[SNS_MODULE_STK_SIZE_DSPS_PM_TEST];
static uint8_t ind_count = 0;
static uint8_t ind_count_limit = 30;

/*========================================================================
                        MACROS
========================================================================*/
#ifdef SNS_PCSIM
#define PM_TEST_LOG_0(a) { printf(a); printf ("\n"); }
#define PM_TEST_LOG_1(a,b) { printf(a,b); printf ("\n"); }
#define PM_TEST_LOG_2(a,b,c) { printf(a,b,c); printf ("\n"); }
#define PM_TEST_LOG_3(a,b,c,d) { printf(a,b,c,d); printf ("\n"); }
#else
/* Use ULOG for logging */
#define PM_TEST_LOG_0(a)   if (ulog_test_init_success) { ULOG_RT_PRINTF_0(sns_pm_test_ulog_hdl,a); }
#define PM_TEST_LOG_1(a,b) if (ulog_test_init_success) { ULOG_RT_PRINTF_1(sns_pm_test_ulog_hdl,a,b); }
#define PM_TEST_LOG_2(a,b,c) if (ulog_test_init_success) { ULOG_RT_PRINTF_2(sns_pm_test_ulog_hdl,a,b,c); }
#define PM_TEST_LOG_3(a,b,c,d) if (ulog_test_init_success) { ULOG_RT_PRINTF_3(sns_pm_test_ulog_hdl,a,b,c,d); }
#endif

/*===========================================================================
                               FUNCTIONS
===========================================================================*/
/*===========================================================================

  FUNCTION:   sns_pm_test_wait_to_collect_logs

===========================================================================*/
/*!
  @brief
  Provided to collect Ulogs for power measuremnt 
   
  @param[i] 
  No input parameters

  @detail
  This function is provided so that Ulogs can be collected on target.
  Ulogs are collected with Trace32.
  To collect Ulogs the processor should not be gated.Having such a function 
  helps test team collect logs.
  
  @return
  No return code
 
*/
/*=========================================================================*/
void sns_pm_test_wait_to_collect_logs()
{
  PM_TEST_LOG_0("Received 10 SMGR indications");
}

/*===========================================================================

  FUNCTION:   sns_pm_test_process_smgr_msg

===========================================================================*/
/*!
  @brief
  Process the messages received from SMGR 
   
  @param[i] smr_hdr_ptr  : Pointer to SMR header
  @param[i] msg_body_ptr : Pointer to Message
    

  @detail
  This function does the following:
  - finds out the message (response or indication) received from SMGR
  - logs the response
  
  @return
  No return code
 
*/
/*=========================================================================*/
void sns_pm_test_process_smgr_msg(sns_smr_header_s* smr_hdr_ptr,
                                  void* msg_body_ptr)
{
  switch (smr_hdr_ptr->msg_id)
  {
    case SNS_SMGR_REPORT_RESP_V01:      
      PM_TEST_LOG_1("Received SMGR Resp (err:%d)",
                    ((sns_smgr_periodic_report_resp_msg_v01*)msg_body_ptr)->Resp.sns_result_t);
      break;

    case SNS_SMGR_REPORT_IND_V01:
      PM_TEST_LOG_3("IND: x = %d, y = %d, z = %d",
                    ((sns_smgr_periodic_report_ind_msg_v01*)msg_body_ptr)->Item[0].ItemData[0], 
                    ((sns_smgr_periodic_report_ind_msg_v01*)msg_body_ptr)->Item[0].ItemData[1], 
                    ((sns_smgr_periodic_report_ind_msg_v01*)msg_body_ptr)->Item[0].ItemData[2]);
      ind_count++;
      if (ind_count == ind_count_limit)
      {
        /* Helps set breakpoint in function to collect ULOG */
        sns_pm_test_wait_to_collect_logs();
      }
      break;

    default:
      PM_TEST_LOG_0("Received unknown message");
      break;
  } //end of switch
} //end of sns_pm_test_process_smgr_msg


/*===========================================================================

  FUNCTION:   sns_pm_test_process_srvc_msg

===========================================================================*/
/*!
  @brief
  Process the message for the Test trigger task 
   
  @param[i] smr_hdr_ptr  : Pointer to SMR header
  @param[i] msg_body_ptr : Pointer to Message
    

  @detail
  This function does the following:
  - parses the service message
  - sends report requests accordingly to SAM or SMGR
  
  @return
  No return code
 
*/
/*=========================================================================*/
void sns_pm_test_process_srvc_msg()
{
#ifdef SNS_PCSIM
  static uint8_t                        test_enabled = 1;
  sns_smgr_periodic_report_req_msg_v01  *report_msg_ptr;
  uint8_t                               msg_body_size = 0;
  sns_smr_header_s                      msg_header;

  if (test_enabled != 0)
  {
    msg_body_size = sizeof(sns_smgr_periodic_report_req_msg_v01);
    report_msg_ptr = (sns_smgr_periodic_report_req_msg_v01*)sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_PWR,msg_body_size);
  
    report_msg_ptr->ReportId = 25;
    report_msg_ptr->Action = SNS_SMGR_REPORT_ACTION_ADD_V01;
    if (test_enabled == 1)
    {
      /* Send report request to SMGR for Accel @ 1Hz */
      report_msg_ptr->ReportRate = 1;
    }
    else
    {
      /* Send report request to SMGR for Accel @ 5Hz */
      report_msg_ptr->ReportRate = 5;
    }
    report_msg_ptr->BufferFactor = 1;
    report_msg_ptr->Item_len = 1;
  
    report_msg_ptr->Item[0].SensorId = SNS_SMGR_ID_ACCEL_V01;
    report_msg_ptr->Item[0].DataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
    report_msg_ptr->Item[0].Sensitivity = 0;
    report_msg_ptr->Item[0].Decimation = SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01;
    report_msg_ptr->Item[0].MinSampleRate = 0;
    report_msg_ptr->Item[0].StationaryOption = 0;
    report_msg_ptr->Item[0].DoThresholdTest = 0;
    report_msg_ptr->Item[0].ThresholdOutsideMinMax = 0;
    report_msg_ptr->Item[0].ThresholdDelta = 0;
    report_msg_ptr->Item[0].ThresholdAllAxes = 0;
    report_msg_ptr->Item[0].ThresholdMinMax[0] = (int32_t)(0);
    report_msg_ptr->Item[0].ThresholdMinMax[1] = (int32_t)(0);

    /* SMR Header details */
    msg_header.src_module = SNS_MODULE_DSPS_POWER_TEST;
    msg_header.dst_module = SNS_MODULE_DSPS_SMGR;
    msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  
    msg_header.txn_id = 0;
    msg_header.ext_clnt_id = 0;
    msg_header.msg_type = SNS_SMR_MSG_TYPE_REQ;
    msg_header.svc_num = SNS_SMGR_SVC_ID_V01;
    msg_header.msg_id = SNS_SMGR_REPORT_REQ_V01;
    msg_header.body_len = msg_body_size;
    sns_smr_set_hdr ( &msg_header, report_msg_ptr );
    sns_smr_send ( report_msg_ptr );
  }

  test_enabled = 0;
#endif /* SNS_PC_SIM */
} //end of function sns_pm_test_process_msg


/*===========================================================================

  FUNCTION:   sns_pm_test_trigger_task

===========================================================================*/
/*!
  @brief
  Main routine of Test Trigger task (used during power measurement). 
   
  @param[i] p_arg : Pointer to arguments

  @detail
  This function does the following:
  - Registers queue with SMR
  - Initializes ULOG 
  - while loop to process events that come to the thread

  To enable the Power Manager Test thread do the following:
  - Define feature SNS_PM_TEST in sns_common.h
  - Add the sns_pm_test_task to the init functions array
  
  @return
  No return code
 
*/
/*=========================================================================*/
void sns_pm_test_trigger_task(void *p_arg)
{
   uint8_t          err;
   void             *msgPtr;
   sns_smr_header_s msgHdr;
#ifndef SNS_PCSIM
   ULogResult       ulog_result;
#endif

   OS_FLAG_GRP *sig_grp_ptr = sns_os_sigs_create((OS_FLAGS)0x0, &err);
   SNS_ASSERT(sig_grp_ptr != NULL);

   sns_smr_register(SNS_MODULE_DSPS_POWER_TEST, sig_grp_ptr, 0x01);

#ifndef SNS_PCSIM
   /* Have ULOG for the test trigger task */
   ulog_result = ULogFront_RealTimeInit(&sns_pm_test_ulog_hdl,  // The log to create
                                        "SNS_PMGR_Test Log",    // The log name                                           
                                        SNS_PM_TEST_ULOG_SIZE,  // The amount of memory to allocate
                                        ULOG_MEMORY_LOCAL,      // Allocated from our heap
                                        ULOG_LOCK_NONE);        // SNS Power Manager is the only one going to use the log
                                                                // no need lock
   /* Ulog init sucess: This global indicates that a ULOG was indeed
    * created. If the buffer size is 0 the ULOG fn call will go successfully
    * and provide us with a handle. But we should not write to the log 
    * until we have assigned memory to it. This variable will be set if 
    * we have a valid handle AND assigned memory to the log
    */
   if ( (ulog_result == 0) && (sns_pm_test_ulog_hdl) )
   {
     ulog_test_init_success = 1;
   }
#endif

   sns_init_done();

   while (1)
   {
#ifdef SNS_PCSIM
      sns_pm_test_process_srvc_msg();
#endif

      sns_os_sigs_pend(sig_grp_ptr, 
                                  SNS_PM_TEST_SMR_MSG_SIG, 
                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 
                                  0, 
                                  &err);

      while ( (msgPtr = sns_smr_rcv(SNS_MODULE_DSPS_POWER_TEST)) != NULL )
      {         
         err = sns_smr_get_hdr(&msgHdr, msgPtr);

         if ( (err == SNS_SUCCESS) &&
              (msgHdr.svc_num == SNS_SMGR_SVC_ID_V01) )
         {
           /* Process messages from SMGR */
           sns_pm_test_process_smgr_msg(&msgHdr,msgPtr);
         }
         sns_smr_msg_free(msgPtr);
      } //end of while
   } // end of for loop
} //end of function sns_pm_test_trigger_task

#endif /* SNS_PM_TEST */
/*===========================================================================

  FUNCTION:   sns_pm_test_task_init

===========================================================================*/
/*!
  @brief
  Sensors Power Manager Test task initialization. 
   
  @param[i]
  No input parameters

  @detail
  This function initializes the sensors power manager test trigger task 
  i.e a new task for power measurement purposes is created.
  
  @return
  Returns sensors error code
  SNS_SUCCESS: If successful 
  Error code:  In case of any errors
 
*/
/*=========================================================================*/
sns_err_code_e sns_pm_test_task_init(void)
{
#ifdef SNS_PM_TEST

   //create the PM Test trigger task
   sns_os_task_create_ext(sns_pm_test_trigger_task, 
                                NULL, 
                                &sns_pm_test_stk[SNS_MODULE_STK_SIZE_DSPS_PM_TEST-1], 
                                SNS_MODULE_PRI_DSPS_POWER_TEST,
                                SNS_MODULE_PRI_DSPS_POWER_TEST,
                                &sns_pm_test_stk[0],
                                SNS_MODULE_STK_SIZE_DSPS_PM_TEST,
                                (void *)0,
                                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                                "SNS_PowerTest");


   return SNS_SUCCESS;
#else
   sns_init_done();
   return SNS_SUCCESS;
#endif /* SNS_PM_TEST */
} //end of sns_pm_test_task_init
