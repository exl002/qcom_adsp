#ifndef ROTATION_VECTOR_H
#define ROTATION_VECTOR_H

/*=============================================================================
  Qualcomm Rotation Vector header file 
 
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
 * 2011-08-25  pd   initial version
 *
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"

#define ROTATION_VECTOR_COL   (4)
#define GRAVITY_COL (3)
#define MAGNETIC_COL (3)

#define ROTATION_VECTOR_ANDROID_COORD_SYS 0 // Android (ENU = East North Up)
#define ROTATION_VECTOR_SAE_COORD_SYS 1     // SAE (NED = North East Down)

#define ROTATION_VECTOR_GRAVITY_DATA  0
#define ROTATION_VECTOR_MAG_VEC_DATA  1

typedef struct
{
  float gravity[GRAVITY_COL];
  uint8_t accuracy;
  uint32_t timestamp;
} gravity_vector_t;

typedef struct
{
  float filtered_mag[MAGNETIC_COL];
  uint8_t accuracy;
  uint32_t timestamp;
} magnetic_vector_t;

typedef struct
{
  int32_t sample_rate;       // sample rate in Hz, Q16
  uint8_t coordinate_sys;    // coordinate system used in output, 0=SAE (default), 1=Android 
} rotation_vector_config_s;

typedef struct
{
  uint8_t input_type;             // 0=GRAVITY, 1=MAG_VEC   
  gravity_vector_t  gravity_vector;  
  magnetic_vector_t  mag_vector;
} rotation_vector_input_s;

typedef struct
{
  bool valid_result;
  float rotation_vector[ROTATION_VECTOR_COL];
  uint8_t accuracy;  
  uint32_t timestamp;
  uint8_t coordinate_sys;       //0=ENU(Android), 1=NED(SAE)
} rotation_vector_output_s;

typedef struct
{
  rotation_vector_config_s config;
  gravity_vector_t  gravity_vector;  
  magnetic_vector_t  mag_vector;
  bool gravity_received;
  bool mag_vector_received;
} rotation_vector_state_s;

/*=========================================================================
  FUNCTION:  rotation_vector_sam_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed by the algorithm with given configuration

    @param[i] config_data: Pointer to algorithm configuration

    @return Size of state memory required, given the configuration settings
            0 if the configuration is invalid
*/
/*=======================================================================*/
int32_t rotation_vector_sam_mem_req(const rotation_vector_config_s *config_data);

/*=========================================================================
  FUNCTION:  rotation_vector_sam_state_reset
  =======================================================================*/
/*!
    @brief Resets the algorithm instance with given configuration

    @param[i] config_data: Pointer to algorithm configuration
    @param[i] mem: Pointer to a block of allocated algorithm state memory

    @return Pointer to a valid algorithm state memory if successful
            NULL if error
*/
/*=======================================================================*/
rotation_vector_state_s* rotation_vector_sam_state_reset(const rotation_vector_config_s *config_data, void *mem);

/*=========================================================================
  FUNCTION:  rotation_vector_sam_update
  =======================================================================*/
/*!
    @brief Execute the algorithm, update the output with given input

    @param[i] rotation_vector_algo: Pointer to algorithm state memory
    @param[i] input: Pointer to algorithm input data
    @param[o] output: Pointer to algorithm output data

    @return none
*/
/*=======================================================================*/
void rotation_vector_sam_update(rotation_vector_state_s *rotation_vector_algo, 
                                const rotation_vector_input_s *input,
                                rotation_vector_output_s *output);

#endif
