#ifndef SNS_SMGR_DEFINE_H
#define SNS_SMGR_DEFINE_H
/*=============================================================================
  @file sns_smgr_define.h

  This header file contains the data definitions used within the DSPS
  Sensor Manager (SMGR)

******************************************************************************
* Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
******************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_define.h#3 $ */
/* $DateTime: 2014/08/18 11:42:01 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-08-12  sd   Added sns_smgr_compute_device_sampling_factor
  2014-05-15  sd   Added smgr_mark_reports_for_flushing_after_odr_change
  2014-05-12  MW   Make lpf_table q16
  2014-04-11  DC   Fix sns_smgr_cic_update signature
  2014-03-06  jms  Removing synchronization and enabling separate interrupt for each fifo
  2014-02-25  sd   Added new marco and constant regarding interpolation
  2014-02-13  tc   Added the Driver Access Framework and the SMGR_RESTRICTED QMI service
  2014-02-04  tc   Expanded SMGR_SENSOR_IS_x macros to support multiple sensors
  2014-01-12  jms  Merged discrete with unified fifo support
  2014-01-07  pn   Added SMGR_OK_TO_SEND()
  2013-12-18  jms  General fifo support
  2013-12-10  pn   Added connection_list to sns_smgr_s
  2013-11-26  pn   Added declaration for sns_smgr_flush_fifo()
  2013-10-16  pn   Added declaration for sns_smgr_reset_sensor()
  2013-10-09  pn   Updated smgr_cic_obj_s to support factor greater than 255
  2013-10-03  pn   Added depot iterator to report item
  2013-08-05  pk   Added support for SrcModule in buffering query request
  2013-08-21  pn   Restored sns_smgr_flush_reports_to_same_processor() signature
  2013-08-19  pn   Updated sns_smgr_flush_reports_to_same_processor()
  2013-07-04  sd   Recovered FIFO change from merging
  2013-06-25  asj  Added Circular buffer and compensation matrix for reports
  2013-06-23  lka  Added function prototypes for SSI.
  2013-06-21  pn   Changed hp_odr type to sns_ddf_odr_t
  2013-06-18  pn   Added support for back-to-back Buffering indications
  2013-06-12  pn   Removed unused flags field from smgr_ddf_sensor_s
  2013-06-11  ae   Added QDSP SIM playback support
  2013-06-06  pn   Keeps track of AP state change
  2013-06-02  pn   Added support for back-to-back Buffering indications
  2013-05-22  lka  Re-factored code and defined new function prototype for MD.
  2013-05-16  asr  Removed unused log types.
  2013-05-05  asr  Added support for FIFO.
  2013-05-01  lka  Fixed ODR table sizes.
  2013-04-25  pn   Fixed timing issues occured when sample quality transitioned
                   between interpolated and/or filtered and current sample
  2013-04-17  dc   Defined a bit to indicate that the HW init is complete.
  2013-04-11  pn   Improved timestamp comparison macros
  2013-04-09  pn   Changed effective_rate_hz to q16
  2013-03-29  pn   Removed device_sampling_rate from smgr_ddf_sensor_s
  2013-03-11  vh   Optimized QMI callback functions
  2013-03-11  pn   Added boolean field periodic_like to smgr_rpt_spec_s
  2013-02-27  br   Increased MAX_REPORT_CNT definition
  2013-02-26  pk   Added support to initialize sensor dependent registry items
                   after autodetect
  2013-02-19  dc   Add proc type and notification during suspend types to SMGR
                   report structure.
  2013-02-19  jtl  Added WAITING_AUTODETECT state for initialization
  2013-02-13  pn   Sensor Status request is associated with ddf_sensor 
                   rather than with sensor
  2013-02-06  dc   Removed the SNS_SMGR_MAX_PROCS_V01 constant from IDL file.
                   Defined it locally here.
  2013-02-05  pn   Added EIS constants
  2013-01-30  pn   Simplified sensor state machine
  2013-01-09  sd   Added support for SSI
  2012-12-26  pn   Saves last sample timestamp rather last sample pointer
  2012-12-19  jhh  Add definition for requested frequency threshold
  2012-12-14  pn   Added timer for sensor event processing
  2012-12-08  pn   Added odr_change_tick field to smgr_sensor_s
  2012-12-03  pn   Added Buffering feature
  2012-10-23  vh   Processing the messages in task loop
  2012-10-19  ag   Update function prototypes
  2012-10-11  sc   Added QMI support for registry request and processing
  2012-10-03  sc   Scale timestamp back down to 32768Hz tick.
  2012-10-02  ag   Restructured to look similar to SMR based targets
  2012-09-24  sd   Removed unused code
  2012-09-14  br   Inserted macroes and max freq field in sns_type_info_s
  2012-09-13  vh   Eliminated compiler warnings
  2012-09-06  asj  Extended Sensor Status Indication
  2012-08-29  ag   Added SMGR as client of PM via QCCI; registered SMGR int service via QCSI
  2012-08-28  br   Updated SMGR_CALIBRATE
  2012-08-23  ag   Fix timestamps; handle 19.2MHz QTimer
  2012-07-29  ag   Porting from SMR to QMI
  2012-07-18  sd   Added checking divide by 0 in SMGR_LPF_DURATION
  2012-06-22  sd   optimization of code
  2012-06-20  sd   Updated SMGR_LPF_DURATION.
  2012-06-19  sd   Removed configuration time margin 2ms
  2012-06-18  br   Inserted some margin into SMGR_LPF_DURATION
  2012-06-06  sd   Added to put sensors to low power mode upon power rail is turned on
  2012-06-11  pn   Added PRINTF macros for commonly used strings.
  2012-06-05  ag   Save sensor state before sensor test starts
  2012-05-25  sd   Removed unused sensor state
                   added sns_smgr_is_odr_supported
  2012-05-12  sd   Changed SMGR_LPF_DURATION macro to support 1hz parameter
  2012-04-30  br   Added latency node support
  2012-04-22  br   Added sample count variables and IS_EVENT_SENSOR_TYPE macro
  2012-03-06  pn   Added SNS_SMGR_DEBUG macros
  2012-03-05  br   Added a few item into structures for DRI support
  2012-01-17  sc   Added parameters to handle calibration data in test info structure
  2011-11-22  br   Defined the prototype of smgr_config_filter_sister_sensor_type()
  2011-10-28  br   Rolled back inserting txn_id field smgr_header_abstract_s
  2011-10-28  br   Inserted SMGR_DELAY_US macro, and txn_id field in smgr_header_abstract_s
  2011-10-19  br   Deleted unused field which is "lpf_invert"
  2011-10-05  br   Added prototype definition of smgr_load_default_cal()
  2011-10-05  br   inserted some definitions in sns_smgr_s for MPU6050
  2011-09-29  sd   set SMGR_GYRO_MINIMUM_ODR 25 and SMGR_ACCEL_INITIAL_ODR to 400
  2011-09-06  sc   Added power rail state field in test information structure
                   so it can be restored after test
  2011-08-31  sc   Added function declaration for updating registry
  2011--8-28  sc   Added fields in test info structure to store client information
  2011-08-26  sc   Added states and structures to track sensor test information
  2011-08-23  sd   added resetting state and LD09 timer value
  2011-08-22  sc   Added prototype for generating self-test result indication msg.
  2011-08-11  br   changed names from bw to odr, and cic filter related definitions
  2011-08-10  yk   Added timestamp of last MD interrupt to support logging
  2011-07-18  br   inserted extern definition of const smgr_sensor_cfg_s
  2011-07-13  br   defined SMGR_CIC_FILTER_MAX_FACTOR
  2011-07-11  br   changed smgr_cic_info_s, smgr_cal_s, and smgr_cic_obj_s structure.
  2011-07-05  sc   Added extra parenthesis for macros to avoid potential danger
  2011-06-27  br   disabled applying scale factor in SMGR_CALIBRATE
  2011-06-24  br   inserted cal_sel field in smgr_rpt_item_s, and inserted auto_cal/full_cal field in smgr_sns_type_info_s
  2011-06-22  sd   removed a few unused function.
  2011-06-19  sd   fix replaced function sns_smgr_set_data_gathering_cycle_flag by sns_smgr_set_sampling_cycle_flag
  2011-06-17  sd   smgr code clean up
  2011-06-11  br   inserted or changed definition of SMGR_SMR_MAX_TRY_CNT, SENSOR_ITEM_STATE_PENDING_CIC, and SMGR_DEFAULT_LPF_BW
  2011-05-16  br   inserted SMGR_TOO_SHORT_TICKS_FOR_TIMER, and deleted un-used structure sns_smgr_Sol_u
  2011-05-13  br   changed the type of smgr_tick_s for build warning
  2011-05-12  br   consolidated sensor related status. i.e. smgr_sensor_state_e, SensorPowerState_e, and ActionState_e
  2011-05-11  jb   Remove unused constants
  2011-05-10  sd   added SENSOR_ITEM_STATE_SUSPENDED state
                   removed ignore_item_cnt in sns_smgr.md.
  2011-05-04  jb   deleted unused word from sensor structure
  2011-05-03  sd   Removed some functions and field in sns_smgr.md.
  2011-04-25  sd   Added HW MD int and power vote related functions and data structures
  2001-04-25  br   changed a definition: SMGR_IS_IT_IN_QUE
  2011-04-24  br   deleted unnecessary definition, changed values(ex.SMGR_SCHEDULE_US)
  2011-04-20  br   changed for the new scheduler which deployed fixed heartbeat
  2011-04-01  jb   Reenter calibration from registry
  2011-03-25  jh   Revert previous checkin
  2011-03-16  jb   Support reading data from registry
  2011-03-09  sd   added calibration structure
  2011-02-28  sd   added structure for sensor status report
  2011-02-04  jb   Fix long delay before startup
  2010-12-13  jb   Support PlanCycle revision
  2010-11-18  jb   Set LPF bandwidth
  2010-11-15  jb   Fix warning
  2010-11-12  sc   Use 32-bit value for smgr handle passed to drivers
  2010-10-15  sd   Added sns_smgr_PowerSensorOn
  2010-10-06  jb   Change data types to be compatible with drivers
..2010-10-06  sd   added cancel request support
  2010-10-05  jb   CC3 initial build
  2010-09-24  sd   Fixed  CR 256589  DSPS Hangs after processing 'report delete'
                   CR256593  DSPS Hangs when second periodic report is added
  2010-08-30  JB   Add driver installer
  2010-08-20  JB   Gather include files for all of smgr. Gather sensor type
                   definitions
  2010-08-09  JB   Add prototypes, Edit comments
  2010-08-06  JB   Complete merge
  2010-08-04  JB   Merge with revised ucos, use PC timer
  2007-07-16  JB   API changed how parameter values are defined
  2010-07-09  JB   Follow API changes, changes in support of scheduling update
  2010-06-25  JB   Changes to support new scheduling code
  2010-06-11  JB   Initial version

============================================================================*/

