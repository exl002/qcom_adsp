/*==============================================================================

    S E N S O R S   AKM 8975 M A G N E T O M E T E R    D R I V E R

DESCRIPTION

   Impelements the driver for the magnetometer driver

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/dd/qcom/src/sns_dd_mag_akm8975.c#1 $


when         who    what, where, why
----------   ---    ----------------------------------------------------------- 
09/13/12     vh     Eliminated compiler warnings
06/20/12     ag     Removing feature FEATURE_AKM8975_SELF_TEST_IMPL. Assume 
                    it is always defined.
06/19/12     vh     Added run time switch for I2C code
02/02/12     ad     Send error report to sensor manager with valid sensor id 
01/06/12     SD     Changed max frequence from 60hz to 50hz 
11/09/11     YK     Added support for axis mapping. 
10/12/11     AG     Don't release timer when init fails
10/11/11     br     deleted line which releases resources when init fail(we need the resource for get_attr()..)
09/27/11     AG     Moved definition of FEATURE_AKM8975_SELF_TEST_IMPL to arm7.scons
09/21/11     AG     Featurize self test; implement AKM review feedback
09/01/11     AG     Implement changes from code review
08/28/11     AG     Added self test functionality 
06/23/11     dc     removed debug flag for external release
06/04/11     br     Changed to honor SENSOR__ALL too in addition to SENSOR_MAG. Changed log msg level
03/24/11     OF     Adjust sensitivity values to the measurement data read out
02/27/11     OF     Add Device ID and Device Info, read data in one 8 byte read
01/18/11     OF     Split mag driver to AK8975 & AK8973 separte files, splited from sns_dd_mag.c version#8
04/08/10     RY     Added axis conversion for FLUID and FFA/SURF.
==============================================================================*/

#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_log_api.h"
#include "sns_log_types.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_dd_mag_akm8975_priv.h"


/*=======================================================================

                  Preprocessor Definitions and Constants

========================================================================*/

/* Temporary, until we will have release will all DDF changes available*/
//#define USE_CC41

/*
 * From the table 8.3 of the magnetometer datasheet we see
 * that the sensitivity of the magnetometer is 0.3 microTesla/LSB.
 * Expressed in Gauss units (1 Tesla == 10000 Gauss, 1 microTesla == 0.01Gauss)
 * this means that the sensitivity is 0.003 Gauss/LSB.
 */
#define TESLA_TO_GAUSS 10000 // Gauss

/* The FX_FLTTOFIX_Q16 macro provides data in Q16 format as output */
static q16_t mag_akm8975_scale = FX_FLTTOFIX_Q16(0.3 * TESLA_TO_GAUSS / 1000000);

#define AKM8975_RAW_TO_GAUSS(x) (x * mag_akm8975_scale)

// 9 msec maximum time per section 5.3.3 in AKM8975 data sheet version MS1187-E-02
#define USEC_TIME_FOR_MEASUREMENT_8975 9000

#define NUM_SENSORS_SUPPORTED_BY_AK8975 1

// Should be commented when checked-in
//#define MAG8975_DEBUG

#ifdef MAG8975_DEBUG
#define MAG8975_DBG_MSG_0( msg )          \
  SNS_PRINTF_STRING_ID_MEDIUM_0( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg) )
#define MAG8975_DBG_MSG_1( msg, p1 )            \
  SNS_PRINTF_STRING_ID_MEDIUM_1( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg), p1 )
#define MAG8975_DBG_MSG_2( msg, p1, p2 )        \
  SNS_PRINTF_STRING_ID_MEDIUM_2( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg), p1, p2 )
#define MAG8975_DBG_MSG_3( msg, p1, p2, p3 )          \
  SNS_PRINTF_STRING_ID_MEDIUM_3( SNS_DBG_MOD_DSPS_DD_MAG8975, (msg), p1, p2, p3 )
#else
#define MAG8975_DBG_MSG_0( msg )
#define MAG8975_DBG_MSG_1( msg, p1 )
#define MAG8975_DBG_MSG_2( msg, p1, p2 )
#define MAG8975_DBG_MSG_3( msg, p1, p2, p3 )
#endif

#define MAG8975_SELF_TEST_DEBUG

#ifdef MAG8975_SELF_TEST_DEBUG
#define MAG8975_SMGR_DBG_MSG_0( msg )          \
  SNS_PRINTF_STRING_ID_MEDIUM_0( SNS_DBG_MOD_DSPS_SMGR, (msg) )
#define MAG8975_SMGR_DBG_MSG_1( msg, p1 )            \
  SNS_PRINTF_STRING_ID_MEDIUM_1( SNS_DBG_MOD_DSPS_SMGR, (msg), p1 )
#define MAG8975_SMGR_DBG_MSG_2( msg, p1, p2 )        \
  SNS_PRINTF_STRING_ID_MEDIUM_2( SNS_DBG_MOD_DSPS_SMGR, (msg), p1, p2 )
#define MAG8975_SMGR_DBG_MSG_3( msg, p1, p2, p3 )          \
  SNS_PRINTF_STRING_ID_MEDIUM_3( SNS_DBG_MOD_DSPS_SMGR, (msg), p1, p2, p3 )
#else 
#define MAG8975_SMGR_DBG_MSG_0( msg )
#define MAG8975_SMGR_DBG_MSG_1( msg, p1 )
#define MAG8975_SMGR_DBG_MSG_2( msg, p1, p2 )
#define MAG8975_SMGR_DBG_MSG_3( msg, p1, p2, p3 )
#endif 

