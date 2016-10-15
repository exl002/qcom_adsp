/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Init and get_attrib routines for ALS/PRX driver. These can be placed outside
   TCM if needed.

Copyright (c) 2011-2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/dd/qcom/src/sns_dd_alsprx_isl29044a.c#1 $


when         who     what, where, why
-----------------------------------------------------------
08/09/13     yh      Initial version of alsprx driver for isl29044a
==============================================================================*/
/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_dd.h"
#include "sns_dd_alsprx_isl29044a_priv.h"

extern volatile boolean EnableI2C;

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
static BOOLEAN  sns_dd_alsprx_isl29044a_prx_prepare_data_from_device
(
  sns_dd_alsprx_state_t* dd_ptr
);

static BOOLEAN sns_dd_alsprx_isl29044a_als_prepare_data_from_device
(
  sns_dd_alsprx_state_t* dd_ptr
);

extern sns_ddf_status_e sns_dd_alsprx_isl29044a_init
(
  sns_ddf_handle_t*        dd_handle_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
);

static sns_ddf_status_e sns_dd_alsprx_isl29044a_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_alsprx_isl29044a_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_alsprx_isl29044a_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_alsprx_isl29044a_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

sns_ddf_status_e sns_dd_alsprx_isl29044a_reset
(
  sns_ddf_handle_t dd_handle
);

static sns_ddf_status_e sns_dd_alsprx_isl29044a_run_test
(
  sns_ddf_handle_t    handle,
  sns_ddf_sensor_e    sensor_type,
  sns_ddf_test_e      test,
  uint32_t*           err
);
static sns_ddf_status_e sns_dd_alsprx_isl29044a_probe
(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors
);

sns_ddf_driver_if_s sns_alsprx_isl29044a_driver_fn_list =
{
  .init = &sns_dd_alsprx_isl29044a_init,
  .get_data = &sns_dd_alsprx_isl29044a_get_data,
  .set_attrib = &sns_dd_alsprx_isl29044a_set_attrib,
  .get_attrib = &sns_dd_alsprx_isl29044a_get_attrib,
  .handle_timer = &sns_dd_alsprx_isl29044a_handle_timer,
  .handle_irq = NULL,
  .reset = &sns_dd_alsprx_isl29044a_reset,
  .run_test = &sns_dd_alsprx_isl29044a_run_test,
  .probe = &sns_dd_alsprx_isl29044a_probe
};

/*===========================================================================
FUNCTION      sns_dd_alsprx_isl29044a_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
void sns_dd_alsprx_isl29044a_log(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t               data1,
  uint32_t               data1_q16,
  uint32_t               data2,
  uint32_t               data2_q16,
  uint32_t               raw_data )
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  /* Allocate log packet */
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = sensor_type;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_INTERSIL;

    /* Timestamp the log with sample time */
    log_struct_ptr->timestamp = sns_ddf_get_timestamp();

    /* Log the sensor data */
    if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
    {
      log_struct_ptr->num_data_types = 3;
      log_struct_ptr->data[0] = data1;
      log_struct_ptr->data[1] = data1_q16;
      log_struct_ptr->data[2] = raw_data;
    }
    else
    {
      log_struct_ptr->num_data_types = 5;
      log_struct_ptr->data[0] = data1;
      log_struct_ptr->data[1] = data1_q16;
      log_struct_ptr->data[2] = data2;
      log_struct_ptr->data[3] = data2_q16;
      log_struct_ptr->data[4] = raw_data;
    }

    /* Commit log (also frees up the log packet memory) */
    err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
  }
  else
  {
    dd_ptr->dropped_logs++;
  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_start_general_timer

DESCRIPTION   Starts a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_start_general_timer
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t duration
)
{
  sns_ddf_status_e status;
  sns_dd_timer_type* timer_ptr;

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.als_timer;
  }
  else
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.prx_timer;
  }

  if ( TRUE == timer_ptr->active )
  {
    return;
  }

#ifdef ALSPRX_DEBUG_TIMER
  SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_ALS_RES_INVALID,
                               100);
#endif

  status = sns_ddf_timer_start(timer_ptr->timer_obj, duration);
  if ( status != SNS_DDF_SUCCESS )
  {
    return;
  }
  timer_ptr->active = TRUE;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_stop_general_timer

