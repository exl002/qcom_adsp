/*=============================================================================
  @file sns_smgr_sensor.c

  This file contains the logic for managing sensor data sampling
  in the DSPS Sensor Manager (SMGR)

*******************************************************************************
* Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_sensor.c#6 $ */
/* $DateTime: 2014/08/18 11:42:01 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-08-12  sd   make sns_smgr_compute_device_sampling_factor available for other SMGR files.
  2014-07-28  sd   clear sensor depot when all reports for this data type are deleted
  2014-06-11  sd   after a report to app processor is ready to send, flush all reports 
                   from app processor
  2014-06-03  sd   Use correct sensor dimension in notify_data
  2014-05-19  jms  Flush fifo before ODR change.
  2014-05-16  sd   refactor code for odr changed event and enable scheduler for polling mode
  2014-05-15  sd   flush reports after odr change event
  2014-05-12  MW   Enhanced SNS_DDF_ATTRIB_LOWPASS handling
  2013-03-20  pn   Suppresses initial invalid samples in reports
  2013-03-14  pn   Fixed timestamp rollover bug in binary search function
  2014-03-10  jms  Klockwork fixes
  2014-02-27  sd   Interpolate samples even when accurate time is requested if the 
                   potential report rate is more/less than certain % of requested rate.  
  2014-02-26  MW   Add CCT as event sensor
  2014-01-29  hw   Add support for ambient temp sensor type
  2014-01-16  hw   Fix latency node voting according to ADSP wakeup rate
  2014-01-15  pn   Properly handles asynch sensor reporting of failure to get_data()
  2014-01-12  jms  Merged discrete with unified fifo support
  2014-01-07  pn   - Correctly determines if HP ODRs should be used
                   - Correctly determines if ODR table should be used
  2013-12-18  jms  General fifo support
  2013-12-18  pn   No longer adjusts Buffering report schedule when sensor config completes
  2013-12-18  MW   Added support for Hall Effect sensor  
  2013-12-12  pn   - Each sensor is reset as soon as its off-to-idle wait is over
                   - First polling starts as soon as sensor enters READY state
                   - Initial invalid samples are now sent for Periodic reports
  2013-11-26  pn   - Uses exact timestamp for last, or only, sample from FIFO in a set
                   - Polling rate of 50Hz no longer disables use of FIFO
  2013-11-13  pn   Considers existing event sensor sample when setting up reports.
  2013-11-06  sd   updated FIFO log msg 
  2013-11-04  hw   Added QDSS SW events in smgr framework
  2013-10-31  sc   Added NULL check before calling self-sched driver functions.
  2013-10-23  MW   Added support for SAR sensor
  2013-10-24  pn   Fixed timing issues for reports involving multiple sensors
  2013-10-21  pn   Correctly checks for Query client when configuring FIFO
  2013-10-16  pn   Made sns_smgr_reset_sensor() global
  2013-10-09  pn   Polling sensors coupled with FIFO sensor no longer polled
                   out of schedule
  2013-10-03  pn   Buffering and Periodic report generation changes
  2013-09-26  pn   Correctly reschedules report when report interval changes
  2013-09-18  pn   Triggers report generation when FIFO data are received
  2013-09-12  MW   Added support for RGB sensor type
  2013-09-06  pn   Fixed timestamp calculation for FIFO data
  2013-08-29  sd   Not use interpolation if odr matches requested sample rate
  2013-09-12  DC   Support for gesture type
  2013-09-03  pn   All samples for event sensors are sent to clients
  2013-08-22  jhh  Change latency voting according to ADSP wake up rate
  2013-07-31  vh   Eliminated Klocwork warnings
  2013-08-28  lka  Fixed debug macros for FIFO-related messages.
  2013-08-21  pn   Restored sns_smgr_flush_reports_to_same_processor() signature
  2013-08-19  pn   - Updated sns_smgr_flush_reports_to_same_processor()
                   - Sensor state transition timer is started only when necessary
  2013-07-15  dc   Support for humidity sensor type
  2013-07-10  asj  Removed post-proc buffering for magnetometer samples
  2013-07-06  sd   Changed when disabling FIFO, sensor doesn't need to be in ready state
  2013-07-04  sd   Recovered FIFO change from merging
  2013-07-01  hw   Add Sampling latency support
  2013-06-18  pn   Added support for back-to-back Buffering indications
  2013-06-12  agk  Vote to latency node to enable/disable Power Collapse
  2013-06-12  pn   - Interpolator object gets updated with initial sample only if
                     no indications have been generated
                   - Report interval involving interpolated samples is reduced
                     by desired sampling interval only if necessary to prevent
                     loss of samples
                   - Clears sensor's ITEM_ADD flag when item is deleted
  2013-06-11  ae   Added QDSP SIM playback support
  2013-06-10  sd   Changed report interval calculation to support long report interval over 1hz
  2013-06-06  pn   Report indications suspended/resumed on AP state change
  2013-05-24  asr  Fix to pass appropriate data sample to sns_smgr_handle_new_sample()
                   in notify_data function.
  2013-05-22  asr  Fixed FIFO flush issue when FIFO Overflow event occurs
                   and watermark is reduced to 1.
  2013-05-21  pn   Corrected report interval calculation involving interpolated samples
  2013-05-22  lka  Added Double-Tap as an event sensor.
  2013-05-20  br   Inserted validity checking and fixed possible array boudary line
  2013-05-16  asr  Removed unused log types.
  2013-05-05  asr  Added support for FIFO and did some code cleanup.
  2013-05-01  pn   Added debug logging for biases received from driver
  2013-05-01  lka  Added support for SNS_DDF_EVENT_ODR_CHANGED.
  2013-04-28  vh   Sensor state validation before calling the driver
  2013-04-25  pn   Fixed timing issues occured when sample quality transitioned
                   between interpolated and/or filtered and current sample
  2013-04-22  br   Inserted a line to call md_update() in process_sensor_event()
  2013-04-18  br   Replaced ASSERT to a log message in sns_smgr_config_odr()
  2013-04-11  pn   Replaced ASSERT from sns_smgr_interpolator_update()
  2013-04-09  pn   - Changed effective_rate_hz to q16
                   - Refactored sns_smgr_update_items_info()
                   - Fixed issue with CIC factor computation
  2013-04-04  pn   - Enables support for Secondary type without Primary type
                   - Allows data type to be configured
  2013-04-04  pn   Clears interpolation object when interpolation is not required
  2013-03-29  pn   Refactored function that computes depot data rate
  2013-03-21  pn   Fixed conditions for (re)scheduling Buffering report
  2013-03-11  pn   - Adjusted Buffering report interval and scheduling of initial report
                   - Properly handles periodic-like Buffering reports
  2013-03-01  pn   Further simplified sensor state machine
  2013-02-15  pn   Correctly determines timestamp for first interpolated sample
  2013-02-12  ar   Fixed the calibration flags in sns_ddf_smgr_notify_event function
  2013-02-10  sc   Error logging (instead of assert) upon register-timer failure
  2013-02-07  pn   - Changed report scheduling
                   - Changed when to update latency node
  2013-02-05  pn   Fixed potential endless loop in sns_smgr_interpolator_update()
  2013-01-30  pn   - Simplified sensor state machine
                   - Reduced config time for sibling sensors
  2013-01-24  jhh  Add new boundary for sampling frequency < 2Hz
  2013-01-17  pn   Fixed problem configuring sensors supporting LPF attribute
                   but not ODR attribute
  2013-01-13  sd   added retry registrating timer in case of failure
  2013-01-09  pn   Fixed issue with sample quality determination
  2013-01-08  sd   turning on power rail at power up event before reset sensors
                   clear data_poll_ts at no sample event
  2012-12-26  pn   Fixed various timing failures
  2012-12-21  br   Updated interpolation function
  2012-12-19  pn   - INTERPOLATED_FILTERED samples are not properly created
                   - Also considers sibling sensors in Polling mode
  2012-12-19  pn   Clears interpolator data when not in use
  2012-12-19  jhh  Add filters to vote different latency value depending on
                   max_odr
  2012-12-14  pn   Sensor events now get their own timer
  2012-12-10  pn   Recomputes sampling factor when max requested rate changes
  2012-12-08  pn   - Fixed issue of sensor being stuck in CONFIGURING state when
                   report items are rapidly added and/or removed
                   - Adjusted report interval computation to fix issue of buffer
                   overflow at high sampling rate
                   - Recomputes sampling factor when max requested rate changes
                   in Polling mode
  2012-12-05  pn   Powers off sensor device if MD interrupt is also disabled
                   when processing NO_SAMPLE event
  2012-12-03  pn   Added Buffering feature
  2012-09-13  vh   Eliminated compiler warnings
  2012-07-02  sd   optimization of code
  2012-06-27  sd   Fixed Klocwork error/warning
  2012-06-20  sd   use max of idle to ready time and LPF_DURATION when sensor transit from wakeup state to ready
                   state and ODR is configured.
  2012-06-19  sd   Fixed first few initial sample problem. Moved configuring ODR after sensor
                   becomes idle state.
  2012-06-18  br   Clears ODR value so so that SMGR can mark INVALID to the beginning samples.
  2012-06-15  sd   Added configuring 2ndary data type event
                   Mark data invalid during configuring state
  2012-06-13  sd   Added to put sensors to low power mode upon power rail is turned on
  2012-06-11  pn   Added sns_smgr_set_sensor_state()
  2012-06-06  br   cleared the current odr value to 0 when EVENT_NO_SAMPLE
  2012-06-05  ag   after self test, reset sensor, restore state and set power rail
                   correctly
  2012-05-21  sd   avoid divide by 0 by SMGR_LPF_DURATION
  2012-05-16  br   fixed stop streaming when CIC filter is enabled
  2012-05-07  sd   use the right index for nested for loop
  2012-04-30  br   Added latency node support
  2012-04-22  br   Fixed for event sensor types for no indication and added keeping sample counts
  2012-04-18  br   Enabled qup clock when calling enable_sched_data()
  2012-04-13  br   Fixed memory leakage after getting ODR attribute
  2012-03-29  pn   Added support for UPDATE_REGISTRY_GROUP event
  2012-03-18  br   Adjusted initial dri_cur_count_down and modified notify_data() to for INVALID samples
  2012-03-16  sd   Fixed to make sensor to stream at requested frequency when concurrency use case
  2012-03-15  br   Fixed to indicate for the very first samples for Sensors that does't have LBF
  2012-03-13  sd   added nullpointer check
  2012-03-13  br   inseted lines to call enable_sched_data(FALSE) when a client is exiting
  2012-03-11  br   enabled seconday sensor support
  2012-03-06  pn   Added DRI test support
  2012-03-05  br   Added DRI fuctionality
  2012-02-02  br   Changed to apply GYRO_MINIMUM_ODR only if there is any client for GYRO
  2012-01-20  sc   Fixed the condition for applying calibration upon BIAS_READY event
  2012-01-17  sc   Handle calibration data in self-test according to test info parameters
  2011-12-27  sd   Moved qup clock on/off just before/after calling DD interface
  2011-12-27  sd   Fixed potential back to back request problem
                   Do not change qup clock state when setting DD attribute
  2011-12-12  sc   Reset autocal bias in the event of factory bias update
  2011-11-27  br   Fixed the array index for type_info_ptr in smgr_get_data().
  2011-11-22  br   Changed is_other_type_scheduled() function.
  2011-11-18  sc   Fix compilation warnings
  2011-11-16  sd   moved turn off qup clock inside sensor reset function
  2011-11-08  sd   turn off qup clock after sensor reset
  2011-10-20  sd   Use Consistent Pointer Type Between SMGR/DD When Setting Attributes
  2011-10-19  br   removed lines using "lpf_invert" in  set_fillter()
  2011-10-10  sd   only get data if sensor state is ready ( when we delete a request, notify data
                   could be called afterwards, at that time, sensor is not ready , so need to ignore it)
  2011-10-07  sd   moved sns_smgr_proc_sensor_event out of sns_smgr_reset_sensor
  2011-10-05  br   inserted functions/lines for MPU6050
  2011-10-05  sd   removed invalid data in the middle of stream at 1hz because of cic_factor
  2011-10-02  jh   adds diag message in smgr_get_data()
  2011-09-29  sd   set gyro minimum ODR to 25
                   reset sensor after off to idle timer expires after power rail is turned on
  2011-09-22  sd   do not report when the sensor is not in ready state ( ex configuring filter)
  2011-09-12  sd   added debug msg for proc event
  2011-09-09  sc   Update with registry service V02
  2011-09-09  sd   Set sensor to low power mode when it goes to idle when no sample event happens.
                   remove set power rail before/after set DD attribute
  2011-09-06  sc   Update the calibration info for the sensor after bias event
  2011_08-31  sc   fixed compilation error because of type mismatch
  2011-08-31  sd   Set the range during wake up process since sensor could be reset
                   then wake up.
  2011-08-31  sc   Update registry bias upon receiving SNS_DDF_EVENT_BIAS_READY
  2011-08-26  sc   Added more features upon test completion (reset, get bias attrib, etc.)
  2011-08-23  sd   added resetting state for sensor
  2011-08-22  sc   Added function for driver to notify self-test completion.
  2011-08-11  br   fixed cic related operations
  2011-07-18  br   seperated sensor configuration table into the other file
  2011-07-14  br   fixed not to delete sensor from the event que in sns_smgr_proc_sensor_event()
  2011-07-13  br   updated/fixed in cic releated functions such as proc_cic_schedule,
                   limited cic filter factor
  2011-07-11  br   fixed cic filter related functions and improved sns_smgr_cic_update().
  2011-07-05  sc   Fixed compilation warnings
  2011-06-28  br   deleted unused function which is sns_smgr_calibration()
  2011-06-11  br   changed to support auto cal in addition to factory cal
  2011-06-19  sd   fix replaced function sns_smgr_set_data_gathering_cycle_flag by sns_smgr_set_sampling_cycle_flag
  2011-06-17  sd   smgr code clean up
  2011-06-13  sd   enable/disable qup clock in sns_smgr_proc_cic_schedule since it calls get data
  2011-06-11  br   Changed to support cic filtering
  2011-06-08  br   changed when SMGR_DYNAMIC_LPF_ON is disabled
  2011-05-25  br   chagned scheduling related functions for bug fixes and to process false alarm timer
  2011-05-19  br   bug fixed: err at setting SENSORTYPE_STATE_READ_PENDING for ALS/PRX
  2011-05-16  br   sample together both pri and secondary type if there are the requests among whole reports
                   (previously, only considered within one report)
  2011-05-13  br   changed because of smgr_tick_s definition was changed
  2011-05-12  br   used need based timer instead of heartbeat for scheduling
  2011-05-11  jb   Remove unused constants
  2011-05-06  br   Refactored sns_smgr_proc_rpt_schedule() and bug fixed in sns_ddf_smgr_notify_data()
  2011-05-04  jb   moved sensor init code to init.c, headed for slow memory
  2011-05-03  sd   Moved state check of qup clock and powerrail inside update functions
  2011-04-29  sd   modified a typo
  2011-04-28  jh   inserted power toggling feature between samples
  2011-04-28  sd   updated sns_smgr_notify_event to handle two events
  2011-04-28  jh   Added parentheses in case statement to avoid compiler fault.
  2011-04-27  sd   Changed to avoid compilation internal fault error.
  2011-04-25  sd   Added HW MD int and power saving related features
  2001-04-25  br   disabled REG operation when SMGR_REG_OFF defined
  2001-04-24  br   deployed the result of the code review. fixed bugs after unit testing with PCSIM
  2001-04-20  br   changed for the new scheduler which deployed fixed heartbeat
  2011-04-01  jb   Reenter calibration and registry
  2011-03-29  ad   Fix the gyro calibration report log version
  2011-03-27  jh   Properly use SMGR_DD_INIT_DELAY_US when setting delay
  2011-03-27  jh   Add support to insert delay before device driver initialization
                   to work around I2C issue
  2011-03-25  jh   Revert calibration and registry related changes
  2011-03-16  jb   Get calibration and/or driver data from registry
                   Unblock setting of LPF for gyro (blocked on 2/28). Passes test if
                   startup delay is set to 250 ms
  2011-03-15  sd   added calibration support
  2011-02-28  sd   initialize sensor status structure
  2011-02-28  sd   Set LPF for accel only
  2011-02-23  jb   For ADXL accel, control filtering indirectly by controlling
                   device internal sampling rate
  2011-02-10  jb   Low pass filter values were not reaching hardware registers. Fixed
  2011-02-04  jb   Fix long delay before startup. Change config flag to clarify which
                   devices are asynchronous
  2011-02-04  sd   Fixed railing flag for 2ndary data type was wrong.
  2011-01-28  br   changed constant names for deploying QMI tool ver#2
  2011-01-26  jb   Adjust next tick to heartbeat beyond current time
  2010-12-21  jb   Fix async read logic for irregular intervals and missing samples
  2010-12-13  jb   Revise PlanCycle to avoid irregular sampling at startup
  2010-12-13  sd   Added f3 msg when DD init fails
  2010-12-09  sd   new fix for PROX b2b issue
  2010-11-22  sd   make sensor readstate to SENSOR_READ_STATE_INACTIVE in sensor action excpet when
                   reading long data, to fix PROX back to back read data problem
  2010-11-18  jb   Set LPF bandwidth
  2010-11-15  jb   Set ItemQuality to invalid when sensor is not ready
                   Ensure SensorId and DataType are present in reported data item structure
                   Fix bad values at startup due to changing range
                   Fix warning
  2010-11-12  sc   Fixed asynchronous driver data read; integrated with magnetometer and PRX/ALS drivers.
  2010-10-28  sd   CR fix  #261495 Sensors, 8660 Native: Internal concurrent sensor data stops streaming after 3 minutes
  2010-10-22  sd   Fixed reporting sensitivity with the right values
  2010-10-20  sd   Fixed gyro sensitivity is not set as requested in smgr report request
  2010-10-15  ad   Update quality field in manager report if driver reports error
  2010-10-14  ad   Fix sensor action to send the right sensor type to driver
  2010-10-13  ad   Update the sensor id and data type in the manager report
  2010-10-13  sd   turn on sensor power if sensor is used
  2010-10-12  jb   Prevent power down between samples at low rate
  2010-10-11  sd   Fixed get no accel data
  2010-10-11  jb   Make sensitivity and automatic range selection optional.
                   Recognize short read based on flag
  2010-10-07  jb   Fix sequencing in device driver init
  2010-10-06  jb   Change DDF API. Add memhandler. finalize multi sensor device init
  2010-10-05  jb   CC3 initial build
  2010-09-28  SD   Added sensitivity request support for ACCEL
  2010-09-07  JB   Correct I2C bus read option
  2010-09-03  JB   Enable report generator. Fix warnings
  2010-08-30  JB   Add driver installer
  2010-08-23  JB   Add initialization for accel
  2010-08-20  JTL  Use EM functionality
  2010-08-20  JB   Move sensor type defs to define.h. Force sensitivity low for cc2.
                   Add HW LPF. Handle errors for get_data. Add dynamic range.
                   Revise DDF API.
  2010-08-09  JB   Add fail logic. Handle sensor errors
  2010-08-06  JB   Complete merge
  2010-08-04  JB   Merge with revised ucos, use PC timer
  2010-07-16  JB   Added data cycle planner and driver
  2010-07-09  JB   Start coding cycle planner
  2010-06-11  JB   Partial code as needed for SOL unit testing

============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <math.h>

#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"

#include "sns_smgr_define.h"
#include "sns_dd.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "fixed_point.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_smgr_hw.h"
#include "sns_reg_api_v02.h"

#include "sns_profiling.h"
#if defined(FEATURE_TEST_DRI)
#include "sns_smgr_test_dri.h" /* for DRI test purpose */
#endif

#if defined(SNS_QDSP_SIM)
#include "sns_qdsp_playback_utils.h"
#endif
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
//#define SNS_SMGR_DEPOT_DEBUG
#ifdef SNS_SMGR_DEPOT_DEBUG
#define SNS_SMGR_DEPOT_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_DEPOT_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_DEPOT_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_DEPOT_DBG3 SNS_SMGR_PRINTF3
#define SNS_SMGR_DEPOT_DBG4 SNS_SMGR_PRINTF4
#else
#define SNS_SMGR_DEPOT_DBG0(level,msg)
#define SNS_SMGR_DEPOT_DBG1(level,msg,p1)
#define SNS_SMGR_DEPOT_DBG2(level,msg,p1,p2)
#define SNS_SMGR_DEPOT_DBG3(level,msg,p1,p2,p3)
#define SNS_SMGR_DEPOT_DBG4(level,msg,p1,p2,p3,p4)
#endif

//#define SNS_SMGR_SAMPLING_DEBUG
#ifdef SNS_SMGR_SAMPLING_DEBUG
#define SNS_SMGR_SAMPLING_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_SAMPLING_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_SAMPLING_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_SAMPLING_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_SAMPLING_DBG0(level,msg)
#define SNS_SMGR_SAMPLING_DBG1(level,msg,p1)
#define SNS_SMGR_SAMPLING_DBG2(level,msg,p1,p2)
#define SNS_SMGR_SAMPLING_DBG3(level,msg,p1,p2,p3)
#endif

//#define SNS_SMGR_INTERPOLATOR_DEBUG
#ifdef SNS_SMGR_INTERPOLATOR_DEBUG
#define SNS_SMGR_INTERP_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_INTERP_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_INTERP_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_INTERP_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_INTERP_DBG0(level,msg)
#define SNS_SMGR_INTERP_DBG1(level,msg,p1)
#define SNS_SMGR_INTERP_DBG2(level,msg,p1,p2)
#define SNS_SMGR_INTERP_DBG3(level,msg,p1,p2,p3)
#endif

//#define SNS_SMGR_SAMPLE_PROC_DEBUG
#ifdef SNS_SMGR_SAMPLE_PROC_DEBUG
#define SNS_SMGR_SAMPLE_PROC_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_SAMPLE_PROC_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_SAMPLE_PROC_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_SAMPLE_PROC_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_SAMPLE_PROC_DBG0(level,msg)
#define SNS_SMGR_SAMPLE_PROC_DBG1(level,msg,p1)
#define SNS_SMGR_SAMPLE_PROC_DBG2(level,msg,p1,p2)
#define SNS_SMGR_SAMPLE_PROC_DBG3(level,msg,p1,p2,p3)
#endif


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
#ifdef SNS_QDSP_SIM
#define QDSP_PLAYBACK_SENSOR_READY_TIME_US 1000 
extern sns_dri_sim_s sns_dri_sim;
extern bool md_int_happened;
extern int playback_next_sample_ts;
#endif

/*----------------------------------------------------------------------------
 *  Function Prototypes
 * -------------------------------------------------------------------------*/
static void sns_smgr_config_odr_lpf(smgr_sensor_s* sensor_ptr);
static sns_ddf_time_t smgr_get_scheduled_sampling_time( const smgr_ddf_sensor_s* ddf_sensor_ptr );

void smgr_get_data(smgr_ddf_sensor_s* in_ddf_sensor_ptr);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_smgr_sample_depot_clear