/* Forward declarations */
typedef struct smgr_sensor_s smgr_sensor_s;
typedef struct smgr_rpt_spec_s smgr_rpt_spec_s;
typedef struct smgr_rpt_item_s smgr_rpt_item_s;
typedef struct smgr_ddf_sensor_s smgr_ddf_sensor_s;
typedef struct smgr_query_s smgr_query_s;


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <sns_smgr_api_v01.h>             /* Auto-generated by IDL */
#include <sns_smgr_internal_api_v01.h>    /* Auto-generated by IDL */
#include <sns_smgr_restricted_api_v01.h>  /* Auto-generated by IDL */
#include <sns_smgr.h>
#include "sns_smgr_sensor_config.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_util.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_queue.h"
#include "sns_smgr_hw.h"
#include "sns_log_types.h"
#include "circular_buffer.h"


#include "sns_smgr_fifo.h"

#include <qmi_csi.h>
#include <qmi_client.h>

#define SMGR_DYNAMIC_LPF_ON
#define SMGR_CIC_FILTER_ON
#define SMGR_CIC_FILTER_MAX_FACTOR        20
#define SMGR_MAG_FILTER_LEN               7

#define SNS_SMGR_MAX_PROCS 5
/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Nominal microseconds per tick of the scheduling clock. It is understood
     that the clock can drift because of clock crystal variation. This
     may look like a FLT number, but the compiler uses it in a constant
     expression which is then cast to an integer.
*/
#define SNS_SMGR_USEC_PER_TICK            30.51
#define SNS_SMGR_POWERRAIL_HYSTERESIS_TIMER_VALUE_TICK              30510

/* Number of report items buffered in a SOL block. This should be the number
     that will fit without enlarging the union size */
#define SNS_SMGR_NUM_BUF_ITEMS_PER_BLOCK  2

  /* Characterize dynamic range control */
  /* Up Threshold, fraction of nominal range limit. If magnitude of any axis
     > UP_THRESH, go to next higher range */
#define SMGR_RANGE_UP_THRESH              0.9
  /* Down Threshold, fraction of nominal range limit. If magnitude of any axis
     < DOWN_THRESH for a time duration, go to lower range */
#define SMGR_RANGE_DOWN_THRESH            0.6

/* Define threshold flags. Send report only if one or more items with threshold
   test have true result */
#define SNS_SMGR_THRESHOLD_NULL           0
#define SNS_SMGR_THRESHOLD_DO_TEST        0x01  /* 0= no threshold test */
#define SNS_SMGR_THRESHOLD_OUTSIDE        0x02  /* 0= true if values are within
                                                 min/max range, 2= true if
                                                 values are outside min/max
                                                 range */
#define SNS_SMGR_THRESHOLD_DELTA_SAMP     0x04  /* 0= test this sample, 4= test
                                                 delta from last report */
#define SNS_SMGR_THRESHOLD_ALL_AXES       0x08  /* 0= true if any axis is true,
                                                 8= true if all axes are true */
#define SNS_SMGR_TIMER_QUEUE_SIZE   SNS_SMGR_NUM_SENSORS_DEFINED + 1
#define SNS_SMGR_IRQ_QUEUE_SIZE     SNS_SMGR_NUM_SENSORS_DEFINED

/* Interval and tolerance for 1 Hz are reductions of expressions found in
   SmgrRateToIntvl(). 1E6 = USEC/SEC */
#define INTVL_1HZ     (uint32_t)(1E6 / SNS_SMGR_USEC_PER_TICK)
#define INTVL_TOL_1HZ (uint32_t)(INTVL_1HZ / 3)

/* reg_request_flags */
#define REG_REQ_DRIVER                    0x01
#define REG_REQ_CAL_PRI                   0x02

#define SMGR_SMR_MAX_TRY_CNT              100
#define SNS_SMGR_MAX_REPORT_CNT           21
#define SMGR_MAX_ITEMS_PER_SENSOR         10

#define SMGR_SCHEDULE_GRP_TOGETHER_US     2000
#define SMGR_SCHEDULE_GRP_TOGETHER_TICK (sns_em_convert_usec_to_dspstick(SMGR_SCHEDULE_GRP_TOGETHER_US))

#define SMGR_TOO_SHORT_US_FOR_TIMER       100
#define SMGR_TOO_SHORT_TICKS_FOR_TIMER    (sns_em_convert_usec_to_dspstick(SMGR_TOO_SHORT_US_FOR_TIMER))

#define SMGR_ALIGN_FREQ                   600
#define SMGR_ODR_THRESHOLD_DELTA_PCT      20
#define SMGR_INTERPLTN_RANGE_LOW_PCT      10
#define SMGR_INTERPLTN_RANGE_HIGH_PCT     5
#define SMGR_MIN_EIS_SAMPLING_RATE        100
#define SMGR_MAX_EIS_QUERY_SAMPLES        50
#define SMGR_MAX_EIS_QUERY_REQUESTS       2
#define MAX_ODR_TB_SIZE                   64
#define MAX_HP_ODR_TB_SIZE                3

#define SMGR_GET_ENTRY(ptr, type, member) \
  ((type *)((unsigned long)(ptr)-((unsigned long)(&((type *)0x10000)->member) - 0x10000)))

#define SMGR_CALIBRATE_BIAS_AND_GAIN(in, factor, bias) (FX_MUL_Q16(in,factor)+(bias))

#define SMGR_ITEM_SENSITIVITY_DEFAULT     50

/* loops through singly-linked list of report item associated with each ddf sensor */
#define SMGR_FOR_EACH_ASSOC_ITEM(ddf_sensor_ptr,ip,ipp) \
  for ( ipp=&(ddf_sensor_ptr)->rpt_item_ptr; *ipp!=NULL && ((ip)=*ipp)!=FALSE; \
        ipp=&(*ipp)->next_item_ptr )

