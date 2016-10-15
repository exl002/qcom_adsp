#ifndef TILT_DETECTOR_H
#define TILT_DETECTOR_H

/*============================================================================
 @file tilt_detector.h

 tilt detection header file

 Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 ============================================================================*/

/*============================================================================
 EDIT HISTORY

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2014-06-03  tc   Added support for AMD input and various improvements
 2014-05-05  px   initial version

 ============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"
#include "sns_buffer.h"
#include "sns_sam_qmd_v01.h"

#define TILT_ACCEL_COLS                  (3)
#define TILT_SAMPLE_RATE                 (10.0)  //Hz
#define TILT_ACCEL_TIME_WINDOW_DEF       (2.0)   // seconds
#define TILT_INIT_ACCEL_TIME_WINDOW_DEF  (1.0)   // seconds
#define TILT_ANGLE_THRES_DEF             (35.0*PI/180.0)   // radians
#define TILT_GRAV_BUFF_LEN_DEF           (2.0)   // seconds

typedef struct
{
  float sample_rate;             // Hz 
  float angle_threshold;         // radians
  float init_accel_window_time;  // time window to average accel for initial grav (seconds)
  float accel_window_time;       // time window to average accel for current grav (seconds)
} tilt_config_s;

typedef enum
{
  TILT_INPUT_ACCEL = 0,
  TILT_INPUT_AMD   = 1
} tilt_input_type_e;

typedef struct
{
  union
  {
    int32_t accel[TILT_ACCEL_COLS];  // accel, m/s/s, Q16
    sns_sam_motion_state_e_v01 amd_state;  // state of AMD
  } input_data;
  uint32_t ts;                       // timestamp, in SSC ticks
  tilt_input_type_e input_type;      // ACCEL(0), AMD(1)
} tilt_input_s;

typedef struct
{
  uint32_t event_ts; //reported event timestamp, end of the observation period, SSC ticks
} tilt_output_s;

typedef struct
{
  sns_buffer_type accel_buf;          //accel and dot product buffers
  tilt_config_s config;
  float dprod_threshold;              //dot product threshold
  // todo: instead of using grav_init_set, considering checking to see if reference_grav is non-zero
  bool grav_init_set;                 //indicates if the initial gravity has been stored
  float reference_grav[TILT_ACCEL_COLS];   //initial gravity vector
  float cos_theta_sq; // cosf(config_data->angle_threshold)^2;

  uint32_t init_accel_window_size;    //average window size for initial gravity calculation
  uint32_t accel_window_size;         //average window size for current gravity calculation
//  float dotProdSq;                    //square of dot product of normalized vectors = cos(angle)^2, set to a large initial value

  sns_sam_motion_state_e_v01 motion_state;
} tilt_state_s;

int32_t tilt_sam_mem_req(tilt_config_s *config_data);

tilt_state_s* tilt_sam_state_reset(tilt_config_s *config_data, void *mem);

void tilt_sam_update(
    tilt_state_s *tilt_algo,
    tilt_input_s *input,
    tilt_output_s *output);

#endif /* TILT_DETECTOR_H */
