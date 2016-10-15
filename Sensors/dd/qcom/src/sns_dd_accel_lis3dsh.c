/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 *
 * File Name         : lis3dsh_ddf_drv.c
 * Authors           : Jianjian(Samuel) Huo
 *                   : karimuddin Sayed
 * Version           : V 3.6.4
 * Date              : 03/13/2014
 * Description       : LIS3DSH Accelerometer driver source file
 *
 ********************************************************************************
 * Copyright (c) 2013, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************
 * REVISON HISTORY
 *
 * VERSION | DATE          | DESCRIPTION
 * 3.6.4      | 05/16/2014 | Fixed KW issue
 * 3.6.3      | 04/25/2014 | Eliminated compiler wanring
 * 3.6.2      | 03/19/2014 | Changed interrupt detection from rising edge to high
 * 3.6.1      | 03/13/2014 | Added INT registration tracking becaused of new sensor ODR inquiry logic during init.
 * 3.6.0      | 03/11/2014 | Used a different setting for MD, once double tap is enabled.
 *                           Redesigned sensor turning off procedures in set ODR, added a new turnoff_dbt function.
 * 3.5.9      | 03/07/2014 | Fixed the bug in lis3dsh_is_active_other_sub_dev
 *                           Merged process_daf_req.
 * 3.5.8      | 01/23/2014 | Flush FIFO when FIFO interrupt is missed during fifo int enable. 
 * 3.5.7      | 12/16/2013 | Fixed two FIFO related problems when gyro FIFO sync mode is used. 
 * 3.5.6      | 11/15/2013 | Marks invalid FIFO data produced before new ODR is settled.
 * 3.5.5      | 10/18/2013 | Set different delays between ODR change in set_datarate().
 * 3.5.4      | 10/16/2013 | Fixed the bandwidth setting in set_datarate()
 * 3.5.3      | 05/23/2013 | Disabled BDU during FIFO and SM usages.
 * 3.5.2      | 05/03/2013 | Cleaned up some comments, disabled log messages.
 * 3.5.0      | 04/30/2013 | Updated timestamp calculation in handle_irq for fifo drain.
 * 3.4.9      | 04/29/2013 | Fixed bugs in handle_irq.
                             Updated get_attr for double tap.
 * 3.4.7      | 04/25/2013 | Changed the max freq in get_attr to 200.
                             Added changes to support WM and OVERFLOW concurrency.
                             Added while loop in irq_handle to support all INT source concurrency.
                             Read all samples in FIFO instead of watermark level.
                             Change IEL in CTRL_REG3 back to pulse mode.
 * 3.4.2      | 04/24/2013 | Added IO read/write in get/set attr.
                             Updated dd_find_free_sub_dev_slot function return error.
 * 3.4.0      | 04/23/2013 | Merged self test from version 1.6.3.
                             Moved MD & DBT interrupt clearing in MD & DBT INT enable.
 * 3.3.0      | 04/22/2013 | Added changes to support dynamic watermark.
 * 3.2.8      | 04/18/2013 | Fixed FIFO INT problems.
 * 3.2.5      | 04/17/2013 | Fixed mfree bugs in disable_fifo.
 * 3.2.4      | 04/16/2013 | Fixed bugs in send_fifo_data.
 * 3.2.0      | 04/15/2013 | Added more debugging messages.
 * 3.1.0      | 04/11/2013 | Merged 2.6.0, updated malloc usage,
                             updated trigger_fifo_data to improve timestamp accuracy.
 * 3.0.0      | 04/10/2013 | Added FIFO support for accel.
 * 2.6.0      | 04/09/2013 | Updated double tap algorithms to reduce false alarms.
 * 2.5.0      | 04/03/2013 | Added concurrency support between MD and DT.
 * 2.4.0      | 04/01/2013 | Fixed concurrency and other bugs.
 * 2.3.0      | 03/14/2013 | Added registry and NV_PARAMS support.
                             Added new ST double tap algorithm.
 * 2.1.0      | 03/14/2013 | Added Bandwidth setting along with ODR.
 * 2.0.0      | 03/12/2013 | New combo driver with double tap as 2nd sensor.
 * 1.6.3      | 04/04/2013 | Added Registry for md parameters
 * 1.6.2      | 04/04/2013 | Added Bias calculation and modified return errors
 * 1.6.1      | 03/21/2013 | updated MD with 50Hz and vector filter
 * 1.6.0      | 03/15/2013 | Added support for OEM selftest
 * 1.5.0      | 03/06/2013 | Updated state machine configs for MD interrupt.
 * 1.4.0      | 02/19/2013 | Added motion detection interrupt support.
 * 1.3.0      | 02/18/2013 | Combined HW and SW self tests.
 * 1.2.2      | 02/13/2013 | Updated self test parameters.
 * 1.2.0      | 01/31/2013 | Added HW and SW self tests.
 * 1.1.0      | 01/18/2013 | Merged a missing patch.
 * 1.0.0      | 10/02/2012 | Created.
*******************************************************************************/

/*******************************************************************************
* Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*******************************************************************************/

/*=============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  03/24/2014  tc   Added support for the Driver Access Framework
  01/12/2014  pn   Flushes FIFO when FIFO interrupt is missed
  11/18/2013  pn   - BW timer replaced with simple timestamp
                   - Marks invalid FIFO data produced before new ODR is settled
  10/25/2013  vh   Synced with STM's version number
  10/22/2013  vh   Added Qualcomm Copyright
=============================================================================*/

#include "sns_dd_accel_lis3dsh.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define CONFIG_OEM

#if STM_LIS3DSH_LOGDATA
//Include files required to support accel log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#endif

#define STM_LIS3DSH_NUM_READ_BYTES  6
#define AUTO_INCREMENT             0x80

sns_ddf_driver_if_s sns_dd_acc_lis3dsh_if;

/*------------------------------------------------------
  Access to externalized functions.
  ------------------------------------------------------*/

