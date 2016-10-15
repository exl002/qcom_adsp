/********************************************************************************
* Copyright (c) 2014, "ams AG"
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
/******************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
* THIS SOFTWARE IS PROVIDED FOR USE ONLY IN CONJUNCTION WITH AMS PRODUCTS.
* USE OF THE SOFTWARE IN CONJUNCTION WITH NON-AMS-PRODUCTS IS EXPLICITLY
* EXCLUDED.
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
02/15/14     fv      Convert tmd37x2 driver to tmg399x
11/14/13     fv      Convert tmd377x driver to tmd37x2
06/10/13     fv      Convert tmd277x driver to tmd377x
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
12/11/09     fv      Convert Intersil driver to drive AMS TSL/TMD 377x ALS/PROX sensor
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "stdbool.h"
#include <string.h>
#include "sns_log_api.h"
#include "sns_log_types.h"

#include "ams_tmg399x.h"
#include "sns_dd_ams_tmg399x_priv.h"
#include "ams_i2c.h"

#ifdef USE_AMS_GESTURE
  #include "cal_offsets.h"
#endif

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
//
// CHECK THIS TABLE
//
//
// Convert ODR in Hz to uSec
//
#define MAX_ODR  40

static uint32_t ams_odr_to_uSec[] = {
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


//
//++ TMG399x ++
//
#define MAX_NUM_OF_SENSORS    5

static enum state_machine_sensors_e currentSensor;

static uint16_t currentState;
static bool     currentlyActiveSensors[MAX_NUM_OF_SENSORS];  // Als, Prox, Gesture, RGB, CT
static uint16_t alsBeingUsed = 0;


  //                      Enable SENSOR
  //==================================================================
  //    ALS                    PRX                    GES                 // STATE
  //--------------------  ---------------------  ---------------------       --------------
static uint8_t enableStates[8][3] =
  {
  {AMS_STATE_ALS,         AMS_STATE_PRX,         AMS_STATE_GES},          // OFF
  {AMS_STATE_ALS,         AMS_STATE_ALS_PRX,     AMS_STATE_GES_ALS},      // ALS
  {AMS_STATE_ALS_PRX,     AMS_STATE_PRX,         AMS_STATE_GES_PRX},      // PRX
  {AMS_STATE_ALS_PRX,     AMS_STATE_ALS_PRX,     AMS_STATE_GES_ALS_PRX},  // ALS + PRX
  {AMS_STATE_GES_ALS,     AMS_STATE_GES_PRX,     AMS_STATE_GES},          // GES
  {AMS_STATE_GES_ALS,     AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_ALS},      // GES + ALS
  {AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_PRX,     AMS_STATE_GES_PRX},      // GES + PRX
  {AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_ALS_PRX, AMS_STATE_GES_ALS_PRX},  // GES + ALS + PRX
  };


  //                     Disable SENSOR
  //==================================================================
  //    ALS                    PRX                    GES                 // STATE
  //--------------------  ---------------------  ---------------------       --------------
static uint8_t disableStates[8][3] =
  {
  {AMS_STATE_OFF,         AMS_STATE_OFF,         AMS_STATE_OFF},          // OFF
  {AMS_STATE_OFF,         AMS_STATE_ALS,         AMS_STATE_ALS},          // ALS
  {AMS_STATE_PRX,         AMS_STATE_OFF,         AMS_STATE_PRX},          // PRX
  {AMS_STATE_PRX,         AMS_STATE_ALS,         AMS_STATE_ALS_PRX},      // ALS + PRX
  {AMS_STATE_GES,         AMS_STATE_GES,         AMS_STATE_OFF},          // GES
  {AMS_STATE_GES,         AMS_STATE_GES_ALS,     AMS_STATE_ALS},          // GES + ALS
  {AMS_STATE_GES_PRX,     AMS_STATE_GES,         AMS_STATE_PRX},          // GES + PRX
  {AMS_STATE_GES_PRX,     AMS_STATE_GES_ALS,     AMS_STATE_ALS_PRX},      // GES + ALS + PRX
  };


static uint8_t const restorable_regs[] = {
    AMS_REG_ATIME,    // TMG399x_ALS_TIME,
    AMS_REG_PERS,     // TMG399x_PERSISTENCE,
    AMS_REG_PPULSE,   // TMG399x_PRX_PULSE_COUNT,
    AMS_REG_CONTROL,  // TMG399x_GAIN,
                      // TMG399x_POFFSET_NE,
};


enum numParamsToReturn_e {
    AMBIENT_PARAMS,
    PROX_PARAMS,
    RGB_PARAMS,
    CTEMP_CLR_PARAMS
};


#define NUM_ALS_SAMPLES_RET       2
#define NUM_PROX_SAMPLES_RET      2
#define NUM_RGB_SAMPLES_RET       3
#define NUM_CTEMP_CLR_SAMPLES_RET 2

static uint16_t numParamsToReturn[] = {
    NUM_ALS_SAMPLES_RET,
    NUM_PROX_SAMPLES_RET,
    NUM_RGB_SAMPLES_RET,
    NUM_CTEMP_CLR_SAMPLES_RET
};


// ** TEMP  **
static uint8_t device_index;
// ** TEMP  **

#define AGAIN1   0
#define AGAIN4   1
#define AGAIN16  2
#define AGAIN64  3

//                                  0  1   2   3
/* static uint8_t ams_gain_value[] = { 1, 4, 16, 60 }; */

int32_t subDevId_AlsProx = -1;
int32_t subDevId_RgbCt   = -1;
int32_t subDevId_Gesture = -1;

#ifdef USE_AMS_GESTURE
#define GESTURE_FIFO_DATA_MAX_ELEMENTS  128
static uint8_t   gestureFifoData[GESTURE_FIFO_DATA_MAX_ELEMENTS];
static uint32_t  sumOfFifoBytesRead;

static bool      doCalibration;
static uint8_t   enableRegCal;
static uint8_t   gconfig4RegCal;
#endif  // USE_AMS_GESTUER

static bool      firstProx;

#define MAX_CYCLE_TIME_ms    (45)  // 50.0
#define ALS_INTEG_TIME_ms    (25)
#if(MAX_CYCLE_TIME_ms < ALS_INTEG_TIME_ms)
  #error MAX CYCLE TIME _must_ be greater than ALS INTEG TIME
#endif
/*
 *  +-------------+      +
 *  | integration | wait |  x N
 *  | time        | time | o o o
 *--+             +------+
 *  ^                    ^
 *  |<- Max Cycle time ->|
 *  |<- ODR (MAX_CYCLE_TIME_ms * N) ->|
 *
 * integration time: 1 .. 256 integration cycles of 2.78 ms.
 * wait time: 0 .. 256, used to reduse current consumption, when WEN
 *            is enabled.
 *
 */

#define AMS_APERS_VAL   2 // 5

//
//-- TMG399x --
//

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_ams_tmg399x_init
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

static sns_ddf_status_e sns_dd_ams_tmg399x_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_ams_tmg399x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_ams_tmg399x_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_ams_tmg399x_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

void sns_dd_ams_tmg399x_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

sns_ddf_status_e sns_dd_ams_tmg399x_reset
(
  sns_ddf_handle_t dd_handle
);


sns_ddf_status_e sns_dd_ams_tmg399x_run_test
(
 sns_ddf_handle_t  dd_handle,
 sns_ddf_sensor_e  sensor,
 sns_ddf_test_e    test,
 uint32_t*         err
);


sns_ddf_status_e sns_dd_ams_tmg399x_enable_sched_data
(
 sns_ddf_handle_t state,
 sns_ddf_sensor_e sensor,
 bool             enable
 );


// Combo Sensor

sns_ddf_status_e sns_dd_read_registry
(
 sns_dd_state_t*      state,
 uint32_t             sub_dev_id,
 sns_ddf_nv_params_s* reg_params
 );

sns_ddf_status_e sns_dd_ams_tmg399x_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
 );

// Combo Sensor

sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list =
{
  &sns_dd_ams_tmg399x_init,
  &sns_dd_ams_tmg399x_get_data,
  &sns_dd_ams_tmg399x_set_attrib,
  &sns_dd_ams_tmg399x_get_attrib,
  &sns_dd_ams_tmg399x_handle_timer,
  &sns_dd_ams_tmg399x_handle_irq,
  &sns_dd_ams_tmg399x_reset,
  &sns_dd_ams_tmg399x_run_test,  /* NULL,  self test */
  &sns_dd_ams_tmg399x_enable_sched_data,
  &sns_dd_ams_tmg399x_probe          // New for combo driver
};


#define REG_FIXED_NV_SIZE_128      128


//
// +++++++++++++++++++  AMS-TAOS USA Code   +++++++++++++++++++
//

//
// ------------------  AMS-TAOS USA Code   ------------------
//
// The following functions based on Sample code v 2.0.6
//

static void ams_update_als_thresh
(
 sns_dd_state_t* state,
 uint16_t        als_low,
 uint16_t        als_high,
 uint8_t         apers
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_MSG_3(HIGH, "ams_update_als_thresh low=%d high=%d apers=%d", als_low, als_high, apers);

    // Set ALS Interrupt Threshold low
    status = ams_i2cWriteField_r(state, AILTH, als_low);
    if ( status != SNS_DDF_SUCCESS ) {
        return;
    }

    // Set ALS Interrupt Threshold high
    status = ams_i2cWriteField_r(state, AIHTH, als_high);
    if ( status != SNS_DDF_SUCCESS ) {
        return;
    }

    ams_i2cWriteField_r(state, APERS, apers);
}

static sns_ddf_status_e ams_initSensorPor
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;


    state->chip.shadow.val[AMS_REG_ENABLE] = 0x00;

    return status;
}


/* static sns_ddf_status_e ams_initSensorPor */
/* ( */
/*  sns_dd_state_t*  state */
/* ) */
/* { */
/*     sns_ddf_status_e status = SNS_DDF_SUCCESS; */

/*     //status = ams_i2c_write_byte(state, AMS_REG_ENABLE, 0x00); */
/*     ams_setField_s(state, LEDBOOST, BOOST_150); */
/*     ams_setField_s(state, PGAIN,     PGAIN_1X); */
/*     ams_setField_s(state, GGAIN,     GGAIN_4X); */
/*     ams_setField_s(state, GIEN,             1); */
/*     status = ams_i2cWriteChangedShadowRegisters(state); */

/*     return status; */
/* } */


static sns_ddf_status_e ams_initSensorAls
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, ATIME, ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));       // 200
    ams_setField_s(state, WTIME, ams_usecToRegVal(state, MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)));  // 300

    // DEBUG
    // Set APERS to 0 to generate an interrupt on every cycle
    /* ams_setField_s(state, PPERS, 2); */
    /* ams_setField_s(state, APERS, 2); */
    ams_setField_s(state, PPERS, 2);
    ams_setField_s(state, APERS, 0);
    status = ams_i2cWriteChangedShadowRegisters(state); if(SNS_DDF_SUCCESS != status) return status;

    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_AIEN | AMS_MSK_AEN | AMS_MSK_PON);

    if((MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)) != 0)
    {
        state->chip.shadow.val[AMS_REG_ENABLE] |= AMS_MSK_WEN;
    }

    return status;
}


static sns_ddf_status_e ams_initSensorProx
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, PITHL,  0xff);
    ams_setField_s(state, PITHH,  0x00);
    ams_setField_s(state, PPERS,  0);
    ams_setField_s(state, APERS,  2);
    ams_setField_s(state, ATIME, ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));  // 25
    ams_setField_s(state, WTIME, ams_usecToRegVal(state, MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)));  // 300
    ams_setField_s(state, PPLEN, PPLEN_4uS);
    ams_setField_s(state, PPULSE, 8);
    status = ams_i2cWriteChangedShadowRegisters(state); if(SNS_DDF_SUCCESS != status) return status;


    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_PIEN | AMS_MSK_PEN | AMS_MSK_AEN | AMS_MSK_PON);

    if((MS_TO_US(MAX_CYCLE_TIME_ms) - MS_TO_US(ALS_INTEG_TIME_ms)) != 0)
    {
        state->chip.shadow.val[AMS_REG_ENABLE] |= AMS_MSK_WEN;
    }

    firstProx = true;


    return status;
}


static sns_ddf_status_e ams_initSensorAlsProx
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    status = ams_initSensorProx(state);

    ams_setField_s(state, AIEN, 1);


    return status;
}


/* static void ams_setVisibleDataMode */
/* ( */
/*  sns_dd_state_t*  state */
/* ) */
/* { */
/*     ams_setField_s(state, GEXMSK, 0x0F);  // Gesture/Prox/Als */
/*     ams_setField_s(state, GPENTH,    0);  // Gesture Entry threshold */
/*     ams_setField_s(state, GEXTH,   255);  // Gesture Exit threshold */
/* } */

static sns_ddf_status_e ams_initSensorGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, PITHL,  0x00);
    ams_setField_s(state, PITHH,  0xFF);

    ams_setField_s(state, ATIME,  ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));
    ams_setField_s(state, PPERS,  0);
    ams_setField_s(state, APERS,  0);
    ams_setField_s(state, LDRIVE, LDRIVE_100);
    ams_setField_s(state, PGAIN,  PGAIN_4X);
    ams_setField_s(state, AGAIN,  AGAIN_16X);
    ams_setField_s(state, PPLEN,  PPLEN_16uS);
    ams_setField_s(state, PPULSE, 4);

    ams_setField_s(state, GPENTH,  0x00);
    ams_setField_s(state, GEXTH,   0xFF);
    ams_setField_s(state, GPLEN,   GPLEN_16uS);
    ams_setField_s(state, GPULSE,  12-1);
    ams_setField_s(state, GFIFOTH, GFIFOTH_1);
    ams_setField_s(state, GEXMSK,  GEXMSK_NONE);
    ams_setField_s(state, GEXPERS, GEXPERS_1);
    ams_setField_s(state, GGAIN,   GGAIN_4X);
    ams_setField_s(state, GLDRIVE, GLDRIVE_100);
    ams_setField_s(state, GWTIME,  GWTIME_2p8mS);
    ams_setField_s(state, GDIMS,   GDIMS_NSWE);
    status = ams_i2cWriteChangedShadowRegisters(state); if(SNS_DDF_SUCCESS != status) return status;

    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_GEN | AMS_MSK_AEN | AMS_MSK_PIEN | AMS_MSK_PEN | AMS_MSK_PON);


    status = ams_i2cWriteField_r(state, GIEN, 1);

    return status;
}


