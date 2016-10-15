#ifndef FACING_H
#define FACING_H

/*=============================================================================
  Qualcomm Facing gesture header file

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
 * 2013-10-29  pk   Removed event flag from output structure
 * 2011-12-13  pd   initial version
 *
 *============================================================================*/

#include "circular_buffer.h"
#include "comdef.h"
#include "rel_rest.h"

#define FACING_SAMPLE_RATE_DEF_HZ   (30)
#define FACING_SAMPLE_RATE_DEF_Q16  (FX_CONV_Q16(30, 0))
#define FACING_TIME_WINDOW   (FX_FLTTOFIX_Q16(0.3))           // seconds, Q16 // 0.3
#define FACING_THRESHOLD_DEF (FX_FLTTOFIX_Q16(20.0*PI/180.0)) // angle in radians, Q16
#define FACING_REPORT_NEUTRAL (0)
#define FACING_DATA_COLS     (3)
#define FACING_REL_REST_ACCEL_TIME_WINDOW (FX_FLTTOFIX_Q16(0.1))           // seconds
#define FACING_REL_REST_ANGLE_THRESH      (FX_FLTTOFIX_Q16(5.0*PI/180.0)) // radians, Q16
#define FACING_REL_REST_DETECT_THRESH_SPI (FX_FLTTOFIX_Q16(0.5*G))         // m/s/s, Q16

typedef enum
{
  FACING_UNKNOWN = 0,
  FACING_UP      = 1,
  FACING_DOWN    = 2,
  FACING_NEUTRAL = 3
} facing_state_e;

typedef struct
{
  int32_t sample_rate;            // sample rate in Hz, Q16
  int32_t facing_angle_threshold; // angle in radians, Q16
  int32_t report_neutral;         // flag to indicate whether Neutral state to be reported
} facing_config_struct;

typedef struct
{
  int32_t a[FACING_DATA_COLS];    // accel, m/s/s, Q16
  int32_t motion_state;           // rest, motion, unknown
} facing_input_struct;

typedef struct
{
  facing_state_e facing_state;
} facing_output_struct;

typedef struct
{
  facing_config_struct config;
  rel_rest_state_s * rel_rest_algo;
  buffer_type buf;
  int32_t buf_sum[FACING_DATA_COLS];
  int32_t facing_thresh_sum;
  facing_state_e facing_state;
  int32_t report_neutral;
} facing_state_struct;

int32_t facing_sam_mem_req(facing_config_struct *config_data);
facing_state_struct* facing_sam_state_reset(facing_config_struct *config_data, void *mem);
void facing_sam_update(facing_state_struct *facing_algo,
                       facing_input_struct *input,
                       facing_output_struct *output);

#endif /* FACING_H */