extern sns_ddf_status_e sns_dd_acc_lis3dsh_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_enable_md(
    sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_disable_md(
    sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_enable_dbt(
    sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_enable_dbt_int(
    sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_acc_lis3dsh_disable_dbt(
    sns_ddf_handle_t dd_handle);

static sns_ddf_status_e sns_dd_acc_lis3dsh_send_fifo_data(
    sns_ddf_handle_t dd_handle,
    uint16_t         num_samples,
    sns_ddf_time_t   timestamp);

/**
 * Accelerometer LIS3DSH ranges array for setting attribute
 */
const stm_lis3dsh_range lis3dsh_ranges[STM_LIS3DSH_RANGE_NUM]={
  STM_LIS3DSH_RANGE_2G,
  STM_LIS3DSH_RANGE_4G,
  STM_LIS3DSH_RANGE_6G,
  STM_LIS3DSH_RANGE_8G,
  STM_LIS3DSH_RANGE_16G};

/**
 * Accelerometer LIS3DSH sampling frequencies array for getting attribute(low pass)
 */
const sns_ddf_lowpass_freq_t lis3dsh_freqs[STM_LIS3DSH_ODR_NUM] = {
  FX_FLTTOFIX_Q16(1.5625),
  FX_FLTTOFIX_Q16(3.125),
  FX_FLTTOFIX_Q16(6.25),
  FX_FLTTOFIX_Q16(12.5),
  FX_FLTTOFIX_Q16(25),
  FX_FLTTOFIX_Q16(50),
  FX_FLTTOFIX_Q16(200),
  FX_FLTTOFIX_Q16(400),
  FX_FLTTOFIX_Q16(800)
};

/**
 * Accelerometer LIS3DSH sampling frequencies array for attribute(ODR)
 */
const sns_ddf_odr_t lis3dsh_odr[STM_LIS3DSH_ODR_NUM] = {
  3,
  6,
  12,
  25,
  50,
  100,
  400,
  800,
  1600
};

/**
 * Accelerometer LIS3DSH sampling frequencies array for setting internal data rate
 */
const stm_lis3dsh_odr lis3dsh_odr_setting[STM_LIS3DSH_ODR_NUM] = {
  STM_LIS3DSH_ODR3,
  STM_LIS3DSH_ODR6,
  STM_LIS3DSH_ODR12p5,
  STM_LIS3DSH_ODR25,
  STM_LIS3DSH_ODR50,
  STM_LIS3DSH_ODR100,
  STM_LIS3DSH_ODR400,
  STM_LIS3DSH_ODR800,
  STM_LIS3DSH_ODR1600};

/**
 * @brief Update only one part of one register in LIS3DSH
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  reg_addr    The address of LIS3DSH register to be updated
 * @param[in]  mask        The mask of this register to specify which part to be updated
 * @param[in]  new_value   The new content for the specified part of this register
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_update_onereg(
    sns_ddf_handle_t dd_handle,
    uint8_t reg_addr,
    uint8_t mask,
    uint8_t new_value)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  // read current value from this register
  status = sns_ddf_read_port(
      state->port_handle,
      reg_addr,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  // generate new value
  rw_buffer = (rw_buffer & (~mask)) | (new_value & mask);

  // write new value to this register
  status = sns_ddf_write_port(
      state->port_handle,
      reg_addr,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Find the matched internal ODR for desired ODR.

 *
 * @param[in]  desired_rate      New desired ODR.
 * @param[out] new_rate          Matched internal ODR.
 * @param[out] new_index         The index of matched internal ODR in lis3dsh_odr.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e  sns_dd_acc_lis3dsh_match_odr(
    sns_ddf_odr_t desired_rate,
    sns_ddf_odr_t *new_rate,
    uint32_t *new_index)
{
  uint8_t idx;

  if((0 == desired_rate) || (desired_rate > STM_LIS3DSH_MAX_ODR))
    return SNS_DDF_EINVALID_PARAM;

  for(idx=0; idx<STM_LIS3DSH_ODR_NUM; idx++)
  {
    if(desired_rate <= lis3dsh_odr[idx])
      break;
  }

  if (idx >= STM_LIS3DSH_ODR_NUM)
    return SNS_DDF_EINVALID_PARAM;

  *new_rate = lis3dsh_odr[idx];
  *new_index = idx;

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Reads FIFO_SRC register to extract the FIFO sample count
 *
 * @param[in]  state      The driver instance
 *
 * @return number of samples in FIFO
 */
static uint8_t sns_dd_acc_lis3dsh_get_fifo_sample_count(
  sns_dd_acc_lis3dsh_state_t* state)
{
  uint8_t fifo_count;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  sns_ddf_status_e status;
  
  if(1 == state->fifo_wmk)
  {
    fifo_count = 1;
  }
  else
  {
    status  = sns_ddf_read_port(state->port_handle,
                                STM_LIS3DSH_FIFO_SRC,
                                &rw_buffer,
                                1,
                                &rw_bytes);
    if((status != SNS_DDF_SUCCESS) || (rw_bytes != 1))
    {
      fifo_count = 0;
    }
    else
    {
      fifo_count = rw_buffer & 0x1F;

      // if it's overflow, add one more sample.
      if(rw_buffer & 0x40)
      {
        fifo_count++;
      }
    }
  }
  return fifo_count;
}

/**
 * @brief Triggers sending FIFO data if FIFO interrupt was missed
 *
 * @param[in]  state      Handle to a driver instance.
 *
 * @return none
 */
static void sns_dd_acc_lis3dsh_catch_missed_interrupt(
  sns_dd_acc_lis3dsh_state_t* state)
{
  uint8_t fifo_count = 0; 
 
  //do nothing if wmk == 1
  if(state->fifo_wmk == 1)
    return;

  fifo_count = sns_dd_acc_lis3dsh_get_fifo_sample_count(state);

  //1.use (fifo_wmk -1) instead of fifo_wmk, in case fifo int is high
  //when fifo_count < fifo_wmk, possible leftover issue from trigger_fifo
  //2.use while loop to make sure the condition finally become false,
  //in case new samples go into fifo while fifo reading.
  while (fifo_count >= (state->fifo_wmk - 1))
  {
    MSG_2(MSG_SSID_SNS, DBG_MED_PRIO, "missed_int - count(%d) wm(%d)", 
          fifo_count, state->fifo_wmk);
    sns_dd_acc_lis3dsh_send_fifo_data(state, fifo_count, sns_ddf_get_timestamp());

    fifo_count = sns_dd_acc_lis3dsh_get_fifo_sample_count(state);
  }
}

/**
 * @brief Check ZYXDA in STATUS register, return data ready flag.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[out] data_flag      data ready flag to set.
 *                            true:  a new set of data is available.
 *                            false: a new set of data is not yet available.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_is_dataready(
    sns_ddf_handle_t dd_handle,
    bool *data_flag)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  // read status register
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_STATUS_REG,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  if((rw_buffer&0x08) > 0)
    *data_flag = true;
  else
    *data_flag = false;

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Collects sample data for lis3dsh self test.
 *
 * @param[in]  dd_handle     Handle to a driver instance.
 * @param[out] data           Output data buffer to be filled in, should be set 0 before called.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_collect_testdata(
    sns_ddf_handle_t dd_handle,
    int16_t *data,
    uint8_t num_samples)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer[STM_LIS3DSH_NUM_READ_BYTES] = { 0 };
  int32_t average_data[STM_LIS3DSH_NUM_AXES] = {0};
  uint8_t rw_bytes = 0;
  bool data_rdy = false;
  uint8_t idx_i;

  //Collect data
  for(idx_i=0;idx_i<num_samples;idx_i++)
  {
    do{
      status = sns_dd_acc_lis3dsh_is_dataready(dd_handle,&data_rdy);
      if(status != SNS_DDF_SUCCESS)
        return status;
    }while(data_rdy == false);

    status = sns_ddf_read_port(
        state->port_handle,
        AUTO_INCREMENT | STM_LIS3DSH_OUT_X_L_A,
        rw_buffer,
        STM_LIS3DSH_NUM_READ_BYTES,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != STM_LIS3DSH_NUM_READ_BYTES)
      return SNS_DDF_EBUS;

    if(0==idx_i)
      continue;

    average_data[0] += (int16_t) (((rw_buffer[1]) << 8) | rw_buffer[0]);
    average_data[1] += (int16_t) (((rw_buffer[3]) << 8) | rw_buffer[2]);
    average_data[2] += (int16_t) (((rw_buffer[5]) << 8) | rw_buffer[4]);
  }

  for(idx_i=0;idx_i<STM_LIS3DSH_NUM_AXES;idx_i++)
    data[idx_i] = average_data[idx_i] / (num_samples - 1); //ignored first sample

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To check if the other sub device is in active state
 *
 * @param[in]  sub_dev_inx    - the sub device index
 * @param[in]  state               - Handle to a driver instance.
 *
 * @return true - one of the other sub device is in active state.
 */
static bool lis3dsh_is_active_other_sub_dev (sns_dd_acc_lis3dsh_state_t* state, uint32_t sub_dev_inx)
{
  uint32_t i, j;
  bool     return_val = false;
  stm_lis3dsh_sub_dev_s *sub_dev_slot_ptr = &state->sub_dev[0];

  for (i =0; i < LIS3DSH_MAX_SUB_DEV; i++)
  {
    if (i == sub_dev_inx) continue;
    for (j =0; (i < LIS3DSH_MAX_SUB_DEV) &&
               (j < sub_dev_slot_ptr[i].num_sensors) &&
               (j < ARR_SIZE(sub_dev_slot_ptr[i].sensors)); j++)
    {
      switch (sub_dev_slot_ptr[i].sensors[j])
      {
        case SNS_DDF_SENSOR_ACCEL:
          if (state->acc_cur_rate) return_val = true;
          i = LIS3DSH_MAX_SUB_DEV;
          break;
        case SNS_DDF_SENSOR_DOUBLETAP:
          if (state->dbt_cur_rate) return_val = true;
          i = LIS3DSH_MAX_SUB_DEV;
          break;
        default:
          break;
      }
    }
  }
  return return_val;
}

/**
 * @brief To search which sub device has a specific sensor.
 *
 * @param[in]  state               - Handle to a driver instance.
 * @param[in]  sensor            - the sensor type.
 * @param[out]  sub_dev_inx  - the index of sub_dev which contains this sensor.
 *
 * @return SNS_DDF_SUCCESS if this sensor is found. Otherwise return SNS_DDF_EFAIL.
 */
static sns_ddf_status_e lis3dsh_search_subdev_by_sensortype (
    sns_dd_acc_lis3dsh_state_t* state,
    sns_ddf_sensor_e sensor,
    uint32_t *sub_dev_inx)
{
  uint32_t i, j;
  stm_lis3dsh_sub_dev_s *sub_dev_slot_ptr;

  for (i =0; i < LIS3DSH_MAX_SUB_DEV; i++)
  {
    sub_dev_slot_ptr = &state->sub_dev[i];
    for (j =0; j < sub_dev_slot_ptr->num_sensors; j++)
    {
      if(sensor == sub_dev_slot_ptr->sensors[j])
      {
        *sub_dev_inx = i;
        return SNS_DDF_SUCCESS;
      }
    }
  }

  return SNS_DDF_EFAIL;
}

/**
 * @brief To enable LIS3DSH FIFO.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in] watermark      Watermark level.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_enable_fifo(
    sns_ddf_handle_t dd_handle,
    uint16_t watermark)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  LIS3DSH_MSG_1(HIGH, "lis3dsh_enable_fifo: get called. watermark=%u", watermark);

  //malloc memory for state->fifo_data.samples
  if(NULL == state->fifo_data.samples)
  {
    status = sns_ddf_malloc((void**)&(state->fifo_data.samples),
        STM_LIS3DSH_MAX_FIFO * STM_LIS3DSH_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));

    if(status != SNS_DDF_SUCCESS)
    {
      LIS3DSH_MSG_1(HIGH, "lis3dsh_enable_fifo: malloc fifo_data samples failed. status=%u", status);

      return status;
    }
  }

  //malloc memory for state->fifo_raw_buffer
  if(NULL == state->fifo_raw_buffer)
  {
    status = sns_ddf_malloc((void**)&(state->fifo_raw_buffer),
        STM_LIS3DSH_NUM_READ_BYTES * STM_LIS3DSH_MAX_FIFO * sizeof(uint8_t));
    if(status != SNS_DDF_SUCCESS)
    {
      LIS3DSH_MSG_1(HIGH, "lis3dsh_enable_fifo: malloc fifo_raw_buffer failed. status=%u", status);

      return status;
    }
  }

  if(watermark > 1)
  {
    // Configure accelerometer FIFO control registers to enable FIFO
    rw_buffer = 0x40 | (uint8_t)(watermark - 1);
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_FIFO_MODE,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

    rw_buffer = 0x0
      | (0<<7)           // force reboot
      | (1<<6)           // FIFO enable
      | (0<<5)           // Enable FIFO Watermark level use
      | (1<<4)           // ADD_INC bit
      | (0<<3)           // Enable FIFO Empty indication on int1.
      | (0<<2)           // FIFO Watermark interrupt on int1.
      | (0<<1)           // FIFO overrun interrupt on int1.
      | 0;               // BOOT interrupt on int2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FIFO,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

  }
  else if(state->fifo_enabled)
  {
    // Configure accelerometer FIFO control registers to disable FIFO
    rw_buffer = 0x0;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_FIFO_MODE,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

    rw_buffer = 0x0
      | (0<<7)           // force reboot
      | (0<<6)           // FIFO enable
      | (0<<5)           // Enable FIFO Watermark level use
      | (1<<4)           // ADD_INC bit
      | (0<<3)           // Enable FIFO Empty indication on int1.
      | (0<<2)           // FIFO Watermark interrupt on int1.
      | (0<<1)           // FIFO overrun interrupt on int1.
      | 0;               // BOOT interrupt on int2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FIFO,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;
  }

  state->fifo_wmk = watermark;
  state->fifo_enabled = true;
  state->last_timestamp = sns_ddf_get_timestamp();

  LIS3DSH_MSG_1(HIGH, "lis3dsh_enable_fifo: finished. watermark=%u", watermark);

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To disable LIS3DSH FIFO.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_disable_fifo(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  LIS3DSH_MSG_1(HIGH, "lis3dsh_disable_fifo: get called. current_watermark=%u", state->fifo_wmk);

  if(state->fifo_wmk > 1)
  {
    // Configure accelerometer FIFO control registers
    rw_buffer = 0x0;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_FIFO_MODE,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

    rw_buffer = 0x0
      | (0<<7)           // force reboot
      | (0<<6)           // FIFO enable
      | (0<<5)           // Enable FIFO Watermark level use
      | (1<<4)           // ADD_INC bit
      | (0<<3)           // Enable FIFO Empty indication on int1.
      | (0<<2)           // FIFO Watermark interrupt on int1.
      | (0<<1)           // FIFO overrun interrupt on int1.
      | 0;               // BOOT interrupt on int2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FIFO,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

  }

  state->fifo_wmk = 0;
  state->fifo_enabled = false;

  //free memory for state->fifo_data.samples
  if(NULL != state->fifo_data.samples)
  {
    status = sns_ddf_mfree(state->fifo_data.samples);
    if(status != SNS_DDF_SUCCESS)
    {
      LIS3DSH_MSG_1(HIGH, "lis3dsh_disable_fifo: mfree fifo_data samples failed. status=%u", status);

      return status;
    }

    state->fifo_data.samples = NULL;
  }

  //free memory for state->fifo_raw_buffer
  if(NULL != state->fifo_raw_buffer)
  {
    status = sns_ddf_mfree(state->fifo_raw_buffer);
    if(status != SNS_DDF_SUCCESS)
    {
      LIS3DSH_MSG_1(HIGH, "lis3dsh_disable_fifo: mfree fifo_raw_buffer failed. status=%u", status);

      return status;
    }

    state->fifo_raw_buffer = NULL;
  }

  LIS3DSH_MSG_1(HIGH, "lis3dsh_disable_fifo: finished. current_watermark=%u", state->fifo_wmk);

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To enable LIS3DSH FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_enable_fifo_int(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  if(false == state->fifo_enabled)
    return SNS_DDF_SUCCESS;

  if(state->fifo_enabled && state->fifo_int_enabled)
    return SNS_DDF_SUCCESS;

  if(state->fifo_wmk > 1)
  {
    // Configure accelerometer FIFO control registers
    rw_buffer = 0x0
      | (0<<7)           // force reboot
      | (1<<6)           // FIFO enable
      | (0<<5)           // Enable FIFO Watermark level use
      | (1<<4)           // ADD_INC bit
      | (0<<3)           // Enable FIFO Empty indication on int1.
      | (1<<2)           // FIFO Watermark interrupt on int1.
      | (1<<1)           // FIFO overrun interrupt on int1.
      | 0;               // BOOT interrupt on int2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FIFO,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

  }

  state->fifo_int_enabled = true;
  //state->last_timestamp = sns_ddf_get_timestamp();

  LIS3DSH_MSG_2(HIGH, "enable_fifo_int: md_enabled=%u, dbt_desired_rate=%u",
      state->md_enabled, state->dbt_desired_rate);

  if((false == state->md_enabled) && (0 == state->dbt_desired_rate))
  {
    rw_buffer = 0x68; //enable INT1, interrupt active high, signal pulsed.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_INT,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

    status =  sns_ddf_signal_register(
        state->gpio_num,
        state,
        &sns_dd_acc_lis3dsh_if,
        SNS_DDF_SIGNAL_IRQ_HIGH);

    state->int_registered = true;

    LIS3DSH_MSG_2(HIGH, "fifo int enabled: gpio_num=%u, status=%u",
        state->gpio_num, status);

    if(status != SNS_DDF_SUCCESS)
      return status;

  }
  
  //flush FIFO if FIFO interrupt is missed during int enable.  
  sns_dd_acc_lis3dsh_catch_missed_interrupt(state);

  //special case, watermark is 1, enable DRI instead of FIFO.
  if(1 == state->fifo_wmk)
  {
    rw_buffer = 0x80;
    status = sns_dd_acc_lis3dsh_update_onereg(
        dd_handle,
        STM_LIS3DSH_CTRL_INT,
        0x80,
        rw_buffer);
    if(status != SNS_DDF_SUCCESS)
      return status;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To disable LIS3DSH FIFO interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_disable_fifo_int(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;

  if((false == state->fifo_enabled) || (false == state->fifo_int_enabled))
    return SNS_DDF_SUCCESS;

  if(state->fifo_wmk > 1)
  {
    // Configure accelerometer FIFO control registers
    rw_buffer = 0x0
      | (0<<7)           // force reboot
      | (1<<6)           // FIFO enable
      | (0<<5)           // Enable FIFO Watermark level use
      | (1<<4)           // ADD_INC bit
      | (0<<3)           // Enable FIFO Empty indication on int1.
      | (0<<2)           // FIFO Watermark interrupt on int1.
      | (0<<1)           // FIFO overrun interrupt on int1.
      | 0;               // BOOT interrupt on int2.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FIFO,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;
  }
  else
  {
    //special case, if watermark is 1, clear DRI INT.
    rw_buffer = 0x00;
    status = sns_dd_acc_lis3dsh_update_onereg(
        dd_handle,
        STM_LIS3DSH_CTRL_INT,
        0x80,
        rw_buffer);
    if(status != SNS_DDF_SUCCESS)
      return status;
  }

  state->fifo_int_enabled = false;

  LIS3DSH_MSG_2(HIGH, "disable_fifo_int: md_enabled=%u, dbt_desired_rate=%u",
      state->md_enabled, state->dbt_desired_rate);

  if((false == state->md_enabled) && (0 == state->dbt_desired_rate) 
                                  && (true == state->int_registered))
  {
    //Don't disable INT register for FIFO INT.
    /*rw_buffer = 0x00; //disable INT1
      status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_INT,
      &rw_buffer,
      1,
      &rw_bytes);
      if(status != SNS_DDF_SUCCESS)
      return status;
      if(rw_bytes != 1)
      return SNS_DDF_EBUS;*/

    status = sns_ddf_signal_deregister(state->gpio_num);
    if(SNS_DDF_SUCCESS != status)
      return status;
    state->int_registered = false;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To send a double tap event data to SMGR.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_send_dbt_event(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer = 0;
  uint8_t rw_bytes = 0;
  uint32_t sub_dev_inx = 0;

  //read OUTS2 register to reset the interrupt
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_OUTS2,
      &rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;

  status = lis3dsh_search_subdev_by_sensortype(
      state,
      SNS_DDF_SENSOR_DOUBLETAP,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
    return status;

  LIS3DSH_MSG_2(HIGH, "handle_irq: doubletap: rw_buffer=%u, status=%u", rw_buffer, status);

  state->dbt_data.sensor = SNS_DDF_SENSOR_DOUBLETAP;
  state->dbt_data.status = SNS_DDF_SUCCESS;
  state->dbt_data.timestamp = sns_ddf_get_timestamp();

  //Fill out sensor output data and status
  state->dbt_data.samples[0].sample = SNS_DDF_TAP_DIR_Z;
  state->dbt_data.samples[0].status = SNS_DDF_SUCCESS;
  state->dbt_data.samples[1].sample = 0;
  state->dbt_data.samples[1].status = SNS_DDF_SUCCESS;
  state->dbt_data.samples[2].sample = 0;
  state->dbt_data.samples[2].status = SNS_DDF_SUCCESS;
  state->dbt_data.num_samples = STM_LIS3DSH_NUM_AXES;

  // double tap detected, send an event to SMGR
  status = sns_ddf_smgr_notify_data(
      state->sub_dev[sub_dev_inx].smgr_handle,
      &state->dbt_data,
      1);
  if(status != SNS_DDF_SUCCESS)
    return status;

  return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION:   sns_dd_accel_lis3dsh_parse_and_send_fifo_data
===========================================================================*/
/*!
  @brief Parses FIFO data and sends to SMGR
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_accel_lis3dsh_parse_and_send_fifo_data(
   sns_dd_acc_lis3dsh_state_t* state,
   sns_ddf_handle_t            smgr_handle,
   const uint8_t*              raw_buf_ptr,
   uint16_t                    num_samples
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t idx;
  // sensor coordinate x,y,z axis raw data sample
  static int16_t hw_d[STM_LIS3DSH_NUM_AXES];
  // SAE coordinate x,y,z axis data sample
  static q16_t sae_d[STM_LIS3DSH_NUM_AXES];

  for(idx=0; idx<num_samples; idx++)
  {
    //convert the raw data in read_buffer to X/Y/Z axis sensor data
    hw_d[0] = (int16_t) (((raw_buf_ptr[1 + idx*6]) << 8) | raw_buf_ptr[0 + idx*6]);
    hw_d[1] = (int16_t) (((raw_buf_ptr[3 + idx*6]) << 8) | raw_buf_ptr[2 + idx*6]);
    hw_d[2] = (int16_t) (((raw_buf_ptr[5 + idx*6]) << 8) | raw_buf_ptr[4 + idx*6]);

    //adjust sensor data per current sensitivity, and convert to Q16
    sae_d[0] = FX_FLTTOFIX_Q16(hw_d[0] * state->sstvt_adj * G/1000000);
    sae_d[1] = FX_FLTTOFIX_Q16(hw_d[1] * state->sstvt_adj * G/1000000);
    sae_d[2] = FX_FLTTOFIX_Q16(hw_d[2] * state->sstvt_adj * G/1000000);

    //map the sensor data to the phone's coordinates
    sns_ddf_map_axes(&state->axes_map, sae_d);

    //Fill out sensor output data and status
    state->fifo_data.samples[0 + idx*3].sample = sae_d[0];
    state->fifo_data.samples[0 + idx*3].status = state->fifo_data.status;
    state->fifo_data.samples[1 + idx*3].sample = sae_d[1];
    state->fifo_data.samples[1 + idx*3].status = state->fifo_data.status;
    state->fifo_data.samples[2 + idx*3].sample = sae_d[2];
    state->fifo_data.samples[2 + idx*3].status = state->fifo_data.status;

    /*LIS3DSH_MSG_3(HIGH, "LIS3DSH fifo_data=s0(%d), s1(%d), s2(%d)",
      FX_FIXTOFLT_Q16(sae_d[0]),
      FX_FIXTOFLT_Q16(sae_d[1]),
      FX_FIXTOFLT_Q16(sae_d[2]));*/
  }

  state->fifo_data.num_samples = STM_LIS3DSH_NUM_AXES * num_samples;

  // send fifo data to SMGR
  status = sns_ddf_smgr_notify_data(smgr_handle, &state->fifo_data, 1);
  if(status != SNS_DDF_SUCCESS)
    return status;

  LIS3DSH_MSG_1(HIGH, "parse_and_send: sent data successfully. fifo_data.num_samples=%u",
      state->fifo_data.num_samples);

  LIS3DSH_MSG_1(HIGH, "parse_and_send: end time=%u", sns_ddf_get_timestamp());

#if STM_LIS3DSH_LOGDATA
  sns_dd_acc_lis3dshaccel_log_fifo(&state->fifo_data);
#endif

  return SNS_DDF_SUCCESS;
}

/**
 * @brief To send FIFO data to SMGR, numbers of sample up to watermark level.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  num_samples Num of data samples in FIFO to read.
 * @param[in]  timestamp      The end timestamp.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_send_fifo_data(
    sns_ddf_handle_t dd_handle,
    uint16_t num_samples,
    sns_ddf_time_t    timestamp)
{
  sns_dd_acc_lis3dsh_state_t* state = 
    (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t read_count = 0;
  uint16_t num_invalid_samples;
  uint32_t sub_dev_inx = 0;
  uint8_t* raw_buf_ptr = state->fifo_raw_buffer;

  // read all FIFO data up to num_samples.
  status = sns_ddf_read_port(
      state->port_handle,
      AUTO_INCREMENT | STM_LIS3DSH_OUT_X_L_A,
      state->fifo_raw_buffer,
      STM_LIS3DSH_NUM_READ_BYTES * num_samples,
      &read_count);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(read_count != STM_LIS3DSH_NUM_READ_BYTES * num_samples)
  {
    LIS3DSH_MSG_2(HIGH, "send_fifo_data: raw data reading fails, num_samples_to_read=%u, actual_read_bytes=%u",
        num_samples, read_count);
    return SNS_DDF_EBUS;
  }

  /*if(1 == state->fifo_wmk)
    {
    sns_ddf_read_port(
    state->port_handle,
    AUTO_INCREMENT | STM_LIS3DSH_OUT_X_L_A,
    (uint8_t *)hw_d,
    STM_LIS3DSH_NUM_READ_BYTES,
    &read_count);
    }*/

  //search for accel sensor in sub devices
  status = lis3dsh_search_subdev_by_sensortype(
      state,
      SNS_DDF_SENSOR_ACCEL,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
  {
     LIS3DSH_MSG_1(HIGH, "sns_dd_acc_lis3dsh_send_fifo_data:lis3dsh_search_subdev_by_sensortype status==%u ",status );
    return status;
   }
   
  // fill out fifo_data structure
  if(0 == state->last_timestamp)
  {
    LIS3DSH_MSG_0(HIGH, "sns_dd_acc_lis3dsh_send_fifo_data:last_timestamp==0 ");
    return SNS_DDF_EFAIL;
  }
  
  state->fifo_data.sensor = SNS_DDF_SENSOR_ACCEL;
  state->fifo_data.end_timestamp = timestamp;
  state->fifo_data.timestamp = state->last_timestamp +
    (timestamp - state->last_timestamp)/num_samples;

  LIS3DSH_MSG_2(HIGH, "send_fifo_data: start_timestamp=%u, end_timestamp=%u",
      state->fifo_data.timestamp, state->fifo_data.end_timestamp);

  if ( (int32_t)(state->odr_settled_ts - state->fifo_data.timestamp) <= 0 )
  {
    /* all samples in FIFO produced after ODR change settled */
    num_invalid_samples = 0;
  }
  else if ( (int32_t)(state->odr_settled_ts - state->fifo_data.end_timestamp) > 0 )
  {
    /* all samples in FIFO produced before ODR change settled */
    num_invalid_samples = num_samples;
  }
  else /* some samples in FIFO produced before ODR change settled */
  {
    uint32_t time_interval = state->fifo_data.timestamp - state->last_timestamp;
    num_invalid_samples = 
      ((state->odr_settled_ts - state->fifo_data.timestamp) / time_interval) + 1;
    state->fifo_data.end_timestamp = 
      state->last_timestamp + (time_interval * num_invalid_samples);
    state->last_timestamp = state->fifo_data.end_timestamp;
  }

  if ( num_invalid_samples > 0 )
  {
    state->fifo_data.status      = SNS_DDF_EINVALID_DATA;
    state->fifo_data.num_samples = STM_LIS3DSH_NUM_AXES * num_invalid_samples;
    status = sns_dd_accel_lis3dsh_parse_and_send_fifo_data(
       state, state->sub_dev[sub_dev_inx].smgr_handle, raw_buf_ptr, num_invalid_samples);
    MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
          "send_fifo_data: #samples=%d #invalid=%d end_ts=%u", 
          num_samples, num_invalid_samples, state->fifo_data.end_timestamp);

    /* prepares to send remaining samples */
    num_samples -= num_invalid_samples;
    if ( num_samples > 0 )
    {
      raw_buf_ptr = &state->fifo_raw_buffer[state->fifo_data.num_samples<<1];
      state->fifo_data.end_timestamp = timestamp;
      state->fifo_data.timestamp     = state->last_timestamp +
        (timestamp - state->last_timestamp)/num_samples;
      MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
            "send_fifo_data: #valid=%d ts=%u end_ts=%u", 
            num_samples, state->fifo_data.timestamp, state->fifo_data.end_timestamp);
    }
  }
  if ( num_samples > 0 )
  {
    state->fifo_data.status = SNS_DDF_SUCCESS;
    state->fifo_data.num_samples = STM_LIS3DSH_NUM_AXES * num_samples;
    status |= sns_dd_accel_lis3dsh_parse_and_send_fifo_data(
       state, state->sub_dev[sub_dev_inx].smgr_handle, raw_buf_ptr, num_samples);
  }
  state->last_timestamp = timestamp;

  return status;
}

/**
 * @brief Resets the driver and device so they return to the state they were
 *        in after init() was called.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_reset(sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint32_t sub_dev_idx, i;
  stm_lis3dsh_sub_dev_s *sub_dev_slot_ptr;
  uint8_t rw_buffer[2];
  uint8_t rw_bytes = 0;

  sub_dev_idx = (uint32_t)dd_handle & (~DD_HANDLE_ALIGN_BITS);
  if ( sub_dev_idx < ARR_SIZE(state->sub_dev) )
  {
    sub_dev_slot_ptr = &state->sub_dev[sub_dev_idx];
  }
  else
  {
    return SNS_DDF_EFAIL;
  }

  if (!lis3dsh_is_active_other_sub_dev(state, sub_dev_idx))
  {
    LIS3DSH_MSG_0(HIGH, "lis3dsh_reset(), reset accel ctl registers.");
    // Reset driver state.
    // Configure accelerometer control registers
    rw_buffer[0] = 0x0
      | (0<<7)           // ODR3 bit
      | (0<<6)           // ODR2 bit
      | (0<<5)           // ODR1 bit
      | (0<<4)           // ODR0 bit
      | (0<<3)           // BDU bit
      | (0<<2)           // Z axis enable bit
      | (0<<1)           // Y axis enable bit
      | 0;               // X axis enable bit
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_ODR,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 1)
      return SNS_DDF_EBUS;

    rw_buffer[0] = 0x0
      | (0<<7)           // BW2 bit
      | (0<<6)           // BW1 bit
      | (0<<5)           // FSCALE2 bit
      | (0<<4)           // FSCALE1 bit
      | (1<<3)           // FSCALE0 bit
      | 0;               // SIM bit
    rw_buffer[1] = 0x0
      | (0<<7)           // BOOT bit
      | (0<<6)           // FIFO_EN bit
      | (0<<5)           // WTM_EN bit
      | (1<<4)           // ADD_INC bit
      | 0;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DSH_CTRL_FS,
        rw_buffer,
        2,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return status;
    if(rw_bytes != 2)
      return SNS_DDF_EBUS;
    state->range = STM_LIS3DSH_RANGE_4G;
    state->sstvt_adj = STM_LIS3DSH_SSTVT_4G;
  }

  for (i =0; i < sub_dev_slot_ptr->num_sensors; i++)
  {
    switch (sub_dev_slot_ptr->sensors[i])
    {
      case SNS_DDF_SENSOR_ACCEL:
        state->acc_cur_rate = 0;
        state->acc_desired_rate = 0;
        state->md_enabled = false;
        state->fifo_enabled = false;
        state->fifo_int_enabled = false;
        state->fifo_wmk = 0;
        state->last_timestamp = 0;
        break;
      case SNS_DDF_SENSOR_DOUBLETAP:
        state->dbt_desired_rate = 0;
        state->dbt_cur_rate = 0;
        break;
      default:
        break;
    }
  }
  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set data sampling rate operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  data_rate       Data sampling rate settings for LIS3DSH
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_set_datarate(
    sns_ddf_handle_t dd_handle,
    stm_lis3dsh_odr data_rate)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t i;
  uint8_t write_buffer = 0;
  uint8_t bytes_written = 0;
  uint32_t delay = 0;
  struct {
    uint32_t        odr;                /* in Hz */
    stm_lis3dsh_odr odr_reg_val;        /* register value */
    uint8_t         bw_reg_val;         /* register value for corresponding BW */
    uint32_t        samples_to_discard; /* samples to be discarded before ODR settled */
  } sns_dd_acc_lis3dsh_odr_config_table[] =
  {
    {    3, STM_LIS3DSH_ODR3,     STM_LIS3DSH_BW50,  3 },
    {    6, STM_LIS3DSH_ODR6,     STM_LIS3DSH_BW50,  3 },
    {   12, STM_LIS3DSH_ODR12p5,  STM_LIS3DSH_BW50,  3 },
    {   25, STM_LIS3DSH_ODR25,    STM_LIS3DSH_BW50,  3 },
    {   50, STM_LIS3DSH_ODR50,    STM_LIS3DSH_BW50,  3 },
    {  100, STM_LIS3DSH_ODR100,   STM_LIS3DSH_BW200, 4 },
    {  400, STM_LIS3DSH_ODR400,   STM_LIS3DSH_BW200, 5 },
    {  800, STM_LIS3DSH_ODR800,   STM_LIS3DSH_BW400, 5 },
    { 1600, STM_LIS3DSH_ODR1600,  STM_LIS3DSH_BW800, 5 },
  };

  for ( i=0; i<ARR_SIZE(sns_dd_acc_lis3dsh_odr_config_table); i++)
  {
    if (sns_dd_acc_lis3dsh_odr_config_table[i].odr_reg_val == data_rate)
    {
      break;
    }
  }
  if (i >= ARR_SIZE(sns_dd_acc_lis3dsh_odr_config_table))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  // Configure ODR in CTRL_ODR register
  write_buffer = ((uint8_t)data_rate)|0x07;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_ODR,
      &write_buffer,
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(bytes_written != 1)
    return SNS_DDF_EBUS;

  // Configure Bandwidth along with ODR.
  status = sns_dd_acc_lis3dsh_update_onereg(
      dd_handle,
      STM_LIS3DSH_CTRL_FS,
      0xC0,
      sns_dd_acc_lis3dsh_odr_config_table[i].bw_reg_val);
  if(status != SNS_DDF_SUCCESS)
    return status;

  delay = sns_dd_acc_lis3dsh_odr_config_table[i].samples_to_discard * 
          1000000 / sns_dd_acc_lis3dsh_odr_config_table[i].odr; /* in microseconds */

  state->odr_settled_ts = 
    sns_ddf_get_timestamp() + sns_ddf_convert_usec_to_tick(delay);

  MSG_2(MSG_SSID_SNS, DBG_HIGH_PRIO, 
        "set_datarate: delay_us=%d odr_settled_ts=%u", 
        delay, state->odr_settled_ts);

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Called in response to an motion detection interrupt for this driver.
 *
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[in]  gpio_num     GPIO number that generated the interrupt.
 * @param[in]  timestamp    Time at which ISR is called
 *
 * @return none
 * */

void sns_dd_acc_lis3dsh_handle_irq(
    sns_ddf_handle_t  handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
  sns_dd_acc_lis3dsh_state_t* state=NULL;
  sns_ddf_status_e status;
  uint8_t rw_buffer[2] = {0, 0};
  uint8_t rw_buffer_temp = 0;
  uint8_t rw_bytes = 0;
  uint32_t sub_dev_inx = 0;
  uint8_t total_in_fifo = 0;
  //flag for the while loop to guarantee no MD/DBT interrupt get dropped during fifo interrupt.
  bool run_again = true;
  //how many times fifo_read happens
  uint8_t fifo_run_cnt = 0;
  sns_ddf_time_t entry_time;

  if(handle==NULL)
  {
    LIS3DSH_MSG_0(HIGH, "handle_irq: Received Null Pointer.");
    return;
  }
  entry_time = sns_ddf_get_timestamp();

  LIS3DSH_MSG_1(HIGH, "handle_irq: CPU IRQ time=%u", timestamp);
  LIS3DSH_MSG_1(HIGH, "handle_irq: DD ISR start time=%u", entry_time);

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)handle & DD_HANDLE_ALIGN_BITS);

  if (gpio_num != state->gpio_num)
    return;

  status = lis3dsh_search_subdev_by_sensortype(
      state,
      SNS_DDF_SENSOR_ACCEL,
      &sub_dev_inx);
  if(status != SNS_DDF_SUCCESS)
    return;

  while(run_again)
  {
    run_again = false;

    if(fifo_run_cnt > 0)
    {
         timestamp = sns_ddf_get_timestamp();
    }

    // verify source of interrupts by reading INT1_SRC and FIFO_SRC.
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LIS3DSH_INT_STAT,
        rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return;
    if(rw_bytes != 1)
      return;

    status = sns_ddf_read_port(
        state->port_handle,
        STM_LIS3DSH_FIFO_SRC,
        rw_buffer+1,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
      return;
    if(rw_bytes != 1)
      return;

    LIS3DSH_MSG_2(HIGH, "handle_irq: INT1_SRC=%u, FIFO_SRC=%u", rw_buffer[0], rw_buffer[1]);

    //FIFO overflow interrupt handling.
    if((rw_buffer[1] & 0x40) && (fifo_run_cnt==0))
    {
      LIS3DSH_MSG_2(HIGH, "handle_irq: to send overflow event, watermark=%u, timestamp=%u",
          state->fifo_wmk, timestamp);

      // send a FIFO overflow event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_FIFO_OVERFLOW);
      if(status != SNS_DDF_SUCCESS)
        return;

      fifo_run_cnt ++;
      run_again = true;
    }

    //FIFO watermark interrupt(without overflow flag) handling
    if((0x0 == (rw_buffer[1] & 0x40)) && (rw_buffer[1] & 0x80))
    {
      total_in_fifo = rw_buffer[1] & 0x1F;
      LIS3DSH_MSG_1(HIGH, "handle_irq: WM INT happenes. total_in_fifo=%u", total_in_fifo);

      //if there is no data in FIFO.
      if(0 == total_in_fifo)
        return;

      // send a FIFO Watermark event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_FIFO_WM_INT);
      if(status != SNS_DDF_SUCCESS)
        return;

      // if more samples than fifo_wmk, recalcuate the end timestamp of last sample.
      if((total_in_fifo > state->fifo_wmk) && (!fifo_run_cnt))
      {
        sns_ddf_time_t interrupt_timestamp = timestamp;
        timestamp = state->last_timestamp +
                    ((timestamp - state->last_timestamp)/state->fifo_wmk) * total_in_fifo;

        if(timestamp > entry_time)
          timestamp = entry_time;

        MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, 
              "handle_irq - #in_fifo=%u interrupt_ts=%u recomputed_ts=%u", 
              total_in_fifo, interrupt_timestamp, timestamp);
      }

      LIS3DSH_MSG_2(HIGH, "handle_irq: to call send_fifo_data, watermark=%u, real_timestamp=%u",
          state->fifo_wmk, timestamp);

      // send all data samples in FIFO to SMGR.
      status = sns_dd_acc_lis3dsh_send_fifo_data(state,
          total_in_fifo,
          timestamp);
      if(status != SNS_DDF_SUCCESS)
        return;

      fifo_run_cnt ++;
      run_again = true;
    }

    //double tap interrupt handling.
    if((rw_buffer[0] & 0x04))
    {
      status = sns_dd_acc_lis3dsh_send_dbt_event(state);
      if(status != SNS_DDF_SUCCESS)
        return;

      //run_again = true;
    }

    //motion detection interrupt handling.
    if((rw_buffer[0] & 0x08))
    {
      //read OUTS1 register to reset the interrupt
      sns_ddf_read_port(
          state->port_handle,
          STM_LIS3DSH_OUTS1,
          &rw_buffer_temp,
          1,
          &rw_bytes);

      // motion detected, send an event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_MOTION);
      if(status != SNS_DDF_SUCCESS)
        return;

      //run_again = true;
    }

    //special case, FIFO DRI interrupt handling for watermark 1.
    if((rw_buffer[0] & 0x01) && (state->fifo_enabled)
      && (state->fifo_int_enabled) && (1==state->fifo_wmk))
    {
      // send a FIFO Watermark event to SMGR
      status = sns_ddf_smgr_notify_event(
          state->sub_dev[sub_dev_inx].smgr_handle,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_EVENT_FIFO_WM_INT);
      if(status != SNS_DDF_SUCCESS)
        return;

      LIS3DSH_MSG_2(HIGH, "handle_irq: to call send_fifo_data, watermark=%u, timestamp=%u",
          state->fifo_wmk, timestamp);

      status = sns_dd_acc_lis3dsh_send_fifo_data(state,
          state->fifo_wmk,
          timestamp);
      if(status != SNS_DDF_SUCCESS)
        return;

      //run_again = true;
    }

  }
