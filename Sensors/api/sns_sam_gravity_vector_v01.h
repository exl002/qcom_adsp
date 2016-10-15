#ifndef SNS_SAM_GRAVITY_VECTOR_SVC_SERVICE_01_H
#define SNS_SAM_GRAVITY_VECTOR_SVC_SERVICE_01_H
/**
  @file sns_sam_gravity_vector_v01.h

  @brief This is the public header file which defines the SNS_SAM_GRAVITY_VECTOR_SVC service Data structures.

  This header file defines the types and structures that were defined in
  SNS_SAM_GRAVITY_VECTOR_SVC. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set 
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc. All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.



  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/api/sns_sam_gravity_vector_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====* 
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.6 
   It was generated on: Fri Dec 20 2013 (Spin 0)
   From IDL File: sns_sam_gravity_vector_v01.idl */

/** @defgroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "sns_sam_common_v01.h"
#include "sns_common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SNS_SAM_GRAVITY_VECTOR_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SNS_SAM_GRAVITY_VECTOR_SVC_V01_IDL_MINOR_VERS 0x06
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SNS_SAM_GRAVITY_VECTOR_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SNS_SAM_GRAVITY_VECTOR_SVC_V01_MAX_MESSAGE_ID 0x0024
/**
    @}
  */


/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_consts 
    @{ 
  */
#define SNS_SENSOR_ACCURACY_UNRELIABLE_V01 0
#define SNS_SENSOR_ACCURACY_LOW_V01 1
#define SNS_SENSOR_ACCURACY_MEDIUM_V01 2
#define SNS_SENSOR_ACCURACY_HIGH_V01 3

/**  Max number of reports in a batch indication -
     set based on max payload size supported by transport framework */
#define SNS_SAM_GRAVITY_MAX_REPORTS_IN_BATCH_V01 57
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_enums
    @{
  */
typedef enum {
  SNS_SAM_GRAVITY_DEV_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SNS_SAM_GRAVITY_DEV_STATE_UNKNOWN_V01 = 0, 
  SNS_SAM_GRAVITY_DEV_ABS_REST_V01 = 1, 
  SNS_SAM_GRAVITY_DEV_REL_REST_V01 = 2, 
  SNS_SAM_GRAVITY_DEV_MOTION_V01 = 3, 
  SNS_SAM_GRAVITY_DEV_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}sns_sam_gravity_dev_e_v01;
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_aggregates
    @{
  */
typedef struct {

  float gravity[3];
  /**<   gravity vector along axis x/y/z, unit m/s/s, SAE coordinate frame */

  float lin_accel[3];
  /**<   linear acceleration along axis x/y/z,unit m/s/s, SAE coordinate frame */

  uint8_t accuracy;
  /**<   sensor accuracy
       SNS_SENSOR_ACCURACY_UNRELIABLE = 0,
       SNS_SENSOR_ACCURACY_LOW        = 1,
       SNS_SENSOR_ACCURACY_MEDIUM     = 2,
       SNS_SENSOR_ACCURACY_HIGH       = 3 */
}sns_sam_gravity_result_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Request Message; This command enables a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  uint32_t report_period;
  /**<   unit of seconds, Q16; determines algorithm output report rate
      specify 0 to report at sampling rate
  */

  /* Optional */
  uint8_t sample_rate_valid;  /**< Must be set to true if sample_rate is being passed */
  uint32_t sample_rate;
  /**<   sample rate in Hz, Q16;
      if sample rate is less than report rate, it is set to report rate
  */

  /* Optional */
  uint8_t notify_suspend_valid;  /**< Must be set to true if notify_suspend is being passed */
  sns_suspend_notification_s_v01 notify_suspend;
  /**<   Identifies if indications for this request should be sent
       when the processor is in suspend state.

       If this field is not specified, default value will be set to
       notify_suspend->proc_type                  = SNS_PROC_APPS
       notify_suspend->send_indications_during_suspend  = false

       This field does not have any bearing on error indication
       messages, which will be sent even during suspend.
    */
}sns_sam_gravity_enable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Response Message; This command enables a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<  
    Instance ID is assigned by SAM.
    The client shall use this instance ID for future messages associated with
    this algorithm instance.
  */
}sns_sam_gravity_enable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Request Message; This command disables a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance.  */
}sns_sam_gravity_disable_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Response Message; This command disables a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance. */
}sns_sam_gravity_disable_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing Gravity algorithm output */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance. */

  /* Mandatory */
  uint32_t timestamp;
  /**<   Timestamp of input used to generate the algorithm output */

  /* Mandatory */
  sns_sam_gravity_result_s_v01 result;
  /**<   Output of the Gravity algorithm instance */

  /* Optional */
  uint8_t device_motion_state_valid;  /**< Must be set to true if device_motion_state is being passed */
  sns_sam_gravity_dev_e_v01 device_motion_state;
  /**<   The device's current motion-state, as calculated in Gravity algo
       GRAVITY_DEV_STATE_UNKNOWN = 0,
       GRAVITY_DEV_ABS_REST      = 1,
       GRAVITY_DEV_REL_REST      = 2,
       GRAVITY_DEV_MOTION        = 3 */
}sns_sam_gravity_report_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Request Message; This command fetches latest report output of a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance.  */
}sns_sam_gravity_get_report_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Response Message; This command fetches latest report output of a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance. */

  /* Optional */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp of input used to generate the algorithm output */

  /* Optional */
  uint8_t result_valid;  /**< Must be set to true if result is being passed */
  sns_sam_gravity_result_s_v01 result;
  /**<   Output of the Gravity algorithm instance */

  /* Optional */
  uint8_t device_motion_state_valid;  /**< Must be set to true if device_motion_state is being passed */
  sns_sam_gravity_dev_e_v01 device_motion_state;
  /**<   The device's current motion-state, as calculated in Gravity algo
       GRAVITY_DEV_STATE_UNKNOWN = 0
       GRAVITY_DEV_ABS_REST      = 1
       GRAVITY_DEV_REL_REST      = 2
       GRAVITY_DEV_MOTION        = 3 */
}sns_sam_gravity_get_report_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Indication Message; Asynchronous error indication for a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t error;
  /**<   Sensor1 error code */

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance. */
}sns_sam_gravity_error_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Request Message; This command handles batch mode for a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance.  */

  /* Mandatory */
  int32_t batch_period;
  /**<   Specifies interval over which reports are to be batched in seconds, Q16.
       P = 0 to disable batching.
       P > 0 to enable batching.
    */

  /* Optional */
  uint8_t req_type_valid;  /**< Must be set to true if req_type is being passed */
  sns_batch_req_type_e_v01 req_type;
  /**<   Optional request type
       0 � Do not wake client from suspend when buffer is full.
       1 � Wake client from suspend when buffer is full.
       2 � Use to get max buffer depth. Does not enable/disable batching.
           instance_id and batch_period are ignored for this request type.
       Defaults to 0.
    */
}sns_sam_gravity_batch_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Response Message; This command handles batch mode for a Gravity algorithm. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Algorithm instance ID maintained/assigned by SAM */

  /* Optional */
  uint8_t max_batch_size_valid;  /**< Must be set to true if max_batch_size is being passed */
  uint32_t max_batch_size;
  /**<   Max supported batch size */

  /* Optional */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp when the batch request was processed in SSC ticks */
}sns_sam_gravity_batch_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_aggregates
    @{
  */
typedef struct {

  sns_sam_gravity_result_s_v01 result;
  /**<   Gravity algorithm output report */

  sns_sam_gravity_dev_e_v01 device_motion_state;
  /**<   The device's current motion-states, as calculated in Gravity algo
       GRAVITY_DEV_STATE_UNKNOWN = 0,
       GRAVITY_DEV_ABS_REST      = 1,
       GRAVITY_DEV_REL_REST      = 2,
       GRAVITY_DEV_MOTION        = 3 */
}sns_sam_gravity_report_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Indication Message; Report containing a batch of algorithm outputs */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance id identifies the algorithm instance */

  /* Mandatory */
  uint32_t first_report_timestamp;
  /**<   Timestamp of input used to generate the first algorithm output in batch, in SSC ticks */

  /* Mandatory */
  uint32_t timestamp_offsets_len;  /**< Must be set to # of elements in timestamp_offsets */
  uint16_t timestamp_offsets[SNS_SAM_GRAVITY_MAX_REPORTS_IN_BATCH_V01];
  /**<   Offsets from timestamp of previous report in batch */

  /* Mandatory */
  uint32_t reports_len;  /**< Must be set to # of elements in reports */
  sns_sam_gravity_report_s_v01 reports[SNS_SAM_GRAVITY_MAX_REPORTS_IN_BATCH_V01];
  /**<   Gravity algorithm output reports */

  /* Optional */
  uint8_t ind_type_valid;  /**< Must be set to true if ind_type is being passed */
  uint8_t ind_type;
  /**<   Optional batch indication type
       SNS_BATCH_ONLY_IND - Standalone batch indication. Not part of a back to back indication stream
       SNS_BATCH_FIRST_IND - First indication in stream of back to back indications
       SNS_BATCH_INTERMEDIATE_IND - Intermediate indication in stream of back to back indications
       SNS_BATCH_LAST_IND - Last indication in stream of back to back indications
    */
}sns_sam_gravity_batch_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Request Message; This command updates active batch period for a Gravity algorithm
           when batching is ongoing. */