===========================================================================*/
/*!
  @brief Clears sample depot for given sensors

  @param[i] sensor_ptr - the sensor of interest

  @return none
*/
/*=========================================================================*/
static void sns_smgr_sample_depot_clear(smgr_sample_depot_s* depot_ptr,
                                        uint16_t num_save)
{
  if ( (depot_ptr != NULL) && (depot_ptr->used_bufs > num_save) )
  {
    SNS_SMGR_PRINTF3(
      LOW, "depot_clear - used=%d first_ts=%u last_ts=%u", depot_ptr->used_bufs,
      depot_ptr->used_bufs ? depot_ptr->samples[depot_ptr->first_idx].time_stamp : 0,
      depot_ptr->used_bufs ? depot_ptr->samples[depot_ptr->last_idx].time_stamp : 0 );
    depot_ptr->first_idx = (depot_ptr->last_idx + depot_ptr->max_bufs -
                            num_save + 1) % depot_ptr->max_bufs;
    depot_ptr->used_bufs = num_save;
    SNS_SMGR_PRINTF3(
      LOW, "depot_clear - used=%d first_idx=%d last_idx=%d",
      depot_ptr->used_bufs, depot_ptr->first_idx, depot_ptr->last_idx);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_max_sample_count

===========================================================================*/
/*!
  @brief Returns max number of samples that can be saved in the given depot.

  @param[i] depot_ptr - the sample depot

  @return Count of buffers
*/
/*=========================================================================*/
uint16_t sns_smgr_get_max_sample_count(smgr_sample_depot_s* depot_ptr)
{
  uint16_t count = 0;
  if ( depot_ptr != NULL )
  {
    count = depot_ptr->max_bufs;
  }
  return count;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_sample_count

===========================================================================*/
/*!
  @brief Returns number of samples in the given depot.

  @param[i] depot_ptr - the sample depot

  @return Count of used buffers
*/
/*=========================================================================*/
uint16_t sns_smgr_get_sample_count(smgr_sample_depot_s* depot_ptr)
{
  uint16_t count = 0;
  if ( depot_ptr != NULL )
  {
    count = depot_ptr->used_bufs;
  }
  return count;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_latest_sample

===========================================================================*/
/*!
  @brief Returns pointer to latest buffered sample of a given sensor/data type pair.

  @param[i] sensor_ptr - the sensor of interest
  @param[i] type       - primary or secondary

  @return Read-only poinnter to latest sample, or NULL if no sample available
*/
/*=========================================================================*/
const smgr_sample_s* sns_smgr_get_latest_sample(smgr_sample_depot_s* depot_ptr)
{
  const smgr_sample_s* sample_ptr = NULL;
  if ( (depot_ptr != NULL) && (depot_ptr->used_bufs > 0) )
  {
    sample_ptr = &depot_ptr->samples[depot_ptr->last_idx];
  }
  return sample_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_oldest_sample

===========================================================================*/
/*!
  @brief Returns pointer to oldest buffered sample of a given sensor/data type pair.

  @param[i] sensor_ptr - the sensor of interest
  @param[i] type       - primary or secondary

  @return Read-only poinnter to oldest sample, or NULL if no sample available
*/
/*=========================================================================*/
const smgr_sample_s* sns_smgr_get_oldest_sample(smgr_sample_depot_s* depot_ptr)
{
  const smgr_sample_s* sample_ptr = NULL;
  if ( (depot_ptr != NULL) && (depot_ptr->used_bufs > 0) )
  {
    sample_ptr = &depot_ptr->samples[depot_ptr->first_idx];
  }
  return sample_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_depot_bsearch

===========================================================================*/
/*!
  @brief Performs binary search to find the sample of desired timesamp 
         within the given range of indices.

  @param[i/o] iter_ptr   the iterator on which to operate
  @param[i]   desired_ts the desired timestamp
  @param[i]   low_idx    index of first sample in the series
  @param[i]   hi_idx     index of last sample in the series

  @return Pointer to the sample if found, NULL if not
*/
/*=========================================================================*/
static const smgr_sample_s* sns_smgr_depot_bsearch(
  smgr_sensor_depot_iter_s* iter_ptr,
  sns_ddf_time_t desired_ts,
  uint16_t low_idx,
  uint16_t hi_idx
)
{
  const smgr_sample_s* sample_ptr = NULL;
  SNS_SMGR_DEPOT_DBG4(
     MED, "bsearch - idx=%d/%d ts=%u/%u", low_idx, hi_idx,
     iter_ptr->depot_ptr->samples[low_idx].time_stamp,
     iter_ptr->depot_ptr->samples[hi_idx].time_stamp);

  if ( (hi_idx - low_idx) < 2 )
  {
    if ( TICK1_GEQ_TICK2(iter_ptr->depot_ptr->samples[low_idx].time_stamp, desired_ts) )
    {
      iter_ptr->idx = low_idx;
      sample_ptr = &iter_ptr->depot_ptr->samples[iter_ptr->idx];
    }
    else if ( TICK1_GEQ_TICK2(iter_ptr->depot_ptr->samples[hi_idx].time_stamp, desired_ts) )
    {
      iter_ptr->idx = hi_idx;
      sample_ptr = &iter_ptr->depot_ptr->samples[iter_ptr->idx];
    }
  }
  else if ( TICK1_GEQ_TICK2(iter_ptr->depot_ptr->samples[hi_idx].time_stamp, desired_ts) )
  {
    uint16_t mid_idx = low_idx + ((hi_idx - low_idx) >> 1);
    if ( TICK1_GTR_TICK2(iter_ptr->depot_ptr->samples[mid_idx].time_stamp, desired_ts) )
    {
      /* desired timestamp is within lower portion */
      sample_ptr = sns_smgr_depot_bsearch(iter_ptr, desired_ts, low_idx, mid_idx);
    }
    else if ( TICK1_GTR_TICK2(desired_ts, 
                              iter_ptr->depot_ptr->samples[mid_idx].time_stamp) )
    {
      /* desired timestamp is within upper portion */
      sample_ptr = sns_smgr_depot_bsearch(iter_ptr, desired_ts, mid_idx, hi_idx);
    }
    else
    {
      iter_ptr->idx = mid_idx;
      sample_ptr = &iter_ptr->depot_ptr->samples[iter_ptr->idx];
    }
  }
  return sample_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_depot_iter_ext

===========================================================================*/
/*!
  @brief Initializes the given iterator with the given sample depot and
         returns the sample of given timestamp or first samples in depot if
         given timestamp is in the past.

  @param[in]  depot_ptr sample depot of a sensor
  @param[in]  ts        timestamp of sample to find in depot
  @param[out] iter_ptr  the iterator to be initialized

  @return Pointer to the sample if found, NULL if not
*/
/*=========================================================================*/
const smgr_sample_s* sns_smgr_init_depot_iter_ext(
  const smgr_sample_depot_s* depot_ptr,
  sns_ddf_time_t ts,
  smgr_sensor_depot_iter_s* iter_ptr
)
{
  const smgr_sample_s* sample_ptr = NULL;
  if ( (depot_ptr != NULL) && (iter_ptr != NULL) && (depot_ptr->used_bufs > 0) )
  {
    SNS_SMGR_DEPOT_DBG4(
      HIGH, "depot_iter_ext - desired_ts=%u avail=%d 1st=%d last=%d", 
      ts, depot_ptr->used_bufs, depot_ptr->first_idx, depot_ptr->last_idx);

    iter_ptr->depot_ptr = depot_ptr;
    if ( depot_ptr->first_idx <= depot_ptr->last_idx ) /* not wrapped around */
    {
      sample_ptr = sns_smgr_depot_bsearch(iter_ptr, ts, depot_ptr->first_idx, 
                                          depot_ptr->last_idx);
    }
    else if ( NULL == (sample_ptr = sns_smgr_depot_bsearch(iter_ptr, ts,
                                                           depot_ptr->first_idx, 
                                                           depot_ptr->max_bufs-1)) )
    {
      sample_ptr = sns_smgr_depot_bsearch(iter_ptr, ts, 0, depot_ptr->last_idx);
    }
    if ( sample_ptr == NULL )
    {
      iter_ptr->depot_ptr = NULL;
    }
    SNS_SMGR_DEPOT_DBG4(
       MED, "depot_iter_ext - returning %u idx=%d 1st=%u last=%u", 
       sample_ptr?sample_ptr->time_stamp:0, sample_ptr?iter_ptr->idx:-1,
       depot_ptr->samples[depot_ptr->first_idx].time_stamp, 
       depot_ptr->samples[depot_ptr->last_idx].time_stamp);
  }
  return sample_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_depot_iter

===========================================================================*/
/*!
  @brief Initializes the given iterator with the given sample depot and
         returns the first sample of the series.

  @param[in]  depot_ptr sample depot of a sensor
  @param[in]  forward   TRUE if interator should iterate from oldest to latest samples
                        FALSE from latest to oldest sample
  @param[out] iter_ptr  the iterator to be initialized

  @return An iterator for the given sample depot
*/
/*=========================================================================*/
const smgr_sample_s* sns_smgr_init_depot_iter(
  const smgr_sample_depot_s* depot_ptr,
  boolean forward,
  smgr_sensor_depot_iter_s* iter_ptr
)
{
  const smgr_sample_s* sample_ptr = NULL;
  if ( (depot_ptr != NULL) && (iter_ptr != NULL) )
  {
    iter_ptr->depot_ptr = depot_ptr;
    iter_ptr->idx = forward ? depot_ptr->first_idx : depot_ptr->last_idx;

    SNS_SMGR_DEPOT_DBG3(
       MED, "init_depot_iter - 1st=%d last=%d used=%d",
       depot_ptr->first_idx, depot_ptr->last_idx,
       depot_ptr->used_bufs);

    if ( depot_ptr->used_bufs > 0 )
    {
      sample_ptr = &depot_ptr->samples[iter_ptr->idx];
      SNS_SMGR_DEPOT_DBG3(
         MED, "init_depot_iter - forward=%d sample_ptr=0x%x timestamp=%u",
         forward, sample_ptr, sample_ptr->time_stamp);
    }
  }
  return sample_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_sample

===========================================================================*/
/*!
  @brief Returns pointer to the "next" buffered sample from the current sample
  referenced by the iterator.

  @param[i] iter_ptr - the sample depot interator
  @param[i] next     - 0=current,
                       greater than 0=forward "next" indices,
                       less than 0=backward "next" indices

  @return Read-only poinnter to next sample, or NULL if no sample available
*/
/*=========================================================================*/
const smgr_sample_s* sns_smgr_get_sample(smgr_sensor_depot_iter_s* iter_ptr, int8_t next)
{
  const smgr_sample_s* sample_ptr = NULL;
  if ( (iter_ptr != NULL) && (iter_ptr->depot_ptr != NULL) &&
       (iter_ptr->depot_ptr->used_bufs > 0) )
  {
    int8_t i = 0;
    if ( next > 0 )
    {
      while ( (iter_ptr->idx != iter_ptr->depot_ptr->last_idx) && (i < next) )
      {
        iter_ptr->idx = (iter_ptr->idx + 1) % iter_ptr->depot_ptr->max_bufs;
        i++;
      }
    }
    else if ( next < 0 )
    {
      while ( (iter_ptr->idx != iter_ptr->depot_ptr->first_idx) && (i > next) )
      {
        iter_ptr->idx = (iter_ptr->idx + iter_ptr->depot_ptr->max_bufs
                         - 1) % iter_ptr->depot_ptr->max_bufs;
        i--;
      }
    }
    if ( i == next )
    {
      sample_ptr = &iter_ptr->depot_ptr->samples[iter_ptr->idx];
      SNS_SMGR_DEPOT_DBG2(LOW, "get_sample - idx=%d ts=%u",
                          iter_ptr->idx, sample_ptr->time_stamp);
    }
  }
  return sample_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_deposit_sample

===========================================================================*/
/*!
  @brief Stores the given sample in the given depot.

  @param[i] depot_ptr - sample depot of a sensor
  @param[i] type       - primary or secondary

  @return Read-only poinnter to oldest sample, or NULL if no sample available
*/
/*=========================================================================*/
static const smgr_sample_s* sns_smgr_deposit_sample(
  smgr_sample_depot_s*         depot_ptr,
  const sns_ddf_sensor_data_s* src_ptr)
{
  depot_ptr->last_idx  = (depot_ptr->last_idx + 1) % depot_ptr->max_bufs;
  depot_ptr->used_bufs = MIN(depot_ptr->used_bufs+1, depot_ptr->max_bufs);
  if ( depot_ptr->used_bufs == depot_ptr->max_bufs )
  {
    depot_ptr->first_idx = (depot_ptr->last_idx + depot_ptr->max_bufs -
                            depot_ptr->used_bufs + 1) % depot_ptr->max_bufs;
  }

  depot_ptr->samples[depot_ptr->last_idx].time_stamp = src_ptr->timestamp;
  depot_ptr->samples[depot_ptr->last_idx].status     = src_ptr->status;
  if (src_ptr->samples != NULL)
  {
    uint8_t i;
    for ( i=0; i<depot_ptr->num_axes; i++ )
    {
      depot_ptr->samples[depot_ptr->last_idx].data[i] = src_ptr->samples[i].sample;
    }
  }
  SNS_SMGR_DEPOT_DBG3(
     LOW, "deposit_sample - 1st=%d last=%d ts=%u",
     depot_ptr->first_idx, depot_ptr->last_idx, 
     depot_ptr->samples[depot_ptr->last_idx].time_stamp);
  return &depot_ptr->samples[depot_ptr->last_idx];
}

/*===========================================================================

  FUNCTION:   sns_smgr_match_ddf_sensor

===========================================================================*/
/*!
  @brief Given the enumerated type that device driver framework calls "sensor"
         finds the corresponding ddf_sensor structure.
  @detail
  @param[i] sensor_ptr
  @param[i] ddf_sensor_type

  @return
   Pointer the the corresponding DDF sensor, or NULL of not found
 */
/*=========================================================================*/
smgr_ddf_sensor_s* sns_smgr_match_ddf_sensor(
  const smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     ddf_sensor_type)
{
  smgr_ddf_sensor_s* ddf_sensor_ptr = NULL;
  uint8_t i;
  for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    //SNS_SMGR_PRINTF3(HIGH, "sns_smgr_match_ddf_sensor- smgr_handle=%u, idx=%u, ddf_sensor=%u", sensor_ptr, i, sensor_ptr->ddf_sensor_ptr[i]);
    if(sensor_ptr->ddf_sensor_ptr[i] == NULL)
    {
      SNS_SMGR_PRINTF0(LOW, "sns_smgr_match_ddf_sensor- Received NUll ddf pointer.");
    }
    //SNS_SMGR_PRINTF2(LOW, "sns_smgr_match_ddf_sensor- ddf_sensor_type=%d, match_type=%d",ddf_sensor_type, SMGR_SENSOR_TYPE(sensor_ptr,i));
    if ( (sensor_ptr->ddf_sensor_ptr[i] != NULL) &&
         (SMGR_SENSOR_TYPE(sensor_ptr, i) == ddf_sensor_type) )
    {
      ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
      break;
    }
  }
  return ddf_sensor_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_compute_effective_rate

===========================================================================*/
/*!
  @brief Computes the effective sampling rate of the given sensor/data type.

  @detail

  @param[i]  item_ptr - the report item representing a sensor/data type
  @return
    The effective sampling rate
 */
/*=========================================================================*/
static q16_t sns_smgr_compute_effective_rate(const smgr_rpt_item_s* item_ptr)
{
  const smgr_ddf_sensor_s* ddf_sensor_ptr = item_ptr->ddf_sensor_ptr;
  q16_t effective_rate;
  if ( item_ptr->cic_obj.factor > 1 )
  {
    effective_rate = FX_DIV_Q16(FX_FLTTOFIX_Q16(item_ptr->cic_obj.input_frequency),
                                FX_FLTTOFIX_Q16(item_ptr->cic_obj.factor));
  }
  else if ( SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B) &&
            SMGR_LOW_RANGE_TEST (ddf_sensor_ptr->depot_data_rate, 
                              item_ptr->sampling_rate_hz*item_ptr->sampling_factor,
                                                 SMGR_INTERPLTN_RANGE_LOW_PCT ) &&
            SMGR_HIGH_RANGE_TEST (ddf_sensor_ptr->depot_data_rate, 
                              item_ptr->sampling_rate_hz*item_ptr->sampling_factor,
                                                 SMGR_INTERPLTN_RANGE_HIGH_PCT ))
  {
    /* no interpolation */
    effective_rate = FX_DIV_Q16(FX_FLTTOFIX_Q16(ddf_sensor_ptr->depot_data_rate),
                                  FX_FLTTOFIX_Q16(item_ptr->sampling_factor));
   
  }
  else
  {
    effective_rate = FX_FLTTOFIX_Q16(item_ptr->sampling_rate_hz);
  }
  return effective_rate;
}

/*===========================================================================

  FUNCTION:   sns_smgr_set_sensor_state

===========================================================================*/
/*!
  @brief Changes the state of the given sensor

  @Detail

  @param[i] sensor_ptr: the sensor whose state should be changed
  @param[i] state: the state to which to change

  @return
   NONE
*/
/*=========================================================================*/
void sns_smgr_set_sensor_state(smgr_sensor_s* sensor_ptr,
                               smgr_sensor_state_e state)
{
  if ( state != sensor_ptr->sensor_state )
  {
    SNS_SMGR_PRINTF3(HIGH, "set_sensor_state - sensor=%d was=%d now=%d",
                     SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, state);
    sensor_ptr->sensor_state = state;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_find_sensor

===========================================================================*/
/*!
  @brief Finds the associated sensor structure given a sensor ID.

  @detail
  @param[i] sensor_id - the sensor ID passed in via Sensor1 API
  @return
   sensor structure, or NULL if not found
 */
/*=========================================================================*/
smgr_sensor_s* sns_smgr_find_sensor(smgr_sensor_id_e sensor_id)
{
  smgr_sensor_s* sensor_ptr = NULL;
  uint8_t i;
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor) && sensor_ptr==NULL; i++ )
  {
    if ( SMGR_SENSOR_ID(&sns_smgr.sensor[i]) == sensor_id )
    {
      sensor_ptr = &sns_smgr.sensor[i];
    }
  }
  return sensor_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_is_event_sensor

===========================================================================*/
/*!
  @brief Checks whether given sensor is an event sensor.

  @Detail

  @param[i] sensor_ptr - the sensor leader structure
  @param[i] data_type  - primary or secondary

  @return
    TRUE if given sensor type is an event sensor
    FALSE to indicate streaming sensor
*/
/*=========================================================================*/
boolean sns_smgr_is_event_sensor(smgr_sensor_s* sensor_ptr, uint8_t data_type)
{
  boolean is_event_sensor = FALSE;
  if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
  {
    sns_ddf_sensor_e ddf_sensor_type = SMGR_SENSOR_TYPE(sensor_ptr, data_type);
    if ( (ddf_sensor_type == SNS_DDF_SENSOR_PROXIMITY) ||
         (ddf_sensor_type == SNS_DDF_SENSOR_AMBIENT) ||
         (ddf_sensor_type == SNS_DDF_SENSOR_HUMIDITY) ||
         (ddf_sensor_type == SNS_DDF_SENSOR_RGB) ||
         (ddf_sensor_type == SNS_DDF_SENSOR_CT_C) ||
         (ddf_sensor_type == SNS_DDF_SENSOR_DOUBLETAP)||
         (ddf_sensor_type == SNS_DDF_SENSOR_IR_GESTURE) ||
         (ddf_sensor_type == SNS_DDF_SENSOR_DOUBLETAP) ||
         //step event
         (ddf_sensor_type == SNS_DDF_SENSOR_STEP_EVENT ) ||
         //step count
         (ddf_sensor_type == SNS_DDF_SENSOR_STEP_COUNT ) ||
         //SMD
         (ddf_sensor_type == SNS_DDF_SENSOR_SMD ) ||
         (ddf_sensor_type == SNS_DDF_SENSOR_SAR)  ||
         (ddf_sensor_type == SNS_DDF_SENSOR_HALL_EFFECT)  ||
         (ddf_sensor_type == SNS_DDF_SENSOR_AMBIENT_TEMP ))
    {
      is_event_sensor = TRUE;
    }
  }
  return is_event_sensor;
}

/*===========================================================================

  FUNCTION:   smgr_sensor_type_read_state_to_idle

===========================================================================*/
/*!
  @brief Sets the read state for all types to idle

  @Detail

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
static void smgr_sensor_type_read_state_to_idle(smgr_sensor_s* sensor_ptr)
{
  uint32_t i;
  for ( i = 0; i < ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
    {
      sensor_ptr->ddf_sensor_ptr[i]->sensor_type_state = SENSORTYPE_STATE_IDLE;
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_get_max_requested_freq

===========================================================================*/
/*!
  @brief Returns the max requested frequency of all sensor types

  @param[i] sensor_ptr - the sensor leader structure

  @return The highest requested frequency
*/
/*=========================================================================*/
uint32_t smgr_get_max_requested_freq(smgr_sensor_s* sensor_ptr)
{
  uint8_t i;
  uint16_t max_freq = 0;
  for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
    {
      max_freq = MAX(max_freq,
                     sensor_ptr->ddf_sensor_ptr[i]->max_requested_freq_hz);
    }
  }
  return max_freq;
}

/*===========================================================================

  FUNCTION:   smgr_sensor_type_max_odr

===========================================================================*/
/*!
  @brief Returns the max ODR value of all sensor types

  @Detail

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
uint32_t smgr_sensor_type_max_odr( const smgr_sensor_s* sensor_ptr)
{
  uint8_t i;
  uint16_t max_odr = 0;
  for ( i = 0; i < ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
    {
      max_odr = MAX(max_odr, sensor_ptr->ddf_sensor_ptr[i]->current_odr);
    }
  }
  return max_odr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_max_req_freq

===========================================================================*/
/*!
  @brief Returns the max requested frequency of all sensors

  @param None

  @return The highest requested frequency
*/
/*=========================================================================*/
uint32_t sns_smgr_get_max_req_freq(void)
{
  uint16_t max_freq = 0;
  uint8_t i;

  for ( i=0; (i < ARR_SIZE(sns_smgr.sensor)); i++ )
  {
    max_freq = MAX(max_freq, smgr_get_max_requested_freq(&sns_smgr.sensor[i]));
  }
  return max_freq;
}

/*===========================================================================

  FUNCTION:   smgr_fake_sample_ready

===========================================================================*/
static void smgr_fake_sample_ready(smgr_rpt_item_s* item_ptr)
{
  sns_ddf_time_t next_sched_sampling =
    smgr_get_scheduled_sampling_time(item_ptr->ddf_sensor_ptr);
  int32_t delta_time =
    (int32_t)next_sched_sampling - (int32_t)item_ptr->ts_last_sent -
    (int32_t)item_ptr->sampling_interval;
  if ( (delta_time << 1) > (int32_t)item_ptr->sampling_interval )
  {
    if ( item_ptr->parent_report_ptr->header_abstract.msg_id ==
        SNS_SMGR_REPORT_REQ_V01 )
    {
      item_ptr->state                       = SENSOR_ITEM_STATE_DONE;
      item_ptr->resulting_sample.status     = SNS_DDF_EINVALID_DATA;
      item_ptr->resulting_sample.time_stamp =
        item_ptr->ts_last_sent + item_ptr->sampling_interval;
      SNS_OS_MEMZERO(item_ptr->resulting_sample.data,
                     sizeof(item_ptr->resulting_sample.data));
    }
    /* todo: flush BUFFERING report */

    if ( item_ptr->state == SENSOR_ITEM_STATE_DONE )
    {
      SNS_SMGR_PRINTF1(
        HIGH, "fake_ready_sample - ts=%u", item_ptr->resulting_sample.time_stamp);
      item_ptr->parent_report_ptr->state = SMGR_RPT_STATE_FLUSHING;
      smgr_flush_reports();
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_conclude_cic

===========================================================================*/
static void smgr_conclude_cic(smgr_rpt_item_s* item_ptr)
{
  SNS_SMGR_PRINTF3(
    LOW, "conclude_cic - b4(factor=%d freq=%d count=%d)",
    item_ptr->cic_obj.factor, item_ptr->cic_obj.input_frequency,
    item_ptr->cic_obj.count);

  if ( (item_ptr->cic_obj.count << 1) >= item_ptr->cic_obj.factor )
  {
    smgr_fake_sample_ready(item_ptr);
    sns_smgr_cic_reset(&item_ptr->cic_obj);
  }
}

/*===========================================================================

  FUNCTION:   smgr_compute_cic_factor

===========================================================================*/
static void smgr_compute_cic_factor(smgr_ddf_sensor_s* ddf_sensor_ptr,
                                    smgr_rpt_item_s*   item_ptr)
{
  uint16_t cic_factor;
  uint32_t cic_frequency;

  if ( SMGR_BIT_CLEAR_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_DECIM_FILTER_B) )
  {
    cic_factor = 1;
    cic_frequency = item_ptr->sampling_rate_hz;
  }
  else
  {
    cic_factor = item_ptr->sampling_factor;
    
    if ( SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B) &&
         SMGR_LOW_RANGE_TEST(ddf_sensor_ptr->depot_data_rate, 
              item_ptr->sampling_rate_hz*cic_factor,SMGR_INTERPLTN_RANGE_LOW_PCT ) &&
          SMGR_HIGH_RANGE_TEST(ddf_sensor_ptr->depot_data_rate, 
              item_ptr->sampling_rate_hz*cic_factor,SMGR_INTERPLTN_RANGE_HIGH_PCT ))
    {
      cic_frequency = ddf_sensor_ptr->depot_data_rate;
    }
    else
    {
      cic_frequency = item_ptr->sampling_rate_hz * cic_factor;
    }
  }

  if ( (cic_factor != item_ptr->cic_obj.factor) ||
       (cic_frequency != item_ptr->cic_obj.input_frequency) )
  {
    smgr_conclude_cic(item_ptr);
    sns_smgr_cic_init(&item_ptr->cic_obj, cic_factor, cic_frequency,
                      ddf_sensor_ptr->depot_ptr->num_axes);
  }
  SNS_SMGR_PRINTF3(
    LOW, "comp_cic_factor - factor=%d freq=%d count=%d",
    cic_factor, cic_frequency, item_ptr->cic_obj.count);
}

/*===========================================================================

  FUNCTION:   smgr_calculate_sample_factor

===========================================================================*/
uint8_t smgr_calculate_sample_factor( smgr_ddf_sensor_s* ddf_sensor_ptr, uint16_t sampling_rate_hz)
{
  uint8 sample_factor1, sample_factor2, sample_factor;
  uint16_t depot_data_rate_hz = ddf_sensor_ptr->depot_data_rate;
  if ( sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type) ||
        ( depot_data_rate_hz <= sampling_rate_hz ) )
  {
    sample_factor = 1;
  }
  else
  {
    sample_factor1 = depot_data_rate_hz/sampling_rate_hz;
    if ( depot_data_rate_hz == sample_factor1*sampling_rate_hz )
    {
      sample_factor = sample_factor1;
    }
    else
    {
      sample_factor2 = sample_factor1+1;
      /* choose sample factor so depot rate divide by  sample rate is closer to sample rate
        if ( (dr/sf1 - sr) <= (sr - dr/sf2) )
           choose sf1 
        else
           choose sf2 
       */
      if ( ( depot_data_rate_hz - sampling_rate_hz*sample_factor1 )*sample_factor2 <=
             (sampling_rate_hz*sample_factor2 -depot_data_rate_hz)*sample_factor1 )
      {
        sample_factor = sample_factor1;
      }
      else
      {
        sample_factor = sample_factor2;
      }
    }
  }
  return sample_factor;
}

/*===========================================================================

  FUNCTION:   smgr_interpolation_required

===========================================================================*/
static boolean smgr_interpolation_required(smgr_ddf_sensor_s* ddf_sensor_ptr,
                                           smgr_rpt_item_s*   item_ptr)
{
  boolean interpolation_required = FALSE;
  uint8_t sampling_factor = item_ptr->sampling_factor;
  
  if ( !sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr,
                                 ddf_sensor_ptr->data_type) )
  {
    if ( SMGR_BIT_TEST(item_ptr->flags,SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B) &&
            SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr))
    {
      if ( SMGR_LOW_RANGE_TEST(ddf_sensor_ptr->depot_data_rate, 
                  item_ptr->sampling_rate_hz*sampling_factor,SMGR_INTERPLTN_RANGE_LOW_PCT) &&
           SMGR_HIGH_RANGE_TEST(ddf_sensor_ptr->depot_data_rate, 
                  item_ptr->sampling_rate_hz*sampling_factor,SMGR_INTERPLTN_RANGE_HIGH_PCT) )
      {
        interpolation_required = FALSE; 
      }
      else
      {
        interpolation_required = TRUE;
      }
    }
    else
    {
      if ( ddf_sensor_ptr->depot_data_rate != item_ptr->sampling_rate_hz)
      {
        interpolation_required = TRUE;  
      }
    }
  }
  return interpolation_required;
}

/*===========================================================================

  FUNCTION:   smgr_conclude_interpolator

===========================================================================*/
static void smgr_conclude_interpolator(smgr_rpt_item_s* item_ptr)
{
  smgr_interpolator_obj_s* iobj_ptr = &item_ptr->interpolator_obj;

  if ( iobj_ptr->interval_ticks != 0 )
  {
    SNS_SMGR_PRINTF3(
      LOW, "conclude_interp - ts1=%u desired=%u ts2=%u",
      iobj_ptr->sample1_ptr ? iobj_ptr->sample1_ptr->time_stamp : 0,
      iobj_ptr->desired_timestamp,
      iobj_ptr->sample2_ptr ? iobj_ptr->sample2_ptr->time_stamp : 0);

    if ( (iobj_ptr->sample1_ptr != NULL) &&
         (iobj_ptr->sample2_ptr != NULL) &&
         TICK1_GTR_TICK2(sns_smgr.last_tick.u.low_tick +
                         SMGR_SCHEDULE_GRP_TOGETHER_TICK,
                         iobj_ptr->desired_timestamp) )
    {
      smgr_fake_sample_ready(item_ptr);
    }
  }
  SNS_OS_MEMZERO(iobj_ptr, sizeof(smgr_interpolator_obj_s));
}

/*===========================================================================

  FUNCTION:   smgr_determine_sample_quality

===========================================================================*/
static void smgr_determine_sample_quality(smgr_ddf_sensor_s* ddf_sensor_ptr,
                                          smgr_rpt_item_s*   item_ptr)
{
  /* determines sample quality to be used in indications */
  if ( smgr_interpolation_required(ddf_sensor_ptr, item_ptr) != FALSE )
  {
    if ( item_ptr->cic_obj.factor > 1 )
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED_V01;
      item_ptr->interpolator_obj.interval_ticks =
        smgr_get_sched_intval(item_ptr->cic_obj.input_frequency);
    }
    else
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01;
      item_ptr->interpolator_obj.interval_ticks = item_ptr->sampling_interval;
    }
  }
  else
  {
    smgr_conclude_interpolator(item_ptr);
    if ( item_ptr->cic_obj.factor > 1 )
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_FILTERED_V01;
    }
    else
    {
      item_ptr->quality = SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01;
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_init_interp_time

===========================================================================*/
static void smgr_init_interp_time(smgr_rpt_item_s* item_ptr)
{
  if ( ((item_ptr->quality == SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01) ||
        (item_ptr->quality == SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED_V01)) &&
       (item_ptr->interpolator_obj.interval_ticks != 0) )
  {
    const smgr_sample_s* sample_ptr;
    smgr_interpolator_obj_s* iobj_ptr = &item_ptr->interpolator_obj;
    iobj_ptr->desired_timestamp =
      (item_ptr->ts_last_sent + iobj_ptr->interval_ticks)/
      iobj_ptr->interval_ticks * iobj_ptr->interval_ticks;

    while ( TICK1_GTR_TICK2(iobj_ptr->interval_ticks >> 1,
                            iobj_ptr->desired_timestamp - item_ptr->ts_last_sent) )
    {
      iobj_ptr->desired_timestamp += iobj_ptr->interval_ticks;
    }

    if ( SMGR_BIT_CLEAR_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B) )
    {
      sample_ptr = sns_smgr_get_latest_sample(item_ptr->ddf_sensor_ptr->depot_ptr);
      if ( (sample_ptr != NULL) && (sample_ptr->status == SNS_DDF_SUCCESS) )
      {
        sns_smgr_interpolator_update(iobj_ptr, sample_ptr);
      }
    }
    SNS_SMGR_PRINTF3(
      MED, "init_interp - ts1=%u desired=%u ts2=%u",
      iobj_ptr->sample1_ptr ? iobj_ptr->sample1_ptr->time_stamp : 0,
      iobj_ptr->desired_timestamp,
      iobj_ptr->sample2_ptr ? iobj_ptr->sample2_ptr->time_stamp : 0);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_items_info

===========================================================================*/
/*!
  @brief After DDF sensor is configured with new ODR its associated report
         items must be updated.

  @param[i] ddf_sensor_ptr - the DDF sensor

  @return
*/
/*=========================================================================*/
void sns_smgr_update_items_info(smgr_ddf_sensor_s* ddf_sensor_ptr,
                                boolean            new_item_only)
{
  smgr_rpt_item_s*  item_ptr;
  smgr_rpt_item_s** item_ptr_ptr;

  SNS_SMGR_PRINTF3(
    MED, "update_items_info - ddf_sensor=%d odr=%d depot_data_rate=%d",
    SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
    ddf_sensor_ptr->current_odr, ddf_sensor_ptr->depot_data_rate);

  SMGR_FOR_EACH_ASSOC_ITEM( ddf_sensor_ptr, item_ptr, item_ptr_ptr )
  {
    if ( (new_item_only != FALSE) && (item_ptr->state != SENSOR_ITEM_STATE_LINKING) )
    {
      continue;
    }
    if ( item_ptr->ts_last_sent == 0 )
    {
#ifndef SNS_QDSP_SIM
      item_ptr->ts_last_sent = sns_smgr.last_tick.u.low_tick;
#else
      /* On SIM set last sent ts to a value less than the next sample to be provided */
      item_ptr->ts_last_sent = playback_next_sample_ts - QDSP_PLAYBACK_SAMPLE_LAST_TICK_DELTA;
#endif
    }

    if ( sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr,
                                    ddf_sensor_ptr->data_type) )
    {
      item_ptr->sampling_factor = 1;
    }
    else
    {
      item_ptr->sampling_factor=smgr_calculate_sample_factor(ddf_sensor_ptr,
                                                         item_ptr->sampling_rate_hz);
    }
    smgr_compute_cic_factor(ddf_sensor_ptr, item_ptr);
    smgr_determine_sample_quality(ddf_sensor_ptr, item_ptr);
    smgr_init_interp_time(item_ptr);

    item_ptr->effective_rate_hz = sns_smgr_compute_effective_rate(item_ptr);
    item_ptr->sampling_count    = 0;
    if ( item_ptr->state < SENSOR_ITEM_STATE_IDLE )
    {
      const smgr_sample_s* sample_ptr;
      if ( sns_smgr_is_event_sensor(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                    item_ptr->ddf_sensor_ptr->data_type) &&
           ((sample_ptr = sns_smgr_get_latest_sample(item_ptr->ddf_sensor_ptr->
                                                     depot_ptr)) != NULL) )
      {
        item_ptr->state         = SENSOR_ITEM_STATE_DONE;
        item_ptr->ts_last_sent  = sample_ptr->time_stamp - 1;

        if ( item_ptr->parent_report_ptr->num_items == 1 )
        {
          /* this is the only requested sensor in the report; send indication now */
          sns_smgr_ready_reports_list_add(item_ptr->parent_report_ptr);
          SNS_SMGR_PRINTF0(HIGH, "items_info - report added to ready queue");
        } /* else, indication sent when other requested sensors provide samples */
      }
      else
      {
        item_ptr->state         = SENSOR_ITEM_STATE_IDLE;
      }
    }

    SNS_SMGR_PRINTF3(MED, "items_info - rpt_id=%d ind_cnt=%d quality=%d",
                     item_ptr->parent_report_ptr->rpt_id,
                     item_ptr->parent_report_ptr->ind_count, item_ptr->quality);
    SNS_SMGR_PRINTF3(LOW, "items_info - rate=0x%x samp_intvl=%d samp_factor=%d",
                     item_ptr->effective_rate_hz, item_ptr->sampling_interval,
                     item_ptr->sampling_factor);
    SNS_SMGR_PRINTF3(LOW, "items_info - interp_intvl=%d desired=%u last=%u",
                     item_ptr->interpolator_obj.interval_ticks,
                     item_ptr->interpolator_obj.desired_timestamp,
                     item_ptr->ts_last_sent);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_compute_device_sampling_factor

===========================================================================*/
/*!
  @details For sensors in DRI mode, if no High Performance clients request CIC
  filtering, computes the factor between ODR and max requested rate.
  The samples deposited into buffer will have the sampling rate that is
  ODR/device_sampling_factor.

  @param[i] ddf_sensor_ptr  ddf sensor

  @return
*/
/*=========================================================================*/
void sns_smgr_compute_device_sampling_factor(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  if ( sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type) )
  {
    ddf_sensor_ptr->device_sampling_factor = 1;
  }
  else if ( SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) )
  {
    uint32_t max_rate = ddf_sensor_ptr->max_requested_freq_hz;
    smgr_rpt_item_s*  item_ptr;
    smgr_rpt_item_s** item_ptr_ptr;

    ddf_sensor_ptr->device_sampling_factor = 1;
    SMGR_FOR_EACH_ASSOC_ITEM( ddf_sensor_ptr, item_ptr, item_ptr_ptr )
    {
      if ( SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_DECIM_FILTER_B) ||
           SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B) )
      {
        max_rate = ddf_sensor_ptr->current_odr;
        break;
      }
    }
    max_rate = MAX(1, max_rate);
    ddf_sensor_ptr->device_sampling_factor =
      MAX(1, ddf_sensor_ptr->current_odr/max_rate);
  }
  SNS_SMGR_PRINTF2(
    MED, "compute_dev_sampling_factor - odr=%d factor=%d",
      ddf_sensor_ptr->current_odr, ddf_sensor_ptr->device_sampling_factor);
}

/*===========================================================================

  FUNCTION:   sns_smgr_compute_depot_data_rate

===========================================================================*/
/*!
  @details

  @param[i] ddf_sensor_ptr  ddf sensor

  @return
*/
/*=========================================================================*/
uint16_t sns_smgr_compute_depot_data_rate(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint16_t depot_data_rate;
  if ( SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) )
  {
    depot_data_rate =
      ddf_sensor_ptr->current_odr / ddf_sensor_ptr->device_sampling_factor;
    /* in DRI mode, when there is any request for acurate timestamp or
       for CIC filtering, sampling_factor would be 1, and depot_data_rate
       would equal ODR */
  }
  else
  {
    smgr_rpt_item_s*  item_ptr;
    smgr_rpt_item_s** item_ptr_ptr;

    depot_data_rate = ddf_sensor_ptr->max_requested_freq_hz;

    SMGR_FOR_EACH_ASSOC_ITEM( ddf_sensor_ptr, item_ptr, item_ptr_ptr )
    {
      if ( SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_DECIM_FILTER_B) &&
           (ddf_sensor_ptr->current_odr > ddf_sensor_ptr->max_requested_freq_hz) )
      {
        /* when there is any request for CIC filtering and the highest requested
        rate is lower than the chosen ODR, it might be necessary to poll the
        sensor multiple times higher than the requested rate;
        for instance, when a sensor's supported ODR for a 2Hz request is 5Hz,
        depot_data_rate (i.e. polling rate) must be 4Hz if CIC filtering is
        requested */
        depot_data_rate =
          ddf_sensor_ptr->max_requested_freq_hz *
          (ddf_sensor_ptr->current_odr/ddf_sensor_ptr->max_requested_freq_hz);
        break;
      }
    }
  }
  SNS_SMGR_PRINTF3(
    MED, "compute_depot_rate - ddf_sensor=%d odr=%d depot_rate=%d",
    SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
    ddf_sensor_ptr->current_odr, depot_data_rate);
  return depot_data_rate;
}

/*===========================================================================

  FUNCTION:   sns_smgr_config_odr

===========================================================================*/
/*!
  @brief configure ODR(Output Data Rate) for all sensor types of the sensor

  @Detail

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
static void sns_smgr_config_odr(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_ddf_status_e  ddf_status;
  smgr_sensor_s*    sensor_ptr = ddf_sensor_ptr->sensor_ptr;
  uint32_t          odr = 0;

  SNS_ASSERT(SMGR_BIT_TEST(sensor_ptr->flags,
                           SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B));

  if ( ddf_sensor_ptr->max_requested_freq_hz > 0 )
  {
    odr = sns_smgr_choose_odr(sensor_ptr);
  }
  SNS_SMGR_PRINTF3(MED, "config_odr - ddf_sensor=%d curr=%d new=%d",
                   SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
                   ddf_sensor_ptr->current_odr, odr);
  if ( odr != ddf_sensor_ptr->current_odr )
  {
    if ( SMGR_SENSOR_FIFO_IS_SUPPORTED(sensor_ptr) &&
         SMGR_SENSOR_FIFO_IS_ENABLE(sensor_ptr) )
    {
      sns_ddf_sensor_e ddf_sensor_type =
          SMGR_SENSOR_TYPE(sensor_ptr,SNS_SMGR_DATA_TYPE_PRIMARY_V01);
      bool intr_enabled = sensor_ptr->is_self_sched_intr_enabled;
      if ( intr_enabled )
      { //disable intr to flush fifo
        sns_smgr_enable_sched_data( sensor_ptr, ddf_sensor_type, false );
      }
      sns_smgr_fifo_flush(sensor_ptr);
      if ( intr_enabled )
      { //reenable prior intr status
        sns_smgr_enable_sched_data( sensor_ptr, ddf_sensor_type, true );
      }
    }
    ddf_status =
      sns_smgr_set_attr(sensor_ptr,
                        SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
                        SNS_DDF_ATTRIB_ODR,
                        &odr);
    if  (SNS_DDF_SUCCESS != ddf_status )
    {
      SNS_SMGR_PRINTF3(ERROR, "config_odr error - sensor=%d type=%d status=%d",
                       SMGR_SENSOR_ID(sensor_ptr), ddf_sensor_ptr->data_type, ddf_status);
    }

    if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
    {
      if ( odr > 0 )
      {
        /* trigger self schedule */
        sns_smgr_enable_sched_data(
           sensor_ptr, SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type), TRUE);
      }
      else if ( 0 != ddf_sensor_ptr->current_odr )
      {
        /* stops streaming */
        ddf_sensor_ptr->current_odr = 0;

        sns_smgr_enable_sched_data(
           sensor_ptr, SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type), FALSE);
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_schedule_sensor_event

===========================================================================*/
/*!
  @brief Schedules the next sensor event processing

  @param[i] tick_offset - wait time (in ticks)

  @return none
*/
/*=========================================================================*/
void sns_smgr_schedule_sensor_event(uint32_t tick_offset)
{
  sns_err_code_e status = ADSP_EFAILED;
  uint8_t i;

  tick_offset = MAX(tick_offset, SMGR_TOO_SHORT_TICKS_FOR_TIMER);
  SNS_SMGR_PRINTF1(LOW, "sched_sensor_ev - offset=%d", tick_offset);
  sns_em_cancel_timer(sns_smgr.sensor_ev_tmr_obj);
  for ( i=0; i<100 && status!=ADSP_EOK; i++ )
  {
    status = sns_em_register_timer(sns_smgr.sensor_ev_tmr_obj, tick_offset);
  }
  if ( status != ADSP_EOK )
  {
    SNS_SMGR_PRINTF0(ERROR, "sched_sensor_ev - register_timer failed");
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_insert_configuring_event

===========================================================================*/
/*!
  @brief insert configuring event

  @Detail

  @param[i] sensor_ptr - the sensor leader structure
            lpf_odr    low pass filter ODR

  @return
    NONE
*/
/*=========================================================================*/
static void sns_smgr_schedule_next_event(smgr_sensor_s* sensor_ptr)
{
  uint32_t lpf_dur = 0;
  if ( sensor_ptr->num_lpf > 0 )
  {
    lpf_dur = SMGR_LPF_DURATION(smgr_sensor_type_max_odr(sensor_ptr));
  }
  if ( SENSOR_STATE_IDLE == sensor_ptr->sensor_state )
  {
    lpf_dur = MAX(lpf_dur, sensor_ptr->const_ptr->idle_to_ready_time);
  }
  sensor_ptr->event_done_tick = sensor_ptr->odr_change_tick + lpf_dur;
  SNS_SMGR_PRINTF3(MED, "sched_next_ev - sensor=%d state=%d lpf_dur=%d",
                   SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, lpf_dur);
  SNS_SMGR_PRINTF3(MED, "sched_next_ev - odr_tick=%u done_tick=%u time_now=%u",
                   sensor_ptr->odr_change_tick, sensor_ptr->event_done_tick,
                   sns_smgr.last_tick.u.low_tick);
  if ( TICK1_GTR_TICK2(sensor_ptr->event_done_tick, sns_smgr.last_tick.u.low_tick) )
  {
    sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_CONFIGURING);
  }
  else
  {
    sensor_ptr->event_done_tick = 0;
  }
}

/*===========================================================================

  FUNCTION:   smgr_find_slow_or_same_scheduler

===========================================================================*/
/*!
  @brief Find the scheduler that has slow freq(i.e. bigger interval) or the same interval.

  @param[i] interval - The timetick of the scheduling period

  @return
   A pointer to the scheduler block which is slower or the same frequency,
   or returns NULL when there is nothing
*/
/*=========================================================================*/
static smgr_sched_block_s *smgr_find_slow_or_same_scheduler (uint32_t interval)
{
  smgr_sched_block_s* found_ptr = NULL;
  smgr_sched_block_s* sched_blk_ptr;
  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.sched_que, sched_blk_ptr, sched_link)
  {
    if ( sched_blk_ptr->sched_intvl >= interval )
    {
      found_ptr = sched_blk_ptr;
      break;
    }
  }
  return found_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_get_tick64

===========================================================================*/
/*!
  @brief returns 64 bits current tick.

  @detail also the tick is saved into sns_smgr.last_tick so that SMGR can maintains the high_tick

  @param[i] NONE

  @return
    NONE

*/
/*=========================================================================*/
uint64_t sns_smgr_get_tick64(void)
{
  uint32_t tick32 = sns_em_get_timestamp();
  if ( tick32 < sns_smgr.last_tick.u.low_tick )
  {
    sns_smgr.last_tick.u.high_tick++;
  }
  sns_smgr.last_tick.u.low_tick = tick32;
  return sns_smgr.last_tick.tick64;
}

/*===========================================================================

  FUNCTION:   sns_smgr_register_into_scheduler

===========================================================================*/
/*!
  @brief Insert given sensor type into a schedule block.

  @param[i] ddf_sensor_ptr - sensor type representing a sensor/data type pair

  @return
   TRUE Successfully registerd into a scheduler
   FALSE Failed to register into a scheduler because of:
   - Memory allocation failed for the new scheduler
   - etc
*/
/*=========================================================================*/
static boolean sns_smgr_register_into_scheduler(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t sched_intvl;
  smgr_sched_block_s* existing_sched_ptr;
  sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&ddf_sensor_ptr->sched_link);
  if ( q_ptr != NULL )
  {
    return false;
  }

  sched_intvl = smgr_get_sched_intval(ddf_sensor_ptr->depot_data_rate);
  existing_sched_ptr = smgr_find_slow_or_same_scheduler(sched_intvl);

  SNS_SMGR_PRINTF3(HIGH, "reg_into_scheduler - ddf_sensor=%d depot_rate=%d intvl=%d",
                   SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                    ddf_sensor_ptr->data_type),
                   ddf_sensor_ptr->depot_data_rate, sched_intvl);

  if ( (NULL != existing_sched_ptr) &&
       (existing_sched_ptr->sched_intvl == sched_intvl) )
  {
    sns_q_put(&existing_sched_ptr->ddf_sensor_queue, &ddf_sensor_ptr->sched_link);
    SNS_SMGR_PRINTF2(HIGH, "reg_into_scheduler - existing - next_tick=%u q_cnt=%d",
                     existing_sched_ptr->next_tick,
                     sns_q_cnt(&existing_sched_ptr->ddf_sensor_queue));
  }
  else
  {
    smgr_sched_block_s* new_sched_ptr =
      SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(smgr_sched_block_s));

    if ( NULL != new_sched_ptr )
    {
      SNS_OS_MEMZERO(new_sched_ptr, sizeof(smgr_sched_block_s));

      sns_q_link(new_sched_ptr, &new_sched_ptr->sched_link);  /* init the link field */
      sns_q_init(&new_sched_ptr->ddf_sensor_queue);           /* init the queue */

      new_sched_ptr->sampling_rate = ddf_sensor_ptr->depot_data_rate;
      new_sched_ptr->sched_intvl   = sched_intvl;
      new_sched_ptr->next_tick     = 
        (uint32_t)((sns_smgr.last_tick.tick64 + sched_intvl)/sched_intvl * sched_intvl);
      if ( (new_sched_ptr->next_tick - sns_smgr.last_tick.u.low_tick) >
           (sched_intvl >> 2) )
      {
        new_sched_ptr->next_tick   = sns_smgr.last_tick.u.low_tick;
      }
      SNS_SMGR_PRINTF2(HIGH, "reg_into_scheduler - new - next_tick=%u now=%u",
                       new_sched_ptr->next_tick, sns_smgr.last_tick.tick64);
      sns_q_put(&new_sched_ptr->ddf_sensor_queue, &ddf_sensor_ptr->sched_link);

      if ( NULL == existing_sched_ptr )
      {
        sns_q_put(&sns_smgr.sched_que, &new_sched_ptr->sched_link);
      }
      else
      {
        /* Ascending order by sched_intvl(i.e. Descending order by freq) */
        sns_q_insert(&new_sched_ptr->sched_link, &existing_sched_ptr->sched_link);
      }
      /* schedule a plan cycle */
      sns_smgr_plan_sampling_now();
    }
    else
    {
      SNS_SMGR_PRINTF2(ERROR, "reg_into_scheduler - alloc %d bytes failed for intvl %d",
                       sizeof(smgr_sched_block_s), sched_intvl);
      return FALSE;
    }
  }
  SNS_SMGR_PRINTF1(MED, "reg_into_scheduler - num schedules=%d",
                   sns_q_cnt(&sns_smgr.sched_que));
  return TRUE;
}

