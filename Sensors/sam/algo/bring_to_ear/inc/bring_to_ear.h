#ifndef BRING_TO_EAR_H
#define BRING_TO_EAR_H

/*============================================================================
  @file bring_to_ear.h

  Bring to ear gesture recognition header file 

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/sam/algo/bring_to_ear/inc/bring_to_ear.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
 * EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * when        who  what, where, why
 * ----------  ---  -----------------------------------------------------------
 * 2011-03-21  ad   define default values for internal config params
 * 2011-02-25  sc   initial version
 *
 *============================================================================*/

#include "axis_direction.h"
#include "rel_rest.h"
#include "comdef.h"

#define BRING_TO_EAR_SAMPLE_RATE_DEF_HZ         (30)
#define BRING_TO_EAR_SAMPLE_RATE_DEF_Q16        (FX_CONV_Q16(30, 0))
#define BRING_TO_EAR_HORIZ_ANG                  (FX_FLTTOFIX_Q16(20.0*PI/180.0)) // radians, Q16
#define BRING_TO_EAR_VERT_ANG                   (FX_FLTTOFIX_Q16(20.0*PI/180.0)) // radians, Q16
#define BRING_TO_EAR_FACING_ANGLE_THRESH        (FX_FLTTOFIX_Q16(65.0*PI/180.0)) // radians, Q16
#define BRING_TO_EAR_DATA_COLS                  (3)

#define BRING_TO_EAR_DEF_INT_CFG_PARAM1         (FX_FLTTOFIX_Q16(0.3*G))
#define BRING_TO_EAR_DEF_INT_CFG_PARAM2         (FX_FLTTOFIX_Q16(0.2))
#define BRING_TO_EAR_DEF_INT_CFG_PARAM3         (FX_FLTTOFIX_Q16(15.0*PI/180.0))
#define BRING_TO_EAR_DEF_INT_CFG_PARAM4         (FX_FLTTOFIX_Q16(0.2))

#define BRING_TO_EAR_NEAR                       (1)
#define BRING_TO_EAR_FAR                        (0)

enum BRING_TO_EAR_EVENTS
{
  BRING_TO_EAR_EVENT = 1
};

typedef struct
{
  int32_t sample_rate;                // sample rate in Hz, Q16
  int32_t facing_angle_threshold;     // radians, Q16
  int32_t horiz_angle_threshold;      // radians, Q16
  int32_t vert_angle_threshold;       // radians, Q16
  int32_t prox_enabled;               // boolean, 1=TRUE, 0=FALSE
  int32_t internal_config_param1;
  int32_t internal_config_param2;
  int32_t internal_config_param3;
  int32_t internal_config_param4;
} bring_to_ear_config_s;

typedef struct
{
  int32_t a[BRING_TO_EAR_DATA_COLS]; // accel, m/s/s, Q16
  int32_t proximity;
  int32_t unused[2];   // to give enough memory to copy sensor data item  
} bring_to_ear_input_s;

typedef struct
{
  int32_t bring_to_ear_event;   // event type
  int32_t event;                // boolean, true if new event detected
} bring_to_ear_output_s;

typedef struct
{
  bring_to_ear_config_s config;  
  int32_t bring_to_ear_state;
  axis_state_s *axis_algo;
  rel_rest_state_s *rel_rest_algo;
  int32_t horiz_angle_threshold;
  int32_t vert_angle_threshold;
  int32_t prox_enabled;
} bring_to_ear_state_s;

int32_t bring_to_ear_sam_mem_req(bring_to_ear_config_s *config_data);
bring_to_ear_state_s* bring_to_ear_sam_state_reset(bring_to_ear_config_s *config_data, void *mem);
void bring_to_ear_sam_update(bring_to_ear_state_s *bring_to_ear_algo, 
                             bring_to_ear_input_s *input,
                             bring_to_ear_output_s *output);

#endif /* BRING_TO_EAR_H */

