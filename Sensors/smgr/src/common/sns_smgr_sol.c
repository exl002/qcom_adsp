/*=============================================================================
  @file smgr_sol.c

  This file contains functions for managing the Standing Orders List (SOL) of
  the DSPS Sensor Manager (SMGR)

******************************************************************************
* Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
******************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_sol.c#3 $ */
/* $DateTime: 2014/08/18 11:42:01 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-08-12  sd   update sampling factore before calculate depot rate
  2014-05-15  sd   Refactored smgr_mark_reports_for_flushing
  2014-03-26  pn   Clears rpt_tick when deactivating a report
  2014-03-14  pn   No longer flushes reports that are still being initialized
  2014-02-26  MW   Make CT_C event sensor
  2014-02-21  pn   Requests above max freq for event sensors are no longer rejected
  2014-01-29  MW   Added support for ambient temp sensor type
  2014-01-16  pn   No longer flushes FIFO before flushing reports being deleted.
  2014-01-12  jms  Merged discrete with unified fifo support
  2014-01-07  pn   Flushes reports even during initial cycle
  2013-12-18  MW   Added support for Hall Effect sensor
  2013-11-26  pn   Flushes FIFO before flushing reports being deleted.
  2013-11-13  pn   No longer sends existing event sensor data when processing request
  2013-10-29  pn   ACK_MODIFIED response sent for modified report requests
  2013-10-23  MW   Added support for SAR sensor
  2013-10-03  pn   Buffering and Periodic report generation changes
  2013-09-24  pn   Makes use of SrcModule in determination of processor ID
  2013-09-12  MW   Added support for RGB sensor type
  2013-09-03  pn   Additional condition for report activation
  2013-09-12  DC   Added support for gesture sensor type
  2013-08-05  pk   Added support for SrcModule in buffering query request
  2013-08-09  hw   Fixed Klocwork errors
  2013-07-31  vh   Eliminated Klocwork warnings
  2013-08-28  pn   Cleans up when deactivating report
  2013-08-22  pn   More bookeeping info saved when replacing a report
  2013-08-19  pn   Flushes Buffering report before replacing it
  2013-07-23  pk   Increased report rate of buffering requests in periodic-
                   like mode
  2013-07-15  sd   Changed send_ind_during_suspend default to false
  2013-06-18  pn   Added Buffering debug message for exceeding max report count
  2013-06-12  pn   Removed unused flags field from smgr_ddf_sensor_s
  2013-06-06  pn   Available event sensors' samples must be processed before
                   newly added Periodic reports can be generated
  2013-05-30  sc   Removed un-used logging functions and flags.
  2013-06-02  pn   Added Buffering debug message for exceeding max report count
  2013-05-16  asr  Removed unused log types.
  2013-05-01  dc   Set default notification to true to support clients
                   on the Modem processor who do not use the notify field.
  2013-05-02  pk   Added SrcModule to buffering request message
  2013-04-09  pn   Changed effective_rate_hz to q16
  2013-03-29  pn   Changed condition for flushing Buffering reports
  2013-03-14  pn   Takes care of all pending Query requests when associated
                   Buffering request is deleted
  2013-03-11  pn   Reclassified DRI sensors for data synchronization purpose
  2013-03-01  pn   Sends buffering reports, if necessary, before deleting them
  2013-02-20  vh   Modified sns_smgr_schedule_periodic_report() to add appropriate debug message
  2013-02-07  dc   Save the client passed information about notification
                   during kernel suspend.
  2013-02-13  pn   Sensor Status request is associated with ddf_sensor
                   rather than with sensor
  2013-02-07  pn   Uses smgr_get_sched_intval() from DSPS 1.1
  2013-02-05  pn   Added more thorough validations for Query requests
  2013-01-09  pn   - Cleaned up smgr_update_sensor_when_item_changed()
                   - Reclassified DRI sensors
                   - Available event sensor samples sent for new report
  2013-01-08  sd   do not schedule event in  smgr_update_sensor_when_item_changed
                   if there is already one
  2012-12-26  pn   - Schedules sensor event only if necessary
                   - Computes report interval only if report rate is not zero
  2012-12-19  pn   Periodic reports are no longer flushed when ODRs change
  2012-12-14  pn   Sensor events now get their own timer
  2012-12-08  pn   - Prevents CIC filtering of PROX and/or LIGHT data
                   - Sets ReasonPair_valid flag in Buffering response.
  2012-12-05  pn   No power nodes voting needed when activating reports.
  2012-12-04  pn   Connection handle is needed to identify a unique report.
  2012-12-03  pn   Added Buffering feature
  2012-11-11  sc   Merged from DSPS 1.1; temporary solution for getting item module
  2012-10-31  jhh  Add num_client count when report is inserted / deleted
  2012-10-19  ag   Add support for src module in report_req
  2012-10-03  sc   Scale timestamp back down to 32768Hz tick.
  2012-09-19  sd   removed/featurized unused code
  2012-09-14  br   Inserted more parameter validity checking for periodic report request
  2012-09-13  vh   Eliminated compiler warnings
  2012-09-06  asj  Extended Sensor Status Indication
  2012-08-23  ag   Fix timestamps; handle 19.2MHz QTimer
  2012-07-29  ag   Porting from SMR to QMI
  2012-07-20  sd   moved sns_smgr_check_rpts_for_md_update() from sns_smgr_delete_report
                   to sns_smgr_proc_periodic_req_msg so it won't be called
                   twice when smgr replacing an existing report.
  2012-07-20  sd   moved update motion detection state before configuring filter when processing sensor report request
  2012-07-12  pn   Removed compiler warning.
  2012-07-02  sd   optimization of code
  2012-06-15  sd   Updated reset sensor function, removed set MPU6050 default ODR
  2012-06-03  sd   Added to put sensors to low power mode upon power rail is turned on
  2012-06-11  pn   Made use of sns_smgr_set_sensor_state()
  2012-06-05  ag   Make sure odr is 32 bits
  2012-06-01  sd   Moved the location to clear the sample_cnt to fix CR360015
                   Avago DRI: ALS concurrent tests report more than one ALS indication per client/report
  2012-05-24  sd   set gyro ODR during reset to cycle through invalid samples for MPU6050
  2012-04-30  br   Added latency node support
  2012-04-22  br   Fixed no indication with concurrency for event sensors
  2012-03-02  pn   Added accounting for indications.
  2012-02-28  br   Deleted unnecessary lines which makes the deice AVTIVE before reset() it.
  2012-01-20  ad   Fixed factory calibration corruption issue by initializing item specification
                   completely before linking it to the associated sensor
  2012-01-05  br   Fixed the last sample history
  2011-12-28  sd   Increased delay between setting sensor active power and sensor reset for MPU3050 configuring interrupt
                   during reset
  2011-12-27  sd   Moved set active power from immediately after turning on power rail to before sensor reset
                   because for some sensor it needs some wait time after turning on power rail.
  2011-11-28  sd   Do not change the sensor state to off if the state is failed
  2011-11-27  br   Inserted static function which is is_there_actived_this_data_type() for invalidate issued_tick.
  2011-11-22  br   Sorted schedule list by frequency
  2011-11-16  sd   moved turn off qup clock inside sensor reset function
  2011-11-14  jhh  Updated alloc and free function calls to meet new API
  2011-11-08  sd   Remove unecessary qup clock turn off that was used for sensor reset function which was moved.
  2011-10-20  sd   Use Consistent Pointer Type Between SMGR/DD When Setting Attributes
  2011-10-11  sd   make sure rpt_item_cnt do not exceed SNS_SMGR_MAX_ITEMS_PER_REPORT_V01
  2011-10-07  sd   moved sns_smgr_proc_sensor_event out of sns_smgr_reset_sensor
  2011-10-05  br   Inserted lines for MPU6050 support
  2011-09-29  sd   Updated sns_smgr_reset_sensor, use off to idle timer to reset sensor after powerrail is turned on
  2011-09-23  sd   Added 200us more time between set sensor active power to reset to ensure gyro reset successful
  2011-09-19  sd   set current_lpf_odr to 0 after sensor reset
  2011-09-14  sd   Added deblay between set sensor to active power and reset
  2011-09-13  sd   added debug message when add/delete a sensor request
  2011-09-12  sd   set sensor to high power before reset ( mainlyfor gyro )
  2011-08-26  sc   Return error when streaming is requested during test stage
  2011-08-13  sd   Added resetting DD when sensor state is off
  2011-08-13  br   removed the item from cic_que in smgr_unlink_item_to_sensor()
  2011-07-14  br   inserted lines to delete sensor from event que when report is deleting
  2011-07-13  br   updated/fixed delete report procedure for cic filter enabled report
  2011-06-27  sd   Do not delete sensor request after one client indication is sent
  2011-06-24  br   changed to support auto cal in addition to factory cal
  2011-06-17  sd   smgr code clean up
  2011-06-11  br   Fixed for potential memory leakage when MALLOC error happened
  2011-05-25  br   changed calculating scheded time in sns_smgr_register_report_into_scheduler
  2011-05-19  br   changed to deploy the default rate if the requested freq is 0
  2011-05-16  br   changed variable and function names
  2011-05-13  br   changed because of smgr_tick_s definition was changed
  2011-05-12  br   changed action schedule to event schedule to consolidate sensor related status
  2011-05-11  jb   Fix warning
  2011-05-10  sd   keep items in report linked in sensor lead when the report is moved to suspended que
  2011-05-09  sd   Check if newly added report need to move to be suspended if Motion Detection is enabled
  2011-04-29  sd   Handling a new sensor request that maybe already in motion detect suspend que
  2011-04-25  sd   Added HW MD int and power saving related features
  2011-04-25  br   bugs were fixed for releasing stream connecitons: in unlink_item_to_sensor() and delete_report()
  2011-04-24  br   deployed the result of the code review. fixed bugs after unit testing with PCSIM
  2011-04-20  br   changed for the new scheduler which deployed fixed heartbeat
  2011-02-28  sd   added some code for power status in the future
  2011-02-02  sd   Fixed schedule function was called twice when delete command it was issued, and IND stopped afterwards
  2011-02-02  jb   Resolve reports with some sensor rates maxed out below request rate
  2011-01-28  br   changed constant names for deploying QMI tool ver#2
  2011-01-28  sd   If a datatype is not supported, drop the item in the request
  2010-12-20  sd   Disregard minimum samle rate in user request
  2010-12-03  pg   Added __SNS_MODULE__ to be used by OI mem mgr in debug builds.
  2010-11-12  sc   Merge with the fix that clears flag when sol report is inserted to an empty list
  2010-10-22  sd   if SNS_SMGR_NO_SENSITIVITY is set, use default value, disregard the sensiticity value in client request
  2010-10-15  sd   Fixed delete report request not working when 2 sensors are working
  2010-10-07  jb   prevent divide by zero in buffering setup
  2010-10-06  sd   added cancel request support
  2010-10-05  jb   CC3 initial build
  2010-08-09  JB   Add sensor failure logic
  2010-08-06  JB   Complete merge
  2010-08-04  JB   Merge with revised ucos, use PC timer
  2010-07-16  JB   Modified an API parameter value name
  2010-07-09  JB   Follow API changes
  2010-06-11  JB   Initial version

============================================================================*/