/* sns_q macros */
#define SMGR_QUE_HEADER_FROM_LINK(link_ptr) ((NULL==(link_ptr))? NULL:((link_ptr)->q_ptr))
#define SMGR_QUE_SELF_FROM_LINK(link_ptr)   ((NULL==(link_ptr))?NULL:(((sns_q_link_s*)(link_ptr))->self_ptr))
#define SMGR_NEXT_Q_ITEM(q,ip,l) ((ip)=SMGR_QUE_SELF_FROM_LINK(sns_q_next((q),&((ip)->l))))
#define SMGR_FOR_EACH_Q_ITEM(q,ip,l) for((ip)=sns_q_check(q); (ip)!=NULL; SMGR_NEXT_Q_ITEM(q,ip,l))

#define SMGR_MAX_TICKS                      0x7fffffff
#define SMGR_SENSOR_CLK_ACCURACY            (1.1)
#define SMGR_LPF_DURATION(lpf_odr)          \
  ((lpf_odr==0)?0:sns_em_convert_usec_to_dspstick((1000000*SMGR_SENSOR_CLK_ACCURACY+SMGR_SCHEDULE_GRP_TOGETHER_US)/(lpf_odr+4)))

#define SMGR_DELAY_US(delay_usec)           sns_ddf_delay(delay_usec)

#define SMGR_ADJ_CIC_TS(factor, freq) (sns_em_convert_usec_to_localtick((1000000/freq)*(factor-1)/factor/2))

/* TRUE if t1 is not in the past of t2 */
#define TICK1_GEQ_TICK2(t1,t2) (((t1) == (t2)) || ((t2==0) || (SMGR_MAX_TICKS >= ((t1) - (t2)))))

/* TRUE if t1 is in future of t2 */
#define TICK1_GTR_TICK2(t1,t2) (((t1) != (t2)) && ((t2==0) || (SMGR_MAX_TICKS >= ((t1) - (t2)))))

#ifndef SNS_SMGR_DEBUG
#define SMGR_UPDATE_INTERP_TIMESTAMP(iobj) \
  ( (iobj)->desired_timestamp = \
      ((iobj)->interval_ticks != 0) ? \
      (((iobj)->desired_timestamp + (iobj)->interval_ticks)/ (iobj)->interval_ticks*(iobj)->interval_ticks) : \
      (iobj)->desired_timestamp )
#else
#define SMGR_UPDATE_INTERP_TIMESTAMP(iobj) \
do { \
  ( (iobj)->desired_timestamp = \
      ((iobj)->interval_ticks != 0) ? \
      (((iobj)->desired_timestamp + (iobj)->interval_ticks)/ (iobj)->interval_ticks*(iobj)->interval_ticks) : \
      (iobj)->desired_timestamp ); \
   SNS_SMGR_PRINTF2(LOW, "desiredTS = %d, obj=0x%x", (iobj)->desired_timestamp, (iobj)); \
} while ( 0 );
#endif

#define SMGR_OK_TO_SEND(r) (((r)->proc_type != SNS_PROC_APPS_V01) || \
                            sns_smgr.app_is_awake || \
                            (r)->send_ind_during_suspend)

#define SMGR_DRV_FN_PTR(s) ((s)->const_ptr->drv_fn_ptr)
#define SMGR_SENSOR_ID(s) ((s)->const_ptr->sensor_id)
#define SMGR_SENSOR_IS_ACCEL(s) ( (SNS_SMGR_ID_ACCEL_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_2_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_3_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_4_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_ACCEL_5_V01 == SMGR_SENSOR_ID(s)) )
#define SMGR_SENSOR_IS_GYRO(s)  ( (SNS_SMGR_ID_GYRO_V01  == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_2_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_3_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_4_V01 == SMGR_SENSOR_ID(s)) || \
                                  (SNS_SMGR_ID_GYRO_5_V01 == SMGR_SENSOR_ID(s)) )
#define SMGR_SENSOR_IS_MAG(s)  (SNS_SMGR_ID_MAG_V01  == SMGR_SENSOR_ID(s))
#define SMGR_SENSOR_IS_SELF_SCHED(s) ((s)->const_ptr->flags & SNS_SMGR_DD_SELF_SCHED)
#define SMGR_SENSOR_FIFO_IS_ENABLE(s) ((s)->const_ptr->flags & SNS_SMGR_ENABLE_FIFO) 
#define SMGR_SENSOR_TYPE(s,i) ((s)->const_ptr->data_types[i])
#define SMGR_SENSOR_TYPE_PRIMARY(s) ((s)->const_ptr->data_types[SNS_SMGR_DATA_TYPE_PRIMARY_V01])
#define SMGR_SENSOR_TYPE_SECONDARY(s) ((s)->const_ptr->data_types[SNS_SMGR_DATA_TYPE_SECONDARY_V01])
#define SMGR_SENSOR_FIFO_IS_SUPPORTED(s) (((s)->fifo_cfg).is_fifo_supported)

#define SMGR_HANDLE_VALID(h) \
  (((h)>=&sns_smgr.sensor[0]) && ((h)<=&sns_smgr.sensor[ARR_SIZE(sns_smgr.sensor)-1]))

#define SMGR_DATATYPE_VALID(d) \
  ((d==SNS_SMGR_DATA_TYPE_PRIMARY_V01) || (d==SNS_SMGR_DATA_TYPE_SECONDARY_V01))

#define SMGR_MSG_ALLOC(t) ((t*)sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, sizeof(t)))

#define SMGR_SENSOR_INTERRUPT_ENABLED(sensor_ptr) ((sensor_ptr)->const_ptr->first_gpio != 0xffff)
#define SMGR_LATENCY_NODE_THRESH_ODR         50
#define SMGR_LATENCY_NODE_THRESH_ODR_TS_ACCU 10
#define SMGR_LATENCY_NODE_US                200
#define SMGR_MSG_HISTORY_MAX_COUNT           16
#define SMGR_MSG_HISTORY_BYTES2SAVE          32

#define SNS_SMGR_DEBUG0(level, msg)              \
   SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR, (msg))
#define SNS_SMGR_DEBUG1(level, msg, p1)          \
   SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR, (msg), p1)
#define SNS_SMGR_DEBUG2(level, msg, p1, p2)      \
   SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR, (msg), p1, p2)
#define SNS_SMGR_DEBUG3(level, msg, p1, p2, p3)  \
   SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR, (msg), p1, p2, p3)

#define SNS_SMGR_PRINTF0(level,msg)              SMGR_MSG_0(DBG_##level##_PRIO, msg)
#define SNS_SMGR_PRINTF1(level,msg,p1)           SMGR_MSG_1(DBG_##level##_PRIO, msg, p1)
#define SNS_SMGR_PRINTF2(level,msg,p1,p2)        SMGR_MSG_2(DBG_##level##_PRIO, msg, p1, p2)
#define SNS_SMGR_PRINTF3(level,msg,p1,p2,p3)     SMGR_MSG_3(DBG_##level##_PRIO, msg, p1, p2, p3)
#define SNS_SMGR_PRINTF4(level,msg,p1,p2,p3, p4) SMGR_MSG_4(DBG_##level##_PRIO, msg, p1, p2, p3, p4)

/** This macro's value is TRUE if the bit specified by b is set in flags f. */
#define SMGR_BIT_TEST(f,b)   (((f) & (b)) == (b))

/* check if ( a/b >(1-r/100) */
#define SMGR_LOW_RANGE_TEST(a,b,r)  ( (b*(100-r))<=a*100 )
/* check if ( a/b < (1+r/100) */
#define SMGR_HIGH_RANGE_TEST(a,b,r) ( a*100 <=(b*(100+r)) )

/** This macro's value is TRUE if the bit specified by b is not set in flags f. */
#define SMGR_BIT_CLEAR_TEST(f, b)  (((f) & (b)) == 0)

/** This macro sets the bit specified by b in flags f. */
#define SMGR_BIT_SET(f,b)    ((f) |= (b))

/** This macro clears the bit specified by b in flags f. */
#define SMGR_BIT_CLEAR(f,b)  ((f) &= ~(b))

/* bits assigned to sns_smgr_s.flags */
#define SMGR_FLAGS_MESSAGE_B                  0x01
#define SMGR_FLAGS_DD_INIT_B                  0x02
#define SMGR_FLAGS_DATA_GATHERING_B           0x04
#define SMGR_FLAGS_DATA_SAMPLING_B            0x08
#define SMGR_FLAGS_DD_SERVICE_B               0x10
#define SMGR_FLAGS_SENSOR_EVENT_B             0x20
#define SMGR_FLAGS_HW_INIT_B                  0x40