/*
  //test only start
  status = sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_INT_STAT,
      rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return;
  if(rw_bytes != 1)
    return;

  status = sns_ddf_read_port(
      state->port_handle,
      STM_LIS3DSH_FIFO_SRC,
      rw_buffer+1,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return;
  if(rw_bytes != 1)
    return;

	LIS3DSH_MSG_2(HIGH, "handle_irq END: INT1_SRC=%u, FIFO_SRC=%u", rw_buffer[0], rw_buffer[1]);
  //test only end

  LIS3DSH_MSG_1(HIGH, "handle_irq: DD ISR end time=%u", sns_ddf_get_timestamp());
 */
}

/**
 * @brief Turns off double tap virtual sensor, handles driver state and concurrencies.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_turnoff_dbt(
    sns_ddf_handle_t dd_handle)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  sns_ddf_odr_t new_rate;
  uint32_t new_rate_index;

  //disable dbt sensor.
  status = sns_dd_acc_lis3dsh_disable_dbt(state);
  LIS3DSH_MSG_1(HIGH,"turnoff_dbt(): disable dbt=%u",status);
  if(SNS_DDF_SUCCESS != status)
    return status;

  state->dbt_desired_rate = 0;
  state->dbt_cur_rate = 0;

  //double tap is being closed, but accel streaming request is still valid.
  if(state->acc_desired_rate > 0)
  {
    //find the matched internal ODR for desired ODR
    status =  sns_dd_acc_lis3dsh_match_odr(state->acc_desired_rate,
        &new_rate,
        &new_rate_index);
    if(SNS_DDF_SUCCESS != status)
      return SNS_DDF_EINVALID_PARAM;

    LIS3DSH_MSG_2(HIGH,"turnoff_dbt set_power(): new_rate=%u, acc_cur_rate=%u", new_rate ,state->acc_cur_rate);
    if(new_rate != state->acc_cur_rate)
    {
      status = sns_dd_acc_lis3dsh_set_datarate(state,
          lis3dsh_odr_setting[new_rate_index]);
      if(SNS_DDF_SUCCESS == status)
      {
        state->acc_cur_rate = new_rate;

        sns_ddf_smgr_notify_event(state->sub_dev[0].smgr_handle,
            SNS_DDF_SENSOR_ACCEL,
            SNS_DDF_EVENT_ODR_CHANGED);
      }
    }
    else
    {
      status = SNS_DDF_SUCCESS;
    }
    return status;
  }

  //double tap is being closed, but motion detection is still enabled, then reset motion detection ODR.
  if(true == state->md_enabled)
  {
    status = sns_dd_acc_lis3dsh_enable_md(state);
    if(status != SNS_DDF_SUCCESS)
      return status;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set power state operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  power_state    Power state to be set.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_set_power(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint32_t sub_dev_idx,
    sns_ddf_powerstate_e power_state)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t write_buffer = 0;
  uint8_t bytes_written = 0;
  sns_ddf_sensor_e ddf_sensor_type;

  LIS3DSH_MSG_1(HIGH,"set_power(): dd_handle=%u", dd_handle);

  if(SNS_DDF_POWERSTATE_LOWPOWER == power_state)
  {
    state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    ddf_sensor_type = state->sub_dev[sub_dev_idx].sensors[0]; // '0' --> Primary
    LIS3DSH_MSG_1(HIGH,"set_power(): ddf_sensor_type=%u",ddf_sensor_type);

    if(ddf_sensor_type == SNS_DDF_SENSOR_ACCEL) {
      switch(sensor) {
        case SNS_DDF_SENSOR_TEMP:  // secondary : ignoring for now
          break;
        case SNS_DDF_SENSOR__ALL:
          // TODO: need to include secondary (above) under this
          /* falling through... */
        case SNS_DDF_SENSOR_ACCEL: // primary
          state->acc_desired_rate = 0;
          break;
        default:
          return SNS_DDF_EINVALID_PARAM;
          break;
      }
    } else if(ddf_sensor_type == SNS_DDF_SENSOR_DOUBLETAP) {
      switch(sensor) {
        case SNS_DDF_SENSOR_SINGLETAP: // secondary : ignoring for now
          break;
        case SNS_DDF_SENSOR__ALL:
          // TODO: need to include secondary (above) under this
          /* falling through... */
        case SNS_DDF_SENSOR_DOUBLETAP: // primary

          LIS3DSH_MSG_1(HIGH,"set_power(): disable dbt. Desired_rate=%u",state->dbt_desired_rate);
          //dbt_desired_rate and cur_rate should be 0 already.
          if((0!=state->dbt_desired_rate)||(0!=state->dbt_cur_rate))
          {
            //turn off dbt again, and reset desired_rate and cur_rate to 0.
            status = sns_dd_acc_lis3dsh_turnoff_dbt(state);
            if(SNS_DDF_SUCCESS != status)
              return status;
          }

          break;
        default:
          return SNS_DDF_EINVALID_PARAM;
          break;
      }
    } else {
      /* should never be here... */
      return SNS_DDF_EFAIL;
    }
    LIS3DSH_MSG_2(HIGH,"set_power(): acc_desired_rate=%u, dbt_desired_rate=%u",state->acc_desired_rate,state->dbt_desired_rate);

    //nobody is going to use accel, turn off the sensor
    if((0 == state->dbt_desired_rate) && (0 == state->acc_desired_rate) && (false == state->md_enabled))
    {
      LIS3DSH_MSG_0(HIGH, "lis3dsh_setpower(), turn off the sensor.");
      //turn off the sensor
      // Configure ODR of control register 1 to enter power down mode.
      write_buffer = 0x0
        | (0<<7)             // ODR3 bit
        | (0<<6)             // ODR2 bit
        | (0<<5)             // ODR1 bit
        | (0<<4)             // ODR0 bit
        | (0<<3)             // BDU bit
        | (0<<2)             // Z axis enable bit
        | (0<<1)             // Y axis enable bit
        | 0;                 // X axis enable bit

      status = sns_ddf_write_port(
          state->port_handle,
          STM_LIS3DSH_CTRL_ODR,
          &write_buffer,
          1,
          &bytes_written);
      if(status != SNS_DDF_SUCCESS)
        return status;
      if(bytes_written != 1)
        return SNS_DDF_EBUS;

      //set current accel data rate to zero.
      state->acc_cur_rate = 0;
    }

  }
  else if(SNS_DDF_POWERSTATE_ACTIVE == power_state)
  {
    LIS3DSH_MSG_2(HIGH,"set_power(active): acc_desired_rate=%u, dbt_desired=%u",
        state->acc_desired_rate, state->dbt_desired_rate);
    ///sns_ddf_delay(2000000);
    // call sns_dd_acc_lis3dsh_set_datarate to enter ACTIVE mode, set default 100Hz.
    if((state->acc_desired_rate == 0) && (state->dbt_desired_rate == 0) && (false == state->md_enabled))
    {
      status = sns_dd_acc_lis3dsh_set_datarate(state, STM_LIS3DSH_ODR100);
      if(status != SNS_DDF_SUCCESS)
        return status;

      state->acc_cur_rate= 100;
      state->dbt_cur_rate = 0;
    }

    state->sub_dev[sub_dev_idx].powerstate = SNS_DDF_POWERSTATE_ACTIVE;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Performs a set dynamic range operation
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  range          Dynamic range settings for LIS3DSH
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_set_range(
    sns_ddf_handle_t dd_handle,
    stm_lis3dsh_range range)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t write_buffer = 0;
//  uint8_t bytes_written = 0;
  stm_lis3dsh_sstvt sstvt;

  //set sensor sensitivity for data conversion
  switch (range) {
    case STM_LIS3DSH_RANGE_2G:
      sstvt = STM_LIS3DSH_SSTVT_2G;
      break;
    case STM_LIS3DSH_RANGE_4G:
      sstvt = STM_LIS3DSH_SSTVT_4G;
      break;
    case STM_LIS3DSH_RANGE_6G:
      sstvt = STM_LIS3DSH_SSTVT_6G;
      break;
    case STM_LIS3DSH_RANGE_8G:
      sstvt = STM_LIS3DSH_SSTVT_8G;
      break;
    case STM_LIS3DSH_RANGE_16G:
      sstvt = STM_LIS3DSH_SSTVT_16G;
      break;
    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  // Configure CTRL_FS register
  write_buffer = ((uint8_t)range)|0x00;
  status = sns_dd_acc_lis3dsh_update_onereg(
      state,
      STM_LIS3DSH_CTRL_FS,
      0x38,
      write_buffer);
  if(status != SNS_DDF_SUCCESS)
    return status;

  state->range = range;
  state->sstvt_adj = sstvt;


  return SNS_DDF_SUCCESS;
}

#if STM_LIS3DSH_LOGDATA

/*===========================================================================
FUNCTION:   sns_dd_acc_lis3dshaccel_log_data
===========================================================================*/
/*!
  @brief log accel sensor data

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
void sns_dd_acc_lis3dshaccel_log_data(sns_ddf_sensor_data_s *accel_data_ptr)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s),
      (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = accel_data_ptr->timestamp;

    //Log the sensor data
    log_struct_ptr->num_data_types = STM_LIS3DSH_NUM_AXES;
    log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
        log_struct_ptr);
  }
}

/*===========================================================================
FUNCTION:   sns_dd_acc_lis3dshaccel_log_fifo
===========================================================================*/
/*!
  @brief log accel sensor data in fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
void sns_dd_acc_lis3dshaccel_log_fifo(sns_ddf_sensor_data_s *accel_data_ptr)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;
  uint16 idx =0 ;

  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
      sizeof(sns_log_sensor_data_pkt_s) + (accel_data_ptr->num_samples -1)*sizeof(int32_t),
      (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = accel_data_ptr->timestamp;
    log_struct_ptr->end_timestamp = accel_data_ptr->end_timestamp;

    log_struct_ptr->num_data_types = STM_LIS3DSH_NUM_AXES;
    log_struct_ptr->num_samples = accel_data_ptr->num_samples / STM_LIS3DSH_NUM_AXES;

    //Log the sensor fifo data
    log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

    for(idx=0; idx<accel_data_ptr->num_samples; idx++)
    {
      log_struct_ptr->samples[idx]  = accel_data_ptr->samples[idx].sample;
    }

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
        log_struct_ptr);
  }
}

#endif

/**
 * @brief Retrieves a single set of sensor data from lis3dsh.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
  uint8_t read_buffer[STM_LIS3DSH_NUM_READ_BYTES] = { 0,0,0,0,0,0 };
  uint8_t read_count = 0;
  sns_ddf_time_t timestamp;
  sns_ddf_status_e status;
  sns_ddf_sensor_data_s *data_ptr;
  sns_dd_acc_lis3dsh_state_t *state;
  // sensor coordinate x,y,z axis raw hardware data
  int16_t hw_d[STM_LIS3DSH_NUM_AXES] = { 0, 0, 0 };
  // SAE coordinate x,y,z axis data
  q16_t sae_d[STM_LIS3DSH_NUM_AXES] = {0, 0, 0 };

  state = (sns_dd_acc_lis3dsh_state_t*)((uint32_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  // This is a synchronous driver, so try to read data now.
  status = sns_ddf_read_port(
      state->port_handle,
      AUTO_INCREMENT | STM_LIS3DSH_OUT_X_L_A,
      read_buffer,
      STM_LIS3DSH_NUM_READ_BYTES,
      &read_count);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(read_count != STM_LIS3DSH_NUM_READ_BYTES)
    return SNS_DDF_EBUS;

  // get current time stamp
  timestamp = sns_ddf_get_timestamp();

  //convert the raw data in read_buffer to X/Y/Z axis sensor data
  hw_d[0] = (int16_t) (((read_buffer[1]) << 8) | read_buffer[0]);
  hw_d[1] = (int16_t) (((read_buffer[3]) << 8) | read_buffer[2]);
  hw_d[2] = (int16_t) (((read_buffer[5]) << 8) | read_buffer[4]);

  //adjust sensor data per current sensitivity, and convert to Q16
  sae_d[0] = FX_FLTTOFIX_Q16(hw_d[0] * state->sstvt_adj * G/1000000);
  sae_d[1] = FX_FLTTOFIX_Q16(hw_d[1] * state->sstvt_adj * G/1000000);
  sae_d[2] = FX_FLTTOFIX_Q16(hw_d[2] * state->sstvt_adj * G/1000000);

  //map the sensor data to the phone's coordinates
  sns_ddf_map_axes(&state->axes_map, sae_d);

  //allocate memory for sns_ddf_sensor_data_s data structure
  data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_data_s));
  if(NULL == data_ptr)
  {
    return SNS_DDF_ENOMEM;
  }
  *data = data_ptr;

  data_ptr->sensor = sensors[0];
  data_ptr->status = SNS_DDF_SUCCESS;
  data_ptr->timestamp = timestamp;

  //allocate memory for data samples.
  data_ptr->samples = sns_ddf_memhandler_malloc(memhandler, STM_LIS3DSH_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
  if(NULL == data_ptr->samples)
  {
    return SNS_DDF_ENOMEM;
  }

  //Fill out sensor output data and status
  //Axis and polarity conversion are configured in the header file.
  data_ptr->samples[0].sample = sae_d[0];
  data_ptr->samples[1].sample = sae_d[1];
  data_ptr->samples[2].sample = sae_d[2];
  data_ptr->num_samples = STM_LIS3DSH_NUM_AXES;

  // If the timer is still pending, invalidate the data
  if((int32_t)(state->odr_settled_ts - data_ptr->timestamp) > 0)
  {
    LIS3DSH_MSG_1(HIGH, "LIS3DSH - Invalidating sample at time: %u", data_ptr->timestamp);
    // Note: the status values for each samples[i] is UNUSED
    data_ptr->samples[0].status = SNS_DDF_EINVALID_DATA;
    data_ptr->samples[1].status = SNS_DDF_EINVALID_DATA;
    data_ptr->samples[2].status = SNS_DDF_EINVALID_DATA;
    data_ptr->status = SNS_DDF_EINVALID_DATA;
  }
  else
  {
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
  }

  LIS3DSH_MSG_3(HIGH, "LIS3DSH time=%u \tget_data=s0(%d), s1(%d)",
      data_ptr->timestamp, data_ptr->samples[0].sample, data_ptr->samples[1].sample);
  /*
     LIS3DSH_MSG_3(HIGH, "LIS3DSH get_data=s0(%d), s1(%d), s2(%d)",
     data_ptr->samples[0].sample, data_ptr->samples[1].sample,
     data_ptr->samples[2].sample);
     */
#if STM_LIS3DSH_LOGDATA
  sns_dd_acc_lis3dshaccel_log_data(data_ptr);
#endif

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Sets a lis3dsh sensor attribute to a specific value.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
  sns_dd_acc_lis3dsh_state_t* state;
  sns_ddf_status_e status;
  uint32_t sub_dev_idx;

  if (dd_handle == NULL || value == NULL)
    return SNS_DDF_EINVALID_PARAM;

  sub_dev_idx = (uint32_t)dd_handle & (~DD_HANDLE_ALIGN_BITS);
  state = (sns_dd_acc_lis3dsh_state_t*)((uint32_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      {
        sns_ddf_powerstate_e* power_state = value;
        return sns_dd_acc_lis3dsh_set_power(dd_handle, sensor, sub_dev_idx, *power_state);
      }

    case SNS_DDF_ATTRIB_RANGE:
      {
        LIS3DSH_MSG_1(HIGH, "ATTRIB range: sensor=%u", sensor);
        if(sensor == SNS_DDF_SENSOR_ACCEL)
        {
          uint32_t* range_idx = value;
          stm_lis3dsh_range range = lis3dsh_ranges[*range_idx];
          if(range > STM_LIS3DSH_MAX_RANGE)
            return SNS_DDF_EINVALID_PARAM;

          return sns_dd_acc_lis3dsh_set_range(dd_handle, range);
        }
        else
        {
          return SNS_DDF_EINVALID_ATTR;
        }
        break;
      }

    case SNS_DDF_ATTRIB_ODR:
      {
        sns_ddf_odr_t new_rate;
        uint32_t new_rate_index;

        LIS3DSH_MSG_3(HIGH, "LIS3DSH set ODR(%d), sensor(%d), dd_handle(%d)", *(sns_ddf_odr_t*)value, sensor, dd_handle);
        if(SNS_DDF_SENSOR_ACCEL == sensor)
        {
          if (*(sns_ddf_odr_t*)value > 0)
          {
          //find the matched internal ODR for desired ODR
          status =  sns_dd_acc_lis3dsh_match_odr(*(sns_ddf_odr_t*)value,
              &new_rate,
              &new_rate_index);
          if(SNS_DDF_SUCCESS != status)
            return SNS_DDF_EINVALID_PARAM;

          if((new_rate != state->acc_cur_rate) && (0 == state->dbt_desired_rate))
          {
            LIS3DSH_MSG_3(HIGH, "LIS3DSH set_datarate(%d), new_rate(%d), new_rate_index(%d)", *(sns_ddf_odr_t*)value, new_rate, new_rate_index);
            ///                  sns_ddf_delay(2000000);
            status = sns_dd_acc_lis3dsh_set_datarate(state,
                lis3dsh_odr_setting[new_rate_index]);
            if(SNS_DDF_SUCCESS == status)
            {
              state->acc_cur_rate = new_rate;
              state->acc_desired_rate= *(sns_ddf_odr_t*)value;
              return status;
            }
            else
            {
              return status;
            }

          }
          else
          {
            LIS3DSH_MSG_3(HIGH, "LIS3DSH  -- else -- set_datarate(%d), new_rate(%d), new_rate_index(%d)", *(sns_ddf_odr_t*)value, new_rate, new_rate_index);
            state->acc_desired_rate = *(sns_ddf_odr_t*)value;
            return SNS_DDF_SUCCESS;
            }
          }
          else
          {
            state->acc_desired_rate = *(sns_ddf_odr_t*)value;
          }
        }
        else if(SNS_DDF_SENSOR_DOUBLETAP == sensor)
        {
          //uint16_t old_acc_desired_rate;
          uint32_t sub_dev_inx = 0;
          new_rate = *(sns_ddf_odr_t*)value;
          LIS3DSH_MSG_2(HIGH, "LIS3DSH set_datarate(%d), new_rate(%d)", *(sns_ddf_odr_t*)value, new_rate);
          //sns_ddf_delay(2000000);

          //enable double tap.
          if(new_rate > 0)
          {
            status = sns_dd_acc_lis3dsh_enable_dbt(state);
            if(SNS_DDF_SUCCESS != status)
              return status;

            state->dbt_desired_rate = new_rate;
            state->dbt_cur_rate = LIS3DSH_DBT_ODR;

            //if ODR is different, notify SMGR.
            if(STM_LIS3DSH_DBT_ODR != state->acc_cur_rate)
            {
              LIS3DSH_MSG_1(HIGH, "--- inside before: acc_curr_rate=%u", state->acc_cur_rate);
              //sns_ddf_delay(2000000);

              state->acc_cur_rate = STM_LIS3DSH_DBT_ODR;

              if((state->acc_desired_rate > 0) && (state->acc_desired_rate != STM_LIS3DSH_DBT_ODR))
              {
                status = lis3dsh_search_subdev_by_sensortype(
                    state,
                    SNS_DDF_SENSOR_ACCEL,
                    &sub_dev_inx);
                if(status != SNS_DDF_SUCCESS)
                  return status;

                sns_ddf_smgr_notify_event(state->sub_dev[sub_dev_inx].smgr_handle,
                    SNS_DDF_SENSOR_ACCEL,
                    SNS_DDF_EVENT_ODR_CHANGED);
              }
            }

            //reset MD setting if MD is enabled.
            if(true == state->md_enabled)
            {
              status = sns_dd_acc_lis3dsh_enable_md(state);
              if(status != SNS_DDF_SUCCESS)
                return status;
            }

            LIS3DSH_MSG_2(HIGH, "LIS3DSH --finish-- set_datarate(%d), new_rate(%d)", *(sns_ddf_odr_t*)value, new_rate);

          }else if(0==new_rate)
          {
            status = sns_dd_acc_lis3dsh_turnoff_dbt(state);
            if(SNS_DDF_SUCCESS != status)
              return status;
          }

        }
        else
        {
          return SNS_DDF_EINVALID_PARAM;
        }
        break;
      } // ATTRIB_ODR

    case SNS_DDF_ATTRIB_MOTION_DETECT:
      {
        sns_ddf_motion_detect_enable_t* flag = value;

        if(!sns_ddf_signal_irq_enabled())
          return SNS_DDF_EFAIL;

        if(*flag)
        {
          //enable motion detection
          LIS3DSH_MSG_1(HIGH, "+++ enable MD: sensor=%d", sensor);

          return sns_dd_acc_lis3dsh_enable_md(state);
        }else{
          //disable motion detection
          LIS3DSH_MSG_1(HIGH, "--- disable MD: sensor=%d", sensor);

          return sns_dd_acc_lis3dsh_disable_md(state);
        }
        break;
      }

    case SNS_DDF_ATTRIB_FIFO:
      {
        sns_ddf_fifo_set_s* fifo = value;

        if(0 == fifo->watermark)
        {
          return sns_dd_acc_lis3dsh_disable_fifo(state);
        }else if(fifo->watermark <= STM_LIS3DSH_MAX_FIFO){
          //accel should be up and running.
          if(0 == state->acc_cur_rate)
            return SNS_DDF_EFAIL;

          return sns_dd_acc_lis3dsh_enable_fifo(state, fifo->watermark);
        }else{
          return SNS_DDF_EINVALID_ATTR;
        }

        break;
      }

    case SNS_DDF_ATTRIB_IO_REGISTER:
      {
        uint8_t bytes_written;
        sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) value;

        if ((uint32_t)(io_reg_in->address_ptr) > 0xFFFF)
        {
          return SNS_DDF_EINVALID_PARAM;
        }
        if (io_reg_in->length > 0xFF)
        {
          return SNS_DDF_EINVALID_PARAM;
        }

        //sns_ddf_status_e status;
        status = sns_ddf_write_port(
            state->port_handle,
            (uint16_t) ((uint32_t)io_reg_in->address_ptr),
            (uint8_t*) io_reg_in->data,
            (uint8_t) io_reg_in->length,
            &bytes_written);

        return status;
      }

    default:
      return SNS_DDF_EINVALID_ATTR;
  } //switch()

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Callback function to report inertial SW self test and bias calibration results.
 *
 * @param[in]  handle    Handle to a driver instance.
 * @param[in]  sensor    Sensor type.
 * @param[in]  status    Inertial SW self test result.
 * @param[in]  err       Device error code.
 * @param[in]  biases    Sensor biases after offset calibration.
 *
 * @return No return;
 */
static void sns_dd_acc_lis3dsh_SWtest_notify(
    sns_ddf_handle_t handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_status_e status,
    uint32_t err,
    q16_t* biases)
{
  sns_dd_acc_lis3dsh_state_t* state = handle;
  q16_t* biases_ptr;
  uint8_t i;

  LIS3DSH_MSG_3(HIGH, "LIS3DSH, self-test notify begin: Sensor(%d), status(%d), err(%d)",
      sensor, status, err);
  sns_ddf_smgr_notify_test_complete(
      state->sub_dev[0].smgr_handle,
      sensor,
      status,
      err);

  if(status == SNS_DDF_SUCCESS)
  {
    biases_ptr = state->biases;

    for(i = 0; i < STM_LIS3DSH_NUM_AXES; i++)
    {
      biases_ptr[i] = biases[i];
    }
    LIS3DSH_MSG_3(HIGH, "LIS3DSH, self-test notify bias: biasX(%d), biasY(%d), biasZ(%d)",
        biases_ptr[0], biases_ptr[1], biases_ptr[2]);
    LIS3DSH_MSG_3(HIGH, "LIS3DSH, self-test notify event: sensor(%d), SNS_DDF_EVENT_BIAS_READY(%d), rsvd(%d)",
        sensor, SNS_DDF_EVENT_BIAS_READY, 0);
    sns_ddf_smgr_notify_event(
        state->sub_dev[0].smgr_handle,
        sensor,
        SNS_DDF_EVENT_BIAS_READY);
  }

}

/**
 * @brief Runs lis3dsh inertial software self test and bias calibration.
 *
 * @param[in]  dd_handle   Handle to a driver instance.
 * @param[in]  sensor      Sensor type.
 *
 * @return SNS_DDF_PENDING if this SW self test is prepared to be performed later; SNS_DDF_EFAIL
 *         if SW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_SW_selftest(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor
    )
{
  sns_ddf_status_e status;
  sns_dd_inertial_test_config_s test_config;
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  uint8_t rw_buffer[2];
  uint8_t rw_bytes = 0;

  LIS3DSH_MSG_3(HIGH, "LIS3DSH sw self test sensor (%d), (%d), (%d)",sensor, 0, 0);
  test_config.sample_rate = 15;
  test_config.num_samples = 64;
  test_config.max_variance = FX_FLTTOFIX_Q16(88);                                       // max variance
  test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(STM_LIS3DSH_SWST_MAX_BIAS * G/1000); // x-axis max bias
  test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(STM_LIS3DSH_SWST_MAX_BIAS * G/1000); // y-axis max bias
  test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(STM_LIS3DSH_SWST_MAX_BIAS * G/1000); // z-axis max bias

  // reset accel to power down mode
  rw_buffer[0] = 0x0
    | (0<<7)           // ODR3 bit
    | (0<<6)           // ODR2 bit
    | (0<<5)           // ODR1 bit
    | (0<<4)           // ODR0 bit
    | (0<<3)           // BDU bit
    | (1<<2)           // Z axis enable bit
    | (1<<1)           // Y axis enable bit
    | 1;               // X axis enable bit
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_ODR,
      rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS)
    return status;
  if(rw_bytes != 1)
    return SNS_DDF_EBUS;

  status = sns_dd_acc_lis3dsh_set_datarate(dd_handle, STM_LIS3DSH_ODR100);
  if(SNS_DDF_SUCCESS != status) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH sw self test sensor (%d) set datarate failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  // wait 3 ODR.
  sns_ddf_delay(30000);

  status = sns_dd_inertial_test_run(
      sensor,
      dd_handle,
      &sns_dd_acc_lis3dsh_if,
      &test_config,
      &sns_dd_acc_lis3dsh_SWtest_notify);

  if(SNS_DDF_SUCCESS != status) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH sw self test sensor (%d) inertial test run failed (%d),(%d)",sensor, 0, 0);
    return status;
  }

  LIS3DSH_MSG_3(HIGH, "LIS3DSH sw self test sensor (%d) success - returning SNS_DDF_PENDING (%d),(%d)",sensor, 0, 0);
  return SNS_DDF_PENDING;
}
/**
 * @brief Runs lis3dsh bias test as part of OEM selftest and returns the test result.
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[out] err          update err with error code apart from return status.
 *
 * @return SNS_DDF_SUCCESS if this bias test was done successfully; SNS_DDF_EFAIL
 *         if bias test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_biastest(
    sns_ddf_handle_t  dd_handle, sns_ddf_sensor_e  sensor, uint32_t*   err)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;

  uint8_t rw_buffer[3] = {0, 0, 0};
  uint8_t rw_bytes = 0;

  int32_t axis_validated = 0x00;
  int32_t axis_bias_validated= 0x00;

  int16_t data[STM_LIS3DSH_NUM_AXES] = {0};
  int32_t bias[STM_LIS3DSH_NUM_AXES] = { 0 };
  int32_t bias_thresholds[STM_LIS3DSH_NUM_AXES] = { 0 };

  *err = 0;

  LIS3DSH_MSG_3(HIGH, "LIS3DSH bias test starting..(%d) (%d),(%d)",0,0,0);
  // Initialize and turn on sensor
  // 2g full scale, 100Hz ODR.
  rw_buffer[0] = 0x6F;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_ODR,
      rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH bias test write port failed status %d %d %d",status,0,0);
    return status;
  }
  if(rw_bytes != 1) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH bias test write port failed rw bytes %d %d %d",rw_bytes,0,0);
    return SNS_DDF_EBUS;
  }

  rw_buffer[0] = 0xC0;
  rw_buffer[1] = 0x10;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_FS,
      rw_buffer,
      2,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH bias test write port failed status %d %d %d",status, 0,0);
    return status;
  }
  if(rw_bytes != 2){
    LIS3DSH_MSG_3(HIGH, "LIS3DSH bias test write port failed rw bytes %d %d %d",rw_bytes, 0, 0);
    return SNS_DDF_EBUS;
  }

  // wait 141 ms.
  sns_ddf_delay(141000);

  //collect 20 accel data samples at 100Hz
  status = sns_dd_acc_lis3dsh_collect_testdata(dd_handle, data, 21);
  if(status != SNS_DDF_SUCCESS){
    LIS3DSH_MSG_3(HIGH, "LIS3DSH SNS_DDF_TEST_OEM collect test data failed status %d %d %d",status, 0, 0);
    return status;
  }
  //convert the data into to mg using sensitivity 2G - STM_LIS3DSH_SSTVT_2G
  bias[0] = (data[0] * STM_LIS3DSH_SSTVT_2G) / 1000;
  bias[1] = (data[1] * STM_LIS3DSH_SSTVT_2G) / 1000;
  bias[2] = ((data[2] * STM_LIS3DSH_SSTVT_2G) / 1000) - 1000;

  //map the sensor data to the phone's coordinates
  sns_ddf_map_axes(&state->axes_map, bias);

  //calculate max bias threshold  in mg
  bias_thresholds[0] = STM_LIS3DSH_SWST_MAX_BIAS ; // x-axis max bias
  bias_thresholds[1] = STM_LIS3DSH_SWST_MAX_BIAS ; // y-axis max bias
  bias_thresholds[2] = STM_LIS3DSH_SWST_MAX_BIAS ; // z-axis max bias

  if(abs(bias[0]) > bias_thresholds[0])
    axis_bias_validated |= BIAS_FAIL_Y;

  if(abs(bias[1]) > bias_thresholds[1])
    axis_bias_validated |= BIAS_FAIL_X;

  if(abs(bias[2]) > bias_thresholds[2])
    axis_bias_validated |= BIAS_FAIL_Z;

  *err = (axis_bias_validated << 8 | axis_validated);
  if (*err)
  {
    status = SNS_DDF_EFAIL;
  }

  //Disable sensor
  rw_buffer[0] = 0x0;
  sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_ODR,
      rw_buffer,
      1,
      &rw_bytes);
  sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_FS,
      rw_buffer,
      1,
      &rw_bytes);

  LIS3DSH_MSG_3(HIGH, "LIS3DSH bias test returning status(%d) err(0x%x) rsvd(%d)",status, *err, 0);
  return status;
}

/**
 * @brief Runs lis3dsh factory hardware self test and returns the test result.
 *        There is no need to call init before self test, while haveing init called
 *        before it won't be harmful though.
 *        After self test is done, lis3dsh driver and device will return to the state
 *        they were in after device is just powered on(need to call init and enable
 *        before getting any data).
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 * @param[in]  oem_flag      For OEM specific HW selftst
 * @param[out] err          update err with error code apart from return status.
 *
 * @return SNS_DDF_SUCCESS if this HW self test was done successfully; SNS_DDF_EFAIL
 *         if HW self test fails. Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *         SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_HW_selftest(
    sns_ddf_handle_t  dd_handle, sns_ddf_sensor_e  sensor, uint32_t*   err, bool oem_flag)
{
  sns_dd_acc_lis3dsh_state_t* state = (sns_dd_acc_lis3dsh_state_t* )
                                        ((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);
  sns_ddf_status_e status;
  uint8_t rw_buffer[3] = {0, 0, 0};
  uint8_t rw_bytes = 0;
  int16_t OUT_NOST[STM_LIS3DSH_NUM_AXES];
  int16_t OUT_ST[STM_LIS3DSH_NUM_AXES];
  int32_t OUTX_NOST,OUTY_NOST,OUTZ_NOST;
  int32_t OUTX_ST,OUTY_ST,OUTZ_ST;

  *err = 0;
  LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test starting..(%d) (%d),(%d)",0,0,0);
  // Configure accelerometer control registers for self test.
  // Initialize and turn on sensor
  // 2g full scale, 50Hz ODR.
  rw_buffer[0] = 0x5F;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_ODR,
      rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test write port failed status %d %d %d",status,0,0);
    return status;
  }
  if(rw_bytes != 1) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test write port failed rw bytes %d %d %d",rw_bytes,0,0);
    return SNS_DDF_EBUS;
  }

  rw_buffer[0] = 0xC0;
  rw_buffer[1] = 0x10;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_FS,
      rw_buffer,
      2,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test write port failed status %d %d %d",status, 0,0);
    return status;
  }
  if(rw_bytes != 2){
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test write port failed rw bytes %d %d %d",rw_bytes, 0, 0);
    return SNS_DDF_EBUS;
  }

  // wait 141 ms.
  sns_ddf_delay(141000);

  //Collect data
  memset(OUT_NOST, 0x0, sizeof(int16_t)*STM_LIS3DSH_NUM_AXES);
  status = sns_dd_acc_lis3dsh_collect_testdata(dd_handle, OUT_NOST, 6);
  if(status != SNS_DDF_SUCCESS){
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test collect test data failed status %d %d %d",status, 0, 0);
    return status;
  }

  //Enable self test
  rw_buffer[0] = 0xC2;
  status = sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_FS,
      rw_buffer,
      1,
      &rw_bytes);
  if(status != SNS_DDF_SUCCESS) {
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test write port failed status %d %d %d", status, 0,0);
    return status;
  }
  if(rw_bytes != 1){
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test write port failed rw bytes %d %d %d",rw_bytes,0,0);
    return SNS_DDF_EBUS;
  }

  // wait 100 ms.
  sns_ddf_delay(100000);

  //Collect data
  memset(OUT_ST, 0x0, sizeof(int16_t)*STM_LIS3DSH_NUM_AXES);
  status = sns_dd_acc_lis3dsh_collect_testdata(dd_handle, OUT_ST, 6);
  if(status != SNS_DDF_SUCCESS){
    LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test collect test data failed status %d %d %d",status, 0,0);
    return status;
  }

  //Check self test limits
  OUTX_NOST = OUT_NOST[0] >> 4;
  OUTY_NOST = OUT_NOST[1] >> 4;
  OUTZ_NOST = OUT_NOST[2] >> 4;
  OUTX_ST = OUT_ST[0] >> 4;
  OUTY_ST = OUT_ST[1] >> 4;
  OUTZ_ST = OUT_ST[2] >> 4;


  if((STM_LIS3DSH_ST_2G_MIN > abs(OUTX_ST-OUTX_NOST))
      ||(abs(OUTX_ST-OUTX_NOST) > STM_LIS3DSH_ST_2G_MAX)
      || (STM_LIS3DSH_ST_2G_MIN > abs(OUTY_ST-OUTY_NOST))
      ||(abs(OUTY_ST-OUTY_NOST) > STM_LIS3DSH_ST_2G_MAX)
      ||(STM_LIS3DSH_ST_2G_MIN > abs(OUTZ_ST-OUTZ_NOST))
      ||(abs(OUTZ_ST-OUTZ_NOST) > STM_LIS3DSH_ST_2G_MAX))
  {
    status = SNS_DDF_EFAIL;
  }
#ifdef CONFIG_OEM
  int axis_validated = 0x00;
  int axis_bias_validated= 0x00;

  if (oem_flag)
  {
    if((STM_LIS3DSH_ST_2G_MIN > abs(OUTX_ST-OUTX_NOST))
        ||(abs(OUTX_ST-OUTX_NOST) > STM_LIS3DSH_ST_2G_MAX))
      axis_validated |= AXIS_FAIL_Y;

    if((STM_LIS3DSH_ST_2G_MIN > abs(OUTY_ST-OUTY_NOST))
        ||(abs(OUTY_ST-OUTY_NOST) > STM_LIS3DSH_ST_2G_MAX))
      axis_validated |= AXIS_FAIL_X;

    if((STM_LIS3DSH_ST_2G_MIN > abs(OUTZ_ST-OUTZ_NOST))
        ||(abs(OUTZ_ST-OUTZ_NOST) > STM_LIS3DSH_ST_2G_MAX))
      axis_validated |= AXIS_FAIL_Z;

    *err = (axis_bias_validated << 8 | axis_validated);
    if (*err)
    {
      status = SNS_DDF_EFAIL;
    }
    LIS3DSH_MSG_3(HIGH, "LIS3DSH OEM HW self test collect test data status %d 0x%x %d", status, *err, 0);
  }
#endif
  //Disable sensor and self test
  rw_buffer[0] = 0x0;
  sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_ODR,
      rw_buffer,
      1,
      &rw_bytes);
  sns_ddf_write_port(
      state->port_handle,
      STM_LIS3DSH_CTRL_FS,
      rw_buffer,
      1,
      &rw_bytes);

  LIS3DSH_MSG_3(HIGH, "LIS3DSH HW self test returning status(%d) err(0x%x) rsvd(%d)",status, *err, 0);
  return status;
}

/**
 * @brief Runs lis3dsh self test and returns the test result.
 *        Supports SNS_DDF_TEST_SELF_HW and SNS_DDF_TEST_SELF_SW.
 *        SNS_DDF_TEST_SELF_HW is the hardware self test provided by LIS3DSH.
 *        SNS_DDF_TEST_SELF_SW is the inertial software self test and bias calibration.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_run_test(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  *err = 0;
  LIS3DSH_MSG_3(HIGH, "LIS3DSH run test sensor (%d) test (%d) %d", sensor, test,0);
  if ((sensor != SNS_DDF_SENSOR_ACCEL) && (sensor != SNS_DDF_SENSOR_GYRO))
  {
    status = SNS_DDF_EINVALID_PARAM;
    LIS3DSH_MSG_3(HIGH, "LIS3DSH run test returning error - status %d %d %d", status, 0, 0);
  }
  else if(test == SNS_DDF_TEST_SELF)
  {
    status = sns_dd_acc_lis3dsh_SW_selftest(dd_handle, sensor);
    if ( SNS_DDF_SUCCESS == status )
    {
      status = SNS_DDF_PENDING;
    }
  }
  else if(test == SNS_DDF_TEST_SELF_HW)
  {
    bool oem_test_flag = false;
    status = sns_dd_acc_lis3dsh_HW_selftest(dd_handle, sensor, err, oem_test_flag);
  }
  else if (test == SNS_DDF_TEST_OEM)
  {
    bool oem_test_flag = true;
    status = sns_dd_acc_lis3dsh_HW_selftest(dd_handle, sensor, err, oem_test_flag);
    if (SNS_DDF_SUCCESS == status) {
      status = sns_dd_acc_lis3dsh_biastest(dd_handle, sensor, err);
    }
  }
  else
  {
    status = SNS_DDF_EINVALID_TEST;
  }
  LIS3DSH_MSG_3(HIGH, "LIS3DSH, self-test result: Sensor(%d), test_type(%d), status(%d)",
      sensor, test, status);
  LIS3DSH_MSG_3(HIGH, "LIS3DSH, self-test result: err(0x%x), rsvd(%d), rsvd(%d)",
      *err, 0, 0);
  return status;
}

/**
 * @brief Implement enable_sched_data() DDF API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
sns_ddf_status_e sns_dd_acc_lis3dsh_enable_sched(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
//  sns_ddf_status_e status;
  sns_dd_acc_lis3dsh_state_t *state;

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  LIS3DSH_MSG_2(HIGH, "enable_sched_data: sensor=%u, enable=%u", sensor, enable);

  //if(SNS_DDF_SENSOR_DOUBLETAP != sensor)
  //  return SNS_DDF_EINVALID_PARAM;

  //each called function will handle concurrencies when to enable/disable FIFO int or dbt int.

  switch(sensor)
  {
    case SNS_DDF_SENSOR_DOUBLETAP:
      {
        if(enable)
        {
          return sns_dd_acc_lis3dsh_enable_dbt_int(state);

        }else{

          return sns_dd_acc_lis3dsh_disable_dbt(state);
        }
        break;
      }

    case SNS_DDF_SENSOR_ACCEL:
      {
        if(enable)
        {
          return sns_dd_acc_lis3dsh_enable_fifo_int(state);

        }else{

          return sns_dd_acc_lis3dsh_disable_fifo_int(state);
        }

        break;
      }
    default:
      return SNS_DDF_EINVALID_ATTR;
  }

  return SNS_DDF_SUCCESS;
}

/**
 * @brief Implement trigger_fifo_data API.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_trigger_fifo(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    uint16_t num_samples,
    bool trigger_now)
{
  sns_dd_acc_lis3dsh_state_t *state;
  uint8_t total_in_fifo = 0;
  uint8_t total_to_read = 0;
  sns_ddf_time_t time;

  state = (sns_dd_acc_lis3dsh_state_t*)((uint32_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  LIS3DSH_MSG_3(HIGH, "trigger_fifo: #samples=%u now=%u sensor=%u",
                num_samples, trigger_now, sensor);

  if((SNS_DDF_SENSOR_ACCEL != sensor) || (num_samples > STM_LIS3DSH_MAX_FIFO))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if(!trigger_now || !state->fifo_enabled)
  {
    return SNS_DDF_SUCCESS;
  }

  total_in_fifo = sns_dd_acc_lis3dsh_get_fifo_sample_count(state);
  if(0 == total_in_fifo)
  {
    return SNS_DDF_SUCCESS;
  }

  time = sns_ddf_get_timestamp();

  if(0 == num_samples)
  {
    total_to_read = total_in_fifo;
  }
  else
  {
    total_to_read = MIN(num_samples, total_in_fifo);
    total_to_read = MIN(total_to_read, state->fifo_wmk);

    //rare case: fifo has more data samples than what SMGR wants,
    //then recalculate end_timestamp
    if(total_to_read < total_in_fifo)
    {
      time = state->last_timestamp +
        total_to_read * (time - state->last_timestamp)/total_in_fifo;
    }
  }

  LIS3DSH_MSG_2(HIGH, "trigger_fifo: total_to_read=%u time=%u",
        total_to_read, time);

  return sns_dd_acc_lis3dsh_send_fifo_data(state, total_to_read, time);
}

/**
 * @brief Called when the reverb timer set by this driver has expired.
 *
 * @note This is used directly by the EM timer and NOT by the DDF timer
 *
 * @param[in] dd_handle  Handle to a driver instance.
 *
 */
static void sns_dd_acc_lis3dsh_handle_reverb_timer(
    void*  dd_handle)
{
  LIS3DSH_MSG_0(MED, "LIS3DSH - Handling reverb timer...");

  sns_dd_acc_lis3dsh_state_t *state;
  sns_daf_general_reverb_ind_v01 reverb_ind;
  uint8_t *trans_id_ptr = NULL;
  uint8_t trans_id;

  if (dd_handle == NULL)
  {
    return;
  }

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  // Generate a DAF Reverb indication and notify the SMGR
  if (state->daf_reverb_item.trans_id_valid)
  {
    trans_id = state->daf_reverb_item.trans_id;
    trans_id_ptr = &trans_id;
  }

  reverb_ind.echo = state->daf_reverb_item.reverb.echo;

  sns_ddf_smgr_notify_daf_ind(SNS_DAF_GENERAL_REVERB_IND_V01,
                              (void *) &reverb_ind,
                              sizeof(sns_daf_general_reverb_ind_v01),
                              trans_id_ptr,
                              state->daf_reverb_item.conn_handle);
}

/**
 * @brief Delivers a Driver Access Framework message to the driver.
 *        Asynchronous/Synchronous API.
 *
 * @detail
 *
 * @param[in]  dd_handle     Handle to a driver instance.
 * @param[in]  req_id        Request identifier.
 * @param[in]  req_msg       Request message in the opaque payload. If no
 *                           payload is supplied, then this pointer will be
 *                           null.
 * @param[in]  req_size      Number of bytes in @req_msg. If req_msg is empty,
 *                           this value must be 0.
 * @param[in]  memhandler    Memory handler used to dynamically allocate
 *                           output parameters, if applicable.
 * @param[out] resp_msg      Pointer to the output message pointer. The output
 *                           message must be allocated first using @memhandler.
 * @param[out] resp_size     Pointer to number of bytes in @resp_msg. If there
 *                           is no DAF response message for the request, then
 *                           this must be 0 to show that the DAF response is
 *                           not present. Response messages are limited in
 *                           size to @SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01 bytes.
 *                           Any response message larger than
 *                           @SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01 bytes will be
 *                           truncated.
 * @param[in]  trans_id_ptr  Pointer to the optional transaction identifier.
                             This will be null if a transaction ID was not
                             provided.
 * @param[in]  conn_handle   The connection handle for the request message.
 *                           This value must be saved if the particular request
 *                           is expected to generate indications. Upon
 *                           notifying the SMGR of an indication, this value
 *                           must be provided to the SMGR.
 *
 * @return Success if the message was retrieved and the buffer was correctly
 *         populated. Otherwise a specific error code is returned.
 */
static sns_ddf_status_e sns_dd_acc_lis3dsh_process_daf_req(
    sns_ddf_handle_t       dd_handle,
    uint32_t               req_id,
    const void*            req_msg,
    uint32_t               req_size,
    sns_ddf_memhandler_s*  memhandler,
    void**                 resp_msg,
    uint32_t*              resp_size,
    const uint8_t*         trans_id_ptr,
    void*                  conn_handle)
{
  sns_ddf_status_e err_code;
  sns_dd_acc_lis3dsh_state_t *state;

  if (dd_handle == NULL)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  LIS3DSH_MSG_1(HIGH, "sns_dd_acc_lis3dsh_process_daf_req: req_id %d", req_id);

  // Set default values
  *resp_size = 0;

  // All of the RTT message processing happens in this switch statement
  switch (req_id)
  {
    case SNS_DAF_GENERAL_WHO_AM_I_V01:
    {
      LIS3DSH_MSG_0(HIGH, "SNS_DAF_GENERAL_WHO_AM_I_V01");

      uint8_t read_buf;
      uint8_t read_bytes;
      sns_daf_general_who_am_i_resp_v01* resp_msg_ptr;

      // conn_handle is not used for this message since this message does not make use of indications
      (void) conn_handle;

      // No need to validate the request message. The message should be empty.

      // Read the WHO AM I register and return the information in the response.
      if ( (err_code = sns_ddf_read_port(state->port_handle,
                                         STM_LIS3DSH_WHO_AM_I,
                                         &read_buf,
                                         1,
                                         &read_bytes))
           != SNS_DDF_SUCCESS)
      {
        LIS3DSH_MSG_1(ERROR, "Error reading port %d", err_code);
        break;
      }

      // Allocate the response message and fill it in
      resp_msg_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_daf_general_who_am_i_resp_v01));
      if (resp_msg_ptr == NULL)
      {
        err_code = SNS_DDF_ENOMEM;
        break;
      }

      resp_msg_ptr->hardware_id = read_buf;
      // No firmware ID
      resp_msg_ptr->firmware_id = SNS_DAF_GENERAL_WHO_AM_I_NO_ID;

      *resp_msg = resp_msg_ptr;
      *resp_size = sizeof(sns_daf_general_who_am_i_resp_v01);

      err_code = SNS_DDF_SUCCESS;
      break;
    }
    case SNS_DAF_GENERAL_ECHO_V01:
    {
      LIS3DSH_MSG_0(HIGH, "SNS_DAF_GENERAL_ECHO_V01");

      sns_daf_general_echo_req_v01*  req_msg_ptr = (sns_daf_general_echo_req_v01*) req_msg;
      sns_daf_general_echo_resp_v01* resp_msg_ptr;

      // Validate the request message
      if ((req_msg == NULL) || (req_size != sizeof(sns_daf_general_echo_req_v01)))
      {
        LIS3DSH_MSG_3(ERROR, "process_daf_req: Invalid DAF Request Message. req_msg 0x%x  req_size %d (expected %d)",
            req_msg, req_size, sizeof(sns_daf_general_echo_req_v01));
        err_code = SNS_DDF_EINVALID_PARAM;
        break;
      }

      // Generate the response message
      resp_msg_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_daf_general_echo_resp_v01));
      if (resp_msg_ptr == NULL)
      {
        err_code = SNS_DDF_ENOMEM;
        break;
      }
      resp_msg_ptr->echo = req_msg_ptr->echo;

      *resp_msg = resp_msg_ptr;
      *resp_size = sizeof(sns_daf_general_echo_resp_v01);

      err_code = SNS_DDF_SUCCESS;
      break;
    }
    case SNS_DAF_GENERAL_REVERB_START_V01:
    {
      LIS3DSH_MSG_0(HIGH, "SNS_DAF_GENERAL_REVERB_START_V01");

      sns_daf_general_reverb_start_req_v01*  req_msg_ptr = (sns_daf_general_reverb_start_req_v01*) req_msg;
      sns_daf_general_reverb_start_resp_v01* resp_msg_ptr;

      // Validate the request message
      if ((req_msg == NULL) || (req_size != sizeof(sns_daf_general_reverb_start_req_v01)))
      {
        LIS3DSH_MSG_3(ERROR, "process_daf_req: Invalid RTT Request Message. req_msg 0x%x  req_size %d (expected %d)",
            req_msg, req_size, sizeof(sns_daf_general_reverb_start_req_v01));
        err_code = SNS_DDF_EINVALID_PARAM;
        break;
      }

      // TODO: This should be able to support concurrent DAF Reverb requests
      // If there's already an existing DAF Reverb request, do not accept this one
      if (state->daf_reverb_item.request_id != SNS_DAF_NO_MSG)
      {
        err_code = SNS_DDF_EDEVICE_BUSY;
        break;
      }

      // Validate the parameters to make sure they're in-bounds
      if (req_msg_ptr->reverb_period < SNS_DAF_GENERAL_REVERB_START_REQ_V01_REVERB_PERIOD_MIN)
      {
        err_code = SNS_DDF_EINVALID_PARAM;
        break;
      }

      // Parse out and store the parameters
      state->daf_reverb_item.request_id = SNS_DAF_GENERAL_REVERB_START_V01;
      state->daf_reverb_item.reverb.echo = req_msg_ptr->echo;
      state->daf_reverb_item.reverb.reverb_period = req_msg_ptr->reverb_period;
      // Save the transaction ID, if it is present
      if (trans_id_ptr != NULL)
      {
        state->daf_reverb_item.trans_id_valid = true;
        state->daf_reverb_item.trans_id = *trans_id_ptr;
      }
      else
      {
        state->daf_reverb_item.trans_id_valid = false;
      }
      // Store the connection handle so indications may be generated and sent
      state->daf_reverb_item.conn_handle = conn_handle;

      // If the DAF timer is NULL, initialize it
      if (state->daf_reverb_timer == NULL)
      {
        if((err_code = sns_em_create_timer_obj(
                           sns_dd_acc_lis3dsh_handle_reverb_timer,
                           dd_handle,
                           SNS_EM_TIMER_TYPE_PERIODIC,
                           &state->daf_reverb_timer)) != SNS_DDF_SUCCESS)
        {
          LIS3DSH_MSG_1(ERROR, "Error initializing timer %d", err_code);
          break;
        }
      }

      // Start the timer
      if ( (err_code = sns_em_register_timer(state->daf_reverb_timer,
                                             sns_em_convert_usec_to_localtick(req_msg_ptr->reverb_period)))
            != SNS_DDF_SUCCESS )
      {
        LIS3DSH_MSG_1(ERROR, "Error starting timer %d", err_code);
        break;
      }

      // Generate the response message
      resp_msg_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_daf_general_reverb_start_resp_v01));
      if (resp_msg_ptr == NULL)
      {
        err_code = SNS_DDF_ENOMEM;
        LIS3DSH_MSG_1(ERROR, "Error generating response message %d", err_code);
        break;
      }
      resp_msg_ptr->echo = req_msg_ptr->echo;

      *resp_msg = resp_msg_ptr;
      *resp_size = sizeof(sns_daf_general_reverb_start_resp_v01);

      err_code = SNS_DDF_SUCCESS;
      break;
    }
    case SNS_DAF_GENERAL_REVERB_STOP_V01:
    {
      LIS3DSH_MSG_0(HIGH, "SNS_DAF_GENERAL_REVERB_STOP_V01");

      // TODO: This should be able to support concurrent DAF Reverb requests
      // If there's no existing DAF Reverb request, exit happily
      if (state->daf_reverb_item.request_id == SNS_DAF_NO_MSG)
      {
        err_code = SNS_DDF_SUCCESS;
        break;
      }

      // If the DAF timer is not NULL, then delete it and set it to NULL
      if ( (state->daf_reverb_timer != NULL) &&
           ((err_code = sns_em_delete_timer_obj(state->daf_reverb_timer)) != SNS_DDF_SUCCESS) )
      {
        LIS3DSH_MSG_1(ERROR, "Error releasing the DAF timer %d", err_code);
        break;
      }
      state->daf_reverb_timer = NULL;

      // Clear the DAF Reverb Request
      state->daf_reverb_item.request_id = SNS_DAF_NO_MSG;

      *resp_size = 0;

      err_code = SNS_DDF_SUCCESS;
      break;
    }
    default:
    {
      // Unsupported DAF request
      err_code = SNS_DDF_EINVALID_DAF_REQ;
      break;
    }
  }

  return err_code;
}


