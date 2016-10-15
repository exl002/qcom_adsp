
/*=============================================================================
  @file sns_smgr_fifo.c

  This file contains the logic for managing sensor discrete fifo
  in the DSPS Sensor Manager (SMGR)

*******************************************************************************
* Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-06-18  hw   flush fifo data when there's shared fifo sensors enabled
  2014-05-23  sd   update wm calculation in  sns_smgr_fifo_calculate_wm_and_div   
  2014-04-07  sd   updated get_suitable_max_watermark to prevent fifo overflow
  2014-03-06  jms  Removing synchronization and enabling separate interrupt for each fifo
  2014-02-25  sd   Limit when to use lower interrupt rate sensor to synchronize FIFO
  2014-02-19  jms  Improved fifo sync interrupt source selection
                   - Intr rate closest to global max report rate for all fifos
  2014-01-24  sd   flush fifo before enable FIFO int even watermark is not changed.
                   Modified sensor_id in fifo event log.
  2014-01-12  jms  Merged discrete with unified fifo support
  2013-12-23  jms  Initial version

============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <limits.h>

#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_smgr_define.h"
#include "sns_dd.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "fixed_point.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_smgr_hw.h"
#include "sns_reg_api_v02.h"
#include "sns_profiling.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

//suitable_max_watermark = SNS_SMGR_FIFO_CALC_SUITABLE_MAX_WM(max_watermark);
#define SNS_SMGR_FIFO_SAFE_WM_PERCENT 90
#define SNS_SMGR_FIFO_CALC_SUITABLE_MAX_WM(max_wm)\
  ((max_wm * SNS_SMGR_FIFO_SAFE_WM_PERCENT) / 100)


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Function Prototypes
 * -------------------------------------------------------------------------*/

static void sns_smgr_fifo_log_event
(
  const smgr_ddf_sensor_s* ddf_sensor_ptr,
  sns_fifo_event_e fifo_event
);
static void sns_smgr_fifo_flush_sync( const smgr_sensor_s* sensor_ptr );
static sns_ddf_status_e sns_smgr_fifo_set_watermark
(
  const smgr_sensor_s*  sensor_ptr,
  sns_ddf_sensor_e      sensor,
  uint16_t              desired_watermark
);
static void sns_smgr_fifo_set_active_info
(
  const smgr_sensor_s* sensor_ptr,
  smgr_ddf_sensor_s*   ddf_sensor_ptr
);

static uint16_t sns_smgr_fifo_calculate_suitable_max_watermark( smgr_sensor_s* sensor_ptr );

static void sns_smgr_fifo_discrete_calculate_watermarks( void );
static void sns_smgr_fifo_discrete_configure( smgr_sensor_s* sensor_ptr );

static sns_err_code_e sns_smgr_fifo_unified_calculate_watermarks(smgr_sensor_s* sensor_ptr);
static sns_err_code_e sns_smgr_fifo_unified_configure( smgr_sensor_s* sensor_ptr );
static bool sns_smgr_fifo_has_active_shared_sensor( smgr_sensor_s* sensor_ptr );

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


//============================================================================
//============================================================================
//                       I N I T    F I F O
//============================================================================
//============================================================================

/*===========================================================================
  FUNCTION:   sns_smgr_find_sensor_by_ddf_sensor_type_and_dd_handle
===========================================================================*/
/*!
  @brief finds smgr_sensor_s* - pointer to sensor
  @details Identifies sensor by ddf_sensor_type and dd_handle_ptr

  @param[in] sns_ddf_sensor_e  ddf_sensor_type
  @param[in] sns_ddf_handle_t  dd_handle_ptr
  @return
   smgr_sensor_s* - pointer to the sensor of interest
 */
/*=====================================================================*/
static smgr_sensor_s* sns_smgr_find_sensor_by_ddf_sensor_type_and_dd_handle
(
  sns_ddf_sensor_e       ddf_sensor_type,
  const sns_ddf_handle_t dd_handle_ptr
)
{
  uint8_t k;
  for ( k=0; k<ARR_SIZE(sns_smgr.sensor); k++ )
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[k];
    uint8_t i;
    for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
    {
      if ( sensor_ptr->dd_handle           == dd_handle_ptr &&
           SMGR_SENSOR_TYPE(sensor_ptr, i) == ddf_sensor_type )
      {
        return sensor_ptr;
      }
    }
  }
  return NULL;
}

