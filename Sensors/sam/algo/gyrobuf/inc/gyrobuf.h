#ifndef _GYROBUF_H_
#define _GYROBUF_H_

/*============================================================================
  @file gyrobuf.h

  Gyro Buffering algorithm.

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/sam/algo/gyrobuf/inc/gyrobuf.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013/02/07  asj  Added time salt to gyrobuf
  2012/03/26  lka  Initial version

============================================================================*/

/**------------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/**----------------------------------------------------------------------------
 * Macro Declarations
 * --------------------------------------------------------------------------*/
#define MAX_SAMPLE_LEN        16     /* Must be power of 2 (2,4,16,...,65536),
                                        and should be at least as large as
                                        SNS_SAM_GYROBUF_MAX_BUFSIZE_V01
                                      */
#define MAX_CBUFFER_SIZE      MAX_SAMPLE_LEN

#define MAX_PENDING_REPORT    4      /* keep it no more than 4; linear search */
#define FLAG_EMPTY_REPORT     0      /* Indicator for empty report entry */

/**----------------------------------------------------------------------------
 * Type Declarations
 * --------------------------------------------------------------------------*/
/**
 * @brief   Gyro sample: (x,y,z,ts)
 */
typedef struct {
  int32_t   x;        /* gyro x value */
  int32_t   y;        /* gyro y value */
  int32_t   z;        /* gyro z value */
  uint32_t  ts;       /* timestamp (usec) corresponding to this gyro sample */
} gyrobuf_sample_t;

/**
 * @brief   Timestamp request pair: (t_start,t_end)
 */
typedef struct {
  uint32_t  t_start;
  uint32_t  t_end;
  uint8_t   valid;
  uint8_t   seqnum;   /* sequence number from get_report_req */
} gyrobuf_ts_t;

/**
 *  @brief  Circular buffer
 */
typedef struct {
  uint16_t          head;           /* points to oldest element in buffer */
  uint16_t          tail;           /* points to next empty slot in buffer */
  gyrobuf_sample_t  val[MAX_CBUFFER_SIZE]; /* sample values */
} cbuf_t;

typedef struct {
  uint32_t          sample_rate;    /* (Hz) */
  bool              extra_sample;   /* setup byte */
  uint8_t           max_num_sample_per_msg;
  uint32_t          init_ts;        /* timestamp of creation */
} gyrobuf_config_s;

typedef struct {
  int32_t           sample[3];      /* gyro, deg/sec, Q16 */
  uint32_t          timestamp;      /* timestamp associated with these samples */
} gyrobuf_input_s;

typedef struct {
  gyrobuf_config_s  input_config;   /* keeps a copy of original input parameters */
  uint32_t          sample_int;     /* (ticks) */
  cbuf_t            buf;            /* circular buffer for gyro samples (x,y,z) */
  uint32_t          timestamp;

  /* Buffer for pending data requests */
  gyrobuf_ts_t      pending_report[MAX_PENDING_REPORT];
  uint8_t           num_pending_report;
} gyrobuf_state_s;

typedef struct {
  gyrobuf_state_s   *state;
  uint32_t          t_start;  /* t_start from the most recent pending_report */
  uint32_t          t_end;    /* t_end from the most recent pending_report */
  gyrobuf_sample_t  outbuf[8]; /* output buffer for gyro samples;
                                * MAX_SAMPLE_LEN >=
                                *   SNS_SAM_GYROBUF_BUFSIZE_V01
                                *
                                * Note: it is set at 8 because we think memory
                                * pool on the DSPS does not have any buffers
                                * of that size. Will resolve this later.
                                */
  uint32_t          outbuf_len;             /* number of samples in outbuf */
  uint8_t           seqnum;   /* sequence number associated with (t_start,t_end) */
} gyrobuf_output_s;

/**----------------------------------------------------------------------------
 * Function Prototypes
 * --------------------------------------------------------------------------*/
/**
 *  @brief    Queues up sensor data requests
 *
 *  @param    out     - gyrobuf algo output C-struct
 *  @param    t_start - start timestamp (DSPS tick)
 *  @param    t_end   - end timestamp (DSPS tick)
 *  @param    seqnum  - sequence number provided by client (optional)
 *  @return   true if successful, false if queue is full
 */
bool gyrobuf_sam_enqueue_req( gyrobuf_output_s  *out,
                              uint32_t  t_start,
                              uint32_t  t_end,
                              uint8_t   seqnum);

/**
 *  @brief    Fetches buffered data
 *
 *  @param    output    - gyrobuf algo output C-struct
 *  @param    entry     - index to the corresponding request (entry==0 --> oldest, etc)
 *  @param    max_num_sample  - maximum number of samples to return in p_outbuf
 *  @return   number of samples returned
 */
int gyrobuf_sam_fetch_data( gyrobuf_output_s  *output,
                            uint8_t           entry,
                            uint16_t          max_num_sample);

/**
 *  @brief    Gyrobuf SAM Memory Request
 *
 *  @param    config_data - gyrobuf algo config C-struct
 *  @return   memory usage of algorithm
 */
int32_t gyrobuf_sam_mem_req(gyrobuf_config_s *config_data);

/**
 *  @brief    Gyrobuf SAM State Reset
 *
 *  @param    config_data - gyrobuf algo config C-struct
 *  @param    mem  - pointer to actual memory for config_data
 *  @return   state pointer
 */
gyrobuf_state_s* gyrobuf_sam_state_reset(gyrobuf_config_s *config_data, void *mem);

/**
 *  @brief    Gyrobuf SAM Update
 *
 *  @param    state   - gyrobuf algo state C-struct
 *  @param    input   - gyrobuf algo input C-struct
 *  @param    output  - gyrobuf algo output C-struct
 *  @return   None
 */
void gyrobuf_sam_update(gyrobuf_state_s   *state,
                        gyrobuf_input_s   *input,
                        gyrobuf_output_s  *output);

#endif /* _GYROBUF_H_ */