static sns_ddf_status_e ams_initSensorAlsGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    status = ams_initSensorGesture(state);

    return status;
}


static sns_ddf_status_e ams_initSensorProxGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    ams_setField_s(state, ATIME,   ams_usecToRegVal(state, MS_TO_US(ALS_INTEG_TIME_ms)));
    ams_setField_s(state, PPERS,   0x00);
    ams_setField_s(state, APERS,   0x00);
    ams_setField_s(state, LDRIVE,  LDRIVE_100);
    ams_setField_s(state, PGAIN,   PGAIN_4X);
    ams_setField_s(state, AGAIN,   AGAIN_16X);
    ams_setField_s(state, PPLEN,   PPLEN_16uS);
    ams_setField_s(state, PPULSE,  4);

    ams_setField_s(state, GPENTH,  0x00);
    ams_setField_s(state, GEXTH,   0xFF);
    ams_setField_s(state, GPLEN,   GPLEN_32uS);
    ams_setField_s(state, GPULSE,  4);
    ams_setField_s(state, GGAIN,   GGAIN_4X);
    ams_setField_s(state, GLDRIVE, GLDRIVE_100);
    ams_setField_s(state, GWTIME,  GWTIME_2p8mS);
    ams_setField_s(state, GDIMS,   GDIMS_NSWE);
    /* ams_setField_s(state, PPLEN,  2); */
    /* ams_setField_s(state, PPULSE, 2); */
    /* ams_setField_s(state, GPLEN,  2); */
    /* ams_setField_s(state, GPULSE, 9); */
    /* ams_setField_s(state, GWTIME, 0); */
    status = ams_i2cWriteChangedShadowRegisters(state);             if(SNS_DDF_SUCCESS != status) return status;

    state->chip.shadow.val[AMS_REG_ENABLE] = (AMS_MSK_GEN | AMS_MSK_AEN | AMS_MSK_PIEN | AMS_MSK_PEN | AMS_MSK_PON);


    return status;
}


static sns_ddf_status_e ams_initSensorAlsProxGesture
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    status = ams_initSensorProxGesture(state);

    return status;
}


/*===========================================================================
FUNCTION      sns_dd_ams_tmg399x__alsprx_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
static void sns_dd_ams_tmg399x__alsprx_log(
   sns_dd_state_t*  state,
   sns_ddf_sensor_e sensor_type,
   uint32_t         data1,
   uint32_t         data1_q16,
   uint32_t         data2,
   uint32_t         data2_q16,
   uint32_t         raw_data )
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   DD_MSG_0(HIGH, "LogData: ENTER");

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version   = SNS_LOG_SENSOR_DATA_PKT_VERSION;
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
     else if (SNS_DDF_SENSOR_PROXIMITY == sensor_type)
     {
       log_struct_ptr->num_data_types = 5;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = data2;
       log_struct_ptr->data[3] = data2_q16;
       log_struct_ptr->data[4] = raw_data;
     }
     else if (SNS_DDF_SENSOR_RGB == sensor_type)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = data2;
     }
     else if (SNS_DDF_SENSOR_CT_C == sensor_type)
     {
       log_struct_ptr->num_data_types = 2;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
     }
     /* else if (SNS_DDF_SENSOR_RGB == sensor_type) */
     /* { */
     /*   log_struct_ptr->num_data_types = 5; */
     /*   log_struct_ptr->data[0] = data1; */
     /*   log_struct_ptr->data[1] = data1_q16; */
     /*   log_struct_ptr->data[2] = data2; */
     /*   log_struct_ptr->data[3] = data2_q16; */
     /*   log_struct_ptr->data[4] = raw_data; */
     /* } */
     /* else if (SNS_DDF_SENSOR_CT_C == sensor_type) */
     /* { */
     /*   log_struct_ptr->num_data_types = 5; */
     /*   log_struct_ptr->data[0] = data1; */
     /*   log_struct_ptr->data[1] = data1_q16; */
     /*   log_struct_ptr->data[2] = data2; */
     /*   log_struct_ptr->data[3] = data2_q16; */
     /*   log_struct_ptr->data[4] = raw_data; */
     /* } */

     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
   }
   else
   {
       DD_MSG_1(HIGH, "LogData: - logpkt_malloc failed with err: %d", err_code);
       state->dropped_logs++;
   }

   DD_MSG_0(HIGH, "LogData: EXIT");
}



/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x__alsprx_prx_binary

DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x__alsprx_prx_binary
(
  sns_dd_state_t* state
)
{
    uint16_t pdata;
    uint16_t detect;
    uint16_t release;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_MSG_0(HIGH, "PrxBinary:");

    pdata   = state->chip.prx_info.pdata;
    DD_MSG_1(MED, "PrxBinary: pdata %d",  pdata );
    detect  = state->chip.setup.prox.detect;
    DD_MSG_1(MED, "PrxBinary: detect %d",  detect );
    release = state->chip.setup.prox.release;
    DD_MSG_1(MED, "PrxBinary: release %d",  release );

    DD_I2C_DB_3(0x1012, 3, pdata);
    DD_I2C_DB_3(0x1012, 4, detect);
    DD_I2C_DB_3(0x1012, 5, release);
    DD_I2C_DB_3(0x1012, 6, state->chip.prx_info.event);

    if(state->chip.prx_info.event == SNS_PRX_FAR_AWAY)
    {
        if(pdata > detect)
        {
            state->chip.prx_info.event = SNS_PRX_NEAR_BY;

            // Set Prox interrupt low
            status = ams_i2cWriteField_s(state, PITHL, state->chip.setup.prox.release);
            if ( status != SNS_DDF_SUCCESS ) {
                DD_MSG_0(HIGH, "PrxBinary: ALSPRX prx processing err");
                return status;
            }
            DD_MSG_2(MED, "PrxBinary: pdata > detect.. release: %d var2: %d", state->chip.setup.prox.release, state->chip.shadow.val[AMS_REG_PITHL]);

            // Set Prox interrupt high
            status = ams_i2cWriteField_s(state, PITHH, 0xFF);
            if ( status != SNS_DDF_SUCCESS ) {
                DD_MSG_0(HIGH, "PrxBinary: ALSPRX prx processing err");
                return status;
            }
            DD_MSG_2(MED, "PrxBinary: prox high var1: %d var2: %d", 1108, state->chip.shadow.val[AMS_REG_PITHH]);
        }
    }
    else  // SNS_PRX_NEAR_BY
    {
        if(pdata < release)
        {
            state->chip.prx_info.event = SNS_PRX_FAR_AWAY;

            // Set Prox interrupt low
            status = ams_i2cWriteField_s(state, PITHL, 0x00);
            if ( status != SNS_DDF_SUCCESS ) {
                DD_MSG_0(HIGH, "PrxBinary: ALSPRX prx processing err");
                return status;
            }
            DD_MSG_2(MED, "PrxBinary: pdata < release: %d var2: %d", 1109, state->chip.shadow.val[AMS_REG_PITHL]);

            // Set Prox interrupt high
            status = ams_i2cWriteField_s(state, PITHH, state->chip.setup.prox.detect);
            if ( status != SNS_DDF_SUCCESS ) {
                DD_MSG_0(HIGH, "PrxBinary: ALSPRX prx processing err");
                return status;
            }
            DD_MSG_2(MED, "PrxBinary: var1: %d var2: %d", 1110, state->chip.shadow.val[AMS_REG_PITHH]);
        }
    }
    return status;
}


static uint16_t ams_get_num_params_to_ret
(
 sns_ddf_sensor_e sensor
)
{
    uint16_t paramsToRet;


    switch(sensor)
    {
    case SNS_DDF_SENSOR_AMBIENT:
        paramsToRet = numParamsToReturn[AMBIENT_PARAMS];
        break;

    case SNS_DDF_SENSOR_PROXIMITY:
        paramsToRet = numParamsToReturn[PROX_PARAMS];
        break;

    case SNS_DDF_SENSOR_RGB:
        paramsToRet = numParamsToReturn[RGB_PARAMS];
        break;

    case SNS_DDF_SENSOR_CT_C:
        paramsToRet = numParamsToReturn[CTEMP_CLR_PARAMS];
        break;

    default:
            return SNS_DDF_EDATA_BOUND;
        break;
    }

    return paramsToRet;
}


sns_ddf_status_e ams_tmg399x_state_machine
(
  sns_dd_state_t*        state,
  sns_ddf_sensor_e       sensor,
  bool                   sensorEnable
)
{
    uint16_t         newState        = currentState;
    sns_ddf_status_e status          = SNS_DDF_SUCCESS;


    DD_MSG_0(MED, "StMach: ENTER");

    DD_I2C_DB_3(0x1200, 0x00, sensor);
    DD_I2C_DB_3(0x1200, 0x01, sensorEnable);

    switch(sensor)
    {
    case SNS_DDF_SENSOR_AMBIENT:
        currentSensor = AMS_SENSOR_ALS;
        break;

    case SNS_DDF_SENSOR_PROXIMITY:
        currentSensor = AMS_SENSOR_PROX;
        break;

    case SNS_DDF_SENSOR_IR_GESTURE:
        currentSensor = AMS_SENSOR_GESTURE;
        break;

    case SNS_DDF_SENSOR_RGB:
        currentSensor = AMS_SENSOR_RGB;
        break;

    case SNS_DDF_SENSOR_CT_C:
        currentSensor = AMS_SENSOR_CT;
        break;

    default:
        return SNS_DDF_EINVALID_DATA;
        break;
    };

    DD_I2C_DB_3(0x1200, 0x02, currentSensor);
    DD_I2C_DB_3(0x1200, 0x03, currentState);

    DD_MSG_2(MED, "StMach: currentSensor: %d sensorEnable: %d", currentSensor, sensorEnable);

    DD_MSG_2(HIGH, "StMach: ALS: %d prox: %d", currentlyActiveSensors[0], currentlyActiveSensors[1]);
    DD_MSG_2(HIGH, "StMach: RGB: %d CT:   %d", currentlyActiveSensors[3], currentlyActiveSensors[4]);
    DD_MSG_1(HIGH, "StMach: Gest: %d",         currentlyActiveSensors[2]);

    if ( sensorEnable )
    {
        currentlyActiveSensors[currentSensor] = true;

        if(currentSensor == AMS_SENSOR_RGB || currentSensor == AMS_SENSOR_CT)
        {
            currentSensor = AMS_SENSOR_ALS;
            ++alsBeingUsed;
            DD_MSG_1(MED, "StMach: alsBeingUsed[1]: %d", alsBeingUsed );
        }

        /* Set ALS thresholds to ensure first interrupt */
        if(currentSensor == AMS_SENSOR_ALS)
        {
            DD_MSG_0(MED, "StMach: ams_update_als_thresh" );
            ams_update_als_thresh(state, 0x00, 0x00, 0);
        }

        newState = enableStates[currentState][currentSensor];
    }
    else /* sensorDisable */
    {
        /* If a sensor that is being disabled is already disabled then
        don't process state machine */
        if(!currentlyActiveSensors[currentSensor])
        {
            return status;
        }

        currentlyActiveSensors[currentSensor] = false;

        if(currentSensor == AMS_SENSOR_ALS)
        {
            if(!currentlyActiveSensors[AMS_SENSOR_RGB] && !currentlyActiveSensors[AMS_SENSOR_CT])
            {
                newState = disableStates[currentState][currentSensor];
            }
        }

        if(currentSensor == AMS_SENSOR_RGB)
        {
            if(!currentlyActiveSensors[AMS_SENSOR_CT] && !currentlyActiveSensors[AMS_SENSOR_ALS])
            {
                currentSensor = AMS_SENSOR_ALS;
                newState = disableStates[currentState][currentSensor];
            }
        }

        if(currentSensor == AMS_SENSOR_CT)
        {
            if(!currentlyActiveSensors[AMS_SENSOR_RGB] && !currentlyActiveSensors[AMS_SENSOR_ALS])
            {
                currentSensor = AMS_SENSOR_ALS;
                newState = disableStates[currentState][currentSensor];
            }
        }

        if(currentSensor == AMS_SENSOR_PROX)
        {
            newState = disableStates[currentState][currentSensor];
        }

        if(currentSensor == AMS_SENSOR_GESTURE)
        {
            newState = disableStates[currentState][currentSensor];
        }
    }

    DD_MSG_2(HIGH, "StMach: newState: %d currentState: %d", newState, currentState);

    DD_I2C_DB_3(0x1200, 0x04, newState);

    if( newState != currentState )
    {
        DD_MSG_1(HIGH, "StMach: Change to newState: %d", newState);

        switch(newState)
        {
        case AMS_STATE_OFF:          // Off
            status = ams_initSensorPor(state);
            break;

        case AMS_STATE_ALS:          // Als
            status = ams_initSensorAls(state);
            break;

        case AMS_STATE_PRX:          // Prox
            status = ams_initSensorProx(state);
            break;

        case AMS_STATE_ALS_PRX:      // Als + Prox
            status = ams_initSensorAlsProx(state);
            break;

        case AMS_STATE_GES:          // Gesture
            status = ams_initSensorGesture(state);
            break;

        case AMS_STATE_GES_PRX:      // Gesture + Prox
            status = ams_initSensorProxGesture(state);
            break;

        case AMS_STATE_GES_ALS:      // Gesture + Als
            status = ams_initSensorAlsGesture(state);
            break;

        case AMS_STATE_GES_ALS_PRX:  // Gesture + Als + Prox
            status = ams_initSensorAlsProxGesture(state);
            break;
        }

        currentState = newState;

        status = ams_i2c_write_byte(state, AMS_REG_ENABLE, state->chip.shadow.val[AMS_REG_ENABLE]);
        DD_MSG_2(MED, "StMach: AMS_REG_ENABLE: 0x%X status: %d", state->chip.shadow.val[AMS_REG_ENABLE], status);
   }


    DD_I2C_DB_2(0x1200, 0xFF);

    DD_MSG_0(MED, "StMach: EXIT");

    return status;
}


