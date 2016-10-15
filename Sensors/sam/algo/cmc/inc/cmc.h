#ifndef CMC_H
#define CMC_H

/*=============================================================================
 Qualcomm Coarse Motion Classifier (CMC) header file

 Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 =============================================================================*/

/*============================================================================
 * EDIT HISTORY
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * when        who  what, where, why
 * ----------  ---  -----------------------------------------------------------
 * 2013-04-08  jayarama   changes for CMC 1.0
 * 2013-03-04  ks         initial version
 *
 *============================================================================*/
/**----------------------------------------------------------------------------
 * Include Files
 * --------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "motion_feature.h"
#include "cmc_log_utils.h"
#include "cmc_stats_model.h"

/* Target specific stuff */
#ifndef OFFTARGET_UNIT_TESTING

#include "sns_sam_priv.h"
#include <fixed_point.h>
#include "sns_em.h"
#include "sns_debug_str.h"

#endif

/**----------------------------------------------------------------------------
 * Macro Declarations
 * --------------------------------------------------------------------------*/

/* Target specific stuff */
#ifndef OFFTARGET_UNIT_TESTING

#define CMC_MAX_CLIENTS SNS_SAM_MAX_CLIENT_REQS_PER_ALGO_INST
/*default accel sample rate*/
#define CMC_SAMPLE_RATE_DEF_Q16 FX_CONV_Q16(20, 0) // Default sample rate
#define CMC_ACCEL_INITIAL_STATE (1)
#define ROUND_TO_4_DECIMAL_PLACES(x) (x)

#define CMC_MEMZERO(x,y) SNS_OS_MEMZERO(x,y)

#else

#define CMC_MAX_CLIENTS 2
#define PI 3.141592f
#define FX_FIXTOFLT_Q16(x) ((float)(x) / (float)(65536.0f))
#define FX_FLTTOFIX_Q16(x) ((int32_t) (x*65536))

#define ROUND_TO_4_DECIMAL_PLACES(x) (float) ((x)>0.00005 ? (ceil(((x)*10000)-0.5)/10000) : 0)

#define CMC_MEMZERO(x,y) memset(x,0,y)

#endif

/* regularization constants */
#define LOGREGULARIZATION (0.000001f)
#define PROBABILITYREGULARIZATION (0.000001f)

/*sample rate for accel stream in Hz*/
#define CMC_SAMPLE_RATE (20)

/*window length in sec over which features are extracted*/
#define CMC_MMT_WIN_LEN (1)

/*total supported device positions*/
#define CMC_DEVPOS_NUM (9)

/*total supported nodes in the motion state tree*/
#define CMC_STATE_TREE_MAX_CHILD_NODES (3)

/*total supported leaf nodes in motion state tree*/
#define CMC_MS_LEAF_NUM (CMC_NUM_MOTION_STATES)
#define CMC_MS_LEAF_NUM_WITH_UNKNOWN (CMC_MS_LEAF_NUM + 1)

/*total supported leaf nodes in motion state tree*/
#define CMC_CONFIDENCE_THRESH_MAX (10)

#define QMD_REST_PERSISTENCE_TIME_THRESH (10)
#define QMD_REST_PERSISTENCE_COUNT_THRESH (QMD_REST_PERSISTENCE_TIME_THRESH*CMC_SAMPLE_RATE)

/*number of states in the logistic regression is 2 since it is a binary classifier*/
#define CMC_NUM_LR_STATES (2)

/*Indices of different motion states in the unfiltered probability array*/
#define CMC_PROB_UNKNOWN_IDX (5)

#define CMC_PROB_WALK_IDX (0)

#define CMC_PROB_RUN_IDX (1)

#define CMC_PROB_STATIONARY_IDX (2)

#define CMC_PROB_VEHICLE_IDX (3)

#define CMC_PROB_BIKE_IDX (4)

#define CMC_INVALID_IDX (-1)

#define MAX_UINT8_VALUE (255)

/**----------------------------------------------------------------------------
 * Type Declarations
 * --------------------------------------------------------------------------*/
