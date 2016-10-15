#ifndef _PEDOMETER_H
#define _PEDOMETER_H

/*============================================================================
  @file pedometer.h

  Pedometer header file 

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-08-12  pk   Initial version

============================================================================*/
/**----------------------------------------------------------------------------
 * Include Files
 * --------------------------------------------------------------------------*/
#include <stdint.h>
#include "sns_buffer.h"
#include "circular_buffer.h"
#include "sns_sam_priv.h"

/**----------------------------------------------------------------------------
 * Macro Declarations
 * --------------------------------------------------------------------------*/
#define PED_ACCEL_COLS    3 // No of accelerometer axes
#define PED_MAX_CLIENTS   SNS_SAM_MAX_CLIENT_REQS_PER_ALGO_INST // Max no of clients supported

#define PED_SAMPLE_RATE_DEF_Q16       FX_CONV_Q16(20, 0) // Default sample rate
#define PED_STEP_COUNT_THRESHOLD_DEF  0 // Default step count threshold

#define PED_MAX_REPORT_PERIOD_Q16     FX_CONV_Q16(3600,0) // Max reporting period

/**----------------------------------------------------------------------------
 * Type Declarations
 * --------------------------------------------------------------------------*/

// Pedometer algorithm configuration structure
typedef struct
{
   uint32_t sample_rate;          // sample rate, Hz, Q16
   uint32_t step_count_threshold; // number of steps after which output is updated
   float    step_threshold;       // Min accel magnitude to be considered a step, in m/s/s
   float    swing_threshold;      // Min accel magnitude for swing detection, in m/s/s
   float    step_prob_threshold;  // Probability threshold for step detection
} ped_config_s;

// Pedometer algorithm input structure
typedef struct
{
   int32_t  accel[PED_ACCEL_COLS]; // accel, m/s/s, Q16
   uint32_t timestamp;             // Timestamp of input
} ped_input_s;

// Client specific output structure
typedef struct
{
   uint8_t  client_id;       // Client id
   uint8_t  step_event;      // Indicates if a step event has been detected
                             // since the last client initiated reset
   uint8_t  step_confidence; // Probability with which latest step was detected
                             // Ranges from 0 to 100 (in percentage)
   uint32_t step_count;      // Count of steps detected since the last client
                             // initiated reset
   int32_t  step_count_error;// Error metric associated with reported step
                             // count, in steps
   float    step_rate;       // Rate in Hz at which steps are detected since
                             // the last client report/reset whichever is later
   uint32_t timestamp;       // Timestamp of input corresponding to the latest
                             // step detection (in ticks)
} ped_client_output_s;

// Pedometer algorithm output structure
typedef struct
{
   uint8_t             num_active_clients;           // No of clients
   ped_client_output_s client_data[PED_MAX_CLIENTS]; // Packed array of client data
} ped_output_s;

// Client specific state structure
typedef struct
{
   uint32_t step_count;          // No of steps since last output update
   int32_t step_count_error;     // Actual step count error for client
   uint32_t swing_step_count;    // No of steps detected in swing detection stage
   uint32_t steps_since_report;  // No of steps since last report/reset
   uint32_t overcount;           // No of steps with 0.49 < step_prob <= 0.8
   uint32_t undercount;          // No of steps with 0.2 < step_prob <= 0.49
   float total_step_rate;        // Total step rate since last report
} ped_client_state_s;

// Pedometer algorithm state structure
typedef struct
{
   ped_config_s config;        // Algo config
   uint32_t prev_input_timestamp; // Timestamp of previous sample
   uint32_t num_samples;       // No of samples processed so far
   float total_sample_rate;    // Total sample rate
   float sample_rate;          // Average sample rate, in Hz
   sns_buffer_type unfiltered_accel_buf; // Circular buffer of unfiltered accel magnitudes
   sns_buffer_type accel_buf;  // Circular buffer of filtered accel magnitudes
   buffer_type timestamp_buf;  // Circular buffer of accel sample timestamps
   uint32_t xcorr_timestamp;   // Time at which autocorrelation was calculated
   int8_t periodic;            // Indicates if acceleration is periodic
   uint32_t periodic_time;     // Timestamp of sample at which periodicity was established
   float deltaTx;              // Step interval calculated using autocorrelation
   int8_t prev_in_step;        // Sign of previous accel spread
   bool prev_valley_exists;    // Indicates if valley was detected before
   uint32_t prev_valley_index; // Index of previous valley in accel_buf
   uint32_t prev_valley_time;  // Timestamp of sample at which previous valley was detected
   float max_spread;           // Max accel spread between two consecutive valleys
   float pstate[2];            // Vector of walking and fidgeting probabilities
   ped_client_state_s client_data[PED_MAX_CLIENTS]; // Client specific data
   uint8_t step_confidence;    // Confidence of latest step
   float step_rate;            // Step rate of latest step
   uint32_t step_timestamp;    // Timestamp of latest step
   struct
   {
      bool valid;               // Indicates if data is valid
      float step_prob;          // Step probability
      uint32_t timestamp;       // Step timestamp
    } buffered_report;          // Buffer to report a step in the next iteration
} ped_state_s;


/**----------------------------------------------------------------------------
 * Function Prototypes
 * --------------------------------------------------------------------------*/

/* Pedometer algorithm API for SAM to query memory requirement */
int32_t ped_sam_mem_req(
   ped_config_s *config);

/* Pedometer algorithm API for SAM to reset algorithm */
ped_state_s* ped_sam_state_reset(
   const ped_config_s *config,
   void *mem);

/* Pedometer algorithm API for SAM to execute algorithm with new input */
void ped_sam_update(
   ped_state_s *state,
   const ped_input_s *input,
   ped_output_s *output);

/* Pedometer algorithm API for SAM to register/deregister a client */
bool ped_sam_register_client(
   uint8_t client_id,
   bool action,
   ped_state_s *state,
   ped_output_s *output,
   uint32_t timestamp);

/* Pedometer algorithm API for SAM to reset a client's statistics */
bool ped_sam_reset_client_stats(
   uint8_t client_id,
   ped_state_s *state,
   ped_output_s *output);

/* Pedometer algorithm API for SAM to inform when duty cycle state changes */
void ped_sam_handle_duty_cycle_state_change(
   bool duty_cycle_state,
   ped_state_s *state,
   ped_output_s *output,
   uint32_t timestamp);

#endif /* _PEDOMETER_H */
