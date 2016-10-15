#define __SNS_MODULE__ SNS_SMR

#ifndef SNS_SMR_C
#define SNS_SMR_C

/*============================================================================

  @file sns_smr.c

  @brief
  This file contains the implementation of SMR APIs and SMR internal functions

  Copyright (c) 2010-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/common/smr/src/sns_smr.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
2012-12-07   gju     Provide get_hdr and set_hdr for all platforms
2012-10-04   dc      Use the priority infomation to determine the smr queue to be used
                     for indications. Call smr_dsps_smsm_init() to handle SMSM notifications.
2012-10-02   ag      Allow compatibility with SMR and QMI targets
2012-10-02   sc      Disabled sns_dsps_smdl_init() if using QMI
2012-08-09   ag      Updated to use official QMI service IDs
2012-05-01   gvg     Added additional signal support to smr
2011-11-14   jhh     Updated alloc and free functions to meet new API
2011-08-19   gju     Change ASSERT(var-var) to ASSERT(false); add NULL check
2011-07_19   br      returned NULL if svc_num is not defined in sns_smr_get_svc_obj
2011-05-19   jtl     Removing references to LAST_SVC_ID_V01
2011-05-17   jtl     Handle errors in LA init.
2011-04-22   agk     Added changes for modem SMR.
2011-04-20   br      inserted like for q link init because FEATURE_Q_NO_SELF_QPTR was undefined in sns_queue.h
2010-04-18   br      Deleted obsolete API: sns_smr_q_register()
2011-04-18   ry      Changed #elseif to #elif
2011-04-15   agk     Added changes required for running SMR on modem.
2011-04-08   jtl     ifdef-ing out printfs to reduce log size
2011-04-11   jh      Used proper loop bounds in smr_set_qmi_max_encode_msg_len()
2011-02-24   br      Used sns_os_task_create_ext() for DSPS instead of sns_os_task_create()
2011-02-16   br      changed to get qmi svc object through sns_qmi_svc_accessor_s
2011-01-28   br      changed constant names and get_qmi_svc_obj() for deploying QMI tool ver#2,
2010-11-04   jtl     Removing GCC compiler warnings.
2010-10-29   jtl     Avoiding threadding issues in debug printf by using the stack.
2010-10-28   sj      Added debug strings for SMR common files
2010-10-27   br      Added PRINT at msg_alloc(), and fixed size para for qmi_idl_message_decode()
2010-10-20   br      Inserted new functions: smr_set_qmi_max_encode_msg_len, smr_set_qmi_service_obj.
                     Optimized smr_which_dst_module(), sns_smr_get_svc_obj(), and smr_process_rcvd_msg
2010-10-07   jtl     Removing unnecessary header.
2010-09-30   br      Changed sns_smr_q_register() to sns_smr_register()
2010-09-10   br      Updated QMI encoding feature
2010-08-25   br      Applied integration test result especially with SNS_LA_SIM and SMR_ENCODE_ON
2010-06-20   br      Inserted features for SDM communication and QMI encoding
2010-06-28   br      Check message type before deriving the destination module
2010-06-20   br      Initial Release

============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include <stddef.h>
#include <string.h>
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_smr.h"
#include "sns_common.h"
#include "sns_debug_api.h"
#if defined(SNS_DSPS_BUILD) || defined(SNS_PCSIM) || defined(SNS_BLAST)
#include "smd_lite.h"
#endif
#include "sns_queue.h"
#include "sns_em.h"
#include "sns_smr_priv.h"
#if defined(SNS_BLAST)
#include <stringl/stringl.h>
#include "sns_debug_str_mdm.h"
#else
#include "sns_debug_str.h"
#endif

/*===========================================================================

                            INTERNAL DEFINITION AND TYPES

===========================================================================*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void smr_set_qmi_service_obj (void);

#ifdef SNS_QMI_ENABLE
# define SMR_DBG_MODULE_ID SNS_DBG_MOD_DSPS_SMR
#else
static void smr_set_qmi_max_encode_msg_len (void);

/*===========================================================================

                            STATIC VARIABLES

===========================================================================*/
sns_smr_s      sns_smr;
static OS_STK  smr_stack[SNS_MODULE_STK_SIZE_APPS_SMR];


/*===========================================================================

                            MACROS

===========================================================================*/
#if defined(SNS_DSPS_BUILD) || defined(SNS_PCSIM)
#  define smr_msg_print(a,b)
/* This is a common file shared between apps and DSPS.
 * Use DSPS Module ID if its a DSPS build
 */
#  define SMR_DBG_MODULE_ID SNS_DBG_MOD_DSPS_SMR

#elif defined(SNS_SMR_EXTRA_DEBUG)