/*===========================================================================
  FUNCTION:   sns_smgr_init_fifo_cfg
===========================================================================*/
/*!
  @brief Initializes the fifo configuration of given sensor.
  @param[in/out] sensor_ptr - the sensor of interest
  @return
   none
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_fifo_init_fifo_cfg( smgr_sensor_s *sensor_ptr )
{
  uint8_t  i    = 0;
  uint32_t len  = 0;
  uint16_t shared_sensor_cnt = 0;
  sns_ddf_fifo_attrib_get_s* fifo_attr_ptr = NULL;
  sns_ddf_status_e           ddf_status    = SNS_DDF_SUCCESS;

  SNS_OS_MEMSET(&(sensor_ptr->fifo_cfg),0,sizeof(smgr_sensor_fifo_cfg_s));

  if ( !SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) )
  {
    return SNS_ERR_NOTALLOWED;
  }

  ddf_status = sns_smgr_get_attr( sensor_ptr,
                                  SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                                  SNS_DDF_ATTRIB_FIFO,
                                  (void**)&fifo_attr_ptr,
                                  &len );

  if ( SNS_DDF_SUCCESS != ddf_status )
  {
    SNS_SMGR_PRINTF1( ERROR, "FIFO Init: get_attr ERROR, SensId=%d",
                      SMGR_SENSOR_ID(sensor_ptr) );
    return SNS_ERR_FAILED;
  }
  sensor_ptr->fifo_cfg.is_fifo_supported  = fifo_attr_ptr->is_supported;
  sensor_ptr->fifo_cfg.is_hw_watermark_int_supported = !(fifo_attr_ptr->is_sw_watermark);
  sensor_ptr->fifo_cfg.max_watermark      = fifo_attr_ptr->max_watermark;
  sensor_ptr->fifo_cfg.is_fifo_enabled    = false;
  if ( fifo_attr_ptr->is_supported )
  {
    sensor_ptr->fifo_cfg.can_fifo_be_used = true;
  }

  if ( !sensor_ptr->fifo_cfg.is_fifo_supported ||
        sensor_ptr->fifo_cfg.max_watermark == 0 )
  {
    SNS_SMGR_PRINTF3( ERROR,
                      "FIFO Init: fifoSupported=%d maxWM=%d, SensId=%d",
                      sensor_ptr->fifo_cfg.is_fifo_supported,
                      sensor_ptr->fifo_cfg.max_watermark, SMGR_SENSOR_ID(sensor_ptr) );
    return SNS_ERR_NOTALLOWED;
  }

  //----------------- Shared fifo - sensor siblings ------------------------
  shared_sensor_cnt = 0;
  for ( i=0; i < fifo_attr_ptr->share_sensor_cnt; i++ )
  {
    smgr_fifo_shared_sensor_s* shared_sen_ptr =
        &sensor_ptr->fifo_cfg.shared_ddf_sensor[shared_sensor_cnt];

    shared_sen_ptr->sibling_ddf_sensor_type = fifo_attr_ptr->share_sensors[i]->sensor;
    shared_sen_ptr->sibling_dd_handle_ptr   = fifo_attr_ptr->share_sensors[i]->dd_handle;

    if ( shared_sen_ptr->sibling_dd_handle_ptr == NULL )
    {
      SNS_SMGR_PRINTF1( ERROR,
                        "FIFO Init: sibling_dd_handle_ptr == NULL, SensType=%d",
                        shared_sen_ptr->sibling_ddf_sensor_type );
      continue;
    }

    shared_sen_ptr->sensor_ptr = sns_smgr_find_sensor_by_ddf_sensor_type_and_dd_handle(
          shared_sen_ptr->sibling_ddf_sensor_type,
          shared_sen_ptr->sibling_dd_handle_ptr );

    if ( shared_sen_ptr->sensor_ptr == NULL )
    {
      SNS_SMGR_PRINTF1( ERROR,
                        "FIFO Init: Cannot find shared sensor, SensType=%d",
                        shared_sen_ptr->sibling_ddf_sensor_type );
      continue;
    }

    if ( !SMGR_SENSOR_FIFO_IS_ENABLE(shared_sen_ptr->sensor_ptr) )
    {
      SNS_SMGR_PRINTF1( ERROR,
                        "FIFO Init: FIFO NOT ENABLE, SensType=%d",
                        shared_sen_ptr->sibling_ddf_sensor_type );
      continue;
    }

    shared_sen_ptr->sibling_sensor_id = shared_sen_ptr->sensor_ptr->const_ptr->sensor_id;

    shared_sensor_cnt++;
  }
  sensor_ptr->fifo_cfg.shared_sensor_cnt = shared_sensor_cnt;
  //-------------------------------------------------------------------------

  sns_smgr_fifo_calculate_suitable_max_watermark( sensor_ptr );

  sns_ddf_memhandler_free(&sensor_ptr->memhandler);
  return SNS_SUCCESS;
}

//============================================================================
//============================================================================
//            F I F O     E V E N T     P R O C E S S I N G
//============================================================================
//============================================================================

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_sns_ddf_event_odr_changed
===========================================================================*/
/*!
  @brief configuring fifo on SNS_DDF_EVENT_ODR_CHANGED
  @param[in/out] ddf_sensor_ptr
  @param[in] sensor
  @return none
*/
/*=========================================================================*/
void sns_smgr_fifo_on_event_odr_changed
(
  smgr_ddf_sensor_s* ddf_sensor_ptr,
  sns_ddf_sensor_e   sensor
)
{
  SNS_SMGR_FIFO_DBG1( MED,"FIFO SNS_DDF_EVENT_ODR_CHANGED ddf_sensor=%d", sensor );
  sns_smgr_fifo_configure( ddf_sensor_ptr->sensor_ptr );
  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_ODR_CHANGED );
}

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_sns_ddf_event_fifo_wm_int
===========================================================================*/
/*!
  @brief processing SNS_DDF_EVENT_FIFO_WM_INT event
  @param[in] ddf_sensor_ptr
  @param[in] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_wm_int
(
  const smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     sensor
)
{
  smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  //SNS_SMGR_FIFO_DBG1( MED,"FIFO SNS_DDF_EVENT_FIFO_WM_INT ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* Check if the sync interrupt is enabled/configured */
  if ( ddf_sensor_ptr == sns_smgr.fifo_active_info.ddf_sensor_sync_intr_ptr )
  {
    /* Go to the list for which the data needs to be read and
         then call get_data on those other sensors as well. e,g temp */
    sns_smgr_fifo_flush_sync( ddf_sensor_ptr->sensor_ptr );
  }
  else
  if ( ddf_sensor_ptr->data_type == SNS_SMGR_DATA_TYPE_SECONDARY_V01 )
  {
    smgr_get_data(ddf_sensor_ptr); //sensor_type SMGR_SENSOR_TYPE_SECONDARY
  }
  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_WM_INT );
  return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_sns_ddf_event_fifo_overflow
===========================================================================*/
/*!
  @brief reconfiguring fifo on SNS_DDF_EVENT_FIFO_OVERFLOW
  @param[in] sensor_ptr
  @param[in] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_overflow
(
  const smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     sensor
)
{
  smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  SNS_SMGR_FIFO_DBG1( MED,"FIFO SNS_DDF_EVENT_FIFO_OVERFLOW ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  //Reduce desired_watermark by 50%
  uint16_t desired_watermark = (sensor_ptr->fifo_cfg.current_watermark >> 1);

  sns_smgr_fifo_flush( sensor_ptr );
  if ( desired_watermark > 0 )
  {
    /* Set the new desired watermark only if it's value is  1 or more */
    sns_smgr_fifo_set_watermark( sensor_ptr, sensor, desired_watermark );
  }
  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_OVERFLOW );
  return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_event_fifo_available
===========================================================================*/
/*!
  @brief Fifo is available since the DD enabled fifo
         - configuring fifo on SNS_DDF_EVENT_FIFO_AVAILABLE event
  @param[in/out] sensor_ptr
  @param[in] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_available
(
  smgr_sensor_s*    sensor_ptr,
  sns_ddf_sensor_e  sensor
)
{
  smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  SNS_SMGR_FIFO_DBG1( MED, "FIFO SNS_DDF_EVENT_FIFO_AVAILABLE ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  sns_smgr_fifo_configure( sensor_ptr );
  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_AVAILABLE );
  return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_smgr_fifo_on_sns_ddf_event_fifo_unavailable
===========================================================================*/
/*!
  @brief Fifo is unavailable since the DD disabled fifo
         - clearing fifo structures onSNS_DDF_EVENT_FIFO_UNAVAILABLE
  @param[i] sensor_ptr
  @param[i] sensor
  @return sns_ddf_status_e
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_unavailable
(
  smgr_sensor_s*    sensor_ptr,
  sns_ddf_sensor_e  sensor
)
{
  smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr_match_ddf_sensor( sensor_ptr, sensor );
  SNS_SMGR_FIFO_DBG1( MED, "FIFO SNS_DDF_EVENT_FIFO_UNAVAILABLE ddf_sensor=%d", sensor );
  if ( ddf_sensor_ptr == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  //TODO: erase all fields -> sns_smgr_fifo_discrete_clean_sync_info();
  sensor_ptr->fifo_cfg.can_fifo_be_used  = false;
  sensor_ptr->fifo_cfg.current_watermark = 0;
  sensor_ptr->fifo_cfg.is_fifo_enabled   = false;

  sns_smgr_fifo_log_event( ddf_sensor_ptr, SNS_DDF_FIFO_DISABLED );
  return SNS_DDF_SUCCESS;
}



//============================================================================
//============================================================================
//                      P U B L I C     U T I L S
//============================================================================
//============================================================================

/*===========================================================================
FUNCTION:   sns_smgr_fifo_log_event
===========================================================================*/
/*!
  @brief Log an occurrence of a fifo event.
  @param[in] ddf_sensor_ptr   - the sensor pointer
  @param[in] sns_fifo_event_e - fifo_event
  @return none
 */