/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x__alsprx_set_powerstate

DESCRIPTION   Sets the power state of the AMS ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x__alsprx_set_powerstate
(
  sns_dd_state_t*        state,
  uint32_t               sub_dev_id,
  sns_ddf_powerstate_e   op_mode,
  sns_ddf_sensor_e       sensor,
  bool                   init_data
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    int i;
    bool activeSensors = false;

    DD_MSG_0(HIGH, "SetPwrState: ENTER");

    if(op_mode == SNS_DDF_POWERSTATE_ACTIVE )
    {
        ams_i2cWriteField_s(state, PON, 1);
        state->sub_dev[sub_dev_id].powerstate = SNS_DDF_POWERSTATE_ACTIVE;
        DD_MSG_0(HIGH, "SetPwrState: Set PON");
    }
    else   /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
    {
        state->sub_dev[sub_dev_id].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

        for(i = 0; i < state->sub_dev_count; ++i)
        {
            if(state->sub_dev[i].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
               activeSensors = true;
        }

        if(!activeSensors)
        {
            status = ams_i2c_write_byte(state, AMS_REG_ENABLE, 0x00);
            DD_MSG_0(HIGH, "SetPwrState: Cleared PON");
        }

    }

    state->sns_dd_alsprx_common_db.pwr_mode = op_mode;
    state->sns_dd_alsprx_common_db.state    = SNS_DD_DEV_CONFIGURED;

    DD_MSG_0(HIGH, "SetPwrState: EXIT");

    return status;
}


uint32_t ams_prox_time_us(sns_dd_state_t* state)
{
    // Prox enabled
    DD_MSG_1(MED, "PrxTime_us: %d", 2 * state->chip.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);

    return ( 2 * state->chip.Min_Integ_Time_us +
             16 * state->chip.setup.prox.pulse_count  +
             state->chip.setup.prox.ptime_us);
}


uint32_t ams_odr_from_uSec(uint32_t requestedTime_us)
{
    uint16_t i;
    uint16_t foundOdr;
    uint32_t midDn;

    foundOdr = MAX_ODR - 1;

    if(requestedTime_us > 750000)
    {
        return 1;
    }

    if(requestedTime_us < 46536)
    {
        return 21;
    }

    for(i = 1; i < MAX_ODR; i++)
    {
        midDn = ((ams_odr_to_uSec[i] - ams_odr_to_uSec[i-1]) / 2) - 1;

        if(requestedTime_us > (ams_odr_to_uSec[i] - midDn))
        {
            foundOdr = i + 1;
            break;
        }
    }
    DD_MSG_1(MED, "TMG399x Calculated ODR: %d", foundOdr);
    return foundOdr;
}


static sns_ddf_status_e ams_readDataRegisters
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e status  = SNS_DDF_SUCCESS;
    sns_ddf_status_e statusa = SNS_DDF_SUCCESS;
    sns_ddf_status_e statusc = SNS_DDF_SUCCESS;

    /* uint8_t  statusReg; */


    DD_MSG_0(HIGH, "RdDataRegs: ENTER");

    DD_I2C_DB_2(0x1020, 0);

    /* // DEBUG */
    /* status = ams_i2c_read_byte(state, AMS_REG_STATUS, &statusReg); */
    /* if(status != SNS_DDF_SUCCESS) */
    /* { */
    /*     DD_MSG_0(HIGH, "RdDataRegs: could not read status reg 1"); */

    /*     return status; */
    /* } */

    status = ams_i2c_read_buf(state,
                              AMS_REG_CDATAL,
                              AMS_REG_PDATA - AMS_REG_CDATAL + 1,
                              &state->chip.shadow.val[AMS_REG_CDATAL]);
    if(status != SNS_DDF_SUCCESS)
    {
        DD_MSG_0(HIGH, "RdDataRegs: could not read to buf");
    }

    /* // DEBUG */
    /* status = ams_i2c_read_byte(state, AMS_REG_STATUS, &statusReg); */
    /* if(status != SNS_DDF_SUCCESS) */
    /* { */
    /*     DD_MSG_0(HIGH, "RdDataRegs: could not read status reg 1"); */

    /*     return status; */
    /* } */

    /* // DEBUG */
    /* ams_i2c_write_spec_func(state->port_handle, 0xE7); */

    statusa = ams_i2c_read_byte(state, AMS_REG_ATIME,   &state->chip.shadow.val[AMS_REG_ATIME]);
    statusc = ams_i2c_read_byte(state, AMS_REG_CONTROL, &state->chip.shadow.val[AMS_REG_CONTROL]);  // for gain
    if(statusa != SNS_DDF_SUCCESS) status = statusa;
    if(statusc != SNS_DDF_SUCCESS) status = statusc;

    DD_MSG_0(HIGH, "RdDataRegs: EXIT");

    return status;
}


