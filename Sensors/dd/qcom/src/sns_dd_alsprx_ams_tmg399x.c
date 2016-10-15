/********************************************************************************
* Copyright (c) 2013, "ams AG"
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of "ams AG" nor the
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
*******************************************************************************/
/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the AMS ALS(Light) and Proximity Sensor
   This driver has 3 sub-modules:
   1. The common handler that provides the driver interface
   2. The light(ALS) driver that handles light data type
   3. The proximity driver that handles proximity data type

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     -----------------------------------------------------------
08/15/13     vh      Eliminated Klocwork warnings
06/27/13     fv      Convert tmd377x driver to tmg399x
06/10/13     fv      Convert tmd277x driver to tmd377x
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
12/11/09     fv      Convert Intersil driver to drive AMS TSL/TMD 377x ALS/PROX sensor
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_dd_alsprx_ams_tmg399x_priv.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
//#include "sns_dd.h"
#include <string.h>
//#include <stdlib.h>

#ifndef DDK
#include "sns_memmgr.h"
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
//#include "sns_ddf_util.h"
#endif

#ifdef ALSPRX_SDD_TEST
#error code not present
#endif


/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
/* static ams_als_lux_coef ams_tmg399x_init_lux_coefs[] = { */
/*     {53, 2000, 600, 1000},   /\* TSL2571 *\/ */
/*     {53, 2000, 600, 1000},   /\* TSL2671 *\/ */
/*     {53, 2000, 600, 1000},   /\* TSL2771 *\/ */

/*     {24, 2000, 600, 1000},   /\* TMD2671 *\/ */
/*     {24, 2000, 600, 1000},   /\* TMD2771 *\/ */

/*     {60, 1870, 630, 1000},   /\* TSL2572 *\/ */
/*     {60, 1870, 630, 1000},   /\* TSL2672 *\/ */
/*     {60, 1870, 630, 1000},   /\* TSL2772 *\/ */

/*     {20, 1750, 630, 1000},   /\* TMD2672 *\/ */
/*     {20, 1750, 630, 1000}    /\* TMD2772 *\/ */
/* }; */


//
// CHECK THIS TABLE
//
//
// Convert ODR in Hz to uSec
//
#define MAX_ODR  40
#define NUM_ALS_SAMPLES_RET 7
#define NUM_PROX_SAMPLES_RET 2
uint32 myGPIO;
static uint32_t ams_tmg399x_odr_to_uSec[] = {
    // uSec        odr
    1000000,    //  1
     500000,    //  2
     333333,    //  3
     250000,    //  4
     200000,    //  5
     166667,    //  6
     142857,    //  7
     125000,    //  8
     111111,    //  9
     100000,    // 10
      90909,    // 11
      83333,    // 12
      76923,    // 13
      71429,    // 14
      66667,    // 15
      62500,    // 16
      58824,    // 17
      55556,    // 18
      52632,    // 19
      50000,    // 20
      47619,    // 21
      45455,    // 22
      43478,    // 23
      41667,    // 24
      40000,    // 25
      38462,    // 26
      37037,    // 27
      35714,    // 28
      34483,    // 29
      33333,    // 30
      32258,    // 31
      31250,    // 32
      30303,    // 33
      29412,    // 34
      28571,    // 35
      27778,    // 36
      27027,    // 37
      26316,    // 38
      25641,    // 39
      25000     // 40
};

#define ODR_USEC_MAX  ams_tmg399x_odr_to_uSec[0]
#define ODR_USEC_MIN  ams_tmg399x_odr_to_uSec[MAX_ODR-1]


//
//++ TMG399x ++
//
static uint8_t const ams_tmg399x_als_gains[] = {1, 4, 16, 64};


static uint16_t ams_tmg399x_min_integ_time_us[] = {
    2780,    // TMG3992
    2780,    // TMG39923
};


static struct lux_segment ams_tmg399x_lux_coef[] = {
    {486,  93, 1000, 522, 4490, 1695},    // TMG3992
    {486,  93, 1000, 522, 4490, 1695},    // TMG3993
};


/* static uint8_t const ams_tmg399x_ids[] = { */
/*     0x9C,    // TMG39921/5 */
/*     0x9F     // TMG39923/7 */
/* }; */

//
//-- TMG399x --
//

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_init
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

static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_ams_tmg399x_alsprx_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

void sns_dd_ams_tmg399x_alsprx_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_reset
(
  sns_ddf_handle_t dd_handle
);


sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);


sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );

sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list =
{
  &sns_dd_ams_tmg399x_alsprx_init,
  &sns_dd_ams_tmg399x_alsprx_get_data,
  &sns_dd_ams_tmg399x_alsprx_set_attrib,
  &sns_dd_ams_tmg399x_alsprx_get_attrib,
  &sns_dd_ams_tmg399x_alsprx_handle_timer,
  &sns_dd_ams_tmg399x_alsprx_handle_irq,
  &sns_dd_ams_tmg399x_alsprx_reset,
  &sns_dd_ams_tmg399x_alsprx_run_test,
  &sns_dd_ams_tmg399x_alsprx_enable_sched_data
};

sns_ddf_status_e ams_tmg399x_als_sensor_samples(sns_dd_alsprx_state_t   *state,
                                                sns_ddf_sensor_data_s   *data_ptr,
                                                sns_ddf_memhandler_s    *memhandler,
                                                sns_ddf_sensor_sample_s *sample_data);

sns_ddf_status_e ams_tmg399x_prox_sensor_samples(sns_dd_alsprx_state_t   *state,
                                                 sns_ddf_sensor_data_s   *data_ptr,
                                                 sns_ddf_memhandler_s    *memhandler,
                                                 sns_ddf_sensor_sample_s *sample_data);

sns_ddf_status_e ams_tmg399x_set_default_reg_values(sns_dd_alsprx_state_t* dd_ptr);
sns_ddf_status_e ams_tmg399x_get_lux(sns_dd_alsprx_state_t* state);
sns_ddf_status_e ams_tmg399x_get_als_data(sns_dd_alsprx_state_t* state);
sns_ddf_status_e ams_tmg399x_get_prox_data(sns_dd_alsprx_state_t* state);
sns_ddf_status_e ams_tmg399x_calibrate_als(sns_dd_alsprx_state_t* state);
void             ams_tmg399x_set_reg_values_from_nv(sns_dd_alsprx_state_t* state);
sns_ddf_status_e ams_tmg399x_read_all_data(sns_dd_alsprx_state_t* state);


//
// +++++++++++++++++++  AMS-TAOS USA Code   +++++++++++++++++++
//

/*!
 ***************************************
 * @brief Function to write a single byte
 * to the TMG399x at register /b reg
 *
 * @param reg : Register address to write byte
 * @param byteVal : byte value to write at register
 *
 * @return status of write
 ***************************************
 */
