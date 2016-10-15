#ifndef QMD_H
#define QMD_H

/*============================================================================
  @file qmd.h

  Qualcomm motion detector header file 

  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/sam/algo/qmd/inc/qmd.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-02-22  ad   add QMD output state  
  2011-01-24  ad   initial version  

============================================================================*/

#include <stdint.h>
#include "rel_rest.h"
#include "spi.h"

#define QMD_ACCEL_COLS (3)

typedef struct
{
  int32_t sample_rate;
  int32_t enable_veh;
  int32_t enable_abs;
  int32_t enable_rel;
  int32_t internal_config_param1;
  int32_t internal_config_param2;
  int32_t internal_config_param3;
  int32_t internal_config_param4;
} qmd_config_s;

typedef struct
{
  int32_t a[QMD_ACCEL_COLS];  // accel, m/s/s, Q16
} qmd_input_s;

typedef struct
{
  int32_t absolute_rest_state; // unknown=0, rest=1, motion=2
  int32_t vehicle_rest_state;  // unknown=0, rest=1, motion=2
  int32_t relative_rest_state; // unknown=0, rest=1, motion=2
} qmd_output_s;

typedef struct
{
  spi_state_s *abs_rest;
  spi_state_s *veh_rest;
  rel_rest_state_s *rel_rest;
  qmd_config_s config;
} qmd_state_s;

int32_t qmd_sam_mem_req(qmd_config_s *config_data);
qmd_state_s* qmd_sam_state_reset(qmd_config_s *config_data, 
                                 void *mem);
void qmd_sam_update(qmd_state_s *qmd_algo, 
                    qmd_input_s *input, 
                    qmd_output_s *output);

#endif /* QMD_H */