/*===========================================================================

  FUNCTION:   sns_smgr_deregister_from_scheduler

===========================================================================*/
/*!
  @brief Removes given item from scheduler's queue.

  @param[i] sensor_ptr - the sensr to be removed from scheduler
  @param[i] data_type  - primary or secondary

  @return
   none
*/
/*=========================================================================*/
static void sns_smgr_deregister_from_scheduler(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&ddf_sensor_ptr->sched_link);
  if ( NULL != q_ptr )
  {
    smgr_sched_block_s* sched_ptr =
      SMGR_GET_ENTRY(q_ptr, smgr_sched_block_s, ddf_sensor_queue);

    SNS_SMGR_PRINTF3(HIGH, "dereg_from_scheduler - ddf_sensor=%d intvl=%d next=%u",
                     SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                      ddf_sensor_ptr->data_type),
                     sched_ptr->sched_intvl, sched_ptr->next_tick);

    sns_q_delete(&ddf_sensor_ptr->sched_link); /* removed from schedule block */
    if ( sns_q_cnt(q_ptr) == 0 ) /* last sensor removed from queue? */
    {
      sns_q_delete(&sched_ptr->sched_link); /* removed from schedule queue */
      SNS_OS_FREE(sched_ptr);
    }
  }
}

/*===========================================================================

  FUNCTION:   smgr_compute_report_interval

===========================================================================*/
/*!
  @brief Determines the report interval for the given report spec

  @details Called after ODR configuration is done

  @param[i] rpt_ptr - report spec representing a report request

  @return
    NONE
*/
/*=========================================================================*/
void smgr_compute_report_interval(smgr_rpt_spec_s* rpt_ptr)
{
  uint8_t i;
  if ( rpt_ptr->q16_rpt_rate != 0 )
  {
    uint32_t min_rpt_intvl_ms = FX_FLTTOFIX_Q16(1000)/rpt_ptr->q16_rpt_rate;
    SNS_SMGR_PRINTF1(
            LOW, "compute_rpt_intvl - min_rpt_intvl_ms= %u",
            min_rpt_intvl_ms);
    rpt_ptr->rpt_interval = sns_em_convert_usec_to_dspstick(min_rpt_intvl_ms*1000);

    if ( rpt_ptr->header_abstract.msg_id == SNS_SMGR_BUFFERING_REQ_V01 )
    {
      uint16_t max_odr = 1;
      for ( i=0; i<rpt_ptr->num_items; i++ )
      {
        smgr_rpt_item_s* item_ptr = rpt_ptr->item_list[i];
        smgr_ddf_sensor_s* ddf_sensor_ptr = item_ptr->ddf_sensor_ptr;
        uint32_t depot_data_rate = ddf_sensor_ptr->depot_data_rate;
        if ( SMGR_SENSOR_IS_SELF_SCHED(ddf_sensor_ptr->sensor_ptr) )
        {
          depot_data_rate = depot_data_rate * 105 / 100; /* 5% higher */
        }
        if ( (ddf_sensor_ptr->sensor_ptr->sensor_state == SENSOR_STATE_READY) ||
             (ddf_sensor_ptr->sensor_ptr->sensor_state == SENSOR_STATE_CONFIGURING) )
        {
          /* amount of time it takes for buffer to be filled up */
          uint32_t depot_fill_ms;
          uint32_t max_sample_count =
              sns_smgr_get_max_sample_count(ddf_sensor_ptr->depot_ptr);

          if ( SMGR_SENSOR_FIFO_IS_SUPPORTED(ddf_sensor_ptr->sensor_ptr) )
          {
            uint32_t fifo_wm = (uint32_t)sns_smgr_fifo_get_suitable_max_watermark(
                  ddf_sensor_ptr->sensor_ptr);
            if ( fifo_wm > 0 )
            {
              /* DD can set wm < fifo_wm, which means  max_sample_count could be bigger
                 and report interval would stay the same  when report interval is limited
                 by buffer size if report interval is not updated */
              /* maximum number of times to fill the buffer without overflow
                 is  max_sample_count/fifo_wm */
              max_sample_count = (max_sample_count/fifo_wm)*fifo_wm;
            }
            else
            {
              SNS_SMGR_PRINTF0(ERROR, "FIFO suitable_max_watermark is 0");
            }
          }

          depot_fill_ms = 1000 * max_sample_count / depot_data_rate;

          if ( item_ptr->quality != SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01 )
          {
            uint32_t sampling_intvl = 1000 / item_ptr->sampling_rate_hz;
            if ( depot_fill_ms > sampling_intvl )
            {
               depot_fill_ms -= (1000 / item_ptr->sampling_rate_hz);
            }
          }

          min_rpt_intvl_ms = MIN(min_rpt_intvl_ms, depot_fill_ms);
          max_odr = MAX(max_odr, ddf_sensor_ptr->current_odr);
          SNS_SMGR_PRINTF3(
            LOW, "compute_rpt_intvl - max_samples=%d depot_rate=%d depot_fill_ms=%d",
            max_sample_count, depot_data_rate, depot_fill_ms);
        }
      }
      rpt_ptr->min_sampling_interval = 1000 / max_odr;
    }
    rpt_ptr->rpt_interval = sns_em_convert_usec_to_dspstick(min_rpt_intvl_ms*1000);
    SNS_SMGR_PRINTF3(LOW, "compute_rpt_intvl - rpt_id=%d intvl=%d ms=%d",
                     rpt_ptr->rpt_id, rpt_ptr->rpt_interval, min_rpt_intvl_ms);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_set_lpf

===========================================================================*/
/*!
  @brief Configures the LPF of sensor device.

  @param[in] sensor_ptr: Ptr to the data structure for a specific device driver
  @param[in] bw:  bandwidth, Hz, must be a legitimate value for the device,

  @return
    SNS_SUCCESS -     LPF was set
    SNS_ERR_FAILED - invalid argument or driver reported failure
*/
/*=========================================================================*/
static sns_err_code_e sns_smgr_set_lpf(smgr_sensor_s* sensor_ptr, q16_t bw)
{
  sns_err_code_e status = SNS_SUCCESS;
  uint32_t idx;

  for ( idx=0; idx<sensor_ptr->num_lpf; idx++ )
  {
    if ( bw <= sensor_ptr->lpf_table[idx] )
    {
      sns_ddf_status_e  ddf_status =
        sns_smgr_set_attr(sensor_ptr,
                          SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                          SNS_DDF_ATTRIB_LOWPASS,
                          &idx);
      if ( SNS_DDF_SUCCESS != ddf_status )
      {
        status = SNS_ERR_FAILED;
        SNS_SMGR_PRINTF3(ERROR, "set_lpf - sensor=%d bw=%d status=%d",
                         SMGR_SENSOR_ID(sensor_ptr), bw, ddf_status);
      }
      break;
    }
  }
  if ( idx == sensor_ptr->num_lpf )
  {
    /* BW not found in table */
    SNS_SMGR_PRINTF2(ERROR, "set_lpf - sensor=%d num_lpf=%d",
                     SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->num_lpf);
    status = SNS_ERR_FAILED;
  }
  return status;
}

/*===========================================================================

  FUNCTION:   sns_smgr_config_lpf

===========================================================================*/
/*!
  @brief Selects and sets the LPF for the given sensor.

  @detail This function is called only for sensors that do not support ODR
          attribute (or are not self-scheduling?)

  @param[i] sensor_ptr - sensor structure

  @return
   lpf bandwidth
 */
/*=========================================================================*/
static void sns_smgr_config_lpf(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t odr;
  q16_t    lpf;
  SNS_ASSERT(0 != ddf_sensor_ptr->sensor_ptr->num_lpf);

  odr = sns_smgr_choose_odr(ddf_sensor_ptr->sensor_ptr);

  SNS_SMGR_PRINTF2(LOW, "config_lpf - curr_odr=%d new_odr=%d",
                   ddf_sensor_ptr->current_odr, odr);

  if ( odr != ddf_sensor_ptr->current_odr )
  {
    lpf = FX_FLTTOFIX_Q16(odr) >> 1; /* BW = odr/2 */
    if ( (odr > 0) &&
         (sns_smgr_set_lpf(ddf_sensor_ptr->sensor_ptr, lpf) == SNS_SUCCESS) )
    {
      ddf_sensor_ptr->current_odr = odr;
    }
    else
    {
      ddf_sensor_ptr->current_odr = 0;
    }
    SNS_SMGR_PRINTF3(LOW, "config_lpf - ddf_sensor=%d odr=%d max_req_freq=%d",
                     SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                      ddf_sensor_ptr->data_type),
                     ddf_sensor_ptr->current_odr,
                     ddf_sensor_ptr->max_requested_freq_hz);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_sampling_setting

===========================================================================*/
/*!
  @brief    Configures ODR or LPF of a sensor

  @param[i] ddf_sensor_ptr
  @param[i] prev_odr

  @return
    NONE
*/
/*=========================================================================*/
static void sns_smgr_update_sampling_setting(
  smgr_ddf_sensor_s*  ddf_sensor_ptr,
  uint16_t            prev_odr)
{
  smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;

  SNS_SMGR_PRINTF3(MED, "update_sampling - ddf_sensor=%d old_odr=%d new_odr=%d",
                   SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
                   prev_odr, ddf_sensor_ptr->current_odr);
  SNS_SMGR_PRINTF2(MED, "update_sampling - max_freq=%d depot_rate=%d",
                   ddf_sensor_ptr->max_requested_freq_hz,
                   ddf_sensor_ptr->depot_data_rate);

  if ( (ddf_sensor_ptr->current_odr != prev_odr) ||
       (ddf_sensor_ptr->max_requested_freq_hz != ddf_sensor_ptr->depot_data_rate) )
  {
    uint16_t depot_data_rate = ddf_sensor_ptr->depot_data_rate;
    if ( ddf_sensor_ptr->current_odr != prev_odr )
    {
      sensor_ptr->odr_change_tick = sns_smgr.last_tick.u.low_tick;
      ddf_sensor_ptr->data_poll_ts = 0;
    }
    if ( ddf_sensor_ptr->max_requested_freq_hz )
    {
      sns_smgr_compute_device_sampling_factor(ddf_sensor_ptr);
      ddf_sensor_ptr->depot_data_rate =
        sns_smgr_compute_depot_data_rate(ddf_sensor_ptr);
      if ( ddf_sensor_ptr->depot_data_rate > 0 )
      {
        ddf_sensor_ptr->depot_data_interval =
          smgr_get_sched_intval(ddf_sensor_ptr->depot_data_rate);
      }
    }
    else
    {
      ddf_sensor_ptr->depot_data_rate = 0;
    }

    sns_smgr_deregister_from_scheduler(ddf_sensor_ptr);
    sns_smgr_update_items_info(ddf_sensor_ptr, FALSE);
    if ( (depot_data_rate != ddf_sensor_ptr->depot_data_rate) &&
         !sns_smgr_is_event_sensor(sensor_ptr, ddf_sensor_ptr->data_type) )
    {
      sns_smgr_sample_depot_clear(ddf_sensor_ptr->depot_ptr, 0);
    }
  }
  else if ( ddf_sensor_ptr->current_odr != 0 )
  {
    sns_smgr_update_items_info(ddf_sensor_ptr, TRUE);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_odr

===========================================================================*/
/*!
  @brief    Retrieves sensor ODR and updates DDF sensor with the value.

  @param[i] ddf_sensor_ptr  : pointer to DDF sensor

  @return
    NONE
*/
/*=========================================================================*/
static void sns_smgr_update_odr(
  smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  uint32_t len;
  sns_ddf_odr_t* odr_ptr;
  sns_ddf_status_e  ddf_status;
  smgr_sensor_s* sensor_ptr = ddf_sensor_ptr->sensor_ptr;

  if( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B) )
  {
    ddf_status = sns_smgr_get_attr(
                      sensor_ptr,
                      SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
                      SNS_DDF_ATTRIB_ODR,
                      (void**)&odr_ptr,
                      &len);

    SNS_ASSERT(SNS_DDF_SUCCESS == ddf_status);

    ddf_sensor_ptr->current_odr = (uint16_t) (*(sns_ddf_odr_t *)odr_ptr);
    sns_ddf_memhandler_free(&sensor_ptr->memhandler);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_config_odr_lpf

===========================================================================*/
/*!
  @brief configure ODR or LPF

  @Detail set appropriate odr/LPF and reconfigure CIC configuration

  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
static void sns_smgr_config_odr_lpf(smgr_sensor_s* sensor_ptr)
{
  uint8_t i;
  uint16_t old_odr[ARR_SIZE(sensor_ptr->const_ptr->data_types)]={0};

  SNS_SMGR_PRINTF3(
    HIGH, "config_odr_lpf - sensor=%d state=%d num_lpf=%d",
    SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state, sensor_ptr->num_lpf);
  for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
  {
    smgr_ddf_sensor_s* ddf_sensor_ptr;
    if ( sensor_ptr->ddf_sensor_ptr[i] == NULL )
    {
      continue;
    }
    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
    old_odr[i] = ddf_sensor_ptr->current_odr;

    if( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B) )
    {
      sns_smgr_config_odr(ddf_sensor_ptr);
    }
    else if( 0 != sensor_ptr->num_lpf)
    {
      sns_smgr_config_lpf(ddf_sensor_ptr);
    }
    else
    {
      ddf_sensor_ptr->current_odr =
        ddf_sensor_ptr->depot_data_rate = ddf_sensor_ptr->max_requested_freq_hz;
    }
  }

  for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
  {
    smgr_ddf_sensor_s* ddf_sensor_ptr;
    if ( sensor_ptr->ddf_sensor_ptr[i] == NULL )
    {
      continue;
    }
    ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
    sns_smgr_update_odr(ddf_sensor_ptr);
    sns_smgr_update_sampling_setting(ddf_sensor_ptr, old_odr[i]);
  }
  
  /* schedule state change event if ODR is changed */
  if ( sensor_ptr->odr_change_tick == sns_smgr.last_tick.u.low_tick )
  {
    sns_smgr_schedule_next_event(sensor_ptr);
  }

  SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B);
  SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B);
}

/*===========================================================================

  FUNCTION:   sns_smgr_adjust_latency_node

===========================================================================*/
/*!
  @brief adjust latency node as odr or freq was changed

  @Detail

  @param NONE

  @return
    NONE
*/
/*=========================================================================*/
void sns_smgr_adjust_latency_node(void)
{
  uint16_t max_odr = 0;
  uint16_t latency_node_thresh = SMGR_LATENCY_NODE_THRESH_ODR;
  uint8_t i;
  uint32_t latency_in_sclk = 0;

  for ( i=0; (i < ARR_SIZE(sns_smgr.sensor)) &&
             (max_odr < latency_node_thresh); i++ )
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];

    /* latency node is only applicable to interrupt driven devices */
    if ( SMGR_SENSOR_INTERRUPT_ENABLED(sensor_ptr) )
    {
      if ( sensor_ptr->fifo_cfg.is_fifo_enabled )
      {
        max_odr = MAX(max_odr, smgr_sensor_type_max_odr(sensor_ptr)/(sensor_ptr->fifo_cfg.current_watermark));
      }
      else if(SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr))
      {
        max_odr = MAX(max_odr, smgr_sensor_type_max_odr(sensor_ptr));
      }
      else
      {
        max_odr = MAX(max_odr, smgr_get_max_requested_freq(sensor_ptr));
      }
    }
  }
  /* control the latency node */
  if(max_odr == 0)
  {
    latency_in_sclk = 0;
  }
  else if(max_odr < 101)
  {
    if(max_odr > FIRST_PC_THRESHOLD)
    {
      latency_in_sclk = 324;
    }
    else if(max_odr > SECOND_PC_THRESHOLD)
    {
      latency_in_sclk = 489;
    }
    else if(max_odr > THIRD_PC_THRESHOLD)
    {
      latency_in_sclk = 595;
    }
    else if(max_odr > FOURTH_PC_THRESHOLD)
    {
      latency_in_sclk = 15744;
    }
    else
    {
      latency_in_sclk = 30000;
    }
  }
  else
  {
    latency_in_sclk = 1;
  }

  SNS_SMGR_PRINTF2(HIGH, "SMGR : max_odr: %d, latency vote in sclks:%d ",max_odr, latency_in_sclk);
  /* Using a SYNC client. One could use an ASYNC client too, but it is important that this async client
   * is different from regAsynClient used to disable and enable power collapse.
   * For now, retaining this to be a SYNC client, will revisit to change to async after a round of
   * tests */
  sns_hw_power_npa_vote_latency(SNS_SMGR_NPA_CLIENT_SYNC, latency_in_sclk);

  SNS_SMGR_PRINTF2(HIGH, "adjust_latency - max_odr=%d threshold=%d",
                   max_odr, latency_node_thresh);
}