DESCRIPTION   Stops a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_stop_general_timer
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type
)
{
  sns_dd_timer_type* timer_ptr;

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.als_timer;
  }
  else
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.prx_timer;
  }

  if ( TRUE == timer_ptr->active )
  {
#ifdef ALSPRX_DEBUG_TIMER
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_ALS_RES_INVALID,
                                 500);
#endif
    sns_ddf_timer_cancel(timer_ptr->timer_obj); /* will return error if timer already stopped */
    timer_ptr->active = FALSE;
  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_req_data

DESCRIPTION   function that handles data request from sub-drivers

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_isl29044a_req_data
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       data_type,
  uint8_t                rng_sel
)
{
  uint8_t i2c_config_reg, bytes_rw;
  uint32_t data_ready_us;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  /* Read current config before writing */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             SNS_DD_ISL_ALSPRX_CONFIG_REG,
                             (uint8_t*)&i2c_config_reg,
                             1,
                             &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                 status);
    return status;
  }

  if ( data_type == SNS_DDF_SENSOR_AMBIENT )
  {
    data_ready_us = SNS_DD_ALSPRX_ALS_DATA_READY_US;
  
    i2c_config_reg |= SNS_DD_ISL_ALSPRX_ALS_MODE_EN |
                      SNS_DD_ISL_ALSPRX_ALS_RANGE_HIGH |
                      SNS_DD_ISL_ALSPRX_ALSIR_MODE_VISIBLE;
    dd_ptr->sns_dd_als_db.als_req_cnt++;
  }
  else
  {
    data_ready_us = SNS_DD_ALSPRX_PRX_DATA_READY_US;
    i2c_config_reg |= SNS_DD_ISL_ALSPRX_PROX_MODE_EN |
                      SNS_DD_ISL_ALSPRX_PROX_DR_110MA |
                      SNS_DD_ISL_ALSPRX_PROX_SLP_MS;
    dd_ptr->sns_dd_prx_db.prx_req_cnt++;
  }

  status = sns_ddf_write_port(dd_ptr->port_handle,
                              SNS_DD_ISL_ALSPRX_CONFIG_REG,
                              &i2c_config_reg,
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_WR_ERR,
                                 SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                 status);
    return status;
  }

  sns_dd_alsprx_isl29044a_start_general_timer(dd_ptr,
                                              data_type,
                                              data_ready_us);
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_als_convert_to_mlux

DESCRIPTION   Convert a raw data to a real milli lux

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None

===========================================================================*/
uint32_t sns_dd_alsprx_isl29044a_als_convert_to_mlux
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint8_t rng,
  uint16_t raw_data
)
{
  uint64_t data_64;
  uint32_t data_32;

  data_64 = (uint64_t)raw_data * dd_ptr->sns_dd_als_db.als_conversion;
  data_32 = (uint32_t)((data_64 > 0xFFFFFFFF)? 0xFFFFFFFF : data_64);

#ifdef ALSPRX_DEBUG_REG
  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_CONVERSION1,
                             raw_data,
                             dd_ptr->sns_dd_als_db.als_conversion,
                             data_32);

  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_CONVERSION2,
                             data_64>>32,
                             data_64);
#endif /* ALSPRX_DEBUG_REG */

  return (data_32);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_async_notify