sns_ddf_status_e ams_tmg399x_write_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    DDF_MSG_0(HIGH, "ams_tmg399x_write_byte");
    status = sns_ddf_write_port(port_handle,
                                AMS_tmg399x_CMD_REG | reg,
                                byte_val,
                                1,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e ams_tmg399x_write_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          write_count;

    status = sns_ddf_write_port(port_handle,
                                AMS_tmg399x_CMD_REG | reg,
                                &word_val->byte[0],
                                2,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e ams_tmg399x_write_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* buf, uint8_t size)
{
    sns_ddf_status_e status;
    uint8_t          write_count;


    status = sns_ddf_write_port(port_handle,
                                AMS_tmg399x_CMD_REG | reg,
                                buf,
                                size,
                                &write_count
                                );
    return status;
}


sns_ddf_status_e ams_tmg399x_read_buf(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t byteCnt, uint8_t* buf)
{
    sns_ddf_status_e status;
    uint8_t          read_count;


    status = sns_ddf_read_port(port_handle,
                               AMS_tmg399x_CMD_REG | reg,
                               buf,
                               byteCnt,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e ams_tmg399x_read_byte(sns_ddf_handle_t port_handle, uint8_t reg, uint8_t* byte_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;


    status = sns_ddf_read_port(port_handle,
                               AMS_tmg399x_CMD_REG | reg,
                               byte_val,
                               1,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e ams_tmg399x_read_word(sns_ddf_handle_t port_handle, uint8_t reg, uword_u* word_val)
{
    sns_ddf_status_e status;
    uint8_t          read_count;
    uword_u          word_tmp;


    status = sns_ddf_read_port(port_handle,
                               AMS_tmg399x_CMD_REG | reg,
                               &word_tmp.byte[0],
                               2,
                               &read_count
                               );
    if(status == SNS_DDF_SUCCESS)
    {
        word_val->word = word_tmp.word;
    }
    return status;
}


/*!
 ***************************************
 * @brief Write special function to the TMG399x command register
 *
 * Will write special function byte to slave device, and dummy byte will be ignored.
 *
 * @param port_handle : Handle to port data
 * @param spec_func   : Special function to write to command register
 * - 0x00  - Normal operation
 * - 0x05  - Proximity interrupt clear
 * - 0x06  - ALS interrupt clear
 * - 0x07  - Proximity and ALS interrupt clear
 * - other - Reserved, do NOT write
 *
 * @return status
 * - SNS_DDF_SUCCESS    - No error
 * - SNS_DDF_EDEVICE    - Comm error
 * - SNS_DDF_EBUS       - Comm error
 * - SNS_EINVALID_PARAM - Incorrect Config parameters
 ***************************************
 */
sns_ddf_status_e ams_tmg399x_write_spec_func(sns_ddf_handle_t port_handle, uint8_t spec_func)
{
    sns_ddf_status_e status;
    uint8_t          rw_count;
    uint8_t          i2c_data;
    uint8_t          dummy_byte;

    i2c_data   = (AMS_tmg399x_CMD_REG | AMS_tmg399x_CMD_SPL_FN | spec_func);
    dummy_byte = i2c_data;
#ifdef DDK
    status = sns_ddf_read_port(port_handle,
                               i2c_data,
                               (uint8_t*)&dummy_byte,
                               1,
                               &rw_count
                               );
#else
    status = sns_ddf_write_port(port_handle,
                                i2c_data,
                                NULL,
                                0,
                                &rw_count
                                );
#endif
    return status;
}


uint8_t ams_tmg399x_usecToIcycles(uint32_t time_us, uint32_t minIntegTime_us)
{
    uint8_t integrationCycles;

    if(time_us < minIntegTime_us)
    {
        time_us = minIntegTime_us;
    }

    integrationCycles = time_us / minIntegTime_us;

    return (256 - integrationCycles);
}

#define MS_TO_US(t) (t * 1000)
#define US_TO_MS(t) (t / 1000)

//
// ------------------  AMS-TAOS USA Code   ------------------
//

/*===========================================================================
FUNCTION      sns_dd_ams_tmg399x_alsprx_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
void sns_dd_ams_tmg399x_alsprx_log(
   sns_dd_alsprx_state_t* dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               data1,
   uint32_t               data1_q16,
   uint32_t               data2,
   uint32_t               data2_q16,
   uint32_t               raw_data )
{
#ifndef DDK
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   DDF_MSG_0(HIGH, "TMG399x Log Data");

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     DDF_MSG_0(HIGH, "in sns_dd_ams_tmg399x_alsprx_log");
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor_type;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AMS;

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
       DDF_MSG_1(HIGH, "TMG399x Log Data - logpkt_malloc failed with err: %d", err_code);
       dd_ptr->dropped_logs++;
   }
#endif
}



sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_enable_sched_data(sns_ddf_handle_t dd_handle,
                                                         sns_ddf_sensor_e sensor,
                                                         bool             enable
                                                         )
{
    sns_dd_alsprx_state_t* state = dd_handle;

    uint8_t i2c_data;
    uint8_t byteVal;
    uint8_t readByte;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    ams_setup*       setup;

    setup = &state->chip.setup;

    //
    // Clear interrupts
    // Clear any pending interrupts
    byteVal = 0x07;
    DDF_MSG_0(HIGH, "sns_dd_ams_tmg399x_alsprx_enable_sched_data");
    status = ams_tmg399x_write_spec_func(state->port_handle, byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    DDF_MSG_2(MED, "TMG399 enable: %d var2: %d", 1100, enable);

    // set up for run.
    //

    if(sensor == SNS_DDF_SENSOR_PROXIMITY)
    {
        if(enable)
        {
            if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_PEN) == 0x00)
            {
                // if prox was not enabled previously, write new ptime
                byteVal = ams_tmg399x_usecToIcycles(setup->prox.ptime_us, setup->Min_Integ_Time_us);
                status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPTIME, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
                DDF_MSG_2(MED, "TMG399 enb sch data prx var1: %d var2: %d", 1102, byteVal);

                // DEBUG READ
                ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rPTIME, &readByte);
                DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1103, readByte);
            }
            state->chip.setup.prox.enabled = true;
            state->chip.setup.reg.enable  |= (AMS_tmg399x_ENABLE_MSK_PIEN | AMS_tmg399x_ENABLE_MSK_PEN);
        }
        else
        {
            state->chip.setup.reg.enable &= ~(AMS_tmg399x_ENABLE_MSK_PIEN | AMS_tmg399x_ENABLE_MSK_PEN);
        }
    }
    else if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        if(enable)
        {
            if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_AEN) == 0x00)
            {
                // if ALS not enabled, write new atime
                byteVal = ams_tmg399x_usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us);
                status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rATIME, &byteVal);
                if ( status != SNS_DDF_SUCCESS ) {
                    return status;
                }
                DDF_MSG_2(MED, "TMG399 TMD277 enb sch data als: %d var2: %d", 1104, setup->als.atime_us);
                DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1105, byteVal);
            }
            state->chip.setup.als.enabled = true;
            state->chip.setup.reg.enable |= (AMS_tmg399x_ENABLE_MSK_AIEN | AMS_tmg399x_ENABLE_MSK_AEN);
        }
        else
        {
            state->chip.setup.reg.enable &= ~(AMS_tmg399x_ENABLE_MSK_AIEN | AMS_tmg399x_ENABLE_MSK_AEN);
        }
    }

    state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PON;
    i2c_data = state->chip.setup.reg.enable;
    status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &i2c_data);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1106, i2c_data);

    return status;
}


/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_prx_binary

DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_prx_binary
(
  sns_dd_alsprx_state_t* state
)
{
    uint16_t pdata;
    uint16_t detect;
    uint16_t release;
    uword_u wordVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;


    pdata   = state->chip.data.prox.data;
    DDF_MSG_1(MED, "pdata %d",  pdata );
    detect  = state->chip.setup.prox.detect;
    DDF_MSG_1(MED, "detect %d",  detect );
    release = state->chip.setup.prox.release;
    DDF_MSG_1(MED, "release %d",  release );

    if(state->chip.data.prox.event == SNS_PRX_FAR_AWAY)
    {
        if(pdata > detect)
        {
            state->chip.data.prox.event = SNS_PRX_NEAR_BY;

            state->chip.setup.prox.thresh_high = 0xFFFF;
            state->chip.setup.prox.thresh_low  = state->chip.setup.prox.release;

            // Set Prox interrupt low
            wordVal.word = state->chip.setup.prox.thresh_low;
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHL, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 pdata > detect.. release: %d var2: %d", state->chip.setup.prox.release, wordVal.byte[0]);

            // Set Prox interrupt high
            wordVal.word = state->chip.setup.prox.thresh_high;
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHH, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 prox high var1: %d var2: %d", 1108, wordVal.byte[0]);
        }
    }
    else  // SNS_PRX_NEAR_BY
    {
        if(pdata < release)
        {
            state->chip.data.prox.event = SNS_PRX_FAR_AWAY;

            state->chip.setup.prox.thresh_low  = 0x0000;
            state->chip.setup.prox.thresh_high = state->chip.setup.prox.detect;

            // Set Prox interrupt low
            wordVal.word = state->chip.setup.prox.thresh_low;
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHL, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 pdata < release: %d var2: %d", 1109, wordVal.byte[0]);

            // Set Prox interrupt high
            wordVal.word = state->chip.setup.prox.thresh_high;
            status = ams_tmg399x_write_byte(state->port_handle, TMG399X_PITHH, &wordVal.byte[0]);
            if ( status != SNS_DDF_SUCCESS ) {
                DDF_MSG_0(HIGH, "ALSPRX prx processing err");
                return status;
            }
            DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1110, wordVal.byte[0]);
        }
    }
    return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_tmg399x_set_als_thresh

DESCRIPTION   This function is called in DRI mode- sets the ALS thresholds to
              make ALS sensor function like an event sensor.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_tmg399x_set_als_thresh
(
  sns_dd_alsprx_state_t* state
)
{
    uint32_t clearData;
    uint32_t t_low;
    uint32_t t_high;
    uword_u  wordVal;
    uword_u  readALSTh;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DDF_MSG_0(HIGH, "sns_dd_alsprx_tmg399x_set_als_thresh:");
    DDF_MSG_1(MED, "ALS High Threshold %d",  state->chip.setup.als.thresh_high );
	DDF_MSG_1(MED, "ALS Low Threshold %d",  state->chip.setup.als.thresh_low);

    clearData   = state->chip.als_inf.clear_raw;
    DDF_MSG_1(MED, "clearData %d",  clearData );

    t_low = (clearData * (100 - state->chip.setup.als.thresh_low)) / 100;   //set to 20 as default
    DDF_MSG_1(MED, "als t low %d",  t_low );

    t_high = (clearData * (100 + state->chip.setup.als.thresh_high)) / 100; //set to 20 as default
    DDF_MSG_1(MED, "als t high %d",  t_high);

	//set als threshold low at -x% of data and set als threshold high at x% of the pdata, x defined in registry

    wordVal.word = t_low;
    DDF_MSG_1(MED, "als t low wordval.word %d",  wordVal.word);
    status = ams_tmg399x_write_word(state->port_handle, TMG399X_AILTHL, &wordVal);
        if ( status != SNS_DDF_SUCCESS )
           {
              DDF_MSG_1(HIGH, "ALS thresh low set error %d", wordVal.word);
              return status;
           }
    status = ams_tmg399x_read_word(state->port_handle, TMG399X_AILTHL, &readALSTh);
    DDF_MSG_1(MED, "als t low wordval.word readback %d",  readALSTh.word);
	    if ( status != SNS_DDF_SUCCESS )
           {
              DDF_MSG_1(HIGH, "ALS thresh low readback error %d", readALSTh.word);
              return status;
           }

    wordVal.word = t_high;
    DDF_MSG_1(MED, "als t high wordval.word %d",  wordVal.word);
    status = ams_tmg399x_write_word(state->port_handle, TMG399X_AIHTHL, &wordVal);

       if ( status != SNS_DDF_SUCCESS )
          {
             DDF_MSG_1(HIGH, "ALS Thresh set high error %d", wordVal.word);
             return status;
          }
    status = ams_tmg399x_read_word(state->port_handle, TMG399X_AIHTHL, &readALSTh);
   DDF_MSG_1(MED, "als t high wordval.word readback %d",  readALSTh.word);
	   if ( status != SNS_DDF_SUCCESS )
          {
             DDF_MSG_1(HIGH, "ALS thresh high readback error %d", readALSTh.word);
             return status;
          }

    return status;
}


void ams_tmg399x_read_status_reg(sns_dd_alsprx_state_t* state, sns_ddf_time_t timestamp, AMS_tmg399x_SIG_TYPE sig_type)
{
    uint8_t  statusReg;
    uint8_t  enableReg;
    uint8_t  numOfSensors;
    uint8_t  i;
    uint8_t  clrInterrupts;
    bool     hasInterrupt;
    sns_ddf_status_e eresult;
    static sns_ddf_sensor_sample_s *buf_ptr;
    static sns_ddf_sensor_data_s   *sensor_data;

    sns_ddf_sensor_e sensors[2];


    numOfSensors  = 0;
    clrInterrupts = 0;
    hasInterrupt  = false;

/* #ifdef DDK */
/*     uint8_t byteBuf[20]; */
/*     ams_tmg399x_read_buf(state->port_handle, */
/*                          AMS_tmg399x_rENABLE, */
/*                          16, */
/*                          byteBuf); */
/*     ams_tmg399x_read_buf(state->port_handle, */
/*                          AMS_tmg399x_rPDATA, */
/*                          2, */
/*                          byteBuf); */
/* #endif */

    /* ams_tmg399x_read_byte(state->port_handle, */
    /*                       AMS_tmg399x_rSTATUS, */
    /*                       &statusReg); */

    // Read all of the data
    ams_tmg399x_read_all_data(state);

    statusReg = state->chip.shadow[TMG399X_STATUS];
    enableReg = state->chip.shadow[TMG399X_ENABLE];
    DDF_MSG_1(MED, "TMG399 in read_status_reg  status_reg is: %d" , statusReg);
    DDF_MSG_1(MED, "TMG399 in read_status_reg  enable_reg is: %d" , enableReg);

    if(statusReg & AMS_tmg399x_STA_ALS_INTR)
    {
        //
        // ALS interrupt
        //

        DDF_MSG_1(MED, "TMG399 ALS Interrupt  (int reg): %d", AMS_tmg399x_STA_ALS_INTR);

        clrInterrupts |= AMS_tmg399x_CMD_ALS_INTCLR;
        hasInterrupt = true;

        //
        // On an ALS interrupt, will read the ALS data and store it.
        // Will NOT return data (lux).
        // Lux will be returned in the timer.
        //
#ifdef DDK
        if(sig_type == TIMER_TYPE)
#endif
        {
           DDF_MSG_0(HIGH, "TIMER_TYPE:");
            // Read ALS data
            ams_tmg399x_get_als_data(state);

            /* set als threshold to be +/- 20% of current value for next interrupt.*/
            sns_dd_alsprx_tmg399x_set_als_thresh(state);

            // compute LUX
            ams_tmg399x_get_lux(state);

            sensors[numOfSensors] = SNS_DDF_SENSOR_AMBIENT;
            numOfSensors++;
            DDF_MSG_1(MED, "TMG399 ALS Interrupt, data read: %d", AMS_tmg399x_STA_ALS_INTR);
        }

        DDF_MSG_2(MED, "TMG399 ALS int numOfSensors: %d lux: %d", numOfSensors, state->chip.data.als.lux);
    }


    if(statusReg & AMS_tmg399x_STA_PRX_INTR)
    {
        //
        // PROX interrupt
        //

        DDF_MSG_1(MED, "TMG399 in prox interrupt (prox int reg): %d", AMS_tmg399x_STA_PRX_INTR);

        ams_tmg399x_get_prox_data(state);
        sns_dd_ams_tmg399x_alsprx_prx_binary(state); /* set near or far */

        clrInterrupts |= AMS_tmg399x_CMD_PROX_INTCLR;
        hasInterrupt = true;
        sensors[numOfSensors] = SNS_DDF_SENSOR_PROXIMITY;
        numOfSensors++;

        DDF_MSG_1(MED, "TMG399 out of prox data and prx_binary set state: %d", 1115);

    }

    if(numOfSensors > 0)
    {
        // Had at least one interrupt
        if ((sns_ddf_malloc((void **)&sensor_data, numOfSensors*sizeof(sns_ddf_sensor_data_s))) != SNS_DDF_SUCCESS )
        {
            DDF_MSG_1(MED, "TMG399 var1: %d", 1116);
            /* Failed miserably. Can't even notify SMGR */
            return;
        }

        for(i = 0; i < numOfSensors; i++)
        {
            sensor_data[i].sensor    = sensors[i];
            sensor_data[i].status    = SNS_DDF_SUCCESS;
            sensor_data[i].timestamp = timestamp;

            switch(sensors[i])
            {
            case SNS_DDF_SENSOR_PROXIMITY:
                DDF_MSG_1(MED, "TMG399 var1: %d", 1117);
                if((sns_ddf_malloc((void **)&buf_ptr, NUM_PROX_SAMPLES_RET * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(MED, "TMG399 cannot notify smgr prx: %d", 1118);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                ams_tmg399x_prox_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;

            case SNS_DDF_SENSOR_AMBIENT:
                DDF_MSG_1(MED, "TMG399 var1: %d", 1119);
                if((sns_ddf_malloc((void **)&buf_ptr, 3 * sizeof(sns_ddf_sensor_sample_s))) != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_1(HIGH, "TMG399 cannot notify smgr als: %d", 1120);
                    /* Failed miserably. Can't even notify SMGR */
                    return;
                }

                ams_tmg399x_als_sensor_samples(state, &sensor_data[i], NULL, buf_ptr);
                break;

            default:
                DDF_MSG_1(HIGH, "Unexpected sensor type: %d", sensors[i]);
                break;
            }
        }

        eresult = sns_ddf_smgr_notify_data(state->smgr_handle, sensor_data, numOfSensors);
        DDF_MSG_2(HIGH, "result sns_ddf_smgr_notify_data %d,numOfSensors %d", eresult,numOfSensors);
        sns_ddf_mfree(buf_ptr);
        sns_ddf_mfree(sensor_data);
    }

    if(hasInterrupt)
    {
        DDF_MSG_1(MED, "TMG399 clearing interrrupt write (05-prox 06 als 07 both)   written value: %d", clrInterrupts);
        ams_tmg399x_write_spec_func(state->port_handle, clrInterrupts);

		/* - 0x05  - Proximity interrupt clear * - 0x06  - ALS interrupt clear * - 0x07  - Proximity and ALS interrupt clear*/


#ifdef DDK
        ams_tmg399x_read_byte(state->port_handle,
                              AMS_tmg399x_rSTATUS,
                              &statusReg);
        DDF_MSG_2(MED, "TMG399 status reg var1: %d var2: %d", 1122, statusReg);
#endif
    }

    DDF_MSG_1(MED, "TMG399 out of read status reg: %d", 1123);
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_handle_irq

===========================================================================*/
/*!
  @brief AMS TMG399x interrupt handler

  @detail
  This function will be called within the context of the Interrupt
  Service Task (IST), *not* the ISR.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] irq        The IRQ representing the interrupt that occured.

  @return None
*/
/*=========================================================================*/
void sns_dd_ams_tmg399x_alsprx_handle_irq
(
  sns_ddf_handle_t  handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;

    DDF_MSG_1(HIGH, "TMG399 in handle irq: %d", 1124);
    DDF_MSG_1(HIGH, "Time stamp : %d", timestamp);
    ams_tmg399x_read_status_reg(state, timestamp, IRQ_TYPE);
}

/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_handle_timer

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
static void sns_dd_ams_tmg399x_alsprx_handle_timer
(
  sns_ddf_handle_t handle,
  void*            arg
)
{
    sns_ddf_time_t    timestamp;
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t*)handle;

    DDF_MSG_1(LOW, "TMG399 var1: %d", 5);

    timestamp = sns_ddf_get_timestamp();

    ams_tmg399x_read_status_reg(state, timestamp, TIMER_TYPE);
}


/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_als_init

DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_ams_tmg399x_alsprx_als_init (sns_dd_alsprx_state_t* dd_ptr)
{
/*  sns_dd_als_db_type           *als_db_ptr    = &dd_ptr->sns_dd_als_db; */
/*  sns_dd_alsprx_common_db_type *common_db_ptr = &dd_ptr->sns_dd_alsprx_common_db; */

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_INIT);

/*   als_db_ptr->pend_state        = SNS_DD_PEND_STATE_IDLE; */
/*   als_db_ptr->next_rng_sel      = SNS_DD_ISL_ALS_DEF_RNG; */
/*   als_db_ptr->req_rng_sel       = SNS_DD_ISL_ALS_DEF_RNG; */

/* #ifndef DDK */
/*   als_db_ptr->last_get_time     = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp()); */
/* #endif */
/*   als_db_ptr->last_mlux         = SNS_DD_ALS_DFLT_MILLI_LUX; */

/*   als_db_ptr->als_conversion_lo = (SNS_DD_ALSPRX_ALPHA_LO * (uint32_t) common_db_ptr->nv_db.als_factor) / \ */
/*                                   (common_db_ptr->nv_db.visible_ratio * 10); */

/*   als_db_ptr->als_conversion_hi = (SNS_DD_ALSPRX_ALPHA_HI * (uint32_t) common_db_ptr->nv_db.als_factor) / \ */
/*                                   (common_db_ptr->nv_db.visible_ratio * 10); */

}

/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_prx_init

DESCRIPTION   initializes the AMS ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_ams_tmg399x_alsprx_prx_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  sns_dd_prx_db_type     *prx_db_ptr     = &dd_ptr->sns_dd_prx_db;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_PRX_INIT);

  prx_db_ptr->pend_state  = SNS_DD_PEND_STATE_IDLE;
  /* prx_db_ptr->last_nearby = SNS_PRX_NEAR_BY_UNKNOWN; */
}

/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_common_init

DESCRIPTION   initializes the AMS ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
void sns_dd_ams_tmg399x_alsprx_common_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  dd_ptr->dropped_logs = 0;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_STOPPED;

#ifdef ALSPRX_USE_POLLING
  /* dd_ptr->sns_dd_alsprx_common_db.als_timer.active = FALSE; */
  /* dd_ptr->sns_dd_alsprx_common_db.als_timer.defined = FALSE; */
  /* dd_ptr->sns_dd_alsprx_common_db.prx_timer.active = FALSE; */
  /* dd_ptr->sns_dd_alsprx_common_db.prx_timer.defined = FALSE; */
#endif /* ALSPRX_USE_POLLING */

  /* initialize NV item db to default values if needed */
  if (dd_ptr->sns_dd_alsprx_common_db.nv_source == SNS_DD_ALSPRX_NV_SOURCE_DEFAULT)
  {
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = SNS_DD_VISIBLE_TRANS_RATIO; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio      = SNS_DD_IR_TRANS_RATIO; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset     = SNS_DD_DC_OFFSET; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near   = SNS_DD_PRX_THRESH_NEAR; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far    = SNS_DD_PRX_THRESH_FAR; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor    = SNS_DD_PRX_FACTOR; */
    /* dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor    = SNS_DD_ALS_FACTOR; */

#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_USE_DRIVER_DEFAULTS);
#endif
  }

#ifdef ALSPRX_DEBUG_REG
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_source); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor); */
    /* SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX, */
    /*                            DBG_DD_ALSPRX_NV_PARAMS, */
    /*                            dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor); */
#endif /* ALSPRX_DEBUG_REG */

}

/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_init_data_struct

DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void sns_dd_ams_tmg399x_alsprx_init_data_struct
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  /* Initialize the common database */
  sns_dd_ams_tmg399x_alsprx_common_init(dd_ptr);

  /* Initialize ALS database */
  sns_dd_ams_tmg399x_alsprx_als_init(dd_ptr);

  /* Initialize PRX database */
  sns_dd_ams_tmg399x_alsprx_prx_init(dd_ptr);
}


