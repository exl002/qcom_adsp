#ifndef GYRO_QUAT_H
#define GYRO_QUAT_H

/*=============================================================================
  Copyright (c) 2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 * 2011-08-25  ad   initial version for qfusion
 *
 *============================================================================*/

#include <stdint.h>

#define GYRO_QUAT_ELEMS       (4)
#define GYRO_QUAT_GYRO_ELEMS  (3)

/** Quaternion configuration structure */
typedef struct
{
   int32_t sample_rate;        // sample rate in Hz, Q16
} gyro_quat_config_s;

/** Quaternion input structure */
typedef struct
{
   int32_t gyro[GYRO_QUAT_GYRO_ELEMS]; // gyro sample, radians/second, Q16
   uint32_t timestamp;                // gyro sample timestamp
} gyro_quat_input_s;

/** Quaternion output structure */
typedef struct
{
   float quat[GYRO_QUAT_ELEMS];    // quaternion output
   uint32_t timestamp;            // sensor data timestamp
} gyro_quat_output_s;

/** Quaternion state structure */
typedef struct
{
   gyro_quat_config_s config;         // quaternion algorithm configuration
   int32_t gyro[GYRO_QUAT_GYRO_ELEMS];  // angular rate, radians/second, Q16
   uint32_t timestamp;                 // gyro sample timestamp
   float quat[GYRO_QUAT_ELEMS];         // quaternion 
   float sample_period;                // instantaneous gyro sample period
} gyro_quat_state_s;

/*=========================================================================
  FUNCTION:  gyro_quat_sam_quat_reset
  =======================================================================*/
/*!
    @brief Resets the internal cumulative quaternion state

    @param[i] state: Pointer to algorithm state memory

    @return none
*/
/*=======================================================================*/
void gyro_quat_sam_quat_reset(
   gyro_quat_state_s *state);

/*=========================================================================
  FUNCTION:  gyro_quat_sam_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed by the algorithm with given configuration

    @param[i] config_data: Pointer to algorithm configuration

    @return Size of state memory required, given the configuration settings
            0 if the configuration is invalid
*/
/*=======================================================================*/
int32_t gyro_quat_sam_mem_req(
   const gyro_quat_config_s *config_data);

/*=========================================================================
  FUNCTION:  gyro_quat_sam_state_reset
  =======================================================================*/
/*!
    @brief Resets the algorithm instance with given configuration

    @param[i] config_data: Pointer to algorithm configuration
    @param[i] mem: Pointer to a block of allocated algorithm state memory

    @return Pointer to a valid algorithm state memory if successful
            NULL if error
*/
/*=======================================================================*/
gyro_quat_state_s* gyro_quat_sam_state_reset(
   const gyro_quat_config_s *config_data, 
   void *mem);

/*=========================================================================
  FUNCTION:  gyro_quat_sam_update
  =======================================================================*/
/*!
    @brief Execute the algorithm, update the output with given input

    @param[i] state: Pointer to algorithm state memory
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void gyro_quat_sam_update(
   gyro_quat_state_s *state,
   const gyro_quat_input_s *input,
   gyro_quat_output_s *output);

#endif /* GYRO_QUAT_H */