// Use define to avoid numbers in code
#define DDF_WRITE_ONE_BYTE    1
#define DDF_READ_ONE_BYTE     1
#define SENSITIVITY_NUM_BYTES 3
#define MAG_DATA_NUM_BYTES    8

/*=======================================================================

                  Extern Variables

========================================================================*/
extern volatile boolean EnableI2C;

/*=======================================================================

  Global Data Definitions

 ========================================================================*/

  /*static*/ sns_dd_mag_akm8975_state_s *drv_akm8975_dbg = NULL;

/*=======================================================================

                   INTERNAL FUNCTION PROTOTYPES

========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);


static sns_ddf_status_e sns_dd_akm8975_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_akm8975_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

static sns_ddf_status_e sns_dd_akm8975_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

static void sns_dd_akm8975_handle_timer(
        sns_ddf_handle_t dd_handle,
        void* arg);

static void sns_dd_akm8975_hdle_irq(
  sns_ddf_handle_t dd_handle,
  uint32_t         gpio_num,
  sns_ddf_time_t   timestamp);


static sns_ddf_status_e sns_dd_akm8975_reset(
  sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_akm8975_self_test(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_test_e       test,
  uint32_t             *err);

/*=======================================================================

                   STRUCTURE DEFINITIONS

========================================================================*/
sns_ddf_driver_if_s sns_mag_akm8975_driver_fn_list =
{
  &sns_dd_akm8975_init,
  &sns_dd_akm8975_get_data,
  &sns_dd_akm8975_set_attr,
  &sns_dd_akm8975_get_attr,
  &sns_dd_akm8975_handle_timer,
  &sns_dd_akm8975_hdle_irq,
  &sns_dd_akm8975_reset,
  &sns_dd_akm8975_self_test
};

/*###########################################################################*/
/*                    Internal Utilities APIs                                */
/*###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_config_pwr_mode

===========================================================================*/
/*!
  @brief Routine used to set the power state when set as an attrib by
  SMGR that calls the appropriate pwr setting routines based on the
  device type requested

  @detail
  When power is turned ON, AK8975 is in power-down mode. When any other mode
  is selected the AK8975 transits to the specified mode and starts operation.
  When operation finishes the AK8975 turns back to power-down mode.
  Therefore there isn't much difference between SNS_DDF_POWERSTATE_LOWPOWER and
  SNS_DDF_POWERSTATE_ACTIVE modes. When the former is set, we need to cancel
  any measurement in progress.

  @param[in/out] state: Ptr to the driver structure
  @param[in] *mode: Power state change requested with an pwr attrib set
  routine

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success;

  $TODO:
*/

/*=========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_config_pwr_mode
(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_powerstate_e        mode
)
{
  uint8_t akm_write_buffer, out;

  if(mode == state->curr_pwr_state)
  {
    return SNS_DDF_SUCCESS;
  }

  if(mode == SNS_DDF_POWERSTATE_LOWPOWER)
  {
    if(state->curr_state != SNSD_MGT8975_ST_IDLE)
    {
      MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8888);

      akm_write_buffer = AKM_MAGNETOMETER_PWR_DOWN_MODE_8975; 
      if (sns_ddf_write_port(state->port_handle,
                             AKM_MAG_REG_ADDR_CNTL_8975,
                             &akm_write_buffer,
                             DDF_WRITE_ONE_BYTE,
                             &out) != SNS_DDF_SUCCESS)
      {
        SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_FAILED_TO_PWR_DOWN);
        return (SNS_DDF_EDEVICE);
      }
      // If current state is not IDLE, it means that we start measurement mode and also started the timer
      sns_ddf_timer_cancel(state->sns_dd_mag_tmr_obj);
      state->curr_state = SNSD_MGT8975_ST_IDLE;
    }
    state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  }
  else if(mode == SNS_DDF_POWERSTATE_ACTIVE)
  {
    state->curr_pwr_state = SNS_DDF_POWERSTATE_ACTIVE;
  }
  else
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_PWR_MODE_NOT_SUPPORTED, mode);
    return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   snsd_mgt_akm_start_meas_8975

===========================================================================*/
/*!
  @brief Program the meas mode on AKM 8975

  @detail
  - Prog the AKM_MAG_REG_ADDR_CNTL_8975 register for meas mode

  @param[in] *state - ptr to the driver state

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_mgt_akm_start_meas_8975
(
  sns_dd_mag_akm8975_state_s *state
)
{
  uint8_t           akm_write_buffer, out;
  sns_ddf_status_e  err_code = SNS_DDF_SUCCESS;

  /* start measurement mode */
  akm_write_buffer = AKM_MAGNETOMETER_SINGLE_MEAS_MODE_8975;
  if((err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_CNTL_8975,
                          &akm_write_buffer,
                          DDF_WRITE_ONE_BYTE,
                          &out)) == SNS_DDF_SUCCESS)
  {
    err_code = sns_ddf_timer_start(state->sns_dd_mag_tmr_obj, USEC_TIME_FOR_MEASUREMENT_8975);
  }
  return err_code;
}