/* bits assigned to smgr_sensor_s.flags */
#define SMGR_SENSOR_FLAGS_REG_REQ_DRIVER_B    0x01
#define SMGR_SENSOR_FLAGS_REG_REQ_CAL_PRI_B   0x02
#define SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B     0x04
#define SMGR_SENSOR_FLAGS_ITEM_ADD_B          0x08
#define SMGR_SENSOR_FLAGS_ITEM_DEL_B          0x10
#define SMGR_SENSOR_FLAGS_RATE_CHANGED_B      0x20

/* bits assigned to smgr_rpt_item_s.flags */
#define SMGR_RPT_ITEM_FLAGS_DECIM_FILTER_B    0x01
#define SMGR_RPT_ITEM_FLAGS_ACCURATE_TS_B     0x02
#define SMGR_RPT_ITEM_FLAGS_DRI_SENSOR_B      0x04
#define SMGR_RPT_ITEM_FLAGS_SAMPLES_SENT_B    0x08

#define FIRST_PC_THRESHOLD       66
#define SECOND_PC_THRESHOLD      57
#define THIRD_PC_THRESHOLD       2
#define FOURTH_PC_THRESHOLD      1


// The macro generates compile-time error if cond is 'false', otherwise no code
// is generated. Assumes cond can be evaluated at compile time.
#define SMGR_ASSERT_AT_COMPILE(cond) ( (void) sizeof(char[1 - 2*!!!(cond)]) )

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/*
 * SMGR QMI message type
*/
typedef enum
{
  SNS_SMGR_MSG_INTERNAL,
  SNS_SMGR_MSG_EXTERNAL,
  SNS_SMGR_MSG_RESTRICTED
} sns_qmi_msgr_msg_type_e;

/*
 * SMGR time sturcture for scheduling
*/
typedef struct smgr_tick32s_s
{
  uint32_t low_tick;
  uint32_t high_tick;
} smgr_tick32s_s;

typedef union    smgr_tick_s
{
  uint64_t tick64;
  smgr_tick32s_s u;
} smgr_tick_s;


/* Reason codes for substituting a default or deleting an item.
   The response message includes a list of reason
   codes paired with the item # of the request that was affected. Item #0
   is the request body. When an item is deleted, it does not appear in the
   report. If all items are deleted, the request response is NAK */
typedef uint8_t    smgr_error_reason_e;
#if 0 /* Now defined in api.h file */
{
  SNS_SMGR_REASON_NULL            = 0,
  SNS_SMGR_REASON_DEFAULT_RATE    = 10,   /* Rate out of range */
  SNS_SMGR_REASON_DEFAULT_TYPE    = 11,   /* Type set to Engineering Units */
  SNS_SMGR_REASON_DEFAULT_DECIM   = 12,   /* Decimation set to Latest Sample */
  SNS_SMGR_REASON_DEFAULT_STIVTY  = 13,
  SNS_SMGR_REASON_DEFAULT_FINAL   = 14,

  SNS_SMGR_REASON_UNKNOWN_SENSOR  = 15,   /* Item deleted */
  SNS_SMGR_REASON_FAILED_SENSOR   = 16,   /* Item deleted */
  SNS_SMGR_REASON_OTHER_FAILURE   = 17,   /* Item deleted */
  SNS_SMGR_REASON_FINAL
} smgr_error_reason_e;
#endif

/* Reasons for canceling a report */
typedef uint8_t    smgr_cance_reason_e;
#if 0 /* Now defined in api.h file */
{
  SNS_SMGR_REPORT_OK                  = 0,  /* Client requested delete or
                                             closed or shut down  */
  SNS_SMGR_REPORT_CANCEL_RESOURCE            = 1,  /* Lack table space */
  SNS_SMGR_REPORT_CANCEL_FAILURE             = 2,  /* All req sensors have failed */
  SNS_SMGR_REPORT_CANCEL_SHUT_DOWN           = 3   /* Server shut down */
} smgr_cance_reason_e;
#endif

/* The Decimation word specifies how to reduce oversampled data.
   Report most recent sample (Default)
   Average samples since previous report
   Filter at half the reporting rate or next lower available frequency
*/
typedef uint8_t smgr_decimation_e;
#if 0 /* Now defined in api.h file */
{
  SNS_SMGR_DECIMATION_RECENT_SAMPLE             = 1,
  SNS_SMGR_DECIMATION_AVERAGE                   = 2,
  SNS_SMGR_DECIMATION_FILTER                    = 3
} smgr_decimation_e;
#endif

/*  =============== Enumerations for sensor report message ===============
*/


/* Define sensor identifier.
*/
#if 0 /* Now defined in api.h file */
{
  SNS_SMGR_ID_ACCEL       = 0,    /* Bosch BMA150 */
  SNS_SMGR_ID_ACCEL_2     = 1,
  SNS_SMGR_ID_ACCEL_3     = 2,
  SNS_SMGR_ID_ACCEL_4     = 3,
  SNS_SMGR_ID_ACCEL_5     = 4,

  SNS_SMGR_ID_GYRO        = 10,   /* MPU3050 */
  SNS_SMGR_ID_GYRO_2      = 11,
  SNS_SMGR_ID_GYRO_3      = 12,
  SNS_SMGR_ID_GYRO_4      = 13,
  SNS_SMGR_ID_GYRO_5      = 14,

  SNS_SMGR_ID_MAG         = 20,   /* AK8973 */

  SNS_SMGR_ID_PRESSURE    = 30,   /* Bosch BMP085 */

  SNS_SMGR_ID_PROX_LIGHT  = 40,   /* ILS29011 */
} smgr_sensor_id_e;
#endif

/* Select the sensor data type that should be reported from the sensor device.
   Primary types are, for example, acceleration, pressure, proximity, etc.
   Secondary types are temperature, ambient light, etc. The definition may be
   expanded in the future for devices that sense more than two types of data.
*/
typedef uint8_t  smgr_sensor_data_type_e;
#define SNS_SMGR_DATA_TYPE_INVALID 0xFF
#if 0 /* Now defined in api.h file */
{
  SNS_SMGR_DATA_TYPE_PRIMARY     =0,
  SNS_SMGR_DATA_TYPE_SECONDARY   =1    /* Available from Accel, Gyro,
                                           Pressure */
} smgr_sensor_data_type_e;
#endif

/* Select the option for how reports will be generated when the unit is
   stationary (not moving). This is a power saving feature. The goal is to
   suspend sampling and/or reporting while the unit is at rest.

   If all items of a report vote for NO_REPORT, that report is suspended
   until motion resumes. However, if any item of this report votes to
   continue reporting, this item is effectively promoted to REPORT_PRIOR.

   REPORT_PRIOR votes to continue generating the report, but suspend sampling
   the sensor named by this item, that is, keep reporting the last available
   sample. However, if some other report votes to keep this sensor sampling,
   then the prior sample continues to be updated.

   REPORT_FULL votes to continue sampling this sensor and generating this
   report.

   REPORT_INTERIM means sample and report in the interlocutory period when
   hardware has detected motion but SMD algorithm has not confirmed it.
*/
typedef uint8_t   smgr_stationary_option_e;
#if 0 /* Now defined in api.h file */
{
  SNS_SMGR_REST_OPTION_NO_REPORT      =0,
  SNS_SMGR_REST_OPTION_REPORT_PRIOR   =1,
  SNS_SMGR_REST_OPTION_REPORT_FULL    =2,
  SNS_SMGR_REST_OPTION_REPORT_INTERIM =3
} smgr_stationary_option_e;
#endif

/* End of enums related to API */


typedef enum
{
  SENSOR_STATE_PRE_INIT,
  SENSOR_STATE_FAILED,      /*1 No success over N successive sample attempts */
  SENSOR_STATE_OFF,         /*2 Power off state because of POWER RAIL off */
  SENSOR_STATE_POWERING_UP, /*3 Power rail coming up */
  SENSOR_STATE_IDLE,        /*4 Low power mode or sleep mode */
  SENSOR_STATE_CONFIGURING, /*5 Being configured */
  SENSOR_STATE_READY,       /*6 Active, ready to command */
  SENSOR_STATE_TESTING      /*7 In test mode */
} smgr_sensor_state_e;

/* Sensor events */
typedef enum
{
  SENSOR_EVENT_POWERING_UP,
  SENSOR_EVENT_WAKE_UP,
  SENSOR_EVENT_CONFIG_FILTER,
  SENSOR_EVENT_CONFIG_FILTER_DONE,
  SENSOR_EVENT_NO_SAMPLE
} smgr_sensor_event_e;