/*===========================================================================

  FUNCTION:   sns_smgr_reset_sensor

===========================================================================*/
/*!
  @brief Resets the given sensor.

  @Detail

  @param[i] sensor_ptr: Ptr to the data structure for a specific device driver

  @return
   NONE
*/
/*=========================================================================*/
void sns_smgr_reset_sensor(smgr_sensor_s* sensor_ptr)
{
  sns_ddf_status_e    reset_status = SNS_DDF_EFAIL;
  smgr_sensor_state_e next_state   = SENSOR_STATE_FAILED;
  boolean qup_clck_state = sns_hw_qup_clck_status();

  if ( (SMGR_DRV_FN_PTR(sensor_ptr) != NULL) && (sensor_ptr->dd_handle != NULL) &&
         (sensor_ptr->sensor_state != SENSOR_STATE_FAILED) )
  {
    SNS_SMGR_PRINTF1(HIGH, "reset_sensor - sensor=%d", SMGR_SENSOR_ID(sensor_ptr));

    sns_hw_set_qup_clk(true);
    reset_status = SMGR_DRV_FN_PTR(sensor_ptr)->reset(sensor_ptr->dd_handle);
    sns_hw_set_qup_clk(qup_clck_state);
  }

   /* reset should not return pending, SMGR will wait using
      idle to ready time at wake up event */
  if ( SNS_DDF_SUCCESS == reset_status )
  {
    uint32_t i;
    for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
    {
      if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
      {
        sensor_ptr->ddf_sensor_ptr[i]->current_odr = 0;
      }
    }
    next_state = SENSOR_STATE_IDLE;
  }
  sns_smgr_set_sensor_state(sensor_ptr, next_state);
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_powering_up_event

===========================================================================*/
/*!
  @brief Handles SENSOR_EVENT_POWERING_UP event for given sensor

  @Detail
    POWERING_UP event is only expected when sensor is in OFF state.

  @param
    None

  @return
    TRUE if event was processed
*/
/*=========================================================================*/
static boolean sns_smgr_process_powering_up_event(void)
{
  boolean processed = false;
  uint8_t index;

  SNS_SMGR_PRINTF0(HIGH, "powering_up_ev");

  sns_hw_power_rail_config(SNS_SMGR_POWER_HIGH);
  for ( index=0; index<ARR_SIZE(sns_smgr.sensor); index++ )
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[index];
    if( sensor_ptr->sensor_state == SENSOR_STATE_OFF )
    {
      sensor_ptr->event_done_tick = 
        sns_smgr.last_tick.u.low_tick + sensor_ptr->const_ptr->off_to_idle_time;
      sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_POWERING_UP);
      processed = true;
    }
  }
  return processed;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_wake_up_event

===========================================================================*/
/*!
  @brief Handles SENSOR_EVENT_WAKE_UP event for given sensor

  @Detail
    WAKE_UP event is only expected when sensor is in IDLE state.
    1. Initializes Read state for all data type
    2. Set device to Active power state
    3. Set device range
    4. Configures ODR
    5. Schedule sampling
    6. Update sensor state to CONFIGURING

  @param[i] sensor_ptr - the sensor leader structure

  @return
    TRUE if event was processed
*/
/*=========================================================================*/
static boolean sns_smgr_process_wake_up_event(smgr_sensor_s* sensor_ptr)
{
  boolean processed = FALSE;
  if ( SENSOR_STATE_IDLE == sensor_ptr->sensor_state )
  {
    sns_ddf_status_e     driver_status;
    uint32_t set_data = (uint32_t)SNS_DDF_POWERSTATE_ACTIVE;

    smgr_sensor_type_read_state_to_idle(sensor_ptr);
    driver_status = sns_smgr_set_attr(sensor_ptr,
                                      SNS_DDF_SENSOR__ALL,
                                      SNS_DDF_ATTRIB_POWER_STATE,
                                      &set_data);
    if ( SNS_DDF_SUCCESS == driver_status )
    {
      uint32_t range_set = (uint32_t)sensor_ptr->const_ptr->sensitivity_default;
      sns_smgr_set_attr(sensor_ptr,
                        SMGR_SENSOR_TYPE(sensor_ptr,
                                         sensor_ptr->const_ptr->range_sensor),
                        SNS_DDF_ATTRIB_RANGE,
                        &range_set);
      sns_smgr_config_odr_lpf(sensor_ptr);
      if ( sensor_ptr->sensor_state == SENSOR_STATE_IDLE )
      {
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_CONFIGURING);
      }
    }
    else
    {
      sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_FAILED);
    }
    processed = TRUE;
  }
  return processed;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_no_sample_event