DESCRIPTION   Notify to SNSM with a sensor data.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_async_notify
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t               data_val,
  uint32_t               prx_bin,
  uint32_t               raw_data
)
{
  uint8_t num_sensors = 1, num_samples=2;
  uint16_t size;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  static sns_ddf_sensor_sample_s *buf_ptr;
  static sns_ddf_sensor_data_s *sensor_data;
  uint8_t sensor_idx=0;
  uint32_t data_lux;

  /* allocate memory for one or both sensors as needed */
  size = num_sensors*sizeof(sns_ddf_sensor_data_s);
  if ((status = sns_ddf_malloc((void **)&sensor_data, size)) != SNS_DDF_SUCCESS )
  {
    /* Failed miserably. Can't even notify SMGR */
    return;
  }

  size = num_samples*sizeof(sns_ddf_sensor_sample_s);
  if ((status = sns_ddf_malloc((void **)&buf_ptr, size)) != SNS_DDF_SUCCESS )
  {
    /* Failed miserably. Can't even notify SMGR */
    return;
  }

  if ( sensor_type == SNS_DDF_SENSOR_AMBIENT )
  {
     /* convert to lux */
    data_lux = (uint32_t) (data_val / 1000);

    /* saturate */
    data_lux = (data_lux > 0x7FFF) ? 0x7FFF : data_lux;

    /* related to floating point library removal; change to FX_CONV_Q16(val,0) */
    buf_ptr[0].sample = FX_FLTTOFIX_Q16(data_lux);
    buf_ptr[0].status = SNS_DDF_SUCCESS;
    buf_ptr[1].sample = raw_data;                 /* ADC output */
    buf_ptr[1].status = SNS_DDF_SUCCESS;

    sns_dd_alsprx_isl29044a_log(dd_ptr, sensor_type, data_val, buf_ptr[0].sample, 0, 0, raw_data);
  }
  else
  {
    buf_ptr[0].sample = FX_CONV_Q16(prx_bin, 0);  /* proximity binary */
    buf_ptr[0].status = SNS_DDF_SUCCESS;
    buf_ptr[1].sample = raw_data;                 /* ADC output */
    buf_ptr[1].status = SNS_DDF_SUCCESS;
    sns_dd_alsprx_isl29044a_log(dd_ptr, sensor_type, prx_bin, buf_ptr[0].sample, 0, 0, raw_data);
  }

  sensor_data[sensor_idx].status = status;
  sensor_data[sensor_idx].sensor = sensor_type;
  sensor_data[sensor_idx].num_samples = num_samples;
  sensor_data[sensor_idx].timestamp = sns_ddf_get_timestamp();
  sensor_data[sensor_idx].samples = buf_ptr;

  sns_ddf_smgr_notify_data(dd_ptr->smgr_handle, sensor_data, num_sensors);

  sns_ddf_mfree(buf_ptr);
  sns_ddf_mfree(sensor_data);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_als_calc_new_rng

DESCRIPTION   This function is called to calculate the range to use for the
              passed in milli-lux

DEPENDENCIES  None

RETURN VALUE  The range corresponding to the milli-lux

SIDE EFFECT   None

===========================================================================*/
uint8_t sns_dd_alsprx_isl29044a_als_calc_new_rng (uint32_t data_mlux)
{
  uint8_t ret=SNS_DD_ISL_ALSPRX_ALS_RANGE_HIGH;

  return ret;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_als_data_ready

DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_als_data_ready
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  uint32_t data_mlux;
  uint8_t  bytes_rw;
  uint8_t  i2c_data[2];
  uint16_t raw_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if ( dd_ptr->sns_dd_als_db.pend_state != SNS_DD_PEND_STATE_PEND )
  {
    /* Ignore the response */
    SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_ALS_DATA_READY_ERR);
    return;
  }

  dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_IDLE;

  /* read data */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             SNS_DD_ISL_ALSPRX_ALSIR_DT1_REG,
                             i2c_data,
                             2,
                             &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 SNS_DD_ISL_ALSPRX_ALSIR_DT1_REG,
                                 status);
    return;
  }
  raw_data = (i2c_data[1]<<8 | i2c_data[0]) & SNS_DD_ISL_ALSPRX_ALSIR_DT_MASK;

  /* convert reading to mlux */
  data_mlux = sns_dd_alsprx_isl29044a_als_convert_to_mlux(dd_ptr,
                                                dd_ptr->sns_dd_als_db.req_rng_sel,
                                                raw_data);

  *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache = data_mlux;

  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_RSP_READY,
                             raw_data,
                             *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache,
                             dd_ptr->sns_dd_als_db.req_rng_sel);

  sns_dd_alsprx_isl29044a_async_notify(dd_ptr, SNS_DDF_SENSOR_AMBIENT,
                             *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache, 0,
                             (uint32_t) raw_data);

  /* Range adaption */
  dd_ptr->sns_dd_als_db.req_rng_sel = dd_ptr->sns_dd_als_db.next_rng_sel;
  dd_ptr->sns_dd_als_db.last_rng_sel = dd_ptr->sns_dd_als_db.req_rng_sel;
  dd_ptr->sns_dd_als_db.next_rng_sel = sns_dd_alsprx_isl29044a_als_calc_new_rng(data_mlux);
  dd_ptr->sns_dd_als_db.last_get_time = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp());
  dd_ptr->sns_dd_als_db.last_mlux  = data_mlux;

}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_prx_binary

DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_prx_binary
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint16_t               raw_data_unsigned
)
{
  uint16_t adjusted_adc;

  adjusted_adc = ALSPRX_CONV_TO_UNSIGNED((raw_data_unsigned -
                     dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset), 16);

  if ( adjusted_adc >= dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near )
  {
    if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_NEAR_BY )
    {
      *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache = (uint32_t)SNS_PRX_NEAR_BY;
    }
    dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_NEAR_BY;
  }
  else if ( adjusted_adc < dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far )
  {
    if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_FAR_AWAY )
    {
      *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache = SNS_PRX_FAR_AWAY;
    }
    dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_FAR_AWAY;
  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_prx_data_ready

DESCRIPTION   This function is called when proximity data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_prx_data_ready
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  uint16_t raw_data_unsigned;
  uint8_t  raw_data, bytes_rw;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  /* read data */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                              SNS_DD_ISL_ALSPRX_PROX_DATA_REG,
                              (uint8_t*)&raw_data,
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 SNS_DD_ISL_ALSPRX_PROX_DATA_REG,
                                 status);
    return;
  }

  raw_data_unsigned = (uint16_t)raw_data;

  /* TODO - Add proximity distance processing here */
  *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache = 0;

  if ( dd_ptr->sns_dd_prx_db.pend_state != SNS_DD_PEND_STATE_PEND )
  {
    /* Ignore the response */
    SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_PRX_NOT_PENDING);
  }
  else
  {
    dd_ptr->sns_dd_prx_db.pend_state = SNS_DD_PEND_STATE_IDLE;

    sns_dd_alsprx_isl29044a_prx_binary(dd_ptr, raw_data_unsigned); /* binary decision */

    sns_dd_alsprx_isl29044a_async_notify(dd_ptr, SNS_DDF_SENSOR_PROXIMITY,
                               *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache,
                               dd_ptr->sns_dd_prx_db.prx_data_cache,
                               (uint32_t)raw_data_unsigned);

    SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_PRX_RSP_READY,
                               raw_data,
                               *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache);
  }

}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_data_ready

DESCRIPTION   This function is called when data ready indication(timer/irq)
              is received from SMGR

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_data_ready
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type
)
{
  if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
  {
    /* indicate the data to the light(ALS) sub-driver */
    sns_dd_alsprx_isl29044a_als_data_ready(dd_ptr);
  }
  else if (sensor_type == SNS_DDF_SENSOR_PROXIMITY)
  {
    /* indicate the data to the proximity common handler */
    sns_dd_alsprx_isl29044a_prx_data_ready(dd_ptr);
  }
  else
  {
    SNS_PRINTF_STRING_ID_HIGH_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_STRING2,
                                500, sensor_type);
  }

}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29044a_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This is
         the callback function submitted when initializing a timer.

  @detail
  This will be called within the context of the Sensors Manager task.
  It indicates that sensor data is ready

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] arg        The argument submitted when the timer was set.

  @return None
*/
/*=========================================================================*/
static void sns_dd_alsprx_isl29044a_handle_timer
(
  sns_ddf_handle_t handle,
  void* arg
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t*)handle;
  sns_ddf_sensor_e       sensor_type=*(sns_ddf_sensor_e*)arg;

#ifdef ALSPRX_DEBUG_TIMER
  SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_HANDLE_TIMER);
#endif

  if ((sensor_type != SNS_DDF_SENSOR_AMBIENT) &&
      (sensor_type != SNS_DDF_SENSOR_PROXIMITY))
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_STRING2,
                                 800,
                                 sensor_type);
    return;
  }

  sns_dd_alsprx_isl29044a_stop_general_timer(dd_ptr, sensor_type);

  sns_dd_alsprx_isl29044a_data_ready(dd_ptr, sensor_type);

}