/*=========================================================================*/
void sns_smgr_fifo_log_event
(
  const smgr_ddf_sensor_s* ddf_sensor_ptr,
  sns_fifo_event_e         fifo_event
)
{
  sns_err_code_e        status = SNS_ERR_UNKNOWN;
  sns_log_fifo_event_s* log    = NULL;

  if( ddf_sensor_ptr == NULL )
  {
    SNS_SMGR_FIFO_DBG0(ERROR, "sns_smgr_log_fifo_event - Received NULL Pointer");
    return;
  }
  status = sns_logpkt_malloc( SNS_LOG_SENSOR_FIFO_EVENT,
                              sizeof(sns_log_fifo_event_s), (void**)&log );
  if( status != SNS_SUCCESS || log == NULL )
  {
    return;
  }

  log->version = SNS_LOG_FIFO_EVENT_VERSION;
  log->timestamp = sns_em_get_timestamp();
  log->sensor_id = SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,SNS_SMGR_DATA_TYPE_PRIMARY_V01);
  log->fifo_event = fifo_event;
  log->watermark_depth =
    ddf_sensor_ptr->sensor_ptr->fifo_cfg.current_watermark;
  log->odr = ddf_sensor_ptr->current_odr;
  log->hw_watermark_int_supported =
    ddf_sensor_ptr->sensor_ptr->fifo_cfg.is_hw_watermark_int_supported;
  log->max_watermark =
    ddf_sensor_ptr->sensor_ptr->fifo_cfg.max_watermark;
  /*Commit log (also frees up the log packet memory)*/
  sns_logpkt_commit(SNS_LOG_SENSOR_FIFO_EVENT, log);
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_flush
===========================================================================*/
/*!
  @brief Flush FIFO for the sensors.
  @details
  - This Function flushes fifo for the respective sensors.
  @param[in] sensor_ptr - the sensor leader structure
  @return NONE
 */
/*=========================================================================*/
void sns_smgr_fifo_flush( const smgr_sensor_s* sensor_ptr )
{
  if ( SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) )
  {
    if ( (sensor_ptr->fifo_cfg.current_watermark >= 1 && sensor_ptr->fifo_cfg.is_fifo_enabled) ||
         sns_smgr_fifo_has_active_shared_sensor(sensor_ptr) )
    {
      sns_ddf_status_e ddf_status = SNS_DDF_EFAIL;

      if ( sns_smgr.fifo_active_info.ddf_sensor_sync_intr_ptr )
      {
        sns_smgr_fifo_flush_sync(sensor_ptr);
      }

      /*Flush now, num_samples=0,return data now.*/
      if ( (SENSOR_STATE_FAILED != sensor_ptr->sensor_state) &&
           (NULL != SMGR_DRV_FN_PTR(sensor_ptr)->trigger_fifo_data) )
      {
        ddf_status = SMGR_DRV_FN_PTR(sensor_ptr)->trigger_fifo_data(
              sensor_ptr->dd_handle,
              SMGR_SENSOR_TYPE(sensor_ptr,SNS_SMGR_DATA_TYPE_PRIMARY_V01),0,true );
      }
      //TODO: else

      if( ddf_status != SNS_DDF_SUCCESS )
      {
        SNS_SMGR_PRINTF2( ERROR,
                          "sns_smgr_flush_fifo -  Flushing FIFO Failed ddf_status=%d, %d",
                          ddf_status, SMGR_SENSOR_ID(sensor_ptr) );
      }
      sns_smgr_fifo_log_event((sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01]),
        SNS_DDF_FIFO_READ_FLUSH);
    }
  }
}

/*===========================================================================
FUNCTION:   sns_smgr_config_fifo
===========================================================================*/
/*!
  @brief configure FIFO for the sensor
  @details
  - This Function configures FIFO if necessary by
    recalculating watermark and sets it if it has changed.
  - Call this function for any change
    in report rates or the sensor ODR properties.
  @param[in/out] sensor_ptr - the sensor leader structure
  @return NONE
 */
/*=========================================================================*/
void sns_smgr_fifo_configure( smgr_sensor_s* sensor_ptr )
{
  if ( !SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) ||
       !SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
  {
    return;
  }

  if ( SNS_SMGR_FIFO_IS_UNIFIED(sensor_ptr) )
  {
    sns_smgr_fifo_unified_configure(sensor_ptr);
  }
  else
  {
    sns_smgr_fifo_discrete_configure(sensor_ptr);
  }
}

//============================================================================
//============================================================================
//                   P R O T E C T E D     U T I L S
//============================================================================
//============================================================================

/*===========================================================================
FUNCTION:   sns_smgr_fifo_clean_fifo_cfg
===========================================================================*/
/*!
  @brief clean fifo_cfg for all sensors
  @details
  @param[in] none
  @return NONE
 */
/*=====================================================================*/
static void sns_smgr_fifo_clean_fifo_cfg( void )
{
  uint8_t i;
  //------- Clear/Initialize the fifo_cfg of all sensors. NONFIFO too --------
  //SNS_OS_MEMZERO(sns_smgr.sensor, ARR_SIZE(sns_smgr.sensor)*sizeof(smgr_sensor_s));
  for ( i=0; i < ARR_SIZE(sns_smgr.sensor); i++ ) //for each sensor DEVICE
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
    sensor_ptr->fifo_cfg.req_rpts_cnt      = 0;
    sensor_ptr->fifo_cfg.max_req_rpt_rate  = 0;
    sensor_ptr->fifo_cfg.desired_watermark = 0;
    sensor_ptr->fifo_cfg.is_datatype_requested[SNS_SMGR_DATA_TYPE_PRIMARY_V01]   = false;
    sensor_ptr->fifo_cfg.is_datatype_requested[SNS_SMGR_DATA_TYPE_SECONDARY_V01] = false;
  }
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_calculate_max_rpt_rate
===========================================================================*/
/*!
  @brief find max report rate of fifo related sensors
  @details
  - Traverses all reports and items to determine max report rate for fifo sensors
  - Stores max rpt rate for fifo sensor in sens_ptr->fifo_cfg.max_req_rpt_rate
  - Configures sync structure for discrete fifo
  @param[in] none
  @return Global max rpt rate for all primary and secondary sensors on fifo devices
 */