===========================================================================*/
/*!
  @brief Handles SENSOR_EVENT_NO_SAMPLE event for given sensor

  @Detail
    NO_SAMPLE event is only expected when sensor is not in OFF state.
    1. Clears all ODR values
    2. Disable self scheduling if necessary
    3. Update sensor state to IDLE
    4. Set device to Low power state

  @param[i] sensor_ptr - the sensor leader structure

  @return
    TRUE if event was processed
*/
/*=========================================================================*/
static boolean sns_smgr_process_no_sample_event(smgr_sensor_s* sensor_ptr)
{
  uint8_t i;

  if ( SENSOR_STATE_OFF != sensor_ptr->sensor_state )
  {
    uint32_t set_data = (uint32_t)SNS_DDF_POWERSTATE_LOWPOWER;
    sns_ddf_status_e     status   = SNS_DDF_SUCCESS;
    /* If there is a sensor request immediately replaced by another, no sample event is triggered when
       the first request is deleted, but sensor state may not have changed from off, so SMGR should not
       change the state  to idle, otherwise, SMGR will not call sensor reset if it is in idle state */

    for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
    {
      smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
      if ( ddf_sensor_ptr != NULL )
      {
        sns_smgr_deregister_from_scheduler(ddf_sensor_ptr);
        ddf_sensor_ptr->current_odr = 0;
        ddf_sensor_ptr->max_high_perf_freq_hz = 0;
        ddf_sensor_ptr->max_requested_freq_hz = 0;
        ddf_sensor_ptr->dri_count = 0;
        ddf_sensor_ptr->device_sampling_factor = 1;
        ddf_sensor_ptr->depot_data_rate = 0;
        ddf_sensor_ptr->data_poll_ts = 0;
        ddf_sensor_ptr->sensor_type_state = SENSORTYPE_STATE_IDLE;
        sns_smgr_sample_depot_clear(ddf_sensor_ptr->depot_ptr, 0);
        if( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B) )
        {
          (void) sns_smgr_set_attr(
                   sensor_ptr,
                   SMGR_SENSOR_TYPE(sensor_ptr, ddf_sensor_ptr->data_type),
                   SNS_DDF_ATTRIB_ODR, &ddf_sensor_ptr->current_odr);
        }
      }
    }

    /* stop the Device Driver's self schedule */
    if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
    {
      for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
      {
        if ( NULL != sensor_ptr->ddf_sensor_ptr[i] )
        {
          sns_smgr_enable_sched_data(sensor_ptr,
                                     SMGR_SENSOR_TYPE(sensor_ptr, i),
                                     FALSE);
        }
      }
    }
    sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_IDLE);
    if ( ! SMGR_SENSOR_IS_ACCEL(sensor_ptr) ||
         (sns_smgr.md.is_md_int_enabled == FALSE) )
    {
      status = sns_smgr_set_attr(sensor_ptr,
                                 SNS_DDF_SENSOR__ALL,
                                 SNS_DDF_ATTRIB_POWER_STATE,
                                 &set_data);
    }
    sns_smgr_adjust_latency_node();
    SNS_SMGR_PRINTF2(HIGH, "no_sample_ev - sensor=%d status=%d",
                     SMGR_SENSOR_ID(sensor_ptr), status);
  }
  return TRUE;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_config_filter_event

===========================================================================*/
/*!
  @brief Handles SENSOR_EVENT_CONFIG_FILTER event for given sensor

  @Detail
    CONFIG_FILTER event is only expected when sensor is READY or CONFIGURING state.
    1. Configure ODR

  @param[i] sensor_ptr - the sensor leader structure

  @return
    TRUE if event was processed
*/
/*=========================================================================*/
static boolean sns_smgr_process_config_filter_event(smgr_sensor_s* sensor_ptr)
{
  boolean processed = FALSE;
  if ( (SENSOR_STATE_READY       == sensor_ptr->sensor_state) ||
       (SENSOR_STATE_CONFIGURING == sensor_ptr->sensor_state) )
  {
    sns_smgr_config_odr_lpf(sensor_ptr);
    processed = TRUE;
  }
  return processed;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_config_filter_done_event

===========================================================================*/
/*!
  @brief Handles CONFIG_FILTER_DONE event for given sensor

  @Detail
    CONFIG_FILTER_DONE event is only expected when sensor is CONFIGURING state.
    1. Update sensor state to READY

  @param[i] sensor_ptr - the sensor leader structure

  @return
    TRUE if event was processed
*/
/*=========================================================================*/
static boolean sns_smgr_process_config_filter_done_event(smgr_sensor_s* sensor_ptr)
{
  boolean processed = FALSE;
  uint8_t i;
  for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    if ( sensor_ptr->ddf_sensor_ptr[i] != NULL )
    {
      smgr_rpt_item_s*  item_ptr;
      smgr_rpt_item_s** item_ptr_ptr;
      SMGR_FOR_EACH_ASSOC_ITEM( sensor_ptr->ddf_sensor_ptr[i], item_ptr,
                                item_ptr_ptr )
      {
        int rpt_intvl = (int) item_ptr->parent_report_ptr->rpt_interval;
        int delta_intvl;
        smgr_compute_report_interval(item_ptr->parent_report_ptr);
        delta_intvl = rpt_intvl - (int) item_ptr->parent_report_ptr->rpt_interval;
        if ( (item_ptr->parent_report_ptr->rpt_tick == 0) || (rpt_intvl == 0) )
        {
          SNS_SMGR_PRINTF2(
             LOW, "config_done - old rpt_tick=%u old_intvl=%d",
             item_ptr->parent_report_ptr->rpt_tick, rpt_intvl);
#ifndef SNS_QDSP_SIM
          item_ptr->parent_report_ptr->rpt_tick =
            sns_smgr.last_tick.u.low_tick +
            item_ptr->parent_report_ptr->rpt_interval;
#else
          item_ptr->parent_report_ptr->rpt_tick =
             sns_qdsp_sim_low_tick() +
            item_ptr->parent_report_ptr->rpt_interval;
#endif
        }
        else if ( delta_intvl > 0 )
        {
          SNS_SMGR_PRINTF2(
             LOW, "config_done - old rpt_tick=%u delta_intvl=%d",
             item_ptr->parent_report_ptr->rpt_tick, delta_intvl);
          item_ptr->parent_report_ptr->rpt_tick -= delta_intvl;
        }

        SNS_SMGR_PRINTF3(LOW, "config_done - rpt_id=%d intvl=%d tick=%u",
                         item_ptr->parent_report_ptr->rpt_id,
                         item_ptr->parent_report_ptr->rpt_interval,
                         item_ptr->parent_report_ptr->rpt_tick);
        SNS_SMGR_PRINTF3(LOW, "config_done - ddf_sensor=%d samples_sent=%d last=%u",
                         SMGR_SENSOR_TYPE(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                          item_ptr->ddf_sensor_ptr->data_type),
                         SMGR_BIT_TEST(item_ptr->flags,
                                       SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B),
                         item_ptr->ts_last_sent);
      }
      if ( !SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) &&
           (sensor_ptr->ddf_sensor_ptr[i]->max_requested_freq_hz > 0) )
      {
        sns_smgr_register_into_scheduler(sensor_ptr->ddf_sensor_ptr[i]);
      }
    }
  }

  sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_READY);
  processed = TRUE;
  return processed;
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_sensor_event

===========================================================================*/
/*!
  @brief process the event for the sensor leader

  @Detail during processing events, it is allowed the sensor to be inserted into event que.
          However, the sensor shall not be deleted from event que if it is already in the event que.


  @param[i] sensor_ptr - the sensor leader structure

  @return
    NONE
*/
/*=========================================================================*/
static boolean smgr_process_sensor_event_internal(smgr_sensor_s *sensor_ptr,
                                                  smgr_sensor_event_e event)
{
  boolean processed = FALSE;

  SNS_SMGR_PRINTF3(HIGH, "sensor_ev_internal - sensor=%d state=%d ev=%d",
                   SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state,
                   event);

  switch ( event )
  {
    case SENSOR_EVENT_POWERING_UP:
      processed = sns_smgr_process_powering_up_event();
      break;
    case SENSOR_EVENT_WAKE_UP:
      processed = sns_smgr_process_wake_up_event(sensor_ptr);
      break;
    case SENSOR_EVENT_CONFIG_FILTER:
      processed = sns_smgr_process_config_filter_event(sensor_ptr);
      break;
    case SENSOR_EVENT_CONFIG_FILTER_DONE:
      processed = sns_smgr_process_config_filter_done_event(sensor_ptr);
      break;
    case SENSOR_EVENT_NO_SAMPLE:
      processed = sns_smgr_process_no_sample_event(sensor_ptr);
      break;
  }
  SNS_SMGR_PRINTF3(HIGH, "sensor_ev_internal - sensor=%d new_state=%d done_ticks=%u",
                   SMGR_SENSOR_ID(sensor_ptr), sensor_ptr->sensor_state,
                   sensor_ptr->event_done_tick);
  return processed;
}


/*===========================================================================

  FUNCTION:   smgr_mark_reports_for_flushing_after_odr_change

===========================================================================*/
/*!
  @brief Changes state of affected reports to FLUSHING after odr changed

  @param[i] ddf_sensor_ptr

  @return None
*/
/*=========================================================================*/
void smgr_mark_reports_for_flushing_after_odr_change(smgr_ddf_sensor_s* ddf_sensor_ptr)
{
 const smgr_sample_s* sample_ptr = NULL;

  sample_ptr = sns_smgr_get_latest_sample(ddf_sensor_ptr->depot_ptr);
  
  SNS_SMGR_PRINTF2(MED, "mark_4_flushing after_odr_change - sensor=%d sample_ts=%u", 
                   SMGR_SENSOR_ID(ddf_sensor_ptr->sensor_ptr),  
                   sample_ptr ? sample_ptr->time_stamp : 0);

  if ( sample_ptr != NULL )
  {
    smgr_rpt_item_s* item_ptr = ddf_sensor_ptr->rpt_item_ptr;
    while ( item_ptr != NULL )
    {
      smgr_rpt_spec_s* rpt_ptr = item_ptr->parent_report_ptr;
      if ( (rpt_ptr->header_abstract.msg_id == SNS_SMGR_BUFFERING_REQ_V01) &&
           (item_ptr->state != SENSOR_ITEM_STATE_LINKING) &&
           TICK1_GTR_TICK2(sample_ptr->time_stamp, item_ptr->ts_last_sent) )
      {
        rpt_ptr->state = SMGR_RPT_STATE_FLUSHING;
        SNS_SMGR_PRINTF1(HIGH, "mark_4_flushing - rpt_id=%d", rpt_ptr->rpt_id);
      }
      item_ptr = item_ptr->next_item_ptr;
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_event

===========================================================================*/
/*!
  @brief  A device driver calls this function to deliver an event

  @Detail

  @param[i] smgr_handle  smgr handle passed to DD
  @param[i] sensor  sensor enum
  @param[i] event   event name
  @return
   SNS_DDF_SUCCESS
   SNS_DDF_EINVALID_PARAM if event with the sensor is not expected
*/
/*=========================================================================*/
sns_ddf_status_e sns_ddf_smgr_notify_event(
    sns_ddf_handle_t  smgr_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_event_e   event)
{
  smgr_sensor_s*    sensor_ptr = (smgr_sensor_s*)smgr_handle;
  sns_ddf_status_e  rc = SNS_DDF_SUCCESS;

  if ( SMGR_HANDLE_VALID(sensor_ptr) )
  {
    switch (event)
    {
      case SNS_DDF_EVENT_INIT:
      {
        sensor_ptr->init_state = SENSOR_INIT_SUCCESS;
        /* Enter init for all device drivers */
        SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
        break;
      }
      case SNS_DDF_EVENT_BIAS_READY:
      {
        sns_ddf_bias_t *bias_ptr;
        uint32_t len;
        /* Read the bias attribute */
        if ( sns_smgr_get_attr(sensor_ptr,
                               sensor,
                               SNS_DDF_ATTRIB_BIAS,
                               (void**)&bias_ptr,
                               &len) == SNS_DDF_SUCCESS )
        {
          /* only process further if we can read all 3-axis bias */
          if (len != SNS_SMGR_SENSOR_DIMENSION_V01)
          {
            sns_ddf_memhandler_free( &sensor_ptr->memhandler );
            rc = SNS_DDF_EFAIL;
            break;
          }
          SNS_SMGR_PRINTF3(LOW, "bias - 0x%08x 0x%08x 0x%08x",
                           bias_ptr[0], bias_ptr[1], bias_ptr[2]);

          if (sns_smgr.sensor_test_info.save_to_registry)
          {
             sns_ddf_bias_t *local_bias_ptr = bias_ptr;

             /* Write to registry file */
             if (sensor == SNS_DDF_SENSOR_ACCEL)
             {
                sns_smgr_update_reg_data(SNS_REG_ITEM_ACC_X_BIAS_V02,
                                      SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                      sizeof(sns_ddf_bias_t),
                                      (uint8_t*)local_bias_ptr++);
                sns_smgr_update_reg_data(SNS_REG_ITEM_ACC_Y_BIAS_V02,
                                      SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                      sizeof(sns_ddf_bias_t),
                                      (uint8_t*)local_bias_ptr++);
                sns_smgr_update_reg_data(SNS_REG_ITEM_ACC_Z_BIAS_V02,
                                      SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                      sizeof(sns_ddf_bias_t),
                                      (uint8_t*)local_bias_ptr);
             }
             else if (sensor == SNS_DDF_SENSOR_GYRO)
             {
                sns_smgr_update_reg_data(SNS_REG_ITEM_GYRO_X_BIAS_V02,
                                     SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                     sizeof(sns_ddf_bias_t),
                                     (uint8_t*)local_bias_ptr++);
                sns_smgr_update_reg_data(SNS_REG_ITEM_GYRO_Y_BIAS_V02,
                                     SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                     sizeof(sns_ddf_bias_t),
                                     (uint8_t*)local_bias_ptr++);
                sns_smgr_update_reg_data(SNS_REG_ITEM_GYRO_Z_BIAS_V02,
                                     SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                     sizeof(sns_ddf_bias_t),
                                     (uint8_t*)local_bias_ptr);
             }
             else
             {
                sns_ddf_memhandler_free( &(sensor_ptr->memhandler) );
                rc = SNS_DDF_EINVALID_PARAM;
                break;
             }
          }
          /* Update calibration information in sensor structure */
          if ( (sensor_ptr->ddf_sensor_ptr[0] != NULL) &&
               (sensor_ptr->ddf_sensor_ptr[0]->auto_cal.used == true) &&
               (sns_smgr.sensor_test_info.save_to_registry) )

          {
             /* reset autocal parameters in registry */
             int32_t def_auto_bias = 0;
             if (sensor == SNS_DDF_SENSOR_ACCEL)
             {
                sns_smgr_update_reg_data(SNS_REG_ITEM_ACC_X_DYN_BIAS_V02,
                                         SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                         sizeof(int32_t),
                                         (uint8_t *)&def_auto_bias);
                sns_smgr_update_reg_data(SNS_REG_ITEM_ACC_Y_DYN_BIAS_V02,
                                         SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                         sizeof(int32_t),
                                         (uint8_t *)&def_auto_bias);
                sns_smgr_update_reg_data(SNS_REG_ITEM_ACC_Z_DYN_BIAS_V02,
                                         SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                         sizeof(int32_t),
                                         (uint8_t *)&def_auto_bias);
             }
             else if (sensor == SNS_DDF_SENSOR_GYRO)
             {
                sns_smgr_update_reg_data(SNS_REG_ITEM_GYRO_X_DYN_BIAS_V02,
                                         SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                         sizeof(int32_t),
                                         (uint8_t *)&def_auto_bias);
                sns_smgr_update_reg_data(SNS_REG_ITEM_GYRO_Y_DYN_BIAS_V02,
                                         SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                         sizeof(int32_t),
                                         (uint8_t *)&def_auto_bias);
                sns_smgr_update_reg_data(SNS_REG_ITEM_GYRO_Z_DYN_BIAS_V02,
                                         SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                         sizeof(int32_t),
                                         (uint8_t *)&def_auto_bias);
             }
          }

          if ( (sns_smgr.sensor_test_info.apply_cal_now) &&
               (sensor_ptr->ddf_sensor_ptr[0] != NULL) )
          {
            smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[0];
             /* reset autocal parameters in SMGR */
            smgr_load_default_cal( &(ddf_sensor_ptr->auto_cal) );
              /* update factory cal params */
            SNS_OS_MEMCOPY(&(ddf_sensor_ptr->factory_cal.zero_bias[0]),
                           bias_ptr, sizeof(sns_ddf_bias_t) * len);

            /* update full cal params */
            ddf_sensor_ptr->full_cal.zero_bias[0] =
                    ddf_sensor_ptr->factory_cal.zero_bias[0] +
                    ddf_sensor_ptr->auto_cal.zero_bias[0];
            ddf_sensor_ptr->full_cal.zero_bias[1] =
                    ddf_sensor_ptr->factory_cal.zero_bias[1] +
                    ddf_sensor_ptr->auto_cal.zero_bias[1];
            ddf_sensor_ptr->full_cal.zero_bias[2] =
                    ddf_sensor_ptr->factory_cal.zero_bias[2] +
                    ddf_sensor_ptr->auto_cal.zero_bias[2];

            ddf_sensor_ptr->auto_cal.used = FALSE;
            ddf_sensor_ptr->factory_cal.used = TRUE;
            ddf_sensor_ptr->full_cal.used = TRUE;
          }
        }
        /* Free memory */
        sns_ddf_memhandler_free( &(sensor_ptr->memhandler) );
        break;
      }
      case  SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP:
      {
        sns_ddf_registry_group_s* reg_group_ptr = NULL;
        uint32_t len = 0;
        rc = SNS_DDF_EFAIL;

        if ( sns_smgr_get_attr(sensor_ptr,
                               sensor,
                               SNS_DDF_ATTRIB_REGISTRY_GROUP,
                               (void**)&reg_group_ptr,
                               &len) == SNS_DDF_SUCCESS )
        {
         if ( sns_smgr_update_reg_data(sensor_ptr->const_ptr->driver_reg_id,
                                       sensor_ptr->const_ptr->driver_reg_type,
                                       reg_group_ptr->size,
                                       (uint8_t*) reg_group_ptr->group_data) == SNS_SUCCESS )
         {
           rc = SNS_DDF_SUCCESS;
         }
        }
        sns_ddf_memhandler_free( &sensor_ptr->memhandler );
        break;
      }
      case SNS_DDF_EVENT_MOTION:
      {
        if (sensor==SNS_DDF_SENSOR_ACCEL)
        {
            sns_smgr_handle_md_int(sns_em_get_timestamp());
        }
        else
        {
            rc = SNS_DDF_EINVALID_PARAM;
        }
        break;
      }
      case SNS_DDF_EVENT_ODR_CHANGED:
      {
        smgr_ddf_sensor_s*  ddf_sensor_ptr;
        uint16_t prev_odr = 0;

        /* Determines the new ODR */
        if( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B) )
        {
          ddf_sensor_ptr = sns_smgr_match_ddf_sensor(sensor_ptr, sensor);

          if(ddf_sensor_ptr != NULL)
          {
            SNS_SMGR_PRINTF1(MED,"FIFO EVENT ODR CHANGED -ddf_sensor=%d",sensor);
            prev_odr = ddf_sensor_ptr->current_odr;
            sns_smgr_fifo_flush( sensor_ptr );
            smgr_mark_reports_for_flushing_after_odr_change( ddf_sensor_ptr );
            smgr_flush_reports();
            sns_smgr_update_odr( ddf_sensor_ptr );
            sns_smgr_fifo_on_event_odr_changed( ddf_sensor_ptr, sensor );
            sns_smgr_update_sampling_setting( ddf_sensor_ptr, prev_odr );
            if ( !SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) &&
                  (ddf_sensor_ptr->max_requested_freq_hz > 0) )
            {
               sns_smgr_register_into_scheduler(ddf_sensor_ptr);
            }
          }
        }
        else
        {
          rc = SNS_DDF_EINVALID_PARAM;
        }
        break;
      }

      //--------------- FIFO events -----------------
      case SNS_DDF_EVENT_FIFO_WM_INT:
      {
        rc = sns_smgr_fifo_on_event_fifo_wm_int( sensor_ptr, sensor );
        break;
      }
      case SNS_DDF_EVENT_FIFO_OVERFLOW:
      {
        rc = sns_smgr_fifo_on_event_fifo_overflow( sensor_ptr, sensor );
        break;
      }
      case SNS_DDF_EVENT_FIFO_AVAILABLE:
      {
        rc = sns_smgr_fifo_on_event_fifo_available( sensor_ptr, sensor );
        break;
      }
      case SNS_DDF_EVENT_FIFO_UNAVAILABLE:
      {
        rc = sns_smgr_fifo_on_event_fifo_unavailable( sensor_ptr, sensor );
        break;
      }

      default:
      {
        rc = SNS_DDF_EINVALID_PARAM;
        break;
      }
    }  //switch (event)
  }  //if ( SMGR_HANDLE_VALID(sensor_ptr) )
  else
  {
    rc = SNS_DDF_EINVALID_PARAM;
  }
  return rc;
}

/*===========================================================================

  FUNCTION:   sns_smgr_ready_reports_list_add

===========================================================================*/
/*!
  @brief Adds the given report to the list of reports ready to be sent.

  @param[i] report_ptr - the report to add

  @return none
*/
/*=========================================================================*/
bool sns_smgr_ready_reports_list_add(smgr_rpt_spec_s* report_ptr)
{
  bool added = false;
  smgr_rpt_spec_s** rpp = &sns_smgr.ready_report_ptr;
  while ( *rpp != NULL )
  {
    if ( *rpp == report_ptr )
    {
      /* already on ready list */
      break;
    }
    rpp = &(*rpp)->next_ready_report_ptr;
  }
  if (*rpp == NULL)
  {
    /* not already on ready list */
    *rpp = report_ptr;
    sns_smgr_set_data_gathering_cycle_flag();
    added = true;
  }
  return added;
}

/*===========================================================================

  FUNCTION:   sns_smgr_ready_reports_list_remove

===========================================================================*/
/*!
  @brief Removes the given report from the list of reports ready to be sent.

  @param[i] report_ptr - the report to remove from ready list

  @return none
*/
/*=========================================================================*/
void sns_smgr_ready_reports_list_remove(smgr_rpt_spec_s* report_ptr)
{
  smgr_rpt_spec_s** rpp = &sns_smgr.ready_report_ptr;
  while ( *rpp != NULL )
  {
    if ( *rpp == report_ptr )
    {
      /* remove this report from ready list */
      *rpp = report_ptr->next_ready_report_ptr;
      report_ptr->next_ready_report_ptr = NULL;
      SNS_SMGR_PRINTF1(MED, "ready_reports_list_remove - rpt_id=%d", report_ptr->rpt_id);
      break;
    }
    rpp = &(*rpp)->next_ready_report_ptr;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_handle_ready_item

===========================================================================*/
/*!
  @brief  A newly ready item may be the trigger for sending its parent report.

  @param[i] item_ptr - the Ready report item

  @return
   true if parent report added to ready report list

*/
/*=========================================================================*/
static bool sns_smgr_handle_ready_item(smgr_rpt_item_s* item_ptr)
{
  bool added = false;
  uint8_t i, ready_sensors = 0;
  uint8_t ready_items = 0, ready_dri_items = 0, ready_polling_items = 0;
  smgr_rpt_spec_s* rpt_ptr = item_ptr->parent_report_ptr;

  for ( i=0; i<rpt_ptr->num_items; i++ )
  {
    smgr_sensor_s* sensor_ptr = rpt_ptr->item_list[i]->ddf_sensor_ptr->sensor_ptr;

    if ( rpt_ptr->item_list[i]->state == SENSOR_ITEM_STATE_DONE )
    {
      ready_items++;
      if ( SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_DRI_SENSOR_B) )
      {
        ready_dri_items++;
      }
      else
      {
        ready_polling_items++;
      }
    }
    if ( (sensor_ptr->sensor_state == SENSOR_STATE_READY) ||
         ((sensor_ptr->sensor_state == SENSOR_STATE_CONFIGURING) &&
          (rpt_ptr->ind_count > 0)) )
    {
      ready_sensors++;
    }
  }
  SNS_SMGR_SAMPLING_DBG3(LOW, "ready_item - rpt_id=%d ddf_sensor=%d items=%d",
                         rpt_ptr->rpt_id,
                         SMGR_SENSOR_TYPE(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                          item_ptr->ddf_sensor_ptr->data_type),
                         rpt_ptr->num_items);
  SNS_SMGR_SAMPLING_DBG3(LOW, "ready_item - sensors=%d dri=%d polling=%d",
                         ready_sensors, ready_dri_items, ready_polling_items);
  if ( ready_sensors > 0 )
  {
    SNS_SMGR_SAMPLING_DBG2(
      MED, "rpt_id=%d ready_items=%d ", rpt_ptr->rpt_id, ready_items);
    sns_smgr_ready_reports_list_add(rpt_ptr);
    added = true;
  }
  return added;
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_sample

===========================================================================*/
/*!
  @brief Processes the given sample and sets item state accordingly.

  @param[i]  item_ptr      - report item
  @param[i]  sample_ptr    - sample from device
  @param[o]  dest_data     - destination for the resulting data (xyz)

  @return
  SNS_DDF_SUCCESS - sample processed successfully
  SNS_DDF_PENDING - no valid sample was created using given sample
  SNS_DDF_EFAIL - given sample already processed
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_process_sample(
  smgr_rpt_item_s*     item_ptr,
  const smgr_sample_s* sample_ptr,
  sns_ddf_time_t*      timestamp_ptr,
  int32_t              dest_data[SNS_SMGR_SENSOR_DIMENSION_V01]
)
{
  sns_ddf_status_e sample_status = SNS_DDF_PENDING;
  smgr_interpolator_obj_s* int_obj_ptr = &item_ptr->interpolator_obj;
  smgr_sample_s tmp_sample;
  boolean same_sample;

  SNS_ASSERT( (timestamp_ptr != NULL) && (dest_data != NULL) );

  SNS_SMGR_SAMPLE_PROC_DBG3(
    LOW, "process_sample - rpt_id=%d quality=%d sampling_count=%d",
    item_ptr->parent_report_ptr->rpt_id,
    item_ptr->quality, item_ptr->sampling_count);

  if ( (sample_ptr != NULL) &&
       (item_ptr->last_processed_sample_timestamp != sample_ptr->time_stamp) )
  {
    item_ptr->last_processed_sample_timestamp = sample_ptr->time_stamp;
    same_sample = FALSE;
  }
  else
  {
    same_sample = TRUE;
  }

  item_ptr->state = SENSOR_ITEM_STATE_IDLE;
  switch ( item_ptr->quality )
  {
  case SNS_SMGR_ITEM_QUALITY_CURRENT_SAMPLE_V01:
    if ( same_sample == FALSE )
    {
      if ( item_ptr->sampling_count == 0 )
      {
        if ( ((sample_ptr->time_stamp - item_ptr->ts_last_sent) >=
              (item_ptr->sampling_interval >> 1)) ||
             (item_ptr->parent_report_ptr->ind_count == 0) ||
              sns_smgr_is_event_sensor(item_ptr->ddf_sensor_ptr->sensor_ptr,
                                       item_ptr->ddf_sensor_ptr->data_type) )
        {
          SNS_OS_MEMCOPY(dest_data, sample_ptr->data, sizeof(sample_ptr->data));
          *timestamp_ptr = sample_ptr->time_stamp;
          item_ptr->state = SENSOR_ITEM_STATE_DONE;
          sample_status = sample_ptr->status;
        }
        else /* sample too close to last sample sent */
        {
          SNS_SMGR_PRINTF1(
            LOW, "process_sample - skipped ts=%u", sample_ptr->time_stamp);
        }
      }
      item_ptr->sampling_count =
        (item_ptr->sampling_count+1) % item_ptr->sampling_factor;
    }
    else
    {
      sample_status = SNS_DDF_EFAIL;
    }
    break;

  case SNS_SMGR_ITEM_QUALITY_INTERPOLATED_V01:
    if ( same_sample == FALSE )
    {
      sns_smgr_interpolator_update(int_obj_ptr, sample_ptr);
    }
    sample_status = sns_smgr_interpolate(int_obj_ptr, dest_data);
    SNS_SMGR_SAMPLE_PROC_DBG2(
      MED, "process_sample - rpt_id=%d status=%d",
      item_ptr->parent_report_ptr->rpt_id, sample_status);
    if ( sample_status != SNS_DDF_EINVALID_PARAM )
    {
      *timestamp_ptr = int_obj_ptr->desired_timestamp;
      item_ptr->state = SENSOR_ITEM_STATE_DONE;
    }
    else
    {
      sample_status = SNS_DDF_PENDING;
    }
    break;

  case SNS_SMGR_ITEM_QUALITY_INTERPOLATED_FILTERED_V01:
    if ( same_sample == FALSE )
    {
      sns_smgr_interpolator_update(int_obj_ptr, sample_ptr);
      SNS_SMGR_SAMPLE_PROC_DBG3(
        MED, "interp fed - ts1=%u desired=%u ts2=%u",
        int_obj_ptr->sample1_ptr ? int_obj_ptr->sample1_ptr->time_stamp : 0,
        int_obj_ptr->desired_timestamp,
        int_obj_ptr->sample2_ptr ? int_obj_ptr->sample2_ptr->time_stamp : 0);
    }
    tmp_sample.status = SNS_DDF_SUCCESS;
    while ( (tmp_sample.status == SNS_DDF_SUCCESS) &&
            (sample_status == SNS_DDF_PENDING) )
    {
      tmp_sample.status = sns_smgr_interpolate(int_obj_ptr, tmp_sample.data);
      SNS_SMGR_SAMPLE_PROC_DBG2(
        LOW, "interpolated - status=%d ts=%u",
        tmp_sample.status, int_obj_ptr->desired_timestamp);
      if ( tmp_sample.status != SNS_DDF_EINVALID_PARAM )
      {
        tmp_sample.time_stamp = int_obj_ptr->desired_timestamp;
        sample_status = sns_smgr_cic_update(&item_ptr->cic_obj, &tmp_sample,
                                            timestamp_ptr, dest_data);
        SNS_SMGR_SAMPLE_PROC_DBG3(
          LOW, "cic - status=%d ts=%u count=%d",
          sample_status, *timestamp_ptr, item_ptr->cic_obj.count);
        if ( sample_status == SNS_DDF_PENDING )
        {
          SMGR_UPDATE_INTERP_TIMESTAMP(int_obj_ptr);
        }
        else
        {
          item_ptr->state = SENSOR_ITEM_STATE_DONE;
        }
      }
    }
    break;

  case SNS_SMGR_ITEM_QUALITY_FILTERED_V01:
    if ( same_sample != FALSE )
    {
      sample_status = SNS_DDF_EFAIL;
    }
    else
    {
      sample_status = sns_smgr_cic_update(&item_ptr->cic_obj, sample_ptr,
                                          timestamp_ptr, dest_data);

      if ( sample_status != SNS_DDF_PENDING )
      {
        item_ptr->state = SENSOR_ITEM_STATE_DONE;
      }
    }
    break;
  }
  SNS_SMGR_SAMPLE_PROC_DBG2(LOW, "process_sample - sample_status=%d item_state=%d",
                            sample_status, item_ptr->state);
  return sample_status;
}