/*==============================================================================

FUNCTION      sns_dd_alsprx_isl29044a_als_init

DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_alsprx_isl29044a_als_init (sns_dd_alsprx_state_t* dd_ptr)
{
  sns_dd_als_db_type *als_db_ptr = &dd_ptr->sns_dd_als_db;
  sns_dd_alsprx_common_db_type *common_db_ptr = &dd_ptr->sns_dd_alsprx_common_db;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_INIT);
  als_db_ptr->pend_state        = SNS_DD_PEND_STATE_IDLE;
  als_db_ptr->last_rng_sel      = SNS_DD_ISL_ALS_DEF_RNG;
  als_db_ptr->next_rng_sel      = SNS_DD_ISL_ALS_DEF_RNG;
  als_db_ptr->req_rng_sel       = SNS_DD_ISL_ALS_DEF_RNG;
  als_db_ptr->last_get_time     = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp());
  als_db_ptr->last_mlux         = SNS_DD_ALS_DFLT_MILLI_LUX;

#define ALS_CONVERSION  (common_db_ptr->nv_db.als_factor / common_db_ptr->nv_db.visible_ratio)
  als_db_ptr->als_conversion = (uint32_t)SNS_DD_ALSPRX_ALPHA_HI * ALS_CONVERSION / 10;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_isl29044a_prx_init

DESCRIPTION   initializes the Intersil ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_alsprx_isl29044a_prx_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  sns_dd_prx_db_type     *prx_db_ptr     = &dd_ptr->sns_dd_prx_db;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_PRX_INIT);
  prx_db_ptr->pend_state = SNS_DD_PEND_STATE_IDLE;
  prx_db_ptr->last_nearby = SNS_PRX_NEAR_BY_UNKNOWN;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_isl29044a_common_init

DESCRIPTION   initializes the Intersil ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_alsprx_isl29044a_common_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  dd_ptr->dropped_logs = 0;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_STOPPED;

  dd_ptr->sns_dd_alsprx_common_db.als_timer.active = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.als_timer.defined = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.prx_timer.active = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.prx_timer.defined = FALSE;

  /* initialize NV item db to default values if needed */
  if (dd_ptr->sns_dd_alsprx_common_db.nv_source == SNS_DD_ALSPRX_NV_SOURCE_DEFAULT)
  {
    dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = SNS_DD_VISIBLE_TRANS_RATIO;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio = SNS_DD_IR_TRANS_RATIO;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset = SNS_DD_DC_OFFSET;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = SNS_DD_PRX_THRESH_NEAR;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = SNS_DD_PRX_THRESH_FAR;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor  = SNS_DD_PRX_FACTOR;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor  = SNS_DD_ALS_FACTOR;

#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_USE_DRIVER_DEFAULTS);
#endif
  }

#ifdef ALSPRX_DEBUG_REG
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_source);
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio);
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio);
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset);
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near);
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far);
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor);
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_NV_PARAMS,
                             dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor);
#endif /* ALSPRX_DEBUG_REG */

}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_init_data_struct

DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_alsprx_isl29044a_init_data_struct
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  /* Initialize the common database */
  sns_dd_alsprx_isl29044a_common_init(dd_ptr);

  /* Initialize ALS database */
  sns_dd_alsprx_isl29044a_als_init(dd_ptr);

  /* Initialize PRX database */
  sns_dd_alsprx_isl29044a_prx_init(dd_ptr);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_set_powerstate

DESCRIPTION   Sets the power state of the ISL ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_isl29044a_set_powerstate
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_powerstate_e   op_mode,
  boolean                init_data
)
{
  uint16_t  i2c_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t bytes_w;

  if (EnableI2C != TRUE)
    return SNS_DDF_SUCCESS;

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
    /* Set thresholds such that interrupt is generated after every ADC conversion.
      High threshold < Low threshold will do this. */
    i2c_data = SNS_DD_ISL_ALSPRX_PROX_LT;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_PROX_LT_REG,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_PROX_LT_REG,
                                   status);
      return status;
    }

    i2c_data = SNS_DD_ISL_ALSPRX_PROX_HT;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_PROX_HT_REG,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_PROX_HT_REG,
                                   status);
      return status;
    }

    i2c_data = SNS_DD_ISL_ALSPRX_ALSIR_TH1;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_ALSIR_TH1_REG,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_ALSIR_TH1_REG,
                                   status);
      return status;
    }

    i2c_data = SNS_DD_ISL_ALSPRX_ALSIR_TH2;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_ALSIR_TH2_REG,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_ALSIR_TH2_REG,
                                   status);
      return status;
    }

    i2c_data = SNS_DD_ISL_ALSPRX_ALSIR_TH3;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_ALSIR_TH3_REG,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_ALSIR_TH3_REG,
                                   status);
      return status;
    }

    i2c_data = SNS_DD_ISL_ALSPRX_PWR_DOWN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                   status);
      return status;
    }
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    /* stop any pending timers */
    sns_dd_alsprx_isl29044a_stop_general_timer( dd_ptr, SNS_DDF_SENSOR_AMBIENT );
    sns_dd_alsprx_isl29044a_stop_general_timer( dd_ptr, SNS_DDF_SENSOR_PROXIMITY );

    if (init_data == TRUE)
    {
      /* cleanup */
      sns_dd_alsprx_isl29044a_init_data_struct(dd_ptr);
    }
