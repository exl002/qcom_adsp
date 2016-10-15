/*============================================================================
  FILE: sns_sam_dsps_ext.c

  This file contains Sensors Algorithm Manager implementation specific to DSPS

  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 ============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/sam/framework/src/common/sns_sam_ext_dsps.c#10 $ */
/* $DateTime: 2014/07/09 16:48:51 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-06-09  ad   Update advertised min rates for fusion sensors
  2014-05-23  ad   Support fusion max rate based on the sensors used
  2014-05-13  pk   Isolate enhanced batching to SSC_BATCH_ENHANCE switch
  2014-05-12  ad   Support SMGR specified max mag sample rate for algorithms
  2014-05-12  ad   Support enhanced batching
  2014-04-11  rt   Disable initializing modem scn to prevent crash on 8084 RCM
  2014-03-20  pk   Register Fusion algos on SSC if QCompass reg query fails
  2014-03-20  pk   Set algorithm_revision to 1 for initial versions of algos
  2014-03-14  rt   Fixes sensors hang issue during modem scn client init
  2014-02-19  pk   Reduced max rates for Ped, PAM and a few gestures
  2014-02-06  pk   Changed min/max report rates for algorithms
  2014-02-05  ks   Integrated Feature Motion Classifier support
  2013-10-20  ad   Fixed klocwork errors
  2013-10-14  sc   Added support for oem_1 algo
  2013-10-10  sc   Add relative threshold support
  2013-10-03  pk   Allow disabling SAM buffering
  2013-09-26  ps   Add BRINGUP_8084 flag to disable Modem client 
  2013-09-16  pk   Enabled SAM buffering by default
  2013-09-06  dk   Increased the number of SMD tunable parameters supported
  2013-09-03  ag   Remove double free when sam_mr_send fails
  2013-08-28  ad   Enable SMD to use buffered data
  2013-07-31  hw   Fix Klocwork warning error
  2013-07-26  vh   Eliminated compiler warnings
  2013-07-11  pk   Use signals to receive AP suspend state notification from PM
  2013-06-27  gju  Remove Mag cal and use SMGR for mag instead
  2013-06-15  asj  Completed stubs for modem SCN algo
  2013-06-05  pd   Add Significant Motion Detection
  2013-06-05  tc   Added support for Game Rotation Vector
  2013-02-12  hw   Remove the Gravity vector service registration on ADSP
  2013-01-13  asj  Added Modem SCN algo
  2012-11-15  hw   Migrates Gestures Features from APPS to ADSP
  2012-11-08  sc   Merged from DSPS 1.1 mainline
  2012-10-09  hw   Changed default sensor_id in threshold algorithm to ACCEL
  2012-08-10  sc   Merged from mainline the gyrobuf algo service
  2012-08-09  ag   Updated to use official QMI service IDs
  2012-05-08  lka  Fixed (gyrobuf algorithm) the fixed-point conversion
  2012-04-11  lka  Added algorithm registration for Gyrobuf
  2011-11-21  ad   SAM registers algorithms supported by the sensors available on plaform
  2011-11-14  jhh  Updated alloc function calls to meet new API
  2011-11-04  sc   Added support for sensors threshold algorithm
  2011-09-09  sc   Update with registry service V02
  2011-07-18  ad   refactor SAM for memory optimization

 ============================================================================*/
/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <qmi_csi.h>
#include <qmi_client.h>
#include <qmi_idl_lib.h>
#include <qmi_cci_target_ext.h>

#include "sns_sam_priv.h"

#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_reg_common.h"
#include "sns_debug_str.h"
#include "sns_em.h"
#include "sns_pm_api_v01.h"

#include "qmd.h"
#include "sns_sam_amd_v01.h"
#include "sns_sam_vmd_v01.h"
#include "sns_sam_rmd_v01.h"
#include "sns_reg_api_v02.h"

#include "threshold.h"
#include "sns_sam_sensor_thresh_v01.h"
#include "sns_smgr_api_v01.h"

#include "gyrobuf.h"
#include "sns_sam_gyrobuf_v01.h"
#include "radio_frequency_radiated_performance_enhancement_v01.h"
#include "sns_sam_modem_scn_v01.h"
#include "sns_time_api_v02.h"

/* The face and shake algo interfaces */
#ifndef FEATURE_SNS_SAM_FNS
#define FEATURE_SNS_SAM_FNS
#endif
#ifdef FEATURE_SNS_SAM_FNS
#include "face_n_shake.h"
#endif /* FEATURE_SNS_SAM_FNS */

#ifndef FEATURE_SNS_SAM_BTE
#define FEATURE_SNS_SAM_BTE
#endif
#ifdef FEATURE_SNS_SAM_BTE
#include "bring_to_ear.h"
#endif /* FEATURE_SNS_SAM_BTE */

#ifndef FEATURE_SNS_SAM_BASIC_GESTURES
#define FEATURE_SNS_SAM_BASIC_GESTURES
#endif
//#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
#include "shake_gestures.h"
#include "basic_gestures.h"
//#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifndef FEATURE_SNS_SAM_TAP
#define FEATURE_SNS_SAM_TAP
#endif
#ifdef FEATURE_SNS_SAM_TAP
#include "tap.h"
#endif /* FEATURE_SNS_SAM_TAP */

#ifndef FEATURE_SNS_SAM_GYRO_TAP
#define FEATURE_SNS_SAM_GYRO_TAP
#endif
#ifdef FEATURE_SNS_SAM_GYRO_TAP
#include "gyro_tap.h"
#endif /* FEATURE_SNS_SAM_GYRO_TAP */

#define FEATURE_SNS_SAM_TILT
#ifdef FEATURE_SNS_SAM_TILT
#include "integ_angle.h"
#endif /* FEATURE_SNS_SAM_TILT */

#define FEATURE_SNS_SAM_FACING
#ifdef FEATURE_SNS_SAM_FACING
#include "facing.h"
#endif /* FEATURE_SNS_SAM_FACING */

#include "sns_sam_fns_v01.h"
#include "sns_sam_bte_v01.h"

#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
#include "sns_sam_basic_gestures_v01.h"
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_TAP
#include "sns_sam_tap_v01.h"
#endif /* FEATURE_SNS_SAM_TAP */

#ifdef FEATURE_SNS_SAM_GYRO_TAP
#include "sns_sam_gyro_tap2_v01.h"
#endif /* FEATURE_SNS_SAM_GYRO_TAP */

#ifdef FEATURE_SNS_SAM_FACING
#include "sns_sam_facing_v01.h"
#endif /* FEATURE_SNS_SAM_FACING */

#ifdef FEATURE_SNS_SAM_TILT
#include "sns_sam_integ_angle_v01.h"
#endif /* FEATURE_SNS_SAM_TILT */

#include "pedometer.h"
#include "sns_sam_ped_v01.h"

#include "pam.h"
#include "sns_sam_pam_v01.h"

#include "modem_scn.h"
#include "sns_sam_modem_scn_v01.h"

#include "cmc.h"
#include "sns_sam_cmc_v01.h"

#include "distance_bound.h"
#include "sns_sam_distance_bound_v01.h"

#include "gyro_quaternion.h"
#include "sns_sam_quaternion_v01.h"

#include "gravity.h"
#include "sns_sam_gravity_vector_v01.h"

#include "fmv.h"
#include "sns_sam_filtered_mag_v01.h"

#include "rotation_vector.h"
#include "sns_sam_rotation_vector_v01.h"

#include "orientation.h"
#include "sns_sam_orientation_v01.h"

#include "smd_algo.h"
#include "sns_sam_smd_v01.h"

#include "game_rotation_vector.h"
#include "sns_sam_game_rotation_vector_v01.h"

#include "tilt_detector.h"
#include "sns_sam_tilt_detector_v01.h"

#ifdef CONFIG_USE_OEM_1_ALGO
#include "sns_oem_1_v01.h"
#include "oem_1.h"
#endif //CONFIG_USE_OEM_1_ALGO

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_SAM_MODEM_INFO_TIMER_PERIOD_USEC 20000000

 //TODO: Move to target specific header
#ifdef SSC_BATCH_ENHANCE
   #define SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS 50
#else
   #define SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS 4
#endif

// Max sample rates
#define SNS_SAM_MAX_PED_SAMPLE_RATE_Q16       FX_CONV_Q16(50,0)
#define SNS_SAM_MAX_GESTURES_SAMPLE_RATE_Q16  FX_CONV_Q16(100,0)

// Min report rates
#define SNS_SAM_MIN_ALGO_REPORT_RATE     (1.0f/60) // 60 seconds interval
#define SNS_SAM_MIN_ALGO_REPORT_RATE_Q16 FX_FLTTOFIX_Q16(SNS_SAM_MIN_ALGO_REPORT_RATE)
#define SNS_SAM_MIN_PED_REPORT_RATE      (1.0f/3600) // 60 minute interval
#define SNS_SAM_MIN_PED_REPORT_RATE_Q16  FX_FLTTOFIX_Q16(SNS_SAM_MIN_PED_REPORT_RATE)

#define SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ (5)
#define SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16  (FX_CONV_Q16(SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ, 0))

/*---------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

// Mag Cal support on ADSP flag
static bool sns_sam_mag_cal_adsp_support_flag = true;

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static sns_em_timer_obj_t sns_sam_modem_info_timer;
static qmi_client_type    modem_scn_user_handle;

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/

/*=========================================================================
  FUNCTION:  sns_sam_reg_algo
  =========================================================================*/
/*!
  @brief Register specified algorithm

  @param[i/o] algoPtr: pointer to algorithm
  @param[i] uuids: UUIDs of detected sensors

  @return None
*/
/*=======================================================================*/
sns_err_code_e sns_sam_reg_algo(
   sns_sam_algo_s* algoPtr,
   const sns_sam_sensor_uuid_s* uuids)
{

   UNREFERENCED_PARAMETER(uuids);

   /* Cap max fusion sensor sampling rate to lower of max sample rates for accel and gyro */
   int32_t maxFusionSampleRate = sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxSampleRate;
   if (maxFusionSampleRate > sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxSampleRate)
   {
      maxFusionSampleRate = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxSampleRate;
   }

   algoPtr->disableAlgoInstReuse = FALSE;

#ifdef FEATURE_SNS_SAM_FNS
   if (SNS_SAM_FNS_SVC_ID_V01 == algoPtr->serviceId)
   {

      face_n_shake_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(face_n_shake_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (face_n_shake_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(face_n_shake_config_s);
      algoPtr->defInputDataSize = sizeof(face_n_shake_input_s);
      algoPtr->defOutputDataSize = sizeof(face_n_shake_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))face_n_shake_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))face_n_shake_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))face_n_shake_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      defCfgPtr->sample_rate            = FACE_N_SHAKE_SAMPLE_RATE_DEF_Q16;
      defCfgPtr->facing_angle_threshold = FACE_N_SHAKE_FACING_THRESH_DEF;
      defCfgPtr->basic_sleep            = FACE_N_SHAKE_SLEEP_DEF;
      defCfgPtr->basic_shake_threshold  = FACE_N_SHAKE_SHAKE_THRESHOLD_DEF;
      defCfgPtr->timeout                = FACE_N_SHAKE_TIMEOUT;
      defCfgPtr->internal_config_param1 = FACE_N_SHAKE_DEF_INT_CFG_PARAM1;
      defCfgPtr->internal_config_param2 = FACE_N_SHAKE_DEF_INT_CFG_PARAM2;
      defCfgPtr->internal_config_param3 = FACE_N_SHAKE_DEF_INT_CFG_PARAM3;
      defCfgPtr->internal_config_param4 = FACE_N_SHAKE_DEF_INT_CFG_PARAM4;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = FACE_N_SHAKE_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_sample_rate = SNS_SAM_MAX_GESTURES_SAMPLE_RATE_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   if (SNS_SAM_BTE_SVC_ID_V01 == algoPtr->serviceId)
   {

      bring_to_ear_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(bring_to_ear_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (bring_to_ear_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(bring_to_ear_config_s);
      algoPtr->defInputDataSize = sizeof(bring_to_ear_input_s);
      algoPtr->defOutputDataSize = sizeof(bring_to_ear_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))bring_to_ear_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))bring_to_ear_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))bring_to_ear_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      defCfgPtr->sample_rate            = BRING_TO_EAR_SAMPLE_RATE_DEF_Q16;
      defCfgPtr->facing_angle_threshold = BRING_TO_EAR_FACING_ANGLE_THRESH;
      defCfgPtr->horiz_angle_threshold  = BRING_TO_EAR_HORIZ_ANG;
      defCfgPtr->vert_angle_threshold   = BRING_TO_EAR_VERT_ANG;