typedef struct {

  /* Mandatory */
  uint8_t instance_id;
  /**<   Instance ID identifies the algorithm instance.  */

  /* Mandatory */
  int32_t active_batch_period;
  /**<   Specifies new interval (in seconds, Q16) over which reports are to be
       batched when the client processor is awake. Only takes effect if
       batching is ongoing.
       P > 0 to to override active batch period set in batch enable request.
       P = 0 to switch to active batch period set in batch enable request.
    */
}sns_sam_grav_update_batch_period_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_messages
    @{
  */
/** Response Message; This command updates active batch period for a Gravity algorithm
           when batching is ongoing. */
typedef struct {

  /* Mandatory */
  sns_common_resp_s_v01 resp;

  /* Optional */
  uint8_t instance_id_valid;  /**< Must be set to true if instance_id is being passed */
  uint8_t instance_id;
  /**<   Algorithm instance ID maintained/assigned by SAM */

  /* Optional */
  uint8_t timestamp_valid;  /**< Must be set to true if timestamp is being passed */
  uint32_t timestamp;
  /**<   Timestamp when the batch request was processed in SSC ticks */
}sns_sam_grav_update_batch_period_resp_msg_v01;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup SNS_SAM_GRAVITY_VECTOR_SVC_qmi_msg_ids
    @{
  */
#define SNS_SAM_GRAVITY_CANCEL_REQ_V01 0x0000
#define SNS_SAM_GRAVITY_CANCEL_RESP_V01 0x0000
#define SNS_SAM_GRAVITY_VERSION_REQ_V01 0x0001
#define SNS_SAM_GRAVITY_VERSION_RESP_V01 0x0001
#define SNS_SAM_GRAVITY_ENABLE_REQ_V01 0x0002
#define SNS_SAM_GRAVITY_ENABLE_RESP_V01 0x0002
#define SNS_SAM_GRAVITY_DISABLE_REQ_V01 0x0003
#define SNS_SAM_GRAVITY_DISABLE_RESP_V01 0x0003
#define SNS_SAM_GRAVITY_GET_REPORT_REQ_V01 0x0004
#define SNS_SAM_GRAVITY_GET_REPORT_RESP_V01 0x0004
#define SNS_SAM_GRAVITY_REPORT_IND_V01 0x0005
#define SNS_SAM_GRAVITY_ERROR_IND_V01 0x0006
#define SNS_SAM_GRAVITY_BATCH_REQ_V01 0x0021
#define SNS_SAM_GRAVITY_BATCH_RESP_V01 0x0021
#define SNS_SAM_GRAVITY_BATCH_IND_V01 0x0022
#define SNS_SAM_GRAV_UPDATE_BATCH_PERIOD_REQ_V01 0x0023
#define SNS_SAM_GRAV_UPDATE_BATCH_PERIOD_RESP_V01 0x0023
#define SNS_SAM_GRAV_GET_ATTRIBUTES_REQ_V01 0x0024
#define SNS_SAM_GRAV_GET_ATTRIBUTES_RESP_V01 0x0024
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor 
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro SNS_SAM_GRAVITY_VECTOR_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type SNS_SAM_GRAVITY_VECTOR_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );
 
/** This macro should be used to get the service object */ 
#define SNS_SAM_GRAVITY_VECTOR_SVC_get_service_object_v01( ) \
          SNS_SAM_GRAVITY_VECTOR_SVC_get_service_object_internal_v01( \
            SNS_SAM_GRAVITY_VECTOR_SVC_V01_IDL_MAJOR_VERS, SNS_SAM_GRAVITY_VECTOR_SVC_V01_IDL_MINOR_VERS, \
            SNS_SAM_GRAVITY_VECTOR_SVC_V01_IDL_TOOL_VERS )
/** 
    @} 
  */


#ifdef __cplusplus
}
#endif
#endif