#ifdef ALSPRX_DEBUG_BASIC
  /* Read current config before writing */
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               SNS_DD_ISL_ALSPRX_CONFIG_REG,
                               (uint8_t*)&i2c_data,
                               1,
                               &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_RD_ERR,
                                   SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                   status);
      return status;
    }
#endif

    i2c_data  = SNS_DD_ISL_ALSPRX_PWR_DOWN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                   status);
      return status;
    }
  }

  dd_ptr->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_CONFIGURED;

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29044a_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  Resets  the driver state structure

  @param[in] handle  Handle to a driver instance.

  @return Success if the driver was able to reset its state and the device.
          Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_alsprx_isl29044a_reset
(
  sns_ddf_handle_t dd_handle
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)dd_handle;
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING1,
                             200);
#endif /* ALSPRX_DEBUG_BASIC */

  if ( dd_handle == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  sns_dd_alsprx_isl29044a_init_data_struct(dd_ptr);

  if ( (init_status = sns_dd_alsprx_isl29044a_set_powerstate(dd_ptr,
                          SNS_DDF_POWERSTATE_LOWPOWER, FALSE)) != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_SET_PWR_ST_ERR);
    return init_status;
  }

  return init_status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29044a_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the ISL29044A ALS/Prx sensor

  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode and range.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_isl29044a_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_dd_alsprx_state_t* state = dd_handle;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING1,
                             300);
#endif /* ALSPRX_DEBUG_BASIC */

  if ( (dd_handle == NULL) ||
       (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch ( attrib ) {
  case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      status = sns_dd_alsprx_isl29044a_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            TRUE);
      break;

  case SNS_DDF_ATTRIB_ODR:
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
        state->chip.prx_odr = *(uint32_t*)value;
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
        state->chip.als_odr = *(uint32_t*)value;
      }
      else
      {
        return SNS_DDF_EINVALID_ATTR;
      }
      break;

  default:
      return SNS_DDF_EINVALID_PARAM;
  }

  return(status);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_prx_prepare_data_from_device

DESCRIPTION   For preparing a sensor data, this function issues a request to
              the proximity common handler

DEPENDENCIES  None

RETURN VALUE  if error, return TRUE, else FALSE

SIDE EFFECT   None

===========================================================================*/
static BOOLEAN sns_dd_alsprx_isl29044a_prx_prepare_data_from_device(sns_dd_alsprx_state_t* dd_ptr)
{
  BOOLEAN err=TRUE;

  if ( dd_ptr->sns_dd_prx_db.pend_state == SNS_DD_PEND_STATE_IDLE )
  {
    if(sns_dd_alsprx_isl29044a_req_data (dd_ptr,SNS_DDF_SENSOR_PROXIMITY,0) == SNS_DDF_SUCCESS)
    {
      dd_ptr->sns_dd_prx_db.pend_state = SNS_DD_PEND_STATE_PEND;
      err = FALSE;
    }
  }
  else
  {
    /* Already PEND state, so nothing is to be done in here */
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_PRX_STATE_PEND);
  }
  return err;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_isl29044a_als_prepare_data_from_device