/*===========================================================================

  FUNCTION:   sns_mgt_akm_get_sensitivity_data_8975

===========================================================================*/
/*!
  @brief Gets the sensitivity adjusment values of the AKM8975 magnetometer

  @detail
  - Get the axis sensitivity adjusment values from the fuse ROM, store it in the driver state
    structure in Q16 gauss units.

  @param[in] *state - ptr to the driver state

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success;

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_mgt_akm_get_sensitivity_data_8975(sns_dd_mag_akm8975_state_s *state)
{
  uint8_t           out, akm_write_buffer;
  uint8_t           read_data[SENSITIVITY_NUM_BYTES];
  sns_ddf_status_e  err_code;
  int               i;

  /* Set Fuse ROM access mode */
  akm_write_buffer = AKM_MAGNETOMETER_FUSE_ROM_ACCESS_MODE_8975;
  if((err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_CNTL_8975,
                          &akm_write_buffer,
                          DDF_WRITE_ONE_BYTE,
                          &out)) != SNS_DDF_SUCCESS)
  {
    // TODO: ADD Write Error string
    //SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_WRITE_FAILURE, AKM_MAGNETOMETER_FUSE_ROM_ACCESS_MODE_8975);
    return err_code;
  }

  /* Read the data */
  if ( (err_code = sns_ddf_read_port(state->port_handle,
                              AKM_MAG_REG_ADDR_X_SEN_ADJUST_8975,
                              read_data,
                              SENSITIVITY_NUM_BYTES,
                              &out)) != SNS_DDF_SUCCESS)
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_READ_FAILURE, AKM_MAG_REG_ADDR_X_SEN_ADJUST_8975);
    return err_code;
  }

  for(i=0 ; i<3 ; ++i)
  {
    state->sens_cache[i] = read_data[i];
    // Sensitivity adjusment is calculated acording to AKM8975 datasheet version MS1187-E-02 section 8.3.11
    state->sens_adj[i] = (((state->sens_cache[i] - 128 ) * 0.5)/128) + 1;
  }

  MAG8975_DBG_MSG_3(DBG_DD_MAG8975_SENSITIVITY_DATA, state->sens_cache[0], state->sens_cache[1], state->sens_cache[2]);

  /* Set Power-down mode */
  akm_write_buffer = AKM_MAGNETOMETER_PWR_DOWN_MODE_8975;
  if((err_code = sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_CNTL_8975,
                          &akm_write_buffer,
                          DDF_WRITE_ONE_BYTE,
                          &out)) != SNS_DDF_SUCCESS)
  {
    // TODO: ADD Write Error string
    //SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_WRITE_FAILURE, AKM_MAGNETOMETER_PWR_DOWN_MODE_8975);
    return err_code;
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_dd_mag8975_log_data
===========================================================================*/
/*!
  @brief log sensor data for AKM8975 magnetometer

  @detail
  - Logs latest set of sensor data sampled from the AKM8975 mag sensor.

  @param[in] state: ptr to the driver structure
  @param[in] sample_time: Time that the sensor was sampled

  @return
  None

*/
/*=========================================================================*/
void sns_dd_mag8975_log_data(sns_dd_mag_akm8975_state_s *state,
                         sns_ddf_time_t sample_time)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version   = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_MAG;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AKM;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = sample_time;

    //Log the sensor data
    log_struct_ptr->num_data_types = AKM_MAG8975_NUM_DATATYPES;
    log_struct_ptr->data[0]        = state->data_cache[0];
    log_struct_ptr->data[1]        = state->data_cache[1];
    log_struct_ptr->data[2]        = state->data_cache[2];


    //Commit log (also frees up the log packet memory)
    sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log_struct_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_dd_mag8975_notify_smgr

===========================================================================*/
/*!
  @brief Called SMGR needs to be notified of data ready

  @note This will routine will be called from
  sns_mgt_akm_sample_sensor_* routine when new data is ready

  @param[in] *state               ptr to the driver state structure
  @param[in] sensor_data_report   Sensor data to pass to the Sensors Manager.
  @param[in] num_sensors          Length of sensor_data_report.

  @return
  None

*/
/*=========================================================================*/
static void sns_dd_mag8975_notify_smgr(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_sensor_data_s*      sensor_data_report,
  uint32_t                    num_sensors)
{
#ifndef MAG8975_UNIT_TEST
  sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, num_sensors);
#endif
}

/*===========================================================================

  FUNCTION:   sns_dd_mag8975_notify_smgr_err

===========================================================================*/
/*!
  @brief Called SMGR needs to be notified of error

  @note This will routine will be called from
  sns_mgt_akm_sample_sensor_* routine when an error is seen in memory
  allocation for sample data or problem in reading the data from the
  Mag. The top level sensor data structure with filled with appropriate
  error type before calling the sns_dd_mag8975_notify_smgr routine

  @param[in] *state               ptr to the driver state structure
  @param[in] err_code             error code for the problem
  @param[in] sensor_data_report   Sensor data to pass to the Sensors Manager.
  @param[in] num_sensors          Length of sensor_data_report.

  @return
  None

  $TODO:
*/
/*=========================================================================*/
static void sns_dd_mag8975_notify_smgr_err(
  sns_dd_mag_akm8975_state_s* state,
  sns_ddf_status_e            err_code,
  sns_ddf_sensor_data_s*      sensor_data_report,
  uint32_t                    num_sensors)
{
  uint8_t i;

  for(i=0; i<(uint8_t)num_sensors; i++)
  {
    sensor_data_report[i].sensor = SNS_DDF_SENSOR_MAG; 
    sensor_data_report[i].status = err_code;
    sensor_data_report[i].timestamp = sns_ddf_get_timestamp();
  }

  sns_dd_mag8975_notify_smgr(state, sensor_data_report, num_sensors);
}