/* State of overall device driver initialization */
typedef enum
{
  SENSOR_ALL_INIT_NOT_STARTED,
  SENSOR_ALL_INIT_WAITING_CFG,
  SENSOR_ALL_INIT_WAITING_AUTODETECT,
  SENSOR_ALL_INIT_AUTODETECT_DONE,
  SENSOR_ALL_INIT_CONFIGURED,
  SENSOR_ALL_INIT_IN_PROGRESS,
  SENSOR_ALL_INIT_DONE
} smgr_sensor_all_init_state_e;

/* State of initialization for a device driver */
typedef enum
{
  SENSOR_INIT_NOT_STARTED,
  SENSOR_INIT_WAITING_REG,
  SENSOR_INIT_REG_READY,
  SENSOR_INIT_WAITING_TIMER,
  SENSOR_INIT_FAIL,
  SENSOR_INIT_SUCCESS
} smgr_sensor_init_state_e;

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/

/*  ********** Message Header Abstract **********
    Message header data needed for processing a sensor service request
*/
typedef struct
{
  uint8_t   src_module;
  uint8_t   ext_clnt_id;
  uint8_t   priority;
  uint8_t   txn_id;
  uint16_t  msg_id;
  void*     connection_handle;
} smgr_header_abstract_s;


/*  =============== Standing Operating List ===============
    Contains data needed to manage all active reports.
    Organized as a union of structures that are linked together to fully
    define a report. A SOL entry for single or periodic report contains one
    ReportSpec linked to as many ReportItem structures as requested for the
    report.
*/


typedef enum
{
  SENSOR_ITEM_STATE_LINKING,         /* being added */
  SENSOR_ITEM_STATE_UNLINKING,       /* being removed */
  SENSOR_ITEM_STATE_IDLE,           /* after initialization or after send the report */
  SENSOR_ITEM_STATE_PENDING,
  SENSOR_ITEM_STATE_DONE,           /* sample available */
} smgr_item_status_e;



/*  =============== Buffering support structures =============== */
typedef struct
{
  q16_t              data[SNS_SMGR_SENSOR_DIMENSION_V01];
  sns_ddf_time_t     time_stamp;
  sns_ddf_status_e   status;
} smgr_sample_s;

typedef struct
{
  uint16_t           max_bufs;
  uint16_t           used_bufs;
  uint16_t           first_idx;  /* index to oldest sample */
  uint16_t           last_idx;   /* index to latest sample */
  uint8_t            num_axes;   /* depending on sensor type */
  smgr_sample_s      samples[1]; /* many to be saved */
} smgr_sample_depot_s;

typedef struct
{
  const smgr_sample_depot_s* depot_ptr;
  uint16_t                   idx;
} smgr_sensor_depot_iter_s;

/*  =============== Schedule structure ===============
    Contains scheduling parameters. One for each requested reporting
    rate, and one for each sensor for sensor-specific scheduling data.
    Report and sensor leader blocks point here for scheduling information.
    This structure is used for deconflicting reporting rates. Deconfliction
    happens when a new rate is introduced to the SOL mix or a rate becomes
    disused or we need to modify the maximum sampling rate.

    When a report is added or has its rate modified it needs to link to one
    of these structures, adding a new structure if introducing a new rate.
    Each active sensor needs a structure here, too. The structures must be
    added when the need is recognized, although they will not be initialized
    until later. If too few blocks are available, the new request must be
    rejected. Excess blocks will be returned when deconfliction happens.
*/
typedef struct  smgr_sched_block_s
{
  /* Chain schedule structures together in order: heartbeat then
     shortest to longest reporting/sampling intervals */
  sns_q_link_s                        sched_link;

  /* Maintain all sensor types belonging to this schedule block */
  sns_q_s                             ddf_sensor_queue;

  /* Requested rate */
  uint16_t                            sampling_rate;

  /* The curreent scheduling interval by clock ticks. This tick is calculated by req_rate */
  uint32_t                            sched_intvl;

  /* Current assigned interval, scheduling clock ticks. Zero if not
     initialized */
  uint32_t                            next_tick;
} smgr_sched_block_s;

/* Filter object */
typedef struct
{
  q16_t             accumulator[SMGR_MAX_VALUES_PER_DATA_TYPE];
  uint16_t          input_frequency;
  uint16_t          count;
  uint16_t          factor;
  uint8_t           axis_cnt;
  sns_ddf_status_e  status;
} smgr_cic_obj_s;

typedef struct
{
  /* interpolator object can produce new sample with desired_timestamp 
     when sample1's timestamp <= desired_timestamp <= sample2's timestamp */
  const smgr_sample_s* sample1_ptr; /* known sample */
  const smgr_sample_s* sample2_ptr; /* known sample */
  sns_ddf_time_t       desired_timestamp; 
  uint32_t             interval_ticks; /* interval computed from desired frequency */
} smgr_interpolator_obj_s;

typedef struct
{
  buffer_type                   *pc_buff;          /* Circular Buffer pointer */
  void                          *buff_mem;        /* Circular buffer memory  */
  int32_t                        buf_sum[3];      /* Sum of elements in buffer*/
} smgr_circular_buff_s;


/*  =============== Query structure ===============
    Contains data to define and manage an outstanding Buffering Query.
    A client adds a Buffering report with report rate of 0Hz then follows that
    by a series of Buffering Query requests.  Each Buffering Query request
    will have one corresponding response and one corresponding indication.
*/
struct smgr_query_s
{
  smgr_query_s*                 next_query_ptr;
  smgr_rpt_item_s*              item_ptr; /* necessary? */
  smgr_header_abstract_s        header_abstract;
  uint16_t                      query_id;
  uint32_t                      T0;
  uint32_t                      T1;
  boolean                       ready;
};

/*  =============== ReportSpec structure ===============
    Contains data to define and manage a report. A client defines, modifies
    or deletes a report using the Report Request Message. A valid add or
    modify request is accepted if enough memory space is available.
*/
typedef enum
{
  SMGR_RPT_STATE_ACTIVE,      /* default */
  SMGR_RPT_STATE_FLUSHING,    /* pending unscheduled report due to ODR(s) change */
  SMGR_RPT_STATE_INACTIVE     /* due to MD */
} smgr_rpt_state_e;

typedef union
{
  sns_smgr_periodic_report_ind_msg_v01  periodic;
  sns_smgr_buffering_ind_msg_v01        buffering;
  sns_smgr_buffering_query_ind_msg_v01  query;
} smgr_report_indication_s;

struct smgr_rpt_spec_s
{
  /* Link to report queue */ 
  sns_q_link_s                  rpt_link;

  /* Reports ready to be sent are added to singly-linked list */
  smgr_rpt_spec_s*              next_ready_report_ptr;

  /* list of items belonging to this report */
  smgr_rpt_item_s*              item_list[SNS_SMGR_MAX_ITEMS_PER_REPORT_V01];
  uint8_t                       num_items;

  /* Information from request message header */
  smgr_header_abstract_s        header_abstract;

  smgr_rpt_state_e              state;
  
  /* Report ID is defined by the client to distinguish the client's
     various reports, 0-255 */
  uint8_t                       rpt_id;
  
  q16_t                         q16_rpt_rate;
  uint32_t                      rpt_interval; /* time ticks between indications */
  uint32_t                      rpt_tick; /* time tick at which to send indication */
  uint32_t                      min_sampling_interval;

  /* Counts number of indications sent for this report ID.
     For informational only as count can rollover */
  uint32_t                      ind_count;

  uint8_t                       num_dri_sensors;
#ifdef SMGR_BUFFERED_RPT_ON
  uint8_t                       buff_factor;
#endif


  /* processor on which this report originated */
  uint32_t                      proc_type; 
  /* if indications should be sent for this report 
     when proc_type is in suspend state*/
  boolean                       send_ind_during_suspend; 
  boolean                       periodic_like; /* when report rate == all sampling rates */
};

/*  =============== ReportItem structure ===============
    Contains data to define each sensor item in a report. Accumulates
    the averages for this item.
*/
struct smgr_rpt_item_s
{
  smgr_rpt_item_s*              next_item_ptr;
  smgr_ddf_sensor_s*            ddf_sensor_ptr;

  /* the report to which this item belongs */
  smgr_rpt_spec_s*              parent_report_ptr;

  /* list of outstanding Buffering Query */
  smgr_query_s*                 query_ptr;

