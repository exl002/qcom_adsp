/*=============================================================================
  @file sns_smgr_task.c

  This file contains task of SMGR, message interface, report generator,
  and handling of messages other than sensor request.

*******************************************************************************
* Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_task.c#4 $ */
/* $DateTime: 2014/07/29 14:14:02 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-07  hw   Add the storage of sensor state back after selftest check
  2014-07-08  ks   Added mag anomaly detector
  2014-05-29  hw   Save sensor state to sensor_test_info in the beginning of selftest
  2014-03-24  tc   Added SMGR_RESTRICTED_CANCEL support
  2013-03-20  pn   Suppresses initial invalid samples in reports
  2014-03-19  sd   wait correct off to idle time before selftest
  2014-03-14  pn   Prevents unnecessary attempt at sending b2b Periodic reports
  2014-02-25  pn   Periodic indications for event sensors always use latest sample only
  2014-02-13  tc   Added the Device Access Framework and the SMGR Restricted QMI service
  2014-02-04  tc   Added support for Accel 2 through 5, RGB 2, and SAR 2
  2014-01-27  hw   Add QDSS logging when smgr timer callback is called
  2014-01-12  jms  Merged discrete with unified fifo support
  2014-01-07  pn   - Retries sending indication when QMI connection is busy
                   - Pauses when sending large number of back-to-back indications
                   - Adjusts estimated available sample count if in FIFO/DRI mode
  2013-12-18  pn   No longer adjusts Buffering report schedule when failing to 
                   generate indication
  2013-12-18  pn   When starting self-test only turns on powerrail and waits if necessary
  2013-12-16  rt   Fixed memory leak issues on QMI client errors
  2013-12-18  MW   Added support for Hall Effect sensor  
  2013-12-12  pn   Initial invalid samples are now sent for Periodic reports
  2013-12-05  rt   Added fix to not invoke self test if device init has failed
  2013-12-10  pn   Report cleanup is done in SMGR context
  2013-12-04  pk   SMGR report cleanup on disconnect
  2013-11-26  pn   Only includes in Periodic indications newly processed samples
  2013-11-24  pn   Sensors reset at APP_WAKE is triggered rather than invoked directly.
  2013-10-31  hw   Add QDSS SW events into SMGR
  2013-10-23  MW   Added support for SAR sensor  
  2013-10-24  pn   Excluded initial invalid samples from Buffering indications
  2013-10-24  pn   Reduced Query delay
  2013-10-24  pn   Fixed Period reports involving interpolated samples from FIFO data
  2013-10-24  pn   Simplified smgr_init_item_depot_iter()
  2013-10-20  sc   Fixed a bug to handle more than 256 samples in buffer
  2013-10-18  pn   Added Batch indicator support for Buffering reports
  2013-10-16  pn   Resets all sensors when receiving APP_WAKE signal if in idle mode
  2013-10-03  pk   Notify SAM when SMGR sends batch indications to WuFF client
  2013-10-03  pn   Buffering and Periodic report generation changes
  2013-09-12  MW   Added support for RGB sensor type.
  2013-09-16  pn   Prevents adding to ready queue reports being initialized
  2013-09-10  pn   Reschedules next indication when sending an opportunistic indication
  2013-09-12  DC   Added humidity sensor name
  2013-08-12  ps   Eliminate compiler warning when ADSP_STANDALONE is defined
  2013-08-26  pn   Updated sensor short name list
  2013-08-22  pn   Fixed bug that limited sample count to 255
  2013-08-21  pn   Restored sns_smgr_flush_reports_to_same_processor() signature
  2013-08-19  pn   Updated sns_smgr_flush_reports_to_same_processor()
  2013-08-09  pn   Updated single_sensor_info processing
  2013-08-09  hw   Fix the Klocwork errors
  2013-07-31  vh   Eliminated Klocwork warnings
  2013-07-26  vh   Eliminated compiler warnings
  2013-08-01  lka  Added safeguard to prevent processing of spurious interrupt events.
  2013-07-15  sc   Fixed the condition for filling sensor info
  2013-07-14  lka  Moved sensor bias application to sns_smgr_proc_calibration() function.
  2013-07-11  pn   - Invalid selftest need not reset sensor
                   - AP sleep/wake signal processing updated
  2013-07-10  asj  Code cleanup, typo fixes
  2013-07-04  pn   Included buffer depth in single_sensor_info response
  2013-07-03  sd   Added check of gyro when set bias
  2013-07-01  hw   Add sampling latency measurement support
  2013-07-01  sd   Fixed MPU6515 doesn't have right range info.
  2013-06-23  lka  Enhanced SSI support: multiple groups and devices.
  2013-06-18  pn   Added support for back-to-back Buffering indications
  2013-06-15  lka  Deleted hack for removing SMD (one-shot) client.
  2013-06-12  agk  Remove voting for vdd resource to disable Standalone Power Collapse
  2013-06-11  ae   Added QDSP SIM playback support
  2013-06-06  pn   Report indications suspended/resumed on AP state change
  2013-06-03  lka  Added support for setting gyro bias in device driver.
  2013-06-03  sd   Added SMGR support of SMD as one shot event  sensor
  2013-06-03  pn   Report indications suspended/resumed on AP state change
  2013-06-02  pn   Added support for back-to-back Buffering indications
  2013-05-22  lka  Added support for direct sensor device access. Re-factored code.
  2013-05-09  br   Inserted validity checking in single_test_req_msg()
  2013-04-28  vh   Sensor state validation before calling the driver
  2013-04-25  pn   Fixed timing issues occured when sample quality transitioned
                   between interpolated and/or filtered and current sample
  2013-04-17  dc   Wait on SMGR_FLAGS_HW_INIT_B flag to be set before doing dd init.
  2013-04-11  dc   Read GPIO information from registry and use them for hw init.
  2013-04-11  pn   Reverted NULL pointer access fix in smgr_send_query_indication()
  2013-04-09  pn   Changed effective_rate_hz to q16
  2013-04-06  ag   Remove APDS DRI workaround
  2013-04-05  pn   Fixed wrong message type in msg_header in sns_smgr_q_put()
  2013-04-05  pn   Fixed NULL pointer access in smgr_send_query_indication()
  2013-04-05  pn   Fixed memory leak when processing Power messages
  2013-04-04  pn   Uses correct array index in sns_smgr_single_sensor_info_fill()
  2013-04-01  pk   Allow SMGR to initialize if SSI sensor dep reg group read fails
  2013-03-22  pn   Fixed issues of T0 and/or T1 in Query request not properly
                   interpolated in some cases, or unnecessarily interpolated
  2013-03-21  pn   Logs timestamps as unsigned values.
  2013-03-14  pn   Properly handles Query requests with time periods covering
                   sensor warm-up duration
  2013-03-11  vh   Added NULL condition check in QMI callback functions' memory allocation
  2013-03-11  pn   Properly schedules Buffering reports to prevent reporting at
                   lower rate than requested
  2013-02-07  dc   Filter indications for periodic and buffering reports when
                   kernel has suspended and when the client has opted not to
                   receive indications in kernel suspend state.
  2013-02-15  pn   Query indication now reports correct sampling rate.
  2013-02-13  vh   Added temporary variable in sns_smgr_proc_single_test_req_msg()
                   to remove segmentation fault.
  2013-02-13  pn   Default data type for Sensor Status request is PRIMARY
  2013-02-07  pn   Changed how next Buffering report is scheduled
  2013-02-06  hw   Add sensors timeline profiling support
  2013-02-05  pn   Fixed issue of missing samples in Query indications
  2013-01-18  ag
              pn   APDS DRI workaround
  2013-01-18  sd   corrected sensor short name
  2013-01-17  sd   do not send status indictaion when sensor fails after sending reply
  2013-01-16  sd   updated sns init flag when initialization done,
                   do not process smgr message until smgr init is done
                   when checking pending msg also including registry response
                   set name length in get all sensor info
  2013-01-03  jhh  Restored powerrail control for SNS_SMGR_LDO9_FLAG
  2012-12-26  pn   Fixed issues of large timing gaps
  2012-12-19  pn   Prevents updating interpolator with old samples
  2012-12-14  pn   Added timer for sensor events processing
  2012-12-10  ag   Initialize ext_signal to NULL, remove macro OLD_QCCI
  2012-12-08  pn   Prevents allocating zero bytes and freeing NULL pointers
  2012-12-03  pn   Added Buffering feature
  2012-11-26  vh   Replaced SNS_SMGR_MODULE with SNS_DBG_MOD_DSPS_SMGR
  2012-11-21  sc   Put back the changes for disabling sensors on WinP
  2012-11-20  vh   Eliminated Segmentation fault in SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01
  2012-11-20  pd   Disable sensors for WinP target
  2012-11-14  sc   Merged from DSPS 1.1 branch
  2012-10-31  jhh  Add INACTIVE power state vote
  2012-10-30  vh   CR 413356, sensors stream continuously after terminating all active clients.
                            Passing connection handle in sns_smgr_cancel_service().
  2012-10-23  vh   Processing the messages in task loop
  2012-10-19  vh   Eliminated Segmentation fault for SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01 &
                            SNS_SMGR_SENSOR_STATUS_REQ_V01
  2012-10-19  ag   Add support for standalone ADSP builds
  2012-10-11  sc   Added QMI support for registry request and processing
  2012-10-05  sc   Enable pressure sensor info in reference target build
  2012-10-04  br   Update priority field. Make request, responses and async indications,
                   and log and debug indications as high priority.
  2012-10-02  ag   Restructured to look similar to SMR based targets
  2012-09-19  sd   removed/featurized unused code
  2012-09-17  vh   Added non SNS_VIRTIO_ONLY dependency for PM code
  2012-09-14  br   Changed a naming SMGR_MAX_REPORT_CNT, and validity check for single_sensor_info processing
  2012-09-14  sc   Enable SMGR as a client of PM (dependency: sns_pm_init must be done)
  2012-09-13  sc   Disable SMGR being a client of PM, until PM is tested and enabled
  2012-09-10  br   Inserted parameter validity check and fixed qup clk control for single sensor test
  2012-09-06  asj  Extended sensor status indication content, clients
  2012-08-29  ag   Added SMGR as client of PM via QCCI; registered SMGR int service via QCSI
  2012-08-17  vh   Assigned connection_handle in SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01
  2012-08-07  sc   Moved struct definition client_info_type to common header
  2012-08-05  ag   Add SNS_SMGR_LDO9_FLAG to SMGR's sig channel; disable NPA code until
                   PM is tested
  2012-07-31  jhh  Added npa and OCMEM routine
  2012-07-29  ag   Porting from SMR to QMI
  2012-07-20  sd   moved update motion detection state before configuring filter when processing sensor report request
  2012-07-06  sd   report pressure temperature info if configured
  2012-06-11  pn   Made use of sns_smgr_set_sensor_state()
  2012-06-05  ag   after self test, reset sensor, restore state and set power rail
                   correctly
  2012-06-01  dc   Add support for ST pressure sensor LPS331AP.
  2012-05-25  ag   Clear sensor_state if sensor test fails; allow test if state is FAILED
  2012-05-25  pn   Prints message when test fails.
  2012-05-03  pn   Self-test info is saved whether or not test is run.
  2012-04-13  br   Inserted dynamic reporting freq. Fixed self test, changed signal_dispatch for racing condition
  2012-04-02  sd   do not send indication if device is busy when requesting selftest.
  2012-03-06  sd   Added FEATURE_TEST_DRI support
  2012-03-05  br   Adjust timestamp for report with CIC filtering
  2012-01-17  sc   Added parameters to handle calibration data in self-test
  2011-12-27  sd   Moved qup clock on/off just before/after calling DD interface
  2011-12-01  sd   Changed sns_ddf_delay to SMGR_DELAY_US
  2011-11-30  sd   added wait time after turn on powerrail in selftest
  2011-11-28  sd   Return fail for self test status if sensor state is fail
  2011-11-14  jhh  Updated alloc and free function calls to meet new API
  2011-11-14  sd   Do not send report if the sensor is not in configuring or ready state
  2011-10-28  br   Factory/Auto cal are applied ItemFlags and fixed transaction ID to be 0 in ind packet
  2011-10-09  br   Fixed sns_smgr_proc_calibration() for factory calibration
  2011-10-08  br   Fixed factory calibration in sns_smgr_proc_calibration()
  2011-09-27  ag   Fixed merge issue to get descriptive error for concurrent
                   self test
  2011-09-15  sc   Update with registry service V02
  2011-09-15  sc   Return more descriptive error coe for concurrent self-test
  2011-09-06  sc   Turn on power rail before self-test, and restore after done
  2011-08-31  sc   Turn off QUP clock after self-test finishes
  2011-08-29  sd   Added get sensor info for pressure sensor
  2011--8-28  sc   Fill in destination fields in self-test indication header
  2011-08-26  sc   Added logics to reject concurrent tests or test/streaming.
  2011-08-23  sd   added LD09 timer support
  2011-08-22  sc   Support sensor self-test (very basic functionalities).
  2011-08-11  br   set ItemQuality with SNS_SMGR_ITEM_QUALITY_FILTERED_V01 when CIC filter is on
  2011-08-10  yk   Now saving a timestamp when a MD interrupt occurs to support logging
  2011-08-05  br   rollback the change, define smgr_sensor_cfg again.
  2011-08-04  br   deleted a line which defines smgr_sensor_cfg
  2011-07-13  br   cleared cic_valid flag after the process
  2011-07-11  br   used cic_out_valid flag to deploy cic filter result, and maked auto cal logging only once.
  2011-07-05  sd   fixed a compile error
  2011-06-29  sd   modified gyro range in sensor info for ST gyro
                   modified get single sensor info to check if 2ndary data type exists in cfg
  2011-06-28  br   inserted cal log message and changed for getting range info
  2011-06-27  br   inserted a debug message out in generate_sensor_status_ind()
  2011-06-24  br   changed to support auto cal in addition to factory cal
  2011-06-17  sd   smgr code clean up
  2011-06-11  br   Changed to support cic filtering, handling smr_send() failure
  2011-06-02  sd   enable/disable qup clk before/after sns_ddf_signal_dispatch
                   also fixed in get accel sensor info,  accel only has primary data type when
                   CONFIG_USE_LIS3DH is defined
  2011-05-25  br   fixed a bug in calibration flag checking
  2011-05-16  br   set appropriate value into ItemQuality along with ItemFlags
  2011-05-10  sd   removed some warnings, updated sns_smgr_send_resp to accommodate smgr internal service
  2011-05-09  sc   Skip plan and data cyle for playback (if SNS_PLAYBACK_SKIP_SMGR is defined).
  2011-05-04  jb   moved registry request code to init.c, headed for slow memory
  2011-05-03  sd   fixed a schedule link bug, separated sns_smgr_md_init into several functions.
  2011-04-26  sd   added support for power vote and HW MD interrupt handling
  2001-04-25  br   restructured sns_smgr_cancel_service()
  2001-04-24  br   deployed the result of the code review. fixed bugs in sns_smgr_generate_report
  2001-04-20  br   changed for the new scheduler which deployed fixed heartbeat
  2011-04-04  as   Add ADXL Unit-Test (under #ifdef)
  2011-04-01  jb   Reenter calibration from registry
  2011-03-29  ad   Fix the sensor bias calibration request processing
  2011-03-27  jh   Removing driver initialization delay support
  2011-03-25  jh   Add support to insert delay before device driver initialization
                   to work around I2C issue
  2011-03-25  jh   Revert previous check-in
  2011-03-16  jb   Read data from registry. Fix warning in SENSOR_STATUS_REQ
  2011-03-15  sd   Added calibration support
  2011-03-93  as   Add BMP085 Pressure sensor Unit-Test (under #ifdef)
  2001-02-28  sd   Added sensor status support
  2011-02-24  br   Used sns_os_task_create_ext() for DSPS instead of sns_os_task_create()
  2011-02-08  ag   Add AKM8975 Magnetometer support (original checkin by osnat)
  2011-02-08  sd   Fixed gyro temperature range wasn't shown correctly in get sensor info
                   Fixed ADXL range report to match with DD change.
  2011-01-28  br   changed constant names for deploying QMI tool ver#2
  2011-01-28  sd   Added get sensor info for gyro temperature,
                   log error if no memory to generate report msg.
                   Fixed ADXL range report to match with DD change.
  2011-01-26  sd   Added buffer time for checking when to report sensor data
  2011-01-20  sd   recovered get range support for PROX_LIGHT sensor
  2011-01-19  sd   Use sensor model name as the sensor name in getting single sensor info
                   update get ADXL sensor info dynamic range fixed at 16G
  2011-01-18  sd   Updated get sensor info after DD/DDF change
  2011-01-14  sd   report sensor info only if configured, check return code of get sensor attributes
  2011-01-11  yk   Added support for new definition of SNS_DDF_ATTRIB_RESOLUTION
  2010-12-21  jb   modify diagnostics
  2010-12-13  sd   Check dd_handle_ptr before calling  DD to get attribute
  2010-12-03  sd   updated range in sensor info
  2010-11-29  jb   Fix excessive delay between delete message and its response
  2010-11-12  sc   Fixed issues related to asynchronous driver data read
  2010-11-11  sd   Updated get sensor info for PROX LIGHT
  2010-11-02  sd   Get sensor info for mag, gyro, and prox light
  2010-10-21  sd   updated sns_smgr_cancel_service
  2010-10-15  sd   fixed plan cycle and data cycle dead loop, not able to process msg
  2010-10-15  ad   Fixed reports with multiple sensor datatypes
  2010-10-15  pg   Updated sns_err_t field in sensor message response.
  2010-10-07  ad   include sns_dd.h for access to shared driver function list
  2010-10-06  br   Changed SMR API from sns_smr_q_register() to sns_smr_register()
  2010-10-06  sd   added cancel request support
  2010-10-05  jb   CC3 initial build
  2010-09-28  sd   Changed some uint32_t to uint32_t, etc
  2010-09-24  sd   Fixed  CR 256589  DSPS Hangs after processing 'report delete'
                   CR256593  DSPS Hangs when second periodic report is added
  2010-09-15  sd   Free msg report if SMR send failed/Added using DDF interface to get sensor attributes
  2010-09-08  sd   Added NACK response in common response field in report response msg
  2010-09-03  sd   Updated initialization
  2010-08-30  jtl  Moving variable defn from header file to here.
  2010-08-27  jb   updated sns_init_done() location
  2010-08-27  sd   added sns_init_done in sns_smgr_init
  2010-08-26  sd   updated power status change interface
  2010-08-25  sd   Updated sns_em_register_timer
  2010-08-25  sd   Updated with latest EM interface
  2010-08-25  sd   Removed some warnings
  2010-08-20  JTL  Use EM. Cleanup to allow compilation on AP.
  2010-08-18  sd   Updated with comments and sensor info inquery.
  2010-08-09  JB   Use ucos timer for pcsim. Edit comments
  2010-07-7   SD   Initial version
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sensor1.h"
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_init.h"
#include "fixed_point.h"

#include <qmi_csi.h>
#include <qmi_csi_common.h>
#include <qmi_csi_target_ext.h>
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"

#include <sns_smgr_api_v01.h>  /* Auto-generated by IDL */
#include <sns_common_v01.h>    /* Auto-generated by IDL */
#include <sns_smgr_internal_api_v01.h>  /* Auto-generated by IDL */
#include <sns_sam_amd_v01.h>
#include "sns_reg_api_v02.h"   /* registry messages */
#include "sns_pm_api_v01.h"
#include "sns_smgr_define.h"
#include "sns_smgr_hw.h"
#include "sns_smgr.h"
#include "sns_smgr_sensor_config.h"
#include "sns_dd.h"
#include "sns_ddf_signal.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "npa.h"

#include <stringl.h>
#include "sns_debug_str.h"
#include "sns_debug_api.h"

#include "sns_profiling.h"

#if defined(FEATURE_TEST_DRI)
#include "sns_smgr_test_dri.h" /* for DRI test purpose */
#endif

#ifdef MAG8975_UNIT_TEST
  extern void sns_dd_mag_akm8975_test();
#endif

#ifdef CONFIG_BMP085_UNIT_TEST
  extern void sns_dd_alt_bmp085_on_target_test_main(void);
#endif

#ifdef CONFIG_ADXL_UNIT_TEST
  extern void adxl350_on_target_test_main(void);
#endif

#ifdef CONFIG_DRI_UNIT_TEST
  extern void sns_smgr_dri_test_main(void);
#endif

extern boolean smgr_process_sensor_event_internal(smgr_sensor_s *sensor_ptr, 
                                                  smgr_sensor_event_e event);
extern void sns_sam_ap_state_change_cb( bool suspendState );

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define TICK_MOD_MASK                      0xFFFFFFFF
#define __SNS_MODULE__ SNS_SMGR
#define SNS_REG_SVC_TIMEOUT_MS             10000

//#define SNS_SMGR_B2B_IND_DEBUG
#ifdef SNS_SMGR_B2B_IND_DEBUG
#define SNS_SMGR_B2B_IND_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_B2B_IND_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_B2B_IND_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_B2B_IND_DBG1(level,msg,p1)
#define SNS_SMGR_B2B_IND_DBG2(level,msg,p1,p2)
#define SNS_SMGR_B2B_IND_DBG3(level,msg,p1,p2,p3)
#endif

//#define SNS_SMGR_REPORT_DEBUG
#ifdef SNS_SMGR_REPORT_DEBUG
#define SNS_SMGR_REPORT_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_REPORT_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_REPORT_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_REPORT_DBG1(level,msg,p1)
#define SNS_SMGR_REPORT_DBG2(level,msg,p1,p2)
#define SNS_SMGR_REPORT_DBG3(level,msg,p1,p2,p3)
#endif

//#define SNS_SMGR_QUERY_DEBUG
#ifdef SNS_SMGR_QUERY_DEBUG
#define SNS_SMGR_QUERY_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_QUERY_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_QUERY_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_QUERY_DBG1(level,msg,p1)
#define SNS_SMGR_QUERY_DBG2(level,msg,p1,p2)
#define SNS_SMGR_QUERY_DBG3(level,msg,p1,p2,p3)
#endif

#define MAG_CAL_ANOMALY_DET_THRESH_GAUSS_SQ  (2.0f*2.0f)

#define SNS_SMGR_CAL_ACCURACY_UNRELIABLE        (0)
#define SNS_SMGR_CAL_ACCURACY_LOW               (1)
#define SNS_SMGR_CAL_ACCURACY_MEDIUM            (2)
#define SNS_SMGR_CAL_ACCURACY_HIGH              (3)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
  smgr_sensor_id_e sensor_id;
  char*            short_name_ptr;
} sns_smgr_sensor_short_name_s;

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
static const sns_smgr_sensor_short_name_s smgr_sensor_short_names[] =
{
  { SNS_SMGR_ID_ACCEL_V01,                "ACCEL"                     },
  { SNS_SMGR_ID_ACCEL_2_V01,              "ACCEL2"                    },
  { SNS_SMGR_ID_ACCEL_3_V01,              "ACCEL3"                    },
  { SNS_SMGR_ID_ACCEL_4_V01,              "ACCEL4"                    },
  { SNS_SMGR_ID_ACCEL_5_V01,              "ACCEL5"                    },
  { SNS_SMGR_ID_GYRO_V01,                 "GYRO"                      },
  { SNS_SMGR_ID_MAG_V01,                  "MAG"                       },
  { SNS_SMGR_ID_PRESSURE_V01,             "PRESSURE"                  },
  { SNS_SMGR_ID_PROX_LIGHT_V01,           "PROX_LIGHT"                },
  { SNS_SMGR_ID_IR_GESTURE_V01,           "IR_GESTURE"                },
  { SNS_SMGR_ID_TAP_V01,                  "TAP"                       },
  { SNS_SMGR_ID_STEP_EVENT_V01,           "StepDetector"              },
  { SNS_SMGR_ID_STEP_COUNT_V01,           "StepCount"                 },
  { SNS_SMGR_ID_SMD_V01,                  "SignificantMotionDetector" },
  { SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01, "GameRotationVector"        },
  { SNS_SMGR_ID_HUMIDITY_V01,             "HUMIDITY"                  },
  { SNS_SMGR_ID_RGB_V01,                  "RGB"                       },
  { SNS_SMGR_ID_RGB_2_V01,                "RGB2"                      },
  { SNS_SMGR_ID_SAR_V01,                  "SAR"                       },
  { SNS_SMGR_ID_SAR_2_V01,                "SAR2"                      },
  { SNS_SMGR_ID_HALL_EFFECT_V01,          "HALL_EFFECT"               }
};

uint8_t       err;
sns_smgr_s    sns_smgr;
smgr_sensor_cfg_s       smgr_sensor_cfg[SNS_SMGR_NUM_SENSORS_DEFINED];

#ifdef SNS_QDSP_SIM
sns_dri_sim_s sns_dri_sim;
#endif

boolean       enable_num_clnt_ind;

/* QMI client interface */
static qmi_client_os_params   smgr_pm_cl_os_params, smgr_reg_cl_os_params;
qmi_client_type               smgr_pm_cl_user_handle, smgr_reg_cl_user_handle;
sns_q_s                       smgr_cl_queue; /* used in sns_smgr_md.c */
OS_EVENT                      *smgr_cl_que_mutex_ptr; /* used in sns_smgr_md.c */