/*===========================================================================

  FUNCTION:   sns_mgt_akm_sample_sensor_8975

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired.

  @note This routine will read the data status & read the data and copy
  it at a malloced location. The address of the data location is passed
  as parameter in the notify_data routine.

  @param[in] *state   ptr to the driver state structure

  @return
  None

  $TODO:
*/
/*=========================================================================*/
static void sns_mgt_akm_sample_sensor_8975(sns_dd_mag_akm8975_state_s *state)
{
  uint8_t                   akm_read_data[MAG_DATA_NUM_BYTES]= {0}, out, size;
  int16_t                   temp_q16[AKM_MAG8975_NUM_DATATYPES];
  sns_ddf_status_e          err_code;
  sns_ddf_sensor_data_s*    sensor_data_report;
  sns_ddf_sensor_sample_s*  data_ptr;
  int                       i;

  /* Resetting the state to idle once into this routine implying
  whatever the error case SMGR could request for data again & the driver
  would come back with appropriate error condition again instead of
  nacking based on state being not idle*/
  state->curr_state = SNSD_MGT8975_ST_IDLE;

  size = sizeof(sns_ddf_sensor_data_s);
  if((err_code = sns_ddf_malloc((void **)&sensor_data_report, size)) != SNS_DDF_SUCCESS)
  {
    // returning the NULL to SMGR indicating no memory even for basic err notify
    sns_dd_mag8975_notify_smgr(state, NULL, 0);
    return;
  }

  /* Read ST1, 6 data registers, ST2 in a single 8-byte I2C transaction.
     Then check ST1 and ST2 values to determine if data is really good */
  if ( (err_code = sns_ddf_read_port(state->port_handle,
                              AKM_MAG_REG_ADDR_STATUS1_8975,
                              akm_read_data,
                              MAG_DATA_NUM_BYTES,
                              &out)) != SNS_DDF_SUCCESS)
  {
    //Handling the err cond when status from Mag is not a success
    sns_dd_mag8975_notify_smgr_err(state, err_code, sensor_data_report, 1);
    sns_ddf_mfree(sensor_data_report);

    return;
  }

  if (EnableI2C == TRUE)
  {
    if((akm_read_data[0] & AKM_MAGNETOMETER_DRDY_BIT_MASK) == 0)
    {
      /* Handling the err cond when status indicates data is still not
               ready after giving it ample time to be ready 7msec */
      sns_dd_mag8975_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, 1);
      sns_ddf_mfree(sensor_data_report);

      return;
    }

    if(akm_read_data[7] & AKM_MAGNETOMETER_HOFL_BIT_MASK)
    {
      /* Handling the err cond when magnetic sensor may overflow even though measurement
               data regiseter is not saturated */
      sns_dd_mag8975_notify_smgr_err(state, SNS_DDF_EDEVICE, sensor_data_report, 1);
      sns_ddf_mfree(sensor_data_report);
      return;
    }
  }

//Storing raw data in 16 bits
  temp_q16[0] = akm_read_data[1]|(akm_read_data[2]<<8);
  temp_q16[1] = akm_read_data[3]|(akm_read_data[4]<<8);
  temp_q16[2] = akm_read_data[5]|(akm_read_data[6]<<8);

  for(i=0 ; i<3 ; i++)
  {
    // Adjust sensitivity values to the measurement data read out from the measurement data register
    temp_q16[i] = temp_q16[i]*state->sens_adj[i];
    /* convert based on base range */
    state->data_cache[i] = AKM8975_RAW_TO_GAUSS(temp_q16[i]);
  }

  size = AKM_MAG8975_NUM_DATATYPES*sizeof(sns_ddf_sensor_sample_s);
  if((err_code = sns_ddf_malloc((void **)&data_ptr, size)) != SNS_DDF_SUCCESS)
  {
    // Handling the err cond when no mem avail to give data samples to SMGR
    sns_dd_mag8975_notify_smgr_err(state, SNS_DDF_ENOMEM, sensor_data_report, 1);
    sns_ddf_mfree(sensor_data_report);
    return;
  }

  // Convert sensor data to the phone's coordinate system.
  sns_ddf_map_axes(&state->axes_map, state->data_cache);

  if (EnableI2C == TRUE)
  {
    data_ptr[0].sample = state->data_cache[0];
    data_ptr[1].sample = state->data_cache[1];
    data_ptr[2].sample = state->data_cache[2];
  }
  else
  {
    static uint32 magDummyData = 2001;
    data_ptr[0].sample = magDummyData++;
    data_ptr[1].sample = magDummyData++;
    data_ptr[2].sample = magDummyData++;
  }

  data_ptr[0].status = data_ptr[1].status = data_ptr[2].status = SNS_DDF_SUCCESS;

  MAG8975_DBG_MSG_3(DBG_DD_MAG8975_GET_DATA_REQ, state->data_cache[0], state->data_cache[1], state->data_cache[2]);

  /* mark data as ready */
  sensor_data_report->sensor      = SNS_DDF_SENSOR_MAG;
  sensor_data_report->status      = SNS_DDF_SUCCESS;
  sensor_data_report->timestamp   = sns_ddf_get_timestamp();
  sensor_data_report->samples     = data_ptr;
  sensor_data_report->num_samples = AKM_MAG8975_NUM_DATATYPES;

  sns_dd_mag8975_notify_smgr(state, sensor_data_report, 1);
  sns_dd_mag8975_log_data(state, sensor_data_report->timestamp);

  sns_ddf_mfree(data_ptr);
  sns_ddf_mfree(sensor_data_report);
}