/*=====================================================================*/
static q16_t sns_smgr_fifo_calculate_max_rpt_rate( void )
{
  //global max rpt rate for primary and secondary sensors on fifo devices
  q16_t    max_rpt_rate    = 0;
  smgr_rpt_spec_s* rpt_ptr = NULL;
  uint8_t  i = 0;

  sns_smgr_fifo_clean_fifo_cfg();

  // traverses through all Rpts and Items (sensors on report) to find max_rep_rate
  //--------------------------------------------------------------------------
  SMGR_FOR_EACH_Q_ITEM( &sns_smgr.report_queue, rpt_ptr, rpt_link )
  {
    if ( rpt_ptr->state == SMGR_RPT_STATE_INACTIVE )
    {
      continue;
    }
    if ( rpt_ptr->q16_rpt_rate < 0 )
    {
      SNS_SMGR_FIFO_DBG0( ERROR, "FIFO calc max_rpt_rate: q16_rpt_rate < 0" );
      continue;
    }

    for ( i=0; i<rpt_ptr->num_items; i++ )
    {
      //each item has 1 sensor in it (pointed by ddf_sensor_ptr)
      q16_t              q16_rpt_rate = rpt_ptr->q16_rpt_rate;
      smgr_rpt_item_s*   rpt_item_ptr = rpt_ptr->item_list[i];
      smgr_ddf_sensor_s* ddf_sens_ptr = rpt_item_ptr->ddf_sensor_ptr;
      smgr_sensor_s*     sens_ptr     = ddf_sens_ptr->sensor_ptr;

      if ( !SMGR_SENSOR_FIFO_IS_SUPPORTED(sens_ptr) ) //DEVICE has no FIFO support
      {
        continue; //and cannot be used for WM calc
      }

      sens_ptr->fifo_cfg.req_rpts_cnt++; //counts also queries

      if ( q16_rpt_rate == 0 ) //QUERY
      {
        q16_rpt_rate = FX_FLTTOFIX_Q16(rpt_item_ptr->sampling_rate_hz);

        SNS_SMGR_FIFO_DBG2( MED,
          "FIFO calc max_rpt_rate: QUERY - SensId=%d, rpt_rate_used=%u",
          SMGR_SENSOR_ID(sens_ptr), rpt_item_ptr->sampling_rate_hz );
      }

      //---------- max rpt rate for current sensor device -----------
      sens_ptr->fifo_cfg.max_req_rpt_rate = MAX( sens_ptr->fifo_cfg.max_req_rpt_rate,
                                                 q16_rpt_rate );
      //---------- global max rpt rate -----------
      max_rpt_rate = MAX( max_rpt_rate, q16_rpt_rate );


      if ( ddf_sens_ptr->data_type == SNS_SMGR_DATA_TYPE_SECONDARY_V01 )
      {
        sens_ptr->fifo_cfg.is_datatype_requested[SNS_SMGR_DATA_TYPE_SECONDARY_V01] = true;
      }
      else
      if ( ddf_sens_ptr->data_type == SNS_SMGR_DATA_TYPE_PRIMARY_V01 )
      {
        sens_ptr->fifo_cfg.is_datatype_requested[SNS_SMGR_DATA_TYPE_PRIMARY_V01]   = true;
        if ( !SNS_SMGR_FIFO_IS_UNIFIED(sens_ptr) )
        {
          //TODO: sync discrete and unified sensors
          sns_smgr_fifo_set_active_info( sens_ptr, ddf_sens_ptr );
        }
      }
      else
      {
        SNS_SMGR_FIFO_DBG0( ERROR, "FIFO calulate wm - unsupported data type" );
      }

    } //for ( Items )
  } //SMGR_FOR_EACH_Q_ITEM( Reports )
  SNS_SMGR_PRINTF1( MED, "FIFO calc max_rpt_rate[Hz]=%u/100", max_rpt_rate*100/65536 );

  return max_rpt_rate;
}



//============================================================================
//============================================================================
//                          W A T E R  M A R K
//============================================================================
//============================================================================

/*===========================================================================
FUNCTION:   sns_smgr_fifo_set_watermark
===========================================================================*/
/*!
  @brief Set Watermark level for the sensor
  @details
  - This Function sets the watermark for sensor.
  - Watermark Value 0 means disable fifo. Value > 0 means fifo is enabled.
  @param[in] sensor_ptr        - the sensor leader structure
  @param[in] sensor            - sensor type
  @param[in] desired_watermark - desired watermark to set.
  @return  sns_ddf_status_e
 */
/*=========================================================================*/
sns_ddf_status_e sns_smgr_fifo_set_watermark
(
  const smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     sensor,
  uint16_t             desired_watermark
)
{
  sns_ddf_fifo_set_s fifo_attr;
  sns_ddf_status_e   ddf_status        = SNS_DDF_SUCCESS;
  uint16_t           current_watermark = sensor_ptr->fifo_cfg.current_watermark;

  SNS_OS_MEMZERO(&fifo_attr,sizeof(sns_ddf_fifo_set_s));

  if( (desired_watermark != current_watermark) &&
      SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) &&
      sensor_ptr->fifo_cfg.can_fifo_be_used)
  {
    fifo_attr.watermark = desired_watermark;
    ddf_status = sns_smgr_set_attr(sensor_ptr, sensor, SNS_DDF_ATTRIB_FIFO, &fifo_attr);
    SNS_SMGR_FIFO_DBG1(MED, "FIFO set wm - desired_watermark=%d",desired_watermark);
  }
  SNS_SMGR_FIFO_DBG1(MED, "FIFO set wm - ddf_status=%d",ddf_status);
  return ddf_status;
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_calculate_suitable_max_watermark
===========================================================================*/
/*!
  @brief Calculate suitable Maximum Watermark for the sensor
  @details
  - This function calculates suitable max size of watermark as 90% of max watermark.
  - Reading sensor FIFO with big WM takes a few ms and may cause FIFO overflow!
    Suitable_max_wm is safe WM size to prevent FIFO overflow while reading from it.
  - TODO: This function should calculate suitable max size of watermark base on
    current ODR and maximum report rate
  - TODO: Should be called every time before calculating a watermark

  @param[in/out] sensor_ptr - the sensor leader structure

  @return
  uint16_t suitable maximum watermark
 */
/*=====================================================================*/
static uint16_t sns_smgr_fifo_calculate_suitable_max_watermark( smgr_sensor_s* sensor_ptr )
{
  sensor_ptr->fifo_cfg.suitable_max_watermark =
      SNS_SMGR_FIFO_CALC_SUITABLE_MAX_WM(sensor_ptr->fifo_cfg.max_watermark);

  SNS_SMGR_PRINTF2(HIGH , "FIFO Calc & SET suitable_max_wmark to %u  SensId=%d",
                   sensor_ptr->fifo_cfg.suitable_max_watermark,SMGR_SENSOR_ID(sensor_ptr));

  return sensor_ptr->fifo_cfg.suitable_max_watermark;
}
/*===========================================================================
FUNCTION:   sns_smgr_fifo_get_suitable_max_watermark
===========================================================================*/
/*!
  @brief Returns suitable Maximum Watermark for the sensor
  @details
  - This Function returns suitable max size of safe watermark
  @param[in] sensor_ptr - the sensor leader structure
  @return  uint16_t suitable maximum watermark
 */
