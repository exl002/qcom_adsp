#ifndef THRESHOLD_H
#define THRESHOLD_H

/*============================================================================
  @file threshold.h

  The threshold algorithm header file

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-10-10  sc   Added threshold type (to support relative threshold)
  2011-11-03  sc   initial version  

============================================================================*/

#include <stdint.h>
#include <stdbool.h>

#define THRESH_MAX_AXIS_NUM      (3)

#define THRESH_TYPE_ABSOLUTE     (0)
#define THRESH_TYPE_RELATIVE     (1)

typedef struct
{
  uint8_t  sensor_id;
  uint8_t  data_type;
  uint8_t  num_axis;
  uint32_t sample_rate;  // in Hz, Q16
  uint32_t thresh[THRESH_MAX_AXIS_NUM];    // Q16
  uint32_t report_period; // in dsps clock ticks, same ticks as timestamp
  uint8_t  threshold_type;
} threshold_config_s;

typedef struct
{
  int32_t sample[3];     // sensor sample data, Q16
  uint32_t timestamp;
} threshold_input_s;

typedef struct
{
  bool    do_report;     // whether samples are to be reported
  uint8_t  num_axis;
  int32_t sample[3];     // sensor sample data, Q16
  uint32_t timestamp;
} threshold_output_s;

typedef struct
{
  threshold_config_s config;
  int32_t last_report[3];
  bool    first_time;
  uint32_t ts_start_period[3];
} threshold_state_s;

/*=========================================================================
  FUNCTION:  threshold_sam_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed by the algorithm with given configuration

    @param[i] config_data: Pointer to algorithm configuration

    @return Size of state memory required, given the configuration settings
            0 if the configuration is invalid
*/
/*=======================================================================*/
int32_t threshold_sam_mem_req(const threshold_config_s *config_data);

/*=========================================================================
  FUNCTION:  threshold_sam_state_reset
  =======================================================================*/
/*!
    @brief Resets the algorithm instance with given configuration

    @param[i] config_data: Pointer to algorithm configuration
    @param[i] mem: Pointer to a block of allocated algorithm state memory

    @return Pointer to a valid algorithm state memory if successful
            NULL if error
*/
/*=======================================================================*/
threshold_state_s* threshold_sam_state_reset(
   const threshold_config_s *config_data, 
   void *mem);

/*=========================================================================
  FUNCTION:  threshold_sam_update
  =======================================================================*/
/*!
    @brief Execute the algorithm, update the output with given input

    @param[i] quat_algo: Pointer to algorithm state memory
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void threshold_sam_update(threshold_state_s *thresh_algo, 
                          const threshold_input_s *input, 
                          threshold_output_s *output);

#endif /* THRESHOLD_H */