/*===========================================================================

  FUNCTION:   sns_mgt_akm_self_test_result

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. 
 
  @note This routine will read the data status & read the data and assess if
  data is within range to determine pass/fail verdict.
 
  @param[in] *state   ptr to the driver state structure

  @return
  None
 
  $TODO: 
*/
/*=========================================================================*/
static void sns_mgt_akm_self_test_result(sns_dd_mag_akm8975_state_s *state)
{
  uint8_t                   akm_read_data[MAG_DATA_NUM_BYTES], out;
  int                       i;
  uint8_t					akm_write_buffer_astc;
  int16_t                   data[AKM_MAG8975_NUM_DATATYPES];

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8400);

  /* Reset the state to idle */
  state->curr_state = SNSD_MGT8975_ST_IDLE;

  /* Sensor transitions to low power automatically */
  state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;

  /* Read ST1, 6 data registers, ST2 in a single 8-byte I2C transaction. 
     Then check ST1 and ST2 values to determine if data is really good */
  if ( sns_ddf_read_port(state->port_handle,
                         AKM_MAG_REG_ADDR_STATUS1_8975, 
                         akm_read_data,
                         MAG_DATA_NUM_BYTES,
                         &out) != SNS_DDF_SUCCESS)
  {
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_I2C);
    return;
  }

  akm_write_buffer_astc = AKM_MAGNETOMETER_ASTC_SELF_TEST_DISABLE;
  if ( sns_ddf_write_port(state->port_handle,
                          AKM_MAG_REG_ADDR_SELF_TEST_8975,
                          &akm_write_buffer_astc,
                          DDF_WRITE_ONE_BYTE,  
                          &out) != SNS_DDF_SUCCESS)
  {
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_I2C);
    return;
  }

  if((akm_read_data[0] & AKM_MAGNETOMETER_DRDY_BIT_MASK) == 0)
  {
    /* Handling the err cond when status indicates data is still not
       ready after giving it ample time to be ready 7msec */
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_DRDY);
    return;
  }

  if (akm_read_data[7] & AKM_MAGNETOMETER_HOFL_BIT_MASK)
  {
    /* Handling the err cond when magnetic sensor may overflow even though measurement 
      data regiseter is not saturated */
    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_OVFL);
    return;
  }

  /* raw data in 16 bits  */
  data[0] = akm_read_data[1]|(akm_read_data[2]<<8);
  data[1] = akm_read_data[3]|(akm_read_data[4]<<8);
  data[2] = akm_read_data[5]|(akm_read_data[6]<<8);  

  MAG8975_SMGR_DBG_MSG_3(DBG_SMGR_GENERIC_STRING3,
                         data[0],
                         data[1],
                         data[2]);

  MAG8975_SMGR_DBG_MSG_3(DBG_SMGR_GENERIC_STRING3, 
                         state->sens_cache[0], state->sens_cache[1], 
                         state->sens_cache[2]);

  for(i=0 ; i<3 ; i++)
  {
    /* Adjust sensitivity values to the measurement data read
       out from the measurement data register */
    state->data_cache[i] = (data[i]*state->sens_adj[i]);
  }

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8500);

  MAG8975_SMGR_DBG_MSG_3(DBG_SMGR_GENERIC_STRING3,
                         state->data_cache[0],
                         state->data_cache[1],
                         state->data_cache[2]);

  /* check if data is within valid range */
  if ((FX_ABS(state->data_cache[0]) <= 100.0) &&
      (FX_ABS(state->data_cache[1]) <= 100.0) &&
      ((state->data_cache[2] >= -1000.0) && 
       (state->data_cache[2] <= -300.0)))
  {
    MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8600);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_SUCCESS,
                                      SNSD_MAG_ERR_GENERIC);
  }
  else
  {
    MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8700);

    sns_ddf_smgr_notify_test_complete(state->smgr_hndl,
                                      SNS_DDF_SENSOR_MAG,
                                      SNS_DDF_EFAIL,
                                      SNSD_MAG_ERR_RESULT_OUT_OF_RANGE);
  }

  sns_dd_mag8975_log_data(state, sns_ddf_get_timestamp()); 

}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This must be
         the callback function submitted when initializing a timer.

  @note This will be called within the context of the Sensors Manager task.

     @param[in] dd_handle   Handle to a driver instance.
     @param[in] arg         The argument submitted when the timer was set.

  @return
    None

  $TODO:
*/
/*=========================================================================*/
static void sns_dd_akm8975_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
  sns_dd_mag_akm8975_state_s *state = (sns_dd_mag_akm8975_state_s *)dd_handle;

  if (state->curr_state == SNSD_MGT8975_ST_TEST_STARTED)
  {
    sns_mgt_akm_self_test_result(state);
  }
  else
  {
    sns_mgt_akm_sample_sensor_8975(state);
  }
}

/*###########################################################################
  *                    DDF APIs
 ###########################################################################*/

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_init