static sns_ddf_status_e ams_get_prox_data
(
 sns_dd_state_t* state
)
{
    uint8_t* tBuf;

    tBuf = &state->chip.shadow.val[AMS_REG_PDATA];

    //
    // Get Prox data
    //
    state->chip.prx_info.pdata = getWord(tBuf, 0);
    state->chip.prx_info.pdata = state->chip.shadow.val[AMS_REG_PDATA];

    DD_MSG_1(MED, "Get prox data: %d", state->chip.prx_info.pdata);

    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_get_als_data
(
 sns_dd_state_t* state
)
{
    double denom;
    double IRed;
    double IGrn;
    double IBlu;
    double IClr;

    //
    // Get Color data
    //
    state->chip.als_info.clear_raw = ams_getField_s(state, CDATA);
    state->chip.als_info.red_raw   = ams_getField_s(state, RDATA);
    state->chip.als_info.green_raw = ams_getField_s(state, GDATA);
    state->chip.als_info.blue_raw  = ams_getField_s(state, BDATA);
    state->chip.als_info.atime     = ams_getField_s(state, ATIME);
    state->chip.als_info.again     = ams_getField_s(state, AGAIN);

    //
    // Compute IR
    // ir = (R + G + B - C + 1) / 2
    //
    state->chip.als_info.ir =
        (state->chip.als_info.red_raw  + state->chip.als_info.green_raw +
         state->chip.als_info.blue_raw - state->chip.als_info.clear_raw + 1) >> 1;

    if (state->chip.als_info.ir < 0)
    {
        state->chip.als_info.ir = 0;
    }

    //
    // Compute irradiances in uW/cm^2
    //
    denom = (REG_TO_TIME_uS(state->chip.als_info.atime, state->chip.Min_Integ_Time_us) / 1000.0)
             * ams_tmg399x_alsGainVal[state->chip.als_info.again]; // gain * time in msec

    IClr = ((31.94/denom) * state->chip.als_info.clear_raw);
    IRed = ((30.33/denom) * state->chip.als_info.red_raw);
    IGrn = ((46.38/denom) * state->chip.als_info.green_raw);
    IBlu = ((48.63/denom) * state->chip.als_info.blue_raw);

    state->chip.als_info.clear_irradiance = FX_FLTTOFIX_Q16(IClr);
    state->chip.als_info.red_irradiance   = FX_FLTTOFIX_Q16(IRed);
    state->chip.als_info.green_irradiance = FX_FLTTOFIX_Q16(IGrn);
    state->chip.als_info.blue_irradiance  = FX_FLTTOFIX_Q16(IBlu);

    DD_I2C_DB_3(0x1021, 0x01, state->chip.als_info.clear_raw);
    DD_I2C_DB_3(0x1021, 0x02, state->chip.als_info.red_raw);
    DD_I2C_DB_3(0x1021, 0x03, state->chip.als_info.green_raw);
    DD_I2C_DB_3(0x1021, 0x04, state->chip.als_info.blue_raw);
    DD_I2C_DB_3(0x1021, 0x05, state->chip.als_info.atime);

    DD_I2C_DB_3(0x1021, 0x06, ams_getField_s(state, CDATA));
    DD_I2C_DB_3(0x1021, 0x07, ams_getField_s(state, RDATA));
    DD_I2C_DB_3(0x1021, 0x08, ams_getField_s(state, GDATA));
    DD_I2C_DB_3(0x1021, 0x09, ams_getField_s(state, BDATA));
    DD_I2C_DB_3(0x1021, 0x0A, state->chip.shadow.val[AMS_REG_ATIME]);

    DD_MSG_1(MED, "get_als_data: clr: 0x%04X", state->chip.als_info.clear_raw);
    DD_MSG_1(MED, "get_als_data: red: 0x%04X", state->chip.als_info.red_raw);
    DD_MSG_1(MED, "get_als_data: grn: 0x%04X", state->chip.als_info.green_raw);
    DD_MSG_1(MED, "get_als_data: blu: 0x%04X", state->chip.als_info.blue_raw);

    denom *= 1000;
    DD_MSG_1(MED, "get_als_data: denom: %d", (uint32_t)denom);
    DD_MSG_1(MED, "get_als_data: ATIME: 0x%02X", state->chip.als_info.atime);
    DD_MSG_2(MED, "get_als_data: AGAIN: 0x%02X, val = %d", state->chip.als_info.again, ams_tmg399x_alsGainVal[state->chip.als_info.again]);

    DD_MSG_1(MED, "get_als_data: IrrClear: %08X", IClr*1000);
    DD_MSG_1(MED, "get_als_data: IrrRed:   %08X", IRed*1000);
    DD_MSG_1(MED, "get_als_data: IrrGreen: %08X", IGrn*1000);
    DD_MSG_1(MED, "get_als_data: IrrBlue:  %08X", IBlu*1000);

    DD_MSG_1(MED, "get_als_data: Q16:IrrClear: %08X", state->chip.als_info.clear_irradiance);
    DD_MSG_1(MED, "get_als_data: Q16:IrrRed:   %08X", state->chip.als_info.red_irradiance);
    DD_MSG_1(MED, "get_als_data: Q16:IrrGreen: %08X", state->chip.als_info.green_irradiance);
    DD_MSG_1(MED, "get_als_data: Q16:IrrBlue:  %08X", state->chip.als_info.blue_irradiance);

    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_set_als_gain
(
 sns_dd_state_t* state,
 int             gain
)
{
    uint8_t gVal = 0;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    switch (gain) {
    case 1:
        gVal = AGAIN1;
        break;
    case 4:
        gVal = AGAIN4;
        break;
    case 16:
        gVal = AGAIN16;
        break;
    case 60:
    case 64:
        gVal = AGAIN64;
        break;
    default:
         return SNS_DDF_EINVALID_PARAM;
    }

    status = ams_i2cWriteField_s(state, AGAIN, gVal);
    if ( status != SNS_DDF_SUCCESS )
    {
        return status;
    }

    state->chip.als_info.again = gain;

    return status;
}


static void ams_calc_cpl
(
 tmg399x_chip *chip
)
{
    uint32_t cpl;
    uint32_t sat;
    uint8_t  atime;
    uint8_t  again;

    DD_MSG_1(MED, "calc_cpl: old: %d", chip->als_info.cpl);

    atime = chip->als_info.atime;
    again = chip->als_info.again;

    DD_MSG_1(MED, "calc_cpl:  atime: 0x%02X", chip->als_info.atime);
    DD_MSG_1(MED, "calc_cpl:  again: 0x%02X", chip->als_info.again);
    DD_MSG_1(MED, "calc_cpl: I time: %d", chip->Min_Integ_Time_us);

    cpl  = 256 - atime;
    cpl *= chip->Min_Integ_Time_us;
    cpl *= ams_tmg399x_alsGainVal[again];
    cpl /= chip->lux_coef[chip->device_index].d_factor;

    sat = min((uint32_t)MAX_ALS_VALUE, (uint32_t)(256 - atime) << 10);
    sat = sat * 8 / 10;

    chip->als_info.cpl        = cpl;
    chip->als_info.saturation = sat;

    DD_MSG_1(MED, "calc_cpl: new: %d", chip->als_info.cpl);
}


static void ams_inc_gain
(
 sns_dd_state_t* state
)
{
    int32_t rc;
    tmg399x_chip *chip = &state->chip;
    uint8_t       gain = chip->als_info.again;

    DD_MSG_1(MED, "inc_gain: old: %d", gain);

    if (gain > AGAIN16)
    {
        return;
    }

    if (gain < AGAIN4)
    {
        rc = ams_set_als_gain(state, ams_tmg399x_alsGainVal[AGAIN4]);
    }
    else if (gain < AGAIN16)
    {
        rc = ams_set_als_gain(state, ams_tmg399x_alsGainVal[AGAIN16]);
    }
    else
    {
        rc = ams_set_als_gain(state, ams_tmg399x_alsGainVal[AGAIN64]);
    }

    if (!rc)
    {
        ams_calc_cpl(chip);
    }
}


static void ams_dec_gain
(
 sns_dd_state_t* state
)
{
    int rc;
    tmg399x_chip *chip = &state->chip;
    uint8_t       gain = chip->als_info.again;

    DD_MSG_0(MED, "dec_gain: ");

    if (gain == AGAIN1)
    {
        return;
    }

    if (gain > AGAIN16)
    {
        rc = ams_set_als_gain(state, ams_tmg399x_alsGainVal[AGAIN16]);
    }
    else if (gain > AGAIN4)
    {
        rc = ams_set_als_gain(state, ams_tmg399x_alsGainVal[AGAIN4]);
    }
    else
    {
        rc = ams_set_als_gain(state, ams_tmg399x_alsGainVal[AGAIN1]);
    }

    if (!rc)
    {
        ams_calc_cpl(chip);
    }
}


static int32_t ams_max_colorcount(tmg399x_chip *chip)
{
    int32_t t;

    /* t = 256 - chip->shadow.val[AMS_REG_ATIME]; */
    t = 256 - chip->als_info.atime;

    if (t > 63)
    {
        return 65535;
    }
    else
    {
        return ((t * 1024) - 1);
    }
}


static sns_ddf_status_e ams_get_lux
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    tmg399x_chip *chip = &state->chip;
    int32_t rp1, gp1, bp1;    //, cp1;
    int32_t lux = 0;
    int32_t cct;
    uint32_t sat = chip->als_info.saturation;
    int quarter  = (ams_max_colorcount(chip) / 4);


    DD_I2C_DB_3(0x101C, 0x00, chip->als_info.clear_raw);
    DD_I2C_DB_3(0x101C, 0x01, chip->als_info.red_raw);
    DD_I2C_DB_3(0x101C, 0x02, chip->als_info.green_raw);
    DD_I2C_DB_3(0x101C, 0x03, chip->als_info.blue_raw);
    DD_I2C_DB_3(0x101C, 0x04, chip->als_info.ir);

    DD_I2C_DB_3(0x101C, 0x10, chip->device_index);
    DD_I2C_DB_3(0x101C, 0x11, chip->lux_coef[chip->device_index].r_coef);
    DD_I2C_DB_3(0x101C, 0x12, chip->lux_coef[chip->device_index].g_coef);
    DD_I2C_DB_3(0x101C, 0x13, chip->lux_coef[chip->device_index].b_coef);
    DD_I2C_DB_3(0x101C, 0x14, chip->als_info.atime);

    DD_MSG_1(MED, "get_lux: Clear 0x%02X", chip->als_info.clear_raw);
    DD_MSG_1(MED, "get_lux: Red   0x%02X", chip->als_info.red_raw);
    DD_MSG_1(MED, "get_lux: Green 0x%02X", chip->als_info.green_raw);
    DD_MSG_1(MED, "get_lux: Blue  0x%02X", chip->als_info.blue_raw);
    DD_MSG_1(MED, "get_lux: IR    0x%02X", chip->als_info.ir);
    DD_MSG_1(MED, "get_lux: ATIME 0x%02X", chip->als_info.atime);
    DD_MSG_1(MED, "get_lux  AGAIN 0x%02X", chip->als_info.again);
    DD_MSG_1(MED, "get_lux  CPL   %d",     chip->als_info.cpl);

    /* use time in ms get scaling factor */
    ams_calc_cpl(chip);

    /* remove ir from counts*/
    rp1 = chip->als_info.red_raw   - chip->als_info.ir;
    gp1 = chip->als_info.green_raw - chip->als_info.ir;
    bp1 = chip->als_info.blue_raw  - chip->als_info.ir;

    DD_I2C_DB_3(0x101C, 0x09, rp1);
    DD_I2C_DB_3(0x101C, 0x0A, gp1);
    DD_I2C_DB_3(0x101C, 0x0B, bp1);

    if (!chip->als_info.cpl)
    {
        chip->als_info.cpl = 1;
    }

    DD_I2C_DB_3(0x101C, 0xB1, chip->als_info.cpl);

    if (chip->als_info.red_raw > chip->als_info.ir)
    {
        lux += chip->lux_coef[chip->device_index].r_coef * rp1;
    }

    if (chip->als_info.green_raw > chip->als_info.ir)
    {
        lux += chip->lux_coef[chip->device_index].g_coef * gp1;
    }

    if (chip->als_info.blue_raw > chip->als_info.ir)
    {
        lux += chip->lux_coef[chip->device_index].b_coef * bp1;
    }

    DD_I2C_DB_3(0x101C, 0x0C, lux);

    //
    // Round up the lux:
    // lux *= 16
    // lux /= cpl
    // lux += (16/2)
    // lux /= 16
    //
    lux <<= ROUND_SHFT_VAL;     // *= 16
    lux /=  chip->als_info.cpl;
    lux +=  ROUND_ADD_VAL;      // += 8
    lux >>= ROUND_SHFT_VAL;     // /= 16

    DD_I2C_DB_3(0x101C, 0x0D, lux);

    if (lux < 0)
    {
        lux = 0;
    }

    DD_I2C_DB_3(0x101C, 0x0E, lux);

    chip->als_info.lux = (uint32_t) lux;

    cct = ((chip->lux_coef[chip->device_index].ct_coef * bp1) / rp1) +
        chip->lux_coef[chip->device_index].ct_offset;

    chip->als_info.cct = (uint32_t) cct;

    DD_I2C_DB_3(0x101C, 0x0F, cct);


    DD_MSG_1(MED, "get_lux: Lux   %d", chip->als_info.lux);
    DD_MSG_1(MED, "get_lux: cct   %d", chip->als_info.cct);
    DD_MSG_1(MED, "get_lux: atime 0x%02X", chip->als_info.atime);
    DD_MSG_2(MED, "get_lux: again 0x%02X, val = %d", chip->als_info.again, ams_tmg399x_alsGainVal[chip->als_info.again]);

    if (!chip->als_gain_auto)
    {
        if (chip->als_info.clear_raw <= MIN_ALS_VALUE)
        {
            DD_I2C_DB_2(0x101C, 0x05);
            lux = 0;
            chip->als_info.lux = (uint32_t) lux;  //fv ???
        }
        else if (chip->als_info.clear_raw >= sat)
        {
            DD_I2C_DB_2(0x101C, 0x06);
            lux = chip->als_info.lux;  //fv ???
        }
    }
    else
    {
        /* AutoGain */
        if (chip->als_info.clear_raw < 100)
        {
            DD_I2C_DB_2(0x101C, 0x07);
            ams_inc_gain(state);
        }

        if (chip->als_info.clear_raw > (ams_max_colorcount(chip) - quarter))
        {
            DD_I2C_DB_2(0x101C, 0x08);
            ams_dec_gain(state);
        }
    }

    return status;
}


sns_ddf_status_e ams_prox_sensor_samples(sns_dd_state_t          *state,
                                         sns_ddf_sensor_data_s   *data_ptr,
                                         sns_ddf_memhandler_s    *memhandler,
                                         sns_ddf_sensor_sample_s *sample_data)
{
    uint32_t num_samples = NUM_PROX_SAMPLES_RET;

    DD_MSG_0(MED, "PrxSensorSmpls: ENTER");

    if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DD_MSG_0(MED, "PrxSensorSmpls: Invalid Param");
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_1(MED, "PrxSensorSmpls: Faraway event num: %d", SNS_PRX_FAR_AWAY);
    DD_MSG_1(MED, "PrxSensorSmpls: nearby event num: %d",  SNS_PRX_NEAR_BY);

    data_ptr->samples[0].sample = FX_CONV_Q16(state->chip.prx_info.event, 0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.prx_info.pdata;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DD_I2C_DB_3(0x1012, 1, num_samples);
    DD_I2C_DB_3(0x1012, 2, data_ptr->samples[0].sample);

    DD_MSG_0(HIGH, "PrxSensorSmpls: PRX Data...");
    DD_MSG_3(FATAL, "PrxSensorSmpls: Data: Event:%d  Reported Data: %d  Raw Data: %d",
             state->chip.prx_info.event, data_ptr->samples[0].sample, state->chip.prx_info.pdata);
    sns_dd_ams_tmg399x__alsprx_log(state, SNS_DDF_SENSOR_PROXIMITY,
                                   state->chip.prx_info.event, data_ptr->samples[0].sample,
                                   0, 0, (uint32_t) state->chip.prx_info.pdata);

    DD_MSG_0(MED, "PrxSensorSmpls: EXIT");

    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_rgb_sensor_samples
(
 sns_dd_state_t          *state,
 sns_ddf_sensor_data_s   *data_ptr,
 sns_ddf_memhandler_s    *memhandler,
 sns_ddf_sensor_sample_s *sample_data
)
{
    uint32_t num_samples = NUM_RGB_SAMPLES_RET;

    DD_MSG_1(MED, "rgb_sensor_samples: RGB var1: %d", 1161);

    if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DD_MSG_1(MED, "rgb_sensor_samples: RGB var1: %d", 1163);
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_1(MED, "rgb_sensor_samples: RED:   %d", state->chip.als_info.red_irradiance);
    DD_MSG_1(MED, "rgb_sensor_samples: GREEN: %d", state->chip.als_info.green_irradiance);
    DD_MSG_1(MED, "rgb_sensor_samples: BLUE:  %d", state->chip.als_info.blue_irradiance);

    data_ptr->samples[0].sample = state->chip.als_info.red_irradiance;
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.als_info.green_irradiance;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].sample = state->chip.als_info.blue_irradiance;
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DD_MSG_0(HIGH, "RGB Data...");
    DD_MSG_3(FATAL, "RGB Data:  Red:%d Green:%d  Blue: %d", state->chip.als_info.red_raw, state->chip.als_info.green_raw, state->chip.als_info.blue_raw);
    sns_dd_ams_tmg399x__alsprx_log(state, SNS_DDF_SENSOR_RGB,
                                   data_ptr->samples[0].sample, // should be R in irradiance
                                   data_ptr->samples[1].sample, // should be G in irradiance
                                   data_ptr->samples[2].sample, // should be B in irradiance
                                   0, 0);

    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_cct_clear_sensor_samples
(
 sns_dd_state_t          *state,
 sns_ddf_sensor_data_s   *data_ptr,
 sns_ddf_memhandler_s    *memhandler,
 sns_ddf_sensor_sample_s *sample_data
)
{
    uint32_t num_samples = NUM_CTEMP_CLR_SAMPLES_RET;

    DD_MSG_1(MED, "TMG399x CCT_CLR var1: %d", 1161);

    if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DD_MSG_1(MED, "TMG399x CCT_CLR var1: %d", 1163);
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_1(MED, "TMG399x CCT: %d", state->chip.als_info.cct);
    DD_MSG_1(MED, "TMG399x CLR: %d", state->chip.als_info.clear_raw);

    data_ptr->samples[0].sample = FX_FLTTOFIX_Q16(state->chip.als_info.cct);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = state->chip.als_info.clear_irradiance;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DD_MSG_0(HIGH, "CCT_CLR Data...");
    DD_MSG_2(FATAL, "CCT_CLR Data:  CCT:%d CLEAR:%d", state->chip.als_info.cct, state->chip.als_info.clear_raw);
    sns_dd_ams_tmg399x__alsprx_log(state, SNS_DDF_SENSOR_CT_C,
                                   data_ptr->samples[0].sample,
                                   data_ptr->samples[1].sample, // should be clear in irradiance
                                   0, 0, 0);

    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_als_sensor_samples
(
 sns_dd_state_t           *state,
 sns_ddf_sensor_data_s    *data_ptr,
 sns_ddf_memhandler_s     *memhandler,
 sns_ddf_sensor_sample_s  *sample_data
)
{
    // Return Lux, Red, Green, Blue and Clear data
    uint32_t num_samples = NUM_ALS_SAMPLES_RET;
    uint32_t luxCorrection;
    uint32_t lux;

    DD_MSG_0(MED, "SnsrSmpls: ENTER");

    if(sample_data != NULL)
    {
        data_ptr->samples = sample_data;
    }
    else
    {
        DD_MSG_0(MED, "SnsrSmpls: EINVALID_PARAM");
        return SNS_DDF_EINVALID_PARAM;
    }

    lux           = state->chip.als_info.lux;
    luxCorrection = state->nv_db.luxCorrection;

    DD_I2C_DB_3(0x101A, 0x00, num_samples);
    DD_I2C_DB_3(0x101A, 0x01, lux);
    DD_I2C_DB_3(0x101A, 0x02, luxCorrection);


    data_ptr->samples[0].sample = FX_FLTTOFIX_Q16((lux * luxCorrection) / 1024.0);
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = (uint32_t)state->chip.als_info.clear_raw;
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples       = num_samples;

    DD_MSG_0(HIGH, "SnsrSmpls: ALS Data...");
    DD_MSG_1(FATAL, "SnsrSmpls: TMG LIGHT Data: Lux:%d  ", (1000 * (lux * luxCorrection) / 1024.0));
    DD_MSG_2(FATAL, "SnsrSmpls: TMG LIGHT Data: MLux:%d  Reported Data: %d", state->chip.als_info.lux, state->chip.als_info.clear_raw);
    DD_MSG_3(FATAL, "SnsrSmpls: TMG LIGHT Data: R:G:B: %d:%d:%d ", state->chip.als_info.red_raw, state->chip.als_info.green_raw, state->chip.als_info.blue_raw);

    sns_dd_ams_tmg399x__alsprx_log(state, SNS_DDF_SENSOR_AMBIENT,
                                   (1000 * (lux * luxCorrection) / 1024.0),  //state->chip.als_info.lux
                                   data_ptr->samples[0].sample,
                                   0,
                                   0,
                                   state->chip.als_info.clear_raw);

    DD_MSG_0 (HIGH, "SnsrSmpls: EXIT");

    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e ams_build_smgr_message
(
 sns_dd_state_t*        state,
 sns_ddf_memhandler_s*  memhandler,
 sns_ddf_sensor_data_s* data[],
 uint32_t               sub_dev_id
)
{
    uint8_t  numOfSensors = 0;
    uint16_t paramsToRet;
    uint16_t i;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint32_t subDevIndex = 0;

    /* basic sanity check */
    if(state == NULL)
    {
        DD_MSG_0(MED, "BldSmgrMsg: state NULL");

        return SNS_DDF_EINVALID_PARAM;
    }

    numOfSensors  = state->chip.numOfSensors;

    DD_MSG_2(HIGH, "BldSmgrMsg: numOfSensors = %d sub_dev_id = %d", numOfSensors, sub_dev_id);

    static sns_ddf_sensor_sample_s* sample_ptr[2];
    static sns_ddf_sensor_data_s*   sensor_data;

    DD_I2C_DB_2(0x1010, 0);
    DD_I2C_DB_3(0x1010, 1, numOfSensors);

    if(numOfSensors == 0)
    {
        DD_I2C_DB_2(0x1010, 0x03);

        DD_MSG_0(MED, "BldSmgrMsg: numOfSensors ZERO");
        return SNS_DDF_EDATA_BOUND;
    }

    if(memhandler == NULL)
    {
        DD_I2C_DB_2(0x1010, 0x04);
        DD_MSG_0(MED, "BldSmgrMsg: memhandler = NULL, ddf_malloc");

        status = sns_ddf_malloc((void **)&sensor_data, (numOfSensors * sizeof(sns_ddf_sensor_data_s)));
        if (status != SNS_DDF_SUCCESS )
        {
            DD_I2C_DB_2(0x1010, 0x05);

            DD_MSG_0(MED, "BldSmgrMsg: Failed miserably. Can't even notify SMGR");
            /* Failed miserably. Can't even notify SMGR */
            return SNS_DDF_EFAIL;
        }
    }
    else
    {
        DD_I2C_DB_2(0x1010, 0x06);
        DD_MSG_0(MED, "BldSmgrMsg: memhandler = defined, using ddf_memhandler_malloc");

        sensor_data  = sns_ddf_memhandler_malloc(memhandler, (numOfSensors * sizeof(sns_ddf_sensor_data_s)));
        if(sensor_data == NULL)
        {
            DD_I2C_DB_2(0x1010, 0x07);

            DD_MSG_1(HIGH, "BldSmgrMsg: Get Data malloc error, Size: %d", (numOfSensors * sizeof(sns_ddf_sensor_data_s)));
            return SNS_DDF_ENOMEM;
        }
        *data = sensor_data;
    }

    DD_I2C_DB_3(0x1010, 0x071, (uint32_t) state);
    ams_readDataRegisters(state);

    for(i = 0; i < numOfSensors; i++)
    {
        DD_I2C_DB_2(0x1010, 0x08);
        DD_MSG_2(HIGH, "BldSmgrMsg: chip.sensors[%d] = %d", i, state->chip.sensors[i]);

        sensor_data[i].sensor    = state->chip.sensors[i];
        sensor_data[i].status    = SNS_DDF_SUCCESS;
        sensor_data[i].timestamp = sns_ddf_get_timestamp();

        paramsToRet = ams_get_num_params_to_ret(state->chip.sensors[i]);
        DD_MSG_1(HIGH, "BldSmgrMsg: paramsToRet %d", paramsToRet);
        if(memhandler == NULL)
        {
            DD_I2C_DB_2(0x1010, 0x09);

            status = sns_ddf_malloc((void **)&sample_ptr[i], (paramsToRet * sizeof(sns_ddf_sensor_sample_s)));
            if(status != SNS_DDF_SUCCESS )
            {
                DD_I2C_DB_2(0x1010, 0x0A);

                DD_MSG_1(MED, "BldSmgrMsg: TMG399x cannot notify smgr prx: %d", 1118);
                return SNS_DDF_EFAIL;
            }
        }
        else
        {
            DD_I2C_DB_2(0x1010, 0x0B);

            sample_ptr[i] = sns_ddf_memhandler_malloc(memhandler, (paramsToRet * sizeof(sns_ddf_sensor_sample_s)));
            if(sample_ptr[i] == NULL)
            {
                DD_I2C_DB_2(0x1010, 0x0C);

                return SNS_DDF_ENOMEM;
            }
        }

        DD_I2C_DB_3(0x1010, 0x0D, sensor_data[i].sensor);

        switch(sensor_data[i].sensor)
        {
        case SNS_DDF_SENSOR_AMBIENT:
            DD_I2C_DB_2(0x1010, 0x0E);

            DD_MSG_0(MED, "BldSmgrMsg: AMBIENT");
            status = ams_get_als_data(state);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x0F);

                DD_MSG_1(HIGH, "BldSmgrMsg: Get Data als err1: %d", status);
                return status;
            }

            status = ams_get_lux(state);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x10);

                DD_MSG_1(HIGH, "BldSmgrMsg: Build SMGR msg, Get Data als err2: %d", status);
                return status;
            }

            subDevIndex = subDevId_AlsProx;

            DD_MSG_0(HIGH, "BldSmgrMsg: Get Data, returning als data to smgr");

            status = ams_als_sensor_samples(state, &sensor_data[i], NULL, sample_ptr[i]);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x11);

                DD_MSG_0(HIGH, "BldSmgrMsg: Get Data, ERROR returning als data to smgr");
                return status;
            }

            break;

        case SNS_DDF_SENSOR_PROXIMITY:
            DD_I2C_DB_2(0x1010, 0x12);

            DD_MSG_0(MED, "BldSmgrMsg: PROX");
            status = ams_get_prox_data(state);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x13);

                DD_MSG_1(HIGH, "BldSmgrMsg: Get Data prx err1: %d", status);
                return status;
            }

            status = sns_dd_ams_tmg399x__alsprx_prx_binary(state);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x14);

                DD_MSG_1(HIGH, "BldSmgrMsg: Get Data prx err2: %d", status);
                return status;
            }

            subDevIndex = subDevId_AlsProx;

            status = ams_prox_sensor_samples(state, &sensor_data[i], NULL, sample_ptr[i]);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x15);

                DD_MSG_0(HIGH, "BldSmgrMsg: Get Data, ERROR returning prx data to smgr");
                return status;
            }

            break;

        case SNS_DDF_SENSOR_RGB:
            DD_I2C_DB_2(0x1010, 0x16);

            // Get RGB and clear data
            status = ams_get_als_data(state);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x17);

                DD_MSG_1(HIGH, "BldSmgrMsg: Get Data als err1: %d", status);
                return status;
            }

            subDevIndex = subDevId_RgbCt;

            DD_MSG_0(HIGH, "BldSmgrMsg: returning RGB data to smgr");
            status = ams_rgb_sensor_samples(state, &sensor_data[i], NULL, sample_ptr[i]);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x18);

                DD_MSG_0(HIGH, "BldSmgrMsg: Get Data, ERROR returning als data to smgr");
                return status;
            }

            break;

        case SNS_DDF_SENSOR_CT_C:
            DD_I2C_DB_2(0x1010, 0x19);

            // Get RGB and clear data
            status = ams_get_als_data(state);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x1A);

                DD_MSG_1(HIGH, "BldSmgrMsg: Get Data als err1: %d", status);
                return status;
            }

            // Compute color temp (by-product of lux computation)
            status = ams_get_lux(state);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x1B);

                DD_MSG_1(HIGH, "BldSmgrMsg: Get Data als err2: %d", status);
                return status;
            }

            subDevIndex = subDevId_RgbCt;

            DD_MSG_0(HIGH, "BldSmgrMsg: Get Data, returning color temp and clear data to smgr");
            status = ams_cct_clear_sensor_samples(state, &sensor_data[i], NULL, sample_ptr[i]);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x1010, 0x1C);

                DD_MSG_0(HIGH, "BldSmgrMsg: Get Data, ERROR returning als data to smgr");
                return status;
            }

            break;

        default:
            DD_I2C_DB_2(0x1010, 0x1D);

            break;
        }  // switch(sensor_data[i].sensor)
    }  // for(i = 0; i < numOfSensors; i++)

    if(memhandler == NULL)
    {
        DD_MSG_0(MED, "BldSmgrMsg: exit memhandler = NULL");

        DD_I2C_DB_2(0x1010, 0x1F);
        DD_I2C_DB_3(0x1010, 0x1F, numOfSensors);
        DD_I2C_DB_3(0x1010, 0x1F, subDevIndex);


        DD_MSG_0(MED, "BldSmgrMsg: exit before smgr_notify_data()");

        status = sns_ddf_smgr_notify_data(state->sub_dev[subDevIndex].smgr_handle, sensor_data, numOfSensors);

        DD_MSG_1(MED, "BldSmgrMsg: exit after smgr_notify_data() %d", status);

        for(i = 0; i < numOfSensors; i++)
        {
            DD_MSG_1(MED, "BldSmgrMsg: exit mfree(sample_ptr[%d])", i);

            sns_ddf_mfree(sample_ptr[i]);
            DD_I2C_DB_2(0x1010, 0x21+i);
        }

        DD_MSG_0(MED, "BldSmgrMsg: exit mfree(sensor_data)");

        sns_ddf_mfree(sensor_data);
        DD_I2C_DB_2(0x1010, 0x23);
    }

    DD_I2C_DB_2(0x1010, 0xFF);

    DD_MSG_0(MED, "BldSmgrMsg: EXIT");

    return status;
}