/* TODO: supply proximity data as part of playback in future */
#ifdef ENABLE_APPS_PLAYBACK
      defCfgPtr->prox_enabled           = 0;
#else
      defCfgPtr->prox_enabled           = 1;
#endif
      defCfgPtr->internal_config_param1 = BRING_TO_EAR_DEF_INT_CFG_PARAM1;
      defCfgPtr->internal_config_param2 = BRING_TO_EAR_DEF_INT_CFG_PARAM2;
      defCfgPtr->internal_config_param3 = BRING_TO_EAR_DEF_INT_CFG_PARAM3;
      defCfgPtr->internal_config_param4 = BRING_TO_EAR_DEF_INT_CFG_PARAM4;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = BRING_TO_EAR_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_sample_rate = SNS_SAM_MAX_GESTURES_SAMPLE_RATE_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_TAP
   if (SNS_SAM_TAP_SVC_ID_V01 == algoPtr->serviceId)
   {
      tap_config_struct *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(tap_config_struct));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (tap_config_struct *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(tap_config_struct);
      algoPtr->defInputDataSize = sizeof(tap_input_struct);
      algoPtr->defOutputDataSize = sizeof(tap_output_struct);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))tap_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))tap_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))tap_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      defCfgPtr->sample_rate = TAP_MIN_SAMPLE_RATE;
      defCfgPtr->tap_threshold = TAP_THRESHOLD;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = TAP_MIN_SAMPLE_RATE;
      algoPtr->max_sample_rate = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxSampleRate;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == algoPtr->serviceId)
   {
      basic_gestures_config_struct *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(basic_gestures_config_struct));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (basic_gestures_config_struct *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(basic_gestures_config_struct);
      algoPtr->defInputDataSize = sizeof(basic_gestures_input_struct);
      algoPtr->defOutputDataSize = sizeof(basic_gestures_output_struct);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))basic_gestures_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))basic_gestures_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))basic_gestures_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      algoPtr->defSensorReportRate = SNS_REG_BASIC_GESTURES_DEF_SENSOR_REPORT_RATE_Q16;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_BASIC_GESTURES_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate            = BASIC_SAMPLE_RATE_DEF_Q16;
      defCfgPtr->sleep                  = BASIC_SLEEP_DEF;
      defCfgPtr->push_threshold         = BASIC_PUSH_THRESH_DEF;
      defCfgPtr->pull_threshold         = BASIC_PULL_THRESH_DEF;
      defCfgPtr->shake_threshold        = BASIC_SHAKE_THRESH_DEF;
      defCfgPtr->internal_config_param1 = BASIC_REL_REST_ACCEL_TIME_WINDOW;
      defCfgPtr->internal_config_param2 = BASIC_REL_REST_DETECT_THRESH_SPI;
      defCfgPtr->internal_config_param3 = BASIC_REL_REST_ACCEL_TIME_WINDOW;
      defCfgPtr->internal_config_param4 = BASIC_REL_REST_ANGLE_THRESH;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = BASIC_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_sample_rate = SNS_SAM_MAX_GESTURES_SAMPLE_RATE_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TILT
   if (SNS_SAM_INTEG_ANGLE_SVC_ID_V01 == algoPtr->serviceId)
   {
      integ_angle_config_struct *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(integ_angle_config_struct));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (integ_angle_config_struct *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(integ_angle_config_struct);
      algoPtr->defInputDataSize = sizeof(integ_angle_input_struct);
      algoPtr->defOutputDataSize = sizeof(integ_angle_output_struct);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))integ_angle_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))integ_angle_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))integ_angle_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      defCfgPtr->sample_rate = IA_MIN_GYRO_RATE_Q16;
      defCfgPtr->clip_threshold = FX_IA_RES_LIMIT;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 |
                                           SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_ALGO_REPORT_RATE_Q16;
      algoPtr->max_report_rate = IA_MAX_GYRO_RATE_Q16;
      algoPtr->min_sample_rate = IA_MIN_GYRO_RATE_Q16;
      algoPtr->max_sample_rate = IA_MAX_GYRO_RATE_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxPower;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
   if (SNS_SAM_FACING_SVC_ID_V01 == algoPtr->serviceId)
   {
      facing_config_struct *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(facing_config_struct));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (facing_config_struct *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(facing_config_struct);
      algoPtr->defInputDataSize = sizeof(facing_input_struct);
      algoPtr->defOutputDataSize = sizeof(facing_output_struct);

      algoPtr->algoApi.sns_sam_algo_mem_req =
        (int32_t(*)(void*))facing_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
        (void*(*)(void*,void*))facing_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
        (void(*)(void*,void*,void*))facing_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      algoPtr->defSensorReportRate = SNS_REG_FACING_DEF_SENSOR_REPORT_RATE_Q16;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_FACING_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = FACING_SAMPLE_RATE_DEF_Q16;
      defCfgPtr->facing_angle_threshold = FACING_THRESHOLD_DEF;
      defCfgPtr->report_neutral = FACING_REPORT_NEUTRAL;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 |
                                           SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = SNS_SAM_MAX_GESTURES_SAMPLE_RATE_Q16;
      algoPtr->min_sample_rate = FACING_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_sample_rate = SNS_SAM_MAX_GESTURES_SAMPLE_RATE_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_GYRO_TAP2_SVC_ID_V01 == algoPtr->serviceId)
   {
      gtap_config_struct *defCfgPtr;
      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
             sizeof(gtap_config_struct) * (1 + SNS_REG_MAX_GYRO_TAP_SCENARIOS));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (gtap_config_struct *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(gtap_config_struct);
      algoPtr->defInputDataSize =      sizeof(gtap_input_struct);
      algoPtr->defOutputDataSize =     sizeof(gtap_output_struct);
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_GYRO_TAP_V02;

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))gtap_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))gtap_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))gtap_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      defCfgPtr->scenario                = SNS_REG_GYRO_INVALID_SCENARIO;
      defCfgPtr->tap_time_win_secs       = GTAP_TIME_WIN_SECS;
      defCfgPtr->tap_time_sleep_secs     = GTAP_TIME_SLEEP_SECS;
      defCfgPtr->acc_tap_thr             = GTAP_ACCEL_THRESHOLD;
      defCfgPtr->lr_accel_jerk_min_thr   = GTAP_LR_ACCEL_JERK_MIN_THRESH;
      defCfgPtr->tb_accel_jerk_min_thr   = GTAP_TB_ACCEL_JERK_MIN_THRESH;
      defCfgPtr->jerk_win_secs           = GTAP_JERK_WIN_SECS;
      defCfgPtr->lr_accel_rat_jerk_yx    = GTAP_LR_ACCEL_RAT_JERK_YX;
      defCfgPtr->tb_accel_rat_jerk_xy    = GTAP_TB_ACCEL_RAT_JERK_XY;
      defCfgPtr->accel_z_thr             = GTAP_TB_ACCEL_Z_THRESH;
      defCfgPtr->ori_check_win_secs      = GTAP_ORI_CHECK_WIN_SECS;
      defCfgPtr->ori_check_int_secs      = GTAP_ORI_CHNG_INT_SECS;
      defCfgPtr->ori_check_thr           = GTAP_ORI_CHNG_THR;
      defCfgPtr->z_anamoly_inc           = GTAP_Z_ANAMOLY_INC;
      defCfgPtr->stress_top_bottom       = GTAP_STRESS_TOP_BOTTOM;
      defCfgPtr->strong_z_anamoly_tb     = GTAP_STRONG_Z_ANAMOLY_TB;
      defCfgPtr->stress_right_left       = GTAP_STRESS_RIGHT_LEFT;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = GTAP_MIN_SAMPLE_RATE_Q16;
      algoPtr->max_sample_rate = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxSampleRate;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