#define __SNS_MODULE__ SNS_SMGR

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_osa.h"
#include "sns_memmgr.h"

#include "sns_smgr_define.h"
#include "sns_smgr.h"
#include "sns_em.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_smgr_hw.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
 /* Temporary to use ucos timer as timestamp on PCSIM */
//#define sns_em_get_timestamp()  OSTimeGet()


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*===========================================================================

  FUNCTION:   sns_smgr_sol_init

===========================================================================*/
/*!
  @brief  Initialize the standing orders list (SOL)
    Link available blocks

  @detail

  @param[in] none
  @return  none
 */
/*=========================================================================*/
void sns_smgr_sol_init( void )
{
  sns_q_init(&sns_smgr.report_queue);
  sns_q_init(&sns_smgr.sched_que);
}

/*===========================================================================

  FUNCTION:   smgr_add_reason_code

===========================================================================*/
/*!
  @brief  Add a pair (item # and reason code) to the list of reasons for failure or
    modification that will be sent in the response message.

  @detail

  @param[i]   item_idx      - 0 for report body, item 1...
  @param[i]   reason_code   - see enumeration
  @param[io]  num_pairs_ptr - number of existing items in reason pairs array
  @param[o]   reason_pairs  - array of reason pairs
  @return  none
 */
/*=========================================================================*/
static void smgr_add_reason_code( uint32_t                   item_idx,
                                  smgr_error_reason_e        reason_code,
                                  uint32_t                  *num_pairs_ptr,
                                  sns_smgr_reason_pair_s_v01 reason_pairs[] )
{
  SNS_SMGR_PRINTF3(LOW, "add_reason_code - idx=%d reason=%d pairs=%d",
                   item_idx, reason_code, *num_pairs_ptr );
  if ( *num_pairs_ptr < SNS_SMGR_MAX_NUM_REASONS_V01 )
  {
    reason_pairs[*num_pairs_ptr].ItemNum = (uint8_t)item_idx;
    reason_pairs[*num_pairs_ptr].Reason  = reason_code;
    (*num_pairs_ptr)++;
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_check_sensor_status

===========================================================================*/
/*!
  @brief Checks each sensor for status change.

  @param None

  @return None
*/
/*=========================================================================*/
void sns_smgr_check_sensor_status(void)
{
  uint8_t s, d, p;
  for ( s=0; s<ARR_SIZE(sns_smgr.sensor); s++ )
  {
    for ( d=0;
          d<ARR_SIZE(sns_smgr.sensor[s].const_ptr->data_types) &&
          sns_smgr.sensor[s].ddf_sensor_ptr[d] != NULL;
          d++ )
    {
      smgr_sensor_status_s* sensor_status_ptr =
        &sns_smgr.sensor[s].ddf_sensor_ptr[d]->sensor_status;
      for (p=0; p<ARR_SIZE(sensor_status_ptr->status_clients); p++)
      {
        smgr_status_client_info_s* status_client_ptr =
          &sensor_status_ptr->status_clients[p];
        if ( (status_client_ptr->used != FALSE) &&
             (status_client_ptr->last_status != status_client_ptr->pending_status) )
        {
          sns_smgr_generate_sensor_status_ind(sensor_status_ptr, p);
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:  sns_smgr_find_and_delete_report

===========================================================================*/
/*!
  @brief Searches active queue and suspended queue for the same report
         from the same client and deletes it.

  @param[i]  hdr_ptr   - message header of new request
  @param[i]  report_id - report ID to match
  @param[o]  found_ptr - will be set to TRUE if report is found
  @param[o]  was_suspended_ptr - will be set to TRUE if report is
                                 found in suspended queue

  @return none
 */
/*=========================================================================*/
static uint8_t sns_smgr_find_and_delete_report(
  const sns_smr_header_s* hdr_ptr,
  uint8_t                 report_id
)
{
  uint8_t ack_nak = SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01;
  smgr_rpt_spec_s* rpt_ptr;
  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.report_queue, rpt_ptr, rpt_link)
  {
    if ( (hdr_ptr->src_module  == rpt_ptr->header_abstract.src_module) &&
         (hdr_ptr->ext_clnt_id == rpt_ptr->header_abstract.ext_clnt_id) &&
         (hdr_ptr->connection_handle == rpt_ptr->header_abstract.connection_handle) &&
         (report_id == rpt_ptr->rpt_id) )
    {
      boolean suspended =
        (rpt_ptr->state == SMGR_RPT_STATE_ACTIVE) ? FALSE : TRUE;

      SNS_SMGR_PRINTF2(MED, "find_and_delete_rpt - state=%d items=%d",
                       rpt_ptr->state, rpt_ptr->num_items);

      if (  (rpt_ptr->header_abstract.msg_id == SNS_SMGR_BUFFERING_REQ_V01) &&
            (rpt_ptr->q16_rpt_rate != 0) )
      {
        /* Buffering report may have accumulated samples waiting to be sent */
        rpt_ptr->state = SMGR_RPT_STATE_FLUSHING;
        smgr_flush_reports();
      }

      sns_smgr_delete_report(rpt_ptr);
      sns_smgr_check_sensor_status();
      if ( suspended == FALSE )
      {
        sns_smgr_check_rpts_for_md_update();
      }
      ack_nak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
      break;
    }
  }
  return ack_nak;
}

/*===========================================================================

  FUNCTION:  sns_smgr_find_report

===========================================================================*/
/*!
  @brief Searches active queue and suspended queue for the same report
         from the same client and deletes it.

  @param[i]  hdr_ptr   - message header of new request
  @param[i]  report_id - report ID to match
  @param[o]  found_ptr - will be set to TRUE if report is found
  @param[o]  was_suspended_ptr - will be set to TRUE if report is
                                 found in suspended queue

  @return none
 */
/*=========================================================================*/
static smgr_rpt_spec_s* sns_smgr_find_report(
  const sns_smr_header_s* hdr_ptr,
  uint8_t                 report_id
)
{
  smgr_rpt_spec_s* found_rpt_ptr = NULL;
  smgr_rpt_spec_s* rpt_ptr;
  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.report_queue, rpt_ptr, rpt_link)
  {
    if ( (hdr_ptr->src_module  == rpt_ptr->header_abstract.src_module) &&
         (hdr_ptr->ext_clnt_id == rpt_ptr->header_abstract.ext_clnt_id) &&
         (hdr_ptr->connection_handle == rpt_ptr->header_abstract.connection_handle) &&
         (report_id == rpt_ptr->rpt_id) )
    {
      SNS_SMGR_PRINTF2(LOW, "find_report - found rpt_id=%d(0x%x)",
                       report_id, report_id);
      found_rpt_ptr = rpt_ptr;
      break;
    }
  }
  return found_rpt_ptr;
}

/*===========================================================================

  FUNCTION:   smgr_find_query_item

===========================================================================*/
/*!
  @brief Searches for the report item in the Buffering request corresponding
  to the given Query request.

  @param[i] hdr_ptr  - message header
  @param[i] req_ptr  - incoming request message body
  @param[i] item_ptr_ptr - destination for address of the corresponding item, if found

  @return
   SNS_SMGR_RESPONSE_ACK_SUCCESS_V01   - found what we're looking for
   SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01 - if no matching Buffering report found
   SNS_SMGR_RESPONSE_NAK_SENSOR_ID_V01 - if Buffering report does not include the
     requested sensor type
*/
/*=========================================================================*/
static uint8_t smgr_find_query_item(
  const sns_smr_header_s*                hdr_ptr,
  sns_smgr_buffering_query_req_msg_v01*  req_ptr,
  smgr_rpt_item_s**                      item_ptr_ptr
)
{
  uint8_t report_id = (uint8_t)req_ptr->QueryId;
  smgr_rpt_spec_s* report_ptr = sns_smgr_find_report(hdr_ptr, report_id);
  uint8_t ackNak = SNS_SMGR_RESPONSE_NAK_REPORT_ID_V01;
  if ( report_ptr != NULL )
  {
    /* found the corresponding Buffering request */
    uint8_t i;
    for ( i=0; i<report_ptr->num_items; i++ )
    {
      if ( (SMGR_SENSOR_ID(report_ptr->item_list[i]->ddf_sensor_ptr->sensor_ptr) ==
            req_ptr->SensorId) &&
           (report_ptr->item_list[i]->ddf_sensor_ptr->data_type ==
            req_ptr->DataType) )
      {
        *item_ptr_ptr = report_ptr->item_list[i];
        ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
        break;
      }
    }
    if ( ackNak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
    {
      ackNak = SNS_SMGR_RESPONSE_NAK_SENSOR_ID_V01;
    }
  }
  return ackNak;
}

/*===========================================================================

  FUNCTION:   smgr_override_response_ack

===========================================================================*/
/*!
  @brief

  @detail

  @param[io]  orig_ack_ptr - pointer to original ack code
  @param[i]   ack_code     -  ack code to be put in response

  @return  none
 */
/*=========================================================================*/
static void smgr_override_response_ack (uint8_t* orig_ack_ptr,
                                        uint8_t ack_code)
{
  SNS_SMGR_PRINTF2(HIGH, "override_resp_ack - new=%d orig=%d", ack_code, *orig_ack_ptr );
  switch ( ack_code )
  {
    case SNS_SMGR_RESPONSE_ACK_SUCCESS_V01:     /* No action is needed as SUCCESS can't
                                                   override any ack/nack code other than SUCCESS itself */
      break;
    case SNS_SMGR_RESPONSE_ACK_MODIFIED_V01:
      if ( SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 == *orig_ack_ptr )
      {
        *orig_ack_ptr = ack_code;
      }
      break;
    default:
      if ( (SNS_SMGR_RESPONSE_ACK_SUCCESS_V01  == *orig_ack_ptr) ||
           (SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 == *orig_ack_ptr) )
      {
        *orig_ack_ptr = ack_code;
      }
      break;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_change_state

===========================================================================*/
/*!
  @brief Changes the state of all sensors

  @detail  Updates state of all sensors that are not in fail state

  @param[i] state the state to be changed to

  @return
   NONE
*/
/*=========================================================================*/
void sns_smgr_change_state(smgr_sensor_state_e state)
{
  smgr_sensor_s* sensor_ptr;
  uint8_t        i;
  for ( i=0; i<SNS_SMGR_NUM_SENSORS_DEFINED ; i++ )
  {
    sensor_ptr = &sns_smgr.sensor[i];
    if ((sensor_ptr->sensor_state != state) &&
        (sensor_ptr->sensor_state != SENSOR_STATE_FAILED) )
    {
      sns_smgr_set_sensor_state(sensor_ptr, state);
      if ( state == SENSOR_STATE_OFF )
      {
        sensor_ptr->event_done_tick = 0;
      }
    }
  }
}

/*===========================================================================

  FUNCTION:  smgr_update_max_frequencies

===========================================================================*/
/*!
  @brief Updates max requested HP and overall frequencies for the given DDF sensor

  @param[i] ddf_sensor_ptr - the DDF sensor

  @return  None
*/
/*=========================================================================*/
void smgr_update_max_frequencies(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  smgr_rpt_item_s* item_ptr = ddf_sensor_ptr->rpt_item_ptr;
  ddf_sensor_ptr->max_requested_freq_hz = ddf_sensor_ptr->max_high_perf_freq_hz = 0;
  while ( item_ptr != NULL  )
  {
    if ( SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B) )
    {
      ddf_sensor_ptr->max_high_perf_freq_hz =
        MAX(ddf_sensor_ptr->max_high_perf_freq_hz, item_ptr->sampling_rate_hz);
    }
    ddf_sensor_ptr->max_requested_freq_hz =
      MAX(ddf_sensor_ptr->max_requested_freq_hz, item_ptr->sampling_rate_hz);
    item_ptr = item_ptr->next_item_ptr;
  }

  SNS_SMGR_PRINTF3(MED, "update_max_freq = sensor=%d max=%d max_hp=%d",
                   SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                    ddf_sensor_ptr->data_type),
                   ddf_sensor_ptr->max_requested_freq_hz,
                   ddf_sensor_ptr->max_high_perf_freq_hz);
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_sensor_status

===========================================================================*/
/*!
  @brief Updates given sensor status depending on client being added or removed.

  @param[i] status_ptr
  @param[i] in_item_spec_ptr the item spec in the report request

  @return None
*/
/*=========================================================================*/
void sns_smgr_update_sensor_status(smgr_sensor_status_s* status_ptr,
                                   smgr_rpt_item_s* in_item_spec_ptr)
{
  int8_t change =
    (in_item_spec_ptr->state == SENSOR_ITEM_STATE_LINKING) ? 1 : -1;
  uint8_t proc =
    (in_item_spec_ptr->parent_report_ptr->header_abstract.src_module &
     SNS_MODULE_GRP_MASK);

  status_ptr->num_requested_items += change;
  if (SNS_MODULE_DSPS == proc)
  {
    status_ptr->num_requested_dsps += change;
  }
  else
  {
    status_ptr->num_requested_apps += change;
  }
  status_ptr->time_stamp = sns_em_get_timestamp();

  for ( proc=0; proc<ARR_SIZE(status_ptr->status_clients); proc++ )
  {
    smgr_status_client_info_s* status_client_ptr = &status_ptr->status_clients[proc];
    if ( status_client_ptr->used != FALSE )
    {
      if ( (in_item_spec_ptr->state == SENSOR_ITEM_STATE_UNLINKING) &&
           (status_client_ptr->req_data_type_num == status_ptr->num_requested_items) )
      {
        status_client_ptr->pending_status = SNS_SMGR_SENSOR_STATUS_ONE_CLIENT_V01;
      }
      else if ( status_ptr->num_requested_items == 0 )
      {
        status_client_ptr->pending_status = SNS_SMGR_SENSOR_STATUS_IDLE_V01;
      }
      else
      {
        status_client_ptr->pending_status = SNS_SMGR_SENSOR_STATUS_ACTIVE_V01;
      }
    }
  }
}


/*===========================================================================

  FUNCTION:   smgr_mark_reports_for_flushing

===========================================================================*/
/*!
  @brief Changes state of affected reports to FLUSHING if ODR for their sensors
         would change.

  @param[i] ddf_sensor_ptr

  @return None
*/
/*=========================================================================*/
static void smgr_mark_reports_for_flushing(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t new_odr = sns_smgr_choose_odr(ddf_sensor_ptr->sensor_ptr);

  sns_smgr_compute_device_sampling_factor(ddf_sensor_ptr);
  if ( ((new_odr > 0) && (new_odr != ddf_sensor_ptr->current_odr)) || 
       (sns_smgr_compute_depot_data_rate(ddf_sensor_ptr) != 
        ddf_sensor_ptr->depot_data_rate) )
  {
      smgr_mark_reports_for_flushing_after_odr_change(ddf_sensor_ptr);
  }

}


/*===========================================================================

  FUNCTION:   smgr_update_sensor_when_item_changed

===========================================================================*/
/*!
  @brief Updates bookeeping info when an item is linked to or unlinked from
         the given DDF sensor.

  @param[i] ddf_sensor_ptr
  @param[i] boolean - TRUE indicates an item is linked, FALSE unlinked
  @param[i] in_item_spec_ptr the item spec in the new report or to bedeleted
  @return None
*/
/*=========================================================================*/
static void smgr_update_sensor_when_item_changed (smgr_ddf_sensor_s* ddf_sensor_ptr,
                                                  smgr_rpt_item_s* in_item_spec_ptr)
{
  smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  int8_t change;
  uint8_t sensor_flags_bit;

  SNS_SMGR_PRINTF3(HIGH, "item_changed - sensor=%d items=%d item_state=%d",
                   SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                    ddf_sensor_ptr->data_type),
                   ddf_sensor_ptr->num_rpt_items, in_item_spec_ptr->state);

  if ( in_item_spec_ptr->state == SENSOR_ITEM_STATE_LINKING )
  {
    change               = 1;
    sensor_flags_bit     = SMGR_SENSOR_FLAGS_ITEM_ADD_B;
  }
  else
  {
    change               = -1;
    sensor_flags_bit     = SMGR_SENSOR_FLAGS_ITEM_DEL_B;
  }

  ddf_sensor_ptr->num_rpt_items += change;

  smgr_update_max_frequencies(ddf_sensor_ptr);
  sns_smgr_update_sensor_status(&ddf_sensor_ptr->sensor_status, in_item_spec_ptr);

  SMGR_BIT_SET(sensor_ptr->flags, sensor_flags_bit);

  if ( sensor_ptr->event_done_tick == 0 )
  {
    sensor_ptr->event_done_tick = sns_em_get_timestamp();
    sns_smgr_schedule_sensor_event(1);
  }

  smgr_mark_reports_for_flushing(ddf_sensor_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_link_item_to_sensor

===========================================================================*/
/*!
  @brief associate a item spec from the sensor that the item have associated with

  @Detail if this is the first for the data type, invalidate the latest issued tick

  @param[i] in_item_spec_ptr - Point to the report specification structure

  @return
   TRUE the item spec associated successfully to the sensor
   FALSE  the item spec could not associate to the sensor because of some reasons
*/
/*=========================================================================*/
static void sns_smgr_link_item_to_sensor(smgr_rpt_item_s* in_item_spec_ptr)
{
  smgr_ddf_sensor_s* ddf_sensor_ptr;
  smgr_rpt_item_s** item_ptr_ptr;

  SNS_ASSERT(NULL != in_item_spec_ptr);
  ddf_sensor_ptr = in_item_spec_ptr->ddf_sensor_ptr;
  item_ptr_ptr = &ddf_sensor_ptr->rpt_item_ptr;

  while ( *item_ptr_ptr != NULL )
  {
    item_ptr_ptr = &(*item_ptr_ptr)->next_item_ptr;
  }
  *item_ptr_ptr = in_item_spec_ptr;
  in_item_spec_ptr->state = SENSOR_ITEM_STATE_LINKING;
  smgr_update_sensor_when_item_changed(ddf_sensor_ptr, in_item_spec_ptr);
}

/*===========================================================================

  FUNCTION:   smgr_unlink_item_to_sensor

===========================================================================*/
/*!
  @brief de-associate a item spec from the sensor that the item have associated with

  @Detail

  @param[i] ItemSpec_p - Point to the report specification structure

  @return
   TRUE the item spec de-associated successfully
   FAIL could not de-associate because the item spec was not associated with the sensor
*/
/*=========================================================================*/
static boolean smgr_unlink_item_to_sensor(smgr_rpt_item_s* in_item_spec_ptr)
{
  boolean found = FALSE;
  smgr_ddf_sensor_s* ddf_sensor_ptr;
  smgr_rpt_item_s** item_ptr_ptr;

  SNS_ASSERT( NULL != in_item_spec_ptr);

  ddf_sensor_ptr = in_item_spec_ptr->ddf_sensor_ptr;
  item_ptr_ptr = &ddf_sensor_ptr->rpt_item_ptr;
  while ( (*item_ptr_ptr != in_item_spec_ptr) && (*item_ptr_ptr != NULL) )
  {
    item_ptr_ptr = &(*item_ptr_ptr)->next_item_ptr;
  }
  if ( *item_ptr_ptr == in_item_spec_ptr )     /* found the element */
  {
    *item_ptr_ptr  = in_item_spec_ptr->next_item_ptr;
    in_item_spec_ptr->next_item_ptr = NULL;
    in_item_spec_ptr->state = SENSOR_ITEM_STATE_UNLINKING;

    smgr_update_sensor_when_item_changed(ddf_sensor_ptr, in_item_spec_ptr);
    found = TRUE;
  }
  return found;
}

/*===========================================================================

  FUNCTION:   smgr_parse_item_decimation

===========================================================================*/
/*!
  @brief Parses Decimation field of report request.

  @Detail

  @param[i] decimation - the Decimation from report request
  @param[i] item_ptr   - the report item to be updated based on decimation value

  @return
   SNS_SMGR_REASON_NULL_V01 - everything's fine
   SNS_SMGR_REASON_DEFAULT_DECIM_V01 - Decimation value requested can not be honored
*/
/*=========================================================================*/
static smgr_error_reason_e smgr_parse_item_decimation(uint8_t          decimation,
                                                      smgr_rpt_item_s* item_ptr)
{
  smgr_error_reason_e reason_code  = SNS_SMGR_REASON_NULL_V01;
  sns_ddf_sensor_e ddf_sensor_type =
    SMGR_SENSOR_TYPE(item_ptr->ddf_sensor_ptr->sensor_ptr,
                     item_ptr->ddf_sensor_ptr->data_type);


  if ( (SNS_SMGR_DECIMATION_FILTER_V01  == decimation) ||
       (SNS_SMGR_DECIMATION_AVERAGE_V01 == decimation) )
  {
    if ( (ddf_sensor_type != SNS_DDF_SENSOR_PROXIMITY) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_AMBIENT) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_IR_GESTURE) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_HUMIDITY) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_RGB) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_CT_C) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_SAR) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_HALL_EFFECT) &&
         (ddf_sensor_type != SNS_DDF_SENSOR_AMBIENT_TEMP) )
    {
      SMGR_BIT_SET(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_DECIM_FILTER_B);
    }
    else
    {
      SNS_SMGR_PRINTF2(ERROR, "parse_item_decim - ddf_sensor=%d decim=%d",
                       ddf_sensor_type, decimation);
      reason_code = SNS_SMGR_REASON_DEFAULT_DECIM_V01;
    }
  }
  else if ( SNS_SMGR_DECIMATION_RECENT_SAMPLE_V01 != decimation )
  {
    reason_code = SNS_SMGR_REASON_DEFAULT_DECIM_V01;
  }
  return reason_code;
}

/*===========================================================================

  FUNCTION:   smgr_parse_periodic_report_item

===========================================================================*/
/*!
  @brief Fill the item spec structure with the input parameter

  @Detail: SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 could override SNS_SMGR_RESPONSE_ACK_SUCCESS_V01
    NAK could overide any ACKs.

  @param ItemDef_p[i] points to the structure of incoming request message
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] cal_sel - calibraion selection
  @resp_ptr[i] Point to instance of response message structure

  @return
   TRUE Successfully filled the item spec
   FALSE Failed to fill the itemp spec because:
   - the sensor state is SENSOR_STATE_FAILED or otherwise not ready
   - Linking the item to the sensor had failed
   - etc
*/
/*=========================================================================*/
static boolean smgr_parse_periodic_report_item(
  uint8_t                                     item_idx,
  const sns_smgr_periodic_report_req_msg_v01* req_ptr,
  smgr_rpt_item_s*                            item_ptr,
  sns_smgr_periodic_report_resp_msg_v01*      resp_ptr
)
{
  boolean success = FALSE;
  const sns_smgr_periodic_report_item_s_v01* in_item_ptr;
  smgr_sensor_s *sensor_ptr;

  in_item_ptr = &req_ptr->Item[item_idx];
  sensor_ptr = sns_smgr_find_sensor(in_item_ptr->SensorId);

  SNS_SMGR_PRINTF2(LOW, "parse_periodic_item - sensor=%d dtype=%d",
                   in_item_ptr->SensorId, in_item_ptr->DataType);

  if ( (sensor_ptr == NULL) || !SMGR_DATATYPE_VALID(in_item_ptr->DataType) )
  {
    /* Unknown sensor ID, delete incoming item */
    smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_UNKNOWN_SENSOR_V01,
                         &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
  }
  else if ( (SENSOR_STATE_FAILED   == sensor_ptr->sensor_state) ||
            (SENSOR_STATE_PRE_INIT == sensor_ptr->sensor_state) ||
            (SENSOR_STATE_TESTING  == sensor_ptr->sensor_state) ||
            (NULL == sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType]) )
  {
    /* Device not ready or data type not supported, delete incoming item */
    smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_FAILED_SENSOR_V01,
                         &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
  }
  else if ( (req_ptr->ReportRate >
             sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType]->max_supported_freq_hz) &&
            !sns_smgr_is_event_sensor(sensor_ptr, in_item_ptr->DataType) )
  {
    smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLING_RATE_V01,
                         &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
    SNS_SMGR_PRINTF2(ERROR, "parse_periodic_item - req_rate=%d max_freq=%d",
                     req_ptr->ReportRate,
                     sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType]->
                     max_supported_freq_hz);
  }
  else if ( (req_ptr->ReportRate == 0) &&
            (SNS_SMGR_REPORT_RATE_DEFAULT_V01 >
             sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType]->max_supported_freq_hz) &&
            (req_ptr->Item_len > 1) )
  {
    smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLING_RATE_V01,
                         &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
  }
  else
  {
    SNS_OS_MEMZERO(item_ptr, sizeof(smgr_rpt_item_s));
    item_ptr->ddf_sensor_ptr    =
      sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType];
    item_ptr->resulting_sample.status = SNS_DDF_PENDING;
    item_ptr->sampling_rate_hz  = req_ptr->ReportRate;
    if ( req_ptr->ReportRate > item_ptr->ddf_sensor_ptr->max_supported_freq_hz )
    {
      item_ptr->sampling_rate_hz  = item_ptr->ddf_sensor_ptr->max_supported_freq_hz;
      smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_DEFAULT_RATE_V01,
                           &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
      SNS_SMGR_PRINTF2(ERROR, "parse_periodic_item - req_rate=%d max_freq=%d",
                       req_ptr->ReportRate,
                       item_ptr->ddf_sensor_ptr->max_supported_freq_hz);
    }
    if ( item_ptr->sampling_rate_hz == 0 )
    {
      item_ptr->sampling_rate_hz = SNS_SMGR_REPORT_RATE_DEFAULT_V01;
      if ( item_ptr->sampling_rate_hz >
           sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType]->max_supported_freq_hz )
      {
        item_ptr->sampling_rate_hz =
          sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType]->max_supported_freq_hz;
      }
    }
    item_ptr->sampling_interval = smgr_get_sched_intval(item_ptr->sampling_rate_hz);
    item_ptr->sampling_factor   = 1;

    item_ptr->sensitivity = in_item_ptr->Sensitivity;
    if ( smgr_parse_item_decimation(in_item_ptr->Decimation, item_ptr) != 0 )
    {
      smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_DEFAULT_DECIM_V01,
                           &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
    }

    /* when cal_sel_valid is FALSE, cal_sel[i] shall be 0 */
    item_ptr->cal_sel = req_ptr->cal_sel[item_idx];
    if ( (FALSE == req_ptr->cal_sel_valid) || (item_idx >= req_ptr->cal_sel_len ) )
    {
      item_ptr->cal_sel = SNS_SMGR_CAL_SEL_FULL_CAL_V01;
    }

    sns_smgr_cic_init(&item_ptr->cic_obj, 1, item_ptr->sampling_rate_hz,
                      item_ptr->ddf_sensor_ptr->depot_ptr->num_axes);

    sns_smgr_cbuff_init(&(item_ptr->cbuff_obj),
                        item_ptr->ddf_sensor_ptr->sensor_ptr);

    if ( (req_ptr->SampleQuality_valid != FALSE) &&
         (item_idx < req_ptr->SampleQuality_len) &&
         (req_ptr->SampleQuality[item_idx] &
          SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01) )
    {
      if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
      {
        SMGR_BIT_SET(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B);
      }
      else
      {
        smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLE_QUALITY_NORMAL_V01,
                             &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
      }
    }
    SNS_SMGR_PRINTF1(LOW, "parse_periodic_item - flags=0x%x", item_ptr->flags);
    success = TRUE;
  }
  if ( resp_ptr->ReasonPair_len > 0 )
  {
    smgr_override_response_ack(&resp_ptr->AckNak, SNS_SMGR_RESPONSE_ACK_MODIFIED_V01);
  }
  return success;
}