#ifdef USE_AMS_GESTURE
void ams_setupCalibrateOffset
(
 sns_dd_state_t*  state
)
{
    ams_i2c_read_byte(state, AMS_REG_ENABLE,    &enableRegCal);
    ams_i2c_read_byte(state, AMS_REG_CONFIG_AB, &gconfig4RegCal);

    // Turn off ALS and GES
    ams_clrBits_r(state, AMS_REG_ENABLE, (AMS_MSK_AEN | AMS_MSK_GEN));
    ams_setBits_r(state, AMS_REG_ENABLE, (AMS_MSK_PEN | AMS_MSK_PIEN));

    calInitialize(state);

    doCalibration = TRUE;
    firstProx     = TRUE;
}


static sns_ddf_status_e ams_readGestureData
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t     numOfDataSets;
    uint16_t    numOfBytes;
    uint8_t    *pFifoData;

    //
    // Read gesture data from Scorpion 6 FIFO until empty
    //

    sumOfFifoBytesRead = 0;
    numOfDataSets      = 1;
    pFifoData          = &gestureFifoData[0];

    do
    {
        // Have a Gesture interrupt, read four bytes from FIFO, then read the
        // FIFO level register to find out if ther are more data in the FIFO.

        numOfBytes = numOfDataSets << 2;   // numOfDataSets * 4;
        ams_i2c_read_buf(state, AMS_REG_GFIFO_N, numOfBytes, pFifoData);
        sumOfFifoBytesRead += numOfBytes;

        ams_i2c_read_byte(state, AMS_REG_GFLVL, &numOfDataSets);
    } while( numOfDataSets != 0 );

    // Do Calibration
    if(doCalibration)
    {
        if(! calIsGestureFinished())
        {
            ams_setField_s(state, GMODE, 1);
        }
        else  // Gesture is finished
        {
            doCalibration = false;

            ams_i2c_write_byte(state, AMS_REG_CONFIG_AB, gconfig4RegCal);
            ams_i2c_write_byte(state, AMS_REG_ENABLE,    enableRegCal);
        }
    }
    else  // if(!doCalibration)
    {
        // Send data to be processed and generate a gesture.
    }

    return status;
}
#endif  // USE_AMS_GESTURE


