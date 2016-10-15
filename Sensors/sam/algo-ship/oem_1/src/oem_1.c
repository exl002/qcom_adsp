/*============================================================================
  @file oem_1.c

  OEM_1 Algorithm sample source file 

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/



/*============================================================================
  EDIT HISTORY

============================================================================*/

#include "oem_1.h"

int32_t oem_1_sam_mem_req(oem_1_config_s *config_data)
{
//   int cnt;
   return (sizeof(oem_1_state_s));
}

oem_1_state_s* oem_1_sam_state_reset(oem_1_config_s *config_data, void *mem)
{
   oem_1_state_s *oem_1_state = (oem_1_state_s *)mem ;

   oem_1_state->config = *config_data;
   oem_1_state->state_param1 = 0;
   return oem_1_state;
}

void oem_1_sam_update(oem_1_state_s *p_state, 
                      oem_1_input_s *p_input, 
                      oem_1_output_s *p_output)
{

  if (p_input->sample_1[2] < 0)   //face-up
  {
    p_output->data[0] = 1; 
  } 
  else                           //face-down  
  {
    p_output->data[0] = 2; 
  }

  p_output->data[1] = p_input->sample_1[2];
  p_output->data[2] = 100;


  p_output->timestamp = p_input->sample1_ts;

  return;
}
