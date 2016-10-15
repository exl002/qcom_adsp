/*============================================================================
  @file threshold.c

  The threshold algorithm source file 

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-10-23  sc   Fix an issue handling negative number in relative threshold
  2013-10-10  sc   Added support for relative threshold
  2013-08-14  sc   Fix the condition for updating outputs
  2012-01-10  hw   Fix the typecasting issue in threshold comparison 
  2012-12-20  hw   Add absolution value to right hand side for threshold comparison
  2012-10-10  hw   Updated the type casting for threshold algorithm
  2011-11-03  sc   initial version

============================================================================*/

#include "threshold.h"
#include "fixed_point.h"

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
int32_t threshold_sam_mem_req(const threshold_config_s *config_data)
{
  return (sizeof(threshold_state_s));
}

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
  void *mem)
{
  threshold_state_s *thresh_algo;

  thresh_algo = (threshold_state_s*)mem;
  thresh_algo->config = *config_data;   // structure copy
  thresh_algo->first_time = true;
  thresh_algo->last_report[0] = 0;
  thresh_algo->last_report[1] = 0;
  thresh_algo->last_report[2] = 0;

  return thresh_algo;
}

/*=========================================================================
  FUNCTION: threshold_update_results
  ========================================================================*/
/*!
    @brief Updates the output structure 

    @param[i] quat_algo: Pointer to algorithm state data
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void threshold_update_results(threshold_state_s *thresh_algo, 
                          const threshold_input_s *input, 
                          threshold_output_s *output)
{
  uint32_t i;
  output->timestamp = input->timestamp;
  output->num_axis = thresh_algo->config.num_axis;
  for (i=0; i < output->num_axis; i++)
  {
    output->sample[i] = input->sample[i];
    thresh_algo->ts_start_period[i] = 0;
    thresh_algo->last_report[i] = input->sample[i];
  }
  return;
}


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
                          threshold_output_s *output)
{
  int32_t i;
  uint8_t num_axis = thresh_algo->config.num_axis;

  /* first time running the algo, always report */
  if (thresh_algo->first_time == true)
  {
    thresh_algo->first_time = false;
    threshold_update_results(thresh_algo, input, output);
    return;
  }

  /* if not first time, check with the threshold */
  for (i=0; i < num_axis; i++) 
  {
    bool b_update = false;
    if (thresh_algo->config.threshold_type == THRESH_TYPE_ABSOLUTE) 
    {
      if ( (uint32_t)FX_ABS(thresh_algo->last_report[i] - input->sample[i]) >=
           (uint32_t)thresh_algo->config.thresh[i] )
      {
        b_update = true;
      }
    }
    else if (thresh_algo->config.threshold_type == THRESH_TYPE_RELATIVE)
    {
      if ( (uint32_t)FX_ABS(thresh_algo->last_report[i] - input->sample[i]) >=
           (uint32_t)FX_ABS(FX_MUL_Q16(thresh_algo->config.thresh[i],thresh_algo->last_report[i])) )
      {
        b_update = true;
      }
    }

    if (b_update)
    {
      if (0 == thresh_algo->ts_start_period[i])
      { 
        thresh_algo->ts_start_period[i] = input->timestamp;
      } 
      if (input->timestamp >= thresh_algo->ts_start_period[i])
      {
        if ( input->timestamp - thresh_algo->ts_start_period[i] >=
             thresh_algo->config.report_period )
        {
          threshold_update_results(thresh_algo, input, output);
        }
      }
      else if (((UINT32_MAX - thresh_algo->ts_start_period[i]) +
                input->timestamp) >= thresh_algo->config.report_period)
      {
        threshold_update_results(thresh_algo, input, output);
      }
    } 
    else /* b_update is false */
    {
      thresh_algo->ts_start_period[i] = 0;
    }
  }
}
