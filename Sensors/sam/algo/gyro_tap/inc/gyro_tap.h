/*=============================================================================
  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
=============================================================================*/

#ifndef _GYRO_TAP_H
#define _GYRO_TAP_H

#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"

// We only support one rate
////////////////////////////////////////////////////////////////////////////////
#define GTAP_SAMPLE_RATE                    (200.0)
#define GTAP_MIN_SAMPLE_RATE_Q16            (FX_FLTTOFIX_Q16(GTAP_SAMPLE_RATE))
#define GTAP_SAMPLE_RATE_Q16                GTAP_MIN_SAMPLE_RATE_Q16
#define GTAP_TIME_WIN_SECS                  (FX_FLTTOFIX_Q16(0.25))
#define GTAP_TIME_SLEEP_SECS                (FX_FLTTOFIX_Q16(0.5))
#define GTAP_ORI_CHECK_WIN_SECS             (FX_FLTTOFIX_Q16(0.25))
#define GTAP_ORI_CHNG_INT_SECS              (FX_FLTTOFIX_Q16(1.00))
#define GTAP_ORI_CHNG_THR                   (FX_FLTTOFIX_Q16(0.35))// 20 degrees
#define GTAP_JERK_WIN_SECS                  (FX_FLTTOFIX_Q16(0.125))
#define GTAP_ACCEL_THRESHOLD                (FX_FLTTOFIX_Q16(1.962))
#define GTAP_LR_ACCEL_JERK_MIN_THRESH       (FX_FLTTOFIX_Q16(2.0))
#define GTAP_LR_ACCEL_RAT_JERK_YX           (FX_FLTTOFIX_Q16(3.0))
#define GTAP_TB_ACCEL_JERK_MIN_THRESH       (FX_FLTTOFIX_Q16(3.0))
#define GTAP_TB_ACCEL_RAT_JERK_XY           (FX_FLTTOFIX_Q16(1.0))
#define GTAP_Z_ANAMOLY_INC                  1
#define GTAP_STRONG_Z_ANAMOLY_TB            1
#define GTAP_TB_ACCEL_Z_THRESH              (FX_FLTTOFIX_Q16(3.0))
#define GTAP_STRESS_TOP_BOTTOM              1
#define GTAP_STRESS_RIGHT_LEFT              0
////////////////////////////////////////////////////////////////////////////////

#define GTAP_ACCEL_ELEMENTS_IN_BUFFER       50
#define GTAP_GYRO_ELEMENTS_IN_BUFFER        50
#define GTAP_ORIENTATION_ELEMENTS_IN_BUFFER 250
#define GTAP_ORIENTATION_CHECK_ELEMENTS_IN_BUFFER 50

#define GYRO_TAP_FLAGS_STRESS_RL                0x1
#define GYRO_TAP_FLAGS_STRESS_TB                0x2
#define GYRO_TAP_FLAGS_STRONG_Z_ANAMOLY_TB      0x4
#define GYRO_TAP_FLAGS_Z_ANAMOLY_INC            0x8

// Tap states enumerated
enum GTAP_STATES
{
  GTAP_UNKNOWN = 0,
  GTAP_LEFT   = 1,
  GTAP_RIGHT  = 2,
  GTAP_TOP    = 3,
  GTAP_BOTTOM = 4,
  GTAP_FRONT  = 5,
  GTAP_BACK   = 6
};

typedef struct
{
  int32_t scenario;
  int32_t tap_time_win_secs;
  int32_t tap_time_sleep_secs;
  int32_t acc_tap_thr;
  int32_t lr_accel_jerk_min_thr;
  int32_t tb_accel_jerk_min_thr;
  int32_t jerk_win_secs;
  int32_t lr_accel_rat_jerk_yx;
  int32_t tb_accel_rat_jerk_xy;
  int32_t accel_z_thr;
  int32_t ori_check_win_secs;
  int32_t ori_check_int_secs;
  int32_t ori_check_thr;
  int32_t z_anamoly_inc;
  int32_t strong_z_anamoly_tb;
  int32_t stress_right_left;
  int32_t stress_top_bottom;
}gtap_config_struct;

typedef enum
{
   GTAP_ACCEL_INPUT,
   GTAP_GYRO_INPUT
} gtap_input_e;

typedef struct
{
   int32_t a[3]; // accel, m/s/s, Q16
   int32_t g[3]; // gyro,  rad/s
} gtap_input_s;