/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_set_powerstate

DESCRIPTION   Sets the power state of the AMS ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_set_powerstate
(
  sns_dd_alsprx_state_t* state,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  BOOLEAN                init_data
)
{
  uint8_t          i2c_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

#ifdef DDK
  uint32_t         odr_uSec;
  static uint16_t  numTimers = 0;
#endif

  DDF_MSG_1(MED, "TMG399 var1: %d", 1126);

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
          DDF_MSG_1(MED, "ONLY PROX", 1126);
          state->chip.setup.reg.enable  = AMS_tmg399x_ENABLE_MSK_PEN;
#ifdef DDK
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PIEN;
          state->chip.setup.odr    = 20;
          /* state->chip.setup.prox.odr    = 20; */
#endif
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          DDF_MSG_1(MED, "ONLY ALS", 1126);
          state->chip.setup.reg.enable  = AMS_tmg399x_ENABLE_MSK_AEN;
#ifdef DDK
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_AIEN;
          state->chip.setup.odr     =  7;
          /* state->chip.setup.als.odr     =  7; */
#endif
      }
      else if(sensor == SNS_DDF_SENSOR__ALL)
      {
          DDF_MSG_1(MED, "CHECK IF RE-ENABLE", 1126);
          //SNS_PRINTF_MEDIUM_1(8890);
          state->chip.setup.reg.enable  = AMS_tmg399x_ENABLE_MSK_PEN;
          //SNS_PRINTF_MEDIUM_1(8891);
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_AEN;
          DDF_MSG_1(MED, "TMG399 var1: %d", 5555);

#ifdef DDK
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PIEN;
          state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_AIEN;
          state->chip.setup.odr     =  7;
          /* state->chip.setup.als.odr     =  20; */
#endif
      }

      state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_PON;
      i2c_data = state->chip.setup.reg.enable;
      status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &i2c_data);
      if ( status != SNS_DDF_SUCCESS ) {
          DDF_MSG_1(MED, "TMG399 var1: %d", 1127);
          return status;
      }
      DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1128, i2c_data);
#ifdef DDK
      // Set up a timer to read the status register to see if an interrupt has occured.
      /* if(state->chip.setup.als.odr > state->chip.setup.prox.odr) */
      /* { */
      /*     odr_uSec = ams_tmg399x_odr_to_uSec[state->chip.setup.prox.odr - 1]; */
      /* } */
      /* else */
      /* { */
      /*     odr_uSec = ams_tmg399x_odr_to_uSec[state->chip.setup.als.odr - 1]; */
      /* } */

      odr_uSec = ams_tmg399x_odr_to_uSec[state->chip.setup.odr - 1];

      if (numTimers == 0)
      {
          ++numTimers;
          status = sns_ddf_timer_start(state->sns_dd_alsprx_common_db.ddk_timer.timer_obj,
                                       odr_uSec
                                       /* MS_TO_US(50) */
                                       );
          if ( status != SNS_DDF_SUCCESS ) {
              DDF_MSG_1(MED, "TMG399 var1: %d", 1129);
              return status;
          }
      }
#endif
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
    if (init_data == TRUE)
    {
      /* cleanup */
      sns_dd_ams_tmg399x_alsprx_init_data_struct(state);
    }
#ifdef ALSPRX_DEBUG_BASIC
#error code not present
#endif

    i2c_data = AMS_tmg399x_CNTL_PWROFF;
    status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &i2c_data);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   AMS_tmg399x_rCONFIG,
                                   status);
      return status;
    }
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1130, i2c_data);

    state->chip.setup.odr = 0;

    /* state->chip.setup.prox.odr = 0; */
    /* state->chip.setup.als.odr  = 0; */

