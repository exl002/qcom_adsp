#ifndef SNS_PROFILING_H
#define SNS_PROFILING_H

/*============================================================================
  @file sns_profiling.h

  The profiling code header file

  Copyright (c) 2012 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential
============================================================================*/

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-10-31  hw   add QDSS support for sensor
  2013-07-24  vh   Eliminated compiler warnings
  2013-06-25  hw   Add latency measurement support
  2012-12-31  hw   initial version

============================================================================*/

#include <stdio.h>
#include <qurt.h>
#include "qurt_timer.h"
#include "qurt_elite.h"

#ifdef SNS_QDSS_SWE
#include "tracer.h"
#include "tracer_event_ids.h"
#endif /* SNS_QDSS_SWE */

#define SNS_PWR_PROFILING_BUF_SIZE 4096

/* SMGR process latency profiling markers */
#define SNS_SMGR_LATENCY_PROFILE_START  0x11111111
#define SNS_SMGR_LATENCY_PROFILE_END    0x66666666

/* SAM algorithm execution time profiling markers */
#define SNS_SAM_ALGO_EXEC_PROFILE_START 0x77777777
#define SNS_SAM_ALGO_EXEC_PROFILE_END   0x55555555

/* SMGR & driver function makers for QDSS */
#define SNS_QDSS_DD_GET_DATA 0
#define SNS_QDSS_DD_HANDLE_TIMER 1
#define SNS_QDSS_DD_HANDLE_IRQ 2

#ifdef SNS_QDSS_SWE
#define sns_profiling_log_qdss(a, b, ... ) \
        (b==0)? \
        tracer_event_simple( (tracer_event_id_t)a ): \
        tracer_event_simple_vargs( (tracer_event_id_t)a, b, ## __VA_ARGS__ )
#else
#define sns_profiling_log_qdss(a, b, ... )
#endif /* SNS_QDSS_SWE */

unsigned int sns_pwr_prof_loc_count, sns_pwr_wraparound_count;            //count the size of current buffer
typedef struct {
  uint64_t code_loc;
  uint64_t timestamp;
}sns_pwr_prof_struct;

typedef struct {
  boolean  ltcy_measure_enabled;
  uint32_t polling_cb_ts;
  uint32_t polling_get_data_start_ts;
  uint32_t polling_get_data_end_ts;
  uint32_t dri_notify_irq_ts;
  uint32_t dri_get_data_start_ts;
  uint32_t dri_get_data_end_ts;
}sns_profiling_latency_s;

sns_profiling_latency_s sns_latency;

volatile sns_pwr_prof_struct sns_pwr_profiler[SNS_PWR_PROFILING_BUF_SIZE] __attribute__ ((aligned (256)));
//volatile sns_pwr_prof_struct *ptr_to_curr_loc_in_profiling_struct_array;

/* Initiate the buffer */
void sns_profiling_struct_init(sns_pwr_prof_struct *profiler);

/* Get the current location and timestamp */
void sns_profiling_log_timestamp(uint64_t curr_loc_tag);

/* Initiate the profiling code */
void sns_profiling_init(void);

/* Sampling latency measurement for Polling mode */
void sns_profiling_log_latency_poll(
        sns_profiling_latency_s    latency,
        int32_t                    sensor_id);

/* Sampling latency measurement for DRI mode */
void sns_profiling_log_latency_dri(
        sns_profiling_latency_s    latency,
        int32_t                    sensor_id);

#endif /* SNS_PROFILING_H */
