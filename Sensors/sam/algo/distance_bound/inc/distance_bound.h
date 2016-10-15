#ifndef DISTANCE_BOUND_H
#define DISTANCE_BOUND_H

/*=============================================================================
Qualcomm Distance Bound header file 

Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
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
* 2013-03-26  yg   initial version
*
*============================================================================*/
/**----------------------------------------------------------------------------
* Include Files
* --------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include "sns_sam_distance_bound_v01.h"

/* Target specific stuff */
#ifndef OFFTARGET_UNIT_TESTING

#include "sns_sam_priv.h"
#include <fixed_point.h>

#endif

/**----------------------------------------------------------------------------
* Macro Declarations
* --------------------------------------------------------------------------*/

//TODO: Move them to right file
#define SNS_SAM_ONE_SHOT_PERIOD_REQ_NONE  -1
#define SNS_SAM_ONE_SHOT_PERIOD_REQ_DISABLE  -2
#define SNS_SAM_ONE_SHOT_PERIOD_REQ_ENABLE  -3
#define SNS_SAM_MIN_ONE_SHOT_PERIOD  5 // in microseconds
#define SNS_SAM_PROCESSING_DELAY  1

#define DISTANCE_BOUND_MOTION_STATE_NUM 9

/**----------------------------------------------------------------------------
* Type Declarations
* --------------------------------------------------------------------------*/

/* input datatypes */
typedef enum 
{
  DISTANCE_BOUND_INACTIVE = 0,
  DISTANCE_BOUND_ACTIVE = 1 
} distance_bound_algo_state_e;

typedef enum 
{
  DISTANCE_BOUND_SET_DISTANCE_BOUND = 0, // Set distance bound from client
  DISTANCE_BOUND_MOTION_STATE_INPUT = 1, // motion state input from CMC
  DISTANCE_BOUND_REFRESH_OUTPUT = 2      // Refresh algo output
} distance_bound_input_e;
  
typedef struct
{
   float motion_state_speed_bounds[DISTANCE_BOUND_MOTION_STATE_NUM];
} distance_bound_config_s;


// client specific state structure
typedef struct
{
   uint8_t      client_id;
   boolean      is_registered;                /**< false: client not registered; true: client is registered */
   uint8_t      algo_state;                   /**< 0: inactive 1: active                          */
   uint8_t      prev_cmc_motion_state;        /**< Motion State used at last distance bound computation */
   uint32_t     prev_timestamp;               /**< Time stamp at last distance bound computation  */
   float        prev_speed;                   /**< Speed at last distance bound computation       */
   boolean      is_simultaneous_state;        /**< true: CMC last reported simultaneous states; false: CMC did not last report simultaneous states  */
   uint8_t      prev_simultaneous_cmc_state;  /**< Previous simultaneous CMC state                */
   float        time_to_breach;               /**< Time to breach of client set bound             */
   float        distance_bound;               /**< Initial distance bound value in meters         */
   float        remaining_distance;           /**< Remaining distance in meters                   */
} distance_bound_client_state_s;

/* Distance Bound algorithm state structure */
typedef struct
{
  boolean      is_configured;                /**< false: not configured; true: configured */
  distance_bound_config_s config; // algorithm configuration
  uint64_t     config_timestamp;                 /**< Time in SSC ticks when algorithm is configured */
  distance_bound_client_state_s client_data; // client data
} distance_bound_state_s;

typedef struct
{
  uint8_t motion_state;
  uint8_t motion_event;
} cmc_motion_event_s;

/* Distance Bound algorithm input structure */
typedef struct
{
  union
  {
    uint8_t                  session_key;           /**< A unique key provided by the client that identifies the session associated
                                                        with the sns_sam_distance_bound_set_bound_req_msg. A session is defined as
                                                        the period between the arrival of a sns_sam_distance_bound_set_bound_req_msg
                                                        and the generation of a sns_sam_distance_bound_report_ind_msg associated with
                                                        it. */
    float                    distance_bound;        /**< distance bound setting from client  */
    cmc_motion_event_s       cmc_motion_event;      /**< Motion event input from CMC         */
    uint8_t                  client_req_id;         /**< client request id                   */
  } data;
  distance_bound_input_e datatype; // type of input data
  uint32_t timestamp;   // data timestamp in clockticks
} distance_bound_input_s;

// client specific output structure
typedef struct
{
  uint8_t client_id;
  uint8_t      session_key;               /**< A unique key provided by the client that identifies the session associated
                                                with the sns_sam_distance_bound_set_bound_req_msg. A session is defined as
                                                the period between the arrival of a sns_sam_distance_bound_set_bound_req_msg
                                                and the generation of a sns_sam_distance_bound_report_ind_msg associated with
                                                it. */
  int32_t one_shot_period_status;        /* Set to one of: 
                                                SNS_SAM_ONE_SHOT_PERIOD_REQ_NONE
                                                SNS_SAM_ONE_SHOT_PERIOD_REQ_DISABLE
                                                SNS_SAM_ONE_SHOT_PERIOD_REQ_ENABLE */
  uint32_t one_shot_period;              /* Valid only when one_shot_period_status is set to SNS_SAM_ONE_SHOT_PERIOD_REQ_ENABLE */
  uint32_t timestamp; // Timestamp of input with which latest distance bound report was computed in SSC ticks 
  float        distance_accumulated;        /**< distance accumulated in meters */
  sns_sam_distance_bound_breach_event_e_v01 is_distance_bound_breached;
                    /*
                      Indicates if the distance bound set by client has been breached.
                      Set to SNS_SAM_DISTANCE_BOUND_BREACH_EVENT_UNKNOWN if
                      no sns_sam_distance_bound_set_bound_req_msg has been received. All other
                      output fields are to be ignored.
                      Set to SNS_SAM_DISTANCE_BOUND_BREACH_EVENT_TRUE if breach has occurred.
                      Set to SNS_SAM_DISTANCE_BOUND_BREACH_EVENT_FALSE if breach has not occurred.
                    */
} distance_bound_client_output_s;

// Distance Bound output structure
typedef struct
{
  distance_bound_client_output_s client_data; // client data
} distance_bound_output_s;

/**----------------------------------------------------------------------------
* Function Prototypes
* --------------------------------------------------------------------------*/

/* Distance Bound algorithm API for SAM to query memory requirement */
int32_t distance_bound_sam_mem_req(const distance_bound_config_s* config_data);

/* Distance Bound algorithm API for SAM to reset algorithm */
distance_bound_state_s* distance_bound_sam_state_reset(const distance_bound_config_s* config_data, void* mem);

/* Distance Bound algorithm API for SAM to execute algorithm with new input */
void distance_bound_sam_update(distance_bound_state_s* distance_bound_algo,  const distance_bound_input_s* input, distance_bound_output_s* output);

/* Distance Bound algorithm API for SAM to register/deregister a client */
bool distance_bound_sam_register_client(uint8_t client_id, bool action, distance_bound_state_s *state,
                    distance_bound_output_s *output, uint32_t timestamp);

float distance_bound_process_get_max_distance_supported(void);

#endif // DISTANCE_BOUND_H