#  define SMR_DBG_MODULE_ID SNS_DBG_MOD_APPS_SMR
/*===========================================================================

  FUNCTION:   smr_bin2hex

===========================================================================*/
/*!
  @brief This function convert a input char to hex format

  @param[i] input_char: an input data
            out_ptr: a pointer in which the hex values are saved.

  @return
   None
*/
/*=========================================================================*/
static void smr_bin2hex (uint8_t input_char, int8_t * out_ptr)
{
  int8_t nibble;
  nibble = (input_char & 0xf0) >> 4;
  out_ptr[0] = (nibble < 0x0a) ? nibble+'0' : nibble - 0xa +'A';

  nibble = (input_char & 0x0f) >> 0;
  out_ptr[1] = (nibble < 0x0a) ? nibble+'0' : nibble - 0xa +'A';
  out_ptr[2] = ' ';
}

/*===========================================================================

  FUNCTION:   smr_msg_print

===========================================================================*/
/*!
  @brief This function prints out a SMR message

  @param[i] msg_ptr: A pointer to a SMR msg block
            encoded: identifire if the message is encoded or not

  @return
   None

  $TODO: Search algorithm enhancement. ex) If service numbers are sequentially ordered,
         direct mapping can be possible
*/
/*=========================================================================*/
void smr_msg_print (smr_msg_s *msg_ptr, boolean encoded)
{
  /* Note: "body_str" will grow in size with SMR_ONE_LINE*3.
   * Take care about increasing this define, as large values could overflow
   * the stack. */
#  define SMR_ONE_LINE 16
#  define SMR_LOG_LINE 1
#  define SMR_BODY_BANNER "xxBody:"
#  define SMR_BODY_BANNER_LEN (strlen(SMR_BODY_BANNER))

  int32_t i, line_no;
  int8_t  body_str[SMR_BODY_BANNER_LEN + SMR_ONE_LINE*3 + 1];          /* including NULL string */

  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_APPS_SMR, DBG_SMR_MSG_HDR_DETAILS1,
                             msg_ptr->header.dst_module,
                             msg_ptr->header.src_module,
                             msg_ptr->header.msg_type);
#  if 0
  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_APPS_SMR, DBG_SMR_MSG_HDR_DETAILS2,
                      msg_ptr->header.txn_id, msg_ptr->header.ext_clnt_id,
                             msg_ptr->header.priority);
#  endif
  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_APPS_SMR, DBG_SMR_MSG_HDR_DETAILS3,
                             msg_ptr->header.svc_num, msg_ptr->header.msg_id,
                             msg_ptr->header.body_len);

  SNS_OS_MEMCOPY ( body_str,  SMR_BODY_BANNER, SMR_BODY_BANNER_LEN);
  if (encoded)
  {
    SNS_OS_MEMCOPY ( body_str,  "En", strlen("En"));
  }
  else
  {
    SNS_OS_MEMCOPY ( body_str,  "De", strlen("De"));
  }

  for ( line_no = 0; line_no < SMR_LOG_LINE; line_no++)
  {
    if ( msg_ptr->header.body_len <= ( line_no * SMR_ONE_LINE ) )
    {
      break;
    }
    for ( i = 0; i < (MIN(SMR_ONE_LINE, (int32_t)msg_ptr->header.body_len - line_no*SMR_ONE_LINE)); i++)
    {
      smr_bin2hex(msg_ptr->body[line_no * SMR_ONE_LINE + i ],  &body_str[SMR_BODY_BANNER_LEN + i * 3]);
    }
    body_str[SMR_BODY_BANNER_LEN + i*3] = '\0';
    // TODO: This macro isn't designed to print strings.
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_APPS_SMR,
                               DBG_SMR_BODY_STR,
                               (int32_t)(uintptr_t)body_str);

  }
}
#else
/* Not DSPS or PCSIM build, and extra debugging is disabled */
#  define smr_msg_print(a,b)
#  define SMR_DBG_MODULE_ID SNS_DBG_MOD_DSPS_SMR
#endif /* if defined(SNS_DSPS_BUILD) || defined(SNS_PCSIM) */

/*===========================================================================

  FUNCTION:   smr_which_dst_module

===========================================================================*/
/*!
  @brief This function identifies the destination module by using the routing table.

  @param[i] svc_num: The service number which determines the destination module

  @return
   The destination mudule id

*/
/*=========================================================================*/
uint8_t smr_which_dst_module (uint32_t svc_num)
{
  SNS_ASSERT ( svc_num < SNS_SMR_RTB_SIZE );
  return sns_rtb[svc_num].module_id;
}