DESCRIPTION   For preparing a sensor data, this function issues a request to
              the arbitration manager

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static BOOLEAN  sns_dd_alsprx_isl29044a_als_prepare_data_from_device (sns_dd_alsprx_state_t* dd_ptr)
{
  BOOLEAN err=TRUE;
  uint8_t rng_sel;
  if ( dd_ptr->sns_dd_als_db.pend_state == SNS_DD_PEND_STATE_IDLE )
  {
    rng_sel = SNS_DD_ISL_ALS_DEF_RNG;

    if (sns_dd_alsprx_isl29044a_req_data(dd_ptr, SNS_DDF_SENSOR_AMBIENT, rng_sel) == SNS_DDF_SUCCESS)
    {
      dd_ptr->sns_dd_als_db.req_rng_sel = rng_sel;
      dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_PEND;
      err = FALSE;
    }
  }
  else
  {
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_DEV_STATE_PEND);
  }
  return err;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29044a_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  Requests a single sample of sensor data from each of the specified
  sensors. Data is returned immediately after being read from the
  sensor, in which case data[] is populated in the same order it was
  requested

  This driver is a pure asynchronous one, so no data will be written to buffer.
  As a result, the return value will be always PENDING if the process does
  not fail.  This driver will notify the Sensors Manager via asynchronous
  notification when data is available.

  @param[in]  dd_handle    Handle to a driver instance.
  @param[in]  sensors      List of sensors for which data is requested.
  @param[in]  num_sensors  Length of @a sensors.
  @param[in]  memhandler   Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements must
                           match @a num_sensors.

  @return SNS_DDF_SUCCESS if data was populated successfully. If any of the
          sensors queried are to be read asynchronously SNS_DDF_PENDING is
          returned and data is via @a sns_ddf_smgr_data_notify() when
          available. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_isl29044a_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
  uint8_t i;
  BOOLEAN err=FALSE;
  sns_ddf_status_e status = SNS_DDF_PENDING;
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)dd_handle;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_GET_DATA_REQ,
                                num_sensors);