/*----------------------------------------------------------------------------
 * Macro
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
static void sns_smgr_cancel_service(void* connection_handle);

/*=============================================================================
  FUNCTION sns_smgr_q_put
=============================================================================*/
/*!
@brief
  This puts the SMGR QMI messages into Queue and posts the signal

@param[in]  connection_handle  connection handle to differntiate the clients

@param[in]  req_handle            handle used for each requests

@param[in]  msg_id                  Message ID of the indication

@param[in]  buff_ptr                 Buffer holding the data

@param[in]  qmi_msg_type       QMI message type

@param[in]  smgr_msg_type     SMGR message type

@param[in]  svc_num               Service number

@return  Returns QMI_CSI_CB_NO_ERR or QMI_CSI_CB_INTERNAL_ERR or QMI_CSI_CB_NO_MEM

*/
/*=========================================================================*/
qmi_csi_cb_error sns_smgr_q_put(
  void                         *connection_handle,
  qmi_req_handle               req_handle,
  unsigned int                 msg_id,
  void                         *buff_ptr,
  unsigned int                 buf_ptr_len,
  qmi_idl_type_of_message_type qmi_msg_type,
  sns_qmi_msgr_msg_type_e      smgr_msg_type,
  uint8_t                      svc_num
)
{
   void          *body_ptr = NULL;
   unsigned int  body_ptr_len = 0;
   smgr_q_item_s *msg_ptr = NULL;
   uint8_t      os_err;

   if ( qmi_msg_type != QMI_IDL_RESPONSE )
   {
      if (buf_ptr_len != 0)
      {
         body_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, buf_ptr_len);
         if ( body_ptr ==  NULL )
         {
            SNS_SMGR_PRINTF2(ERROR,
                             "Msg Alloc fail, requested size = %d, smgr_msg_type = %d",
                             buf_ptr_len, smgr_msg_type);

            return QMI_CSI_CB_NO_MEM;
         }
         body_ptr_len = buf_ptr_len;
         SNS_OS_MEMCOPY(body_ptr, buff_ptr, buf_ptr_len);
      }
   }
   else
   {
     body_ptr = buff_ptr;
     body_ptr_len = buf_ptr_len;
   }

   msg_ptr = (smgr_q_item_s*)SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(smgr_q_item_s));
   if ( msg_ptr == NULL )
   {
      sns_smr_msg_free(body_ptr);

      SNS_SMGR_PRINTF2(ERROR,
                       "Msg Alloc fail, requested size = %d, smgr_msg_type = %d",
                       sizeof(smgr_q_item_s), smgr_msg_type);

      return QMI_CSI_CB_NO_MEM;
   }

   msg_ptr->header.dst_module = 0;
   msg_ptr->header.src_module = 0;
   msg_ptr->header.priority = SNS_SMR_MSG_PRI_LOW;
   msg_ptr->header.txn_id = 0;
   msg_ptr->header.ext_clnt_id = 0;
   msg_ptr->header.msg_type = qmi_msg_type;
   msg_ptr->header.for_align0 = 0;
   msg_ptr->header.svc_num = svc_num;
   msg_ptr->header.msg_id = msg_id;
   msg_ptr->header.body_len = body_ptr_len;
   msg_ptr->header.connection_handle = connection_handle;
   msg_ptr->header.req_handle = req_handle;

   msg_ptr->smgr_msg_type = smgr_msg_type;
   msg_ptr->body_ptr = body_ptr;
   sns_q_link(msg_ptr, &msg_ptr->q_link);

   sns_os_mutex_pend(smgr_cl_que_mutex_ptr, 0, &os_err);
   if (os_err != OS_ERR_NONE)
   {
     sns_smr_msg_free(msg_ptr->body_ptr);
     SNS_OS_FREE(msg_ptr);

     SNS_SMGR_PRINTF1(ERROR, "Error acquiring mutex = %d", os_err);

     return QMI_CSI_CB_INTERNAL_ERR;
   }

   sns_q_put(&smgr_cl_queue, &msg_ptr->q_link);

   os_err = sns_os_mutex_post(smgr_cl_que_mutex_ptr );
   SNS_ASSERT(os_err == OS_ERR_NONE);

   sns_os_sigs_post ( sns_smgr.sig_grp, SNS_SMGR_QMI_PM_CL_RX_SIG,
                     OS_FLAG_SET, &os_err );

   return QMI_CSI_CB_NO_ERR;
}


/*=========================================================================
  FUNCTION:  sns_smgr_q_get
  =========================================================================*/
void* sns_smgr_q_get (void)
{
  uint8_t os_err;
  void    *msg_ptr = NULL;

  sns_os_mutex_pend(smgr_cl_que_mutex_ptr, 0, &os_err );
  SNS_ASSERT ( os_err == OS_ERR_NONE );

  msg_ptr = sns_q_get(&smgr_cl_queue);

  os_err = sns_os_mutex_post(smgr_cl_que_mutex_ptr );
  SNS_ASSERT ( os_err == OS_ERR_NONE );

  return msg_ptr;
}

/*=============================================================================
  CALLBACK FUNCTION sns_smgr_client_error_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when the service
  terminates or deregisters

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.
@param[in]   error              Error code
@param[in]   err_cb_data        User-data

*/
/*=========================================================================*/
void sns_smgr_client_error_cb
(
  qmi_client_type       user_handle,
  qmi_client_error_type error,
  void                  *err_cb_data
)
{
  SMGR_MSG_2(DBG_FATAL_PRIO, "client_error_cb: handle=0x%x err=%d",
             user_handle, error);

  /* TODO - use a timer to periodically try reconnecting to service */
  /* TODO - any requests that returned an error will need to be resent */

  if (user_handle == smgr_pm_cl_user_handle)
  {
    SMGR_MSG_0(DBG_FATAL_PRIO, "SNS QMI: SMGR received service error from PM");
  }
  else if (user_handle == smgr_reg_cl_user_handle)
  {
    SMGR_MSG_0(DBG_FATAL_PRIO, "SNS QMI: SMGR received service error from REG");
  }
  return;
}

/*=============================================================================
  CALLBACK FUNCTION sns_smgr_client_ind_cb
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an indication for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to
                 identify different services.

@param[in]   msg_id              Message ID of the indication

@param[in]  ind_buf              Buffer holding the encoded indication

@param[in]  ind_buf_len          Length of the encoded indication

@param[in]  ind_cb_data          Cookie value supplied by the client during registration

*/
/*=========================================================================*/
#ifndef ADSP_STANDALONE
static void sns_smgr_client_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *ind_buf,
  unsigned int                   ind_buf_len,
  void                           *ind_cb_data
)
{
   uint8_t          svc_num;
   qmi_csi_cb_error qmi_error;

   if (user_handle == smgr_pm_cl_user_handle)
   {
     svc_num = SNS_PM_SVC_ID_V01;
   }
   else
   {
      SNS_SMGR_PRINTF0(ERROR, "Unknown client handle");
      return;
   }

   qmi_error =  sns_smgr_q_put(NULL,
                               NULL,
                               msg_id,
                               ind_buf,
                               ind_buf_len,
                               QMI_IDL_INDICATION,
                               SNS_SMGR_MSG_EXTERNAL,
                               svc_num);
}
#endif /* ADSP_STANDALONE */

/*=========================================================================
  CALLBACK FUNCTION:  sns_smgr_client_resp_cb
  =========================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure when
  infrastructure receives an asynchronous response for this client

@param[in]   user_handle         Opaque handle used by the infrastructure to
                 identify different services.

@param[in]   msg_id              Message ID of the response

@param[in]   buf                 Buffer holding the decoded response

@param[in]   len                 Length of the decoded response

@param[in]   resp_cb_data        Cookie value supplied by the client

@param[in]   transp_err          Error value

*/
/*=======================================================================*/
void sns_smgr_client_resp_cb
(
  qmi_client_type         user_handle,
  unsigned int            msg_id,
  void                    *resp_c_struct,
  unsigned int            resp_c_struct_len,
  void                    *resp_cb_data,
  qmi_client_error_type   transp_err
)
{
   uint16_t         body_size;
   uint8_t          svc_num;
   qmi_csi_cb_error qmi_error;

   if (transp_err != QMI_NO_ERR)
   {
     sns_smr_msg_free(resp_c_struct);
     SNS_SMGR_PRINTF2(ERROR, "QMI transp_err = %d, msg_id = %d", transp_err, msg_id);
     return;
   }

   if (resp_c_struct == NULL)
   {
     SNS_SMGR_PRINTF1(ERROR, "QMI response is null, msg_id = %d", msg_id);
     return;
   }

   if (user_handle == smgr_pm_cl_user_handle)
   {
     svc_num = SNS_PM_SVC_ID_V01;
     switch (msg_id)
     {
       case SNS_PM_ACTIVE_PWR_ST_CHANGE_RESP_V01:
         body_size = sizeof(sns_pm_active_pwr_st_change_resp_msg_v01);
         break;

       default:
         sns_smr_msg_free(resp_c_struct);
         return;
     }
   }
   else if (user_handle == smgr_reg_cl_user_handle)
   {
     svc_num = SNS_REG2_SVC_ID_V01;
     switch (msg_id)
     {
       case SNS_REG_GROUP_READ_REQ_V02:
         body_size = sizeof(sns_reg_group_read_resp_msg_v02);
         // Get group id
         if( resp_cb_data )
         {
           sns_smgr.last_received_reg_group_id = *(uint16_t*)resp_cb_data;
           SNS_OS_FREE( resp_cb_data );
         }
         break;

       case SNS_REG_SINGLE_READ_REQ_V02:
         body_size = sizeof(sns_reg_single_read_resp_msg_v02);
         break;

       default:
         sns_smr_msg_free(resp_c_struct);
         return;
     }
   }
   else
   {
      SNS_SMGR_PRINTF0(ERROR, "Unknown client handle");
      sns_smr_msg_free(resp_c_struct);
      return;
   }

   if (body_size != resp_c_struct_len)
   {
      SNS_SMGR_PRINTF1(ERROR, "Invalid response, msg_id = %d", msg_id);

      sns_smr_msg_free(resp_c_struct);
      return;
   }

   qmi_error =  sns_smgr_q_put(NULL,
                               NULL,
                               msg_id,
                               resp_c_struct,
                               resp_c_struct_len,
                               QMI_IDL_RESPONSE,
                               SNS_SMGR_MSG_EXTERNAL,
                               svc_num);
}

/*=========================================================================
  CALLBACK FUNCTION:  sns_smgr_connect_cb
  =========================================================================*/
/*!
  @brief Callback registered with QCSI to receive connect requests
*/
/*=======================================================================*/
static qmi_csi_cb_error smgr_connect_cb
(
  qmi_client_handle         client_handle,
  void                      *service_cookie,
  void                      **connection_handle
)
{
  qmi_csi_cb_error qmi_err = QMI_CSI_CB_CONN_REFUSED;
  if ( connection_handle != NULL )
  {
    uint8_t i;

    for ( i=0; i<ARR_SIZE(sns_smgr.connection_list); i++ )
    {
      if ( !sns_smgr.connection_list[i].is_valid && 
           sns_smgr.connection_list[i].client_handle == NULL )
      {
        sns_smgr.connection_list[i].is_valid = true;
        sns_smgr.connection_list[i].client_handle = client_handle;
        *connection_handle = (void*)&sns_smgr.connection_list[i];
        qmi_err = QMI_CSI_CB_NO_ERR;
        SNS_SMGR_PRINTF2(HIGH, "connect_cb - conn=0x%x cli_hndl=0x%x", 
                         *connection_handle, client_handle);
        break;
      }
    }
  }
  if ( qmi_err == QMI_CSI_CB_CONN_REFUSED )
  {
    SNS_SMGR_PRINTF1(ERROR, "connect_cb - cli_hndl 0x%x refused", client_handle);
  }
  return qmi_err;
}


/*=========================================================================
  CALLBACK FUNCTION:  sns_smgr_disconnect_cb
  =========================================================================*/
/*!
  @brief Callback registered with QCSI to receive disconnect requests
*/
/*=======================================================================*/
void smgr_disconnect_cb
(
  void* connection_handle,
  void* service_cookie
)
{
  uint8_t i;
  SNS_SMGR_PRINTF1(HIGH, "disconnect_cb - conn_handle=0x%x", connection_handle);
  for ( i=0; i<ARR_SIZE(sns_smgr.connection_list); i++ )
  {
    if ( connection_handle == &sns_smgr.connection_list[i] &&
         sns_smgr.connection_list[i].is_valid )
    {
      uint8_t err=0; 
      sns_smgr.connection_list[i].is_valid = false;
      sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_QMI_DISC_SIG, OS_FLAG_SET, &err);
      break;
    }
  }
}


/*=========================================================================
  CALLBACK FUNCTION:  sns_smgr_handle_req_cb
  =========================================================================*/
/*!
  @brief Callback registered with QCSI to receive service requests
*/
/*=======================================================================*/
static qmi_csi_cb_error smgr_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
)
{
   return sns_smgr_q_put(connection_handle,
                         req_handle,
                         msg_id,
                         req_c_struct,
                         req_c_struct_len,
                         QMI_IDL_REQUEST,
                         SNS_SMGR_MSG_EXTERNAL,
                         0);
}

/*=========================================================================
  CALLBACK FUNCTION:  smgr_restrict_handle_req_cb
  =========================================================================*/
/*!
  @brief Callback registered with QCSI to receive service requests
*/
/*=======================================================================*/
static qmi_csi_cb_error smgr_restrict_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
)
{
   return sns_smgr_q_put(connection_handle,
                         req_handle,
                         msg_id,
                         req_c_struct,
                         req_c_struct_len,
                         QMI_IDL_REQUEST,
                         SNS_SMGR_MSG_RESTRICTED,
                         0);
}


/*===========================================================================

  @brief This function returns the processor for a given module group

  @param[i] group of module

  @return  processor for the group
 */