#endif // FEATURE_SNS_SAM_GYRO_TAP

   if (algoPtr->serviceId == SNS_SAM_AMD_SVC_ID_V01 ||
       algoPtr->serviceId == SNS_SAM_VMD_SVC_ID_V01 ||
       algoPtr->serviceId == SNS_SAM_RMD_SVC_ID_V01)
   {
      qmd_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(qmd_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (qmd_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(qmd_config_s);
      algoPtr->defInputDataSize = sizeof(qmd_input_s);
      algoPtr->defOutputDataSize = sizeof(qmd_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))qmd_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))qmd_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))qmd_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;

      switch(algoPtr->serviceId)
      {
      case SNS_SAM_AMD_SVC_ID_V01:
         algoPtr->defSensorReportRate = SNS_REG_AMD_DEF_SENSOR_REPORT_RATE_Q16;
         algoPtr->regItemId = SNS_REG_SAM_GROUP_AMD_V02;
         algoPtr->min_sample_rate = SNS_REG_AMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         algoPtr->max_sample_rate = SNS_REG_AMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         defCfgPtr->enable_abs = true;
         defCfgPtr->enable_veh = false;
         defCfgPtr->enable_rel = false;
         defCfgPtr->sample_rate = SNS_REG_AMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         defCfgPtr->internal_config_param1 = SNS_REG_AMD_DEF_INT_CFG_PARAM1;
         defCfgPtr->internal_config_param2 = SNS_REG_AMD_DEF_INT_CFG_PARAM2;
         break;
      case SNS_SAM_VMD_SVC_ID_V01:
         algoPtr->regItemId = SNS_REG_SAM_GROUP_VMD_V02;
         algoPtr->min_sample_rate = SNS_REG_VMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         algoPtr->max_sample_rate = SNS_REG_VMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         defCfgPtr->enable_abs = false;
         defCfgPtr->enable_veh = true;
         defCfgPtr->enable_rel = false;
         defCfgPtr->sample_rate = SNS_REG_VMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         defCfgPtr->internal_config_param1 = SNS_REG_VMD_DEF_INT_CFG_PARAM1;
         defCfgPtr->internal_config_param2 = SNS_REG_VMD_DEF_INT_CFG_PARAM2;
         break;
      case SNS_SAM_RMD_SVC_ID_V01:
         algoPtr->defSensorReportRate = SNS_REG_RMD_DEF_SENSOR_REPORT_RATE_Q16;
         algoPtr->regItemId = SNS_REG_SAM_GROUP_RMD_V02;
         algoPtr->min_sample_rate = SNS_REG_RMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         algoPtr->max_sample_rate = SNS_REG_RMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         defCfgPtr->enable_abs = false;
         defCfgPtr->enable_veh = false;
         defCfgPtr->enable_rel = true;
         defCfgPtr->sample_rate = SNS_REG_RMD_DEF_ACC_SAMP_RATE_HZ_Q16;
         defCfgPtr->internal_config_param1 = SNS_REG_RMD_DEF_INT_CFG_PARAM1;
         defCfgPtr->internal_config_param2 = SNS_REG_RMD_DEF_INT_CFG_PARAM2;
         defCfgPtr->internal_config_param3 = SNS_REG_RMD_DEF_INT_CFG_PARAM3;
         defCfgPtr->internal_config_param4 = SNS_REG_RMD_DEF_INT_CFG_PARAM4;
         break;
      default:
         SNS_OS_FREE(algoPtr->defConfigData.memPtr);
         algoPtr->defConfigData.memPtr = NULL;
         algoPtr->defConfigData.memSize = 0;
         SNS_SAM_DEBUG1(ERROR, DBG_SAM_REG_ALGO_DFLT_ERR, algoPtr->serviceId);
         return SNS_ERR_FAILED;
      }

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 |
                                           SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = SNS_REG_AMD_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      threshold_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(threshold_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (threshold_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(threshold_config_s);
      algoPtr->defInputDataSize = sizeof(threshold_input_s);
      algoPtr->defOutputDataSize = sizeof(threshold_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))threshold_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))threshold_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))threshold_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_NONE;

      /* set 10Hz accel as default */
      defCfgPtr->sensor_id = SNS_SMGR_ID_ACCEL_V01;
      defCfgPtr->data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
      defCfgPtr->num_axis = 3;
      defCfgPtr->sample_rate = FX_CONV_Q16(10, 0);
      defCfgPtr->report_period = 0;
      defCfgPtr->thresh[0] = 0xFFFFFFFF;  /* Max threshold*/
      defCfgPtr->thresh[1] = 0xFFFFFFFF;
      defCfgPtr->thresh[2] = 0xFFFFFFFF;
      defCfgPtr->threshold_type = (uint8_t)SNS_SAM_SENSOR_ABSOLUTE_THRESHOLD_V01;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = 0;
      algoPtr->max_sample_rate = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxSampleRate;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (algoPtr->serviceId == SNS_OEM_1_SVC_ID_V01)
   {
      oem_1_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(oem_1_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (oem_1_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(oem_1_config_s);
      algoPtr->defInputDataSize = sizeof(oem_1_input_s);
      algoPtr->defOutputDataSize = sizeof(oem_1_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))oem_1_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))oem_1_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))oem_1_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_NONE;

      /* set default values */
      defCfgPtr->sample_rate = 1<<16; /*1Hz as default*/
      defCfgPtr->internal_config_param1 = 0;

      return SNS_SUCCESS;
   }
