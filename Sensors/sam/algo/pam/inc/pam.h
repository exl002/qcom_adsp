#ifndef PAM_H
#define PAM_H

/*=============================================================================
  Qualcomm Pedestrian Activity Monitor (PAM) header file 
 
  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
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
 * 2012-07-30  ad   initial version
 *
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>

/* Algorithm configuration parameters */
//Activity measurement period in seconds
#define PAM_DEF_MEASUREMENT_PERIOD_SECS (20)    // default 30 seconds
#define PAM_MAX_MEASUREMENT_PERIOD_SECS (3600)  // maximum 1 hour
#define PAM_MIN_MEASUREMENT_PERIOD_SECS (15)    // minimum 15 seconds

//Default step count threshold used to report change in activity
#define PAM_DEF_STEP_COUNT_THRESHOLD (2)

//Default sensor sampling rate in Hz
#define PAM_DEF_SAMPLE_RATE_HZ (20)

//Default duty cycle on percentage for sensor stream
#define PAM_DEF_DUTY_CYCLE_ON_PERCENTAGE (20)

/* Reported device states when the step count is 0 */
typedef enum 
{
   PAM_DEVICE_STATE_UNKNOWN = -2,      // initial state
   PAM_DEVICE_STATE_STATIONARY = -1,   // device in absolute stationary state
   PAM_DEVICE_STATE_MOTION = 0         // device in motion with 0 step count
} pam_device_state_e;

/* Algorithm input datatypes */
typedef enum 
{
   PAM_QMD_INPUT, // Qualcomm Motion Detector (QMD) input
   PAM_PED_INPUT  // Pedometer input
} pam_input_e;

/* PAM algorithm configuration structure */
typedef struct
{
  uint32_t sample_rate;          // sample rate, Hz, Q16
  uint16_t measurement_period;   // measurement period, seconds
  uint16_t step_count_threshold; // threshold in number of steps
                                 // if set to 0 report every measurement period
  uint8_t dutycycleOnPercent;    // duty cycle on percentage 
} pam_config_s;

/* PAM algorithm state structure */
typedef struct
{
  pam_config_s config;           // algorithm configuration
  pam_device_state_e dev_state;  // device state
  int32_t current_step_count;    // step count in current measurement period
  int32_t reported_step_count;   // last reported step count
  int32_t pedometer_last_step_count;   // last reported pedometer step count
  uint32_t pedometer_last_timestamp; // previous pedometer input timestamp
  bool report_decision; // 1: report to client 0: do not report to client
} pam_state_s;

/* PAM algorithm input structure */
typedef struct
{
  union
  {
    int32_t step_count;    // pedometer input, step count
    int32_t motion_state;  // QMD input, 1 - stationary, 2 motion
   } data;
  pam_input_e datatype; // type of input data
  uint32_t timestamp;   // data timestamp in clockticks
} pam_input_s;

/* PAM algorithm output structure */
typedef struct
{
  int32_t step_count;   // step count OR pam_device_state_e if step count is 0
  uint32_t timestamp;   // timestamp in clockticks of last sensor sample
                        // in the measurement period
} pam_output_s;

/* PAM algorithm API for SAM to query memory requirement */
int32_t pam_sam_mem_req(
   const pam_config_s* config_data  // algorithm configuration
);

/* PAM algorithm API for SAM to reset algorithm */
pam_state_s* pam_sam_state_reset(
   const pam_config_s* config_data, // algorithm configuration
   void* mem                        // memory allocated for algorithm state
);

/* PAM algorithm API for SAM to execute algorithm with new input */
void pam_sam_update(
   pam_state_s* pam_algo,     // algorithm state
   const pam_input_s* input,  // algorithm input
   pam_output_s* output       // algorithm output
);

#endif // PAM_H