/*===========================================================================

  FUNCTION:   sns_smgr_item_done

===========================================================================*/
/*!
  @brief  Handles the item in DONE state

  @param[i] item_ptr  - the report item

  @return
   None

*/
/*=========================================================================*/
static void sns_smgr_item_done(smgr_rpt_item_s* item_ptr)
{
  if ( SMGR_OK_TO_SEND(item_ptr->parent_report_ptr) )
  {
    bool report_2b_generated = sns_smgr_handle_ready_item(item_ptr);
    if ( report_2b_generated &&
         (item_ptr->parent_report_ptr->proc_type == SNS_PROC_APPS_V01) )
    {
      /* opportunity for all reports destined for AP to be generated */
      sns_smgr_flush_reports_to_same_processor(SNS_PROC_APPS_V01);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_check_report_schedule

===========================================================================*/
/*!
  @brief  Checks whether reports should be sent

  @param[i] ddf_sensor_ptr  - the DDF sensor 
  @param[i] num_samples_set - number of samples just received

  @return
   None

*/
/*=========================================================================*/
static void sns_smgr_check_report_schedule(
   smgr_ddf_sensor_s* ddf_sensor_ptr,
   uint32_t num_samples_set
)
{
  smgr_rpt_item_s*     item_ptr;
  smgr_rpt_item_s**    item_ptr_ptr;
  const smgr_sample_s* sample_ptr = 
    sns_smgr_get_latest_sample(ddf_sensor_ptr->depot_ptr);
  uint32_t half_fifo_time;
  if ( sample_ptr == NULL )
  {
    return; /* no sample to report */
  }

  half_fifo_time = 
    (num_samples_set >> 1) * (DSPS_SLEEP_CLK/MAX(1,ddf_sensor_ptr->current_odr));
  SMGR_FOR_EACH_ASSOC_ITEM( ddf_sensor_ptr, item_ptr, item_ptr_ptr )
  {
    if ( SMGR_OK_TO_SEND(item_ptr->parent_report_ptr) &&
         (item_ptr->parent_report_ptr->rpt_tick != 0) &&
         (item_ptr->state == SENSOR_ITEM_STATE_IDLE) &&
         (item_ptr->parent_report_ptr->header_abstract.msg_id == 
          SNS_SMGR_BUFFERING_REQ_V01) &&
         (item_ptr->parent_report_ptr->q16_rpt_rate != 0) &&
         TICK1_GEQ_TICK2(sample_ptr->time_stamp + half_fifo_time,
                         item_ptr->parent_report_ptr->rpt_tick) )
    {
      if ( sns_smgr_ready_reports_list_add(item_ptr->parent_report_ptr) )
      {
        SNS_SMGR_PRINTF1(
          LOW, "ck_rpt_sched - id=%d", item_ptr->parent_report_ptr->rpt_id);
        item_ptr->parent_report_ptr->state = SMGR_RPT_STATE_FLUSHING;
      }
      if ( item_ptr->parent_report_ptr->proc_type == SNS_PROC_APPS_V01 )
      {
         /* opportunity for all reports destined for AP to be generated */
         sns_smgr_flush_reports_to_same_processor(SNS_PROC_APPS_V01);
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_handle_new_sample

===========================================================================*/
/*!
  @brief  Saves the given data into given DDF sensor's depot and updates all
          associated items' state.

  @param[i] ddf_sensor_ptr - the DDF sensor
  @param[i] data_ptr       - sample polled from device

  @return
   None

*/
/*=========================================================================*/
static void sns_smgr_handle_new_sample(smgr_ddf_sensor_s* ddf_sensor_ptr,
                                       const sns_ddf_sensor_data_s* data_ptr)
{
  smgr_rpt_item_s*     item_ptr;
  smgr_rpt_item_s**    item_ptr_ptr;
  const smgr_sample_s* sample_ptr =
    sns_smgr_deposit_sample(ddf_sensor_ptr->depot_ptr, data_ptr);

  SNS_SMGR_SAMPLING_DBG3(
    LOW, "new_sample - ddf_sensor=%d sample_status=%d ts=%u",
    SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
    data_ptr->status, data_ptr->timestamp);
  if ( data_ptr->samples != NULL )
  {
    SNS_SMGR_SAMPLING_DBG3(LOW, "new_sample - X=%x Y=%x Z=%x",
                           data_ptr->samples[0].sample,
                           data_ptr->samples[1].sample,
                           data_ptr->samples[2].sample);
  }

  ddf_sensor_ptr->sensor_type_state = SENSORTYPE_STATE_IDLE;

  SMGR_FOR_EACH_ASSOC_ITEM( ddf_sensor_ptr, item_ptr, item_ptr_ptr )
  {
    if ( item_ptr->state >= SENSOR_ITEM_STATE_IDLE )
    {
      if ( item_ptr->parent_report_ptr->header_abstract.msg_id ==
           SNS_SMGR_REPORT_REQ_V01 )
      {
        if ( ((data_ptr->status == SNS_DDF_SUCCESS) ||
              SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B)) &&
             TICK1_GEQ_TICK2(sample_ptr->time_stamp,
                             item_ptr->interpolator_obj.desired_timestamp) )
        {
          item_ptr->state = SENSOR_ITEM_STATE_DONE;
        }
      }
      else if ( item_ptr->parent_report_ptr->q16_rpt_rate != 0 )
      {
        if ( !item_ptr->parent_report_ptr->periodic_like &&
             (item_ptr->parent_report_ptr->rpt_tick != 0) &&
             TICK1_GEQ_TICK2(sample_ptr->time_stamp,
                             item_ptr->parent_report_ptr->rpt_tick) &&
             TICK1_GEQ_TICK2(sample_ptr->time_stamp,
                             item_ptr->interpolator_obj.desired_timestamp) )
        {
          item_ptr->state = SENSOR_ITEM_STATE_DONE;
        }
        else if ( item_ptr->parent_report_ptr->periodic_like &&
                  TICK1_GEQ_TICK2(sample_ptr->time_stamp,
                                  item_ptr->interpolator_obj.desired_timestamp) &&
                  ((data_ptr->status == SNS_DDF_SUCCESS) ||
                   SMGR_BIT_TEST(item_ptr->flags, SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B)) )
        {
          item_ptr->state = SENSOR_ITEM_STATE_DONE;
        }
        else if ( sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr,
                                           ddf_sensor_ptr->data_type) &&
                  (data_ptr->status == SNS_DDF_SUCCESS) )
        {
          item_ptr->state = SENSOR_ITEM_STATE_DONE;
        }
      }
      else
      {
        smgr_query_s* query_ptr = item_ptr->query_ptr;
        while ( query_ptr != NULL )
        {
          if ( TICK1_GEQ_TICK2(sample_ptr->time_stamp, item_ptr->query_ptr->T1) )
          {
            sns_smgr_ready_reports_list_add(item_ptr->parent_report_ptr);
            break;
          }
          query_ptr = query_ptr->next_query_ptr;
        }
      }
    }
    if ( item_ptr->state == SENSOR_ITEM_STATE_DONE )
    {
      sns_smgr_item_done(item_ptr);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_data

===========================================================================*/
/*!
  @brief  A device driver calls this function to deliver data that was previously
    requested using get_data()

  @Detail

  @param[i] smgr_handle - A value supplied when initializing the device driver. It is the
                         index to the sensor structures
  @param[i] data - pointer to an array of data types (known as sensors in DDF)
  @param[i] data_len - Number of data types
  @return
   SNS_DDF_SUCCESS
   SNS_DDF_EFAIL
*/
/*=========================================================================*/
sns_ddf_status_e sns_ddf_smgr_notify_data (
    sns_ddf_handle_t       smgr_handle,
    sns_ddf_sensor_data_s  data[],
    uint32_t               data_len)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  smgr_sensor_s* sensor_ptr = (smgr_sensor_s*)smgr_handle;

  if ( sensor_ptr->const_ptr->flags & SNS_REG_SSI_FLAG_LTCY_ENABLE )
  {
    sns_latency.dri_get_data_end_ts = sns_em_get_timestamp();
    sns_latency.dri_notify_irq_ts = data[0].timestamp;
    sns_profiling_log_latency_dri(sns_latency, data[0].sensor);
  }
  if ( SMGR_HANDLE_VALID(sensor_ptr) &&
       (data_len <= SMGR_MAX_DATA_TYPES_PER_DEVICE) )
  {
    uint8_t i;

    #if defined(FEATURE_TEST_DRI)
    if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
    {
      sns_test_dri_sampling_latency();
    }
    #endif /* defined(FEATURE_TEST_DRI) */

    for ( i = 0; i < data_len; i++ )
    {
      smgr_ddf_sensor_s* ddf_sensor_ptr =
        sns_smgr_match_ddf_sensor(sensor_ptr, data[i].sensor);
      if ( ddf_sensor_ptr != NULL )
      {
        const smgr_sample_s* last_sample_ptr =
          sns_smgr_get_latest_sample(ddf_sensor_ptr->depot_ptr);
        uint8_t dimension = ddf_sensor_ptr->depot_ptr->num_axes;
        sns_ddf_sensor_data_s single_data;
        sns_ddf_time_t* end_ts_ptr = &data[i].timestamp;
        uint32_t j=0;
        uint32_t samples_set_index = 0;
        /*number of samples , x,y,z comprize of 1 sample set. */
        uint32_t num_samples_set=1;
        float time_interval_offset = 0.0;
        float time_interval = 0.0;
        float first_ts = (float)data[i].timestamp;

        SNS_ASSERT(ddf_sensor_ptr != NULL);

        SNS_SMGR_SAMPLING_DBG3(
          LOW, "notify_data - ddf_sensor=%d state=%d type_state=%d",
          data[i].sensor, sensor_ptr->sensor_state,
          ddf_sensor_ptr->sensor_type_state);

        single_data.sensor = data[i].sensor;
        single_data.status = data[i].status;
        single_data.num_samples = data[i].num_samples;
        num_samples_set = ((data[i].num_samples)+(dimension-1))/dimension;
        if( num_samples_set > 0)
        {
          single_data.num_samples = data[i].num_samples / num_samples_set;
          if ( num_samples_set > 1 )
          {
            time_interval = 
              ((float)data[i].end_timestamp-data[i].timestamp)/(num_samples_set-1);
            end_ts_ptr = &data[i].end_timestamp;
          }
        }
        SNS_SMGR_SAMPLING_DBG3(
           LOW, "#sets=%d #dataSamples=%d intvl=%u", 
           num_samples_set, data[i].num_samples, time_interval);
        SNS_SMGR_SAMPLING_DBG3(
           LOW, "1st_ts=%d last_ts=%d invt*100=%d", 
           data[i].timestamp, data[i].end_timestamp, time_interval*100);
        for ( j=0; j< num_samples_set; j++ )
        {
          single_data.samples = &(data[i].samples[samples_set_index]);
          single_data.timestamp = (sns_ddf_time_t)(first_ts + time_interval_offset);
          if ( j == num_samples_set-1 )
          {
            single_data.timestamp = *end_ts_ptr;
          }
          samples_set_index += dimension;
          time_interval_offset += time_interval;

          if ( sensor_ptr->sensor_state == SENSOR_STATE_READY )
          {
            SNS_SMGR_SAMPLING_DBG3(
              LOW, "notify_data - ddf_sensor=%d dri_count=%d data_poll_ts=%d",
              single_data.sensor, ddf_sensor_ptr->dri_count,
              ddf_sensor_ptr->data_poll_ts);
            if ( ddf_sensor_ptr->sensor_type_state == SENSORTYPE_STATE_IDLE )
            {
              if ( ddf_sensor_ptr->dri_count == 0 )
              {
                sns_smgr_handle_new_sample(ddf_sensor_ptr, &single_data);
              }
              ddf_sensor_ptr->dri_count =
                (ddf_sensor_ptr->dri_count + 1) % ddf_sensor_ptr->device_sampling_factor;
            }
            else if ( ddf_sensor_ptr->data_poll_ts != 0 )
            {
              /* asynchronous response */
              SNS_SMGR_SAMPLING_DBG3(
                LOW, "notify_data - delay=%d poll_ts=%u sample_ts=%u",
                sns_em_get_timestamp() - ddf_sensor_ptr->data_poll_ts,
                ddf_sensor_ptr->data_poll_ts, single_data.timestamp);
              sns_smgr_handle_new_sample(ddf_sensor_ptr, &single_data);
            }
            ddf_sensor_ptr->sensor_type_state = SENSORTYPE_STATE_IDLE;
          }
          else
          {
            SNS_SMGR_SAMPLING_DBG2(
              HIGH, "notify_data - ddf_sensor %d not ready ts=%u",
              single_data.sensor, single_data.timestamp);
            single_data.status = SNS_DDF_EINVALID_DATA;
            sns_smgr_handle_new_sample(ddf_sensor_ptr, &single_data);
          }
        }
        if ( num_samples_set > 1 )
        {
          sns_smgr_check_report_schedule(ddf_sensor_ptr, num_samples_set);
        }
        else if ( (num_samples_set == 0) && (data[i].status != SNS_DDF_SUCCESS) )
        {
          ddf_sensor_ptr->sensor_type_state = SENSORTYPE_STATE_IDLE;
        }

        if ( last_sample_ptr != NULL )
        {
          uint32_t delta_time =
            data[i].timestamp - last_sample_ptr->time_stamp;
          uint8_t num_skipped_samples =
            delta_time / ddf_sensor_ptr->depot_data_interval;
          ddf_sensor_ptr->max_skipped_samples =
            MAX(ddf_sensor_ptr->max_skipped_samples, num_skipped_samples);
        }
      }
    }
    status = SNS_DDF_SUCCESS;
  }
  return status;
}

/*===========================================================================

  FUNCTION:   sns_ddf_smgr_notify_test_complete

===========================================================================*/
/*!
  @brief  A device driver calls this function to notify the completion of
    a factory test which was previously requested using run_test()

  @Detail

  @param[i] smgr_handle  smgr handle passed to DD
  @param[i] sensor  sensor enum
  @param[i] status  pass or fail
  @param[i] err     driver-specific error code

  @return
   SNS_DDF_SUCCESS
   DDF error codes
*/
/*=========================================================================*/
sns_ddf_status_e sns_ddf_smgr_notify_test_complete(
    sns_ddf_handle_t  smgr_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_status_e  status,
    uint32_t          err)
{
  smgr_sensor_s* sensor_ptr = (smgr_sensor_s*)smgr_handle;

  SNS_SMGR_PRINTF3(MED, "notify_test_cmplt - sensor=%d status=%d err=%d",
                   sensor, status, err);

  if ( SMGR_HANDLE_VALID(sensor_ptr) &&
       (SMGR_SENSOR_ID(sensor_ptr) == sns_smgr.sensor_test_info.sensor_id) )
  {
    /* we only need to know whether status is SUCCESS or not */
    sns_smgr_test_status_e_v01 test_status = (status == SNS_DDF_SUCCESS)
       ? SNS_SMGR_TEST_STATUS_SUCCESS_V01 : SNS_SMGR_TEST_STATUS_FAIL_V01;
    sns_smgr_generate_test_result(&sns_smgr.sensor_test_info,
                                  test_status,
                                  err);
    /* reset the sensor */
    if (SENSOR_STATE_FAILED != sensor_ptr->sensor_state)
    {
      SMGR_DRV_FN_PTR(sensor_ptr)->reset(sensor_ptr->dd_handle);
    }
    sns_smgr_set_sensor_state(sensor_ptr,
                              sns_smgr.sensor_test_info.saved_sensor_state);

    /* test is done */
    sns_smgr.sensor_test_info.test_done = true;

#ifndef SNS_EXCLUDE_POWER
    if ( sns_q_cnt(&sns_smgr.report_queue) == 0 )
    {
      sns_hw_power_rail_config(SNS_SMGR_POWER_OFF);
    }
#endif
    return SNS_DDF_SUCCESS;
  }
  else
  {
    return SNS_DDF_EINVALID_PARAM;
  }
}

/*===========================================================================

  FUNCTION:   smgr_process_sensor_event

===========================================================================*/
/*!
  @brief Checks each sensor for event to process.

  @param[i] NONE

  @return
    The next schedule timetick offset from now
*/
/*=========================================================================*/
void smgr_process_sensor_event(void)
{
  uint8_t   event_count = 0;
  uint32_t  next_tick_offset = SMGR_MAX_TICKS;

  sns_smgr_get_tick64();
  SNS_SMGR_PRINTF1(MED, "sensor_event - time now=%u", sns_smgr.last_tick.u.low_tick);

  uint8_t   i;
  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
    if ( (sensor_ptr->event_done_tick != 0) &&
         TICK1_GEQ_TICK2(sns_smgr.last_tick.u.low_tick, sensor_ptr->event_done_tick) )
    {
      SNS_SMGR_PRINTF3(MED, "sensor_event - sensor=%d state=%d flags=0x%x",
                       SMGR_SENSOR_ID(sensor_ptr),
                       sensor_ptr->sensor_state, sensor_ptr->flags);
      sensor_ptr->event_done_tick = 0;

      if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B) )
      {
        if ( smgr_get_max_requested_freq(sensor_ptr) == 0 )
        {
          SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B);
          SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B);

          /* Reconfigure fifo, special case of disabling fifo before everything else */
          SNS_SMGR_FIFO_DBG1( MED, "FIFO SMGR_SENSOR_FLAGS_ITEM_DEL_B sensor=%d",
                              sensor_ptr->const_ptr->sensor_id );
          sns_smgr_fifo_configure(sensor_ptr);

          smgr_process_sensor_event_internal(sensor_ptr, SENSOR_EVENT_NO_SAMPLE);
          continue; /* done with this sensor; move on to the next */
        }   
        else 
        { 
          uint8_t j=0;
          for ( j=0; j<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); j++ )
          {
            smgr_ddf_sensor_s* ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[j];
            if ( (ddf_sensor_ptr != NULL)  && ( ddf_sensor_ptr->max_requested_freq_hz == 0))
            {
              sns_smgr_deregister_from_scheduler(ddf_sensor_ptr);
              ddf_sensor_ptr->current_odr = 0;
              ddf_sensor_ptr->max_high_perf_freq_hz = 0;
              ddf_sensor_ptr->max_requested_freq_hz = 0;
              ddf_sensor_ptr->dri_count = 0;
              ddf_sensor_ptr->device_sampling_factor = 1;
              ddf_sensor_ptr->depot_data_rate = 0;
              ddf_sensor_ptr->data_poll_ts = 0;
              ddf_sensor_ptr->sensor_type_state = SENSORTYPE_STATE_IDLE;
              sns_smgr_sample_depot_clear(ddf_sensor_ptr->depot_ptr, 0);
              SNS_SMGR_PRINTF2(MED, "sensor_event clear depot - sensor id =%u data_type=%u",
                               sensor_ptr->const_ptr->sensor_id, ddf_sensor_ptr->data_type );
              continue;
            }
          }
        }
      }

      switch ( sensor_ptr->sensor_state )
      {
        case SENSOR_STATE_OFF:
          smgr_process_sensor_event_internal(sensor_ptr, SENSOR_EVENT_POWERING_UP);
          break;
        case SENSOR_STATE_POWERING_UP:
          sns_smgr_reset_sensor(sensor_ptr);
          if ( sensor_ptr->sensor_state != SENSOR_STATE_IDLE )
          {
            break;
          }
          /* falling through */
        case SENSOR_STATE_IDLE:
          if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B) )
          {
            smgr_process_sensor_event_internal(sensor_ptr, SENSOR_EVENT_WAKE_UP);

          }
          break;
        case SENSOR_STATE_CONFIGURING:
        case SENSOR_STATE_READY:
          if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_ADD_B) ||
               SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ITEM_DEL_B) )
          {
            smgr_process_sensor_event_internal(sensor_ptr, SENSOR_EVENT_CONFIG_FILTER);
          }
          if ( sensor_ptr->event_done_tick == 0 )
          {
            smgr_process_sensor_event_internal(sensor_ptr, SENSOR_EVENT_CONFIG_FILTER_DONE);

            SNS_SMGR_FIFO_DBG1( MED, "FIFO SENSOR_STATE_CONFIGURING/READY sensor=%d",
                                sensor_ptr->const_ptr->sensor_id );
            sns_smgr_fifo_configure(sensor_ptr);

            sns_smgr_adjust_latency_node();
          }
          break;

        case SENSOR_STATE_PRE_INIT:
        case SENSOR_STATE_FAILED:
        case SENSOR_STATE_TESTING:
          /* should never be here */
          break;
      }
      SNS_SMGR_PRINTF3(MED, "sensor_event - sensor=%d state=%d tick=%u",
                       SMGR_SENSOR_ID(sensor_ptr),
                       sensor_ptr->sensor_state, sensor_ptr->event_done_tick);
    }
  }

  for ( i=0; i<ARR_SIZE(sns_smgr.sensor); i++ )
  {
    smgr_sensor_s* sensor_ptr = &sns_smgr.sensor[i];
    if ( sensor_ptr->event_done_tick != 0 )
    {
      SNS_SMGR_PRINTF3(LOW, "sensor_event - sensor=%d state=%d tick=%u",
                       SMGR_SENSOR_ID(sensor_ptr), 
                       sensor_ptr->sensor_state, sensor_ptr->event_done_tick);
      event_count++;
      next_tick_offset = MIN(next_tick_offset, (sensor_ptr->event_done_tick -
                                                sns_smgr.last_tick.u.low_tick));
    }
  }

  SNS_SMGR_PRINTF3(MED, "sensor_event - count=%d next_tick_offset=%d(0x%x)", 
                   event_count, next_tick_offset, next_tick_offset);
  if ( next_tick_offset != SMGR_MAX_TICKS )
  {
    sns_smgr_schedule_sensor_event(next_tick_offset);
  }
  /* check for md_update in case when MD wasn't enabled because of Gyro ODR wasn't 0
   * at the time of the gyro client left in a combo device driver. Note that ODR is set by sensor_event
   * not by delete or cancle request */
  sns_smgr_check_rpts_for_md_update();
}