/*=========================================================================*/
static uint8_t sns_smgr_proc_from_group(uint8_t group)
{
  uint8_t ret_val;
  switch ( group )
  {
     case SNS_MODULE_APPS:
       ret_val = SNS_SMGR_APPS_CLIENTS_V01;
       break;

     case  SNS_MODULE_DSPS:
       ret_val = SNS_SMGR_DSPS_CLIENTS_V01;
       break;

     case SNS_MODULE_MDM:
       ret_val = SNS_SMGR_MODEM_CLIENT_V01;
       break;

     default:
       ret_val = SNS_SMGR_MAX_PROCS;
       break;
  }

  return (ret_val);
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_sensor_status_req_msg

===========================================================================*/
/*!
  @brief This function process a sensor status report request message, add or
         delete a client from power status report list

  @detail

  @param[i] Hdr_p request message header
  @param[i] Msg_p request message pointer
  @param[o] Response  response pointer
  @return  none
 */
/*=========================================================================*/
static void sns_smgr_proc_sensor_status_req_msg (
  sns_smr_header_s                    *Hdr_p,
  sns_smgr_sensor_status_req_msg_v01  *Msg_p,
  sns_smgr_sensor_status_resp_msg_v01 *resp_ptr
)
{
  sns_smr_header_s  resp_msg_header;
  smgr_sensor_s*    sensor_ptr = sns_smgr_find_sensor(Msg_p->SensorID);
  uint8_t           data_type = SNS_SMGR_DATA_TYPE_PRIMARY_V01;
  uint8_t           dst_module_proc =
    sns_smgr_proc_from_group(SNS_MODULE_GRP_MASK & Hdr_p->src_module);
  smgr_sensor_status_s* sensor_status_ptr = NULL;
  smgr_status_client_info_s* status_client_ptr = NULL;

  SNS_SMGR_PRINTF3(MED, "proc_sensor_status_req - action=%d sensor=%d src_mod=0x%x",
                   Msg_p->Action, Msg_p->SensorID, Hdr_p->src_module);

  /* response msg construct */
  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id   = SNS_SMGR_SENSOR_STATUS_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_sensor_status_resp_msg_v01);

  resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;
  resp_ptr->SensorID          = Msg_p->SensorID;

  if ( (sensor_ptr == NULL) || (dst_module_proc >= SNS_SMGR_MAX_PROCS) )
  {
    /* This is not a sane message */
    resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
    resp_ptr->Resp.sns_err_t = SENSOR1_EBAD_PARAM;
    SNS_SMGR_PRINTF1(ERROR, "proc_sensor_status_req - sensor=%d", Msg_p->SensorID);
  }
  else if ( sensor_ptr->ddf_sensor_ptr[data_type] == NULL )
  {
    resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
    resp_ptr->Resp.sns_err_t = SENSOR1_ENOTALLOWED;
    SNS_SMGR_PRINTF0(ERROR, "proc_sensor_status_req - sensor init not completed");
  }
  else if ( Msg_p->Action == SNS_SMGR_SENSOR_STATUS_ADD_V01 )
  {
    smgr_ddf_sensor_s* ddf_sensor_ptr =
      sensor_ptr->ddf_sensor_ptr[data_type];

    status_client_ptr = &ddf_sensor_ptr->sensor_status.status_clients[dst_module_proc];

    if ( (status_client_ptr->used == FALSE) &&
         (sensor_ptr->sensor_state != SENSOR_STATE_FAILED) )
    {
      sensor_status_ptr = &ddf_sensor_ptr->sensor_status;

      status_client_ptr->used = TRUE;
      status_client_ptr->dst_module        = Hdr_p->src_module;
      status_client_ptr->ext_clnt_id       = Hdr_p->ext_clnt_id;
      status_client_ptr->connection_handle = Hdr_p->connection_handle;
      status_client_ptr->req_data_type_num = Msg_p->ReqDataTypeNum;
      status_client_ptr->last_status       = SNS_SMGR_SENSOR_STATUS_UNKNOWN_V01;
      status_client_ptr->pending_status    =
        sensor_status_ptr->num_requested_items ?
        SNS_SMGR_SENSOR_STATUS_ACTIVE_V01 : SNS_SMGR_SENSOR_STATUS_IDLE_V01;
    }
    else
    {
      /* outstanding request exists, only one outstanding sensor status request
           is allowed per sensor or sensor initialization failed */
      resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      resp_ptr->Resp.sns_err_t    =
          (sensor_ptr->sensor_state != SENSOR_STATE_FAILED) ?
          SENSOR1_ENOTALLOWED : SENSOR1_EFAILED;
    }
  }
  else if ( Msg_p->Action == SNS_SMGR_SENSOR_STATUS_DEL_V01 )
  {
    SNS_OS_MEMZERO(&sensor_ptr->ddf_sensor_ptr[data_type]->sensor_status.
                   status_clients[dst_module_proc],
                   sizeof(smgr_status_client_info_s) );
  }

  sns_smgr_send_resp(&resp_msg_header, resp_ptr, *Hdr_p);
  if ( sensor_status_ptr != NULL )
  {
    sns_smgr_generate_sensor_status_ind(sensor_status_ptr, dst_module_proc);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_single_sensor_test

===========================================================================*/
/*!
  @brief This function self-tests a sensor and fills out result in response
    message body.

  @detail

  @param[o] req_ptr   request pointer
  @param[o] resp_ptr  response pointer
  @return  device-specific test error code
 */
/*=========================================================================*/
static uint32_t sns_smgr_single_sensor_test (
  const smgr_sensor_test_s*                 test_info_ptr,
  sns_smgr_single_sensor_test_req_msg_v01*  req_ptr,
  sns_smgr_single_sensor_test_resp_msg_v01* resp_ptr
)
{
   uint32_t err = 0;
   sns_ddf_status_e status;
   smgr_sensor_s *sensor_ptr = sns_smgr_find_sensor(req_ptr->SensorID);
   const smgr_sensor_cfg_s *sensor_cfg_ptr;
   smgr_sensor_state_e saved_sensor_state;

   resp_ptr->SensorID = req_ptr->SensorID;
   resp_ptr->DataType = req_ptr->DataType;
   resp_ptr->TestType = req_ptr->TestType;

   SNS_SMGR_PRINTF3(HIGH, "single_sensor_test - sensor=%d dtype=%d test=%d",
                    req_ptr->SensorID, req_ptr->DataType, req_ptr->TestType);
   /* validate the request */
   if ( (sensor_ptr == NULL) || (req_ptr->DataType >= SMGR_MAX_DATA_TYPES_PER_DEVICE) )
   {
      if ( sensor_ptr != NULL )
      {
        SNS_SMGR_PRINTF1(HIGH, "single_sensor_test - sensor state=%d",
                      sensor_ptr->sensor_state);
      }
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_INVALID_PARAM_V01;
      resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      resp_ptr->Resp.sns_err_t = SENSOR1_EBAD_PARAM;
      return err;
   }

   /* save the sensor state to sns_smgr.sensor_test_info */
   sns_smgr.sensor_test_info.saved_sensor_state = sensor_ptr->sensor_state;

   if ( sensor_ptr->init_state == SENSOR_INIT_FAIL )
   {
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_FAIL_V01;
      resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      resp_ptr->Resp.sns_err_t = SENSOR1_EFAILED;
      return err;
   }

   saved_sensor_state = sensor_ptr->sensor_state;
   sensor_cfg_ptr = sensor_ptr->const_ptr;

   /* check whether another test is running concurrently */
   if ( sns_smgr.sensor_test_info.test_done == false )
   {
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_BUSY_TESTING_V01;
      resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;
      return err;
   }

   /* Only allow test when sensor is not actively doing something. */
   /* Allow tests in FAILED state in case test can pin point issue. */
   if ( (sensor_ptr->sensor_state != SENSOR_STATE_OFF) &&
        (sensor_ptr->sensor_state != SENSOR_STATE_IDLE) &&
        (sensor_ptr->sensor_state != SENSOR_STATE_FAILED))
   {
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_DEVICE_BUSY_V01;
      resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;
      return err;
   }

   /* now, set the rest of sns_smgr.sensor_test_info */
   sns_smgr.sensor_test_info = *test_info_ptr;
   sns_smgr.sensor_test_info.saved_sensor_state = sensor_ptr->sensor_state;
   sns_smgr.sensor_test_info.test_done = false;
   sns_smgr.sensor_test_info.save_to_registry = (req_ptr->SaveToRegistry_valid == 1) ?
      (bool)(req_ptr->SaveToRegistry) : true;
   sns_smgr.sensor_test_info.apply_cal_now = (req_ptr->ApplyCalNow_valid == 1) ?
      (bool)(req_ptr->ApplyCalNow) : true;
   sns_smgr.sensor_test_info.powerrail_state = sns_hw_powerrail_status();

   if (sns_hw_powerrail_status() == SNS_SMGR_POWER_OFF)
   {
     sns_hw_power_rail_config(SNS_SMGR_POWER_HIGH);
     SMGR_DELAY_US(sns_em_convert_dspstick_to_usec(sensor_ptr->const_ptr->off_to_idle_time));
   }

   sns_hw_set_qup_clk(true);
   sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_TESTING);

   status = (SMGR_DRV_FN_PTR(sensor_ptr)->run_test == NULL) ?
            SNS_DDF_EINVALID_TEST : ( (saved_sensor_state != SENSOR_STATE_FAILED) ?
            SMGR_DRV_FN_PTR(sensor_ptr)->run_test(sensor_ptr->dd_handle,
                                                  sensor_cfg_ptr->data_types[req_ptr->DataType],
                                                  (sns_ddf_test_e)(req_ptr->TestType),
                                                  &err) : SNS_DDF_EFAIL);
   sns_hw_set_qup_clk(false);

   switch ( status )
   {
   case SNS_DDF_SUCCESS:
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_SUCCESS_V01;
      break;
   case SNS_DDF_PENDING:
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_PENDING_V01;
      break;
   case SNS_DDF_EDEVICE_BUSY:
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_DEVICE_BUSY_V01;
      break;
   case SNS_DDF_EINVALID_TEST:
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_INVALID_TEST_V01;
      break;
   case SNS_DDF_EINVALID_PARAM:
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_INVALID_PARAM_V01;
      break;
   case SNS_DDF_EFAIL:
   default:
      resp_ptr->TestStatus = SNS_SMGR_TEST_STATUS_FAIL_V01;
      break;
   }

   if ( status != SNS_DDF_PENDING )
   {
     /* restore sensor state on failure */
     sns_smgr_set_sensor_state(sensor_ptr, saved_sensor_state);
   }

   resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
   resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;

   return err;
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_single_test_req_msg

===========================================================================*/
/*!
  @brief This function self-tests a sensor and fills out result in response
    message body.

  @detail

  @param[o] req_ptr   request pointer
  @param[o] resp_ptr  response pointer
  @return  device-specific test error code
 */
/*=========================================================================*/
static void sns_smgr_proc_single_test_req_msg (
  const sns_smr_header_s*                   hdr_ptr,
  sns_smgr_single_sensor_test_req_msg_v01*  req_ptr,
  sns_smgr_single_sensor_test_resp_msg_v01* resp_ptr
)
{
  sns_smr_header_s resp_msg_header;
  uint32_t test_err_code;
  smgr_sensor_test_s test_info;
  smgr_sensor_id_e saved_sensor_id;

  SNS_OS_MEMSET(&test_info,  0,  sizeof(test_info));
  test_info.hdr_abstract.src_module        = hdr_ptr->src_module;
  test_info.hdr_abstract.ext_clnt_id       = hdr_ptr->ext_clnt_id;
  test_info.hdr_abstract.connection_handle = hdr_ptr->connection_handle;
  test_info.sensor_id                      = req_ptr->SensorID;
  test_info.data_type                      = req_ptr->DataType;
  test_info.test_type                      = req_ptr->TestType;

  test_err_code = sns_smgr_single_sensor_test(&test_info, req_ptr, resp_ptr);

  /* save the resp contents for future use */
  sns_smgr_test_status_e_v01 resp_TestStatus = resp_ptr->TestStatus;
  SNS_SMGR_PRINTF3(MED, "result=%d err=%d status=%d", resp_ptr->Resp.sns_result_t,
                   resp_ptr->Resp.sns_err_t, resp_ptr->TestStatus);

  /* fill-out the header */
  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id = SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_single_sensor_test_resp_msg_v01);
  saved_sensor_id = resp_ptr->SensorID;
  sns_smgr_send_resp(&resp_msg_header,resp_ptr, *hdr_ptr);

  /* if test is not asynchronous, send test result indication now */
  if ( (resp_TestStatus != SNS_SMGR_TEST_STATUS_PENDING_V01) &&
       (resp_TestStatus != SNS_SMGR_TEST_STATUS_INVALID_PARAM_V01) )
  {
    smgr_sensor_s* sensor_ptr = sns_smgr_find_sensor(saved_sensor_id);
    sns_smgr_generate_test_result(&test_info, resp_TestStatus, test_err_code);

    if ( NULL == sensor_ptr )
    {
      SNS_SMGR_PRINTF2(ERROR, "Bad Sensor Id, test_status=%d sensor_id=%d", resp_TestStatus, saved_sensor_id);
    }
    else
    {
      /* Reset the sensor if it is NOT BUSY streaming/testing */
      if ( (resp_TestStatus != SNS_SMGR_TEST_STATUS_DEVICE_BUSY_V01) &&
           (resp_TestStatus != SNS_SMGR_TEST_STATUS_BUSY_TESTING_V01) &&
           (resp_TestStatus != SNS_SMGR_TEST_STATUS_INVALID_TEST_V01) &&
           (sns_smgr.sensor_test_info.saved_sensor_state != SENSOR_STATE_FAILED) )
      {
        sns_hw_set_qup_clk(true);
        SMGR_DRV_FN_PTR(sensor_ptr)->reset(sensor_ptr->dd_handle);
        sns_hw_set_qup_clk(false);
      }
      sensor_ptr->sensor_state = sns_smgr.sensor_test_info.saved_sensor_state;
    }
    /* test is done */
    sns_smgr.sensor_test_info.test_done = true;
#ifndef SNS_EXCLUDE_POWER
    sns_hw_power_rail_config(sns_smgr.sensor_test_info.powerrail_state);
#endif
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_attribute_max_freq

===========================================================================*/
/*!
  @brief This function gets sensor maximum frequency from DDF

  @detail

  @param[i] sensor_ptr - the sensor of interest
  @param[i] DataType - SNS_SMGR_DATA_TYPE_PRIMARY_V01 or SNS_SMGR_DATA_TYPE_SECONDARY_V01
  @param[o] sensor_info - sensor info pointer

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_get_attribute_max_freq(
  smgr_sensor_s*                       sensor_ptr,
  uint32_t                             DataType,
  sns_smgr_sensor_datatype_info_s_v01* sensor_info
)
{
  if ( sensor_ptr->ddf_sensor_ptr[DataType]->max_supported_freq_hz == 0 )
  {
    sns_smgr_init_max_frequency(sensor_ptr);
  }
  sensor_info->MaxSampleRate =
    sensor_ptr->ddf_sensor_ptr[DataType]->max_supported_freq_hz;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_attribute_power

===========================================================================*/
/*!
  @brief This function gets sensor power info from DDF

  @detail

  @param[i] sensor_ptr - the sensor of interest
  @param[i] DataType - SNS_SMGR_DATA_TYPE_PRIMARY_V01 or SNS_SMGR_DATA_TYPE_SECONDARY_V01
  @param[o] sensor_info - sensor info pointer

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_get_attribute_power(
  smgr_sensor_s*                       sensor_ptr,
  uint32_t                             DataType,
  sns_smgr_sensor_datatype_info_s_v01* sensor_info
)
{
  uint32_t len;
  sns_ddf_power_info_s* power_info_ptr;

  /* get power info */
  sns_ddf_memhandler_init(&sensor_ptr->memhandler);
  if ( sns_smgr_get_attr(sensor_ptr,
                         SMGR_SENSOR_TYPE(sensor_ptr, DataType),
                         SNS_DDF_ATTRIB_POWER_INFO,
                         (void**)&power_info_ptr,
                         &len) == SNS_DDF_SUCCESS )
  {
      sensor_info->IdlePower = power_info_ptr->lowpower_current;
      sensor_info->MaxPower = power_info_ptr->active_current;
  }
  sns_ddf_memhandler_free(&sensor_ptr->memhandler);
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_attribute_range

===========================================================================*/
/*!
  @brief This function gets sensor range info from DDF

  @detail

  @param[i] sensor_ptr - the sensor of interest
  @param[i] DataType - SNS_SMGR_DATA_TYPE_PRIMARY_V01 or SNS_SMGR_DATA_TYPE_SECONDARY_V01
  @param[o] sensor_info - sensor info pointer

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_get_attribute_range(
  smgr_sensor_s*                       sensor_ptr,
  uint32_t                             DataType,
  sns_smgr_sensor_datatype_info_s_v01* sensor_info
)
{
  sns_ddf_range_s *range_ptr;
  uint32_t len;

  sns_ddf_memhandler_init(&sensor_ptr->memhandler);
  if ( sns_smgr_get_attr(sensor_ptr,
                         SMGR_SENSOR_TYPE(sensor_ptr, DataType),
                         SNS_DDF_ATTRIB_RANGE,
                         (void**)&range_ptr,
                         &len) == SNS_DDF_SUCCESS )
  {
    if ( (SMGR_SENSOR_IS_ACCEL(sensor_ptr) || SMGR_SENSOR_IS_GYRO(sensor_ptr)) &&
         (len > sensor_ptr->const_ptr->sensitivity_default) )
    {
      sensor_info->MaxRange = range_ptr[sensor_ptr->const_ptr->sensitivity_default].max;
    }
    else if (len > 0)
    {
      sensor_info->MaxRange = range_ptr[len-1].max;
    }
  }
  sns_ddf_memhandler_free(&sensor_ptr->memhandler);
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_attribute_resolution

===========================================================================*/
/*!
  @brief This function gets sensor resolution info from DDF

  @detail

  @param[i] sensor_ptr - the sensor of interest
  @param[i] DataType - SNS_SMGR_DATA_TYPE_PRIMARY_V01 or SNS_SMGR_DATA_TYPE_SECONDARY_V01
  @param[o] sensor_info - sensor info pointer

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_get_attribute_resolution(
  smgr_sensor_s*                       sensor_ptr,
  uint32_t                             DataType,
  sns_smgr_sensor_datatype_info_s_v01* sensor_info
)
{
  sns_ddf_resolution_t* resolution_ptr;
  uint32_t len;

  sns_ddf_memhandler_init(&sensor_ptr->memhandler);
  if ( sns_smgr_get_attr(sensor_ptr,
                         SMGR_SENSOR_TYPE(sensor_ptr, DataType),
                         SNS_DDF_ATTRIB_RESOLUTION,
                         (void**)&resolution_ptr,
                         &len) == SNS_DDF_SUCCESS )
  {
    sensor_info->Resolution = (uint32_t)*resolution_ptr;
  }
  sns_ddf_memhandler_free(&sensor_ptr->memhandler);
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_attribute_name_vendor_version

===========================================================================*/
/*!
  @brief This function gets sensor name, vendor, version from DDF

  @detail

  @param[i] sensor_ptr - the sensor of interest
  @param[i] DataType - SNS_SMGR_DATA_TYPE_PRIMARY_V01 or SNS_SMGR_DATA_TYPE_SECONDARY_V01
  @param[o] sensor_info - sensor info pointer

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_get_attribute_name_vendor_version(
  smgr_sensor_s*                       sensor_ptr,
  uint32_t                             DataType,
  sns_smgr_sensor_datatype_info_s_v01* sensor_info
)
{
  sns_ddf_device_info_s *dev_info_ptr;
  uint32_t len;

  sns_ddf_memhandler_init(&sensor_ptr->memhandler);
  if ( sns_smgr_get_attr(sensor_ptr,
                         SMGR_SENSOR_TYPE(sensor_ptr, DataType),
                         SNS_DDF_ATTRIB_DEVICE_INFO,
                         (void**)&dev_info_ptr,
                         &len) == SNS_DDF_SUCCESS )
  {
    if (strlen(dev_info_ptr->model) + strlen(dev_info_ptr->name) + 1 < 
        sizeof(sensor_info->SensorName))
    {
      snprintf(sensor_info->SensorName, sizeof(sensor_info->SensorName),
               "%s %s", dev_info_ptr->model, dev_info_ptr->name);
    }
    else
    {
      strlcpy(sensor_info->SensorName, dev_info_ptr->name,
              sizeof(sensor_info->SensorName));
    }
    sensor_info->SensorName_len = strlen(sensor_info->SensorName);

    strlcpy(sensor_info->VendorName, dev_info_ptr->vendor,
            sizeof(sensor_info->VendorName));
    sensor_info->VendorName_len = strlen(sensor_info->VendorName);
    sensor_info->Version = dev_info_ptr->version;
  }
  sns_ddf_memhandler_free(&sensor_ptr->memhandler);
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_all_sensor_info_req

===========================================================================*/
/*!
  @brief Handles request for all sensor info.

  @detail

  @param[i]  resp_ptr    - Response Pointer

  @return none
 */
/*=========================================================================*/
static void sns_smgr_proc_all_sensor_info_req(
  const sns_smr_header_s*                hdr_ptr,
  sns_smgr_all_sensor_info_resp_msg_v01* resp_ptr
)
{
  uint8_t i, j;
  sns_smr_header_s resp_msg_header;

  SNS_OS_MEMZERO( resp_ptr, sizeof(sns_smgr_all_sensor_info_resp_msg_v01) );
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
    if ( (sensor_ptr != NULL) && (SMGR_DRV_FN_PTR(sensor_ptr) != NULL) &&
         (sensor_ptr->dd_handle != NULL ) &&
         (sensor_ptr->sensor_state != SENSOR_STATE_FAILED) )
    {
      sns_smgr_sensor_id_info_s_v01* sensor_info_ptr =
        &resp_ptr->SensorInfo[resp_ptr->SensorInfo_len++];
      for ( j=0; j<ARR_SIZE(smgr_sensor_short_names); j++ )
      {
        if ( smgr_sensor_short_names[j].sensor_id ==
             sensor_ptr->const_ptr->sensor_id )
        {
          strlcpy(sensor_info_ptr->SensorShortName, 
                  smgr_sensor_short_names[j].short_name_ptr,
                  sizeof(sensor_info_ptr->SensorShortName));
          break;
        }
      }
      if ( j >= ARR_SIZE(smgr_sensor_short_names))
      {
        strlcpy(sensor_info_ptr->SensorShortName, "UNKNOWN",
                sizeof(sensor_info_ptr->SensorShortName));
      }
      sensor_info_ptr->SensorShortName_len = strlen(sensor_info_ptr->SensorShortName);
      sensor_info_ptr->SensorID = sensor_ptr->const_ptr->sensor_id;
    }
  }
  resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;

  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id   = SNS_SMGR_ALL_SENSOR_INFO_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_all_sensor_info_resp_msg_v01);
  sns_smgr_send_resp(&resp_msg_header,resp_ptr, *hdr_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_single_sensor_info_fill

===========================================================================*/
/*!
  @brief This function fills out sensor info for a sensor

  @detail

  @param[i]  SensorID - Sensor ID
  @param[i]  resp_ptr    - Response Pointer

  @return  TRUE if successful
 */
/*=========================================================================*/
static void sns_smgr_single_sensor_info_fill(
  const sns_smr_header_s*                   hdr_ptr,
  uint8_t                                   SensorID,
  sns_smgr_single_sensor_info_resp_msg_v01* resp_ptr
)
{
  sns_smr_header_s resp_msg_header;
  smgr_sensor_s* sensor_ptr = sns_smgr_find_sensor(SensorID);

  SNS_SMGR_PRINTF1(MED, "single_sensor_info - sensor=%d", SensorID);

  resp_ptr->SensorInfo.data_type_info_len = 0;
  if ( (sensor_ptr != NULL) && (SMGR_DRV_FN_PTR(sensor_ptr) != NULL) &&
       (sensor_ptr->dd_handle != NULL ) &&
       (sensor_ptr->sensor_state != SENSOR_STATE_FAILED) )
  {
    uint8_t i;
    for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
    {
      if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
      {
        sns_smgr_sensor_datatype_info_s_v01* datatype_ptr =
          &resp_ptr->SensorInfo.data_type_info[resp_ptr->SensorInfo.data_type_info_len];

        datatype_ptr->SensorID = SensorID;
        datatype_ptr->DataType = i;
        sns_smgr_get_attribute_max_freq( sensor_ptr, i, datatype_ptr );
        sns_smgr_get_attribute_name_vendor_version(
          sensor_ptr, i, datatype_ptr );
        sns_smgr_get_attribute_power( sensor_ptr, i, datatype_ptr );
        sns_smgr_get_attribute_range( sensor_ptr, i, datatype_ptr );
        sns_smgr_get_attribute_resolution( sensor_ptr, i, datatype_ptr );
        
        resp_ptr->num_buffered_reports[resp_ptr->SensorInfo.data_type_info_len] =
          sns_smgr_get_max_sample_count(sensor_ptr->ddf_sensor_ptr[i]->depot_ptr);
        SNS_SMGR_PRINTF1(
           LOW, "single_sensor_info - num_buf=%d", 
           resp_ptr->num_buffered_reports[resp_ptr->SensorInfo.data_type_info_len]);
        resp_ptr->SensorInfo.data_type_info_len++;
      }
    }
    resp_ptr->num_buffered_reports_valid = true;
    resp_ptr->num_buffered_reports_len   = resp_ptr->SensorInfo.data_type_info_len;
    resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;
  }
  else
  {
    resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_EBAD_PARAM;
    SNS_SMGR_PRINTF0(ERROR, "single_sensor_info - bad param");
  }

  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id   = SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_single_sensor_info_resp_msg_v01);
  sns_smgr_send_resp( &resp_msg_header, resp_ptr, *hdr_ptr );
}

/*===========================================================================

  FUNCTION:   sns_smgr_cancel_service

===========================================================================*/
/*!
  @brief This function cancels all requests to the service from this client

  @detail

  @param[i]  src_module - source module
  @param[i]  client_id  - clinet ID
  @param[i]  qcci_handle_ptr - QCCI handle

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_cancel_service(void* connection_handle)
{
  smgr_rpt_spec_s *delete_rpt_ptr[SNS_SMGR_MAX_REPORT_CNT];
  uint8_t i = 0, num_reports = 0;

  smgr_rpt_spec_s *rpt_ptr;
  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.report_queue, rpt_ptr, rpt_link)
  {
    if (connection_handle == rpt_ptr->header_abstract.connection_handle)
    {
      SNS_SMGR_PRINTF1(MED, "cancel_service - found report %d", rpt_ptr->rpt_id);
      delete_rpt_ptr[num_reports++] = rpt_ptr;
    }
  }/* for each report in queue */

  while ( i < num_reports )
  {
    sns_smgr_delete_report(delete_rpt_ptr[i++]);
  }
  if (num_reports > 0)
  {
    sns_smgr_check_rpts_for_md_update();
    sns_smgr_check_sensor_status();
  }
  else
  {
    SNS_SMGR_PRINTF1(LOW, "cancel_service - no reports associated with conn_hndl 0x%x", 
                     connection_handle);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_cancel_restricted_service

===========================================================================*/
/*!
  @brief This function cancels all requests to the SMGR Restricted service from
         this client.

  @detail

  @param[i]  connection_handle - The connection handle for the client
                                 requesting the cancellation.

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_cancel_restricted_service(void* connection_handle)
{
  smgr_sensor_s* sensor_ptr;

  // Iterate through all drivers and pass on the cancel request.
  int i;
  for(i = 0; i < SNS_SMGR_NUM_SENSORS_DEFINED; i++)
  {
    sensor_ptr = &sns_smgr.sensor[i];
    if (SMGR_DRV_FN_PTR(sensor_ptr) != NULL &&
        SMGR_DRV_FN_PTR(sensor_ptr)->cancel_daf_trans != NULL)
    {
      SMGR_DRV_FN_PTR(sensor_ptr)->cancel_daf_trans(
                             sensor_ptr->dd_handle,
                             connection_handle);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_cancel_service_req_msg

===========================================================================*/
/*!
  @brief Handles SNS_SMGR_CANCEL_REQ.

  @detail

  @param[i]  hdr_ptr   the request message header
  @param[i]  resp_ptr  the response message

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_proc_cancel_service_req_msg(
  const sns_smr_header_s*         hdr_ptr,
  sns_common_cancel_resp_msg_v01* resp_ptr
)
{
  sns_smr_header_s resp_msg_header;
  SNS_SMGR_PRINTF1(MED, "cancel_svc - conn=0x%x",hdr_ptr->connection_handle);
  sns_smgr_cancel_service(hdr_ptr->connection_handle);
  resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;

  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id   = SNS_SMGR_CANCEL_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_common_cancel_resp_msg_v01);
  sns_smgr_send_resp(&resp_msg_header, resp_ptr, *hdr_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_version_req_msg

===========================================================================*/
/*!
  @brief Handles SNS_SMGR_VERSION_REQ.

  @detail

  @param[i]  hdr_ptr   the request message header
  @param[i]  resp_ptr  the response message

  @return  none
 */
/*=========================================================================*/
static void sns_smgr_proc_version_req_msg(
  const sns_smr_header_s*          hdr_ptr,
  sns_common_version_resp_msg_v01* resp_ptr
)
{
  sns_smr_header_s resp_msg_header;

  resp_ptr->max_message_id = SNS_SMGR_SVC_V01_MAX_MESSAGE_ID;
  resp_ptr->interface_version_number = SNS_SMGR_SVC_V01_IDL_MINOR_VERS;
  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id   = SNS_SMGR_VERSION_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_common_version_resp_msg_v01);
  resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->resp.sns_err_t    = SENSOR1_SUCCESS;
  sns_smgr_send_resp(&resp_msg_header, resp_ptr, *hdr_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_calibration

===========================================================================*/
/*!
  @brief This function processes sensor calibration

  @detail

  @param[i]  msg_header - message header
  @param[i]  req_ptr  - request message pointer
  @param[o]  resp_ptr  - response message pointer
  @return  none
 */
/*=========================================================================*/
static void sns_smgr_proc_calibration(const sns_smr_header_s *msg_header,
                                      sns_smgr_sensor_cal_req_msg_v01 *req_ptr,
                                      sns_smgr_sensor_cal_resp_msg_v01 *resp_ptr)
{
  smgr_sensor_s *sensor_ptr = sns_smgr_find_sensor(req_ptr->SensorId);
  sns_smr_header_s resp_msg_header;
  sns_ddf_status_e err;
  uint32_t i;

  SNS_SMGR_PRINTF3(MED, "proc_calibration - sensor=%d dtype=%d usage=%d",
                   req_ptr->SensorId, req_ptr->DataType, req_ptr->usage);
  if ( (sensor_ptr != NULL) &&
       (req_ptr->DataType < ARR_SIZE(sensor_ptr->ddf_sensor_ptr)) &&
       (sensor_ptr->ddf_sensor_ptr[req_ptr->DataType] != NULL) &&
       (req_ptr->ZeroBias_len <= SNS_SMGR_SENSOR_DIMENSION_V01) &&
       (req_ptr->ScaleFactor_len <= SNS_SMGR_SENSOR_DIMENSION_V01) )
  {
    smgr_ddf_sensor_s* ddf_sensor_ptr =
      sensor_ptr->ddf_sensor_ptr[req_ptr->DataType];
    smgr_cal_s* cal_db_ptr;

    if (SNS_SMGR_CAL_FACTORY_V01 == req_ptr->usage)
    {
      /* Nullify all autocal data */
      ddf_sensor_ptr->auto_cal.used = FALSE;
      smgr_load_default_cal( &(ddf_sensor_ptr->auto_cal) );

      /* set logging flag to be logged */
      ddf_sensor_ptr->factory_cal.used = TRUE;
      ddf_sensor_ptr->full_cal.used = TRUE;
      ddf_sensor_ptr->factory_cal.need_to_log = TRUE;
      ddf_sensor_ptr->full_cal.need_to_log = TRUE;
      cal_db_ptr = &ddf_sensor_ptr->factory_cal;
    }
    else  /* SNS_SMGR_CAL_DYNAMIC_V01 */
    {
      ddf_sensor_ptr->auto_cal.used = TRUE;
      ddf_sensor_ptr->full_cal.used = TRUE;
      ddf_sensor_ptr->auto_cal.need_to_log = TRUE;
      ddf_sensor_ptr->full_cal.need_to_log = TRUE;
      cal_db_ptr = &ddf_sensor_ptr->auto_cal;
    }

    cal_db_ptr->zero_bias_len = req_ptr->ZeroBias_len;
    cal_db_ptr->scale_factor_len = req_ptr->ScaleFactor_len;
    cal_db_ptr->calibration_accuracy = req_ptr->CalibrationAccuracy;
    ddf_sensor_ptr->full_cal.calibration_accuracy = req_ptr->CalibrationAccuracy;

    SNS_SMGR_PRINTF1(HIGH, "calibration - accuracy is %d", 
                     cal_db_ptr->calibration_accuracy);
    for (i=0; i<req_ptr->ScaleFactor_len; i++ )
    {
       if (cal_db_ptr->scale_factor[i] == 0)
       {
          SNS_SMGR_PRINTF1(HIGH, "calibration - scale factor[%d] is 0", i);
          resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
          resp_ptr->Resp.sns_err_t = SENSOR1_EFAILED;
          return;
       }
    }
    for (i=0; i < req_ptr->ZeroBias_len; i++ )
    {
       cal_db_ptr->zero_bias[i] = req_ptr->ZeroBias[i];
       ddf_sensor_ptr->full_cal.zero_bias[i] = ddf_sensor_ptr->factory_cal.zero_bias[i] +
                   ddf_sensor_ptr->auto_cal.zero_bias[i];
    }
    
    for (i=0; i<req_ptr->ScaleFactor_len; i++ )
    {
       cal_db_ptr->scale_factor[i] = req_ptr->ScaleFactor[i];
       ddf_sensor_ptr->full_cal.scale_factor[i] = req_ptr->ScaleFactor[i];
    }
    if (req_ptr->CompensationMatrix_valid)
    {
      cal_db_ptr->compensation_matrix_valid = true;
      for (i = 0; i < SNS_SMGR_COMPENSATION_MATRIX_SIZE_V01; i++)
      {
        cal_db_ptr->compensation_matrix[i] = req_ptr->CompensationMatrix[i];
        SNS_SMGR_PRINTF2(ERROR, "proc_calibration - Matrix[%d] = %d", 
                         i, cal_db_ptr->compensation_matrix[i]);
      }
    }

    err = sns_smgr_set_attr(ddf_sensor_ptr->sensor_ptr,
                            SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
                            SNS_DDF_ATTRIB_BIAS,
                            cal_db_ptr->zero_bias);

    if(err != SNS_DDF_SUCCESS && err != SNS_DDF_EINVALID_PARAM)
    {
      SNS_SMGR_PRINTF2(ERROR, "Failed to set bias (ddf_sensor=%d, err=%d)",
                       SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type), err);
    }

    resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
    resp_ptr->Resp.sns_err_t    = SENSOR1_SUCCESS;
  }
  else
  {
     /* error in request*/
     resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
     resp_ptr->Resp.sns_err_t    = SENSOR1_EBAD_PARAM;
     SNS_SMGR_PRINTF0(ERROR, "proc_calibration - bad param");
  }

  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id = SNS_SMGR_CAL_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_sensor_cal_resp_msg_v01);
  sns_smgr_send_resp( &resp_msg_header,resp_ptr, *msg_header );
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_driver_access_req_msg

===========================================================================*/
/*!
  @brief This function processes the Driver Access message and passes on the
         message to the device driver.

  @detail

  @param[in]  hdr_ptr   request message header pointer
  @param[in]  req_ptr   request pointer
  @param[out] resp_ptr  response pointer
  @return     device-specific test error code
 */
/*=========================================================================*/
static void sns_smgr_proc_driver_access_req_msg (
  const sns_smr_header_s*                   hdr_ptr,
  sns_smgr_driver_access_req_msg_v01*       req_ptr,
  sns_smgr_driver_access_resp_msg_v01*      resp_ptr
)
{
  sns_smr_header_s resp_msg_header;
  sns_ddf_status_e err_code;
  smgr_sensor_s* sensor_ptr;
  smgr_sensor_cfg_s* sensor_const_ptr;

  // Set default response values
  resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
  resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;
  resp_ptr->ResponseStatus_valid = true;
  resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_FAIL_V01;
  resp_ptr->ResponseMsg_valid = false;

  // TODO: Expand on this to allow for Multiple Sensors of the Same Type
  /* Iterate through all of the sensors and check to see if the UUID in
     the request message matches any of the UUIDs for the sensors. If a
     match is found, use that driver
     NOTE: IF THERE ARE MULTIPLE SENSORS WITH THE SAME UUID, ONLY THE
           FIRST SENSOR WILL RECEIVE THE MESSAGE. */
  int i;
  for(i = 0; i < SNS_SMGR_NUM_SENSORS_DEFINED; i++)
  {
    // Compare UUIDs
    sensor_ptr = &sns_smgr.sensor[i];
    sensor_const_ptr = &smgr_sensor_cfg[i];
    if(SNS_OS_MEMCMP(req_ptr->Uuid, sensor_const_ptr->uuid, 16 )==0)
    {
      // Found a matching UUID

      if (SMGR_DRV_FN_PTR(sensor_ptr) == NULL)
      {
        // Matching device driver function pointers for the UUID were not found
        SNS_SMGR_PRINTF0(ERROR, "Matching device driver function pointers for the UUID were not found");
        resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_INVALID_UUID_V01;
        break;
      }

      if (SMGR_DRV_FN_PTR(sensor_ptr)->process_daf_req != NULL)
      {
        void* req_msg = NULL;
        uint32_t req_msg_len = 0;
        uint8_t* trans_id_ptr = NULL;
        uint8_t trans_id;

        /* Determine if the DAF request message should be passed in */
        if (req_ptr->RequestMsg_valid)
        {
          req_msg = (void*) req_ptr->RequestMsg;
          req_msg_len = req_ptr->RequestMsg_len;
        }

        /* Determine if the DAF transaction ID should be passed in */
        if (req_ptr->TransactionId_valid)
        {
          trans_id = req_ptr->TransactionId;
          trans_id_ptr = &trans_id;

          // Fill in the transaction ID in the response message too
          resp_ptr->TransactionId_valid = true;
          resp_ptr->TransactionId = req_ptr->TransactionId;
        }

        // Allocate the memhandler for driver
        sns_ddf_memhandler_init(&sensor_ptr->memhandler);
        void* dd_resp_ptr = NULL;
        uint32_t dd_resp_len = 0;

        // Call the DD's process_daf_req function
        err_code = SMGR_DRV_FN_PTR(sensor_ptr)->process_daf_req(
                       sensor_ptr->dd_handle,
                       req_ptr->RequestId,
                       req_msg,
                       req_msg_len,
                       &sensor_ptr->memhandler,
                       &dd_resp_ptr,
                       &dd_resp_len,
                       trans_id_ptr,
                       hdr_ptr->connection_handle);

        /* Determine if anything is in the DAF response message, if yes, mark
           the field as valid and fill it in */
        if ( (dd_resp_len != 0) && (dd_resp_ptr != NULL) )
        {
          // Check the response size
          if (dd_resp_len > SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01)
          {
            SNS_SMGR_PRINTF2(ERROR, "DAF Response Message is too large! Current size: %d  Max allowable size: %d",
                dd_resp_len, SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01);
            // Truncate the message to fit within the limits
            dd_resp_len = SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01;
          }

          resp_ptr->ResponseMsg_valid = true;
          SNS_OS_MEMCOPY(&resp_ptr->ResponseMsg, dd_resp_ptr, dd_resp_len);
          resp_ptr->ResponseMsg_len = dd_resp_len;
        }

        // Free the memhandler
        sns_ddf_memhandler_free(&sensor_ptr->memhandler);

        switch (err_code)
        {
          case SNS_DDF_SUCCESS:
          {
            SNS_SMGR_PRINTF0(LOW, "SNS_SMGR_DRIVER_ACCESS_STATUS_SUCCESS_V01");
            resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_STATUS_SUCCESS_V01;
            break;
          }
          case SNS_DDF_PENDING:
          {
            SNS_SMGR_PRINTF0(ERROR, "SNS_SMGR_DRIVER_ACCESS_PENDING_V01");
            resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_PENDING_V01;
            break;
          }
          case SNS_DDF_EINVALID_PARAM:
          {
            SNS_SMGR_PRINTF0(ERROR, "SNS_SMGR_DRIVER_ACCESS_INVALID_PARAM_V01");
            resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_INVALID_PARAM_V01;
            break;
          }
          case SNS_DDF_EINVALID_DAF_REQ:
          {
            SNS_SMGR_PRINTF0(ERROR, "SNS_SMGR_DRIVER_ACCESS_INVALID_REQ_V01");
            resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_INVALID_REQ_V01;
            break;
          }
          default:
          {
            SNS_SMGR_PRINTF0(ERROR, "SNS_SMGR_DRIVER_ACCESS_DD_FAILURE_V01");
            resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_DD_FAILURE_V01;
            break;
          }
        }
      }
      else
      {
        // The target device driver does not support DAF
        SNS_SMGR_PRINTF0(ERROR, "process_daf_req is not supported for this DD");
        resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_FAIL_V01;
      }
      break;
    }
  }

  if (i == SNS_SMGR_NUM_SENSORS_DEFINED)
  {
    // Matching device driver for the UUID was not found
    SNS_SMGR_PRINTF0(ERROR, "Matching device driver for the UUID was not found");
    resp_ptr->ResponseStatus = SNS_SMGR_DRIVER_ACCESS_INVALID_UUID_V01;
  }

  // Fill-out the header
  resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
  resp_msg_header.msg_id = SNS_SMGR_DRIVER_ACCESS_RESP_V01;
  resp_msg_header.body_len = sizeof(sns_smgr_driver_access_resp_msg_v01);

  // Send the response
  sns_smgr_send_resp(&resp_msg_header,resp_ptr, *hdr_ptr);
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_req

===========================================================================*/
/*!
  @brief This function fills request message header info then sends the message.

  @detail

  @param[i]  req_msg_header  - request message  header
  @param[i]  req_ptr - request message pointer
  @param[i]  resp_ptr  - response message pointer
  @param[i]  client_handle - client handle
  @return  none
 */
/*=========================================================================*/
void sns_smgr_send_req(sns_smr_header_s *req_msg_header, void *req_ptr, void *resp_ptr, qmi_client_type client_handle)
{
  qmi_txn_handle txn_handle;
  qmi_client_error_type qmi_err;
  uint16_t       req_msg_size=req_msg_header->body_len;
  uint16_t       resp_msg_size = 0;
  void *         resp_cb_data = NULL;

  if (client_handle == smgr_pm_cl_user_handle)
  {
    SMGR_MSG_1(DBG_HIGH_PRIO, "SNS QMI: SMGR sending REQ to PM msg id: %d", req_msg_header->msg_id);
    if (req_msg_header->msg_id == SNS_PM_ACTIVE_PWR_ST_CHANGE_REQ_V01)
    {
      resp_msg_size = sizeof(sns_pm_active_pwr_st_change_resp_msg_v01);
    }
  }
  else if (client_handle == smgr_reg_cl_user_handle)
  {
    SMGR_MSG_1(DBG_HIGH_PRIO, "SNS QMI: SMGR sending REQ to REG msg id: %d", req_msg_header->msg_id);
    if (req_msg_header->msg_id == SNS_REG_GROUP_READ_REQ_V02)
    {
      resp_msg_size = sizeof(sns_reg_group_read_resp_msg_v02);
      resp_cb_data = SNS_OS_MALLOC( SNS_DBG_MOD_DSPS_SMGR, sizeof(uint16) );
      if( resp_cb_data )
      {
        *(uint16_t *)resp_cb_data = ((sns_reg_group_read_req_msg_v02*)req_ptr)->group_id;
      }
    }
    else if (req_msg_header->msg_id == SNS_REG_SINGLE_READ_REQ_V02)
    {
      resp_msg_size = sizeof(sns_reg_single_read_resp_msg_v02 );
    }
    else if (req_msg_header->msg_id == SNS_REG_GROUP_WRITE_REQ_V02)
    {
      resp_msg_size = sizeof(sns_reg_group_write_resp_msg_v02);
    }
    else if (req_msg_header->msg_id == SNS_REG_SINGLE_WRITE_REQ_V02)
    {
      resp_msg_size = sizeof(sns_reg_single_write_resp_msg_v02 );
    }
  }
  else
  {
    SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_SMGR,
                                 DBG_SMGR_GENERIC_STRING1,
                                 11000 );
    return;
  }

  qmi_err = qmi_client_send_msg_async(client_handle,
                            req_msg_header->msg_id,
                            req_ptr,
                            req_msg_size,
                            resp_ptr,
                            resp_msg_size,
                            sns_smgr_client_resp_cb,
                            resp_cb_data,
                            &txn_handle);
  if (QMI_NO_ERR != qmi_err)
  {
    /* If QMI returns an error, free response struct and callback data to prevent memory leaks */
    if (resp_ptr)
    {
      sns_smr_msg_free( resp_ptr );
    }

    if (resp_cb_data)
    {
      SNS_OS_FREE( resp_cb_data );
    }

    SNS_SMGR_PRINTF1(ERROR, "error in qmi_client_send_msg_async: QMI error:%d", qmi_err);
  }
  sns_smr_msg_free( req_ptr );
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_resp

===========================================================================*/
/*!
  @brief This function fills response message header info then sends the message.

  @detail

  @param[i]  msg_header - message header pointer
  @param[i]  resp_ptr - response message pointer
  @param[i]  request_msg_header  - request message  header
  @return  none
 */
/*=========================================================================*/
void sns_smgr_send_resp(sns_smr_header_s* msg_header,
                        void*             resp_ptr,
                        sns_smr_header_s  request_msg_header)
{
  qmi_csi_error qmi_err =
    qmi_csi_send_resp(request_msg_header.req_handle,
                      msg_header->msg_id,
                      (void *)resp_ptr,
                      msg_header->body_len);
  SNS_SMGR_PRINTF2(LOW, "send_resp - msg_id=0x%x err=%d",
                   msg_header->msg_id, qmi_err);
  sns_smr_msg_free( resp_ptr );

}

/*===========================================================================

  FUNCTION:   sns_smgr_send_indication

===========================================================================*/
/*!
  @brief This function populates message header and sends the given indication
   to destination module.

  @detail

  @param[i]  ind_msg_ptr - the indication message to send
  @param[i]  msg_id      - indication ID
  @param[i]  len         - length of the indication body
  @param[i]  connection_handle - client handle
  @return    true if indication was sent
 */
/*=========================================================================*/
bool sns_smgr_send_indication(void* ind_msg_ptr,
                              uint16_t msg_id, uint16_t len,
                              void* connection_handle)
{
  qmi_csi_error err;
  uint8_t attempts = 0;
  do
  {
    if ( attempts != 0 )
    {
      SMGR_DELAY_US(1000);
      SNS_SMGR_PRINTF2(ERROR, "send_ind - msg_id(0x%x) attempts(%d)", 
                       msg_id, attempts);
    }
    err = qmi_csi_send_ind(((client_info_type*)connection_handle)->client_handle,
                           msg_id, ind_msg_ptr, len);
  } while ( (++attempts < 5) && (QMI_CSI_CONN_BUSY == err) );

  if ( QMI_CSI_NO_ERR != err )
  {
    SNS_SMGR_PRINTF2(ERROR, "send_ind - msg_id(0x%x) err(%d)", msg_id, err);
  }

  if ( ind_msg_ptr != (void*)&sns_smgr.report_ind )
  {
    sns_smr_msg_free(ind_msg_ptr);
  }
  return (QMI_CSI_NO_ERR == err) ? true : false;
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_resp_w_err

===========================================================================*/
/*!
  @brief This function sends error response due to unavailable memory resource.

  @detail

  @param[i]   msg_id         - gee i wonder.... what could it be?
  @param[i]   msg_header_ptr - message header pointer
  @param[io]  body_ptr_ptr   - address of message body pointer
  @return  none
 */
/*=========================================================================*/
void sns_smgr_send_resp_w_err (uint16_t msg_id,
                               sns_smr_header_s* msg_header_ptr,
                               void* body_ptr,
                               void *item_ptr)
{
  SNS_SMGR_PRINTF1(ERROR, "send_resp_w_err - dropping msg_id=0x%x", msg_id);
  if ( body_ptr )
  {
    sns_smr_msg_free(body_ptr);
  }
  SNS_OS_FREE( item_ptr );
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_unsupported_msg

===========================================================================*/
/*!
  @brief This function processes unsupported incoming messages for SMGR

  @detail

  @param[i]
            msg_header - message header
            body_ptr  - message body pointer
  @return  true - if response message is allocated
           false - if no response message is allocated
 */
/*=========================================================================*/
bool sns_smgr_process_unsupported_msg(sns_smr_header_s msg_header,uint8_t *body_ptr,uint8_t *item_ptr)
{
  sns_smr_header_s   resp_msg_header;
  uint16_t           msg_body_size=0;
  void              *resp_ptr;
  sns_common_resp_s_v01 *commom_rsp_ptr=NULL;
  switch ( msg_header.msg_id )
  {
     case SNS_SMGR_SENSOR_TEST_REQ_V01:
     {
        msg_body_size = sizeof(sns_smgr_sensor_test_resp_msg_v01);
        resp_ptr = (sns_smgr_sensor_test_resp_msg_v01 *)sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR,msg_body_size);
        if (resp_ptr)
        {
           /* prepare header */
           resp_msg_header.msg_id = SNS_SMGR_SENSOR_TEST_RESP_V01;
           commom_rsp_ptr = &(((sns_smgr_sensor_test_resp_msg_v01 *)resp_ptr)->Resp);
        }
        else
        {
           msg_header.msg_id = SNS_SMGR_SENSOR_TEST_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_SENSOR_POWER_STATUS_REQ_V01:
     {
        msg_body_size = sizeof(sns_smgr_sensor_power_status_resp_msg_v01);
        resp_ptr = (sns_smgr_sensor_power_status_resp_msg_v01 *)sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR,msg_body_size);
        if (resp_ptr)
        {
           /* prepare header */
           resp_msg_header.msg_id = SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01;
           commom_rsp_ptr = &( ((sns_smgr_sensor_power_status_resp_msg_v01 *)resp_ptr)->Resp);
        }
        else
        {
           msg_header.msg_id = SNS_SMGR_SENSOR_POWER_STATUS_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_SENSOR_POWER_CONTROL_REQ_V01:
     {
        msg_body_size = sizeof(sns_smgr_sensor_power_control_resp_msg_v01);
        resp_ptr = (sns_smgr_sensor_power_control_resp_msg_v01 *)sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR,msg_body_size);
        if (resp_ptr)
        {
           /* prepare header */
           resp_msg_header.msg_id = SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01;
           commom_rsp_ptr = &(((sns_smgr_sensor_power_control_resp_msg_v01 *)resp_ptr)->Resp);
        }
        else
        {
           msg_header.msg_id = SNS_SMGR_SENSOR_POWER_CONTROL_RESP_V01;
        }
     }
     break;
     default:
       /* not expected here, free the request message */
       return true;
   }

   if (commom_rsp_ptr)
   {
      /* prepare header */
      resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
      resp_msg_header.priority = SNS_SMR_MSG_PRI_LOW;
      resp_msg_header.body_len = msg_body_size;
      commom_rsp_ptr->sns_result_t=SNS_RESULT_FAILURE_V01;
      commom_rsp_ptr->sns_err_t = SENSOR1_SUCCESS;
      sns_smgr_send_resp(&resp_msg_header,resp_ptr, msg_header);
      return true;
   }
   else
   {
      sns_smgr_send_resp_w_err(msg_header.msg_id, &msg_header, body_ptr, item_ptr);
      return false;
   }
}

/*===========================================================================

  FUNCTION:   sns_smgr_rcv

===========================================================================*/
void* sns_smgr_rcv(void)
{
#if 0
  return sns_smr_rcv(SNS_MODULE_DSPS_SMGR);
#endif
  return sns_smgr_q_get();

}


/*===========================================================================

  FUNCTION:   sns_smgr_get_hdr

===========================================================================*/
sns_err_code_e sns_smgr_get_hdr
(
  sns_smr_header_s * smr_header_ptr, //address of local var in process_msg
  const void *       item_ptr, //from smgr_rcv
  void **            body_ptr
)
{
#if 0
  {
    sns_err_code_e err;

    err = sns_smr_get_hdr(smr_header_ptr, item_ptr);
    msg_header_ptr = smr_header_ptr;
    body_ptr = item_ptr;
    return err;
  }
#endif
  *body_ptr = ((smgr_q_item_s*)item_ptr)->body_ptr;
  SNS_OS_MEMCOPY(smr_header_ptr, &((smgr_q_item_s*)item_ptr)->header, sizeof(sns_smr_header_s));//(to,from,size)
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smgr_restricted_process_msg

===========================================================================*/
/*!
  @brief This function processes incoming smgr restricted messages.

  @param[in] msg_header: Message header
  @param[in] item_ptr: Message items pointer
  @param[in] body_ptr: Message body pointer

  @return none
 */
/*=========================================================================*/
static void sns_smgr_restricted_process_msg(
    sns_smr_header_s  msg_header,
    void              *item_ptr,
    void              *body_ptr)
{
  uint16_t         resp_msg_id = 0xFFFF;
  SNS_SMGR_PRINTF2(MED, "restricted_msg - conn=0x%x msg_id=%d",
                   msg_header.connection_handle, msg_header.msg_id);
  switch ( msg_header.msg_id )
  {
     case SNS_SMGR_RESTRICTED_CANCEL_REQ_V01:
     {
       sns_common_cancel_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_common_cancel_resp_msg_v01);
        if (resp_ptr)
        {
          sns_smgr_cancel_restricted_service(msg_header.connection_handle);
          sns_smr_header_s               resp_msg_header;
          uint16_t                       msg_body_size=0;
          msg_body_size = sizeof(sns_common_cancel_resp_msg_v01);
          resp_msg_header.msg_id = SNS_SMGR_RESTRICTED_CANCEL_RESP_V01;
          resp_msg_header.body_len = msg_body_size;
          resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
          resp_ptr->resp.sns_err_t = SENSOR1_SUCCESS;
          sns_smgr_send_resp(&resp_msg_header,resp_ptr, msg_header);
        }
        else
        {
          resp_msg_id = SNS_SMGR_RESTRICTED_CANCEL_RESP_V01;
        }
        break;
     }
     case SNS_SMGR_RESTRICTED_VERSION_REQ_V01:
     {
        sns_common_version_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_common_version_resp_msg_v01);
        if (resp_ptr)
        {
          sns_smr_header_s               resp_msg_header;
          uint16_t                       msg_body_size=0;
          msg_body_size = sizeof(sns_common_version_resp_msg_v01);
          resp_ptr->max_message_id = SNS_SMGR_RESTRICTED_SVC_V01_MAX_MESSAGE_ID;
          resp_ptr->interface_version_number = SNS_SMGR_RESTRICTED_SVC_V01_IDL_MINOR_VERS;
          resp_msg_header.msg_id = SNS_SMGR_RESTRICTED_VERSION_RESP_V01;
          resp_msg_header.body_len = msg_body_size;
          resp_ptr->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
          resp_ptr->resp.sns_err_t = SENSOR1_SUCCESS;
          sns_smgr_send_resp(&resp_msg_header,resp_ptr, msg_header);
        }
        else
        {
           resp_msg_id = SNS_SMGR_RESTRICTED_VERSION_RESP_V01;
        }
        break;
     }
     case SNS_SMGR_DRIVER_ACCESS_REQ_V01:
     {
       sns_smgr_driver_access_req_msg_v01* req_ptr =
          (sns_smgr_driver_access_req_msg_v01*)(body_ptr);
       sns_smgr_driver_access_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_driver_access_resp_msg_v01);

        SNS_SMGR_PRINTF1(LOW, "driver_access request: request ID %d",
                         req_ptr->RequestId);

        if (resp_ptr)
        {
          sns_smgr_proc_driver_access_req_msg(&msg_header, req_ptr, resp_ptr);
        }
        else
        {
          resp_msg_id = SNS_SMGR_DRIVER_ACCESS_RESP_V01;
        }
        break;
     }
     default:
        resp_msg_id = msg_header.msg_id;
        break;
  }
  if ( resp_msg_id != 0xFFFF )
  {
    sns_smgr_send_resp_w_err( resp_msg_id, &msg_header, body_ptr, item_ptr );
  }
  else
  {
    if ( body_ptr )
    {
      sns_smr_msg_free(body_ptr);
    }
    SNS_OS_FREE(item_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_internal_process_dev_access

===========================================================================*/
/*!
  @brief This function processes incoming device access request

  @param[in] msg_header: Message header
  @param[in] item_ptr: Message items pointer
  @param[in] body_ptr: Message body pointer

  @return   Response message ID
 */
/*=========================================================================*/
uint16_t sns_smgr_internal_process_dev_access(
  sns_smr_header_s msg_header,
  void *item_ptr,
  void *body_ptr,
  sns_smr_header_s req_msg_hdr)
{
  uint16_t resp_msg_id = 0xFFFF;
  uint32_t dev_reg_len;
  uint8_t msg_id = req_msg_hdr.msg_id;
  sns_ddf_io_register_s     dev_reg, *dev_reg_addr;
  smgr_power_state_type_e   prev_power_state;
  sns_smr_header_s  msg_hdr;
  sns_ddf_status_e  err;
  smgr_sensor_s     *sensor_ptr;

  msg_hdr.txn_id = req_msg_hdr.txn_id;

  if(msg_id == SNS_SMGR_INTERNAL_DEV_ACCESS_READ_REQ_V01)
  {
    sns_smgr_internal_dev_access_read_req_msg_v01 *req_ptr = NULL;
    sns_smgr_internal_dev_access_read_resp_msg_v01 *resp_ptr = NULL;
    msg_hdr.msg_id = SNS_SMGR_INTERNAL_DEV_ACCESS_READ_RESP_V01;
    msg_hdr.body_len = sizeof(sns_smgr_internal_dev_access_read_resp_msg_v01);

    req_ptr = (sns_smgr_internal_dev_access_read_req_msg_v01 *) body_ptr;
    sensor_ptr = sns_smgr_find_sensor(req_ptr->SensorId);

    if(sensor_ptr != NULL)
    {
      resp_ptr = (sns_smgr_internal_dev_access_read_resp_msg_v01 *)
         sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, msg_hdr.body_len);
    }

    if(sensor_ptr != NULL && resp_ptr != NULL)
    {
      resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;
      resp_ptr->Result_valid = true;
      resp_ptr->Result = SNS_SMGR_INTERNAL_DEV_ACCESS_RESP_SUCCESS_V01;
      resp_ptr->SensorId_valid = true;
      resp_ptr->SensorId = req_ptr->SensorId;
      resp_ptr->Addr_valid = true;
      resp_ptr->Addr = req_ptr->Addr;
      resp_ptr->Bytes_valid = false;
      resp_ptr->Bytes_len = 0;

      dev_reg.address_ptr = (uint32_t *) ((uint32_t)req_ptr->Addr);
      dev_reg.length = req_ptr->Bytes_len;
      dev_reg.addr_fixed = true;
      dev_reg.data_width = SNS_DDF_IO_DATA_WIDTH_UINT8;
      dev_reg_addr = &dev_reg;

      SNS_SMGR_PRINTF3(MED, "internal_dev_access (read) - sensor_type=%u, addr=%u, len=%u",
                             SMGR_SENSOR_TYPE(sensor_ptr, 0), dev_reg.address_ptr, dev_reg.length);

      /* Switches on device to access register if necessary. */
      prev_power_state = sns_hw_powerrail_status();
      if(prev_power_state != SNS_SMGR_POWER_HIGH)
      {
        sns_hw_power_rail_config(SNS_SMGR_POWER_HIGH);
        SMGR_DELAY_US(SMGR_MAX_OFF_TO_IDLE);
        sns_hw_set_qup_clk(true);
      }
      err = sns_smgr_get_attr(sensor_ptr,
                              SMGR_SENSOR_TYPE(sensor_ptr, 0),
                              SNS_DDF_ATTRIB_IO_REGISTER,
                              (void**) &dev_reg_addr,
                              &dev_reg_len);

      sns_hw_power_rail_config(prev_power_state);
      if(prev_power_state != SNS_SMGR_POWER_HIGH)
      {
        sns_hw_set_qup_clk(false);
      }
      resp_ptr->Result |= ( (err<<8) & 0xFF00 );
      SNS_SMGR_PRINTF3(MED, "internal_dev_access (read) -> err=%u, data=%u, dev_reg_len=%u",
                            err, dev_reg_addr->data[0], dev_reg_len);

      /* Sends response message. */
      if(err == SNS_DDF_SUCCESS)
      {
        resp_ptr->Bytes_valid = true;
        resp_ptr->Bytes_len = dev_reg_addr->length;
        SNS_OS_MEMCOPY(resp_ptr->Bytes, dev_reg_addr->data, resp_ptr->Bytes_len);
      }

      sns_smgr_send_resp(&msg_hdr, resp_ptr, msg_header);
    }
    else
    {
      resp_msg_id = SNS_SMGR_INTERNAL_DEV_ACCESS_READ_RESP_V01;
    }
  }
  else if(msg_id == SNS_SMGR_INTERNAL_DEV_ACCESS_WRITE_REQ_V01)
  {
    sns_smgr_internal_dev_access_write_req_msg_v01 *req_ptr = NULL;
    sns_smgr_internal_dev_access_write_resp_msg_v01 *resp_ptr = NULL;
    msg_hdr.msg_id = SNS_SMGR_INTERNAL_DEV_ACCESS_WRITE_RESP_V01;
    msg_hdr.body_len = sizeof(sns_smgr_internal_dev_access_write_resp_msg_v01);

    req_ptr = (sns_smgr_internal_dev_access_write_req_msg_v01 *) body_ptr;
    sensor_ptr = sns_smgr_find_sensor(req_ptr->SensorId);

    if(sensor_ptr != NULL)
    {
      resp_ptr = (sns_smgr_internal_dev_access_write_resp_msg_v01 *)
         sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, msg_hdr.body_len);
    }

    if(sensor_ptr != NULL && resp_ptr != NULL)
    {
      resp_ptr->Resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
      resp_ptr->Resp.sns_err_t = SENSOR1_SUCCESS;
      resp_ptr->Result_valid = true;
      resp_ptr->Result = SNS_SMGR_INTERNAL_DEV_ACCESS_RESP_SUCCESS_V01;
      resp_ptr->SensorId_valid = true;
      resp_ptr->SensorId = req_ptr->SensorId;

      dev_reg.address_ptr = (uint32_t*) ((uint32_t)req_ptr->Addr);
      dev_reg.length = req_ptr->Bytes_len;
      dev_reg.addr_fixed = true;
      dev_reg.data_width = SNS_DDF_IO_DATA_WIDTH_UINT8;
      SNS_OS_MEMCOPY(dev_reg.data, req_ptr->Bytes, dev_reg.length);

      SNS_SMGR_PRINTF3(MED, "internal_dev_access (write) - sensor_type=%u, addr=%u, len=%u",
                             SMGR_SENSOR_TYPE(sensor_ptr, 0), dev_reg.address_ptr, dev_reg.length);

      /* Switches on device to access register if necessary. */
      prev_power_state = sns_hw_powerrail_status();
      if(prev_power_state != SNS_SMGR_POWER_HIGH)
      {
        sns_hw_power_rail_config(SNS_SMGR_POWER_HIGH);
        SMGR_DELAY_US(SMGR_MAX_OFF_TO_IDLE);
        sns_hw_set_qup_clk(true);
      }
      err = sns_smgr_set_attr(sensor_ptr,
                              SMGR_SENSOR_TYPE(sensor_ptr, 0),
                              SNS_DDF_ATTRIB_IO_REGISTER,
                              &dev_reg);

      sns_hw_power_rail_config(prev_power_state);
      if(prev_power_state != SNS_SMGR_POWER_HIGH)
      {
        sns_hw_set_qup_clk(false);
      }

      SNS_SMGR_PRINTF1(MED, "internal_dev_access (write) -> err=%u", err);

      resp_ptr->Result |= ( (err<<8) & 0xFF00 );
      if(err != SNS_DDF_SUCCESS)
      {
        resp_ptr->Resp.sns_result_t = SNS_RESULT_FAILURE_V01;
      }

      sns_smgr_send_resp(&msg_hdr, resp_ptr, msg_header);
    }
    else
    {
      resp_msg_id = SNS_SMGR_INTERNAL_DEV_ACCESS_WRITE_RESP_V01;
    }
  }
  else
  {
    resp_msg_id = req_msg_hdr.msg_id;
  }

  return resp_msg_id;
}

/*===========================================================================

  FUNCTION:   sns_smgr_internal_process_msg

===========================================================================*/
/*!
  @brief This function processes incoming smgr internal messages.

  @param[in] msg_header: Message header
  @param[in] item_ptr: Message items pointer
  @param[in] body_ptr: Message body pointer

  @return none
 */
/*=========================================================================*/
void sns_smgr_internal_process_msg(sns_smr_header_s msg_header, void *item_ptr, void *body_ptr)
{
  uint16_t         resp_msg_id = 0xFFFF;
  SNS_SMGR_PRINTF2(MED, "internal_msg - conn=0x%x msg_id=%d",
                   msg_header.connection_handle, msg_header.msg_id);
  switch ( msg_header.msg_id )
  {
     case SNS_SMGR_REG_HW_MD_INT_REQ_V01:
     {
        resp_msg_id = sns_smgr_internal_process_md(msg_header, item_ptr, body_ptr);
     }
     break;
     case SNS_SMGR_INTERNAL_DEV_ACCESS_READ_REQ_V01:
     case SNS_SMGR_INTERNAL_DEV_ACCESS_WRITE_REQ_V01:
     {
        resp_msg_id = sns_smgr_internal_process_dev_access(msg_header, item_ptr, body_ptr, msg_header);
     }
     break;
     case SNS_SMGR_INTERNAL_CANCEL_REQ_V01:
     {
        sns_common_cancel_resp_msg_v01  *Rsp_p;
        sns_smr_header_s               resp_msg_header;
        uint16_t                       msg_body_size=0;
        msg_body_size = sizeof(sns_common_cancel_resp_msg_v01);
        Rsp_p = (sns_common_cancel_resp_msg_v01 *)sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR,msg_body_size);
        if (Rsp_p)
        {
           sns_smgr_cancel_internal_service(msg_header.connection_handle);
           resp_msg_header.msg_id = SNS_SMGR_CANCEL_RESP_V01;
           resp_msg_header.body_len = msg_body_size;
           Rsp_p->resp.sns_result_t=SNS_RESULT_SUCCESS_V01;
           Rsp_p->resp.sns_err_t = SENSOR1_SUCCESS;
           sns_smgr_send_resp(&resp_msg_header,Rsp_p, msg_header);
        }
        else
        {
           resp_msg_id = SNS_SMGR_CANCEL_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_INTERNAL_VERSION_REQ_V01:
     {
        sns_common_version_resp_msg_v01  *Rsp_p;
        sns_smr_header_s               resp_msg_header;
        uint16_t                       msg_body_size=0;
        msg_body_size = sizeof(sns_common_version_resp_msg_v01);
        Rsp_p = (sns_common_version_resp_msg_v01 *)sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR,msg_body_size);
        if (Rsp_p)
        {
           Rsp_p->max_message_id = SNS_SMGR_INTERNAL_SVC_V01_MAX_MESSAGE_ID;
           Rsp_p->interface_version_number = SNS_SMGR_INTERNAL_SVC_V01_IDL_MINOR_VERS; /* JohnL needed for target */
           resp_msg_header.msg_id = SNS_SMGR_VERSION_RESP_V01;
           resp_msg_header.body_len = msg_body_size;
           Rsp_p->resp.sns_result_t = SNS_RESULT_SUCCESS_V01;
           Rsp_p->resp.sns_err_t = SENSOR1_SUCCESS;
           sns_smgr_send_resp(&resp_msg_header,Rsp_p, msg_header);
        }
        else
        {
           resp_msg_id = SNS_SMGR_VERSION_RESP_V01;
        }
     }
     break;
     default:
        resp_msg_id = msg_header.msg_id;
        break;
  }
  if ( resp_msg_id != 0xFFFF )
  {
    sns_smgr_send_resp_w_err( resp_msg_id, &msg_header, body_ptr, item_ptr );
  }
  else
  {
    if ( body_ptr )
    {
      sns_smr_msg_free(body_ptr);
    }
    SNS_OS_FREE(item_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_msg

===========================================================================*/
/*!
  @brief This function processes incoming messages for SMGR

  @detail

  @param[i]  item_ptr  - message item pointer
  @return  none
 */
/*=========================================================================*/
static void sns_smgr_process_msg(uint8_t* item_ptr)
{
  sns_smr_header_s msg_header;
  void             *body_ptr;
  uint16_t         resp_msg_id = 0xFFFF;
  uint32_t         msg_history_idx;
  #ifdef CONFIG_DRI_UNIT_TEST
  static int run_test = 10;
  #endif

  (void)sns_smgr_get_tick64();
  msg_history_idx = sns_smgr.msg_history.num_rcvd_msgs %
    ARR_SIZE(sns_smgr.msg_history.msg_contents);
  sns_smgr.msg_history.msg_contents[msg_history_idx].timestamp =
    sns_smgr.last_tick.u.low_tick;
  sns_smgr_get_hdr(&sns_smgr.msg_history.msg_contents[msg_history_idx].msg_header,
                   item_ptr, &body_ptr);
  if ( body_ptr != NULL )
  {
    SNS_OS_MEMCOPY(sns_smgr.msg_history.msg_contents[msg_history_idx].body, body_ptr,
                   sizeof(sns_smgr.msg_history.msg_contents[msg_history_idx].body));
  }
  sns_smgr.msg_history.num_rcvd_msgs++;

  sns_smgr_get_hdr(&msg_header, item_ptr, &body_ptr);

  SNS_SMGR_PRINTF3(MED, "process_msg - conn=0x%x svc=%d msg=0x%x",
                   msg_header.connection_handle, msg_header.svc_num,
                   msg_header.msg_id);
  SNS_SMGR_PRINTF1(MED, "process_msg - time_now=%u", sns_smgr.last_tick.u.low_tick);

  if (((smgr_q_item_s*)item_ptr)->smgr_msg_type == SNS_SMGR_MSG_INTERNAL)
  {
    sns_smgr_internal_process_msg(msg_header, item_ptr, body_ptr);
    return;
  }

  if (((smgr_q_item_s*)item_ptr)->smgr_msg_type == SNS_SMGR_MSG_RESTRICTED)
  {
    sns_smgr_restricted_process_msg(msg_header, item_ptr, body_ptr);
    return;
  }

  if ( SNS_REG2_SVC_ID_V01 == msg_header.svc_num )
  {
    /* Response from register read request of group or single item */
    if ( SNS_REG_GROUP_READ_RESP_V02 == msg_header.msg_id )
    {
#if defined(FEATURE_TEST_DRI)
      if ( sns_test_dri_init_done() == FALSE )
      {
        sns_test_dri_init(&body_ptr, &msg_header);
        if ( body_ptr == NULL ) /* message consumed by test code? */
        {
          return;
        }
      }
#endif /* defined(FEATURE_TEST_DRI) */

      sns_smgr_process_reg_data( SNS_SMGR_REG_ITEM_TYPE_GROUP,
        ((sns_reg_group_read_resp_msg_v02*)body_ptr)->group_id,
        ((sns_reg_group_read_resp_msg_v02*)body_ptr)->data_len,
        ((sns_reg_group_read_resp_msg_v02*)body_ptr)->data,
        ((sns_reg_group_read_resp_msg_v02*)body_ptr)->resp );
    }
    else if ( SNS_REG_SINGLE_READ_RESP_V02 == msg_header.msg_id )
    {
      sns_smgr_process_reg_data( SNS_SMGR_REG_ITEM_TYPE_SINGLE,
        ((sns_reg_single_read_resp_msg_v02*)body_ptr)->item_id,
        ((sns_reg_single_read_resp_msg_v02*)body_ptr)->data_len,
        ((sns_reg_single_read_resp_msg_v02*)body_ptr)->data,
        ((sns_reg_single_read_resp_msg_v02*)body_ptr)->resp );
    }
    else  /* response from register write */
    {
      SNS_PRINTF_STRING_ID_MEDIUM_2(SNS_DBG_MOD_DSPS_SMGR,
                                    DBG_SMGR_GENERIC_STRING2,
                                    88,
                                    msg_header.msg_id);
    }
    sns_smr_msg_free(body_ptr);
    SNS_OS_FREE(item_ptr);
    return;
  }

  if ( SNS_PM_SVC_ID_V01 == msg_header.svc_num)
  {
     if(msg_header.msg_type == QMI_IDL_INDICATION)
     {
       if (SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01 == msg_header.msg_id)
       {
         SNS_SMGR_PRINTF0(HIGH, "SMGR : received SNS_PM_ACTIVE_PWR_ST_CHANGE_IND_V01");
       }
       else
       {
         SNS_SMGR_PRINTF0(HIGH, "SMGR : received Error PM indication");
       }
     }
     else if (msg_header.msg_type == QMI_IDL_RESPONSE)
     {
       if( SNS_PM_ACTIVE_PWR_ST_CHANGE_RESP_V01 == msg_header.msg_id)
       {
         SNS_SMGR_PRINTF0(HIGH, "SMGR : received SNS_PM_ACTIVE_PWR_ST_CHANGE_RESP_V01");
       }
       else
       {
         SNS_SMGR_PRINTF0(HIGH, "SMGR : received Error PM response");
       }
     }
     else
     {
       SNS_SMGR_PRINTF0(HIGH, "SMGR : received Error PM invalid message");
     }
     sns_smr_msg_free(body_ptr);
     SNS_OS_FREE(item_ptr);
     return;
  }

  switch ( msg_header.msg_id )
  {
     case SNS_SMGR_REPORT_REQ_V01:
     {
        sns_smgr_periodic_report_req_msg_v01   *req_ptr =
          (sns_smgr_periodic_report_req_msg_v01*)body_ptr;
        sns_smgr_periodic_report_resp_msg_v01  *resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_periodic_report_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_proc_periodic_req_msg(&msg_header, req_ptr, resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_REPORT_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_BUFFERING_REQ_V01:
     {
       sns_smgr_buffering_req_msg_v01*  req_ptr =
         (sns_smgr_buffering_req_msg_v01*) body_ptr;
       sns_smgr_buffering_resp_msg_v01* resp_ptr =
         SMGR_MSG_ALLOC(sns_smgr_buffering_resp_msg_v01);
       if ( resp_ptr != NULL )
       {
          sns_smgr_proc_buffering_req_msg(&msg_header, req_ptr, resp_ptr);
       }
       else
       {
          resp_msg_id = SNS_SMGR_BUFFERING_RESP_V01;
       }
       break;
     }
      case SNS_SMGR_BUFFERING_QUERY_REQ_V01:
      {
        sns_smgr_buffering_query_req_msg_v01*  req_ptr =
          (sns_smgr_buffering_query_req_msg_v01*) body_ptr;
        sns_smgr_buffering_query_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_buffering_query_resp_msg_v01);
        if ( resp_ptr != NULL )
        {
           sns_smgr_proc_buffering_query_req_msg(&msg_header, req_ptr, resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_BUFFERING_QUERY_RESP_V01;
        }
        break;
      }
     case SNS_SMGR_CANCEL_REQ_V01:
     {
        sns_common_cancel_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_common_cancel_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_proc_cancel_service_req_msg(&msg_header, resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_CANCEL_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_VERSION_REQ_V01:
     {
        sns_common_version_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_common_version_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_proc_version_req_msg(&msg_header, resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_VERSION_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_CAL_REQ_V01:
     {
        sns_smgr_sensor_cal_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_sensor_cal_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_proc_calibration(&msg_header,
                                     (sns_smgr_sensor_cal_req_msg_v01*)body_ptr,
                                     resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_CAL_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_ALL_SENSOR_INFO_REQ_V01:
     {
        sns_smgr_all_sensor_info_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_all_sensor_info_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_proc_all_sensor_info_req(&msg_header, resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_ALL_SENSOR_INFO_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_SINGLE_SENSOR_INFO_REQ_V01:
     {
        sns_smgr_single_sensor_info_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_single_sensor_info_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_single_sensor_info_fill(
             &msg_header,
             ((sns_smgr_single_sensor_info_req_msg_v01*)body_ptr)->SensorID,
             resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_SINGLE_SENSOR_INFO_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_SENSOR_STATUS_REQ_V01:
     {
        sns_smgr_sensor_status_req_msg_v01*  req_ptr =
          (sns_smgr_sensor_status_req_msg_v01*)(body_ptr);
        sns_smgr_sensor_status_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_sensor_status_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_proc_sensor_status_req_msg(&msg_header, req_ptr, resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_SENSOR_STATUS_RESP_V01;
        }
     }
     break;
     case SNS_SMGR_SINGLE_SENSOR_TEST_REQ_V01:
     {
        sns_smgr_single_sensor_test_resp_msg_v01* resp_ptr =
          SMGR_MSG_ALLOC(sns_smgr_single_sensor_test_resp_msg_v01);
        if (resp_ptr)
        {
           sns_smgr_proc_single_test_req_msg(
             &msg_header,
             (sns_smgr_single_sensor_test_req_msg_v01*)body_ptr,
             resp_ptr);
        }
        else
        {
           resp_msg_id = SNS_SMGR_SINGLE_SENSOR_TEST_RESP_V01;
        }
        break;
     }
     case SNS_SMGR_SENSOR_TEST_REQ_V01:
     case SNS_SMGR_SENSOR_POWER_STATUS_REQ_V01:
     case SNS_SMGR_SENSOR_POWER_CONTROL_REQ_V01:
     {
        if ( false == sns_smgr_process_unsupported_msg(msg_header, body_ptr, item_ptr ))
        {
          /* no need to free message */
          return;
        }

     }
     break;
     default:
     {
        resp_msg_id = msg_header.msg_id;
        break;
     }
  }

  if ( resp_msg_id != 0xFFFF )
  {
    sns_smgr_send_resp_w_err(resp_msg_id, &msg_header, body_ptr, item_ptr);
  }
  else
  {
    if ( body_ptr )
    {
      sns_smr_msg_free(body_ptr);
    }
    SNS_OS_FREE(item_ptr);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_dd_service

===========================================================================*/
/*!
  @brief This function Enter the device driver framework for each queued DD
         timer expiration or IRQ handling

  @detail

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
static void sns_smgr_dd_service(void)
{
  if(sns_hw_powerrail_status() != SNS_SMGR_POWER_OFF)
  {
    sns_hw_set_qup_clk(true);

    sns_ddf_signal_dispatch();
#ifndef SNS_QDSP_SIM  
    while ( sns_smgr.timer_que_out != sns_smgr.timer_que_in )
    {
      /* turn on qup clock now and turn off after sns_ddf_smgr_notify_data */
      sns_ddf_timer_dispatch((sns_ddf_timer_s)sns_smgr.timer_que[sns_smgr.timer_que_out]);
      sns_smgr.timer_que_out++;
      if ( sns_smgr.timer_que_out >= SNS_SMGR_TIMER_QUEUE_SIZE )
      {
         sns_smgr.timer_que_out = 0;
      }
    }
#else
    {
      uint8_t err=0; 
      sns_os_sigs_post (sns_dri_sim.sns_dri_sig_grp, SNS_DRI_SIMULATE, OS_FLAG_SET, &err );
    }
#endif //SNS_QDSP_SIM  
    
    sns_hw_set_qup_clk(false);
  }
  else
  {
    SNS_SMGR_PRINTF0(ERROR, "Spurious signal detected by SMGR");
  }
}

//uint32 TimeStampDD[SNS_SMGR_TIMER_QUEUE_SIZE];
/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_timer

===========================================================================*/
/*!
  @brief DDF, device driver call back. Entered on interrupt.
    Queue the argument

  @detail

  @param[i]  timer
  @return   none
 */
/*=========================================================================*/
void sns_ddf_smgr_notify_timer(void* timer)
{
  int32_t dif;

  // TimeStampDD[sns_smgr.timer_que_in] = sns_em_get_timestamp();
  if ( (dif = sns_smgr.timer_que_out - sns_smgr.timer_que_in) < 0 )
  {
    dif += SNS_SMGR_TIMER_QUEUE_SIZE;
  }
  /* Queue only if that will not use up the last empty slot */
  if ( 1 != dif )
  {
    sns_smgr.timer_que[sns_smgr.timer_que_in++] = timer;
  }
  if ( sns_smgr.timer_que_in >= SNS_SMGR_TIMER_QUEUE_SIZE )
  {
    sns_smgr.timer_que_in = 0;
  }
  sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_DD_CB_FLAG, OS_FLAG_SET, &err);
}

/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_timer

===========================================================================*/
/*!
  @brief DDF, device driver call back. Entered on interrupt.
    Queue the argument

  @detail  After Driver registers callback with DDF.
    ISR calls sns_ddf_smgr_notify_irq, irq gets queued
    sns_smgr_dd_service gets called in SMGR context, and calls sns_ddf_signal_dispatch()
    DDF calls the driver's registered callback
    Finally, if the driver has an event (like motion detect)
    it'll call sns_ddf_smgr_notify_event to let SMGR know

  @param[i]  param  interrupt param
  @return   none
 */
/*=========================================================================*/
void sns_ddf_smgr_notify_irq(uint32_t param)
{
  sns_profiling_log_qdss(SNS_SMGR_NOTIFY_IRQ, 1, param);
  sns_ddf_signal_post(param);

  #if defined(FEATURE_TEST_DRI)
  sns_test_dri_timestamp_delay();
  #endif /* defined(FEATURE_TEST_DRI) */

  // Notify the SMGR task that a driver interrupt requires handling.
  sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_DD_CB_FLAG, OS_FLAG_SET, &err);

}

/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_daf_ind

===========================================================================*/
/**
 * @brief Notifies a Sensor1 client of a Driver Access message from the driver.
 *        Asynchronous API.
 *
 * @param[in]  ind_id        Indication identifier.
 * @param[in]  ind_msg       Pointer to the Indication message. This is allocated
 *                           by the device driver and must be freed by the driver
 *                           upon returning from this function!
 * @param[in]  ind_size      The number of bytes in @ind_msg. If there is no
 *                           indication message for the indication, then this
 *                           will be 0 to show that the DAF indication is not
 *                           present. This cannot be larger than
 *                           @SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01 bytes. If it is
 *                           the indication message will be truncated.
 * @param[in]  trans_id_ptr  Pointer to the transaction identifier. If there is
                             no transaction ID, this field will be null.
 * @param[in]  conn_handle   The connection handle for the request message.
 *                           This value must be saved by the device driver if the
 *                           corresponding request is expected to generate
 *                           indications. Upon notifying the SMGR of an
 *                           indication, this value must be provided to the SMGR
 *                           so the SMGR can forward the indication to the client.
 *
 * @return Success if the message was correctly sent. Otherwise a specific
 *         error code is returned.
 */
/*=========================================================================*/
sns_ddf_status_e sns_ddf_smgr_notify_daf_ind(
    uint32_t               ind_id,
    const void*            ind_msg,
    uint32_t               ind_size,
    const uint8_t*         trans_id_ptr,
    void*                  conn_handle)
{
  uint32_t indication_len = ind_size;
  sns_ddf_status_e status = SNS_DDF_EFAIL;

  SNS_SMGR_PRINTF2(MED, "DAF Indication ID: %d  size: %d", ind_id, ind_size);

  if (conn_handle == NULL)
  {
    SNS_SMGR_PRINTF0(ERROR, "DAF Indication NULL connection handle error!");
    return SNS_DDF_EINVALID_PARAM;
  }

  // Allocate the indication message
  sns_smgr_driver_access_ind_msg_v01* ind_ptr =
    SMGR_MSG_ALLOC(sns_smgr_driver_access_ind_msg_v01);

  if (ind_ptr == NULL)
  {
    return SNS_DDF_ENOMEM;
  }

  // Fill out the indication message fields
  ind_ptr->IndicationId = ind_id;

  if (indication_len > 0)
  {
    ind_ptr->IndicationMsg_valid = true;

    // Check for overflow condition
    if (indication_len > SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01)
    {
      // The indication message is TOO BIG! Truncate to 256 bytes.
      // TODO: CONSIDER ADDING AN ERROR CODE FOR "MESSAGE TOO LARGE FOR BUFFER"
      SNS_SMGR_PRINTF2(ERROR, "Indication Message is too large! Current size: %d  Max allowable size: %d", ind_size, SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01);
      indication_len = SNS_SMGR_MAX_DAF_MESSAGE_SIZE_V01;
    }
    ind_ptr->IndicationMsg_len = indication_len;
    SNS_OS_MEMCOPY(ind_ptr->IndicationMsg, ind_msg, indication_len);
  }
  else
  {
    ind_ptr->IndicationMsg_valid = false;
  }

  if (trans_id_ptr != NULL)
  {
    ind_ptr->TransactionId_valid = true;
    ind_ptr->TransactionId = *trans_id_ptr;
  }
  else
  {
    ind_ptr->TransactionId_valid = false;
  }

  // Send the indication message
  sns_smgr_send_indication(ind_ptr,
                           SNS_SMGR_DRIVER_ACCESS_IND_V01,
                           sizeof(sns_smgr_driver_access_ind_msg_v01),
                           conn_handle);
  status = SNS_DDF_SUCCESS;

  SNS_SMGR_PRINTF1(MED, "DAF Indication sent. status = %d", status);

  return status;
}

/*===========================================================================

  FUNCTION:   sns_smgr_timer_cb

===========================================================================*/
/*!
  @brief timer call back. Entered on interrupt

  @detail

  @param[i]  cb_arg  callback argument
  @return   none
 */
/*=========================================================================*/
void sns_smgr_timer_cb(void *cb_arg)
{
  if( sns_latency.ltcy_measure_enabled )
  {
    //latency measurement under polling mode
    sns_latency.polling_cb_ts = sns_em_get_timestamp();
  }
  sns_profiling_log_qdss(SNS_SMGR_TIMER_CB, 0);
  /* TODO  set signal for timer (generic signal is for test driver */
  sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_SENSOR_READ_FLAG, OS_FLAG_SET, &err);
}

/*===========================================================================

  FUNCTION:   sns_smgr_sensor_ev_timer_cb

===========================================================================*/
/*!
  @brief Sensor event timer call back

  @param[i]  cb_arg  callback argument
  @return   none
 */
/*=========================================================================*/
void sns_smgr_sensor_ev_timer_cb(void *cb_arg)
{
  uint8_t       os_err;
  sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_SENSOR_STATE_EV, OS_FLAG_SET, &os_err);
}

/*  =============== Set scheduling flags from task functions ===============
*/

/*===========================================================================

  FUNCTION:   sns_smgr_set_data_gathering_cycle_flag

===========================================================================*/
/*!
  @brief set data_gathering_cycle_flag

  @detail

  @param[i]  none
  @return   none
 */
/*=========================================================================*/
void sns_smgr_set_data_gathering_cycle_flag(void)
{
  SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DATA_GATHERING_B);
}

/*===========================================================================

  FUNCTION:   smgr_log_calibration

===========================================================================*/
/*!
  @brief calibrate the sampled data and save them into their saving location

  @Detail

  @param[i] sensor_ptr  A sensor leader sturcture that holds information for a sensor
  @param[i] data_type Primary or Secondary

  @return
   NONE
*/
/*=========================================================================*/
static void smgr_log_calibration(q16_t sample_data[],
                                 smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  if ( SMGR_SENSOR_IS_GYRO(ddf_sensor_ptr->sensor_ptr) &&
       (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == ddf_sensor_ptr->data_type) &&
       (TRUE == ddf_sensor_ptr->auto_cal.need_to_log) )
  {
    /* gyro logpacket */
    sns_err_code_e retval;
    sns_log_gyro_cal_report_s* log_struct_ptr;
    retval = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_REPORT,
               sizeof(sns_log_gyro_cal_report_s),(void**)&log_struct_ptr);
    if ( retval == SNS_SUCCESS )
    {
      log_struct_ptr->version = SNS_LOG_GYRO_CAL_REPORT_VERSION;
      log_struct_ptr->timestamp = sns_em_get_timestamp();
      log_struct_ptr->gyro_x = sample_data[0];
      log_struct_ptr->gyro_y = sample_data[1];
      log_struct_ptr->gyro_z = sample_data[2];
      log_struct_ptr->gyro_zero_bias_corr_x = ddf_sensor_ptr->auto_cal.zero_bias[0];
      log_struct_ptr->gyro_zero_bias_corr_y = ddf_sensor_ptr->auto_cal.zero_bias[1];
      log_struct_ptr->gyro_zero_bias_corr_z = ddf_sensor_ptr->auto_cal.zero_bias[2];
      //Commit log (also frees up the log packet memory)
      retval = sns_logpkt_commit(SNS_LOG_GYRO_CAL_REPORT,log_struct_ptr);
    }
    /* clear the flag at any case */
    ddf_sensor_ptr->auto_cal.need_to_log = FALSE;
  }
  else if (SMGR_SENSOR_IS_MAG(ddf_sensor_ptr->sensor_ptr) &&
       (SNS_SMGR_DATA_TYPE_PRIMARY_V01 == ddf_sensor_ptr->data_type) &&
       (TRUE == ddf_sensor_ptr->auto_cal.need_to_log)) 
  {
	sns_err_code_e err;
	sns_log_qmag_cal_report_s* log_qmag_struct_ptr;   
	err = sns_logpkt_malloc(SNS_LOG_GYRO_CAL_REPORT,
               sizeof(sns_log_qmag_cal_report_s),(void**)&log_qmag_struct_ptr);

	if ( err == SNS_SUCCESS )
    {
	  log_qmag_struct_ptr->version = SNS_LOG_QMAG_CAL_REPORT_VERSION;
	  log_qmag_struct_ptr->timestamp_type = SNS_TIMESTAMP_DSPS;
	  log_qmag_struct_ptr->timestamp = sns_em_get_timestamp();
	  log_qmag_struct_ptr->qmag_x = sample_data[0];
	  log_qmag_struct_ptr->qmag_y = sample_data[1];
	  log_qmag_struct_ptr->qmag_z = sample_data[2];
	  log_qmag_struct_ptr->qmag_zero_bias_corr_x = ddf_sensor_ptr->auto_cal.zero_bias[0];
	  log_qmag_struct_ptr->qmag_zero_bias_corr_y = ddf_sensor_ptr->auto_cal.zero_bias[1];
	  log_qmag_struct_ptr->qmag_zero_bias_corr_z = ddf_sensor_ptr->auto_cal.zero_bias[2];
	  //Commit log (also frees up the log packet memory)
	  err = sns_logpkt_commit(SNS_LOG_GYRO_CAL_REPORT,log_qmag_struct_ptr);
    }
    /* clear the flag at any case */
    ddf_sensor_ptr->auto_cal.need_to_log = FALSE;
  }
}

//void smgr_calibrate_cm_and_bias(q16_t *sample, q16_t *cm q16_t *zb)
void smgr_calibrate_cm_and_bias(int32_t *sample, int32_t *cm, int32_t *zb)
{
  q16_t temp[3];
  /* Get rid of bias, Note for legacy reasons, bias is additive*/
  sample[0] += zb[0];
  sample[1] += zb[1];
  sample[2] += zb[2];

  /* Apply CM */
  temp[0] = FX_MUL_Q16(cm[0], sample[0]) +  FX_MUL_Q16(cm[1], sample[1]) +  FX_MUL_Q16(cm[2], sample[2]);
  temp[1] = FX_MUL_Q16(cm[3], sample[0]) +  FX_MUL_Q16(cm[4], sample[1]) +  FX_MUL_Q16(cm[5], sample[2]);
  temp[2] = FX_MUL_Q16(cm[6], sample[0]) +  FX_MUL_Q16(cm[7], sample[1]) +  FX_MUL_Q16(cm[8], sample[2]);

  /* Copy back to sample */
  sample[0] = temp[0];
  sample[1] = temp[1];
  sample[2] = temp[2];
}

/*===========================================================================

  FUNCTION:   smgr_apply_calibration

===========================================================================*/
/*!
  @brief This function apply the calibratin data into the sampled data

  @detail

  @param[io]  sample_data the sampled data to be calibrated
  @param[i]   sensor_ptr A sensor leader sturcture that holds information for a sensor
  @param[i]   data_type sensor data type information either primary or secondary
  @param[i]   cal_sel calibration selection flag
  @return     The calibration applied flags
 */
/*=========================================================================*/
static uint8_t smgr_apply_calibration(q16_t sample_data[],
                                      smgr_ddf_sensor_s* ddf_sensor_ptr,
                                      uint8_t cal_sel)
{
  uint8_t  cal_applied_flags = 0;
  uint32_t i;
  smgr_cal_s *cal_data_ptr = NULL;

  if ( (SNS_SMGR_CAL_SEL_FULL_CAL_V01 == cal_sel) &&
       (TRUE == ddf_sensor_ptr->full_cal.used) )
  {
    cal_data_ptr = &ddf_sensor_ptr->full_cal;
    if ( TRUE == ddf_sensor_ptr->factory_cal.used )
    {
      cal_applied_flags |= SNS_SMGR_ITEM_FLAG_FAC_CAL_V01;
    }
    if ( TRUE == ddf_sensor_ptr->auto_cal.used )
    {
      cal_applied_flags |= SNS_SMGR_ITEM_FLAG_AUTO_CAL_V01;
    }
  }
  else if ( (SNS_SMGR_CAL_SEL_FACTORY_CAL_V01 == cal_sel) &&
            (TRUE == ddf_sensor_ptr->factory_cal.used) )
  {
    cal_data_ptr = &ddf_sensor_ptr->factory_cal;
    if ( TRUE == ddf_sensor_ptr->factory_cal.used )
    {
      cal_applied_flags |= SNS_SMGR_ITEM_FLAG_FAC_CAL_V01;
    }
  }
  if ( NULL != cal_data_ptr )
  {
    if ( TRUE == cal_data_ptr->need_to_log )
    {
      smgr_log_calibration(sample_data, ddf_sensor_ptr);
      cal_data_ptr->need_to_log = FALSE;
    }

    if (cal_data_ptr->compensation_matrix_valid)
    {
      /* Compensation Matrix and bias*/
      //SNS_SMGR_PRINTF0(ERROR, " Compensation Matrix and bias");
      smgr_calibrate_cm_and_bias(sample_data, 
                                 cal_data_ptr->compensation_matrix, 
                                 cal_data_ptr->zero_bias);
    }
    else
    {
	  /* Only Bias and Scale Factor */
       for ( i=0; i < SMGR_MAX_VALUES_PER_DATA_TYPE ; i++ )
       {
         //SNS_SMGR_PRINTF1(ERROR, " Only Bias and Scale Factor BEFORE %d",sample_data[i]);
         // SNS_SMGR_PRINTF2(ERROR, " Zero Bias %d SF %d",cal_data_ptr->zero_bias[i],cal_data_ptr->scale_factor[i]);
         sample_data[i] = SMGR_CALIBRATE_BIAS_AND_GAIN(sample_data[i],
                                      cal_data_ptr->scale_factor[i],
                                      cal_data_ptr->zero_bias[i]);
       }
    }

    if (SMGR_SENSOR_IS_MAG(ddf_sensor_ptr->sensor_ptr))
    {
      float cal_mag_norm_sq = 0.0; 

      // Mag Anomaly Detector
      for(i=0; i<SMGR_MAX_VALUES_PER_DATA_TYPE; i++)
      {
         cal_mag_norm_sq += (float)FX_FIXTOFLT_Q16(sample_data[i]) *
                        (float)FX_FIXTOFLT_Q16(sample_data[i]);
      }

      if (cal_mag_norm_sq > (MAG_CAL_ANOMALY_DET_THRESH_GAUSS_SQ))
      {
        cal_data_ptr->calibration_accuracy = SNS_SMGR_CAL_ACCURACY_UNRELIABLE;
        SNS_SMGR_PRINTF0(HIGH, "Smgr Mag Anomaly Detection");
      }

      if (cal_data_ptr->calibration_accuracy <= SNS_SMGR_CAL_ACCURACY_HIGH)
      {
        uint8_t accuracy = 0;

        accuracy = (uint8_t)(cal_data_ptr->calibration_accuracy);
        accuracy <<= 1; // bits 1,2 of ItemFlags used to convey accuracy
        cal_applied_flags |= accuracy;
      }
      else
      {
        SNS_SMGR_PRINTF1(ERROR, "unsupported cal accuracy: %d", cal_data_ptr->calibration_accuracy);
      }
      

      //SNS_SMGR_PRINTF1(HIGH, "cal_applied_flags: %d", cal_applied_flags);
    }
  } // CAL Ptr != NULL

  return cal_applied_flags;
}

/*===========================================================================

  FUNCTION:   smgr_init_item_depot_iter

===========================================================================*/
/*!
  @brief Initializes given item's depot_iter object.

  @detail Also returned are timestamps of oldest sample sent and latest sample available.

  @param[i]  item_ptr - the report item whose depot_iter to initialize

  @return   None
 */
/*=========================================================================*/
static const smgr_sample_s* smgr_init_item_depot_iter(smgr_rpt_item_s* item_ptr)
{
  const smgr_sample_s* sample_ptr;
  if ( item_ptr->last_processed_sample_timestamp != 0 )
  {
    sample_ptr = sns_smgr_init_depot_iter_ext(
                   item_ptr->ddf_sensor_ptr->depot_ptr,
                   item_ptr->last_processed_sample_timestamp,
                   &item_ptr->depot_iter);
  }
  else
  {
    sample_ptr = sns_smgr_init_depot_iter_ext(
                   item_ptr->ddf_sensor_ptr->depot_ptr,
                   item_ptr->ts_last_sent, 
                   &item_ptr->depot_iter);
  }

  if ( SMGR_BIT_CLEAR_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B) )
  {
    while ( (sample_ptr != NULL) && (sample_ptr->status != SNS_DDF_SUCCESS) )
    {
      sample_ptr = sns_smgr_get_sample(&item_ptr->depot_iter, 1);
    }
  }
  return sample_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_generate_periodic_report

===========================================================================*/
/*!
  @brief This function generates periodic report indication message.

  @detail

  @param[i]  rpt_spec_ptr  report pointer
  @return   TRUE if report was generated
 */
/*=========================================================================*/
static boolean sns_smgr_generate_periodic_report(smgr_rpt_spec_s* rpt_spec_ptr)
{
  boolean                               sent = FALSE;
  sns_smgr_periodic_report_ind_msg_v01* ind_ptr = NULL;
  sns_smgr_data_item_s_v01*             rsp_item_ptr;
  uint8_t                               i;

  SNS_SMGR_REPORT_DBG2(MED, "periodic_rpt - rpt_id=%d items=%d",
                       rpt_spec_ptr->rpt_id, rpt_spec_ptr->num_items);

  ind_ptr = &sns_smgr.report_ind.periodic;
  SNS_OS_MEMZERO(ind_ptr, sizeof(sns_smgr_periodic_report_ind_msg_v01));
  ind_ptr->ReportId = rpt_spec_ptr->rpt_id;
  ind_ptr->status   = SNS_SMGR_REPORT_OK_V01;

  /* gather all items */
  rsp_item_ptr = ind_ptr->Item;
  for ( i=0; i<rpt_spec_ptr->num_items; i++ )
  {
    smgr_rpt_item_s* item_spec_ptr = rpt_spec_ptr->item_list[i];
    smgr_ddf_sensor_s* ddf_sensor_ptr = item_spec_ptr->ddf_sensor_ptr;
    const smgr_sample_s* sample_ptr;

    if ( item_spec_ptr->state != SENSOR_ITEM_STATE_DONE )
    {
      SNS_SMGR_REPORT_DBG2(MED, "periodic_rpt - ddf_sensor=%d state=%d",
                           SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                            ddf_sensor_ptr->data_type),
                           ddf_sensor_ptr->sensor_ptr->sensor_state);
      continue;
    }
    item_spec_ptr->state = SENSOR_ITEM_STATE_IDLE;

    SNS_SMGR_REPORT_DBG2(HIGH, "resulting_sample status=%d ts=%u",
                         item_spec_ptr->resulting_sample.status,
                         item_spec_ptr->resulting_sample.time_stamp);

    if ( !sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr,
                                   ddf_sensor_ptr->data_type) )
    {
      sample_ptr = smgr_init_item_depot_iter(item_spec_ptr);
      while ( (item_spec_ptr->state != SENSOR_ITEM_STATE_DONE) && (sample_ptr != NULL) )
      {
        item_spec_ptr->resulting_sample.status =
          sns_smgr_process_sample(item_spec_ptr, sample_ptr,
                                  &item_spec_ptr->resulting_sample.time_stamp,
                                  item_spec_ptr->resulting_sample.data);
        sample_ptr = sns_smgr_get_sample(&item_spec_ptr->depot_iter, 1);
        SNS_SMGR_REPORT_DBG3(MED, "resulting_sample status=%d ts=%u sample_ts=%u",
                             item_spec_ptr->resulting_sample.status,
                             item_spec_ptr->resulting_sample.time_stamp,
                             sample_ptr ? sample_ptr->time_stamp : 0);
      }
    }
    else
    {
      sample_ptr = sns_smgr_get_latest_sample(ddf_sensor_ptr->depot_ptr);
      if ( sample_ptr != NULL )
      {
        item_spec_ptr->resulting_sample.status =
          sns_smgr_process_sample(item_spec_ptr, sample_ptr,
                                  &item_spec_ptr->resulting_sample.time_stamp,
                                  item_spec_ptr->resulting_sample.data);
      }
    }

    if ( (item_spec_ptr->state == SENSOR_ITEM_STATE_DONE) &&
         ((item_spec_ptr->resulting_sample.status == SNS_DDF_SUCCESS) ||
          (item_spec_ptr->resulting_sample.status == SNS_DDF_EINVALID_DATA)) )
    {
      /* Calculate current reporting rate: Only for the first item */
      if ( 0 == ind_ptr->Item_len++ )
      {
        ind_ptr->CurrentRate = FX_FIXTOFLT_Q16(item_spec_ptr->effective_rate_hz);
      }
      SNS_OS_MEMCOPY(rsp_item_ptr->ItemData,
                     item_spec_ptr->resulting_sample.data,
                     sizeof(rsp_item_ptr->ItemData));
      if ( item_spec_ptr->resulting_sample.status != SNS_DDF_SUCCESS )
      {
        rsp_item_ptr->ItemFlags   = SNS_SMGR_ITEM_FLAG_INVALID_V01;
        rsp_item_ptr->ItemQuality = SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY_V01;
        SNS_SMGR_REPORT_DBG1(ERROR, "periodic_rpt - ts=%u",
                             item_spec_ptr->resulting_sample.time_stamp);
      }
      else
      {
        rsp_item_ptr->ItemQuality = item_spec_ptr->quality;
        rsp_item_ptr->ItemFlags  |=
          smgr_apply_calibration(rsp_item_ptr->ItemData,
                                 item_spec_ptr->ddf_sensor_ptr,
                                 item_spec_ptr->cal_sel);

        /* If the data is meant for the magnetometer, and the requested 
           calibration is full cal, filter the output */
        if (SMGR_SENSOR_IS_MAG(item_spec_ptr->ddf_sensor_ptr->sensor_ptr) &&
            item_spec_ptr->cal_sel == SNS_SMGR_CAL_SEL_FULL_CAL_V01)
        {
          sns_smgr_cbuff_update(&item_spec_ptr->cbuff_obj, rsp_item_ptr->ItemData);
        }
      }
      SMGR_BIT_SET(item_spec_ptr->flags, SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B);
      SMGR_UPDATE_INTERP_TIMESTAMP(&item_spec_ptr->interpolator_obj);

      rsp_item_ptr->SensorId = SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr);
      rsp_item_ptr->DataType = ddf_sensor_ptr->data_type;
      rsp_item_ptr->ItemSensitivity = item_spec_ptr->sensitivity;
      rsp_item_ptr->TimeStamp   = item_spec_ptr->resulting_sample.time_stamp;

      item_spec_ptr->ts_last_sent = item_spec_ptr->resulting_sample.time_stamp;
      rsp_item_ptr++;       /* next item */

      if ( sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr,
                                    ddf_sensor_ptr->data_type) ||
           item_spec_ptr->quality == SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01 )
      {
        item_spec_ptr->state = SENSOR_ITEM_STATE_IDLE;
      }
    }
  }/* for each item in report */

  if ( ind_ptr->Item_len > 0 )
  {
    if ( rpt_spec_ptr->ind_count == 0 )
    {
      SNS_SMGR_PRINTF3(
        HIGH, "periodic_rpt - id=%d items=%d ind_cnt=%d",
        ind_ptr->ReportId, ind_ptr->Item_len, rpt_spec_ptr->ind_count);
    }
    sent = sns_smgr_send_indication(
             ind_ptr,
             SNS_SMGR_REPORT_IND_V01,
             sizeof(sns_smgr_periodic_report_ind_msg_v01),
             rpt_spec_ptr->header_abstract.connection_handle);
    if ( sent )
    {
      rpt_spec_ptr->ind_count++;
    }
    else
    {
      SNS_SMGR_PRINTF2(
        ERROR, "periodic_rpt - id=%d last_sent[0]=%u",
        ind_ptr->ReportId, rpt_spec_ptr->item_list[0]->ts_last_sent);
    }

    #if defined(FEATURE_TEST_DRI)
    if ( rpt_spec_ptr->ind_count == 1 ) /* first indication? */
    {
      sns_test_dri_round_trip_delay( FALSE );
    }
    #endif /* defined(FEATURE_TEST_DRI) */
  }
  else
  {
    SNS_SMGR_REPORT_DBG2(
       ERROR, "periodic_rpt - no ready items for rpt %d ts_now=%u",
       rpt_spec_ptr->rpt_id, sns_smgr.last_tick.u.low_tick);
  }
  return sent;
}

/*===========================================================================

  FUNCTION:   sns_smgr_generate_test_result

===========================================================================*/
/*!
  @brief This function generates sensor test result indication message

  @detail

  @param[i]  test_info     pointer to sensor test information
  @param[i]  test_status   sensor test status
  @param[i]  test_err_code sensor test error code (device-specific)
  @return   none
 */
/*=========================================================================*/
void sns_smgr_generate_test_result(smgr_sensor_test_s* test_info,
                                   sns_smgr_test_status_e_v01 test_status,
                                   uint32_t test_err_code)
{
  sns_smgr_single_sensor_test_ind_msg_v01* result_msg_p=NULL;

  result_msg_p = SMGR_MSG_ALLOC(sns_smgr_single_sensor_test_ind_msg_v01);
  if ( result_msg_p == NULL )
  {
     SNS_SMGR_PRINTF0(ERROR, "gen_test_result - alloc failed");
     return;
  }
  result_msg_p->SensorID = test_info->sensor_id;
  result_msg_p->DataType = test_info->data_type;
  result_msg_p->TestType = test_info->test_type;
  result_msg_p->TestResult = (test_status == SNS_SMGR_TEST_STATUS_SUCCESS_V01)
     ? (SNS_SMGR_TEST_RESULT_PASS_V01) : (SNS_SMGR_TEST_RESULT_FAIL_V01);

  /* If test_err_code is none-zero, it must have been generated from driver;
     so we need to fill the optional error code field in the indication.
  */

#ifdef ADSP_HWCONFIG_L
  if (test_err_code != 0)
  {
      result_msg_p->ErrorCode_valid = 1;

      if(result_msg_p->SensorID == SNS_SMGR_ID_PROX_LIGHT_V01)
      {
          result_msg_p->DataType = (test_err_code >> 8)&0xff;
          result_msg_p->ErrorCode = test_err_code&0xff;
      }
      else /* in case of Other sensors */
      {
          result_msg_p->ErrorCode = test_err_code;
      }
  }
#else
  if (test_err_code != 0)
  {
     result_msg_p->ErrorCode_valid = 1;
     result_msg_p->ErrorCode = test_err_code;
  }
#endif
  else
  {
     result_msg_p->ErrorCode_valid = 0;
  }

  // send test result indication
  SNS_SMGR_PRINTF3(HIGH, "gen_test_result - sensor=%d result=%d, err=%d", 
                   result_msg_p->SensorID, result_msg_p->TestResult, test_err_code);
  sns_smgr_send_indication(result_msg_p,
                           SNS_SMGR_SINGLE_SENSOR_TEST_IND_V01,
                           sizeof(sns_smgr_single_sensor_test_ind_msg_v01),
                           test_info->hdr_abstract.connection_handle);
}


/*===========================================================================

  FUNCTION:   sns_smgr_generate_sensor_status_ind

===========================================================================*/
/*!
  @brief This function generate sensor status indication

  @detail

  @param[i]  sensor_status_ptr - sensor status block
  @param[i]  proc              - processor index

  @return   none
 */
/*=========================================================================*/
void sns_smgr_generate_sensor_status_ind(smgr_sensor_status_s* sensor_status_ptr,
                                         uint8_t proc)
{
  smgr_status_client_info_s* status_client_ptr =
    &sensor_status_ptr->status_clients[proc];
  sns_smgr_sensor_status_ind_msg_v01* ind_msg_p =
    SMGR_MSG_ALLOC(sns_smgr_sensor_status_ind_msg_v01);
  if ( ind_msg_p )
  {
    ind_msg_p->SensorID    =
      SMGR_SENSOR_ID(sensor_status_ptr->ddf_sensor_ptr->sensor_ptr);
    ind_msg_p->SensorState = status_client_ptr->pending_status;
    ind_msg_p->TimeStamp   = sensor_status_ptr->time_stamp;
    ind_msg_p->PerProcToalClients_valid = TRUE;
    ind_msg_p->PerProcToalClients_len   = 3;
    ind_msg_p->PerProcToalClients[SNS_SMGR_DSPS_CLIENTS_V01] =
      sensor_status_ptr->num_requested_dsps;
    ind_msg_p->PerProcToalClients[SNS_SMGR_APPS_CLIENTS_V01] =
      sensor_status_ptr->num_requested_apps;
    ind_msg_p->PerProcToalClients[SNS_SMGR_MODEM_CLIENT_V01] = 0;

    status_client_ptr->last_status = status_client_ptr->pending_status;

    SNS_SMGR_PRINTF3(MED, "gen_status_ind - sensor=%d status=%d conn=0x%x",
                     ind_msg_p->SensorID, ind_msg_p->SensorState,
                     status_client_ptr->connection_handle);

    sns_smgr_send_indication(ind_msg_p,
                             SNS_SMGR_SENSOR_STATUS_IND_V01,
                             sizeof(sns_smgr_sensor_status_ind_msg_v01),
                             status_client_ptr->connection_handle);
  }
  else
  {
    SNS_SMGR_PRINTF0(ERROR, "gen_status_ind - alloc failed");
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_pack_sample

===========================================================================*/
/*!
  @brief Packs the given sample into indication message.

  @param[i]  idx_ptr            - pointer to the sample indices array in indication
  @param[i]  ind_sample_ptr     - pointer to the sample array in indication
  @param[i]  item_spec_ptr      - pointer to item spec
  @param[i]  sample_ptr         - pointer to the source sample
  @param[io] prev_timestamp_ptr - pointer to the previous timestamp

  @return   None
 */
/*=========================================================================*/
static void sns_smgr_pack_sample(
  sns_smgr_buffering_sample_index_s_v01* idx_ptr,
  sns_smgr_buffering_sample_s_v01*       ind_sample_ptr,
  smgr_rpt_item_s*                       item_spec_ptr,
  sns_ddf_status_e                       sample_status,
  const uint32_t*                        timestamp_ptr,
  uint32_t*                              prev_timestamp_ptr
)
{
  if ( sample_status == SNS_DDF_SUCCESS )
  {
    ind_sample_ptr->Quality = item_spec_ptr->quality;
    ind_sample_ptr->Flags   = smgr_apply_calibration(ind_sample_ptr->Data,
                                                     item_spec_ptr->ddf_sensor_ptr,
                                                     item_spec_ptr->cal_sel);

    /* If the data is meant for the magnetometer, and the requested 
       calibration is full cal, filter the output */
    if (SMGR_SENSOR_IS_MAG(item_spec_ptr->ddf_sensor_ptr->sensor_ptr) &&
        item_spec_ptr->cal_sel == SNS_SMGR_CAL_SEL_FULL_CAL_V01)
    {
      sns_smgr_cbuff_update(&item_spec_ptr->cbuff_obj, 
                             ind_sample_ptr->Data);
    }
  }
  else
  {
    ind_sample_ptr->Quality = SNS_SMGR_ITEM_QUALITY_INVALID_NOT_READY_V01;
    ind_sample_ptr->Flags   = SNS_SMGR_ITEM_FLAG_INVALID_V01;
  }
  if ( idx_ptr->SampleCount == 0 )
  {
    ind_sample_ptr->TimeStampOffset = 0;
    idx_ptr->FirstSampleTimestamp   = *timestamp_ptr;
  }
  else
  {
    ind_sample_ptr->TimeStampOffset = *timestamp_ptr - *prev_timestamp_ptr;
  }
  *prev_timestamp_ptr = *timestamp_ptr;

  SNS_SMGR_REPORT_DBG2(LOW, "pack_sample - timestamp=%u offset=%d",
                       *timestamp_ptr, ind_sample_ptr->TimeStampOffset);
}

/*===========================================================================

  FUNCTION:   sns_smgr_generate_buffering_report

===========================================================================*/
/*!
  @brief This function generates buffering report indication messages.

  @detail

  @param[i]  rpt_spec_ptr  - the report spec
  @param[i]  end_timestamp - samples with timestamp newer than end_timestamp
                             should not be sent
  @return   number of samples sent
 */
/*=========================================================================*/
static uint32_t sns_smgr_generate_buffering_report(
  smgr_rpt_spec_s*  rpt_spec_ptr,
  uint32_t          end_timestamp,
  uint8_t           ind_type
)
{
  uint8_t  i;
  sns_smgr_buffering_ind_msg_v01* ind_ptr;

  SNS_SMGR_REPORT_DBG1(HIGH, "buffering_rpt - end_ts=%u", end_timestamp);

  ind_ptr = &sns_smgr.report_ind.buffering;
  SNS_OS_MEMZERO(ind_ptr, sizeof(sns_smgr_buffering_ind_msg_v01));
  for ( i=0; i<rpt_spec_ptr->num_items; i++ )
  {
    uint32_t previous_timestamp;
    sns_smgr_buffering_sample_index_s_v01* idx_ptr = NULL;
    smgr_rpt_item_s* item_spec_ptr = rpt_spec_ptr->item_list[i];
    smgr_ddf_sensor_s* ddf_sensor_ptr = item_spec_ptr->ddf_sensor_ptr;
    const smgr_sample_s* sample_ptr =
      sns_smgr_get_sample(&item_spec_ptr->depot_iter, 0);

    if (ddf_sensor_ptr->sensor_ptr->sensor_state != SENSOR_STATE_READY)
    {
      continue;
    }
    SNS_SMGR_REPORT_DBG3(
      MED, "buffering_rpt - rpt_id=%d ddf_sensor=%d rate=0x%x", rpt_spec_ptr->rpt_id,
      SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
      item_spec_ptr->effective_rate_hz);
    SNS_SMGR_REPORT_DBG2(
      MED, "buffering_rpt - ts_last_sent=%u ts_last_processed=%u",
      item_spec_ptr->ts_last_sent, item_spec_ptr->last_processed_sample_timestamp);

    previous_timestamp = item_spec_ptr->ts_last_sent;
    idx_ptr = &ind_ptr->Indices[ind_ptr->Indices_len];
    idx_ptr->SensorId       = SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr);
    idx_ptr->DataType       = ddf_sensor_ptr->data_type;
    idx_ptr->FirstSampleIdx = ind_ptr->Samples_len;
    idx_ptr->FirstSampleTimestamp = 0;
    idx_ptr->SamplingRate   = item_spec_ptr->effective_rate_hz;

    while ( (ind_ptr->Samples_len<ARR_SIZE(ind_ptr->Samples)) &&
            (sample_ptr != NULL) &&
            ((sample_ptr->status == SNS_DDF_SUCCESS) ||
             SMGR_BIT_TEST(item_spec_ptr->flags, 
                           SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B)) &&
            TICK1_GEQ_TICK2(end_timestamp, sample_ptr->time_stamp) )
    {
      sns_smgr_buffering_sample_s_v01* ind_sample_ptr =
        &ind_ptr->Samples[ind_ptr->Samples_len];
      sns_ddf_time_t   timestamp;
      sns_ddf_status_e sample_status =
        sns_smgr_process_sample(item_spec_ptr, sample_ptr, &timestamp,
                                ind_sample_ptr->Data);
      SNS_SMGR_REPORT_DBG2(LOW, "buffering_rpt - ts=%u status=%u",
                           sample_ptr->time_stamp, sample_status);
      if ( (sample_status == SNS_DDF_SUCCESS) ||
           (sample_status == SNS_DDF_EINVALID_DATA) )
      {
        sns_smgr_pack_sample(idx_ptr, ind_sample_ptr, item_spec_ptr,
                             sample_status, &timestamp, &previous_timestamp);
        idx_ptr->SampleCount++;
        ind_ptr->Samples_len++;
        SMGR_BIT_SET(item_spec_ptr->flags, SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B);
        SMGR_UPDATE_INTERP_TIMESTAMP(&item_spec_ptr->interpolator_obj);
      }
      else
      {
        sample_ptr = sns_smgr_get_sample(&item_spec_ptr->depot_iter, 1);
      }
    } /* while ( (ind_ptr->Samples_len<ARR_SIZE(ind_ptr->Samples)) &&
                 (sample_ptr != NULL) ) */

    item_spec_ptr->state = SENSOR_ITEM_STATE_IDLE;
    item_spec_ptr->ts_last_sent = previous_timestamp;

    SNS_SMGR_REPORT_DBG3(
      MED, "buffering_rpt - sensor=%d first=%u last=%u",
      idx_ptr->SensorId, idx_ptr->FirstSampleTimestamp, 
      item_spec_ptr->ts_last_sent);

    if ( idx_ptr->SampleCount > 0 )
    {
      ind_ptr->Indices_len++;
    }
  }/* for each item in report spec */

  if ( ind_ptr->Samples_len > 0 )
  {
    ind_ptr->ReportId      = rpt_spec_ptr->rpt_id;
    ind_ptr->IndType_valid = true;
    ind_ptr->IndType       = ind_type;

    if ( rpt_spec_ptr->ind_count == 0 )
    {
      SNS_SMGR_PRINTF3(
         HIGH, "buffering_rpt - id=%d samples=%d indices=%d",
         ind_ptr->ReportId, ind_ptr->Samples_len, ind_ptr->Indices_len);
    }
    if ( sns_smgr_send_indication(
           ind_ptr,
           SNS_SMGR_BUFFERING_IND_V01,
           sizeof(sns_smgr_buffering_ind_msg_v01),
           rpt_spec_ptr->header_abstract.connection_handle) )
    {
      rpt_spec_ptr->ind_count++;
    }
    else
    {
      SNS_SMGR_PRINTF3(
         ERROR, "buffering_rpt - id=%d #samples=%d last_sent[0]=%u",
         ind_ptr->ReportId, ind_ptr->Samples_len,
         rpt_spec_ptr->item_list[0]->ts_last_sent);
      ind_ptr->Samples_len = 0;
    }
  }
  else
  {
    SNS_SMGR_REPORT_DBG2(
       ERROR, "buffering_rpt - no ready items rpt_id=%d tick=%u",
       rpt_spec_ptr->rpt_id, rpt_spec_ptr->rpt_tick);
  }
  return ind_ptr->Samples_len;
}

/*===========================================================================

  FUNCTION:   smgr_compute_num_avail_samples

===========================================================================*/
/*!
  @brief Computes number of samples available for given report since the
         previous report indication was sent.

  @detail Also returned are timestamps of oldest sample sent and latest sample available.

  @param[i]  rpt_spec_ptr            - the report for which to compute sample count
  @param[i]  oldest_last_sent_ts_ptr - destination for timestamp of oldest sample sent
  @param[i]  latest_sample_ts_ptr    - destination for timestamp of latest sample available

  @return   The computed number of samples available.
 */
/*=========================================================================*/
static uint32_t smgr_compute_num_avail_samples(
  const smgr_rpt_spec_s* rpt_spec_ptr,
  uint32_t*              oldest_last_sent_ts_ptr,
  uint32_t*              latest_sample_ts_ptr
)
{
  uint32_t total_samples = 0;
  uint32_t max_samples = 0;
  uint8_t i;

  *oldest_last_sent_ts_ptr = 0xFFFFFFFF;
  *latest_sample_ts_ptr = 0;
  for ( i=0; i<rpt_spec_ptr->num_items; i++ )
  {
    smgr_rpt_item_s* item_ptr = rpt_spec_ptr->item_list[i];
    const smgr_sample_s* sample_ptr =
      sns_smgr_get_latest_sample(item_ptr->ddf_sensor_ptr->depot_ptr);
    if ( (sample_ptr != NULL) &&
         TICK1_GTR_TICK2(sample_ptr->time_stamp, item_ptr->ts_last_sent ) )
    {
      uint32_t delta_time = sample_ptr->time_stamp - item_ptr->ts_last_sent;
      uint32_t samp_intvl = item_ptr->sampling_interval;
      if ( SMGR_SENSOR_IS_SELF_SCHED(item_ptr->ddf_sensor_ptr->sensor_ptr) )
      {
        samp_intvl = samp_intvl * 95 / 100;
      }
      item_ptr->num_samples = delta_time / samp_intvl;
      if ( delta_time % samp_intvl )
      {
        item_ptr->num_samples++;
      }
      total_samples += item_ptr->num_samples;
      if ( max_samples < item_ptr->num_samples )
      {
        max_samples = item_ptr->num_samples;
        *oldest_last_sent_ts_ptr = MIN(*oldest_last_sent_ts_ptr, item_ptr->ts_last_sent);
      }
      *latest_sample_ts_ptr = MAX(*latest_sample_ts_ptr, sample_ptr->time_stamp);
      smgr_init_item_depot_iter(item_ptr);
    }
    else
    {
      item_ptr->num_samples = 0;
    }
  }
  if ( total_samples > 0 )
  {
    SNS_SMGR_B2B_IND_DBG3(
      MED, "avail_sample_cnt=%d oldest=%u latest=%u",
      total_samples, *oldest_last_sent_ts_ptr, *latest_sample_ts_ptr);
  }
  else
  {
    SNS_SMGR_B2B_IND_DBG2(
      MED, "no samples for rpt_id=%d rpt_tick=%u",
      rpt_spec_ptr->rpt_id, rpt_spec_ptr->rpt_tick);
  }
  return total_samples;
}

/*===========================================================================

  FUNCTION:   sns_smgr_generate_buffering_indications

===========================================================================*/
/*!
  @brief Determines number of indications required to send all available samples
         for the given report, generates and sends them.

  @param[i]  rpt_spec_ptr  - the report spec

  @return   TRUE if one ore more indications were sent
 */
/*=========================================================================*/
static bool sns_smgr_generate_buffering_indications(smgr_rpt_spec_s* rpt_spec_ptr)
{
  uint8_t i, num_inds = 0;
  uint32_t estimated_num_samples, num_samples_sent = 0;
  uint32_t oldest_last_sent_ts, latest_sample_ts;

#ifdef SNS_SMGR_B2B_IND_DEBUG
  uint32_t ts_before = sns_em_get_timestamp();
#endif

  SNS_SMGR_B2B_IND_DBG3(
    HIGH, "buff_rpt_prep - rpt_id=%d rate=0x%x items=%d",
    rpt_spec_ptr->rpt_id, rpt_spec_ptr->q16_rpt_rate, rpt_spec_ptr->num_items);

  estimated_num_samples = smgr_compute_num_avail_samples(rpt_spec_ptr,
                                                         &oldest_last_sent_ts,
                                                         &latest_sample_ts);
  if ( estimated_num_samples > 0 )
  {
    uint32_t time_span, time_per_ind, end_ts;

    num_inds = (estimated_num_samples / SNS_SMGR_BUFFERING_REPORT_MAX_SAMPLES_V01);
    if ( estimated_num_samples % SNS_SMGR_BUFFERING_REPORT_MAX_SAMPLES_V01 )
    {
      num_inds++;
    }
    time_span = latest_sample_ts - oldest_last_sent_ts;
    time_per_ind = time_span / num_inds;

    SNS_SMGR_B2B_IND_DBG3(
      MED, "buff_inds - #ind=%d time_span=%d time/ind=%d",
      num_inds, time_span, time_per_ind);

    for ( i = 0, end_ts = oldest_last_sent_ts + time_per_ind;
          i < num_inds; i++, end_ts += time_per_ind )
    {
      uint8_t ind_type;
      if ( num_inds == 1 )
      {
        ind_type = SNS_BATCH_ONLY_IND_V01;
        end_ts = latest_sample_ts;
      }
      else if ( i == 0 )
      {
        ind_type = SNS_BATCH_FIRST_IND_V01;
      }
      else if ( i == num_inds-1 )
      {
        ind_type = SNS_BATCH_LAST_IND_V01;
        end_ts = latest_sample_ts;
      }
      else
      {
        ind_type = SNS_BATCH_INTERMEDIATE_IND_V01;
      }
      num_samples_sent += sns_smgr_generate_buffering_report(rpt_spec_ptr, end_ts, ind_type);
      if ( (i & 0x0f) == 0x0f )

      {
        SMGR_DELAY_US(1000); /* gives QMI time (crude flow control) */
      }
    }

    SNS_SMGR_B2B_IND_DBG3(
      MED, "buff_inds - %d ticks, %d samples, %d inds",
      sns_em_get_timestamp() - ts_before, num_samples_sent, num_inds);
  }
  return (num_samples_sent > 0);
}

/*===========================================================================

  FUNCTION:   sns_smgr_pack_query_sample

===========================================================================*/
/*!
  @brief Packs the given sample into indication message.

  @param[i]  query_ptr          - pointer to the query spec
  @param[i]  ind_ptr            - pointer to the query indication
  @param[i]  timestamp_ptr      - pointer to the sample timestamp
  @param[io] prev_timestamp_ptr - pointer to the previous timestamp

  @return   None
*/
/*=========================================================================*/
static void sns_smgr_pack_query_sample(
  smgr_query_s*                         query_ptr,
  sns_smgr_buffering_query_ind_msg_v01* ind_ptr,
  const uint32_t*                       timestamp_ptr,
  uint32_t*                             prev_timestamp_ptr,
  uint8_t                               quality
)
{
  smgr_rpt_item_s*                  item_ptr       = query_ptr->item_ptr;
  sns_smgr_buffering_sample_s_v01*  ind_sample_ptr =
    &ind_ptr->Samples[ind_ptr->Samples_len];

  ind_sample_ptr->Flags   = smgr_apply_calibration(ind_sample_ptr->Data,
                                                   item_ptr->ddf_sensor_ptr,
                                                   item_ptr->cal_sel);
  /* If the data is meant for the magnetometer, and the requested 
     calibration is full cal, filter the output */
  if (SMGR_SENSOR_IS_MAG(item_ptr->ddf_sensor_ptr->sensor_ptr) &&
      item_ptr->cal_sel == SNS_SMGR_CAL_SEL_FULL_CAL_V01)
  {
    sns_smgr_cbuff_update(&item_ptr->cbuff_obj, 
                           ind_sample_ptr->Data);
  }

  ind_sample_ptr->Quality = quality;

  if ( ind_ptr->Samples_len++ == 0 )
  {
    ind_ptr->FirstSampleTimestamp   = *timestamp_ptr;
    ind_sample_ptr->TimeStampOffset = 0;
  }
  else
  {
    ind_sample_ptr->TimeStampOffset = *timestamp_ptr - *prev_timestamp_ptr;
  }
  *prev_timestamp_ptr = *timestamp_ptr;
}

/*===========================================================================

  FUNCTION:   smgr_send_empty_query_indication

===========================================================================*/
/*!
  @brief Generates one Query indication message with no samples.

  @param[i] query_ptr

  @return TRUE if indication sent; FALSE otherwise
*/
/*=========================================================================*/
boolean smgr_send_empty_query_indication(smgr_query_s* query_ptr)
{
  sns_smgr_buffering_query_ind_msg_v01* ind_ptr = &sns_smgr.report_ind.query;

  SNS_OS_MEMZERO(ind_ptr, sizeof(sns_smgr_buffering_query_ind_msg_v01));
  ind_ptr->QueryId              = query_ptr->query_id;
  ind_ptr->FirstSampleTimestamp = 0;
  ind_ptr->SamplingRate         = 0;
  ind_ptr->Samples_len          = 0;
  SNS_SMGR_PRINTF1(HIGH, "send_empty_query_ind - ID=0x%x", ind_ptr->QueryId);
  sns_smgr_send_indication(
    ind_ptr, SNS_SMGR_BUFFERING_QUERY_IND_V01,
    sizeof(sns_smgr_buffering_query_ind_msg_v01),
    query_ptr->item_ptr->parent_report_ptr->header_abstract.connection_handle);
  return TRUE;
}

/*===========================================================================

  FUNCTION:   smgr_send_query_indication

===========================================================================*/
/*!
  @brief Generates one Query indication message.

  @param[i] query_ptr

  @return
    true if indication sent, false otherwise

*/
/*=========================================================================*/
static boolean smgr_send_query_indication(smgr_query_s* query_ptr)
{
  sns_smgr_buffering_query_ind_msg_v01* ind_ptr = &sns_smgr.report_ind.query;

  smgr_interpolator_obj_s t0_t1_interp_obj;
  uint32_t previous_timestamp;
  const smgr_sample_s* sample_ptr;
  smgr_rpt_item_s* item_ptr = query_ptr->item_ptr;

  if ( item_ptr == NULL )
  {
    return false;
  }

  sample_ptr = sns_smgr_get_sample(&item_ptr->depot_iter, 0);

  SNS_OS_MEMZERO(ind_ptr, sizeof(sns_smgr_buffering_query_ind_msg_v01));
  ind_ptr->QueryId              = query_ptr->query_id;
  ind_ptr->SamplingRate         = item_ptr->effective_rate_hz;

  SNS_OS_MEMZERO(&t0_t1_interp_obj, sizeof(smgr_interpolator_obj_s));
  t0_t1_interp_obj.desired_timestamp = query_ptr->T0;
  item_ptr->interpolator_obj.desired_timestamp = query_ptr->T0;
  item_ptr->interpolator_obj.sample1_ptr = NULL;
  item_ptr->interpolator_obj.sample2_ptr = NULL;
  item_ptr->last_processed_sample_timestamp = 0;

  /* skip invalid samples */
  while ( (sample_ptr != NULL) && (sample_ptr->status != SNS_DDF_SUCCESS) )
  {
    sample_ptr = sns_smgr_get_sample(&item_ptr->depot_iter, 1);
  }

  if ( sample_ptr != NULL )
  {
    /* feeds interpolator with first valid sample */
    sns_smgr_interpolator_update(&t0_t1_interp_obj, sample_ptr);
  }

  /* skip old samples */
  while ( (sample_ptr != NULL) && 
          TICK1_GTR_TICK2(query_ptr->T0, sample_ptr->time_stamp) )
  {
    sample_ptr = sns_smgr_get_sample(&item_ptr->depot_iter, 1);
    sns_smgr_interpolator_update(&t0_t1_interp_obj, sample_ptr);
  }

  if ( (t0_t1_interp_obj.sample1_ptr != NULL) &&
       (sample_ptr != NULL) && (query_ptr->T0 != sample_ptr->time_stamp))
  {
    SNS_SMGR_QUERY_DBG3(HIGH, "query_ind - s1=%u T0=%u s2=%u",
                        t0_t1_interp_obj.sample1_ptr->time_stamp,
                        query_ptr->T0,
                        t0_t1_interp_obj.sample2_ptr->time_stamp);
    sns_smgr_interpolate(&t0_t1_interp_obj,
                         ind_ptr->Samples[ind_ptr->Samples_len].Data);
    sns_smgr_pack_query_sample(query_ptr, ind_ptr,
                               &query_ptr->T0, &previous_timestamp,
                               SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01);

    /* compute next desired timestamp in case it's needed*/
    SMGR_UPDATE_INTERP_TIMESTAMP(&item_ptr->interpolator_obj);
  }

  /* prepares to interpolate for T1*/
  t0_t1_interp_obj.desired_timestamp = query_ptr->T1;
  sns_smgr_interpolator_update(&t0_t1_interp_obj, sample_ptr);

  if ( item_ptr->quality != SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01 )
  {
    SNS_SMGR_QUERY_DBG3(MED, "query_ind - quality=%d interp_intvl=%d desired_ts=%u",
                        item_ptr->quality, item_ptr->interpolator_obj.interval_ticks,
                        item_ptr->interpolator_obj.desired_timestamp);
  }

  while ( (sample_ptr != NULL) &&
          (ind_ptr->Samples_len < ARR_SIZE(ind_ptr->Samples)) &&
          TICK1_GEQ_TICK2(query_ptr->T1, sample_ptr->time_stamp) )
  {
    sns_ddf_status_e status;
    sns_ddf_time_t   timestamp;

    status = sns_smgr_process_sample(item_ptr, sample_ptr, &timestamp,
                                     ind_ptr->Samples[ind_ptr->Samples_len].Data);
    if ( status == SNS_DDF_SUCCESS )
    {
      SNS_SMGR_QUERY_DBG1(MED, "query_ind - ts=%u", timestamp);
      sns_smgr_pack_query_sample(query_ptr, ind_ptr,
                                 &timestamp, &previous_timestamp,
                                 item_ptr->quality);

      /* prepare for next sample */
      SMGR_UPDATE_INTERP_TIMESTAMP(&item_ptr->interpolator_obj);
    }
    else if ( status != SNS_DDF_PENDING )
    {
      SNS_SMGR_QUERY_DBG2(ERROR, "query_ind - ts=%u status=%d",
                          sample_ptr->time_stamp, status);
    }
    if ( ((item_ptr->quality != SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01) &&
          (item_ptr->quality != SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED_V01) &&
          (query_ptr->T1 != sample_ptr->time_stamp)) ||
         (status != SNS_DDF_SUCCESS) )
    {
      sample_ptr = sns_smgr_get_sample(&item_ptr->depot_iter, 1);
      sns_smgr_interpolator_update(&t0_t1_interp_obj, sample_ptr);
    }
  }
  if ( (sample_ptr != NULL) &&
       (ind_ptr->Samples_len < ARR_SIZE(ind_ptr->Samples)) &&
       TICK1_GTR_TICK2(sample_ptr->time_stamp, query_ptr->T1) )
  {
    SNS_SMGR_QUERY_DBG3(HIGH, "query_ind - s1=%u T1=%u s2=%u",
                        t0_t1_interp_obj.sample1_ptr->time_stamp,
                        query_ptr->T1,
                        t0_t1_interp_obj.sample2_ptr->time_stamp);
    sns_smgr_interpolate(&t0_t1_interp_obj,
                         ind_ptr->Samples[ind_ptr->Samples_len].Data);
    sns_smgr_pack_query_sample(query_ptr, ind_ptr,
                               &query_ptr->T1, &previous_timestamp,
                               SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01);
  }
  SNS_SMGR_PRINTF2(HIGH, "send_query_ind - ID=0x%x samples=%d",
                   ind_ptr->QueryId, ind_ptr->Samples_len);
  sns_smgr_send_indication(
    ind_ptr, SNS_SMGR_BUFFERING_QUERY_IND_V01,
    sizeof(sns_smgr_buffering_query_ind_msg_v01),
    item_ptr->parent_report_ptr->header_abstract.connection_handle);
  item_ptr->parent_report_ptr->ind_count++;
  return TRUE;
}

/*===========================================================================

  FUNCTION:   sns_smgr_send_query_indication

===========================================================================*/
/*!
  @brief Generates one Query indication message.

  @param[i] NONE

  @return
   NONE

*/
/*=========================================================================*/
static boolean sns_smgr_send_query_indication(smgr_rpt_spec_s* rpt_ptr)
{
  boolean sent = FALSE;
  uint8_t i;
  for ( i=0; i<rpt_ptr->num_items; i++ )
  {
    smgr_query_s* query_ptr = rpt_ptr->item_list[i]->query_ptr;
    if ( query_ptr != NULL )
    {
      sns_ddf_time_t first_ts = 0, last_ts = 0;
      smgr_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
      const smgr_sample_s* sample_ptr =
        sns_smgr_init_depot_iter_ext(item_ptr->ddf_sensor_ptr->depot_ptr,
                                     query_ptr->T0, &item_ptr->depot_iter);
      if (sample_ptr != NULL)
      {
        /* iterate to previous sample so T0 would be included in indication */
        sample_ptr = sns_smgr_get_sample(&item_ptr->depot_iter, -1);
      }
      while ( (sample_ptr != NULL) && (sample_ptr->status != SNS_DDF_SUCCESS) )
      {
        /* skip invalid samples */
        sample_ptr = sns_smgr_get_sample(&item_ptr->depot_iter, 1);
      }
      SNS_SMGR_QUERY_DBG3(
         MED, "send_query_ind - T0=%u iter_sample_ts=%u status=%d",
         query_ptr->T0, sample_ptr?sample_ptr->time_stamp:0,
         sample_ptr?sample_ptr->status:-1);

      if ( sample_ptr != NULL )
      {
        first_ts = sample_ptr->time_stamp;
      }
      if ( first_ts != 0 )
      {
        if ( (sample_ptr = sns_smgr_get_latest_sample(item_ptr->ddf_sensor_ptr->
                                                      depot_ptr)) != NULL )
        {
          last_ts = sample_ptr->time_stamp;
        }
        if ( TICK1_GTR_TICK2(first_ts, query_ptr->T1) )
        {
          SNS_SMGR_PRINTF2(LOW, "send_query_ind - no valid samples between %u and %u",
                           query_ptr->T0, query_ptr->T1);
          /* the requested time period is in the past; no samples to send */
          sent = smgr_send_empty_query_indication(query_ptr);
        }
        else if ( TICK1_GEQ_TICK2(last_ts, query_ptr->T1) )
        {
          /* some or all of available samples fall within requested time period */
          sent = smgr_send_query_indication(query_ptr);
        }
        /* else, not time to send indication for this Query */
      }
      else if ( TICK1_GTR_TICK2(sns_em_get_timestamp(), query_ptr->T1) )
      {
        SNS_SMGR_PRINTF0(LOW, "send_query_ind - no valid samples available");
        /* the requested time period is in the past; no samples to send */
        sent = smgr_send_empty_query_indication(query_ptr);
      }
      /* else, not time to send indication for this Query */

      if ( sent != FALSE )
      {
        rpt_ptr->item_list[i]->query_ptr = query_ptr->next_query_ptr;
        SNS_OS_FREE(query_ptr);
      }
    }
  }
  return sent;
}

/*===========================================================================

  FUNCTION:   sns_smgr_data_gathering_cycle

===========================================================================*/
/*!
  @brief Generates one report.

  @Detail this function is called from SMGR main when smgr_data_gathering_cycleFlag is set.
          the flag is set when any sampling for a sensor was done.

  @param[i] NONE

  @return
   NONE

*/
/*=========================================================================*/
static void sns_smgr_data_gathering_cycle(void)
{
  smgr_rpt_spec_s* rpt_spec_ptr = sns_smgr.ready_report_ptr;

  if ( rpt_spec_ptr != NULL )
  {
    boolean sent = FALSE;

    sns_smgr.ready_report_ptr = rpt_spec_ptr->next_ready_report_ptr;
    rpt_spec_ptr->next_ready_report_ptr = NULL;
    if ( rpt_spec_ptr->header_abstract.msg_id == SNS_SMGR_REPORT_REQ_V01 )
    {
      sent = sns_smgr_generate_periodic_report(rpt_spec_ptr);
    }
    else if ( rpt_spec_ptr->q16_rpt_rate != 0 )
    {
      sent = sns_smgr_generate_buffering_indications(rpt_spec_ptr);
    }
    else
    {
      sent = sns_smgr_send_query_indication(rpt_spec_ptr);
    }
    if ( sent )
    {
      uint8_t i;
      for ( i=0; i<rpt_spec_ptr->num_items; i++ )
      {
        if ( rpt_spec_ptr->item_list[i]->state == SENSOR_ITEM_STATE_DONE )
        {
          break;
        }
      }
      if ( i >= rpt_spec_ptr->num_items )
      {
        if ( rpt_spec_ptr->state == SMGR_RPT_STATE_FLUSHING )
        {
          rpt_spec_ptr->state = SMGR_RPT_STATE_ACTIVE;
          rpt_spec_ptr->rpt_tick = sns_em_get_timestamp();
        }
        rpt_spec_ptr->rpt_tick += rpt_spec_ptr->rpt_interval;
      }
      else
      {
        /* add it back to end of ready report list */
        sns_smgr_ready_reports_list_add(rpt_spec_ptr);
      }

      if ( rpt_spec_ptr->proc_type == SNS_PROC_APPS_V01 &&
           !sns_smgr.app_is_awake && 
           rpt_spec_ptr->send_ind_during_suspend )
      {
        /* notify SAM that AP will be woken up */
        sns_sam_ap_state_change_cb(false);
      }
    }
    #ifdef SNS_SMGR_DEBUG
    else
    {
      SNS_SMGR_PRINTF2(ERROR, "data_gathering_cycle - rpt_id=%d tick=%u",
                       rpt_spec_ptr->rpt_id, rpt_spec_ptr->rpt_tick);
    }
    #endif
  }
  if ( sns_smgr.ready_report_ptr != NULL )
  {
    sns_smgr_set_data_gathering_cycle_flag();
  }
}

/*===========================================================================

  FUNCTION:   smgr_flush_reports

===========================================================================*/
/*!
  @brief Called when a new report is added or an existing report is removed
         resulting in changes to ODRs of associated sensors.

  @param    none

  @return   number of reports generated
 */
/*=========================================================================*/
uint32_t smgr_flush_reports(void)
{
  uint32_t time_now = sns_em_get_timestamp();
  uint32_t num_rpt_sent = 0;
  smgr_rpt_spec_s *rpt_ptr;
  SMGR_FOR_EACH_Q_ITEM( &sns_smgr.report_queue, rpt_ptr, rpt_link )
  {
    if ( (rpt_ptr->state == SMGR_RPT_STATE_FLUSHING) && SMGR_OK_TO_SEND(rpt_ptr) )
    {
      boolean sent = FALSE;
      SNS_SMGR_PRINTF2(MED, "flush_reports - rpt_id=%d ind_count=%d",
                       rpt_ptr->rpt_id, rpt_ptr->ind_count);
      if ( rpt_ptr->header_abstract.msg_id == SNS_SMGR_BUFFERING_REQ_V01 )
      {
        sent = sns_smgr_generate_buffering_indications(rpt_ptr);
      }
      else
      {
        sent = sns_smgr_generate_periodic_report(rpt_ptr);
      }
      if ( sent )
      {
        rpt_ptr->rpt_tick = time_now + rpt_ptr->rpt_interval;
        num_rpt_sent++;
        SNS_SMGR_PRINTF3(MED, "flush_reports - rpt_id=%d intvl=%d next_tick=%u",
                         rpt_ptr->rpt_id, rpt_ptr->rpt_interval, rpt_ptr->rpt_tick);
      }
      sns_smgr_ready_reports_list_remove(rpt_ptr);
    }
  }
  SMGR_FOR_EACH_Q_ITEM( &sns_smgr.report_queue, rpt_ptr, rpt_link )
  {
    if ( rpt_ptr->state == SMGR_RPT_STATE_FLUSHING )
    {
      rpt_ptr->state = SMGR_RPT_STATE_ACTIVE;
    }
  }
  return num_rpt_sent;
}

/*===========================================================================

  FUNCTION:   sns_smgr_flush_reports_to_same_processor

===========================================================================*/
/*!
  @brief Send all available samples to the given processor.

  @detail
  @param[i] proc - processor type
  @return   none
 */
/*=========================================================================*/
void sns_smgr_flush_reports_to_same_processor(sns_proc_type_e_v01 proc)
{
  smgr_rpt_spec_s *rpt_ptr;
  uint8_t num_tag_along_reports = 0;

  SMGR_FOR_EACH_Q_ITEM( &sns_smgr.report_queue, rpt_ptr, rpt_link )
  {
    if ( (rpt_ptr->state == SMGR_RPT_STATE_ACTIVE) && (rpt_ptr->proc_type == proc) &&
         (rpt_ptr->rpt_tick != 0) )
    {
      if ( (rpt_ptr->header_abstract.msg_id == SNS_SMGR_BUFFERING_REQ_V01) &&
           (rpt_ptr->q16_rpt_rate != 0) )
      {
        if ( sns_smgr_ready_reports_list_add(rpt_ptr) )
        {
          rpt_ptr->state = SMGR_RPT_STATE_FLUSHING;
          num_tag_along_reports++;
        }
      }
      else if ( rpt_ptr->header_abstract.msg_id == SNS_SMGR_REPORT_REQ_V01 )
      {
        uint8_t i;
        for ( i=0; i<rpt_ptr->num_items; i++ )
        {
          smgr_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
          if ( sns_smgr_is_event_sensor(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                        item_ptr->ddf_sensor_ptr->data_type) )
          {
            if ( sns_smgr_ready_reports_list_add(rpt_ptr) )
            {
              num_tag_along_reports++;
            }
            break;
          }
        }
      }
    }
  }
  if ( num_tag_along_reports > 0 )
  {
    SNS_SMGR_REPORT_DBG1(HIGH, "Added %d reports to ready queue", num_tag_along_reports);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_handle_disconnect_sig

===========================================================================*/
/*!
  @brief Handles SNS_SMGR_QMI_DISC_SIG signal

  @detail
  @param  none
  @return none
 */
/*=========================================================================*/
static void sns_smgr_handle_disconnect_sig(void)
{
  uint8_t i;

  for ( i=0; i<ARR_SIZE(sns_smgr.connection_list); i++ )
  {
    if ( (sns_smgr.connection_list[i].client_handle != NULL) &&
         !sns_smgr.connection_list[i].is_valid )
    {
      sns_smgr_cancel_restricted_service(&sns_smgr.connection_list[i]);
      sns_smgr_cancel_internal_service(&sns_smgr.connection_list[i]);
      sns_smgr_cancel_service(&sns_smgr.connection_list[i]);
      sns_smgr.connection_list[i].client_handle = NULL;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_handle_app_wake_sig

===========================================================================*/
/*!
  @brief Handles APP_WAKE signal

  @detail
  @param  none
  @return none
 */
/*=========================================================================*/
static void sns_smgr_handle_app_wake_sig(void)
{
  smgr_power_state_type_e curr_powerrail = sns_hw_powerrail_status();

  SNS_SMGR_PRINTF3(
     HIGH, "WAKE_SIG curr state(%d) powerrail(%d) change count(%d)",
     sns_smgr.app_is_awake, curr_powerrail, sns_smgr.app_state_toggle_count);

  sns_smgr.app_is_awake = true;
  sns_smgr.app_state_toggle_count++;

  if ( curr_powerrail == SNS_SMGR_POWER_OFF )
  {
    uint8_t i;
    for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
    {
      if ( sns_smgr.sensor[i].sensor_state == SENSOR_STATE_OFF )
      {
        sns_smgr.sensor[i].event_done_tick = sns_em_get_timestamp();
        sns_smgr_schedule_sensor_event(1);
        break; /* one is enough */
      }
    }
  }
  else
  {
    sns_smgr_flush_reports_to_same_processor(SNS_PROC_APPS_V01);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_handle_app_sleep_sig

===========================================================================*/
/*!
  @brief Handles APP_SLEEP signal

  @detail
  @param  none
  @return none
 */
/*=========================================================================*/
static void sns_smgr_handle_app_sleep_sig(void)
{
  SNS_SMGR_PRINTF2(HIGH, "SLEEP_SIG curr state(%d), change count(%d)",
                   sns_smgr.app_is_awake, sns_smgr.app_state_toggle_count);
  sns_smgr.app_is_awake = false;
  sns_smgr.app_state_toggle_count++;
}

/*===========================================================================

  FUNCTION:   sns_smgr_check_msg_pending

===========================================================================*/
/*!
  @brief check if there is any msg pending to be processed, not blocking

  @detail
  @param[i]  none
  @return   none
 */
/*=========================================================================*/
static boolean sns_smgr_check_msg_pending(void)
{
  OS_FLAGS  sig_flags;
  OS_FLAGS  wait_type;

  wait_type = SMGR_QCSI_WAIT_SIG + SNS_SMGR_QMI_PM_CL_RX_SIG + SMGR_INT_QCSI_WAIT_SIG + SMGR_RESTRICT_QCSI_WAIT_SIG;
  sig_flags = sns_os_sigs_accept(sns_smgr.sig_grp, wait_type, OS_FLAG_WAIT_SET_ANY, &err);
  return ( sig_flags || SMGR_BIT_TEST(sns_smgr.flags,SMGR_FLAGS_MESSAGE_B) ) ? TRUE : FALSE;
}
/*===========================================================================

  FUNCTION:   sns_smgr_ltcy_measure_check(void)

===========================================================================*/
/*!
  @brief Check whether latency measurement is enabled or not.

  @detial
  @param[i] none
  @return  none
 */
/*=========================================================================*/
void sns_smgr_ltcy_measure_check(void)
{
  uint8_t i;

  sns_latency.ltcy_measure_enabled = false;
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    if( sns_smgr.sensor[i].const_ptr->flags == SNS_REG_SSI_FLAG_LTCY_ENABLE )
    {
      sns_latency.ltcy_measure_enabled = true;
      break;
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_init

===========================================================================*/
/*!
  @brief initialize some flags and variables in sns_smgr

  @detail

  @param[i] none
  @return   none
 */
/*=========================================================================*/
static void smgr_init(void)
{
  SNS_OS_MEMZERO(&sns_smgr, sizeof(sns_smgr_s));
  sns_smgr.sensor_test_info.test_done = true;
  sns_smgr.sensor_test_info.save_to_registry = true;
  sns_smgr.sensor_test_info.apply_cal_now = true;
  sns_smgr.app_is_awake = true;
}

/*===========================================================================

  FUNCTION:   sns_smgr_task

===========================================================================*/
/*!
  @brief SMGRT task

  @detail

  @param[i]  p_arg
  @return   none
 */
/*=========================================================================*/
void sns_smgr_task(void* p_arg)
{
  OS_FLAGS       sig_flags;
  uint8_t       *item_ptr;
  static uint8_t cb_arg;
  boolean        is_msg_waiting=FALSE;

  unsigned int OS_FLAGS_sig_type;
  qmi_csi_service_handle smgr_service_handle=NULL;
  qmi_csi_service_handle smgr_int_service_handle=NULL;
  qmi_csi_service_handle smgr_restrict_service_handle=NULL;
  void                   *service_cookie_ptr=NULL;
  qmi_csi_os_params      smgr_os_params;
  qmi_csi_os_params      smgr_int_os_params;
  qmi_csi_os_params      smgr_restrict_os_params;
  uint32_t               smgr_qcsi_service_status;
  uint32_t               smgr_int_qcsi_service_status;
  uint32_t               smgr_restrict_qcsi_service_status;
  qmi_client_error_type  qmi_err;

  enable_num_clnt_ind = FALSE;

  /* Initialize SMGR modules */
  smgr_init();
  sns_smgr_sol_init();
  sns_smgr_sensor_init();
  sns_em_create_timer_obj(&sns_smgr_timer_cb, &cb_arg,SNS_EM_TIMER_TYPE_ONESHOT, &sns_smgr.tmr_obj);
  sns_em_create_timer_obj(&sns_smgr_sensor_ev_timer_cb, &cb_arg,SNS_EM_TIMER_TYPE_ONESHOT, &sns_smgr.sensor_ev_tmr_obj);

  /* Init signal events */
  sns_smgr.sig_grp = sns_os_sigs_create (SMGR_QCSI_WAIT_SIG, &err);
  SNS_ASSERT(sns_smgr.sig_grp != NULL);

  sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_SENSOR_READ_FLAG);
  sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_SENSOR_STATE_EV);
  sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_DD_CB_FLAG);
  sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_LDO9_FLAG);
  sns_os_sigs_add(sns_smgr.sig_grp, SMGR_INT_QCSI_WAIT_SIG);
  sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_QMI_PM_CL_RX_SIG);
  sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_APP_WAKE_SIG);
  sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_APP_SLEEP_SIG);
  sns_os_sigs_add(sns_smgr.sig_grp, SMGR_RESTRICT_QCSI_WAIT_SIG);

  smgr_cl_que_mutex_ptr = sns_os_mutex_create(SNS_SMGR_QUE_MUTEX, &err);
  SNS_ASSERT (err == OS_ERR_NONE );

  sns_q_init(&smgr_cl_queue);

  sns_os_set_qmi_csi_params(sns_smgr.sig_grp,SMGR_QCSI_WAIT_SIG,&smgr_os_params,&err);
  SNS_ASSERT(err == OS_ERR_NONE);

  sns_os_set_qmi_csi_params(sns_smgr.sig_grp,SMGR_INT_QCSI_WAIT_SIG,&smgr_int_os_params,&err);
  SNS_ASSERT(err == OS_ERR_NONE);

  sns_os_set_qmi_csi_params(sns_smgr.sig_grp,SMGR_RESTRICT_QCSI_WAIT_SIG,&smgr_restrict_os_params,&err);
  SNS_ASSERT(err == OS_ERR_NONE);

  SMGR_MSG_0(DBG_HIGH_PRIO, "SMGR : Register with QCSI and QCCI");

  /* register SMGR, SMGR INTERNAL, and SMGR RESTRICTED services with QCSI */
  smgr_qcsi_service_status = (uint32_t) qmi_csi_register(sns_smr_get_svc_obj(SNS_SMGR_SVC_ID_V01),
                                                         smgr_connect_cb,smgr_disconnect_cb,smgr_handle_req_cb,
                                                         service_cookie_ptr, &smgr_os_params, &smgr_service_handle);

  smgr_int_qcsi_service_status = (uint32_t) qmi_csi_register(sns_smr_get_svc_obj(SNS_SMGR_INTERNAL_SVC_ID_V01),
                                                             smgr_connect_cb,smgr_disconnect_cb,smgr_int_handle_req_cb,
                                                             NULL, &smgr_int_os_params, &smgr_int_service_handle);

  smgr_restrict_qcsi_service_status = (uint32_t) qmi_csi_register(sns_smr_get_svc_obj(SNS_SMGR_RESTRICTED_SVC_ID_V01),
                                                                  smgr_connect_cb,smgr_disconnect_cb,smgr_restrict_handle_req_cb,
                                                                  NULL, &smgr_restrict_os_params, &smgr_restrict_service_handle);

  /* Register as client with QCCI for PM & REG service */
  {
     qmi_client_type pm_user_handle, reg_user_handle;

     sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_QMI_PM_CL_MSG_SIG);
     sns_os_sigs_add(sns_smgr.sig_grp, SNS_SMGR_QMI_REG_CL_MSG_SIG);

     smgr_pm_cl_os_params.ext_signal = NULL;
     smgr_pm_cl_os_params.sig = SNS_SMGR_QMI_PM_CL_MSG_SIG;
     smgr_pm_cl_os_params.timer_sig = SNS_SMGR_QMI_PM_CL_TIMER_SIG;

     qmi_client_notifier_init(sns_smr_get_svc_obj(SNS_PM_SVC_ID_V01),
                              &smgr_pm_cl_os_params,
                              &pm_user_handle);

     smgr_reg_cl_os_params.ext_signal = NULL;
     smgr_reg_cl_os_params.sig = SNS_SMGR_QMI_REG_CL_MSG_SIG;
     smgr_reg_cl_os_params.timer_sig = SNS_SMGR_QMI_REG_CL_TIMER_SIG;

     qmi_err = qmi_client_notifier_init(sns_smr_get_svc_obj(SNS_REG2_SVC_ID_V01),
                                        &smgr_reg_cl_os_params,
                                        &reg_user_handle);
     if ( QMI_NO_ERR == qmi_err )
     {
       SNS_SMGR_PRINTF0(HIGH, "SMGR: qmi_client_notifier_init done");
     }
  }

  /* Inform init code that SMGR init is done */
  sns_init_done();

#ifndef ADSP_STANDALONE
   {
      qmi_idl_service_object_type service_object;
      qmi_service_info service_info;
      unsigned int num_entries=1;
      unsigned int num_services;
      qmi_client_error_type err;

#ifndef SNS_QDSP_SIM
     {
      static uint32_t pm_svc_index=SNS_PM_SVC_ID_V01;
      QMI_CCI_OS_SIGNAL_WAIT(&smgr_pm_cl_os_params, 0);
      QMI_CCI_OS_SIGNAL_CLEAR(&smgr_pm_cl_os_params);

      service_object = sns_smr_get_svc_obj(SNS_PM_SVC_ID_V01);

      err = qmi_client_get_service_list(service_object, &service_info,
                                        &num_entries, &num_services);

      if (err == QMI_CSI_NO_ERR)
      {
         err = qmi_client_init(&service_info, service_object,
                               sns_smgr_client_ind_cb, &pm_svc_index, &smgr_pm_cl_os_params,
                               &smgr_pm_cl_user_handle);
      }
     }
#endif //SNS_QDSP_SIM
     {

      static uint32_t reg_svc_index=SNS_REG2_SVC_ID_V01;
      QMI_CCI_OS_SIGNAL_WAIT(&smgr_reg_cl_os_params, 0);
      QMI_CCI_OS_SIGNAL_CLEAR(&smgr_reg_cl_os_params);

      service_object = sns_smr_get_svc_obj(SNS_REG2_SVC_ID_V01);

      err = qmi_client_get_service_list(service_object, &service_info,
                                        &num_entries, &num_services);

      if (err == QMI_CSI_NO_ERR)
      {
         err = qmi_client_init(&service_info, service_object,
                               sns_smgr_client_ind_cb, &reg_svc_index, &smgr_reg_cl_os_params,
                               &smgr_reg_cl_user_handle);
         if (err == QMI_CSI_NO_ERR)
         {
           qmi_client_register_error_cb(smgr_reg_cl_user_handle, sns_smgr_client_error_cb, NULL);
         }
         else
         {
           SNS_SMGR_PRINTF1(HIGH, "SMGR : qmi_client_init(REG2_SVC) failed err=%d", err);
         }
      }
      else
      {
        SNS_SMGR_PRINTF1(HIGH, "SMGR : qmi_client_get_service_list(REG2_SVC) failed err=%d", err);
      }
     }
   }
#endif //ADSP_STANDALONE
   OS_FLAGS_sig_type = SNS_SMGR_DD_CB_FLAG  + SNS_SMGR_SENSOR_READ_FLAG + SNS_SMGR_SENSOR_STATE_EV +
     SNS_SMGR_LDO9_FLAG + SMGR_QCSI_WAIT_SIG + SMGR_INT_QCSI_WAIT_SIG + SNS_SMGR_QMI_PM_CL_RX_SIG +
     SNS_SMGR_QMI_DISC_SIG + SNS_SMGR_APP_WAKE_SIG + SNS_SMGR_APP_SLEEP_SIG + SMGR_RESTRICT_QCSI_WAIT_SIG;

  SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
#ifdef ADSP_STANDALONE
  SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_HW_INIT_B);
#endif /* ADSP_STANDALONE */
  sns_hw_bd_config();
  sns_smgr_power_init();
  sns_smgr_md_init();

#if defined(FEATURE_TEST_DRI)
  sns_test_dri_init(NULL, NULL);
#endif

#ifdef SNS_SMGR_UNIT_TEST
  sns_smgr_test_init();
#endif
//MAG8975 unit test
#ifdef MAG8975_UNIT_TEST
  sns_dd_mag_akm8975_test();
#endif

#ifdef CONFIG_BMP085_UNIT_TEST
  sns_dd_alt_bmp085_on_target_test_main();
#endif

#ifdef CONFIG_ADXL_UNIT_TEST
  adxl350_on_target_test_main();
#endif

  /* Power will be set back to off after DD init is complete */
  sns_hw_power_rail_config( SNS_SMGR_POWER_HIGH );

/* ========================= MAIN TASK LOOP ===============================*/
  while ( 1 )
  {
     is_msg_waiting = sns_smgr_check_msg_pending();

     if ( SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B) &&
          SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_HW_INIT_B)
        )
     {
         sns_profiling_log_qdss(SNS_SMGR_FUNC_ENTER, 1, SMGR_FLAGS_DD_INIT_B);
         sns_smgr_dd_init();
         if (!SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B))
         {
           sns_os_sigs_post(sns_smgr.sig_grp, SMGR_QCSI_WAIT_SIG, OS_FLAG_SET, &err);
           sns_os_sigs_post(sns_smgr.sig_grp, SMGR_INT_QCSI_WAIT_SIG, OS_FLAG_SET, &err);
           sns_os_sigs_post(sns_smgr.sig_grp, SMGR_RESTRICT_QCSI_WAIT_SIG, OS_FLAG_SET, &err);

           //check the registry flags if latency measurement enabled
           sns_smgr_ltcy_measure_check();
         }
     }

#ifndef SNS_PLAYBACK_SKIP_SMGR
     if ( SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DATA_SAMPLING_B) )
     {
       SMGR_BIT_CLEAR(sns_smgr.flags, SMGR_FLAGS_DATA_SAMPLING_B);
       sns_profiling_log_qdss(SNS_SMGR_FUNC_ENTER, 1, SMGR_FLAGS_DATA_SAMPLING_B);
       sns_smgr_sampling_cycle();
       if (!is_msg_waiting )
       {
         continue;
       }
     }
     if ( SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DATA_GATHERING_B) )
     {
       SMGR_BIT_CLEAR(sns_smgr.flags, SMGR_FLAGS_DATA_GATHERING_B);
       sns_profiling_log_qdss(SNS_SMGR_FUNC_ENTER, 1, SMGR_FLAGS_DATA_GATHERING_B);
       sns_smgr_data_gathering_cycle();
       if (!is_msg_waiting )
       {
         continue;
       }
     }
#endif

     if ( SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DD_SERVICE_B) )
     {
       SMGR_BIT_CLEAR(sns_smgr.flags, SMGR_FLAGS_DD_SERVICE_B);
       sns_profiling_log_qdss(SNS_SMGR_FUNC_ENTER, 1, SMGR_FLAGS_DD_SERVICE_B);
       sns_smgr_dd_service();
       if (!is_msg_waiting )
       {
         continue;
       }
     }

     if ( SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_SENSOR_EVENT_B) )
     {
       SMGR_BIT_CLEAR(sns_smgr.flags, SMGR_FLAGS_SENSOR_EVENT_B);
       sns_profiling_log_qdss(SNS_SMGR_FUNC_ENTER, 1, SMGR_FLAGS_SENSOR_EVENT_B);
       smgr_process_sensor_event();
       if (!is_msg_waiting )
       {
         continue;
       }
     }

     if ( SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_MESSAGE_B) )
     {
       sns_profiling_log_qdss(SNS_SMGR_FUNC_ENTER, 1, SMGR_FLAGS_MESSAGE_B);
       while ( 1 )
       {
         item_ptr = (uint8_t*)sns_smgr_rcv();
         if  (item_ptr)
         {
           sns_smgr_process_msg(item_ptr);
         }
         else
         {
           /* no more message in the SMR message queue for processing */
           SMGR_BIT_CLEAR(sns_smgr.flags, SMGR_FLAGS_MESSAGE_B);
           break;
         }
       }
       /* Go to top of outer while loop after any received message(s) */
       continue;
     }

     sns_profiling_log_timestamp((uint64_t) SNS_SMGR_LATENCY_PROFILE_END);  /*end of smgr process latency profiling*/
     sns_profiling_log_qdss(SNS_SMGR_EXIT, 0);

     /* OS_FLAG_CONSUME will make the SMR clear the flag after the call */
     sig_flags = sns_os_sigs_pend(sns_smgr.sig_grp, OS_FLAGS_sig_type,
                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);

     sns_profiling_log_qdss(SNS_SMGR_ENTER, 1, sig_flags);
     sns_profiling_log_timestamp((uint64_t) SNS_SMGR_LATENCY_PROFILE_START); /*start of smgr process latency profiling*/

     if ( sig_flags & SNS_SMGR_QMI_DISC_SIG )
     {
       sns_smgr_handle_disconnect_sig();
     }
     if ( sig_flags & SNS_SMGR_APP_WAKE_SIG )
     {
       sns_smgr_handle_app_wake_sig();
     }
     if ( sig_flags & SNS_SMGR_APP_SLEEP_SIG )
     {
       sns_smgr_handle_app_sleep_sig();
     }
     if ( sig_flags & SNS_SMGR_SENSOR_READ_FLAG )
     {
       SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DATA_SAMPLING_B);
     }
     if ( sig_flags & SNS_SMGR_SENSOR_STATE_EV )
     {
       SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_SENSOR_EVENT_B);
     }
     if ( sig_flags &  SNS_SMGR_DD_CB_FLAG ) /* Device Driver Call Back */
     {
       SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_SERVICE_B);
     }
     if (sig_flags & SNS_SMGR_QMI_PM_CL_RX_SIG)
     {
       SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_MESSAGE_B);
     }
     if ( (sig_flags & SMGR_QCSI_WAIT_SIG) && (!SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B) ) )
     {
       // REQUEST from clients to SMGR service
       qmi_csi_handle_event(smgr_service_handle, &smgr_os_params);
     }
     if ( ( sig_flags & SMGR_INT_QCSI_WAIT_SIG)  && (!SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B) ) )
     {
       // REQUEST from clients to SMGR internal service
       qmi_csi_handle_event(smgr_int_service_handle, &smgr_int_os_params);
     }
     if ( ( sig_flags & SMGR_RESTRICT_QCSI_WAIT_SIG)  && (!SMGR_BIT_TEST(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B) ) )
     {
       // REQUEST from clients to SMGR restricted service
       qmi_csi_handle_event(smgr_restrict_service_handle, &smgr_restrict_os_params);
     }
     if ( sig_flags & SNS_SMGR_LDO9_FLAG )
     {
       sns_hw_send_powerrail_msg_tmr_cb_proc();
     }
     if ( sig_flags & (~OS_FLAGS_sig_type))
     {
       /* unknown signal received */
       sns_smgr_err_handle(SNS_ERR_UNKNOWN);
     }
  } /* while */
}

