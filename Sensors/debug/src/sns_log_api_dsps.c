/*============================================================================

@file 
sns_log_api_dsps.c

@brief
Contains implementation of Sensors Logging API's on the DSPS processor.

Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.


============================================================================*/

/*===========================================================================

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/debug/src/sns_log_api_dsps.c#1 $

when         who  what, where, why
(mm/dd/yyyy)
--------     ---  ----------------------------------------------------------
05/07/2014   pk   Retry QMI send if QMI buffer is full
05/30/2013   sc   Added support for extended log mask (beyong 64 bits)
04/30/2013   dc   Do not send qmi indication for messages during kernel suspend.
04/02/2013   ag   Ensure sns_logpkt_commit returns SUCCESS for QMI enabled version
01/29/2013   gju  Include different file for SMR utlity functions.
10/04/2012   dc   Update priority field. Make request, responses and async indications,
                  and log and debug indications as high priority.
10/02/2012   ag   SMR cleanup
09/13/2012   vh   Eliminated compiler warnings
08/08/2012   ag   Fix size param of csi_send_ind
07/30/2012   ag   Porting from SMR to QMI
11/18/2011   sc   Fix compilation warnings
11/14/2011   jhh  Updated sns_smr_msg_alloc and sns_smr_msg_free to meet new API
05/23/2011   sj   Disable all logs by default (this is helpful during 8960 bringup)
02/28/2011   br   disable the logging feature with SNS_DSPS_PROFILE_ON definition
01/28/2011   br   changed constant names for deploying QMI tool ver#2
01/21/2011   sc   Added support for PCSIM playback
12/09/2010   sj   Fixed debug.idl file based on code review comments. Hence changes
                  to C code to match those.
11/09/2010   ad   added support for log filtering 
10/22/2010   pg   Updated to DAL_UCOS_TEST featurization to free memory.
10/22/2010   pg   Featurized out call to sns_smr_send() in DAL_UCOS_TEST
                  builds.
10/07/2010   sj   Created
===========================================================================*/

/*=====================================================================
                 INCLUDE FILES
=======================================================================*/
#include "sns_debug_str.h"
#include "sns_log_api.h"
#include "sns_smr_util.h"
#include "sns_log_types.h"
#include "sns_em.h"

#ifdef SNS_PCSIM
#include "sns_playback.h"
#endif

#ifdef SNS_QMI_ENABLE
#include "sns_memmgr.h"
#include "sns_debug_interface_v01.h"
#include "qmi_csi.h"
#include "qmi_csi_common.h"
#include "qmi_csi_target_ext.h"

extern qmi_client_handle debug_ind_client_handle;
#endif

#define NUM_MASK_BITS (sizeof(sns_log_mask_t) * 8)

/* log mask to filter log packets on DSPS */
static sns_log_mask_t sns_diag_dsps_log_mask = 0xffffffffffffffffULL; //all logs disabled
static sns_log_mask_t sns_diag_dsps_log_mask_ext = 0xffffffffffffffffULL; //all logs disabled

/*===========================================================================
                    FUNCTIONS
===========================================================================*/