/*===========================================================================

  FUNCTION:   smgr_get_scheduled_sampling_time

===========================================================================*/
/*!
  @brief  Given a DDF sensor, returns its next scheduled sampling time.

  @param[i] ddf_sensor_ptr - the DDF sensor

  @return
   scheduled sampling time, or 0 if not scheduled

*/
/*=========================================================================*/
static sns_ddf_time_t smgr_get_scheduled_sampling_time(
  const smgr_ddf_sensor_s* ddf_sensor_ptr
)
{
  sns_ddf_time_t timestamp = 0;
  sns_q_s* q_ptr = SMGR_QUE_HEADER_FROM_LINK(&ddf_sensor_ptr->sched_link);
  if ( q_ptr != NULL )
  {
    smgr_sched_block_s* sched_ptr =
      SMGR_GET_ENTRY(q_ptr, smgr_sched_block_s, ddf_sensor_queue);
    timestamp = sched_ptr->next_tick;
  }
  return timestamp;
}

/*===========================================================================

  FUNCTION:   smgr_get_data

===========================================================================*/
/*!
  @brief issue driver get_data(), and save the sampled data into a saving area.

  @detail

  @param[i] ddf_sensor_ptr - the sensor from which to sample data

  @return
    TRUE the sample data is availble
    FALSE the sample data is not availble yet

*/
/*=========================================================================*/
void smgr_get_data(smgr_ddf_sensor_s* in_ddf_sensor_ptr)
{
  smgr_sensor_s*        sensor_ptr = in_ddf_sensor_ptr->sensor_ptr;
  sns_ddf_sensor_e      ddf_sensor_type[SMGR_MAX_DATA_TYPES_PER_DEVICE];
  uint8_t               i, num_data_types = 0;
  sns_ddf_status_e      driver_status = SNS_DDF_EFAIL;
  sns_ddf_sensor_data_s* data_ptr;

  ddf_sensor_type[num_data_types++] =
    SMGR_SENSOR_TYPE(sensor_ptr, in_ddf_sensor_ptr->data_type);

  for ( i=0; i<ARR_SIZE(ddf_sensor_type) && num_data_types<ARR_SIZE(ddf_sensor_type); i++ )
  {
    if ( (i != in_ddf_sensor_ptr->data_type) &&
         (sensor_ptr->ddf_sensor_ptr[i] != NULL) &&
         (smgr_get_scheduled_sampling_time(sensor_ptr->ddf_sensor_ptr[i]) -
          sns_smgr.sched_tick.u.low_tick <= SMGR_SCHEDULE_GRP_TOGETHER_TICK) )
    {
      ddf_sensor_type[num_data_types++] = SMGR_SENSOR_TYPE(sensor_ptr, i);
      SNS_SMGR_SAMPLING_DBG2(MED, "get_data - ddf_s0=%d ddf_s1=%d",
                             ddf_sensor_type[0], ddf_sensor_type[1]);
    }
  }

  if ( sensor_ptr->const_ptr->flags & SNS_REG_SSI_FLAG_LTCY_ENABLE )
  {
    sns_latency.polling_get_data_start_ts = sns_em_get_timestamp();
  }

  sns_ddf_memhandler_init( &(sensor_ptr->memhandler) );
  sns_profiling_log_qdss(SNS_SMGR_DD_ENTER_GET_DATA, 1, sensor_ptr->const_ptr->sensor_id);
  driver_status = SMGR_DRV_FN_PTR(sensor_ptr)->get_data(sensor_ptr->dd_handle,
                                                        ddf_sensor_type,
                                                        num_data_types,
                                                        &sensor_ptr->memhandler,
                                                        &data_ptr);
  sns_profiling_log_qdss(SNS_SMGR_DD_EXIT, 1, SNS_QDSS_DD_GET_DATA);
  SNS_SMGR_SAMPLING_DBG3(LOW, "get_data - sensor=%d types=%d status=%d",
                         SMGR_SENSOR_ID(sensor_ptr), num_data_types, driver_status);

  for ( i=0;  i<num_data_types; i++ )
  {
    smgr_ddf_sensor_s* ddf_sensor_ptr =
      sns_smgr_match_ddf_sensor(sensor_ptr, ddf_sensor_type[i]);
    if ( ddf_sensor_ptr != NULL )
    {
      ddf_sensor_ptr->data_poll_ts = sns_smgr.sched_tick.u.low_tick;
      if ( driver_status == SNS_DDF_SUCCESS )
      {
        /* if latency measurement enabled, run latency measurement */
        if ( sensor_ptr->const_ptr->flags & SNS_REG_SSI_FLAG_LTCY_ENABLE )
        {
          sns_latency.polling_get_data_end_ts = data_ptr[i].timestamp;
          sns_profiling_log_latency_poll(sns_latency, data_ptr[i].sensor);
        }
        sns_smgr_handle_new_sample(ddf_sensor_ptr, &data_ptr[i]);
      }
      else if ( driver_status == SNS_DDF_PENDING )
      {
        ddf_sensor_ptr->sensor_type_state = SENSORTYPE_STATE_READ_PENDING;
      }
      else
      {
        sns_ddf_sensor_data_s  data;
        SNS_SMGR_PRINTF2(ERROR, "get_data - ddf_sensor=%d drv_stat=%d",
                         ddf_sensor_type[i], driver_status);
        SNS_OS_MEMZERO(&data, sizeof(data));
        data.status = SNS_DDF_EINVALID_DATA;
        data.timestamp = sns_em_get_timestamp();
        sns_smgr_handle_new_sample(ddf_sensor_ptr, &data);
      }
    }
  }
  sns_ddf_memhandler_free(&sensor_ptr->memhandler);
}

/*===========================================================================

  FUNCTION:   sns_smgr_proc_sampling_schedule

===========================================================================*/
/*!
  @brief  traverse the report schedule queue and make samples when its the time to sample.
          After sampling, save the result into a last sample saving area.
          Also, re-schdule the next time to sample.

  @detail Data gathering cycle is flagged for execution within sampling_cycle. Data gathering cycle calls
          the functions that occur after reading data: GenerateReports and etc.

  @param[i] NONE

  @return
    The next schedule timetick offset from now

*/
/*=========================================================================*/
static uint32_t sns_smgr_proc_sampling_schedule(void)
{
  smgr_sched_block_s  *sched_blk_ptr;
  uint32_t             next_tick_offset = SMGR_MAX_TICKS;

  /* Disable Standalone PC */
  sns_hw_power_npa_vote_latency(SNS_SMGR_NPA_CLIENT_ASYNC, 1);
  sns_hw_set_qup_clk(true);

  /* Traverse all the schedule block */
  SMGR_FOR_EACH_Q_ITEM(&sns_smgr.sched_que, sched_blk_ptr, sched_link)
  {
    uint32_t delta_time = sns_smgr.sched_tick.u.low_tick +
      SMGR_SCHEDULE_GRP_TOGETHER_TICK - sched_blk_ptr->next_tick;
    uint8_t num_skipped_samples = delta_time / sched_blk_ptr->sched_intvl;

    SNS_SMGR_SAMPLING_DBG3(
      LOW, "sampling_sched - rate=%d sched_tick=%u delta=%d",
      sched_blk_ptr->sampling_rate, sns_smgr.sched_tick.u.low_tick, delta_time);

    if ( delta_time <= SMGR_MAX_TICKS )
    {
      smgr_ddf_sensor_s* ddf_sensor_ptr;

      /* it's time to read the data */
      SMGR_FOR_EACH_Q_ITEM(&sched_blk_ptr->ddf_sensor_queue, ddf_sensor_ptr, sched_link)
      {
        smgr_sensor_s *sensor_ptr = ddf_sensor_ptr->sensor_ptr;
        if ( SENSOR_STATE_READY == sensor_ptr->sensor_state)
        {
          SNS_SMGR_SAMPLING_DBG3(
            MED, "sampling_sched - sensor_state=%d type_state=%d poll_ts=%u",
            sensor_ptr->sensor_state,
            ddf_sensor_ptr->sensor_type_state, ddf_sensor_ptr->data_poll_ts);
          if ( SENSORTYPE_STATE_READ_PENDING == ddf_sensor_ptr->sensor_type_state )
          {
            SNS_SMGR_SAMPLING_DBG0(LOW, "sampling_sched - pending");
          }
          else if ( TICK1_GTR_TICK2(sns_smgr.sched_tick.u.low_tick,
                                    ddf_sensor_ptr->data_poll_ts) )
          {
            smgr_get_data(ddf_sensor_ptr);
          }
        }
        else
        {
          sns_ddf_sensor_data_s data;
          data.status = SNS_DDF_EINVALID_DATA;
          data.timestamp = sns_em_get_timestamp();
          data.samples = NULL;
          SNS_SMGR_SAMPLING_DBG2(
            HIGH, "sampling_sched - ddf_sensor %d not ready ts=%u",
            SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                             ddf_sensor_ptr->data_type), data.timestamp);
          sns_smgr_handle_new_sample(ddf_sensor_ptr, &data);
        }
        ddf_sensor_ptr->max_skipped_samples =
          MAX(ddf_sensor_ptr->max_skipped_samples, num_skipped_samples);
        if ( num_skipped_samples > 0 )
        {
          SNS_SMGR_PRINTF3(ERROR, "sampling_sched - ddf_sensor=%d skipped=%d max=%d",
                           SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr,
                                            ddf_sensor_ptr->data_type),
                           num_skipped_samples, ddf_sensor_ptr->max_skipped_samples);
        }
      } /* for each sensor type in schedule block */

      sched_blk_ptr->next_tick =
        (uint32_t)((sns_smgr.sched_tick.tick64 + SMGR_SCHEDULE_GRP_TOGETHER_TICK +
                    sched_blk_ptr->sched_intvl) /
                   sched_blk_ptr->sched_intvl * sched_blk_ptr->sched_intvl);

    } /* ( sns_smgr.sched_tick.u.low_tick + SMGR_SCHEDULE_GRP_TOGETHER_TICK..*/
    next_tick_offset = MIN(next_tick_offset,
                           sched_blk_ptr->next_tick - sns_smgr.sched_tick.u.low_tick);
  } /* for each schedule block */

  sns_hw_set_qup_clk(false);
  /* Enable Standalone Power Collapse */
  sns_hw_power_npa_vote_latency(SNS_SMGR_NPA_CLIENT_ASYNC, 0);
  return next_tick_offset;
}

/*===========================================================================

  FUNCTION:   sns_smgr_plan_sampling_now

===========================================================================*/
/*!
  @brief schedule the plan cycle now

  @detail set the schedule time by now. The time is adjusted SMGR_SCHEDULE_GRP_TOGETHER_TICK
          to avoid to sample in advance because of group together margin

  @param[i]
   NONE

  @return
   NONE
*/
/*=========================================================================*/
void sns_smgr_plan_sampling_now(void)
{
  uint8_t os_err;
  /* cancel the scheded timer which might be running */
  //sns_em_cancel_timer(sns_smgr.tmr_obj);
  sns_smgr.sched_tick.tick64 =
    sns_smgr.last_tick.tick64 - SMGR_SCHEDULE_GRP_TOGETHER_TICK;

  sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_SENSOR_READ_FLAG, OS_FLAG_SET, &os_err);
  SNS_ASSERT (os_err == OS_ERR_NONE);
}

/*===========================================================================

  FUNCTION:   sns_smgr_sampling_cycle

===========================================================================*/
/*!
  @brief  plans the the next schedule time and processing each sechedule which are:
          - event_schedule
          - cic_schedule
          - report_schedule

  @param[i] NONE

  @return
    NONE

*/
/*=========================================================================*/
void sns_smgr_sampling_cycle(void)
{
  uint32_t next_tick_offset;
  uint32_t time_now = sns_em_get_timestamp();
  if ( ( time_now + SMGR_TOO_SHORT_TICKS_FOR_TIMER -
         sns_smgr.sched_tick.u.low_tick ) > SMGR_MAX_TICKS )
  {
    SNS_SMGR_PRINTF2(LOW, "sampling_cycle - sched=%u now=%u",
                     sns_smgr.sched_tick.u.low_tick, time_now);
    return;
  }

  /* Process sampling schedule */
  next_tick_offset = sns_smgr_proc_sampling_schedule();

  if ( next_tick_offset < SMGR_MAX_TICKS )
  {
    uint32_t    tick_offset_for_timer;
    smgr_tick_s smgr_now_tick;

    sns_smgr.sched_tick.tick64 += next_tick_offset;
    smgr_now_tick.tick64 = sns_smgr_get_tick64();
    /* If the scheduling time was passed already, set it by the current time */
    sns_smgr.sched_tick.tick64 = MAX(sns_smgr.sched_tick.tick64, smgr_now_tick.tick64);

    tick_offset_for_timer =  sns_smgr.sched_tick.u.low_tick - smgr_now_tick.u.low_tick;
    if ( (tick_offset_for_timer - SMGR_TOO_SHORT_TICKS_FOR_TIMER) > SMGR_MAX_TICKS )
    {
      uint8_t os_err;
      sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_SENSOR_READ_FLAG, OS_FLAG_SET,
                       &os_err);
      SNS_ASSERT(os_err == OS_ERR_NONE);
    }
    else
    {
      sns_em_cancel_timer(sns_smgr.tmr_obj);
      if ( SNS_ERR_FAILED ==
           sns_em_register_timer(sns_smgr.tmr_obj, tick_offset_for_timer) )
      {
        SNS_SMGR_PRINTF0(ERROR, "sns_smgr_sampling_cycle - register_timer failed");
      }
    }
  }
}

/*******************************************
 CIC FILTER OBJECT AND FUNCTIONS
********************************************/

/*===========================================================================

  FUNCTION:   sns_smgr_cic_init

===========================================================================*/
/*!
  @brief Initializes CIC filter object

  @param[io] cic_obj_ptr: CIC filter state structure
  @param[i] factor: Decimation factor, must be >0, >= 2 for practical uses
  @param[i] freq: frequency of input data
  @param[i] axis_cnt: Number of axes (example 3 for XYZ)

  @return
    void
*/
/*=========================================================================*/
void sns_smgr_cic_init(smgr_cic_obj_s* cic_obj_ptr,
                       uint16_t factor,
                       uint32_t freq,
                       uint8_t num_axes)
{
  SNS_OS_MEMZERO(cic_obj_ptr, sizeof(smgr_cic_obj_s));
  cic_obj_ptr->factor          = factor;
  cic_obj_ptr->input_frequency = freq;
  cic_obj_ptr->axis_cnt        = num_axes;
}


/*===========================================================================

  FUNCTION:   sns_smgr_cic_reset

===========================================================================*/
/*!
  @brief Reset CIC filter state

  @detail
  - Prepare a filter for its first input
  - Zero accumulator and count, store the decimation factor and axis_cnt.
  - axis_cnt <= SMGR_MAX_VALUES_PER_DATA_TYPE. May be truncated without warning

  @param[update] cic_obj: Ptr to the CIC filter state structure
  @param[in] factor: Decimation factor, must be >0, >= 2 for practical uses
  @param[in] axis_cnt: Number of axes (example 3 for XYZ)

  @return
    void
*/
/*=========================================================================*/
void sns_smgr_cic_reset(smgr_cic_obj_s* cic_obj_ptr)
{
  SNS_OS_MEMZERO(cic_obj_ptr->accumulator, sizeof(cic_obj_ptr->accumulator));
  cic_obj_ptr->status = SNS_DDF_SUCCESS;
  cic_obj_ptr->count  = 0;
}

/*===========================================================================

  FUNCTION:   sns_smgr_cic_update

===========================================================================*/
/*!
  @brief Update CIC filter

  @detail
  - Accepts input data, outputs filtered data every <factor> entry
  - Input data samples must be equally spaced in time, within small tolerance
  - Filter state must be initialized with sns_smgr_cic_init() before starting
    a series of updates. If used without init, update guards against fatal
    error, but output is meaningless.

  @param[in/out] cic_obj: Ptr to the CIC filter state structure
  @param[in] input_ptr: input sample
  @param[out] timestamp_ptr: destination for resulting timestamp
  @param[out] output: Ptr to array of filtered data

  @return
    SNS_DDF_SUCCESS or
    SNS_DDF_EINVALID_DATA - output has been populated (every <factor> entry)

    SNS_DDF_PENDING - input has updated cic_obj - no output

  $TODO:
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_cic_update(
  smgr_cic_obj_s*      cic_obj_ptr,
  const smgr_sample_s* input_ptr,
  sns_ddf_time_t*      timestamp_ptr,
  int32_t              dest_data[SNS_SMGR_SENSOR_DIMENSION_V01]
)
{
  uint32_t  i;
  sns_ddf_status_e status = SNS_DDF_PENDING;

  SNS_SMGR_SAMPLE_PROC_DBG3(
    LOW, "cic_update - ts=%u status=%d count=%d",
    input_ptr->time_stamp, input_ptr->status, cic_obj_ptr->count);

  /* Guard against uninitialized axis_cnt */
  if ( SMGR_MAX_VALUES_PER_DATA_TYPE >= cic_obj_ptr->axis_cnt )
  {
    /* Sum axes into their accumulators */
    for ( i = 0; i < cic_obj_ptr->axis_cnt ; i++ )
    {
      /* Sum input into accumulator */
      /* Must be allowed to roll over on overflow. No more than one overflow per
         decimation cycle. Expected magnitude of accel and gyro values provides
         enough headroom to allow summing of several hundred values without an
         extra overflow */
      cic_obj_ptr->accumulator[i] += input_ptr->data[i];
    }
    if ( input_ptr->status != SNS_DDF_SUCCESS )
    {
      cic_obj_ptr->status = input_ptr->status;
    }

    /* Tally count. Count can never be >factor, but prevent runaway if it
      happens. Prevent zero divide if filter is run without initialization */
    if ( (++(cic_obj_ptr->count)) >= cic_obj_ptr->factor &&
         (0 != cic_obj_ptr->factor) )
    {
      if ( cic_obj_ptr->status == SNS_DDF_SUCCESS )
      {
        /* Output the decimated sample and cycle the filter */
        for ( i=0; i<cic_obj_ptr->axis_cnt; i++)
        {
          dest_data[i] = cic_obj_ptr->accumulator[i] / (q16_t)cic_obj_ptr->factor;
        }
      }
      *timestamp_ptr = input_ptr->time_stamp -
        SMGR_ADJ_CIC_TS(cic_obj_ptr->factor, cic_obj_ptr->input_frequency);
      status = cic_obj_ptr->status;

      sns_smgr_cic_reset(cic_obj_ptr);

      SNS_SMGR_SAMPLE_PROC_DBG3(
        MED, "cic_update - timestamp=%u freq=%d factor=%d",
        *timestamp_ptr, cic_obj_ptr->input_frequency, cic_obj_ptr->factor);
    }
  }
  return status;
}

