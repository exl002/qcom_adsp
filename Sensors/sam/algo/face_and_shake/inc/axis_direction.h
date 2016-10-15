/*=============================================================================
  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
=============================================================================*/

#ifndef AXIS_DIRECTION_H
#define AXIS_DIRECTION_H

#include <stdint.h>
#include "circular_buffer.h"
#include "fx_iir.h"

#define AXIS_X_THRESH_UP_DEF   (FX_FLTTOFIX_Q16(66.0*PI/180.0)) // angle in radians, Q16
#define AXIS_Y_THRESH_UP_DEF   (FX_FLTTOFIX_Q16(66.0*PI/180.0)) // angle in radians, Q16
#define AXIS_Z_THRESH_UP_DEF   (FX_FLTTOFIX_Q16(66.0*PI/180.0)) // angle in radians, Q16
#define AXIS_X_THRESH_DOWN_DEF (FX_FLTTOFIX_Q16(66.0*PI/180.0)) // angle in radians, Q16
#define AXIS_Y_THRESH_DOWN_DEF (FX_FLTTOFIX_Q16(66.0*PI/180.0)) // angle in radians, Q16
#define AXIS_Z_THRESH_DOWN_DEF (FX_FLTTOFIX_Q16(66.0*PI/180.0)) // angle in radians, Q16
#define AXIS_ACCEL_COLS        (3)

enum AXIS_STATES
{
  AXIS_DIR_UNKNOWN = 0,
  AXIS_DIR_UP      = 1,
  AXIS_DIR_DOWN    = 2,
  AXIS_DIR_NEUTRAL = 3
};

typedef struct
{
  int32_t sample_rate;                             // sample rate in Hz, Q16
  int32_t axis_angle_thresh_up[AXIS_ACCEL_COLS];   // angle in radians, Q16
  int32_t axis_angle_thresh_down[AXIS_ACCEL_COLS]; // angle in radians, Q16
} axis_config_s;

typedef struct
{
  int32_t a[AXIS_ACCEL_COLS];     // accel, m/s/s, Q16
  int32_t motion_state;           // output of QMD or rel rest detector
} axis_input_s;

typedef struct
{
  int32_t axis_state[AXIS_ACCEL_COLS];
  int32_t event;
} axis_output_s;

typedef struct
{
  axis_config_s config;
  fx_iir_state_s  *fx_iir_lpf;
  int32_t axis_thresh_up[AXIS_ACCEL_COLS];
  int32_t axis_thresh_down[AXIS_ACCEL_COLS];
  int32_t axis_state[AXIS_ACCEL_COLS];
} axis_state_s;

int32_t axis_sam_mem_req(axis_config_s *config_data);
axis_state_s* axis_sam_state_reset(axis_config_s *config_data, void *mem);
void axis_sam_update(axis_state_s *axis_algo, 
                     axis_input_s *input,
                     axis_output_s *output);

#endif /* AXIS_DIRECTION_H */