#ifdef DDK
    status = sns_ddf_timer_cancel(state->sns_dd_alsprx_common_db.ddk_timer.timer_obj);
    if ( status != SNS_DDF_SUCCESS ) {
        DDF_MSG_1(MED, "TMG399 var1: %d", 1131);
        return status;
    }
#endif

  }

  state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;

  return status;
}


sns_ddf_status_e ams_tmg399x_update_registers(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;
    /* uword_u wordVal; */
    uint8_t *sh = state->chip.shadow;


#ifdef DDK
    uint8_t byteBuf[16];
#endif

    ams_setup*       setup;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    setup  = &state->chip.setup;

#ifdef DDK
    status = ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rID, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
#endif

    // Reset the device

    // Power off
    byteVal = 0x00;
    status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    sh[TMG399X_ATIME]   = ams_tmg399x_usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us);
    sh[TMG399X_WTIME]   = 256 - state->chip.setup.wait.wtime;;

    sh[TMG399X_AILTHL]  = setup->als.thresh_low & 0xFF;
    sh[TMG399X_AILTHH]  = setup->als.thresh_low >> 8;
    sh[TMG399X_AIHTHL]  = setup->als.thresh_high & 0xFF;
    sh[TMG399X_AIHTHH]  = setup->als.thresh_high >> 8;

    sh[TMG399X_PITHL]   = setup->prox.thresh_low;
    sh[TMG399X_PITHH]   = setup->prox.thresh_high;

    sh[TMG399X_PERS]    = setup->persistence;
    sh[TMG399X_CONFIG1] = setup->reg.config;
    sh[TMG399X_PPULSE]  = setup->prox.pulse_count;

    sh[TMG399X_CONTROL] = setup->reg.control;
    sh[TMG399X_CONFIG2] = setup->reg.config2;

    status = ams_tmg399x_write_buf(state->port_handle, TMG399X_ATIME, &sh[TMG399X_ATIME], TMG399X_CONFIG2 - TMG399X_ATIME + 1);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }


    /* sh[TMG399X_ENABLE]  = state->chip.setup.reg.enable; */
    /* status = ams_tmg399x_write_byte(state->port_handle, TMG399X_ENABLE, &sh[TMG399X_ENABLE]); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */


    /* // Power off */
    /* byteVal = 0x00; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set the integration cycle time */
    /* byteVal = ams_tmg399x_usecToIcycles(setup->als.atime_us, setup->Min_Integ_Time_us); */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rATIME, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set PTIME */
    /* byteVal = ams_tmg399x_usecToIcycles(setup->prox.ptime_us, setup->Min_Integ_Time_us); */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPTIME, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set WTIME */
    /* byteVal = 256 - state->chip.setup.wait.wtime; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rWTIME, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set ALS Interrupt Threshold low */
    /* wordVal.word = setup->als.thresh_low; */
    /* status = ams_tmg399x_write_word(state->port_handle, AMS_tmg399x_rAILTL, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set ALS Interrupt Threshold high */
    /* wordVal.word = setup->als.thresh_high; */
    /* status = ams_tmg399x_write_word(state->port_handle, AMS_tmg399x_rAIHTL, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Prox Interrupt Threshold low */
    /* wordVal.word = setup->prox.thresh_low; */
    /* status = ams_tmg399x_write_word(state->port_handle, TMG399X_PITHL, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Prox Interrupt Threshold high */
    /* wordVal.word = setup->prox.thresh_high; */
    /* status = ams_tmg399x_write_word(state->port_handle, TMG399X_PITHH, &wordVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Persistence (Prox and ALS) */
    /* byteVal = setup->persistence; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPERS, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Config Reg */
    /* byteVal = setup->reg.config; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rCONFIG, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // Set Prox pulses */
    /* byteVal = setup->prox.pulse_count; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rPPULSE, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

    /* // LED drive to 100 mA, use IR (CH1) diode for prox, set AGain to 1x */
    /* byteVal = setup->reg.control; */
    /* status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rCONTROL, &byteVal); */
    /* if ( status != SNS_DDF_SUCCESS ) { */
    /*     return status; */
    /* } */

#ifdef DDK
    ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rCONTROL, &byteVal);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1132, byteVal);
#endif

    // Clear any pending interrupts
    byteVal = 0x07;
    status = ams_tmg399x_write_spec_func(state->port_handle, byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Enable Power on (PON).
    byteVal = state->chip.setup.reg.enable;
    status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal);

#ifdef DDK
    ams_tmg399x_read_buf(state->port_handle, AMS_tmg399x_rENABLE, 20, byteBuf);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1133, byteBuf[0]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1134, byteBuf[1]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1135, byteBuf[2]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1136, byteBuf[3]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1137, byteBuf[4]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1138, byteBuf[5]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1139, byteBuf[6]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1140, byteBuf[7]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1141, byteBuf[8]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1142, byteBuf[9]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1143, byteBuf[10]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1144, byteBuf[11]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1145, byteBuf[12]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1146, byteBuf[13]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1147, byteBuf[14]);
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1148, byteBuf[15]);
#endif

    return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_reset

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
sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_alsprx_state_t* state = (sns_dd_alsprx_state_t *) dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;

    DDF_MSG_0(HIGH, "sns_dd_ams_tmg399x_alsprx_reset");
    DDF_MSG_1(MED, "TMG399 var1: %d", 1149);

    sns_ddf_delay(1000);

    // Set the sensor in use
    DDF_MSG_1(MED, "TMG399 1371: %d", status);

    // Reset driver state
    status = ams_tmg399x_set_default_reg_values(state);
    if(status != SNS_DDF_SUCCESS ) {
        return status;
    }

    // Now set the usable values
    ams_tmg399x_set_reg_values_from_nv(state);

    // Set register values
    status = ams_tmg399x_update_registers(state);
    DDF_MSG_1(MED, "TMG399 reset exit: %d", status);
    return status;
}


uint32_t ams_tmg399x_als_time_us(sns_dd_alsprx_state_t* state)
{
    // ALS enabled
	DDF_MSG_1(MED, "TMG399 als time us: %d", state->chip.setup.Min_Integ_Time_us + state->chip.setup.als.atime_us);
    return state->chip.setup.Min_Integ_Time_us + state->chip.setup.als.atime_us;
}


uint32_t ams_tmg399x_prox_time_us(sns_dd_alsprx_state_t* state)
{
    // Prox enabled
	DDF_MSG_1(MED, "TMG399 prx time us: %d", 2 * state->chip.setup.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);

    return ( 2 * state->chip.setup.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);
}


uint32_t ams_tmg399x_wait_time(sns_dd_alsprx_state_t* state)
{
    uint32_t waitTime;

    waitTime   = state->chip.setup.Min_Integ_Time_us * state->chip.setup.wait.wtime;

    return (waitTime + ((state->chip.setup.wait.lTime * 11) * waitTime));
}


uint32_t ams_tmg399x_compute_total_time_us(sns_dd_alsprx_state_t* state)
{
    return (ams_tmg399x_als_time_us(state) + ams_tmg399x_prox_time_us(state) + ams_tmg399x_wait_time(state));
}


sns_ddf_status_e ams_tmg399x_write_wtime(sns_dd_alsprx_state_t* state)
{
    uint8_t byteVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    // set WEN in enable register
    if(state->chip.setup.wait.wen == 1)
    {
        state->chip.setup.reg.enable |= AMS_tmg399x_ENABLE_MSK_WEN;
        DDF_MSG_1(MED, "TMG399 WENset %d", 1);
    }
    else
    {
        state->chip.setup.reg.enable &= ~AMS_tmg399x_ENABLE_MSK_WEN;
        DDF_MSG_1(MED, "TMG399 WENset to 0 %d", 0);
    }
    byteVal = state->chip.setup.reg.enable;
    status   = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rENABLE, &byteVal);
    DDF_MSG_1(MED, "TMG399 rEnable %d", 1);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    DDF_MSG_2(MED, "TMG399 var1: %d var2: %d", 1150, byteVal);

    // Set WTIME
    byteVal = 256 - state->chip.setup.wait.wtime;
    status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rWTIME, &byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }
    DDF_MSG_2(MED, "TMG399 rWTime %d var2: %d", 1151, byteVal);

    // Set Config Reg
    if(state->chip.setup.wait.lTimeChanged)
    {
        if(state->chip.setup.wait.lTime)
        {
            state->chip.setup.reg.config |= TMG399X_CONFIG_WLONG_MSK;
            DDF_MSG_1(MED, "config retgset %d", 1);
        }
        else
        {
            state->chip.setup.reg.config &= ~TMG399X_CONFIG_WLONG_MSK;
        }

        byteVal = state->chip.setup.reg.config;
        status = ams_tmg399x_write_byte(state->port_handle, AMS_tmg399x_rCONFIG, &byteVal);
        if ( status != SNS_DDF_SUCCESS ) {
            return status;
        }
        DDF_MSG_2(MED, "TMG399 var1: %d config reg i2c %d", 1152, byteVal);
        state->chip.setup.wait.lTimeChanged = false;
    }
    return status;
}


uint32_t ams_tmg399x_odr_from_uSec(uint32_t requestedTime_us)
{
    uint16_t i;
    uint16_t foundOdr;
    uint32_t midDn;

    foundOdr = MAX_ODR - 1;

    /* if(requestedTime_us > 750000) */
    if(requestedTime_us > ODR_USEC_MAX)
    {
        return 1;
    }

    /* if(requestedTime_us < 46536) */
    if(requestedTime_us < ODR_USEC_MIN)
    {
        return MAX_ODR;
    }

    for(i = 1; i < MAX_ODR-1; i++)
    {
        /* midDn = ((ams_tmg399x_odr_to_uSec[i] - ams_tmg399x_odr_to_uSec[i-1]) / 2) - 1; */
        midDn = ((ams_tmg399x_odr_to_uSec[i-1] - ams_tmg399x_odr_to_uSec[i]) / 2) - 1;

        if(requestedTime_us > (ams_tmg399x_odr_to_uSec[i] - midDn))
        {
            foundOdr = i + 1;
            break;
        }
    }
    DDF_MSG_1(MED, "TMG399 Calculated ODR: %d", foundOdr);
    return foundOdr;
}

