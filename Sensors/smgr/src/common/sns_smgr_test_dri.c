/*=============================================================================
  @file sns_smgr_test_dri.c

  This file implements DRI test support.

Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_test_dri.c#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-01-29  gju  Include different file for SMR utlity functions.
  2012-03-01  PN   Initial version for 8960
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_smgr_define.h"

#if defined(FEATURE_TEST_DRI)
#include "sns_smr_util.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_common.h"
#include "sns_reg_api_v02.h"
#include "sns_smgr_hw.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_TEST_DRI_GPIO            79 
#define SNS_TEST_DRI_GPIO_HIGH_USEC  1000

#define SNS_TEST_DRI_MODE_MASK             0x03 /* 2 LSB for DRI test modes */
#define SNS_TEST_DRI_MODE_ROUND_TRIP_DELAY 0x01
#define SNS_TEST_DRI_MODE_TIME_STAMP_DELAY 0x02
#define SNS_TEST_DRI_MODE_SAMPLING_LATENCY 0x03

typedef enum
{
  SNS_TEST_DRI_STATE_PREINIT,
  SNS_TEST_DRI_STATE_REGISTRY_REQ,
  SNS_TEST_DRI_STATE_REGISTRY_WAIT,
  SNS_TEST_DRI_STATE_INIT_DONE
} sns_test_dri_state_e;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/* SNS_TEST_DRI_MODE_ */
typedef uint8_t sns_test_dri_mode_bm;

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
sns_test_dri_mode_bm test_dri_mode    = 0;
static sns_test_dri_state_e e_test_dri_state = SNS_TEST_DRI_STATE_PREINIT;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
static boolean sns_test_dri_in_round_trip_delay_mode( void )
{
  return (test_dri_mode == SNS_TEST_DRI_MODE_ROUND_TRIP_DELAY) ? TRUE : FALSE;
}

static boolean sns_test_dri_in_time_stamp_delay_mode( void )
{
  return (test_dri_mode == SNS_TEST_DRI_MODE_TIME_STAMP_DELAY) ? TRUE : FALSE;
}

static boolean sns_test_dri_in_sampling_latency_mode( void )
{
  return (test_dri_mode == SNS_TEST_DRI_MODE_SAMPLING_LATENCY) ? TRUE : FALSE;
}

#endif


/****************************************************************************/
/*                        Public Functions                                  */
/****************************************************************************/

/*===========================================================================

  FUNCTION:   sns_test_dri_init

===========================================================================*/
/*!
  @brief Called from sns_smgr_task() to initialize DRI test support.

  @detail If the message passed in is the regisry response this module is
          expecting, the message is consumed.

  @param body_ptr_ptr: if not NULL, it's a pointer to a message
  @param msg_hdr: if not NULL, it's a pointer to SMR message header
  @return none
 */
/*=========================================================================*/
void sns_test_dri_init( uint8_t** body_ptr_ptr, uint8_t* msg_header )
{
  #if defined(FEATURE_TEST_DRI)
  sns_err_code_e e_err;
  const uint16_t cu16_Id  = SNS_REG_GROUP_SENSOR_TEST_V02;
  const uint8_t  cu8_Type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
  sns_smr_header_s *msg_hdr = (sns_smr_header_s *)msg_header;

  SNS_SMGR_DEBUG2(HIGH, DBG_SMGR_GENERIC_STRING2, e_test_dri_state, body_ptr_ptr);

  switch ( e_test_dri_state )
  {
    case SNS_TEST_DRI_STATE_PREINIT:
    {
      sns_hw_gpio_out_config(SNS_TEST_DRI_GPIO);
      e_test_dri_state = SNS_TEST_DRI_STATE_REGISTRY_REQ;
      /* falls through!!!*/
    }

    case SNS_TEST_DRI_STATE_REGISTRY_REQ:
    {
      uint32_t reg_req_attempts = 0;
      e_err = SNS_ERR_FAILED;
      do
      {
        SMGR_DELAY_US( 1000 ); /* wait 1 ms */
        e_err = sns_smgr_req_reg_data(cu16_Id, cu8_Type);
      } while ( (e_err != SNS_SUCCESS) && (reg_req_attempts++ < 1000) );

      if ( e_err == SNS_SUCCESS )
      {
        e_test_dri_state = SNS_TEST_DRI_STATE_REGISTRY_WAIT; /* move on */
      }
      else /* will try again later */
      {
        SNS_SMGR_DEBUG2(ERROR, DBG_SMGR_GENERIC_STRING2, e_err, reg_req_attempts);
      }
      break;
    }

    case SNS_TEST_DRI_STATE_REGISTRY_WAIT:
    {
      uint8_t  u_err;
      uint8_t* body_ptr = NULL;
      if ( body_ptr_ptr != NULL )
      {
        body_ptr = *body_ptr_ptr;
      }
      if ( body_ptr != NULL && msg_hdr != NULL )
      {
        sns_reg_group_read_resp_msg_v02* resp_ptr =
          (sns_reg_group_read_resp_msg_v02*)body_ptr;

         if( (msg_hdr->msg_type == SNS_SMR_MSG_TYPE_RESP) &&
             (msg_hdr->msg_id == SNS_REG_GROUP_READ_REQ_V02) &&
             (resp_ptr->group_id == cu16_Id) )
        {
          if ( (SNS_SUCCESS == resp_ptr->resp.sns_result_t) &&
               (sizeof(sns_reg_sensor_test_data_group_s) == resp_ptr->data_len) )
          {
            sns_reg_sensor_test_data_group_s* test_data_ptr =
              (sns_reg_sensor_test_data_group_s*)resp_ptr->data;
            test_dri_mode = 
              (sns_test_dri_mode_bm)(test_data_ptr->test_en_flags & 
                                     SNS_TEST_DRI_MODE_MASK);
          }
          else /* received negative response */
          {
            SNS_SMGR_DEBUG2(ERROR, DBG_SMGR_GENERIC_STRING2, 
                            resp_ptr->resp.sns_result_t, resp_ptr->data_len);
            /* so we can at least test something */
            test_dri_mode = SNS_TEST_DRI_MODE_ROUND_TRIP_DELAY;
          }
          sns_smr_msg_free(body_ptr);
          *body_ptr_ptr = NULL; /* consume the message */
          e_test_dri_state = SNS_TEST_DRI_STATE_INIT_DONE;
        }
      }
      break;
    }

    default:
    {
      SNS_SMGR_DEBUG1(ERROR, DBG_SMGR_GENERIC_STRING1, e_test_dri_state);
      break;
    }
  }
  #endif
}