/**
 * @brief Cancels all of the driver's current Driver Access Framework
 *        asynchronous transactions for the provided connection handle.
 *
 * @note This does not have to cancel a response message in the process of
 *       being created.
 *       This function does not have to be implemented for drivers that do
 *       not support or implement any asynchronous messages (these messages
 *       require the usage of sns_ddf_smgr_notify_daf_ind).
 *
 * @param[in]  dd_handle     Handle to a driver instance.
 * @param[in]  conn_handle   The connection handle for the client that is
 *                           canceling the Driver Access Framework
 *                           transaction.
 */
static void sns_dd_acc_lis3dsh_cancel_daf_trans(
     sns_ddf_handle_t       dd_handle,
     void*                  conn_handle)
{
  if (dd_handle == NULL)
  {
    return;
  }
  sns_dd_acc_lis3dsh_state_t *state;

  state = (sns_dd_acc_lis3dsh_state_t*)((dd_handle_align_t)dd_handle & DD_HANDLE_ALIGN_BITS);

  LIS3DSH_MSG_1(HIGH, "sns_dd_acc_lis3dsh_cancel_daf_trans: conn_handle 0x%X", conn_handle);

  // Stop the "REVERB" transaction, if it exists and the connection handle matches
  if (state->daf_reverb_item.request_id != SNS_DAF_NO_MSG &&
      state->daf_reverb_item.conn_handle == conn_handle)
  {
    // If the DAF timer is not NULL, then delete it and set it to NULL
    if ( (state->daf_reverb_timer != NULL) &&
         (sns_em_delete_timer_obj(state->daf_reverb_timer) != SNS_SUCCESS) )
    {
      LIS3DSH_MSG_1(ERROR, "Error releasing the DAF timer %d", err_code);
    }
    state->daf_reverb_timer = NULL;

    // Clear the DAF Reverb Request
    state->daf_reverb_item.request_id = SNS_DAF_NO_MSG;
  }
}


/**
 * LIS3DSH device driver interface.
 */
sns_ddf_driver_if_s sns_dd_acc_lis3dsh_if =
{
  &sns_dd_acc_lis3dsh_init,
  &sns_dd_acc_lis3dsh_get_data,
  &sns_dd_acc_lis3dsh_set_attr,
  &sns_dd_acc_lis3dsh_get_attr,
  NULL,  // handle_timer
  &sns_dd_acc_lis3dsh_handle_irq,
  &sns_dd_acc_lis3dsh_reset,
  &sns_dd_acc_lis3dsh_run_test,
  &sns_dd_acc_lis3dsh_enable_sched,
  NULL,  // probe
  &sns_dd_acc_lis3dsh_trigger_fifo,
  .process_daf_req = &sns_dd_acc_lis3dsh_process_daf_req,
  .cancel_daf_trans = &sns_dd_acc_lis3dsh_cancel_daf_trans
};
