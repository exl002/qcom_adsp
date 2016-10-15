#ifndef FACE_N_SHAKE_H
#define FACE_N_SHAKE_H

/*============================================================================
  @file face_and_shake.h

  Face and shake gesture recognition header file 

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/sam/algo/face_and_shake/inc/face_n_shake.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-03-21  ad   add default values for internal config parameters  
  2011-02-04  aj   initial version  

============================================================================*/

#include <stdint.h>
#include "shake_gestures.h"
#include "basic_gestures.h"
#include "axis_direction.h"
#include "rel_rest.h"

#define FACE_N_SHAKE_SAMPLE_RATE_DEF_HZ         (30) 
#define FACE_N_SHAKE_SAMPLE_RATE_DEF_Q16        (FX_CONV_Q16(30, 0)) 
#define FACE_N_SHAKE_SHAKE_THRESHOLD_DEF        (FX_FLTTOFIX_Q16(3.5*G))        // m/s/s, Q16
#define FACE_N_SHAKE_TIMEOUT                    (FX_FLTTOFIX_Q16(5.0))           // seconds, Q16
#define FACE_N_SHAKE_FACING_THRESH_DEF          (FX_FLTTOFIX_Q16(65.0*PI/180.0)) // radians, Q16
#define FACE_N_SHAKE_SLEEP_DEF                  (FX_FLTTOFIX_Q16(0.15))          // seconds, Q16
#define FACE_N_SHAKE_DATA_COLS                  (3)

#define FACE_N_SHAKE_DEF_INT_CFG_PARAM1         (FX_FLTTOFIX_Q16(0.5*G))
#define FACE_N_SHAKE_DEF_INT_CFG_PARAM2         (FX_FLTTOFIX_Q16(0.1))
#define FACE_N_SHAKE_DEF_INT_CFG_PARAM3         (FX_FLTTOFIX_Q16(15.0*PI/180.0))
#define FACE_N_SHAKE_DEF_INT_CFG_PARAM4         (FX_FLTTOFIX_Q16(0.1))

typedef struct
{
  int32_t sample_rate;                 // sample rate in Hz, Q16
  int32_t facing_angle_threshold;      // angle in radians, Q16
  int32_t basic_sleep;                 // after gesture is generated, sleep for this long, seconds, Q16
  int32_t basic_shake_threshold;       // m/s/s, Q16
  int32_t timeout;                     // seconds, Q16
  int32_t internal_config_param1;
  int32_t internal_config_param2;
  int32_t internal_config_param3;
  int32_t internal_config_param4;
} face_n_shake_config_s;

typedef struct
{
  int32_t a[FACE_N_SHAKE_DATA_COLS];    // accel, m/s/s, Q16
  int32_t motion_state;
} face_n_shake_input_s;

typedef struct
{
  int32_t face_n_shake_event; // event type
  int32_t event;              // boolean, true if new event detected
} face_n_shake_output_s;

typedef struct
{
  face_n_shake_config_s config;
  basic_gestures_state_struct *basic_gestures_algo;
  axis_state_s *axis_algo;
  rel_rest_state_s  *rel_rest_algo;
  int32_t face_n_shake_state;
  int32_t timeout;
  int32_t timeout_count;
  int32_t begin_countdown;
} face_n_shake_state_s;

int32_t face_n_shake_sam_mem_req(face_n_shake_config_s *config_data);
face_n_shake_state_s* face_n_shake_sam_state_reset(face_n_shake_config_s *config_data, void *mem);
void face_n_shake_sam_update(face_n_shake_state_s *face_n_shake_algo, 
                             face_n_shake_input_s *input,
                             face_n_shake_output_s *output);

#endif /* FACE_N_SHAKE_H */