===========================================================================*/
/*!
  @brief Mag driver initialization.

  @detail
  Allocates memory for driver state structure.
  Opens the device port by calling sns_ddf_open_port routine

  @param[out] dd_handle_ptr  Pointer that this function must malloc and
                             populate. This is a handle to the driver
                             instance that will be passed in to all
                             other functions.
  @param[in]  smgr_handle    Handle used to identify this driver when it
                             calls into Sensors Manager functions.
  @param[in]  nv_params      NV parameters retrieved for the driver.
  @param[in]  device_info    Information describing each of the physical
                             devices controlled by this driver. This is
                             used to configure the bus and talk to the
                             device.
  @param[in]  memhandler     Memory handler used to dynamically allocate
                             output parameters, if applicable. NB: Do
                             not use memhandler to allocate memory for
                             @a dd_handle_ptr.
  @param[in]  num_devices    Length of @a device_info.
  @param[out] sensors        List of supported sensors, populated and
                              returned by this function.
  @param[out] num_sensors    Length of @a sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_akm8975_init(
  sns_ddf_handle_t*        dd_handle_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
)
{
  sns_dd_mag_akm8975_state_s* state;
  sns_ddf_status_e            status;
  uint8_t                     out;

  *dd_handle_ptr = NULL;
  *sensors      = NULL;

  MAG8975_DBG_MSG_0(DBG_DD_MAG8975_INITIALIZING);

  if(sns_ddf_malloc((void **)&state, sizeof(sns_dd_mag_akm8975_state_s)) != SNS_DDF_SUCCESS)
  {
    return SNS_DDF_ENOMEM;
  }

  memset(state, 0, sizeof(sns_dd_mag_akm8975_state_s));
  drv_akm8975_dbg = state; /* Debug Only */
  *dd_handle_ptr  = state;

  if((status = sns_ddf_open_port(&(state->port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
  {
    goto errorCleanUp;
  }

  state->smgr_hndl      = smgr_handle;
  state->dev_info       = device_info;
  state->curr_pwr_state = SNS_DDF_POWERSTATE_LOWPOWER;
  state->curr_state     = SNSD_MGT8975_ST_IDLE;

  sns_ddf_axes_map_init(
      &state->axes_map, ((nv_params != NULL) ? nv_params->data : NULL));

  if((*sensors = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_e))) == NULL)
  {
    status = SNS_DDF_ENOMEM;
    goto errorCleanUp;
  }

  /* Fill out supported sensor info */
  (*sensors)[0]   = SNS_DDF_SENSOR_MAG;
  *num_sensors    = NUM_SENSORS_SUPPORTED_BY_AK8975;

  /* Init akm8975 timer */
  if((status = sns_ddf_timer_init(&state->sns_dd_mag_tmr_obj,
                                (sns_ddf_handle_t)state,
                                &sns_mag_akm8975_driver_fn_list,
                                state,
                                0)) != SNS_DDF_SUCCESS)
  {
    goto errorCleanUp;
  }

  // Read Device ID, to make sure device is working properly
  status = sns_ddf_read_port((sns_ddf_handle_t)state->port_handle,
                              AKM_MAG_REG_ADDR_DEVICEID_8975,
                              &state->device_id,
                              DDF_READ_ONE_BYTE,
                              &out);

  if(status != SNS_DDF_SUCCESS)
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_READ_FAILURE, AKM_MAG_REG_ADDR_DEVICEID_8975);
    goto errorCleanUp;
  }

  if (EnableI2C == TRUE)
  {
    if(state->device_id != AKM_MAG_8975_DEVICE_ID)
    {
      status = SNS_DDF_EDEVICE;
      goto errorCleanUp;
    }
  }

  // Read Device INFO
  status = sns_ddf_read_port((sns_ddf_handle_t)state->port_handle,
                              AKM_MAG_REG_ADDR_INFO_8975,
                              &state->device_info,
                              DDF_READ_ONE_BYTE,
                              &out);

  if(status != SNS_DDF_SUCCESS)
  {
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_MAG8975, DBG_DD_MAG8975_READ_FAILURE, AKM_MAG_REG_ADDR_INFO_8975);
    goto errorCleanUp;
  }

  /* Read sensitivity data */
  if ((status = sns_mgt_akm_get_sensitivity_data_8975(state)) != SNS_DDF_SUCCESS)
  {
    goto errorCleanUp;
  }

  return SNS_DDF_SUCCESS;

  errorCleanUp:
    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  - resets some of the driver states
  call the sns_mgt_akm_config_default_8975 in which the DAC offset and
  gain registers set based on the EEPROM values

  @param[in] dd_handle   Handle to a driver instance.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_reset(sns_ddf_handle_t dd_handle)
{
  sns_ddf_status_e            status;
  sns_dd_mag_akm8975_state_s* state = (sns_dd_mag_akm8975_state_s *)dd_handle;

  status = sns_dd_akm8975_config_pwr_mode(state, SNS_DDF_POWERSTATE_LOWPOWER);

  state->curr_state      = SNSD_MGT8975_ST_IDLE;

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the either the AKM8973 or AKM8975 mag

  @detail
  - Called by SMGR to set certain device attributes that are
    programmable. Mag has only pwr attribute that can be changed.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] sensor     Sensor for which this attribute is to be set.
  @param[in] attrib     Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_set_attr
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e            ret_val = SNS_DDF_SUCCESS;
  sns_dd_mag_akm8975_state_s* state = (sns_dd_mag_akm8975_state_s *)dd_handle;
  sns_ddf_powerstate_e        power_state;

  MAG8975_DBG_MSG_2(DBG_DD_MAG8975_SET_ATTRIB_REQ,
                                sensor,
                                attrib );

  if ((sensor != SNS_DDF_SENSOR_MAG) && (sensor != SNS_DDF_SENSOR__ALL))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch(attrib)
  {
     case SNS_DDF_ATTRIB_POWER_STATE:
        /* set power mode */
       power_state = *(sns_ddf_powerstate_e *)value;
       ret_val = sns_dd_akm8975_config_pwr_mode(state, power_state);
       MAG8975_DBG_MSG_1(DBG_DD_MAG8975_SET_POWER_STATE_REQ, state);
        break;

     case SNS_DDF_ATTRIB_RANGE:
        /* AKM8975 support only 1 set of range */
       MAG8975_DBG_MSG_0(DBG_DD_MAG8975_SET_RANGE_REQ);
        break;

     case SNS_DDF_ATTRIB_RESOLUTION:
        /* AKM8975 only supports 1 resolution level */
       MAG8975_DBG_MSG_0(DBG_DD_MAG8975_SET_RESOLUTION_REQ);
       break;

     default:
       return SNS_DDF_EINVALID_ATTR;
  }
  return(ret_val);
}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - All the info of the devices is from their data sheet.

  @param[in] dd_handle    Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in] attrib       Attribute to be retrieved.
  @param[in] memhandler   Memory handler used to dynamically allocate
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_get_attr
(
  sns_ddf_handle_t       dd_handle,
  sns_ddf_sensor_e       sensor,
  sns_ddf_attribute_e    attrib,
  sns_ddf_memhandler_s*  memhandler,
  void**                 value,
  uint32_t*              num_elems
)
{
  MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_ATTRIB_REQ, sensor, attrib );

  if (sensor != SNS_DDF_SENSOR_MAG)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
    {
      sns_ddf_power_info_s* power_attrib;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
           sizeof(sns_ddf_power_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      power_attrib                    = *value;
      *num_elems                      = 1;
      power_attrib->active_current    = AKM_MAG8975_HI_PWR; /* unit of uA */
      power_attrib->lowpower_current  = AKM_MAG8975_LO_PWR; /* unit of uA */
      MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_POWER_INFO_REQ,
                                   power_attrib->active_current,
                                   power_attrib->lowpower_current);
    }
    break;
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))
            == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
      device_ranges       = *value;
      *num_elems          = 1;
      device_ranges->min  = AKM_MAG8975_MIN_RANGE;
      device_ranges->max  = AKM_MAG8975_MAX_RANGE;
      MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_RANGE_REQ,
                                   device_ranges->min,
                                   device_ranges->max);
    }
    break;
	#ifndef USE_CC41
	case SNS_DDF_ATTRIB_RESOLUTION_ADC:
	{
		sns_ddf_resolution_adc_s *device_res_adc;
		if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
		{
		  return SNS_DDF_ENOMEM;
		}
		device_res_adc 				= *value;
		*num_elems 					= 1;
		device_res_adc->bit_len 	= AKM_MAG8975_RESOLUTION_ADC; //13 bit ADC resolution
		device_res_adc->max_freq 	= 50;
	}
	break;
	#endif
    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t *device_res;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_t))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_res  = *value;
      *num_elems  = 1;