  /* the status such as if the sensor value have been read or pending */
  smgr_item_status_e            state;

  /* See definitions for SMGR_RPT_ITEM_FLAGS_... */
  uint8_t                       flags;

  uint8_t                       quality;

  /* sampling rate in Hz */
  uint16_t                      sampling_rate_hz;
  uint32_t                      sampling_interval;
  uint8_t                       sampling_factor;
  uint8_t                       sampling_count;
  uint16_t                      num_samples; /* number of samples expected for sampling_interval */
  q16_t                         effective_rate_hz;
  uint32_t                      last_processed_sample_timestamp;

  /* keeps track of timestamp of latest sample in previous indication to
     prevent sending old samples */
  sns_ddf_time_t                ts_last_sent;

  uint8_t                       sensitivity;     /* This item's sensor sensitivity parameter.
                                                    =0 for default or 1-100 for lowest to highest sensitivity */
  smgr_cic_obj_s                cic_obj;
  smgr_interpolator_obj_s       interpolator_obj;
  smgr_sample_s                 resulting_sample;

  smgr_circular_buff_s          cbuff_obj;
  smgr_sensor_depot_iter_s      depot_iter;
  uint8_t                       cal_sel;  /* see define SNS_SMGR_CAL_SEL_XXX */

};     
     
  /*  =============== smgr_dynamic_range_limits_s ===============
    Data for each range
  */
typedef struct
{
  /* Up threshold, some fraction of nominal range limit, Q16, standard units
     for this sensor. If sample magnitude > thresh, switch to next higher range. */
  int32_t                           up_thresh;
  /* Down threshold. If sample magnitude < thresh, count delay time */
  int32_t                           down_thresh;
  /* Delay time, ticks. If samples have been below down_thresh for a defined
   time, switch to this range */
  uint32_t                          delay_time;
} smgr_dynamic_range_limits_s;

  /*  =============== smgr_dynamic_range_control_s ===============
    Manage data for dynamic range
  */
typedef struct
{
  /* Point to range data ordered by increasing enum value. TODO Use malloc space,
     number based on device driver init */
  smgr_dynamic_range_limits_s       ranges[SMGR_MAX_RANGES_IN_SENSOR];
  /* Set to index of next to highest range, that is, Num-2 */
  uint8_t                           num_ranges;
  /* Value -1 signals go to range specified by commanded min sensitivity */
  int8_t                            range_now;
} smgr_dynamic_range_control_s;


 /*  =============== Sensor Status structure ===============
  	Contains VARIABLE parameters to manage sensor status for a sensor
  	assuming one outstanding report a time
*/

  /* Structure to keep track of any clients of the STATUS of this sensor */
typedef struct
{
  boolean  used;                /* indicate if there is an outstanding request */ 
  uint8_t  last_status;         /* the last status reported to client */
  uint8_t  pending_status;      /* current status yet to be reported to client */
  uint8_t  dst_module;          /* Destination module ID within the Sensors framework */
  uint8_t  ext_clnt_id;         /* External client ID provided by a SMR client */
  uint8_t  txn_id;              /* SMR client provided transaction ID */
  uint8_t  req_data_type_num;   /* number of requested items dropped to before SMGR send one client indication */
  void*    connection_handle;   /* QCCI Handle: NULL if on SMR */
} smgr_status_client_info_s;

typedef struct
{
  smgr_ddf_sensor_s* ddf_sensor_ptr;

  /* Clients interested in the status of this sensor - NOT the clients of the
     sensor itself. Note that we are supporting only one client per processor
     This is expected to be the SCM on that processor.
  */
  smgr_status_client_info_s status_clients[SNS_SMGR_MAX_PROCS];

  uint8_t  num_requested_items; /* Keep track of how many requests for this sensor */
  uint8_t  num_requested_apps;  /* Number of clients on the apps processor */
  uint8_t  num_requested_dsps;  /* Number of clients on the dsps */
                                /* We do not support any Modem clients currently */
  uint32_t time_stamp;          /* when the status is changed*/
} smgr_sensor_status_s;

/* Only valid when smgr_sensor_state_e is SENSOR_STATE_READY */
typedef enum
{
  SENSORTYPE_STATE_IDLE,
  SENSORTYPE_STATE_READ_PENDING,
} smgr_sensor_type_state_e;

/*  =============== Sensor Calibration structure ===============
  	Contains VARIABLE parameters to manage sensor status for a sensor
  	assuming one outstanding report a time
*/
typedef struct
{
  boolean   used;           /* indicate if calibration is applicable */
  boolean   need_to_log;    /* mark filed if the sample need to be logged when sending a response */
  uint8_t   zero_bias_len;	/* Must be set to # of elements in zero_error */
  /*
    Q16 format (16 bits for integer part, 16 bits for decimal part), indicating the zero bias that is to be added (in
    nominal engineering units); each sensor type could have up to 3 data, for example, x, y, z axis for primary datatype
    , or temperature for secondary datatype.
  */
  int32_t zero_bias[SNS_SMGR_SENSOR_DIMENSION_V01];

  uint8_t scale_factor_len;	/* Must be set to # of elements in scale_error */
  /*
    Q16 format, a multiplier that indicates scale factor need to be multiplied to current data .
    For example, enter 1.01 if the scaling is 1% less aggressive or 0.95 if it is 5% more aggressive.
  */
  uint32_t scale_factor[SNS_SMGR_SENSOR_DIMENSION_V01];

  /*  
    If the Compensation Matrix is valid, the scale_factor above is ignored. 
    The calibrated sample (Sc) is coputed as 
    Sc = CM(Sr - Bias)
    where : 
        Sc = Calibrated sensor sample
        Sr = Read sensor sample
        CM = Compensation Matrix
        Bias =

    Matrix elements are in Q16 format in row major order ie: 
    CM =  CM0  CM1  CM2
          CM3  CM4  CM5
          CM6  CM7  CM8
  */
  bool compensation_matrix_valid; /* True if CM is valid, false otherwise */
  int32_t compensation_matrix[SNS_SMGR_COMPENSATION_MATRIX_SIZE_V01];

  uint32_t calibration_accuracy;
  /* The higher the better : Default 0 - 5 */

} smgr_cal_s;




struct smgr_ddf_sensor_s
{
  /* link to sampling schedule queue */
  sns_q_link_s              sched_link;

  /* the sensor to which this DDF sensor belongs */
  smgr_sensor_s*            sensor_ptr;
  smgr_sensor_data_type_e   data_type;    /* primary or secondary */

  /* maintained for sns_smgr_sensor_status_ind_msg */
  smgr_sensor_status_s      sensor_status;

  /* singly-linked list of report items */
  smgr_rpt_item_s*          rpt_item_ptr;
  uint8_t                   num_rpt_items;

  /* calibration data */
  smgr_cal_s                factory_cal;
  smgr_cal_s                auto_cal;
  smgr_cal_s                full_cal;     /* factory cal + auto_cal */

  smgr_sensor_type_state_e  sensor_type_state;
  uint32_t                  data_poll_ts; /* timestamp when data was last polled */

  /* the ODR configured in device */
  sns_ddf_odr_t             current_odr;

  /* ratio between current ODR and max requested rate;
     only one in device_sampling_factor samples will go into the depot */
  uint8_t                   device_sampling_factor;

  /* the sampling frequency of data in depot; 
     = device_sampling_rate/device_sampling_factor 
     only greater than 1 when HP clients exist and no CIC request */
  uint16_t                  depot_data_rate;
  uint32_t                  depot_data_interval;

  /* highest requested rate amongst all high performance requests */
  uint16_t                  max_high_perf_freq_hz;

  /* highest requested rate amongst all reports */
  uint16_t                  max_requested_freq_hz;

  /* highest supported rate; provided by DD at init */
  uint16_t                  max_supported_freq_hz;


  /* count down from dri_ratio to 0 */
  uint8_t                   dri_count;

  /* keeps track of number of skipped samples */
  uint8_t                   max_skipped_samples;

  smgr_sample_depot_s*      depot_ptr;

};

/*  =============== SensorLeader structure ===============
    Contains VARIABLE parameters to manage collection from a sensor
*/
struct smgr_sensor_s 
{
  /* references the static table entry for the sensor */
  smgr_sensor_cfg_s*              const_ptr;

  /* For hardware that houses multiple sensors */
  smgr_sensor_s*                  sibling_ptr;

  /* handle to driver object */
  sns_ddf_handle_t                dd_handle;
  