void ams_tmg399x_compute_new_wtime(sns_dd_alsprx_state_t* state, uint32_t requestedOdr, uint32_t time_us)
{
    uint32_t requestedOdr_us;
    uint32_t wtime_us;

    uint8_t  wtime;
    uint8_t  wen;
    uint8_t  lTime;

    bool     lTimeChanged;
    DDF_MSG_1(MED, "TMG399 req ODR: %d", requestedOdr);

    if(requestedOdr > MAX_ODR)
    {
        requestedOdr = MAX_ODR;
    }
    requestedOdr_us = ams_tmg399x_odr_to_uSec[requestedOdr - 1];

    // Check to make sure sum of als and prox is less than requestedOdr_us
    // WEN check

    wtime_us = 0;

    if(time_us > requestedOdr_us)
    {
        wtime_us = 0;
        wen      = 0;
    }
    else
    {
        wtime_us = requestedOdr_us - time_us;
        wen      = 1;
    }

    if(wtime_us > 696150)
    {
        wtime_us     = 1000000;
        wtime        = 31;
        lTime        = 1;
        lTimeChanged = true;
    }
    else
    {
        wtime        = wtime_us / state->chip.setup.Min_Integ_Time_us;
        lTime        = 0;
        lTimeChanged = true;
    }

    DDF_MSG_1(MED, "TMG399 wtimeus %d", wtime_us);
    DDF_MSG_1(MED, "TMG399 wtime %d", state->chip.setup.Min_Integ_Time_us);

    state->chip.setup.wait.wtime_us     = wtime_us;
    state->chip.setup.wait.wtime        = wtime;

    DDF_MSG_1(MED, "TMG399 wtime %d", state->chip.setup.wait.wtime);

    state->chip.setup.wait.wen          = wen;
    state->chip.setup.wait.lTime        = lTime;

    DDF_MSG_1(MED, "TMG399 ltime: %d", state->chip.setup.wait.lTime);

    state->chip.setup.wait.lTimeChanged = lTimeChanged;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the TMG 399x ALS/Prx sensor

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
static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;

    uint32_t alsTime_us;
    uint32_t proxTime_us;

    DDF_MSG_1(MED, "TMG399 var1: %d", 1153);

#ifdef ALSPRX_DEBUG_BASIC
#error code not present
#endif /* ALSPRX_DEBUG_BASIC */

    alsTime_us  = 0;
    proxTime_us = 0;

  if ( (dd_handle == NULL) ||
       (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch ( attrib )
  {
  case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      DDF_MSG_1(MED, "power mode set %d", 1154);
      status = sns_dd_ams_tmg399x_alsprx_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            sensor,
                                            TRUE);
      if(status != SNS_DDF_SUCCESS)
      {
          DDF_MSG_1(MED, "TMG399 var1: %d", 1154);
          return status;
      }

      break;

  case SNS_DDF_ATTRIB_ODR:
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
         state->chip.setup.odr = *(uint32_t*)value;
         /* state->chip.setup.prox.odr = *(uint32_t*)value; */

         DDF_MSG_1(MED, "orig prox requested odr  %d", state->chip.setup.odr);
         /* DDF_MSG_1(MED, "orig prox requested odr  %d", state->chip.setup.prox.odr); */

         /* if (state->chip.setup.prox.odr > 5) */
         if (state->chip.setup.odr > 5)
         {
             state->chip.setup.odr = 5;
             DDF_MSG_1(MED, "Set prox odr to max  %d", state->chip.setup.odr);

             /* state->chip.setup.prox.odr = 5; */
             /* DDF_MSG_1(MED, "Set prox odr to max  %d", state->chip.setup.prox.odr); */
         }

         DDF_MSG_2(MED, "TMG399 PRX ODR var1: %d var2: %d", 1155, state->chip.setup.odr);
         /* DDF_MSG_2(MED, "TMG399 PRX ODR var1: %d var2: %d", 1155, state->chip.setup.prox.odr); */
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
          state->chip.setup.odr = *(uint32_t*)value;
          DDF_MSG_2(MED, "TMG399 ALS ODR var1: %d var2: %d", 1156, state->chip.setup.odr);

          /* state->chip.setup.als.odr = *(uint32_t*)value; */
          /* DDF_MSG_2(MED, "TMG399 ALS ODR var1: %d var2: %d", 1156, state->chip.setup.als.odr); */
      }
      else
      {
          return SNS_DDF_EINVALID_ATTR;
      }


      alsTime_us  = 0;
      proxTime_us = 0;

      if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_AEN) != 0)
      {
          alsTime_us  = ams_tmg399x_als_time_us(state);
      }

      if((state->chip.setup.reg.enable & AMS_tmg399x_ENABLE_MSK_PEN) != 0)
      {
          proxTime_us = ams_tmg399x_prox_time_us(state);
      }

      if((alsTime_us > 0) && (proxTime_us > 0))
      {
          if(state->chip.setup.odr > 5)
          {
              state->chip.setup.odr = 5;
          }
      }
      ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, (alsTime_us + proxTime_us));

      /* if((state->chip.setup.prox.odr > 0) && (state->chip.setup.als.odr > 0)) */
      /* { */
      /*     if(state->chip.setup.prox.odr > 5) */
      /*     { */
      /*         state->chip.setup.prox.odr = 5; */
      /*     } */

      /*     if(state->chip.setup.als.odr > 5) */
      /*     { */
      /*         state->chip.setup.als.odr = 5; */
      /*     } */

      /*     // Both als and prox are enabled at the same time */
      /*     // Now see which is greater */
      /*     if(state->chip.setup.prox.odr > state->chip.setup.als.odr) */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.prox.odr, (alsTime_us + proxTime_us)); */
      /*     } */
      /*     else  // state->chip.setup.als.odr > state->chip.setup.prox.odr */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.als.odr, (alsTime_us + proxTime_us)); */
      /*     } */
      /* } */
      /* else  // only one or the other */
      /* { */
      /*     if(state->chip.setup.odr > 0) */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, alsTime_us); */
      /*     } */
      /*     else  // state->chip.setup.prox.odr > 0 */
      /*     { */
      /*         ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, proxTime_us); */
      /*     } */
      /* } */

      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  DDF_MSG_3(MED, "TMG399 : %d final wtime: %d final ltime: %d", 1157, state->chip.setup.wait.wtime, state->chip.setup.wait.lTime);

  status = ams_tmg399x_write_wtime(state);

  return(status);
}

//                                          0  1   2   3
static uint8_t ams_tmg399x_gain_value[] = { 1, 4, 16, 64 };

uint8_t ams_tmg399x_getAgain(sns_dd_alsprx_state_t* state)
{
    uint8_t gain;
    uint8_t retVal;

    gain = state->chip.setup.reg.control & TMG399X_AGAIN_MASK;

    retVal = ams_tmg399x_gain_value[gain];

    return retVal;
}


uint32_t ams_tmg399x_Get_Max_ALS_Count(sns_dd_alsprx_state_t* state)
{
    uint32_t retVal;

    if (state->chip.setup.als.atime_us > 63) retVal = 65535; //65k * 3 / 4
    retVal = (state->chip.setup.als.atime_us * 1024) - 1;

    return retVal;
}


/* bool ams_tmg399x_Saturated(sns_dd_alsprx_state_t* state, uint16_t data) */
/* { */
/*     uint32_t max_count; */

/*     max_count = ams_tmg399x_Get_Max_ALS_Count(state); */
/*     if (data >= max_count) */
/*     { */
/*         DDF_MSG_0(HIGH, "ALSPRX als saturation error"); */
/*         return true; */
/*     } */
/*     return false; */
/* } */


/* sns_ddf_status_e ams_tmg399x_API_APP_Get_mLUX(sns_dd_alsprx_state_t* state) */
/* { */
/*     int32_t  CPkL; */
/*     int32_t  Lux1; */
/*     int32_t  Lux2; */
/*     int32_t  DGF; */
/*     int32_t  CoefB; */
/*     int32_t  CoefC; */
/*     int32_t  CoefD; */
/*     uint16_t C0Data; */
/*     uint16_t C1Data; */
/*     uint8_t  ALS_xGain; */
/*     uint32_t ALS_Time_us; */
/*     DDF_MSG_0(HIGH, "api app get mlux"); */
/*     DGF   = state->chip.als_setup.DGF; */
/*     CoefB = state->chip.als_setup.iCoefB; */
/*     CoefC = state->chip.als_setup.iCoefC; */
/*     CoefD = state->chip.als_setup.iCoefD; */

/*     C0Data = state->chip.data.als.ch0; */
/*     C1Data = state->chip.data.als.ch1; */
/*     DDF_MSG_0(HIGH, "c0data"); */
/*     ALS_xGain   = ams_tmg399x_getAgain(state); */
/*     ALS_Time_us = state->chip.setup.als.atime_us; */
/*     DDF_MSG_0(HIGH, "als gain"); */
/*     if (C0Data == 0) return SNS_DDF_EINVALID_DATA;                    // Check bad read */
/*     DDF_MSG_0(HIGH, "good read"); */
/*     if (ams_tmg399x_Saturated(state, C1Data)) return SNS_DDF_EDATA_BOUND; // Check for saturated data */
/*     DDF_MSG_0(HIGH, "data not saturated"); */
/*     CPkL = (ALS_xGain * (int)ALS_Time_us / DGF); */
/*     //max CPkL = 120 * 700,000 / 24 = 3,500,000 / DGF */
/*     //max CPkL = 120 * 200,000 / 24 = 1,000,000 / DGF */
/*     if (CPkL == 0) CPkL = 1;                        // avoid divide by zero */
/*     Lux1 = 0; */
/*     Lux2 = 0; */

/*     Lux1 = 20 * (1000  * C0Data - CoefB * C1Data); */
/*     Lux2 = 20 * (CoefC * C0Data - CoefD * C1Data); */
/*     state->chip.data.als.Lux_Extended = false; */
/*     DDF_MSG_0(HIGH, "setting lux1 and lux 2"); */

/*     /\* if (Chip.mAPP_ALS_Setup.Lux_Extend_Enable) *\/ */
/*     /\* { *\/ */
/*     /\*     if (ams_tmg399x_CloseSaturation75(state, C0Data)) //If saturated and gain = 1x *\/ */
/*     /\*     { *\/ */
/*     /\*         Lux1 = 20 * (state->chip.data.als.Last_C1C0_Ratio1000 - CoefB) * C1Data; *\/ */
/*     /\*         Lux2 = 0; *\/ */
/*     /\*         state->chip.data.als.Lux_Extended = true; *\/ */
/*     /\*     } *\/ */
/*     /\*     else *\/ */
/*     /\*     { *\/ */
/*     /\*         if (C1Data > 0) *\/ */
/*     /\*         { *\/ */
/*     /\*             state->chip.data.als.Last_C1C0_Ratio1000 = C0Data * 1000 / C1Data; *\/ */
/*     /\*         } *\/ */
/*     /\*     } *\/ */
/*     /\* } *\/ */
/*     /\* else *\/ */
/*     { */
/*         if (ams_tmg399x_Saturated(state, C0Data)) //If saturated and gain = 1x */
/*         { */
/*             DDF_MSG_0(HIGH, "ALSPRX als saturation error"); */
/*             return SNS_DDF_EDATA_BOUND; */
/*         } */
/*     } */

/*     if (Lux2 > Lux1) Lux1 = Lux2; */
/*     if (Lux1 < 0) */
/*     { */
/*         DDF_MSG_0(HIGH, "lux1 below 0"); */
/*         return SNS_DDF_EDATA_BOUND; */
/*     } */
/*     if (Lux1 > 10000000) //10,000,000 Check to keep from overflowing 2,147,483,648 maximum 32 bit signed integer */
/*     { */
/*         Lux1 = 50 * (Lux1 / CPkL); */
/*         DDF_MSG_0(HIGH, "setting lux1 a"); */
/*     } */
/*     else */
/*     { */
/*         Lux1 = 50 * Lux1 / CPkL; */
/*         DDF_MSG_0(HIGH, "setting lux1 b"); */
/*     } */

/*     state->chip.data.als.lux = Lux1; */

/*     state->chip.data.als.Lux_OK = true; */
/*     DDF_MSG_3(MED, "als.lux: %d lux1: %d lux2: %d", state->chip.data.als.lux , Lux1, Lux2); */
/*     return SNS_DDF_SUCCESS; */
/* } */


sns_ddf_status_e ams_tmg399x_read_all_data(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;

    status = ams_tmg399x_read_buf(state->port_handle,
                                  TMG399X_STATUS,
                                  TMG399X_PDATA - TMG399X_STATUS + 1,
                                  &state->chip.shadow[TMG399X_STATUS]);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "ALSPRX als data err1");
    }

    return status;
}