#endif //CONFIG_USE_OEM_1_ALGO  
   else if (algoPtr->serviceId == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      modem_scn_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(modem_scn_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (modem_scn_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(modem_scn_config_s);
      algoPtr->defInputDataSize = sizeof(modem_scn_input_s);
      algoPtr->defOutputDataSize = sizeof(modem_scn_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))modem_scn_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))modem_scn_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))modem_scn_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_NONE;

      /* set 1Hz PRX as default */
      defCfgPtr->sample_rate = FX_CONV_Q16(1, 0);
      defCfgPtr->quiet_period = MODEM_SCN_DEF_QUIET_PERIOD;
      defCfgPtr->thresh = MODEM_SCN_DEF_THRESH;  /* Max threshold*/

      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      gyrobuf_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(gyrobuf_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (gyrobuf_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(gyrobuf_config_s);
      algoPtr->defInputDataSize = sizeof(gyrobuf_input_s);
      algoPtr->defOutputDataSize = sizeof(gyrobuf_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))gyrobuf_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))gyrobuf_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))gyrobuf_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_NONE;

      /* Default values */
      defCfgPtr->sample_rate   = FX_CONV_Q16(100, 0);
      defCfgPtr->extra_sample  = 1;

      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_PED_SVC_ID_V01)
   {
      ped_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(ped_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (ped_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(ped_config_s);
      algoPtr->defInputDataSize = sizeof(ped_input_s);
      algoPtr->defOutputDataSize = sizeof(ped_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void *))ped_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))ped_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))ped_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client =
         (bool(*)(uint8_t,bool,void*,void*,uint32_t))ped_sam_register_client;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats =
         (bool(*)(uint8_t,void*,void*))ped_sam_reset_client_stats;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change =
         (void(*)(bool,void*,void*,uint32_t))ped_sam_handle_duty_cycle_state_change;

      algoPtr->defSensorReportRate = SNS_REG_PED_DEF_SENSOR_REPORT_RATE_Q16;

      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId = SNS_REG_SAM_GROUP_PED_V02;

      /*Default Values*/
      defCfgPtr->sample_rate = PED_SAMPLE_RATE_DEF_Q16;
      defCfgPtr->step_count_threshold = PED_STEP_COUNT_THRESHOLD_DEF;
      defCfgPtr->step_prob_threshold = SNS_REG_PED_DEF_STEP_PROB_THRESHOLD;
      defCfgPtr->step_threshold = SNS_REG_PED_DEF_STEP_THRESHOLD;
      defCfgPtr->swing_threshold = SNS_REG_PED_DEF_SWING_THRESHOLD;

      //setup algorithm dependency
      algoPtr->algoDepDbase[0] = SNS_SAM_AMD_SVC_ID_V01;
      algoPtr->algoDepCount = 1;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 |
                                           SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_PED_REPORT_RATE_Q16;
      algoPtr->max_report_rate = SNS_REG_PED_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->min_sample_rate = PED_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_sample_rate = SNS_SAM_MAX_PED_SAMPLE_RATE_Q16;
      algoPtr->max_batch_size = SNS_SAM_PED_MAX_ITEMS_IN_BATCH_V01 *
                                SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_PAM_SVC_ID_V01)
   {
      pam_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(pam_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (pam_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(pam_config_s);
      algoPtr->defInputDataSize = sizeof(pam_input_s);
      algoPtr->defOutputDataSize = sizeof(pam_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void *))pam_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))pam_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))pam_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_PAM_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = PAM_DEF_SAMPLE_RATE_HZ;
      defCfgPtr->measurement_period = PAM_DEF_MEASUREMENT_PERIOD_SECS;
      defCfgPtr->step_count_threshold = PAM_DEF_STEP_COUNT_THRESHOLD;
      defCfgPtr->dutycycleOnPercent = PAM_DEF_DUTY_CYCLE_ON_PERCENTAGE;

      /*Dependencies*/
      algoPtr->algoDepDbase[0] = SNS_SAM_AMD_SVC_ID_V01;
      algoPtr->algoDepDbase[1] = SNS_SAM_PED_SVC_ID_V01;
      algoPtr->algoDepCount = 2;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 |
                                           SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = SNS_REG_PED_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->min_sample_rate = FX_CONV_Q16(PAM_DEF_SAMPLE_RATE_HZ, 0);
      algoPtr->max_sample_rate = SNS_SAM_MAX_PED_SAMPLE_RATE_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_CMC_SVC_ID_V01)
   {
      cmc_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(cmc_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (cmc_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(cmc_config_s);
      algoPtr->defInputDataSize = sizeof(cmc_input_s);
      algoPtr->defOutputDataSize = sizeof(cmc_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void *))cmc_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))cmc_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))cmc_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client = NULL;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      algoPtr->defSensorReportRate = SNS_REG_CMC_DEF_SENSOR_REPORT_RATE_Q16;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_CMC_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = CMC_SAMPLE_RATE_DEF_Q16;
      defCfgPtr->accel_on = CMC_ACCEL_INITIAL_STATE;

      //setup algorithm dependency
      algoPtr->algoDepDbase[0] = SNS_SAM_AMD_SVC_ID_V01;
      algoPtr->algoDepCount = 1;

      algoPtr->algorithm_revision = 2;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = CMC_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_sample_rate = CMC_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_batch_size = SNS_SAM_CMC_MAX_ITEMS_IN_BATCH_V01 *
                                SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS;;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      distance_bound_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof(distance_bound_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (distance_bound_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(distance_bound_config_s);
      algoPtr->defInputDataSize = sizeof(distance_bound_input_s);
      algoPtr->defOutputDataSize = sizeof(distance_bound_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void *))distance_bound_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))distance_bound_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))distance_bound_sam_update;
      algoPtr->algoApi.sns_sam_algo_register_client =
         (bool(*)(uint8_t,bool,void*,void*,uint32_t))distance_bound_sam_register_client;
      algoPtr->algoApi.sns_sam_algo_reset_client_stats = NULL;
      algoPtr->algoApi.sns_sam_algo_handle_duty_cycle_state_change = NULL;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_DISTANCE_BOUND_V02;

      /* Initialize default config */
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_UNKNOWN_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_UNKNOWN;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_STATIONARY_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_STATIONARY;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_INMOTION_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_INMOTION;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_FIDDLE_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_FIDDLE;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_PEDESTRIAN_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_PEDESTRIAN;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_VEHICLE_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_VEHICLE;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_WALK_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_WALK;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_RUN_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_RUN;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_BIKE_V01] =
         SNS_REG_DISTANCE_BOUND_DEF_SPEEDBOUNDS_BIKE;

      //setup algorithm dependency
      algoPtr->algoDepDbase[0] = SNS_SAM_CMC_SVC_ID_V01;
      algoPtr->algoDepCount = 1;

      // Force a new Distance Bound algorithm instance for each new client
      algoPtr->disableAlgoInstReuse = TRUE;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ONE_SHOT_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = CMC_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_sample_rate = CMC_SAMPLE_RATE_DEF_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == algoPtr->serviceId)
   {
      gyro_quat_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(gyro_quat_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (gyro_quat_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(gyro_quat_config_s);
      algoPtr->defInputDataSize = sizeof(gyro_quat_input_s);
      algoPtr->defOutputDataSize = sizeof(gyro_quat_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))gyro_quat_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))gyro_quat_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))gyro_quat_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_QUATERNION_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->dataSyncRequired = true;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_QUATERNION_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_ALGO_REPORT_RATE_Q16;
      algoPtr->max_report_rate = sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxSampleRate;
      algoPtr->min_sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_sample_rate = sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxSampleRate;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxPower;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == algoPtr->serviceId)
   {
      gravity_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(gravity_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (gravity_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(gravity_config_s);
      algoPtr->defInputDataSize = sizeof(gravity_input_s);
      algoPtr->defOutputDataSize = sizeof(gravity_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))gravity_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))gravity_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))gravity_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_GRAVITY_VECTOR_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->dataSyncRequired = true;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_GRAVITY_VECTOR_PARAMS_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      defCfgPtr->filter_tc[GRAVITY_DEV_STATE_UNKNOWN] =
         SNS_REG_DEF_GRAVITY_VECTOR_INT_CFG_PARAM1;
      defCfgPtr->filter_tc[GRAVITY_DEV_ABS_REST] =
         SNS_REG_DEF_GRAVITY_VECTOR_INT_CFG_PARAM2;
      defCfgPtr->filter_tc[GRAVITY_DEV_REL_REST] =
         SNS_REG_DEF_GRAVITY_VECTOR_INT_CFG_PARAM3;
      defCfgPtr->filter_tc[GRAVITY_DEV_MOTION] =
         SNS_REG_DEF_GRAVITY_VECTOR_INT_CFG_PARAM4;

      defCfgPtr->accel_outage_period = GRAVITY_ACCEL_OUTAGE_PERIOD;
      defCfgPtr->quat_outage_period = GRAVITY_QUAT_OUTAGE_PERIOD;

      defCfgPtr->motion_detect_period =
         (float) FX_FIXTOFLT_Q16(SNS_REG_AMD_DEF_INT_CFG_PARAM2);
      //[Q16m/s2 Std Dev]=>[m/s2 Var]
      defCfgPtr->abs_rest_var_thold =
         (float)( FX_FIXTOFLT_Q16(SNS_REG_AMD_DEF_INT_CFG_PARAM1) *
                  FX_FIXTOFLT_Q16(SNS_REG_AMD_DEF_INT_CFG_PARAM1) );
      defCfgPtr->rel_rest_var_thold = SNS_REG_DEF_GRAVITY_VECTOR_INT_CFG_PARAM5;

      //setup algorithm dependency
      algoPtr->algoDepDbase[0] = SNS_SAM_QUATERNION_SVC_ID_V01;
      algoPtr->algoDepCount = 1;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_ALGO_REPORT_RATE_Q16;
      algoPtr->max_report_rate = maxFusionSampleRate;
      algoPtr->min_sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_sample_rate = maxFusionSampleRate;
      algoPtr->max_batch_size = SNS_SAM_GRAVITY_MAX_REPORTS_IN_BATCH_V01 *
                                SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower +
                       sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxPower;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == algoPtr->serviceId)
   {
      fmv_config_s *defCfgPtr;

      SNS_SAM_PRINTF1(LOW, "sns_sam_reg_algo: Registering algo service %d",SNS_SAM_FILTERED_MAG_SVC_ID_V01);
      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(fmv_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (fmv_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(fmv_config_s);
      algoPtr->defInputDataSize = sizeof(fmv_input_s);
      algoPtr->defOutputDataSize = sizeof(fmv_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))fmv_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))fmv_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))fmv_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_FMV_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->dataSyncRequired = true;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_FMV_PARAMS_V02;

      /* Initialize default config */
      defCfgPtr->fmv_tcs[0] = SNS_REG_DEF_FMV_TC_ACCURACY_0;
      defCfgPtr->fmv_tcs[1] = SNS_REG_DEF_FMV_TC_ACCURACY_1;
      defCfgPtr->fmv_tcs[2] = SNS_REG_DEF_FMV_TC_ACCURACY_2;
      defCfgPtr->fmv_tcs[3] = SNS_REG_DEF_FMV_TC_ACCURACY_3;
      defCfgPtr->max_gyro_sample_gap_tolerance = SNS_REG_DEF_FMV_GYRO_GAP_THRESH;
      defCfgPtr->max_mag_innovation = SNS_REG_DEF_FMV_MAX_MAG_INNOV;
      defCfgPtr->max_mag_sample_gap_factor = SNS_REG_DEF_FMV_MAG_GAP_FACTOR;
      defCfgPtr->min_turn_rate = SNS_REG_DEF_FMV_ROT_FOR_ZUPT;
      defCfgPtr->sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      defCfgPtr->mag_max_sample_rate =
         sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxSampleRate >> 16;
      defCfgPtr->abs_rest_var_thold = SNS_REG_DEF_GYRO_AMD_INT_CFG_PARAM1;
      defCfgPtr->rel_rest_var_thold = SNS_REG_DEF_GYRO_AMD_INT_CFG_PARAM3;
      defCfgPtr->motion_detect_period = SNS_REG_DEF_GYRO_AMD_INT_CFG_PARAM2;
      defCfgPtr->mag_cal_lat_num_samps = SNS_REG_DEF_MAG_CAL_LAT_NUM_SAMPLES;

      /* dependency on Quat service */
      algoPtr->algoDepDbase[0] = SNS_SAM_QUATERNION_SVC_ID_V01;
      algoPtr->algoDepCount = 1;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_ALGO_REPORT_RATE_Q16;
      algoPtr->max_report_rate = maxFusionSampleRate;
      algoPtr->min_sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_sample_rate = maxFusionSampleRate;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxPower + 
                       sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxPower;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == algoPtr->serviceId)
   {
      rotation_vector_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(rotation_vector_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (rotation_vector_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(rotation_vector_config_s);
      algoPtr->defInputDataSize = sizeof(rotation_vector_input_s);
      algoPtr->defOutputDataSize = sizeof(rotation_vector_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))rotation_vector_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))rotation_vector_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))rotation_vector_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_ROTATION_VECTOR_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->dataSyncRequired = true;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId = SNS_REG_SAM_GROUP_ROTATION_VECTOR_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      defCfgPtr->coordinate_sys = ROTATION_VECTOR_ANDROID_COORD_SYS;

      /* dependency on GRAVITY/MAG VEC services */
      algoPtr->algoDepDbase[0] = SNS_SAM_FILTERED_MAG_SVC_ID_V01;
      algoPtr->algoDepDbase[1] = SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01;
      algoPtr->algoDepCount = 2;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_ALGO_REPORT_RATE_Q16;
      algoPtr->max_report_rate = maxFusionSampleRate;
      algoPtr->min_sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_sample_rate = maxFusionSampleRate;
      algoPtr->max_batch_size = SNS_SAM_ROTATION_VECTOR_MAX_REPORTS_IN_BATCH_V01 *
                                SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower +
                       sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxPower +
                       sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxPower;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == algoPtr->serviceId)
   {
      orientation_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(orientation_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (orientation_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(orientation_config_s);
      algoPtr->defInputDataSize = sizeof(orientation_input_s);
      algoPtr->defOutputDataSize = sizeof(orientation_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))orientation_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))orientation_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))orientation_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_ORIENTATION_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->dataSyncRequired = true;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_ORIENTATION_PARAMS_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      defCfgPtr->coordinate_sys = ORIENTATION_DEFAULT_COORD_SYS;

      defCfgPtr->accel_filter_tc = SNS_REG_DEF_ORIENTATION_INT_CFG_PARAM1;
      defCfgPtr->mag_filter_tc = SNS_REG_DEF_MAG_CAL_LAT_NUM_SAMPLES;

      defCfgPtr->motion_detect_period =
         FX_FIXTOFLT_Q16(SNS_REG_AMD_DEF_INT_CFG_PARAM2);
      //[Q16m/s2 Std Dev]=>[m/s2 Var]
      defCfgPtr->abs_rest_var_thold =
         (float)(FX_FIXTOFLT_Q16(SNS_REG_AMD_DEF_INT_CFG_PARAM1) *
                 FX_FIXTOFLT_Q16(SNS_REG_AMD_DEF_INT_CFG_PARAM1));

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_ALGO_REPORT_RATE_Q16;
      algoPtr->max_report_rate = sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxSampleRate;
      algoPtr->min_sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_sample_rate = sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxSampleRate;
      algoPtr->max_batch_size = SNS_SAM_ORIENTATION_MAX_REPORTS_IN_BATCH_V01 *
                                SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower +
                       sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxPower;
      return SNS_SUCCESS;
   }

   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == algoPtr->serviceId)
   {
      game_rot_vec_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr =
         SNS_OS_MALLOC(SNS_SAM_DBG_MOD, sizeof( game_rot_vec_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = ( game_rot_vec_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof( game_rot_vec_config_s);
      algoPtr->defInputDataSize = sizeof( game_rot_vec_input_s);
      algoPtr->defOutputDataSize = sizeof( game_rot_vec_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*)) game_rot_vec_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*)) game_rot_vec_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*)) game_rot_vec_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_GAME_ROT_VEC_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->dataSyncRequired = true;

      /* Define registry items */
      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId   = SNS_REG_SAM_GROUP_GAME_ROTATION_VECTOR_V02;

      /* Initialize default config */
      defCfgPtr->sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      defCfgPtr->coordinate_sys = GAME_ROT_VEC_ANDROID_COORD_SYS;

      /* dependency on Gravity and Gyro-Quaternion services */
      algoPtr->algoDepDbase[0] = SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01;
      algoPtr->algoDepDbase[1] = SNS_SAM_QUATERNION_SVC_ID_V01;
      algoPtr->algoDepCount = 2;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_PERIODIC_REPORT_V01 | SNS_SAM_SYNC_REPORT_V01;
      algoPtr->min_report_rate = SNS_SAM_MIN_ALGO_REPORT_RATE_Q16;
      algoPtr->max_report_rate = maxFusionSampleRate;
      algoPtr->min_sample_rate = SNS_SAM_QFUSION_MIN_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_sample_rate = maxFusionSampleRate;
      algoPtr->max_batch_size = SNS_SAM_GAME_ROTATION_VECTOR_MAX_REPORTS_IN_BATCH_V01 *
                                SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower +
                       sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->maxPower;
      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_SMD_SVC_ID_V01)
   {
      smd_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(smd_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (smd_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(smd_config_s);
      algoPtr->defInputDataSize = sizeof(smd_input_s);
      algoPtr->defOutputDataSize = sizeof(smd_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))smd_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))smd_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))smd_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_SMD_DEF_SENSOR_REPORT_RATE_HZ_Q16;

      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId = SNS_REG_SAM_GROUP_SMD_V02;

      /* Default values */
      defCfgPtr->sample_rate   = SNS_REG_SMD_DEF_SAMPLE_RATE_HZ_Q16;
      defCfgPtr->accel_window_time = SNS_REG_SMD_DEF_ACCEL_WINDOW_TIME;
      defCfgPtr->detect_threshold = SNS_REG_SMD_DEF_DETECT_THRESH;
      defCfgPtr->self_transition_prob_sm = SNS_REG_SMD_DEF_SELF_TRANS_PROB_SM;
      defCfgPtr->variable_decision_latency = SNS_REG_SMD_DEF_VAR_DECISION_LATENCY;
      defCfgPtr->max_latency = SNS_REG_SMD_DEF_MAX_LATENCY;
      defCfgPtr->accel_norm_std_thresh = SNS_REG_SMD_DEF_ACC_NORM_STD_DEV_THRESH;
      defCfgPtr->eigen_thresh = SNS_REG_SMD_DEF_EIGEN_THRESH;
      defCfgPtr->step_count_thresh = SNS_REG_SMD_DEF_STEP_COUNT_THRESH;
      defCfgPtr->step_window_time = SNS_REG_SMD_DEF_STEP_WINDOW_TIME;

      //setup algorithm dependency
      algoPtr->algoDepDbase[0] = SNS_SAM_AMD_SVC_ID_V01;
      algoPtr->algoDepDbase[1] = SNS_SAM_PED_SVC_ID_V01;
      algoPtr->algoDepCount = 2;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = SNS_REG_SMD_DEF_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_sample_rate = SNS_REG_SMD_DEF_SAMPLE_RATE_HZ_Q16;
      algoPtr->max_batch_size = 0;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
   else if (algoPtr->serviceId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01)
   {
      tilt_config_s *defCfgPtr;

      algoPtr->defConfigData.memPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,sizeof(tilt_config_s));
      SNS_ASSERT(algoPtr->defConfigData.memPtr != NULL);
      defCfgPtr = (tilt_config_s *)(algoPtr->defConfigData.memPtr);

      algoPtr->defConfigData.memSize = sizeof(tilt_config_s);
      algoPtr->defInputDataSize = sizeof(tilt_input_s);
      algoPtr->defOutputDataSize = sizeof(tilt_output_s);

      algoPtr->algoApi.sns_sam_algo_mem_req =
         (int32_t(*)(void*))tilt_sam_mem_req;
      algoPtr->algoApi.sns_sam_algo_reset =
         (void*(*)(void*,void*))tilt_sam_state_reset;
      algoPtr->algoApi.sns_sam_algo_update =
         (void(*)(void*,void*,void*))tilt_sam_update;

      algoPtr->defSensorReportRate = SNS_REG_TILT_DETECTOR_DEF_SENSOR_REPORT_RATE_Q16;

      algoPtr->regItemType = SNS_SAM_REG_ITEM_TYPE_GROUP;
      algoPtr->regItemId = SNS_REG_SAM_GROUP_TILT_DETECTOR_V02;

      /* Default values */
      defCfgPtr->sample_rate   = (float) SNS_REG_TILT_DETECTOR_DEF_SAMPLE_RATE;
      defCfgPtr->accel_window_time = (float) SNS_REG_TILT_DETECTOR_DEF_ACCEL_WINDOW_TIME;
      defCfgPtr->angle_threshold = (float) SNS_REG_TILT_DETECTOR_DEF_TILT_ANGLE_THRESH_RAD;
      defCfgPtr->init_accel_window_time = (float) SNS_REG_TILT_DETECTOR_DEF_INIT_ACCEL_WINDOW_TIME;

      //setup algorithm dependency
      algoPtr->algoDepDbase[0] = SNS_SAM_AMD_SVC_ID_V01;
      algoPtr->algoDepCount = 1;

      algoPtr->algorithm_revision = 1;
      algoPtr->supported_reporting_modes = SNS_SAM_ASYNC_REPORT_V01;
      algoPtr->min_report_rate = 0;
      algoPtr->max_report_rate = 0;
      algoPtr->min_sample_rate = SNS_REG_TILT_DETECTOR_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->max_sample_rate = SNS_REG_TILT_DETECTOR_DEF_SENSOR_REPORT_RATE_Q16;
      algoPtr->max_batch_size = SNS_SAM_TILT_DETECTOR_MAX_ITEMS_IN_BATCH_V01 *
                                SNS_SAM_MAX_BACK_TO_BACK_BATCH_INDS;
      algoPtr->power = sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->maxPower;
      return SNS_SUCCESS;
   }
   else
   {
      SNS_SAM_DEBUG1(ERROR, DBG_SAM_REG_ALGO_DFLT_ERR, algoPtr->serviceId);
      return SNS_ERR_FAILED;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_reg_algos
  =========================================================================*/
/*!
  @brief Register all algorithms

  @return None
*/
/*=======================================================================*/
void sns_sam_reg_algos(void)
{
   if (sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->valid)
   {

#ifdef FEATURE_SNS_SAM_FNS
      sns_sam_reg_algo_svc(SNS_SAM_FNS_SVC_ID_V01);
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
      sns_sam_reg_algo_svc(SNS_SAM_BTE_SVC_ID_V01);
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
      sns_sam_reg_algo_svc(SNS_SAM_BASIC_GESTURES_SVC_ID_V01);
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
      sns_sam_reg_algo_svc(SNS_SAM_TAP_SVC_ID_V01);
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_FACING
      sns_sam_reg_algo_svc(SNS_SAM_FACING_SVC_ID_V01);
#endif /* FEATURE_SNS_SAM_FACING */

      sns_sam_reg_algo_svc(SNS_SAM_AMD_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_RMD_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_VMD_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_PED_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_PAM_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_CMC_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_DISTANCE_BOUND_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_SMD_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_TILT_DETECTOR_SVC_ID_V01);
   }

   sns_sam_reg_algo_svc(SNS_SAM_SENSOR_THRESH_SVC_ID_V01);

   if (sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->valid)
   {
      sns_sam_reg_algo_svc(SNS_SAM_QUATERNION_SVC_ID_V01);
      sns_sam_reg_algo_svc(SNS_SAM_GYROBUF_SVC_ID_V01);
#ifdef FEATURE_SNS_SAM_TILT
      sns_sam_reg_algo_svc(SNS_SAM_INTEG_ANGLE_SVC_ID_V01);
#endif /* FEATURE_SNS_SAM_TILT */
   }

   if (sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->valid &&
       sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->valid)
   {
      if(sns_sam_mag_cal_adsp_support_flag)
      {
         sns_sam_reg_algo_svc(SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01);
         SNS_SAM_PRINTF2(MED, "sns_sam_reg_algos: svc_num: %d, sam_module: %d",
                         SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01,SNS_SAM_MODULE);
         sns_sam_reg_algo_svc(SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01);
         SNS_SAM_PRINTF2(MED, "sns_sam_reg_algos: svc_num: %d, sam_module: %d",
                         SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01,SNS_SAM_MODULE);
      }
#ifdef FEATURE_SNS_SAM_GYRO_TAP
      sns_sam_reg_algo_svc(SNS_SAM_GYRO_TAP2_SVC_ID_V01);
#endif // FEATURE_SNS_SAM_GYRO_TAP
   }

   if (sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->valid &&
       sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->valid)
   {
      if(sns_sam_mag_cal_adsp_support_flag)
      {
         sns_sam_reg_algo_svc(SNS_SAM_ORIENTATION_SVC_ID_V01);
         SNS_SAM_PRINTF2(MED, "sns_sam_reg_algos: svc_num: %d, sam_module: %d",
                         SNS_SAM_ORIENTATION_SVC_ID_V01,SNS_SAM_MODULE);
      }
   }

   if (sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->valid &&
       sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->valid)
   {
      if(sns_sam_mag_cal_adsp_support_flag)
      {
         sns_sam_reg_algo_svc(SNS_SAM_FILTERED_MAG_SVC_ID_V01);
         SNS_SAM_PRINTF2(MED, "sns_sam_reg_algos: svc_num: %d, sam_module: %d",
                         SNS_SAM_FILTERED_MAG_SVC_ID_V01,SNS_SAM_MODULE);
      }
   }

   if (sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->valid &&
       sns_sam_sensor_info_dbase_acc(SNS_SAM_GYRO)->valid &&
       sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->valid)
   {
      if(sns_sam_mag_cal_adsp_support_flag)
      {
         sns_sam_reg_algo_svc(SNS_SAM_ROTATION_VECTOR_SVC_ID_V01);
         SNS_SAM_PRINTF2(MED, "sns_sam_reg_algos: svc_num: %d, sam_module: %d",
                         SNS_SAM_ROTATION_VECTOR_SVC_ID_V01,SNS_SAM_MODULE);
      }
   }

#ifdef CONFIG_USE_OEM_1_ALGO   
   // This sample OEM1 algorithm will be started only if the platform has Accel
   // OEMs can modify it based on the algorithm usage   
   if (sns_sam_sensor_info_dbase_acc(SNS_SAM_ACCEL)->valid) 
   {
      sns_sam_reg_algo_svc(SNS_OEM_1_SVC_ID_V01);  
   }
#endif //CONFIG_USE_OEM_1_ALGO

//   sns_sam_reg_algo_svc(SNS_SAM_MODEM_SCN_SVC_ID_V01);
}

/*=========================================================================
  FUNCTION:  sns_sam_process_reg_data
  =========================================================================*/
/*!
  @brief Process data received from registry

  @param[i] regItemType - registry item type
  @param[i] regItemId - registry item identifier
  @param[i] regDataLen - registry data length
  @param[i] regDataPtr - registry data pointer

  @return None
*/
/*=======================================================================*/
void sns_sam_process_reg_data(
   sns_sam_reg_item_type_e regItemType,
   uint16_t regItemId,
   uint32_t regDataLen,
   const uint8_t* regDataPtr)
{
   sns_sam_algo_s* algoPtr = NULL;
   static int32_t reg_amd_stddev_thresh, reg_amd_det_period;
   static float reg_gyro_amd_abs_rest_thresh, reg_gyro_amd_rel_rest_thresh;
   static float reg_gyro_amd_det_period;

   switch(regItemId)
   {
   case SNS_REG_SAM_GROUP_AMD_V02:
   {
      qmd_config_s *defCfgPtr;
      sns_reg_amd_data_group_s *dataPtr = (sns_reg_amd_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_AMD_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_AMD_SVC_ID_V01);
         return;
      }
      defCfgPtr = (qmd_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->sample_rate = dataPtr->def_acc_samp_rate;
      defCfgPtr->internal_config_param1 = dataPtr->int_cfg_param1;
      defCfgPtr->internal_config_param2 = dataPtr->int_cfg_param2;
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_AMD_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = dataPtr->sensor_report_rate;
      }
      reg_amd_stddev_thresh = dataPtr->int_cfg_param1; // Q16 [m/s2]
      reg_amd_det_period = dataPtr->int_cfg_param2; // Q16 [s]
      break;
   }
   case SNS_REG_SAM_GROUP_VMD_V02:
   {
      qmd_config_s *defCfgPtr;
      sns_reg_vmd_data_group_s *dataPtr = (sns_reg_vmd_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_VMD_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_VMD_SVC_ID_V01);
         return;
      }
      defCfgPtr = (qmd_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->sample_rate = dataPtr->def_acc_samp_rate;
      defCfgPtr->internal_config_param1 = dataPtr->int_cfg_param1;
      defCfgPtr->internal_config_param2 = dataPtr->int_cfg_param2;
      break;
   }
   case SNS_REG_SAM_GROUP_RMD_V02:
   {
      qmd_config_s *defCfgPtr;
      sns_reg_rmd_data_group_s *dataPtr = (sns_reg_rmd_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_RMD_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_RMD_SVC_ID_V01);
         return;
      }
      defCfgPtr = (qmd_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->sample_rate = dataPtr->def_acc_samp_rate;
      defCfgPtr->internal_config_param1 = dataPtr->int_cfg_param1;
      defCfgPtr->internal_config_param2 = dataPtr->int_cfg_param2;
      defCfgPtr->internal_config_param3 = dataPtr->int_cfg_param3;
      defCfgPtr->internal_config_param4 = dataPtr->int_cfg_param4;
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_RMD_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = dataPtr->sensor_report_rate;
      }
      break;
   }
   case SNS_REG_SAM_GROUP_GYRO_AMD_V02:
   {
      sns_reg_gyro_amd_data_group_s *grpPtr =
         (sns_reg_gyro_amd_data_group_s *)regDataPtr;
      reg_gyro_amd_abs_rest_thresh = grpPtr->int_cfg_param1; // [(rad/s)^2]
      reg_gyro_amd_det_period = grpPtr->int_cfg_param2; // [s]
      reg_gyro_amd_rel_rest_thresh = grpPtr->int_cfg_param3; // [(rad/s)^2]
      break;
   }
   case SNS_REG_SAM_GROUP_GYRO_TAP_V02:
   {
      uint8_t i;
      gtap_config_struct *defCfgPtr;
      sns_reg_gyro_tap_group_s *grpPtr = (sns_reg_gyro_tap_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_GYRO_TAP2_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_GYRO_TAP2_SVC_ID_V01);
         return;
      }
      defCfgPtr = (gtap_config_struct *)(algoPtr->defConfigData.memPtr);

      defCfgPtr++;
      if (grpPtr->gyro_tap_version != SNS_REG_GYRO_TAP_DEF_VERSION)
      {
         /* This is a version of the registry that we do not recognize
            Or the registry is corrupted - do not use the read parameters
            Mark the registry scenarios as invalid.
          */
         for (i = 0; i < SNS_REG_MAX_GYRO_TAP_SCENARIOS; i++, defCfgPtr++)
         {
            defCfgPtr->scenario   =  SNS_REG_GYRO_INVALID_SCENARIO;
         }
         break;
      }
      else
      {
         for (i = 0; i < SNS_REG_MAX_GYRO_TAP_SCENARIOS; i++, defCfgPtr++)
         {
            defCfgPtr->scenario             =  grpPtr->scn[i].gyro_tap_scenario;

            defCfgPtr->z_anamoly_inc        =  (grpPtr->scn[i].gyro_tap_flags & GYRO_TAP_FLAGS_Z_ANAMOLY_INC) !=0;
            defCfgPtr->strong_z_anamoly_tb  =  (grpPtr->scn[i].gyro_tap_flags & GYRO_TAP_FLAGS_STRONG_Z_ANAMOLY_TB) != 0;
            defCfgPtr->stress_right_left    =  (grpPtr->scn[i].gyro_tap_flags & GYRO_TAP_FLAGS_STRESS_RL) != 0;
            defCfgPtr->stress_top_bottom    =  (grpPtr->scn[i].gyro_tap_flags & GYRO_TAP_FLAGS_STRESS_TB) != 0;

            defCfgPtr->tap_time_win_secs    =  grpPtr->scn[i].gyro_tap_time_secs_0;
            defCfgPtr->tap_time_sleep_secs  =  grpPtr->scn[i].gyro_tap_time_secs_1;
            defCfgPtr->jerk_win_secs        =  grpPtr->scn[i].gyro_tap_time_secs_2;
            defCfgPtr->ori_check_win_secs   =  grpPtr->scn[i].gyro_tap_time_secs_3;
            defCfgPtr->ori_check_int_secs   =  grpPtr->scn[i].gyro_tap_time_secs_4;

            defCfgPtr->lr_accel_rat_jerk_yx  = grpPtr->scn[i].gyro_tap_ratio_0;
            defCfgPtr->tb_accel_rat_jerk_xy  = grpPtr->scn[i].gyro_tap_ratio_1;

            defCfgPtr->acc_tap_thr           = grpPtr->scn[i].gyro_tap_thresh_0;
            defCfgPtr->lr_accel_jerk_min_thr = grpPtr->scn[i].gyro_tap_thresh_1;
            defCfgPtr->tb_accel_jerk_min_thr = grpPtr->scn[i].gyro_tap_thresh_2;
            defCfgPtr->accel_z_thr           = grpPtr->scn[i].gyro_tap_thresh_3;
            defCfgPtr->ori_check_thr         = grpPtr->scn[i].gyro_tap_thresh_4;
         }
      }
      break;
   }
   case SNS_REG_SAM_GROUP_PED_V02:
   {
      ped_config_s *defCfgPtr;
      sns_reg_ped_data_group_s *dataPtr = (sns_reg_ped_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_PED_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_PED_SVC_ID_V01);
         return;
      }
      defCfgPtr = (ped_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->step_threshold = dataPtr->step_threshold;
      defCfgPtr->swing_threshold = dataPtr->swing_threshold;
      defCfgPtr->step_prob_threshold = dataPtr->step_prob_threshold;
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_PED_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = dataPtr->sensor_report_rate;
      }
      break;
   }
   case SNS_REG_SAM_GROUP_PAM_V02:
   {
      pam_config_s *defCfgPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_PAM_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_PAM_SVC_ID_V01);
         return;
      }
      defCfgPtr = (pam_config_s *)(algoPtr->defConfigData.memPtr);
      sns_reg_pam_data_group_s *dataPtr = (sns_reg_pam_data_group_s *)regDataPtr;

      defCfgPtr->measurement_period = (uint16_t)dataPtr->int_cfg_param1;
      defCfgPtr->step_count_threshold = (uint16_t)dataPtr->int_cfg_param2;
      defCfgPtr->sample_rate = dataPtr->int_cfg_param3;
      defCfgPtr->dutycycleOnPercent = dataPtr->int_cfg_param4;
      break;
   }
   case SNS_REG_SAM_GROUP_BASIC_GESTURES_V02:
   {
      sns_reg_basic_gestures_data_group_s *dataPtr =
         (sns_reg_basic_gestures_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_BASIC_GESTURES_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_BASIC_GESTURES_SVC_ID_V01);
         return;
      }
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_BASIC_GESTURES_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = dataPtr->sensor_report_rate;
      }
      break;
   }
   case SNS_REG_SAM_GROUP_FACING_V02:
   {
      sns_reg_facing_data_group_s *dataPtr =
         (sns_reg_facing_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_FACING_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_FACING_SVC_ID_V01);
         return;
      }
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_FACING_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = dataPtr->sensor_report_rate;
      }
      break;
   }
   case SNS_REG_SAM_GROUP_DISTANCE_BOUND_V02:
   {
      distance_bound_config_s *defCfgPtr;
      sns_reg_distance_bound_data_group_s *dataPtr =
         (sns_reg_distance_bound_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_DISTANCE_BOUND_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_DISTANCE_BOUND_SVC_ID_V01);
         return;
      }
      defCfgPtr = (distance_bound_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_UNKNOWN_V01] =
         dataPtr->speedbounds_unknown;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_STATIONARY_V01] =
         dataPtr->speedbounds_stationary;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_INMOTION_V01] =
         dataPtr->speedbounds_inmotion;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_FIDDLE_V01] =
         dataPtr->speedbounds_fiddle;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_PEDESTRIAN_V01] =
         dataPtr->speedbounds_pedestrian;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_VEHICLE_V01] =
         dataPtr->speedbounds_vehicle;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_WALK_V01] =
         dataPtr->speedbounds_walk;
      defCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_RUN_V01] =
         dataPtr->speedbounds_run;
      break;
   }
   case SNS_REG_SAM_GROUP_CMC_V02:
   {
      sns_reg_cmc_data_group_s *dataPtr =
         (sns_reg_cmc_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_CMC_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_CMC_SVC_ID_V01);
         return;
      }
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_CMC_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = dataPtr->sensor_report_rate;
      }
      break;
   }
   case SNS_REG_SAM_GROUP_GAME_ROTATION_VECTOR_V02:
   {
      sns_reg_game_rot_vec_data_group_s *dataPtr =
         (sns_reg_game_rot_vec_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01);
         return;
      }
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = dataPtr->sensor_report_rate;
         if( algoPtr->defSensorReportRate != 0 )
         {
            algoPtr->dataSyncRequired = true;
         }
         else
         {
            algoPtr->dataSyncRequired = false;
         }
         if( ((game_rot_vec_config_s *)(algoPtr->defConfigData.memPtr))->sample_rate <
               dataPtr->sensor_report_rate )
         {
            ((game_rot_vec_config_s *)(algoPtr->defConfigData.memPtr))->sample_rate =
               dataPtr->sensor_report_rate;
         }
      }
      break;
   }
   case SNS_REG_SAM_GROUP_QUATERNION_V02:
   {
      sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(SNS_SAM_QUATERNION_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_QUATERNION_SVC_ID_V01);
         return;
      }
      sns_reg_quaternion_data_group_s *grpPtr = (sns_reg_quaternion_data_group_s *)regDataPtr;
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_QUATERNION_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = grpPtr->sensor_report_rate;
         if( algoPtr->defSensorReportRate != 0 )
         {
            algoPtr->dataSyncRequired = true;
            ((gyro_quat_config_s *)(algoPtr->defConfigData.memPtr))->sample_rate =
               grpPtr->sensor_report_rate;
         }
         else
         {
            algoPtr->dataSyncRequired = false;
         }
      }
      break;
   }
   case SNS_REG_SAM_GROUP_ROTATION_VECTOR_V02:
   {
      sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(SNS_SAM_ROTATION_VECTOR_SVC_ID_V01);
      if ( NULL == algoPtr )
      {
         SNS_SAM_PRINTF1(ERROR,
             "sns_sam_process_reg_data: failed to get algo handle for service id %d",
             SNS_SAM_ROTATION_VECTOR_SVC_ID_V01);
         return;
      }
      sns_reg_rotation_vector_data_group_s *grpPtr = (sns_reg_rotation_vector_data_group_s *)regDataPtr;
      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_ROTATION_VECTOR_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = grpPtr->sensor_report_rate;
         if( algoPtr->defSensorReportRate != 0 )
         {
            algoPtr->dataSyncRequired = true;
         }
         else
         {
            algoPtr->dataSyncRequired = false;
         }
         if( ((rotation_vector_config_s *)(algoPtr->defConfigData.memPtr))->sample_rate <
               grpPtr->sensor_report_rate )
         {
            ((rotation_vector_config_s *)(algoPtr->defConfigData.memPtr))->sample_rate =
               grpPtr->sensor_report_rate;
         }
      }
      break;
   }
   case SNS_REG_SAM_GROUP_FMV_PARAMS_V02:
   {
      fmv_config_s *defCfgPtr;
      sns_reg_fmv_data_group_s *grpPtr = (sns_reg_fmv_data_group_s *)regDataPtr;
      sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(SNS_SAM_FILTERED_MAG_SVC_ID_V01);
      if(NULL == algoPtr)
      {
         SNS_SAM_PRINTF1(HIGH,
            "sns_sam_process_reg_data: algo handle null for service id %d",
            SNS_SAM_FILTERED_MAG_SVC_ID_V01);
         return;
      }

      defCfgPtr = (fmv_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->fmv_tcs[0] = grpPtr->accuracies[0];
      defCfgPtr->fmv_tcs[1] = grpPtr->accuracies[1];
      defCfgPtr->fmv_tcs[2] = grpPtr->accuracies[2];
      defCfgPtr->fmv_tcs[3] = grpPtr->accuracies[3];

      defCfgPtr->max_gyro_sample_gap_tolerance = grpPtr->gyro_gap_thresh;
      defCfgPtr->max_mag_innovation            = grpPtr->max_mag_innovation;
      defCfgPtr->max_mag_sample_gap_factor     = grpPtr->mag_gap_factor;
      defCfgPtr->min_turn_rate                 = grpPtr->gyro_thresh_for_zupt;

      // Use Gyro based AMD reg item values for motion detection config params
      if( reg_gyro_amd_abs_rest_thresh==0 ||
          reg_gyro_amd_rel_rest_thresh==0 ||
          reg_gyro_amd_det_period==0 )
      {
         // Gyro AMD reg item values unavailable. Use Default values.
      }
      else
      {
         defCfgPtr->abs_rest_var_thold = reg_gyro_amd_abs_rest_thresh;
         defCfgPtr->rel_rest_var_thold = reg_gyro_amd_rel_rest_thresh;
         defCfgPtr->motion_detect_period = reg_gyro_amd_det_period;
      }

      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_FILTERED_MAG_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = grpPtr->sensor_report_rate;
         if( algoPtr->defSensorReportRate != 0 )
         {
            algoPtr->dataSyncRequired = true;
         }
         else
         {
            algoPtr->dataSyncRequired = false;
         }
         if( defCfgPtr->sample_rate < grpPtr->sensor_report_rate )
         {
            defCfgPtr->sample_rate = grpPtr->sensor_report_rate;
         }
      }

      break;
   }

   case SNS_REG_SAM_GROUP_GRAVITY_VECTOR_PARAMS_V02:
   {
      gravity_config_s *defCfgPtr;
      sns_reg_gravity_data_group_s *grpPtr = (sns_reg_gravity_data_group_s *)regDataPtr;
      sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01);
      if(NULL == algoPtr)
      {
         SNS_SAM_PRINTF1(HIGH,
            "sns_sam_process_reg_data: algo handle null for service id %d",
            SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01);
         return;
      }

      defCfgPtr = (gravity_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->filter_tc[0] = grpPtr->int_cfg_arr1[0];
      defCfgPtr->filter_tc[1] = grpPtr->int_cfg_arr1[1];
      defCfgPtr->filter_tc[2] = grpPtr->int_cfg_arr1[2];
      defCfgPtr->filter_tc[3] = grpPtr->int_cfg_arr1[3];
      defCfgPtr->rel_rest_var_thold = grpPtr->int_cfg_param1;

      // Use AMD reg item values for absolute rest onfig paras
      if( (reg_amd_stddev_thresh==0) || (reg_amd_det_period==0) )
      {
         // AMD reg item values unavailable. Use Default values.
      }
      else
      {
         //[Q16m/s2 Std Dev]=>[m/s2 Var]
         defCfgPtr->abs_rest_var_thold =
            (float)( FX_FIXTOFLT_Q16(reg_amd_stddev_thresh) *
            FX_FIXTOFLT_Q16(reg_amd_stddev_thresh) );
         //[Q16s]=>[s]
         defCfgPtr->motion_detect_period =
            (float)FX_FIXTOFLT_Q16(reg_amd_det_period);
      }

      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = grpPtr->sensor_report_rate;
         if( algoPtr->defSensorReportRate != 0 )
         {
            algoPtr->dataSyncRequired = true;
         }
         else
         {
            algoPtr->dataSyncRequired = false;
         }
         if( defCfgPtr->sample_rate < grpPtr->sensor_report_rate )
         {
            defCfgPtr->sample_rate = grpPtr->sensor_report_rate;
         }
      }

      break;
   }

   case SNS_REG_SAM_GROUP_ORIENTATION_PARAMS_V02:
   {
      orientation_config_s *defCfgPtr;
      sns_reg_orientation_data_group_s *grpPtr = (sns_reg_orientation_data_group_s *)regDataPtr;
      sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(SNS_SAM_ORIENTATION_SVC_ID_V01);
      if(NULL == algoPtr)
      {
         SNS_SAM_PRINTF1(HIGH,
            "sns_sam_process_reg_data: algo handle null for service id %d",
            SNS_SAM_ORIENTATION_SVC_ID_V01);
         return;
      }

      defCfgPtr = (orientation_config_s *)(algoPtr->defConfigData.memPtr);

      defCfgPtr->accel_filter_tc = grpPtr->int_cfg_param1;
      defCfgPtr->mag_filter_tc = grpPtr->mag_cal_lat_num_samps;

      // Use AMD reg item values for absolute rest onfig paras
      if( (reg_amd_stddev_thresh==0) || (reg_amd_det_period==0) )
      {
         // AMD reg item values unavailable. Use Default values.
      }
      else
      {
         //[Q16m/s2 Std Dev]=>[m/s2 Var]
         defCfgPtr->abs_rest_var_thold =
            (float)( FX_FIXTOFLT_Q16(reg_amd_stddev_thresh) *
            FX_FIXTOFLT_Q16(reg_amd_stddev_thresh) );
         //[Q16s]=>[s]
         defCfgPtr->motion_detect_period =
            (float)( FX_FIXTOFLT_Q16(reg_amd_det_period) );
      }

      if( sns_sam_is_sensor_report_rate_available(SNS_SAM_ORIENTATION_SVC_ID_V01) )
      {
         algoPtr->defSensorReportRate = grpPtr->sensor_report_rate;
         if( algoPtr->defSensorReportRate != 0 )
         {
            defCfgPtr->sample_rate = grpPtr->sensor_report_rate;
         }
      }

      // Update filtered mag vector algorithm configuration
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_FILTERED_MAG_SVC_ID_V01);
      if (NULL != algoPtr)
      {
         fmv_config_s* cfgPtr =
            (fmv_config_s*)(algoPtr->defConfigData.memPtr);
         cfgPtr->mag_cal_lat_num_samps = grpPtr->mag_cal_lat_num_samps;
      }

      break;
   }
   case SNS_REG_SCM_GROUP_QMAG_CAL_ALGO_V02:
   {
      sns_reg_qmag_cal_algo_data_group_s *dataPtr =
         (sns_reg_qmag_cal_algo_data_group_s *)regDataPtr;

      if (!dataPtr->enable_algo)
      {
         // Mag Cal is not supported on ADSP
         // QFusion algos cannot be supported on ADSP SAM
         sns_sam_mag_cal_adsp_support_flag = false;
      }

      SNS_SAM_PRINTF2(MED, "sns_sam_process_reg_data: reg item id %d, sns_sam_mag_cal_adsp_support_flag: %d",
                      SNS_REG_SCM_GROUP_QMAG_CAL_ALGO_V02,(uint8_t)(sns_sam_mag_cal_adsp_support_flag));

      break;
   }

   case SNS_REG_SAM_GROUP_SMD_V02:
   {
      sns_reg_smd_data_group_s *grpPtr = (sns_reg_smd_data_group_s *)regDataPtr;
      algoPtr = sns_sam_get_algo_handle(SNS_SAM_SMD_SVC_ID_V01);
      if (NULL != algoPtr)
      {
         smd_config_s* defCfgPtr = (smd_config_s*)(algoPtr->defConfigData.memPtr);
         defCfgPtr->accel_window_time = grpPtr->accel_window_time;
         defCfgPtr->detect_threshold = grpPtr->detect_threshold;
         defCfgPtr->max_latency = grpPtr->max_latency;
         defCfgPtr->self_transition_prob_sm = grpPtr->self_transition_prob_sm;
         defCfgPtr->variable_decision_latency = grpPtr->variable_decision_latency;
         defCfgPtr->sample_rate = grpPtr->sample_rate;
         defCfgPtr->accel_norm_std_thresh = grpPtr->acc_norm_std_dev_thresh;
         defCfgPtr->eigen_thresh = grpPtr->eigen_thresh;
         defCfgPtr->step_count_thresh = grpPtr->step_count_thresh;
         defCfgPtr->step_window_time = grpPtr->step_window_time;

         if( sns_sam_is_sensor_report_rate_available(SNS_SAM_SMD_SVC_ID_V01) )
         {
            algoPtr->defSensorReportRate = grpPtr->sensor_report_rate;
         }
      }
      break;
   }
   case SNS_REG_SAM_GROUP_TILT_DETECTOR_V02:
   {
     sns_reg_tilt_detector_data_group_s *grpPtr = (sns_reg_tilt_detector_data_group_s *)regDataPtr;
     algoPtr = sns_sam_get_algo_handle(SNS_SAM_TILT_DETECTOR_SVC_ID_V01);
     if (NULL != algoPtr)
     {
        tilt_config_s* defCfgPtr = (tilt_config_s*)(algoPtr->defConfigData.memPtr);
        defCfgPtr->accel_window_time = grpPtr->accel_window_time;
        defCfgPtr->sample_rate = FX_FIXTOFLT_Q16(grpPtr->sensor_sample_rate);
        defCfgPtr->angle_threshold = grpPtr->def_tilt_angle_thresh;
        defCfgPtr->init_accel_window_time = grpPtr->init_accel_window_time;

        if( sns_sam_is_sensor_report_rate_available(SNS_SAM_TILT_DETECTOR_SVC_ID_V01) )
        {
           algoPtr->defSensorReportRate = grpPtr->sensor_report_rate;
        }
     }
     break;
   }
   default:
      break;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_modem_scn_ind_cb
  =========================================================================*/
