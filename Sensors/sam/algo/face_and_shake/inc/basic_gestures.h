/*=============================================================================
  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
=============================================================================*/

#ifndef BASIC_GESTURES_H
#define BASIC_GESTURES_H

#include <stdint.h>
#include "circular_buffer.h"

#define BASIC_TIME_WINDOW      (FX_FLTTOFIX_Q16(0.15))   // seconds, Q16
#define BASIC_SLEEP_DEF        (FX_FLTTOFIX_Q16(0.3))    // seconds, Q16
#define BASIC_PUSH_THRESH_DEF  (FX_FLTTOFIX_Q16(1.5*G))  // m/s/s, Q16
#define BASIC_PULL_THRESH_DEF  (FX_FLTTOFIX_Q16(1.5*G))  // m/s/s, Q16
#define BASIC_SHAKE_THRESH_DEF (FX_FLTTOFIX_Q16(1.5*G))  // m/s/s, Q16
#define BASIC_LOADED_Z_THRESH  (FX_FLTTOFIX_Q16(1.1  ))  // unitless, Q16
#define BASIC_SIGNAL_DELAY     (FX_FLTTOFIX_Q16(0.11))   // seconds, Q16
#define BASIC_HOME_THRESH      (FX_FLTTOFIX_Q16(0.15))   // unitless threshold, Q16
#define BASIC_HOME_WINDOW      (FX_FLTTOFIX_Q16(0.07))   // seconds, Q16
#define BASIC_REVERSAL_THRESH  (FX_FLTTOFIX_Q16(1.1))    // unitless threshold, Q16
#define BASIC_SHAKE_DIR        (FX_FLTTOFIX_Q16(1.0))    // unitless threshold, Q16
#define BASIC_REST_WINDOW      (FX_FLTTOFIX_Q16(0.1))    // seconds, Q16
#define BASIC_DATA_COLS        (3)



int32_t fns_basic_gestures_sam_mem_req(basic_gestures_config_struct *config_data);
basic_gestures_state_struct* fns_basic_gestures_sam_state_reset(basic_gestures_config_struct *config_data,
                                                            void *mem);
void fns_basic_gestures_sam_update(basic_gestures_state_struct *basic_algo,
                               basic_gestures_input_struct *input,
                               basic_gestures_output_struct *output);

#endif /* BASIC_GESTURES_H */
