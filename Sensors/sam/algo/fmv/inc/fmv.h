#ifndef _FILTERED_MAGNETIC_VECTOR_H
#define _FILTERED_MAGNETIC_VECTOR_H

/*=============================================================================
  Filtered Magnetic Vector Interface Definition
 
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*============================================================================
 * EDIT HISTORY
 *
 * This secticon contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * when        who  what, where, why
 * ----------  ---  -----------------------------------------------------------
 * 2014-05-12  ad   Add support for SMGR specified max mag sample rate
 * 2012-02-21  ks   Added registry support for fmv motion detection algo config
 * 2012-01-09  ad   add magnetic anomaly rejection and stationary detection support
 * 2011-12-01  ad   update the default calibrated mag rate
 * 2011-10-26  gju  Increase max_mag_outage[] size to FMV_MAX_ACCURACY_LEVELS
 * 2011-08-24  asj  initial version
 *
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"
#include "sns_buffer.h"
#include "sns_sam_priv.h"

#define FMV_MAX_ACCURACY_LEVELS 4 // number of accuracy bins supported

#define FMV_ABS_REST_SUMSQ_THOLD (0.002)  // absolute rest sum square threshold in (rad/s)^2
                                          // 2.56dps gyro offset error (in absolute rest)
#define FMV_MIN_VALID_MAG_NORM_GAUSS_SQ (0.0225) // square of min valid mag vec in Gauss^2
#define FMV_MAX_VALID_MAG_NORM_GAUSS_SQ (0.81)   // square of max valid mag vec in Gauss^2

typedef enum
{
   FMV_DEV_STATE_UNKNOWN,    // device state is unknown
   FMV_DEV_ABS_REST,         // device is in absolute rest
   FMV_DEV_REL_REST,         // device is in relative rest
   FMV_DEV_MOTION,           // device is moving
   FMV_DEV_STATES            // number of device states
} fmv_device_state_e;

//        Configuration structure for FMV 
typedef struct
{
  int32_t report_period;          // report periodz, Q16
  int32_t sample_rate;            // sample rate in Hz, Q16
  int32_t mag_max_sample_rate;    // mag max sample rate in Hz
  uint32_t fmv_tcs[FMV_MAX_ACCURACY_LEVELS];// time constants
                                          //for each accuracy level (ms)
  uint32_t max_gyro_sample_gap_tolerance;  // reset filter if quat/gyro time diff 
                                          // exceeds this threshold(ms)
  float  max_mag_sample_gap_factor;      // reset filter if mag time diff 
                                          // exceeds this threshold * tc(ms)
  float  min_turn_rate;                  // assume Zero turn if sample is below 
                                          // this number (rad/s)
  float max_mag_innovation;              // clamp norm(innov) to this (Gauss)
  float motion_detect_period; // motion detect period in seconds
  float abs_rest_var_thold;   // absolute rest variance threshold in (rad/s)^2
  float rel_rest_var_thold;   // relative rest variance threshold in (rad/s)^2
  uint8_t mag_cal_lat_num_samps; // mag calibration latency in sample number
} fmv_config_s;


// Type of update being poked into the algo 
typedef enum 
{
  FMV_CALIB_MAG_UPDATE,      // Updating algo with calibrated magnetometer data
  FMV_QUAT_UPDATE,           // Updating algo with quaternion data 
  FMV_GYRO_UPDATE            //  Updating algo with Gyro Data 
} fmv_update_e;              // Type of update

// FMV input data structure 
typedef struct
{
  uint32_t      ts;     // timestamp of input - milliseconds
  fmv_update_e  type;   // type of input
  union
  {
    float  calib_mag[3]; // calibrated magnetometer data
    float  q[4];         // quaternion data
    float  gyro[3];      // gyro data Q16 format
  } data;
  uint8_t accuracy;     // Keeping option open to allow accuracy for all data
                        // Currently, though, accuracy is only provided by 
                        // the calib_mag algo
} fmv_input_s;

// FMV output data structure 
typedef struct
{
  uint32_t ts;            // timestamp of applicability of output
  float  filtered_mag[3]; // filtered magnetic vector
  uint8_t accuracy;       // estimate of accuracy of the fmv 
} fmv_output_s;


// Algo state 
typedef struct
{
  fmv_config_s config;                 // FMV configuration

  float alpha[FMV_MAX_ACCURACY_LEVELS];  // filter constants by accuracy level

  float fmv[3];                      // Filtered magnetic vector as of now
  uint32_t fmv_ts;                    // time stamp of FMV estimate 
  bool fmv_valid;                     // is our fmv estimate valid
  uint8_t fmv_accuracy;               // our accuracy estimate of the FMV 

  uint32_t gyro_ts;                   // time stamp of latched gyro sample
  bool gyro_ts_valid;                 // Is the Gyro ts valid
  float gyro[3];                     // Last latched gyro sample

  float gyro_period;                 // time interval between gyro samples
  bool gyro_period_valid;             // Is our estimate of gyro period valid

  uint32_t mag_ts;                    // time stamp of last received mag sample
  bool     mag_ts_valid;              // is mag_ts valid 

  uint32_t quat_ts;                   // time stamp of last quaternion update
  bool quat_ts_valid;                 // is quat_is valid
  float quat[4];                      // last quaternion input received

  uint32_t mag_samples_since_reset;   // num mag samples received since we reset

  uint32_t max_gyro_outage;           // declare outage of two gyro/quat samples
                                      // are farther apart than this threshold
  uint32_t max_mag_outage[FMV_MAX_ACCURACY_LEVELS];
                                      // declare outage if two mag samples are 
                                      // farther apart than this threshold
  float cos_min_turn_rate;            // derived from minimum turn rate in config

  sns_buffer_type gyro_buf;           // gyro samples
  double gyro_buf_sum[3];             //sum of gyro samples
  double gyro_buf_sum_sq[3];          //sum of square of gyro samples
  float abs_rest_var_thold;   // absolute rest variance threshold in (rad/s)^2
  float rel_rest_var_thold;   // relative rest variance threshold in (rad/s)^2
  fmv_device_state_e dev_state;       // device state

  uint32_t mag_samples_since_dev_abs_rest; // num mag samples received since
                                           // we detected device at abs reset

  uint32_t mag_samples_for_mar;   // num of mag samples that must be
                                  // received while device is at abs
                                  // rest for MAR, magnetic anomaly
                                  // rejection, to occur
} fmv_state_struct_t;

/*=========================================================================
  FUNCTION:  fmv_sam_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed by the algorithm with given configuration

    @param[i] config_data: Pointer to algorithm configuration

    @return Size of state memory required, given the configuration settings
            0 if the configuration is invalid
*/
/*=======================================================================*/
int32_t fmv_sam_mem_req(const fmv_config_s *config_data);

/*=========================================================================
  FUNCTION:  fmv_sam_state_reset
  =======================================================================*/
/*!
    @brief Resets the algorithm instance with given configuration

    @param[i] config_data: Pointer to algorithm configuration
    @param[i] mem: Pointer to a block of allocated algorithm state memory

    @return Pointer to a valid algorithm state memory if successful
            NULL if error
*/
/*=======================================================================*/
fmv_state_struct_t *fmv_sam_state_reset(const fmv_config_s *config_data, void *mem);

/*=========================================================================
  FUNCTION:  fmv_sam_update
  =======================================================================*/
/*!
    @brief Execute the algorithm, update the output with given input

    @param[i] algo_state: Pointer to algorithm state memory
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void fmv_sam_update(fmv_state_struct_t *algo_state,
                    const fmv_input_s *input,
                    fmv_output_s *output);

#endif // End include guard