#endif

  if ( (dd_handle == NULL) || (sensors == NULL) ||
       (num_sensors < 1) || (num_sensors > 2) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if (EnableI2C == FALSE)
  {
    static uint8_t toggle[2] = {0};
    sns_ddf_sensor_data_s *data_ptr;
    sns_ddf_sensor_sample_s *sample_ptr;
    int8 i;

    *data = sns_ddf_memhandler_malloc(memhandler,
                   num_sensors * sizeof(sns_ddf_sensor_data_s));

    if(*data == NULL)
        return SNS_DDF_ENOMEM;

    for (i = 0; i < num_sensors; i++)
    {
      data_ptr = &((*data)[i]);
      if( (sample_ptr = sns_ddf_memhandler_malloc(memhandler,
                          (2*sizeof(sns_ddf_sensor_sample_s)))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      sns_ddf_delay(50);

      if (sensors[i] == SNS_DDF_SENSOR_PROXIMITY)
      {
        data_ptr->sensor = SNS_DDF_SENSOR_PROXIMITY;
        if (toggle[i])
        {
          sample_ptr[0].sample = FX_CONV_Q16(0, 0); /* far = 0; near = 1 */
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 100; /* ADC */
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
        else
        {
          sample_ptr[0].sample = FX_CONV_Q16(1, 0); /* far = 0; near = 1 */
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 5; /* ADC */
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
      }
      else
      {
        data_ptr->sensor = SNS_DDF_SENSOR_AMBIENT;
        if (toggle[i])
        {
          sample_ptr[0].sample = (int32_t)(20<<16); /* 20 lux */
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 60; /* ADC */
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
        else
        {
          sample_ptr[0].sample = (int32_t)(50<<16); /* 50 lux */
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 150; /* ADC */
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
      }
      data_ptr->status = SNS_DDF_SUCCESS;
      data_ptr->num_samples = 2;
      data_ptr->timestamp = sns_ddf_get_timestamp();
      data_ptr->samples = sample_ptr;
      toggle[i] = ~toggle[i];
    }

    return SNS_DDF_SUCCESS;
  }

  /* make sure device is in the right state */
  if ( dd_ptr->sns_dd_alsprx_common_db.state != SNS_DD_DEV_CONFIGURED )
  {
    /* device state is stopped OR invalid */
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                                 dd_ptr->sns_dd_alsprx_common_db.state);
    return SNS_DDF_EFAIL;
  }

  /* Look at all the requests */
  for ( i = 0; i < num_sensors; i++ )
  {
#ifdef ALSPRX_DEBUG_BASIC
    SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_GET_DATA_REQ,
                                  sensors[i]);
#endif
    switch ( sensors[i] ) {
    case SNS_DDF_SENSOR_AMBIENT:
        err = sns_dd_alsprx_isl29044a_als_prepare_data_from_device(dd_ptr);
        break;

    case SNS_DDF_SENSOR_PROXIMITY:
        err = sns_dd_alsprx_isl29044a_prx_prepare_data_from_device(dd_ptr);
        break;

    default:
        SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                     DBG_DD_ALSPRX_GET_DATA_INVALID_SENSOR_ERR,
                                     sensors[i]);
        err = SNS_DDF_EINVALID_PARAM;
    }

    if ( err == TRUE )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_GET_DATA_REQUEST_ERR);
      status = SNS_DDF_EFAIL;
      break;
    }
  }

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29044a_run_test

===========================================================================*/
/*!
  @ Called by the SMGR to run calibration test for ALS or PRX.

  @detail
  Returns the cal_factor in error, to be stored in NV memory

  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor_type Sensor whose attribute is to be retrieved.
  @param[in] test         Test ID used to decide what to test.
  @param[out] err         prx_factor or als_factor.

  @return
    Success if no error. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_isl29044a_run_test
(
  sns_ddf_handle_t    handle,
  sns_ddf_sensor_e    sensor_type,
  sns_ddf_test_e      test,
  uint32_t*           err
)
{
  uint8_t bytes_rw;
  uint8_t i2c_data;
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)handle;
  sns_ddf_status_e status, status2;
  *err = 0;
  status = SNS_DDF_EFAIL;
  status2  = SNS_DDF_EFAIL;

  switch (test) {
  case SNS_DDF_TEST_SELF:         /**< Hardware self test. */
    /* read ALS/PRX control register  */
      status = sns_ddf_read_port(dd_ptr->port_handle,
                                 SNS_DD_ISL_ALSPRX_CONFIG_REG,
                                 (uint8_t*)&i2c_data,
                                 1,
                                 &bytes_rw);
      if (status != SNS_DDF_SUCCESS) break;
      sns_ddf_delay(1000);
      status2 = SNS_DDF_SUCCESS;
      break;

  case SNS_DDF_TEST_IRQ:          /**< Interrupt test. */
      status2 = SNS_DDF_EINVALID_TEST;
      break;

  case SNS_DDF_TEST_CONNECTIVITY:  /**< Basic connectivity test. */
      status = sns_ddf_read_port(dd_ptr->port_handle,
                                 SNS_DD_ISL_ALSPRX_RESERVED_REG,
                                 (uint8_t*)&i2c_data,
                                 1,
                                 &bytes_rw);
      if (status != SNS_DDF_SUCCESS) break;
      if ((i2c_data>>2) != SNS_DD_ALSPRX_ISL29044A_CHIPID) break;
      status2 = SNS_DDF_SUCCESS;
      break;

  default:
      status = SNS_DDF_EINVALID_ATTR;
      break;
  }
  
  if (status == SNS_DDF_SUCCESS)
    return status2;
  else
    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_isl29044a_probe

===========================================================================*/
/**
 * @brief Probes for the device with a given configuration.
 *
 * This commands the driver to look for the device with the specified
 * configuration (ie, I2C address/bus defined in the sns_ddf_device_access_s
 * struct.
 *
 * @param[in]  dev_info    Access info for physicol devices controlled by 
 *                         this driver. Used to determine if the device is
 *                         physically present.
 * @param[in]  memhandler  Memory handler used to dynamically allocate 
 *                         output parameters, if applicable.
 * @param[out] num_sensors Number of sensors supported. 0 if none.
 * @param[out] sensor_type Array of sensor types supported, with num_sensor
 *                         elements. Allocated by this function.
 *
 * @return SNS_DDF_SUCCESS if the part was probed function completed, even
 *         if no device was found (in which case num_sensors will be set to
 *         0).
 */
static sns_ddf_status_e sns_dd_alsprx_isl29044a_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
)
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;

  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }
  
 /* Read & Verify manufacture ID */
  status = sns_ddf_read_port(port_handle,
                             SNS_DD_ISL_ALSPRX_RESERVED_REG,
                             &i2c_buff,
                             1,
                             &bytes_read);
                             
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != SNS_DD_ALSPRX_ISL29028_CHIPID /* ISL29028 */ &&
      i2c_buff != SNS_DD_ALSPRX_ISL29044_CHIPID /* ISL29044 */ &&
      i2c_buff>>2 != SNS_DD_ALSPRX_ISL29044A_CHIPID /* ISL29044A */ )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* Registers registers are correct. This is probably an isl29028 or isl29044 or isl29044a */
  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc(memhandler,
                                       sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
    (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }

  sns_ddf_close_port( port_handle );
  return status;
}