/*===========================================================================

  FUNCTION:   smgr_parse_buffering_item

===========================================================================*/
/*!
  @brief Fill the item spec with the input

  @Detail: SNS_SMGR_RESPONSE_ACK_MODIFIED_V01 could override SNS_SMGR_RESPONSE_ACK_SUCCESS_V01
    NAK could overide any ACKs.

  @param ItemDef_p[i] points to the structure of incoming request message
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] ItemSpec_p - Point to the report specification structure
  @param[i] cal_sel - calibraion selection
  @resp_ptr[i] Point to instance of response message structure

  @return
   TRUE Successfully filled the item spec
   FALSE Failed to fill the itemp spec becasue of:
   - the sensor state is SENSOR_STATE_FAILED
   - Linking the item to the sensor was fail
   - etc
*/
/*=========================================================================*/
static boolean smgr_parse_buffering_item(
  uint8_t                               item_idx,
  const sns_smgr_buffering_req_msg_v01 *req_ptr,
  smgr_rpt_item_s                      *item_ptr,
  sns_smgr_buffering_resp_msg_v01      *resp_ptr
)
{
  boolean success = FALSE;
  const sns_smgr_buffering_req_item_s_v01* in_item_ptr;
  smgr_sensor_s *sensor_ptr;
  smgr_ddf_sensor_s* ddf_sensor_ptr = NULL;

  in_item_ptr = &req_ptr->Item[item_idx];
  sensor_ptr = sns_smgr_find_sensor(in_item_ptr->SensorId);

  SNS_SMGR_PRINTF3(LOW, "parse_buffering_item - sensor=%d dtype=%d rate=%d",
                   in_item_ptr->SensorId, in_item_ptr->DataType,
                   in_item_ptr->SamplingRate);

  if ( (sensor_ptr != NULL) && SMGR_DATATYPE_VALID(in_item_ptr->DataType) )
  {
    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[in_item_ptr->DataType];
  }

  if ( (sensor_ptr == NULL) || !SMGR_DATATYPE_VALID(in_item_ptr->DataType) )
  {
    /* Unknown sensor ID, delete incoming item */
    smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_UNKNOWN_SENSOR_V01,
                         &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
  }
  else if ( (SENSOR_STATE_FAILED   == sensor_ptr->sensor_state) ||
            (SENSOR_STATE_PRE_INIT == sensor_ptr->sensor_state) ||
            (SENSOR_STATE_TESTING  == sensor_ptr->sensor_state) ||
            (NULL == ddf_sensor_ptr) )
  {
    /* Unknown sensor ID, delete incoming item */
    smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_FAILED_SENSOR_V01,
                         &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
    SNS_SMGR_PRINTF2(ERROR, "parse_buffering_item - sensor=%d state=%d",
                     in_item_ptr->SensorId, sensor_ptr->sensor_state);
  }
  else if ( (in_item_ptr->SamplingRate < 1) ||
            (in_item_ptr->SamplingRate > ddf_sensor_ptr->max_supported_freq_hz) )
  {
    smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLING_RATE_V01,
                         &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
    SNS_SMGR_PRINTF2(ERROR, "parse_buffering_item - req_rate=%d max=%d",
                     in_item_ptr->SamplingRate,
                     ddf_sensor_ptr->max_supported_freq_hz);
  }
  else
  {
    SNS_OS_MEMZERO(item_ptr, sizeof(smgr_rpt_item_s));
    item_ptr->ddf_sensor_ptr    = ddf_sensor_ptr;
    item_ptr->sampling_rate_hz  = in_item_ptr->SamplingRate;
    item_ptr->sampling_interval = smgr_get_sched_intval(item_ptr->sampling_rate_hz);
    item_ptr->sampling_factor   = 1;

    if ( smgr_parse_item_decimation(in_item_ptr->Decimation, item_ptr) != 0 )
    {
      smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_DEFAULT_DECIM_V01,
                           &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
    }

    /* when cal_sel_valid is FALSE, cal_sel[i] shall be 0 */
    item_ptr->cal_sel = in_item_ptr->Calibration;

    sns_smgr_cic_init(&item_ptr->cic_obj, 1, item_ptr->sampling_rate_hz,
                      item_ptr->ddf_sensor_ptr->depot_ptr->num_axes);

    sns_smgr_cbuff_init(&(item_ptr->cbuff_obj),
                        item_ptr->ddf_sensor_ptr->sensor_ptr);

    if ( in_item_ptr->SampleQuality & SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01 )
    {
      if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
      {
        SMGR_BIT_SET(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B);
      }
      else
      {
        smgr_add_reason_code(item_idx+1, SNS_SMGR_REASON_SAMPLE_QUALITY_NORMAL_V01,
                             &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
      }
    }
    SNS_SMGR_PRINTF1(LOW, "parse_buffering_item - flags=0x%x", item_ptr->flags);
    success = TRUE;
  }
  if ( resp_ptr->ReasonPair_len > 0 )
  {
    smgr_override_response_ack(&resp_ptr->AckNak, SNS_SMGR_RESPONSE_ACK_MODIFIED_V01);
  }
  return success;
}

/*===========================================================================

  FUNCTION:   smgr_copy_message_header

===========================================================================*/
/*!
  @brief Copies the SMR message header for future use.

  @param[i]  hdr_ptr          - source
  @param[io] hdr_abstract_ptr - destination

  @return
   None
*/
/*=========================================================================*/
static void smgr_copy_message_header(const sns_smr_header_s* hdr_ptr,
                                     smgr_header_abstract_s* hdr_abstract_ptr)
{
  hdr_abstract_ptr->src_module        = hdr_ptr->src_module;
  hdr_abstract_ptr->ext_clnt_id       = hdr_ptr->ext_clnt_id;
  hdr_abstract_ptr->priority          = hdr_ptr->priority;
  hdr_abstract_ptr->txn_id            = hdr_ptr->txn_id;
  hdr_abstract_ptr->msg_id            = hdr_ptr->msg_id;
  hdr_abstract_ptr->connection_handle = hdr_ptr->connection_handle;
}

/*===========================================================================

  FUNCTION:   smgr_parse_report_request

===========================================================================*/
/*!
  @brief Parses the incoming periodic or buffering report request

  @param[i]  req_ptr  - the incoming report request
  @param[io] resp_ptr - response to the request
  @param[io] rpt_ptr  - report specification

  @return
    SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 - if all goes well
    SNS_SMGR_RESPONSE_NAK_RESOURCES_V01 - out of mem
    SNS_SMGR_RESPONSE_NAK_NO_ITEMS_V01 - no item saved
*/
/*=========================================================================*/
static uint8_t smgr_parse_report_request(
  const sns_smr_header_s* hdr_ptr,
  void*                   req_ptr,
  void*                   resp_ptr,
  smgr_rpt_spec_s*        rpt_ptr
)
{
  uint8_t i, item_len, ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  sns_suspend_notification_s_v01* suspend_notif_ptr = NULL;

  SNS_OS_MEMZERO(rpt_ptr, sizeof(smgr_rpt_spec_s));
  sns_q_link(rpt_ptr, &rpt_ptr->rpt_link);
  smgr_copy_message_header(hdr_ptr, &rpt_ptr->header_abstract);

  if ( rpt_ptr->header_abstract.msg_id == SNS_SMGR_REPORT_REQ_V01 )
  {
    sns_smgr_periodic_report_req_msg_v01*  pr_req_ptr =
      (sns_smgr_periodic_report_req_msg_v01*)req_ptr;
    item_len              = pr_req_ptr->Item_len;
    rpt_ptr->rpt_id       = pr_req_ptr->ReportId;
    if ( pr_req_ptr->notify_suspend_valid )
    {
      suspend_notif_ptr   = &pr_req_ptr->notify_suspend;
    }
    SNS_SMGR_PRINTF1(HIGH, "parse_report_req - periodic rate %d", pr_req_ptr->ReportRate);

#ifdef SMGR_BUFFERED_RPT_ON
    rpt_ptr->buff_factor = pr_req_ptr->BufferFactor;
#endif
  }
  else
  {
    sns_smgr_buffering_req_msg_v01*  pr_req_ptr =
      (sns_smgr_buffering_req_msg_v01*)req_ptr;
    item_len              = pr_req_ptr->Item_len;
    rpt_ptr->rpt_id       = pr_req_ptr->ReportId;
    if ( pr_req_ptr->notify_suspend_valid )
    {
      suspend_notif_ptr   = &pr_req_ptr->notify_suspend;
    }
    SNS_SMGR_PRINTF1(HIGH, "parse_report_req - buffering rate 0x%X ",
                     pr_req_ptr->ReportRate);
  }

  /* proc_type and send_ind_during_suspend */
  if ( suspend_notif_ptr != NULL )
  {
    rpt_ptr->proc_type = suspend_notif_ptr->proc_type;
    rpt_ptr->send_ind_during_suspend = 
      suspend_notif_ptr->send_indications_during_suspend;
  }
  else
  {
    if ( hdr_ptr->src_module < SNS_MODULE_DSPS )
    {
      rpt_ptr->proc_type = SNS_PROC_APPS_V01;
    }
    else if ( hdr_ptr->src_module < SNS_MODULE_MDM )
    {
      rpt_ptr->proc_type = SNS_PROC_SSC_V01;
    }
    else
    {
      rpt_ptr->proc_type = SNS_PROC_MODEM_V01;
    }
    rpt_ptr->send_ind_during_suspend = false;
  }

  for (i=0; i<item_len && ackNak==SNS_SMGR_RESPONSE_ACK_SUCCESS_V01; i++)
  {
    smgr_rpt_item_s *item_ptr = SNS_OS_MALLOC( SNS_DBG_MOD_DSPS_SMGR,
                                               sizeof(smgr_rpt_item_s) );
    if ( NULL != item_ptr )
    {
      boolean success = FALSE;
      if ( rpt_ptr->header_abstract.msg_id == SNS_SMGR_REPORT_REQ_V01 )
      {
        success = smgr_parse_periodic_report_item(i, req_ptr, item_ptr, resp_ptr);
      }
      else
      {
        success = smgr_parse_buffering_item(i, req_ptr, item_ptr, resp_ptr);
      }
      SNS_SMGR_PRINTF2(LOW, "parse_report_req - i=%d success=%d", i, success);
      if (success == FALSE )
      {
        SNS_OS_FREE(item_ptr);
      }
      else
      {
        item_ptr->parent_report_ptr = rpt_ptr;
        rpt_ptr->item_list[rpt_ptr->num_items++] = item_ptr;
        if ( SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B) ||
             sns_smgr_is_event_sensor(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                      item_ptr->ddf_sensor_ptr->data_type) )
        {
          SMGR_BIT_SET(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_DRI_SENSOR_B);
          rpt_ptr->num_dri_sensors++;
        }
      }
    }
    else
    {
      ackNak = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
      SNS_SMGR_PRINTF1(ERROR, "parse_report_req - no resource for rpt_id=%d",
                       rpt_ptr->rpt_id);
    }
  }
  SNS_SMGR_PRINTF2(LOW, "parse_report_req - rpt_ptr=%d items=%d",
                   rpt_ptr->rpt_id, rpt_ptr->num_items);

  if ( ((ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) || 
        (ackNak == SNS_SMGR_RESPONSE_ACK_MODIFIED_V01)) &&
       (rpt_ptr->num_items == 0) )
  {
    ackNak = SNS_SMGR_RESPONSE_NAK_NO_ITEMS_V01;
  }
  SNS_SMGR_PRINTF1(HIGH, "parse_report_req - ackNak=%d", ackNak);
  return ackNak;
}

