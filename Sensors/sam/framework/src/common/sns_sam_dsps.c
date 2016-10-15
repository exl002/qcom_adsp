/*============================================================================
  FILE: sns_sam_dsps.c

  This file contains Sensors Algorithm Manager implementation specific to DSPS

  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
 ============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/sam/framework/src/common/sns_sam_dsps.c#10 $ */
/* $DateTime: 2014/06/24 18:44:01 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-05-23  ad   Support fusion max rate based on the sensors used
  2014-05-12  ad   Support SMGR specified max mag sample rate for algorithms
  2014-05-04  hw   Propagating data type checking for each algorithm's algo input update
  2014-03-24  pk   Prevent bad input from being fed to RV and Game RV
  2014-03-11  rt   Fix SAM memory leak. Free request msg ptr for dep. co-located algos
  2014-02-19  pk   Add enable request validation
  2014-02-05  ks   Integrated Feature Motion Classifier support
  2014-01-30  pk   Allow sns_sam_prep_algo_report_ind() to return error code
  2014-01-27  hw   Patch QDSS SW logging into SAM framework 
  2014-01-23  pk   Support direct function calls (non QMI) for ALGO_UPDATE_REQ
  2013-12-17  lka  Refactored sns_sam_update_input_type()
  2013-12-10  ad   Support config of min default sample rate of fusion algorithms
  2013-10-29  pk   Removed event flag from output structure of Facing algorithm
  2013-10-24  ad   Fix Gyro tap integration
  2013-10-20  ad   Fixed klocwork errors
  2013-10-18  ad   Fix bug in updating batch period
  2013-10-14  sc   Added support for oem_1 algo
  2013-10-10  sc   Add relative threshold support
  2013-10-03  pk   Add support for batch indicator
  2013-09-18  pk   Rework sns_sam_algo_send_batch_ind()
  2013-09-18  pk   Use client specified coordinate system for Rotation vector
  2013-09-11  ad   Fix suspend behavior for SMD and Gravity
  2013-09-09  ad   Update default behavior to not wakeup Apps from suspend for indications
  2013-09-06  dk   Increased logging support for SMD
  2013-09-03  ag   Remove double free when sam_mr_send fails
  2013-08-15  vh   Eliminated Klocwork warnings
  2013-08-27  dk   Added logging support for SMD's step inference
  2013-08-22  pk   Changed max back to back batch indications per target
  2013-07-31  hw   Fix Klocwork warning error  
  2013-07-26  vh   Eliminated compiler warnings
  2013-07-13  asj  Limit mag data requests from FMV to 50 Hz
  2013-07-12  gju  Limit mag data requests from FMV to 60 Hz
  2013-06-27  gju  Remove Mag cal and use SMGR for mag instead
  2103-06-06  pd   Added Significant Motion Detection
  2013-06-05  tc   Added support for Game Rotation Vector
  2013-05-16 asr   Removed unused log types.
  2013-05-14  sc   Fixed an issue with updating sensor threshold configuration
                   for the case of magnetometer streaming
  2013-05-01  dc   Set default notification to true to support clients
                   on the Modem processor who do not use the notify field.
  2013-03-16  lka  Modified logic related to MD interrupt processing
  2013-02-19  dc   Retrieve info about notifications during suspend that is
                   passed by the client.
  2013-02-15  asj  added modem scn algo
  2013-02-12  gju  Use SMR message allocation functions
  2013-02-07  asj  added time salt to gyrobuf
  2012-12-08  lka  added support for multiple clients to MD interrupt
  2012-12-07  gju  Use updated SAM framework API
  2012-11-20  vh   Eliminated compiler warnings
  2012-11-15  hw   Migrates Gestures Features from APPS to ADSP
  2012-10-14  sc   Use AMD enable request structure instead of common QMD struct
  2012-10-10  hw   Added ACCEL part in sansor algorithm configuration update
  2012-09-13  vh   Eliminated compiler warnings
  2012-08-10  sc   Merged from mainline the gyrobuf algo service
  2012-08-09  ag   Updated to use official QMI service IDs
  2012-08-09  rp   Add threshold detection support for pressure sensor(windows requirement)
  2012-07-23  ag   Porting from SMR to QMI
  2012-06-13  lka  Increased buffer size in gyrobuf indication message
  2012-05-08  lka  Fixed (gyrobuf algorithm) fixed-point conversion and buffer
                   limit issues
  2012-04-17  lka  Added support for Gyrobuf algorithm
  2012-03-07  ad   Support motion detect algo sample rate config via registry
  2011-11-14  jhh  Updated alloc and free function calls to meet new API
  2011-11-04  sc   Added support for sensor threshold algorithm
  2011-09-21  ad   disable interrupt mode when non-AMD algorithms are active
  2011-07-18  ad   refactor SAM for memory optimization
  2011-07-05  sc   Use timestamp from sensor data for algo report and logging
  2011-06-08  ad   changed the accel sampling rate for QMD to 25Hz
  2011-05-06  ad   support algorithm configuration using sensors registry
  2011-05-02  sc   Modified with updated function prototype for processing
                   messages from another SAM module
  2011-04-20  ad   added support for low power QMD
  2011-04-19  sc   Removed quaternion and gravity algorithm from DSPS
  2011-04-18  ad   move target specific code to target file
  2011-04-12  ad   support SAM power vote
  2011-04-04  sc   Added support for gravity algorithm
  2011-03-31  ad   update algorithm registration
  2011-03-30  sc   Added support for quaternion logging
  2011-03-18  sc   Added support for quaternion algorithm
  2011-03-11  ad   initialize QMD internal config params
  2011-03-11  ad   update algorithm configuration per client specification
  2011-01-28  br   changed constant names for deploying QMI tool ver#2
  2011-01-21  sc   Added support for PCSIM playback
  2011-01-11  ad   initial version

 ============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <math.h>

#include "sns_sam_priv.h"

#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_debug_str.h"
#include "sns_log_types.h"
#include "sns_reg_common.h"
#include "sns_log_api.h"
#include "sns_profiling.h"

#include "qmd.h"
#include "sns_sam_amd_v01.h"
#include "sns_sam_vmd_v01.h"
#include "sns_sam_rmd_v01.h"
#include "sns_smgr_api_v01.h"

#ifdef CONFIG_USE_OEM_1_ALGO
#include "sns_oem_1_v01.h"
#include "oem_1.h"
#endif //CONFIG_USE_OEM_1_ALGO

#include "threshold.h"
#include "sns_sam_sensor_thresh_v01.h"
#include "gyrobuf.h"
#include "sns_sam_gyrobuf_v01.h"

#include "pedometer.h"
#include "sns_sam_ped_v01.h"

#include "pam.h"
#include "sns_sam_pam_v01.h"
#include "rel_rest.h"

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

#define FEATURE_SNS_SAM_BASIC_GESTURES
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
#include "shake_gestures.h"
#include "basic_gestures.h"
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#define FEATURE_SNS_SAM_TAP
#ifdef FEATURE_SNS_SAM_TAP
#include "tap.h"
#endif /* FEATURE_SNS_SAM_TAP */

#define FEATURE_SNS_SAM_GYRO_TAP
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

#include "sns_sam_modem_scn_v01.h"
#include "modem_scn.h"

#include "sns_sam_cmc_v01.h"
#include "cmc.h"
#include "sns_sam_distance_bound_v01.h"
#include "distance_bound.h"

#include "sns_sam_smd_v01.h"
#include "smd_algo.h"

#include "sns_sam_game_rotation_vector_v01.h"
#include "game_rotation_vector.h"

#include "sns_sam_tilt_detector_v01.h"
#include "tilt_detector.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_SAM_SVC_IS_QMD_ALGO(svc_index) (((svc_index) == SNS_SAM_AMD_SVC_ID_V01 ||    \
                                             (svc_index) == SNS_SAM_VMD_SVC_ID_V01 ||    \
                                             (svc_index) == SNS_SAM_RMD_SVC_ID_V01) ? 1 : 0)


/*---------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

// Structures for Batching
typedef struct {
   sns_sam_game_rotation_vector_result_s_v01 report;
   uint32_t timestamp;
} sns_sam_game_rotation_vector_batch_item_s;

typedef struct {
   sns_sam_gravity_report_s_v01 report;
   uint32_t timestamp;
} sns_sam_gravity_batch_item_s;

typedef struct {
   sns_sam_orientation_result_s_v01 report;
   uint32_t timestamp;
} sns_sam_orientation_batch_item_s;

typedef struct {
   sns_sam_rotation_vector_result_s_v01 report;
   uint32_t timestamp;
} sns_sam_rotation_vector_batch_item_s;

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/
extern sns_err_code_e sns_sam_send_modem_scenario(uint8_t scn);

sns_err_code_e sns_sam_log_smd_report(
   const sns_sam_client_req_s* clientReqPtr,
   int8_t motionState,
   uint8_t motionStateProbability,
   uint32_t timeStamp);

#ifdef FEATURE_SNS_SAM_TILT
/*=========================================================================
  FUNCTION:  sns_sam_log_integ_angle_config
  =========================================================================*/
sns_err_code_e sns_sam_log_integ_angle_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
  sns_log_integ_angle_config_s* logPtr;
  sns_err_code_e err = SNS_SUCCESS;
  integ_angle_config_struct *algoCfgPtr;

  //Allocate log packet
  err = sns_logpkt_malloc(SNS_LOG_INTEG_ANGLE_CONFIG,
                          sizeof(sns_log_integ_angle_config_s),
                          (void**)&logPtr);

  if ((err == SNS_SUCCESS) && (logPtr != NULL))
  {
     //Generate log packet
     logPtr->version = SNS_LOG_INTEG_ANGLE_CONFIG_VERSION;

     //Timestamp
     logPtr->timestamp      = sns_em_get_timestamp();
     logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

     algoCfgPtr = (integ_angle_config_struct *)(algoInstPtr->configData.memPtr);
     logPtr->algo_inst_id   = algoInstId;

     logPtr->sample_rate         = algoCfgPtr->sample_rate;
     logPtr->clip_threshold      = algoCfgPtr->clip_threshold;

     //Commit log (also frees up the log packet memory)
     err = sns_logpkt_commit(SNS_LOG_INTEG_ANGLE_CONFIG, logPtr);
  }

  return(err);
}

/*=========================================================================
  FUNCTION:  sns_sam_log_integ_angle_result
  =========================================================================*/
sns_err_code_e sns_sam_log_integ_angle_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
  uint8_t i;
  sns_log_integ_angle_result_s* logPtr;
  sns_err_code_e err = SNS_SUCCESS;
  integ_angle_input_struct *input =
    (integ_angle_input_struct *)(algoInstPtr->inputData.memPtr);
  integ_angle_output_struct *output =
    (integ_angle_output_struct *)(algoInstPtr->outputData.memPtr);

  //Allocate log packet
  err = sns_logpkt_malloc(SNS_LOG_INTEG_ANGLE_RESULT,
                          sizeof(sns_log_integ_angle_result_s),
                          (void**)&logPtr);

  if ((err == SNS_SUCCESS) && (logPtr != NULL))
  {
    logPtr->algo_inst_id   = algoInstId;
    logPtr->version        = SNS_LOG_INTEG_ANGLE_RESULT_VERSION;
    logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
    logPtr->timestamp      = output->ts;

    for (i = 0; i < 3; i++)
    {
      logPtr->g[i]  = input->gyro[i];
      logPtr->ia[i] = output->angle_change[i];
    }

    //Commit log (also frees up the log packet memory)
    err = sns_logpkt_commit(SNS_LOG_INTEG_ANGLE_RESULT, logPtr);
  }

  return(err);
}

/*=========================================================================
  FUNCTION:  sns_sam_log_integ_angle_report
  =========================================================================*/
sns_err_code_e  sns_sam_log_integ_angle_report(
    const sns_sam_client_req_s* clientReqPtr,
    const sns_sam_integ_angle_report_ind_msg_v01 *msgPtr )
{
  sns_log_integ_angle_report_s* logPtr;
  sns_err_code_e err = SNS_SUCCESS;

  //Allocate log packet
  err = sns_logpkt_malloc(SNS_LOG_INTEG_ANGLE_REPORT,
                          sizeof(sns_log_integ_angle_report_s),
                          (void**)&logPtr);

  if ((err == SNS_SUCCESS) && (logPtr != NULL))
  {
    logPtr->algo_inst_id   = clientReqPtr->algoInstId;
    logPtr->version        = SNS_LOG_INTEG_ANGLE_REPORT_VERSION;
    logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
    logPtr->timestamp      = msgPtr->timestamp;
    logPtr->report_id      = clientReqPtr->reportId;
    logPtr->report_type    = clientReqPtr->reportType;
    logPtr->client_id      = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
    logPtr->ia[0]          = msgPtr->angle[0];
    logPtr->ia[1]          = msgPtr->angle[1];
    logPtr->ia[2]          = msgPtr->angle[2];

    //Commit log (also frees up the log packet memory)
    err = sns_logpkt_commit(SNS_LOG_INTEG_ANGLE_REPORT, logPtr);
  }
  return(err);
}
#endif /* FEATURE_SNS_SAM_TILT */

#ifdef FEATURE_SNS_SAM_GYRO_TAP

/*=========================================================================
  FUNCTION:  sns_sam_log_gtap_config
  =========================================================================*/
sns_err_code_e sns_sam_log_gtap_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
  sns_log_gyro_tap_config_s* logPtr;
  sns_err_code_e err = SNS_SUCCESS;
  gtap_config_struct *algoCfgPtr;

  //Allocate log packet
  err = sns_logpkt_malloc(SNS_LOG_GYRO_TAP_CONFIG,
                          sizeof(sns_log_gyro_tap_config_s),
                          (void**)&logPtr);

  if ((err == SNS_SUCCESS) && (logPtr != NULL))
  {
     //Generate log packet
     logPtr->version = SNS_LOG_GYRO_TAP_CONFIG_VERSION;

     //Timestamp
     logPtr->timestamp      = sns_em_get_timestamp();
     logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

     algoCfgPtr = (gtap_config_struct *)(algoInstPtr->configData.memPtr);
     logPtr->algo_inst_id   = algoInstId;

     logPtr->tap_time_win                  = algoCfgPtr->tap_time_win_secs;
     logPtr->tap_dir_win                   = 0xFFFFFFFF;
     logPtr->tap_time_sleep                = algoCfgPtr->tap_time_sleep_secs;
     logPtr->tap_dir_win                   = 0xFFFFFFFF;
     logPtr->accel_tap_thr                 = algoCfgPtr->acc_tap_thr;
     logPtr->lr_accel_jerk_min_thresh_min  = algoCfgPtr->lr_accel_jerk_min_thr;
     logPtr->lr_gyro_jerk_min_thresh_min   = 0xFFFFFFFF;
     logPtr->tb_accel_jerk_min_thr         = algoCfgPtr->tb_accel_jerk_min_thr;
     logPtr->tb_gyro_jerk_min_thr          = 0xFFFFFFFF;
     logPtr->jerk_win                      = algoCfgPtr->jerk_win_secs;
     logPtr->lr_accel_rat_jerk_yx          = algoCfgPtr->lr_accel_rat_jerk_yx;
     logPtr->lr_accel_rat_jerk_yz          = 0xFFFFFFFF;
     logPtr->lr_gyro_rat_jerk_zy           = 0xFFFFFFFF;
     logPtr->lr_gyro_rat_jerk_zx           = 0xFFFFFFFF;
     logPtr->tb_accel_rat_jerk_xy          = algoCfgPtr->tb_accel_rat_jerk_xy;
     logPtr->tb_accel_rat_jerk_xz          = 0xFFFFFFFF;
     logPtr->tb_gyro_rat_jerk_yx           = 0xFFFFFFFF;
     logPtr->tb_gyro_rat_jerk_yz           = 0xFFFFFFFF;
     logPtr->tb_accel_z_thresh             = algoCfgPtr->accel_z_thr;
     logPtr->tb_accel_z_rat_zx             = 0xFFFFFFFF;
     logPtr->tb_accel_z_rat_zy             = 0xFFFFFFFF;
     logPtr->ori_change_win                = algoCfgPtr->ori_check_win_secs;
     logPtr->ori_check_win                 = algoCfgPtr->ori_check_int_secs;
     logPtr->ori_change_thr                = algoCfgPtr->ori_check_thr;
     logPtr->z_axis_inc                    = algoCfgPtr->z_anamoly_inc;
     logPtr->loaded_axis_3_valid           = algoCfgPtr->strong_z_anamoly_tb;
     logPtr->lr_min_accel_jerk_thresh_min  = 0xFFFFFFFF;
     logPtr->lr_max_accel_jerk_thresh_min  = 0xFFFFFFFF;
     logPtr->lr_min_gyro_jerk_thresh_min   = 0xFFFFFFFF;
     logPtr->lr_max_gyro_jerk_thresh_min   = 0xFFFFFFFF;
     /* This is a dellibrate reuse of the last field */
     // logPtr->mild_accel_tap_thresh         = 0xFFFFFFFF;
     logPtr->mild_accel_tap_thresh         = algoCfgPtr->scenario;
     logPtr->loaded_z_axis_anamoly         = algoCfgPtr->stress_top_bottom;
     logPtr->stress_right_left             = algoCfgPtr->stress_right_left;

     //Commit log (also frees up the log packet memory)
     err = sns_logpkt_commit(SNS_LOG_GYRO_TAP_CONFIG, logPtr);
  }

  return(err);
}

/*=========================================================================
  FUNCTION:  sns_sam_log_gtap_result
  =========================================================================*/
sns_err_code_e sns_sam_log_gtap_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
  uint8_t i;
  sns_log_gyro_tap_result_s* logPtr;
  sns_err_code_e err = SNS_SUCCESS;
  gtap_input_struct *input =
    (gtap_input_struct *)(algoInstPtr->inputData.memPtr);
  gtap_output_struct *output =
    (gtap_output_struct *)(algoInstPtr->outputData.memPtr);

  //Allocate log packet
  err = sns_logpkt_malloc(SNS_LOG_GYRO_TAP_RESULT,
                          sizeof(sns_log_gyro_tap_result_s),
                          (void**)&logPtr);

  if ((err == SNS_SUCCESS) && (logPtr != NULL))
  {
    logPtr->algo_inst_id   = algoInstId;
    logPtr->version        = SNS_LOG_GYRO_TAP_RESULT_VERSION;
    logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
    logPtr->timestamp      = algoInstPtr->outputData.timestamp;

    for (i = 0; i < 3; i++)
    {
      logPtr->g[i]  = input->data.g[i];
      logPtr->a[i] =  input->data.a[i];
    }

    logPtr->tap_event = output->event;
    logPtr->algo_state = output->tap_state;

    //Commit log (also frees up the log packet memory)
    err = sns_logpkt_commit(SNS_LOG_GYRO_TAP_RESULT, logPtr);
  }

  return(err);
}

/*=========================================================================
  FUNCTION:  sns_sam_log_gtap_report
  =========================================================================*/
sns_err_code_e  sns_sam_log_gtap_report(
    const sns_sam_client_req_s* clientReqPtr,
    const sns_sam_gyro_tap2_report_ind_msg_v01 *msgPtr )
{
  sns_log_gyro_tap_report_s* logPtr;
  sns_err_code_e err = SNS_SUCCESS;

  //Allocate log packet
  err = sns_logpkt_malloc(SNS_LOG_GYRO_TAP_REPORT,
                          sizeof(sns_log_gyro_tap_report_s),
                          (void**)&logPtr);

  if ((err == SNS_SUCCESS) && (logPtr != NULL))
  {
    logPtr->algo_inst_id   = clientReqPtr->algoInstId;
    logPtr->version        = SNS_LOG_GYRO_TAP_REPORT_VERSION;
    logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
    logPtr->timestamp      = msgPtr->timestamp;
    logPtr->report_id      = clientReqPtr->reportId;
    logPtr->report_type    = clientReqPtr->reportType;
    logPtr->client_id      = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
    logPtr->tap_event      = msgPtr->tap_event;

    //Commit log (also frees up the log packet memory)
    err = sns_logpkt_commit(SNS_LOG_GYRO_TAP_REPORT, logPtr);
  }
  return(err);
}

#endif /* FEATURE_SNS_SAM_GYRO_TAP*/

#ifdef FEATURE_SNS_SAM_FNS
/*===========================================================================
  FUNCTION:   sns_sam_log_fns_report
  =========================================================================*/
/*!
  @brief log FNS algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] fnsState:     FNS state
  @param[i] timeStamp:    time stamp for algo report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_fns_report(
   const sns_sam_client_req_s* clientReqPtr,
   int32_t fnsState,
   uint32_t timeStamp)
{
   sns_err_code_e err;
   sns_log_fns_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FNS_REPORT,
                           sizeof(sns_log_fns_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_FNS_REPORT_VERSION;

      //Generate the log packet
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->fns_result = (uint8_t)fnsState;

      logPtr->algo_inst_id = algoInstId;
      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FNS_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_fns_config
  =========================================================================*/
/*!
  @brief log FNS algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_fns_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_fns_config_s* logPtr;
   face_n_shake_config_s *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FNS_CONFIG,
                           sizeof(sns_log_fns_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_FNS_CONFIG_VERSION;

      algoCfgPtr = (face_n_shake_config_s *)
         (algoInstPtr->configData.memPtr);

      logPtr->algo_instance_id   = algoInstId;

      logPtr->sample_rate    = algoCfgPtr->sample_rate;
      logPtr->facing_angle_thresh = algoCfgPtr->facing_angle_threshold;
      logPtr->basic_sleep         = algoCfgPtr->basic_sleep;
      logPtr->basic_shake_thresh  = algoCfgPtr->basic_shake_threshold;
      logPtr->timeout             = algoCfgPtr->timeout;
      logPtr->ic_param_1          = algoCfgPtr->internal_config_param1;
      logPtr->ic_param_2          = algoCfgPtr->internal_config_param2;
      logPtr->ic_param_3          = algoCfgPtr->internal_config_param3;
      logPtr->ic_param_4          = algoCfgPtr->internal_config_param4;

      //Timestamp
      logPtr->timestamp      = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FNS_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_fns_result
  =========================================================================*/
/*!
  @brief log FNS algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_fns_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   face_n_shake_input_s  *inputPtr;
   face_n_shake_output_s *outputPtr;
   face_n_shake_state_s  *algoPtr;
   sns_log_fns_result_s  *logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FNS_RESULT,
                           sizeof(sns_log_fns_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_FNS_RESULT_VERSION;

      algoPtr  =   (face_n_shake_state_s *)algoInstPtr->stateData.memPtr;
      inputPtr =   (face_n_shake_input_s *)algoInstPtr->inputData.memPtr;
      outputPtr = (face_n_shake_output_s *)algoInstPtr->outputData.memPtr;

      logPtr->accel_x  = inputPtr->a[0];
      logPtr->accel_y  = inputPtr->a[1];
      logPtr->accel_z  = inputPtr->a[2];
      logPtr->fns_result = outputPtr->face_n_shake_event;
      logPtr->fns_state  = algoPtr->face_n_shake_state;
      logPtr->axis_state = algoPtr->axis_algo->axis_state[2];
      logPtr->algo_inst_id = algoInstId;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FNS_RESULT, logPtr);
   }

   return err;
}

#endif /* FEATURE_SNS_SAM_FNS */

#ifdef FEATURE_SNS_SAM_BTE
/*===========================================================================
  FUNCTION:   sns_sam_log_bte_report
  =========================================================================*/
/*!
  @brief log BTE algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] bteState:     BTE state
  @param[i] timeStamp:    time stamp for algo report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_bte_report(
   const sns_sam_client_req_s* clientReqPtr,
   int32_t bteState,
   uint32_t timeStamp)
{
   sns_err_code_e err;
   sns_log_bte_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_BTE_REPORT,
                           sizeof(sns_log_bte_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_BTE_REPORT_VERSION;

      //Generate the log packet
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->bte_result = (uint8_t)bteState;

      logPtr->algo_inst_id = algoInstId;
      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_BTE_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_bte_config
  =========================================================================*/
/*!
  @brief log BTE algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_bte_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_bte_config_s* logPtr;
   bring_to_ear_config_s *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_BTE_CONFIG,
                           sizeof(sns_log_bte_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_BTE_CONFIG_VERSION;

      algoCfgPtr = (bring_to_ear_config_s *)
         (algoInstPtr->configData.memPtr);

      logPtr->algo_instance_id   = algoInstId;

      logPtr->sample_rate         = algoCfgPtr->sample_rate;
      logPtr->facing_angle_thresh = algoCfgPtr->facing_angle_threshold;
      logPtr->horiz_angle_thresh  = algoCfgPtr->horiz_angle_threshold;
      logPtr->vert_angle_thresh   = algoCfgPtr->vert_angle_threshold;
      logPtr->prox_enabled        = algoCfgPtr->prox_enabled;
      logPtr->ic_param_1          = algoCfgPtr->internal_config_param1;
      logPtr->ic_param_2          = algoCfgPtr->internal_config_param2;
      logPtr->ic_param_3          = algoCfgPtr->internal_config_param3;
      logPtr->ic_param_4          = algoCfgPtr->internal_config_param4;

      //Timestamp
      logPtr->timestamp      = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_BTE_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_bte_result
  =========================================================================*/
/*!
  @brief log BTE algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_bte_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   bring_to_ear_input_s  *inputPtr;
   bring_to_ear_output_s *outputPtr;
   bring_to_ear_state_s  *algoPtr;
   sns_log_bte_result_s  *logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_BTE_RESULT,
                           sizeof(sns_log_bte_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_BTE_RESULT_VERSION;

      algoPtr  =   (bring_to_ear_state_s *)algoInstPtr->stateData.memPtr;
      inputPtr =   (bring_to_ear_input_s *)algoInstPtr->inputData.memPtr;
      outputPtr = (bring_to_ear_output_s *)algoInstPtr->outputData.memPtr;

      logPtr->accel_x  = inputPtr->a[0];
      logPtr->accel_y  = inputPtr->a[1];
      logPtr->accel_z  = inputPtr->a[2];
      logPtr->proximity = inputPtr->proximity;
      logPtr->bte_result = outputPtr->bring_to_ear_event;
      logPtr->bte_state  = algoPtr->bring_to_ear_state;
      logPtr->axis_state = algoPtr->axis_algo->axis_state[2];
      logPtr->algo_inst_id = algoInstId;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_BTE_RESULT, logPtr);
   }

   return err;
}
#endif /* FEATURE_SNS_SAM_BTE */

#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
/*===========================================================================
  FUNCTION:   sns_sam_log_basic_gestures_report
  =========================================================================*/
/*!
  @brief log Basic Gestures algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] bgState:     Basic Gestures state
  @param[i] timeStamp:    time stamp for algo report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_basic_gestures_report(
   const sns_sam_client_req_s* clientReqPtr,
   int32_t bgState,
   uint32_t timeStamp)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_log_basic_gestures_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_BASIC_GESTURES_REPORT,
                           sizeof(sns_log_basic_gestures_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_BASIC_GESTURES_REPORT_VERSION;

      //Generate the log packet
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->basic_state = (uint8_t)bgState;

      logPtr->algo_inst_id = algoInstId;
      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_BASIC_GESTURES_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_basic_gestures_config
  =========================================================================*/
/*!
  @brief log Basic Gestures algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_basic_gestures_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_log_basic_gestures_config_s* logPtr;
   basic_gestures_config_struct *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_BASIC_GESTURES_CONFIG,
                           sizeof(sns_log_basic_gestures_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      uint8_t algoIndex = sns_sam_get_algo_index(SNS_SAM_BASIC_GESTURES_SVC_ID_V01);

      //Generate log packet
      logPtr->version = SNS_LOG_BASIC_GESTURES_CONFIG_VERSION;

      algoCfgPtr = (basic_gestures_config_struct *)
         (algoInstPtr->configData.memPtr);

      logPtr->algo_instance_id   = algoInstId;

      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->sleep = algoCfgPtr->sleep;
      logPtr->push_threshold = algoCfgPtr->push_threshold;
      logPtr->pull_threshold = algoCfgPtr->pull_threshold;
      logPtr->shake_threshold = algoCfgPtr->shake_threshold;
      logPtr->internal_config_param_1 = algoCfgPtr->internal_config_param1;
      logPtr->internal_config_param_2 = algoCfgPtr->internal_config_param2;
      logPtr->internal_config_param_3 = algoCfgPtr->internal_config_param3;
      logPtr->internal_config_param_4 = algoCfgPtr->internal_config_param4;
      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Timestamp
      logPtr->timestamp      = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_BASIC_GESTURES_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_basic_gestures_result
  =========================================================================*/
/*!
  @brief log Basic Gestures algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_basic_gestures_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = SNS_SUCCESS;
   basic_gestures_input_struct  *inputPtr;
   basic_gestures_output_struct *outputPtr;
   basic_gestures_state_struct  *algoPtr;
   sns_log_basic_gestures_result_s  *logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_BASIC_GESTURES_RESULT,
                           sizeof(sns_log_basic_gestures_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_BASIC_GESTURES_RESULT_VERSION;

      algoPtr  =   (basic_gestures_state_struct *)algoInstPtr->stateData.memPtr;
      inputPtr =   (basic_gestures_input_struct *)algoInstPtr->inputData.memPtr;
      outputPtr =  (basic_gestures_output_struct *)algoInstPtr->outputData.memPtr;

      logPtr->accel_x = inputPtr->a[0];
      logPtr->accel_y = inputPtr->a[1];
      logPtr->accel_z = inputPtr->a[2];
      logPtr->motion_state = inputPtr->motion_state;
      logPtr->basic_state = outputPtr->basic_state;
      logPtr->algo_inst_id = algoInstId;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_BASIC_GESTURES_RESULT, logPtr);
   }

   return err;
}
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_TAP
/*===========================================================================
  FUNCTION:   sns_sam_log_tap_report
  =========================================================================*/
/*!
  @brief log Tap algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] bteState:     Basic Gestures state
  @param[i] timeStamp:    time stamp for algo report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_tap_report(
   const sns_sam_client_req_s* clientReqPtr,
   int32_t bgState,
   uint32_t timeStamp)
{
   sns_err_code_e err = SNS_SUCCESS;
/*
   sns_log_basic_gestures_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_TAP_REPORT,
                           sizeof(sns_log_tap_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_TAP_REPORT_VERSION;

      //Generate the log packet
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->basic_state = (uint8_t)bgState;

      logPtr->algo_inst_id = algoInstId;
      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_TAP_REPORT, logPtr);
   }
*/
   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_tap_config
  =========================================================================*/
/*!
  @brief log Tap algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_tap_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = SNS_SUCCESS;
/*
   sns_log_tap_config_s* logPtr;
   tap_config_struct *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_TAP_CONFIG,
                           sizeof(sns_log_tap_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_TAP_CONFIG_VERSION;

      algoCfgPtr = (tap_config_struct *)
         (algoInstPtr->configData.memPtr);

      logPtr->algo_instance_id   = algoInstId;

      logPtr->sample_rate         = algoCfgPtr->sample_rate;

      //Timestamp
      logPtr->timestamp      = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_TAP_CONFIG, logPtr);
   }
*/
   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_tap_result
  =========================================================================*/
/*!
  @brief log Tap algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_tap_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = SNS_SUCCESS;
/*
   tap_input_struct  *inputPtr;
   tap_output_struct *outputPtr;
   tap_state_struct  *algoPtr;
   sns_log_tap_result_s  *logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_TAP_RESULT,
                           sizeof(sns_log_tap_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_TAP_RESULT_VERSION;

      algoPtr  =   (tap_state_struct *)algoInstPtr->stateData.memPtr;
      inputPtr =   (tap_input_struct *)algoInstPtr->inputData.memPtr;
      outputPtr =  (tap_output_struct *)algoInstPtr->outputData.memPtr;

      logPtr->tap_state = outputPtr->tap_state;
      logPtr->algo_inst_id = algoInstId;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_TAP_RESULT, logPtr);
   }
*/
   return err;
}
#endif /* FEATURE_SNS_SAM_TAP*/

#ifdef FEATURE_SNS_SAM_FACING
/*===========================================================================
  FUNCTION:   sns_sam_log_facing_report
  =========================================================================*/
/*!
  @brief log Facing algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] bteState:     Basic Gestures state
  @param[i] timeStamp:    time stamp for algo report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_facing_report(
   const sns_sam_client_req_s* clientReqPtr,
   int32_t state,
   uint32_t timeStamp)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_log_facing_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FACING_REPORT,
                           sizeof(sns_log_facing_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_FACING_REPORT_VERSION;

      //Generate the log packet
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->facing_state = (uint8_t)state;

      logPtr->algo_inst_id = algoInstId;
      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FACING_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_facing_config
  =========================================================================*/
/*!
  @brief log Facing algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_facing_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_log_facing_config_s* logPtr;
   facing_config_struct *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FACING_CONFIG,
                           sizeof(sns_log_facing_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_FACING_CONFIG_VERSION;

      algoCfgPtr = (facing_config_struct *)
         (algoInstPtr->configData.memPtr);

      logPtr->algo_instance_id = algoInstId;

      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->facing_angle_threshold = algoCfgPtr->facing_angle_threshold;
      logPtr->report_neutral = algoCfgPtr->report_neutral;

      //Timestamp
      logPtr->timestamp      = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FACING_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_facing_result
  =========================================================================*/
/*!
  @brief log Facing algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_facing_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = SNS_SUCCESS;
   facing_input_struct  *inputPtr;
   facing_output_struct *outputPtr;
   facing_state_struct  *algoPtr;
   sns_log_facing_result_s  *logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FACING_RESULT,
                           sizeof(sns_log_facing_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_FACING_RESULT_VERSION;

      algoPtr  =   (facing_state_struct *)algoInstPtr->stateData.memPtr;
      inputPtr =   (facing_input_struct *)algoInstPtr->inputData.memPtr;
      outputPtr =  (facing_output_struct *)algoInstPtr->outputData.memPtr;

      logPtr->accel_x = inputPtr->a[0];
      logPtr->accel_y = inputPtr->a[1];
      logPtr->accel_z = inputPtr->a[2];
      logPtr->motion_state = inputPtr->motion_state;
      logPtr->facing_state = outputPtr->facing_state;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FACING_RESULT, logPtr);
   }

   return err;
}
#endif /* FEATURE_SNS_SAM_FACING */

/*===========================================================================
  FUNCTION:   sns_sam_log_game_rotation_vector_report
  =========================================================================*/
/*!
  @brief log Game Rotation Vector algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] result: gravity result
  @param[i] timeStamp: time stamp for gravity report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_game_rotation_vector_report(
   const sns_sam_client_req_s* clientReqPtr,
   sns_sam_game_rotation_vector_result_s_v01 *result,
   uint32_t timeStamp)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_game_rotation_vector_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GAME_ROTATION_VECTOR_REPORT,
                           sizeof(sns_log_game_rotation_vector_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_GAME_ROTATION_VECTOR_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      SNS_OS_MEMCOPY(logPtr->game_rotation_vector,
                     result->game_rotation_vector,
                     sizeof(result->game_rotation_vector));
      logPtr->accuracy = result->accuracy;
      logPtr->coordinate_sys = 0;

      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GAME_ROTATION_VECTOR_REPORT, logPtr);
   }
   return err;
}


/*===========================================================================
  FUNCTION:   sns_sam_log_game_rotation_vector_result
  =========================================================================*/
/*!
  @brief log Game Rotation Vector algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_game_rotation_vector_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   uint8_t i;
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_game_rotation_vector_result_s* logPtr;

   game_rot_vec_output_s* outputPtr =
      ( game_rot_vec_output_s *)(algoInstPtr->outputData.memPtr);
   game_rot_vec_state_s* algoPtr =
      (game_rot_vec_state_s *)(algoInstPtr->stateData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GAME_ROTATION_VECTOR_RESULT,
                           sizeof(sns_log_game_rotation_vector_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_GAME_ROTATION_VECTOR_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      // log inputs
      for (i=0; i<3; i++)
      {
        logPtr->gravity[i] = algoPtr->gravity_vector.gravity[i];
        logPtr->gyro_quaternion[i] = (float)algoPtr->gyro_azimuth.azimuth[i];
      }
      logPtr->gyro_quaternion[3] = (float)algoPtr->gyro_quaternion.quaternion[3];


      logPtr->gravity_accuracy = algoPtr->gravity_vector.accuracy;
      logPtr->gravity_timestamp = algoPtr->gravity_vector.timestamp;
      logPtr->gravity_dev_mot_state = algoPtr->gravity_vector.device_state;
      logPtr->gyro_quat_timestamp = algoPtr->gyro_quaternion.timestamp;

      // log outputs
      for (i=0; i<4; i++)
      {
         logPtr->game_rotation_vector[i] = outputPtr->rotation_vector[i];
      }
      logPtr->coordinate_sys = 0;
      logPtr->accuracy = outputPtr->accuracy;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GAME_ROTATION_VECTOR_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_game_rotation_vector_config
  =========================================================================*/
/*!
  @brief log Game Rotation Vector algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_game_rotation_vector_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_game_rotation_vector_config_s* logPtr;
   game_rot_vec_config_s *algoCfgPtr =
      (game_rot_vec_config_s *)(algoInstPtr->configData.memPtr);
   game_rot_vec_state_s *algoStatePtr =
      (game_rot_vec_state_s *)(algoInstPtr->stateData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GAME_ROTATION_VECTOR_CONFIG,
                           sizeof(sns_log_game_rotation_vector_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_GAME_ROTATION_VECTOR_CONFIG_VERSION;

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->coordinate_sys = 0;

      //Algorithm-specific constants
      logPtr->abs_rest_gyro_az_tilt_tc = algoStatePtr->abs_rest_gyro_azimuth_tilt_alpha;
      logPtr->rel_rest_gyro_az_tilt_tc = algoStatePtr->rel_rest_gyro_azimuth_tilt_alpha;
      logPtr->motion_gyro_az_tilt_tc = algoStatePtr->motion_gyro_azimuth_tilt_alpha;
      logPtr->gyro_noise_threshold = algoStatePtr->gyro_noise_threshold;
      logPtr->max_gyro_az_tilt_freeze = algoStatePtr->max_gyro_azimuth_tilt_freeze_limit;
      logPtr->max_gyro_az_tilt_reset = algoStatePtr->max_gyro_azimuth_tilt_reset_limit;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GAME_ROTATION_VECTOR_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_tilt_detector_config
  =========================================================================*/
/*!
  @brief log Tilt Detector algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoCfgPtr: pointer to Tilt configuration

  @return Sensors error code
*/
/*=========================================================================*/
static sns_err_code_e sns_sam_log_tilt_detector_config(
   uint8_t algoInstId,
   const tilt_config_s* algoCfgPtr)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_tilt_detector_config_s* logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_TILT_DETECTOR_CONFIG,
                           sizeof(sns_log_tilt_detector_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_TILT_DETECTOR_CONFIG_VERSION;

      logPtr->algo_inst_id = algoInstId;

      //Algorithm-specific constants
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->accel_window_time = algoCfgPtr->accel_window_time;
      logPtr->angle_threshold = algoCfgPtr->angle_threshold;
      logPtr->init_accel_window_time = algoCfgPtr->init_accel_window_time;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_TILT_DETECTOR_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_tilt_detector_result
  =========================================================================*/
/*!
  @brief log Tilt Detector algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
static sns_err_code_e sns_sam_log_tilt_detector_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_tilt_detector_result_s* logPtr;

   tilt_input_s* inputPtr = (tilt_input_s *)(algoInstPtr->inputData.memPtr);
   tilt_state_s* algoPtr = (tilt_state_s *)(algoInstPtr->stateData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_TILT_DETECTOR_RESULT,
                           sizeof(sns_log_tilt_detector_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_TILT_DETECTOR_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      // log inputs
      if (TILT_INPUT_ACCEL == inputPtr->input_type)
      {
        SNS_OS_MEMCOPY(logPtr->accel, inputPtr->input_data.accel, 3);
        logPtr->accel_timestamp = inputPtr->ts;
      }

      // log outputs
      logPtr->dprod_threshold = algoPtr->dprod_threshold;
      SNS_OS_MEMCOPY(logPtr->reference_gravity, algoPtr->reference_grav, 3);

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_TILT_DETECTOR_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_tilt_detector_report
  =========================================================================*/
/*!
  @brief log Tilt Detector algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] outPtr: pointer to tilt's output structure
  @param[i] algoPtr: pointer to tilt's state structure
  @param[i] timeStamp: time stamp for motion state

  @return Sensors error code
*/
/*=========================================================================*/
static sns_err_code_e sns_sam_log_tilt_detector_report(
   const sns_sam_client_req_s* clientReqPtr,
   tilt_output_s * outPtr,
   tilt_state_s * algoPtr,
   uint32_t timeStamp)
{
  sns_err_code_e err = SNS_ERR_FAILED;
  sns_log_tilt_detector_report_s* logPtr;
  uint8_t algoInstId = clientReqPtr->algoInstId;

  //Allocate log packet
  err = sns_logpkt_malloc(SNS_LOG_TILT_DETECTOR_REPORT,
                          sizeof(sns_log_tilt_detector_report_s),
                          (void**)&logPtr);

  if ((err == SNS_SUCCESS) && (logPtr != NULL))
  {
     logPtr->version = SNS_LOG_TILT_DETECTOR_REPORT_VERSION;

     //Generate the log packet
     logPtr->client_id = clientReqPtr->mrClientId;
     logPtr->report_id = clientReqPtr->reportId;
     logPtr->report_type = clientReqPtr->reportType;
     logPtr->algo_inst_id = algoInstId;

     logPtr->event_ts = outPtr->event_ts;
     SNS_OS_MEMCOPY(logPtr->new_reference_gravity, algoPtr->reference_grav, 3);

     logPtr->timestamp = timeStamp;
     logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

     //Commit log (also frees up the log packet memory)
     err = sns_logpkt_commit(SNS_LOG_TILT_DETECTOR_REPORT, logPtr);
  }
  return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_enable_err
  =========================================================================*/
/*!
  @brief Prepare error message to client for algorithm enable request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.
  @param[i] algoSvcId: algorithm service id
  @param[i] errCode: error code

  @return None
*/
/*=======================================================================*/
void sns_sam_prep_algo_enable_err(
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr,
   uint8_t algoSvcId,
   sns_err_code_e errCode)
{
   *clientRespMsgPtr = NULL;

   SNS_SAM_PRINTF2(LOW,
                  "sns_sam_prep_algo_enable_err: algo service id %d, error %d",
                  algoSvcId, errCode);
#ifdef FEATURE_SNS_SAM_FNS
   if (SNS_SAM_FNS_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_fns_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_fns_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_fns_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   if (SNS_SAM_BTE_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_bte_enable_resp_msg_v01 *enableRespMsgPtr =
       sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_bte_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                          enableRespMsgPtr,
                          sizeof(sns_sam_bte_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_basic_gestures_enable_resp_msg_v01 *enableRespMsgPtr =
       sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                          enableRespMsgPtr,
                          sizeof(sns_sam_basic_gestures_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_TAP
   if (SNS_SAM_TAP_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_tap_enable_resp_msg_v01 *enableRespMsgPtr =
       sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                          enableRespMsgPtr,
                          sizeof(sns_sam_tap_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
   if (SNS_SAM_INTEG_ANGLE_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_integ_angle_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_integ_angle_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_integ_angle_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
   if (SNS_SAM_FACING_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_facing_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_facing_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_GYRO_TAP2_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gyro_tap2_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyro_tap2_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_gyro_tap2_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif // FEATURE_SNS_SAM_GYRO_TAP
//#endif /*SNS_SAM_PORT_GESTURE*/

   if (SNS_SAM_SVC_IS_QMD_ALGO(algoSvcId))
   {
      sns_sam_qmd_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_qmd_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      sns_sam_sensor_thresh_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = 0;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_sensor_thresh_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (algoSvcId == SNS_OEM_1_SVC_ID_V01)
   {
      sns_oem_1_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = 0;
    sns_sam_prep_resp_msg(clientReqMsgPtr,
                          enableRespMsgPtr,
                          sizeof(sns_oem_1_enable_resp_msg_v01));
    *clientRespMsgPtr = enableRespMsgPtr;
   }
#endif //CONFIG_USE_OEM_1_ALGO
   else if (algoSvcId == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      sns_sam_modem_scn_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = 0;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_modem_scn_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      sns_sam_gyrobuf_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_gyrobuf_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = 0;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_gyrobuf_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_PED_SVC_ID_V01)
   {
      sns_sam_ped_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD, sizeof(sns_sam_ped_enable_resp_msg_v01) );
      SNS_ASSERT( enableRespMsgPtr != NULL );

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = 0;
      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             enableRespMsgPtr,
                             sizeof(sns_sam_ped_enable_resp_msg_v01) );
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_PAM_SVC_ID_V01)
   {
      sns_sam_pam_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_pam_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_pam_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_CMC_SVC_ID_V01)
   {
      sns_sam_cmc_enable_resp_msg_v01 *enableRespMsgPtr =
         (sns_sam_cmc_enable_resp_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_cmc_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_cmc_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      sns_sam_distance_bound_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_distance_bound_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_distance_bound_enable_resp_msg_v01));
     *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_quat_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_quat_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gravity_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_gravity_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_gravity_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_filtered_mag_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_filtered_mag_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_rotation_vector_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_rotation_vector_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_orientation_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_orientation_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_SMD_SVC_ID_V01)
   {
      sns_sam_smd_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_smd_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_smd_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_game_rotation_vector_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_game_rotation_vector_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_game_rotation_vector_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else if (SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == algoSvcId)
   {
     sns_sam_tilt_detector_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tilt_detector_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      enableRespMsgPtr->resp.sns_err_t = errCode;
      enableRespMsgPtr->instance_id_valid = false;
      enableRespMsgPtr->instance_id = SNS_SAM_INVALID_ID;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_tilt_detector_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
   }
   else
   {
     SNS_SAM_PRINTF1(HIGH,
                     "sns_sam_prep_algo_enable_err: Cannot handle algo service id %d",
                     algoSvcId);
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_disable_err
  =========================================================================*/
/*!
  @brief Prepare error message to client for algorithm disable request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.
  @param[i] algoSvcId: algorithm service id
  @param[i] errCode: error code

  @return None
*/
/*=======================================================================*/
void sns_sam_prep_algo_disable_err(
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr,
   uint8_t algoSvcId,
   sns_err_code_e errCode)
{
   *clientRespMsgPtr = NULL;

   SNS_SAM_PRINTF2(LOW,
                  "sns_sam_prep_algo_disable_err: algo serviceid %d, error %d",
                  algoSvcId, errCode);
#ifdef FEATURE_SNS_SAM_FNS
   if (SNS_SAM_FNS_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_fns_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_fns_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id =
         ((sns_sam_fns_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_fns_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   if (SNS_SAM_BTE_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_bte_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_bte_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id =
         ((sns_sam_bte_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_bte_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_basic_gestures_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_basic_gestures_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_basic_gestures_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
   if (SNS_SAM_TAP_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_tap_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_tap_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_tap_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
   if (SNS_SAM_INTEG_ANGLE_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_integ_angle_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_integ_angle_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_integ_angle_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_integ_angle_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_TILT */

#ifdef FEATURE_SNS_SAM_FACING
   if (SNS_SAM_FACING_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_facing_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_facing_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_facing_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_GYRO_TAP2_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gyro_tap2_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyro_tap2_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_gyro_tap2_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_gyro_tap2_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#endif // FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_SVC_IS_QMD_ALGO(algoSvcId))
   {
      sns_sam_qmd_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id =
         ((sns_sam_qmd_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_qmd_disable_resp_msg_v01));

      if (algoSvcId != SNS_SAM_AMD_SVC_ID_V01 &&
          algoSvcId != SNS_SAM_RMD_SVC_ID_V01)
      {
         sns_sam_motion_client_update(false);
      }
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      sns_sam_sensor_thresh_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id =
         ((sns_sam_sensor_thresh_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_sensor_thresh_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (algoSvcId == SNS_OEM_1_SVC_ID_V01)
   {
      sns_oem_1_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = 1;	  
      disableRespMsgPtr->instance_id =
         ((sns_oem_1_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_oem_1_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;                           
   }
#endif // CONFIG_USE_OEM_1_ALGO
   else if (algoSvcId == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      sns_sam_modem_scn_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id =
         ((sns_sam_modem_scn_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_modem_scn_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      sns_sam_gyrobuf_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_gyrobuf_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id =
         ((sns_sam_gyrobuf_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_gyrobuf_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_PED_SVC_ID_V01)
   {
       sns_sam_ped_disable_resp_msg_v01 *disableRespMsgPtr =
           sns_smr_msg_alloc( SNS_SAM_DBG_MOD, sizeof(sns_sam_ped_disable_resp_msg_v01) );
       SNS_ASSERT( disableRespMsgPtr != NULL );

       disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       disableRespMsgPtr->resp.sns_err_t = errCode;
       disableRespMsgPtr->instance_id_valid = 1;
       disableRespMsgPtr->instance_id =
           ((sns_sam_ped_disable_req_msg_v01 *) clientReqMsgPtr)->instance_id;
       sns_sam_prep_resp_msg( clientReqMsgPtr, disableRespMsgPtr,
                              sizeof(sns_sam_ped_disable_resp_msg_v01) );
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_PAM_SVC_ID_V01)
   {
       sns_sam_pam_disable_resp_msg_v01 *disableRespMsgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_pam_disable_resp_msg_v01));
       SNS_ASSERT(disableRespMsgPtr != NULL);

       disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       disableRespMsgPtr->resp.sns_err_t = errCode;
       disableRespMsgPtr->instance_id_valid = 1;
       disableRespMsgPtr->instance_id =
           ((sns_sam_pam_disable_req_msg_v01 *) clientReqMsgPtr)->instance_id;
       sns_sam_prep_resp_msg(clientReqMsgPtr, disableRespMsgPtr,
                             sizeof(sns_sam_pam_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_CMC_SVC_ID_V01)
   {
       sns_sam_cmc_disable_resp_msg_v01 *disableRespMsgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_cmc_disable_resp_msg_v01));
       SNS_ASSERT(disableRespMsgPtr != NULL);

       disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       disableRespMsgPtr->resp.sns_err_t = errCode;
       disableRespMsgPtr->instance_id_valid = 1;
       disableRespMsgPtr->instance_id =
           ((sns_sam_cmc_disable_req_msg_v01 *) clientReqMsgPtr)->instance_id;
       sns_sam_prep_resp_msg(clientReqMsgPtr, disableRespMsgPtr,
                             sizeof(sns_sam_cmc_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
       sns_sam_distance_bound_disable_resp_msg_v01 *disableRespMsgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_distance_bound_disable_resp_msg_v01));
       SNS_ASSERT(disableRespMsgPtr != NULL);

       disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       disableRespMsgPtr->resp.sns_err_t = errCode;
       disableRespMsgPtr->instance_id_valid = 1;
       disableRespMsgPtr->instance_id =
           ((sns_sam_distance_bound_disable_req_msg_v01 *) clientReqMsgPtr)->instance_id;
       sns_sam_prep_resp_msg(clientReqMsgPtr, disableRespMsgPtr,
                             sizeof(sns_sam_distance_bound_disable_resp_msg_v01));

       *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_quat_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_quat_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_quat_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gravity_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gravity_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_gravity_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_gravity_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if(SNS_SAM_FILTERED_MAG_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_filtered_mag_disable_resp_msg_v01 *disableRespMsgPtr =
      sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_filtered_mag_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_filtered_mag_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;

   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_rotation_vector_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_rotation_vector_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_rotation_vector_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_orientation_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_orientation_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_orientation_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_SMD_SVC_ID_V01)
   {
      sns_sam_smd_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_smd_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_smd_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_smd_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_game_rotation_vector_disable_resp_msg_v01 *disableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                           sizeof(sns_sam_game_rotation_vector_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      disableRespMsgPtr->resp.sns_err_t = errCode;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id =
         ((sns_sam_game_rotation_vector_disable_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_game_rotation_vector_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
   }
   else if (SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == algoSvcId)
      {
     sns_sam_tilt_detector_disable_resp_msg_v01 *disableRespMsgPtr =
            sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                              sizeof(sns_sam_tilt_detector_disable_resp_msg_v01));
         SNS_ASSERT(disableRespMsgPtr != NULL);

         disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
         disableRespMsgPtr->resp.sns_err_t = errCode;
         disableRespMsgPtr->instance_id_valid = true;
         disableRespMsgPtr->instance_id =
            ((sns_sam_tilt_detector_disable_resp_msg_v01 *)clientReqMsgPtr)->instance_id;
         sns_sam_prep_resp_msg(clientReqMsgPtr,
                               disableRespMsgPtr,
                               sizeof(sns_sam_tilt_detector_disable_resp_msg_v01));
         *clientRespMsgPtr = disableRespMsgPtr;
      }
   else
   {
     SNS_SAM_PRINTF1(HIGH,
                     "sns_sam_prep_algo_disable_err: Cannot handle algo service id %d",
                     algoSvcId);
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_report_err
  =========================================================================*/
/*!
  @brief Prepare error message to client for algorithm report request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.
  @param[i] algoSvcId: algorithm service id
  @param[i] errCode: error code

  @return None
*/
/*=======================================================================*/
void sns_sam_prep_algo_report_err(
   const void* clientReqMsgPtr,
   void **clientRespMsgPtr,
   uint8_t algoSvcId,
   sns_err_code_e errCode)
{
   *clientRespMsgPtr = NULL;
   SNS_SAM_PRINTF2(LOW,
                  "sns_sam_prep_algo_report_err: algo serviceid %d, error %d",
                  algoSvcId, errCode);
#ifdef FEATURE_SNS_SAM_FNS
   if (SNS_SAM_FNS_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_fns_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_fns_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id =
         ((sns_sam_fns_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->state = FACE_N_SHAKE_AXIS_UNK_V01;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_fns_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   if (SNS_SAM_BTE_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_bte_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_bte_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id =
         ((sns_sam_bte_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->state = BRING_TO_EAR_UNKNOWN_V01;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_bte_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_basic_gestures_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_basic_gestures_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->state = BASIC_GESTURE_UNKNOWN;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_basic_gestures_get_report_resp_msg_v01));
    *clientRespMsgPtr = reportRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
   if (SNS_SAM_TAP_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_tap_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_tap_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->state = TAP_UNKNOWN;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_tap_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_TAP */

       /* Tilt does not have a get report response  */

#ifdef FEATURE_SNS_SAM_FACING
   if (SNS_SAM_FACING_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_facing_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_facing_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->state = FACING_UNKNOWN;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_facing_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
#endif /* FEATURE_SNS_SAM_FACING */
   if (SNS_SAM_SVC_IS_QMD_ALGO(algoSvcId))
   {
      sns_sam_qmd_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id =
         ((sns_sam_qmd_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->state = SNS_SAM_MOTION_UNKNOWN_V01;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_qmd_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      sns_sam_sensor_thresh_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      /* respond with instance ID, other optional fields are not needed */
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id =
         ((sns_sam_sensor_thresh_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = 0;
      reportRespMsgPtr->sample_value_valid = 0;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_sensor_thresh_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (algoSvcId == SNS_OEM_1_SVC_ID_V01)
   {
      sns_oem_1_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      /* respond with instance ID, other optional fields are not needed */
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id =
         ((sns_oem_1_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = 0;
      reportRespMsgPtr->output_valid = 0;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_oem_1_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;                            
   }
#endif //CONFIG_USE_OEM_1_ALGO
   else if (algoSvcId == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      sns_sam_modem_scn_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      /* respond with instance ID, other optional fields are not needed */
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id =
         ((sns_sam_modem_scn_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = 0;
      reportRespMsgPtr->scenario_valid = 0;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_modem_scn_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      sns_sam_gyrobuf_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyrobuf_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      /* respond with instance ID, other optional fields are not needed */
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id =
         ((sns_sam_gyrobuf_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_gyrobuf_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_PED_SVC_ID_V01 )
   {
      sns_sam_ped_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD, sizeof(sns_sam_ped_get_report_resp_msg_v01) );
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      /*respond with instace ID, other optional feilds are not needed*/
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id =
          ((sns_sam_ped_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = 0;
      reportRespMsgPtr->report_data_valid = 0;
      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             reportRespMsgPtr,
                             sizeof(sns_sam_ped_get_report_resp_msg_v01) );
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if(algoSvcId == SNS_SAM_PAM_SVC_ID_V01)
   {
       sns_sam_pam_get_report_resp_msg_v01 *reportRespMsgPtr =
          sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_pam_get_report_resp_msg_v01));
       SNS_ASSERT(reportRespMsgPtr != NULL);

       reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       reportRespMsgPtr->resp.sns_err_t = errCode;
       /*respond with instace ID, other optional feilds are not needed*/
      reportRespMsgPtr->instance_id_valid = 1;
       reportRespMsgPtr->instance_id =
           ((sns_sam_pam_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
       sns_sam_prep_resp_msg(clientReqMsgPtr,
                             reportRespMsgPtr,
                             sizeof(sns_sam_pam_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if(algoSvcId == SNS_SAM_CMC_SVC_ID_V01)
   {
       sns_sam_cmc_get_report_resp_msg_v01 *reportRespMsgPtr =
          sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_cmc_get_report_resp_msg_v01));
       SNS_ASSERT(reportRespMsgPtr != NULL);

       reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       reportRespMsgPtr->resp.sns_err_t = errCode;
       /*respond with instace ID, other optional feilds are not needed*/
       reportRespMsgPtr->instance_id_valid = 1;
       reportRespMsgPtr->instance_id =
           ((sns_sam_cmc_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
       sns_sam_prep_resp_msg(clientReqMsgPtr,
                             reportRespMsgPtr,
                             sizeof(sns_sam_cmc_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if(algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
       sns_sam_distance_bound_get_report_resp_msg_v01 *reportRespMsgPtr =
          sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_distance_bound_get_report_resp_msg_v01));
       SNS_ASSERT(reportRespMsgPtr != NULL);

       reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
       reportRespMsgPtr->resp.sns_err_t = errCode;
       /*respond with instace ID, other optional feilds are not needed*/
       reportRespMsgPtr->instance_id_valid = 1;
       reportRespMsgPtr->instance_id =
           ((sns_sam_distance_bound_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
       sns_sam_prep_resp_msg(clientReqMsgPtr,
                             reportRespMsgPtr,
                             sizeof(sns_sam_distance_bound_get_report_resp_msg_v01));
       *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_quat_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_quat_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->result_valid = false;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_quat_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gravity_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gravity_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_gravity_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->result_valid = false;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_gravity_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_filtered_mag_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_filtered_mag_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->result_valid = false;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_filtered_mag_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_rotation_vector_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_rotation_vector_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->result_valid = false;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_rotation_vector_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_orientation_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_orientation_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->result_valid = false;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_orientation_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (algoSvcId == SNS_SAM_SMD_SVC_ID_V01)
   {
      sns_sam_smd_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_smd_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_smd_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_smd_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_game_rotation_vector_get_report_resp_msg_v01 *reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_game_rotation_vector_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      reportRespMsgPtr->resp.sns_err_t = errCode;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id =
         ((sns_sam_game_rotation_vector_get_report_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = sns_em_get_timestamp();
      reportRespMsgPtr->result_valid = false;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_game_rotation_vector_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
   }
   else
   {
     SNS_SAM_PRINTF1(HIGH,
                     "sns_sam_prep_algo_report_err: Cannot handle algo service id %d",
                     algoSvcId);
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_update_err
  =========================================================================*/
/*!
  @brief Send error message to client for algorithm update request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.
  @param[i] algoSvcId: algorithm service id
  @param[i] errCode: error code

  @return None
*/
/*=======================================================================*/
void sns_sam_prep_algo_update_err(
   const void* clientReqMsgPtr,
   void **clientRespMsgPtr,
   uint8_t algoSvcId,
   sns_err_code_e errCode)
{
   *clientRespMsgPtr = NULL;
   SNS_SAM_PRINTF2(LOW,
                   "sns_sam_prep_algo_update_err: algo serviceid %d, error %d",
                   algoSvcId, errCode);
   if( algoSvcId == SNS_SAM_PED_SVC_ID_V01 )
   {
      sns_sam_ped_reset_resp_msg_v01 *resetRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_ped_reset_resp_msg_v01) );
      SNS_ASSERT( resetRespMsgPtr != NULL );

      resetRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      resetRespMsgPtr->resp.sns_err_t = errCode;
      /*respond with instace ID, other optional fields are not needed*/
      resetRespMsgPtr->instance_id_valid = 1;
      resetRespMsgPtr->instance_id =
           ((sns_sam_ped_reset_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             resetRespMsgPtr,
                             sizeof(sns_sam_ped_reset_resp_msg_v01) );
      *clientRespMsgPtr = resetRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01 )
   {
      sns_sam_distance_bound_set_bound_resp_msg_v01 *setboundRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_distance_bound_set_bound_resp_msg_v01) );
      SNS_ASSERT( setboundRespMsgPtr != NULL );

      setboundRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      setboundRespMsgPtr->resp.sns_err_t = errCode;

      /*respond with instace ID, other optional fields are not needed*/
      setboundRespMsgPtr->instance_id_valid = 1;
      setboundRespMsgPtr->instance_id =
           ((sns_sam_distance_bound_set_bound_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             setboundRespMsgPtr,
                             sizeof(sns_sam_distance_bound_set_bound_resp_msg_v01) );
      *clientRespMsgPtr = setboundRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_CMC_SVC_ID_V01 )
   {
      sns_sam_cmc_update_reporting_resp_msg_v01 *updateRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_cmc_update_reporting_resp_msg_v01) );
      SNS_ASSERT( updateRespMsgPtr != NULL );

      updateRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      updateRespMsgPtr->resp.sns_err_t = errCode;

      /*respond with instace ID, other optional fields are not needed*/
      updateRespMsgPtr->instance_id_valid = 1;
      updateRespMsgPtr->instance_id =
           ((sns_sam_cmc_update_reporting_resp_msg_v01 *)clientReqMsgPtr)->instance_id;
      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             updateRespMsgPtr,
                             sizeof(sns_sam_cmc_update_reporting_resp_msg_v01) );
      *clientRespMsgPtr = updateRespMsgPtr;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_batch_err
  =========================================================================*/
/*!
  @brief Send error message to client for algorithm batch request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.  
  @param[i] algoSvcId: algorithm service id
  @param[i] errCode: error code

  @return None
*/
/*=======================================================================*/
void sns_sam_prep_algo_batch_err(
   const void* clientReqMsgPtr,
   void **clientRespMsgPtr,
   uint8_t algoSvcId,
   sns_err_code_e errCode)
{
   if( algoSvcId == SNS_SAM_PED_SVC_ID_V01 )
   {
      sns_sam_ped_batch_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_ped_batch_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = 
           ((sns_sam_ped_batch_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 0;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_ped_batch_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 )
   {
      sns_sam_gravity_batch_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_gravity_batch_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = 
           ((sns_sam_gravity_batch_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 0;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_gravity_batch_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_game_rotation_vector_batch_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_game_rotation_vector_batch_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = 
           ((sns_sam_game_rotation_vector_batch_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 0;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_game_rotation_vector_batch_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_rotation_vector_batch_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_rotation_vector_batch_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = 
           ((sns_sam_rotation_vector_batch_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 0;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_rotation_vector_batch_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_ORIENTATION_SVC_ID_V01 )
   {
      sns_sam_orientation_batch_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_orientation_batch_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = 
           ((sns_sam_orientation_batch_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 0;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_orientation_batch_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01 )
   {
     sns_sam_tilt_detector_batch_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_tilt_detector_batch_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_tilt_detector_batch_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 0;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_tilt_detector_batch_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_CMC_SVC_ID_V01 )
   {
      sns_sam_cmc_batch_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_cmc_batch_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = 
           ((sns_sam_cmc_batch_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 0;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_cmc_batch_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_update_batch_period_err
  =========================================================================*/
/*!
  @brief Send error message to client for algorithm update batch period req

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.
  @param[i] algoSvcId: algorithm service id
  @param[i] errCode: error code

  @return None
*/
/*=======================================================================*/
void sns_sam_prep_algo_update_batch_period_err(
   const void* clientReqMsgPtr,
   void **clientRespMsgPtr,
   uint8_t algoSvcId,
   sns_err_code_e errCode)
{
   if( algoSvcId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_game_rv_update_batch_period_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_game_rv_update_batch_period_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_game_rv_update_batch_period_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_game_rv_update_batch_period_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 )
   {
      sns_sam_grav_update_batch_period_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_grav_update_batch_period_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_grav_update_batch_period_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_grav_update_batch_period_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_ORIENTATION_SVC_ID_V01 )
   {
      sns_sam_orient_update_batch_period_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_orient_update_batch_period_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_orient_update_batch_period_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_orient_update_batch_period_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_PED_SVC_ID_V01 )
   {
      sns_sam_ped_update_batch_period_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_ped_update_batch_period_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_ped_update_batch_period_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_ped_update_batch_period_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_rot_vec_update_batch_period_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_rot_vec_update_batch_period_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_rot_vec_update_batch_period_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_rot_vec_update_batch_period_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01 )
   {
     sns_sam_tilt_detector_update_batch_period_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_tilt_detector_update_batch_period_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_tilt_detector_update_batch_period_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_tilt_detector_update_batch_period_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
   else if( algoSvcId == SNS_SAM_CMC_SVC_ID_V01 )
   {
      sns_sam_cmc_update_batch_period_resp_msg_v01 *batchRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_cmc_update_batch_period_resp_msg_v01) );
      SNS_ASSERT( batchRespMsgPtr != NULL );

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      batchRespMsgPtr->resp.sns_err_t = errCode;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id =
           ((sns_sam_cmc_update_batch_period_req_msg_v01 *)clientReqMsgPtr)->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg( clientReqMsgPtr,
                             batchRespMsgPtr,
                             sizeof(sns_sam_cmc_update_batch_period_resp_msg_v01) );
      *clientRespMsgPtr = batchRespMsgPtr;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_algo_version_resp
  =========================================================================*/
/*!
  @brief Send response message to client for algorithm version request

  @param[i] algoSvcId: algorithm service id
  @param[o] versionRespMsgPtr: pointer to version response message

  @return None
*/
/*=======================================================================*/
void sns_sam_get_algo_version_resp(
   uint8_t algoSvcId,
   sns_common_version_resp_msg_v01 *versionRespMsgPtr)
{
   SNS_SAM_PRINTF1(LOW, "sns_sam_get_algo_version_resp: algo service id %d",
                   algoSvcId);

   switch (algoSvcId)
   {
#ifdef FEATURE_SNS_SAM_FNS
   case SNS_SAM_FNS_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_FNS_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_FNS_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif /* FEATURE_SNS_SAM_FNS */

#ifdef FEATURE_SNS_SAM_BTE
   case SNS_SAM_BTE_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_BTE_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_BTE_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif /* FEATURE_SNS_SAM_BTE */

#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_BASIC_GESTURES_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_BASIC_GESTURES_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_TAP
   case SNS_SAM_TAP_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_TAP_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_TAP_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif /* FEATURE_SNS_SAM_TAP */

#ifdef FEATURE_SNS_SAM_TILT
   case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_INTEG_ANGLE_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_INTEG_ANGLE_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif /* FEATURE_SNS_SAM_TILT */

#ifdef FEATURE_SNS_SAM_FACING
   case SNS_SAM_FACING_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_FACING_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_FACING_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_GYRO_TAP2_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_GYRO_TAP2_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif // FEATURE_SNS_SAM_GYRO_TAP

   case SNS_SAM_AMD_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_AMD_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_AMD_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_VMD_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_VMD_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_VMD_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_RMD_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_RMD_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_RMD_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_SENSOR_THRESH_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_SENSOR_THRESH_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_SENSOR_THRESH_SVC_V01_MAX_MESSAGE_ID;
      break;

#ifdef CONFIG_USE_OEM_1_ALGO	  
   case SNS_OEM_1_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_OEM_1_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_OEM_1_SVC_V01_MAX_MESSAGE_ID;
      break;
#endif //CONFIG_USE_OEM_1_ALGO

   case SNS_SAM_MODEM_SCN_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_MODEM_SCN_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_MODEM_SCN_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_PED_SVC_ID_V01:
       versionRespMsgPtr->interface_version_number =
          SNS_SAM_PED_SVC_V01_IDL_MINOR_VERS;
       versionRespMsgPtr->max_message_id = SNS_SAM_PED_SVC_V01_MAX_MESSAGE_ID;
       break;

   case SNS_SAM_PAM_SVC_ID_V01:
       versionRespMsgPtr->interface_version_number =
          SNS_SAM_PAM_SVC_V01_IDL_MINOR_VERS;
       versionRespMsgPtr->max_message_id = SNS_SAM_PAM_SVC_V01_MAX_MESSAGE_ID;
       break;

   case SNS_SAM_CMC_SVC_ID_V01:
       versionRespMsgPtr->interface_version_number =
          SNS_SAM_CMC_SVC_V01_IDL_MINOR_VERS;
       versionRespMsgPtr->max_message_id = SNS_SAM_CMC_SVC_V01_MAX_MESSAGE_ID;
       break;

   case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
       versionRespMsgPtr->interface_version_number =
          SNS_SAM_DISTANCE_BOUND_SVC_V01_IDL_MINOR_VERS;
       versionRespMsgPtr->max_message_id = SNS_SAM_DISTANCE_BOUND_SVC_V01_MAX_MESSAGE_ID;
       break;

   case SNS_SAM_QUATERNION_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_QUATERNION_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_QUATERNION_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_GRAVITY_VECTOR_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_GRAVITY_VECTOR_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_FILTERED_MAG_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_FILTERED_MAG_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_ROTATION_VECTOR_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_ROTATION_VECTOR_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_ORIENTATION_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_ORIENTATION_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_ORIENTATION_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_SMD_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
        SNS_SAM_SMD_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_SMD_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_GAME_ROTATION_VECTOR_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_GAME_ROTATION_VECTOR_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_GYROBUF_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
         SNS_SAM_GYROBUF_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_GYROBUF_SVC_V01_MAX_MESSAGE_ID;
      break;

   case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      versionRespMsgPtr->interface_version_number =
          SNS_SAM_TILT_DETECTOR_SVC_V01_IDL_MINOR_VERS;
      versionRespMsgPtr->max_message_id = SNS_SAM_TILT_DETECTOR_SVC_V01_MAX_MESSAGE_ID;
      break;

   default:
      versionRespMsgPtr->interface_version_number = 0;
      versionRespMsgPtr->max_message_id = 0;
      SNS_SAM_DEBUG1(ERROR, DBG_SAM_PROCESS_REQ_INVALID_ALGOREQ, algoSvcId);
      break;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_algo_attrib_resp
  =========================================================================*/
/*!
  @brief Send response message to client for algorithm attributes request

  @param[i] algoSvcId: algorithm service id
  @param[o] attribRespMsgPtr: pointer to attributes query response message

  @return SNS_SUCCESS, if successful
          SNS_ERR_BAD_MSG_ID, if request is unsupported
          SNS_ERR_BAD_PARM, if service is unsupported
*/
/*=======================================================================*/
sns_err_code_e sns_sam_get_algo_attrib_resp(
   sns_sam_algo_s* algoPtr,
   sns_sam_get_algo_attrib_resp_msg_v01 *attribRespMsgPtr)
{
   sns_err_code_e err = SNS_ERR_BAD_PARM;
   if( !algoPtr )
   {
      return err;
   }

   SNS_SAM_PRINTF1(LOW, "sns_sam_get_algo_attrib_resp: algo service id %d",
                   algoPtr->serviceId);

   switch( algoPtr->serviceId )
   {
#ifdef FEATURE_SNS_SAM_FNS
   case SNS_SAM_FNS_SVC_ID_V01:
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   case SNS_SAM_BTE_SVC_ID_V01:
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
   case SNS_SAM_TAP_SVC_ID_V01:
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
   case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
   case SNS_SAM_FACING_SVC_ID_V01:
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
#endif // FEATURE_SNS_SAM_GYRO_TAP
   case SNS_SAM_AMD_SVC_ID_V01:
   case SNS_SAM_VMD_SVC_ID_V01:
   case SNS_SAM_RMD_SVC_ID_V01:
   case SNS_SAM_SENSOR_THRESH_SVC_ID_V01:
   case SNS_SAM_PED_SVC_ID_V01:
   case SNS_SAM_PAM_SVC_ID_V01:
   case SNS_SAM_CMC_SVC_ID_V01:
   case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
   case SNS_SAM_QUATERNION_SVC_ID_V01:
   case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
   case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
   case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
   case SNS_SAM_ORIENTATION_SVC_ID_V01:
   case SNS_SAM_SMD_SVC_ID_V01:
   case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
   case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      err = SNS_SUCCESS;
      attribRespMsgPtr->algorithm_revision = algoPtr->algorithm_revision;
      attribRespMsgPtr->proc_type = SNS_PROC_SSC_V01;
      attribRespMsgPtr->supported_reporting_modes = algoPtr->supported_reporting_modes;
      attribRespMsgPtr->min_report_rate = algoPtr->min_report_rate;
      attribRespMsgPtr->max_report_rate = algoPtr->max_report_rate;
      attribRespMsgPtr->min_sample_rate = algoPtr->min_sample_rate;
      attribRespMsgPtr->max_sample_rate = algoPtr->max_sample_rate;
      attribRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      attribRespMsgPtr->power = algoPtr->power;
      break;

   default:
      err = SNS_ERR_BAD_MSG_ID;
      attribRespMsgPtr->algorithm_revision = 0;
      attribRespMsgPtr->proc_type = 0;
      attribRespMsgPtr->supported_reporting_modes = 0;
      attribRespMsgPtr->min_report_rate = 0;
      attribRespMsgPtr->max_report_rate = 0;
      attribRespMsgPtr->min_sample_rate = 0;
      attribRespMsgPtr->max_sample_rate = 0;
      attribRespMsgPtr->max_batch_size = 0;
      attribRespMsgPtr->power = 0;
      SNS_SAM_PRINTF1(ERROR, "sns_sam_get_algo_attrib_resp: algo service id %d not supported",
                   algoPtr->serviceId);
      break;
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_qmd_report
  =========================================================================*/
/*!
  @brief log QMD algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] motionState: motion state
  @param[i] timeStamp: time stamp for motion state
  @param[i] algoSvcId: algorithm service id

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_qmd_report(
   const sns_sam_client_req_s* clientReqPtr,
   int32_t motionState,
   uint32_t timeStamp,
   uint8_t algoSvcId)
{
   sns_err_code_e err;
   sns_log_qmd_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QMD_REPORT,
                           sizeof(sns_log_qmd_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_QMD_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      logPtr->algo_service_id = algoSvcId;
      logPtr->qmd_result = (uint8_t)motionState;

      //Timestamp with report time
      logPtr->timestamp = timeStamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QMD_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_ped_report
  =========================================================================*/
/*!
  @brief log Pedometer algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] clientData: output generated for this client

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_ped_report(
   const sns_sam_client_req_s* clientReqPtr,
   const ped_client_output_s* clientOutput)
{
   sns_err_code_e err;
   sns_log_ped_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_PED_REPORT,
                           sizeof(sns_log_ped_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_PED_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      logPtr->step_event = clientOutput->step_event;
      logPtr->step_confidence = clientOutput->step_confidence;
      logPtr->step_count = clientOutput->step_count;
      logPtr->step_count_error = clientOutput->step_count_error;
      logPtr->step_rate = clientOutput->step_rate;

      //Timestamp with report time
      logPtr->timestamp = clientOutput->timestamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_PED_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_pam_report
  =========================================================================*/
/*!
  @brief log PAM algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] result: pam result
  @param[i] timeStamp: time stamp for pam report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_pam_report(
   const sns_sam_client_req_s* clientReqPtr,
   int32_t step_count,
   uint32_t timeStamp)
{
   sns_err_code_e err;
   sns_log_pam_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_PAM_REPORT,
                           sizeof(sns_log_pam_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_PAM_REPORT_VERSION;

      //Generate the log packet
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = timeStamp;
      logPtr->step_count = step_count;
      logPtr->algo_inst_id = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_PAM_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_cmc_report
  =========================================================================*/
/*!
  @brief log CMC algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] motion_state: motion state of the device
  @param[i] motion_event: motion state event
  @param[i] motion_state_probability: confidence in motion state detected
  @param[i] timeStamp: time stamp for cmc report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_cmc_report(
   const sns_sam_client_req_s* clientReqPtr,
   sns_sam_cmc_ms_e_v01 motion_state,
   sns_sam_cmc_ms_event_e_v01 motion_event,
   float motion_state_probability,
   uint32_t timeStamp)
{
   sns_err_code_e err;
   sns_log_cmc_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_CMC_REPORT,
                           sizeof(sns_log_cmc_report_s),
                           (void**)&logPtr);


   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_CMC_REPORT_VERSION;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = timeStamp;

      //logPtr->output_gyro_on = gyro_on;
      logPtr->motion_state = motion_state;
      logPtr->motion_state_event = motion_event;
      logPtr->motion_state_probability = motion_state_probability;

      logPtr->algo_inst_id = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_CMC_REPORT, logPtr);
   }
   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_distance_bound_report
  =========================================================================*/
/*!
  @brief log Distance Bound algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] motion_state: motion state of the device
  @param[i] motion_state_probability: confidence in motion state detected
  @param[i] timeStamp: time stamp for distance_bound report
  @param[i] sessionkey: session_key for distance_bound report
 
  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_distance_bound_report(
   const sns_sam_client_req_s* clientReqPtr,
   sns_sam_distance_bound_breach_event_e_v01 is_distance_bound_breached,
   uint32_t timeStamp,
   uint8_t sessionkey)
{
   sns_err_code_e err;
   sns_log_distance_bound_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_DISTANCE_BOUND_REPORT,
                           sizeof(sns_log_distance_bound_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_DISTANCE_BOUND_REPORT_VERSION;

      //Generate the log packet
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = timeStamp;
      logPtr->algo_inst_id = algoInstId;
      logPtr->distance_bound_breach_event = is_distance_bound_breached;
      logPtr->session_key = sessionkey;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_DISTANCE_BOUND_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_gyro_quat_report
  =========================================================================*/
/*!
  @brief log Quaternion algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] result: gyro quaternion result
  @param[i] timeStamp: time stamp for gyro quaternion report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_gyro_quat_report(
   const sns_sam_client_req_s* clientReqPtr,
   sns_sam_quat_result_s_v01 *result,
   uint32_t timeStamp)
{
   sns_err_code_e err;
   sns_log_quaternion_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QUAT_REPORT,
                           sizeof(sns_log_quaternion_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_QUATERNION_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      SNS_OS_MEMCOPY(logPtr->quaternion, result->quaternion, sizeof(result->quaternion));

      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QUAT_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_gyro_quat_result
  =========================================================================*/
/*!
  @brief log Quaternion algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_gyro_quat_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_quaternion_result_s* logPtr;
   gyro_quat_input_s* inputPtr =
      (gyro_quat_input_s *)(algoInstPtr->inputData.memPtr);
   gyro_quat_output_s* outputPtr =
      (gyro_quat_output_s *)(algoInstPtr->outputData.memPtr);
   gyro_quat_state_s* algoPtr =
      (gyro_quat_state_s *)(algoInstPtr->stateData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QUAT_RESULT,
                           sizeof(sns_log_quaternion_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_QUATERNION_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;
      logPtr->gyro_x = inputPtr->gyro[0];
      logPtr->gyro_y = inputPtr->gyro[1];
      logPtr->gyro_z = inputPtr->gyro[2];
      SNS_OS_MEMCOPY(logPtr->quaternion, outputPtr->quat, sizeof(outputPtr->quat));
      logPtr->delta_t = FX_FLTTOFIX_Q16(algoPtr->sample_period);

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QUAT_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_gyro_quat_config
  =========================================================================*/
/*!
  @brief log Quaternion algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_gyro_quat_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err;
   sns_log_quaternion_config_s* logPtr;
   gyro_quat_config_s *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QUAT_CONFIG,
                           sizeof(sns_log_quaternion_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_QUATERNION_CONFIG_VERSION;
      algoCfgPtr = (gyro_quat_config_s *)(algoInstPtr->configData.memPtr);

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QUAT_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_gravity_report
  =========================================================================*/
/*!
  @brief log Gravity algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] result: gravity result
  @param[i] timeStamp: time stamp for gravity report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_gravity_report(
   const sns_sam_client_req_s* clientReqPtr,
   sns_sam_gravity_result_s_v01 *result,
   uint32_t timeStamp)
{
   sns_err_code_e err;
   sns_log_gravity_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GRAVITY_REPORT,
                           sizeof(sns_log_gravity_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_GRAVITY_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      SNS_OS_MEMCOPY(logPtr->gravity, result->gravity, sizeof(result->gravity));
      SNS_OS_MEMCOPY(logPtr->lin_accel, result->lin_accel, sizeof(result->lin_accel));

      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GRAVITY_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_gravity_result
  =========================================================================*/
/*!
  @brief log Gravity algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_gravity_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_gravity_result_s* logPtr;
   gravity_state_s* algoPtr =
      (gravity_state_s *)(algoInstPtr->stateData.memPtr);
   gravity_output_s* outputPtr =
      (gravity_output_s *)(algoInstPtr->outputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GRAVITY_RESULT,
                           sizeof(sns_log_gravity_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_GRAVITY_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      //Log algorithm input
      logPtr->quaternion_x = algoPtr->quat[1];
      logPtr->quaternion_y = algoPtr->quat[2];
      logPtr->quaternion_z = algoPtr->quat[3];
      logPtr->accel_x = FX_FLTTOFIX_Q16(algoPtr->accel[0]);
      logPtr->accel_y = FX_FLTTOFIX_Q16(algoPtr->accel[1]);
      logPtr->accel_z = FX_FLTTOFIX_Q16(algoPtr->accel[2]);
      logPtr->delta_t = (int32_t)(algoPtr->quat_samp_per);

      logPtr->alpha = FX_FLTTOFIX_Q16(algoPtr->filter_alpha[algoPtr->dev_state]);
      switch (algoPtr->dev_state)
      {
      case GRAVITY_DEV_ABS_REST:
         logPtr->abs_rest_state = SNS_SAM_MOTION_REST_V01;
         logPtr->rel_rest_state = SNS_SAM_MOTION_REST_V01;
         break;
      case GRAVITY_DEV_REL_REST:
         logPtr->abs_rest_state = SNS_SAM_MOTION_MOVE_V01;
         logPtr->rel_rest_state = SNS_SAM_MOTION_REST_V01;
         break;
      case GRAVITY_DEV_MOTION:
         logPtr->abs_rest_state = SNS_SAM_MOTION_MOVE_V01;
         logPtr->rel_rest_state = SNS_SAM_MOTION_MOVE_V01;
         break;
      case GRAVITY_DEV_STATE_UNKNOWN:
      default:
         logPtr->abs_rest_state = SNS_SAM_MOTION_UNKNOWN_V01;
         logPtr->rel_rest_state = SNS_SAM_MOTION_UNKNOWN_V01;
         break;
      }

      //Log algorithm output
      SNS_OS_MEMCOPY(logPtr->gravity, algoPtr->gravity, sizeof(algoPtr->gravity));
      SNS_OS_MEMCOPY(logPtr->lin_accel, outputPtr->lin_accel, sizeof(outputPtr->lin_accel));

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GRAVITY_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_gravity_config
  =========================================================================*/
/*!
  @brief log Gravity algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_gravity_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err;
   sns_log_gravity_config_s* logPtr;
   gravity_config_s *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GRAVITY_CONFIG,
                           sizeof(sns_log_gravity_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_GRAVITY_CONFIG_VERSION;
      algoCfgPtr = (gravity_config_s *)(algoInstPtr->configData.memPtr);

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->time_const_abs = algoCfgPtr->filter_tc[GRAVITY_DEV_ABS_REST];
      logPtr->time_const_rel = algoCfgPtr->filter_tc[GRAVITY_DEV_REL_REST];
      logPtr->time_const_mot = algoCfgPtr->filter_tc[GRAVITY_DEV_MOTION];
      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GRAVITY_CONFIG, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_log_fmv_config
  =========================================================================*/
/*!
  @brief log Filtered magnetic vector algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_fmv_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err;
   sns_log_fmv_config_s* logPtr;
   fmv_config_s *algoCfgPtr;
   uint8_t i;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FMV_CONFIG,
                           sizeof(sns_log_fmv_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_FMV_CONFIG_VERSION;

      algoCfgPtr = (fmv_config_s *) (algoInstPtr->configData.memPtr);

      logPtr->algo_inst_id   = algoInstId;

      logPtr->sample_rate         = algoCfgPtr->sample_rate;
      logPtr->report_period       = algoCfgPtr->report_period;
      logPtr->num_accuracy_bins   = SNS_FMV_LOG_NUM_ACCURACY_BINS;

      for (i = 0; i < SNS_FMV_LOG_NUM_ACCURACY_BINS; i++)
      {
        logPtr->accuracy_bin_tcs[i] = (float)(algoCfgPtr->fmv_tcs[i]);
      }
      logPtr->gyro_gap_thresh       = (float)(algoCfgPtr->max_gyro_sample_gap_tolerance);
      logPtr->mag_gap_thresh        = (float)(algoCfgPtr->max_mag_sample_gap_factor);
      logPtr->min_turn_rate         = (float)(algoCfgPtr->min_turn_rate);
      logPtr->max_mag_innovation    = (float)(algoCfgPtr->max_mag_innovation);

      //Timestamp
      logPtr->timestamp      = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FMV_CONFIG, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_log_fmv_report
  =========================================================================*/
sns_err_code_e sns_sam_log_fmv_report(
   const sns_sam_filtered_mag_result_s_v01 *resultPtr,
   const sns_sam_client_req_s* clientReqPtr,
   uint32_t ts)
{
   sns_err_code_e err;
   sns_log_fmv_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FMV_REPORT,
                           sizeof(sns_log_fmv_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_FMV_REPORT_VERSION;

      //Generate the log packet
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->client_id = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);

      logPtr->fmv[0] = (float)(resultPtr->filtered_mag[0]);
      logPtr->fmv[1] = (float)(resultPtr->filtered_mag[1]);
      logPtr->fmv[2] = (float)(resultPtr->filtered_mag[2]);
      logPtr->accuracy = resultPtr->accuracy;

      logPtr->algo_inst_id = algoInstId;
      logPtr->timestamp = ts;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FMV_REPORT, logPtr);
   }
   else
   {
      SNS_SAM_PRINTF2(LOW,
                      "sns_sam_app: Could not log report err = %d, log_ptr %d!",
                      err, (intptr_t)logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_log_fmv_quat_update
  =========================================================================*/
sns_err_code_e sns_sam_log_fmv_quat_update(
   uint8_t algoInstId,
   const fmv_input_s *fmvInputPtr,
   const fmv_output_s *fmvOutPtr)
{
   sns_err_code_e err;
   uint8_t i;
   sns_log_fmv_quat_update_s* logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FMV_QUAT_UPDATE,
                           sizeof(sns_log_fmv_quat_update_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_FMV_QUAT_UPDATE_VERSION;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      for(i = 0; i < 4; i++)
      {
         logPtr->q[i] = (float)(fmvInputPtr->data.q[i]);
      }
      logPtr->timestamp_quat = fmvInputPtr->ts;

      for(i = 0; i < 3; i++)
      {
         logPtr->fmv[i] = (float)(fmvOutPtr->filtered_mag[i]);
      }
      logPtr->accuracy =   fmvOutPtr->accuracy;
      logPtr->timestamp      = fmvOutPtr->ts;

      logPtr->algo_inst_id  = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FMV_QUAT_UPDATE, logPtr);
   }
   else
   {
      SNS_SAM_PRINTF2(LOW,
                      "sns_sam_app: Could not log report err = %d, log_ptr %d!",
                      err, (intptr_t)logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_log_fmv_mag_update
  =========================================================================*/
sns_err_code_e sns_sam_log_fmv_mag_update(
   uint8_t algoInstId,
   const fmv_input_s *fmvInputPtr,
   const fmv_output_s *fmvOutPtr)
{
   sns_err_code_e err;
   uint8_t i;
   sns_log_fmv_mag_update_s* logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FMV_MAG_UPDATE,
                           sizeof(sns_log_fmv_mag_update_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_FMV_MAG_UPDATE_VERSION;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      logPtr->timestamp_mag  = fmvInputPtr->ts;
      logPtr->timestamp      = fmvOutPtr->ts;

      for(i = 0; i < 3; i++)
      {
         /* the input */
         logPtr->b[i] = (float)(fmvInputPtr->data.calib_mag[i]);

      /* The output it produced */
         logPtr->fmv[i]      = (float)(fmvOutPtr->filtered_mag[i]);

         /* Gyro data : unused - so set it to zero */
         logPtr->gyro[i]   = 0.0;
      }

      logPtr->accuracy       =   fmvOutPtr->accuracy;
      logPtr->timestamp      = fmvOutPtr->ts;

      logPtr->algo_inst_id  = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FMV_MAG_UPDATE, logPtr);
   }
   else
   {
      SNS_SAM_PRINTF2(LOW,
                      "sns_sam_app: Could not log report err = %d, log_ptr %d!",
                      err, (intptr_t)logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_log_fmv_gyro_update
  =========================================================================*/
sns_err_code_e sns_sam_log_fmv_gyro_update(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   uint8_t i;
   sns_log_fmv_gyro_update_s* logPtr;
   fmv_input_s *fmvInputPtr = (fmv_input_s *)(algoInstPtr->inputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_FMV_GYRO_UPDATE,
                           sizeof(sns_log_fmv_gyro_update_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_FMV_GYRO_UPDATE_VERSION;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      /* the input */
      for(i = 0; i < 3; i++)
      {
         logPtr->gyro[i] = (float)(fmvInputPtr->data.gyro[i]);
      }
      logPtr->timestamp  = fmvInputPtr->ts;

      /* The Gyro update produces no output */
      logPtr->algo_inst_id  = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_FMV_GYRO_UPDATE, logPtr);
   }
   else
   {
      SNS_SAM_PRINTF2(LOW,
                      "sns_sam_app: Could not log report err = %d, log_ptr %d!",
                      err, (intptr_t)logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_rotation_vector_report
  =========================================================================*/
/*!
  @brief log Gravity algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] result: gravity result
  @param[i] timeStamp: time stamp for gravity report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_rotation_vector_report(
   const sns_sam_client_req_s* clientReqPtr,
   sns_sam_rotation_vector_result_s_v01 *result,
   uint32_t timeStamp)
{
   sns_err_code_e err = SNS_ERR_FAILED;

   sns_log_rotation_vector_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_ROTATION_VECTOR_REPORT,
                           sizeof(sns_log_rotation_vector_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_ROTATION_VECTOR_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      SNS_OS_MEMCOPY(logPtr->rotation_vector, result->rotation_vector, sizeof(result->rotation_vector));
      logPtr->accuracy = result->accuracy;
      logPtr->coordinate_sys = result->coordinate_sys;

      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_ROTATION_VECTOR_REPORT, logPtr);
   }
   return err;
}


/*===========================================================================
  FUNCTION:   sns_sam_log_rotation_vector_result
  =========================================================================*/
/*!
  @brief log Rotation Vector algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_rotation_vector_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   uint8_t i;
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_rotation_vector_result_s* logPtr;

   rotation_vector_input_s* inputPtr =
      (rotation_vector_input_s *)(algoInstPtr->inputData.memPtr);
   rotation_vector_output_s* outputPtr =
      (rotation_vector_output_s *)(algoInstPtr->outputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_ROTATION_VECTOR_RESULT,
                           sizeof(sns_log_rotation_vector_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_ROTATION_VECTOR_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      // log inputs
      for (i=0; i<3; i++)
      {
         logPtr->gravity[i] = inputPtr->gravity_vector.gravity[i];
         logPtr->filtered_mag[i] = inputPtr->mag_vector.filtered_mag[i];

      }
      logPtr->gravity_accuracy = inputPtr->gravity_vector.accuracy;
      logPtr->gravity_timestamp = inputPtr->gravity_vector.timestamp;
      logPtr->filtered_mag_accuracy = inputPtr->mag_vector.accuracy;
      logPtr->filtered_mag_timestamp = inputPtr->mag_vector.timestamp;

      // log outputs
      for (i=0; i<4; i++)
      {
         logPtr->rotation_vector[i] = outputPtr->rotation_vector[i];
      }
      logPtr->coordinate_sys = outputPtr->coordinate_sys;
      logPtr->accuracy = outputPtr->accuracy;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_ROTATION_VECTOR_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_rotation_vector_config
  =========================================================================*/
/*!
  @brief log Rotation Vector algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_rotation_vector_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_rotation_vector_config_s* logPtr;
   rotation_vector_config_s *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_ROTATION_VECTOR_CONFIG,
                           sizeof(sns_log_rotation_vector_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_ROTATION_VECTOR_CONFIG_VERSION;
      algoCfgPtr = (rotation_vector_config_s *)(algoInstPtr->configData.memPtr);

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->coordinate_sys = algoCfgPtr->coordinate_sys;
      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_ROTATION_VECTOR_CONFIG, logPtr);
   }

   return err;
}
/*===========================================================================
  FUNCTION:   sns_sam_log_orientation_gravity_report
  =========================================================================*/
/*!
  @brief log Orientation Gravity algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] result: gravity result
  @param[i] timeStamp: time stamp for gravity report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_orientation_gravity_report(
   const sns_sam_client_req_s* clientReqPtr,
   const sns_sam_orientation_result_s_v01 *result,
   uint32_t timeStamp)
{
   sns_err_code_e err;
   sns_log_gravity_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GRAVITY_REPORT,
                           sizeof(sns_log_gravity_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_ORIENTATION_GRAVITY_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      SNS_OS_MEMCOPY(logPtr->gravity, result->gravity, sizeof(result->gravity));
      SNS_OS_MEMCOPY(logPtr->lin_accel, result->lin_accel, sizeof(result->lin_accel));

      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GRAVITY_REPORT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_orientation_rotvec_report
  =========================================================================*/
/*!
  @brief log Gravity algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] result: gravity result
  @param[i] timeStamp: time stamp for gravity report

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_orientation_rotvec_report(
   const sns_sam_client_req_s* clientReqPtr,
   const sns_sam_orientation_result_s_v01 *result,
   uint32_t timeStamp)
{
   sns_err_code_e err = SNS_ERR_FAILED;

   sns_log_rotation_vector_report_s* logPtr; // Use the same log packet as rotation_vector
   uint8_t algoInstId = clientReqPtr->algoInstId;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_ROTATION_VECTOR_REPORT,
                           sizeof(sns_log_rotation_vector_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_ORIENTATION_ROTVEC_REPORT_VERSION;

      //Generate the log packet
      logPtr->client_id = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->algo_inst_id = algoInstId;
      SNS_OS_MEMCOPY(logPtr->rotation_vector, result->rotation_vector, sizeof(result->rotation_vector));
      logPtr->accuracy = result->rotation_vector_accuracy;
      logPtr->coordinate_sys = result->coordinate_system;

      logPtr->timestamp = timeStamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_ROTATION_VECTOR_REPORT, logPtr);
   }
   return err;
}
/*===========================================================================
  FUNCTION:   sns_sam_log_orientation_rotvec_result
  =========================================================================*/
/*!
  @brief log Rotation Vector algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_orientation_rotvec_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   uint8_t i;
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_rotation_vector_result_s* logPtr;  // Use Rotation Vector Log with different version number

   orientation_input_s* inputPtr =
      (orientation_input_s *)(algoInstPtr->inputData.memPtr);
   orientation_output_s* outputPtr =
      (orientation_output_s *)(algoInstPtr->outputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_ROTATION_VECTOR_RESULT,
                           sizeof(sns_log_rotation_vector_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_ORIENTATION_ROTVEC_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      // log inputs
      for (i=0; i<3; i++)
      {
         logPtr->gravity[i] = outputPtr->gravity[i];
         logPtr->filtered_mag[i] = (float)FX_FIXTOFLT_Q16(inputPtr->data.mag_vector[i]);

      }
      logPtr->gravity_accuracy = SNS_SENSOR_ACCURACY_HIGH_V01;
      logPtr->gravity_timestamp = outputPtr->timestamp;
      logPtr->filtered_mag_accuracy = inputPtr->accuracy;
      logPtr->filtered_mag_timestamp = inputPtr->timestamp;

      // log outputs
      for (i=0; i<4; i++)
      {
         logPtr->rotation_vector[i] = outputPtr->rotation_vector[i];
      }
      logPtr->coordinate_sys = outputPtr->coordinate_sys;
      logPtr->accuracy = outputPtr->accuracy;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_ROTATION_VECTOR_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_orientation_gravity_result
  =========================================================================*/
/*!
  @brief log Orientation - Gravity algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_orientation_gravity_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_gravity_result_s* logPtr;
   orientation_output_s* outputPtr =
      (orientation_output_s *)(algoInstPtr->outputData.memPtr);
   orientation_input_s* inputPtr =
      (orientation_input_s *)(algoInstPtr->inputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GRAVITY_RESULT,
                           sizeof(sns_log_gravity_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_ORIENTATION_GRAVITY_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      //Log algorithm input
      logPtr->quaternion_x = 0; // unused
      logPtr->quaternion_y = 0; // unused
      logPtr->quaternion_z = 0; // unused
      logPtr->accel_x = FX_FLTTOFIX_Q16(inputPtr->data.acc_vector[0]);
      logPtr->accel_y = FX_FLTTOFIX_Q16(inputPtr->data.acc_vector[1]);
      logPtr->accel_z = FX_FLTTOFIX_Q16(inputPtr->data.acc_vector[2]);
      logPtr->delta_t = 0; // unused

      logPtr->alpha = 0; // unused
      logPtr->abs_rest_state = 0; // unused
      logPtr->rel_rest_state = 0; // unused

      //Log algorithm output
      SNS_OS_MEMCOPY(logPtr->gravity, outputPtr->gravity, sizeof(outputPtr->gravity));
      SNS_OS_MEMCOPY(logPtr->lin_accel, outputPtr->lin_accel, sizeof(outputPtr->lin_accel));

      //Timestamp
      logPtr->timestamp = outputPtr->timestamp;
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GRAVITY_RESULT, logPtr);
   }

   return err;
}
/*===========================================================================
  FUNCTION:   sns_sam_log_orientation_gravity_config
  =========================================================================*/
/*!
  @brief log Orientation Gravity algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_orientation_gravity_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err;
   sns_log_gravity_config_s* logPtr;
   orientation_config_s *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_GRAVITY_CONFIG,
                           sizeof(sns_log_gravity_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_ORIENTATION_GRAVITY_CONFIG_VERSION;
      algoCfgPtr = (orientation_config_s *)(algoInstPtr->configData.memPtr);

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->time_const_abs = algoCfgPtr->accel_filter_tc;
      logPtr->time_const_rel = algoCfgPtr->accel_filter_tc;
      logPtr->time_const_mot = algoCfgPtr->mag_filter_tc;
      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_GRAVITY_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_orientation_rotvec_config
  =========================================================================*/
/*!
  @brief log Rotation Vector algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_orientation_rotvec_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_log_rotation_vector_config_s* logPtr; // Use Rotation Vector Log with different version number
   orientation_config_s *algoCfgPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_ROTATION_VECTOR_CONFIG,
                           sizeof(sns_log_rotation_vector_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_ORIENTATION_ROTVEC_CONFIG_VERSION;
      algoCfgPtr = (orientation_config_s *)(algoInstPtr->configData.memPtr);

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->coordinate_sys = algoCfgPtr->coordinate_sys;
      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_ROTATION_VECTOR_CONFIG, logPtr);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_report_ind
  =========================================================================*/
/*!
  @brief Prepare an algorithm report indication to client

  @param[i] clientReqPtr: pointer to client request
  @param[o] clientIndMsgPtr: pointer to report indication;
                             NULL, if return value is not SNS_SUCCESS
  @param[i] algoRptPtr: pointer to algorithm report
  @param[io] msgHdrPtr: pointer to client request message header
  @param[i] mrClntId: MR Client ID

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_prep_algo_report_ind(
   const sns_sam_client_req_s* clientReqPtr,
   void** clientIndMsgPtr,
   const sns_sam_algo_rpt_s *algoRptPtr,
   sns_smr_header_s* msgHdrPtr,
   sns_sam_mr_conn_hndl     mrClntId)
{
  SNS_SAM_PRINTF1(LOW,
                  "sns_sam_prep_algo_report_ind: service num %d",
                  msgHdrPtr->svc_num);
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_GYRO_TAP2_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      int32_t tapState;
      int32_t tapEvent;
      sns_sam_gyro_tap2_report_ind_msg_v01 *msgPtr;

      //Don't report to client if new event is not detected by algorithm
      tapState = ((gtap_output_struct *)(algoRptPtr->memPtr))->tap_state;
      tapEvent = ((gtap_output_struct *)(algoRptPtr->memPtr))->event;

      if(tapEvent == GTAP_UNKNOWN)
      {
        return SNS_ERR_WOULDBLOCK;
      }
      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                 sizeof(sns_sam_gyro_tap2_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;
      msgHdrPtr->body_len = sizeof(sns_sam_gyro_tap2_report_ind_msg_v01);
      msgHdrPtr->msg_id = SNS_SAM_GYRO_TAP2_REPORT_IND_V01;
      msgPtr->tap_event = (sns_sam_gyro_tap2_event_e_v01)tapEvent;

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      sns_sam_log_gtap_report(clientReqPtr, msgPtr);

      *clientIndMsgPtr = msgPtr;
   }
#endif // FEATURE_SNS_SAM_GYRO_TAP
#ifdef FEATURE_SNS_SAM_FNS
   else if (SNS_SAM_FNS_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      int32_t fnsEvent;
      sns_sam_fns_report_ind_msg_v01 *msgPtr;

      //Dont report to client if new event is not detected by algorithm
      if (((face_n_shake_output_s *)(algoRptPtr->memPtr))->event == false)
      {
         SNS_SAM_PRINTF2(LOW,
                        "Dropping invalid event report to client %d, algo service %d",
                        clientReqPtr->mrClientId, msgHdrPtr->svc_num);
         return SNS_ERR_WOULDBLOCK;
      }

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_fns_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_fns_report_ind_msg_v01);

      fnsEvent = ((face_n_shake_output_s *)(algoRptPtr->memPtr))->face_n_shake_event;

      msgPtr->state = (sns_sam_face_and_shake_event_e_v01)fnsEvent;

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      //log FNS report
      sns_sam_log_fns_report( clientReqPtr, fnsEvent, algoRptPtr->timestamp);

      *clientIndMsgPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   else if (SNS_SAM_BTE_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      int32_t bteEvent;
      sns_sam_bte_report_ind_msg_v01 *msgPtr;

      //Dont report to client if new event is not detected by algorithm
      if (((bring_to_ear_output_s *)(algoRptPtr->memPtr))->event == false)
      {
         SNS_SAM_PRINTF2(LOW,
                        "Dropping invalid event report to client %d, algo service %d",
                        clientReqPtr->mrClientId, msgHdrPtr->svc_num);
         return SNS_ERR_WOULDBLOCK;
      }

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_bte_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_bte_report_ind_msg_v01);

      bteEvent = ((bring_to_ear_output_s *)(algoRptPtr->memPtr))->bring_to_ear_event;

      msgPtr->state = (sns_sam_bring_to_ear_event_e_v01)bteEvent;

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      //log BTE report
      sns_sam_log_bte_report(clientReqPtr, bteEvent, algoRptPtr->timestamp);
      *clientIndMsgPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_BTE */

#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   else if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      int32_t bgEvent;
      int32_t ppsState;
      int32_t ppsEvent;

      sns_sam_basic_gestures_report_ind_msg_v01 *msgPtr;

      //Don't report to client if new event is not detected by algorithm
      ppsState = ((basic_gestures_output_struct *)(algoRptPtr->memPtr))->basic_state;
      ppsEvent = ((basic_gestures_output_struct *)(algoRptPtr->memPtr))->event;
      if(BASIC_GESTURE_UNKNOWN == ppsState)
      {
         SNS_SAM_PRINTF2(LOW,
                        "Dropping invalid event report to client %d, algo service %d",
                        clientReqPtr->mrClientId, msgHdrPtr->svc_num);
        return SNS_ERR_WOULDBLOCK;
      }

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_basic_gestures_report_ind_msg_v01);

      bgEvent = ((basic_gestures_output_struct *)(algoRptPtr->memPtr))->basic_state;

      //msgPtr->state = (sns_sam_basic_gestures_event_e_v01)bgEvent;
      msgPtr->state = (sns_sam_basic_gestures_event_e_v01)ppsState;

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      //log Basic Gestures report
      sns_sam_log_basic_gestures_report(clientReqPtr, bgEvent, algoRptPtr->timestamp);
      *clientIndMsgPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_TAP
   else if (SNS_SAM_TAP_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      int32_t tapState;
      int32_t tapEvent;
      sns_sam_tap_report_ind_msg_v01 *msgPtr;

      //Don't report to client if new event is not detected by algorithm
      tapState = ((tap_output_struct *)(algoRptPtr->memPtr))->tap_state;
      tapEvent = ((tap_output_struct *)(algoRptPtr->memPtr))->event;

      if(TAP_UNKNOWN == tapState)
      {
         SNS_SAM_PRINTF2(LOW,
                        "Dropping invalid event report to client %d, algo service %d",
                        clientReqPtr->mrClientId, msgHdrPtr->svc_num);
        return SNS_ERR_WOULDBLOCK;
      }
      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_tap_report_ind_msg_v01);

      tapState = ((tap_output_struct *)(algoRptPtr->memPtr))->tap_state;
      msgPtr->state = (sns_sam_tap_event_e_v01)tapState;

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      //log Tap report
      sns_sam_log_tap_report(clientReqPtr, tapState, algoRptPtr->timestamp);
      *clientIndMsgPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_TAP */

#ifdef FEATURE_SNS_SAM_TILT
   else if (SNS_SAM_INTEG_ANGLE_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      sns_sam_integ_angle_report_ind_msg_v01 *msgPtr;
      integ_angle_output_struct * algoOutPtr =
                              (integ_angle_output_struct *)(algoRptPtr->memPtr);

      //Tilt report is always sent to the client

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                               sizeof(sns_sam_integ_angle_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoOutPtr->ts;

      msgHdrPtr->body_len = sizeof(sns_sam_integ_angle_report_ind_msg_v01);
      msgHdrPtr->msg_id = SNS_SAM_INTEG_ANGLE_REPORT_IND_V01;
      msgPtr->angle[0]    = algoOutPtr->angle_change[0];
      msgPtr->angle[1]    = algoOutPtr->angle_change[1];
      msgPtr->angle[2]    = algoOutPtr->angle_change[2];

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      /* log the tilt report */
      sns_sam_log_integ_angle_report(clientReqPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_TILT */


#ifdef FEATURE_SNS_SAM_FACING
   else if (SNS_SAM_FACING_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      facing_state_e facingState;
      sns_sam_facing_report_ind_msg_v01 *msgPtr;

      facingState = ((facing_output_struct *)(algoRptPtr->memPtr))->facing_state;

      //Don't report to client if any event is not detected by algorithm
      if(FACING_UNKNOWN == facingState)
      {
         SNS_SAM_PRINTF2(LOW,
                        "Dropping invalid event report to client %d, algo service %d",
                        clientReqPtr->mrClientId, msgHdrPtr->svc_num);
        return SNS_ERR_WOULDBLOCK;
      }
      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_facing_report_ind_msg_v01);

      msgPtr->state = facingState;

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      //log Facing report
      sns_sam_log_facing_report(clientReqPtr, facingState, algoRptPtr->timestamp);
      *clientIndMsgPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_FACING */
   if (SNS_SAM_SVC_IS_QMD_ALGO(msgHdrPtr->svc_num))
   {
      int32_t motionState;
      uint16_t algoSvcId = msgHdrPtr->svc_num;

      sns_sam_qmd_report_ind_msg_v01 *msgPtr =
      sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_report_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_qmd_report_ind_msg_v01);

      switch(algoSvcId)
      {
      case SNS_SAM_AMD_SVC_ID_V01:
         motionState = ((qmd_output_s *)(algoRptPtr->memPtr))->absolute_rest_state;
         break;
      case SNS_SAM_VMD_SVC_ID_V01:
         motionState = ((qmd_output_s *)(algoRptPtr->memPtr))->vehicle_rest_state;
         break;
      case SNS_SAM_RMD_SVC_ID_V01:
         motionState = ((qmd_output_s *)(algoRptPtr->memPtr))->relative_rest_state;
         break;
      default:
         motionState = SNS_SAM_MOTION_UNKNOWN_V01;
         break;
      }
      msgPtr->state = (sns_sam_motion_state_e_v01)motionState;

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;

      //update GPIO state
      sns_sam_set_qmd_report_gpio(motionState);

      //log QMD report
      sns_sam_log_qmd_report(clientReqPtr, motionState,
                             algoRptPtr->timestamp, algoSvcId);
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      uint8_t i;
      sns_sam_sensor_thresh_report_ind_msg_v01 *msgPtr;
      threshold_output_s *outPtr = (threshold_output_s*)(algoRptPtr->memPtr);

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_report_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;
      for (i=0; i < outPtr->num_axis; i++)
      {
         msgPtr->sample_value[i] = outPtr->sample[i];
      }

      msgHdrPtr->body_len = sizeof(sns_sam_sensor_thresh_report_ind_msg_v01);
      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (msgHdrPtr->svc_num == SNS_OEM_1_SVC_ID_V01)
   {
      uint8_t i;
      oem_1_output_s *outPtr = (oem_1_output_s*)(algoRptPtr->memPtr);
      
      sns_oem_1_report_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_report_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = outPtr->timestamp;
      msgPtr->output.data_len=3; //3 is the number of axis in oem_1.h	  
      for (i=0; i < msgPtr->output.data_len; i++) //3 is the number of axis in oem.h
      {
        msgPtr->output.data[i] = outPtr->data[i];
      }
      msgHdrPtr->body_len = sizeof(sns_oem_1_report_ind_msg_v01);
      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;
   }
#endif //CONFIG_USE_OEM_1_ALGO     
   else if (msgHdrPtr->svc_num == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      sns_sam_modem_scn_report_ind_msg_v01 *msgPtr;
      modem_scn_output_s *outPtr = (modem_scn_output_s*)(algoRptPtr->memPtr);

      if (!outPtr->do_report)
      {
         return SNS_ERR_WOULDBLOCK;
      }
      else
      {
         SNS_SAM_PRINTF2(LOW,
                        "Modem scn req_module %d, DSPS module %d",
                        clientReqPtr->reqModule, SNS_SAM_MODULE);
         if (clientReqPtr->reqModule == 0)
         {
            SNS_SAM_PRINTF2(LOW,
                           "Sending modem scenario directly %d, %d",
                           0, 0);
            /* This algo instance was instantiated by the SAM itself, we need
               not send a report indication, instead send an update directly
               to the modem. This saves on a QMI roundtrip.
            */
            sns_sam_send_modem_scenario(outPtr->scn);
            return SNS_ERR_WOULDBLOCK;
         }
         else
         {
            /* This algo instance has been used by some other client, report
               response just as other algorithms.
            */
            SNS_SAM_PRINTF2(LOW,
                           "Sending modem scenario throught SAM %d, %d",
                           0, 0);
            msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_report_ind_msg_v01));
            SNS_ASSERT(msgPtr != NULL);

            msgPtr->instance_id = clientReqPtr->algoInstId;
            msgPtr->timestamp = algoRptPtr->timestamp;
            msgPtr->scenario  = outPtr->scn;

            msgHdrPtr->body_len = sizeof(sns_sam_modem_scn_report_ind_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, msgPtr);
            *clientIndMsgPtr = msgPtr;
         }
      }
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      int32_t num_sample;
      uint8_t entry;
      uint32_t mem_size;

      gyrobuf_output_s                    *outPtr;
      sns_sam_gyrobuf_report_ind_msg_v01  *msgPtr;

      outPtr = (gyrobuf_output_s *)(algoRptPtr->memPtr);

      if( outPtr->state->num_pending_report == 0 )
      {
         return SNS_ERR_WOULDBLOCK;
      }

      /*
       * Traverses the pending requests in GYROBUF. Every time an indication
       * is supposed to be sent, each request is checked to see whether all
       * the gyro samples are available for it. Per instance, there is a
       * maximum of 'num_pending_report' indications from GYROBUF.
       *
       * Note: gyrobuf_sam_fetch_data() is called here because (a) it needs
       *       to be called as soon as gyrobuf_state_s contains the necessary
       *       state information, and (b) it may lead to a large memory
       *       requirement for gyrobuf_output_s. This is triggered by a new
       *       sample from SMGR. It is potentially possible for this algo to
       *       be sending multiple IND, with each containing different samples.
       */
      /* Entry '0' implies the oldest request, '1' second-oldest, etc. */
      for (entry = 0; entry < outPtr->state->num_pending_report; entry++)
      {
         /* Fetches data from gyrobuf */
         num_sample = gyrobuf_sam_fetch_data(outPtr, entry,
                        (SNS_SAM_GYROBUF_MAX_BUFSIZE_V01+SNS_SAM_GYROBUF_MAX_BUFSIZE2_V01));

         /* Generates report response */
         if (num_sample >= 0)
         {
            /* Message Body */
            msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                                        sizeof(sns_sam_gyrobuf_report_ind_msg_v01));
            ///SNS_ASSERT(msgPtr != NULL);
            /* SNS_ASSERT() leads to DSPS crash. Due to memory use of gyrobuf algorithm,
             * tolerate some lost messages whenever SMR cannot allocate a message
             */
            if (msgPtr != NULL)
            {
               msgPtr->instance_id = clientReqPtr->algoInstId;
               msgPtr->timestamp   = algoRptPtr->timestamp;
               msgPtr->seqnum      = outPtr->seqnum;
               msgPtr->t_start     = outPtr->t_start;
               msgPtr->t_end       = outPtr->t_end;

               if (outPtr->outbuf_len > SNS_SAM_GYROBUF_MAX_BUFSIZE_V01)
               {
                  msgPtr->gyro2_valid = 1;
                  msgPtr->gyro_len    = SNS_SAM_GYROBUF_MAX_BUFSIZE_V01;
                  msgPtr->gyro2_len   = outPtr->outbuf_len - SNS_SAM_GYROBUF_MAX_BUFSIZE_V01;
               }
               else
               {
                  msgPtr->gyro2_valid = 0;
                  msgPtr->gyro_len    = outPtr->outbuf_len;
               }

               /* Copies data to msg: Assumes the sizes and data types of both
                * C-structs match up exactly.
                */
               mem_size = msgPtr->gyro_len * sizeof(sns_sam_gyrobuf_sample_t_v01);
               SNS_OS_MEMCOPY( &(msgPtr->gyro[0]), &(outPtr->outbuf[0]), mem_size );

               if (msgPtr->gyro2_valid)
               {
                  mem_size = msgPtr->gyro2_len * sizeof(sns_sam_gyrobuf_sample_t_v01);
                  SNS_OS_MEMCOPY( &(msgPtr->gyro2[0]),
                                  &(outPtr->outbuf[SNS_SAM_GYROBUF_MAX_BUFSIZE_V01]),
                                  mem_size );
               }

               /* Message Header */
               msgHdrPtr->body_len = sizeof(sns_sam_gyrobuf_report_ind_msg_v01);
               sns_smr_set_hdr(msgHdrPtr, msgPtr);
               *clientIndMsgPtr = msgPtr;
            }
         } /* if(num_sample >= 0) */
         else
         {
            return SNS_ERR_WOULDBLOCK;
         }
      } /* for(scanning through all pending request reports) */

   }
   else if (msgHdrPtr->svc_num == SNS_SAM_PED_SVC_ID_V01)
   {
      sns_sam_ped_report_ind_msg_v01 *msgPtr = NULL;
      ped_output_s *outPtr = (ped_output_s *)(algoRptPtr->memPtr);
      sns_sam_algo_mem_s * algoStatePtr;
      ped_state_s *statePtr;
      uint8_t i;

      algoStatePtr = sns_sam_get_algo_state(clientReqPtr->algoInstId);

      if( algoStatePtr == NULL )
      {
         return SNS_ERR_BAD_PTR;
      }
      statePtr = (ped_state_s *)(algoStatePtr->memPtr);

      for( i = 0; i < outPtr->num_active_clients; ++i )
      {
         if( outPtr->client_data[i].client_id == clientReqPtr->mrClientId )
         {
            msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                       sizeof(sns_sam_ped_report_ind_msg_v01));
            SNS_ASSERT(msgPtr != NULL);

            msgPtr->instance_id = clientReqPtr->algoInstId;
            msgPtr->timestamp = outPtr->client_data[i].timestamp;
            msgPtr->report_data.step_event = outPtr->client_data[i].step_event;
            msgPtr->report_data.step_confidence = outPtr->client_data[i].step_confidence;
            msgPtr->report_data.step_count = outPtr->client_data[i].step_count;
            msgPtr->report_data.step_count_error = outPtr->client_data[i].step_count_error;
            msgPtr->report_data.step_rate = outPtr->client_data[i].step_rate;

            msgHdrPtr->body_len = sizeof(sns_sam_ped_report_ind_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, msgPtr);
            *clientIndMsgPtr = msgPtr;

            // log Pedometer report
            sns_sam_log_ped_report( clientReqPtr, &outPtr->client_data[i] );

            // Clear step rate after every report
            outPtr->client_data[i].step_rate = 0.0f;
            statePtr->client_data[i].steps_since_report = 0;
            statePtr->client_data[i].total_step_rate = 0.0f;
            break;
         }
      }
      if( !msgPtr )
      {
         SNS_SAM_PRINTF1(ERROR,"Client %d not registered with Ped",clientReqPtr->mrClientId);
         return SNS_ERR_BAD_PARM;
      }
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_PAM_SVC_ID_V01)
   {
      sns_sam_pam_report_ind_msg_v01 *msgPtr;
      pam_output_s *outPtr = (pam_output_s *)(algoRptPtr->memPtr);

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                 sizeof(sns_sam_pam_report_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;
      msgPtr->step_count = outPtr->step_count;
      msgHdrPtr->body_len = sizeof(sns_sam_pam_report_ind_msg_v01);
      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;

      //log pam report
      sns_sam_log_pam_report(clientReqPtr, outPtr->step_count, algoRptPtr->timestamp);
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_CMC_SVC_ID_V01)
   {
      uint8_t i = 0, numReport = 0;
      uint8_t indexReport[CMC_MS_LEAF_NUM_WITH_UNKNOWN];
      cmc_output_s *outPtr = (cmc_output_s *)(algoRptPtr->memPtr);

      SNS_OS_MEMZERO(indexReport,(sizeof(uint8_t)*CMC_MS_LEAF_NUM_WITH_UNKNOWN));

      // Find the Exit events first and save their locations
      for (i = 0; i < CMC_MS_LEAF_NUM_WITH_UNKNOWN; i++)
      {
         if (outPtr->motion_outputs[i].motion_event == CMC_MOTION_EVENT_EXIT)
         {
            numReport++;
            indexReport[numReport-1]=i;
         }
      }

      // Find the Entry events next and save their locations
      for (i = 0; i < CMC_MS_LEAF_NUM_WITH_UNKNOWN; i++)
      {
         if (outPtr->motion_outputs[i].motion_event == CMC_MOTION_EVENT_ENTRY)
         {
            numReport++;
            indexReport[numReport-1]=i;
         }
      }

      msgHdrPtr->body_len = sizeof(sns_sam_cmc_report_ind_msg_v01);

      for (i = 0; i < numReport; i++)
      {
         cmc_motion_s *motionOutPtr = &(outPtr->motion_outputs[indexReport[i]]);
         sns_sam_cmc_report_ind_msg_v01 *msgPtr =
            sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                              sizeof(sns_sam_cmc_report_ind_msg_v01));
         SNS_ASSERT(msgPtr != NULL);

         msgPtr->instance_id = clientReqPtr->algoInstId;

         msgPtr->timestamp = motionOutPtr->timestamp;

         msgPtr->report_data.motion_state = motionOutPtr->motion_state;
         msgPtr->report_data.motion_state_probability = motionOutPtr->probability;

         msgPtr->ms_event_valid = true;

         // Translate the internal CMC enum to QMI Enum.
         // It will be either ENTRY or EXIT
         if (motionOutPtr->motion_event == CMC_MOTION_EVENT_ENTRY)
         {
            msgPtr->ms_event = SNS_SAM_CMC_MS_EVENT_ENTER_V01; 
         }
         else 
         {
            msgPtr->ms_event = SNS_SAM_CMC_MS_EVENT_EXIT_V01;
         }

         SNS_PRINTF_STRING_MEDIUM_3( SNS_SAM_DBG_MOD,
                             "Prepared CMC algo report. Motion State:%d, Event: %d, Time: %d",
                              msgPtr->report_data.motion_state,
                              msgPtr->ms_event,
                              msgPtr->timestamp);

         sns_sam_log_cmc_report(clientReqPtr,
                                msgPtr->report_data.motion_state,
                                msgPtr->ms_event,
                                msgPtr->report_data.motion_state_probability,
                                msgPtr->timestamp);

         sns_smr_set_hdr(msgHdrPtr, msgPtr);

         if (i == (numReport-1))
         {
            *clientIndMsgPtr = msgPtr;
            break;
         }
         else
         {
            sns_sam_mr_send_ind(msgPtr,
                                mrClntId,
                                clientReqPtr->notify_suspend.send_ind_during_suspend);
         }
      }
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      sns_sam_distance_bound_report_ind_msg_v01* msgPtr;

      distance_bound_output_s *outPtr = (distance_bound_output_s *)(algoRptPtr->memPtr);
      SNS_ASSERT(outPtr != NULL);

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_distance_bound_report_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgPtr->report_data.session_key = outPtr->client_data.session_key;
      msgPtr->report_data.distance_bound_breach_event = SNS_SAM_DISTANCE_BOUND_BREACH_EVENT_TRUE_V01;

      msgPtr->distance_accumulated_valid = TRUE;
      msgPtr->distance_accumulated = outPtr->client_data.distance_accumulated;

      msgHdrPtr->body_len = sizeof(sns_sam_distance_bound_report_ind_msg_v01);
      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;

      //log distance_bound report
      sns_sam_log_distance_bound_report(clientReqPtr,
                                        msgPtr->report_data.distance_bound_breach_event,
                                        msgPtr->timestamp,
                                        msgPtr->report_data.session_key);
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_quat_result_s_v01 quatResult;
      sns_sam_algo_mem_s *quatState;

      sns_sam_quat_report_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;

      SNS_OS_MEMCOPY(quatResult.quaternion,
                     algoRptPtr->memPtr,
                     sizeof(quatResult.quaternion));
      msgPtr->result = quatResult;  /* structure assignment */
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_quat_report_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;

      //reset cumulative quaternion state
      quatState = sns_sam_get_algo_state(clientReqPtr->algoInstId);
      if (quatState != NULL)
      {
         SNS_ASSERT(quatState->memSize == sizeof(gyro_quat_state_s));
         gyro_quat_sam_quat_reset((gyro_quat_state_s*)(quatState->memPtr));
      }

      //log quaternion report
      sns_sam_log_gyro_quat_report(clientReqPtr, &quatResult, algoRptPtr->timestamp);
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_gravity_result_s_v01 gravityResult;
      gravity_output_s *outPtr = (gravity_output_s *)(algoRptPtr->memPtr);

      sns_sam_gravity_report_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gravity_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->instance_id = clientReqPtr->algoInstId;

      SNS_OS_MEMCOPY(gravityResult.gravity, outPtr->gravity, sizeof(gravityResult.gravity));
      SNS_OS_MEMCOPY(gravityResult.lin_accel, outPtr->lin_accel, sizeof(gravityResult.lin_accel));
      gravityResult.accuracy = outPtr->accuracy;
      msgPtr->result = gravityResult;  /* structure assignment */
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_gravity_report_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;

      //log gravity report
      sns_sam_log_gravity_report(clientReqPtr, &gravityResult, algoRptPtr->timestamp);
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_filtered_mag_result_s_v01 result;
      fmv_output_s *outPtr = (fmv_output_s *)algoRptPtr->memPtr;

      sns_sam_filtered_mag_report_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      //SNS_OS_MEMCOPY(result.filtered_mag, outPtr->filtered_mag, sizeof(result.filtered_mag));
      result.filtered_mag[0] = (float)(outPtr->filtered_mag[0]);
      result.filtered_mag[1] = (float)(outPtr->filtered_mag[1]);
      result.filtered_mag[2] = (float)(outPtr->filtered_mag[2]);
      result.accuracy = outPtr->accuracy;

      msgPtr->result = result;  /* structure assignment */
      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_filtered_mag_report_ind_msg_v01);
      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      *clientIndMsgPtr = msgPtr;

      //log  report
      sns_sam_log_fmv_report(&result, clientReqPtr, algoRptPtr->timestamp);
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_rotation_vector_result_s_v01 result;
      rotation_vector_output_s *outPtr = (rotation_vector_output_s *)algoRptPtr->memPtr;

      sns_sam_rotation_vector_report_ind_msg_v01 *msgPtr;

      if (!outPtr->valid_result)
      {
         return SNS_ERR_WOULDBLOCK;
      }

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      SNS_OS_MEMCOPY(result.rotation_vector, outPtr->rotation_vector, sizeof(result.rotation_vector));
      result.accuracy = outPtr->accuracy;
      result.coordinate_sys = outPtr->coordinate_sys;

      msgPtr->instance_id = clientReqPtr->algoInstId;

      msgPtr->result = result;  /* structure assignment */
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_rotation_vector_report_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      *clientIndMsgPtr = msgPtr;

      //log rotation vector report
      sns_sam_log_rotation_vector_report(clientReqPtr, &result, algoRptPtr->timestamp);
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_orientation_result_s_v01 result;
      orientation_output_s *outPtr = (orientation_output_s *)algoRptPtr->memPtr;

      sns_sam_orientation_report_ind_msg_v01 *msgPtr;

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_report_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return SNS_ERR_NOMEM;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      // rotation vector
      SNS_OS_MEMCOPY(result.rotation_vector, outPtr->rotation_vector, sizeof(result.rotation_vector));
      result.rotation_vector_accuracy = outPtr->accuracy;
      result.coordinate_system = outPtr->coordinate_sys;

      // gravity
      SNS_OS_MEMCOPY(result.gravity, outPtr->gravity, sizeof(result.gravity));
      result.gravity_accuracy = SNS_SENSOR_ACCURACY_HIGH_V01;

      // linear acceleration
      SNS_OS_MEMCOPY(result.lin_accel, outPtr->lin_accel, sizeof(result.lin_accel));

      msgPtr->instance_id = clientReqPtr->algoInstId;

      msgPtr->result = result;  /* structure assignment */
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_orientation_report_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);

      *clientIndMsgPtr = msgPtr;

      //log orientation rotvec report  & orientation gravity report
      sns_sam_log_orientation_rotvec_report(clientReqPtr, &result, algoRptPtr->timestamp);
      sns_sam_log_orientation_gravity_report(clientReqPtr, &result, algoRptPtr->timestamp);
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_SMD_SVC_ID_V01)
   {
      sns_sam_smd_report_ind_msg_v01  *msgPtr;
      smd_output_s* outPtr = (smd_output_s *)(algoRptPtr->memPtr);
      SNS_PRINTF_STRING_LOW_1( SNS_SAM_DBG_MOD,
                                "Prepared SMD algo report. Motion State:%d",
                                outPtr->motion_state);

        msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                   sizeof(sns_sam_smd_report_ind_msg_v01));
        SNS_ASSERT(msgPtr != NULL);

        msgPtr->instance_id = clientReqPtr->algoInstId;
        msgPtr->timestamp = algoRptPtr->timestamp;

        msgPtr->report_data.motion_state = outPtr->motion_state;
        msgPtr->report_data.motion_state_probability = outPtr->motion_state_probability;

        msgHdrPtr->body_len = sizeof(sns_sam_smd_report_ind_msg_v01);
        sns_smr_set_hdr(msgHdrPtr, msgPtr);
        *clientIndMsgPtr = msgPtr;

        // log SMD report
        sns_sam_log_smd_report(clientReqPtr,
                               outPtr->motion_state,
                               outPtr->motion_state_probability,
                               algoRptPtr->timestamp);
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_game_rotation_vector_result_s_v01 result;
      game_rot_vec_output_s *outPtr = ( game_rot_vec_output_s *)algoRptPtr->memPtr;

      sns_sam_game_rotation_vector_report_ind_msg_v01 *msgPtr;

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                 sizeof(sns_sam_game_rotation_vector_report_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      SNS_OS_MEMCOPY(result.game_rotation_vector,
                     outPtr->rotation_vector,
                     sizeof(result.game_rotation_vector));
      result.accuracy = outPtr->accuracy;

      msgPtr->instance_id = clientReqPtr->algoInstId;

      msgPtr->result = result;  /* structure assignment */
      msgPtr->timestamp = algoRptPtr->timestamp;

      msgHdrPtr->body_len = sizeof(sns_sam_game_rotation_vector_report_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;

      //log rotation vector report
      sns_sam_log_game_rotation_vector_report(clientReqPtr, &result, algoRptPtr->timestamp);
   }
   else if (SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      tilt_output_s *outPtr = ( tilt_output_s *)algoRptPtr->memPtr;
      sns_sam_algo_mem_s * algoStatePtr;
      tilt_state_s *algoPtr;

      algoStatePtr = sns_sam_get_algo_state(clientReqPtr->algoInstId);
      algoPtr = (tilt_state_s *) algoStatePtr->memPtr;

      sns_sam_tilt_detector_report_ind_msg_v01 *msgPtr;

      msgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                 sizeof(sns_sam_tilt_detector_report_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->instance_id = clientReqPtr->algoInstId;
      msgPtr->tilt_timestamp = outPtr->event_ts;

      msgHdrPtr->body_len = sizeof(sns_sam_tilt_detector_report_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *clientIndMsgPtr = msgPtr;

      //log tilt detector report
      sns_sam_log_tilt_detector_report(clientReqPtr, outPtr, algoPtr, algoRptPtr->timestamp);
   }
   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sam_create_batch_item
  =========================================================================*/
/*!
  @brief Create a batch item to buffer

  @param[i] clientReqPtr: pointer to client request
  @param[i] algoRptPtr: pointer to algorithm report
  @param[o] batchItemPtr: pointer to batch item
  @param[i] clientReqId: index to client request database

  @return None
*/
/*=======================================================================*/
void sns_sam_create_batch_item(
   const sns_sam_client_req_s* clientReqPtr,
   const sns_sam_algo_rpt_s *algoRptPtr,
   void** batchItemPtr,
   uint8_t clientReqId)
{
   uint8_t serviceId;
   sns_sam_algo_inst_s* algoInstPtr = sns_sam_get_algo_inst_handle(clientReqPtr->algoInstId);
   if (NULL == algoInstPtr)
   {
      return;
   }

   serviceId = algoInstPtr->serviceId;

   if( SNS_SAM_PED_SVC_ID_V01 == serviceId )
   {
      sns_sam_ped_batch_item_s_v01 *itemPtr;
      ped_output_s *outPtr = (ped_output_s *)(algoRptPtr->memPtr);
      sns_sam_algo_mem_s * algoStatePtr;
      ped_state_s *statePtr;
      uint8_t i;

      algoStatePtr = sns_sam_get_algo_state(clientReqPtr->algoInstId);

      if( algoStatePtr == NULL )
      {
         return;
      }
      statePtr = (ped_state_s *)(algoStatePtr->memPtr);

      for( i = 0; i < outPtr->num_active_clients; ++i )
      {
         if( outPtr->client_data[i].client_id == clientReqPtr->mrClientId )
         {
            itemPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
                                       sizeof(sns_sam_ped_batch_item_s_v01));
            SNS_ASSERT(itemPtr != NULL);

            itemPtr->timestamp = outPtr->client_data[i].timestamp;
            itemPtr->report.step_event = outPtr->client_data[i].step_event;
            itemPtr->report.step_confidence = outPtr->client_data[i].step_confidence;
            itemPtr->report.step_count = outPtr->client_data[i].step_count;
            itemPtr->report.step_count_error = outPtr->client_data[i].step_count_error;
            itemPtr->report.step_rate = outPtr->client_data[i].step_rate;

            *batchItemPtr = itemPtr;

            // log Pedometer report
            sns_sam_log_ped_report( clientReqPtr, &outPtr->client_data[i] );

            // Clear step rate after every report
            outPtr->client_data[i].step_rate = 0.0f;
            statePtr->client_data[i].steps_since_report = 0;
            statePtr->client_data[i].total_step_rate = 0.0f;
            break;
         }
      }
   }
   else if( SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == serviceId )
   {
      sns_sam_gravity_batch_item_s *itemPtr;
      gravity_output_s *outPtr = (gravity_output_s *)(algoRptPtr->memPtr);

      itemPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
                              sizeof(sns_sam_gravity_batch_item_s));
      SNS_ASSERT(itemPtr != NULL);

      SNS_OS_MEMCOPY(itemPtr->report.result.gravity, outPtr->gravity, sizeof(itemPtr->report.result.gravity));
      SNS_OS_MEMCOPY(itemPtr->report.result.lin_accel, outPtr->lin_accel, sizeof(itemPtr->report.result.lin_accel));
      itemPtr->report.result.accuracy = outPtr->accuracy;
      itemPtr->report.device_motion_state = outPtr->dev_state;
      itemPtr->timestamp = algoRptPtr->timestamp;

      //log gravity report
      sns_sam_log_gravity_report(clientReqPtr, &itemPtr->report.result, algoRptPtr->timestamp);

      *batchItemPtr = itemPtr;
   }
   else if( SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == serviceId )
   {
      sns_sam_game_rotation_vector_batch_item_s *itemPtr;
      game_rot_vec_output_s *GameRVOutPtr = ( game_rot_vec_output_s *)(algoRptPtr->memPtr);
      uint8_t idx;

      itemPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
                              sizeof(sns_sam_game_rotation_vector_batch_item_s));
      SNS_ASSERT(itemPtr != NULL);

      for(idx = 0; idx < 4; idx++)
      {
         itemPtr->report.game_rotation_vector[idx] =
            GameRVOutPtr->rotation_vector[idx];
      }
      itemPtr->report.accuracy = GameRVOutPtr->accuracy;
      itemPtr->report.coordinate_sys = GameRVOutPtr->coordinate_sys;
      itemPtr->timestamp = algoRptPtr->timestamp;

      // log game rotation vector report
      sns_sam_log_game_rotation_vector_report(clientReqPtr,
                                              &itemPtr->report,
                                              algoRptPtr->timestamp);

      *batchItemPtr = itemPtr;
   }
   else if( SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == serviceId )
   {
      sns_sam_rotation_vector_batch_item_s *itemPtr;
      rotation_vector_output_s *outPtr = (rotation_vector_output_s *)algoRptPtr->memPtr;

      if (!outPtr->valid_result)
      {
         return;
      }

      itemPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
                              sizeof(sns_sam_rotation_vector_batch_item_s));
      SNS_ASSERT(itemPtr != NULL);

      SNS_OS_MEMCOPY(itemPtr->report.rotation_vector, outPtr->rotation_vector, sizeof(itemPtr->report.rotation_vector));
      itemPtr->report.accuracy = outPtr->accuracy;
      itemPtr->report.coordinate_sys = outPtr->coordinate_sys;
      itemPtr->timestamp = algoRptPtr->timestamp;

      //log rotation vector report
      sns_sam_log_rotation_vector_report(clientReqPtr, &itemPtr->report, algoRptPtr->timestamp);

      *batchItemPtr = itemPtr;
   }
   else if( SNS_SAM_ORIENTATION_SVC_ID_V01 == serviceId )
   {
      sns_sam_orientation_batch_item_s *itemPtr;
      orientation_output_s *outPtr = (orientation_output_s *)algoRptPtr->memPtr;

      itemPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
                              sizeof(sns_sam_orientation_batch_item_s));
      SNS_ASSERT(itemPtr != NULL);

      // rotation vector
      SNS_OS_MEMCOPY(itemPtr->report.rotation_vector, outPtr->rotation_vector, sizeof(itemPtr->report.rotation_vector));
      itemPtr->report.rotation_vector_accuracy = outPtr->accuracy;
      itemPtr->report.coordinate_system = outPtr->coordinate_sys;

      // gravity
      SNS_OS_MEMCOPY(itemPtr->report.gravity, outPtr->gravity, sizeof(itemPtr->report.gravity));
      itemPtr->report.gravity_accuracy = SNS_SENSOR_ACCURACY_HIGH_V01;

      // linear acceleration
      SNS_OS_MEMCOPY(itemPtr->report.lin_accel, outPtr->lin_accel, sizeof(itemPtr->report.lin_accel));

      itemPtr->timestamp = algoRptPtr->timestamp;

      //log orientation rotvec report  & orientation gravity report
      sns_sam_log_orientation_rotvec_report(clientReqPtr, &itemPtr->report, algoRptPtr->timestamp);
      sns_sam_log_orientation_gravity_report(clientReqPtr, &itemPtr->report, algoRptPtr->timestamp);

      *batchItemPtr = itemPtr;
   }
   else if( SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == serviceId )
   {
      uint32_t *itemPtr;
      tilt_output_s *outPtr = (tilt_output_s *)algoRptPtr->memPtr;
      sns_sam_algo_mem_s * algoStatePtr;
      tilt_state_s *algoPtr;

      itemPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
                              sizeof(uint32_t));
      SNS_ASSERT(itemPtr != NULL);

      // timestamp
      *itemPtr = outPtr->event_ts;

      // log tilt detector report
      algoStatePtr = sns_sam_get_algo_state(clientReqPtr->algoInstId);
      algoPtr = (tilt_state_s *) algoStatePtr->memPtr;
      sns_sam_log_tilt_detector_report(clientReqPtr, outPtr, algoPtr, algoRptPtr->timestamp);

      *batchItemPtr = itemPtr;
   }
   if( SNS_SAM_CMC_SVC_ID_V01 == serviceId )
   {
      uint8_t i = 0, numReport = 0;
      uint8_t indexReport[CMC_MS_LEAF_NUM_WITH_UNKNOWN];
      cmc_output_s *outPtr = (cmc_output_s *)(algoRptPtr->memPtr);

      SNS_OS_MEMZERO(indexReport,(sizeof(uint8_t)*CMC_MS_LEAF_NUM_WITH_UNKNOWN));

      // Find the Exit events first and save their locations
      for (i = 0; i < CMC_MS_LEAF_NUM_WITH_UNKNOWN; i++)
      {
         if (outPtr->motion_outputs[i].motion_event == CMC_MOTION_EVENT_EXIT)
         {
            numReport++;
            indexReport[numReport-1]=i;
         }
      }

      // Find the Entry events next and save their locations
      for (i = 0; i < CMC_MS_LEAF_NUM_WITH_UNKNOWN; i++)
      {
         if (outPtr->motion_outputs[i].motion_event == CMC_MOTION_EVENT_ENTRY)
         {
            numReport++;
            indexReport[numReport-1]=i;
         }
      }

      for (i = 0; i < numReport; i++)
      {
         cmc_motion_s *motionOutPtr = &(outPtr->motion_outputs[indexReport[i]]);
         sns_sam_cmc_batch_item_s_v01 *itemPtr = SNS_OS_MALLOC(SNS_SAM_DBG_MOD,
                                 sizeof(sns_sam_cmc_batch_item_s_v01));
         SNS_ASSERT(itemPtr != NULL);

         itemPtr->timestamp = motionOutPtr->timestamp;

         itemPtr->report.motion_state = motionOutPtr->motion_state;
         itemPtr->report.motion_state_probability = motionOutPtr->probability;

         // Translate the internal CMC enum to QMI Enum.
         // It will be either ENTRY or EXIT
         if (motionOutPtr->motion_event == CMC_MOTION_EVENT_ENTRY)
         {
            itemPtr->ms_event = SNS_SAM_CMC_MS_EVENT_ENTER_V01; 
         }
         else 
         {
            itemPtr->ms_event = SNS_SAM_CMC_MS_EVENT_EXIT_V01;
         }

         *batchItemPtr = itemPtr;

         sns_sam_log_cmc_report(clientReqPtr,
                       itemPtr->report.motion_state,
                       itemPtr->ms_event,
                       itemPtr->report.motion_state_probability,
                       itemPtr->timestamp);

         if (i == (numReport-1))
         {
            break;
         }
         else
         {
            sns_sam_add_item_to_buffer( clientReqId, batchItemPtr );
         }
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_send_algo_batch_ind
  =========================================================================*/
/*!
  @brief Prepare and send an algorithm batch indication to client

  @param[i] clientReqPtr: pointer to client request

  @return None
*/
/*=======================================================================*/
void sns_sam_send_algo_batch_ind(
   const sns_sam_client_req_s *clientReqPtr)
{
   sns_sam_algo_inst_s * algoInstPtr = NULL;
   uint8_t serviceId;
   sns_smr_header_s msgHdr;
   int numItems, numItemsPending, maxItems, msgSize, itemSize, i;
   uint8_t indType = SNS_BATCH_FIRST_IND_V01;

   if( !clientReqPtr || !clientReqPtr->batchInfo )
   {
      SNS_SAM_PRINTF0(ERROR, "SAM: Bad client request pointer");
      return;
   }

   algoInstPtr = sns_sam_get_algo_inst_handle(clientReqPtr->algoInstId);
   if( !algoInstPtr )
   {
      SNS_SAM_PRINTF0(ERROR, "SAM: Bad algo inst pointer");
      return;
   }

   serviceId = algoInstPtr->serviceId;
   numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );

   if( !numItems )
   {
      SNS_SAM_PRINTF0(LOW, "SAM: No items in circ buffer");
      return;
   }

   //generate message header
   msgHdr.dst_module = clientReqPtr->reqModule;
   msgHdr.src_module = SNS_SAM_MODULE;
   msgHdr.svc_num = serviceId;
   msgHdr.msg_type = SNS_SMR_MSG_TYPE_IND;
   msgHdr.priority = SNS_SMR_MSG_PRI_HIGH;
   msgHdr.txn_id = 0;
   msgHdr.ext_clnt_id = sns_sam_mr_get_client_id(clientReqPtr->mrClientId);
   msgHdr.msg_id = SNS_SAM_ALGO_BATCH_IND;

   if( SNS_SAM_PED_SVC_ID_V01 == serviceId )
   {
      sns_sam_ped_batch_ind_msg_v01 * msgPtr;
      sns_sam_ped_batch_item_s_v01 * itemPtr;

      maxItems = SNS_SAM_PED_MAX_ITEMS_IN_BATCH_V01;
      msgSize = sizeof(sns_sam_ped_batch_ind_msg_v01);
      itemSize = sizeof(sns_sam_ped_batch_item_s_v01);

      if( numItems <= maxItems )
      {
         indType = SNS_BATCH_ONLY_IND_V01;
      }

      while( numItems > 0 )
      {
         numItemsPending = MIN(numItems, maxItems);
         msgHdr.body_len = msgSize;

         // Allocate just enough memory
         msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD, msgSize );
         SNS_ASSERT(msgPtr != NULL);

         msgPtr->instance_id = clientReqPtr->algoInstId;
         msgPtr->ind_type_valid = true;
         msgPtr->ind_type = indType;

         // Fill in reports
         for( i = 0; numItemsPending > 0; numItemsPending-- )
         {
            itemPtr = (sns_sam_ped_batch_item_s_v01 *)sns_circ_buffer_get_item( clientReqPtr->batchInfo->reportBuffer );
            if( itemPtr )
            {
               SNS_OS_MEMCOPY( &(msgPtr->items[i]), itemPtr, itemSize );
               SNS_OS_FREE(itemPtr );
               i++;
            }
            else
            {
               SNS_SAM_PRINTF1(ERROR, "SAM: Bad item in report buffer for client id %d", clientReqPtr->mrClientId);
            }
         }
         msgPtr->items_len = i;
         SNS_SAM_PRINTF2(LOW, "SAM: Sending batch indication with %d items for client id %d", i, clientReqPtr->mrClientId);

         sns_smr_set_hdr(&msgHdr, msgPtr);
         sns_sam_mr_send_ind(msgPtr, clientReqPtr->mrClientId, true);

         numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );
         if( numItems <= maxItems )
         {
            indType = SNS_BATCH_LAST_IND_V01;
         }
         else
         {
            indType = SNS_BATCH_INTERMEDIATE_IND_V01;
         }
      }
   }
   else if( SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == serviceId )
   {
      sns_sam_gravity_batch_ind_msg_v01 * msgPtr;
      sns_sam_gravity_batch_item_s * itemPtr;
      uint32_t prevReportTimestamp = 0;

      maxItems = SNS_SAM_GRAVITY_MAX_REPORTS_IN_BATCH_V01;
      msgSize = sizeof(sns_sam_gravity_batch_ind_msg_v01);
      itemSize = sizeof(sns_sam_gravity_report_s_v01);

      if( numItems <= maxItems )
      {
         indType = SNS_BATCH_ONLY_IND_V01;
      }

      while( numItems > 0 )
      {
         numItemsPending = MIN(numItems, maxItems);

         // Allocate just enough memory
         msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD, msgSize );
         SNS_ASSERT(msgPtr != NULL);

         msgHdr.body_len = msgSize;
         msgPtr->instance_id = clientReqPtr->algoInstId;
         msgPtr->ind_type_valid = true;
         msgPtr->ind_type = indType;

         // Fill in reports
         for( i = 0; numItemsPending > 0; numItemsPending-- )
         {
            itemPtr = (sns_sam_gravity_batch_item_s *)sns_circ_buffer_get_item( clientReqPtr->batchInfo->reportBuffer );
            if( itemPtr )
            {
               if( i == 0 )
               {
                  msgPtr->first_report_timestamp = itemPtr->timestamp;
                  prevReportTimestamp = itemPtr->timestamp;
               }
               SNS_OS_MEMCOPY( &(msgPtr->reports[i]), itemPtr, itemSize );
               msgPtr->timestamp_offsets[i] = itemPtr->timestamp - prevReportTimestamp;
               prevReportTimestamp = itemPtr->timestamp;
               SNS_OS_FREE(itemPtr );
               i++;
            }
            else
            {
               SNS_SAM_PRINTF1(ERROR, "SAM: Bad item in report buffer for client id %d", clientReqPtr->mrClientId);
            }
         }
         msgPtr->reports_len = i;
         msgPtr->timestamp_offsets_len = i;
         SNS_SAM_PRINTF2(LOW, "SAM: Sending batch indication with %d items for client id %d", i, clientReqPtr->mrClientId);

         sns_smr_set_hdr(&msgHdr, msgPtr);
         sns_sam_mr_send_ind(msgPtr, clientReqPtr->mrClientId, true);

         numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );
         if( numItems <= maxItems )
         {
            indType = SNS_BATCH_LAST_IND_V01;
         }
         else
         {
            indType = SNS_BATCH_INTERMEDIATE_IND_V01;
         }
      }
   }
   else if( SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == serviceId )
   {
      sns_sam_game_rotation_vector_batch_ind_msg_v01 * msgPtr;
      sns_sam_game_rotation_vector_batch_item_s * itemPtr;
      uint32_t prevReportTimestamp = 0;

      maxItems = SNS_SAM_GAME_ROTATION_VECTOR_MAX_REPORTS_IN_BATCH_V01;
      msgSize = sizeof(sns_sam_game_rotation_vector_batch_ind_msg_v01);
      itemSize = sizeof(sns_sam_game_rotation_vector_result_s_v01);

      if( numItems <= maxItems )
      {
         indType = SNS_BATCH_ONLY_IND_V01;
      }

      while( numItems > 0 )
      {
         numItemsPending = MIN(numItems, maxItems);

         // Allocate just enough memory
         msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD, msgSize );
         SNS_ASSERT(msgPtr != NULL);

         msgHdr.body_len = msgSize;
         msgPtr->instance_id = clientReqPtr->algoInstId;
         msgPtr->ind_type_valid = true;
         msgPtr->ind_type = indType;

         // Fill in reports
         for( i = 0; numItemsPending > 0; numItemsPending-- )
         {
            itemPtr = (sns_sam_game_rotation_vector_batch_item_s *)sns_circ_buffer_get_item( clientReqPtr->batchInfo->reportBuffer );
            if( itemPtr )
            {
               if( i == 0 )
               {
                  msgPtr->first_report_timestamp = itemPtr->timestamp;
                  prevReportTimestamp = itemPtr->timestamp;
               }
               SNS_OS_MEMCOPY( &(msgPtr->reports[i]), itemPtr, itemSize );
               msgPtr->timestamp_offsets[i] = itemPtr->timestamp - prevReportTimestamp;
               prevReportTimestamp = itemPtr->timestamp;
               SNS_OS_FREE(itemPtr );
               i++;
            }
            else
            {
               SNS_SAM_PRINTF1(ERROR, "SAM: Bad item in report buffer for client id %d", clientReqPtr->mrClientId);
            }
         }
         msgPtr->reports_len = i;
         msgPtr->timestamp_offsets_len = i;
         SNS_SAM_PRINTF2(LOW, "SAM: Sending batch indication with %d items for client id %d", i, clientReqPtr->mrClientId);

         sns_smr_set_hdr(&msgHdr, msgPtr);
         sns_sam_mr_send_ind(msgPtr, clientReqPtr->mrClientId, true);

         numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );
         if( numItems <= maxItems )
         {
            indType = SNS_BATCH_LAST_IND_V01;
         }
         else
         {
            indType = SNS_BATCH_INTERMEDIATE_IND_V01;
         }
      }
   }
   else if( SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == serviceId )
   {
      sns_sam_rotation_vector_batch_ind_msg_v01 * msgPtr;
      sns_sam_rotation_vector_batch_item_s * itemPtr;
      uint32_t prevReportTimestamp = 0;

      maxItems = SNS_SAM_ROTATION_VECTOR_MAX_REPORTS_IN_BATCH_V01;
      msgSize = sizeof(sns_sam_rotation_vector_batch_ind_msg_v01);
      itemSize = sizeof(sns_sam_rotation_vector_result_s_v01);

      if( numItems <= maxItems )
      {
         indType = SNS_BATCH_ONLY_IND_V01;
      }

      while( numItems > 0 )
      {
         numItemsPending = MIN(numItems, maxItems);

         // Allocate just enough memory
         msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD, msgSize );
         SNS_ASSERT(msgPtr != NULL);

         msgHdr.body_len = msgSize;
         msgPtr->instance_id = clientReqPtr->algoInstId;
         msgPtr->ind_type_valid = true;
         msgPtr->ind_type = indType;

         // Fill in reports
         for( i = 0; numItemsPending > 0; numItemsPending-- )
         {
            itemPtr = (sns_sam_rotation_vector_batch_item_s *)sns_circ_buffer_get_item( clientReqPtr->batchInfo->reportBuffer );
            if( itemPtr )
            {
               if( i == 0 )
               {
                  msgPtr->first_report_timestamp = itemPtr->timestamp;
                  prevReportTimestamp = itemPtr->timestamp;
               }
               SNS_OS_MEMCOPY( &(msgPtr->reports[i]), itemPtr, itemSize );
               msgPtr->timestamp_offsets[i] = itemPtr->timestamp - prevReportTimestamp;
               prevReportTimestamp = itemPtr->timestamp;
               SNS_OS_FREE(itemPtr );
               i++;
            }
            else
            {
               SNS_SAM_PRINTF1(ERROR, "SAM: Bad item in report buffer for client id %d", clientReqPtr->mrClientId);
            }
         }
         msgPtr->reports_len = i;
         msgPtr->timestamp_offsets_len = i;
         SNS_SAM_PRINTF2(LOW, "SAM: Sending batch indication with %d items for client id %d", i, clientReqPtr->mrClientId);

         sns_smr_set_hdr(&msgHdr, msgPtr);
         sns_sam_mr_send_ind(msgPtr, clientReqPtr->mrClientId, true);

         numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );
         if( numItems <= maxItems )
         {
            indType = SNS_BATCH_LAST_IND_V01;
         }
         else
         {
            indType = SNS_BATCH_INTERMEDIATE_IND_V01;
         }
      }
   }
   else if( SNS_SAM_ORIENTATION_SVC_ID_V01 == serviceId )
   {
      sns_sam_orientation_batch_ind_msg_v01 * msgPtr;
      sns_sam_orientation_batch_item_s * itemPtr;
      uint32_t prevReportTimestamp = 0;

      maxItems = SNS_SAM_ORIENTATION_MAX_REPORTS_IN_BATCH_V01;
      msgSize = sizeof(sns_sam_orientation_batch_ind_msg_v01);
      itemSize = sizeof(sns_sam_orientation_result_s_v01);

      if( numItems <= maxItems )
      {
         indType = SNS_BATCH_ONLY_IND_V01;
      }

      while( numItems > 0 )
      {
         numItemsPending = MIN(numItems, maxItems);

         // Allocate just enough memory
         msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD, msgSize );
         SNS_ASSERT(msgPtr != NULL);

         msgHdr.body_len = msgSize;
         msgPtr->instance_id = clientReqPtr->algoInstId;
         msgPtr->ind_type_valid = true;
         msgPtr->ind_type = indType;

         // Fill in reports
         for( i = 0; numItemsPending > 0; numItemsPending-- )
         {
            itemPtr = (sns_sam_orientation_batch_item_s *)sns_circ_buffer_get_item( clientReqPtr->batchInfo->reportBuffer );
            if( itemPtr )
            {
               if( i == 0 )
               {
                  msgPtr->first_report_timestamp = itemPtr->timestamp;
                  prevReportTimestamp = itemPtr->timestamp;
               }
               SNS_OS_MEMCOPY( &(msgPtr->reports[i]), itemPtr, itemSize );
               msgPtr->timestamp_offsets[i] = itemPtr->timestamp - prevReportTimestamp;
               prevReportTimestamp = itemPtr->timestamp;
               SNS_OS_FREE(itemPtr );
               i++;
            }
            else
            {
               SNS_SAM_PRINTF1(ERROR, "SAM: Bad item in report buffer for client id %d", clientReqPtr->mrClientId);
            }
         }
         msgPtr->reports_len = i;
         msgPtr->timestamp_offsets_len = i;
         SNS_SAM_PRINTF2(LOW, "SAM: Sending batch indication with %d items for client id %d", i, clientReqPtr->mrClientId);

         sns_smr_set_hdr(&msgHdr, msgPtr);
         sns_sam_mr_send_ind(msgPtr, clientReqPtr->mrClientId, true);

         numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );
         if( numItems <= maxItems )
         {
            indType = SNS_BATCH_LAST_IND_V01;
         }
         else
         {
            indType = SNS_BATCH_INTERMEDIATE_IND_V01;
         }
      }
   }
   else if( SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == serviceId )
   {
      sns_sam_tilt_detector_batch_ind_msg_v01 * msgPtr;
      uint32_t * itemPtr;
      uint32_t prevReportTimestamp = 0;

      maxItems = SNS_SAM_TILT_DETECTOR_MAX_ITEMS_IN_BATCH_V01;
      msgSize = sizeof(sns_sam_tilt_detector_batch_ind_msg_v01);

      if( numItems <= maxItems )
      {
         indType = SNS_BATCH_ONLY_IND_V01;
      }

      while( numItems > 0 )
      {
         numItemsPending = MIN(numItems, maxItems);

         // Allocate just enough memory
         msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD, msgSize );
         SNS_ASSERT(msgPtr != NULL);

         msgHdr.body_len = msgSize;
         msgPtr->instance_id = clientReqPtr->algoInstId;
         msgPtr->ind_type_valid = true;
         msgPtr->ind_type = indType;

         // Fill in reports
         for( i = 0; numItemsPending > 0; numItemsPending-- )
         {
            itemPtr = (uint32_t *)sns_circ_buffer_get_item( clientReqPtr->batchInfo->reportBuffer );
            if( itemPtr )
            {
               msgPtr->tilt_timestamps[i] = *itemPtr;
               SNS_OS_FREE(itemPtr );
               i++;
            }
            else
            {
               SNS_SAM_PRINTF1(ERROR, "SAM: Bad item in report buffer for client id %d", clientReqPtr->mrClientId);
            }
         }
         msgPtr->tilt_timestamps_len = i;
         SNS_SAM_PRINTF2(LOW, "SAM: Sending batch indication with %d items for client id %d", i, clientReqPtr->mrClientId);

         sns_smr_set_hdr(&msgHdr, msgPtr);
         sns_sam_mr_send_ind(msgPtr, clientReqPtr->mrClientId, true);

         numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );
         if( numItems <= maxItems )
         {
            indType = SNS_BATCH_LAST_IND_V01;
         }
         else
         {
            indType = SNS_BATCH_INTERMEDIATE_IND_V01;
         }
      }
   }
   else if( SNS_SAM_CMC_SVC_ID_V01 == serviceId )
   {
      sns_sam_cmc_batch_ind_msg_v01 * msgPtr;
      sns_sam_cmc_batch_item_s_v01 * itemPtr;
      uint32_t prevReportTimestamp = 0;

      maxItems = SNS_SAM_CMC_MAX_ITEMS_IN_BATCH_V01;
      msgSize = sizeof(sns_sam_cmc_batch_ind_msg_v01);
      itemSize = sizeof(sns_sam_cmc_batch_item_s_v01);

      if( numItems <= maxItems )
      {
         indType = SNS_BATCH_ONLY_IND_V01;
      }

      while( numItems > 0 )
      {
         numItemsPending = MIN(numItems, maxItems);
         msgHdr.body_len = msgSize;

         // Allocate just enough memory
         msgPtr = sns_smr_msg_alloc( SNS_SAM_DBG_MOD, msgSize );
         SNS_ASSERT(msgPtr != NULL);

         msgPtr->instance_id = clientReqPtr->algoInstId;
         msgPtr->ind_type_valid = true;
         msgPtr->ind_type = indType;

         // Fill in reports
         for( i = 0; numItemsPending > 0; numItemsPending-- )
         {
            itemPtr = (sns_sam_cmc_batch_item_s_v01 *)sns_circ_buffer_get_item( clientReqPtr->batchInfo->reportBuffer );
            if( itemPtr )
            {
               SNS_OS_MEMCOPY( &(msgPtr->items[i]), itemPtr, itemSize );
               SNS_OS_FREE(itemPtr );
               i++;
            }
            else
            {
               SNS_SAM_PRINTF1(ERROR, "SAM: Bad item in report buffer for client id %d", clientReqPtr->mrClientId);
            }
         }
         msgPtr->items_len = i;
         SNS_SAM_PRINTF2(LOW, "SAM: Sending batch indication with %d items for client id %d", i, clientReqPtr->mrClientId);

         sns_smr_set_hdr(&msgHdr, msgPtr);
         sns_sam_mr_send_ind(msgPtr, clientReqPtr->mrClientId, true);

         numItems = sns_circ_buffer_get_count( clientReqPtr->batchInfo->reportBuffer );
         if( numItems <= maxItems )
         {
            indType = SNS_BATCH_LAST_IND_V01;
         }
         else
         {
            indType = SNS_BATCH_INTERMEDIATE_IND_V01;
         }
      }
   }
   // Send all pending batch indications when sending any indication
   sns_sam_send_pending_batch_inds( clientReqPtr->notify_suspend.proc_type );
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_error_ind
  =========================================================================*/
/*!
  @brief Prepare an algorithm error indication to client

  @param[i] algoInstId: algorithm instance id
  @param[i] error: error code
  @param[io] msgHdrPtr: pointer to client request message header

  @return None
*/
/*=======================================================================*/
void sns_sam_prep_algo_error_ind(
   uint8_t algoInstId,
   uint8_t error,
   sns_smr_header_s* msgHdrPtr,
   void** msgIndPtr)
{
   SNS_SAM_PRINTF2(LOW,
                   "sns_sam_prep_algo_error_ind: service num %d  error %d",
                   msgHdrPtr->svc_num, error);
#ifdef FEATURE_SNS_SAM_FNS
   if (SNS_SAM_FNS_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      sns_sam_fns_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_fns_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_fns_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   if (SNS_SAM_BTE_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      sns_sam_bte_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_bte_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_bte_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      sns_sam_basic_gestures_error_ind_msg_v01 *msgPtr =
        sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }


      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_basic_gestures_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
   if (SNS_SAM_TAP_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      sns_sam_tap_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_tap_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_TAP */

#ifdef FEATURE_SNS_SAM_TILT
   if (SNS_SAM_INTEG_ANGLE_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      sns_sam_integ_angle_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_integ_angle_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_integ_angle_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_TAP */

#ifdef FEATURE_SNS_SAM_FACING
   if (SNS_SAM_FACING_SVC_ID_V01 == (msgHdrPtr->svc_num))
   {
      sns_sam_facing_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_facing_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_GYRO_TAP2_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_gyro_tap2_error_ind_msg_v01 *msgPtr =
        sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyro_tap2_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_DEBUG0( ERROR, DBG_SAM_ALLOC_ERR );
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_gyro_tap2_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#endif // FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_SVC_IS_QMD_ALGO(msgHdrPtr->svc_num))
   {
      sns_sam_qmd_error_ind_msg_v01 *msgPtr =
      sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_error_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_qmd_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      sns_sam_sensor_thresh_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_error_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_sensor_thresh_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (msgHdrPtr->svc_num == SNS_OEM_1_SVC_ID_V01)
   {
      sns_oem_1_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_error_ind_msg_v01));	  
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_oem_1_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;	  
   }
#endif //CONFIG_USE_OEM_1_ALGO
   else if (msgHdrPtr->svc_num == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      sns_sam_modem_scn_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_error_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_modem_scn_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_PED_SVC_ID_V01)
   {
       sns_sam_ped_error_ind_msg_v01 *msgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_ped_error_ind_msg_v01));
       SNS_ASSERT(msgPtr != NULL);

       msgPtr->error = error;
       msgPtr->instance_id = algoInstId;

       msgHdrPtr->body_len = sizeof(sns_sam_ped_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_PAM_SVC_ID_V01)
   {
       sns_sam_pam_error_ind_msg_v01 *msgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_pam_error_ind_msg_v01));
       SNS_ASSERT(msgPtr != NULL);

       msgPtr->error = error;
       msgPtr->instance_id = algoInstId;

       msgHdrPtr->body_len = sizeof(sns_sam_pam_error_ind_msg_v01);

       sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_CMC_SVC_ID_V01)
   {
       sns_sam_cmc_error_ind_msg_v01 *msgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_cmc_error_ind_msg_v01));
       SNS_ASSERT(msgPtr != NULL);

       msgPtr->error = error;
       msgPtr->instance_id = algoInstId;

       msgHdrPtr->body_len = sizeof(sns_sam_cmc_error_ind_msg_v01);

       sns_smr_set_hdr(msgHdrPtr, msgPtr);
       *msgIndPtr = msgPtr;
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
       sns_sam_distance_bound_error_ind_msg_v01 *msgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_distance_bound_error_ind_msg_v01));
       SNS_ASSERT(msgPtr != NULL);

       msgPtr->error = error;
       msgPtr->instance_id = algoInstId;

       msgHdrPtr->body_len = sizeof(sns_sam_distance_bound_error_ind_msg_v01);

       sns_smr_set_hdr(msgHdrPtr, msgPtr);
       *msgIndPtr = msgPtr;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_quat_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_quat_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_gravity_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gravity_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_gravity_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_filtered_mag_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_filtered_mag_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_rotation_vector_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_rotation_vector_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_orientation_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_error_ind_msg_v01));
      if (msgPtr == NULL) {
        if (msgHdrPtr->priority == SNS_SMR_MSG_PRI_LOW)
        {
          SNS_SAM_PRINTF1(ERROR,"Failed to allocate ind msg. Dropping. svc %d",
                          msgHdrPtr->svc_num);
          return;
        }
        else
        {
          SNS_ASSERT(msgPtr != NULL);
        }
      }

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_orientation_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (msgHdrPtr->svc_num == SNS_SAM_SMD_SVC_ID_V01)
   {
      sns_sam_smd_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_smd_error_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_smd_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
      sns_sam_game_rotation_vector_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                           sizeof(sns_sam_game_rotation_vector_error_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_game_rotation_vector_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
   else if (SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == msgHdrPtr->svc_num)
   {
     sns_sam_tilt_detector_error_ind_msg_v01 *msgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                           sizeof(sns_sam_tilt_detector_error_ind_msg_v01));
      SNS_ASSERT(msgPtr != NULL);

      msgPtr->error = error;
      msgPtr->instance_id = algoInstId;

      msgHdrPtr->body_len = sizeof(sns_sam_tilt_detector_error_ind_msg_v01);

      sns_smr_set_hdr(msgHdrPtr, msgPtr);
      *msgIndPtr = msgPtr;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_request_suspend_notif_info
  =========================================================================*/
/*!
  @brief Get information about processor type and about sending indications
         during suspend that is passed in by the client.

  @param[i] algoSvcId: algorithm service id
  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] procType: processor type
  @param[o] notifySuspend: if indications should be sent during suspend.

  @return Processor type information
*/
/*=======================================================================*/
void sns_sam_get_request_suspend_notif_info(
   uint8_t algoSvcId,
   const void* clientReqMsgPtr,
   uint32_t* procType,
   bool* notifySuspend)
{
  /* set default behavior if the client does not specify */
  /* default behavior is to not wakeup Apps from suspend for indications */
  *procType = SNS_PROC_APPS_V01;
  *notifySuspend = false;

  switch(algoSvcId)
  {
   case SNS_SAM_AMD_SVC_ID_V01:
   case SNS_SAM_VMD_SVC_ID_V01:
   case SNS_SAM_RMD_SVC_ID_V01:
     if(((sns_sam_qmd_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_qmd_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_qmd_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_PED_SVC_ID_V01:
     if(((sns_sam_ped_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_ped_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_ped_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_PAM_SVC_ID_V01:
     if(((sns_sam_pam_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_pam_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_pam_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
     if(((sns_sam_basic_gestures_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_basic_gestures_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_basic_gestures_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_GYRO_TAP
   case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
     if(((sns_sam_gyro_tap2_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_gyro_tap2_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_gyro_tap2_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;
#endif /* FEATURE_SNS_SAM_GYRO_TAP */

#ifdef FEATURE_SNS_SAM_FACING
   case SNS_SAM_FACING_SVC_ID_V01:
     if(((sns_sam_facing_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_facing_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_facing_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;
#endif /* FEATURE_SNS_SAM_FACING */

#ifdef FEATURE_SNS_SAM_FNS
   case SNS_SAM_FNS_SVC_ID_V01:
     if(((sns_sam_fns_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_fns_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_fns_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;
#endif /* FEATURE_SNS_SAM_FNS */

#ifdef FEATURE_SNS_SAM_BTE
   case SNS_SAM_BTE_SVC_ID_V01:
     if(((sns_sam_bte_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_bte_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_bte_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;
#endif /* FEATURE_SNS_SAM_BTE */

#ifdef FEATURE_SNS_SAM_TAP
   case SNS_SAM_TAP_SVC_ID_V01:
     if(((sns_sam_tap_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_tap_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_tap_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;
#endif /* FEATURE_SNS_SAM_TAP */

   case SNS_SAM_GYROBUF_SVC_ID_V01:
     if(((sns_sam_gyrobuf_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_gyrobuf_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_gyrobuf_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_SENSOR_THRESH_SVC_ID_V01:
     if(((sns_sam_sensor_thresh_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_sensor_thresh_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_sensor_thresh_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_CMC_SVC_ID_V01:
     if(((sns_sam_cmc_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_cmc_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_cmc_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
     if(((sns_sam_distance_bound_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_distance_bound_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_distance_bound_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
     if(((sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
     if(((sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_ORIENTATION_SVC_ID_V01:
     if(((sns_sam_orientation_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_orientation_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_orientation_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
     if(((sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_SMD_SVC_ID_V01:
     if(((sns_sam_smd_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_smd_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_smd_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
     if(((sns_sam_tilt_detector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend_valid)
     {
       *procType = ((sns_sam_tilt_detector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.proc_type;
       *notifySuspend =
         ((sns_sam_tilt_detector_enable_req_msg_v01 *)clientReqMsgPtr)->notify_suspend.send_indications_during_suspend;
     }
     break;

   default:
     break;
  }
}

/*=========================================================================
  FUNCTION:  sns_sam_validate_algo_report_period
  =========================================================================*/
/*!
  @brief Validate the requested algorithm report period

  @param[i] algoSvcId: algorithm service id
  @param[i] clientReqMsgPtr: pointer to client request message

  @return SNS_SUCCESS, if requested report period is supported
          SNS_ERR_BAD_PARM, otherwise
*/
/*=======================================================================*/
static sns_err_code_e sns_sam_validate_algo_report_period(
   uint8_t algoSvcId,
   const void* clientReqMsgPtr)
{
   uint32_t reportPeriod = 0;

   if (NULL == clientReqMsgPtr)
   {
      return SNS_ERR_BAD_PARM;
   }

   SNS_SAM_PRINTF1(LOW, "sns_sam_validate_algo_report_period: algo service id %d",
                   algoSvcId);

   switch(algoSvcId)
   {
      case SNS_SAM_AMD_SVC_ID_V01:
      case SNS_SAM_VMD_SVC_ID_V01:
      case SNS_SAM_RMD_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_qmd_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
#ifdef FEATURE_SNS_SAM_TILT
      case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_integ_angle_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
      case SNS_SAM_FACING_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_facing_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
#endif /* FEATURE_SNS_SAM_FACING */
      case SNS_SAM_PED_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_ped_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_quat_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_filtered_mag_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_orientation_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      default:
         break;
   }

   // Range check
   if( reportPeriod != 0 )
   {
      uint8_t algoIndex = sns_sam_get_algo_index(algoSvcId);
      float minReportRate = (float)FX_FIXTOFLT_Q16(sns_sam_algo_dbase_acc(algoIndex)->min_report_rate);
      float maxReportRate = (float)FX_FIXTOFLT_Q16(sns_sam_algo_dbase_acc(algoIndex)->max_report_rate);
      uint32_t minReportPeriod = (uint32_t)floor(65536.0f/maxReportRate);
      uint32_t maxReportPeriod = (uint32_t)ceil(65536.0f/minReportRate);

      SNS_SAM_PRINTF3(LOW,"reportPeriod:%d, minReportPeriod:%d, maxReportPeriod:%d",
                      reportPeriod, minReportPeriod, maxReportPeriod);
      if( reportPeriod < minReportPeriod || reportPeriod > maxReportPeriod )
      {
         return SNS_ERR_BAD_PARM;
      }
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sam_validate_algo_sample_rate
  =========================================================================*/
/*!
  @brief Validate the requested algorithm sample rate

  @param[i] algoSvcId: algorithm service id
  @param[i] clientReqMsgPtr: pointer to client request message

  @return SNS_SUCCESS, if requested sample rate is supported
          SNS_ERR_BAD_PARM, otherwise
*/
/*=======================================================================*/
static sns_err_code_e sns_sam_validate_algo_sample_rate(
   uint8_t algoSvcId,
   const void* clientReqMsgPtr)
{
   uint32_t sampleRate = 0;

   if (NULL == clientReqMsgPtr)
   {
      return SNS_ERR_BAD_PARM;
   }

   SNS_SAM_PRINTF1(LOW, "sns_sam_validate_algo_sample_rate: algo service id %d",
                   algoSvcId);

   switch (algoSvcId)
   {
#ifdef FEATURE_SNS_SAM_FNS
      case SNS_SAM_FNS_SVC_ID_V01:
      {
         sns_sam_fns_enable_req_msg_v01 *fns_enable_ptr =
            (sns_sam_fns_enable_req_msg_v01 *)clientReqMsgPtr;
         if (fns_enable_ptr->sample_rate_valid)
         {
            sampleRate = fns_enable_ptr->sample_rate;
         }
         break;
      }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
      case SNS_SAM_BTE_SVC_ID_V01:
      {
         sns_sam_bte_enable_req_msg_v01 *bte_enable_ptr =
           (sns_sam_bte_enable_req_msg_v01 *)clientReqMsgPtr;
         if (bte_enable_ptr->sample_rate_valid)
         {
            sampleRate = bte_enable_ptr->sample_rate;
         }
         break;
      }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
      case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
      {
         sns_sam_basic_gestures_enable_req_msg_v01 *basic_gestures_enable_ptr =
            (sns_sam_basic_gestures_enable_req_msg_v01 *)clientReqMsgPtr;
         if (basic_gestures_enable_ptr->sample_rate_valid)
         {
            sampleRate = basic_gestures_enable_ptr->sample_rate;
         }
         break;
      }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
      case SNS_SAM_TAP_SVC_ID_V01:
      {
         sns_sam_tap_enable_req_msg_v01 *tap_enable_ptr =
            (sns_sam_tap_enable_req_msg_v01 *)clientReqMsgPtr;
         if (tap_enable_ptr->sample_rate_valid)
         {
            sampleRate = tap_enable_ptr->sample_rate;
         }
         break;
      }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
      case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
      {
         sns_sam_integ_angle_enable_req_msg_v01 *enable_ptr =
           (sns_sam_integ_angle_enable_req_msg_v01 *)clientReqMsgPtr;
         if (enable_ptr->sample_rate_valid)
         {
            sampleRate = enable_ptr->sample_rate;
         }
         break;
      }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
      case SNS_SAM_FACING_SVC_ID_V01:
      {
         sns_sam_facing_enable_req_msg_v01 *facing_enable_ptr =
                (sns_sam_facing_enable_req_msg_v01 *)clientReqMsgPtr;
         if (facing_enable_ptr->sample_rate_valid)
         {
            sampleRate = facing_enable_ptr->sample_rate;
         }
         break;
      }
#endif /* FEATURE_SNS_SAM_FACING */
      case SNS_SAM_PED_SVC_ID_V01:
      {
         sns_sam_ped_enable_req_msg_v01 *enablePtr =
            (sns_sam_ped_enable_req_msg_v01 *)clientReqMsgPtr;
         if( enablePtr->sample_rate_valid == true )
         {
            sampleRate = enablePtr->sample_rate;
         }
         break;
      }
      case SNS_SAM_PAM_SVC_ID_V01:
      {
         sns_sam_pam_enable_req_msg_v01 *enablePtr =
            (sns_sam_pam_enable_req_msg_v01 *)clientReqMsgPtr;
         if(enablePtr->sample_rate_valid == 1)
         {
            sampleRate = enablePtr->sample_rate;
         }
         break;
      }
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         sns_sam_quat_enable_req_msg_v01 *req =
            (sns_sam_quat_enable_req_msg_v01 *)clientReqMsgPtr;
         if (req->sample_rate_valid)
         {
            sampleRate = req->sample_rate;
         }
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         sns_sam_gravity_enable_req_msg_v01 *req =
            (sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr;
         if (req->sample_rate_valid)
         {
            sampleRate = req->sample_rate;
         }
         break;
      }
      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         sns_sam_filtered_mag_enable_req_msg_v01 *req =
            (sns_sam_filtered_mag_enable_req_msg_v01 *)clientReqMsgPtr;
         if (req->sample_rate_valid)
         {
            sampleRate = req->sample_rate;
         }
         break;
      }
      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      {
         sns_sam_rotation_vector_enable_req_msg_v01 *req =
            (sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr;
         if (req->sample_rate_valid)
         {
            sampleRate = req->sample_rate;
         }
         break;
      }
      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
         sns_sam_orientation_enable_req_msg_v01 *req =
            (sns_sam_orientation_enable_req_msg_v01 *)clientReqMsgPtr;
         if (req->sample_rate_valid)
         {
            sampleRate = req->sample_rate;
         }
         break;
      }
      case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      {
         sns_sam_game_rotation_vector_enable_req_msg_v01 *req =
            (sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr;
         if (req->sample_rate_valid)
         {
            sampleRate = req->sample_rate;
         }
         break;
      }

      default:
         break;
   }  /* End switch */

   // Range check
   if( sampleRate != 0 )
   {
      uint8_t algoIndex = sns_sam_get_algo_index(algoSvcId);
      uint32_t minSampleRate = sns_sam_algo_dbase_acc(algoIndex)->min_sample_rate;
      uint32_t maxSampleRate = sns_sam_algo_dbase_acc(algoIndex)->max_sample_rate;

      SNS_SAM_PRINTF3(LOW,"sampleRate:%d, minSampleRate:%d, maxSampleRate:%d",sampleRate,minSampleRate,maxSampleRate);
      if( sampleRate < minSampleRate || sampleRate > maxSampleRate )
      {
         return SNS_ERR_BAD_PARM;
      }
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sam_get_algo_report_period
  =========================================================================*/
/*!
  @brief Get the algorithm report period

  @param[i] algoSvcId: algorithm service id
  @param[i] clientReqMsgPtr: pointer to client request message
  @param[i] algoCfgPtr: pointer to algorithm configuration

  @return Algorithm report period. 0 if report is not periodic OR
          algorithm not supported
*/
/*=======================================================================*/
uint32_t sns_sam_get_algo_report_period(
   uint8_t algoSvcId,
   const void* clientReqMsgPtr,
   const void* algoCfgPtr)
{
   uint32_t reportPeriod = 0, sampleRate = 0;

   if (NULL == clientReqMsgPtr || NULL == algoCfgPtr)
   {
      return SNS_SAM_INVALID_PERIOD;
   }
   SNS_SAM_PRINTF1(LOW, "sns_sam_get_algo_report_period: algo service id %d",
                   algoSvcId);

   switch(algoSvcId)
   {
      case SNS_SAM_AMD_SVC_ID_V01:
      case SNS_SAM_VMD_SVC_ID_V01:
      case SNS_SAM_RMD_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_qmd_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_integ_angle_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         sampleRate = ((integ_angle_config_struct *)algoCfgPtr)->sample_rate;
         break;
      }
#ifdef CONFIG_USE_OEM_1_ALGO   
      case SNS_OEM_1_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_oem_1_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
#endif //CONFIG_USE_OEM_1_ALGO
      case SNS_SAM_PED_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_ped_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         break;
      }
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_quat_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         sampleRate = ((gyro_quat_config_s *)algoCfgPtr)->sample_rate;
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         sampleRate = ((gravity_config_s *)algoCfgPtr)->sample_rate;
         break;
      }
      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_filtered_mag_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         sampleRate = ((fmv_config_s *)algoCfgPtr)->sample_rate;
         break;
      }
      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         sampleRate = ((rotation_vector_config_s *)algoCfgPtr)->sample_rate;
         break;
      }
      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_orientation_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         sampleRate = ((orientation_config_s *)algoCfgPtr)->sample_rate;
         break;
      }
      case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      {
         reportPeriod =
            ((sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr)->report_period;
         sampleRate = (( game_rot_vec_config_s *)algoCfgPtr)->sample_rate;
         break;
      }
      default:
         break;
   }

   if (sampleRate > 0)
   {
      float samplePer = (float)(1. / FX_FIXTOFLT_Q16(sampleRate));
      float reportPer = (float)FX_FIXTOFLT_Q16(reportPeriod);

      //sample period will be less than or equal to report period
      //check for sync reporting
      if (reportPer - samplePer < .001)
      {
         reportPeriod = 0;
      }

      if (reportPeriod == 0)
      {
         return reportPeriod;
      }
   }
   SNS_SAM_PRINTF2(LOW, "sns_sam_get_algo_report_period: algo service id %d, report period %d",
                   algoSvcId, reportPeriod);

   if (reportPeriod == SNS_SAM_INVALID_PERIOD)
   {
      return reportPeriod;
   }
   else
   {
      return (sns_em_convert_sec_in_q16_to_localtick(reportPeriod));
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_sensor_buf_req
  =========================================================================*/
/*!
  @brief Gets sensor data requirements for algorithm

  @param[i] algoSvcId: algorithm service id
  @param[i] algoCfgPtr: pointer to algorithm configuration
  @param[o] dataReq: sensor data required by algorithm

  @return None
*/
/*=======================================================================*/
static void sns_sam_get_sensor_buf_req(
   uint8_t algoSvcId,
   void *algoCfgPtr,
   sns_sam_data_req_s *dataReq)
{
   uint32_t algoIndex;

   if( !dataReq )
   {
      SNS_SAM_PRINTF0(ERROR,"sns_sam_get_sensor_buf_req: Bad data req pointer");
      return;
   }

   switch (algoSvcId)
   {
      case SNS_SAM_AMD_SVC_ID_V01:
      {
         qmd_config_s *cfgPtr = (qmd_config_s *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         // AMD starts with periodic request and then switches to buffered request
         dataReq->reportRate = cfgPtr->sample_rate;
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->registerMDInterrupt = true;
         break;
      }
      case SNS_SAM_RMD_SVC_ID_V01:
      {
         dataReq->registerMDInterrupt = true;
      }
      case SNS_SAM_VMD_SVC_ID_V01:
      {
         qmd_config_s *cfgPtr = (qmd_config_s *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#ifdef CONFIG_USE_OEM_1_ALGO   
      case SNS_OEM_1_SVC_ID_V01:
      {
         oem_1_config_s *cfgPtr = (oem_1_config_s *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif //CONFIG_USE_OEM_1_ALGO
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
      case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
      {
         basic_gestures_config_struct *cfgPtr = (basic_gestures_config_struct *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_FACING
      case SNS_SAM_FACING_SVC_ID_V01:
      {
         facing_config_struct *cfgPtr = (facing_config_struct *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_FACING */
      case SNS_SAM_PED_SVC_ID_V01:
      {
          ped_config_s *cfgPtr = (ped_config_s *)algoCfgPtr;
          algoIndex = sns_sam_get_algo_index(algoSvcId);
          dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
          dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
          dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
          dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
          dataReq->sensorCount = 1;
          break;
      }
      case SNS_SAM_CMC_SVC_ID_V01:
      {
         cmc_config_s *cfgPtr = (cmc_config_s *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorCount = 1;
         break;
      }
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         gyro_quat_config_s *cfgPtr = (gyro_quat_config_s *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_GYRO_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorCount = 1;
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         gravity_config_s *cfgPtr = (gravity_config_s *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorCount = 1;
         break;
      }
      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
         orientation_config_s *cfgPtr = (orientation_config_s *)algoCfgPtr;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;

         dataReq->sensorDbase[1].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[1].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[1].sensorId = SNS_SMGR_ID_MAG_V01;
         dataReq->sensorDbase[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorCount = 2;
         break;
      }
      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         fmv_config_s *cfgPtr = (fmv_config_s *)algoCfgPtr;
         int32_t maxMagRateQ16 = sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxSampleRate;
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_GYRO_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;

         dataReq->sensorDbase[1].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[1].sensorId = SNS_SMGR_ID_MAG_V01;
         dataReq->sensorDbase[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[1].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         if (dataReq->sensorDbase[1].sampleRate > (maxMagRateQ16 >> 16))
         {
            dataReq->sensorDbase[1].sampleRate = maxMagRateQ16 >> 16;
         }

         dataReq->sensorCount = 2;
         break;
      }
      case SNS_SAM_SMD_SVC_ID_V01:
      {
         smd_config_s *cfgPtr = (smd_config_s *) algoCfgPtr;
         // SMD TODO: use registry parameters
         algoIndex = sns_sam_get_algo_index(SNS_SAM_AMD_SVC_ID_V01);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorCount = 1;
         break;
      }
      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      {
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         break;
      }
      case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      {
         algoIndex = sns_sam_get_algo_index(algoSvcId);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         break;
      }
      case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      {
         tilt_config_s *cfgPtr = (tilt_config_s *) algoCfgPtr;
         // TILT TODO: use registry parameters
         algoIndex = sns_sam_get_algo_index(SNS_SAM_AMD_SVC_ID_V01);
         dataReq->reportRate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
         dataReq->sensorDbase[0].sampleRate = (uint16_t) cfgPtr->sample_rate;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorCount = 1;
         break;
      }
      default:
         break;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_sensor_samp_req
  =========================================================================*/
/*!
  @brief Gets sensor data requirements for algorithm

  @param[i] algoSvcId: algorithm service id
  @param[i] algoCfgPtr: pointer to algorithm configuration
  @param[o] dataReq: sensor data required by algorithm

  @return None
*/
/*=======================================================================*/
static void sns_sam_get_sensor_samp_req(
   uint8_t algoSvcId,
   void *algoCfgPtr,
   sns_sam_data_req_s *dataReq)
{
   if( !dataReq )
   {
      SNS_SAM_PRINTF0(ERROR,"sns_sam_get_sensor_samp_req: Bad data req pointer");
      return;
   }

   switch (algoSvcId)
   {
      case SNS_SAM_AMD_SVC_ID_V01:
      case SNS_SAM_RMD_SVC_ID_V01:
      {
         dataReq->registerMDInterrupt = true;
      }
      case SNS_SAM_VMD_SVC_ID_V01:
      {
         qmd_config_s *cfgPtr = (qmd_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
      case SNS_SAM_SENSOR_THRESH_SVC_ID_V01:
      {
         threshold_config_s *cfgPtr = (threshold_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);  /* convert to Hz */
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = cfgPtr->sensor_id;
         dataReq->sensorDbase[0].dataType = cfgPtr->data_type;
         break;
      }
#ifdef CONFIG_USE_OEM_1_ALGO    
      case SNS_OEM_1_SVC_ID_V01:
      {
         oem_1_config_s *cfgPtr = (oem_1_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif //CONFIG_USE_OEM_1_ALGO
      case SNS_SAM_MODEM_SCN_SVC_ID_V01:
      {
         modem_scn_config_s *cfgPtr = (modem_scn_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);  /* convert to Hz */
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
      case SNS_SAM_GYROBUF_SVC_ID_V01:
      {
         /* Requires only gyro data, so only sensorReq[0] */
         gyrobuf_config_s *cfgPtr = (gyrobuf_config_s *) algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);  /* Q16 --> Hz */
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_GYRO_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#ifdef FEATURE_SNS_SAM_FNS
      case SNS_SAM_FNS_SVC_ID_V01:
      {
         face_n_shake_config_s *cfgPtr = (face_n_shake_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);  /* convert to Hz */
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
      case SNS_SAM_BTE_SVC_ID_V01:
      {
         bring_to_ear_config_s *cfgPtr = (bring_to_ear_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);  /* convert to Hz */
         if (cfgPtr->prox_enabled)
         {
            dataReq->sensorCount = 2;
            dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
            dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
            dataReq->sensorDbase[1].sensorId = SNS_SMGR_ID_PROX_LIGHT_V01;
            dataReq->sensorDbase[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         }
         else
         {
            dataReq->sensorCount = 1;
            dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
            dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         }
         break;
      }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
      case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
      {
         basic_gestures_config_struct *cfgPtr = (basic_gestures_config_struct *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
      case SNS_SAM_TAP_SVC_ID_V01:
      {
         tap_config_struct *cfgPtr = (tap_config_struct *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);  /* convert to Hz */
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
      case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
      {
         integ_angle_config_struct *cfgPtr = (integ_angle_config_struct *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);  /* convert to Hz */
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_GYRO_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
      case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
      {
         dataReq->reportRate = GTAP_SAMPLE_RATE;
         dataReq->sensorCount = 2;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[1].sensorId = SNS_SMGR_ID_GYRO_V01;
         dataReq->sensorDbase[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_GYRO_TAP */
#ifdef FEATURE_SNS_SAM_FACING
      case SNS_SAM_FACING_SVC_ID_V01:
      {
         facing_config_struct *cfgPtr = (facing_config_struct *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
#endif /* FEATURE_SNS_SAM_FACING */
      case SNS_SAM_PED_SVC_ID_V01:
      {
          ped_config_s *cfgPtr = (ped_config_s *)algoCfgPtr;
          dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
          dataReq->sensorCount = 1;
          dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
          dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
          break;
      }
      case SNS_SAM_CMC_SVC_ID_V01:
      {
         cmc_config_s *cfgPtr = (cmc_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         gyro_quat_config_s *cfgPtr = (gyro_quat_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_GYRO_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         gravity_config_s *cfgPtr = (gravity_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         break;
      }
      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
         orientation_config_s *cfgPtr = (orientation_config_s *)algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 2;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[1].sensorId = SNS_SMGR_ID_MAG_V01;
         dataReq->sensorDbase[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[1].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;

         break;
      }
      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         fmv_config_s *cfgPtr = (fmv_config_s *)algoCfgPtr;
         int32_t maxMagRate = sns_sam_sensor_info_dbase_acc(SNS_SAM_MAG)->maxSampleRate >> 16;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->reportRate = dataReq->reportRate > maxMagRate ? maxMagRate : dataReq->reportRate;
         dataReq->sensorCount = 2;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_GYRO_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[0].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;
         dataReq->sensorDbase[1].sensorId = SNS_SMGR_ID_MAG_V01;
         dataReq->sensorDbase[1].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         dataReq->sensorDbase[1].sampleQual = SNS_SMGR_SAMPLE_QUALITY_ACCURATE_TIMESTAMP_V01;

         break;
       }
      case SNS_SAM_SMD_SVC_ID_V01:
      {
         smd_config_s *cfgPtr = (smd_config_s *) algoCfgPtr;
         dataReq->reportRate = FX_CONV(cfgPtr->sample_rate, 16, 0);
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
      case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      {
         tilt_config_s *cfgPtr = (tilt_config_s *) algoCfgPtr;
         dataReq->reportRate = (uint16_t) cfgPtr->sample_rate;
         dataReq->sensorCount = 1;
         dataReq->sensorDbase[0].sensorId = SNS_SMGR_ID_ACCEL_V01;
         dataReq->sensorDbase[0].dataType = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
         break;
      }
      default:
         break;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_sensor_requirements
  =========================================================================*/
/*!
  @brief Gets sensor data requirements for algorithm

  @param[i] algoSvcId: algorithm service id
  @param[i] algoCfgPtr: pointer to algorithm configuration
  @param[o] dataReq: sensor data required by algorithm

  @return None
*/
/*=======================================================================*/
void sns_sam_get_sensor_requirements(
   uint8_t algoSvcId,
   void *algoCfgPtr,
   sns_sam_data_req_s *dataReq)
{
   uint32_t i, j;

   if( !dataReq )
   {
      SNS_SAM_PRINTF0(ERROR,"sns_sam_get_sensor_requirements: Bad data req pointer");
      return;
   }

   for( i = 0; i < SNS_SAM_MAX_SENSORS_PER_DATA_REQ; i++ )
   {
      dataReq->sensorDbase[i].sampleRate = SNS_SAM_INVALID_SAMPLE_RATE;
      dataReq->sensorDbase[i].sampleQual = SNS_SAM_DEFAULT_SAMPLE_QUALITY;
      dataReq->sensorDbase[i].algoInstCount = 0;

      for(j = 0; j < SNS_SAM_MAX_ALGO_INSTS_PER_SENSOR; j++)
      {
         dataReq->sensorDbase[i].algoInstIds[j] = SNS_SAM_INVALID_ID;
      }
   }

   dataReq->reportRate = 0;
   dataReq->sensorCount = 0;
   dataReq->algoInstCount = 0;
   dataReq->registerMDInterrupt = false;

   if (sns_sam_get_smgr_msg_req_type(algoSvcId) == SNS_SMGR_BUFFERING_REQ_V01)
   {
      sns_sam_get_sensor_buf_req(algoSvcId, algoCfgPtr, dataReq);
   }
   else
   {
      sns_sam_get_sensor_samp_req(algoSvcId, algoCfgPtr, dataReq);
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_send_algo_report_req
  =========================================================================*/
/*!
  @brief Sends a report request to the desired module

  @param[i] algoInstId: algorithm instance ID
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] clientReqMsgPtr: pointer to client request message

  @return   error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_send_algo_report_req(
   uint8_t                            algoInstId,
   const sns_sam_algo_inst_s          *algoInstPtr,
   const void                         *clientReqMsgPtr)
{
  /* Empty Stub; do nothing */
  return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_enable_resp
  =========================================================================*/
/*!
  @brief Send response to algorithm enable request

  @param[i] algoInstId: algorithm instance id
  @param[i] algoSvcId: algorithm service id
  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_prep_algo_enable_resp(
   uint8_t algoInstId,
   uint8_t algoSvcId,
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr)
{
       SNS_SAM_PRINTF2(LOW,
                      "sns_sam_prep_algo_enable_resp: algo inst %d, algo svc %d",
                      algoInstId, algoSvcId);


#ifdef FEATURE_SNS_SAM_FNS
   if (SNS_SAM_FNS_SVC_ID_V01  == algoSvcId)
   {
      sns_sam_fns_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_fns_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_fns_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   else if (SNS_SAM_BTE_SVC_ID_V01  == algoSvcId)
   {
      sns_sam_bte_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_bte_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_bte_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   else if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01  == algoSvcId)
   {
      sns_sam_basic_gestures_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_basic_gestures_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
   else if (SNS_SAM_TAP_SVC_ID_V01  == algoSvcId)
   {
      sns_sam_tap_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_tap_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
   else if (SNS_SAM_INTEG_ANGLE_SVC_ID_V01  == algoSvcId)
   {
      sns_sam_integ_angle_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_integ_angle_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_integ_angle_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
   else if (SNS_SAM_FACING_SVC_ID_V01  == algoSvcId)
   {
      sns_sam_facing_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_facing_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   else if (SNS_SAM_GYRO_TAP2_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gyro_tap2_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyro_tap2_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_gyro_tap2_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif // FEATURE_SNS_SAM_GYRO_TAP
   // TODO: refactor and move to common SAM
   else if (SNS_SAM_SVC_IS_QMD_ALGO(algoSvcId))
   {
      sns_sam_qmd_enable_resp_msg_v01 *enableRespMsgPtr =
      sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_qmd_enable_resp_msg_v01));

      if (algoSvcId != SNS_SAM_AMD_SVC_ID_V01 &&
          algoSvcId != SNS_SAM_RMD_SVC_ID_V01)
      {
         sns_sam_motion_client_update(true);
      }
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      sns_sam_sensor_thresh_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = 1;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_sensor_thresh_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (algoSvcId == SNS_OEM_1_SVC_ID_V01)
   {
      sns_oem_1_enable_resp_msg_v01 *enableRespMsgPtr =
      sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_enable_resp_msg_v01));  
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = 1;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_oem_1_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;                            
      return SNS_SUCCESS;
   }
#endif //CONFIG_USE_OEM_1_ALGO
   else if (algoSvcId == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      sns_sam_modem_scn_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = 1;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_modem_scn_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      sns_sam_gyrobuf_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyrobuf_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = 1;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_gyrobuf_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_PED_SVC_ID_V01)
   {
       sns_sam_ped_enable_resp_msg_v01 *enableRespMsgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_ped_enable_resp_msg_v01));
       SNS_ASSERT(enableRespMsgPtr != NULL);

       enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
       enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
       enableRespMsgPtr->instance_id_valid = 1;
       enableRespMsgPtr->instance_id = algoInstId;
       sns_sam_prep_resp_msg(clientReqMsgPtr, enableRespMsgPtr, sizeof(sns_sam_ped_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
       return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_PAM_SVC_ID_V01)
   {
       sns_sam_pam_enable_resp_msg_v01 *enableRespMsgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_pam_enable_resp_msg_v01));
       SNS_ASSERT(enableRespMsgPtr != NULL);

       enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
       enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
       enableRespMsgPtr->instance_id_valid = 1;
       enableRespMsgPtr->instance_id = algoInstId;
       sns_sam_prep_resp_msg(clientReqMsgPtr, enableRespMsgPtr, sizeof(sns_sam_pam_enable_resp_msg_v01));
       *clientRespMsgPtr = enableRespMsgPtr;
       return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_CMC_SVC_ID_V01)
   {
       sns_sam_cmc_enable_resp_msg_v01 *enableRespMsgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_cmc_enable_resp_msg_v01));
       SNS_ASSERT(enableRespMsgPtr != NULL);

       enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
       enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
       enableRespMsgPtr->instance_id_valid = 1;
       enableRespMsgPtr->instance_id = algoInstId;
       sns_sam_prep_resp_msg(clientReqMsgPtr, enableRespMsgPtr, sizeof(sns_sam_cmc_enable_resp_msg_v01));
       *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
       sns_sam_distance_bound_enable_resp_msg_v01 *enableRespMsgPtr =
           sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_distance_bound_enable_resp_msg_v01));
       SNS_ASSERT(enableRespMsgPtr != NULL);

       enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
       enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
       enableRespMsgPtr->instance_id_valid = 1;
       enableRespMsgPtr->instance_id = algoInstId;
       sns_sam_prep_resp_msg(clientReqMsgPtr, enableRespMsgPtr, sizeof(sns_sam_distance_bound_enable_resp_msg_v01));
       *clientRespMsgPtr = enableRespMsgPtr;
       return SNS_SUCCESS;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_quat_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_quat_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gravity_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gravity_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_gravity_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_filtered_mag_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_filtered_mag_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_rotation_vector_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_rotation_vector_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_orientation_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = true;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_orientation_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_SMD_SVC_ID_V01)
   {
      sns_sam_smd_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_smd_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = 1;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_smd_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_game_rotation_vector_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                           sizeof(sns_sam_game_rotation_vector_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = 1;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_game_rotation_vector_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_tilt_detector_enable_resp_msg_v01 *enableRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                           sizeof(sns_sam_tilt_detector_enable_resp_msg_v01));
      SNS_ASSERT(enableRespMsgPtr != NULL);

      enableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      enableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      enableRespMsgPtr->instance_id_valid = 1;
      enableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            enableRespMsgPtr,
                            sizeof(sns_sam_tilt_detector_enable_resp_msg_v01));
      *clientRespMsgPtr = enableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else
   {
      return SNS_ERR_BAD_PARM;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_disable_resp
  =========================================================================*/
/*!
  @brief Prepare a response to algorithm disable request

  @param[i] algoInstId: algorithm instance id
  @param[i] algoSvcId: algorithm service id
  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_prep_algo_disable_resp(
   uint8_t algoInstId,
   uint8_t algoSvcId,
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr)
{
   *clientRespMsgPtr = NULL;

   SNS_SAM_PRINTF2(LOW,
                  "sns_sam_prep_algo_disable_resp: algo inst %d, algo svc %d",
                  algoInstId, algoSvcId);

#ifdef FEATURE_SNS_SAM_FNS
   if (SNS_SAM_FNS_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_fns_disable_req_msg_v01 *disableReqPtr =
         (sns_sam_fns_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_fns_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_fns_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_fns_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
   if (SNS_SAM_BTE_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_bte_disable_req_msg_v01 *disableReqPtr =
         (sns_sam_bte_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_bte_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_bte_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_bte_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
   if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_basic_gestures_disable_req_msg_v01 *disableReqPtr =
         (sns_sam_basic_gestures_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_basic_gestures_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_basic_gestures_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
   if (SNS_SAM_TAP_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_tap_disable_req_msg_v01 *disableReqPtr =
         (sns_sam_tap_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_tap_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_tap_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
   if (SNS_SAM_INTEG_ANGLE_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_integ_angle_disable_req_msg_v01 *disableReqPtr =
         (sns_sam_integ_angle_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_integ_angle_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_integ_angle_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_integ_angle_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
   if (SNS_SAM_FACING_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_facing_disable_req_msg_v01 *disableReqPtr =
         (sns_sam_facing_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_facing_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_facing_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
   if (SNS_SAM_GYRO_TAP2_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gyro_tap2_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_gyro_tap2_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_gyro_tap2_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyro_tap2_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_gyro_tap2_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif // FEATURE_SNS_SAM_GYRO_TAP
   // TODO: refactor and move to common SAM
   if (SNS_SAM_SVC_IS_QMD_ALGO(algoSvcId))
   {
      sns_sam_qmd_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_qmd_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_qmd_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_qmd_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      if (algoSvcId != SNS_SAM_AMD_SVC_ID_V01 &&
          algoSvcId != SNS_SAM_RMD_SVC_ID_V01)
      {
         sns_sam_motion_client_update(false);
      }

      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      sns_sam_sensor_thresh_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_sensor_thresh_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_sensor_thresh_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_sensor_thresh_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (algoSvcId == SNS_OEM_1_SVC_ID_V01)
   {
      sns_oem_1_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_oem_1_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_oem_1_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_oem_1_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif //CONFIG_USE_OEM_1_ALGO 
   else if (algoSvcId == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      sns_sam_modem_scn_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_modem_scn_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_modem_scn_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_modem_scn_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      sns_sam_gyrobuf_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_gyrobuf_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_gyrobuf_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gyrobuf_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_gyrobuf_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_PED_SVC_ID_V01)
   {
      sns_sam_ped_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_ped_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_ped_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                            sizeof(sns_sam_ped_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_ped_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_PAM_SVC_ID_V01)
   {
      sns_sam_pam_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_pam_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_pam_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                            sizeof(sns_sam_pam_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_pam_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_CMC_SVC_ID_V01)
   {
      sns_sam_cmc_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_cmc_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_cmc_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);

         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                            sizeof(sns_sam_cmc_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_cmc_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      sns_sam_distance_bound_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_distance_bound_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_distance_bound_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);

         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                            sizeof(sns_sam_distance_bound_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_distance_bound_disable_resp_msg_v01));
     *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_quat_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_quat_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_quat_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_quat_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gravity_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_gravity_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_gravity_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gravity_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_gravity_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_filtered_mag_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_filtered_mag_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_filtered_mag_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = true;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_filtered_mag_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_rotation_vector_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_rotation_vector_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_rotation_vector_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id = algoInstId;
      disableRespMsgPtr->instance_id_valid = true;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_rotation_vector_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_orientation_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_orientation_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_orientation_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id = algoInstId;
      disableRespMsgPtr->instance_id_valid = true;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_orientation_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_SMD_SVC_ID_V01)
   {
      sns_sam_smd_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_smd_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_smd_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR,
                        DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id,
                        algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_smd_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_smd_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_game_rotation_vector_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_game_rotation_vector_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_game_rotation_vector_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id, algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                            sizeof(sns_sam_game_rotation_vector_disable_resp_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id = algoInstId;
      disableRespMsgPtr->instance_id_valid = true;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_game_rotation_vector_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01)
   {
      sns_sam_tilt_detector_disable_req_msg_v01 *disableReqMsgPtr =
         (sns_sam_tilt_detector_disable_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_tilt_detector_disable_resp_msg_v01 *disableRespMsgPtr;

      if (algoInstId != disableReqMsgPtr->instance_id)
      {
         SNS_SAM_DEBUG2(ERROR,
                        DBG_SAM_PROCESS_REQ_DISABLE_ERR,
                        disableReqMsgPtr->instance_id,
                        algoInstId);
         return SNS_ERR_BAD_PARM;
      }

      disableRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_tilt_detector_disable_req_msg_v01));
      SNS_ASSERT(disableRespMsgPtr != NULL);

      disableRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      disableRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      disableRespMsgPtr->instance_id_valid = 1;
      disableRespMsgPtr->instance_id = algoInstId;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            disableRespMsgPtr,
                            sizeof(sns_sam_tilt_detector_disable_resp_msg_v01));
      *clientRespMsgPtr = disableRespMsgPtr;
      return SNS_SUCCESS;
   }
   else
   {
      return SNS_ERR_BAD_PARM;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_report_resp
  =========================================================================*/
/*!
  @brief Prepare a response to algorithm enable request

  @param[i] clientReqPtr: pointer to client request
  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.
  @param[i] algoSvcId: algorithm service id

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_prep_algo_report_resp(
   const sns_sam_client_req_s* clientReqPtr,
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr,
   uint8_t algoSvcId)
{
       sns_sam_algo_rpt_s *algoRptPtr;

       uint8_t algoInstId = clientReqPtr->algoInstId;
   *clientRespMsgPtr = NULL;

       SNS_SAM_PRINTF2(LOW,
                      "sns_sam_prep_algo_report_resp: client %d, algo service %d",
                      clientReqPtr->mrClientId, algoSvcId);

#ifdef FEATURE_SNS_SAM_FNS
       if (SNS_SAM_FNS_SVC_ID_V01 == algoSvcId)
       {
          int32_t fnsEvent;
          sns_sam_fns_get_report_resp_msg_v01 *reportRespMsgPtr;
          sns_sam_fns_get_report_req_msg_v01 *reportReqPtr =
             (sns_sam_fns_get_report_req_msg_v01 *)clientReqMsgPtr;

          algoInstId = reportReqPtr->instance_id;

          // get report
          algoRptPtr = sns_sam_get_algo_report(algoInstId);
          if(algoRptPtr == NULL)
          {
             return SNS_ERR_FAILED;
          }

          fnsEvent = ((face_n_shake_output_s *)(algoRptPtr->memPtr))->face_n_shake_event;

          reportRespMsgPtr =
             sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_fns_get_report_resp_msg_v01));
          SNS_ASSERT(reportRespMsgPtr != NULL);

          reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
          reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
          reportRespMsgPtr->instance_id = algoInstId;
          reportRespMsgPtr->timestamp = algoRptPtr->timestamp;
          reportRespMsgPtr->state = (sns_sam_face_and_shake_event_e_v01)fnsEvent;

          //send algo report response to client
          sns_sam_prep_resp_msg(clientReqMsgPtr,
                                reportRespMsgPtr,
                                sizeof(sns_sam_fns_get_report_resp_msg_v01));
          *clientRespMsgPtr = reportRespMsgPtr;

          //log FNS report
          sns_sam_log_fns_report( clientReqPtr, fnsEvent, algoRptPtr->timestamp);

          return SNS_SUCCESS;
       }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
       else if (SNS_SAM_BTE_SVC_ID_V01 == algoSvcId)
       {
          int32_t bteEvent;
          sns_sam_bte_get_report_resp_msg_v01 *reportRespMsgPtr;
          sns_sam_bte_get_report_req_msg_v01 *reportReqPtr =
             (sns_sam_bte_get_report_req_msg_v01 *)clientReqMsgPtr;

          algoInstId = reportReqPtr->instance_id;

          // get report
          algoRptPtr = sns_sam_get_algo_report(algoInstId);
          if(algoRptPtr == NULL)
          {
             return SNS_ERR_FAILED;
          }

          bteEvent = ((bring_to_ear_output_s *)(algoRptPtr->memPtr))->bring_to_ear_event;

          reportRespMsgPtr =
             sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_bte_get_report_resp_msg_v01));
          SNS_ASSERT(reportRespMsgPtr != NULL);

          reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
          reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
          reportRespMsgPtr->instance_id = algoInstId;
          reportRespMsgPtr->timestamp = algoRptPtr->timestamp;
          reportRespMsgPtr->state = (sns_sam_bring_to_ear_event_e_v01)bteEvent;

          // send algo report response to client
          sns_sam_prep_resp_msg(clientReqMsgPtr,
                                reportRespMsgPtr,
                                sizeof(sns_sam_bte_get_report_resp_msg_v01));
          *clientRespMsgPtr = reportRespMsgPtr;

          //log BTE report
          sns_sam_log_bte_report( clientReqPtr, bteEvent, algoRptPtr->timestamp);

          return SNS_SUCCESS;
       }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
       else if (SNS_SAM_BASIC_GESTURES_SVC_ID_V01 == algoSvcId)
       {
          int32_t basicGesturesEvent;
          sns_sam_basic_gestures_get_report_resp_msg_v01 *reportRespMsgPtr;
          sns_sam_basic_gestures_get_report_req_msg_v01 *reportReqPtr =
             (sns_sam_basic_gestures_get_report_req_msg_v01 *)clientReqMsgPtr;

          algoInstId = reportReqPtr->instance_id;

          // get report
          algoRptPtr = sns_sam_get_algo_report(algoInstId);
          if(algoRptPtr == NULL)
          {
            return SNS_ERR_FAILED;
          }

          basicGesturesEvent = ((basic_gestures_output_struct *)(algoRptPtr->memPtr))->basic_state;

          reportRespMsgPtr =
             sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_basic_gestures_get_report_resp_msg_v01));
          SNS_ASSERT(reportRespMsgPtr != NULL);

          reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
          reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
          reportRespMsgPtr->instance_id_valid = true;
          reportRespMsgPtr->instance_id = algoInstId;

          reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

          reportRespMsgPtr->state = (sns_sam_basic_gestures_event_e_v01)basicGesturesEvent;

          // send algo report response to client
          sns_sam_prep_resp_msg(clientReqMsgPtr,
                                reportRespMsgPtr,
                                sizeof(sns_sam_basic_gestures_get_report_resp_msg_v01));
          *clientRespMsgPtr = reportRespMsgPtr;

          //log BTE report
          sns_sam_log_basic_gestures_report( clientReqPtr, basicGesturesEvent, algoRptPtr->timestamp);

          return SNS_SUCCESS;
       }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
       else if (SNS_SAM_TAP_SVC_ID_V01 == algoSvcId)
       {
          int32_t tapEvent;
          sns_sam_tap_get_report_resp_msg_v01 *reportRespMsgPtr;
          sns_sam_tap_get_report_req_msg_v01 *reportReqPtr =
             (sns_sam_tap_get_report_req_msg_v01 *)clientReqMsgPtr;

          algoInstId = reportReqPtr->instance_id;

          // get report
          algoRptPtr = sns_sam_get_algo_report(algoInstId);
          if(algoRptPtr == NULL)
          {
            return SNS_ERR_FAILED;
          }

          tapEvent = ((tap_output_struct *)(algoRptPtr->memPtr))->tap_state;

          reportRespMsgPtr =
             sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_tap_get_report_resp_msg_v01));
          SNS_ASSERT(reportRespMsgPtr != NULL);

          reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
          reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
          reportRespMsgPtr->instance_id_valid = true;
          reportRespMsgPtr->instance_id = algoInstId;

          reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

          reportRespMsgPtr->state = (sns_sam_tap_event_e_v01)tapEvent;

          // send algo report response to client
          sns_sam_prep_resp_msg(clientReqMsgPtr,
                                reportRespMsgPtr,
                                sizeof(sns_sam_tap_get_report_resp_msg_v01));
          *clientRespMsgPtr = reportRespMsgPtr;

          //log tap report
          sns_sam_log_tap_report( clientReqPtr, tapEvent, algoRptPtr->timestamp);

          return SNS_SUCCESS;
       }
#endif /* FEATURE_SNS_SAM_TAP */


#ifdef FEATURE_SNS_SAM_FACING
       else if (SNS_SAM_FACING_SVC_ID_V01 == algoSvcId)
       {
          facing_state_e facingState;
          sns_sam_facing_get_report_resp_msg_v01 *reportRespMsgPtr;
          sns_sam_facing_get_report_req_msg_v01 *reportReqPtr =
             (sns_sam_facing_get_report_req_msg_v01 *)clientReqMsgPtr;

          algoInstId = reportReqPtr->instance_id;

          // get report
          algoRptPtr = sns_sam_get_algo_report(algoInstId);
          if(algoRptPtr == NULL)
          {
            return SNS_ERR_FAILED;
          }

          facingState = ((facing_output_struct *)(algoRptPtr->memPtr))->facing_state;

          reportRespMsgPtr =
             sns_smr_msg_alloc(SNS_SAM_DBG_MOD, sizeof(sns_sam_facing_get_report_resp_msg_v01));
          SNS_ASSERT(reportRespMsgPtr != NULL);

          reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
          reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
          reportRespMsgPtr->instance_id_valid = true;
          reportRespMsgPtr->instance_id = algoInstId;

          reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

          reportRespMsgPtr->state = facingState;

          // send algo report response to client
          sns_sam_prep_resp_msg(clientReqMsgPtr,
                                reportRespMsgPtr,
                                sizeof(sns_sam_facing_get_report_resp_msg_v01));
          *clientRespMsgPtr = reportRespMsgPtr;

          //log facing report
          sns_sam_log_facing_report( clientReqPtr, facingState, algoRptPtr->timestamp);

          return SNS_SUCCESS;
       }
#endif /* FEATURE_SNS_SAM_FACING */

   else if (SNS_SAM_SVC_IS_QMD_ALGO(algoSvcId))
   {
      int32_t motionState;
      uint32_t timeStamp;

      sns_sam_qmd_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_qmd_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_qmd_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }

      switch (algoSvcId)
      {
      case SNS_SAM_AMD_SVC_ID_V01:
         motionState = ((qmd_output_s *)(algoRptPtr->memPtr))->absolute_rest_state;
         break;
      case SNS_SAM_VMD_SVC_ID_V01:
         motionState = ((qmd_output_s *)(algoRptPtr->memPtr))->vehicle_rest_state;
         break;
      case SNS_SAM_RMD_SVC_ID_V01:
         motionState = ((qmd_output_s *)(algoRptPtr->memPtr))->relative_rest_state;
         break;
      default:
         return SNS_ERR_FAILED;
      }

      reportRespMsgPtr =
      sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_qmd_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id = algoInstId;

      timeStamp = algoRptPtr->timestamp;
      reportRespMsgPtr->timestamp = timeStamp;

      reportRespMsgPtr->state = (sns_sam_motion_state_e_v01)motionState;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_qmd_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;

      //log QMD report
      sns_sam_log_qmd_report(clientReqPtr, motionState, timeStamp, algoSvcId);

      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_SENSOR_THRESH_SVC_ID_V01)
   {
      uint8_t i;
      threshold_output_s *rptPtr;

      sns_sam_sensor_thresh_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_sensor_thresh_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_sensor_thresh_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (threshold_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_sensor_thresh_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id = algoInstId;
      reportRespMsgPtr->timestamp_valid = 1;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;
      reportRespMsgPtr->sample_value_valid = 1;
      for (i=0; i < rptPtr->num_axis; i++)
         reportRespMsgPtr->sample_value[i] = rptPtr->sample[i];

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_sensor_thresh_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
      return SNS_SUCCESS;
   }
#ifdef CONFIG_USE_OEM_1_ALGO   
   else if (algoSvcId == SNS_OEM_1_SVC_ID_V01)
   {
      int i;
      oem_1_output_s *outPtr ;

      sns_oem_1_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_oem_1_get_report_req_msg_v01 *reportReqPtr =
         (sns_oem_1_get_report_req_msg_v01 *)clientReqMsgPtr;
      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      outPtr = (oem_1_output_s*)(algoRptPtr->memPtr);

      reportRespMsgPtr = 
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_oem_1_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;

      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id = algoInstId;
 
      reportRespMsgPtr->timestamp_valid = 1;
      reportRespMsgPtr->timestamp = outPtr->timestamp;

      reportRespMsgPtr->output_valid = 1;
      for (i=0; i < 3; i++) //3 is the number of axis in oem.h
      {
         reportRespMsgPtr->output.data[i] = outPtr->data[i];
      }
      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_oem_1_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
      return SNS_SUCCESS;
   }
#endif //CONFIG_USE_OEM_1_ALGO
   else if (algoSvcId == SNS_SAM_MODEM_SCN_SVC_ID_V01)
   {
      modem_scn_output_s *rptPtr;

      sns_sam_modem_scn_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_modem_scn_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_modem_scn_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (modem_scn_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_modem_scn_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id = algoInstId;
      reportRespMsgPtr->timestamp_valid = 1;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;
      reportRespMsgPtr->scenario_valid = 1;
      reportRespMsgPtr->scenario = rptPtr->scn;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_modem_scn_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_GYROBUF_SVC_ID_V01)
   {
      gyrobuf_output_s  *rptPtr;

      sns_sam_gyrobuf_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_gyrobuf_get_report_req_msg_v01  *reportReqPtr =
         (sns_sam_gyrobuf_get_report_req_msg_v01 *)clientReqMsgPtr;
      sns_smr_header_s  reportReqHdrPtr;

      algoInstId = reportReqPtr->instance_id;
      sns_smr_get_hdr(&reportReqHdrPtr, reportReqPtr);

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (gyrobuf_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_gyrobuf_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      /* Enqueues this request */
      /* 'true' if successful. 'false' if the queue is full. */
      if( gyrobuf_sam_enqueue_req(rptPtr, reportReqPtr->t_start, reportReqPtr->t_end,
                                  reportReqPtr->seqnum) )
      {
         /* Populates report response */
         reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
         reportRespMsgPtr->resp.sns_err_t    = SNS_SUCCESS;
         reportRespMsgPtr->instance_id_valid = 1;
         reportRespMsgPtr->instance_id       = algoInstId;
      }
      else
      {
         /* queue is full --> not enough buffer memory */
         reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_FAILURE_V01;
         reportRespMsgPtr->resp.sns_err_t    = SNS_ERR_NOMEM;
         reportRespMsgPtr->instance_id_valid = 1;
         reportRespMsgPtr->instance_id       = algoInstId;
      }

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_gyrobuf_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if(algoSvcId == SNS_SAM_PED_SVC_ID_V01)
   {
      sns_sam_algo_mem_s * algoStatePtr;
      ped_output_s *rptPtr;
      ped_state_s *statePtr;
      uint8_t i;

      sns_sam_ped_get_report_resp_msg_v01 *reportRespMsgPtr = NULL;
      sns_sam_ped_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_ped_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report and state
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      algoStatePtr = sns_sam_get_algo_state(algoInstId);

      if(algoRptPtr == NULL || algoStatePtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (ped_output_s *)(algoRptPtr->memPtr);
      statePtr = (ped_state_s *)(algoStatePtr->memPtr);

      for( i = 0; i < rptPtr->num_active_clients; ++i )
      {
         if( rptPtr->client_data[i].client_id == clientReqPtr->mrClientId )
         {
            reportRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                                 sizeof(sns_sam_ped_get_report_resp_msg_v01));
            SNS_ASSERT(reportRespMsgPtr != NULL);

            reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
            reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
            reportRespMsgPtr->instance_id_valid = 1;
            reportRespMsgPtr->instance_id = algoInstId;
            reportRespMsgPtr->timestamp_valid = 1;
            reportRespMsgPtr->timestamp = statePtr->step_timestamp;
            reportRespMsgPtr->report_data_valid = 1;
            reportRespMsgPtr->report_data.step_event = rptPtr->client_data[i].step_event;
            reportRespMsgPtr->report_data.step_confidence = statePtr->step_confidence;
            reportRespMsgPtr->report_data.step_count = rptPtr->client_data[i].step_count + statePtr->client_data[i].step_count;
            reportRespMsgPtr->report_data.step_count_error = statePtr->client_data[i].step_count_error;
            reportRespMsgPtr->report_data.step_rate = statePtr->step_rate;

            // send algo report response to client
            sns_sam_prep_resp_msg(clientReqMsgPtr,
                reportRespMsgPtr,
                sizeof(sns_sam_ped_get_report_resp_msg_v01));

            // log Pedometer report
            sns_sam_log_ped_report( clientReqPtr, &rptPtr->client_data[i] );

            // Clear step rate after every report
            statePtr->client_data[i].total_step_rate = 0.0f;
            rptPtr->client_data[i].step_rate = 0.0f;
            statePtr->client_data[i].steps_since_report = 0;
         }
      }
      *clientRespMsgPtr = reportRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if(algoSvcId == SNS_SAM_PAM_SVC_ID_V01)
   {
      pam_output_s *rptPtr;

      sns_sam_pam_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_pam_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_pam_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (pam_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_pam_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id = algoInstId;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;
      reportRespMsgPtr->step_count = rptPtr->step_count;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_pam_get_report_resp_msg_v01));

      //log pam report
      sns_sam_log_pam_report(clientReqPtr, rptPtr->step_count, algoRptPtr->timestamp);

      *clientRespMsgPtr = reportRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if(algoSvcId == SNS_SAM_CMC_SVC_ID_V01)
   {
      uint8_t i=0, j=0;
      cmc_output_s *rptPtr;

      sns_sam_cmc_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_cmc_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_cmc_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (cmc_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_cmc_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id = algoInstId;

      for (i = 0; i < CMC_MS_LEAF_NUM_WITH_UNKNOWN; i++)
      {
         if ((rptPtr->motion_outputs[i].motion_event == CMC_MOTION_EVENT_ENTRY) ||
             (rptPtr->motion_outputs[i].motion_event == CMC_MOTION_EVENT_IN))
         {
            reportRespMsgPtr->timestamp_valid = 1;
            reportRespMsgPtr->timestamp[j] = rptPtr->motion_outputs[i].timestamp;

            reportRespMsgPtr->report_data_valid = 1;
            reportRespMsgPtr->report_data[j].motion_state = rptPtr->motion_outputs[i].motion_state;

            j++;
         }
      }

      if (j > 0)
      {
         reportRespMsgPtr->timestamp_len = j;
         reportRespMsgPtr->report_data_len = j;
      }

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_cmc_get_report_resp_msg_v01));

      *clientRespMsgPtr = reportRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if(algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      distance_bound_output_s *rptPtr;

      sns_sam_distance_bound_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_distance_bound_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_distance_bound_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);

      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (distance_bound_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_distance_bound_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = 1;
      reportRespMsgPtr->instance_id = algoInstId;
      reportRespMsgPtr->timestamp_valid = 1;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;
      reportRespMsgPtr->report_data.session_key = rptPtr->client_data.session_key;
      reportRespMsgPtr->report_data_valid = 1;
      reportRespMsgPtr->report_data.distance_bound_breach_event = rptPtr->client_data.is_distance_bound_breached;
      reportRespMsgPtr->distance_accumulated_valid = TRUE;
      reportRespMsgPtr->distance_accumulated = rptPtr->client_data.distance_accumulated;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_distance_bound_get_report_resp_msg_v01));

      //log pam report
      sns_sam_log_distance_bound_report(clientReqPtr,
                                        reportRespMsgPtr->report_data.distance_bound_breach_event,
                                        algoRptPtr->timestamp,
                                        reportRespMsgPtr->report_data.session_key);

      *clientRespMsgPtr = reportRespMsgPtr;
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_QUATERNION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_quat_result_s_v01 quatResult;
      sns_sam_quat_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_quat_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_quat_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_quat_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id = algoInstId;

      SNS_OS_MEMCOPY(quatResult.quaternion,
                     algoRptPtr->memPtr,
                     sizeof(quatResult.quaternion));
      reportRespMsgPtr->result_valid = true;
      reportRespMsgPtr->result = quatResult;  /* structure assignment */

      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_quat_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;

      //log quaternion report
      sns_sam_log_gyro_quat_report(clientReqPtr, &quatResult, algoRptPtr->timestamp);

      return SNS_SUCCESS;
   }
   else if (SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_gravity_result_s_v01 gravityResult;
      sns_sam_gravity_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_gravity_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_gravity_get_report_req_msg_v01 *)clientReqMsgPtr;
      gravity_output_s *outPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      outPtr = (gravity_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_gravity_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id = algoInstId;

      reportRespMsgPtr->result_valid = true;
      SNS_OS_MEMCOPY(gravityResult.gravity, outPtr->gravity, sizeof(gravityResult.gravity));
      SNS_OS_MEMCOPY(gravityResult.lin_accel, outPtr->lin_accel, sizeof(gravityResult.lin_accel));
      gravityResult.accuracy = outPtr->accuracy;
      reportRespMsgPtr->result = gravityResult;  /* structure assignment */

      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_gravity_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;

      //log gravity report
      sns_sam_log_gravity_report(clientReqPtr, &gravityResult, algoRptPtr->timestamp);

      return SNS_SUCCESS;
   }
   else if (SNS_SAM_FILTERED_MAG_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_filtered_mag_result_s_v01 result;
      fmv_output_s *outPtr;
      sns_sam_filtered_mag_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_filtered_mag_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_filtered_mag_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_filtered_mag_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id = algoInstId;

      reportRespMsgPtr->result_valid = true;
      outPtr = (fmv_output_s *) (algoRptPtr->memPtr);
      result.accuracy = outPtr->accuracy;
      result.filtered_mag[0] = outPtr->filtered_mag[0];
      result.filtered_mag[1] = outPtr->filtered_mag[1];
      result.filtered_mag[2] = outPtr->filtered_mag[2];
      reportRespMsgPtr->result = result;  /* structure assignment */

      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_filtered_mag_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;

      //log report
      sns_sam_log_fmv_report(&result, clientReqPtr, algoRptPtr->timestamp);

      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_rotation_vector_result_s_v01 rotationVectorResult;
      rotation_vector_output_s *rotVecOutPtr;
      uint8_t idx;

      sns_sam_rotation_vector_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_rotation_vector_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_rotation_vector_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }

      //Don't report to client if the output is not valid
      if (((rotation_vector_output_s *)(algoRptPtr->memPtr))->valid_result == false)
      {
         SNS_SAM_PRINTF2(LOW,
                         "Dropping invalid event report to client %d, algo inst id %d",
                         sns_sam_mr_get_client_id(clientReqPtr->mrClientId), algoInstId);
         return SNS_ERR_FAILED;
      }

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_rotation_vector_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id = algoInstId;

      reportRespMsgPtr->result_valid = true;
      rotVecOutPtr = (rotation_vector_output_s *)(algoRptPtr->memPtr);
      // SNS_OS_MEMCOPY(&rotationVectorResult, algoRptPtr->memPtr, sizeof(rotation_vector_output_s));
      rotationVectorResult.accuracy = rotVecOutPtr->accuracy;
      rotationVectorResult.coordinate_sys = rotVecOutPtr->coordinate_sys;
      for(idx = 0; idx < 4; idx++)
      {
         rotationVectorResult.rotation_vector[idx] =
                                            rotVecOutPtr->rotation_vector[idx];
      }

      reportRespMsgPtr->result = rotationVectorResult;  /* structure assignment */

      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_rotation_vector_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;

      //log rotation vector report
      sns_sam_log_rotation_vector_report(clientReqPtr, &rotationVectorResult, algoRptPtr->timestamp);

      return SNS_SUCCESS;
   }
   else if (SNS_SAM_ORIENTATION_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_orientation_result_s_v01 orientationResult;

      sns_sam_orientation_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_orientation_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_orientation_get_report_req_msg_v01 *)clientReqMsgPtr;
      orientation_output_s *outPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }

      outPtr = (orientation_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_orientation_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id = algoInstId;

      reportRespMsgPtr->result_valid = true;

      // rotation vector
      SNS_OS_MEMCOPY(orientationResult.rotation_vector, outPtr->rotation_vector, sizeof(orientationResult.rotation_vector));
      orientationResult.rotation_vector_accuracy = outPtr->accuracy;
      orientationResult.coordinate_system = outPtr->coordinate_sys;

      // gravity
      SNS_OS_MEMCOPY(orientationResult.gravity, outPtr->gravity, sizeof(orientationResult.gravity));
      orientationResult.gravity_accuracy = SNS_SENSOR_ACCURACY_HIGH_V01;

      // linear acceleration
      SNS_OS_MEMCOPY(orientationResult.lin_accel, outPtr->lin_accel, sizeof(orientationResult.lin_accel));

      reportRespMsgPtr->result = orientationResult;  /* structure assignment */

      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_orientation_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;

      //log orientation rotvec report  & orientation gravity report
      sns_sam_log_orientation_rotvec_report(clientReqPtr, &orientationResult, algoRptPtr->timestamp);
      sns_sam_log_orientation_gravity_report(clientReqPtr, &orientationResult, algoRptPtr->timestamp);

      return SNS_SUCCESS;
   }
   else if (algoSvcId == SNS_SAM_SMD_SVC_ID_V01)
   {
      smd_output_s  *rptPtr;

      sns_sam_smd_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_smd_get_report_req_msg_v01  *reportReqPtr =
         (sns_sam_smd_get_report_req_msg_v01 *)clientReqMsgPtr;
      sns_smr_header_s  reportReqHdrPtr;

      algoInstId = reportReqPtr->instance_id;
      sns_smr_get_hdr(&reportReqHdrPtr, reportReqPtr);

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }
      rptPtr = (smd_output_s *)(algoRptPtr->memPtr);

      reportRespMsgPtr =
         sns_smr_msg_alloc( SNS_SAM_DBG_MOD,
                            sizeof(sns_sam_smd_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id = algoInstId;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

      reportRespMsgPtr->report_data.motion_state = rptPtr->motion_state;
      reportRespMsgPtr->report_data.motion_state_probability = rptPtr->motion_state_probability;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_smd_get_report_resp_msg_v01));
      *clientRespMsgPtr = reportRespMsgPtr;
      // log SMD report
      sns_sam_log_smd_report(clientReqPtr,
                             rptPtr->motion_state,
                             rptPtr->motion_state_probability,
                             algoRptPtr->timestamp);
      return SNS_SUCCESS;
   }
   else if (SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_game_rotation_vector_result_s_v01 gameRotationVectorResult;
      game_rot_vec_output_s *GameRVOutPtr;
      uint8_t idx;

      sns_sam_game_rotation_vector_get_report_resp_msg_v01 *reportRespMsgPtr;
      sns_sam_game_rotation_vector_get_report_req_msg_v01 *reportReqPtr =
         (sns_sam_game_rotation_vector_get_report_req_msg_v01 *)clientReqMsgPtr;

      algoInstId = reportReqPtr->instance_id;

      // get report
      algoRptPtr = sns_sam_get_algo_report(algoInstId);
      if(algoRptPtr == NULL)
      {
         return SNS_ERR_FAILED;
      }

      reportRespMsgPtr =
         sns_smr_msg_alloc(SNS_SAM_DBG_MOD,sizeof(sns_sam_game_rotation_vector_get_report_resp_msg_v01));
      SNS_ASSERT(reportRespMsgPtr != NULL);

      reportRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      reportRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      reportRespMsgPtr->instance_id_valid = true;
      reportRespMsgPtr->instance_id = algoInstId;

      reportRespMsgPtr->result_valid = true;
      GameRVOutPtr = ( game_rot_vec_output_s *)(algoRptPtr->memPtr);
      gameRotationVectorResult.accuracy = GameRVOutPtr->accuracy;
      for(idx = 0; idx < 4; idx++)
      {
         gameRotationVectorResult.game_rotation_vector[idx] =
            GameRVOutPtr->rotation_vector[idx];
      }

      reportRespMsgPtr->result = gameRotationVectorResult;  /* structure assignment */

      reportRespMsgPtr->timestamp_valid = true;
      reportRespMsgPtr->timestamp = algoRptPtr->timestamp;

      //send algo report response to client
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            reportRespMsgPtr,
                            sizeof(sns_sam_game_rotation_vector_get_report_resp_msg_v01));

      //log game rotation vector report
      sns_sam_log_game_rotation_vector_report(clientReqPtr,
                                              &gameRotationVectorResult,
                                              algoRptPtr->timestamp);

      *clientRespMsgPtr = reportRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if (SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == algoSvcId)
   {
     return SNS_ERR_BAD_MSG_ID;
   }
   else
   {
      return SNS_ERR_BAD_PARM;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_update_resp
  =========================================================================*/
/*!
  @brief Send response to algorithm update request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created.
  @param[i] algoSvcId: algorithm service id

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_prep_algo_update_resp(
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr,
   uint8_t algoSvcId)
{
   *clientRespMsgPtr = NULL;
   if( algoSvcId == SNS_SAM_PED_SVC_ID_V01)
   {
      sns_sam_ped_reset_req_msg_v01 *resetReqPtr =
         (sns_sam_ped_reset_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_ped_reset_resp_msg_v01 *resetRespMsgPtr;

      resetRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_ped_reset_resp_msg_v01));
      SNS_ASSERT(resetRespMsgPtr != NULL);

      resetRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resetRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      resetRespMsgPtr->instance_id_valid = 1;
      resetRespMsgPtr->instance_id = resetReqPtr->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            resetRespMsgPtr,
                            sizeof(sns_sam_ped_reset_resp_msg_v01));
      *clientRespMsgPtr = resetRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if(SNS_SAM_DISTANCE_BOUND_SVC_ID_V01 == algoSvcId)
   {
      sns_sam_distance_bound_set_bound_req_msg_v01 *setDistanceBoundReqPtr =
         (sns_sam_distance_bound_set_bound_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_distance_bound_set_bound_resp_msg_v01 *setDistanceBoundRespMsgPtr;

      setDistanceBoundRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_distance_bound_set_bound_resp_msg_v01));
      SNS_ASSERT(setDistanceBoundRespMsgPtr != NULL);

      setDistanceBoundRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      setDistanceBoundRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      setDistanceBoundRespMsgPtr->instance_id_valid = 1;
      setDistanceBoundRespMsgPtr->instance_id = setDistanceBoundReqPtr->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            setDistanceBoundRespMsgPtr,
                            sizeof(sns_sam_distance_bound_set_bound_resp_msg_v01));

      *clientRespMsgPtr = setDistanceBoundRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoSvcId == SNS_SAM_CMC_SVC_ID_V01)
   {
      sns_sam_cmc_update_reporting_req_msg_v01 *updateReqPtr =
         (sns_sam_cmc_update_reporting_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_cmc_update_reporting_resp_msg_v01 *updateRespMsgPtr;

      updateRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_cmc_update_reporting_resp_msg_v01));
      SNS_ASSERT(updateRespMsgPtr != NULL);

      updateRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      updateRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      updateRespMsgPtr->instance_id_valid = 1;
      updateRespMsgPtr->instance_id = updateReqPtr->instance_id;
      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            updateRespMsgPtr,
                            sizeof(sns_sam_cmc_update_reporting_resp_msg_v01));
      *clientRespMsgPtr = updateRespMsgPtr;

      return SNS_SUCCESS;
   }
   return SNS_ERR_BAD_PARM;
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_batch_resp
  =========================================================================*/
/*!
  @brief Prepare response to algorithm batch request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created
  @param[i] algoPtr: pointer to algorithm structure

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_prep_algo_batch_resp(
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr,
   sns_sam_algo_s* algoPtr)
{
   if( !algoPtr )
   {
      SNS_SAM_PRINTF0(ERROR, "sns_sam_prep_algo_batch_resp: Invalid algo ptr");
      return SNS_ERR_BAD_PTR;
   }

   if( algoPtr->serviceId == SNS_SAM_PED_SVC_ID_V01 )
   {
      sns_sam_ped_batch_req_msg_v01 *batchReqPtr =
         (sns_sam_ped_batch_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_ped_batch_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_ped_batch_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 1;
      batchRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_ped_batch_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoPtr->serviceId == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 )
   {
      sns_sam_gravity_batch_req_msg_v01 *batchReqPtr =
         (sns_sam_gravity_batch_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_gravity_batch_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_gravity_batch_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 1;
      batchRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_gravity_batch_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoPtr->serviceId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_game_rotation_vector_batch_req_msg_v01 *batchReqPtr =
         (sns_sam_game_rotation_vector_batch_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_game_rotation_vector_batch_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_game_rotation_vector_batch_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 1;
      batchRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_game_rotation_vector_batch_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoPtr->serviceId == SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_rotation_vector_batch_req_msg_v01 *batchReqPtr =
         (sns_sam_rotation_vector_batch_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_rotation_vector_batch_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_rotation_vector_batch_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 1;
      batchRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_rotation_vector_batch_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoPtr->serviceId == SNS_SAM_ORIENTATION_SVC_ID_V01 )
   {
      sns_sam_orientation_batch_req_msg_v01 *batchReqPtr =
         (sns_sam_orientation_batch_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_orientation_batch_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_orientation_batch_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 1;
      batchRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_orientation_batch_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoPtr->serviceId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01 )
   {
     sns_sam_tilt_detector_batch_req_msg_v01 *batchReqPtr =
         (sns_sam_tilt_detector_batch_req_msg_v01 *)clientReqMsgPtr;
     sns_sam_tilt_detector_batch_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_tilt_detector_batch_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 1;
      batchRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_tilt_detector_batch_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoPtr->serviceId == SNS_SAM_CMC_SVC_ID_V01 )
   {
      sns_sam_cmc_batch_req_msg_v01 *batchReqPtr =
         (sns_sam_cmc_batch_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_cmc_batch_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_cmc_batch_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->max_batch_size_valid = 1;
      batchRespMsgPtr->max_batch_size = algoPtr->max_batch_size;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_cmc_batch_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }

   return SNS_ERR_NOTSUPPORTED;
}

/*=========================================================================
  FUNCTION:  sns_sam_prep_algo_upd_batch_period_resp
  =========================================================================*/
/*!
  @brief Send response to algorithm update batch period request

  @param[i] clientReqMsgPtr: pointer to client request message
  @param[o] clientRespMsgPtr: Pointer to the response message created
  @param[i] algoSvcId: algorithm service id

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_prep_algo_upd_batch_period_resp(
   const void* clientReqMsgPtr,
   void** clientRespMsgPtr,
   uint8_t algoSvcId)
{
   if( algoSvcId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_game_rv_update_batch_period_req_msg_v01 *batchReqPtr =
         (sns_sam_game_rv_update_batch_period_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_game_rv_update_batch_period_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_game_rv_update_batch_period_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_game_rv_update_batch_period_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoSvcId == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01 )
   {
      sns_sam_grav_update_batch_period_req_msg_v01 *batchReqPtr =
         (sns_sam_grav_update_batch_period_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_grav_update_batch_period_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_grav_update_batch_period_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_grav_update_batch_period_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoSvcId == SNS_SAM_ORIENTATION_SVC_ID_V01 )
   {
      sns_sam_orient_update_batch_period_req_msg_v01 *batchReqPtr =
         (sns_sam_orient_update_batch_period_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_orient_update_batch_period_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_orient_update_batch_period_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_orient_update_batch_period_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoSvcId == SNS_SAM_PED_SVC_ID_V01 )
   {
      sns_sam_ped_update_batch_period_req_msg_v01 *batchReqPtr =
         (sns_sam_ped_update_batch_period_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_ped_update_batch_period_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_ped_update_batch_period_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_ped_update_batch_period_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoSvcId == SNS_SAM_ROTATION_VECTOR_SVC_ID_V01 )
   {
      sns_sam_rot_vec_update_batch_period_req_msg_v01 *batchReqPtr =
         (sns_sam_rot_vec_update_batch_period_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_rot_vec_update_batch_period_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_rot_vec_update_batch_period_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_rot_vec_update_batch_period_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoSvcId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01 )
   {
      sns_sam_tilt_detector_update_batch_period_req_msg_v01 *batchReqPtr =
         (sns_sam_tilt_detector_update_batch_period_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_tilt_detector_update_batch_period_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_tilt_detector_update_batch_period_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_tilt_detector_update_batch_period_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }
   else if( algoSvcId == SNS_SAM_CMC_SVC_ID_V01 )
   {
      sns_sam_cmc_update_batch_period_req_msg_v01 *batchReqPtr =
         (sns_sam_cmc_update_batch_period_req_msg_v01 *)clientReqMsgPtr;
      sns_sam_cmc_update_batch_period_resp_msg_v01 *batchRespMsgPtr;

      batchRespMsgPtr = sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                          sizeof(sns_sam_cmc_update_batch_period_resp_msg_v01));
      SNS_ASSERT(batchRespMsgPtr != NULL);

      batchRespMsgPtr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      batchRespMsgPtr->resp.sns_err_t = SNS_SUCCESS;
      batchRespMsgPtr->instance_id_valid = 1;
      batchRespMsgPtr->instance_id = batchReqPtr->instance_id;
      batchRespMsgPtr->timestamp_valid = 1;
      batchRespMsgPtr->timestamp = sns_em_get_timestamp();

      sns_sam_prep_resp_msg(clientReqMsgPtr,
                            batchRespMsgPtr,
                            sizeof(sns_sam_cmc_update_batch_period_resp_msg_v01));
      *clientRespMsgPtr = batchRespMsgPtr;

      return SNS_SUCCESS;
   }

   return SNS_ERR_NOTSUPPORTED;
}

/*=========================================================================
  FUNCTION:  sns_sam_process_client_duty_cycle_req
  =========================================================================*/
/*!
  @brief
  enables specified algorithm with the specified configuration

  @param[i] algoSvcId: algorithm service Id
  @param[i] clientReqMsgPtr: pointer to client request message

  @return dutycycleOnPercent: On Percentage requested
*/
/*=======================================================================*/
uint8_t sns_sam_process_client_duty_cycle_req(
   uint8_t algoSvcId,
   const void *clientReqMsgPtr)
{
   uint8_t dutycycleOnPercent = 100;
   if (NULL != clientReqMsgPtr)
   {
      switch (algoSvcId)
      {
         case SNS_SAM_PED_SVC_ID_V01:
         {
            sns_sam_ped_enable_req_msg_v01 *reqPtr =
               (sns_sam_ped_enable_req_msg_v01 *)(clientReqMsgPtr);
            if (reqPtr->duty_cycle_on_percent_valid)
            {
               dutycycleOnPercent = reqPtr->duty_cycle_on_percent;
            }
            break;
         }
         default:
            break;
      }
   }
   return dutycycleOnPercent;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_qmd_config
  =========================================================================*/
/*!
  @brief log QMD algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoCfgPtr: pointer to QMD configuration

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_qmd_config(
   uint8_t algoInstId,
   const qmd_config_s* algoCfgPtr)
{
   sns_err_code_e err;
   sns_log_qmd_config_s* logPtr;
   sns_sam_algo_inst_s *algoInstPtr = sns_sam_get_algo_inst_handle(algoInstId);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QMD_CONFIG,
                           sizeof(sns_log_qmd_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL) && (algoInstPtr != NULL))
   {
      uint8_t algoIndex = sns_sam_get_algo_index(algoInstPtr->serviceId);

      //Generate log packet
      logPtr->version = SNS_LOG_QMD_CONFIG_VERSION;

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->enable_amd = algoCfgPtr->enable_abs;
      logPtr->enable_vmd = algoCfgPtr->enable_veh;
      logPtr->enable_rmd = algoCfgPtr->enable_rel;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();

      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QMD_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_ped_config
  =========================================================================*/
/*!
  @brief log Pedometer algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoCfgPtr: pointer to Pedometer configuration

  @return Sensors error code
*/
/*=========================================================================*/
static sns_err_code_e sns_sam_log_ped_config(
   uint8_t algoInstId,
   const ped_config_s* algoCfgPtr)
{
   sns_err_code_e err;
   sns_log_ped_config_s* logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_PED_CONFIG,
                           sizeof(sns_log_ped_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      uint8_t algoIndex = sns_sam_get_algo_index(SNS_SAM_PED_SVC_ID_V01);

      //Generate log packet
      logPtr->version = SNS_LOG_PED_CONFIG_VERSION;

      logPtr->algo_inst_id = algoInstId;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->step_count_threshold = algoCfgPtr->step_count_threshold;
      logPtr->step_threshold = algoCfgPtr->step_threshold;
      logPtr->swing_threshold = algoCfgPtr->swing_threshold;
      logPtr->step_prob_threshold = algoCfgPtr->step_prob_threshold;

      //Timestamp
      logPtr->timestamp = sns_em_get_timestamp();

      logPtr->sensor_report_rate = sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_PED_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_pam_config
  =========================================================================*/
/*!
  @brief log PAM algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoCfgPtr: pointer to PAM configuration

  @return Sensors error code
*/
/*=========================================================================*/
static sns_err_code_e sns_sam_log_pam_config(
   uint8_t algoInstId,
   const pam_config_s* algoCfgPtr)
{
   sns_err_code_e err;
   sns_log_pam_config_s* logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_PAM_CONFIG,
                           sizeof(sns_log_pam_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_PAM_CONFIG_VERSION;

      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = sns_em_get_timestamp();

      logPtr->measurement_period = algoCfgPtr->measurement_period;
      logPtr->step_count_threshold = algoCfgPtr->step_count_threshold;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->duty_cycle_on_percent = algoCfgPtr->dutycycleOnPercent;

      logPtr->algo_inst_id = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_PAM_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_distance_bound_config
  =========================================================================*/
/*!
  @brief log Distance Bound algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoCfgPtr: pointer to distance_bound configuration

  @return Sensors error code
*/
/*=========================================================================*/
static sns_err_code_e sns_sam_log_distance_bound_config(
   uint8_t algoInstId,
   const distance_bound_config_s* algoCfgPtr)
{
   sns_err_code_e err;
   sns_log_distance_bound_config_s* logPtr;

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_DISTANCE_BOUND_CONFIG,
                           sizeof(sns_log_distance_bound_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_DISTANCE_BOUND_CONFIG_VERSION;

      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = sns_em_get_timestamp();

      logPtr->ms_unknown_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_UNKNOWN_V01];
      logPtr->ms_stationary_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_STATIONARY_V01];
      logPtr->ms_inmotion_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_INMOTION_V01];
      logPtr->ms_fiddle_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_FIDDLE_V01];
      logPtr->ms_pedestrian_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_PEDESTRIAN_V01];
      logPtr->ms_vehicle_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_VEHICLE_V01];
      logPtr->ms_walk_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_WALK_V01];
      logPtr->ms_run_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_RUN_V01];
      logPtr->ms_bike_speedbound = algoCfgPtr->motion_state_speed_bounds[SNS_SAM_DISTANCE_BOUND_MOTION_STATE_BIKE_V01];

      logPtr->algo_inst_id = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_DISTANCE_BOUND_CONFIG, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_qmd_result
  =========================================================================*/
/*!
  @brief log QMD algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_qmd_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_qmd_result_s* logPtr;
   qmd_input_s* inputPtr = (qmd_input_s *)(algoInstPtr->inputData.memPtr);
   qmd_output_s* outputPtr = (qmd_output_s *)(algoInstPtr->outputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_QMD_RESULT,
                           sizeof(sns_log_qmd_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_QMD_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;
      logPtr->accel_x = inputPtr->a[0];
      logPtr->accel_y = inputPtr->a[1];
      logPtr->accel_z = inputPtr->a[2];
      logPtr->amd_result = outputPtr->absolute_rest_state;
      logPtr->vmd_result = outputPtr->vehicle_rest_state;
      logPtr->rmd_result = outputPtr->relative_rest_state;

      //Timestamp
      logPtr->timestamp = algoInstPtr->outputData.timestamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_QMD_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_ped_result
  =========================================================================*/
/*!
  @brief log Pedometer algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] clientId: Client id

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_ped_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t clientId)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_log_ped_result_s* logPtr;
   ped_input_s* inputPtr = (ped_input_s *)(algoInstPtr->inputData.memPtr);
   ped_state_s* statePtr = (ped_state_s *)(algoInstPtr->stateData.memPtr);
   ped_output_s* outputPtr = (ped_output_s *)(algoInstPtr->outputData.memPtr);
   int32_t i;
   bool log_all = true;

   if( clientId != SNS_SAM_INVALID_ID )
   {
      log_all = false;
   }

   for( i = 0; i < outputPtr->num_active_clients; ++i)
   {
      if( log_all || (!log_all && outputPtr->client_data[i].client_id == clientId ) )
      {
         //Allocate log packet
         err = sns_logpkt_malloc(SNS_LOG_PED_RESULT,
               sizeof(sns_log_ped_result_s),
               (void**)&logPtr);

         if ((err == SNS_SUCCESS) && (logPtr != NULL))
         {
            //Generate the log packet
            logPtr->version = SNS_LOG_PED_RESULT_VERSION;

            logPtr->algo_inst_id = algoInstId;
            if( log_all )
            {
               // logging result - store input
               logPtr->accel[0] = inputPtr->accel[0];
               logPtr->accel[1] = inputPtr->accel[1];
               logPtr->accel[2] = inputPtr->accel[2];
               logPtr->timestamp = inputPtr->timestamp;
            }
            else
            {
               // logging a reset - clear accel input
               logPtr->accel[0] = 0;
               logPtr->accel[1] = 0;
               logPtr->accel[2] = 0;
               logPtr->timestamp = outputPtr->client_data[i].timestamp;
            }
            logPtr->sample_rate = statePtr->sample_rate;
            logPtr->client_id   = outputPtr->client_data[i].client_id;
            logPtr->reset_stats = !log_all;
            logPtr->step_event  = outputPtr->client_data[i].step_event;
            logPtr->step_confidence = statePtr->step_confidence;
            logPtr->step_count = outputPtr->client_data[i].step_count +
                                 statePtr->client_data[i].step_count;
            logPtr->step_count_error = statePtr->client_data[i].step_count_error;
            logPtr->swing_step_count = statePtr->client_data[i].swing_step_count;
            logPtr->step_rate = statePtr->step_rate;

            //Commit log (also frees up the log packet memory)
            err = sns_logpkt_commit(SNS_LOG_PED_RESULT, logPtr);

            // Break if only logging result for one client
            if( !log_all )
            {
                break;
            }
         }
      }
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_pam_result
  =========================================================================*/
/*!
  @brief log PAM algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_pam_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_pam_result_s* logPtr;
   pam_input_s* inputPtr = (pam_input_s *)(algoInstPtr->inputData.memPtr);
   pam_state_s* statePtr = (pam_state_s *)(algoInstPtr->stateData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_PAM_RESULT,
                           sizeof(sns_log_pam_result_s),
                           (void**)&logPtr);


   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {

      //Generate the log packet
      logPtr->version = SNS_LOG_PAM_RESULT_VERSION;

      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = algoInstPtr->outputData.timestamp;
      logPtr->data_type = inputPtr->datatype;

      if(logPtr->data_type == PAM_QMD_INPUT)
      {
         logPtr->motion_state = inputPtr->data.motion_state;
         logPtr->pedometer_step_count = PAM_DEVICE_STATE_UNKNOWN;
      }
      else
      {
         logPtr->pedometer_step_count = inputPtr->data.step_count ;
         if(statePtr->dev_state == PAM_DEVICE_STATE_STATIONARY)
         {
            logPtr->motion_state = SNS_SAM_MOTION_REST_V01;
         }
         else if (statePtr->dev_state == PAM_DEVICE_STATE_MOTION)
         {
            logPtr->motion_state = SNS_SAM_MOTION_MOVE_V01;
         }
         else
         {
            logPtr->motion_state = SNS_SAM_MOTION_UNKNOWN_V01;
         }
      }
      logPtr->pedometer_last_step_count = statePtr->pedometer_last_step_count;
      logPtr->pam_current_step_count = statePtr->current_step_count;
      logPtr->pam_last_reported_step_count = statePtr->reported_step_count;
      logPtr->pam_report_decision = statePtr->report_decision;

      logPtr->algo_inst_id = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_PAM_RESULT, logPtr);
   }

   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_cmc_result2
  =========================================================================*/
/*!
  @brief log CMC algorithm result2

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_cmc_result2(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_cmc_result2_s* logPtr;
   cmc_state_s *statePtr = (cmc_state_s *)(algoInstPtr->stateData.memPtr);
   cmc_input_s* inputPtr = (cmc_input_s *)(algoInstPtr->inputData.memPtr);
   cmc_output_s* outputPtr = (cmc_output_s *)(algoInstPtr->outputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_CMC_RESULT2,
                           sizeof(sns_log_cmc_result2_s),
                           (void**)&logPtr);


   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      uint8_t i=0;

      //Generate the log packet
      logPtr->version = SNS_LOG_CMC_RESULT_VERSION;

      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = inputPtr->timestamp;

      SNS_OS_MEMCOPY(&(logPtr->motion_feature[0]),
                     &(statePtr->motion_feature.output.iqr),
                     sizeof(float)*34);

      SNS_OS_MEMCOPY(&(logPtr->unfilt_gmm_prob[0]),
                     &(statePtr->unfilt_gmm_prob[0]),
                     (sizeof(float)*CMC_MS_LEAF_NUM));

      SNS_OS_MEMCOPY(&(logPtr->unfilt_lr_prob_stat_dr[0]),
                     &(statePtr->unfilt_lr_prob_stat_dr[0]),
                     (sizeof(float)*CMC_NUM_LR_STATES));

      SNS_OS_MEMCOPY(&(logPtr->unfilt_lr_prob_walk_bike[0]),
                     &(statePtr->unfilt_lr_prob_walk_bike[0]),
                     (sizeof(float)*CMC_NUM_LR_STATES));

      SNS_OS_MEMCOPY(&(logPtr->unfilt_prob[0]),
                     &(statePtr->unfilt_prob[0]),
                     (sizeof(float)*CMC_MS_LEAF_NUM));

      SNS_OS_MEMCOPY(&(logPtr->filt_prob[0]),
                     &(statePtr->filt_prob[0]),
                     (sizeof(float)*CMC_MS_LEAF_NUM));

      logPtr->hyst_stable_state = statePtr->hysteresis_state.stable_state;
      logPtr->hyst_transition_state = statePtr->hysteresis_state.transition_state;
      logPtr->hyst_is_transition = statePtr->hysteresis_state.is_transition;
      logPtr->hyst_transition_count = statePtr->hysteresis_state.transition_count;

      SNS_OS_MEMCOPY(&(logPtr->event_report_types[0]),
               &(statePtr->event_report_types[0]),
               (sizeof(uint8_t)*CMC_MS_LEAF_NUM_WITH_UNKNOWN));

      for (i = 0; i < CMC_MS_LEAF_NUM_WITH_UNKNOWN; i++)
      {
         logPtr->motion_event[i] = statePtr->motion_states[i].motion_event;
         logPtr->ms_timestamp[i] = statePtr->motion_states[i].timestamp;
         logPtr->ms_probability[i] = statePtr->motion_states[i].probability;
         logPtr->op_motion_event[i] = outputPtr->motion_outputs[i].motion_event;
         logPtr->op_ms_timestamp[i] = outputPtr->motion_outputs[i].timestamp;
         logPtr->op_ms_probability[i] = outputPtr->motion_outputs[i].probability;
      }

      logPtr->algo_inst_id = algoInstId;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_CMC_RESULT2, logPtr);
   }

   return err;
}


/*===========================================================================
  FUNCTION:   sns_sam_log_cmc_result
  =========================================================================*/
/*!
  @brief log CMC algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_cmc_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;
   sns_log_cmc_result_s* logPtr;
   cmc_state_s *statePtr = (cmc_state_s *)(algoInstPtr->stateData.memPtr);
   cmc_input_s* inputPtr = (cmc_input_s *)(algoInstPtr->inputData.memPtr);
   cmc_output_s* outputPtr = (cmc_output_s *)(algoInstPtr->outputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_CMC_RESULT,
                           sizeof(sns_log_cmc_result_s),
                           (void**)&logPtr);


   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {

      //Generate the log packet
      logPtr->version = SNS_LOG_CMC_RESULT_VERSION;

      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = inputPtr->timestamp;;
      logPtr->input_type = inputPtr->datatype;

      if (inputPtr->datatype == CMC_ACCEL_INPUT)
      {
         logPtr->accel[0] = inputPtr->data.accel[0];
         logPtr->accel[1] = inputPtr->data.accel[1];
         logPtr->accel[2] = inputPtr->data.accel[2];
      }
      else if (inputPtr->datatype == CMC_QMD_INPUT)
      {
         logPtr->qmd_motion_state = inputPtr->data.qmd_motion_state;
      }
      else if (inputPtr->datatype == CMC_UPDATE_REPORTING_INPUT)
      {
         logPtr->update_report_motion_state = inputPtr->data.update_report_req.ms_event;
         logPtr->update_report_type =  inputPtr->data.update_report_req.report_type;
      }

      //logPtr->update_count = statePtr->update_count;

      logPtr->algo_inst_id = algoInstId;


      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_CMC_RESULT, logPtr);
   }

   if (outputPtr->output_ready)
   {
      err = sns_sam_log_cmc_result2(algoInstId,algoInstPtr);
   }
   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_distance_bound_result
  =========================================================================*/
/*!
  @brief log Distance bound algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_distance_bound_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = SNS_SUCCESS;
   sns_log_distance_bound_result_s *logPtr;
   distance_bound_input_s *inputPtr = (distance_bound_input_s *)(algoInstPtr->inputData.memPtr);
   distance_bound_state_s *statePtr = (distance_bound_state_s *)(algoInstPtr->stateData.memPtr);
   distance_bound_output_s *outPtr = (distance_bound_output_s *)(algoInstPtr->outputData.memPtr);

   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_DISTANCE_BOUND_RESULT,
                           sizeof(sns_log_distance_bound_result_s),
                           (void **)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      logPtr->version = SNS_LOG_DISTANCE_BOUND_RESULT_VERSION;

      logPtr->algo_inst_id = algoInstId;

      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = inputPtr->timestamp;
      logPtr->algo_state = statePtr->client_data.algo_state;
      logPtr->input_type = inputPtr->datatype;
      logPtr->input_cmc_motion_state = inputPtr->data.cmc_motion_event.motion_state;
      logPtr->input_cmc_motion_event = inputPtr->data.cmc_motion_event.motion_event;
      logPtr->input_distance_bound = inputPtr->data.distance_bound;

      logPtr->prev_cmc_motion_state = statePtr->client_data.prev_cmc_motion_state;
      logPtr->prev_distance_bound = statePtr->client_data.remaining_distance;
      logPtr->prev_speed = statePtr->client_data.prev_speed;
      logPtr->prev_timestamp = statePtr->client_data.prev_timestamp;
      logPtr->time_to_breach = statePtr->client_data.time_to_breach;
      logPtr->session_key = outPtr->client_data.session_key;
      logPtr->is_simultaneous_state = statePtr->client_data.is_simultaneous_state;
      logPtr->prev_simultaneous_cmc_state = statePtr->client_data.prev_simultaneous_cmc_state;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_DISTANCE_BOUND_RESULT, logPtr);
   }
   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_smd_config
  =========================================================================*/
/*!
  @brief log SMD algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoCfgPtr: pointer to QMD configuration

  @return Sensors error code
*/
/*=========================================================================*/
static sns_err_code_e sns_sam_log_smd_config(
   uint8_t algoInstId,
   const smd_config_s* algoCfgPtr)
{
   sns_err_code_e err = 0;
   sns_log_smd_config_s* logPtr;
   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_SMD_CONFIG,
                           sizeof(sns_log_smd_config_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate log packet
      logPtr->version = SNS_LOG_SMD_CONFIG_VERSION;
      logPtr->sample_rate = algoCfgPtr->sample_rate;
      logPtr->accel_window_time = algoCfgPtr->accel_window_time;
      logPtr->detect_threshold = algoCfgPtr->detect_threshold;
      //note that prob_nm is not configurable so logging prob_sm makes sense
      //test team should use the log field accordingly
      logPtr->self_transition_prob_nm = algoCfgPtr->self_transition_prob_sm;
      logPtr->variable_latency_flag = algoCfgPtr->variable_decision_latency;
      logPtr->max_latency = algoCfgPtr->max_latency;
      logPtr->algo_inst_id = algoInstId;

      //Timestamp
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = sns_em_get_timestamp();

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_SMD_CONFIG, logPtr);
   }
   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_smd_result
  =========================================================================*/
/*!
  @brief log QMD algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_smd_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err = 0;
   sns_log_smd_result_s* logPtr;
   smd_state_s * statePtr = (smd_state_s *)(algoInstPtr->stateData.memPtr);
   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_SMD_RESULT,
                           sizeof(sns_log_smd_result_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      //Generate the log packet
      int32_t i;
      logPtr->version = SNS_LOG_SMD_RESULT_VERSION;
      smd_input_s *inputPtr = (smd_input_s *)(algoInstPtr->inputData.memPtr);
      if (inputPtr->datatype == SMD_ACCEL_INPUT)
      {
        for (i=0; i<3; i++)
         {
            logPtr->a[i] = inputPtr->data.accel[i];
            logPtr->eigenvalues[i] = statePtr->eigenvalues[i];
         }
        logPtr->sample_cnt = statePtr->sample_cnt;
        logPtr->variance = statePtr->variance;
        logPtr->xcor = statePtr->xcor;
        logPtr->spread_mag = statePtr->spread_mag;
        logPtr->rm = statePtr->accel_norm_mean_ratio;
        logPtr->sa = statePtr->accel_norm_var;
        logPtr->pstate[0] = statePtr->pstate[0];
        logPtr->pstate[1] = statePtr->pstate[1];
        logPtr->filter_delay_cnt = statePtr->filter_delay_cnt;
        logPtr->filter_delay_time = statePtr->filter_delay_time;
        logPtr->algo_inst_id = 0;
        logPtr->eigenvalues[2] = statePtr->step_infer;  // TODO: Remove temporary change and add
                                                        // log support for step_inference.
      }
      else if(inputPtr->datatype == SMD_AMD_INPUT)
      {
        logPtr->a[0] = inputPtr->data.amd_state;
        logPtr->algo_inst_id = 1;
      }
      else if(inputPtr->datatype == SMD_PED_INPUT)
      {
        logPtr->a[0] = inputPtr->data.step_conf;
        logPtr->a[1] = statePtr->step_infer;
        logPtr->algo_inst_id = 2;
      }
      logPtr->motion_state = statePtr->motion_state;
      logPtr->final_motion_state = statePtr->final_motion_state;

      //Timestamp
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = inputPtr->timestamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_SMD_RESULT, logPtr);
   }
   return err;
}

/*===========================================================================
  FUNCTION:   sns_sam_log_smd_report
  =========================================================================*/
/*!
  @brief log SMD algorithm report

  @param[i] clientReqPtr: pointer to client request
  @param[i] motionState: motion state
  @param[i] motionProbablity: motion state probablity
  @param[i] timeStamp: time stamp for motion state
  @param[i] algoSvcId: algorithm service id

  @return Sensors error code
*/
/*=========================================================================*/
sns_err_code_e sns_sam_log_smd_report(
   const sns_sam_client_req_s* clientReqPtr,
   int8_t motionState,
   uint8_t motionStateProbability,
   uint32_t timeStamp)
{
   sns_err_code_e err = 0;
   sns_log_smd_report_s* logPtr;
   uint8_t algoInstId = clientReqPtr->algoInstId;
   //Allocate log packet
   err = sns_logpkt_malloc(SNS_LOG_SMD_REPORT,
                           sizeof(sns_log_smd_report_s),
                           (void**)&logPtr);

   if ((err == SNS_SUCCESS) && (logPtr != NULL))
   {
      logPtr->version = SNS_LOG_SMD_REPORT_VERSION;

      //Generate the log packet
      logPtr->report_id = clientReqPtr->reportId;
      logPtr->report_type = clientReqPtr->reportType;
      logPtr->client_id = clientReqPtr->mrClientId;
      logPtr->motion_state = motionState;
      logPtr->motion_state_probability = motionStateProbability;
      logPtr->algo_inst_id = algoInstId;

      //Timestamp with report time
      logPtr->timestamp_type = SNS_TIMESTAMP_DSPS;
      logPtr->timestamp = timeStamp;

      //Commit log (also frees up the log packet memory)
      err = sns_logpkt_commit(SNS_LOG_SMD_REPORT, logPtr);
   }
   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_log_algo_config
  =========================================================================*/
/*!
  @brief Log algorithm configuration

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] algoIndex: index to algorithm in the algorithm database

  @return None
*/
/*=======================================================================*/
void sns_sam_log_algo_config(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t algoIndex)
{
   SNS_SAM_PRINTF2(LOW,
                   "sns_sam_log_algo_config: algo instance %d, algo service %d",
                   algoInstId, algoInstPtr->serviceId);
   switch (algoInstPtr->serviceId)
   {
#ifdef FEATURE_SNS_SAM_FNS
      case SNS_SAM_FNS_SVC_ID_V01:
         sns_sam_log_fns_config(algoInstId, algoInstPtr);
         break;
#endif

#ifdef FEATURE_SNS_SAM_BTE
      case SNS_SAM_BTE_SVC_ID_V01:
         sns_sam_log_bte_config(algoInstId, algoInstPtr);
         break;
#endif

#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
      case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
         sns_sam_log_basic_gestures_config(algoInstId, algoInstPtr);
         break;
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_TAP
      case SNS_SAM_TAP_SVC_ID_V01:
         sns_sam_log_tap_config(algoInstId, algoInstPtr);
         break;
#endif /* FEATURE_SNS_SAM_TAP */

#ifdef FEATURE_SNS_SAM_TILT
      case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
        sns_sam_log_integ_angle_config(algoInstId, algoInstPtr);
         break;
#endif /* FEATURE_SNS_SAM_TILT */

#ifdef FEATURE_SNS_SAM_GYRO_TAP
      case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
         sns_sam_log_gtap_config(algoInstId, algoInstPtr);
         break;
#endif /* FEATURE_SNS_SAM_GYRO_TAP */

#ifdef FEATURE_SNS_SAM_FACING
      case SNS_SAM_FACING_SVC_ID_V01:
         sns_sam_log_facing_config(algoInstId, algoInstPtr, algoIndex);
         break;
#endif /* FEATURE_SNS_SAM_FACING */
      case SNS_SAM_QUATERNION_SVC_ID_V01:
         sns_sam_log_gyro_quat_config(algoInstId, algoInstPtr, algoIndex);
         break;

      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
         sns_sam_log_gravity_config(algoInstId, algoInstPtr, algoIndex);
         break;

      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
         sns_sam_log_fmv_config(algoInstId, algoInstPtr, algoIndex);
         break;

      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
         sns_sam_log_rotation_vector_config(algoInstId, algoInstPtr, algoIndex);
         break;

      case SNS_SAM_ORIENTATION_SVC_ID_V01:
         sns_sam_log_orientation_rotvec_config(algoInstId, algoInstPtr, algoIndex);
         sns_sam_log_orientation_gravity_config(algoInstId, algoInstPtr, algoIndex);
         break;

      case SNS_SAM_SMD_SVC_ID_V01:
        sns_sam_log_smd_config(algoInstId,
                              (smd_config_s *)(algoInstPtr->configData.memPtr));
        break;

      case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
        sns_sam_log_tilt_detector_config(algoInstId,
                                        (tilt_config_s *)(algoInstPtr->configData.memPtr));
        break;

      default:
         break;
   }

   if (SNS_SAM_SVC_IS_QMD_ALGO(algoInstPtr->serviceId))
   {
      sns_sam_log_qmd_config(algoInstId,
                             (qmd_config_s *)(algoInstPtr->configData.memPtr));
   }
   else if (algoInstPtr->serviceId == SNS_SAM_PED_SVC_ID_V01)
   {
      sns_sam_log_ped_config(algoInstId,
                             (ped_config_s *)(algoInstPtr->configData.memPtr));
   }
   else if (algoInstPtr->serviceId == SNS_SAM_PAM_SVC_ID_V01)
   {
      sns_sam_log_pam_config(algoInstId,
                            (pam_config_s *)(algoInstPtr->configData.memPtr));
   }
   else if (algoInstPtr->serviceId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      sns_sam_log_distance_bound_config(algoInstId,
                            (distance_bound_config_s *)(algoInstPtr->configData.memPtr));
   }
   else if (algoInstPtr->serviceId == SNS_SAM_CMC_SVC_ID_V01)
   {
      /* CMC does not support client configurable parameters*/
   }
   else if (algoInstPtr->serviceId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01)
   {
      sns_sam_log_game_rotation_vector_config(algoInstId, algoInstPtr, algoIndex);
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_log_algo_result
  =========================================================================*/
/*!
  @brief Log algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance
  @param[i] clientId: Client id

  @return None
*/
/*=======================================================================*/
void sns_sam_log_algo_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr,
   uint8_t clientId)
{
   sns_err_code_e err = SNS_SUCCESS;

   if (SNS_SAM_SVC_IS_QMD_ALGO(algoInstPtr->serviceId))
   {
      int32_t motionState;
      qmd_output_s *outputPtr = (qmd_output_s *)algoInstPtr->outputData.memPtr;

      switch(algoInstPtr->serviceId)
      {
      case SNS_SAM_AMD_SVC_ID_V01:
         motionState = outputPtr->absolute_rest_state;
         sns_sam_motion_state_update(algoInstPtr->serviceId,
                                     (sns_sam_motion_state_e_v01)motionState);
         break;
      case SNS_SAM_VMD_SVC_ID_V01:
         motionState = outputPtr->vehicle_rest_state;
         break;
      case SNS_SAM_RMD_SVC_ID_V01:
         motionState = outputPtr->relative_rest_state;
         sns_sam_motion_state_update(algoInstPtr->serviceId,
                                     (sns_sam_motion_state_e_v01)motionState);
         break;
      default:
         motionState = SNS_SAM_MOTION_UNKNOWN_V01;
         break;
      }

      //update GPIO state
      sns_sam_set_qmd_result_gpio(motionState);

      //log QMD result
      sns_sam_log_qmd_result(algoInstId, algoInstPtr);
   }
   else if (algoInstPtr->serviceId == SNS_SAM_PED_SVC_ID_V01)
   {
      sns_sam_log_ped_result( algoInstId, algoInstPtr, clientId );
   }
   else if (algoInstPtr->serviceId == SNS_SAM_CMC_SVC_ID_V01)
   {
      sns_sam_log_cmc_result( algoInstId, algoInstPtr);
   }
   else if (algoInstPtr->serviceId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      sns_sam_log_distance_bound_result( algoInstId, algoInstPtr);
   }
   else if (algoInstPtr->serviceId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01)
   {
      sns_sam_log_game_rotation_vector_result( algoInstId, algoInstPtr );
   }
   else
   {
       switch (algoInstPtr->serviceId)
       {
#ifdef FEATURE_SNS_SAM_FNS
       case SNS_SAM_FNS_SVC_ID_V01:
          err = sns_sam_log_fns_result( algoInstId, algoInstPtr);
          break;
#endif /* FEATURE_SNS_SAM_FNS */

#ifdef FEATURE_SNS_SAM_BTE
       case SNS_SAM_BTE_SVC_ID_V01:
          err = sns_sam_log_bte_result( algoInstId, algoInstPtr);
          break;
#endif /* FEATURE_SNS_SAM_BTE */

#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
       case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
          err = sns_sam_log_basic_gestures_result( algoInstId, algoInstPtr);
          break;
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */

#ifdef FEATURE_SNS_SAM_TAP
       case SNS_SAM_TAP_SVC_ID_V01:
          err = sns_sam_log_tap_result( algoInstId, algoInstPtr);
          break;
#endif /* FEATURE_SNS_SAM_TAP */

#ifdef FEATURE_SNS_SAM_GYRO_TAP
       case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
          err = sns_sam_log_gtap_result( algoInstId, algoInstPtr);
          break;
#endif /* FEATURE_SNS_SAM_GYRO_TAP */

#ifdef FEATURE_SNS_SAM_TILT
       case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
          err = sns_sam_log_integ_angle_result( algoInstId, algoInstPtr);
          break;
#endif /* FEATURE_SNS_SAM_TILT */

#ifdef FEATURE_SNS_SAM_FACING
       case SNS_SAM_FACING_SVC_ID_V01:
          err = sns_sam_log_facing_result( algoInstId, algoInstPtr);
          break;
#endif /* FEATURE_SNS_SAM_FACING */

       case SNS_SAM_QUATERNION_SVC_ID_V01:
          err = sns_sam_log_gyro_quat_result(algoInstId, algoInstPtr);
          break;

       case SNS_SAM_SMD_SVC_ID_V01:
         err = sns_sam_log_smd_result(algoInstId, algoInstPtr);
         break;

       case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
         err = sns_sam_log_tilt_detector_result(algoInstId, algoInstPtr);
         break;

       default:
         err = SNS_ERR_FAILED;
         break;
       }
   }

   SNS_SAM_PRINTF3(LOW,
      "sns_sam_log_algo_result: algo instance %d, algo service %d, err %d",
      algoInstId, algoInstPtr->serviceId, err);
}

/*=========================================================================
  FUNCTION:  sns_sam_log_dep_algo_result
  =========================================================================*/
/*!
  @brief Log dependent algorithm result

  @param[i] algoInstId: algorithm instance id
  @param[i] algoInstPtr: pointer to algorithm instance

  @return None
*/
/*=======================================================================*/
void sns_sam_log_dep_algo_result(
   uint8_t algoInstId,
   const sns_sam_algo_inst_s* algoInstPtr)
{
   sns_err_code_e err;

   SNS_SAM_PRINTF2(LOW,
     "sns_sam_log_dep_algo_result: algo instance %d, algo service %d",
     algoInstId, algoInstPtr->serviceId);

   switch (algoInstPtr->serviceId)
   {
   case SNS_SAM_PAM_SVC_ID_V01:
      err = sns_sam_log_pam_result(algoInstId, algoInstPtr);

   case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
      err = sns_sam_log_distance_bound_result(algoInstId, algoInstPtr);
      break;

   case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      err = sns_sam_log_gravity_result(algoInstId, algoInstPtr);
      break;

   case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      err = sns_sam_log_fmv_gyro_update(algoInstId, algoInstPtr);
      break;

   case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      err = sns_sam_log_rotation_vector_result(algoInstId, algoInstPtr);
      break;

   case SNS_SAM_ORIENTATION_SVC_ID_V01:
      err = sns_sam_log_orientation_rotvec_result(algoInstId, algoInstPtr);
      err = sns_sam_log_orientation_gravity_result(algoInstId, algoInstPtr);
      break;

   case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      err = sns_sam_log_game_rotation_vector_result( algoInstId, algoInstPtr );
      break;

   case SNS_SAM_CMC_SVC_ID_V01:
      err = sns_sam_log_cmc_result(algoInstId, algoInstPtr);
      break;

   default:
      {
         err = SNS_ERR_FAILED;
         break;
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_update_algo_config
  =========================================================================*/
/*!
  @brief Update algorithm configuration per client specification

  @param[i] algoSvcId: algorithm service Id
  @param[i] clientReqMsgPtr: pointer to client request message
  @param[io] algoCfgPtr: pointer to algorithm configuration
  @return None
*/
/*=======================================================================*/
void sns_sam_update_algo_config(
   uint8_t algoSvcId,
   const void* clientReqMsgPtr,
   void *algoCfgPtr)
{
   int32_t* sampleRatePtr = NULL;
   uint32_t reportPeriod = 0;

   if (NULL == clientReqMsgPtr || NULL == algoCfgPtr)
   {
      return;
   }

   switch (algoSvcId)
   {
#ifdef FEATURE_SNS_SAM_FNS
      case SNS_SAM_FNS_SVC_ID_V01:
      {
         face_n_shake_config_s *fns_cfg_ptr = (face_n_shake_config_s*)algoCfgPtr;
         sns_sam_fns_enable_req_msg_v01 *fns_enable_ptr =
            (sns_sam_fns_enable_req_msg_v01 *)clientReqMsgPtr;

         if(fns_enable_ptr->basic_sleep_valid)
         {
            fns_cfg_ptr->basic_sleep = fns_enable_ptr->basic_sleep;
         }

         if (fns_enable_ptr->basic_shake_threshold_valid)
         {
            fns_cfg_ptr->basic_shake_threshold =
               fns_enable_ptr->basic_shake_threshold;
         }

         if (fns_enable_ptr->facing_angle_threshold_valid)
         {
            fns_cfg_ptr->facing_angle_threshold =
               fns_enable_ptr->facing_angle_threshold;
         }

         if (fns_enable_ptr->timeout_valid)
         {
            fns_cfg_ptr->timeout = fns_enable_ptr->timeout;
         }

         if (fns_enable_ptr->sample_rate_valid)
         {
            fns_cfg_ptr->sample_rate = fns_enable_ptr->sample_rate;
         }

         sampleRatePtr = &(fns_cfg_ptr->sample_rate);
         break;
      }
#endif /* FEATURE_SNS_SAM_FNS */
#ifdef FEATURE_SNS_SAM_BTE
      case SNS_SAM_BTE_SVC_ID_V01:
      {
         bring_to_ear_config_s *bte_cfg_ptr = (bring_to_ear_config_s *)algoCfgPtr;
         sns_sam_bte_enable_req_msg_v01 *bte_enable_ptr =
           (sns_sam_bte_enable_req_msg_v01 *)clientReqMsgPtr;

         if (bte_enable_ptr->sample_rate_valid)
         {
            bte_cfg_ptr->sample_rate = bte_enable_ptr->sample_rate;
         }
         if (bte_enable_ptr->facing_angle_threshold_valid)
         {
            bte_cfg_ptr->facing_angle_threshold =
               bte_enable_ptr->facing_angle_threshold;
         }
         if (bte_enable_ptr->horiz_angle_threshold_valid)
         {
            bte_cfg_ptr->horiz_angle_threshold =
               bte_enable_ptr->horiz_angle_threshold;
         }
         if (bte_enable_ptr->vert_angle_threshold_valid)
         {
            bte_cfg_ptr->vert_angle_threshold =
               bte_enable_ptr->vert_angle_threshold;
         }
         if (bte_enable_ptr->prox_enabled_valid)
         {
            bte_cfg_ptr->prox_enabled = bte_enable_ptr->prox_enabled;
         }

         sampleRatePtr = &(bte_cfg_ptr->sample_rate);
         break;
      }
#endif /* FEATURE_SNS_SAM_BTE */
#ifdef FEATURE_SNS_SAM_BASIC_GESTURES
      case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
      {
         basic_gestures_config_struct *basic_gestures_cfg_ptr =
            (basic_gestures_config_struct *)algoCfgPtr;
         sns_sam_basic_gestures_enable_req_msg_v01 *basic_gestures_enable_ptr =
            (sns_sam_basic_gestures_enable_req_msg_v01 *)clientReqMsgPtr;

         if (basic_gestures_enable_ptr->sample_rate_valid)
         {
            basic_gestures_cfg_ptr->sample_rate =
               basic_gestures_enable_ptr->sample_rate;
         }
         if (basic_gestures_enable_ptr->sleep_valid)
         {
            basic_gestures_cfg_ptr->sleep =
               basic_gestures_enable_ptr->sleep;
         }
         if (basic_gestures_enable_ptr->push_threshold_valid)
         {
            basic_gestures_cfg_ptr->push_threshold =
               basic_gestures_enable_ptr->push_threshold;
         }
         if (basic_gestures_enable_ptr->pull_threshold_valid)
         {
            basic_gestures_cfg_ptr->pull_threshold =
               basic_gestures_enable_ptr->pull_threshold;
         }
         if (basic_gestures_enable_ptr->shake_threshold_valid)
         {
            basic_gestures_cfg_ptr->shake_threshold =
               basic_gestures_enable_ptr->shake_threshold;
         }

         sampleRatePtr = &(basic_gestures_cfg_ptr->sample_rate);
         break;
      }
#endif /* FEATURE_SNS_SAM_BASIC_GESTURES */
#ifdef FEATURE_SNS_SAM_TAP
      case SNS_SAM_TAP_SVC_ID_V01:
      {
         tap_config_struct *tap_cfg_ptr = (tap_config_struct *)algoCfgPtr;
         sns_sam_tap_enable_req_msg_v01 *tap_enable_ptr =
            (sns_sam_tap_enable_req_msg_v01 *)clientReqMsgPtr;

         if (tap_enable_ptr->sample_rate_valid)
         {
            tap_cfg_ptr->sample_rate = tap_enable_ptr->sample_rate;
         }
         if (tap_enable_ptr->tap_threshold_valid)
         {
            tap_enable_ptr->tap_threshold =
               tap_enable_ptr->tap_threshold;
         }

         sampleRatePtr = &(tap_cfg_ptr->sample_rate);
         break;
      }
#endif /* FEATURE_SNS_SAM_TAP */
#ifdef FEATURE_SNS_SAM_TILT
      case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
      {
         integ_angle_config_struct *cfg_ptr = (integ_angle_config_struct *)algoCfgPtr;
         sns_sam_integ_angle_enable_req_msg_v01 *enable_ptr =
           (sns_sam_integ_angle_enable_req_msg_v01 *)clientReqMsgPtr;

         if (enable_ptr->sample_rate_valid)
         {
            /* start with what is specified */
            cfg_ptr->sample_rate = enable_ptr->sample_rate;

            /* clip the sample rate */
            if (cfg_ptr->sample_rate < IA_MIN_GYRO_RATE_Q16)
            {
               cfg_ptr->sample_rate = IA_MIN_GYRO_RATE_Q16;
            }
            else
            {
               if (cfg_ptr->sample_rate > IA_MAX_GYRO_RATE_Q16)
               {
                 cfg_ptr->sample_rate = IA_MAX_GYRO_RATE_Q16;
               }
               else
               {
                 /* avoid a dangling else */
               }
            }
         }
         else
         {
            cfg_ptr->sample_rate = IA_DEF_GYRO_RATE_Q16;
         }

         /* Not that the FDD deines the clip theshrhold as configurable - but
           not exposed to the use - so - we do not apply the setting from the
           QMI message.                                                       */
         /*
         if(enable_ptr->integ_angle_angle_threshold_valid)
         {
           cfg_ptr->clip_threshold = enable_ptr->integ_angle_angle_threshold;
         }
         */

         cfg_ptr->clip_threshold = FX_IA_RES_LIMIT;
         cfg_ptr->init_ts        = sns_em_get_timestamp();

         sampleRatePtr = &(cfg_ptr->sample_rate);
         break;
      }
#endif /* FEATURE_SNS_SAM_TILT */
#ifdef FEATURE_SNS_SAM_FACING
      case SNS_SAM_FACING_SVC_ID_V01:
      {
         facing_config_struct *facing_cfg_ptr = (facing_config_struct *)algoCfgPtr;
         sns_sam_facing_enable_req_msg_v01 *facing_enable_ptr =
                (sns_sam_facing_enable_req_msg_v01 *)clientReqMsgPtr;

         if (facing_enable_ptr->sample_rate_valid)
         {
            facing_cfg_ptr->sample_rate = facing_enable_ptr->sample_rate;
         }
         if (facing_enable_ptr->facing_angle_threshold_valid)
         {
            facing_cfg_ptr->facing_angle_threshold = facing_enable_ptr->facing_angle_threshold;
         }
         if (facing_enable_ptr->report_neutral_valid)
         {
            facing_cfg_ptr->report_neutral = facing_enable_ptr->report_neutral;
         }
         sampleRatePtr = &(facing_cfg_ptr->sample_rate);
         break;
      }
#endif /* FEATURE_SNS_SAM_FACING */
#ifdef FEATURE_SNS_SAM_GYRO_TAP
      case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
      {
         uint8_t scn;
         gtap_config_struct *defCfgPtr;
         sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(SNS_SAM_GYRO_TAP2_SVC_ID_V01);
         if ( algoPtr == NULL )
         {
            SNS_SAM_PRINTF0(ERROR, "Failed to get algo handle");
            return;
         }
         sns_sam_gyro_tap2_enable_req_msg_v01 *req =
            (sns_sam_gyro_tap2_enable_req_msg_v01 *)clientReqMsgPtr;
         gtap_config_struct *cfg = (gtap_config_struct *)algoCfgPtr;
         defCfgPtr = (gtap_config_struct *)(algoPtr->defConfigData.memPtr);

         cfg->scenario = SNS_REG_GYRO_INVALID_SCENARIO;

         if(req->scenario_valid)
         {
            defCfgPtr++;
            for(scn = 0; scn < SNS_REG_MAX_GYRO_TAP_SCENARIOS; scn++, defCfgPtr++)
            {
               if(req->scenario != SNS_REG_GYRO_INVALID_SCENARIO &&
                  req->scenario == defCfgPtr->scenario)
               {
                  cfg->scenario            =    req->scenario;
                  cfg->z_anamoly_inc       =    defCfgPtr->z_anamoly_inc;
                  cfg->strong_z_anamoly_tb =    defCfgPtr->strong_z_anamoly_tb;
                  cfg->stress_right_left   =    defCfgPtr->stress_right_left;
                  cfg->stress_top_bottom   =    defCfgPtr->stress_top_bottom;

                  cfg->tap_time_win_secs   =    defCfgPtr->tap_time_win_secs;
                  cfg->tap_time_sleep_secs =    defCfgPtr->tap_time_sleep_secs;
                  cfg->jerk_win_secs       =    defCfgPtr->jerk_win_secs;
                  cfg->ori_check_win_secs  =    defCfgPtr->ori_check_win_secs;
                  cfg->ori_check_int_secs  =    defCfgPtr->ori_check_int_secs;

                  cfg->lr_accel_rat_jerk_yx =   defCfgPtr->lr_accel_rat_jerk_yx;
                  cfg->tb_accel_rat_jerk_xy =   defCfgPtr->tb_accel_rat_jerk_xy;

                  cfg->acc_tap_thr           =  defCfgPtr->acc_tap_thr;
                  cfg->lr_accel_jerk_min_thr =  defCfgPtr->lr_accel_jerk_min_thr;
                  cfg->tb_accel_jerk_min_thr =  defCfgPtr->tb_accel_jerk_min_thr;
                  cfg->accel_z_thr           =  defCfgPtr->accel_z_thr;
                  cfg->ori_check_thr         =  defCfgPtr->ori_check_thr;
                  break;
               }
            }

            if(cfg->scenario == SNS_REG_GYRO_INVALID_SCENARIO)
            {
               /*
                  We could not find the requested scenario
                  The SAM framwork does not allow returning errors  - yet
                  So, we continue to work with the default configuration.
               */

            }
         }

         /* Let see if we need to clobber any other params */
         if(req->accel_tap_thresh_valid)
         {
            cfg->acc_tap_thr = req->accel_tap_thresh;
         }

         if(req->accel_tap_z_thresh_valid)
         {
            cfg->accel_z_thr = req->accel_tap_z_thresh;
         }
         break;
      }
 #endif // FEATURE_SNS_SAM_GYRO_TAP
      case SNS_SAM_SENSOR_THRESH_SVC_ID_V01:
      {
         threshold_config_s *cfgPtr = (threshold_config_s *)algoCfgPtr;
         sns_sam_sensor_thresh_enable_req_msg_v01 *enablePtr =
                    (sns_sam_sensor_thresh_enable_req_msg_v01 *)clientReqMsgPtr;

         if (enablePtr->sensor_id == SNS_SMGR_ID_PROX_LIGHT_V01)
         {
           cfgPtr->sensor_id = enablePtr->sensor_id;
           cfgPtr->data_type = enablePtr->data_type;
           cfgPtr->sample_rate = enablePtr->sample_rate;
           cfgPtr->thresh[0] = enablePtr->threshold[0];
           cfgPtr->num_axis = 1;
           if (enablePtr->threshold_type_valid)
           {
             cfgPtr->threshold_type = enablePtr->threshold_type;
           }
         }
         else if (enablePtr->sensor_id == SNS_SMGR_ID_PRESSURE_V01)
         {
           cfgPtr->sensor_id = enablePtr->sensor_id;
           cfgPtr->data_type = enablePtr->data_type;
           cfgPtr->sample_rate = enablePtr->sample_rate;
           if( enablePtr->report_period_valid != false)
           {
              cfgPtr->report_period = ((enablePtr->report_period)*DSPS_SLEEP_CLK)/1000;
           }
           else
           {
              cfgPtr->report_period = 0;
           }
           cfgPtr->thresh[0] = enablePtr->threshold[0];
           cfgPtr->num_axis = 1;
           if (enablePtr->threshold_type_valid)
           {
             cfgPtr->threshold_type = enablePtr ->threshold_type;
           }
         }
         else if ((enablePtr->sensor_id == SNS_SMGR_ID_ACCEL_V01) ||
                  (enablePtr->sensor_id == SNS_SMGR_ID_GYRO_V01)  ||
                  (enablePtr->sensor_id == SNS_SMGR_ID_MAG_V01))
         {
            cfgPtr->sensor_id = enablePtr->sensor_id;
            cfgPtr->data_type = enablePtr->data_type;
            cfgPtr->sample_rate = enablePtr->sample_rate;
            if(enablePtr->report_period_valid != false)
            {
               cfgPtr->report_period = ((enablePtr->report_period)*DSPS_SLEEP_CLK)/1000;
            }
            else
            {
               cfgPtr->report_period = 0;
            }

            cfgPtr->thresh[0] = enablePtr->threshold[0];
            cfgPtr->thresh[1] = enablePtr->threshold[1];
            cfgPtr->thresh[2] = enablePtr->threshold[2];
            cfgPtr->num_axis = 3;
            if (enablePtr->threshold_type_valid)
            {
              cfgPtr->threshold_type = enablePtr ->threshold_type;
            }
         }
         break;
      }

#ifdef CONFIG_USE_OEM_1_ALGO   
      case SNS_OEM_1_SVC_ID_V01:
      {
         oem_1_config_s *cfgPtr = (oem_1_config_s *)algoCfgPtr;
         sns_oem_1_enable_req_msg_v01 *enablePtr =
            (sns_oem_1_enable_req_msg_v01 *)clientReqMsgPtr;

         //update config parameter here from client enable request
         if (enablePtr->sample_rate_valid)
         {
            cfgPtr->sample_rate = enablePtr->sample_rate;
         }
         break;
      }
#endif //CONFIG_USE_OEM_1_ALGO

      case SNS_SAM_MODEM_SCN_SVC_ID_V01:
      {
         modem_scn_config_s *cfgPtr = (modem_scn_config_s *)algoCfgPtr;
         sns_sam_modem_scn_enable_req_msg_v01 *enablePtr =
                    (sns_sam_modem_scn_enable_req_msg_v01 *)clientReqMsgPtr;

         cfgPtr->sample_rate = enablePtr->sample_rate;
         cfgPtr->quiet_period = MODEM_SCN_DEF_QUIET_PERIOD;
         cfgPtr->thresh = MODEM_SCN_DEF_THRESH;
         break;
      }

      case SNS_SAM_GYROBUF_SVC_ID_V01:
      {
         gyrobuf_config_s *cfgPtr = (gyrobuf_config_s *)algoCfgPtr;
         sns_sam_gyrobuf_enable_req_msg_v01 *enablePtr =
            (sns_sam_gyrobuf_enable_req_msg_v01 *)clientReqMsgPtr;

         cfgPtr->sample_rate   = enablePtr->sample_rate;
         cfgPtr->extra_sample  = ( (enablePtr->extra_sample==1) ? true : false );
         cfgPtr->init_ts       = sns_em_get_timestamp();
         break;
      }

      case SNS_SAM_AMD_SVC_ID_V01:
      {
         qmd_config_s *cfgPtr = (qmd_config_s *)algoCfgPtr;
         sns_sam_qmd_enable_req_msg_v01 *enablePtr =
            (sns_sam_qmd_enable_req_msg_v01 *)clientReqMsgPtr;
         if(TRUE == enablePtr->config_valid)
         {
            cfgPtr->internal_config_param1 = enablePtr->config.var_thresh;
            cfgPtr->internal_config_param2 = enablePtr->config.var_win_len;
         }
         break;
      }

      case SNS_SAM_PED_SVC_ID_V01:
      {
         ped_config_s *cfgPtr = (ped_config_s *)algoCfgPtr;
         sns_sam_ped_enable_req_msg_v01 *enablePtr =
            (sns_sam_ped_enable_req_msg_v01 *)clientReqMsgPtr;
         if( enablePtr->sample_rate_valid == true )
         {
            cfgPtr->sample_rate = enablePtr->sample_rate;
         }
         else
         {
            cfgPtr->sample_rate = PED_SAMPLE_RATE_DEF_Q16;
         }
         if( enablePtr->step_count_threshold_valid == true &&
             enablePtr->report_period == 0 )
         {
            cfgPtr->step_count_threshold = enablePtr->step_count_threshold;
         }
         else
         {
            cfgPtr->step_count_threshold = PED_STEP_COUNT_THRESHOLD_DEF;
         }
         break;
      }

      case SNS_SAM_PAM_SVC_ID_V01:
      {
         pam_config_s *cfgPtr = (pam_config_s *)algoCfgPtr;
         sns_sam_pam_enable_req_msg_v01 *enablePtr =
            (sns_sam_pam_enable_req_msg_v01 *)clientReqMsgPtr;
         if(enablePtr->sample_rate_valid == 1)
         {
            cfgPtr->sample_rate = enablePtr->sample_rate;
         }
         else
         {
            cfgPtr->sample_rate = PED_SAMPLE_RATE_DEF_Q16;
         }
         cfgPtr->measurement_period = enablePtr->measurement_period;
         cfgPtr->step_count_threshold = enablePtr->step_count_threshold;
         break;
      }

      case SNS_SAM_CMC_SVC_ID_V01:
      {
         break;
      }

      case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
      {
         distance_bound_config_s *cfgPtr = (distance_bound_config_s *)algoCfgPtr;
         sns_sam_distance_bound_enable_req_msg_v01 *enablePtr =
            (sns_sam_distance_bound_enable_req_msg_v01 *)clientReqMsgPtr;


         uint8_t motion_state_index;
         if (enablePtr->motion_state_speed_bounds_valid == true)
         {
            for (motion_state_index = 0; motion_state_index < DISTANCE_BOUND_MOTION_STATE_NUM; ++motion_state_index)
            {
               cfgPtr->motion_state_speed_bounds[motion_state_index] = enablePtr->motion_state_speed_bounds[motion_state_index];
            }
         }
         else
         {
            // TODO:
         }
         break;
      }

      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         sns_sam_quat_enable_req_msg_v01 *req =
            (sns_sam_quat_enable_req_msg_v01 *)clientReqMsgPtr;
         gyro_quat_config_s *cfg = (gyro_quat_config_s *)algoCfgPtr;

         if (req->sample_rate_valid)
         {
            cfg->sample_rate = req->sample_rate;
         }

         sampleRatePtr = &(cfg->sample_rate);
         reportPeriod = req->report_period;
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         sns_sam_gravity_enable_req_msg_v01 *req =
            (sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr;
         gravity_config_s *cfg = (gravity_config_s *)algoCfgPtr;

         if (req->sample_rate_valid)
         {
            cfg->sample_rate = req->sample_rate;
         }

         sampleRatePtr = &(cfg->sample_rate);
         reportPeriod = req->report_period;
         break;
      }
      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         sns_sam_filtered_mag_enable_req_msg_v01 *req =
            (sns_sam_filtered_mag_enable_req_msg_v01 *)clientReqMsgPtr;
         fmv_config_s *cfg = (fmv_config_s *)algoCfgPtr;

         if (req->sample_rate_valid)
         {
            cfg->sample_rate = req->sample_rate;
         }

         sampleRatePtr = &(cfg->sample_rate);
         reportPeriod = req->report_period;
         break;
      }
      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      {
         sns_sam_rotation_vector_enable_req_msg_v01 *req =
            (sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr;
         rotation_vector_config_s *cfg = (rotation_vector_config_s *)algoCfgPtr;

         if (req->sample_rate_valid)
         {
            cfg->sample_rate = req->sample_rate;
         }

         if (req->coordinate_sys_valid)
         {
            cfg->coordinate_sys = req->coordinate_sys;
         }

         sampleRatePtr = &(cfg->sample_rate);
         reportPeriod = req->report_period;
         break;
      }
      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
         sns_sam_orientation_enable_req_msg_v01 *req =
            (sns_sam_orientation_enable_req_msg_v01 *)clientReqMsgPtr;
         orientation_config_s *cfg = (orientation_config_s *)algoCfgPtr;

         if (req->sample_rate_valid)
         {
            cfg->sample_rate = req->sample_rate;
         }

         if (req->coordinate_sys_valid)
         {
            cfg->coordinate_sys = req->coordinate_sys;
         }

         sampleRatePtr = &(cfg->sample_rate);
         reportPeriod = req->report_period;
         break;
      }
      case SNS_SAM_SMD_SVC_ID_V01:
      {
         /* SMD does not support client api configurable parameters */
         break;
      }
      case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      {
         sns_sam_game_rotation_vector_enable_req_msg_v01 *req =
            (sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr;
         game_rot_vec_config_s *cfg = ( game_rot_vec_config_s *)algoCfgPtr;

         if (req->sample_rate_valid)
         {
            cfg->sample_rate = req->sample_rate;
         }
         if (req->coordinate_sys_valid)
         {
            cfg->coordinate_sys = req->coordinate_sys;
         }

         sampleRatePtr = &(cfg->sample_rate);
         reportPeriod = req->report_period;
         break;
      }
      case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      {
        sns_sam_tilt_detector_enable_req_msg_v01 *req =
            (sns_sam_tilt_detector_enable_req_msg_v01 *)clientReqMsgPtr;

        tilt_config_s *cfg = (tilt_config_s *)algoCfgPtr;

        // Update the angle threshold if a non-zero value is requested
        if (0 != req->angle_thresh)
        {
          cfg->angle_threshold = req->angle_thresh*PI/180.0;
        }
        break;
      }

      default:
         return;
   }  /* End switch */

   // if sampling rate is slower than report rate, make it match
   if (sampleRatePtr != NULL && *sampleRatePtr > 0 && reportPeriod > 0)
   {
      float sampleRate = (float)FX_FIXTOFLT_Q16(*sampleRatePtr);

      float reportRate = (float)(1. / FX_FIXTOFLT_Q16(reportPeriod));

      if (reportRate - sampleRate > .1)
      {
         *sampleRatePtr = FX_FLTTOFIX_Q16(reportRate);
      }
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_update_algo_input
  =========================================================================*/
/*!
  @brief Update algorithm input structure

  @param[i] algoSvcId: algorithm service Id
  @param[i] indMsgType: indication message type
  @param[i] dataPtr: pointer to data in SMGR report indication
  @param[io] algoInpPtr: pointer to algorithm input structure
  @param[i] algoInpSize: size of algorithm input structure
  @param[i] timestamp: input timestamp
  @param[i] sensorID: SMGR Sensor ID for the indication
                      (invalid unless indMsgType==SNS_SMGR_BUFFERING_IND_V01)

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_update_algo_input(
   uint8_t    algoSvcId,
   uint8_t    indMsgType,
   const void *dataPtr,
   void       *algoInpPtr,
   uint32_t   algoInpSize,
   uint32_t   timestamp,
   uint8_t    sensorID)
{
   if( indMsgType == SNS_SMGR_BUFFERING_IND_V01 )
   {
      // Handle buffering indication
      sns_smgr_buffering_sample_s_v01 * samplePtr =
          (sns_smgr_buffering_sample_s_v01 *)dataPtr;
      uint8_t itemSize = sizeof(samplePtr->Data);

      if( sns_sam_get_smgr_msg_req_type(algoSvcId) != SNS_SMGR_BUFFERING_REQ_V01 )
      {
         // Service did not register for buffering indications
         return SNS_ERR_FAILED;
      }

      switch (algoSvcId)
      {
      case SNS_SAM_PED_SVC_ID_V01:
      {
         if( SNS_SMGR_ID_ACCEL_V01 == sensorID)
         {
            ((ped_input_s*)algoInpPtr)->timestamp = timestamp;
            break;
         }
         else
         {
            SNS_SAM_PRINTF1(MED, "Invalid SMGR data sent to PED (sensor ID: %i)", sensorID);
            return SNS_SUCCESS;
         }
      }
      case SNS_SAM_CMC_SVC_ID_V01:
      {
         cmc_input_s* cmcInpPtr = (cmc_input_s*)algoInpPtr;
         cmcInpPtr->timestamp = timestamp;

         if( SNS_SMGR_ID_ACCEL_V01 == sensorID )
         {
            cmcInpPtr->datatype = CMC_ACCEL_INPUT;
         }
         else
         {
            SNS_SAM_PRINTF1(MED, "Invalid SMGR data sent to CMC (sensor ID: %i)", sensorID);
            return SNS_SUCCESS;
         }
         break;
      }

      case SNS_SAM_SMD_SVC_ID_V01:
      {
         if( SNS_SMGR_ID_ACCEL_V01 == sensorID)
         {
            smd_input_s* smdInpPtr = (smd_input_s*)algoInpPtr;
            smdInpPtr->timestamp = timestamp;
            smdInpPtr->datatype = SMD_ACCEL_INPUT;
            break;
         }
         else
         {
            SNS_SAM_PRINTF1(MED, "Invalid SMGR data sent to SMD (sensor ID: %i)", sensorID);
            return SNS_SUCCESS;
         }
      }

      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         if(SNS_SMGR_ID_GYRO_V01 == sensorID )
         {
            gyro_quat_input_s *quatInpPtr = (gyro_quat_input_s*)algoInpPtr;
            quatInpPtr->timestamp = timestamp;
            break;
         }
         else
         {
            SNS_SAM_PRINTF1(MED, "Invalid SMGR data sent to QUAT(sensor ID: %i)", sensorID);
            return SNS_SUCCESS;
         }
      }

      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         if( SNS_SMGR_ID_ACCEL_V01 == sensorID )
         {
            gravity_input_s* gravityInpPtr = (gravity_input_s*)algoInpPtr;
            gravityInpPtr->timestamp = timestamp;
            gravityInpPtr->datatype = GRAVITY_ACCEL_INPUT;
            break;
         }
         else
         {
            SNS_SAM_PRINTF1(MED, "Invalid SMGR data sent to GRAV (sensor ID: %i)", sensorID);
            return SNS_SUCCESS;
         }
      }

      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      {
         SNS_SAM_PRINTF2(MED, "Invalid SMGR data sent to svc %d(sensor ID: %i)", algoSvcId, sensorID);
         return SNS_SUCCESS;
      }

      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
        orientation_input_s* orientationInpPtr = (orientation_input_s*)algoInpPtr;
        orientationInpPtr->timestamp = timestamp;

        if( SNS_SMGR_ID_ACCEL_V01 == sensorID )
        {
          orientationInpPtr->input_type = ORIENTATION_ACC_DATA;
        }
        else if( SNS_SMGR_ID_MAG_V01 == sensorID )
        {
          orientationInpPtr->input_type = ORIENTATION_MAG_DATA;
        }
        else
        {
           SNS_SAM_PRINTF1(MED, "Invalid SMGR data sent to ORI (sensor ID: %i)", sensorID);
           return SNS_SUCCESS;
        }
        orientationInpPtr->accuracy = 3;  // TODO: Use new SMGR field for accuracy
        break;
      }

      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         fmv_input_s* fmvInpPtr = (fmv_input_s*)algoInpPtr;
         fmvInpPtr->ts = timestamp;

         if( SNS_SMGR_ID_GYRO_V01 == sensorID )
         {
          fmvInpPtr->type = FMV_GYRO_UPDATE;
          fmvInpPtr->data.gyro[0] = (float)FX_FIXTOFLT_Q16(samplePtr->Data[0]);
          fmvInpPtr->data.gyro[1] = (float)FX_FIXTOFLT_Q16(samplePtr->Data[1]);
          fmvInpPtr->data.gyro[2] = (float)FX_FIXTOFLT_Q16(samplePtr->Data[2]);
         }
         else if( SNS_SMGR_ID_MAG_V01 == sensorID )
         {
          fmvInpPtr->type = FMV_CALIB_MAG_UPDATE;
          fmvInpPtr->data.calib_mag[0] = (float)(samplePtr->Data[0]/65536.0);
          fmvInpPtr->data.calib_mag[1] = (float)(samplePtr->Data[1]/65536.0);
          fmvInpPtr->data.calib_mag[2] = (float)(samplePtr->Data[2]/65536.0);
          fmvInpPtr->accuracy = 3;  // TODO: Use new SMGR field for accuracy
         }
         else
         {
           SNS_SAM_PRINTF1( MED,
                            "Invalid SMGR data sent to FMV (sensor ID: %i)",
                            sensorID);
         }
         return SNS_SUCCESS;
      }

      case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      {
         if( SNS_SMGR_ID_ACCEL_V01 == sensorID)
         {
            tilt_input_s* tiltInpPtr = (tilt_input_s*)algoInpPtr;
            tiltInpPtr->ts = timestamp;
            tiltInpPtr->input_type = TILT_INPUT_ACCEL;
            break;
         }
         else
         {
            SNS_SAM_PRINTF1(MED, "Invalid SMGR data sent to Tilt Detector (sensor ID: %i)", sensorID);
            return SNS_SUCCESS;
         }
      }

      default:
         break;
      } /* End switch(algoSvcId) */

      //copy sensor data to algorithm input
      if( itemSize > algoInpSize )
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_SMGR_IND_INVALID,
                        algoInpSize, itemSize);
         return SNS_ERR_FAILED;
      }
      SNS_OS_MEMCOPY(algoInpPtr, samplePtr->Data, itemSize);
   }
   else if( indMsgType == SNS_SMGR_REPORT_IND_V01 )
   {
      // Handle periodic report
      sns_smgr_data_item_s_v01* smgrItemPtr =
         (sns_smgr_data_item_s_v01*)dataPtr;

      /* Copies information (other than sensor data) to algorithm input */
      switch(algoSvcId)
      {
         case SNS_SAM_GYROBUF_SVC_ID_V01:
         {
            gyrobuf_input_s  *InpPtr = (gyrobuf_input_s *)algoInpPtr;
            if (smgrItemPtr->SensorId != SNS_SMGR_ID_GYRO_V01)
            {
               return SNS_ERR_FAILED;
            }
            InpPtr->timestamp = smgrItemPtr->TimeStamp;
            break;
         }
#ifdef FEATURE_SNS_SAM_TILT
         case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
         {
             integ_angle_input_struct* inpPtr = (integ_angle_input_struct *)algoInpPtr;

             if (smgrItemPtr->SensorId != SNS_SMGR_ID_GYRO_V01)
             {
                 return (SNS_ERR_FAILED);
             }

             inpPtr->ts = smgrItemPtr->TimeStamp;

             inpPtr->gyro[0] = smgrItemPtr->ItemData[0];
             inpPtr->gyro[1] = smgrItemPtr->ItemData[1];
             inpPtr->gyro[2] = smgrItemPtr->ItemData[2];

             return SNS_SUCCESS;
         }
#endif /* #ifdef FEATURE_SNS_SAM_TILT */
         case SNS_SAM_SENSOR_THRESH_SVC_ID_V01:
            ((threshold_input_s*) algoInpPtr)->timestamp = smgrItemPtr->TimeStamp;
            break;

#ifdef CONFIG_USE_OEM_1_ALGO
         case SNS_OEM_1_SVC_ID_V01:
            //copy smgr item timestamp to algo input timestamp;
            ((oem_1_input_s*)algoInpPtr)->sample1_ts = smgrItemPtr->TimeStamp; 
            break;
#endif //CONFIG_USE_OEM_1_ALGO

         case SNS_SAM_PED_SVC_ID_V01:
            ((ped_input_s*)algoInpPtr)->timestamp = smgrItemPtr->TimeStamp;
            break;

         case SNS_SAM_MODEM_SCN_SVC_ID_V01:
         {
            modem_scn_input_s *inPtr =  (modem_scn_input_s *)algoInpPtr;
            inPtr->timestamp =  smgrItemPtr->TimeStamp;
            inPtr->sample    = smgrItemPtr->ItemData[0];
            SNS_SAM_PRINTF3(LOW, "%d:ts %u, sample %d",
                            SNS_SAM_MODEM_SCN_SVC_ID_V01,
                            inPtr->timestamp, inPtr->sample);
             return SNS_SUCCESS;
           }

         case SNS_SAM_CMC_SVC_ID_V01:
         {
            cmc_input_s* cmcInpPtr = (cmc_input_s*)algoInpPtr;
            cmcInpPtr->timestamp = smgrItemPtr->TimeStamp;

            if (smgrItemPtr->SensorId == SNS_SMGR_ID_ACCEL_V01)
            {
              cmcInpPtr->datatype = CMC_ACCEL_INPUT;
            }
            else
            {
              SNS_SAM_PRINTF1(HIGH, "Invalid SMGR data sent to CMC (sensor ID: %i)", sensorID);
              return SNS_ERR_FAILED;
            }
            break;
         }

         case SNS_SAM_QUATERNION_SVC_ID_V01:
         {
            gyro_quat_input_s *quatInpPtr = (gyro_quat_input_s*)algoInpPtr;

            if (smgrItemPtr->SensorId != SNS_SMGR_ID_GYRO_V01)
            {
               return SNS_ERR_FAILED;
            }
            quatInpPtr->timestamp = smgrItemPtr->TimeStamp;
            break;
         }

         case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
         {
            gravity_input_s* gravityInpPtr = (gravity_input_s*)algoInpPtr;

            if (smgrItemPtr->SensorId != SNS_SMGR_ID_ACCEL_V01)
            {
               return SNS_ERR_FAILED;
            }
            gravityInpPtr->timestamp = smgrItemPtr->TimeStamp;
            gravityInpPtr->datatype = GRAVITY_ACCEL_INPUT;
            break;
         }

         case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
         {
            fmv_input_s* fmvInpPtr = (fmv_input_s*)algoInpPtr;

            if (smgrItemPtr->SensorId == SNS_SMGR_ID_GYRO_V01)
            {
               fmvInpPtr->type = FMV_GYRO_UPDATE;
               fmvInpPtr->data.gyro[0] = (float)FX_FIXTOFLT_Q16(smgrItemPtr->ItemData[0]);
               fmvInpPtr->data.gyro[1] = (float)FX_FIXTOFLT_Q16(smgrItemPtr->ItemData[1]);
               fmvInpPtr->data.gyro[2] = (float)FX_FIXTOFLT_Q16(smgrItemPtr->ItemData[2]);
            }
            else if (smgrItemPtr->SensorId == SNS_SMGR_ID_MAG_V01)
            {
               fmvInpPtr->type = FMV_CALIB_MAG_UPDATE;
               fmvInpPtr->data.calib_mag[0] = (float)(smgrItemPtr->ItemData[0]/65536.0);
               fmvInpPtr->data.calib_mag[1] = (float)(smgrItemPtr->ItemData[1]/65536.0);
               fmvInpPtr->data.calib_mag[2] = (float)(smgrItemPtr->ItemData[2]/65536.0);
               fmvInpPtr->accuracy = 3; // TODO: Use new SMGR field for accuracy
            }
            else
            {
               return SNS_ERR_FAILED;
            }
            fmvInpPtr->ts = smgrItemPtr->TimeStamp;

            return SNS_SUCCESS;
         }

         case SNS_SAM_ORIENTATION_SVC_ID_V01:
         {
           orientation_input_s* orientationInpPtr = (orientation_input_s*)algoInpPtr;
           orientationInpPtr->timestamp = smgrItemPtr->TimeStamp;

           if (smgrItemPtr->SensorId == SNS_SMGR_ID_ACCEL_V01)
           {
              orientationInpPtr->input_type = ORIENTATION_ACC_DATA;
           }
           else if (smgrItemPtr->SensorId == SNS_SMGR_ID_MAG_V01)
           {
              orientationInpPtr->input_type = ORIENTATION_MAG_DATA;
           }
           else
           {
              SNS_SAM_PRINTF1(HIGH, "Invalid SMGR data sent to ORI (sensor ID: %i)", sensorID);
              return SNS_ERR_FAILED;
           }
           orientationInpPtr->accuracy = 3;  // TODO: Use new SMGR field for accuracy
           break;
         }
         case SNS_SAM_SMD_SVC_ID_V01:
         {
            smd_input_s* inpPtr = (smd_input_s *) algoInpPtr;
            if(smgrItemPtr->SensorId != SNS_SMGR_ID_ACCEL_V01)
            {
               return SNS_ERR_FAILED;
            }
            inpPtr->timestamp = smgrItemPtr->TimeStamp;
            inpPtr->datatype = SMD_ACCEL_INPUT;
            break;
         }
         case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
         {
            gtap_input_struct* gtapInpPtr = (gtap_input_struct*)algoInpPtr;

            if (smgrItemPtr->SensorId == SNS_SMGR_ID_ACCEL_V01)
            {
               gtapInpPtr->datatype = GTAP_ACCEL_INPUT;
               gtapInpPtr->data.a[0] = smgrItemPtr->ItemData[0];
               gtapInpPtr->data.a[1] = smgrItemPtr->ItemData[1];
               gtapInpPtr->data.a[2] = smgrItemPtr->ItemData[2];
            }
            else if (smgrItemPtr->SensorId == SNS_SMGR_ID_GYRO_V01)
            {
               gtapInpPtr->datatype = GTAP_GYRO_INPUT;
               gtapInpPtr->data.g[0] = smgrItemPtr->ItemData[0];
               gtapInpPtr->data.g[1] = smgrItemPtr->ItemData[1];
               gtapInpPtr->data.g[2] = smgrItemPtr->ItemData[2];
            }
            else
            {
               return SNS_ERR_FAILED;
            }
            return SNS_SUCCESS;
         }

         case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
         case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
         {
            // These algos do not take sensor input
            SNS_SAM_PRINTF2(ERROR,
                            "Invalid SMGR data sent to svc %d (sensor ID: %i)",
                            algoSvcId, sensorID);
            return SNS_ERR_FAILED;
         }

         case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
         {
            tilt_input_s* inpPtr = (tilt_input_s *) algoInpPtr;
            if(smgrItemPtr->SensorId != SNS_SMGR_ID_ACCEL_V01)
            {
               return SNS_ERR_FAILED;
            }
            inpPtr->ts = smgrItemPtr->TimeStamp;
            inpPtr->input_type = TILT_INPUT_ACCEL;
            break;
         }

         default:
            break;
      } /* End switch(algoSvcId) */


      //copy sensor data to algorithm input
      if (algoInpSize >= sizeof(smgrItemPtr->ItemData))
      {
         SNS_OS_MEMCOPY(algoInpPtr,
                        smgrItemPtr->ItemData,
                        sizeof(smgrItemPtr->ItemData));
      }
      else
      {
         SNS_SAM_DEBUG2(ERROR, DBG_SAM_SMGR_IND_INVALID,
                        algoInpSize, sizeof(smgrItemPtr->ItemData));
         return SNS_ERR_FAILED;
      }
   }
   else
   {
      // Unknown message type
      SNS_SAM_PRINTF1(ERROR, "SAM: Unknown SMGR ind type %d", indMsgType);
      return SNS_ERR_NOTSUPPORTED;
   }
   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sam_process_sam_response
  =========================================================================*/
/*!
  @brief Process the response received from another SAM module

  @param[i] samRespPtr: Pointer to the sam response message

  @return algo instance id of dependent algorithm
*/
/*=======================================================================*/
uint8_t sns_sam_process_sam_response(
   const void *samRespPtr)
{
   uint8_t depAlgoInstId = SNS_SAM_INVALID_ID;
   sns_smr_header_s msgHdr;
   sns_smr_get_hdr(&msgHdr, samRespPtr);

   if( msgHdr.msg_id == SNS_SAM_ALGO_ENABLE_RESP )
   {
      switch (msgHdr.svc_num)
      {
         case SNS_SAM_AMD_SVC_ID_V01:
            depAlgoInstId =
               ((sns_sam_qmd_enable_resp_msg_v01*)samRespPtr)->instance_id;
            break;
         case SNS_SAM_PED_SVC_ID_V01:
            if (((sns_sam_ped_enable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_ped_enable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_CMC_SVC_ID_V01:
            if (((sns_sam_cmc_enable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_cmc_enable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_QUATERNION_SVC_ID_V01:
            if (((sns_sam_quat_enable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_quat_enable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
            if (((sns_sam_gravity_enable_resp_msg_v01*)samRespPtr)->instance_id)
            {
               depAlgoInstId =
                  ((sns_sam_gravity_enable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
            if (((sns_sam_filtered_mag_enable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_filtered_mag_enable_resp_msg_v01*)samRespPtr)->instance_id;
            }
         case SNS_SAM_SMD_SVC_ID_V01:
            if (((sns_sam_smd_enable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_smd_enable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         default:
            break;
      }
   }
   else if( msgHdr.msg_id == SNS_SAM_ALGO_DISABLE_RESP )
   {
      switch (msgHdr.svc_num)
      {
         case SNS_SAM_AMD_SVC_ID_V01:
            depAlgoInstId =
               ((sns_sam_qmd_disable_resp_msg_v01*)samRespPtr)->instance_id;

            break;
         case SNS_SAM_PED_SVC_ID_V01:
            if (((sns_sam_ped_disable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_ped_disable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_CMC_SVC_ID_V01:
            if (((sns_sam_cmc_disable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_cmc_disable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_QUATERNION_SVC_ID_V01:
            if (((sns_sam_quat_disable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_quat_disable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
            if (((sns_sam_gravity_disable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_gravity_disable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
            if (((sns_sam_filtered_mag_disable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_filtered_mag_disable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         case SNS_SAM_SMD_SVC_ID_V01:
            if (((sns_sam_smd_disable_resp_msg_v01*)samRespPtr)->instance_id_valid)
            {
               depAlgoInstId =
                  ((sns_sam_smd_disable_resp_msg_v01*)samRespPtr)->instance_id;
            }
            break;
         default:
            break;
      }
   }

   return depAlgoInstId;
}

/*=========================================================================
  FUNCTION:  sns_sam_process_sam_report_ind
  =========================================================================*/
/*!
  @brief Process the indication received from another SAM module

  @param[i] samIndPtr: Pointer to the sam indication message
  @param[i] clientAlgoInstPtr: pointer to client algorithm instance
  @param[i] algoSvcId: algorithm service id
  @param[i] algoInstId: algorithm instance id

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_process_sam_report_ind(
   const void* samIndPtr,
   sns_sam_algo_inst_s* clientAlgoInstPtr,
   uint8_t algoSvcId,
   uint8_t algoInstId)
{
   void *dataPtr = NULL;

   switch (algoSvcId)
   {
      case SNS_SAM_AMD_SVC_ID_V01:
      {
         sns_sam_qmd_report_ind_msg_v01 *indPtr =
              (sns_sam_qmd_report_ind_msg_v01 *) samIndPtr;
         if (indPtr->instance_id <  SNS_SAM_MAX_ALGO_INSTS)
         {
            if (clientAlgoInstPtr->serviceId == SNS_SAM_PED_SVC_ID_V01 &&
                clientAlgoInstPtr->motion_state != indPtr->state)
            {
               // start/stop sensor data based on motion state
               clientAlgoInstPtr->motion_state = indPtr->state;
               if( clientAlgoInstPtr->motion_state == SNS_SAM_MOTION_REST_V01)
               {
                  sns_sam_stop_sensor_data( algoInstId );
               }
               else if( clientAlgoInstPtr->motion_state == SNS_SAM_MOTION_MOVE_V01 &&
                  clientAlgoInstPtr->dutycycleStateOn )
               {
                  ped_state_s * statePtr = (ped_state_s*)clientAlgoInstPtr->stateData.memPtr;
                  statePtr->num_samples = 0;
                  statePtr->total_sample_rate = 0.0f;
                  statePtr->prev_input_timestamp = 0;
                  sns_sam_req_sensor_data( algoInstId, SNS_SAM_INVALID_ID );
               }
               return SNS_SUCCESS;
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_PAM_SVC_ID_V01)
            {
               pam_input_s * algoInPtr =  clientAlgoInstPtr->inputData.memPtr;
               algoInPtr->datatype = PAM_QMD_INPUT;
               algoInPtr->timestamp = indPtr->timestamp;
               algoInPtr->data.motion_state = indPtr->state;
               dataPtr = algoInPtr;
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_CMC_SVC_ID_V01)
            {
               cmc_input_s *algoInPtr = (cmc_input_s *)clientAlgoInstPtr->inputData.memPtr;

               // Report AMD state to CMC irrespective of its value
               algoInPtr->datatype = CMC_QMD_INPUT;
               algoInPtr->data.qmd_motion_state = (qmd_ms_e)(indPtr->state);
               algoInPtr->timestamp = indPtr->timestamp;
               dataPtr = algoInPtr;
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_SMD_SVC_ID_V01)
            {
               smd_input_s *algoInpPtr =
                  (smd_input_s *) clientAlgoInstPtr->inputData.memPtr;
           
               algoInpPtr->datatype = SMD_AMD_INPUT;
               algoInpPtr->timestamp = indPtr->timestamp;
               algoInpPtr->data.amd_state = indPtr->state;
               if(indPtr->state == SNS_SAM_MOTION_REST_V01)
               {
                  sns_sam_stop_sensor_data(algoInstId);
               }
               else if(indPtr->state == SNS_SAM_MOTION_MOVE_V01)
               {
                  sns_sam_req_sensor_data(algoInstId, SNS_SAM_INVALID_ID);
               }
               dataPtr = algoInpPtr;
               sns_sam_log_smd_result(algoInstId, clientAlgoInstPtr);
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01)
            {
              tilt_input_s *algoInpPtr =
                  (tilt_input_s *) clientAlgoInstPtr->inputData.memPtr;

               algoInpPtr->input_type = TILT_INPUT_AMD;
               algoInpPtr->ts = indPtr->timestamp;
               algoInpPtr->input_data.amd_state = indPtr->state;
               if(indPtr->state == SNS_SAM_MOTION_REST_V01)
               {
                  sns_sam_stop_sensor_data(algoInstId);
               }
               else if(indPtr->state == SNS_SAM_MOTION_MOVE_V01)
               {
                  sns_sam_req_sensor_data(algoInstId, SNS_SAM_INVALID_ID);
               }
               dataPtr = algoInpPtr;
               sns_sam_log_tilt_detector_result(algoInstId, clientAlgoInstPtr);
            }
         }
      }
      break;

      case SNS_SAM_PED_SVC_ID_V01:
      {
         sns_sam_ped_report_ind_msg_v01 *indPtr =
            (sns_sam_ped_report_ind_msg_v01 *) samIndPtr;
         if(indPtr->instance_id < SNS_SAM_MAX_ALGO_INSTS)
         {
            if(clientAlgoInstPtr->serviceId == SNS_SAM_PAM_SVC_ID_V01)
            {
               pam_input_s * algoInPtr = clientAlgoInstPtr->inputData.memPtr;
               uint8_t i;

               algoInPtr->datatype  = PAM_PED_INPUT;
               algoInPtr->timestamp = indPtr->timestamp;
               algoInPtr->data.step_count = indPtr->report_data.step_count;
               dataPtr = algoInPtr;

               // reset pedometer client stats
               for(i=0; i< SNS_SAM_MAX_ALGO_DEPS; i++)
               {
                  uint8_t depAlgoInstId = clientAlgoInstPtr->algoReqDbase[i];
                  sns_sam_algo_inst_s* depAlgoInstPtr =
                     sns_sam_get_algo_inst_handle(depAlgoInstId);
                  if(depAlgoInstPtr == NULL)
                  {
                     break;
                  }
                  else if(depAlgoInstPtr->serviceId == SNS_SAM_PED_SVC_ID_V01)
                  {
                     sns_err_code_e err;
                     sns_smr_header_s msgHdr;
                     uint8_t depAlgoIndex;
                     void *resp_msg_ptr = NULL;

                     sns_sam_ped_reset_req_msg_v01 *resetMsgPtr =
                        (sns_sam_ped_reset_req_msg_v01 *) sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
                                                                            sizeof(sns_sam_ped_reset_req_msg_v01));
                     SNS_ASSERT(resetMsgPtr != NULL);

                     msgHdr.src_module = SNS_SAM_MODULE;
                     msgHdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
                     msgHdr.priority = SNS_SMR_MSG_PRI_HIGH;
                     msgHdr.msg_id = SNS_SAM_ALGO_UPDATE_REQ;
                     msgHdr.svc_num = depAlgoInstPtr->serviceId;
                     msgHdr.ext_clnt_id = algoInstId;
                     msgHdr.txn_id = 0;
                     msgHdr.body_len = sizeof(sns_sam_ped_reset_req_msg_v01);

                     resetMsgPtr->instance_id = depAlgoInstId;

                     sns_smr_set_hdr(&msgHdr, resetMsgPtr);

                     depAlgoIndex = sns_sam_get_algo_index(SNS_SAM_PED_SVC_ID_V01);
                     if( depAlgoIndex > SNS_SAM_MAX_ALGOS )
                     {
                        // Dependent algorithm is not co-located
                        err = sns_sam_mr_send(resetMsgPtr, clientAlgoInstPtr->mrAlgoConnHndl);
                     }
                     else
                     {
                        // Dependent algorithm is co-located. Skip QMI.
                        err = sns_sam_process_client_req( resetMsgPtr, SNS_SAM_MAX_CLI_ID + algoInstId, &resp_msg_ptr );
                        if( err == SNS_SUCCESS )
                        {
                           sns_smr_msg_free(resp_msg_ptr);
                        }
                        if ( NULL != resetMsgPtr )
                        {
                           sns_smr_msg_free(resetMsgPtr);
                        }
                     }
                     if (err != SNS_SUCCESS)
                     {
                        SNS_SAM_PRINTF1(ERROR, "Failed to reset Ped, error: %d", err);
                     }
                     break;
                  }
               }
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_SMD_SVC_ID_V01)
            {
               smd_input_s *algoInPtr =
                  (smd_input_s *) clientAlgoInstPtr->inputData.memPtr;

               algoInPtr->datatype = SMD_PED_INPUT;
               algoInPtr->timestamp = indPtr->timestamp;
               algoInPtr->data.step_conf = indPtr->report_data.step_confidence;
               dataPtr = algoInPtr;
               sns_sam_log_smd_result(algoInstId, clientAlgoInstPtr);
            }
         }
      }
      break;
      case SNS_SAM_MODEM_SCN_SVC_ID_V01:
      {

         SNS_SAM_PRINTF3(HIGH,
                         "Handling modem scn service indication, %d, %d, %d",
                         0, 0, 0);
         break;
      }

      case SNS_SAM_CMC_SVC_ID_V01:
      {
         sns_sam_cmc_report_ind_msg_v01 *indPtr =
           (sns_sam_cmc_report_ind_msg_v01 *) samIndPtr;

         if (clientAlgoInstPtr->serviceId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
         {
            distance_bound_input_s *algoInPtr = (distance_bound_input_s *)clientAlgoInstPtr->inputData.memPtr;

            algoInPtr->datatype = DISTANCE_BOUND_MOTION_STATE_INPUT;
            algoInPtr->data.cmc_motion_event.motion_state = indPtr->report_data.motion_state;
            algoInPtr->data.cmc_motion_event.motion_event = indPtr->ms_event;
            // Use the timestamp of the CMC motion state indication
            algoInPtr->timestamp = indPtr->timestamp;
            dataPtr = algoInPtr;
         }
      }
      break;
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         sns_sam_quat_report_ind_msg_v01 *indPtr =
         (sns_sam_quat_report_ind_msg_v01*)samIndPtr;

         if (indPtr->instance_id < SNS_SAM_MAX_ALGO_INSTS)
         {
            if (clientAlgoInstPtr->serviceId == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01)
            {
               gravity_input_s* algoInPtr = clientAlgoInstPtr->inputData.memPtr;
               algoInPtr->datatype = GRAVITY_QUAT_INPUT;
               algoInPtr->timestamp = indPtr->timestamp;
               dataPtr = algoInPtr->data.quat;
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01)
            {
               game_rot_vec_input_s* algoInPtr = clientAlgoInstPtr->inputData.memPtr;
               algoInPtr->input_type = GAME_ROT_VEC_GYRO_QUAT_DATA;
               algoInPtr->data.gyro_quat.timestamp = indPtr->timestamp;
               dataPtr = algoInPtr->data.gyro_quat.quaternion;
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_FILTERED_MAG_SVC_ID_V01)
            {
               fmv_input_s* algoInPtr = clientAlgoInstPtr->inputData.memPtr;
               algoInPtr->type = FMV_QUAT_UPDATE;
               algoInPtr->ts = indPtr->timestamp;
               dataPtr = algoInPtr->data.q;
            }
            if (dataPtr != NULL)
            {
               SNS_OS_MEMCOPY(dataPtr,
                              indPtr->result.quaternion,
                              sizeof(indPtr->result.quaternion));
            }
         }
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         sns_sam_gravity_report_ind_msg_v01 *indPtr =
         (sns_sam_gravity_report_ind_msg_v01*)samIndPtr;

         if (indPtr->instance_id < SNS_SAM_MAX_ALGO_INSTS)
         {
            if (clientAlgoInstPtr->serviceId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01)
            {
               game_rot_vec_input_s* algoInPtr = clientAlgoInstPtr->inputData.memPtr;
               algoInPtr->input_type = GAME_ROT_VEC_GRAVITY_DATA;
               algoInPtr->data.gravity.timestamp = indPtr->timestamp;
               algoInPtr->data.gravity.accuracy = indPtr->result.accuracy;
               dataPtr = algoInPtr->data.gravity.gravity;
               SNS_OS_MEMCOPY(dataPtr, indPtr->result.gravity,
                              sizeof(indPtr->result.gravity));
               if (indPtr->device_motion_state_valid)
               {
                  algoInPtr->data.gravity.device_state = indPtr->device_motion_state;
               }
               else
               {
                  algoInPtr->data.gravity.device_state = GAME_ROT_VEC_DEV_STATE_UNKNOWN;
               }
            }
            else if (clientAlgoInstPtr->serviceId == SNS_SAM_ROTATION_VECTOR_SVC_ID_V01)
            {
               rotation_vector_input_s* algoInPtr = clientAlgoInstPtr->inputData.memPtr;
               algoInPtr->input_type = ROTATION_VECTOR_GRAVITY_DATA;
               algoInPtr->gravity_vector.timestamp = indPtr->timestamp;
               algoInPtr->gravity_vector.accuracy = indPtr->result.accuracy;
               dataPtr = algoInPtr->gravity_vector.gravity;
               SNS_OS_MEMCOPY(dataPtr, indPtr->result.gravity,
                              sizeof(indPtr->result.gravity));
            }
         }
         break;
      }
      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         sns_sam_filtered_mag_report_ind_msg_v01 *indPtr =
            (sns_sam_filtered_mag_report_ind_msg_v01*)samIndPtr;
         if (indPtr->instance_id < SNS_SAM_MAX_ALGO_INSTS)
         {
            if (clientAlgoInstPtr->serviceId == SNS_SAM_ROTATION_VECTOR_SVC_ID_V01)
            {
               rotation_vector_input_s* algoInPtr = clientAlgoInstPtr->inputData.memPtr;
               algoInPtr->input_type = ROTATION_VECTOR_MAG_VEC_DATA;
               algoInPtr->mag_vector.timestamp = indPtr->timestamp;
               algoInPtr->mag_vector.accuracy = indPtr->result.accuracy;
               dataPtr = algoInPtr->mag_vector.filtered_mag;
               SNS_OS_MEMCOPY(dataPtr, indPtr->result.filtered_mag,
                              sizeof(indPtr->result.filtered_mag));
            }
         }
         break;
      }
      break;

      default:
         return SNS_SUCCESS;
   }

   if (dataPtr != NULL)
   {
      sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(clientAlgoInstPtr->serviceId);
      if (algoPtr != NULL)
      {
         sns_profiling_log_timestamp((uint64_t) (((uint64_t)(((uint64_t)SNS_SAM_ALGO_EXEC_PROFILE_START)<<32)) |
                                                             ((uint64_t)(clientAlgoInstPtr->serviceId))));
         sns_profiling_log_qdss(SNS_SAM_ALGO_UPDATE_ENTER_2, 1, clientAlgoInstPtr->serviceId);

         algoPtr->algoApi.sns_sam_algo_update(clientAlgoInstPtr->stateData.memPtr,
            clientAlgoInstPtr->inputData.memPtr,
            clientAlgoInstPtr->outputData.memPtr);

         sns_profiling_log_qdss(SNS_SAM_ALGO_UPDATE_EXIT, 1, clientAlgoInstPtr->serviceId);

         sns_profiling_log_timestamp((uint64_t) (((uint64_t)(((uint64_t)SNS_SAM_ALGO_EXEC_PROFILE_END)<<32)) |
                                                             ((uint64_t)(clientAlgoInstPtr->serviceId))));

         sns_sam_check_config_update(clientAlgoInstPtr, algoInstId);

         // TODO: Passing 0 is incorrect. It must be the clientId.
         sns_sam_update_algo_report_ts(clientAlgoInstPtr,0);

         return SNS_SUCCESS;
      }
   }

   return SNS_ERR_FAILED;
}

/*=========================================================================
  FUNCTION:  sns_sam_check_config_update
  =========================================================================*/
/*!
  @brief Checks if the algorithm configuration has changed and performs
  tasks to address any change

  @param[i] algoInstPtr: Pointer to the algorithm instance
  @param[i] algoInstId: Instance ID of the algorithm

  @return None
*/
/*=======================================================================*/
void sns_sam_check_config_update(sns_sam_algo_inst_s *algoInstPtr,
                                 uint8_t algoInstId)
{
   switch(algoInstPtr->serviceId)
   {
   case SNS_SAM_CMC_SVC_ID_V01:
     {
        bool accelOnFlagChanged = false;
        bool accel_on = false;

        if(true == ((cmc_output_s *) algoInstPtr->outputData.memPtr)->config_updated)
        {
           /* Set the config update flag to false before moving on */
           ((cmc_output_s *) algoInstPtr->outputData.memPtr)->config_updated = false;

           accel_on = ((cmc_state_s *) algoInstPtr->stateData.memPtr)->config.accel_on;

           /* Check for changes in accel_on state */
           if( accel_on != ((cmc_config_s *) algoInstPtr->configData.memPtr)->accel_on)
           {
              accelOnFlagChanged = true;
              ((cmc_config_s *) algoInstPtr->configData.memPtr)->accel_on = accel_on;
           }
        }

        if(true == accelOnFlagChanged)
        {
           if(true == accel_on)
           {
             /* Requesting accel stream here. */
              sns_sam_req_sensor_data(algoInstId, SNS_SAM_INVALID_ID);
              SNS_SAM_PRINTF0(MEDIUM, "Requesting accel stream for CMC.");
           }
           else
           {
              /* Gating accel stream here. */
              sns_sam_stop_sensor_data(algoInstId);
              SNS_SAM_PRINTF0(MEDIUM, "Stopping accel stream for CMC.");
           }
        }

        break;
     }
   default:
     return;
   }
}

/*=========================================================================
  FUNCTION:  sns_sam_get_adjusted_sample_rate
  =========================================================================*/
/*!
  @brief Adjusts sample rate to be at least the report rate

  @param[i] sampleRate: sample rate in Hz, Q16
  @param[i] reportPeriod: report period in seconds, Q16

  @return Adjusted sample rate in Hz, Q16
*/
/*=======================================================================*/
static q16_t sns_sam_get_adjusted_sample_rate(
   q16_t sampleRate,
   q16_t reportPeriod
   )
{
   float sample_rate = (float)FX_FIXTOFLT_Q16(sampleRate);
   float report_rate = (float)(1. / FX_FIXTOFLT_Q16(reportPeriod));

   if( report_rate - sample_rate > .1 )
   {
      sampleRate = FX_FLTTOFIX_Q16(report_rate);
   }

   return sampleRate;
}
/*=========================================================================
  FUNCTION:  sns_sam_gen_algo_enable_msg
  =========================================================================*/
/*!
  @brief Generate the algorithm enable request message

  @param[i] algoInstPtr: algorithm instance pointer
  @param[i] clientReqMsgPtr: client request message pointer
  @param[i] msgHdrPtr: message header pointer

  @return Sensors error code
*/
/*=======================================================================*/
void *sns_sam_gen_algo_enable_msg(
   sns_sam_algo_inst_s* algoInstPtr,
   const void* clientReqMsgPtr,
   sns_smr_header_s* msgHdrPtr)
{
   void *msgPtr = NULL;
   uint32_t sampleRate = 0;

   SNS_SAM_PRINTF2(LOW,
                  "sns_sam_gen_algo_enable_msg: algo serviceid %d, message service num %d",
                  algoInstPtr->serviceId, msgHdrPtr->svc_num);

   if (algoInstPtr->serviceId == SNS_SAM_PED_SVC_ID_V01)
   {
      switch (msgHdrPtr->svc_num)
      {
         case SNS_SAM_AMD_SVC_ID_V01:
         {
            sns_sam_qmd_enable_req_msg_v01 *enableMsgPtr =
            (sns_sam_qmd_enable_req_msg_v01 *) sns_smr_msg_alloc(
               SNS_SAM_DBG_MOD,
               sizeof(sns_sam_qmd_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            enableMsgPtr->report_period = 0;
            enableMsgPtr->config_valid = 0;

            /* allow AMD in async mode to send events during suspend */
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;

            msgHdrPtr->body_len = sizeof(sns_sam_qmd_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
            break;
         }
      }
   }

   else if (algoInstPtr->serviceId == SNS_SAM_SMD_SVC_ID_V01)
   {
      switch (msgHdrPtr->svc_num)
      {
         case SNS_SAM_AMD_SVC_ID_V01:
         {
            sns_sam_qmd_enable_req_msg_v01 *enableMsgPtr =
            (sns_sam_qmd_enable_req_msg_v01 *) sns_smr_msg_alloc(
               SNS_SAM_DBG_MOD,
               sizeof(sns_sam_qmd_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            enableMsgPtr->report_period = 0;
            enableMsgPtr->config_valid = 0;

            /* allow AMD in async mode to send events during suspend */
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;

            msgHdrPtr->body_len = sizeof(sns_sam_qmd_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
            break;
         }
         case SNS_SAM_PED_SVC_ID_V01:
         {
            sns_sam_ped_enable_req_msg_v01 *enableMsgPtr =
            (sns_sam_ped_enable_req_msg_v01 *) sns_smr_msg_alloc(
               SNS_SAM_DBG_MOD,
               sizeof(sns_sam_ped_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            enableMsgPtr->report_period = 0;
            enableMsgPtr->sample_rate_valid = 0;
            enableMsgPtr->duty_cycle_on_percent_valid = 0;
            enableMsgPtr->step_count_threshold_valid = 0;

            /* SMD is supposed to function in low power scenarios.
               allow pedometer to function even during apps processor suspend*/
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;

            msgHdrPtr->body_len = sizeof(sns_sam_ped_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
            break;
         }
         default:
            break;
      }
   } /* End SMD case */

   else if (algoInstPtr->serviceId == SNS_SAM_PAM_SVC_ID_V01)
   {
      switch (msgHdrPtr->svc_num)
      {
         case SNS_SAM_AMD_SVC_ID_V01:
         {
            sns_sam_qmd_enable_req_msg_v01 *enableMsgPtr =
            (sns_sam_qmd_enable_req_msg_v01 *) sns_smr_msg_alloc(
               SNS_SAM_DBG_MOD,
               sizeof(sns_sam_qmd_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            enableMsgPtr->report_period = 0;
            enableMsgPtr->config_valid = 0;

            /* allow AMD in async mode to send events during suspend */
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;

            msgHdrPtr->body_len = sizeof(sns_sam_qmd_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
            break;
         }
         case SNS_SAM_PED_SVC_ID_V01:
         {
            sns_sam_ped_enable_req_msg_v01 *enableMsgPtr =
            (sns_sam_ped_enable_req_msg_v01 *) sns_smr_msg_alloc(
               SNS_SAM_DBG_MOD,
               sizeof(sns_sam_ped_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            enableMsgPtr->report_period = FX_CONV_Q16(
               ((pam_config_s *)(algoInstPtr->configData.memPtr))->measurement_period, 0);
            enableMsgPtr->sample_rate_valid = 1;
            enableMsgPtr->sample_rate =
               ((pam_config_s *)(algoInstPtr->configData.memPtr))->sample_rate;
            enableMsgPtr->duty_cycle_on_percent_valid = 1;
            enableMsgPtr->duty_cycle_on_percent =
               ((pam_config_s*)(algoInstPtr->configData.memPtr))->dutycycleOnPercent;
            enableMsgPtr->step_count_threshold_valid = 0;

            /* pam is supposed to function in low power scenarios.
               allow pedometer to function even during apps processor suspend*/
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;

            msgHdrPtr->body_len = sizeof(sns_sam_ped_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
            break;
         }
      }
   } /* End PAM case */

   else if (algoInstPtr->serviceId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      switch (msgHdrPtr->svc_num)
      {
         case SNS_SAM_CMC_SVC_ID_V01:
         {
            sns_sam_cmc_enable_req_msg_v01 *enableMsgPtr =
               (sns_sam_cmc_enable_req_msg_v01 *) sns_smr_msg_alloc(
                  SNS_SAM_DBG_MOD,
                  sizeof(sns_sam_cmc_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            /* CMC is supposed to function in low power scenarios.
               allow CMC to function even during apps processor suspend*/
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;

            msgHdrPtr->body_len = sizeof(sns_sam_cmc_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
         }
      }
   }

   else if (algoInstPtr->serviceId == SNS_SAM_CMC_SVC_ID_V01)
   {
      switch (msgHdrPtr->svc_num)
      {
         case SNS_SAM_AMD_SVC_ID_V01:
         {
            sns_sam_qmd_enable_req_msg_v01 *enableMsgPtr =
            (sns_sam_qmd_enable_req_msg_v01 *) sns_smr_msg_alloc(
               SNS_SAM_DBG_MOD,
               sizeof(sns_sam_qmd_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            enableMsgPtr->report_period = 0;
            enableMsgPtr->config_valid = 0;

            /* allow AMD in async mode to send events during suspend */
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;


            msgHdrPtr->body_len = sizeof(sns_sam_qmd_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
            break;
         }
      }
   }

   else if (algoInstPtr->serviceId == SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01)
   {
      sns_sam_gravity_enable_req_msg_v01 *gravEnMsgPtr
         = (sns_sam_gravity_enable_req_msg_v01 *)clientReqMsgPtr;

      sampleRate = gravEnMsgPtr->sample_rate_valid ?
                   gravEnMsgPtr->sample_rate :
                   ((gravity_config_s*)algoInstPtr->configData.memPtr)->sample_rate;

      // if sampling rate is slower than report rate, make it match
      if( gravEnMsgPtr->report_period > 0 )
      {
         sampleRate = sns_sam_get_adjusted_sample_rate(sampleRate,
                                                       gravEnMsgPtr->report_period);
      }

      switch (msgHdrPtr->svc_num)
      {
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         sns_sam_quat_enable_req_msg_v01 *quatEnMsgPtr =
            (sns_sam_quat_enable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
               sizeof(sns_sam_quat_enable_req_msg_v01));
         SNS_ASSERT(quatEnMsgPtr != NULL);

         quatEnMsgPtr->report_period = 0;  // report synchronous to sampling rate
         quatEnMsgPtr->sample_rate_valid = true;
         quatEnMsgPtr->sample_rate = sampleRate;

         msgHdrPtr->body_len = sizeof(sns_sam_quat_enable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, quatEnMsgPtr);

         msgPtr = quatEnMsgPtr;
         break;
      }

      default:
         break;
      } /* end switch case for algos gravity vector depends on */
   } /* End gravity algo case */

   else if (algoInstPtr->serviceId == SNS_SAM_FILTERED_MAG_SVC_ID_V01)
   {
      sns_sam_filtered_mag_enable_req_msg_v01 *fmvEnMsgPtr
         = (sns_sam_filtered_mag_enable_req_msg_v01 *)clientReqMsgPtr;

      sampleRate = fmvEnMsgPtr->sample_rate_valid ?
                   fmvEnMsgPtr->sample_rate :
                   ((fmv_config_s*)algoInstPtr->configData.memPtr)->sample_rate;

      // if sampling rate is slower than report rate, make it match
      if( fmvEnMsgPtr->report_period > 0 )
      {
         sampleRate = sns_sam_get_adjusted_sample_rate(sampleRate,
                                                       fmvEnMsgPtr->report_period);
      }

      switch (msgHdrPtr->svc_num)
      {
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         sns_sam_quat_enable_req_msg_v01 *quatEnMsgPtr =
            (sns_sam_quat_enable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
               sizeof(sns_sam_quat_enable_req_msg_v01));
         SNS_ASSERT(quatEnMsgPtr != NULL);

         quatEnMsgPtr->report_period = 0;  // report synchronous to sampling rate
         quatEnMsgPtr->sample_rate_valid = true;
         quatEnMsgPtr->sample_rate = sampleRate;

         msgHdrPtr->body_len = sizeof(sns_sam_quat_enable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, quatEnMsgPtr);

         msgPtr = quatEnMsgPtr;
         break;
      }

      default:
         break;
      } /* end switch case for algos magnetic filter depends on */
   } /* End magnetic filter algo case */

   else if (algoInstPtr->serviceId == SNS_SAM_ROTATION_VECTOR_SVC_ID_V01)
   {
      sns_sam_rotation_vector_enable_req_msg_v01 *rotVecEnMsgPtr
      = (sns_sam_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr;

      sampleRate = rotVecEnMsgPtr->sample_rate_valid ?
                   rotVecEnMsgPtr->sample_rate :
                   ((rotation_vector_config_s*)algoInstPtr->configData.memPtr)->sample_rate;

      // if sampling rate is slower than report rate, make it match
      if( rotVecEnMsgPtr->report_period > 0 )
      {
         sampleRate = sns_sam_get_adjusted_sample_rate(sampleRate,
                                                       rotVecEnMsgPtr->report_period);
      }

      switch (msgHdrPtr->svc_num)
      {
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         sns_sam_gravity_enable_req_msg_v01 *gravityEnMsgPtr =
         (sns_sam_gravity_enable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
            sizeof(sns_sam_gravity_enable_req_msg_v01));
         SNS_ASSERT(gravityEnMsgPtr != NULL);

         gravityEnMsgPtr->report_period = 0; // report synchronous to sampling rate
         gravityEnMsgPtr->sample_rate_valid = true;
         gravityEnMsgPtr->sample_rate = sampleRate;

         msgHdrPtr->body_len = sizeof(sns_sam_gravity_enable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, gravityEnMsgPtr);

         msgPtr = gravityEnMsgPtr;
         break;
      }

      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         sns_sam_filtered_mag_enable_req_msg_v01 *fmvEnMsgPtr =
         (sns_sam_filtered_mag_enable_req_msg_v01  *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
            sizeof(sns_sam_filtered_mag_enable_req_msg_v01));
         SNS_ASSERT(fmvEnMsgPtr != NULL);

         fmvEnMsgPtr->report_period = 0; // report synchronous to sampling rate
         fmvEnMsgPtr->sample_rate_valid = true;
         fmvEnMsgPtr->sample_rate = sampleRate;

         msgHdrPtr->body_len = sizeof(sns_sam_filtered_mag_enable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, fmvEnMsgPtr);

         msgPtr = fmvEnMsgPtr;
         break;
      }

      default:
         break;
      } /* end switch case for algos gravity vector depends on */
   }

   else if (algoInstPtr->serviceId == SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01)
   {
      sns_sam_game_rotation_vector_enable_req_msg_v01 *gameRVEnMsgPtr
          = (sns_sam_game_rotation_vector_enable_req_msg_v01 *)clientReqMsgPtr;

      sampleRate = gameRVEnMsgPtr->sample_rate_valid ?
                   gameRVEnMsgPtr->sample_rate :
                   ((game_rot_vec_config_s*)algoInstPtr->configData.memPtr)->sample_rate;

      // if sampling rate is slower than report rate, make it match
      if( gameRVEnMsgPtr->report_period > 0 )
      {
         sampleRate = sns_sam_get_adjusted_sample_rate(sampleRate,
                                                       gameRVEnMsgPtr->report_period);
      }

      switch (msgHdrPtr->svc_num)
      {
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         sns_sam_gravity_enable_req_msg_v01 *gravityEnMsgPtr =
         (sns_sam_gravity_enable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
            sizeof(sns_sam_gravity_enable_req_msg_v01));
         SNS_ASSERT(gravityEnMsgPtr != NULL);

         gravityEnMsgPtr->report_period = 0;  // synchronous reporting
         gravityEnMsgPtr->sample_rate_valid = true;
         gravityEnMsgPtr->sample_rate = sampleRate;

         msgHdrPtr->body_len = sizeof(sns_sam_gravity_enable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, gravityEnMsgPtr);

         msgPtr = gravityEnMsgPtr;
         break;
      }

      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         sns_sam_quat_enable_req_msg_v01 *quatEnMsgPtr =
         (sns_sam_quat_enable_req_msg_v01  *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
            sizeof(sns_sam_quat_enable_req_msg_v01));
         SNS_ASSERT(quatEnMsgPtr != NULL);

         quatEnMsgPtr->report_period = 0;  // synchronous reporting
         quatEnMsgPtr->sample_rate_valid = true;
         quatEnMsgPtr->sample_rate = sampleRate;

         msgHdrPtr->body_len = sizeof(sns_sam_quat_enable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, quatEnMsgPtr);

         msgPtr = quatEnMsgPtr;
         break;
      }

      default:
         break;
      } /* end switch case for algos gravity vector depends on */
   } /* End game rotation vector algo case */

   else if (algoInstPtr->serviceId == SNS_SAM_TILT_DETECTOR_SVC_ID_V01)
   {
      switch (msgHdrPtr->svc_num)
      {
         case SNS_SAM_AMD_SVC_ID_V01:
         {
            sns_sam_qmd_enable_req_msg_v01 *enableMsgPtr =
            (sns_sam_qmd_enable_req_msg_v01 *) sns_smr_msg_alloc(
               SNS_SAM_DBG_MOD,
               sizeof(sns_sam_qmd_enable_req_msg_v01));
            SNS_ASSERT(enableMsgPtr != NULL);

            enableMsgPtr->report_period = 0;
            enableMsgPtr->config_valid = 0;

            /* allow AMD in async mode to send events during suspend */
            enableMsgPtr->notify_suspend_valid = true;
            enableMsgPtr->notify_suspend.proc_type = SNS_PROC_SSC_V01;
            enableMsgPtr->notify_suspend.send_indications_during_suspend = true;

            msgHdrPtr->body_len = sizeof(sns_sam_qmd_enable_req_msg_v01);
            sns_smr_set_hdr(msgHdrPtr, enableMsgPtr);
            msgPtr = enableMsgPtr;
            break;
         }
         default:
            break;
      }
   } /* End Tilt Detector case */

   return msgPtr;
}

/*=========================================================================
  FUNCTION:  sns_sam_gen_algo_disable_msg
  =========================================================================*/
/*!
  @brief Generate the algorithm enable request message

  @param[i] algoInstId: algorithm instance id
  @param[i] msgHdrPtr: message header pointer

  @return Sensors error code
*/
/*=======================================================================*/
void *sns_sam_gen_algo_disable_msg(
   uint8_t instanceId,
   sns_smr_header_s* msgHdrPtr)
{
   void *msgPtr = NULL;

   SNS_SAM_PRINTF1(LOW,
                  "sns_sam_gen_algo_disable_msg: message service num %d",
                  msgHdrPtr->svc_num);

   switch (msgHdrPtr->svc_num)
   {
      case SNS_SAM_AMD_SVC_ID_V01:
      {
         sns_sam_qmd_disable_req_msg_v01 *disableMsgPtr =
         (sns_sam_qmd_disable_req_msg_v01 *) sns_smr_msg_alloc(
            SNS_SAM_DBG_MOD,
            sizeof(sns_sam_qmd_disable_req_msg_v01));
         SNS_ASSERT(disableMsgPtr != NULL);

         disableMsgPtr->instance_id = instanceId;

         msgHdrPtr->body_len = sizeof(sns_sam_qmd_disable_req_msg_v01);
         sns_smr_set_hdr(msgHdrPtr, disableMsgPtr);
         msgPtr = disableMsgPtr;
         break;
      }
      case SNS_SAM_PED_SVC_ID_V01:
      {
        sns_sam_ped_disable_req_msg_v01 *disableMsgPtr =
           (sns_sam_ped_disable_req_msg_v01 *)sns_smr_msg_alloc(
              SNS_SAM_DBG_MOD,
              sizeof(sns_sam_ped_disable_req_msg_v01));
        SNS_ASSERT(disableMsgPtr != NULL);

        disableMsgPtr->instance_id = instanceId;

        msgHdrPtr->body_len = sizeof(sns_sam_ped_disable_req_msg_v01);
        sns_smr_set_hdr(msgHdrPtr, disableMsgPtr);
        msgPtr = disableMsgPtr;
        break;
      }
      case SNS_SAM_CMC_SVC_ID_V01:
      {
        sns_sam_cmc_disable_req_msg_v01 *disableMsgPtr =
           (sns_sam_cmc_disable_req_msg_v01 *)sns_smr_msg_alloc(
              SNS_SAM_DBG_MOD,
              sizeof(sns_sam_cmc_disable_req_msg_v01));
        SNS_ASSERT(disableMsgPtr != NULL);

        disableMsgPtr->instance_id = instanceId;

        msgHdrPtr->body_len = sizeof(sns_sam_cmc_disable_req_msg_v01);
        sns_smr_set_hdr(msgHdrPtr, disableMsgPtr);
        msgPtr = disableMsgPtr;
        break;
      }
      case SNS_SAM_QUATERNION_SVC_ID_V01:
      {
         sns_sam_quat_disable_req_msg_v01 *quatDisMsgPtr =
          (sns_sam_quat_disable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
             sizeof(sns_sam_quat_disable_req_msg_v01));
         SNS_ASSERT(quatDisMsgPtr != NULL);

         quatDisMsgPtr->instance_id = instanceId;

         msgHdrPtr->body_len = sizeof(sns_sam_quat_disable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, quatDisMsgPtr);

         msgPtr = quatDisMsgPtr;
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         sns_sam_gravity_disable_req_msg_v01 *disableMsgPtr =
          (sns_sam_gravity_disable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
             sizeof(sns_sam_gravity_disable_req_msg_v01));
         SNS_ASSERT(disableMsgPtr != NULL);

         disableMsgPtr->instance_id = instanceId;

         msgHdrPtr->body_len = sizeof(sns_sam_gravity_disable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, disableMsgPtr);

         msgPtr = disableMsgPtr;
         break;
      }

      case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
      {
         sns_sam_filtered_mag_disable_req_msg_v01 *disableMsgPtr =
          (sns_sam_filtered_mag_disable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
             sizeof(sns_sam_filtered_mag_disable_req_msg_v01));
         SNS_ASSERT(disableMsgPtr != NULL);

         disableMsgPtr->instance_id = instanceId;

         msgHdrPtr->body_len = sizeof(sns_sam_filtered_mag_disable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, disableMsgPtr);

         msgPtr = disableMsgPtr;
         break;
      }

      case SNS_SAM_SMD_SVC_ID_V01:
      {
         sns_sam_smd_disable_req_msg_v01 *disableMsgPtr =
          (sns_sam_smd_disable_req_msg_v01 *)sns_smr_msg_alloc(SNS_SAM_DBG_MOD,
             sizeof(sns_sam_filtered_mag_disable_req_msg_v01));
         SNS_ASSERT(disableMsgPtr != NULL);

         disableMsgPtr->instance_id = instanceId;

         msgHdrPtr->body_len = sizeof(sns_sam_smd_disable_req_msg_v01);

         sns_smr_set_hdr(msgHdrPtr, disableMsgPtr);

         msgPtr = disableMsgPtr;
         break;
      }
      default:
        break;
   }
   return msgPtr;
}

/*=========================================================================
  FUNCTION:  sns_sam_update_algo_report_ts
  =========================================================================*/
/*!
  @brief Update the algorithm report timestamp

  @param[i] algoInstPtr: pointer to algorithm instance in database
  @param[i] clientId: client identifier

  @return None
*/
/*=======================================================================*/
void sns_sam_update_algo_report_ts(
   sns_sam_algo_inst_s* algoInstPtr,
   uint8_t clientId)
{
    switch (algoInstPtr->serviceId)
    {
       case SNS_SAM_SENSOR_THRESH_SVC_ID_V01:
           algoInstPtr->outputData.timestamp =
               ((threshold_output_s *) algoInstPtr->outputData.memPtr)->timestamp;
           break;

       case SNS_SAM_MODEM_SCN_SVC_ID_V01:
         algoInstPtr->outputData.timestamp =
            ((modem_scn_output_s *) algoInstPtr->outputData.memPtr)->timestamp;
        break;

       case SNS_SAM_PED_SVC_ID_V01:
          {
             uint8_t i;
             ped_output_s * outPtr =
                ((ped_output_s *) algoInstPtr->outputData.memPtr);
             algoInstPtr->outputData.timestamp = 0;
             for( i = 0; outPtr && (i < outPtr->num_active_clients); ++i )
             {
                if( outPtr->client_data[i].client_id == clientId )
                {
                   algoInstPtr->outputData.timestamp =
                      outPtr->client_data[i].timestamp;
                }
             }
           }
           break;

       case SNS_SAM_PAM_SVC_ID_V01:
         algoInstPtr->outputData.timestamp =
            ((pam_output_s *)(algoInstPtr->outputData.memPtr))->timestamp;
         break;

      case SNS_SAM_CMC_SVC_ID_V01:
         algoInstPtr->outputData.timestamp =
            ((cmc_output_s *)(algoInstPtr->outputData.memPtr))->timestamp;
         break;

      case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
         {
            algoInstPtr->outputData.timestamp = sns_em_get_timestamp();
         }
         break;

       case SNS_SAM_QUATERNION_SVC_ID_V01:
       {
          algoInstPtr->outputData.timestamp =
             ((gyro_quat_output_s *)(algoInstPtr->outputData.memPtr))->timestamp;
          break;
       }
       case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
       {
          algoInstPtr->outputData.timestamp =
             ((gravity_output_s *)(algoInstPtr->outputData.memPtr))->timestamp;
          break;
       }
       case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
       {
          algoInstPtr->outputData.timestamp =
             ((fmv_output_s *)(algoInstPtr->outputData.memPtr))->ts;
          break;
       }
       case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
       {
          algoInstPtr->outputData.timestamp =
             ((rotation_vector_output_s *)(algoInstPtr->outputData.memPtr))->timestamp;
          break;
       }
       case SNS_SAM_ORIENTATION_SVC_ID_V01:
       {
          algoInstPtr->outputData.timestamp =
             ((orientation_output_s *)(algoInstPtr->outputData.memPtr))->timestamp;
          break;
       }
       case SNS_SAM_SMD_SVC_ID_V01:
          algoInstPtr->outputData.timestamp =
             ((smd_output_s *) algoInstPtr->outputData.memPtr)->timestamp;
          break;
       case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
       {
          game_rot_vec_output_s *outPtr =
             ((game_rot_vec_output_s *)algoInstPtr->outputData.memPtr);
          algoInstPtr->outputData.timestamp = outPtr->timestamp;
          break;
       }
       case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
       {
          algoInstPtr->outputData.timestamp =
             ((tilt_output_s *) algoInstPtr->outputData.memPtr)->event_ts;
       }

       default:
          break;
    }
}

/*=========================================================================
  FUNCTION:  sns_sam_process_one_shot_algo_report
  =========================================================================*/
/*!
  @brief Process one-shot report from specified algorithm

  @param[i] clientReqPtr: pointer to the client request
  @param[i] algoRptPtr: pointer to algorithm report

  @return Sensors error code
*/
/*=======================================================================*/
void sns_sam_process_one_shot_algo_report(
   uint8_t clientReqId,
   const sns_sam_algo_rpt_s *algoRptPtr)
{
   sns_sam_algo_inst_s* algoInstPtr;
   sns_sam_client_req_s* clientReqPtr = sns_sam_get_client_req_handle(clientReqId);

   if(clientReqPtr == NULL)
   {
     return;
   }

   algoInstPtr = sns_sam_get_algo_inst_handle(clientReqPtr->algoInstId);
   if(algoInstPtr == NULL)
   {
      return;
   }

   switch (algoInstPtr->serviceId)
   {
   case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
      {
         distance_bound_output_s* output = (distance_bound_output_s*)(algoRptPtr->memPtr);

         // No report from DistanceBound yet
         if (algoRptPtr->timestamp == 0)
         {
            return;
         }

         //skip report if output has not been updated since last report
         if (algoRptPtr->timestamp == clientReqPtr->timestamp)
         {
            return;
         }
         distance_bound_client_output_s* client_output = &output->client_data;

         SNS_SAM_PRINTF3(LOW,
            "process_one_shot_algo_report for Distance Bound. ClientReqId:%d, one_shot_period_status:%d one_shot_period(in us):%d ",
            clientReqId, client_output->one_shot_period_status, client_output->one_shot_period);

         switch(client_output->one_shot_period_status)
         {
         case SNS_SAM_ONE_SHOT_PERIOD_REQ_NONE:
            {
               // No action needed
               break;
            }
         case SNS_SAM_ONE_SHOT_PERIOD_REQ_DISABLE:
            {
               // Disable the timer
               sns_sam_dereg_timer(clientReqId);
               break;
            }
         case SNS_SAM_ONE_SHOT_PERIOD_REQ_ENABLE:
            {
               // Deregister existing timer and Register again with new timeout period
               sns_sam_dereg_timer(clientReqId);

               if (sns_sam_reg_timer(clientReqId, sns_em_convert_usec_to_localtick(client_output->one_shot_period)) == SNS_SUCCESS)
               {
                  SNS_SAM_PRINTF2(LOW,
                                  "Registered one-shot timer for Distance Bound. one_shot_period(in us):%d at time:%d",
                                      client_output->one_shot_period, sns_em_get_timestamp());
               }
               else
               {
                  SNS_SAM_PRINTF1(ERROR,
                                  "Failed to register one-shot timer for Distance Bound. one_shot_period(in us):%d",
                                      client_output->one_shot_period);
               }
               break;
            }
          default:
            {
               SNS_SAM_PRINTF1(ERROR, "Unexpected one_shot_period_status:%d", client_output->one_shot_period_status);
               break;
            }
         }

         //update client report timestamp
         clientReqPtr->timestamp = algoRptPtr->timestamp;
      }
      break;
   default:
      {
         break;
      }
   }
}
/*=========================================================================
  FUNCTION:  sns_sam_handle_duty_cycle_state_change
  =========================================================================*/
/*!
  @brief Performs algorithm specific actions to duty cycle state change

  @param[i] algoInstId: index to algorithm instance in the database
  @param[i] algoInstPtr: pointer to algorithm instance

  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_handle_duty_cycle_state_change(
   uint8_t algoInstId,
   sns_sam_algo_inst_s *algoInstPtr)
{
   if (algoInstPtr == NULL)
   {
      return SNS_ERR_FAILED;
    }

   switch(algoInstPtr->serviceId)
   {
      case SNS_SAM_PED_SVC_ID_V01:
      {
         SNS_SAM_PRINTF3(HIGH,
            "sns_sam_handle_duty_cycle_state_change: dutycycle state %d, motion state %d, algo inst id %d",
            algoInstPtr->dutycycleStateOn, algoInstPtr->motion_state, algoInstId);

         if (algoInstPtr->dutycycleStateOn)
         {
            if(algoInstPtr->motion_state == SNS_SAM_MOTION_MOVE_V01)
            {
               ped_state_s * statePtr = (ped_state_s*)algoInstPtr->stateData.memPtr;
               statePtr->num_samples = 0;
               statePtr->total_sample_rate = 0.0f;
               statePtr->prev_input_timestamp = 0;

               sns_sam_req_sensor_data(algoInstId, SNS_SAM_INVALID_ID);
            }
         }
         else
         {
            sns_sam_stop_sensor_data(algoInstId);
         }

         break;
      }
      case SNS_SAM_PAM_SVC_ID_V01:
      {
         algoInstPtr->outputData.timestamp =
            ((pam_output_s *)(algoInstPtr->outputData.memPtr))->timestamp;
         break;
      }
      default:
         break;
   }

   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sam_validate_client_req_parameter
  =========================================================================*/
/*!
  @brief Check if the parameters passed along with client request are valid

  @param[i] msgId: type of client request
  @param[i] clientReqMsgPtr: pointer to client request message
  @param[i] algoSvcId: algorithm service Id number

  @return returns one of the following sensors error codes
          1) SNS_ERR_NOTSUPPORTED if the algorithm service does not support
          parameter validation or a msgId not supported in the function is
          passed
          2) SNS_SUCCESS if the parameters passed along with the client
          request are valid
          3) SNS_ERR_BAD_PARM if the parameters passed along with the client
          request are invalid
*/
/*=======================================================================*/
sns_err_code_e sns_sam_validate_client_req_parameter(
   uint8_t msgId,
   const void* clientReqMsgPtr,
   uint8_t algoSvcId)
{
   switch (msgId)
   {
      case SNS_SAM_ALGO_ENABLE_REQ:
      {
         sns_err_code_e err = sns_sam_validate_algo_report_period(algoSvcId, clientReqMsgPtr);
         if( err != SNS_SUCCESS )
         {
            return err;
         }

         if (SNS_SAM_TILT_DETECTOR_SVC_ID_V01 == algoSvcId)
         {
           sns_sam_tilt_detector_enable_req_msg_v01* tiltEnableReqMsg =
               (sns_sam_tilt_detector_enable_req_msg_v01*) clientReqMsgPtr;

           if (SNS_SAM_TILT_DETECTOR_MAX_TILT_ANGLE_THRESH_V01 < tiltEnableReqMsg->angle_thresh)
           {
             SNS_SAM_PRINTF3(ERROR,
                 "sam_valid_client_req: tilt angle threshold too large! msg_id %d requested_thresh %d max_thresh %d",
                 msgId, tiltEnableReqMsg->angle_thresh, SNS_SAM_TILT_DETECTOR_MAX_TILT_ANGLE_THRESH_V01);
             return SNS_ERR_BAD_PARM;
           }
         }

         return sns_sam_validate_algo_sample_rate(algoSvcId, clientReqMsgPtr);
      }
      case SNS_SAM_ALGO_UPDATE_REQ:
      {
         switch (algoSvcId)
         {
            case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
            {
               sns_sam_distance_bound_set_bound_req_msg_v01* setDistanceBoundMsg = (sns_sam_distance_bound_set_bound_req_msg_v01*) clientReqMsgPtr;
               if (setDistanceBoundMsg->distance_bound > distance_bound_process_get_max_distance_supported())
               {
                  return SNS_ERR_BAD_PARM; 
               }
               
               break;
            }
            default:
            {
               return SNS_ERR_NOTSUPPORTED;
            }
         }
         break;
      }
      default:
      {
         return SNS_ERR_NOTSUPPORTED;
      }
   }
   return SNS_SUCCESS;
}

/*=========================================================================
  FUNCTION:  sns_sam_update_smgr_req
  =========================================================================*/
/*!
  @brief    Updates parameters associated with a SMGR request.

  @param[i] algoSvcId:  algorithm service id
  @param[i] dataReqId:  index of data request entry in dbase
  @param[i] dataReqType:  SMGR request type

  @return   Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_update_smgr_req(
   uint8_t algoSvcId,
   uint8_t dataReqId,
   uint8_t dataReqType)
{
   sns_err_code_e err = SNS_ERR_FAILED;
   sns_sam_data_req_s * dataReqPtr = sns_sam_data_req_dbase_acc(dataReqId);

   if(dataReqPtr != NULL)
   {
      switch( dataReqType )
      {
         case SNS_SMGR_REPORT_REQ_V01:
         {
            dataReqPtr->reportRate =
                sns_sam_find_max_sample_rate(dataReqId);
            err = sns_sam_send_smgr_start_req( dataReqId, algoSvcId );
            break;
         }
         case SNS_SMGR_BUFFERING_REQ_V01:
         {
            uint8_t algoIndex = sns_sam_get_algo_index( algoSvcId );
            dataReqPtr->reportRate = 
                sns_sam_algo_dbase_acc(algoIndex)->defSensorReportRate;
            err = sns_sam_send_smgr_start_req( dataReqId, algoSvcId );
            break;
         }
         default:
            err = SNS_ERR_FAILED;
            break;
      }
      
      SNS_SAM_PRINTF3(HIGH, "mot_resp: algoSvcId=%d switched to req_type=%d (err=%d)",
                      algoSvcId, dataReqType, err);
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_update_input_type
  =========================================================================*/
/*!
  @brief    Switches between buffering and periodic input for algorithms

  @param[i] algoSvcId:  algorithm service id
  @param[i] dataReqId:  index of data request entry in dbase
  @param[i] prevOutputPtr:  previously generated output
  @param[i] currOutputPtr:  newly generated output

  @return   Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_update_input_type(
   uint8_t algoSvcId,
   uint8_t dataReqId,
   const void* prevOutputPtr,
   const void* currOutputPtr )
{
   sns_err_code_e err = SNS_ERR_FAILED;

   if( !prevOutputPtr || !currOutputPtr)
   {
      return err;
   }

   switch( algoSvcId )
   {
      case SNS_SAM_AMD_SVC_ID_V01:
      {
         // AMD uses periodic data to detect motion and
         // buffered data to detect stationarity
         qmd_output_s * prevOutput = (qmd_output_s *)prevOutputPtr;
         qmd_output_s * currOutput = (qmd_output_s *)currOutputPtr;
         if( currOutput->absolute_rest_state == SNS_SAM_MOTION_REST_V01 &&
             prevOutput->absolute_rest_state != SNS_SAM_MOTION_REST_V01 )
         {
            // Switch to using periodic report to detect motion
            err = sns_sam_update_smgr_req(algoSvcId, dataReqId,
                                          SNS_SMGR_REPORT_REQ_V01);
         }
         else if( currOutput->absolute_rest_state == SNS_SAM_MOTION_MOVE_V01 &&
                  prevOutput->absolute_rest_state != SNS_SAM_MOTION_MOVE_V01 )
         {
            // Switch to using buffered report to detect stationarity
            err = sns_sam_update_smgr_req(algoSvcId, dataReqId,
                                          SNS_SMGR_BUFFERING_REQ_V01);
         }
      }
      break;
   default:
      err = SNS_SUCCESS;
      break;
   }
   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_process_algo_update_req
  =========================================================================*/
/*!
  @brief Processes the algo update request

  @param[i] algoInstId: algo instance id
  @param[i] clientReqId: client request id
  @param[i] clientReqMsgPtr: pointer to the client request message

  @return   Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_process_algo_update_req(
   uint8_t algoInstId,
   uint8_t clientReqId,
   const void* clientReqMsgPtr)
{
   sns_err_code_e err = SNS_ERR_FAILED;

   void * statePtr;
   void * inputPtr;
   void * outputPtr;

   sns_sam_algo_inst_s* algoInstPtr = sns_sam_get_algo_inst_handle(algoInstId);
   if( algoInstPtr == NULL )
   {
      return SNS_ERR_BAD_PTR;
   }

   sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(algoInstPtr->serviceId);
   if ( algoPtr == NULL )
   {
      SNS_SAM_PRINTF1(ERROR,
       "Failed to get algorithm handle with algoInstId:%d", algoInstId);
      return SNS_ERR_BAD_PTR;
   }

   switch( algoInstPtr->serviceId )
   {
      case SNS_SAM_PED_SVC_ID_V01:
      {
         statePtr = algoInstPtr->stateData.memPtr;
         outputPtr = algoInstPtr->outputData.memPtr;
         if( algoPtr->algoApi.sns_sam_algo_reset_client_stats(clientReqId, statePtr, outputPtr) )
         {
            err = SNS_SUCCESS;
         }
         else
         {
            // Client not found
            err = SNS_ERR_BAD_PARM;
         }
         break;
      }
      case SNS_SAM_DISTANCE_BOUND_SVC_ID_V01:
      {
         int32_t i;
         distance_bound_input_s * distanceBoundInputPtr;
         sns_sam_distance_bound_set_bound_req_msg_v01* setDistanceBoundMsg;
         statePtr = algoInstPtr->stateData.memPtr;
         inputPtr = algoInstPtr->inputData.memPtr;
         outputPtr = algoInstPtr->outputData.memPtr;
         distanceBoundInputPtr = (distance_bound_input_s *) inputPtr;

         setDistanceBoundMsg = (sns_sam_distance_bound_set_bound_req_msg_v01*) clientReqMsgPtr;
         distanceBoundInputPtr->datatype = DISTANCE_BOUND_SET_DISTANCE_BOUND;
         distanceBoundInputPtr->data.distance_bound = setDistanceBoundMsg->distance_bound;
         distanceBoundInputPtr->data.session_key = setDistanceBoundMsg->session_key;

         #ifdef SNS_PCSIM
         distanceBoundInputPtr->timestamp = 0;
         #else
         distanceBoundInputPtr->timestamp = sns_em_get_timestamp();
         #endif
         algoPtr->algoApi.sns_sam_algo_update(statePtr, inputPtr, outputPtr);

         // TODO: Passing 0 is incorrect. It must be the clientId.
         sns_sam_update_algo_report_ts(algoInstPtr, 0);

         //send synchronous indication
         for (i = 0; i < SNS_SAM_MAX_CLIENT_REQS_PER_ALGO_INST; i++)
         {
            if (algoInstPtr->clientReqDbase[i] < SNS_SAM_MAX_CLIENT_REQS)
            {
               uint8_t clientReqId = algoInstPtr->clientReqDbase[i];
               sns_sam_client_req_s* clientReqPtr = sns_sam_get_client_req_handle(clientReqId);

               if (clientReqPtr != NULL && clientReqPtr->reportType == SNS_SAM_ONE_SHOT_REPORT)
               {
                  sns_sam_process_one_shot_algo_report(clientReqId, &(algoInstPtr->outputData));
               }
            }
         }

         err = SNS_SUCCESS;
         break;
      }
      case SNS_SAM_CMC_SVC_ID_V01:
      {
         uint8_t i = 0;
         sns_sam_cmc_update_reporting_req_msg_v01 *updateReqMsg = 
            (sns_sam_cmc_update_reporting_req_msg_v01*) clientReqMsgPtr;
         cmc_input_s *cmcInput = (cmc_input_s *)(algoInstPtr->inputData.memPtr);

         SNS_SAM_PRINTF3(HIGH,"CMC 2.0 UpdateReportingReq. report_ms_type:%d, report_motion_state:%d, report_event_type:%d",
                         updateReqMsg->report_ms_type, updateReqMsg->report_motion_state, updateReqMsg->report_event_type);

         inputPtr = algoInstPtr->inputData.memPtr;
         statePtr = algoInstPtr->stateData.memPtr;
         outputPtr = algoInstPtr->outputData.memPtr;

         cmcInput->datatype = CMC_UPDATE_REPORTING_INPUT;

         if (updateReqMsg->report_ms_type == SNS_SAM_CMC_MS_REPORT_TYPE_SINGLE_V01)
         {
            cmcInput->data.update_report_req.ms_event = updateReqMsg->report_motion_state;
            if (updateReqMsg->enable)
            {
               if (updateReqMsg->report_event_type == SNS_SAM_CMC_MS_EVENT_REPORT_TYPE_ENTER_V01)
               {
                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_ENABLE_ENTRY; 
               }
               else
               {
                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_ENABLE_EXIT;
               }
            }
            else 
            {
               if (updateReqMsg->report_event_type == SNS_SAM_CMC_MS_EVENT_REPORT_TYPE_ENTER_V01)
               {
                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_DISABLE_ENTRY; 
               }
               else
               {
                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_DISABLE_EXIT;
               }
            }
            algoPtr->algoApi.sns_sam_algo_update(statePtr, inputPtr, outputPtr);

            sns_sam_log_cmc_result( algoInstId, algoInstPtr);
         }
         else if (updateReqMsg->report_ms_type == SNS_SAM_CMC_MS_REPORT_TYPE_ALL_V01)
         {
            for (i = 0; i < SNS_SAM_CMC_MS_NUM_V01; i++)
            {
               cmcInput->data.update_report_req.ms_event = i;
               if (updateReqMsg->enable)
               {
                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_ENABLE_ENTRY; 
                  algoPtr->algoApi.sns_sam_algo_update(statePtr, inputPtr, outputPtr);
                  sns_sam_log_cmc_result( algoInstId, algoInstPtr);

                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_ENABLE_EXIT; 
                  algoPtr->algoApi.sns_sam_algo_update(statePtr, inputPtr, outputPtr);
                  sns_sam_log_cmc_result( algoInstId, algoInstPtr);
               }
               else 
               {
                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_DISABLE_ENTRY;
                  algoPtr->algoApi.sns_sam_algo_update(statePtr, inputPtr, outputPtr);
                  sns_sam_log_cmc_result( algoInstId, algoInstPtr);
                  
                  cmcInput->data.update_report_req.report_type = CMC_UPDATE_REPORT_DISABLE_EXIT; 
                  algoPtr->algoApi.sns_sam_algo_update(statePtr, inputPtr, outputPtr);
                  sns_sam_log_cmc_result( algoInstId, algoInstPtr);
               }
               
            }
         }
         err = SNS_SUCCESS;
         break;
      }
      default:
      {
         err = SNS_ERR_NOTSUPPORTED;
      }
   }

   // Log reset
   sns_sam_log_algo_result(algoInstId, algoInstPtr, clientReqId);

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_process_algo_batch_req
  =========================================================================*/
/*!
  @brief Processes the algo batch request

  @param[i] algoPtr: pointer to algorithm structure
  @param[i] clientReqId: client request id
  @param[i] clientReqMsgPtr: pointer to the client request message

  @return   Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_process_algo_batch_req(
   sns_sam_algo_s * algoPtr,
   uint8_t clientReqId,
   const void* clientReqMsgPtr)
{
   sns_err_code_e err = SNS_SUCCESS;
   bool wakeClient = false;

   if( !algoPtr )
   {
      SNS_SAM_PRINTF0(ERROR, "sns_sam_process_algo_batch_req: Invalid algo ptr");
      return SNS_ERR_BAD_PTR;
   }

   SNS_SAM_PRINTF2(HIGH,"SAM- Rxd batch request for svc %d, client id %d",algoPtr->serviceId,clientReqId);

   switch( algoPtr->serviceId )
   {
      case SNS_SAM_PED_SVC_ID_V01:
      {
         sns_sam_ped_batch_req_msg_v01 * batchReqMsgPtr =
             (sns_sam_ped_batch_req_msg_v01*)clientReqMsgPtr;

         if( batchReqMsgPtr->req_type_valid != true ||
             batchReqMsgPtr->req_type < SNS_BATCH_GET_MAX_FIFO_SIZE_V01 )
         {
            wakeClient = (batchReqMsgPtr->req_type_valid && batchReqMsgPtr->req_type);
            err = sns_sam_process_batch_req( clientReqId,
                                             batchReqMsgPtr->batch_period,
                                             algoPtr->max_batch_size,
                                             wakeClient);
         }
         break;
      }
      case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      {
         sns_sam_gravity_batch_req_msg_v01 * batchReqMsgPtr =
             (sns_sam_gravity_batch_req_msg_v01*)clientReqMsgPtr;

         if( batchReqMsgPtr->req_type_valid != true ||
             batchReqMsgPtr->req_type < SNS_BATCH_GET_MAX_FIFO_SIZE_V01 )
         {
            wakeClient = (batchReqMsgPtr->req_type_valid && batchReqMsgPtr->req_type);
            err = sns_sam_process_batch_req( clientReqId,
                                             batchReqMsgPtr->batch_period,
                                             algoPtr->max_batch_size,
                                             wakeClient);
         }
         break;
      }
      case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      {
         sns_sam_game_rotation_vector_batch_req_msg_v01 * batchReqMsgPtr =
            (sns_sam_game_rotation_vector_batch_req_msg_v01*)clientReqMsgPtr;

         if( batchReqMsgPtr->req_type_valid != true ||
             batchReqMsgPtr->req_type < SNS_BATCH_GET_MAX_FIFO_SIZE_V01 )
         {
            wakeClient = (batchReqMsgPtr->req_type_valid && batchReqMsgPtr->req_type);
            err = sns_sam_process_batch_req( clientReqId,
                                             batchReqMsgPtr->batch_period,
                                             algoPtr->max_batch_size,
                                             wakeClient);
         }
         break;
      }
      case SNS_SAM_ORIENTATION_SVC_ID_V01:
      {
         sns_sam_orientation_batch_req_msg_v01 * batchReqMsgPtr =
             (sns_sam_orientation_batch_req_msg_v01*)clientReqMsgPtr;

         if( batchReqMsgPtr->req_type_valid != true ||
             batchReqMsgPtr->req_type < SNS_BATCH_GET_MAX_FIFO_SIZE_V01 )
         {
            wakeClient = (batchReqMsgPtr->req_type_valid && batchReqMsgPtr->req_type);
            err = sns_sam_process_batch_req( clientReqId,
                                             batchReqMsgPtr->batch_period,
                                             algoPtr->max_batch_size,
                                             wakeClient);
         }
         break;
      }
      case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      {
         sns_sam_rotation_vector_batch_req_msg_v01 * batchReqMsgPtr =
             (sns_sam_rotation_vector_batch_req_msg_v01*)clientReqMsgPtr;

         if( batchReqMsgPtr->req_type_valid != true ||
             batchReqMsgPtr->req_type < SNS_BATCH_GET_MAX_FIFO_SIZE_V01 )
         {
            wakeClient = (batchReqMsgPtr->req_type_valid && batchReqMsgPtr->req_type);
            err = sns_sam_process_batch_req( clientReqId,
                                             batchReqMsgPtr->batch_period,
                                             algoPtr->max_batch_size,
                                             wakeClient);
         }
         break;
      }
      case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      {
        sns_sam_tilt_detector_batch_req_msg_v01 * batchReqMsgPtr =
             (sns_sam_tilt_detector_batch_req_msg_v01*)clientReqMsgPtr;

         if( batchReqMsgPtr->req_type_valid != true ||
             batchReqMsgPtr->req_type < SNS_BATCH_GET_MAX_FIFO_SIZE_V01 )
         {
            wakeClient = (batchReqMsgPtr->req_type_valid && batchReqMsgPtr->req_type);
            err = sns_sam_process_batch_req( clientReqId,
                                             batchReqMsgPtr->batch_period,
                                             algoPtr->max_batch_size,
                                             wakeClient);
         }
         break;
      }
      case SNS_SAM_CMC_SVC_ID_V01:
      {
         sns_sam_cmc_batch_req_msg_v01 * batchReqMsgPtr =
             (sns_sam_cmc_batch_req_msg_v01*)clientReqMsgPtr;

         if( batchReqMsgPtr->req_type_valid != true ||
             batchReqMsgPtr->req_type < SNS_BATCH_GET_MAX_FIFO_SIZE_V01 )
         {
            wakeClient = (batchReqMsgPtr->req_type_valid && batchReqMsgPtr->req_type);
            err = sns_sam_process_batch_req( clientReqId,
                                             batchReqMsgPtr->batch_period,
                                             algoPtr->max_batch_size,
                                             wakeClient);
         }
         break;
      }
      default:
      {
         err = SNS_ERR_NOTSUPPORTED;
         break;
      }
   }

   return err;
}

/*=========================================================================
  FUNCTION:  sns_sam_process_algo_upd_batch_period_req
  =========================================================================*/
/*!
  @brief Processes the algo update batch period request

  @param[i] algoSvcId: algo service id
  @param[i] clientReqId: client request id
  @param[i] clientReqMsgPtr: pointer to the client request message

  @return   Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_sam_process_algo_upd_batch_period_req(
   uint8_t algoSvcId,
   uint8_t clientReqId,
   const void* clientReqMsgPtr)
{
   sns_err_code_e err = SNS_ERR_NOTSUPPORTED;
   int32_t activeBatchPeriod = -1;

   SNS_SAM_PRINTF2(HIGH,"SAM:Rxd upd batch period request for svc %d, client id %d",algoSvcId,clientReqId);

   switch( algoSvcId )
   {
   case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
      activeBatchPeriod = ((sns_sam_game_rv_update_batch_period_req_msg_v01*)clientReqMsgPtr)->active_batch_period;
      break;
   case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
      activeBatchPeriod = ((sns_sam_grav_update_batch_period_req_msg_v01*)clientReqMsgPtr)->active_batch_period;
      break;
   case SNS_SAM_ORIENTATION_SVC_ID_V01:
      activeBatchPeriod = ((sns_sam_orient_update_batch_period_req_msg_v01*)clientReqMsgPtr)->active_batch_period;
      break;
   case SNS_SAM_PED_SVC_ID_V01:
      activeBatchPeriod = ((sns_sam_ped_update_batch_period_req_msg_v01*)clientReqMsgPtr)->active_batch_period;
      break;
   case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
      activeBatchPeriod = ((sns_sam_rot_vec_update_batch_period_req_msg_v01*)clientReqMsgPtr)->active_batch_period;
      break;
   case SNS_SAM_TILT_DETECTOR_SVC_ID_V01:
      activeBatchPeriod = ((sns_sam_tilt_detector_update_batch_period_req_msg_v01*)clientReqMsgPtr)->active_batch_period;
   case SNS_SAM_CMC_SVC_ID_V01:
      activeBatchPeriod = ((sns_sam_cmc_update_batch_period_req_msg_v01*)clientReqMsgPtr)->active_batch_period;
      break;
   default:
      break;
   }

   if (activeBatchPeriod >= 0)
   {
      err = sns_sam_proc_upd_batch_period_req(clientReqId, activeBatchPeriod);
   }
   return err;
}


/*=========================================================================
  FUNCTION:  sns_sam_refresh_algo_output
  =========================================================================*/
/*!
  @brief Refresh the algorithm output. No new inputs are provided.

  @param[i] algoInstId: algo instance id
  @param[i] clientReqId: client request id

  @return none
*/
/*=======================================================================*/
void sns_sam_refresh_algo_output(
   uint8_t algoInstId,
   uint8_t clientReqId)
{
   void * statePtr;
   void * inputPtr;
   void * outputPtr;
   sns_sam_algo_inst_s* algoInstPtr = sns_sam_get_algo_inst_handle(algoInstId);
   /* DistanceBound algorithm's output needs to be refreshed. The output has a field - distance_accumulated - that needs to be updated.
      The distance_accumulated needs to be recalculated based on the remaining time left in a timer that was started by the DistanceBound. 
      The (distance_bound - remainingtime*previous_speed) gives the distance_accumulated value. All these calculations are done in the algorithm
      during this refresh call. */
   if (algoInstPtr != NULL && algoInstPtr->serviceId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      sns_sam_algo_s* algoPtr = sns_sam_get_algo_handle(algoInstPtr->serviceId);
      if (algoPtr != NULL)
      {
         statePtr = algoInstPtr->stateData.memPtr;
         inputPtr = algoInstPtr->inputData.memPtr;
         outputPtr = algoInstPtr->outputData.memPtr;
         distance_bound_input_s* distanceBoundInputPtr = (distance_bound_input_s *) inputPtr;

         distanceBoundInputPtr->datatype = DISTANCE_BOUND_REFRESH_OUTPUT;
#ifdef SNS_PCSIM
         distanceBoundInputPtr->timestamp = 0;
#else
         distanceBoundInputPtr->timestamp = sns_em_get_timestamp();
#endif
         distanceBoundInputPtr->data.client_req_id = clientReqId;

         algoPtr->algoApi.sns_sam_algo_update(statePtr, inputPtr, outputPtr);
      }
   }
}


/*=========================================================================
  FUNCTION:  sns_sam_on_change_report_requirement
  =========================================================================*/
/*!
  @brief Get on change reporting requirement for algo service

  @param[i] algoSvcId: algorithm service Id

  @return true: report only when output has changed since last report
          false: report even if output is unchanged
*/
/*=======================================================================*/
bool sns_sam_on_change_report_requirement(uint8_t algoSvcId)
{
   if (algoSvcId == SNS_SAM_AMD_SVC_ID_V01 ||
       algoSvcId == SNS_SAM_RMD_SVC_ID_V01 ||
       algoSvcId == SNS_SAM_VMD_SVC_ID_V01 ||
       algoSvcId == SNS_SAM_PED_SVC_ID_V01 ||
       algoSvcId == SNS_SAM_DISTANCE_BOUND_SVC_ID_V01)
   {
      /*Generate report indication even if output is unchanged*/
      return false;
   }
   else
   {
      /*Generate report only when output has been updated*/
      return true;
   }
}
/*=========================================================================
  FUNCTION:  sns_sam_get_all_sensor_uuids
  =========================================================================*/
/*!
  @brief Get UUIDs of detected sensors from registry

  @param[i] sigEventPtr: pointer to the signal group flag
  @param[o] uuids: UUIDs of all detected sensors

  @return none
*/
/*=======================================================================*/
void sns_sam_get_all_sensor_uuids(
   OS_FLAG_GRP *sigEventPtr,
   sns_sam_sensor_uuid_s uuids[SNS_SAM_NUM_SENSORS])
{
   // No need to read UUIDs on SSC for mag cal registration
   UNREFERENCED_PARAMETER(sigEventPtr);
   UNREFERENCED_PARAMETER(uuids);
}

/*=========================================================================
 FUNCTION:  sns_sam_detect_smgr_buffering
  =========================================================================*/
/*!
  @brief  Detects support for Buffering in Sensor Manager.

  @return  true, since SMGR supports buffering
*/
/*=======================================================================*/
bool sns_sam_detect_smgr_buffering(
   OS_FLAG_GRP *sigEventPtr)
{
   UNREFERENCED_PARAMETER(sigEventPtr);
   return true;
}