/*===========================================================================

  FUNCTION:   sns_test_dri_round_trip_delay

===========================================================================*/
/*!
  @brief Performs Round Trip Delay test in DRI supported targets.
 
  Test requirement:
  - GPIO goes high when SMGR receives Periodic Report Request
  - GPIO goes low when SMGR sends the first Periodic Report Indication
    belonging to new Request is sent by SMGR.

  @detail

  @param  b_req_rcvd - TRUE to indicate request was received
  @return none
 */
/*=========================================================================*/
void sns_test_dri_round_trip_delay( boolean b_req_rcvd )
{
  #if defined(FEATURE_TEST_DRI)
  /* Test requirements:
     - GPIO goes high when SMGR receives Periodic Report Request
     - GPIO goes low when SMGR sends the first Periodic Report Indication
     belonging to new Request is sent by SMGR. */
  if (sns_test_dri_in_round_trip_delay_mode())
  {
    sns_hw_gpio_out(SNS_TEST_DRI_GPIO, b_req_rcvd ? TRUE : FALSE );
  }
  #endif
}

/*===========================================================================

  FUNCTION:   sns_test_dri_timestamp_delay

===========================================================================*/
/*!
  @brief Performs Timestamp Delay test in DRI supported targets.
 
  Test requirement: GPIO goes high for short duration each time
  interrupt service is attaching time stamp into a measurement

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_test_dri_timestamp_delay( void )
{
  #if defined(FEATURE_TEST_DRI)
  /* Test requirement: GPIO goes high for short duration each time
     interrupt service is attaching time stamp into a measurement.*/
  if (sns_test_dri_in_time_stamp_delay_mode())
  {
    /* As this function is called from inside an ISR, use HWIO macro instead
       of sns_hw_gpio_out() which would indirectly enter a critical section
       and would lead to FIQ exception */
    sns_hw_gpio_out(SNS_TEST_DRI_GPIO, TRUE );
    SMGR_DELAY_US( SNS_TEST_DRI_GPIO_HIGH_USEC );
    sns_hw_gpio_out(SNS_TEST_DRI_GPIO, FALSE );
  }
  #endif
}

/*===========================================================================

  FUNCTION:   sns_test_dri_sampling_latency

===========================================================================*/
/*!
  @brief Performs Samping Latency test in DRI supported targets.
 
  Test requirement: GPIO goes high for short period each time reading
  of the sensor data triggered by DRI interrupt is complete

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_test_dri_sampling_latency( void )
{
  #if defined(FEATURE_TEST_DRI)
  /* Test requirement: GPIO goes high for short period each time reading
     of the sensor data triggered by DRI interrupt is complete */
  if (sns_test_dri_in_sampling_latency_mode())
  {
    sns_hw_gpio_out(SNS_TEST_DRI_GPIO, TRUE );
    SMGR_DELAY_US( SNS_TEST_DRI_GPIO_HIGH_USEC );
    sns_hw_gpio_out(SNS_TEST_DRI_GPIO, FALSE );
  }
  #endif
}

/*===========================================================================

  FUNCTION:   sns_test_dri_ready

===========================================================================*/
/*!
  @brief Returns TRUE if DRI test initialization is complete.
 
  @param  none
  @return boolean
 */
/*=========================================================================*/
boolean sns_test_dri_init_done( void )
{
  boolean b_ready = TRUE;

  #if defined(FEATURE_TEST_DRI)
  b_ready = (SNS_TEST_DRI_STATE_INIT_DONE == e_test_dri_state) ? TRUE : FALSE;
  #endif

  return b_ready;
}