/*=====================================================================*/
uint16_t sns_smgr_fifo_get_suitable_max_watermark( const smgr_sensor_s* sensor_ptr )
{
  uint16_t suitable_max_wm = sensor_ptr->fifo_cfg.suitable_max_watermark;
  if ( smgr_sensor_type_max_odr(sensor_ptr) > SNS_SMGR_FIFO_REDUCE_WM_HIGH_ODR )
  {
    /* to prevent fifo overflow, reduce the suitable wm by 1/2 if ODR is large */
    suitable_max_wm = suitable_max_wm >> 1;
  }
  return suitable_max_wm;
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_calculate_wm_and_div
===========================================================================*/
/*!
  @brief calculate FIFO watermarks and discrete watermark divider for fifo
  @details
  - Calculates watermark = SensorODR / max_report_rate
  - Calculates discrete watermark divider = desired_watermark / suitable_max_wm + 1
  - Stores result in ddf_sensor_ptr->sensor_ptr->fifo_cfg.desired_watermark
  - It's a common function for discrete and unified fifo.
  @param[in/out] ddf_sensor_ptr    - smgr_ddf_sensor_s
  @param[in]     max_req_rpt_rate  - global max rpt rate for all fifo sensors
  @return (used in discrete fifo)
  - Returns watermark divider for discrete fifo or 0
 */
/*=====================================================================*/
static uint16_t sns_smgr_fifo_calculate_wm_and_div
(
  smgr_ddf_sensor_s* ddf_sensor_ptr,
  q16_t              max_rpt_rate
)
{
  uint16_t discrete_wmark_div = 1;
  uint32_t sensor_odr         = 0;
  uint32_t max_sample_count   = 0;
  uint16_t suitable_max_wm    = 0;
  uint16_t desired_watermark  = 0;
  smgr_sensor_s* sn_ptr       = NULL;

  if ( ddf_sensor_ptr == NULL )
  {
    return 0;
  }
  sn_ptr = ddf_sensor_ptr->sensor_ptr;

  if ( !SMGR_SENSOR_FIFO_IS_SUPPORTED(sn_ptr) ||
       !SMGR_SENSOR_IS_SELF_SCHED(sn_ptr) )
  {
    sn_ptr->fifo_cfg.desired_watermark = 0;
    SNS_SMGR_FIFO_DBG1( MED, "FIFO calc wm&div: SensId=%d FIFO NOT supported",
                        SMGR_SENSOR_ID(sn_ptr) );
    return 0;
  }

  sensor_odr = smgr_sensor_type_max_odr(sn_ptr); //Max ODR of primary and secondary sensor
  if ( sn_ptr->fifo_cfg.req_rpts_cnt == 0 || sensor_odr == 0 )
  {
    sn_ptr->fifo_cfg.desired_watermark = 0;
    SNS_SMGR_FIFO_DBG1( MED,
                        "FIFO calc wm&div: === SensId=%d ODR=0 Rpts/Sens=0 ===",
                        SMGR_SENSOR_ID(sn_ptr) );
    return 0;
  }

  SNS_SMGR_PRINTF3( MED,
                    "FIFO calc wm&div: === SensId=%d ODR=%u Rpts/Sens=%d ===",
                    SMGR_SENSOR_ID(sn_ptr), sensor_odr, sn_ptr->fifo_cfg.req_rpts_cnt );

  max_sample_count = sns_smgr_get_max_sample_count(ddf_sensor_ptr->depot_ptr);
  suitable_max_wm  = sns_smgr_fifo_get_suitable_max_watermark(sn_ptr);

  if ( max_rpt_rate > 0 && suitable_max_wm > 0 ) //div by 0 checks!
  {
    desired_watermark   = FX_FLTTOFIX_Q16(sensor_odr)/max_rpt_rate;
    if ( desired_watermark == 0 )
    {
      desired_watermark = 1; //in case of sensor_ODR < max_report_rate !
    }
    // max number of samples that can be saved in the buffer depot.
    if ( desired_watermark > max_sample_count )
    {
      if ( max_sample_count == 0 )
      {
        desired_watermark = 0;
      }
      else
      {
        desired_watermark = max_sample_count - 1; //avoid filling buffer to full due to interpolation, etc
      }
    }

    if ( desired_watermark >= suitable_max_wm )
    {
      //div by 0 check done earlier: if ( suitable_max_wm > 0 )
      discrete_wmark_div = desired_watermark / suitable_max_wm + 1;
    }
  }
  else
  {
    SNS_SMGR_PRINTF2( ERROR,
                      "FIFO calc wm&div: SensId=%d max_req_rpt_rate=%d suitable_max_wm=%d",
                      max_rpt_rate, suitable_max_wm );
  }

  sn_ptr->fifo_cfg.desired_watermark = desired_watermark;

  SNS_SMGR_FIFO_DBG2( MED,
                      "FIFO calc wm&div: MaxRepRate[Hz]=%u/100 DiscreteDiv=%u",
                      max_rpt_rate*100/65536, discrete_wmark_div );

  SNS_SMGR_PRINTF3( MED,
                    "FIFO calc wm&div: SuitMaxWM=%u DesiredWM=%u SensRepRate[Hz]=%u/100",
                    suitable_max_wm, desired_watermark,
                    sn_ptr->fifo_cfg.max_req_rpt_rate*100/65536 ); //65536=0x10000

  return discrete_wmark_div;
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_set_wm_and_intr
===========================================================================*/
/*!
  @brief set FIFO watermark and interrupt for the sensor
  @details
  - This Function sets FIFO watermark if it has changed.
    and intr if requested by enable_intr argument
  - Takes watermark value from sensor_ptr->fifo_cfg.desired_watermark
  @param[in/out] sensor_ptr  - the sensor leader structure
  @param[in]     enable_intr - request to enable intr
  @return  sns_err_code_e
 */
/*=======================================================================*/
static sns_err_code_e sns_smgr_fifo_set_wm_and_intr
(
  smgr_sensor_s* sensor_ptr,
  bool           enable_intr
)
{
  bool     was_intr_enabled        = false;
  sns_ddf_sensor_e ddf_sensor_type = SNS_DDF_SENSOR__NONE;
  uint16_t current_watermark       = sensor_ptr->fifo_cfg.current_watermark;
  uint16_t desired_watermark       = sensor_ptr->fifo_cfg.desired_watermark;

  ddf_sensor_type = SMGR_SENSOR_TYPE(sensor_ptr,SNS_SMGR_DATA_TYPE_PRIMARY_V01);

  SNS_SMGR_PRINTF3( LOW, "FIFO set WM&intr: SensId=%d CurrentWM=%u DesiredWM=%u",
                   SMGR_SENSOR_ID(sensor_ptr), current_watermark, desired_watermark );
  SNS_SMGR_PRINTF2( LOW, "FIFO set WM&intr: intr_enabled=%u enable_intr=%u",
                    sensor_ptr->is_self_sched_intr_enabled, enable_intr );
  SNS_SMGR_FIFO_DBG2( LOW, "FIFO set WM&intr: sensor_type=%d sens_state=%d",
                      ddf_sensor_type, sensor_ptr->sensor_state );

  if ( !sensor_ptr->fifo_cfg.can_fifo_be_used ) //could be temporary unavailable
  {
    SNS_SMGR_PRINTF1( ERROR, "FIFO set WM&intr: !can_fifo_be_used, SensId=%d",
                      SMGR_SENSOR_ID(sensor_ptr) );
    return SNS_ERR_NOTALLOWED;
  }

  if ( desired_watermark >= sensor_ptr->fifo_cfg.max_watermark ) // !tested in WM calc
  {
   SNS_SMGR_PRINTF1( ERROR, "FIFO set WM&intr: desired_WM >= max_watermark, SensId=%d",
                     SMGR_SENSOR_ID(sensor_ptr) );
    return SNS_ERR_BAD_PARM;
  }

  was_intr_enabled = sensor_ptr->is_self_sched_intr_enabled;

  //------------------------ WM not changed ------------------------
  if ( desired_watermark == current_watermark ) //don't need to change WM
  {
    // Change interrupt setting if requested by enable_intr.
    if ( enable_intr != was_intr_enabled )
    {
      SNS_SMGR_PRINTF2( MED,
                        "FIFO set WM&intr: set INTR to enable_intr=%d, SensId=%d",
                        enable_intr, SMGR_SENSOR_ID(sensor_ptr));

      //NOTE: Can disable INTR in any state not only READY/CONFIGURING state
      if ( enable_intr &&
           sensor_ptr->sensor_state != SENSOR_STATE_READY &&
           sensor_ptr->sensor_state != SENSOR_STATE_CONFIGURING )
      { //cannot enable intr in other sensor states
        SNS_SMGR_PRINTF1( ERROR,
          "FIFO set WM&intr: CANNOT enable INTR. Sens not in READY/CONFIGURING state, SensId=%d",
          SMGR_SENSOR_ID(sensor_ptr) );
        return SNS_ERR_NOTALLOWED;
      }
      sns_smgr_fifo_flush(sensor_ptr);  
      sns_smgr_enable_sched_data( sensor_ptr, ddf_sensor_type, enable_intr );
      //changes sensor_ptr->is_self_sched_intr_enabled flag
    }
    else
    {
      SNS_SMGR_FIFO_DBG1( LOW,
        "FIFO set WM&intr: INTR state already as requested, SensId=%d",
        SMGR_SENSOR_ID(sensor_ptr));
    }
    return SNS_SUCCESS;
  }

  //------------------------ SET NEW WM ------------------------

  //cannot set WM (enable fifo) if Sensor not in READY/CONFIGURING state
  if ( desired_watermark > 0 &&
       sensor_ptr->sensor_state != SENSOR_STATE_READY &&
       sensor_ptr->sensor_state != SENSOR_STATE_CONFIGURING )
  {
    SNS_SMGR_PRINTF2(ERROR,
      "FIFO set WM&intr: Sensor not in READY/CONFIGURING state, SensId=%d sensor_type=%d",
      SMGR_SENSOR_ID(sensor_ptr), ddf_sensor_type);
    return SNS_ERR_NOTALLOWED;
  }

  if ( was_intr_enabled )  //If DRI mode is enabled then disable it.
  {
    SNS_SMGR_FIFO_DBG2(LOW, "FIFO set WM&intr: Disable INTR - SensId=%d sensor_type=%d",
                       SMGR_SENSOR_ID(sensor_ptr), ddf_sensor_type);
    sns_smgr_enable_sched_data( sensor_ptr, ddf_sensor_type, false ); //DISABLE to flush
  }

  SNS_SMGR_FIFO_DBG1(MED,"FIFO set WM&intr: FLUSH SensId=%d",SMGR_SENSOR_ID(sensor_ptr));
  sns_smgr_fifo_flush(sensor_ptr);  //flushing to set new WM

  SNS_SMGR_FIFO_DBG2(MED, "FIFO set WM&intr: Set WM=%u - SensId=%d",
                     desired_watermark, SMGR_SENSOR_ID(sensor_ptr) );
  sns_ddf_status_e ddf_status = sns_smgr_fifo_set_watermark(
        sensor_ptr, ddf_sensor_type, desired_watermark );
  if ( ddf_status != SNS_DDF_SUCCESS )
  {
    SNS_SMGR_PRINTF2(ERROR,"FIFO set WM&intr: Set WM FAILED, SensId=%d sensor_type=%d",
                       SMGR_SENSOR_ID(sensor_ptr), ddf_sensor_type);
    if ( was_intr_enabled )
    { //re-enable original mode.
      sns_smgr_enable_sched_data( sensor_ptr, ddf_sensor_type, true );
    }
    return SNS_ERR_FAILED;
  }

  //-------------- WM set done ----------------
  sensor_ptr->fifo_cfg.current_watermark = desired_watermark;

  //----------- SET INTR if required ----------
  if ( desired_watermark > 0 ) //WM>0 => FIFO is ENABLED !!!
  {
    sensor_ptr->fifo_cfg.is_fifo_enabled = true; //also TRUE when WM>0 && INTR is DISABLED

    SNS_SMGR_FIFO_DBG3(MED, "FIFO set WM&intr: Enable_INTR=%d, SensId=%d WM=%u",
                       enable_intr, SMGR_SENSOR_ID(sensor_ptr), desired_watermark );

    if ( enable_intr )
    {
      sns_smgr_enable_sched_data( sensor_ptr, ddf_sensor_type, true );
    }
    sns_smgr_fifo_log_event( sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01],
                             SNS_DDF_FIFO_ENABLED );
    SNS_SMGR_PRINTF3(LOW,
      "FIFO set WM&intr: FIFO ENABLED, currentMW=%u SensId=%d SensType=%d",
      sensor_ptr->fifo_cfg.current_watermark, SMGR_SENSOR_ID(sensor_ptr),ddf_sensor_type);
  }
  else
  {
    //----------- WM was set to 0 so DD disabled FIFO ----------------
    sensor_ptr->fifo_cfg.is_fifo_enabled = false;

    sns_smgr_fifo_log_event( sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01],
                             SNS_DDF_FIFO_DISABLED );
    SNS_SMGR_PRINTF2(LOW,"FIFO set WM&intr: FIFO DISABLED, currentMW=0 SensId=%d SensType=%d",
                     SMGR_SENSOR_ID(sensor_ptr), ddf_sensor_type);
  }
  return SNS_SUCCESS;
}