sns_ddf_status_e ams_tmg399x_get_prox_data(sns_dd_alsprx_state_t* state)
{
    /* uint8_t* tBuf; */

    /* tBuf = &state->chip.shadow[TMG399X_PDATA]; */
    /* state->chip.data.prox.data = getWord(tBuf, 0); */

    //
    // Get Prox data
    //
    state->chip.data.prox.data = state->chip.shadow[TMG399X_PDATA];

    DDF_MSG_2(MED, "TMG399 out of get prox data: %d var2: %d", 1158, state->chip.data.prox.data);
    DDF_MSG_1(HIGH, "GPIO Number: %d", myGPIO);
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e ams_tmg399x_get_als_data(sns_dd_alsprx_state_t* state)
{
    uint8_t* tBuf;

    tBuf = &state->chip.shadow[TMG399X_CDATAL];
    DDF_MSG_0(HIGH, "ams_tmg399x_get_als_data:");
    //
    // Get Color data
    //
    state->chip.als_inf.clear_raw = getWord(tBuf, 0);
    state->chip.als_inf.red_raw   = getWord(tBuf, 2);
    state->chip.als_inf.green_raw = getWord(tBuf, 4);
    state->chip.als_inf.blue_raw  = getWord(tBuf, 6);

    // ir = (R + G + B - C + 1) / 2
    state->chip.als_inf.ir =
        (state->chip.als_inf.red_raw  + state->chip.als_inf.green_raw +
         state->chip.als_inf.blue_raw - state->chip.als_inf.clear_raw + 1) >> 1;

    // ir can not be negative
    if (state->chip.als_inf.ir < 0)
    {
        state->chip.als_inf.ir = 0;
    }
    DDF_MSG_1(HIGH, "state->chip.als_inf.ir %d clr: %d", state->chip.als_inf.ir);
    DDF_MSG_2(MED, "TMG399x var1: %d clr: %d", 1159, state->chip.als_inf.clear_raw);
    DDF_MSG_2(MED, "TMG399x var1: %d red: %d", 1159, state->chip.als_inf.red_raw);
    DDF_MSG_2(MED, "TMG399x var1: %d grn: %d", 1159, state->chip.als_inf.green_raw);
    DDF_MSG_2(MED, "TMG399x var1: %d blu: %d", 1159, state->chip.als_inf.blue_raw);

    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e ams_tmg399x_prox_sensor_samples(sns_dd_alsprx_state_t   *state,
                                                 sns_ddf_sensor_data_s   *data_ptr,
                                                 sns_ddf_memhandler_s    *memhandler,
                                                 sns_ddf_sensor_sample_s *sample_data)
{
    uint32_t num_samples = NUM_PROX_SAMPLES_RET;

    DDF_MSG_1(MED, "TMG399 var1: %d", 1161);

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            DDF_MSG_1(MED, "TMG399 var1: %d", 1162);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "TMG399 var1: %d", 1163);
        return SNS_DDF_EINVALID_PARAM;
    }
    DDF_MSG_1(MED, "TMG399 Faraway event num: %d", SNS_PRX_FAR_AWAY);
    DDF_MSG_1(MED, "TMG399 nearby event num: %d", SNS_PRX_NEAR_BY);
    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.data.prox.event, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = FX_FLTTOFIX_Q16(state->chip.data.prox.data);
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "PRX Data...");
    DDF_MSG_3(FATAL, "TMD PRX Data: Event:%d  Reported Data: %d  Raw Data: %d", state->chip.data.prox.event, data_ptr->samples[0].sample, state->chip.data.prox.data);
    sns_dd_ams_tmg399x_alsprx_log(state, SNS_DDF_SENSOR_PROXIMITY,
                        state->chip.data.prox.event, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.prox.data);

    DDF_MSG_1(HIGH, "GPIO Number: %d", myGPIO);
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e ams_tmg399x_als_sensor_samples(sns_dd_alsprx_state_t    *state,
                                                sns_ddf_sensor_data_s    *data_ptr,
                                                sns_ddf_memhandler_s     *memhandler,
                                                sns_ddf_sensor_sample_s  *sample_data)
{
    static uint32_t mytest;

    // Return Lux, Red, Green, Blue and Clear data
    //uint32_t num_samples = NUM_ALS_SAMPLES_RET;
    uint32_t num_samples = 3;
    uint32_t luxCorrection;
    uint32_t lux;
    mytest = 0;
    DDF_MSG_1(MED, "TMG399 var1: %d", 1164);

    if(memhandler != NULL)
    {
        data_ptr->samples = sns_ddf_memhandler_malloc(
                                                      memhandler,
                                                      num_samples * sizeof(sns_ddf_sensor_sample_s));
        if(data_ptr->samples == NULL)
        {
            DDF_MSG_1(MED, "TMG399 var1: %d", 1165);
            return SNS_DDF_ENOMEM;
        }
    }
    else if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DDF_MSG_1(MED, "TMG399 var1: %d", 1166);
        return SNS_DDF_EINVALID_PARAM;
    }

    lux           = state->chip.data.als.lux;
    luxCorrection = state->sns_dd_alsprx_common_db.nv_db.luxCorrection;
// temporarily commenting out some code
   // DDF_MSG_2(FATAL, "Lux Correction: %d  alslux now not div by 1000: %d",state->chip.setup.als.luxCorrection, alsluxby1000);
  //  DDF_MSG_2(FATAL, "1st term before +512: %d  all before div by 1024: %d", (alsluxby1000 * (state->chip.setup.als.luxCorrection)),((alsluxby1000 * (state->chip.setup.als.luxCorrection)) + 512.0));

    /* data_ptr->samples[0].sample = FX_FLTTOFIX_Q16((((lux / 1000.0) * luxCorrection) + 512.0) / 1024.0); */

    //data_ptr->samples[0].sample = FX_FLTTOFIX_Q16(((lux * luxCorrection) >> 16) / 1000.0);
    DDF_MSG_1(HIGH, "luxCorrection: %d", luxCorrection);
    data_ptr->samples[0].sample = FX_FLTTOFIX_Q16((lux * luxCorrection) / 1024.0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;

    data_ptr->samples[1].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.red_raw);
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;

    data_ptr->samples[2].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.green_raw);
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[3].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.blue_raw);
    //data_ptr->samples[3].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[4].sample = FX_FLTTOFIX_Q16(state->chip.als_inf.clear_raw);
    //data_ptr->samples[4].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[5].sample = FX_FLTTOFIX_Q16(state->chip.shadow[TMG399X_ATIME]);
    //data_ptr->samples[5].status = SNS_DDF_SUCCESS;

    //data_ptr->samples[6].sample = FX_FLTTOFIX_Q16(state->chip.params.als_gain);
    //data_ptr->samples[6].status = SNS_DDF_SUCCESS;

    data_ptr->num_samples       = num_samples;

    DDF_MSG_0(HIGH, "ALS Data...");
    DDF_MSG_2(FATAL, "TMD LIGHT Data: MLux:%d  Reported Data: %d", state->chip.data.als.lux, data_ptr->samples[0].sample);


    sns_dd_ams_tmg399x_alsprx_log(state, SNS_DDF_SENSOR_AMBIENT,
                              state->chip.data.als.lux, data_ptr->samples[0].sample, 0, 0, (uint32_t) state->chip.data.als.ch0);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_alsprx_get_data

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
static sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
    uint8_t i;
    sns_ddf_time_t        timestamp;
    sns_ddf_status_e      status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_alsprx_state_t *state = dd_handle;

    DDF_MSG_0(HIGH, "ALSPRX Get Data ");
    DDF_MSG_0(HIGH, "ALSPRX Get Data prx");

    ams_tmg399x_read_all_data(state);
    status = ams_tmg399x_get_prox_data(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err1: %d", status);
        return status;
    }

    status = sns_dd_ams_tmg399x_alsprx_prx_binary(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data prx err2: %d", status);
        return status;
    }

    status = ams_tmg399x_get_als_data(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data als err1: %d", status);
        return status;
    }

    /* status = ams_tmg399x_API_APP_Get_mLUX(state); */
    status = ams_tmg399x_get_lux(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data als err2: %d", status);
        return status;
    }

    timestamp = sns_ddf_get_timestamp();
    data_ptr  = sns_ddf_memhandler_malloc(
                                         memhandler,
                                         num_sensors * sizeof(sns_ddf_sensor_data_s));
    if(data_ptr == NULL)
    {
        DDF_MSG_1(HIGH, "ALSPRX Get Data malloc error, Size: %d", (num_sensors * sizeof(sns_ddf_sensor_data_s)));
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    DDF_MSG_0(HIGH, "ALSPRX Get Data, processing data for smgr");

    for(i = 0; i < num_sensors; i++)
    {
        data_ptr[i].sensor    = sensors[i];
        data_ptr[i].status    = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = timestamp;

        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_PROXIMITY:
            DDF_MSG_0(HIGH, "ALSPRX Get Data, returning prx data to smgr");
            ams_tmg399x_prox_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        case SNS_DDF_SENSOR_AMBIENT:
            DDF_MSG_0(HIGH, "ALSPRX Get Data, returning als data to smgr");
            ams_tmg399x_als_sensor_samples(state, &data_ptr[i], memhandler, NULL);
            break;

        default:
            DDF_MSG_1(HIGH, "Unexpected sensor type: %d", sensors[i]);
            break;
        }
    }

    return SNS_DDF_SUCCESS;
}


static void ams_tmg399x_set_lux_coefs(sns_dd_alsprx_state_t* state)
{
    int i;

    for(i = 0; (i < ARR_SIZE(state->chip.segment)) &&
               (i < ARR_SIZE(ams_tmg399x_lux_coef)); ++i)
    {
        state->chip.segment[i].d_factor  = ams_tmg399x_lux_coef[i].d_factor;
        state->chip.segment[i].r_coef    = ams_tmg399x_lux_coef[i].r_coef;
        state->chip.segment[i].g_coef    = ams_tmg399x_lux_coef[i].g_coef;
        state->chip.segment[i].b_coef    = ams_tmg399x_lux_coef[i].b_coef;
        state->chip.segment[i].ct_coef   = ams_tmg399x_lux_coef[i].ct_coef;
        state->chip.segment[i].ct_offset = ams_tmg399x_lux_coef[i].ct_offset;
    }
}


/*
 * Provides initial operational parameter defaults.
 * These defaults may be changed through the device's get/set
 * attribute files.
 */