/*===========================================================================

  FUNCTION:   sns_smgr_set_attr

===========================================================================*/
 /**
     * @brief Sets a sensor attribute to a specific value.
     *
     * @param[in] drv_fn_ptr    Driver interface pointer
     * @param[in] dd_handle  Handle to a driver instance.
     * @param[in] sensor     Sensor for which this attribute is to be set. When
     *                       addressing an attribute that refers to the driver
     *                       this value is set to SNS_DDF_SENSOR__ALL.
     * @param[in] attrib     Attribute to be set.
     * @param[in] value      Value to set this attribute.
     *
     * @return Success if the value of the attribute was set properly. Otherwise
     *         a specific error code is returned.
     */
/*=========================================================================*/
sns_ddf_status_e sns_smgr_set_attr(
  const smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  boolean qup_clck_state = sns_hw_qup_clck_status();
  uint32_t before, after;

  sns_hw_set_qup_clk(true);
  before = sns_em_get_timestamp();
  if ( SENSOR_STATE_FAILED != sensor_ptr->sensor_state )
  {
    status = SMGR_DRV_FN_PTR(sensor_ptr)->set_attrib(sensor_ptr->dd_handle,
                                                     sensor,
                                                     attrib,
                                                     value);
  }
  after = sns_em_get_timestamp();
  sns_hw_set_qup_clk(qup_clck_state);

  SNS_SMGR_PRINTF3(MED, "set_attr - ddf_sensor=%d attrib=%d value=%d",
                   sensor, attrib, *((uint8_t*)value) );
  SNS_SMGR_PRINTF2(MED, "set_attr - status=%d (took %d ticks)",
                   status, after-before);

  return status;
}


/*===========================================================================

  FUNCTION:   sns_smgr_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - range and resolution info is from the device data sheet.
  @param[in]  drv_fn_ptr    Driver interface pointer
  @param[in]  dd_handle   Handle to a driver instance.
  @param[in]  sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in]  memhandler  Memory handler used to dynamically allocate
                         output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
                         to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_get_attr(
  smgr_sensor_s*         sensor_ptr,
  sns_ddf_sensor_e       sensor,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  /* get attribute doesn't need I2C access */
  if ( SENSOR_STATE_FAILED != sensor_ptr->sensor_state )
  {
    status = SMGR_DRV_FN_PTR(sensor_ptr)->get_attrib(sensor_ptr->dd_handle,
                                                     sensor,
                                                     attrib,
                                                     &sensor_ptr->memhandler,
                                                     value,
                                                     num_elems);
  }
  SNS_SMGR_PRINTF3(MED, "get_attr - ddf_sensor=%d attrib=%d status=%d",
                   sensor, attrib, status);
  return status;
}


/*===========================================================================

  FUNCTION:   sns_simulate_dri

===========================================================================*/
 /**
     * @brief Simulates Sensor device Data Ready Interrupts (DRI) for
     *        QDSP SIM Playback.
     */
/*=========================================================================*/
#ifdef SNS_QDSP_SIM
static void sns_simulate_dri(void *argPtr)
{
  uint8_t err=0;
  sns_dri_sim.sns_dri_sig_grp = sns_os_sigs_create (SNS_DRI_SIMULATE, &err);

  SMGR_DELAY_US(QDSP_PLAYBACK_SENSOR_READY_TIME_US);

  while(1) {
     sns_ddf_smgr_notify_irq(0);
     sns_os_sigs_pend(sns_dri_sim.sns_dri_sig_grp, SNS_DRI_SIMULATE,
                      OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME, 0, &err);
     if(md_int_happened)
     {
         SMGR_DELAY_US(QDSP_PLAYBACK_SENSOR_READY_TIME_US); 
     }
  }
}
#endif

/*===========================================================================

  FUNCTION:   sns_smgr_enable_sched_data

===========================================================================*/
 /**
     * @brief Begins device-scheduled sampling and enables notification via Data
     *        Ready Interrupts (DRI).
     *
     * @param[in/out] sensor_ptr The sensor of interest
     * @param[in] sensor     Primary or secondary
     * @param[in] enable     Enable or disable
     *
     * @return NS_DDF_SUCCESS if sensor was successfully configured and
     *         internal sampling has commenced or ceased. Otherwise an
     *         appropriate error code.
     */
/*=========================================================================*/
sns_ddf_status_e sns_smgr_enable_sched_data(
  smgr_sensor_s*     sensor_ptr,
  sns_ddf_sensor_e   sensor,
  boolean            enable
)
{
  sns_ddf_status_e status = SNS_DDF_EFAIL;
  boolean qup_clck_state = sns_hw_qup_clck_status();
  uint32_t before, after;

  SNS_SMGR_PRINTF1( MED, "enable_sched_data INTR is_enabled=%d",
                    sensor_ptr->is_self_sched_intr_enabled);

  sns_hw_set_qup_clk(true);
  before = sns_em_get_timestamp();
  if ( (SENSOR_STATE_FAILED != sensor_ptr->sensor_state) &&
       (NULL != SMGR_DRV_FN_PTR(sensor_ptr)->enable_sched_data) )
  {
    status = SMGR_DRV_FN_PTR(sensor_ptr)->enable_sched_data(sensor_ptr->dd_handle,
                                                            sensor,
                                                            enable);
    if ( status == SNS_DDF_SUCCESS )
    {
      sensor_ptr->is_self_sched_intr_enabled = enable;
    }

  }
  after = sns_em_get_timestamp();
  sns_hw_set_qup_clk(qup_clck_state);
  SNS_SMGR_PRINTF3(MED, "enable_sched_data - ddf_sensor=%d enable=%d stat=%d",
                   sensor, enable, status);
  SNS_SMGR_PRINTF1(MED, "enable_sched_data - took %d ticks", after-before);

#ifdef SNS_QDSP_SIM
  if(!sns_dri_sim.dri_sim_task_created)
  {
    sns_os_task_create_ext(sns_simulate_dri, NULL,
                           &sns_dri_sim.DRITask[SNS_MODULE_STK_SIZE_DRI_SIM - 1],
                           SNS_MODULE_PRI_DRI_SIM,
                           SNS_MODULE_PRI_DRI_SIM,
                           &sns_dri_sim.DRITask[0],
			               SNS_MODULE_STK_SIZE_DRI_SIM,
                           (void *)0,
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR,
                           (uint8_t *)"SNS_SIMULATE_DRI");

    sns_dri_sim.dri_sim_task_created = true;
  }
#endif
  return status;
}

/*===========================================================================

  FUNCTION:   sns_smgr_use_hp_odrs

===========================================================================*/
/*!
  @brief Checks whether only HP ODRs should be used.

  @details When there is at least one request for accurate sample timestamp at
           the sampling rate higher than lowest ODR required for accurate
           timestamp, the available ODRs come from the HP_ODR table which
           holds the few ODRs.  Else, the available ODRs come from the
           NORMAL_ODR table that is filled with all supported ODRs discovered
           at boot.
  @param[i] sensor_ptr    - the sensor of interest

  @return
   TRUE if only HP ODRs should be used.
 */
/*=========================================================================*/
static boolean sns_smgr_use_hp_odrs(smgr_sensor_s* sensor_ptr)
{
  boolean use_hp_table = false;
  uint8_t i, s;

  if ( sensor_ptr == NULL || sensor_ptr->hp_odr[0] == 0 )
  {
    return false;
  }
  for ( s=0; s<2; s++ )
  {
    if ( s == 1 && NULL != sensor_ptr )
    {
      /* second round is for sibling sensor */
      sensor_ptr = sensor_ptr->sibling_ptr;
    }
    if ( sensor_ptr != NULL )
    {
      for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr) && !use_hp_table; i++ )
      {
        if ( (sensor_ptr->ddf_sensor_ptr[i] != NULL) &&
             (sensor_ptr->ddf_sensor_ptr[i]->max_high_perf_freq_hz >
              sensor_ptr->hp_odr[0]) )
        {
          use_hp_table = true;
        }
      }
    }
  }
  return use_hp_table;
}
/*===========================================================================

  FUNCTION:   sns_smgr_get_adjacent_odrs

===========================================================================*/
/*!
  @brief Returns the two adjacent ODRs of the given frequency.

  @details If the given ODR is present, it is returned as both adjacent ODRs.

  @param[i] sensor_ptr    - the sensor of interest
  @param[i] frequency     - the base frequency
  @param[o] adjacent_odrs - array of 2 for the next lower and higher ODRs
                            relative to the given ODR

  @return
   none
 */
/*=========================================================================*/
void sns_smgr_get_adjacent_odrs(smgr_sensor_s* sensor_ptr,
                                uint32_t frequency,
                                uint32_t adjacent_odrs[2])
{
  adjacent_odrs[0] = adjacent_odrs[1] = 0;

  if (frequency > 0)
  {
    int8_t i;
    if ( sns_smgr_use_hp_odrs(sensor_ptr) )
    {
      /* Each entry of HP_ODR table hold actual ODR */
      for ( i=ARR_SIZE(sensor_ptr->hp_odr)-1; i>=0 && adjacent_odrs[0]==0; i-- )
      {
        if ( frequency >= sensor_ptr->hp_odr[i] )
        {
          adjacent_odrs[0] = sensor_ptr->hp_odr[i];
        }
      }
      for ( i=0; i<ARR_SIZE(sensor_ptr->hp_odr) && adjacent_odrs[1]==0; i++ )
      {
        if ( frequency <= sensor_ptr->hp_odr[i] )
        {
          adjacent_odrs[1] = sensor_ptr->hp_odr[i];
        }
      }
    }
    else
    {
      for ( i=0; i<2; i++ )
      {
        uint32_t j, next_odr = frequency;
        int8_t step = (i==0) ? -1 : 1;
        adjacent_odrs[i] = 0;
        while ( (adjacent_odrs[i] == 0) &&
                (next_odr >= 1) && (next_odr < sizeof(sensor_ptr->normal_odr)*8) )
        {
          /* Bit position in NORMAL_ODR table maps to available ODRs */
          j = next_odr >> 3;
          if ( sensor_ptr->normal_odr[j] & (1 << (next_odr - (j << 3))) )
          {
            adjacent_odrs[i] = next_odr;
          }
          next_odr += step;
        }
      }
    }
  }
  SNS_SMGR_PRINTF3(MED, "get_adj_odrs - lower=%d given=%d higher=%d",
                   adjacent_odrs[0], frequency, adjacent_odrs[1]);
}

/*===========================================================================

  FUNCTION:   sns_smgr_choose_odr

===========================================================================*/
/*!
  @brief Chooses the appropriate ODR to be set for the given sensor.

  @details The chosen ODR must be high enough to support the max requested
           frequency of the sensor and its sibling.  However, to save power,
           a lower ODR that is within the delta threshold can be chosen.

  @param[i] sensor_ptr - the sensor of interest

  @return the chosen ODR
 */
/*=========================================================================*/
uint32_t sns_smgr_choose_odr(smgr_sensor_s* sensor_ptr)
{
  uint8_t odr_idx = 0;
  uint32_t adjacent_odrs[2] = {0, 0};
  uint32_t odr_delta[2];
  uint32_t odr_threshold_delta;
  uint32_t max_rate;

  max_rate = smgr_get_max_requested_freq(sensor_ptr);
  if ( (sensor_ptr->num_lpf > 0) ||
       SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B) )
  {
    odr_threshold_delta = (max_rate * SMGR_ODR_THRESHOLD_DELTA_PCT)/100;

    sns_smgr_get_adjacent_odrs(sensor_ptr, max_rate, adjacent_odrs);
    odr_delta[0] = max_rate - adjacent_odrs[0];
    odr_delta[1] = adjacent_odrs[1] - max_rate;
    if ( (odr_delta[0] > odr_delta[1]) || (odr_delta[0] > odr_threshold_delta) )
    {
      odr_idx = 1;
    }
  }
  else
  {
    adjacent_odrs[odr_idx] = max_rate;
  }
  SNS_SMGR_PRINTF2(LOW, "choose_odr - sensor=%d odr=%d",
                   SMGR_SENSOR_ID(sensor_ptr), adjacent_odrs[odr_idx]);
  return adjacent_odrs[odr_idx];
}

/*===========================================================================

  FUNCTION:   sns_smgr_interpolator_update

===========================================================================*/
/*!
  @brief Shifts samples so that sample1 is older than sample2

  @param[io] int_obj_ptr - interpolator object
  @param[i]  sample_ptr - the new sample

  @return none
 */
/*=========================================================================*/
void sns_smgr_interpolator_update(smgr_interpolator_obj_s* int_obj_ptr,
                                  const smgr_sample_s* sample_ptr)
{
  if ( sample_ptr == NULL )
  {
    SNS_SMGR_PRINTF3(
      ERROR, "interp_update - ts1=%u desired=%u ts2=%u",
      int_obj_ptr->sample1_ptr ? int_obj_ptr->sample1_ptr->time_stamp : 0,
      int_obj_ptr->desired_timestamp,
      int_obj_ptr->sample2_ptr ? int_obj_ptr->sample2_ptr->time_stamp : 0);
  }
  else if ( (int_obj_ptr->sample2_ptr == NULL) ||
            TICK1_GTR_TICK2(sample_ptr->time_stamp,
                            int_obj_ptr->sample2_ptr->time_stamp) )
  {
    int_obj_ptr->sample1_ptr  = int_obj_ptr->sample2_ptr;
    int_obj_ptr->sample2_ptr  = sample_ptr;

    SNS_SMGR_INTERP_DBG3(LOW, "interp_update - sample1=0x%x ts=%u sample_ts=%u",
                         int_obj_ptr->sample1_ptr, int_obj_ptr->desired_timestamp,
                         sample_ptr->time_stamp);

    if ( (int_obj_ptr->sample1_ptr != NULL) && (int_obj_ptr->interval_ticks != 0) )
    {
      SNS_SMGR_INTERP_DBG3(MED, "interp_update - ts1=%u ts=%u ts2=%u",
                           int_obj_ptr->sample1_ptr->time_stamp,
                           int_obj_ptr->desired_timestamp,
                           int_obj_ptr->sample2_ptr->time_stamp);

      while ( TICK1_GTR_TICK2(int_obj_ptr->sample1_ptr->time_stamp,
                              int_obj_ptr->desired_timestamp) )
      {
        SMGR_UPDATE_INTERP_TIMESTAMP(int_obj_ptr);
        SNS_SMGR_INTERP_DBG3(LOW, "interp_update - ts1=%u ts=%u ts2=%u",
                             int_obj_ptr->sample1_ptr->time_stamp,
                             int_obj_ptr->desired_timestamp,
                             int_obj_ptr->sample2_ptr->time_stamp);
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_interpolate

===========================================================================*/
/*!
  @brief Interpolates a new sample between two known samples.

  @details Upon entering this function desired_timestamp field of the
  given interpolator object would hold the desired timestamp for the
  resulting sample.  Successful interpolation would result in a valid sample
  saved in the given destination.

  @param[i]  smgr_interpolator_obj_s - object holding data needed for interpolation
  @param[io] dest_data - destination for the resulting sample

  @return
  SNS_DDF_SUCCESS        - if a new sample was created
  SNS_DDF_EINVALID_PARAM - if one or more of the input pointers are NULL
  SNS_DDF_EINVALID_DATA  - if the desired timestamp is not between the timestamps
                           of given samples
*/
/*=========================================================================*/
sns_ddf_status_e sns_smgr_interpolate(smgr_interpolator_obj_s* int_obj_ptr,
  int32_t dest_sample[SNS_SMGR_SENSOR_DIMENSION_V01]
)
{
  sns_ddf_status_e err = SNS_DDF_EINVALID_PARAM;

  if ( (int_obj_ptr->sample1_ptr != NULL) &&
       (int_obj_ptr->sample2_ptr != NULL) &&
       (dest_sample != NULL) &&
       /* make sure timestamp of interpolated sample is between timestamps of
          the two known samples */
       TICK1_GEQ_TICK2(int_obj_ptr->desired_timestamp,
                       int_obj_ptr->sample1_ptr->time_stamp) &&
       TICK1_GEQ_TICK2(int_obj_ptr->sample2_ptr->time_stamp,
                       int_obj_ptr->desired_timestamp) )
  {
    SNS_SMGR_INTERP_DBG3(HIGH, "ts_smpl1(%u), ts_desired(%u), ts_smpl2(%u)",
                         int_obj_ptr->sample1_ptr->time_stamp,
                         int_obj_ptr->desired_timestamp,
                         int_obj_ptr->sample2_ptr->time_stamp);
    /* make sure timestamp of interpolated sample is between timestamps of
       the two known samples */
    if ( (int_obj_ptr->sample1_ptr->status == SNS_DDF_SUCCESS) &&
         (int_obj_ptr->sample2_ptr->status == SNS_DDF_SUCCESS))
    {
      uint8_t i;
      q16_t factor = 0;
      uint32 tsgap_smp1_smp2 = int_obj_ptr->sample2_ptr->time_stamp - int_obj_ptr->sample1_ptr->time_stamp;
      uint32 tsgap_smp1_desired = int_obj_ptr->desired_timestamp - int_obj_ptr->sample1_ptr->time_stamp;

      if (0 != tsgap_smp1_smp2)
      {
        uint32_t overflow_divisor = (tsgap_smp1_smp2 / 0x8000) + 1; /* for preventing overflow for q16 */
        /* factor is anywhere between (including) 0 and 1 */
        factor = FX_DIV_Q16(FX_FLTTOFIX_Q16(tsgap_smp1_desired / overflow_divisor),
                            FX_FLTTOFIX_Q16(tsgap_smp1_smp2 / overflow_divisor));
      }
      for ( i=0; i<SMGR_MAX_VALUES_PER_DATA_TYPE; i++)
      {
        dest_sample[i] =
          int_obj_ptr->sample1_ptr->data[i] +
          FX_MUL_Q16((int_obj_ptr->sample2_ptr->data[i]- int_obj_ptr->sample1_ptr->data[i]), factor);
      }
      err = SNS_DDF_SUCCESS;
      SNS_SMGR_INTERP_DBG3(HIGH, "factor(%d), overflow_divisor(%d), tsgap_smp1_smp2(%d)",
                           factor, (tsgap_smp1_smp2 / 0x8000) + 1, tsgap_smp1_smp2);
      SNS_SMGR_INTERP_DBG3(HIGH, "smpl1(%u), smpl2(%u), dest(%u)",
                           int_obj_ptr->sample1_ptr->data[0],
                           int_obj_ptr->sample2_ptr->data[0],
                           dest_sample[0]);
#ifdef SNS_SMGR_INTERPOLATOR_DEBUG
      if (tsgap_smp1_smp2 >= 0x8000 )
      {
        SNS_SMGR_INTERP_DBG1(HIGH, "=====>tsgap_smp1_smp2(%d) is more than 0x8000", tsgap_smp1_smp2);
      }
#endif
    }
    else
    {
      err = SNS_DDF_EINVALID_DATA;
    }
  }
  return err;
}


/* Circular buffer utilities */
/*===========================================================================

FUNCTION:   sns_smgr_c_buf_init

===========================================================================*/
/*!
  @brief Allocates and initilizes a circular buffer object

  @detail
  - This Function allocates memory for and initializes a circular buffer.

  @param[i] cbuff - pointer to circular buffer structure/handle
            sensor_ptr - pointer to the sensor for which we need object

  @return
  NONE
  @comments - For non mag sensors, no memory is allocated

 */
/*=========================================================================*/
void sns_smgr_cbuff_init(smgr_circular_buff_s   *cbuff,
                         smgr_sensor_s*     sensor_ptr)
{
  cbuff->pc_buff = NULL;
  cbuff->buff_mem = NULL;

  SNS_SMGR_PRINTF2(LOW, "Initializing CBUFF %d, sns_type %d",
                   cbuff, SMGR_SENSOR_ID(sensor_ptr));

  if (0) // No buffering for any data
  {
    uint32_t size = buffer_mem_req(SMGR_MAG_FILTER_LEN, 3); // triaxial data. 
    /* the cosntant SMGR_MAG_FILTER_LEN can be a configurable number based 
       on the sensor part being used. The 3 is IMO, OK
      */
    cbuff->pc_buff = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(buffer_type));
    cbuff->buff_mem = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, size);

    if (cbuff->pc_buff && cbuff->buff_mem)
    {
      /* Initialize the circular buffer*/
      buffer_reset(cbuff->pc_buff, SMGR_MAG_FILTER_LEN, 3, cbuff->buff_mem);
    }
    else
    {
      /* Memory allocation failes, free any resources */
      SNS_SMGR_PRINTF0(ERROR, "Could not initialize buffer");
      sns_smgr_cbuff_deinit(cbuff);
    }
  }
  else
  {
    SNS_SMGR_PRINTF0(LOW, "No buffer needed");
  }
}

/*===========================================================================

FUNCTION:   sns_smgr_c_buf_init

===========================================================================*/
/*!
  @brief Destroys a circular buffer

  @detail
  - This Function de-allocates memory and resources for a circular buffer

  @param[i] cbuff - pointer to circular buffer structure/handle

  @return
  NONE
  @comments - Pointers are set to NULL

 */
/*=========================================================================*/

void sns_smgr_cbuff_deinit(smgr_circular_buff_s   *cbuff)
{
  SNS_SMGR_PRINTF1(LOW, "Destroying CBUFF %d.", cbuff);

  /* Memory allocation failes, free any allocated memory*/
  if (cbuff->pc_buff)
  {
    SNS_OS_FREE(cbuff->pc_buff);
  }

  if (cbuff->buff_mem)
  {
    SNS_OS_FREE(cbuff->buff_mem);
  }

  cbuff->pc_buff = NULL;
  cbuff->buff_mem = NULL;
}
/*===========================================================================

FUNCTION:   sns_smgr_cbuff_update

===========================================================================*/
/*!
  @brief Updates a circular buffer with a sample

  @detail
  - This Function
      - inserts a data sample into the circular bufffer
      - replaces the input with the average of samples in the buffer

  @param[i] cbuff - pointer to circular buffer structure/handle
  @param[i/o] dest_dat - input/output data


  @return
  NONE
  @comments - We produce an output even when the circular buffer is not full.

 */
/*=========================================================================*/

bool sns_smgr_cbuff_update(smgr_circular_buff_s* cbuff, 
                           int32_t* dest_data)
{
  uint8_t num_samples;

  if (!cbuff || !dest_data)
  {
    return false;
  }
  /* else */
  if ( cbuff->pc_buff && cbuff->buff_mem )
  {
    buffer_insert(cbuff->pc_buff, dest_data);
    buffer_sum(cbuff->pc_buff, cbuff->buf_sum);
    num_samples = buffer_num_samples(cbuff->pc_buff);
    /* Change the data to an averaged version */
    if(num_samples < 1) /* Should never happen - check anyways */
    {
      SNS_SMGR_PRINTF0(MED, "No Samples in buffer ?! skip averaging !!");
      return(false);
    }
    else
    {
      dest_data[0] = cbuff->buf_sum[0]/num_samples;
      dest_data[1] = cbuff->buf_sum[1]/num_samples;
      dest_data[2] = cbuff->buf_sum[2]/num_samples;
    }
  }
  //SNS_SMGR_PRINTF3(LOW, "Buffered Data out %d, %d, %d",
  //                 dest_data[0], dest_data[1], dest_data[2]);
  return(true);
}