//============================================================================
//============================================================================
//                   D I S C R E T E    F I F O
//============================================================================
//============================================================================


/*===========================================================================
FUNCTION:   sns_smgr_fifo_discrete_clean_active_info
===========================================================================*/
/*!
  @brief Clean / initialize sync struct.
  @details
  @param[in] none
  @return NONE
 */
/*=========================================================================*/
static void sns_smgr_fifo_discrete_clean_active_info( void )
{
  SNS_OS_MEMZERO( &sns_smgr.fifo_active_info, sizeof(sns_smgr.fifo_active_info) );
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_flush_sync
===========================================================================*/
/*!
  @brief Flush FIFO for sensors.
  @details
  - This Function flushes fifo for the respective synced sensors.
  @param[in] sensor_ptr - the sensor leader structure
  @return NONE
 */
/*=========================================================================*/
static void sns_smgr_fifo_flush_sync( const smgr_sensor_s* sensor_ptr )
{
  uint8_t i=0;

  if ( SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) )
  {
    if ( sensor_ptr->fifo_cfg.current_watermark >= 1 &&
         sensor_ptr->fifo_cfg.is_fifo_enabled )
    {
      for( i=0; i<ARR_SIZE(sns_smgr.fifo_active_info.ddf_sensors_active_ptr); i++ )
      {
        sns_ddf_status_e   ddf_status = SNS_DDF_EFAIL;
        smgr_sensor_s*     sync_sens_ptr;
        smgr_ddf_sensor_s* ddf_sens_ptr_sync=sns_smgr.fifo_active_info.ddf_sensors_active_ptr[i];
        if ( ddf_sens_ptr_sync == NULL )
        {
          continue;
        }

        sync_sens_ptr = ddf_sens_ptr_sync->sensor_ptr;
        if ( sync_sens_ptr != NULL &&
             SENSOR_STATE_FAILED != sync_sens_ptr->sensor_state )
        {

          if( (NULL != SMGR_DRV_FN_PTR(sync_sens_ptr)->trigger_fifo_data) &&
              sync_sens_ptr->fifo_cfg.is_fifo_enabled &&
              (ddf_sens_ptr_sync->data_type == SNS_SMGR_DATA_TYPE_PRIMARY_V01) )
          {
            ddf_status = SMGR_DRV_FN_PTR( sync_sens_ptr )->trigger_fifo_data(
                  sync_sens_ptr->dd_handle,
                  SMGR_SENSOR_TYPE(sync_sens_ptr,SNS_SMGR_DATA_TYPE_PRIMARY_V01),
                  0, true );
            //SNS_SMGR_FIFO_DBG1( HIGH, "sns_smgr_flush_fifo_sync - Flushing FIFO SensId=%d",
            //                  SMGR_SENSOR_ID(sync_sens_ptr) );

            if ( ddf_status != SNS_DDF_SUCCESS )
            {
              SNS_SMGR_PRINTF2( ERROR,
                "sns_smgr_flush_fifo_sync - Flushing FIFO Failed ddf_status=%d SensId=%d",
                ddf_status, SMGR_SENSOR_ID(sync_sens_ptr) );
            }
            sns_smgr_fifo_log_event(
                  sync_sens_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01],
                  SNS_DDF_FIFO_READ_FLUSH );
          }
        } //TODO: else
      }
    }
  }
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_set_active_info
===========================================================================*/
/*!
  @brief sets discrete fifo active sensor info and priority if sync is enabled
  @details
  Sensor priority to trigger fifo intr: 1) GYRO, 2) ACCEL, 3) MAG
  @param[in]     sensor_ptr
  @param[in/out] ddf_sensor_ptr
  @return NONE
 */