#ifdef USE_CC41
     *device_res = AKM_MAG8975_RESOLUTION_ADC;
#else
     *device_res = mag_akm8975_scale;
#endif
      MAG8975_DBG_MSG_1(DBG_DD_MAG8975_GET_RESOLUTION_REQ, *device_res);
    }
    break;
    case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s *device_delay;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_delays_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_delay  = *value;
      *num_elems    = 1;
      device_delay->time_to_active  = USEC_TIME_FOR_MEASUREMENT_8975;
      device_delay->time_to_data    = USEC_TIME_FOR_MEASUREMENT_8975;
      MAG8975_DBG_MSG_2(DBG_DD_MAG8975_GET_DELAYS_REQ,
                                   device_delay->time_to_active,
                                   device_delay->time_to_data);
    }
    break;
    case SNS_DDF_ATTRIB_DRIVER_INFO:
    {
      sns_ddf_driver_info_s *driver_info;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_driver_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      driver_info = *value;
      *num_elems = 1;
      driver_info->name = "AK8975";
#ifdef USE_CC41
      driver_info->version = "1.0";
#else
      driver_info->version = 1;
#endif
    }
    break;
    case SNS_DDF_ATTRIB_DEVICE_INFO:
    {
      sns_ddf_device_info_s *device_info;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_device_info_s))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }
      device_info = *value;
      *num_elems = 1;
      device_info->model = "AK8975";
      device_info->vendor = "AKM";
      device_info->name = "Magnetometer";
#ifdef USE_CC41
      device_info->version = "1.0";
#else
      device_info->version = 1;
#endif
    }
    break;
    default:
      return SNS_DDF_EINVALID_PARAM;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_akm8975_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
    sensors. Mag is programmed to measurement mode where it would sample
    data and a few msec later be ready with the three axis Mag data.
    Hence a timer is set to be called back with the appropriate delay
    which would be the time to data ready state

  @param[in] dd_handle     Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
                           output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
                           must match @a num_sensors.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_akm8975_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[])
{
  sns_ddf_status_e err_code;
  sns_dd_mag_akm8975_state_s *state = (sns_dd_mag_akm8975_state_s *)dd_handle;

  /* state machine */
  switch (state->curr_state)
  {
    case SNSD_MGT8975_ST_IDLE:

      if((err_code = sns_dd_mgt_akm_start_meas_8975(state)) == SNS_DDF_SUCCESS)
      {
        state->curr_state = SNSD_MGT8975_ST_STARTED;
        /* Mag returns data asynchronously. Hence set PENDING
               * status to return to Sensor Manager */
        err_code = SNS_DDF_PENDING;

        #ifdef MAG8975_UNIT_TEST
        sns_ddf_delay(USEC_TIME_FOR_MEASUREMENT_8975);
        sns_dd_akm8975_handle_timer(dd_handle, NULL);
        err_code = SNS_DDF_SUCCESS;
        #endif

      }
    break;

    case SNSD_MGT8975_ST_STARTED:
      /* This is added for completeness and making SMGR aware that it first
      needs to change the HW device state to active and then get data which
      in case of mag doesn't actually make a difference*/
      err_code = SNS_DDF_PENDING;
      MAG8975_DBG_MSG_1(DBG_DD_MAG8975_GET_ERROR, state->curr_state);
    break;

    case SNSD_MGT8975_ST_TEST_STARTED:
      /* If self test is currently ongoing, streaming cannot be supported */ 
      err_code = SNS_DDF_EDEVICE_BUSY;
      MAG8975_DBG_MSG_1(DBG_DD_MAG8975_GET_ERROR, state->curr_state);
    break;

    default:
      err_code = SNS_DDF_EFAIL;
    break;
  }

  return err_code;
}