  /* Registry item parameters for device driver initialization */
  sns_ddf_nv_params_s             reg_item_param;

  /* Define sensors available from this device. The driver populates this
     at initialization */
  uint8_t                         num_data_types;
  smgr_ddf_sensor_s*              ddf_sensor_ptr[SMGR_MAX_DATA_TYPES_PER_DEVICE];

  /* Use memhandler for each device in short read. Minimize time delay between
     reading one sensor and the next */
  sns_ddf_memhandler_s            memhandler;

  /* Data for dynamic range control */
  smgr_dynamic_range_control_s    range;

  /* Number of Low Pass Filters in hardware */
  uint8_t                         num_lpf;
  q16_t                           lpf_table[SMGR_MAX_LPF_FREQ_IN_SENSOR];

  /* ODRs available when there are high-performance clients;
     defaults set to 50, 100, and 200Hz */
  sns_ddf_odr_t                   hp_odr[MAX_HP_ODR_TB_SIZE];

  /* ODRs available when there are no high-performance clients;
     represented by bit positions */
  uint8_t                         normal_odr[MAX_ODR_TB_SIZE]; /* 512 bits */

  /* Initialization state */
  smgr_sensor_init_state_e        init_state;

  /* Sensor state (actually applies to a device driver) */
  smgr_sensor_state_e             sensor_state;

  /* Flags to coordinate reading registry */
  uint8_t                         flags;

  uint32_t                        event_done_tick;
  uint32_t                        odr_change_tick; /* timestamp at last ODR config */

  /* for logging purpose */
  smgr_sensor_state_e             prev_state;

  /* Sensor FIFO Configuration */
  smgr_sensor_fifo_cfg_s          fifo_cfg;

  //flag set to true/false when intr enabled/disabled in sns_smgr_enable_sched_data
  //initially set to false in sns_smgr_sensor_init
  bool                            is_self_sched_intr_enabled; //is intr enabled (is in DRI/self sched mode)?

};

/*  =============== Smgr QMD structure ===============
    Contains QMD related info for SMGR
*/

typedef struct smgr_md_client_s
{
  uint8_t               rpt_id;
  uint8_t               ext_clnt_id;
  uint8_t               src_module;
  void                  *connection_handle;
  struct smgr_md_client_s  *md_client;
}smgr_md_client_s;

typedef struct
{
  struct smgr_md_client_s *md_client;
  uint8_t               num_non_accel_items;
  uint8_t               num_accel_rpt_not_coupled_with_md;
  uint8_t               num_accel_full_rpt_from_sam;
  uint8_t               total_rpt_num;
  bool                  is_md_int_enabled;

} smgr_md_s;

typedef struct
{
  uint8_t                      sensor_id;
  uint8_t                      data_type;
  sns_smgr_test_type_e_v01     test_type;
  bool                         test_done;
  bool                         save_to_registry;
  bool                         apply_cal_now;
  smgr_power_state_type_e      powerrail_state;
  smgr_header_abstract_s       hdr_abstract;
  smgr_sensor_state_e          saved_sensor_state;
} smgr_sensor_test_s;

typedef struct
{
  uint32_t         timestamp;
  sns_smr_header_s msg_header;
  uint8_t          body[SMGR_MSG_HISTORY_BYTES2SAVE - sizeof(sns_smr_header_s)];
} smgr_msg_content_s;
typedef struct
{
  uint32_t num_rcvd_msgs;
  smgr_msg_content_s  msg_contents[SMGR_MSG_HISTORY_MAX_COUNT];
} smgr_msg_history_s;



typedef struct
{
  qmi_client_handle client_handle;
  bool is_valid; /* set to false in disconnect_cb()*/
} smgr_connection_s;

typedef struct  
{
  /* State of overall device driver initialization */
  smgr_sensor_all_init_state_e all_init_state;
  uint32_t                     init_start_tick;
  smgr_sensor_s                sensor[SNS_SMGR_NUM_SENSORS_DEFINED];
  smgr_md_s                    md;

  /* the tick value of current scheduling time */
  smgr_tick_s                  sched_tick;
  /* queue for schedule blocks */
  sns_q_s                      sched_que;
  /* queue for all reports */
  sns_q_s                      report_queue;

  /* at most one connection per report and one connection for MD client */
  smgr_connection_s            connection_list[SNS_SMGR_MAX_REPORT_CNT+1];

  /* linked list of reports ready to be sent */
  smgr_rpt_spec_s*             ready_report_ptr;
  smgr_report_indication_s     report_ind;

  /* flags used for multiple purposes */
  uint8_t                      flags;

  /* sns_smgr.last_tick is to maintain 64 bits timeticks per sns_smgr_get_tick64().
   * high_tick could be increased only if sns_smgr_get_tick64() was called. */
  smgr_tick_s                  last_tick;

  sns_em_timer_obj_t           tmr_obj;
  sns_em_timer_obj_t           sensor_ev_tmr_obj;

  bool                         app_is_awake;
  uint32_t                     app_state_toggle_count;
  uint8_t                      last_power_vote;
  uint16_t                     max_off2idle_time;

  OS_FLAG_GRP                  *sig_grp;
  OS_FLAGS                     sig_flag;
  OS_STK                       SMGRTask[SNS_MODULE_STK_SIZE_DSPS_SMGR];

  void                         *timer_que[SNS_SMGR_TIMER_QUEUE_SIZE];
  uint8_t                      timer_que_in;
  uint8_t                      timer_que_out;

  /* data structure to store sensor test information */
  smgr_sensor_test_s           sensor_test_info;
  smgr_msg_history_s           msg_history;

  /* SSI sensor dependent reg items state */
  uint16_t                     last_requested_sensor_dep_reg_group_id;
  uint16_t                     last_received_reg_group_id;
  
  /* Information about the active fifo sensors and synchronization */
  sns_smgr_fifo_s              fifo_active_info;
} sns_smgr_s;

#define SNS_DRI_SIMULATE       0x1
typedef struct
{
  OS_STK                       DRITask[SNS_MODULE_STK_SIZE_DSPS_PLAYBACK];
  uint8_t                      dri_sim_task_created;
  uint8_t                      start_irq;
  OS_FLAG_GRP                  *sns_dri_sig_grp;
} sns_dri_sim_s;

typedef sns_smr_header_s smgr_q_item_header_s;

typedef struct smgr_q_item_s
{
  sns_q_link_s                  q_link;
  smgr_q_item_header_s          header;
  uint8                         smgr_msg_type;
  void                          *body_ptr;
} smgr_q_item_s;

extern smgr_sensor_cfg_s       smgr_sensor_cfg[SNS_SMGR_NUM_SENSORS_DEFINED ];
extern sns_smgr_s              sns_smgr;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
int sns_smgr_ssi_get_cfg_idx(uint16_t id);
bool sns_smgr_ssi_is_last_cfg(uint16_t id);
int32_t sns_smgr_ssi_get_cfg_id(uint8_t idx);

void sns_smgr_internal_process_msg(sns_smr_header_s msg_header, void *item_ptr,
                                           void *body_ptr);

uint16_t sns_smgr_internal_process_md(
  sns_smr_header_s msg_header,
  void *item_ptr,
  void *body_ptr);

void sns_smgr_cancel_internal_service(void *connection_handle);

smgr_q_item_s *sns_smgr_q_item_alloc (void);

void sns_smgr_sol_init( void );

void sns_smgr_sensor_init( void );

void sns_smgr_init_max_frequency(smgr_sensor_s *sensor_ptr);

void sns_smgr_proc_periodic_req_msg( sns_smr_header_s*,
                                     sns_smgr_periodic_report_req_msg_v01*, 
                                     sns_smgr_periodic_report_resp_msg_v01* );

void sns_smgr_proc_buffering_req_msg( sns_smr_header_s*,
                                      sns_smgr_buffering_req_msg_v01*,
                                      sns_smgr_buffering_resp_msg_v01* );

void sns_smgr_proc_buffering_query_req_msg( sns_smr_header_s*,
                                            sns_smgr_buffering_query_req_msg_v01*,
                                            sns_smgr_buffering_query_resp_msg_v01* );

boolean smgr_send_empty_query_indication(smgr_query_s*);

smgr_sensor_s* sns_smgr_find_sensor( smgr_sensor_id_e id);

boolean sns_smgr_is_event_sensor(smgr_sensor_s* sensor_ptr, uint8_t data_type);

int	sns_smgr_test( void );

void sns_smgr_sampling_cycle( void );