/*=========================================================================*/
static void sns_smgr_fifo_set_active_info
(
  const smgr_sensor_s* sensor_ptr,
  smgr_ddf_sensor_s*   ddf_sensor_ptr
)
{
  if ( SMGR_SENSOR_IS_GYRO(sensor_ptr) )
  {
    sns_smgr.fifo_active_info.ddf_sensors_active_ptr[0] = ddf_sensor_ptr;
  }
  else
  if ( SMGR_SENSOR_IS_ACCEL(sensor_ptr) )
  {
    sns_smgr.fifo_active_info.ddf_sensors_active_ptr[1] = ddf_sensor_ptr;
  }
  else
  if ( SMGR_SENSOR_IS_MAG(sensor_ptr) )
  {
    sns_smgr.fifo_active_info.ddf_sensors_active_ptr[2] = ddf_sensor_ptr;
  }
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_calc_intr_rate_hz10
===========================================================================*/
/*!
  @brief Calculate intr rate for FIFO in Hz * 100
  @details
  @param[in] sensor_ptr - the sensor leader structure
  @return  int32_t - real report rate in Hz multiplied by 100
 */
/*=========================================================================*/
static int32_t sns_smgr_fifo_calc_intr_rate_hz100( const smgr_sensor_s* sensor_ptr )
{
  int32_t desired_watermark = (int32_t)sensor_ptr->fifo_cfg.desired_watermark;
  if ( desired_watermark == 0 )
  {
    return 0;
  }
  return ((int32_t)smgr_sensor_type_max_odr(sensor_ptr)*100) / desired_watermark;
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_discrete_calculate_watermarks
===========================================================================*/
/*!
  @brief calculate watermark for all discrete fifos
  @details
  - Calculates watermarks for all discrete fifos.
  - Configures fifo_sync_info struct to sync all discrete fifos on one intr
  @param[in/out] none
  @return none
 */
/*=========================================================================*/
static void sns_smgr_fifo_discrete_calculate_watermarks( void )
{
  uint8_t  i = 0;
  uint16_t wm_max_div   = 0;
  q16_t    max_rpt_rate = 0;

  //------------- Clear/Initialize the fifo_sync_info ---------------
  sns_smgr_fifo_discrete_clean_active_info();

  //TODO: max rep rate for each sns should be calc in RH thread when rept list changed
  max_rpt_rate = sns_smgr_fifo_calculate_max_rpt_rate();

  if (  max_rpt_rate <= 0 )
  {
    sns_smgr_fifo_discrete_clean_active_info();
    return;
  }

  //-----------------------------------------------------------------------------
  //--------- Calculate WM and max DIV for all FIFO sensors to be synced ---------
  for( i=0; i<ARR_SIZE(sns_smgr.fifo_active_info.ddf_sensors_active_ptr); i++ )
  {
    smgr_ddf_sensor_s* ddf_sensor_ptr = sns_smgr.fifo_active_info.ddf_sensors_active_ptr[i];
    if ( ddf_sensor_ptr == NULL )
    {
      continue;
    }
    uint16_t wm_div = sns_smgr_fifo_calculate_wm_and_div( ddf_sensor_ptr, max_rpt_rate );

    if ( wm_div > 1 )
    {
      uint16_t desired_wm = ddf_sensor_ptr->sensor_ptr->fifo_cfg.desired_watermark / wm_div;
      if ( desired_wm == 0 )
      {
        desired_wm = 1; //enable fifo on minimum size
      }
      ddf_sensor_ptr->sensor_ptr->fifo_cfg.desired_watermark = desired_wm;

      SNS_SMGR_PRINTF3( MED, "FIFO discrCalc SensId=%d FinalWM=%u Div=%u",
                        SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr),
                        ddf_sensor_ptr->sensor_ptr->fifo_cfg.desired_watermark,wm_div);

      wm_max_div = MAX( wm_div, wm_max_div );
    }
  }
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_discrete_configure
===========================================================================*/
/*!
  @brief Configures discrete FIFOs
  @details
  - Configures FIFO if necessary recalculates watermarks and sets them up.
  - Call this function for any change in report rates or the sensor ODR properties.
  @param[in/out] sensor_ptr - the sensor leader structure
  @return  void
 */
/*=========================================================================*/
static void sns_smgr_fifo_discrete_configure( smgr_sensor_s* sensor_ptr )
{
  uint8_t i = 0;

  sns_smgr_fifo_discrete_calculate_watermarks();


  for ( i=0; i < ARR_SIZE(sns_smgr.sensor); i++ )
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
    if ( !SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) ||
         !SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
    {
      continue;
    }

    if ( sensor_ptr->fifo_cfg.desired_watermark == 0 )
    {
      //NOTE: not checking SNS_SMGR_FIFO_IS_UNIFIED so INTR on unified will be disabled too
      SNS_SMGR_PRINTF2( MED, "FIFO discrConf DISABLE INTR SensId=%d IntrRate[Hz]=%u/100",
                 SMGR_SENSOR_ID(sensor_ptr),
                 sns_smgr_fifo_calc_intr_rate_hz100(sensor_ptr) );
      sns_smgr_fifo_set_wm_and_intr( sensor_ptr, false );
    }
    else
    {
      SNS_SMGR_PRINTF2( MED, "FIFO discrConf ENABLE INTR SensId=%d IntrRate[Hz]=%u/100",
                 SMGR_SENSOR_ID(sensor_ptr),
                 sns_smgr_fifo_calc_intr_rate_hz100(sensor_ptr) );
      sns_smgr_fifo_set_wm_and_intr( sensor_ptr, true ); //fifo sensor with intr enabled
    }
  }
}


//============================================================================
//============================================================================
//                   U N I F I E D    F I F O
//============================================================================
//============================================================================

/*===========================================================================
FUNCTION:   sns_smgr_fifo_unified_calculate_watermarks
===========================================================================*/
/*!
  @brief calculate all FIFO watermarks with watermark dividers for the shared sensors
  @details
  - Stores calculated water marks in:
    sensor_ptr->fifo_cfg.shared_ddf_sensor[i].sensor_ptr->fifo_cfg.desired_watermark
    end
    sensor_ptr->fifo_cfg.desired_watermark
  @param[in/out] sensor_ptr   - smgr_sensor_s
  @return sns_err_code_e
 */
/*=====================================================================*/
static sns_err_code_e sns_smgr_fifo_unified_calculate_watermarks(smgr_sensor_s* sensor_ptr)
{
  uint16_t unified_wmark_div = 1;
  uint16_t suitable_max_wm   = 0;
  uint32_t desired_watermark = 0;
  uint8_t  i                 = 0;
  uint16_t shared_cnt        = sensor_ptr->fifo_cfg.shared_sensor_cnt;
  q16_t max_rpt_rate         = sns_smgr_fifo_calculate_max_rpt_rate();

  sns_smgr_fifo_calculate_wm_and_div(
        sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01], max_rpt_rate );
  //NOTE: no fifo WM for secondary sensors

  if ( shared_cnt == 0 )
  {
    SNS_SMGR_PRINTF0(ERROR, "FIFO calc WMs: shared_cnt==0");
    return SNS_ERR_FAILED;
  }
  for ( i=0; i<shared_cnt; i++ )
  {
    sns_smgr_fifo_calculate_wm_and_div(
          sensor_ptr->fifo_cfg.shared_ddf_sensor[i].sensor_ptr->
          ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01], max_rpt_rate );
  }
  //--> water marks stored in sensor_ptr->fifo_cfg.desired_watermark

  desired_watermark = sensor_ptr->fifo_cfg.desired_watermark; //TotalEquivalentSensorSamples
  for ( i=0; i<shared_cnt; i++ )
  {
    smgr_sensor_fifo_cfg_s* fifo_cfg_pt =
        &sensor_ptr->fifo_cfg.shared_ddf_sensor[i].sensor_ptr->fifo_cfg;

    desired_watermark +=
        sensor_ptr->fifo_cfg.max_watermark * fifo_cfg_pt->desired_watermark
        / fifo_cfg_pt->max_watermark;
  }

  suitable_max_wm = sns_smgr_fifo_get_suitable_max_watermark(sensor_ptr);
  //SNS_SMGR_PRINTF1( MED, "FIFO calc WMs: suitable_max_wm=%d", suitable_max_wm );
  if ( desired_watermark < suitable_max_wm )
  {
    return SNS_SUCCESS;
  }

  if ( suitable_max_wm == 0 )
  {
    sensor_ptr->fifo_cfg.desired_watermark = 0;
    for ( i=0; i<shared_cnt; i++ )
    {
      sensor_ptr->fifo_cfg.shared_ddf_sensor[i].sensor_ptr->fifo_cfg.desired_watermark = 0;
    }
    return SNS_SUCCESS;
  }

  unified_wmark_div = (desired_watermark + suitable_max_wm - 1) / suitable_max_wm ;
  SNS_SMGR_PRINTF1( MED, "FIFO calc WMs: unified_wmark_div=%d", unified_wmark_div );
  if ( unified_wmark_div == 0 )
  {
    return SNS_SUCCESS;
  }

  for ( i=0; i<shared_cnt; i++ )
  {
    sensor_ptr->fifo_cfg.shared_ddf_sensor[i].sensor_ptr->fifo_cfg.desired_watermark
        /= unified_wmark_div;
  }

  sensor_ptr->fifo_cfg.desired_watermark /= unified_wmark_div;
  return SNS_SUCCESS;
}