static void ams_read_status_reg
(
 sns_dd_state_t* state,
 sns_ddf_time_t  timestamp,
 AMS_SIG_TYPE    sig_type,
 uint32_t        sub_dev_id
)
{
    uint8_t  statusReg;
    uint8_t  numOfSensors;
    uint8_t  clrInterrupts;
    bool     hasAlsInt;
    bool     hasProxInt;
#ifdef USE_AMS_GESTUE
    bool     hasGesInt;
#endif
    sns_ddf_sensor_e *sensors = &state->chip.sensors[0];

    DD_MSG_0(HIGH, "RdStatusReg: ENTER");

    numOfSensors  = 0;
    clrInterrupts = 0;

    ams_i2c_read_byte(state, AMS_REG_STATUS, &state->chip.shadow.val[AMS_REG_STATUS]);
    statusReg = state->chip.shadow.val[AMS_REG_STATUS];

    hasAlsInt  = ((statusReg & AMS_INT_AINT) != 0) || ((statusReg & AMS_INT_AVALID) != 0);
    hasProxInt = ((statusReg & AMS_INT_PINT) != 0) || ((statusReg & AMS_INT_PVALID) != 0);
#ifdef USE_AMS_GESTUE
    hasGesInt  = (statusReg & AMS_INT_GINT) != 0;
    DD_MSG_3(MED, "RdStatusReg: has interrupts A:%d, P:%d, G:%d", hasAlsInt, hasProxInt, hasGesInt);
#endif

    DD_MSG_1(MED, "RdStatusReg: status_reg is: 0x%02X", statusReg);
    DD_MSG_1(MED, "RdStatusReg: ALS  active: %d", currentlyActiveSensors[AMS_SENSOR_ALS]);
    DD_MSG_1(MED, "RdStatusReg: Prox active: %d", currentlyActiveSensors[AMS_SENSOR_PROX]);
    DD_MSG_1(MED, "RdStatusReg: RGB  active: %d", currentlyActiveSensors[AMS_SENSOR_RGB]);
    DD_MSG_1(MED, "RdStatusReg: CT   active: %d", currentlyActiveSensors[AMS_SENSOR_CT]);

    if(hasAlsInt && currentlyActiveSensors[AMS_SENSOR_ALS])
    {
        //
        // ALS interrupt and RGB CLEAR
        //

        DD_MSG_0(MED, "RdStatusReg: in ALS");

        clrInterrupts |= regAddr[AMS_REG_AICLEAR];

        //
        // On an ALS interrupt, will read the ALS data and store it.
        // Will NOT return data (lux).
        // Lux will be returned in the timer.
        //
        sensors[numOfSensors] = SNS_DDF_SENSOR_AMBIENT;
        numOfSensors++;

        DD_MSG_1(MED, "RdStatusReg: ALS Interrupt, data read: 0x%02X", AMS_INT_AINT);
        DD_MSG_1(MED, "RdStatusReg: ALS int numOfSensors: %d", numOfSensors);
    }

    if(hasProxInt && currentlyActiveSensors[AMS_SENSOR_PROX])
    {
        //
        // PROX interrupt
        //

        DD_MSG_0(MED, "RdStatusReg: in PROX");

        if(firstProx)
        {
            firstProx = false;
            ams_i2cWriteField_r(state, PPERS,  2);
            ams_i2cWriteField_r(state, PITHL, 0x00);
            ams_i2cWriteField_r(state, PITHH, state->chip.setup.prox.detect);
        }

        clrInterrupts |= regAddr[AMS_REG_PICLEAR];
        sensors[numOfSensors] = SNS_DDF_SENSOR_PROXIMITY;
        numOfSensors++;

        DD_MSG_1(MED, "RdStatusReg: out of prox data and prx_binary set state: %d", 1115);
    }

#ifdef USE_AMS_GESTUE
    if(hasGesInt)
    {
        // Read gesture data
        DD_MSG_0(MED, "RdStatusReg: in Gesture");

        ams_readGestureData(state);
    }
#endif  // USE_AMS_GESTURE

    state->chip.numOfSensors  = numOfSensors;
    state->chip.clrInterrupts = clrInterrupts;

    if(numOfSensors > 0)
    {
        ams_build_smgr_message(state, NULL, NULL, sub_dev_id);

        DD_MSG_1(MED, "RdStatusReg: clearing interrrupt, (05-prox 06-als 07-both) written value: 0x%02X", clrInterrupts);
        ams_i2c_write_spec_func(state->port_handle, clrInterrupts);
    }

    sensors = &state->chip.sensors[0];

    numOfSensors  = 0;
    clrInterrupts = 0;


    if(hasAlsInt && (currentlyActiveSensors[AMS_SENSOR_RGB] || currentlyActiveSensors[AMS_SENSOR_CT]))
    {
        //
        // ALS interrupt and RGB CLEAR
        //

        DD_MSG_1(MED, "RdStatusReg: ALS Interrupt  (RGB/CT): 0x%02X", AMS_INT_AINT);

        clrInterrupts |= regAddr[AMS_REG_AICLEAR];

        //
        // On an ALS interrupt, will read the ALS data and store it.
        // Will NOT return data (lux).
        // Lux will be returned in the timer.
        //
        if(currentlyActiveSensors[AMS_SENSOR_RGB])
        {
            sensors[numOfSensors] = SNS_DDF_SENSOR_RGB;
            DD_MSG_1(MED, "RdStatusReg: Read Stat Reg: ALS Interrupt sensor: 0x%02X", sensors[numOfSensors]);
            numOfSensors++;
            DD_MSG_1(MED, "RdStatusReg: Read Stat Reg: ALS int numOfSensors: %d", numOfSensors);
        }

        if(currentlyActiveSensors[AMS_SENSOR_CT])
        {
            sensors[numOfSensors] = SNS_DDF_SENSOR_CT_C;
            DD_MSG_1(MED, "RdStatusReg: Read Stat Reg: ALS Interrupt sensor: 0x%02X", sensors[numOfSensors]);
            numOfSensors++;
            DD_MSG_1(MED, "RdStatusReg: Read Stat Reg: ALS int numOfSensors: %d", numOfSensors);
        }
    }

    state->chip.numOfSensors  = numOfSensors;
    state->chip.clrInterrupts = clrInterrupts;

    if(numOfSensors > 0)
    {
        ams_build_smgr_message(state, NULL, NULL, sub_dev_id);

        DD_MSG_1(MED, "RdStatusReg: clearing interrrupt, (05-prox 06-als 07-both) written value: 0x%02X", clrInterrupts);
        ams_i2c_write_spec_func(state->port_handle, clrInterrupts);
    }

    /* If ALS interrupt triggered then update ALS threholds */
    if(hasAlsInt)
    {
        uint16_t  clear_data = state->chip.als_info.clear_raw;
        uint16_t  als_low  = clear_data - (clear_data / 20);   // 20 percent of clear data
        uint16_t  als_high = clear_data + (clear_data / 20);   // 20 percent of clear data

        // When als_low == als_high (low == high = 0), set the thresh to 1, 2 so it will get an interrupt.
        if(als_low == als_high)
        {
            als_low  = 0;
            als_high = 2;
        }

        DD_MSG_1(MED, "RdStatusReg: Als odr: %d", state->chip.setup.als.odr);
        DD_MSG_1(MED, "RdStatusReg: dri flg: %d", state->sub_dev[sub_dev_id].dri_enable);

        // Write new threshold values only if in dri mode
        if(state->sub_dev[sub_dev_id].dri_enable)
        {
            DD_MSG_0(MED, "RdStatusReg: dri enabled, set threshold and apers");
            ams_update_als_thresh(state, als_low, als_high, AMS_APERS_VAL);
        }
    }

    DD_MSG_0(MED, "RdStatusReg: EXIT");
}


static void ams_set_lux_coefs
(
 sns_dd_state_t* state
)
{
    int i;


    for(i = 0; i < AMS_MAX_NUM_OF_399x_DEVICES; ++i)
    {
        state->chip.lux_coef[i].d_factor  = ams_tmg399x_lux_coef[i].d_factor;
        state->chip.lux_coef[i].r_coef    = ams_tmg399x_lux_coef[i].r_coef;
        state->chip.lux_coef[i].g_coef    = ams_tmg399x_lux_coef[i].g_coef;
        state->chip.lux_coef[i].b_coef    = ams_tmg399x_lux_coef[i].b_coef;
        state->chip.lux_coef[i].ct_coef   = ams_tmg399x_lux_coef[i].ct_coef;
        state->chip.lux_coef[i].ct_offset = ams_tmg399x_lux_coef[i].ct_offset;
    }
}


/*
 * Provides initial operational parameter defaults.
 * These defaults may be changed through the device's get/set
 * attribute files.
 */
static sns_ddf_status_e  ams_set_default_reg_values
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e  status;
    uint8_t           devId;


    DD_MSG_1(HIGH, "in set defualt reg values ID=0x%X", state->chip.shadow.val[AMS_REG_ID]);

    // Set all register to power on reset value as defined in data sheet
    status = ams_i2c_write_buf(state, AMS_REG_ENABLE, MAX_NUM_OF_REGS_TO_RESET, (uint8_t*)ams_power_on_reset_values);
    if ( status != SNS_DDF_SUCCESS )
    {
        return status;
    }

    status = ams_i2c_read_buf(state, AMS_REG_ENABLE, (AMS_REG_CONFIG_AF - AMS_REG_ENABLE + 1), &state->chip.shadow.val[AMS_REG_ENABLE]);

    status = ams_i2c_read_buf(state, AMS_REG_REVID, 2, &state->chip.shadow.val[AMS_REG_REVID]);
    devId  = ams_getField_s(state, ID);
    if ( status != SNS_DDF_SUCCESS ) {
        DD_I2C_DB_2(0x0200, 2);
        return status;
    }

    if(!ams_validDeviceId(devId))
    {
        DD_MSG_0(MED, "defaultRegValues: device id invalid");

        return SNS_DDF_EFAIL;
    }

    switch(devId)
    {
    case 0x27:       // tmg3992
        state->chip.device_index = 0;
        break;

    case 0x2A:       // tmg3993
        state->chip.device_index = 1;
        break;

    default:    // UNKNOWN device ERROR!!!!
        state->chip.device_index = 0;
        return SNS_DDF_EFAIL;
        break;
    }

    device_index = state->chip.device_index;

    // Operational parameters
    ams_clearShadowChangedFlag(state);
    ams_set_lux_coefs(state);

    state->chip.Min_Integ_Time_us = ams_tmg399x_min_integ_time_us[state->chip.device_index];

    ams_initSensorPor(state);

    DD_MSG_1(MED, "defaultRegValues: AMS_REG_ENABLE: 0x%02X", state->chip.shadow.val[AMS_REG_ENABLE]);
    status = ams_i2c_write_byte(state, AMS_REG_ENABLE, state->chip.shadow.val[AMS_REG_ENABLE]);

    state->chip.als_info.atime = ams_getField_s(state, ATIME);
    state->chip.als_info.again = ams_getField_s(state, AGAIN);
    ams_calc_cpl(&state->chip);

    state->chip.als_gain_auto      = true;

    state->chip.setup.prox.detect  = PROX_DETECT_CNT;
    state->chip.setup.prox.release = PROX_RELEASE_CNT;

    ams_setField_s(state, PITHL, 0x00);
    ams_setField_s(state, PITHH, state->chip.setup.prox.detect);

    state->chip.prx_info.event         = SNS_PRX_FAR_AWAY;

    state->chip.setup.als.luxCorrection = 1 << 10;

    state->chip.setup.als.odr                 = 0;
    state->chip.setup.rgb.odr                 = 0;
    state->chip.setup.ct_c.odr                = 0;
    state->chip.setup.prox.odr                = 0;
    state->chip.setup.gesture.odr             = 0;

    DD_I2C_DB_2(0x0200, 0xFF);

    return status;
}


/*===========================================================================

FUNCTION      ams_set_reg_values_from_nv

DESCRIPTION   Reads values from registry NV param store and assigns to sensor

DEPENDENCIES  None

RETURN VALUE  No Return

SIDE EFFECT   None

===========================================================================*/


static void ams_set_reg_values_from_nv(sns_dd_state_t* state)
{
    int di = state->chip.device_index;


    DD_I2C_DB_2(0x0300, 0);

    // Now set NV values

    // device
    if((state->nv_db.device != 0) && (state->nv_db.device <= 9))
    {
        /* state->chip.ams_sensor = (AMS_SENSOR_TYPE)state->nv_db.device; */
    }

    // d_factor
    if(state->nv_db.d_factor != 0)
    {
        state->chip.lux_coef[di].d_factor = state->nv_db.d_factor;
    }

    // r_coef
    if(state->nv_db.r_coef != 0)
    {
        state->chip.lux_coef[di].r_coef = state->nv_db.r_coef;
    }

    // g_coef
    if(state->nv_db.g_coef != 0)
    {
        state->chip.lux_coef[di].g_coef = state->nv_db.g_coef;
    }

    // b_coef
    if(state->nv_db.b_coef != 0)
    {
        state->chip.lux_coef[di].b_coef = state->nv_db.b_coef;
    }

    // ct_coef
    if(state->nv_db.ct_coef != 0)
    {
        state->chip.lux_coef[di].ct_coef = state->nv_db.ct_coef;
    }

    // ct_offset
    if(state->nv_db.ct_offset != 0)
    {
        state->chip.lux_coef[di].ct_offset = state->nv_db.ct_offset;
    }

    // proxDetect
    if(state->nv_db.proxDetect != 0)
    {
        state->chip.setup.prox.detect = state->nv_db.proxDetect;
    }

    // proxRelease
    if(state->nv_db.proxRelease != 0)
    {
        state->chip.setup.prox.release = state->nv_db.proxRelease;
    }

    // luxCorrection
    if(state->nv_db.luxCorrection != 0)
    {
        state->chip.setup.als.luxCorrection = state->nv_db.luxCorrection;
        DD_MSG_1(MED, "lux corr written in default %d", state->chip.setup.als.luxCorrection);
    }
}


static sns_ddf_status_e ams_update_registers
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_I2C_DB_2(0x1400, 0);

    // Reset the device

    DD_I2C_DB_2(0x1400, 1);

    return status;
}


static sns_ddf_status_e ams_resetRegisters
(
 sns_dd_state_t*  state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_I2C_DB_2(0x5500, 0);

    // Reset driver state
    ams_set_default_reg_values(state);

    // Now set the usable values
    ams_set_reg_values_from_nv(state);

    // Set register values
    status = ams_update_registers(state);

    return status;
}


#define NUM_OF_LUX_TO_AVE  8


static sns_ddf_status_e ams_compute_lux_ave
(
 sns_dd_state_t* state
)
{
    uint32_t         luxSum;
    int              i;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    luxSum = 0;

    DD_MSG_0(HIGH, "about to get als data and mlux ");

    for(i = 0; i < NUM_OF_LUX_TO_AVE; ++i)
    {
        ams_readDataRegisters(state);
        status = ams_get_als_data(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(HIGH, "failed in get als data ");
            /* return status; */
        }

        status = ams_get_lux(state);
        if(status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(HIGH, "failed in app get mlux ");
            /* return status; */
        }

        luxSum += state->chip.als_info.lux;
        DD_MSG_2(MED, "summing lux luxsum %d curent loop: %d", luxSum, i);

        // Wait some time before another reading
        sns_ddf_delay(15000);
        DD_MSG_0(HIGH, "after 15000 us delay ");
    }

    state->chip.als_info.luxAve = (luxSum / NUM_OF_LUX_TO_AVE);
    DD_MSG_2(MED, "luxsum %d luxsum/8: %d", luxSum, ((luxSum / NUM_OF_LUX_TO_AVE)));

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      ams_calibrate_als

DESCRIPTION   Called from self-test. compares avg lux received to avg lux
              expected and calculates scaling factor.

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/


static sns_ddf_status_e ams_calibrate_als
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    uint32_t  tgtLux;
    uint32_t  avgLux;

    avgLux = 1;
    tgtLux = 1;

    // compare the average lux to the passed in calibration lux,
    DD_MSG_0(HIGH, "CAL_ALS: in calibrate als next compute lux ");
    status = ams_compute_lux_ave(state);
    if(status != SNS_DDF_SUCCESS)
    {
        DD_MSG_0(HIGH, "CAL_ALS: computelux ave failed ");
        return status;
    }
    DD_MSG_0(HIGH, "CAL_ALS: out of compute lux ");

    // Find Lux mid way between calLuxLower and calLuxUpper
    tgtLux = (state->nv_db.calLuxLower + state->nv_db.calLuxUpper) / 2;
    DD_MSG_2(MED, "CAL_ALS: tgtLux: %d callxlw: %d", tgtLux, state->nv_db.calLuxLower);
    //tgtLux= tgtLux;    // removed left shift by 1024
    avgLux = state->chip.als_info.luxAve;

    DD_MSG_2(MED, "CAL_ALS: tgtLux<<10: %d callxhi: %d", tgtLux, state->nv_db.calLuxUpper);

    // Mult by 2^10
    state->nv_db.luxCorrection = ((tgtLux << 10) / avgLux);

    state->chip.setup.als.luxCorrection = state->nv_db.luxCorrection;

    DD_MSG_2(MED, "CAL_ALS: avgLux: %d new val of state->nv_db.luxCorrection: %d", avgLux, state->nv_db.luxCorrection);

    // To use:
    // ((lux * state->nv_db.luxCorrection) + 512) / 1024;
    // or
    // ((lux * state->nv_db.luxCorrection) + 512) >> 10;

    return status;
}