/*===========================================================================

  FUNCTION:   sns_smgr_init

===========================================================================*/
/*!
  @brief This function starts smgr task

  @detail

  @param   none
  @return  return SNS_SUCCESS always
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_init(void)
{
  sns_os_task_create_ext(sns_smgr_task, NULL,
                         &sns_smgr.SMGRTask[SNS_MODULE_STK_SIZE_DSPS_SMGR-1],
                         SNS_MODULE_PRI_DSPS_SMGR,
                         SNS_MODULE_PRI_DSPS_SMGR,
                         &sns_smgr.SMGRTask[0],
                         SNS_MODULE_STK_SIZE_DSPS_SMGR,
                         (void *)0,
                         OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                         (uint8_t *)"SNS_SMGR");
  return SNS_SUCCESS;
}

/*  =============== sns_smgr_err_handle ===============
   This function cprints out error code.

*/
/*===========================================================================

  FUNCTION:   sns_smgr_err_handle

===========================================================================*/
/*!
  @brief This function prints out error code.

  @detail
  @param[i] ec  error code
  @return   none
 */
/*=========================================================================*/
void sns_smgr_err_handle(sns_err_code_e ec)
{
  SMGR_LOG("Error(=%d) occurred during SMR testing.....\n\r", ec);
}

/*===========================================================================

  FUNCTION:   sns_smgr_app_state_change

===========================================================================*/
/*!
  @brief Called by Power module when AP state changes

  @detail
  @param[i] app_is_asleep - TRUE if AP entered sleep state, FALSE wake state
  @return   none
 */
/*=========================================================================*/
void sns_smgr_app_state_change(bool app_is_asleep)
{
  uint32_t set_bit = app_is_asleep ? SNS_SMGR_APP_SLEEP_SIG : SNS_SMGR_APP_WAKE_SIG;
  uint32_t clr_bit = app_is_asleep ? SNS_SMGR_APP_WAKE_SIG  : SNS_SMGR_APP_SLEEP_SIG;
  sns_os_sigs_post ( sns_smgr.sig_grp, clr_bit, OS_FLAG_CLR, &err );
  sns_os_sigs_post ( sns_smgr.sig_grp, set_bit, OS_FLAG_SET, &err );
}