/*===========================================================================

  FUNCTION:   sns_logpkt_malloc

===========================================================================*/
/*!
  @brief
  Allocates memory for the log packet. On the DSPS processor this function
  would implement a call to the local malloc function.
   
  @param[i] log_pkt_type  : Log Packet type
  @param[i] pkt_size      : Size
  @param[o] log_pkt_ptr   : Pointer of the location in which to place the 
                            allocated log packet

  @return
  sns_err_code_e: SNS_SUCCESS if the allocation had no errors.
                  All other values indicate an error has occurred.
*/
/*=========================================================================*/
sns_err_code_e sns_logpkt_malloc(log_pkt_t log_pkt_type,
                                 uint32_t pkt_size,
                                 void** log_pkt_ptr)
{
/*===================================
   IMPLEMENTATION FOR TARGET
====================================*/
  static uint32_t log_contents_max_size,debug_log_qmi_msg_size;
#ifdef SNS_DSPS_PROFILE_ON
  return SNS_ERR_BAD_PARM;
#endif
  /* Input Checks */
  if ((pkt_size == 0) || (log_pkt_ptr == NULL) || 
      (log_pkt_type >= SNS_LOG_NUM_IDS))
  {
    return SNS_ERR_BAD_PARM;
  }

  /* Filter log packets based on log mask */
  if ( (log_pkt_type < NUM_MASK_BITS) &&
       (((sns_log_mask_t)1 << log_pkt_type) & sns_diag_dsps_log_mask) )
  {
     return SNS_ERR_NOTALLOWED;
  }
  if ( (log_pkt_type >= NUM_MASK_BITS) &&
       (((sns_log_mask_t)1 << (log_pkt_type-NUM_MASK_BITS)) & sns_diag_dsps_log_mask_ext) )
  {
     return SNS_ERR_NOTALLOWED;
  }

  /* Get all the sizes one time */
  if (log_contents_max_size == 0)
  {
    log_contents_max_size  = sizeof(((sns_debug_log_ind_msg_v01*)0)->log_pkt_contents);
    debug_log_qmi_msg_size = sizeof(sns_debug_log_ind_msg_v01);
  }

  /* Allocate log packet
   * Size of log pkt qmi msg is size of sns_debug_log_ind_msg_v01 msg struct - size of log_pkt_contents array + 
   * actual size of log packet
   */
#ifdef SNS_QMI_ENABLE
  *log_pkt_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_DIAG,(pkt_size + debug_log_qmi_msg_size - log_contents_max_size));
#else
  *log_pkt_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_DIAG,(pkt_size + debug_log_qmi_msg_size - log_contents_max_size));
#endif  
  /* Check to see if log packet allocated */
  if (*log_pkt_ptr == NULL)
  {
    return SNS_ERR_NOMEM;
  }
  else
  {
    ((sns_debug_log_ind_msg_v01*)(*log_pkt_ptr))->log_pkt_type               = log_pkt_type;
    ((sns_debug_log_ind_msg_v01*)(*log_pkt_ptr))->logpkt_size                = pkt_size;
    ((sns_debug_log_ind_msg_v01*)(*log_pkt_ptr))->log_pkt_contents_len 
      = (pkt_size/sizeof(((sns_debug_log_ind_msg_v01*)0)->log_pkt_contents[0])) + 1;

    /* Pass pointer to log_pkt_contents so that the caller can fill it out */
    *log_pkt_ptr = (char*)(*log_pkt_ptr) + (debug_log_qmi_msg_size - log_contents_max_size);
    return SNS_SUCCESS;
  }
} // end of function sns_logpkt_malloc

/*===========================================================================

  FUNCTION:   add_delay

===========================================================================*/
/**
 * @brief Delays the current task without relinquishing the CPU. 
 *  
 * @note The delay functionality has some overhead associated with it and 
 *       therefore the exact amount of time delayed may include some error.
 *       The expected error tends to increase at smaller values.
 *  
 * @param[in] usec  Number of microseconds to delay the task.
 */
static void add_delay(uint32_t usec)
{
  uint64_t  usec_long;
  uint32_t  start_tick, wait_tick;

  usec_long = usec;
  start_tick = sns_em_get_timestamp();
  wait_tick = (usec_long * 32768 + (1000000-1)) / 1000000;  /* at least one tick */
  while ( ( sns_em_get_timestamp() - start_tick ) < wait_tick )
  {
    ;
  }
}

