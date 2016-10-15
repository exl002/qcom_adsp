#ifndef ORIENTATION_H
#define ORIENTATION_H

/*=============================================================================
  Qualcomm Orientation header file 
 
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*============================================================================
 * EDIT HISTORY
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * when        who  what, where, why
 * ----------  ---  -----------------------------------------------------------
 * 2012-04-01  ks   Fixed mag vector freeze wait duration upon stationary
 * 2012-02-21  ks   Added registry support for orientation algo config
 * 2012-01-11  ad   add magnetic anomaly rejection and stationary detection support
 * 2011-12-09  ks   fixed comment length
 * 2011-12-09  jh   removed tabs and extraneous line-feeds
 * 2011-12-08  ks   added fixes for orientation filter initialization
 * 2011-11-30  ad   update the default report period
 * 2011-11-10  ks   initial version
 *
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"
#include "sns_buffer.h"

#define ORIENTATION_QUAT_COLS   (4)
#define ORIENTATION_VECTOR_COLS (3)

#define ORIENTATION_ANDROID_COORD_SYS 0 // Android (ENU = East North Up)
#define ORIENTATION_SAE_COORD_SYS 1     // SAE (NED = North East Down)

#define ORIENTATION_DEFAULT_COORD_SYS ORIENTATION_ANDROID_COORD_SYS

// Initial samples over which a simple averaging is applied to 
// prepopulate the filter registers. The prepopulation of the 
// filter with the initial values ensures that filter settles 
// quickly. The averaging eliminates some of the initial noise
#define ORIENTATION_FILTER_STARTUP_SAMPLES 4 

// square of min valid mag vec in Gauss^2
#define ORIENTATION_MIN_VALID_MAG_NORM_GAUSS_SQ (0.0225)
// square of max valid mag vec in Gauss^2
#define ORIENTATION_MAX_VALID_MAG_NORM_GAUSS_SQ (0.81)

// mag filter time constant in sec
#define ORIENTATION_MAG_FILTER_TC_SEC (0.5f) 

typedef enum
{
   ORIENTATION_ACC_DATA,
   ORIENTATION_MAG_DATA
} orientation_input_e;

typedef enum
{
   ORIENTATION_DEV_STATE_UNKNOWN,    // device state is unknown
   ORIENTATION_DEV_ABS_REST,         // device is in absolute rest
   ORIENTATION_DEV_REL_REST,         // device is in relative rest
   ORIENTATION_DEV_MOTION,           // device is moving
   ORIENTATION_DEV_STATES            // number of device states
} orientation_device_state_e;

typedef struct
{
   int32_t sample_rate;       // sample rate in Hz, Q16
   uint32_t accel_filter_tc;  // accel filter tc, msec
   uint32_t mag_filter_tc;    // mag filter tc, msec
   float motion_detect_period;// motion detect period, secs
   float abs_rest_var_thold;  // absolute rest variance threshold (m/s/s)2
   uint8_t coordinate_sys;    // coordinate system used in output, 0=SAE (default), 1=Android 
} orientation_config_s;

typedef struct
{
   union
   {
      int32_t acc_vector[ORIENTATION_VECTOR_COLS];     // accelerometer sample, Q16
      int32_t mag_vector[ORIENTATION_VECTOR_COLS];
   } data;
   orientation_input_e input_type;
   uint8_t accuracy;                   // accuracy metric
   uint32_t timestamp;
} orientation_input_s;

typedef struct
{
   float rotation_vector[ORIENTATION_QUAT_COLS];
   float gravity[ORIENTATION_VECTOR_COLS];   // gravity vector m/s2
   float lin_accel[ORIENTATION_VECTOR_COLS]; // linear acceleartion m/s2
   uint32_t timestamp;
   uint8_t accuracy;                   // accuracy metric
   uint8_t coordinate_sys;       //0=ENU(Android), 1=NED(SAE)
} orientation_output_s;

typedef struct
{
   orientation_config_s config;
   float filt_acc_vector[ORIENTATION_VECTOR_COLS];  // accelerometer sample, Q16
   float filt_mag_vector[ORIENTATION_VECTOR_COLS];
   float accel_filter_alpha;
   float mag_filter_alpha;
   uint32_t filt_acc_ts;
   uint32_t filt_mag_ts;
   uint32_t num_accel_samples; // sample count for accel filter register prepopulation
   uint32_t num_mag_samples; // sample count for mag filter register prepopulation
   sns_buffer_type acc_buf;                        // accelerometer samples
   double acc_buf_sum[ORIENTATION_VECTOR_COLS];    //sum of accelerometer samples
   double acc_buf_sum_sq[ORIENTATION_VECTOR_COLS]; //sum of square of accel samples
   orientation_device_state_e dev_state;           // device state
   uint32_t mag_samps_at_rest;    // mag samples since rest was detected
   bool acc_received; 
   bool mag_received;
} orientation_state_s;

/*=========================================================================
  FUNCTION:  orientation_sam_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed by the algorithm with given configuration

    @param[i] config_data: Pointer to algorithm configuration

    @return Size of state memory required, given the configuration settings
            0 if the configuration is invalid
*/
/*=======================================================================*/
int32_t orientation_sam_mem_req(
   const orientation_config_s *config_data);

/*=========================================================================
  FUNCTION:  orientation_sam_state_reset
  =======================================================================*/
/*!
    @brief Resets the algorithm instance with given configuration

    @param[i] config_data: Pointer to algorithm configuration
    @param[i] mem: Pointer to a block of allocated algorithm state memory

    @return Pointer to a valid algorithm state memory if successful
            NULL if error
*/
/*=======================================================================*/
orientation_state_s* orientation_sam_state_reset(
   const orientation_config_s *config_data,
   void *mem);

/*=========================================================================
  FUNCTION:  orientation_sam_update
  =======================================================================*/
/*!
    @brief Execute the algorithm, update the output with given input

    @param[i] algo: Pointer to algorithm state memory
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void orientation_sam_update(
   orientation_state_s *algo,
   const orientation_input_s *input,
   orientation_output_s *output);

#endif