/*===========================================================================

  FUNCTION:   smgr_get_sched_intval

===========================================================================*/
/*!
  @brief Calculate an interval from requested freq

  @detail If the freq is factor of SMGR_ALIGN_FREQ, align its interval.
  @param[i] req_freq - The freq in Hz

  @return
   The timetick of the scheduling period
*/
/*=========================================================================*/
uint32_t smgr_get_sched_intval(uint16_t req_freq)
{
  uint32_t interval_tick;

  /* check if the freq is factor of SMGR_ALIGN_FREQ, and align its interval */
  if ( SMGR_ALIGN_FREQ == (SMGR_ALIGN_FREQ/req_freq * req_freq) )
  {
    uint32_t us_per_tick = sns_em_convert_dspstick_to_usec(1);
    interval_tick = 1000000/us_per_tick/SMGR_ALIGN_FREQ;
    interval_tick *= SMGR_ALIGN_FREQ/req_freq;
  }
  else
  {
    interval_tick = DSPS_SLEEP_CLK/req_freq;
  }
  return interval_tick;
}

/*===========================================================================

  FUNCTION:   sns_smgr_delete_report

===========================================================================*/
/*!
  @brief Remove report block and item blocks.

  @param[i] rpt_spec_ptr: the report to be deleted

  @return
   None
*/
/*=========================================================================*/
void sns_smgr_delete_report(smgr_rpt_spec_s* rpt_spec_ptr)
{
  sns_q_s* q_ptr;
  SNS_ASSERT(rpt_spec_ptr != NULL);

  sns_smgr_ready_reports_list_remove(rpt_spec_ptr);
  while ( rpt_spec_ptr->num_items > 0 )
  {
    smgr_rpt_item_s* item_ptr = rpt_spec_ptr->item_list[--rpt_spec_ptr->num_items];
    while ( item_ptr->query_ptr != NULL )
    {
      smgr_query_s* query_ptr = item_ptr->query_ptr;
      item_ptr->query_ptr = query_ptr->next_query_ptr;
      smgr_send_empty_query_indication(query_ptr);
      SNS_OS_FREE(query_ptr);
    }
    smgr_unlink_item_to_sensor(item_ptr);
    sns_smgr_cbuff_deinit(&(item_ptr->cbuff_obj));

    SNS_OS_FREE(item_ptr);
  }
  q_ptr = SMGR_QUE_HEADER_FROM_LINK(&rpt_spec_ptr->rpt_link);
  if ( NULL != q_ptr )
  {
    sns_q_delete(&rpt_spec_ptr->rpt_link);
    SNS_SMGR_PRINTF1(MED, "delete_report - %d reports left", sns_q_cnt(q_ptr));
  }
  SNS_OS_FREE(rpt_spec_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_deactivate_report

===========================================================================*/
/*!
  @brief

  @param[i] rpt_spec_ptr: the report to be deactivated

  @return
   None
*/
/*=========================================================================*/
void sns_smgr_deactivate_report(smgr_rpt_spec_s* rpt_spec_ptr)
{
  uint8_t i;
  for ( i=0; i<rpt_spec_ptr->num_items; i++ )
  {
    smgr_rpt_item_s* item_ptr = rpt_spec_ptr->item_list[i];
    smgr_unlink_item_to_sensor(item_ptr);
    SNS_OS_MEMZERO(&item_ptr->interpolator_obj, sizeof(smgr_interpolator_obj_s));
    sns_smgr_cic_reset(&item_ptr->cic_obj);
    item_ptr->last_processed_sample_timestamp = 0;
    item_ptr->ts_last_sent = 0;
  }
  rpt_spec_ptr->rpt_tick  = 0;
  rpt_spec_ptr->ind_count = 0;
  rpt_spec_ptr->state = SMGR_RPT_STATE_INACTIVE;
}

/*===========================================================================

  FUNCTION:   sns_smgr_activate_report

===========================================================================*/
/*!
  @brief

  @param[i] rpt_spec_ptr: the report to be activated

  @return
   None
*/
/*=========================================================================*/
void sns_smgr_activate_report(smgr_rpt_spec_s* rpt_spec_ptr)
{
  uint8_t i;
  for ( i=0; i<rpt_spec_ptr->num_items; i++ )
  {
    sns_smgr_link_item_to_sensor(rpt_spec_ptr->item_list[i]);
  }
  rpt_spec_ptr->state = SMGR_RPT_STATE_ACTIVE;
}

/*===========================================================================

  FUNCTION:   sns_smgr_schedule_report

===========================================================================*/
/*!
  @brief Schedules a report.

  @param[i] report_ptr - the new report to schedule

  @return
   None
*/
/*=========================================================================*/
static void smgr_schedule_report(
  const sns_smr_header_s* hdr_ptr,
  smgr_rpt_spec_s*        report_ptr)
{
  uint8_t i;
  smgr_rpt_spec_s* existing_report_ptr =
    sns_smgr_find_report(hdr_ptr, report_ptr->rpt_id);
  if ( existing_report_ptr != NULL )
  {
    /* Client is updating an existing report;
       bookeeping info must be copied from old report */
    uint8_t ie;
    if (  (existing_report_ptr->header_abstract.msg_id == SNS_SMGR_BUFFERING_REQ_V01) &&
          (existing_report_ptr->q16_rpt_rate != 0) )
    {
      /* Buffering report may have accumulated samples waiting to be sent */
      existing_report_ptr->state = SMGR_RPT_STATE_FLUSHING;
      smgr_flush_reports();
    }
    for ( i=0; i<report_ptr->num_items; i++ )
    {
      smgr_rpt_item_s* new_item_ptr = report_ptr->item_list[i];
      for ( ie=0; ie<existing_report_ptr->num_items; ie++ )
      {
        smgr_rpt_item_s* old_item_ptr = existing_report_ptr->item_list[ie];
        if ( old_item_ptr->ddf_sensor_ptr == new_item_ptr->ddf_sensor_ptr )
        {
          new_item_ptr->ts_last_sent = old_item_ptr->ts_last_sent;
          new_item_ptr->last_processed_sample_timestamp = 
            old_item_ptr->last_processed_sample_timestamp;
          new_item_ptr->flags |= 
            (old_item_ptr->flags & SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B);
          new_item_ptr->interpolator_obj = old_item_ptr->interpolator_obj;
          break;
        }
      }
    }
    report_ptr->ind_count = existing_report_ptr->ind_count;
    sns_smgr_delete_report(existing_report_ptr);
  }

  sns_q_put(&sns_smgr.report_queue, &report_ptr->rpt_link);
  SNS_SMGR_PRINTF1(HIGH, "schedule_report - reports=%d",
                   sns_q_cnt(&sns_smgr.report_queue));

  /* Determine if report should not be activated */
  for ( i=0; i<report_ptr->num_items; i++ )
  {
    if ( sns_smgr.md.is_md_int_enabled &&
         SMGR_SENSOR_IS_ACCEL(report_ptr->item_list[i]->
                              ddf_sensor_ptr->sensor_ptr) &&
         sns_smgr_check_accel_rpt_coupled_with_md( 
                report_ptr->rpt_id,
                report_ptr->header_abstract.src_module) )
    {
      report_ptr->state = SMGR_RPT_STATE_INACTIVE;
      break;
    }
  }

  if ( report_ptr->state == SMGR_RPT_STATE_ACTIVE )
  {
    sns_smgr_activate_report(report_ptr);
    sns_smgr_check_rpts_for_md_update();
  }

  sns_smgr_check_sensor_status();
}

/*===========================================================================

  FUNCTION:   sns_smgr_schedule_periodic_report

===========================================================================*/
/*!
  @brief Schedules new Periodic report.

  @param[i]  hdr_ptr  - header for the incoming request
  @param[i]  req_ptr  - incoming request message
  @param[io] resp_ptr - response message structure

  @return
   None
*/
/*=========================================================================*/
static void sns_smgr_schedule_periodic_report(
  const sns_smr_header_s*                hdr_ptr,
  sns_smgr_periodic_report_req_msg_v01*  req_ptr,
  sns_smgr_periodic_report_resp_msg_v01* resp_ptr
)
{
  smgr_rpt_spec_s *rpt_ptr = NULL;

  if ( SNS_SMGR_MAX_REPORT_CNT > sns_q_cnt(&sns_smgr.report_queue) )
  {
    rpt_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(smgr_rpt_spec_s));
  }
  else
  {
    smgr_override_response_ack(&resp_ptr->AckNak,
                               SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
    SNS_SMGR_PRINTF0(ERROR, "schedule_periodic_report - max# of reports present");
  }

  if ( NULL == rpt_ptr )
  {
    if ( resp_ptr->AckNak != SNS_SMGR_RESPONSE_NAK_RESOURCES_V01 )
    {
      smgr_override_response_ack(&resp_ptr->AckNak,
                                 SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
      SNS_SMGR_PRINTF1(ERROR, "schedule_periodic_report - alloc %d bytes failed",
                       sizeof(smgr_rpt_spec_s));
    }
  }
  else
  {
    uint8_t ackNak =
      smgr_parse_report_request(hdr_ptr, req_ptr, resp_ptr, rpt_ptr);
    if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
    {
      if ( req_ptr->ReportRate == 0 )
      {
        req_ptr->ReportRate = SNS_SMGR_REPORT_RATE_DEFAULT_V01;
        smgr_add_reason_code(0, SNS_SMGR_REASON_DEFAULT_RATE_V01,
                             &resp_ptr->ReasonPair_len, resp_ptr->ReasonPair);
        smgr_override_response_ack(&resp_ptr->AckNak,
                                   SNS_SMGR_RESPONSE_ACK_MODIFIED_V01);
      }
      rpt_ptr->q16_rpt_rate = FX_FLTTOFIX_Q16(req_ptr->ReportRate);
      rpt_ptr->rpt_interval = smgr_get_sched_intval(req_ptr->ReportRate);

      smgr_schedule_report(hdr_ptr, rpt_ptr);
    }
    else
    {
      smgr_override_response_ack(&resp_ptr->AckNak, ackNak);
      sns_smgr_delete_report(rpt_ptr);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_periodic_req_msg

===========================================================================*/
/*!
  @brief Called from the input driver when a queued message has been identified as a
    periodic request.
    - Add, modify or delete a SOL entry consisting of the report spec structure
      plus subordinate item structures. Subordinate structures have
      dependencies on sensor sampling structures. When possible, connect to or
      disconnect from sensors without causing changes in the established
      sampling pattern. Set flags if sensor plan must be recalculated.
    - Send a response message for the request message providing ACK or NAK.
      Provide reasons for NAK or dropped items. Notify when defaults have been
      applied.

  @param[i] hdr_ptr - Point to header structure for the incoming request
  @param[i] req_ptr - points to the structure of incoming request message
  @param[i] resp_ptr - Point to instance of response message structure

  @return
   None
*/
/*=========================================================================*/
void
sns_smgr_proc_periodic_req_msg(sns_smr_header_s* hdr_ptr,
                               sns_smgr_periodic_report_req_msg_v01*  req_ptr,
                               sns_smgr_periodic_report_resp_msg_v01* resp_ptr )
{
  sns_smr_header_s resp_msg_header;
  uint8_t ack_nak;

  #if defined(FEATURE_TEST_DRI)
  sns_test_dri_round_trip_delay( TRUE );
  #endif /* defined(FEATURE_TEST_DRI) */

#ifdef SNS_QMI_ENABLE
  hdr_ptr->src_module = 0;
  if ( req_ptr->SrcModule_valid )
  {
    hdr_ptr->src_module = req_ptr->SrcModule;
  }
#endif

  SNS_SMGR_PRINTF3(MED, "periodic_req - rpt_id=%d src=%d conn=0x%x",
                   req_ptr->ReportId, hdr_ptr->src_module,
                   hdr_ptr->connection_handle);
  SNS_SMGR_PRINTF3(MED, "periodic_req - action=%d rate=%d items=%d",
                   req_ptr->Action, req_ptr->ReportRate, req_ptr->Item_len);

  resp_ptr->ReportId = req_ptr->ReportId;
  resp_ptr->ReasonPair_len = 0;
  resp_ptr->AckNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;

  switch( req_ptr->Action )
  {
    case SNS_SMGR_REPORT_ACTION_ADD_V01:
      sns_smgr_schedule_periodic_report(hdr_ptr, req_ptr, resp_ptr);
      break;
    case SNS_SMGR_REPORT_ACTION_DELETE_V01:
      ack_nak = sns_smgr_find_and_delete_report(hdr_ptr, req_ptr->ReportId);
      smgr_override_response_ack(&resp_ptr->AckNak, ack_nak);
      break;
    default:
      /* Unknown action code */
      smgr_override_response_ack(&resp_ptr->AckNak,
                                 SNS_SMGR_RESPONSE_NAK_UNK_ACTION_V01);
      break;
  }

  if ( (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_MODIFIED_V01) )
  {
     resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
     resp_ptr->Resp.sns_err_t = SENSOR1_EFAILED;
  }

  resp_msg_header.msg_id   = SNS_SMGR_REPORT_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_periodic_report_resp_msg_v01);
  sns_smgr_send_resp(&resp_msg_header, resp_ptr, *hdr_ptr);

  smgr_flush_reports();
}

/*===========================================================================

  FUNCTION:   sns_smgr_validate_buffering_rate

===========================================================================*/
/*!
  @brief Validates report rate and sampling rates.

  @param[i] req_ptr - points to the structure of incoming request message

  @return
   SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 - rates in proper ranges
   SNS_SMGR_RESPONSE_NAK_REPORT_RATE_V01 - see the code
*/
/*=========================================================================*/
static uint8_t sns_smgr_validate_buffering_rate(
  sns_smgr_buffering_req_msg_v01*  req_ptr
)
{
  uint8_t ackNak = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  uint32_t min_sampling_rate = 0xFFFF;
  uint32_t max_sampling_rate = 1;
  uint8_t i;
  for ( i=0; i<req_ptr->Item_len && i<ARR_SIZE(req_ptr->Item); i++ )
  {
    min_sampling_rate = MIN(min_sampling_rate, req_ptr->Item[i].SamplingRate);
    max_sampling_rate = MAX(max_sampling_rate, req_ptr->Item[i].SamplingRate);
  }

  if ( ((req_ptr->ReportRate > FX_ONE_Q16) && /* greater than 1Hz*/
        (req_ptr->ReportRate > FX_FLTTOFIX_Q16(max_sampling_rate))) ||
       ((req_ptr->ReportRate == 0) &&
        (min_sampling_rate < SMGR_MIN_EIS_SAMPLING_RATE)) )
  {
    ackNak = SNS_SMGR_RESPONSE_NAK_REPORT_RATE_V01;
  }
  return ackNak;
}

/*===========================================================================

  FUNCTION:   sns_smgr_schedule_buffering_report

===========================================================================*/
/*!
  @brief Insert a buffering report to a schedule block.

  @param[i] hdr_ptr - Point to header structure for the incoming request
  @param[i] req_ptr - points to the structure of incoming request message
  @param[i] resp_ptr - Point to instance of response message structure

  @return
   None
*/
/*=========================================================================*/
static void sns_smgr_schedule_buffering_report(
  const sns_smr_header_s*          hdr_ptr,
  sns_smgr_buffering_req_msg_v01*  req_ptr,
  sns_smgr_buffering_resp_msg_v01* resp_ptr
)
{
  smgr_rpt_spec_s* rpt_ptr = NULL;

  if ( SNS_SMGR_MAX_REPORT_CNT > sns_q_cnt(&sns_smgr.report_queue) )
  {
    rpt_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(smgr_rpt_spec_s));
  }
  else
  {
    smgr_override_response_ack(&resp_ptr->AckNak,
                               SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
    SNS_SMGR_PRINTF0(ERROR, "schedule_periodic_report - max# of reports present");
  }

  if ( NULL == rpt_ptr )
  {
    smgr_override_response_ack(&resp_ptr->AckNak,
                               SNS_SMGR_RESPONSE_NAK_RESOURCES_V01);
    SNS_SMGR_PRINTF1(ERROR, "schedule_buffering_rpt - alloc %d bytes failed",
                     sizeof(smgr_rpt_spec_s));
  }
  else
  {
    uint8_t ackNak = sns_smgr_validate_buffering_rate(req_ptr);
    if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
    {
      ackNak = smgr_parse_report_request(hdr_ptr, req_ptr, resp_ptr, rpt_ptr);
    }
    if ( ackNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
    {
      uint32_t i;
      uint32_t rpt_rate = FX_FIXTOFLT_Q16(req_ptr->ReportRate);
      rpt_ptr->q16_rpt_rate = req_ptr->ReportRate;
      if ( rpt_ptr->q16_rpt_rate != 0 )
      {
        uint32_t max_sampling_rate = 0;
        rpt_ptr->periodic_like = TRUE;
        for ( i=0; i<rpt_ptr->num_items; i++ )
        {
          max_sampling_rate = MAX(max_sampling_rate,
                                  rpt_ptr->item_list[i]->sampling_rate_hz);
          if ( rpt_rate != rpt_ptr->item_list[i]->sampling_rate_hz )
          {
            rpt_ptr->periodic_like = FALSE;
          }
        }
        rpt_ptr->q16_rpt_rate = MIN(rpt_ptr->q16_rpt_rate,
                                    FX_FLTTOFIX_Q16(max_sampling_rate));
        SNS_SMGR_PRINTF2(LOW, "schedule_buffering_rpt - max_smpl_rate=%d per_like=%d",
                         max_sampling_rate, rpt_ptr->periodic_like);
      }
      smgr_schedule_report(hdr_ptr, rpt_ptr);
    }
    else
    {
      smgr_override_response_ack(&resp_ptr->AckNak, ackNak);
      sns_smgr_delete_report(rpt_ptr);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_buffering_req_msg

===========================================================================*/
/*!
  @brief Called from the input driver when a queued message has been identified as a
    buffering request.
    - Add, modify or delete a SOL entry consisting of the report spec structure
      plus subordinate item structures. Subordinate structures have
      dependencies on sensor sampling structures. When possible, connect to or
      disconnect from sensors without causing changes in the established
      sampling pattern. Set flags if sensor plan must be recalculated.
    - Sends a response message for the request message providing ACK or NAK.
      Provides reasons for NAK or dropped items. Notifies when defaults have been
      applied.

  @param[i] hdr_ptr  - Point to header structure for the incoming request
  @param[i] req_ptr  - incoming request message
  @param[i] resp_ptr - response message to build

  @return
   None
*/
/*=========================================================================*/
void sns_smgr_proc_buffering_req_msg(
  sns_smr_header_s*          hdr_ptr,
  sns_smgr_buffering_req_msg_v01*  req_ptr,
  sns_smgr_buffering_resp_msg_v01* resp_ptr
)
{
  uint8_t ack_nak;
  sns_smr_header_s resp_msg_header;

  SNS_SMGR_PRINTF3(MED, "proc_buffering_req - rpt_id=%d action=%d items=%d",
                   req_ptr->ReportId, req_ptr->Action, req_ptr->Item_len);

  resp_ptr->ReportId       = req_ptr->ReportId;
  resp_ptr->ReportId_valid = TRUE;
  resp_ptr->ReasonPair_len = 0;
  resp_ptr->AckNak         = SNS_SMGR_RESPONSE_ACK_SUCCESS_V01;
  resp_ptr->AckNak_valid   = TRUE;
  resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;

  hdr_ptr->src_module = 0;
  if ( req_ptr->SrcModule_valid )
  {
    hdr_ptr->src_module = req_ptr->SrcModule;
  }
  SNS_SMGR_PRINTF3(MED, "proc_buffering_req - src_valid=%d src=%d conn=0x%x",
                   req_ptr->SrcModule_valid, hdr_ptr->src_module,
                   hdr_ptr->connection_handle);

  switch( req_ptr->Action )
  {
    case SNS_SMGR_BUFFERING_ACTION_ADD_V01:
      sns_smgr_schedule_buffering_report(hdr_ptr, req_ptr, resp_ptr);
      break;
    case SNS_SMGR_BUFFERING_ACTION_DELETE_V01:
      ack_nak = sns_smgr_find_and_delete_report(hdr_ptr, req_ptr->ReportId);
      smgr_override_response_ack(&resp_ptr->AckNak, ack_nak);
      break;
    default:
      /* Unknown action code */
      smgr_override_response_ack(&resp_ptr->AckNak,
                                 SNS_SMGR_RESPONSE_NAK_UNK_ACTION_V01);
      break;
  }

  if ( (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (resp_ptr->AckNak != SNS_SMGR_RESPONSE_ACK_MODIFIED_V01) )
  {
     resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
     resp_ptr->Resp.sns_err_t = SENSOR1_EFAILED;
  }
  if ( resp_ptr->ReasonPair_len > 0 )
  {
    resp_ptr->ReasonPair_valid = TRUE;
  }

  resp_msg_header.msg_id   = SNS_SMGR_BUFFERING_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_buffering_resp_msg_v01);
  sns_smgr_send_resp(&resp_msg_header, resp_ptr, *hdr_ptr);

  smgr_flush_reports();
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_buffering_query_req_msg

===========================================================================*/
/*!
  @brief Called from the input driver when a queued message has been identified as a
    Buffering Query request.
    The same client is expected to have already sent Buffering request for
    the sensor specified in this Query request.

  @param[i] hdr_ptr  - message header
  @param[i] req_ptr  - incoming request message
  @param[i] resp_ptr - response message to build

  @return
   None
*/
/*=========================================================================*/
void sns_smgr_proc_buffering_query_req_msg(
  sns_smr_header_s*                hdr_ptr,
  sns_smgr_buffering_query_req_msg_v01*  req_ptr,
  sns_smgr_buffering_query_resp_msg_v01* resp_ptr
)
{
  sns_smr_header_s resp_msg_header;
  smgr_rpt_item_s* item_ptr = NULL;
  smgr_query_s**   qpp = NULL;
  smgr_query_s*    query_ptr = NULL;
  sns_ddf_time_t   time_now = sns_em_get_timestamp();

  SNS_SMGR_PRINTF3(MED, "query_req - id=0x%x sensor=%d dtype=%d",
                   req_ptr->QueryId, req_ptr->SensorId, req_ptr->DataType);
  SNS_SMGR_PRINTF3(MED, "query_req - T0=%u T1=%u now=%u",
                   req_ptr->TimePeriod[0], req_ptr->TimePeriod[1], time_now);

  hdr_ptr->src_module = 0;
  if ( req_ptr->SrcModule_valid )
  {
    hdr_ptr->src_module = req_ptr->SrcModule;
  }

  resp_ptr->QueryId_valid  = TRUE;
  resp_ptr->QueryId        = req_ptr->QueryId;
  resp_ptr->AckNak_valid   = TRUE;
  resp_ptr->AckNak         = smgr_find_query_item(hdr_ptr, req_ptr, &item_ptr);

  /* Verify that Query ID is unique and there are fewer than max outstanding queries */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (item_ptr != NULL) )
  {
    uint8_t outstanding_queries = 0;
    for ( qpp=&item_ptr->query_ptr; *qpp!=NULL && (query_ptr=*qpp)!=FALSE;
          qpp=&(*qpp)->next_query_ptr )
    {
      if ( query_ptr->query_id == req_ptr->QueryId )
      {
        /* Query IDs must be unique; reject this request */
        resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_QUERY_ID_V01;
        break;
      }
      outstanding_queries++;
    }
    if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
         (outstanding_queries >= SMGR_MAX_EIS_QUERY_REQUESTS) )
    {
      resp_ptr->AckNak = SNS_SMGR_REASON_OTHER_FAILURE_V01;
      SNS_SMGR_PRINTF0(ERROR, "query_req - max outstanding queries");
    }
  }

  /* Sanity check for T0 and T1 */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (req_ptr->TimePeriod[0] >= req_ptr->TimePeriod[1]) )
  {
    resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_TIME_PERIOD_V01;
    SNS_SMGR_PRINTF0(ERROR, "query_req - T1 < T0");
  }

  /* Verify that T0 and T1 meet the requirements */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 ) &&
       (item_ptr != NULL) )
  {
    uint8_t num_samples = ((req_ptr->TimePeriod[1] - req_ptr->TimePeriod[0])/
                           item_ptr->sampling_interval);
    uint32_t time_for_max_samples =
      item_ptr->sampling_interval * SMGR_MAX_EIS_QUERY_SAMPLES;
    uint32_t lower_time_limit = time_now - time_for_max_samples;
    uint32_t upper_time_limit = time_now + time_for_max_samples;

    if ( (num_samples > SMGR_MAX_EIS_QUERY_SAMPLES) ||
         (req_ptr->TimePeriod[0] > upper_time_limit) ||
         (req_ptr->TimePeriod[1] < lower_time_limit) )
    {
      resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_TIME_PERIOD_V01;
      SNS_SMGR_PRINTF3(ERROR, "query_req - samples=%d lower=%u upper=%u",
                       num_samples, lower_time_limit, upper_time_limit);
    }
  }

  /* Everything checks out */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (qpp != NULL) )
  {
    query_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(smgr_query_s));
    if ( query_ptr != NULL )
    {
      SNS_OS_MEMZERO(query_ptr, sizeof(smgr_query_s));
      query_ptr->item_ptr = item_ptr;
      query_ptr->query_id = req_ptr->QueryId;
      query_ptr->T0       = req_ptr->TimePeriod[0];
      query_ptr->T1       = req_ptr->TimePeriod[1];
      smgr_copy_message_header(hdr_ptr, &query_ptr->header_abstract);
      *qpp = query_ptr;
    }
    else
    {
      resp_ptr->AckNak = SNS_SMGR_RESPONSE_NAK_RESOURCES_V01;
    }
  }

  /* See if data is available for the requested time period */
  if ( (resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01) &&
       (item_ptr != NULL) )
  {
    const smgr_sample_s* oldest_sample_ptr =
      sns_smgr_get_oldest_sample(item_ptr->ddf_sensor_ptr->depot_ptr);
    const smgr_sample_s* latest_sample_ptr =
      sns_smgr_get_latest_sample(item_ptr->ddf_sensor_ptr->depot_ptr);
    if ( (oldest_sample_ptr != NULL) && (latest_sample_ptr != NULL) )
    {
      SNS_SMGR_PRINTF2(LOW, "query_req - oldest=%u latest=%u",
                       oldest_sample_ptr->time_stamp,
                       latest_sample_ptr->time_stamp);
    }
    if ( (latest_sample_ptr != NULL) &&
         (req_ptr->TimePeriod[1] < latest_sample_ptr->time_stamp) )
    {
      /* data available for the time period */
      sns_smgr_ready_reports_list_add(item_ptr->parent_report_ptr);
    }
    /* else, when T1 is reached, report will be generated */
  }

  if ( resp_ptr->AckNak == SNS_SMGR_RESPONSE_ACK_SUCCESS_V01 )
  {
    resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;
  }
  else
  {
    resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_EFAILED;
  }
  SNS_SMGR_PRINTF1(HIGH, "query_req - ackNak=%d", resp_ptr->AckNak);

  resp_msg_header.msg_id   = SNS_SMGR_BUFFERING_QUERY_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_buffering_query_resp_msg_v01);
  sns_smgr_send_resp(&resp_msg_header, resp_ptr, *hdr_ptr);
}