/* supported motion states*/
typedef enum
{
    CMC_MS_UNKNOWN = 0,
    CMC_MS_STATIONARY = 1,
    CMC_MS_MOVE = 2,
    CMC_MS_FIDDLE = 3,
    CMC_MS_PEDESTRIAN = 4,
    CMC_MS_VEHICLE = 5,
    CMC_MS_WALK = 6,
    CMC_MS_RUN = 7,
    CMC_MS_BIKE = 8,
    CMC_MS_MAX_NUM = 9
} cmc_ms_e;

typedef enum
{
    QMD_MS_UNKNOWN = 0,
    QMD_MS_REST = 1,
    QMD_MS_MOVE = 2
} qmd_ms_e;

static const cmc_ms_e
motion_state_likelihood_and_probability_array_order[CMC_MS_LEAF_NUM] =
{ CMC_MS_WALK, CMC_MS_RUN, CMC_MS_STATIONARY,
        CMC_MS_VEHICLE, CMC_MS_BIKE };

/* input datatypes */
typedef enum
{
    CMC_ACCEL_INPUT, // accel input
    CMC_QMD_INPUT,
    CMC_UPDATE_REPORTING_INPUT
    // Qualcomm Motion Detector (QMD) input
} cmc_input_e;

typedef enum
{
    CMC_MOTION_EVENT_OUT = 0,
    CMC_MOTION_EVENT_ENTRY = 1,
    CMC_MOTION_EVENT_EXIT = 2,
    CMC_MOTION_EVENT_IN = 3
} cmc_motion_event_e;

typedef enum
{
    CMC_MS_EVENT_REPORT_TYPE_NONE = 0,
    CMC_MS_EVENT_REPORT_TYPE_EXIT_ONLY = 1,
    CMC_MS_EVENT_REPORT_TYPE_ENTRY_ONLY = 2,
    CMC_MS_EVENT_REPORT_TYPE_ALL = 3
} cmc_ms_event_report_type_e;

typedef enum
{
    CMC_UPDATE_REPORT_ENABLE_ENTRY = 0,
    CMC_UPDATE_REPORT_ENABLE_EXIT = 1,
    CMC_UPDATE_REPORT_DISABLE_ENTRY = 2,
    CMC_UPDATE_REPORT_DISABLE_EXIT = 3
} cmc_update_report_type_e;

typedef struct
{
    float gmm_likelihood[CMC_MS_LEAF_NUM][CMC_DEVPOS_NUM];
} cmc_feature_likelihood_s;/* CMC algorithm configuration structure */

typedef struct
{
    float probability_threshold;
    int32_t history_length;
    int32_t num_high_outputs_lower_bound;
    int32_t num_high_outputs_upper_bound;
    int32_t max_timer_val;
} cmc_commute_logic_config_s;

typedef struct
{
    uint32_t sample_rate; // sample rate, Hz, Q16
    uint8_t accel_on;
} cmc_config_s;

typedef struct
{
    cmc_ms_e stable_state;
    cmc_ms_e transition_state;
    bool is_transition;
    uint16_t last_known_transition_count_before_reset;
    uint16_t transition_count;
    cmc_ms_e state_with_largest_prob;
    float largest_prob;
    cmc_ms_e state_with_second_largest_prob;
    float second_largest_prob;
} cmc_hysteresis_state_s;

typedef struct
{
    float drive_entry_unfilt_prob_buffer[DRIVE_ENTRY_WINDOW];
    float drive_filt_prob_buffer[DRIVE_LOGIC_HISTORY_LENGTH];
    int32_t drive_filt_prob_buffer_index;
    float bike_filt_prob_buffer[BIKE_LOGIC_HISTORY_LENGTH];
    int32_t bike_filt_prob_buffer_index;
    int32_t stationary_count;
    bool exited_bike;
    bool exited_drive;
} cmc_heuristics_state_s;

typedef struct
{
    uint32_t timestamp;
    cmc_ms_e motion_state;
    cmc_motion_event_e motion_event;
    float probability;
} cmc_motion_s;

typedef struct
{
    cmc_ms_e report_motion_state;
    cmc_ms_event_report_type_e report_event_type;
} cmc_report_mask_s;

typedef struct
{
    cmc_ms_e ms_event; // The motion state for which reporting type needs to be updated
    cmc_update_report_type_e report_type; // Update the current reporting type for the specified motion state
} cmc_update_report_req_s;