static void sns_sam_modem_scn_ind_cb( qmi_client_type user_handle,
                             unsigned int msg_id,
    void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data )
{
   SNS_SAM_PRINTF2(LOW,
                   "modem_scn_ind call back called  %u, %u", 0, 0);

}

/*=========================================================================
  FUNCTION:  sns_sam_kickstart_modem_scn_algo
  =========================================================================*/
bool sns_sam_kickstart_modem_scn_algo(void)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_smr_header_s msgHdr;
   sns_sam_modem_scn_enable_req_msg_v01 *enableMsgPtr =
               (sns_sam_modem_scn_enable_req_msg_v01 *)
                       sns_smr_msg_alloc( SNS_SAM_MODULE,
                                 sizeof(sns_sam_modem_scn_enable_req_msg_v01));


   if(enableMsgPtr == NULL)
   {
      SNS_PRINTF_STRING_ERROR_0( SNS_SAM_DBG_MOD,
                           "Cannot allocate memory to general scn_enable_req");
      return false;
   }

   msgHdr.src_module = SNS_SAM_MODULE;
   msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
   msgHdr.priority = SNS_SMR_MSG_PRI_HIGH;
   msgHdr.msg_id = SNS_SAM_ALGO_ENABLE_REQ;

   msgHdr.svc_num = SNS_SAM_MODEM_SCN_SVC_ID_V01;
   msgHdr.ext_clnt_id = 0;
   msgHdr.txn_id = 0;
   msgHdr.body_len = sizeof(sns_sam_modem_scn_enable_req_msg_v01);

   SNS_SAM_PRINTF2(LOW,
                   "Sending modem_scn enable src module %u, svc %u",
                   msgHdr.src_module, msgHdr.svc_num);

   enableMsgPtr->sample_rate = FX_CONV_Q16(5, 0);
   sns_smr_set_hdr(&msgHdr, enableMsgPtr);

   err = sns_sam_mr_send(enableMsgPtr, NULL);
   if (SNS_SUCCESS != err)
   {
      SNS_PRINTF_STRING_ERROR_1(SNS_SAM_DBG_MOD,
                        "Error sending msg - err %d", err);
      return(false);
   }
   else
   {
      return(true);
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_set_modem_scn_cb
  =========================================================================*/
void sns_sam_set_modem_scn_cb( qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len, void *cb_data,
    qmi_client_error_type transp_err )
{
   SNS_SAM_PRINTF0(LOW, "Received Set Scenario response");
   /* We are not doing any additional protocol here. Hoewever, the modem
      and the ADSP implementation can pass some information in this channel */
}

/*=========================================================================
  FUNCTION:  sns_sam_handle_prov_tbl
  =========================================================================*/
void sns_sam_handle_prov_tbl(rfrpe_get_provisioned_table_revision_resp_v01 *table)
{
   SNS_SAM_PRINTF3( LOW,
                    "Rcd Provisioned Table rsp successfully valid flags %d %d %d",
                    table->provisioned_table_revision,
                    table->provisioned_table_OEM[0],
                    table->provisioned_table_OEM[1]);

   /* Additional checks can be made here to determine if the algo can
      be kick started
   */
   if (table->provisioned_table_revision_valid &&
       table->provisioned_table_OEM_valid &&
       table->provisioned_table_OEM_len > 0
       //&& strncmp(table->provisioned_table_OEM, "Qualcomm") // for example
       )
   {
      if (sns_sam_kickstart_modem_scn_algo() != true)
      {
         SNS_SAM_PRINTF3(LOW,
                        "started modem_scn_algo %d %d %d",
                          0, 0, 0);
      }
      else
      {
         SNS_SAM_PRINTF3(LOW,
                         "Cannot start modem_scn_algo %d %d %d",
                          0, 0, 0);
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_send_modem_scenarios_req
  =========================================================================*/
/*!
  @brief Request modem scenarios information

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_send_modem_scenario(uint8_t scn)
{
   qmi_idl_service_object_type service;
   uint32_t resp_msg_size;
   void *resp_msg;
   qmi_txn_handle qmi_txn;
   qmi_client_error_type qmi_err;
   uint8_t *resp_cb_data;
   rfrpe_set_scenario_req_v01 *msgPtr;

   sns_err_code_e rv = SNS_SUCCESS;
   qmi_client_type service_handle = NULL;

   service = rfrpe_get_service_object_v01();
   service_handle = modem_scn_user_handle;

   if (NULL == service_handle)
   {
      SNS_PRINTF_STRING_ERROR_0(SNS_SAM_DBG_MOD,
                                "Unable to acquire service handle for modem RF service");
      rv = SNS_ERR_INVALID_HNDL;
   }
   else
   {
      qmi_idl_get_message_c_struct_len(service, QMI_IDL_RESPONSE,
                                       QMI_RFRPE_SET_RFM_SCENARIO_REQ_V01,
                                       &resp_msg_size);
      resp_msg = SNS_OS_MALLOC(SNS_SAM_MODULE, resp_msg_size);
      msgPtr = SNS_OS_MALLOC(SNS_SAM_MODULE,
                             sizeof(rfrpe_set_scenario_req_v01));
      resp_cb_data = NULL;

      if (NULL == resp_msg || NULL == msgPtr)
      {
         SNS_PRINTF_STRING_ERROR_0(SNS_SAM_DBG_MOD,
                                   "Failed to allocate resources");
         if (resp_msg)
         {
            SNS_OS_FREE(resp_msg);
         }

         if (msgPtr)
         {
            SNS_OS_FREE(msgPtr);
         }
         rv = SNS_ERR_NOMEM;
      }
      else
      {
         msgPtr->scenarios_len = 1;
         msgPtr->scenarios[0] = scn;

         qmi_err = qmi_client_send_msg_async(service_handle,
                                             QMI_RFRPE_SET_RFM_SCENARIO_REQ_V01,
                                             msgPtr,
                                             sizeof(rfrpe_set_scenario_req_v01),
                                             resp_msg, resp_msg_size,
                                             sns_sam_set_modem_scn_cb,
                                             (void *)resp_cb_data, &qmi_txn);

         if (QMI_NO_ERR != qmi_err)
         {
            SNS_PRINTF_STRING_ERROR_1(SNS_SAM_DBG_MOD,
                                      "Received error from QMI framework call %d",
                                      qmi_err);
            if (resp_cb_data)
            {
               SNS_OS_FREE(resp_cb_data);
            }

            if (resp_msg)
            {
               SNS_OS_FREE(resp_msg);
            }
            if (msgPtr)
            {
               SNS_OS_FREE(msgPtr);
            }
            rv = SNS_ERR_FAILED;

            SNS_SAM_PRINTF2(LOW,
                            "Could not send scenario %u to modem, err = %d", scn, qmi_err);
         }
         else
         {
            SNS_SAM_PRINTF2(LOW,
                            "Sent scenario %u to modem %u", scn, 0);
         }
      }
   }

   return rv;
}

/*=========================================================================
  FUNCTION:  sns_sam_init_modem_qmi_client
  =========================================================================*/
void sns_sam_init_modem_qmi_client(void)
{
   qmi_idl_service_object_type service;
   qmi_cci_os_signal_type os_params_tmp;
   qmi_client_error_type  qmi_err;
   uint8_t *ind_cb_data;
   os_params_tmp.ext_signal = NULL;
   os_params_tmp.sig = SNS_SAM_QMI_CLI_SIG;
   os_params_tmp.timer_sig = SNS_SAM_QMI_TIMER_SIG;

   modem_scn_user_handle = NULL;

   service = rfrpe_get_service_object_v01();
   ind_cb_data = NULL;

   qmi_err = qmi_client_init_instance(service, QMI_CLIENT_INSTANCE_ANY,
                                      sns_sam_modem_scn_ind_cb, ind_cb_data,
                                      &os_params_tmp, 1000 * 1,
                                      &modem_scn_user_handle);

   SNS_SAM_PRINTF3(LOW,
                   "QMI Client init - err %d, handle %u, service %u",
                   qmi_err, modem_scn_user_handle, service);

   if( QMI_NO_ERR != qmi_err  || service == NULL)
   {
     SNS_SAM_PRINTF2(LOW,
                     "qmi_client_notifier_init error %d, service %d", qmi_err, service);
   }
   else
   {
      SNS_SAM_PRINTF2(LOW,
                      "QMI Client initalized - handle %u, service %u",
                      modem_scn_user_handle, service);
   }
}


/*=========================================================================
  FUNCTION:  sns_sam_send_modem_scenarios_req
  =========================================================================*/
/*!
  @brief Request modem scenarios information

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_send_modem_scenarios_req(void)
{
   qmi_idl_service_object_type service;
   uint32_t resp_msg_size;
   void *resp_msg;
   qmi_client_error_type qmi_err;
   qmi_client_type service_handle = NULL;
   uint8_t *resp_cb_data;
   sns_err_code_e rv = SNS_SUCCESS;

   service = rfrpe_get_service_object_v01();
   service_handle = modem_scn_user_handle;

   if (NULL == service_handle)
   {
      SNS_PRINTF_STRING_ERROR_0(SNS_SAM_DBG_MOD,
                                "Unable to acquire service handle for modem RF service");
      rv = SNS_ERR_INVALID_HNDL;
   }
   else
   {
      qmi_err = qmi_idl_get_message_c_struct_len(service, QMI_IDL_RESPONSE,
                                                 QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_REQ_V01,
                                                 &resp_msg_size);
      resp_msg = sns_smr_msg_alloc(SNS_DBG_MOD_APPS_ACM, resp_msg_size);
      if (NULL == resp_msg || qmi_err != QMI_NO_ERR)
      {
         SNS_SAM_PRINTF2(LOW,
                         "Failed to allocate resources %d %d", qmi_err, 0);
         rv = SNS_ERR_NOMEM;
      }
      else
      {
         resp_cb_data = SNS_OS_MALLOC(SNS_SAM_MODULE, sizeof(uint8_t));
         if ( NULL == resp_cb_data )
         {
            SNS_SAM_PRINTF0(ERROR, "error in malloc for resp_cb_data");
            return SNS_ERR_NOMEM;
         }
         *resp_cb_data = 1;

         qmi_err = qmi_client_send_msg_sync(service_handle,
                                            QMI_RFRPE_GET_PROVISIONED_TABLE_REVISION_REQ_V01,
                                            NULL, 0,  // zero length message
                                            resp_msg, resp_msg_size,
                                            10);


         if (QMI_NO_ERR != qmi_err)
         {
            SNS_SAM_PRINTF1(LOW,
                            "Rcd Error from QMI framework %d",
                            qmi_err);
            SNS_OS_FREE(resp_cb_data);
            sns_smr_msg_free(resp_msg);
            rv = SNS_ERR_FAILED;
         }
         else
         {
            SNS_SAM_PRINTF0(LOW,
                            "Sent Provisioned Table req successfully");
            sns_sam_handle_prov_tbl(resp_msg);
         }
      }
   }

   return rv;
}

/*=========================================================================
  FUNCTION:  sns_sam_get_modem_info_delayed
  =========================================================================*/
/*!
  @brief Get information from modem
         Specifically queries the modem to find out if mulitple RF scenarios
         are supported.

  @return none
*/
/*=======================================================================*/
void sns_sam_get_modem_info_delayed(void)
{
/* Temporarily featurizing this code off for 8084, since it causes a bootup 
 * crash on the 8084 RCM when a QCN file is loaded */
#ifndef BRINGUP_8084
   sns_sam_init_modem_qmi_client();

   sns_sam_send_modem_scenarios_req();
#endif /* BRINGUP_8084 */
}

/*=========================================================================
  FUNCTION:  sns_sam_get_modem_info_cb
  =========================================================================*/
/*!
  @brief Callback registered for timer expiry

  @param[i] argPtr: pointer to argument for callback function

  @return None
*/
/*=======================================================================*/
void sns_sam_get_modem_info_cb(void *argPtr)
{
  uint8_t err;

  UNREFERENCED_PARAMETER(argPtr);

  sns_os_sigs_post(sns_sam_sig_event_acc(),
                   SNS_SAM_MODEM_INFO_SIG,
                   OS_FLAG_SET,
                   &err);
  if (err != OS_ERR_NONE)
  {
     SNS_SAM_DEBUG1(ERROR, DBG_SAM_MODEM_INFO_CB_SIGNALERR, err);
  }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_modem_info
  =========================================================================*/
/*!
  @brief Sets a timer to trigger getting information abour the provisioned
        scenario table in the modem.
        This function does not actually do any processing to not delay SAM
        startup.

  @return none
*/
void sns_sam_get_modem_info(void)
{
   /* Start a timer to get modem information */
   sns_err_code_e err;

   err = sns_em_create_timer_obj(sns_sam_get_modem_info_cb,
                                 NULL,
                                 SNS_EM_TIMER_TYPE_ONESHOT,
                                 &(sns_sam_modem_info_timer));
   if (err == SNS_SUCCESS)
   {
      err = sns_em_register_timer(sns_sam_modem_info_timer,
                                  sns_em_convert_usec_to_localtick(
                                     SNS_SAM_MODEM_INFO_TIMER_PERIOD_USEC));
      if (err == SNS_SUCCESS)
      {
        SNS_SAM_DEBUG2(MEDIUM, DBG_SAM_REG_TIMER_STARTED,
                       0, SNS_SAM_MODEM_INFO_TIMER_PERIOD_USEC);
      }
   }
   if (err != SNS_SUCCESS)
   {
      SNS_SAM_DEBUG1(ERROR, DBG_SAM_REG_TIMER_FAILED, err);
   }
   return;
}

/*===========================================================================
  FUNCTION:   sns_sam_ap_state_change_cb
===========================================================================*/
/*!
  @brief Called by Power module when AP state changes

  @param[i] suspendState - true, if AP entered suspend; false, on wake up

  @return   none
 */
/*=========================================================================*/
void sns_sam_ap_state_change_cb( bool suspendState )
{
   uint32_t setBit, clearBit;
   uint8_t err;

   if( suspendState )
   {
      setBit = SNS_SAM_AP_SUSPEND_SIG;
      clearBit = SNS_SAM_AP_AWAKE_SIG;
   }
   else
   {
      setBit = SNS_SAM_AP_AWAKE_SIG;
      clearBit = SNS_SAM_AP_SUSPEND_SIG;
   }
   sns_os_sigs_post( sns_sam_sig_event_acc(), clearBit, OS_FLAG_CLR, &err );
   sns_os_sigs_post( sns_sam_sig_event_acc(), setBit, OS_FLAG_SET, &err );
}
