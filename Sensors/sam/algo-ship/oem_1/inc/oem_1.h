#ifndef OEM1_H
#define OEM1_H

/*============================================================================
  @file oem_1.h

  Qualcomm motion detector header file 

  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/


/*============================================================================


============================================================================*/

#include <stdint.h>


typedef struct
{
  int32_t sample_rate;
  int32_t internal_config_param1;
} oem_1_config_s;

//this is a sample variable, OEMs may or may not use
typedef struct
{
  int32_t sample_1[3]; //input sample from sensor 1
//  int32_t sample_2[3]; //input sample from sensor 2 - jb-ng 
  uint32_t sample1_ts;
//  uint32_t sample2_ts; //jb-ng 
} oem_1_input_s;

//this is a sample variable, OEMs may or may not use
typedef struct
{
  int32_t data[3];
  uint32_t timestamp;
} oem_1_output_s;

//this is a sample state variable, OEMs may or may not use
typedef struct
{
  uint32_t state_param1;
  oem_1_config_s config;
} oem_1_state_s;

int32_t oem_1_sam_mem_req(oem_1_config_s *config_data);
oem_1_state_s* oem_1_sam_state_reset(oem_1_config_s *config_data, 
                                 void *mem);
void oem_1_sam_update(oem_1_state_s *state, 
                    oem_1_input_s *input, 
                    oem_1_output_s *output);

#endif /* OEM1_H */