/*===========================================================================

  FUNCTION:   sns_dd_akm8975_hdle_irq

===========================================================================*/
/*!
  @brief Called by the SMGR in response to an interrupt

  @detail

    @param[in] dd_handle  Handle to a driver instance.
    @param[in] irq        The IRQ representing the interrupt that occured.

  @return
  None

  $TODO:
*/
/*=========================================================================*/
static void sns_dd_akm8975_hdle_irq(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp)
{
  MAG8975_DBG_MSG_0(DBG_DD_MAG8975_HANDLE_IRQ_REQ);
  return;
}
/*===========================================================================

  FUNCTION:   sns_dd_akm8975_self_test

===========================================================================*/
/**
 * @brief Runs a factory self test case. 
 *  
 * Self tests cover an embedded hardware test in cases where the sensor 
 * supports it, as well as driver based sensor tests. This is generally run 
 * in a factory setting, and must not be called while a device is streaming 
 * data. 
 *  
 * @param[in] dd_handle  Handle to a driver instance. 
 * @param[in] sensor     Sensor on which to run the test. 
 * @param[in] test       Test case to run. 
 *  
 * @return One of the following error codes:
 *     SNS_DDF_SUCCESS        - Test passed.
 *     SNS_DDF_PENDING        - Test result will be sent as an event.
 *     SNS_DDF_EDEVICE_BUSY   - Device is busy streaming, cannot run test.
 *     SNS_DDF_EINVALID_TEST  - Test is not defined for this sensor.
 *     SNS_DDF_EINVALID_PARAM - One of the parameters is invalid.
 *     SNS_DDF_EFAIL          - Unknown error occured.
 */
sns_ddf_status_e sns_dd_akm8975_self_test(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_test_e       test,
  uint32_t             *err)
{
  sns_dd_mag_akm8975_state_s *state = (sns_dd_mag_akm8975_state_s *)dd_handle;
  uint8_t                    akm_write_buffer_astc, akm_write_buffer_cntl;
  uint8_t                    out, akm_write_buffer_lpm;
  sns_ddf_status_e           status = SNS_DDF_SUCCESS;
  sns_akm_err_codes_e        err_code = SNSD_MAG_ERR_GENERIC;

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8100);

  *err = SNSD_MAG_ERR_GENERIC;

  /* check parameter - sensor */
  if (sensor != SNS_DDF_SENSOR_MAG)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* check parameter - test */
  if (test != SNS_DDF_TEST_SELF)
  {
    return SNS_DDF_EINVALID_TEST;
  }

  /* check state */
  if (state->curr_state != SNSD_MGT8975_ST_IDLE)
  {
    MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, state->curr_state);
    return SNS_DDF_EDEVICE_BUSY;
  }

  MAG8975_SMGR_DBG_MSG_1(DBG_SMGR_GENERIC_STRING1, 8200);

  akm_write_buffer_astc = AKM_MAGNETOMETER_ASTC_SELF_TEST_ENABLE; 
  akm_write_buffer_cntl = AKM_MAGNETOMETER_SELF_TEST_MODE_8975;
  akm_write_buffer_lpm = AKM_MAGNETOMETER_PWR_DOWN_MODE_8975; 

  status = sns_ddf_write_port(state->port_handle,
                            AKM_MAG_REG_ADDR_CNTL_8975,
                            &akm_write_buffer_lpm,
                            DDF_WRITE_ONE_BYTE,  
                            &out);

  MAG8975_SMGR_DBG_MSG_2(DBG_SMGR_GENERIC_STRING2, 8210, status);

  if (status == SNS_DDF_SUCCESS)
  {
    sns_ddf_delay(100); /* wait for mode change to take effect */
  
    if(((status = sns_ddf_write_port(state->port_handle,
                                     AKM_MAG_REG_ADDR_SELF_TEST_8975,
                                     &akm_write_buffer_astc,
                                     DDF_WRITE_ONE_BYTE,
                                     &out)) == SNS_DDF_SUCCESS) &&
        ((status = sns_ddf_write_port(state->port_handle,
                                      AKM_MAG_REG_ADDR_CNTL_8975,
                                      &akm_write_buffer_cntl,
                                      DDF_WRITE_ONE_BYTE,
                                      &out)) == SNS_DDF_SUCCESS))
    {
      state->curr_state = SNSD_MGT8975_ST_TEST_STARTED;
      if ((status = sns_ddf_timer_start(state->sns_dd_mag_tmr_obj, 
                       USEC_TIME_FOR_MEASUREMENT_8975)) != SNS_DDF_SUCCESS)
      {
        err_code = SNSD_MAG_ERR_TIMER;
        state->curr_state = SNSD_MGT8975_ST_IDLE;
      }
    }
    else
    {
      err_code = SNSD_MAG_ERR_I2C;
    }
    MAG8975_SMGR_DBG_MSG_2(DBG_SMGR_GENERIC_STRING2, 8300, err_code);
  }
  else
  {
    err_code = SNSD_MAG_ERR_I2C;
  }

  if (status == SNS_DDF_SUCCESS)
  {
    return SNS_DDF_PENDING;
  }
  else
  {
    *err = err_code;
    return SNS_DDF_EFAIL;
  }
}