uint64_t sns_smgr_get_tick64(void);

uint32_t sns_em_get_timestamp( void );

void sns_smgr_set_data_gathering_cycle_flag ( void );

void sns_smgr_generate_test_result( smgr_sensor_test_s *test_info,
                                    sns_smgr_test_status_e_v01 test_status,
                                    uint32_t test_err_code );

void sns_smgr_dd_init( void );

void sns_smgr_check_sensor_status(void);
void sns_smgr_generate_sensor_status_ind(smgr_sensor_status_s*, uint8_t);

uint32_t smgr_get_sched_intval(uint16_t req_freq);

void sns_smgr_delete_report(smgr_rpt_spec_s*);
void sns_smgr_deactivate_report(smgr_rpt_spec_s*);
void sns_smgr_activate_report(smgr_rpt_spec_s*);
uint32_t smgr_flush_reports(void);
void sns_smgr_flush_reports_to_same_processor(sns_proc_type_e_v01);

sns_err_code_e sns_smgr_req_reg_data( const uint16_t Id, const uint8_t Type );

sns_err_code_e sns_smgr_update_reg_data( const uint16_t Id, const uint8_t Type,
                                         uint32_t Length, uint8_t* data_p );

void sns_smgr_process_reg_data( sns_smgr_RegItemType_e Type, uint16_t Id,
                              uint32_t Length, uint8_t* data_p, sns_common_resp_s_v01 sns_resp );

void sns_smgr_reset_sensor(smgr_sensor_s*);

void sns_smgr_cbuff_init(smgr_circular_buff_s   *cbuff,
                         smgr_sensor_s*     sensor_ptr);

bool sns_smgr_cbuff_update(smgr_circular_buff_s* cbuff_obj_ptr, 
                           int32_t* input_ptr);

void sns_smgr_cbuff_deinit(smgr_circular_buff_s   *cbuff);

void sns_smgr_cic_init(smgr_cic_obj_s* cic_obj_ptr, uint16_t factor, 
                       uint32_t frequency, uint8_t num_axes);

void sns_smgr_cic_reset(smgr_cic_obj_s* cic_obj_ptr);

sns_ddf_status_e sns_smgr_cic_update(smgr_cic_obj_s* cic_obj_ptr, 
                                     const smgr_sample_s* input_ptr,
                                     sns_ddf_time_t*      timestamp_ptr,
                                     int32_t dest_data[SNS_SMGR_SENSOR_DIMENSION_V01]);

void smgr_compute_report_interval(smgr_rpt_spec_s*);

void smgr_schedule_next_report(smgr_rpt_spec_s*);

void sns_smgr_send_power_vote(uint8_t vote);

void sns_smgr_md_init(void);

void sns_smgr_check_rpts_for_md_update(void);

void sns_smgr_handle_md_int(uint32_t timestamp);

bool sns_smgr_check_accel_rpt_coupled_with_md(uint8_t rpt_id, uint8_t src_module);

void smgr_mark_reports_for_flushing_after_odr_change(smgr_ddf_sensor_s* ddf_sensor_ptr);

sns_ddf_status_e sns_smgr_set_attr
( 
  const smgr_sensor_s       *sensor_ptr,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value);

sns_ddf_status_e sns_smgr_get_attr
(
  smgr_sensor_s       *sensor_ptr,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void**               value,
  uint32_t*            num_elems
);

sns_ddf_status_e sns_smgr_enable_sched_data
(
  smgr_sensor_s*     sensor_ptr,
  sns_ddf_sensor_e   sensor,
  boolean            enable
);

void sns_smgr_send_req(sns_smr_header_s *msg_header, void *req_ptr, void *resp_ptr, qmi_client_type client_handle);

void sns_smgr_send_resp(sns_smr_header_s *msg_header, void *resp_ptr, sns_smr_header_s request_msg_header);

void sns_smgr_send_resp_w_err (uint16_t msg_id, sns_smr_header_s *msg_header_ptr, void *body_ptr, void *item_ptr);

bool sns_smgr_send_indication(void* ind_msg_ptr, uint16_t msg_id, uint16_t len, void* connection_handle);

void sns_smgr_err_handle ( sns_err_code_e ec);

smgr_rpt_spec_s* sns_smgr_find_same_rpt_in_suspend_que( smgr_header_abstract_s *Hdr_p, uint8_t ReportId );

void sns_smgr_power_init(void);

void sns_smgr_plan_sampling_now(void);

void sns_smgr_schedule_sensor_event(uint32_t tick_offset);

void smgr_process_sensor_event(void);

void sns_smgr_change_state(smgr_sensor_state_e state);

void smgr_load_default_cal( smgr_cal_s *Cal_p );

void sns_smgr_adjust_latency_node (void);

void sns_smgr_set_sensor_state( smgr_sensor_s*, smgr_sensor_state_e);

void smgr_update_max_frequencies(smgr_ddf_sensor_s* ddf_sensor_ptr);

void sns_smgr_compute_device_sampling_factor(smgr_ddf_sensor_s* ddf_sensor_ptr);

uint32_t smgr_sensor_type_max_odr( const smgr_sensor_s* sensor_ptr );

uint32_t sns_smgr_get_max_req_freq(void);

uint32_t smgr_get_max_requested_freq(smgr_sensor_s* sensor_ptr);

uint16_t sns_smgr_compute_depot_data_rate(smgr_ddf_sensor_s* ddf_sensor_ptr);

bool sns_smgr_ready_reports_list_add(smgr_rpt_spec_s* report_ptr);
void sns_smgr_ready_reports_list_remove(smgr_rpt_spec_s* report_ptr);

void sns_smgr_get_adjacent_odrs(smgr_sensor_s* sensor_ptr, 
                                uint32_t odr, 
                                uint32_t adjacent_odrs[2]);

uint32_t sns_smgr_choose_odr(smgr_sensor_s* sensor_ptr);

void sns_smgr_interpolator_update(smgr_interpolator_obj_s* int_obj_ptr,
                                  const smgr_sample_s* sample_ptr);

/* sample depot functions */
uint16_t sns_smgr_get_max_sample_count(smgr_sample_depot_s*);
uint16_t sns_smgr_get_sample_count(smgr_sample_depot_s*);
const smgr_sample_s* sns_smgr_get_latest_sample(smgr_sample_depot_s*);
const smgr_sample_s* sns_smgr_get_oldest_sample(smgr_sample_depot_s*);
const smgr_sample_s* sns_smgr_init_depot_iter(const smgr_sample_depot_s*, 
                                              boolean, 
                                              smgr_sensor_depot_iter_s*);
const smgr_sample_s* sns_smgr_init_depot_iter_ext(const smgr_sample_depot_s*,
                                                  sns_ddf_time_t,
                                                  smgr_sensor_depot_iter_s*);
const smgr_sample_s* sns_smgr_get_sample(smgr_sensor_depot_iter_s*, int8_t);

sns_ddf_status_e sns_smgr_process_sample(
  smgr_rpt_item_s*     item_ptr,
  const smgr_sample_s* sample_ptr,
  sns_ddf_time_t*      timestamp_ptr,
  int32_t dest_sample[SNS_SMGR_SENSOR_DIMENSION_V01]);

smgr_ddf_sensor_s* sns_smgr_match_ddf_sensor(
  const smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     ddf_sensor_type);

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
                                      int32_t dest_data[SNS_SMGR_SENSOR_DIMENSION_V01]);

qmi_csi_cb_error smgr_int_handle_req_cb
(
  void                     *connection_handle,
  qmi_req_handle           req_handle,
  unsigned int             msg_id,
  void                     *req_c_struct,
  unsigned int             req_c_struct_len,
  void                     *service_cookie
);

void* sns_smgr_rcv(void);

void sns_smgr_msg_free(void * body_ptr, void * item_ptr);

sns_err_code_e sns_smgr_get_hdr
(
  sns_smr_header_s * smr_header_ptr, //address of local var in process_msg
  const void *       item_ptr, //from smgr_rcv
  void **            body_ptr
);

qmi_csi_cb_error sns_smgr_q_put(
  void                         *connection_handle,
  qmi_req_handle               req_handle,
  unsigned int                 msg_id,
  void                         *buff_ptr,
  unsigned int                 buf_ptr_len,
  qmi_idl_type_of_message_type qmi_msg_type,
  sns_qmi_msgr_msg_type_e      smgr_msg_type,
  uint8_t                      svc_num
);

#endif /* #ifndef SNS_SMGR_DEFINE_H */