/* CMC algorithm state structure */
/* For all arrays declared with size CMC_MS_LEAF_NUM_WITH_UNKNOWN, use CMC_PROB_<state>_IDX
   defined above to index into the arrays. */
typedef struct
{
    uint32_t timestamp; // timestamp corresponding to latest input
    cmc_config_s config; // algorithm configuration
    motion_feature_state_s motion_feature;
    cmc_feature_likelihood_s feature_likelihood;
    float unfilt_gmm_prob[CMC_MS_LEAF_NUM];
    float unfilt_lr_prob_stat_dr[CMC_NUM_LR_STATES];
    float unfilt_lr_prob_walk_bike[CMC_NUM_LR_STATES];
    float unfilt_prob[CMC_MS_LEAF_NUM];
    float filt_prob[CMC_MS_LEAF_NUM];
    float prev_filt_prob[CMC_MS_LEAF_NUM];
    cmc_ms_e pre_hysteresis_motion_state;
    cmc_ms_e post_hysteresis_motion_state;
    cmc_ms_e motion_state_prev;
    qmd_ms_e curr_qmd_motion_state;
    uint8_t in_vehicle;
    uint8_t in_bike;
    uint16_t amd_rest_persistence_count;
    bool vehicle_entry_heuristic_passed;
    int32_t msecs_to_predate;
    cmc_hysteresis_state_s hysteresis_state;
    cmc_heuristics_state_s heuristics_state;
    bool motion_state_changed;
    cmc_motion_s motion_states[CMC_MS_LEAF_NUM_WITH_UNKNOWN];
    // Move this report type into a client structure once client configurability is made available
    cmc_ms_event_report_type_e event_report_types[CMC_MS_LEAF_NUM_WITH_UNKNOWN];
} cmc_state_s;

/* CMC input structure */
typedef struct
{
    union
    {
        int32_t accel[MOTION_FEATURE_ACCEL_COLS]; // accel, m/s/s, Q16
        qmd_ms_e qmd_motion_state; // QMD input, 0 unknown, 1 stationary, 2 motion
        cmc_update_report_req_s update_report_req; // Update Report Req
    } data;
    cmc_input_e datatype; // type of input data
    uint32_t timestamp; // data timestamp in clockticks
} cmc_input_s;

/* CMC output structure */
typedef struct
{
    uint32_t timestamp;
    cmc_motion_s motion_outputs[CMC_MS_LEAF_NUM_WITH_UNKNOWN];
    uint8_t output_ready;
    uint8_t config_updated;
} cmc_output_s;

/**----------------------------------------------------------------------------
 * Function Prototypes
 * --------------------------------------------------------------------------*/

/*============================================================================

 FUNCTION:   convert_ms_to_probability_index

 ============================================================================*/
/*!
 @brief Returns the index into the probability arrays corresponding to a
 motion state

 @return index of the motion state in the probability array
 */
int8_t convert_ms_to_probability_index(cmc_ms_e motion_state);

/*============================================================================

 FUNCTION:   cmc_sam_mem_req

 ============================================================================*/
/*!
 @brief Returns the memory requirements for the CMC module

 @return size in bytes of the memory required for CMC
 */
int32_t cmc_sam_mem_req(const cmc_config_s* config_data);

/*============================================================================

 FUNCTION:   cmc_sam_state_reset

 ============================================================================*/
/*!
 @brief Resets the CMC state structure

 @return The state structure after it has been reset
 */
cmc_state_s* cmc_sam_state_reset(const cmc_config_s* config_data, void* mem);

/*============================================================================

 FUNCTION:   cmc_sam_update

 ============================================================================*/
/*!
 @brief Processes the cmc_input structure and populates the
 cmc_state and cmc_output structures

 */
void cmc_sam_update(cmc_state_s* cmc_algo, const cmc_input_s* input,
        cmc_output_s* output);

// Functions for off-target testing to be featurized out on target
#ifdef OFFTARGET_UNIT_TESTING
void cmc_sam_compute_offtarget(cmc_state_s *state, cmc_output_s *output);
void cmc_check_for_unknown_state_offtarget(cmc_state_s *state);
#endif
#endif // CMC_H

