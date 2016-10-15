#ifndef TAP_H
#define TAP_H

/*=============================================================================
  Qualcomm Tap gesture header file

  Copyright (c) 2011 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
=============================================================================*/

/*============================================================================
 * EDIT HISTORY
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * when        who  what, where, why
 * ----------  ---  -----------------------------------------------------------
 * 2011-12-13  pd   initial version
 *
 *============================================================================*/

#include "fixed_point.h"
#include "comdef.h"

#define TAP_TIME_WINDOW     (FX_FLTTOFIX_Q16(0.30))  // time, seconds, Q16 // changed from 0.15
#define TAP_TIME_SLEEP      (FX_FLTTOFIX_Q16(0.5))   // time, seconds, Q16
#define TAP_THRESHOLD       (FX_FLTTOFIX_Q16(0.2*G)) // accel, m/s/s, Q16 // changed from 0.4*G
#define TAP_MIN_SAMPLE_RATE (FX_FLTTOFIX_Q16(200.0)) // sample rate, Hz, Q16 // changed from 100

#define TAP_DATA_COLS       (3)

enum TAP_STATES
{
  TAP_UNKNOWN     = 0,
  TAP_LEFT        = 1,
  TAP_RIGHT       = 2,
  TAP_TOP         = 3,
  TAP_BOTTOM      = 4,
  TAP_RESERVED_1  = 5,
  TAP_RESERVED_2  = 6
};

typedef struct
{
  int32_t sample_rate;    // sample rate, Hz, Q16
  int32_t tap_threshold;  // m/s/s, Q16
} tap_config_struct;

typedef struct
{
  int32_t a[TAP_DATA_COLS]; // accel, m/s/s, Q16
  int32_t motion_state;     // rest, motion, unknown
} tap_input_struct;

typedef struct
{
  int32_t tap_state;
  int32_t event;
} tap_output_struct;

typedef struct
{
  tap_config_struct config;
  int32_t accel_old[TAP_DATA_COLS];
  int32_t jerk_max;
  int32_t first_reading;
  int32_t readings_above_thr;
  int32_t samples_to_sleep;
  int32_t loaded_axis;
  int32_t tap_state;
  int32_t tap_window_size;
  int32_t tap_sleep_size;
} tap_state_struct;

int32_t tap_sam_mem_req(tap_config_struct *config_data);
tap_state_struct* tap_sam_state_reset(tap_config_struct *config_data, void *mem);
void tap_sam_update(tap_state_struct *tap_algo,
                    tap_input_struct *input,
                    tap_output_struct *output);

#endif /* TAP_H */