typedef struct
{
  gtap_input_s data;
  gtap_input_e datatype; // type of input data
} gtap_input_struct;

typedef struct
{
  int32_t tap_state;
  int32_t event;
} gtap_output_struct;

typedef struct
{
  int32_t *data;
  int32_t *data_sum;
  uint32_t count;     // how many elements are in the buffer
  uint32_t index;     // where is the next data element written
  uint32_t size;      // size of the buffer, num samples
  uint32_t num_cols;  // number of cols per sample
} gtap_buff_t;

typedef struct
{
  gtap_config_struct cfg;       // config that was passed to the algo


  // The following are constants
  int32_t sample_rate;          // 200 Hz
  int32_t acc_dir_tap_thr;
  int32_t turn_ori_rejection_on;// ON
  int32_t tb_gyro_rat_jerk_yx;  // 3
  int32_t tb_gyro_rat_jerk_yz;  // 1.5
  int32_t lr_gyro_rat_jerk_zy;  // 2.5
  int32_t lr_gyro_rat_jerk_zx;  // 2.5
  int32_t lr_gyro_jerk_min_thr;  // 5
  int32_t tb_gyro_jerk_min_thr;  // 5
  int32_t lr_accel_jerk_min_thr_min; // 1
  int32_t lr_gyro_jerk_min_thr_min;  // 10
  int32_t lr_gyro_jerk_min_thr_max;  // 15
  int32_t tap_dir_win_secs;               // 0.05 seconds

  // The following are derived from the passed in configuration
  // These parameters are computed only once
  int32_t tap_time_samples;     // tap time in samples:w
  int32_t samples_to_sleep;     // how long should we continue to sleep
  int32_t tap_dir_samples;      // Tap direction samples
  uint32_t ori_check_win;       // in samples, we want to see if there was no change for this duration
  uint32_t ori_check_int;       // in samples
  int32_t ori_change_thresh;    // Take sampling rate into account for efficiency

  int32_t tb_accel_jerk_min_thr_min;
  int32_t accel_strong_tb_z_thr;
  int32_t tb_accel_rat_jerk_xz;
  int32_t lr_accel_rat_jerk_yz;
  int32_t tb_accel_rat_jerk_zx;
  int32_t tb_accel_rat_jerk_zy;
  int32_t acc_jerk_mild_thr;
  int32_t acc_max_tap_thr;

  // Everything else is dynamic
  int32_t accel_old[3];         // last accel sample seen
  int32_t gyro_old[3];          // last gyro sample seen
  int32_t jerk_max;             // max jerk value in this decision
  int32_t first_reading;        // True if we are waiting on first sample
  int32_t readings_above_thr;   // #of readings that exceed threshold this cycle
  int32_t tap_internal_counter; // wait counter
  int32_t loaded_axis;          // which axis is the tap on
  int32_t tap_state;            // state of the algo

  gtap_buff_t accel_data_buff;          // past accel data
  gtap_buff_t gyro_data_buff;           // past gyro data
  gtap_buff_t orientation_buff;         // past orientation data
  gtap_buff_t orientation_check_buff;   // past orientation checks

  int32_t  tap_axis_t_b;        // potential tap axis top/bottom flag
  int32_t  tap_axis_r_l;        // potential tap axis right/left flag

  int32_t  max_tap_accel_jerk;

  // Useful for deugging etc.
  uint32_t update_count;        // number of times update has been called
  uint8_t  direction_determined;// have we determined the direction of the tap


  int32_t accel_data_hist[3 * GTAP_ACCEL_ELEMENTS_IN_BUFFER];
  int32_t gyro_data_hist[3 * GTAP_GYRO_ELEMENTS_IN_BUFFER];
  int32_t ori_data_hist[3 * GTAP_ORIENTATION_ELEMENTS_IN_BUFFER];
  int32_t ori_data_sum[3];
  uint32_t samples_since_last_ori_trip;
} gtap_state_struct;

// Mandatory interfaces exposed to SAM
int32_t gtap_sam_mem_req(const gtap_config_struct *config_data);
gtap_state_struct* gtap_sam_state_reset(const gtap_config_struct *config_data,
                                        void *mem);
void gtap_sam_update(gtap_state_struct *algo,
                     const gtap_input_struct *input,
                     gtap_output_struct *output);

#endif /* _GYRO_TAP_H */