//
// Combo Sensor
//


sns_ddf_status_e sns_dd_read_registry
(
 sns_dd_state_t*      state,
 uint32_t             sub_dev_id,
 sns_ddf_nv_params_s* reg_params
 )
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_I2C_DB_2(0x0400, 0);

    // handle registry if applicable
    DD_MSG_0(HIGH, "readRegy: set def values");

    state->nv_db_size = 0;      // variable to store size of nv_

    // Load default values in driver - these will be overwritten if
    // registry has good values. if not, these will be written back
    // to nv_db
    ams_set_default_reg_values(state);

    // Recall NV parameters, if any
    if(reg_params)
    {
        bool data_from_registry = false;

        DD_I2C_DB_2(0x0400, 1);

        DD_MSG_0(HIGH, "readRegy: yes reg params");
        if ( (reg_params->status   == SNS_DDF_SUCCESS) &&
             (reg_params->data_len >= sizeof(sns_dd_nv_db_type))
             )
        {
            DD_MSG_0(HIGH, "readRegy: reg params match nv db type");
            sns_dd_nv_db_type *nv_ptr = (sns_dd_nv_db_type *)reg_params->data;
            if ( (nv_ptr->ams_num == ams_magic_num) &&
                 (nv_ptr->drv_ID  == ams_driver_ID))
            {
                DD_I2C_DB_2(0x0400, 2);

                // update value from NV parameters
                DD_MSG_0(HIGH, "readRegy: magic num verified");
                //sns_dd_alsprx_common_db is defined in tmg399x_priv.h file
                data_from_registry = true;
                state->nv_db_size    = reg_params->data_len;

                memcpy(&state->nv_db, nv_ptr, sizeof(sns_dd_nv_db_type));
                DD_MSG_0(HIGH, "readRegy: in reg params past memcopy");
            }
        }

        if (!data_from_registry)
        {
            int di = state->chip.device_index;

            DD_I2C_DB_2(0x0400, 3);
            DD_I2C_DB_3(0x0400, 0x31, AMS_SENSOR);
            DD_I2C_DB_3(0x0400, 0x32, ams_driver_ID);


            DD_MSG_0(HIGH, "readRegy: in no reg data");

            state->nv_db_size           = REG_FIXED_NV_SIZE_128; //defined as 128 max

            state->nv_db.visible_ratio  = SNS_DD_VISIBLE_TRANS_RATIO;
            state->nv_db.ir_ratio       = SNS_DD_IR_TRANS_RATIO;
            state->nv_db.dc_offset      = SNS_DD_DC_OFFSET;
            state->nv_db.thresh_near    = SNS_DD_PRX_THRESH_NEAR;
            state->nv_db.thresh_far     = SNS_DD_PRX_THRESH_FAR;
            state->nv_db.prx_factor     = SNS_DD_PRX_FACTOR;
            state->nv_db.als_factor     = SNS_DD_ALS_FACTOR;

            state->nv_db.ams_num        = ams_magic_num;
            state->nv_db.drv_ID         = ams_driver_ID;
            state->nv_db.calibratePhone = 1;
            state->nv_db.device         = AMS_SENSOR;
            state->nv_db.calLuxLower    = 250;      //avg set as 300
            state->nv_db.calLuxUpper    = 500;

            state->nv_db.d_factor       = state->chip.lux_coef[di].d_factor;
            state->nv_db.r_coef         = state->chip.lux_coef[di].r_coef;
            state->nv_db.g_coef         = state->chip.lux_coef[di].g_coef;
            state->nv_db.b_coef         = state->chip.lux_coef[di].b_coef;
            state->nv_db.ct_coef        = state->chip.lux_coef[di].ct_coef;   // New
            state->nv_db.ct_offset      = state->chip.lux_coef[di].ct_offset; // New

            state->nv_db.proxDetect     = PROX_DETECT_CNT;
            state->nv_db.proxRelease    = PROX_RELEASE_CNT;
            state->nv_db.luxCorrection  = 1 << 10;
            DD_MSG_0(HIGH, "readRegy: past setting defaults for reg data");
            /*  SNS_OS_MEMZERO(state->nv_db.reserved, sizeof(state->nv_db.reserved)); (no reserved space here)*/
        }

        // Now set the usable values
        ams_set_reg_values_from_nv(state);
        DD_MSG_0(HIGH, "readRegy: setting defaults from nv");

        if (state->nv_db.calibratePhone)
        {
            state->calibration_mode = true;
            DD_MSG_0(HIGH, "readRegy: setting phone calib  true");
        }

        // Notify to write back to registry in case defaults had to be loaded.
        if ( (reg_params) && (!data_from_registry) )
        {
            DD_I2C_DB_2(0x0400, 4);

            DD_MSG_0(HIGH, "readRegy: before notify");
#ifdef USE_DD_I2C_DB
            // Do not notify SMGR of event
#else
            sns_ddf_smgr_notify_event(state->sub_dev[sub_dev_id].smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
#endif
            DD_MSG_0(HIGH, "readRegy: after notify");

            DD_I2C_DB_2(0x0400, 5);
        }
    }
    DD_MSG_1(HIGH, "readRegy: NV size = %d", state->nv_db_size);
    return status;
}


sns_ddf_status_e sns_dd_ams_tmg399x_enable_sched_data
(
 sns_ddf_handle_t dd_handle,
 sns_ddf_sensor_e sensor,
 bool             enable
)
{
    sns_dd_state_t* state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t        sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);

    uint8_t byteVal;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    DD_I2C_DB_2(0x8000, 0);
    DD_I2C_DB_3(0x8000, 1, sensor);
    DD_I2C_DB_3(0x8000, 2, enable);


    if ( (state == NULL) || (sub_dev_id >= DD_NUM_SUB_DEV) ) {
        return SNS_DDF_EINVALID_PARAM;
    }

    //
    // Clear interrupts
    // Clear any pending interrupts
    byteVal = regAddr[AMS_REG_AICLEAR];
    status = ams_i2c_write_spec_func(state->port_handle, byteVal);
    if ( status != SNS_DDF_SUCCESS ) {
        return status;
    }

    DD_MSG_2(MED, "TMG399x enable: %d var2: %d", 1100, enable);

    // set up for run.
    //

    if(enable)
    {
        if(state->sub_dev[sub_dev_id].dri_enable)
        {
            DD_I2C_DB_2(0x8000, 5);

            // IRQ's are enabled, register the irq
            status = sns_ddf_signal_register(state->sub_dev[sub_dev_id].interrupt_gpio,
                                             dd_handle,
                                             &sns_ams_tmg399x_alsprx_driver_fn_list,
                                             SNS_DDF_SIGNAL_IRQ_FALLING);

            if( status != SNS_DDF_SUCCESS )
            {
                DD_I2C_DB_2(0x8000, 6);

                return status;
            }

            // Set APERS to interrupt when two values are out of range.
            //ams_i2cWriteField_s(state, APERS, AMS_APERS_VAL);
        }
        else
        {
            DD_I2C_DB_2(0x8000, 7);

            status =  SNS_DDF_EFAIL;
        }
    }
    else
    {
        uint16_t i;
        bool     activeSensors = false;

        DD_I2C_DB_2(0x8000, 8);

        for(i = 0; i < MAX_NUM_OF_SENSORS; ++i)
        {
            activeSensors |= currentlyActiveSensors[i];
        }

        if(activeSensors == false)
        {
            // If NO sensors are active then deregister irq
            sns_ddf_signal_deregister(state->sub_dev[sub_dev_id].interrupt_gpio);
        }
    }

    DD_I2C_DB_2(0x8000, 0xFF);

    return status;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_get_data

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
static sns_ddf_status_e sns_dd_ams_tmg399x_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
    uint8_t          i = 0;
    sns_ddf_status_e status = SNS_DDF_PENDING;
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);
    sns_ddf_timer_s  timer_obj = 0;
    //uint32_t         odrVal = 0;


    DD_I2C_DB_2(0x1000, 0);
    DD_I2C_DB_3(0x1000, 1, num_sensors);

    DD_MSG_0(HIGH, "GetData: ENTER");
    DD_MSG_1(MED,  "GetData: num of sensors: %d", num_sensors);

    if(num_sensors == 0)
    {
        DD_MSG_0(HIGH, "GetData: num_sensors = 0");

        return SNS_DDF_EINVALID_PARAM;
    }

    if ( (state == NULL) || (sub_dev_id >= DD_NUM_SUB_DEV) ) {
        DD_MSG_1(HIGH, "GetData: state == NULL or sub_dev_id >= NUM_SUB_DEV (%d)", sub_dev_id);

        return SNS_DDF_EINVALID_PARAM;
    }

    state->chip.numOfSensors = num_sensors;

    for( i = 0; i < num_sensors; ++i)
    {
        DD_I2C_DB_3(0x1000, 0x02, sensors[i]);
        state->chip.sensors[i] = sensors[i];

        DD_MSG_2(MED,  "GetData: sensor[%d]: %d", i, sensors[i]);

       //ams_tmg399x_state_machine(state, sensors[i], (odrVal != 0));   // ((*(uint32_t*)value) != 0));

        switch(sensors[i])
        {
        case SNS_DDF_SENSOR_PROXIMITY:
            //odrVal    = state->chip.setup.prox.odr;
            timer_obj = state->sub_dev[sub_dev_id].timer_obj[0];
            break;

        case SNS_DDF_SENSOR_AMBIENT:
            //odrVal    = state->chip.setup.als.odr;
            timer_obj = state->sub_dev[sub_dev_id].timer_obj[1];
            break;

        case SNS_DDF_SENSOR_RGB:
            //odrVal    = state->chip.setup.rgb.odr;
            timer_obj = state->sub_dev[sub_dev_id].timer_obj[0];
            break;

        case SNS_DDF_SENSOR_CT_C:
            //odrVal    = state->chip.setup.ct_c.odr;
            timer_obj = state->sub_dev[sub_dev_id].timer_obj[1];
            break;

        /* case SNS_DDF_SENSOR_IR_GESTURE: */
        /*     odrVal = state->chip.setup.gesture.odr; */
        /*     break; */

        default:
            break;
        }
    }

    DD_I2C_DB_3(0x1000, 0x03, sub_dev_id);
    //DD_I2C_DB_3(0x1000, 0x04, odrVal);
    //DD_I2C_DB_3(0x1000, 0x05, ams_odr_to_uSec[odrVal - 1]);

    //DD_MSG_1(HIGH, "GetData: odrVal: %d", odrVal);

    if(timer_obj != 0)
    {
        DD_MSG_1(HIGH, "GetData: Start timer for %d ms", MAX_CYCLE_TIME_ms);

        status = sns_ddf_timer_start(timer_obj, MS_TO_US(MAX_CYCLE_TIME_ms));
        if(status != SNS_DDF_SUCCESS)
        {
           return status;
        }
    }
    else
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_I2C_DB_2(0x1000, 0xFF);

    //status = ams_build_smgr_message(state, memhandler, data, sub_dev_id);

    DD_MSG_0(HIGH, "GetData: EXIT, PENDING");

    return SNS_DDF_PENDING;  // SNS_DDF_SUCCESS;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the TMG 399x ALS/Prx sensor

  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode and range.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value       Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_ams_tmg399x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);
    sns_ddf_status_e status     = SNS_DDF_SUCCESS;

    DD_I2C_DB_3(0x2000, 0x00, attrib);
    DD_I2C_DB_3(0x2000, 0x01, sensor);

    DD_MSG_0(MED, "setAttrib: ");

    if ( (dd_handle == NULL) ||
         (value == NULL) ||
         (sub_dev_id ==  0xFF))
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_POWER_STATE:
        /* set power mode */
        DD_I2C_DB_2(0x2000, 0x02);
        DD_MSG_0(MED, "setAttrib: power mode set");
        status = sns_dd_ams_tmg399x__alsprx_set_powerstate(state, sub_dev_id,
                                                           *(sns_ddf_powerstate_e *)value,
                                                           sensor,
                                                           TRUE);
        if(status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(MED, "setAttrib: ");
            return status;
        }

        ams_calc_cpl(&state->chip);

        break;

    case SNS_DDF_ATTRIB_ODR:
        DD_I2C_DB_2(0x2000, 0x03);

        DD_MSG_1(MED, "setAttrib: set odr to: %d", *(uint32_t*)value);

        // State machine
        ams_tmg399x_state_machine(state, sensor, ((*(uint32_t*)value) != 0));
        // This second call to state_machine ensures first data notification
        // for als/rgb/ct in standalone mode is sent out. Without this call
        // the first interrupt does not fire. TODO - find out why this behavior
        ams_tmg399x_state_machine(state, sensor, ((*(uint32_t*)value) != 0));

        switch(sensor)
        {
        case SNS_DDF_SENSOR_PROXIMITY:
            DD_I2C_DB_2(0x2000, 0x04);
            state->chip.setup.prox.odr = *(uint32_t*)value;

            //if (state->chip.setup.prox.odr > 5)
            //{
            //    state->chip.setup.prox.odr = 5;
            //    DD_MSG_1(MED, "Set prox odr to max  %d", state->chip.setup.prox.odr);
            //}

            break;

        case SNS_DDF_SENSOR_AMBIENT:
            state->chip.setup.als.odr = *(uint32_t*)value;
            break;

        case SNS_DDF_SENSOR_RGB:
            state->chip.setup.rgb.odr = *(uint32_t*)value;
            break;

        case SNS_DDF_SENSOR_CT_C:
            DD_I2C_DB_2(0x2000, 0x05);
            state->chip.setup.ct_c.odr = *(uint32_t*)value;
            break;

        case SNS_DDF_SENSOR_IR_GESTURE:
            DD_I2C_DB_2(0x2000, 0x06);
            state->chip.setup.gesture.odr = *(uint32_t*)value;
            break;

        default:
            DD_I2C_DB_2(0x2000, 0x07);
            break;
        }

        ams_calc_cpl(&state->chip);

        break;

    default:
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_3(MED, "setAttrib: : %d final wtime: %d final ltime: %d", 1157, state->chip.setup.wait.wtime, state->chip.setup.wait.lTime);

    /* status = ams_write_wtime(state); */

    DD_I2C_DB_2(0x2000, 0xFF);

    return(status);
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_handle_timer

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
static void sns_dd_ams_tmg399x_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void*            arg
)
{
    sns_ddf_time_t   timestamp;
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);

    DD_I2C_DB_2(0x4000, 0);

    DD_MSG_0(LOW, "HndlTimer:");

    timestamp = sns_ddf_get_timestamp();

    ams_read_status_reg(state, timestamp, TIMER_TYPE, sub_dev_id);

    /* ams_build_smgr_message(state, NULL, NULL, sub_dev_id); */
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_handle_irq