sns_ddf_status_e ams_tmg399x_set_default_reg_values(sns_dd_alsprx_state_t* state)
{
    uint8_t devID;
    sns_ddf_status_e      status;
    uint8_t *sh = state->chip.shadow;


    /* uint16_t tmp; */
    DDF_MSG_0(HIGH, "in set defualt reg values ");

    status = ams_tmg399x_read_byte(state->port_handle, AMS_tmg399x_rID, &devID);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
        /* return status; */
    }

    switch(devID)
    {
    case 0x9C:
    case 0x9F:
        state->chip.device_index = 0;  // 3992
        break;

    case 0xA8:
    case 0xAB:
        state->chip.device_index = 1;  // 3993
        break;

    default:    // UNKNOWN device ERROR!!!!
        return SNS_DDF_EINVALID_PARAM;
        break;
    }

    sh[TMG399X_ATIME]    = 0x6B; /* 405ms */
    sh[TMG399X_WTIME]    =    0;
    sh[TMG399X_PERS]     = PRX_PERSIST(1) | ALS_PERSIST(2);
    sh[TMG399X_CONFIG1]  = TMG399X_CONFIG_DEFAULT_VALUE;
    sh[TMG399X_PPULSE]   =   10;

    state->chip.params.als_gain = 1;   // ALS Gain = 16
    state->chip.als_gain_auto   = TRUE;   // true;


    /* /\* Operational parameters *\/ */
    ams_tmg399x_set_lux_coefs(state);

    state->chip.setup.Min_Integ_Time_us = ams_tmg399x_min_integ_time_us[state->chip.device_index];

    /* must be a multiple of 50mS */
    state->chip.setup.als.atime_us = MS_TO_US(50);

    /* Min Proximity Integration time us */
    state->chip.setup.prox.ptime_us = MS_TO_US(1);   //state->chip.setup.Min_Integ_Time_us;

    /* this is actually an index into the gain table */
    /* assume clear glass as default */

    DDF_MSG_0(HIGH, "in set defualt reg values -gain ");
    /* default gain trim to account for aperture effects */
    state->chip.setup.als.gain_trim = 1000;

    /* Known external ALS reading used for calibration */
    state->chip.setup.als.cal_target = 130;

    /* CH0 'low' count to trigger interrupt */
    state->chip.setup.als.thresh_low = 0;

    /* CH0 'high' count to trigger interrupt */
    state->chip.setup.als.thresh_high = 0;

    /* Number of 'out of limits' ADC readings */
    /* 3 Consecutive prox values out of range before generating prox interrupt */
    /* Will generate an ALS interrupt after every ALS cycle. */
    state->chip.setup.persistence = PRX_PERSIST(1) | ALS_PERSIST(3);

    /* Default interrupt(s) enabled.
     * 0x00 = none, 0x10 = als, 0x20 = prx 0x30 = bth */
    state->chip.setup.interrupts_en = 0x00;
    state->chip.setup.reg.enable    = AMS_tmg399x_ENABLE_MSK_PON;

    /*default threshold (adjust either manually or with cal routine*/
    state->chip.setup.prox.detect  = PROX_DETECT;
    state->chip.setup.prox.release = PROX_RELEASE;

    /* wait time between prox and als */
    state->chip.setup.wait.wtime = 1;
    state->chip.setup.wait.lTime = 0;

    /* Config register */
    state->chip.setup.reg.config = TMG399X_CONFIG_DEFAULT_VALUE;

    DDF_MSG_0(HIGH, "in set defualt reg values -prox detect ");

    /* Set the prox thresh for detect */
    state->chip.setup.prox.thresh_low  = 0x0000;
    state->chip.setup.prox.thresh_high = state->chip.setup.prox.detect;
    state->chip.data.prox.event        = SNS_PRX_FAR_AWAY;

    state->chip.setup.als.luxCorrection = 1 << 10;

    state->chip.setup.prox.max_samples_cal = 100;
    state->chip.setup.prox.pulse_count     = 8;
    state->chip.setup.odr                  = 0;

    //(0 << 6) | (1 << 5) | (0 << 4) | (0 << 2) | (1);
    state->chip.setup.reg.control = TMG399X_LDRIVE_100pc | TMG399X_PGAIN_4 | TMG399X_AGAIN_4;
    state->chip.setup.reg.config2 = TMG399X_LEDBOOTST_100 | (1);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      ams_tmg399x_set_reg_values_from_nv

DESCRIPTION   Reads values from registry NV param store and assigns to sensor

DEPENDENCIES  None

RETURN VALUE  No Return

SIDE EFFECT   None

===========================================================================*/


void ams_tmg399x_set_reg_values_from_nv(sns_dd_alsprx_state_t* state)
{
    // Now set NV values

    // device
    SNS_PRINTF_MEDIUM_2(1168, state->sns_dd_alsprx_common_db.nv_db.device);
    if((state->sns_dd_alsprx_common_db.nv_db.device != 0) && (state->sns_dd_alsprx_common_db.nv_db.device <= 9))
    {
        /* state->chip.ams_sensor = (AMS_TMG399X_SENSOR_TYPE)state->sns_dd_alsprx_common_db.nv_db.device; */
    }

    // DGF
    SNS_PRINTF_MEDIUM_2(1168, state->sns_dd_alsprx_common_db.nv_db.DGF);
    if(state->chip.als_setup.DGF != state->sns_dd_alsprx_common_db.nv_db.DGF)
    {
        state->chip.als_setup.DGF = state->sns_dd_alsprx_common_db.nv_db.DGF;
    }

    // CoefB
    SNS_PRINTF_MEDIUM_2(1169, state->sns_dd_alsprx_common_db.nv_db.iCoefB);
    if(state->chip.als_setup.iCoefB != state->sns_dd_alsprx_common_db.nv_db.iCoefB)
    {
        state->chip.als_setup.iCoefB = state->sns_dd_alsprx_common_db.nv_db.iCoefB;
    }

    // CoefC
    SNS_PRINTF_MEDIUM_2(1170, state->sns_dd_alsprx_common_db.nv_db.iCoefC);
    if(state->chip.als_setup.iCoefC != state->sns_dd_alsprx_common_db.nv_db.iCoefC)
    {
        state->chip.als_setup.iCoefC = state->sns_dd_alsprx_common_db.nv_db.iCoefC;
    }

    // CoefD
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.iCoefD);
    if(state->chip.als_setup.iCoefD != state->sns_dd_alsprx_common_db.nv_db.iCoefD)
    {
        state->chip.als_setup.iCoefD = state->sns_dd_alsprx_common_db.nv_db.iCoefD;
    }

    // proxDetect
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.proxDetect);
    if(state->chip.setup.prox.detect != state->sns_dd_alsprx_common_db.nv_db.proxDetect)
    {
        state->chip.setup.prox.detect = state->sns_dd_alsprx_common_db.nv_db.proxDetect;
    }

    // proxRelease
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.proxRelease);
    if(state->chip.setup.prox.release != state->sns_dd_alsprx_common_db.nv_db.proxRelease)
    {
        state->chip.setup.prox.release = state->sns_dd_alsprx_common_db.nv_db.proxRelease;
    }

    // luxCorrection
    SNS_PRINTF_MEDIUM_2(1171, state->sns_dd_alsprx_common_db.nv_db.luxCorrection);
    if(state->chip.setup.als.luxCorrection != state->sns_dd_alsprx_common_db.nv_db.luxCorrection)
    {
        state->chip.setup.als.luxCorrection = state->sns_dd_alsprx_common_db.nv_db.luxCorrection;
        DDF_MSG_1(MED, "lux corr written in default %d", state->chip.setup.als.luxCorrection);
    }


}


#define NUM_OF_LUX_TO_AVE  8


static sns_ddf_status_e ams_tmg399x_compute_lux_ave(sns_dd_alsprx_state_t* state)
{
    uint32_t         luxSum;
    int              i;
    sns_ddf_status_e status;

    // Set the device registers - just reset earlier
    /* status = sns_dd_ams_tmg399x_alsprx_reset(state); */
    /* if( status != SNS_DDF_SUCCESS ) */
    /* { */
    /*     SNS_PRINTF_MEDIUM_1(1172); */
    /* } */

    luxSum = 0;

    DDF_MSG_0(HIGH, "about to get als data and mlux ");

    for(i = 0; i < NUM_OF_LUX_TO_AVE; ++i)
    {
        status = ams_tmg399x_read_all_data(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DDF_MSG_0(HIGH, "failed to read data ");
            /* return status; */
        }

        status = ams_tmg399x_get_als_data(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DDF_MSG_0(HIGH, "failed in get als data ");
            /* return status; */
        }

        /* status = ams_tmg399x_API_APP_Get_mLUX(state); */
        status = ams_tmg399x_get_lux(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DDF_MSG_0(HIGH, "failed in app get mlux ");
            /* return status; */
        }

        luxSum += state->chip.data.als.lux;
        DDF_MSG_2(MED, "summing lux luxsum %d curent loop: %d", luxSum, i);

        // Wait some time before another reading
        sns_ddf_delay(15000);
        DDF_MSG_0(HIGH, "after 15000 us delay ");
    }

    state->chip.data.als.luxAve = (luxSum / NUM_OF_LUX_TO_AVE) / 1000;
    DDF_MSG_2(MED, "luxsum %d luxsum/8/1000: %d", luxSum, ((luxSum / NUM_OF_LUX_TO_AVE)/1000));

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      ams_tmg399x_calibrate_als

DESCRIPTION   Called from self-test. compares avg lux received to avg lux
              expected and calculates scaling factor.

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/


sns_ddf_status_e ams_tmg399x_calibrate_als(sns_dd_alsprx_state_t* state)
{
    sns_ddf_status_e status;

    uint32_t  tgtLux;
    uint32_t  avgLux;

    avgLux = 1;
    tgtLux = 1;

    // compare the average lux to the passed in calibration lux,
    DDF_MSG_0(HIGH, "in calibrate als next compute lux ");
    status = ams_tmg399x_compute_lux_ave(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DDF_MSG_0(HIGH, "computelux ave failed ");
        return status;
    }
    DDF_MSG_0(HIGH, "out of compute lux ");

    // Find Lux mid way between calLuxLower and calLuxUpper
    tgtLux = (state->sns_dd_alsprx_common_db.nv_db.calLuxLower + state->sns_dd_alsprx_common_db.nv_db.calLuxUpper) / 2;
    DDF_MSG_2(MED, "tgtLux: %d callxlw: %d", tgtLux, state->sns_dd_alsprx_common_db.nv_db.calLuxLower);
    //tgtLux= tgtLux;    // removed left shift by 1024
    avgLux = state->chip.data.als.luxAve;

    DDF_MSG_2(MED, "tgtLux<<10: %d callxhi: %d", tgtLux, state->sns_dd_alsprx_common_db.nv_db.calLuxUpper);

    // Mult by 2^10
    state->sns_dd_alsprx_common_db.nv_db.luxCorrection = ((tgtLux << 10) / avgLux);

    /* if((state->sns_dd_alsprx_common_db.nv_db.luxCorrection) < 1) */
    /* { */
    /*     state->sns_dd_alsprx_common_db.nv_db.luxCorrection = 1;  //1 is minimum */
    /* } */

    state->chip.setup.als.luxCorrection = state->sns_dd_alsprx_common_db.nv_db.luxCorrection;

    DDF_MSG_2(MED, "avgLux: %d new val of state->sns_dd_alsprx_common_db.nv_db.luxCorrection: %d", avgLux, state->sns_dd_alsprx_common_db.nv_db.luxCorrection);

    // To use:
    // ((lux * state->sns_dd_alsprx_common_db.nv_db.luxCorrection) + 512) / 1024;
    // or
    // ((lux * state->sns_dd_alsprx_common_db.nv_db.luxCorrection) + 512) >> 10;

    return status;
}

/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_alsprx_run_test

DESCRIPTION   OEM Self Test to calibrate ALS Lux. Takes several lux readings
              and calculates scaling factor for ALS reading

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/

sns_ddf_status_e sns_dd_ams_tmg399x_alsprx_run_test
( sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
)
{
    sns_dd_alsprx_state_t *state  = dd_handle;
    sns_ddf_status_e       status = SNS_DDF_SUCCESS;
    sns_ddf_handle_t       smgr_handle;
    uint32_t alsTime_us;
    uint32_t proxTime_us;

    if(sensor == SNS_DDF_SENSOR_AMBIENT)
    {
        DDF_MSG_0(HIGH, "in selftest sensor_ambient ");

        // Test the ALS
        if(test == SNS_DDF_TEST_OEM)
        {
            DDF_MSG_0(HIGH, "in selftest sensor_ambient OEM test ");
            // Perform ams specific test to Calibrate ALS
            if(state->sns_dd_alsprx_common_db.nv_db.calibratePhone)
            {
                // Set Default values
                //fv Read NV
                //fv ams_tmg399x_set_default_reg_values(state);

                status = sns_dd_ams_tmg399x_alsprx_reset(state);
                DDF_MSG_0(HIGH, "after reset");
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "set def reg values failed ");
                    return status;
                }

                alsTime_us  = 0;
                proxTime_us =0;

                state->chip.setup.odr = 10;
                state->chip.setup.odr = 10;

                /* state->chip.setup.als.odr = 10; */
                /* state->chip.setup.prox.odr = 10; */

                //  enable als and prox. sensor
                status = sns_dd_ams_tmg399x_alsprx_set_powerstate(dd_handle, SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);


                alsTime_us  = ams_tmg399x_als_time_us(state);
                proxTime_us = ams_tmg399x_prox_time_us(state);
                ams_tmg399x_compute_new_wtime(state, state->chip.setup.odr, alsTime_us);
                /* ams_tmg399x_compute_new_wtime(state, state->chip.setup.als.odr, alsTime_us); */

                //ams_tmg399x_compute_new_wtime(state, state->chip.setup.prox.odr, proxTime_us);
                status = ams_tmg399x_write_wtime(state);
                sns_ddf_delay(125000);

                // update als scaling
                DDF_MSG_0(HIGH, "into als scaling");
                status = ams_tmg399x_calibrate_als(state);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "calib als failed ");
                    return status;
                }
                DDF_MSG_0(HIGH, "out of als scaling");
                smgr_handle = state->smgr_handle;
                DDF_MSG_0(HIGH, "reg update notify next");
                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    SNS_PRINTF_MEDIUM_1(100010);
                    return status;
                }
                DDF_MSG_0(HIGH, "successful reg update notify");
            }
        }
    }

    return SNS_DDF_SUCCESS;
}