/*===========================================================================
FUNCTION:   sns_smgr_fifo_unified_configure
===========================================================================*/
/*!
  @brief configure unified FIFO for the sensor
  @details
  - Configures FIFO if necessary by recalculating watermarks and setting them up.
  - Call this function for any change in report rates or the sensor ODR properties.
  @param[in/out] sensor_ptr - the sensor leader structure
  @return  sns_err_code_e
 */
/*=========================================================================*/
static sns_err_code_e sns_smgr_fifo_unified_configure( smgr_sensor_s* sensor_ptr )
{
  uint8_t  i          = 0;
  uint16_t shared_cnt = 0;


  if ( sns_smgr_fifo_unified_calculate_watermarks( sensor_ptr ) != SNS_SUCCESS )
  {
    SNS_SMGR_PRINTF0( ERROR, "FIFO unifConf shared WM calc ERROR ");
    return SNS_ERR_FAILED;
  }

  SNS_SMGR_FIFO_DBG1( HIGH, "FIFO unifConf set WM & ENABLE INTR SensId=%d",
                      SMGR_SENSOR_ID(sensor_ptr) );


  sns_smgr_fifo_set_wm_and_intr(sensor_ptr, (sensor_ptr->fifo_cfg.desired_watermark > 0));

  shared_cnt = sensor_ptr->fifo_cfg.shared_sensor_cnt;
  if ( shared_cnt == 0 )
  {
    return SNS_SUCCESS;
  }

  for ( i=0; i<shared_cnt; i++ )
  {
    smgr_sensor_s* shared_sens_ptr = sensor_ptr->fifo_cfg.shared_ddf_sensor[i].sensor_ptr;

    if ( !SMGR_SENSOR_FIFO_IS_SUPPORTED(shared_sens_ptr) ||
         !SMGR_SENSOR_IS_SELF_SCHED(shared_sens_ptr) )
    {
      continue;
    }

    SNS_SMGR_FIFO_DBG1( HIGH, "FIFO unifConf set WM & DISABLE INTR SensId=%d",
                        SMGR_SENSOR_ID(shared_sens_ptr) );

    sns_smgr_fifo_set_wm_and_intr( shared_sens_ptr,
                                  (shared_sens_ptr->fifo_cfg.desired_watermark > 0) );
  }

  return SNS_SUCCESS;
}


/*===========================================================================
FUNCTION:   sns_smgr_fifo_has_active_shared_sensor
===========================================================================*/
/*!
  @brief check if has shared fifo sensor
  @details
  - Check if a particular sensor has shared fifo sensor.
  - If it has, then also check if the shared fifo sensor's watermark and
    whether fifo is enabled.
  @param[in/out] sensor_ptr - the sensor leader structure
  @return  boolean
 */
/*=========================================================================*/
static bool sns_smgr_fifo_has_active_shared_sensor( smgr_sensor_s* sensor_ptr )
{
  uint8_t i = 0;
  uint16_t shared_cnt = sensor_ptr->fifo_cfg.shared_sensor_cnt;

  for( i=0; i<shared_cnt; i++ )
  {
    smgr_sensor_s* shared_sens_ptr = sensor_ptr->fifo_cfg.shared_ddf_sensor[i].sensor_ptr;
    if ( !SMGR_SENSOR_FIFO_IS_SUPPORTED(shared_sens_ptr) ||
         !SMGR_SENSOR_IS_SELF_SCHED(shared_sens_ptr) )
    {
      continue;
    }
    if ( shared_sens_ptr->fifo_cfg.current_watermark >= 1 &&
         shared_sens_ptr->fifo_cfg.is_fifo_enabled )
    {
      return true;
    }
  }
  return false;
}