===========================================================================*/
/*!
  @brief AMS Tmg399x interrupt handler

  @detail
  This function will be called within the context of the Interrupt
  Service Task (IST), *not* the ISR.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] irq        The IRQ representing the interrupt that occured.

  @return None
*/
/*=========================================================================*/
void sns_dd_ams_tmg399x_handle_irq
(
  sns_ddf_handle_t  dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
    sns_dd_state_t* state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t        sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);

    DD_I2C_DB_2(0x5000, 0);

    DD_MSG_1(MED, "TMG399x in handle irq: %d", 1124);

    ams_read_status_reg(state, timestamp, IRQ_TYPE, sub_dev_id);
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_reset

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
sns_ddf_status_e sns_dd_ams_tmg399x_reset
(
 sns_ddf_handle_t dd_handle
)
{
    sns_dd_state_t*  state           = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id      = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);
    sns_ddf_status_e status          = SNS_DDF_SUCCESS;
    bool             reset_registers = true;

    DD_I2C_DB_2(0x6000, 0x00);
    DD_I2C_DB_3(0x6000, 0x01, sub_dev_id);
    DD_I2C_DB_3(0x6000, 0x11, subDevId_AlsProx);
    DD_I2C_DB_3(0x6000, 0x12, subDevId_RgbCt);

    /* basic sanity check */
    if ( dd_handle == NULL || sub_dev_id >= DD_NUM_SUB_DEV )
    {
        DD_I2C_DB_2(0x6000, 0x02);

        return SNS_DDF_EINVALID_PARAM;
    }

    // state variables reset
    state->sub_dev[sub_dev_id].powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
    state->sub_dev[sub_dev_id].odr = 0;

    DD_MSG_3(HIGH,"sns_dd_reset - enter sub_dev_id=%d subDevId_RgbCt=%d subDevId_AlsProx=%d ", sub_dev_id, subDevId_RgbCt, subDevId_AlsProx);

    if(subDevId_AlsProx != -1)
    {
        DD_I2C_DB_2(0x6000, 0x03);

        if(state->sub_dev[subDevId_AlsProx].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        {
            DD_I2C_DB_2(0x6000, 0x04);
            DD_MSG_0(HIGH,"sub device Als/Prox is active");

            reset_registers = false;
        }

        // MW test
        //sns_ddf_smgr_notify_event(state->sub_dev[subDevId_AlsProx].smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
    }

    if(subDevId_RgbCt != -1)
    {
        DD_I2C_DB_2(0x6000, 0x05);

        if(state->sub_dev[subDevId_RgbCt].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        {
            DD_I2C_DB_2(0x6000, 0x06);
            DD_MSG_0(HIGH,"sub device Rgb/Ct is active");

            reset_registers = false;
        }
    }

    if(subDevId_Gesture != -1)
    {
        DD_I2C_DB_2(0x6000, 0x05);

        if(state->sub_dev[subDevId_Gesture].powerstate != SNS_DDF_POWERSTATE_LOWPOWER)
        {
            DD_I2C_DB_2(0x6000, 0xF6);
            DD_MSG_0(HIGH,"sub device Gesture is active");

            reset_registers = false;
        }
    }

    if( reset_registers )
    {
        DD_I2C_DB_2(0x6000, 0x07);

        // Configure device registers - initiate device reset
        DD_MSG_0(HIGH,"reseting sensor device");

        status = ams_resetRegisters(state);

        currentState  = AMS_STATE_OFF;         // Idle
        currentSensor = SNS_DDF_SENSOR__NONE;  // undefined

        // No sensors active:
        memset(&currentlyActiveSensors[0], 0, sizeof(currentlyActiveSensors));
    }

    DD_MSG_0(HIGH,"sns_dd_reset - exit");

    DD_I2C_DB_2(0x6000, 0xFF);

    return status;
}


/*===========================================================================

FUNCTION      sns_dd_ams_tmg399x_run_test

DESCRIPTION   OEM Self Test to calibrate ALS Lux. Takes several lux readings
              and calculates scaling factor for ALS reading

DEPENDENCIES  None

RETURN VALUE  Status

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_run_test
( sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
)
{
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)dd_handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)dd_handle & ((uint32_t)DD_HANDLE_ALIGN);
    sns_ddf_status_e status     = SNS_DDF_EINVALID_TEST;
    sns_ddf_handle_t smgr_handle;
    uint16_t         deviceId;
    uint16_t         prxLoopCnt = 0;


    /* basic sanity check */
    if ( (state == NULL) || (sub_dev_id >= DD_NUM_SUB_DEV) )
    {
        DD_MSG_0(HIGH, "run_test: Sanity Check FAILED");

        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_0(HIGH, "run_test: ");

    switch(sensor)
    {
    case SNS_DDF_SENSOR_AMBIENT:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_AMBIENT: ");

        // Test the ALS
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_AMBIENT: SNS_DDF_TEST_SELF");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        case SNS_DDF_TEST_OEM:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_AMBIENT: SNS_DDF_TEST_OEM");

            // Perform ams specific test to Calibrate ALS
            if(state->nv_db.calibratePhone)
            {
                // Set Default values
                //fv Read NV
                //fv ams_set_default_reg_values(state);

                status = sns_dd_ams_tmg399x_reset(dd_handle);
                DD_MSG_0(HIGH, "run_test: after reset");
                if( status != SNS_DDF_SUCCESS )
                {
                    DD_MSG_0(HIGH, "run_test: set def reg values failed ");
                    return status;
                }

                /* alsTime_us  = 0; */
                /* proxTime_us = 0; */

                state->chip.setup.als.odr = 10;

                //  enable als and prox. sensor
                status = sns_dd_ams_tmg399x__alsprx_set_powerstate(state, sub_dev_id,
                                                                   SNS_DDF_POWERSTATE_ACTIVE ,SNS_DDF_SENSOR__ALL, TRUE);

                ams_tmg399x_state_machine(state, SNS_DDF_SENSOR_AMBIENT, true);

                //alsTime_us  = ams_als_time_us(state);
                /* proxTime_us = ams_prox_time_us(state); */
                //ams_compute_new_wtime(state, state->chip.setup.als.odr, alsTime_us);

                //ams_compute_new_wtime(state, state->chip.setup.prox.odr, proxTime_us);
                //status = ams_write_wtime(state);
                sns_ddf_delay(125000);

                // update als scaling
                DD_MSG_0(HIGH, "run_test: into als scaling");
                status = ams_calibrate_als(state);
                if( status != SNS_DDF_SUCCESS )
                {
                    DD_MSG_0(HIGH, "run_test: calib als failed ");
                    return status;
                }

                DD_MSG_0(HIGH, "run_test: out of als scaling");
                smgr_handle = state->sub_dev[sub_dev_id].smgr_handle;
                DD_MSG_0(HIGH, "run_test: reg update notify next");

                status = sns_ddf_smgr_notify_event(smgr_handle, SNS_DDF_SENSOR_AMBIENT, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
                if( status != SNS_DDF_SUCCESS )
                {
                    DDF_MSG_0(HIGH, "run_test: Could not notify SMGR");

                    return status;
                }
                DD_MSG_0(HIGH, "run_test: successful reg update notify");
            }
            else  // do not calibratePhone
            {
                DD_MSG_0(HIGH, "run_test: do not calibratePhone");

                return SNS_DDF_EFAIL;
            }
            break;

        default:
            return SNS_DDF_EINVALID_TEST;
        }
        break;

    case SNS_DDF_SENSOR_PROXIMITY:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_PROXIMITY: ");

        /* TODO: Add Prox calibration */
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_PROXIMITY: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        case SNS_DDF_TEST_OEM:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_PROXIMITY: SNS_DDF_TEST_OEM: ");

            prxLoopCnt = 0;

            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: init prox");

            if((status = sns_dd_ams_tmg399x_reset(dd_handle)) != SNS_DDF_SUCCESS) return status;

            //  enable als and prox. sensor
            state->chip.setup.als.odr = 10;
            ams_tmg399x_state_machine(state, SNS_DDF_SENSOR_PROXIMITY, true);
            if((status = sns_dd_ams_tmg399x__alsprx_set_powerstate(state, sub_dev_id,
                                                                   SNS_DDF_POWERSTATE_ACTIVE,
                                                                   SNS_DDF_SENSOR__ALL, TRUE)) != SNS_DDF_SUCCESS) return status;

            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: SNS_PRX_FAR_AWAY");
            while(state->chip.prx_info.event != SNS_PRX_NEAR_BY)
            {
                ams_readDataRegisters(state);
                ams_get_prox_data(state);
                sns_dd_ams_tmg399x__alsprx_prx_binary(state);
                sns_ddf_delay(10000);

                if(++prxLoopCnt > 200)
                {
                    DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: Failed");
                    return SNS_DDF_EFAIL;
                }
                DD_MSG_1(HIGH, "run_test: PRX-TEST-OEM: prxLoopCnt: %d", prxLoopCnt);
            }
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: SNS_PRX_NEAR_BY");

            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: Get smgr handle");
            smgr_handle = state->sub_dev[sub_dev_id].smgr_handle;
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: reg update notify next");
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: successful reg update notify");
            break;

        default:
            DD_MSG_0(HIGH, "run_test: PRX-TEST-OEM: NOT SNS_DDF_TEST_OEM");
            return SNS_DDF_EINVALID_TEST;
            break;
        }
        break;

    case SNS_DDF_SENSOR_RGB:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_RGB");
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_RGB: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        default:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_RGB: UNKNOWN TEST");
            return SNS_DDF_EINVALID_TEST;
            break;
        }

        break;

    case SNS_DDF_SENSOR_CT_C:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_CT_C");
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_CT_C: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
                return status;
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        default:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_CT_C: UNKNOWN TEST");
            return SNS_DDF_EINVALID_TEST;
            break;
        }

        break;

    case SNS_DDF_SENSOR_IR_GESTURE:
        DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_IR_GESTURE");
        switch(test)
        {
        case SNS_DDF_TEST_SELF:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_IR_GESTURE: SNS_DDF_TEST_SELF: ");

            deviceId = ams_getField_r(state, ID, &status);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_MSG_0(ERROR, "run_test: I2C Read failed");
            }

            // Now check if the device ID is valid
            if(!ams_validDeviceId(deviceId))
            {
                DD_MSG_1(ERROR, "run_test: device id 0x%02X is invalid", deviceId);
                return SNS_DDF_EFAIL;
            }
            break;

        default:
            DD_MSG_0(HIGH, "run_test: SNS_DDF_SENSOR_IR_GESTURE: UNKNOWN TEST");
            return SNS_DDF_EINVALID_TEST;
            break;
        }

        break;

    default:  // UNKNOWN SENSOR
        DD_MSG_0(HIGH, "run_test: RUN_TEST: UNKNOWN SENSOR");
        return SNS_DDF_EINVALID_PARAM;
        break;
    }


    return status;
}


sns_ddf_status_e sns_dd_ams_tmg399x_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
 )
{
    sns_ddf_handle_t    port_handle;
    sns_ddf_status_e    ret = SNS_DDF_SUCCESS;
    uint8_t             chipId;
    uint8_t             read_count;
    uint8_t             revId[2] = {0};

    DD_MSG_0(HIGH, "sns_dd_ams_tmg399x_probe - enter");

    /* Basic sanity check*/
    if (device_info == NULL || sensors == NULL || num_sensors == NULL)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    *num_sensors = 0;
    *sensors = NULL;

    /* open I2C port */
    ret = sns_ddf_open_port(&port_handle, &device_info->port_config);
    if (ret != SNS_DDF_SUCCESS)
    {
        return ret;
    }

    /* Read Chip ID register */
    ret = sns_ddf_read_port(port_handle,
                            regAddr[AMS_REG_REVID],
                            &revId[0],
                            2,
                            &read_count);
    if(ret != SNS_DDF_SUCCESS)
    {
        return ret;
    }

    chipId = revId[1];

    /* Check if data read is what is expected */
    if(!ams_validDeviceId(chipId))
    {
        return SNS_DDF_EINVALID_DATA;
    }


    // Found device
    *num_sensors = 5;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
    if( *sensors != NULL )
    {
        (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
        (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
        (*sensors)[2] = SNS_DDF_SENSOR_RGB;          // RGB
        (*sensors)[3] = SNS_DDF_SENSOR_CT_C;         // CT_C
        (*sensors)[4] = SNS_DDF_SENSOR_IR_GESTURE;   // IR_GESTURE
        ret = SNS_DDF_SUCCESS;
    }
    else
    {
        ret = SNS_DDF_ENOMEM;
    }

    sns_ddf_close_port(port_handle);
    return ret;
}
