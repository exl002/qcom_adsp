#ifndef MAG_CAL_H
#define MAG_CAL_H

/*=============================================================================
  Qualcomm magnetometer calibration detector header file 
 
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 * 2011-12-01  ad   update the default calibrated mag rate
 * 2011-08-10  dc   initial version
 * 2011-11-03  ry   Added timestamp field to mag_cal_input_s
 *                  Removed internal mag cal function prototypes
 *
 *============================================================================*/
#include <stdint.h>
#include "sensor1.h"
#include "sns_sam_algo.h"

#define MAG_CAL_COLS (3) 

#define SNS_MAG_CAL_MIN_MAG_SAMP_RATE_HZ   (5)
#define SNS_MAG_CAL_MAX_MAG_SAMP_RATE_HZ   (50)
#define SNS_MAG_CAL_MIN_MAG_SAMP_RATE_Q16  \
        (FX_CONV_Q16(SNS_MAG_CAL_MIN_MAG_SAMP_RATE_HZ, 0))
#define SNS_MAG_CAL_MAX_MAG_SAMP_RATE_Q16  \
        (FX_CONV_Q16(SNS_MAG_CAL_MAX_MAG_SAMP_RATE_HZ, 0))

#define SNS_MAG_CAL_STATE_REGISTERED 1
#define SNS_MAG_CAL_STATE_STARTED    2

typedef struct
{
  int32_t sample_rate;        // Hz, Q16
} mag_cal_config_s;

typedef struct
{
  int32_t mag[MAG_CAL_COLS];  // raw magnetometer data in gauss, Q16
  uint32_t timestamp;         // sensor data timestamp
} mag_cal_input_s;

typedef struct
{
  int32_t mag_cal[MAG_CAL_COLS];                // Calibrated Mag output, Q16
  int32_t mag_cal_bias_offset[MAG_CAL_COLS];   // Bias offset output, Q16
  uint8_t accuracy;
} mag_cal_output_s;

typedef struct
{
  mag_cal_config_s config;
  int32_t algo_state;
  int32_t mag_cal[MAG_CAL_COLS];
  int32_t mag_cal_bias_offset[MAG_CAL_COLS];
} mag_cal_state_s;

/*=========================================================================
  FUNCTION:  mag_cal_sam_init
  =======================================================================*/
/*!
    @brief Initializes the SAM algorithm framework for mag cal

    This will search for relevant mag cal libraries, and use them
    if present. Otherwise, it will return an error.

    @param[i] drvUuid: UUID of mag driver
    @param[o] algoPtr: Pointer to algorithm configuration

    @return SENSOR1_SUCCESS if successful
    Error code otherwise
*/
/*=======================================================================*/
sensor1_error_e mag_cal_sam_init(
   const sns_sam_sensor_uuid_s * drvUuid,
   sns_sam_algo_s *algoPtr );

#endif /* MAG_CAL_H */
