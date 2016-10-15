#ifndef SMD_ALGO_H
#define SMD_ALGO_H

/*============================================================================
   @file smd_algo.h
 
   Significant Motion Detection header file 
 
   Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/sam/algo/smd/inc/smd_algo.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
   EDIT HISTORY
 
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order. 
 
   when        who  what, where, why
   ----------  ---  -----------------------------------------------------------
   2013-10-20  ad   Remove static algorithm state
   2013-09-10  dk   Changes to fixed point computation to improve precision
   2013-09-06  dk   Added support for tunable parameters
   2013-09-04  dk   Removed dead code from algorithm
   2013-08-27  dk   Added logging support for SMD's step inference
   2013-08-27  dk   Bug fixes related to pedometer circular buffer
   2013-06-05  pd   Ported algo from DSPS to ADSP
   2013-04-18  lka  Added AMD support for low-power operation
   2013-03-21  pd   Initial version  
 
============================================================================*/

#include <stdint.h>
#include "circular_buffer.h"
#include "sns_sam_qmd_v01.h"


#define SMD_ACCEL_COLS                3                   // accel columm number
#define SMD_STEP_BUF_SIZE             32

enum SMD_STATES
{
   SMD_STATE_UNKNOWN   = 0,        // unknown state
   SMD_STATE_NO_MOTION = 1,        // no motion
   SMD_STATE_MOTION    = 2        // siginificant motion
};

typedef enum
{
   SMD_ACCEL_INPUT,
   SMD_AMD_INPUT,
   SMD_PED_INPUT
} smd_input_e;

typedef enum 
{
   SNS_SMD_STEP_INFER_NONE   = 0,
   SNS_SMD_STEP_INFER_LOW    = 1,
   SNS_SMD_STEP_INFER_HIGH   = 2
} smd_step_infer_e;

typedef struct
{
   int32_t sample_rate;               // sample rate, Hz, Q16, 5Hz or 10Hz performance driven, default 10Hz
   int32_t accel_window_time;         // Size of history throughout the algorithm, sec, default 5s
   int32_t detect_threshold;          // accel, m/s/s, (Q16), AMD like std thr, default sqrt(0.05/2);
   int32_t self_transition_prob_sm;   // Self transition probability of significant motion, Q16, default 0.9
   uint8_t variable_decision_latency; // Flag to indicate: (1) use variable latency , (0) use fixed latency and max_latency always, default 1
   uint8_t max_latency;               // Maximum latency if fixed latency is used, in secs, default 10s
   uint8_t step_count_thresh;         // Step count threshold
   uint8_t step_window_time;          // Step confidence window
   int32_t eigen_thresh;              // Threshold for the eigen value
   int32_t accel_norm_std_thresh;     // Accel norm standard deviation threshold
} smd_config_s;

typedef struct
{
   union {
      int32_t accel[SMD_ACCEL_COLS];         // accel, m/s/s, Q16
      sns_sam_motion_state_e_v01 amd_state;  // state of AMD
      uint8_t step_conf;                     // detection confidence metric of reported step
   } data;
   smd_input_e datatype;                     // type of input data
   uint32_t timestamp;                       // timestamp in DSPS ticks
} smd_input_s;

typedef struct
{
   int8_t motion_state;               // Motion state, UNKNOWN(0), NO_MOTION(1), MOTION(2)
   uint8_t motion_state_probability;  // Probability of the reported motion state scaled to percentage (range 0 to 100)
   uint32_t timestamp;                // timestamp in DSPS ticks
} smd_output_s;

typedef struct
{
   smd_config_s config;
   buffer_type accel_buf;                 // buffer to store accel samples (x, y, z)
   buffer_type amag_buf;                  // buffer to store accel mag used for spread computation
   buffer_type xcor_buf;                  // buffer to store accel mag used for aucorrelation computation
   int32_t detect_threshold;              // detect_threshold square, [m2/s4], Q16
   int32_t accel_sum[SMD_ACCEL_COLS];     // sum of accel x,y,z, [m/s2], Q16
   int64_t accel_sq_sum[SMD_ACCEL_COLS];  // sum of accel square x,y,z, [m2/s4], Q16
   int32_t accel_norm_sum;                // sum of the Euclidean accel norm, [m/s2], Q16
   int64_t accel_norm_sq_sum;             // sum of the Euclidean accel norm square, [m2/s4], Q16. Can be negative
   int64_t accel_mag_sum;                 // sum of accel mag
   int64_t accel_mag_sq_sum;              // sum of square accel mag. Can be negative
   int32_t accel_mag_var;                 // accel mag variance
   int32_t variance;                      // variance, Q16
   int32_t eigenvalues[SMD_ACCEL_COLS];   // eigenvalues, Q16
   int32_t xcor;                          // auto correlation, Q16
   int32_t spread_mag;                    // spread magnitude, Q16
   int32_t accel_norm_mean_ratio;         // Ratio of norm of accel mean to mean of accel norm, Q16
   int32_t accel_norm_var;                // Variance of accel norm, Q16
   int32_t pstate[2];                     // HMM states, Q16
   uint16_t filter_delay_cnt;             // filter delay count
   uint16_t filter_delay_time;            // filter delay time, in sec
   uint16_t sample_cnt;                   // sample count
   uint32_t last_xcor_compute_time;       // last time xcor was computed
   uint32_t last_eigenvalue_compute_time; // last time eigenvalue was computed
   uint32_t accel_ts;                     // ts of last sample received
   uint32_t step_ts;                      // timestamp of latest step event
   uint32_t amd_ts;                       // timestamp of latest AMD input
   int8_t motion_state;                   // transient motion state
   int8_t final_motion_state;             // final motion state as reported to client
   sns_sam_motion_state_e_v01 amd_state;  // state of AMD
   uint8_t step_count;
   uint8_t step_buf_first_ind;
   uint8_t step_buf_last_ind;
   uint8_t step_conf_buf[SMD_STEP_BUF_SIZE];
   uint32_t step_ts_buf[SMD_STEP_BUF_SIZE];
   uint32_t step_conf;                     // detection confidence metric of reported steps
   smd_step_infer_e step_infer;
   int32_t transition_prob[2][2];          // HMM state transition probabilities
                                           // State 1 is Significant motion and state 2 is no motion 
                                           // pstate  = [0.5 0.5]'; %initial state
                                           // TP = [0.90 1-0.90; 1-0.9999 0.9999]'; %TP_ij = probability of going from i to j, 
                                           // i.e., row i is the source state and column j is the target state
                                           // Note the sw inverts the array and uses computation accordingly.
} smd_state_s;

int32_t smd_sam_mem_req(
   smd_config_s *config_data);

smd_state_s* smd_sam_state_reset(
   smd_config_s *config_data,
   void *mem);

void smd_sam_update(
   smd_state_s *smd_algo,
   smd_input_s *input,
   smd_output_s *output);

#endif /* SMD_H */

