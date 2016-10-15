#ifndef GAME_ROTATION_VECTOR_H
#define GAME_ROTATION_VECTOR_H

/*=============================================================================
  Qualcomm Game Rotation Vector header file

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 * 2013-05-06  tc   initial version
 *
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"

#define GAME_ROT_VEC_NUM_QUAT_ELEMS (4)
#define GAME_ROT_VEC_GRAVITY_ELEMS  (3)
#define GAME_ROT_VEC_AZIMUTH_ELEMS  (3)

/**
 * Algorithm Constants !!!Change these to tune the algorithm!!!
 */
#define GAME_ROT_VEC_ABS_REST_GYRO_AZIMUTH_TILT_TC            (3)
#define GAME_ROT_VEC_ABS_REST_GYRO_AZIMUTH_TILT_ALPHA_FACTOR  (1.0 / GAME_ROT_VEC_ABS_REST_GYRO_AZIMUTH_TILT_TC)
#define GAME_ROT_VEC_REL_REST_GYRO_AZIMUTH_TILT_TC            (7.5)
#define GAME_ROT_VEC_REL_REST_GYRO_AZIMUTH_TILT_ALPHA_FACTOR  (1.0 / GAME_ROT_VEC_REL_REST_GYRO_AZIMUTH_TILT_TC)
#define GAME_ROT_VEC_MOTION_GYRO_AZIMUTH_TILT_TC              (50)
#define GAME_ROT_VEC_MOTION_GYRO_AZIMUTH_TILT_ALPHA_FACTOR    (1.0 / GAME_ROT_VEC_MOTION_GYRO_AZIMUTH_TILT_TC)
#define GAME_ROT_VEC_GYRO_NOISE_THRESHOLD_RADIANS             (0.7 / 180 * PI)
#define GAME_ROT_VEC_MAX_GYRO_AZIMUTH_TILT_FREEZE_LIMIT       (0.9994)    //cos(2/180 * PI);
#define GAME_ROT_VEC_MAX_GYRO_AZIMUTH_TILT_RESET_LIMIT        (0.5)       //cos(60/180 * PI);
#define GAME_ROT_VEC_MAX_GRAVITY_SETTLE_PERIOD_SEC            (0.9)       //1.5 * 0.6

//#define USE_SINGLE_PRECISION_FLOAT

#ifdef USE_SINGLE_PRECISION_FLOAT
typedef float float_precision;
#else
typedef double float_precision;
#endif

typedef enum
{
   GAME_ROT_VEC_DEV_STATE_UNKNOWN=0,  // device state is unknown
   GAME_ROT_VEC_DEV_ABS_REST,         // device is in absolute rest
   GAME_ROT_VEC_DEV_REL_REST,         // device is in relative rest
   GAME_ROT_VEC_DEV_MOTION,           // device is moving
   GAME_ROT_VEC_DEV_STATES            // number of device states
} game_rot_vec_dev_state_e;

typedef enum
{
   GAME_ROT_VEC_GRAVITY_DATA,
   GAME_ROT_VEC_GYRO_QUAT_DATA
} game_rot_vec_input_e;

typedef enum
{
   GAME_ROT_VEC_ANDROID_COORD_SYS,  // Android (ENU = East North Up)
   GAME_ROT_VEC_SAE_COORD_SYS       // SAE (NED = North East Down)
} game_rot_vec_coord_sys_e;

typedef struct
{
   float gravity[GAME_ROT_VEC_GRAVITY_ELEMS];
   game_rot_vec_dev_state_e device_state;
   uint8_t accuracy;
   uint32_t timestamp;
} game_rot_vec_gravity_t;

typedef struct
{
   float quaternion[GAME_ROT_VEC_NUM_QUAT_ELEMS];
   uint32_t timestamp;
} game_rot_vec_gyro_quat_t;

typedef struct
{
   float_precision azimuth[GAME_ROT_VEC_AZIMUTH_ELEMS];
   uint32_t timestamp;
} game_rot_vec_gyro_azimuth_t;

typedef struct
{
   float_precision w;
   float_precision x;
   float_precision y;
   float_precision z;
} game_rot_vec_quat_t;

typedef struct
{
   int32_t sample_rate;      // sample rate in Hz, Q16
   uint8_t coordinate_sys;    // coordinate system, 0 = Android, 1 = SAE
} game_rot_vec_config_s;

typedef struct
{
   union
   {
      game_rot_vec_gravity_t gravity;
      game_rot_vec_gyro_quat_t gyro_quat;
   } data;
   game_rot_vec_input_e input_type;
} game_rot_vec_input_s;

typedef struct
{
   float rotation_vector[GAME_ROT_VEC_NUM_QUAT_ELEMS];
   uint8_t accuracy;
   uint32_t timestamp;
   uint8_t coordinate_sys;    // coordinate system, 0 = Android, 1 = SAE
} game_rot_vec_output_s;

typedef struct
{
   game_rot_vec_config_s  config;
   game_rot_vec_gravity_t gravity_vector;
   game_rot_vec_gyro_quat_t gyro_quaternion;
   game_rot_vec_gyro_azimuth_t  gyro_azimuth;
   float abs_rest_gyro_azimuth_tilt_alpha;
   float rel_rest_gyro_azimuth_tilt_alpha;
   float motion_gyro_azimuth_tilt_alpha;
   float gyro_noise_threshold; // gyro-quaternion noise floor adjusted for update rate
   float max_gyro_azimuth_tilt_freeze_limit;
   float max_gyro_azimuth_tilt_reset_limit;
   uint32_t min_in_rest_count;
   uint32_t in_abs_rest_count;
   uint32_t in_rel_rest_count;
} game_rot_vec_state_s;

/*=========================================================================
  FUNCTION:  game_rot_vec_sam_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed by the algorithm with given configuration

    @param[i] config_data: Pointer to algorithm configuration

    @return Size of state memory required, given the configuration settings
            0 if the configuration is invalid
*/
/*=======================================================================*/
int32_t game_rot_vec_sam_mem_req(
   const game_rot_vec_config_s *config_data);

/*=========================================================================
  FUNCTION:  game_rot_vec_sam_state_reset
  =======================================================================*/
/*!
    @brief Resets the algorithm instance with given configuration

    @param[i] config_data: Pointer to algorithm configuration
    @param[i] mem: Pointer to a block of allocated algorithm state memory

    @return Pointer to a valid algorithm state memory if successful
            NULL if error
*/
/*=======================================================================*/
game_rot_vec_state_s* game_rot_vec_sam_state_reset(
   const game_rot_vec_config_s *config_data,
   void *mem);

/*=========================================================================
  FUNCTION:  game_rot_vec_sam_update
  =======================================================================*/
/*!
    @brief Execute the algorithm, update the output with given input

    @param[i] algo: Pointer to algorithm state memory
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void game_rot_vec_sam_update(
   game_rot_vec_state_s *algo,
   const game_rot_vec_input_s *input,
   game_rot_vec_output_s *output);

#endif