/*===========================================================================

  FUNCTION:   smr_send_domestic

===========================================================================*/
/*!
  @brief This function transfers a message to an appropriate module within a same processor

  @detail This function signals to the module after it puts the message into the destination module's queue

  @param[i] msg_ptr: A pointer to the message to be sent

  @return
   - SNS_SUCCESS if the message was sent to the destination module successfully.
   - All other values indicate an error has occurred.

*/
/*=========================================================================*/
sns_err_code_e smr_send_domestic (smr_msg_s *msg_ptr)
{
  uint8_t             os_err;
  sns_smr_header_s *header_ptr = &msg_ptr->header;

  smr_que_entry_s *que_entry_ptr;
  /* put the message into the queues and send signal */
  que_entry_ptr = &sns_smr.smr_que_tb[header_ptr->dst_module & ~SNS_MODULE_GRP_MASK];

  sns_os_mutex_pend(sns_smr.que_mutex_ptr, 0, &os_err);
  SNS_ASSERT(os_err == OS_ERR_NONE);
#ifdef SMR_PRIORITY_QUE_ON
  SNS_ASSERT ((header_ptr->priority <= SNS_SMR_MSG_PRI_HIGH));
  sns_q_put (que_entry_ptr->q_ptr[header_ptr->priority], &msg_ptr->q_link);
#else
  /* even though messages can be SNS_SMR_MSG_PRI_LOW or SNS_SMR_MSG_PRI_HIGH,
     they are put in the same queue with LOW priority */
  SNS_ASSERT ((header_ptr->priority <= SNS_SMR_MSG_PRI_HIGH));
  sns_q_put (que_entry_ptr->q_ptr[SNS_SMR_MSG_PRI_LOW], &msg_ptr->q_link);
#endif
  os_err = sns_os_mutex_post(sns_smr.que_mutex_ptr );
  SNS_ASSERT(os_err == OS_ERR_NONE);

  sns_os_sigs_post ( que_entry_ptr->sig_grp_ptr, que_entry_ptr->sig_flag, OS_FLAG_SET, &os_err );
  SNS_ASSERT (os_err == OS_ERR_NONE);
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smr_send

===========================================================================*/
/*!
  @brief This function transfers the message to an appropriate module within the Sensors framework

  @param[i] body_ptr: A pointer to the message body allocated by sns_smr_msg_alloc()

  @return
   - SNS_SUCCESS if the message header was gotten successfully.
   - SNS_ERR_WOULDBLOCK if some resource is tentatively unavailable.
   - All other values indicate an error has occurred.

*/
/*=========================================================================*/
sns_err_code_e sns_smr_send (void* body_ptr)
{
  smr_msg_s *msg_ptr;
  sns_smr_header_s            *header_ptr;
#if defined(SNS_DSPS_BUILD) || defined(SNS_LA) || defined(SNS_BLAST)
  qmi_idl_service_object_type  svc_obj;
#endif
  msg_ptr = GET_SMR_MSG_PTR(body_ptr);
  header_ptr = &msg_ptr->header;

#if defined( SNS_LA ) && defined( SNS_SMR_EXTRA_DEBUG )
  SNS_PRINTF_STRING_ID_LOW_0(SMR_DBG_MODULE_ID,DBG_SMR_SEND_STATUS);
#endif

  if(SNS_SMR_MSG_TYPE_REQ == header_ptr->msg_type)
  {
    header_ptr->dst_module = smr_which_dst_module( header_ptr->svc_num );
                /* identify the dest module by using the routing table */
  }

  if ((header_ptr->dst_module & SNS_MODULE_GRP_MASK) == SNS_THIS_MODULE_GRP)
                /* check if the dest mudule is in the local processor */
  {
    if ( 1 /* log filtered */ )
    {
#if defined(SNS_DSPS_BUILD) || defined(SNS_PCSIM)
      smr_dsps_log_the_packet(header_ptr);
#elif defined(SNS_LA) || defined(SNS_LA_SIM)
      smr_apps_log_the_packet(header_ptr, SMR_MSG_DECODED);
#else
	  smr_mdm_log_the_packet(header_ptr);
#endif
    }
#if defined( SNS_LA ) && defined( SNS_SMR_EXTRA_DEBUG )
    SNS_PRINTF_STRING_ID_LOW_1(SMR_DBG_MODULE_ID, DBG_SMR_SEND_DEST_MODULE, header_ptr->dst_module);
    SMR_MSG_PRINT(msg_ptr, SMR_MSG_DECODED);
#endif
    return smr_send_domestic(msg_ptr);
  }
  else
  {
#if defined(SNS_DSPS_BUILD) || defined(SNS_LA) || defined(SNS_BLAST)
    svc_obj = sns_smr_get_svc_obj(header_ptr->svc_num);
#endif /* defined(SNS_DSPS_BUILD) || defined(SNS_LA) */

#if defined( SNS_LA ) && defined( SNS_SMR_EXTRA_DEBUG )
    SNS_PRINTF_STRING_ID_LOW_1(SMR_DBG_MODULE_ID, DBG_SMR_DST_MODULE, header_ptr->dst_module);
    SMR_MSG_PRINT(msg_ptr, SMR_MSG_DECODED);
#endif

#if defined( SNS_DSPS_BUILD )
    return smr_dsps_send(body_ptr, svc_obj);
#elif defined( SNS_LA )
    return smr_apps_la_send(body_ptr, svc_obj);
#elif defined(SNS_BLAST)
	return smr_mdm_blast_send(body_ptr, svc_obj);
#else
    /* shouldn't be here... */
    return SNS_ERR_FAILED;
#endif
  }
}

/*===========================================================================

  FUNCTION:   sns_smr_register

===========================================================================*/
/*!
  @brief This function registers queues and the event handle bound to the queues

  @param[i] module_id: The sensor module id which is unique within the sensor framework.
  @param[i] sig_grp_ptr: A pointer to the signal group to which sig_flag is associated
  @param[i] sig_flag: The signal flag that triggers the event
  @param[i] low_q_ptr: A pointer to the low priority queue within the module
  @param[i] high_q_ptr: A pointer to the high priority queue within the module

  @return
   - SNS_SUCCESS if the message queues was registered successfully.
   - All other values indicate an error has occurred.

*/
/*=========================================================================*/
sns_err_code_e sns_smr_register (
  uint8_t     module_id,
  OS_FLAG_GRP *sig_grp_ptr,
  OS_FLAGS    sig_flag )
{
  uint8_t           os_err;
  smr_que_entry_s   *que_entry_ptr;
#if defined(SNS_DSPS_BUILD)
  SNS_ASSERT((SNS_DSPS_MODULE_CNT|SNS_MODULE_DSPS) >  module_id);
#elif defined(SNS_LA)
  SNS_ASSERT((SNS_APPS_MODULE_CNT|SNS_MODULE_APPS) >  module_id);
#elif defined(SNS_BLAST)
  SNS_ASSERT((SNS_MODEM_MODULE_CNT|SNS_MODULE_MDM) >  module_id);
#endif
  que_entry_ptr = &sns_smr.smr_que_tb[module_id & ~SNS_MODULE_GRP_MASK];
  /* save information which will be used for signaling */
  que_entry_ptr->sig_grp_ptr = sig_grp_ptr;
  que_entry_ptr->sig_flag = sig_flag;

  /* If there is a message already in the queues, signal to the module */
  if ( sns_q_check(que_entry_ptr->q_ptr[SNS_SMR_MSG_PRI_LOW])
#ifdef SMR_PRIORITY_QUE_ON
      ||
      sns_q_check(que_entry_ptr->q_ptr[SNS_SMR_MSG_PRI_HIGH])
#endif
      )
  {
    sns_os_sigs_post ( que_entry_ptr->sig_grp_ptr, que_entry_ptr->sig_flag, OS_FLAG_SET, &os_err );
    SNS_ASSERT (os_err == OS_ERR_NONE);
  }
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smr_rcv

===========================================================================*/
/*!
  @brief This function returns a pointer to a message body from the module�s queues

  @param[i] module_id: The module id of the caller.

  @return
  NULL if failed or no more message in the queues, or a pointer to the message body

*/
/*=========================================================================*/
void* sns_smr_rcv (uint8_t module_id)
{
  uint8_t                os_err;
  smr_que_entry_s       *que_entry_ptr;
  smr_msg_s             *msg_ptr;
  sns_smr_msg_pri_e      pri_idx;
  void                  *body_ptr = NULL;
  que_entry_ptr = &sns_smr.smr_que_tb[module_id & ~SNS_MODULE_GRP_MASK]; /* Exclude Processor field */

#ifdef SMR_PRIORITY_QUE_ON
  for ( pri_idx = SNS_SMR_MSG_PRI_LOW; pri_idx <= SNS_SMR_MSG_PRI_HIGH; pri_idx++)
#else
  for ( pri_idx = SNS_SMR_MSG_PRI_LOW; pri_idx <= SNS_SMR_MSG_PRI_LOW; pri_idx++)
#endif
  {
    sns_os_mutex_pend(sns_smr.que_mutex_ptr, 0, &os_err );
    SNS_ASSERT ( os_err == OS_ERR_NONE );
    msg_ptr = sns_q_get (que_entry_ptr->q_ptr[pri_idx]);
    os_err = sns_os_mutex_post(sns_smr.que_mutex_ptr );
    SNS_ASSERT ( os_err == OS_ERR_NONE );
    if ( NULL != msg_ptr )
    {
      body_ptr = &msg_ptr->body;
      break;
    }
  }

  return body_ptr;
}

/*===========================================================================

  FUNCTION:   smr_process_rcvd_msg

===========================================================================*/
/*!
  @brief This function process a message which was received from SMD

  @detail
      - Identify destination
      - Decode the message
      - Send the message to the destination module's queue by calling smr_send_domestic()
  @param[i] en_msg_ptr : A pointer to a msg block which has a encoded msg body
  @param[o] de_msg_ptr : A pointer to a msg block which will have a decoded msg body
  @param[i] svc_obj    : The QMI service object associated with this message
  @param[i] de_body_len: The length of the decoded body

  @return
   SNS_SUCCESS if the message header was gotten successfully.
   Otherwise, it returns a SNS_EXX style error code.
*/
/*=========================================================================*/
sns_err_code_e smr_process_rcvd_msg( const smr_msg_s *en_msg_ptr,
                                     smr_msg_s *de_msg_ptr,
                                     qmi_idl_service_object_type svc_obj,
                                     uint16_t  de_body_len )
{
#ifdef SMR_ENCODE_ON
  int32_t                     qmi_result;
#endif

#ifdef SMR_ENCODE_ON
  SNS_ASSERT ( NULL != svc_obj );

  /* Copy the header */
  SNS_OS_MEMCOPY (&de_msg_ptr->header, &en_msg_ptr->header, sizeof(sns_smr_header_s));
  de_msg_ptr->header.body_len = de_body_len;

  /* decode the message */
  if ( 0 != de_body_len )
  {
    qmi_result = qmi_idl_message_decode ( svc_obj,
                                           (qmi_idl_type_of_message_type)en_msg_ptr->header.msg_type,
                                           en_msg_ptr->header.msg_id,
                                           en_msg_ptr->body,
                                           en_msg_ptr->header.body_len,
                                           de_msg_ptr->body,
                                           de_body_len);
  }
  else
  {
    qmi_result = 0;
  }

  if ( 0 != qmi_result )
  {
    return SNS_ERR_FAILED;
  }
#else
  /* Copy the header & body all together */
  SNS_OS_MEMCOPY (&de_msg_ptr->header, &en_msg_ptr->header, sizeof(sns_smr_header_s) + en_msg_ptr->header.body_len);
#endif

#if SNS_LA
  smr_apps_log_the_packet(&en_msg_ptr->header, SMR_MSG_ENCODED);
#endif

  return smr_send_domestic (de_msg_ptr);
}

/*===========================================================================

  FUNCTION:  smr_change_state
===========================================================================*/
/**
  @brief change SMR state

  @param[i] next_state : The next state of SMR

  @detail

  @return None

*/
/*=========================================================================*/
void smr_change_state ( smr_state_e next_state )
{
  sns_smr.state =  next_state;
}

/*===========================================================================

  FUNCTION:  sns_smr_init

===========================================================================*/
/**
  @brief
   Initialize all necessary data structures of SMR and
   create SMR thread

  @detail
    - Initialize all message que entries with the reset values
    - Create mutexes
    - Register queues by calling sns_smr_register
    - Initialize SMR state

  @return None

*/
/*=========================================================================*/
sns_err_code_e sns_smr_init (void)
{
  int32_t i;
  uint8_t os_err;

  /* create the signal paramter */
  sns_smr.sig_grp_ptr = sns_os_sigs_create ( (OS_FLAGS)0x0, &os_err);

  if( NULL == sns_smr.sig_grp_ptr)
  {
    return SNS_ERR_FAILED;
  }

  /* crete mutexes used by SMR */
#if defined(SNS_LA) || defined(SNS_LA_SIM)
  sns_smr.que_mutex_ptr = sns_os_mutex_create(SNS_SMR_APPS_QUE_MUTEX, &os_err);
  SNS_ASSERT (os_err == OS_ERR_NONE );
#elif defined(SNS_DSPS_BUILD) || defined(SNS_PCSIM)
  sns_smr.que_mutex_ptr = sns_os_mutex_create(SNS_SMR_QUE_MUTEX, &os_err);
  SNS_ASSERT (os_err == OS_ERR_NONE );
#elif defined(SNS_BLAST)
  sns_smr.que_mutex_ptr = sns_os_mutex_create(SNS_SMR_MDM_QUE_MUTEX, &os_err);
  SNS_ASSERT (os_err == OS_ERR_NONE );
#endif

  /* initialize all queue entiries with the reset values */
  for ( i = 0; i < SNS_MODULE_CNT; i++ )
  {
    smr_que_entry_s *que_entry_ptr;
    que_entry_ptr = &sns_smr.smr_que_tb[i];
    /* reset the signaling parameters with the SMR signalling parameter as a default */
    /* i.e. if a que_entry_ptr is not initialized by a module, signal will be issued to SMR */
    que_entry_ptr->sig_grp_ptr = sns_smr.sig_grp_ptr;
#if defined(SNS_LA) || defined(SNS_LA_SIM)
    que_entry_ptr->sig_flag = SMR_APPS_SIG_SMR_MSGQ;
#elif defined(SNS_DSPS_BUILD) || defined(SNS_PCSIM)
    que_entry_ptr->sig_flag = SMR_SIG_SMR_MSGQ;
#elif defined(SNS_BLAST)
    que_entry_ptr->sig_flag = SMR_MDM_SIG_SMR_MSGQ;
#endif
    /* initialize the queue pointers */
    que_entry_ptr->q_ptr[SNS_SMR_MSG_PRI_LOW] = sns_q_init(&que_entry_ptr->queue[SNS_SMR_MSG_PRI_LOW]);
#ifdef SMR_PRIORITY_QUE_ON
    que_entry_ptr->q_ptr[SNS_SMR_MSG_PRI_HIGH] = sns_q_init(&que_entry_ptr->queue[SNS_SMR_MSG_PRI_HIGH]);
#endif
  }

  /* reset the retry count for memory */
  sns_smr.nomem_retry_cnt = 0;

  /* Set qmi object into the RTB */
  smr_set_qmi_service_obj ();

  /* set the qmi max encode msg for the services that Sensors framework is using */
  smr_set_qmi_max_encode_msg_len ();

  /* allocates tx/rx buffer */
#ifdef SMR_ENCODE_ON
  sns_smr.en_rx_msg_ptr = GET_SMR_MSG_PTR(sns_smr_msg_alloc (SNS_SMR_DBG_MOD, sns_smr.qmi_max_encode_len));
  SNS_ASSERT(NULL!=sns_smr.en_rx_msg_ptr->body);
  sns_smr.en_tx_msg_ptr = GET_SMR_MSG_PTR(sns_smr_msg_alloc (SNS_SMR_DBG_MOD, sns_smr.qmi_max_encode_len));
  SNS_ASSERT(NULL!=sns_smr.en_tx_msg_ptr->body);
#else
  sns_smr.en_rx_msg_ptr = GET_SMR_MSG_PTR(sns_smr_msg_alloc (SNS_SMR_DBG_MOD, SMR_MAX_BODY_LEN));
  SNS_ASSERT(NULL!=sns_smr.en_rx_msg_ptr->body);
  sns_smr.en_tx_msg_ptr = GET_SMR_MSG_PTR(sns_smr_msg_alloc (SNS_SMR_DBG_MOD, SMR_MAX_BODY_LEN));
  SNS_ASSERT(NULL!=sns_smr.en_tx_msg_ptr->body);
#endif

#if defined(SNS_LA) || defined(SNS_LA_SIM)
  /* register messages queues for SMR itself */
  sns_smr_register ( SNS_MODULE_APPS_SMR, sns_smr.sig_grp_ptr, SMR_APPS_SIG_SMR_MSGQ );

  /* mutexes exclusivly used by APPS SMR */
  sns_smr.smd_tx_mutex_ptr = sns_os_mutex_create(SNS_SMR_APPS_SMDL_MUTEX, &os_err);
  SNS_ASSERT (os_err == OS_ERR_NONE );

  if( smr_apps_la_smd_init() == false )
  {
    /* an error occured opening the SMD pipe */
    /* This will cause the LA process to die, so there is no need to
     * clean up os_mutex or os_sig resources */
    return SNS_ERR_FAILED;
  }
  /* create SMR task */
  sns_os_task_create (smr_apps_la_thread_main, NULL, &smr_stack[SNS_MODULE_STK_SIZE_APPS_SMR-1],
                      SNS_MODULE_PRI_APPS_SMR);
#elif defined(SNS_BLAST)
  /* register messages queues for SMR itself */
  sns_smr_register ( SNS_MODULE_MDM_SMR, sns_smr.sig_grp_ptr, SMR_MDM_SIG_SMR_MSGQ );

  /* mutexes exclusivly used by modem SMR */
  sns_smr.smd_tx_mutex_ptr = sns_os_mutex_create(SNS_SMR_MDM_SMDL_MUTEX, &os_err);
  SNS_ASSERT (os_err == OS_ERR_NONE );

  smr_mdm_blast_smdl_init();
  /* create SMR task */
  sns_os_task_create (smr_mdm_blast_thread_main, NULL, &smr_stack[SNS_MODULE_STK_SIZE_APPS_SMR-1],
                      SNS_MODULE_PRI_MDM_SMR);
#elif defined(SNS_DSPS_BUILD) || defined(SNS_PCSIM)
  /* register messages queues for SMR itself */
  sns_smr_register ( SNS_MODULE_DSPS_SMR, sns_smr.sig_grp_ptr, SMR_SIG_SMR_MSGQ );
  /* mutexes exclusivly used by DSPS SMR */
  sns_smr.smd_tx_mutex_ptr = sns_os_mutex_create(SNS_SMR_SMDL_MUTEX, &os_err);
  SNS_ASSERT (os_err == OS_ERR_NONE );

  smr_dsps_smdl_init();
  smr_dsps_smsm_init();

  /* create SMR task */
  sns_os_task_create_ext (smr_dsps_thread_main, NULL, &smr_stack[SNS_MODULE_STK_SIZE_DSPS_SMR-1],
                      SNS_MODULE_PRI_DSPS_SMR, SNS_MODULE_PRI_DSPS_SMR, &smr_stack[0],
                      SNS_MODULE_STK_SIZE_DSPS_SMR, (void *)0, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, (uint8_t *)"SNS_SMR");
#endif

  /* initialize SMR state to NORMAL */
  smr_change_state(SMR_STATE_NORMAL);

  return  SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:  smr_set_qmi_max_encode_msg_len

===========================================================================*/
/**
  @brief
    Set the maximum encoded message length for all services

  @detail
    A QMI API, qmi_idl_get_max_service_len(), is provided to get the maximum encoded message length
    for each service.

  @return None


*/
/*=========================================================================*/
static void smr_set_qmi_max_encode_msg_len (void)
{
  uint8_t  i;
  uint32_t svc_len, max_svc_len = 0;
  for (i = 0; i < SNS_SMR_RTB_SIZE; i++)
  {
    /* if the entry has a qmi_svc_obj, the entry is for a valied service id */
    if (NULL != sns_rtb[i].qmi_svc_obj)
    {
      qmi_idl_get_max_service_len(sns_rtb[i].qmi_svc_obj, &svc_len);
      max_svc_len = MAX(svc_len, max_svc_len);
    }
  }
  sns_smr.qmi_max_encode_len = (uint16_t)max_svc_len;
}

/*===========================================================================

  FUNCTION:  sns_smr_get_qmi_max_encode_msg_len

===========================================================================*/
/**
  @brief
    This function returns the maximum encoded message length for all services
    which are used in this framework.

  @detail

  @return the maximum encoded message length

*/
/*=========================================================================*/
uint16_t sns_smr_get_qmi_max_encode_msg_len (void)
{
  return sns_smr.qmi_max_encode_len;
}

/*===========================================================================

  FUNCTION:   sns_smr_close

===========================================================================*/
/*!
  @brief  Close the SMD port

  @param[i] None

  @detail

  @return
   - SNS_SUCCESS if close system call was success.
   - SNS_ERR_FAILED if an error occurred.

*/
/*=========================================================================*/
sns_err_code_e sns_smr_close (void)
{
#if defined(SNS_LA)
  return smr_la_close();
#else
  return SNS_SUCCESS;
#endif
}
#endif /* SNS_QMI_ENABLE */

/*===========================================================================

  FUNCTION:   sns_smr_msg_alloc

===========================================================================*/
/*!
  @brief this function allocates message body and header, and returns the body pointer.

  @param[i] body_size is the message body size to be allocated

  @return
  NULL if failed, or a pointer to the newly allocated message body

*/
/*=========================================================================*/
void * sns_smr_msg_alloc (sns_debug_module_id_e src_module, uint16_t body_size)
{
#ifdef SNS_QMI_ENABLE
  return SNS_OS_MALLOC(src_module, body_size);
#else
  smr_msg_s * msg_ptr;

  if ( SMR_MAX_BODY_LEN < body_size )
  {
    SNS_PRINTF_STRING_ID_HIGH_2(SMR_DBG_MODULE_ID,DBG_SMR_ALLOC_ERR,
                                body_size, SMR_MAX_BODY_LEN);
    SNS_ASSERT ( false );
  }

  msg_ptr = (smr_msg_s*)
  SNS_OS_MALLOC(src_module, (uint16_t)(SMR_MSG_HEADER_BLK_SIZE + body_size));
  if (msg_ptr != NULL)
  {
    sns_q_link(msg_ptr, &msg_ptr->q_link);
#ifdef SNS_DEBUG
    msg_ptr->msg_marker = SMR_MSG_MARKER;
#endif
    return ((void *)msg_ptr->body);
  }
  else
  {
    return NULL;
  }
#endif
}

/*===========================================================================

  FUNCTION:   sns_smr_set_hdr

===========================================================================*/
/*!
  @brief This function sets message header information with the parameters delivered using
         sns_smr_header_s structure type.
         The address of the message header is calculated from body_ptr.

  @param[i] header_type_ptr: A pointer to the header structure type variable which includes all parameters.
            The message header is identified by body_ptr.
  @param[i] body_ptr: A pointer to the message body allocated by sns_smr_msg_alloc

  @return
   - SNS_SUCCESS if the message header was set successfully.
   - All other values indicate an error has occurred.

*/
/*=========================================================================*/
sns_err_code_e sns_smr_set_hdr(const sns_smr_header_s * header_type_ptr, void * body_ptr)
{
  SNS_OS_MEMCOPY((void*)GET_SMR_MSG_HEADER_PTR(body_ptr), (const void *)header_type_ptr,
         sizeof(sns_smr_header_s));
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smr_get_hdr

===========================================================================*/
/*!
  @brief This function gets message header information into sns_smr_header_s structure type.

  @param[o] header_type_ptr: A pointer to the header structure type in which
            the header informaiton will be retrieved
  @param[i] body_ptr: A pointer to the message body allocated by sns_smr_msg_alloc

  @return
   - SNS_SUCCESS if the message header was gotten successfully.
   - All other values indicate an error has occurred.

*/
/*=========================================================================*/
sns_err_code_e sns_smr_get_hdr(sns_smr_header_s * header_type_ptr, const void * body_ptr)
{
  SNS_OS_MEMCOPY((void *)header_type_ptr, (const void*)GET_SMR_MSG_HEADER_PTR(body_ptr),
         sizeof(sns_smr_header_s));
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smr_msg_free

===========================================================================*/
/*!
  @brief This function frees the message header and body allocated by sns_smr_msg_alloc().

  @param[i] body_ptr: A pointer variable to the message body to be freed

  @return
   None

*/
/*=========================================================================*/
void sns_smr_msg_free (void * body_ptr)
{
#ifdef SNS_QMI_ENABLE
  SNS_OS_FREE( body_ptr );
#else
#ifdef SNS_DEBUG
  SNS_ASSERT(IS_SMR_MSG_MARKER(((smr_msg_s *)(GET_SMR_MSG_PTR(body_ptr)))->msg_marker));
#endif
  SNS_OS_FREE( GET_SMR_MSG_PTR(body_ptr) );
#endif /* SNS_QMI_ENABLE */
}

/*===========================================================================

  FUNCTION:  smr_set_qmi_service_obj

===========================================================================*/
/**
  @brief set service object into the routing table

  @detail

  @return None

*/
/*=========================================================================*/
void smr_set_qmi_service_obj (void)
{
  uint8_t i;
  for (i = 0; i < SNS_SMR_RTB_SIZE; i++)
  {
    SNS_ASSERT (i == sns_rtb[i].qmi_svc_num);
    if ( NULL != sns_rtb[i].svc_map.get_svc_obj )
    {
      sns_rtb[i].qmi_svc_obj = sns_rtb[i].svc_map.get_svc_obj(sns_rtb[i].svc_map.maj_ver,
          sns_rtb[i].svc_map.min_ver,
          sns_rtb[i].svc_map.tool_ver);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smr_get_svc_obj

===========================================================================*/
/*!
  @brief This function returns a service object by using the routing table.

  @param[i] svc_num: The service number which determines the service object

  @return
   The service object or NULL if the svc_num is not defined or invalid

*/
/*=========================================================================*/
qmi_idl_service_object_type  sns_smr_get_svc_obj (uint8_t svc_num)
{
  if ( svc_num < SNS_SMR_RTB_SIZE )
  {
    return sns_rtb[svc_num].qmi_svc_obj;
  }
  else
  {
    return NULL;
  }
}

#ifdef SNS_QMI_ENABLE
/*===========================================================================

  FUNCTION:   sns_smr_get_rtb_size

===========================================================================*/
/*!
  @brief This function returns the size of the routing table.

  @param[i] 

  @return
   The macro SNS_SMR_RTB_SIZE is available only in this C file. Hence a function
   is needed to access this by other external modules.

*/
/*=========================================================================*/
uint8_t  sns_smr_get_rtb_size (void)
{
  return SNS_SMR_RTB_SIZE;
}


/*===========================================================================

  FUNCTION:   sns_smr_get_module_id

===========================================================================*/
/*!
  @brief This function returns the module_id from the routing table.

  @param[i] svc_num: The service number which determines the module id

  @return
   0xFF if svc_num is invalid

*/
/*=========================================================================*/
uint8_t  sns_smr_get_module_id (uint8_t svc_num)
{
  if ( svc_num < SNS_SMR_RTB_SIZE )
  {
    return sns_rtb[svc_num].module_id;
  }
  else
  {
    return 0xFF;
  }
}
#endif
#endif /* SNS_SMR_C */
