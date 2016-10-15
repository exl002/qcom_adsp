#ifndef GRAVITY_H
#define GRAVITY_H

/*=============================================================================
  Qualcomm gravity vector / linear acceleration detector header file 
 
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 * 2012-01-11  ad   add accel based stationary detection
 * 2011-08-25  ad   initial version for qfusion
 *
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"
#include "sns_buffer.h"

#define GRAVITY_NUM_ELEMS      (3)
#define GRAVITY_NUM_QUAT_ELEMS (4)

#define GRAVITY_ACCEL_OUTAGE_PERIOD (500)    // milliseconds
#define GRAVITY_QUAT_OUTAGE_PERIOD  (500)    // milliseconds

#define GRAVITY_ACCURACY_UNRELIABLE (0)
#define GRAVITY_ACCURACY_LOW        (1)
#define GRAVITY_ACCURACY_MEDIUM     (2)
#define GRAVITY_ACCURACY_HIGH       (3)

typedef enum
{
   GRAVITY_ACCEL_INPUT,
   GRAVITY_QUAT_INPUT
} gravity_input_e;

typedef enum
{
   GRAVITY_DEV_STATE_UNKNOWN,    // device state is unknown
   GRAVITY_DEV_ABS_REST,         // device is in absolute rest
   GRAVITY_DEV_REL_REST,         // device is in relative rest
   GRAVITY_DEV_MOTION,           // device is moving
   GRAVITY_DEV_STATES            // number of device states
} gravity_device_state_e;

/** Gravity configuration structure */
typedef struct
{
   int32_t sample_rate;          // sample rate in Hz, Q16
   uint32_t filter_tc[GRAVITY_DEV_STATES]; // filter time constants in millisec
   uint32_t quat_outage_period;   // quaternion input outage period in msec
   uint32_t accel_outage_period;  // accelerometer input outage period in msec
   float motion_detect_period;    // motion detect period in seconds
   float abs_rest_var_thold;      // absolute rest variance threshold (m/s/s)2
   float rel_rest_var_thold;      // relative rest variance threshold (m/s/s)2
} gravity_config_s;

/** Gravity input structure */
typedef struct
{
   union
   {
      int32_t accel[GRAVITY_NUM_ELEMS];    // accelerometer sample, Q16
      float quat[GRAVITY_NUM_QUAT_ELEMS];  // gyro based quaternion input
   } data;
   gravity_input_e datatype;               // type of input data
   uint32_t timestamp;                     // timestamp
} gravity_input_s;

/** Gravity output structure */
typedef struct
{
   float gravity[GRAVITY_NUM_ELEMS];   // gravity vector
   float lin_accel[GRAVITY_NUM_ELEMS]; // linear acceleartion
   uint8_t accuracy;                   // accuracy metric
   uint32_t timestamp;     // common timestamp for gravity and linear accel
   gravity_device_state_e dev_state;   // device state
} gravity_output_s;

/** Gravity state structure */
typedef struct
{
   gravity_config_s config;
   float quat[GRAVITY_NUM_QUAT_ELEMS];        // input quaternion
   float accel[GRAVITY_NUM_ELEMS];            // input accel
   float gravity[GRAVITY_NUM_ELEMS];          // gravity
   float filter_alpha[GRAVITY_DEV_STATES];    // gravity filter constant
   uint32_t quat_samp_per;     // instantaneous quaternion sample period
   uint32_t accel_samp_per;    // instantaneous accelerometer sample period
   uint32_t quat_ts;           // input quaternion timestamp
   uint32_t accel_ts;          // input accelerometer timestamp
   uint32_t num_accel_samples; // sample count for initial alpha
   sns_buffer_type acc_buf;                  // accelerometer samples
   double acc_buf_sum[GRAVITY_NUM_ELEMS];    //sum of accelerometer samples
   double acc_buf_sum_sq[GRAVITY_NUM_ELEMS]; //sum of square of accel samples
   float abs_rest_var_thold;      // absolute rest variance threshold (m/s/s)2
   float rel_rest_var_thold;      // relative rest variance threshold (m/s/s)2
   gravity_device_state_e dev_state;         // device state
} gravity_state_s;

/*=========================================================================
  FUNCTION:  gravity_sam_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed by the algorithm with given configuration

    @param[i] config_data: Pointer to algorithm configuration

    @return Size of state memory required, given the configuration settings
            0 if the configuration is invalid
*/
/*=======================================================================*/
int32_t gravity_sam_mem_req(
   const gravity_config_s *config_data);

/*=========================================================================
  FUNCTION:  gravity_sam_state_reset
  =======================================================================*/
/*!
    @brief Resets the algorithm instance with given configuration

    @param[i] config_data: Pointer to algorithm configuration
    @param[i] mem: Pointer to a block of allocated algorithm state memory

    @return Pointer to a valid algorithm state memory if successful
            NULL if error
*/
/*=======================================================================*/
gravity_state_s* gravity_sam_state_reset(
   const gravity_config_s *config_data, 
   void *mem);

/*=========================================================================
  FUNCTION:  gravity_sam_update
  =======================================================================*/
/*!
    @brief Execute the algorithm, update the output with given input

    @param[i] gravity_algo: Pointer to algorithm state memory
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void gravity_sam_update(
   gravity_state_s *gravity_algo, 
   const gravity_input_s *input,
   gravity_output_s *output);

#endif