/*===========================================================================

  FUNCTION:   sns_logpkt_commit

===========================================================================*/
/*!
  @brief
  Commits the log packet to DIAG. On the DSPS processor this function
  would compose a QMI message and send it to the apps processor.
   
  @param[i] log_pkt_type: Log Packet type
  @param[i] log_pkt_ptr : Pointer to the log packet to commit 

  @return
  sns_err_code_e: SNS_SUCCESS if the commit had no errors.
                  All other values indicate an error has occurred.
*/
/*=========================================================================*/
sns_err_code_e sns_logpkt_commit(log_pkt_t log_pkt_type,
                                 void* log_pkt_ptr)
{
  sns_debug_log_ind_msg_v01 *qmi_log_pkt_msg;
  uint16_t log_qmi_msg_hdrs_size;

  /* Input Checks */
  if ( (log_pkt_ptr == NULL) )
  {
    return SNS_ERR_BAD_PARM;
  }

  /* Set the pointer to the beginning of the qmi message
   */
  log_qmi_msg_hdrs_size = (sizeof(sns_debug_log_ind_msg_v01) -  sizeof(((sns_debug_log_ind_msg_v01*)0)->log_pkt_contents));
  qmi_log_pkt_msg = (sns_debug_log_ind_msg_v01*)((char*)(log_pkt_ptr) - log_qmi_msg_hdrs_size);

#ifdef SNS_QMI_ENABLE
  if (debug_ind_client_handle != 0)
  {
    qmi_csi_error err;
    uint8_t attempts = 0;
    do
    {
      if( attempts != 0 )
      {
        add_delay(1000);
      }
      err = qmi_csi_send_ind(debug_ind_client_handle,
                             SNS_DEBUG_LOG_IND_V01,
                             qmi_log_pkt_msg,
                             sizeof(sns_debug_log_ind_msg_v01));
    } while ( (++attempts < 5) && (QMI_CSI_CONN_BUSY == err) );
  }

  SNS_OS_FREE( qmi_log_pkt_msg );
  return SNS_SUCCESS;
#else
 {
   sns_smr_header_s smr_hdr;

  /*  Fill in SMR header details from diag command packet */
  smr_hdr.dst_module = SNS_MODULE_APPS_DIAG;  
  smr_hdr.src_module = SNS_MODULE_DSPS;
  smr_hdr.priority = SNS_SMR_MSG_PRI_HIGH;
  smr_hdr.txn_id = 0;
  smr_hdr.ext_clnt_id = 0;
  smr_hdr.msg_type = SNS_SMR_MSG_TYPE_IND;
  smr_hdr.svc_num = SNS_DEBUG_SVC_ID_V01;
  smr_hdr.msg_id = SNS_DEBUG_LOG_IND_V01;
  smr_hdr.body_len = log_qmi_msg_hdrs_size + qmi_log_pkt_msg->log_pkt_contents_len*sizeof(((sns_debug_log_ind_msg_v01*)0)->log_pkt_contents[0]);

  if (sns_smr_set_hdr(&smr_hdr,qmi_log_pkt_msg) == SNS_SUCCESS)
  {
#ifdef SNS_PCSIM
    sns_playback_log_pkt(qmi_log_pkt_msg);
    sns_smr_msg_free(qmi_log_pkt_msg);
    return SNS_SUCCESS;
#else
#ifndef DAL_UCOS_TEST
    if (sns_smr_send(qmi_log_pkt_msg) == SNS_SUCCESS)
    {
      return SNS_SUCCESS;
    }
#endif
#endif
  }
  sns_smr_msg_free(qmi_log_pkt_msg);
 }
#endif //SNS_QMI_ENABLE

  return SNS_ERR_FAILED;
} // end of function sns_logpkt_commit

/*===========================================================================

  FUNCTION:   sns_diag_dsps_set_log_mask

===========================================================================*/
/*!
  @brief
  Sets the log mask on the DSPS processor
  
   
  @param[i] 
  msg_ptr: pointer to message containing log mask that indicates which
           log packets are enabled/disabled.

  @return
  none
*/
/*=========================================================================*/
void sns_diag_dsps_set_log_mask(sns_diag_set_log_mask_req_msg_v01* msg_ptr)
{
   if (msg_ptr != NULL)
   {
      sns_diag_dsps_log_mask = msg_ptr->log_mask.mask;
      if (msg_ptr->log_mask_ext_valid)
      {
         sns_diag_dsps_log_mask_ext = msg_ptr->log_mask_ext.mask;
      }
   }
}