static void ams_tmg399x_calc_cpl(ams_tmg399x_chip *chip)
{
    uint32_t cpl;
    uint32_t sat;
    uint8_t  atime = chip->shadow[TMG399X_ATIME];

    cpl  = 256 - chip->shadow[TMG399X_ATIME];
    /* cpl *= INTEGRATION_CYCLE; */
    cpl *= chip->setup.Min_Integ_Time_us;
    cpl /= 1000;  // 100;
    cpl *= ams_tmg399x_als_gains[chip->params.als_gain];

    /* sat = min_t(uint32_t, MAX_ALS_VALUE, (uint32_t)(256 - atime) << 10); */
    sat = min(MAX_ALS_VALUE, (uint32_t)(256 - atime) << 10);
    sat = sat * 8 / 10;
    /* dev_info(&chip->client->dev, */
    /* 		"%s: cpl = %u [gain %u, sat. %u]", */
    /* 		__func__, cpl, */
    /* 		ams_tmg399x_als_gains[chip->params.als_gain], sat); */
    chip->als_inf.cpl = cpl;
    chip->als_inf.saturation = sat;
}


/* static int ams_tmg399x_set_als_gain(ams_tmg399x_chip *chip, int gain) */
static sns_ddf_status_e ams_tmg399x_set_als_gain(sns_dd_alsprx_state_t* state, int gain)
{
    /* int rc; */
    ams_tmg399x_chip *chip = &state->chip;
    uint8_t ctrl_reg   = chip->shadow[TMG399X_CONTROL] & ~TMG399X_AGAIN_MASK;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    DDF_MSG_0(HIGH, "ams_tmg399x_set_als_gain");
    switch (gain) {
    case 1:
        ctrl_reg |= TMG399X_AGAIN_1;
        break;
    case 4:
        ctrl_reg |= TMG399X_AGAIN_4;
        break;
    case 16:
        ctrl_reg |= TMG399X_AGAIN_16;
        break;
    case 60:
    case 64:
        ctrl_reg |= TMG399X_AGAIN_64;
        break;
    default:
        /* dev_err(&chip->client->dev, "%s: wrong als gain %d\n", */
        /* 		__func__, gain); */
        return SNS_DDF_EINVALID_PARAM;
    }

    status = ams_tmg399x_write_byte(state->port_handle, TMG399X_CONTROL, &ctrl_reg);
    if ( status == SNS_DDF_SUCCESS )
    {
        chip->shadow[TMG399X_CONTROL] = ctrl_reg;
        chip->params.als_gain = (ctrl_reg & TMG399X_AGAIN_MASK);
    }
    return status;

    /* rc = ams_tmg399x_i2c_write(chip, TMG399X_CONTROL, ctrl_reg); */
    /* if (!rc) { */
    /* 	chip->shadow[TMG399X_CONTROL] = ctrl_reg; */
    /* 	chip->params.als_gain = ctrl_reg; */
    /* 	/\* dev_info(&chip->client->dev, "%s: new gain %d\n", *\/ */
    /* 	/\* 		__func__, gain); *\/ */
    /* } */

    /* return rc; */
}


sns_ddf_status_e ams_tmg399x_get_lux(sns_dd_alsprx_state_t* state)
{
    uint32_t rp1, gp1, bp1, cp1;
    uint32_t lux = 0;
    uint32_t cct;
    /* int ret; */

    ams_tmg399x_chip *chip;
    chip = &state->chip;

    uint32_t sat = chip->als_inf.saturation;
    uint32_t sf;
    DDF_MSG_0(HIGH, "ams_tmg399x_get_lux ");
    /* use time in ms get scaling factor */
    ams_tmg399x_calc_cpl(chip);
    DDF_MSG_1(HIGH, "chip->als_inf.saturation=%d",chip->als_inf.saturation);
    if (!chip->als_gain_auto)
    {
        DDF_MSG_0(HIGH, "!chip->als_gain_auto");
        if (chip->als_inf.clear_raw <= MIN_ALS_VALUE)
        {   DDF_MSG_2(HIGH, "chip->als_inf.clear_raw=%d MIN_ALS_VALUE=%d",chip->als_inf.clear_raw,MIN_ALS_VALUE);
            /* dev_info(&chip->client->dev, */
            /*          "%s: darkness\n", __func__); */
            lux = 0;
            goto exit;
        }
        else if (chip->als_inf.clear_raw >= sat)
        {
          DDF_MSG_2(HIGH, "chip->als_inf.clear_raw=%d sat=%d",chip->als_inf.clear_raw,sat);
          DDF_MSG_1(HIGH, "chip->als_inf.lux=%d",chip->als_inf.lux);
          /* dev_info(&chip->client->dev, */
          /*          "%s: saturation, keep lux & cct\n", __func__); */
          lux = chip->als_inf.lux;
          goto exit;
        }
    }
    else
    {
        DDF_MSG_0(HIGH, "chip->als_gain_auto");
        uint8_t gain = ams_tmg399x_als_gains[chip->params.als_gain];
        /* int rc = -EIO; */
        sns_ddf_status_e rc = SNS_DDF_SUCCESS;

        if (gain == 16 && chip->als_inf.clear_raw >= sat)
        {
            DDF_MSG_0(HIGH, "gain == 16 && chip->als_inf.clear_raw >= sat");
            rc = ams_tmg399x_set_als_gain(state, 1);
            /* rc = ams_tmg399x_set_als_gain(chip, 1); */
        }
        else if (gain == 16 &&
                 chip->als_inf.clear_raw < GAIN_SWITCH_LEVEL)
        {
            rc = ams_tmg399x_set_als_gain(state, 60);
            /* rc = ams_tmg399x_set_als_gain(chip, 60); */
        }
        else if ((gain == 60 &&
                  chip->als_inf.clear_raw >= (sat - GAIN_SWITCH_LEVEL)) ||
                 (gain == 1 &&
                  chip->als_inf.clear_raw > GAIN_SWITCH_LEVEL))
        {
            rc = ams_tmg399x_set_als_gain(state, 16);
            /* rc = ams_tmg399x_set_als_gain(chip, 16); */
        }

        if (rc != SNS_DDF_SUCCESS)
        {
            /* dev_info(&chip->client->dev, "%s: gain adjusted, skip\n", */
            /*          __func__); */
            ams_tmg399x_calc_cpl(chip);
            /* ret = -EAGAIN; */
            lux = chip->als_inf.lux;
            goto exit;
        }

        if (chip->als_inf.clear_raw <= MIN_ALS_VALUE)
        {
            /* dev_info(&chip->client->dev, */
            /*          "%s: darkness\n", __func__); */
            lux = 0;
            goto exit;
        }
        else if (chip->als_inf.clear_raw >= sat)
        {
            /* dev_info(&chip->client->dev, "%s: saturation, keep lux\n", */
            /*          __func__); */
            lux = chip->als_inf.lux;
            goto exit;
        }
    }

    /* remove ir from counts*/
    rp1 = chip->als_inf.red_raw   - chip->als_inf.ir;
    gp1 = chip->als_inf.green_raw - chip->als_inf.ir;
    bp1 = chip->als_inf.blue_raw  - chip->als_inf.ir;
    cp1 = chip->als_inf.clear_raw - chip->als_inf.ir;
    DDF_MSG_1(HIGH, "rp1=%d",rp1);
    DDF_MSG_1(HIGH, "gp1=%d",gp1);
    DDF_MSG_1(HIGH, "bp1=%d",bp1);
    DDF_MSG_1(HIGH, "cp1=%d",cp1);
    DDF_MSG_1(HIGH, "chip->als_inf.ir=%d",chip->als_inf.ir);

    if (!chip->als_inf.cpl)
    {
        /* dev_info(&chip->client->dev, "%s: zero cpl. Setting to 1\n", */
        /*          __func__); */
        chip->als_inf.cpl = 1;
    }

    if (chip->als_inf.red_raw > chip->als_inf.ir)
    {
        DDF_MSG_0(HIGH, "chip->als_inf.red_raw > chip->als_inf.ir");
        lux += chip->segment[chip->device_index].r_coef * rp1;
    }
    else
    {
        /* dev_err(&chip->client->dev, "%s: lux rp1 = %d\n", */
        /* 	__func__, */
        /* 	(chip->segment[chip->device_index].r_coef * rp1)); */
    }

    if (chip->als_inf.green_raw > chip->als_inf.ir)
    {
        DDF_MSG_0(HIGH, "chip->als_inf.green_raw > chip->als_inf.ir");
        lux += chip->segment[chip->device_index].g_coef * gp1;
    }
    else
    {
        /* dev_err(&chip->client->dev, "%s: lux gp1 = %d\n", */
        /* 	__func__, */
        /* 	(chip->segment[chip->device_index].g_coef * rp1)); */
    }

    if (chip->als_inf.blue_raw > chip->als_inf.ir)
    {
        DDF_MSG_0(HIGH, "chip->als_inf.blue_raw > chip->als_inf.ir");
        lux -= chip->segment[chip->device_index].b_coef * bp1;
    }
    else
    {
        /* dev_err(&chip->client->dev, "%s: lux bp1 = %d\n", */
        /* 	__func__, */
        /* 	(chip->segment[chip->device_index].b_coef * rp1)); */
    }

    sf = chip->als_inf.cpl;

    if (sf > 131072)  // 0x2_0000
    {
        goto error;
    }

    lux /= sf;
    lux *= chip->segment[chip->device_index].d_factor;
    lux += 512;
    lux >>= 10;

    // Save as a 32 bit int
    //chip->als_inf.lux = (uint16_t) lux;
    cct = ((chip->segment[chip->device_index].ct_coef * bp1) / rp1) +
        chip->segment[chip->device_index].ct_offset;

    chip->als_inf.cct = (uint16_t) cct;

 exit:

    chip->als_inf.lux  = lux;
    chip->data.als.lux = lux;

    return SNS_DDF_SUCCESS;
    /* return 0; */

 error:
    /* dev_err(&chip->client->dev, "ERROR Scale factor = %d", sf); */

    return SNS_DDF_EINVALID_DATA;
    /* return 1; */
}
