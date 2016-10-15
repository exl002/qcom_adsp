/*=============================================================================
  @file smgr_init.c

  This file contains the initialization logic for the DSPS Sensor Manager (SMGR)

*******************************************************************************
*   Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
*   Qualcomm Technologies Proprietary and Confidential.
*
********************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_init.c#4 $ */
/* $DateTime: 2014/06/17 14:29:07 $ */
/* $Author: pwbldsvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-06-17  lka  Adjusted depot sizes, with added margins.
  2014-05-16  MW   Added HSPPAD038A
  2014-05-13  pk   Isolate enhanced batching to SSC_BATCH_ENHANCE switch
  2014-05-12  MW   Enhanced SNS_DDF_ATTRIB_LOWPASS handling
  2014-05-12  ad   Support enhanced batching
  2014-04-15  lka  Added support for calibration initialization in DD.
  2014-04-08  MW   Added LPS25H
  2014-03-19  sd   wait for correct off to idle time in sns_smgr_dd_init
  2014-02-28  MW   Added SENSORS_DD_DEV_FLAG for HD22
  2014-02-20  pn   Caps sensor max frequency with highest ODR in ODR list
  2014-02-13  tc   Added the Driver Access Framework 
  2014-02-04  tc   Added support for Accel 2 through 5, SAR 2, and RGB 2
  2014-01-29  MW   Updated humidity sensor support
  2013-12-18  MW   Added Hall effect support and BU52061NVX
  2013-10-11  yh   Added AK09911C  
  2013-11-29  yh   Added MC3410    
  2014-01-12  jms  Merged discrete with unified fifo support
  2013-12-18  jms  General fifo support
  2013-12-11  MW   Added MAX44006 
  2013-11-06  sd   Added BMG160 support
  2013-11-04  MW   Added LSM303D
  2013-10-31  sc   Check to ensure critical driver functions are non-NULL
  2013-10-23  MW   Added SAR support and AD7146
  2013-09-23  yh   Added AL3320B
  2013-09-09  yh   Added CM36283
  2013-09-05  yh   Added KXTIK 
  2013-10-10  jtl  Fix Autodetect support during init
  2013-10-09  RS   Added Memsic M34160PJ Mag 
  2013-09-23  pn   Votes for INACTIVE power after initializing all sensors
  2013-09-12  MW   Gesture type support, Added BH1721
  2013-09-05  rs   Added AKM09912
  2013-09-03  yh   Added ISL29044A
  2013-09-02  cj   Added ISL29147
  2013-08-09  yh   Added AP3216C
  2013-08-09  yh   Added MMA8452
  2013-07-25  yh   Added KXCJK
  2013-08-15  vh   Eliminated Klocwork warnings
  2013-08-05  sc   Added AMS TMG399X
  2013-07-31  vh   Eliminated Klocwork warnings
  2013-08-26  pn   Sensor state is set to OFF when initialization completes
  2013-08-22  pn   Defined ACCEL_MAX_BUFS
  2013-07-03  sc   Check whether sensor is valid before requesting and processing
  2013-07-31  MW   Added APDS9950 and MAX88120
  2013-07-15  dc   Support for humidity sensor type
  2013-07-12  lka  Removed spaces.
  2013-07012  asr  Generalized support for akm8963/bmp280 i2c slave devices in DMP Passthrough configuration and cleaned up code.
  2013-07-10  sc   Check whether sensor is valid before requesting and processing
                   calibration data from registry
  2013-07-04  sd   Recovered FIFO change from merging
  2013-06-28  lka  Replaced MPU6515 temporary sensor IDs. Increased sensor depot size.s
  2013-06-26  pn   Fixed forever loop caused by a deceiving device driver
  2013-06-23  lka  Enhanced SSI support: multiple groups and devices.
  2013-06-21  pn   Added support for SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST
  2013-06-19  lka  Modified driver function pointer of MPU6515 driver.
  2013-06-18  pn   Added support for back-to-back Buffering indications
  2013-06-17  lka  Commented out ODR checks. Modified SMGR depot sizes.
  2013-06-17  pf   Added LTR55x
  2013-06-15  lka  Changed OEM_03 to Step Counter (polling sensor)
  2013-06-14  sd   increased buffer size
  2013-06-11  ae   Added QDSP SIM playback support
  2013-06-05  lka  Fixed the pressure sensor registry issue.
  2013-06-02  pn   Added support for back-to-back Buffering indications
  2013-05-22  lka  Renamed MPU function pointer.
  2013-05-21  br   Inserted validity checking in process_reg_devinfo_resp()
  2013-05-05  asr  Added support for FIFO initialization.
  2013-05-02  dc   Do not use the static variable to loop over the sensors to
                   calculate the off_to_idle delays.
  2013-05-01  dc   Wait for the max of off_to_idle of all drivers before initializing
                   the drivers.
  2013-05-01  lka  Added support for LIS3DSH and enhanced DDF interface.
  2013-04-11  dc   Removed second registry read for SMGR SSI since it is already done
                   during HW init.
  2013-04-11  dc   Read GPIO information from registry and use them for hw init.
  2013-04-04  jtl  Added device_select field in SSI.
  2013-04-03  sc   Added SSI support for MPU6500 and MAXIM44009
  2013-04-01  pk   Moved SSI sensor dependent registry state data to sns_smgr_s
  2013-03-12  ps   Added APDS99XX driver functions list for 8x26
  2013-03-11  pn   Increased buffer depths for Pressure and Prox/Light sensors
  2013-02-26  pk   Added support to initialize sensor dependent registry items
                   after autodetect
  2013-02-25  ps   Added smgr_sensor_fn_ptr_map for 8x26.
  2013-02-19  jtl  Added autodetect for SSI.
  2013-02-14  ag   Changes for 16-bit register access
  2013-02-13  pn   Associates Sensor Status and ddf_sensors
  2013-01-30  br   inserted sending/processing registry message for factory calibration
  2013-01-17  sd   updated sns_smgr_init_odr_tables
  2013-01-16  sd   updated sns init flag when initialization done
  2013-01-09  sd   Added SSI support
  2013-01-04  sc   Dynamically discover accel+gyro combo sensor instead of hard-coding
  2012-12-11  pn   The second axis for Prox/Light sensors is used for raw data
  2012-12-03  pn   Added Buffering feature
  2012-11-08  sc   Merge from DSPS 1.1
  2012-10-31  br   Update priority field. Make request, responses and async indications,
                   and log and debug indications as high priority.
  2012-10-09  br   Fixed initializing the max freq of sensors
  2012-09-14  br   Initialized the max freq that sensor can support
  2012-09-06  asj  Extended Sensor Status Indication
  2012-08-15  sc   Temporarily disabled registry handling for SNS_QMI_ENABLE
  2012-07-12  sc   Added bus_instance for DD init
  2012-07-10  br   increased the retry limit definition for sending REG requeest
  2012-06-11  pn   made use of sns_smgr_set_sensor_state()
  2012-05-16  br   fixed compile warning
  2012-03-05  br   deleted unnecessary line(ex. for sensitivity_set which is obsolte)
  2012-02-07  br   changed to retry sending REG request when it failed to send it
  2011-12-06  sd   use one macro at a time.
  2011-11-16  sd   changed the start up delay from 5 seconds to 1 second
  2011-11-14  jhh  Updated alloc and free function calls to meet new API
  2011-10-19  br   Deleted lines using "lpf_invert", and fixed wrong type for RANGE attr setting.
  2011-10-08  br   Deleted processing FACTORY CAL in sns_smgr_process_reg_data() as it was moved to SCM
  2011-10-05  br   Inserted initialization lines for MPU6050 support
  2011-09-29  ag   Remove SMGR_REG_OFF and enable registry read at init
  2011-09-26  sd   Move turning power rail on before SMGR delay
  2011-09-15  sc   Update with registry service V02
  2011-09-13  sd   removed a debug message since it is not shown on qxdm during init time
  2011-09-12  sd   dont set sttribute if sensor init fails
  2011-09-07  agk  Set all sensors to LOW POWER state after initialization.
  2011-08-31  sc   Added a function to update registry data
  2011-08-11  br   Updated as LOWPASS definition was changed from uint32_t to q16_t
  2011-08-05  rb   added SMGR_REG_OFF to prevent smgr from calibrating with registry
  2011-07-11  br   updated for factory cal database when it receives factory cal registry data.
  2011-07-05  sc   Fixed compilation warnings (caused by a potential bug).
  2011-06-27  br   updated initialization value for calibration data
  2011-06-24  br   changed to support auto cal in addition to factory cal
  2011-06-17  sd   smgr code clean up
  2011-05-25  br   initialized memhandler and types info in sensor leader structure
  2011-05-19  br   Set the DD init start timer(i.e. InitStartTime) before and after the initial delay
  2011-05-16  br   defined sns_err_code_e in sns_smgr_dd_init() instead of using global variable
  2011-05-12  br   consolidated sensor related status.
                   inserted a point to the sensor constant table into sensor leader structure
  2011-05-11  jb   Add registry type for calibration
  2011-05-04  jb   moved sensors init code to this new file so init can be moved
                   to DDR memory. Includes registry request code from task.c
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_smgr_define.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_smgr_hw.h"
#include "sns_reg_api_v02.h"
#include "sns_pm_api_v01.h"
#include "sns_dd.h"
#include "sns_reg_common.h"

#ifdef ADSP_HWCONFIG_L
extern void sns_hw_int_gpio_inactive_config(uint16_t gpio_num);
#endif
/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef PACK(struct)
{
  uint8_t drvuuid[16];
  sns_ddf_driver_if_s * fun_ptr;
}smgr_sensor_config_fn_ptr_map_s;

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#if defined SNS_PCSIM || defined VIRTIO_8X26
# define  START_DELAY_TICKS   0         /* no startup delay */
#else
#define  START_DELAY_TICKS   (uint32_t)(500000/SNS_SMGR_USEC_PER_TICK)  /* 0.5 sec startup delay */
#endif

/* the retry counts for a REG request for DD initialization */
#define  SMGR_RETRY_CNT_DD_REG_REQ    20000
static const smgr_sensor_config_fn_ptr_map_s smgr_sensor_fn_ptr_map[] = {
#ifdef CONFIG_SUPPORT_QDSP_SIM_PLAYBACK
  { SNS_REG_UUID_QDSP_SIM_PLAYBACK, &sns_dd_qdsp_playback_if },
#endif
#ifdef CONFIG_SUPPORT_MPU6050
  { SNS_REG_UUID_MPU6050, &sns_dd_mpu6xxx_if },
#endif
#ifdef CONFIG_SUPPORT_MPU6500
  { SNS_REG_UUID_MPU6500, &sns_dd_mpu6xxx_if },
#endif
#ifdef CONFIG_SUPPORT_MPU6515
  { SNS_REG_UUID_MPU6515 , &sns_dd_mpu6515_if },
  { SNS_REG_UUID_MPU6515_AKM8963 , &sns_dd_mpu6515_if },
  { SNS_REG_UUID_MPU6515_BMP280 , &sns_dd_mpu6515_if },
#endif
#ifdef CONFIG_SUPPORT_ADXL
  { SNS_REG_UUID_ADXL350, &sns_accel_adxl350_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LIS3DH
  { SNS_REG_UUID_LIS3DH, &sns_dd_acc_lis3dh_if},
#endif
#ifdef CONFIG_SUPPORT_LIS3DSH
  { SNS_REG_UUID_LIS3DSH, &sns_dd_acc_lis3dsh_if},
#endif
#ifdef CONFIG_SUPPORT_BMA150
  { SNS_REG_UUID_BMA150, &sns_accel_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMA250
  { SNS_REG_UUID_BMA250, &sns_accel_bma250_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMG160
  { SNS_REG_UUID_BMG160,&sns_bmg160_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MPU3050
  { SNS_REG_UUID_MPU3050, &sns_gyro_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_L3G4200D
  { SNS_REG_UUID_L3G4200D, &sns_dd_gyr_l3g4200d_if},
#endif
#ifdef CONFIG_SUPPORT_L3GD20
  { SNS_REG_UUID_L3GD20, &sns_dd_gyr_if},
#endif
#ifdef CONFIG_SUPPORT_AKM8963
  { SNS_REG_UUID_AKM8963, &sns_mag_akm8963_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AKM8975
  { SNS_REG_UUID_AKM8975, &sns_mag_akm8975_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AKM8973
  { SNS_REG_UUID_AKM8973, 0},
#endif
#ifdef CONFIG_SUPPORT_AMI306
  { SNS_REG_UUID_AMI306, &sns_mag_ami306_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMP085
  { SNS_REG_UUID_BMP085, &sns_alt_bmp085_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BMP180
  { SNS_REG_UUID_BMP180, &sns_alt_bmp085_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LPS331AP
  { SNS_REG_UUID_LPS331AP, &sns_dd_press_lps331ap_if},
#endif
#ifdef CONFIG_SUPPORT_APDS99XX
  { SNS_REG_UUID_APDS99XX, &sns_dd_apds99xx_driver_if},
#endif
#ifdef CONFIG_SUPPORT_ISL29028
  { SNS_REG_UUID_ISL29028, &sns_alsprx_isl29028_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29147
  { SNS_REG_UUID_ISL29147, &sns_alsprx_isl29147_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29011
  { SNS_REG_UUID_LPS331AP, &sns_alsprx_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LSM303DLHC
  { SNS_REG_UUID_LSM303DLHC, &sns_dd_mag_lsm303dlhc_if},
#endif
#ifdef CONFIG_SUPPORT_YAS530
  { SNS_REG_UUID_YAS530, &sns_mag_yas_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MAX44009
  { SNS_REG_UUID_MAX44009, &sns_als_max44009_driver_if},
#endif
#ifdef CONFIG_SUPPORT_BMA2X2
  { SNS_REG_UUID_BMA2X2, &sns_accel_bma2x2_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LIS3DH
  { SNS_REG_UUID_LIS3DH, &sns_dd_acc_lis3dh_if},
#endif
#ifdef CONFIG_SUPPORT_BMA150
  { SNS_REG_UUID_BMA150, &sns_accel_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MPU3050
  { SNS_REG_UUID_MPU3050, &sns_gyro_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_HSCD008
  { SNS_REG_UUID_HSCD008, &sns_dd_mag_hscdtd_if},
#endif
#ifdef CONFIG_SUPPORT_AKM8975
  { SNS_REG_UUID_AKM8975, &sns_mag_akm8975_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_APDS99XX
  { SNS_REG_UUID_APDS99XX, &sns_dd_apds99xx_driver_if},
#endif
#ifdef CONFIG_SUPPORT_TMD277X
  { SNS_REG_UUID_TMD277X, &sns_alsprx_tmd277x_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LTR55X
  { SNS_REG_UUID_LTR55X, &sns_alsprx_ltr55x_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AL3320B
	{ SNS_REG_UUID_AL3320B, &sns_alsprx_al3320b_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29028
  { SNS_REG_UUID_ISL29028, &sns_alsprx_isl29028_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_SHTC1
  { SNS_REG_UUID_SHTC1, &sns_rht_shtc1_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_APDS9950
  { SNS_REG_UUID_APDS9950, &sns_dd_apds9950_driver_if},
#endif
#ifdef CONFIG_SUPPORT_MAX88120
  { SNS_REG_UUID_MAX88120, &sns_ges_max88120_driver_if},
#endif
#ifdef CONFIG_SUPPORT_TMG399X
  { SNS_REG_UUID_TMG399X, &sns_ams_tmg399x_alsprx_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_KXCJK
  { SNS_REG_UUID_KXCJK, &sns_accel_kxcjk_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MMA8452
  { SNS_REG_UUID_MMA8452, &sns_dd_acc_mma8452_if},
#endif
#ifdef CONFIG_SUPPORT_AP3216C
  { SNS_REG_UUID_AP3216C, &sns_alsprx_ap3216c_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_CM36283
	{ SNS_REG_UUID_CM36283, &sns_alsprx_cm36283_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_ISL29044A
	{ SNS_REG_UUID_ISL29044A, &sns_alsprx_isl29044a_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AKM09912
  {SNS_REG_UUID_AKM09912, &sns_mag_akm09912_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_BH1721
  { SNS_REG_UUID_BH1721, &sns_als_bh1721_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_M34160PJ
	{ SNS_REG_UUID_M34160PJ, &sns_mmc3xxx_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_KXTIK
  { SNS_REG_UUID_KXTIK, &sns_accel_kxtik_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_AD7146
  { SNS_REG_UUID_ADI7146, &sns_dd_sar_ad7146_fn_list},
#endif
#ifdef CONFIG_SUPPORT_LSM303D
 { SNS_REG_UUID_LSM303D, &sns_dd_lsm303d_if},
#endif
#ifdef CONFIG_SUPPORT_MAX44006
 { SNS_REG_UUID_MAX44006, &sns_als_rgb_max44006_driver_if},
#endif
#ifdef CONFIG_SUPPORT_BU52061NVX
 { SNS_REG_UUID_BU52061NVX, &sns_hall_bu52061_driver_fn_list},
#endif
#ifdef CONFIG_SUPPORT_MC3410
    { SNS_REG_UUID_MC3410, &sns_dd_acc_mc3410_if},
#endif	
#ifdef CONFIG_SUPPORT_AKM09911
	{ SNS_REG_UUID_AKM09911, &sns_mag_akm_driver_fn_list},	
#endif
#ifdef CONFIG_SUPPORT_LPS25H
	{ SNS_REG_UUID_LPS25H, &sns_dd_press_lps25h_if},	
#endif
#ifdef CONFIG_SUPPORT_HSPPAD038A
	{ SNS_REG_UUID_HSPPAD038A, &sns_dd_prs_hsppad_if},	
#endif

/* Generic configs for vendor driver development*/
#ifdef CONFIG_SUPPORT_VENDOR_1
      { SNS_REG_UUID_VENDOR_1, &sns_dd_vendor_if_1},
#endif
#ifdef CONFIG_SUPPORT_VENDOR_2
      { SNS_REG_UUID_VENDOR_2, &sns_dd_vendor_if_2},
#endif
};

#define SMGR_SENSOR_FN_PTR_MAP_TBLE_SIZE sizeof(smgr_sensor_fn_ptr_map)/sizeof(smgr_sensor_fn_ptr_map[0])

/* SSI: SMGR groups */
static const uint16_t smgr_ssi_cfg[] = {
  SNS_REG_GROUP_SSI_SMGR_CFG_V02,
  SNS_REG_GROUP_SSI_SMGR_CFG_2_V02
};

/* SSI: Device Info (auto-detect) */
static const uint16_t smgr_ssi_devinfo[] = {
  SNS_REG_GROUP_SSI_DEVINFO_ACCEL_V02,
  SNS_REG_GROUP_SSI_DEVINFO_GYRO_V02,
  SNS_REG_GROUP_SSI_DEVINFO_MAG_V02,
  SNS_REG_GROUP_SSI_DEVINFO_PROX_LIGHT_V02,
  SNS_REG_GROUP_SSI_DEVINFO_PRESSURE_V02,
  SNS_REG_GROUP_SSI_DEVINFO_TAP_V02,
  SNS_REG_GROUP_SSI_DEVINFO_HUMIDITY_V02,
  SNS_REG_GROUP_SSI_DEVINFO_RGB_V02,
  SNS_REG_GROUP_SSI_DEVINFO_SAR_V02,
  SNS_REG_GROUP_SSI_DEVINFO_HALL_EFFECT_V02
};

static uint8_t smgr_sensor_cfg_cnt = 0;

#define SNS_SMGR_SSI_GET_FIRST_DEVINFO_ID()         ( smgr_ssi_devinfo[0] )
#define SNS_SMGR_SSI_DEVINFO_IDX_TO_CFG_IDX(d_idx)  ( (d_idx)/SNS_REG_SSI_SMGR_CFG_NUM_SENSORS )
#define SNS_SMGR_SSI_IS_LAST_DEVINFO(id)    ( smgr_ssi_devinfo[ARR_SIZE(smgr_ssi_devinfo)-1] == (id) )

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef struct
{
  smgr_sensor_id_e              sensor_id;
  smgr_sensor_data_type_e       data_type;
  uint16_t                      total_bufs;
  uint8_t                       num_axes;   /* depending on sensor type */
} smgr_sensor_depot_config_s;

typedef struct
{
  uint16_t  size;
  uint16_t  line_num;
  uint8_t   extra_info;
} smgr_heap_data_stat_s;

//#define HEAP_STAT_DBG
#ifdef HEAP_STAT_DBG
static uint8_t smgr_heap_data_items = 0;
smgr_heap_data_stat_s smgr_heap_data[250];

#define HEAP_STAT(ln,sz,ei)  \
  smgr_heap_data[smgr_heap_data_items].line_num = ln; \
  smgr_heap_data[smgr_heap_data_items].size = sz; \
  smgr_heap_data[smgr_heap_data_items++].extra_info = ei;
#else
#define HEAP_STAT(ln,sz,ei)
#endif

#ifdef SSC_BATCH_ENHANCE
  #define ACCEL_MAX_BUFS 10000
#else  
  #define ACCEL_MAX_BUFS 1500
#endif

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
extern  qmi_client_type      smgr_reg_cl_user_handle;

// TODO: max bufs should be configured in the registry.
const smgr_sensor_depot_config_s smgr_sensor_depot_config[] =
{
  /* ID,                                data type,                        max bufs,       num axes */
  {SNS_SMGR_ID_ACCEL_V01,                SNS_SMGR_DATA_TYPE_PRIMARY_V01,   ACCEL_MAX_BUFS, 3},
  {SNS_SMGR_ID_ACCEL_V01,                SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_ACCEL_2_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   ACCEL_MAX_BUFS, 3},
  {SNS_SMGR_ID_ACCEL_2_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_ACCEL_3_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   ACCEL_MAX_BUFS, 3},
  {SNS_SMGR_ID_ACCEL_3_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_ACCEL_4_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   ACCEL_MAX_BUFS, 3},
  {SNS_SMGR_ID_ACCEL_4_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_ACCEL_5_V01,              SNS_SMGR_DATA_TYPE_PRIMARY_V01,   ACCEL_MAX_BUFS, 3},
  {SNS_SMGR_ID_ACCEL_5_V01,              SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_GYRO_V01,                 SNS_SMGR_DATA_TYPE_PRIMARY_V01,    2400,          3},
  {SNS_SMGR_ID_GYRO_V01,                 SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_MAG_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,    1500,          3},
  {SNS_SMGR_ID_MAG_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_PRESSURE_V01,             SNS_SMGR_DATA_TYPE_PRIMARY_V01,    1500,          1},
  {SNS_SMGR_ID_PRESSURE_V01,             SNS_SMGR_DATA_TYPE_SECONDARY_V01,  15,            1},
  {SNS_SMGR_ID_PROX_LIGHT_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,    240,           2},
  {SNS_SMGR_ID_PROX_LIGHT_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01,  240,           2},
  {SNS_SMGR_ID_IR_GESTURE_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,    5,             1},
  {SNS_SMGR_ID_IR_GESTURE_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01,  5,             1},
  {SNS_SMGR_ID_TAP_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,    5,             1},
  {SNS_SMGR_ID_TAP_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01,  5,             1},
  {SNS_SMGR_ID_HUMIDITY_V01,             SNS_SMGR_DATA_TYPE_PRIMARY_V01,    10,            1},
  {SNS_SMGR_ID_HUMIDITY_V01,             SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1},
  {SNS_SMGR_ID_RGB_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,    25,            3},
  {SNS_SMGR_ID_RGB_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01,  25,            2},
  {SNS_SMGR_ID_RGB_2_V01,                SNS_SMGR_DATA_TYPE_PRIMARY_V01,    25,            3},
  {SNS_SMGR_ID_RGB_2_V01,                SNS_SMGR_DATA_TYPE_SECONDARY_V01,  25,            2},
  {SNS_SMGR_ID_STEP_EVENT_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,    1,             1},
  {SNS_SMGR_ID_STEP_EVENT_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01,  1,             1},
  {SNS_SMGR_ID_SMD_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,    1,             1},
  {SNS_SMGR_ID_SMD_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01,  1,             1},
  {SNS_SMGR_ID_STEP_COUNT_V01,           SNS_SMGR_DATA_TYPE_PRIMARY_V01,    1,             1},
  {SNS_SMGR_ID_STEP_COUNT_V01,           SNS_SMGR_DATA_TYPE_SECONDARY_V01,  1,             1},
  {SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01, SNS_SMGR_DATA_TYPE_PRIMARY_V01,    2400,          3},
  {SNS_SMGR_ID_GAME_ROTATION_VECTOR_V01, SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            3},
  {SNS_SMGR_ID_SAR_V01,                  SNS_SMGR_DATA_TYPE_PRIMARY_V01,    25,            3},
  {SNS_SMGR_ID_SAR_V01,                  SNS_SMGR_DATA_TYPE_SECONDARY_V01,  25,            3},
  {SNS_SMGR_ID_SAR_2_V01,                SNS_SMGR_DATA_TYPE_PRIMARY_V01,    25,            3},
  {SNS_SMGR_ID_SAR_2_V01,                SNS_SMGR_DATA_TYPE_SECONDARY_V01,  25,            3},
  {SNS_SMGR_ID_HALL_EFFECT_V01,          SNS_SMGR_DATA_TYPE_PRIMARY_V01,    10,            1},
  {SNS_SMGR_ID_HALL_EFFECT_V01,          SNS_SMGR_DATA_TYPE_SECONDARY_V01,  10,            1}
};


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/*===========================================================================

  FUNCTION:   smgr_load_default_cal

===========================================================================*/
/*!
  @brief CTemporary function to load default calibration. Modify when final cal
    structure is in place
  @detail

  @param Cal_p Calibration pointer
  @return
   none
 */
/*=========================================================================*/
void smgr_load_default_cal( smgr_cal_s *Cal_p )
{
  uint32  i;

  for ( i = 0; i < SNS_SMGR_SENSOR_DIMENSION_V01; i++ )
  {
    Cal_p->zero_bias[i] = 0;
    /* 1.00000 in Q16 format */
    Cal_p->scale_factor[i] = FX_FLTTOFIX_Q16(1.0);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_sensor_init

===========================================================================*/
/*!
  @brief initialize smgr_sensor_s structure

  @Detail

  @param[i] NONE

  @return
   NONE
*/
/*=========================================================================*/
void
sns_smgr_sensor_init( void )
{
  smgr_sensor_s       *sensor_ptr;
  uint32_t            ix;

  SNS_OS_MEMZERO (sns_smgr.sensor, sizeof(sns_smgr.sensor));
  sns_smgr.all_init_state = SENSOR_ALL_INIT_NOT_STARTED;
  for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    sensor_ptr->const_ptr = &smgr_sensor_cfg[ix];
    sensor_ptr->init_state = SENSOR_INIT_NOT_STARTED;
    sensor_ptr->reg_item_param.nvitem_grp = SNS_SMGR_REG_ITEM_TYPE_NONE;
    sensor_ptr->reg_item_param.status = SNS_DDF_EFAIL;
    sensor_ptr->is_self_sched_intr_enabled = false;

    /* init the memory handler which will be used for this sensor */
    sns_ddf_memhandler_init( &(sensor_ptr->memhandler) );

  }
  sns_smgr.last_requested_sensor_dep_reg_group_id = 0xFFFF;
}

/*===========================================================================

  FUNCTION:   sns_smgr_create_sample_depot

===========================================================================*/
/*!
  @brief Allocates depot space for sensor samples

  @details Given sensor must already be successfully initialized.

  @param sensor_ptr the sensor of interest
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_create_sample_depot( smgr_sensor_s *sensor_ptr )
{
  uint8_t i;
  SNS_ASSERT_DBG( sensor_ptr->init_state == SENSOR_INIT_SUCCESS );

  for ( i=0; i<ARR_SIZE(smgr_sensor_depot_config); i++ )
  {
    const smgr_sensor_depot_config_s* cfg_ptr = &smgr_sensor_depot_config[i];
    if ( (SMGR_SENSOR_ID(sensor_ptr) == cfg_ptr->sensor_id) &&
         (sensor_ptr->const_ptr->data_types[cfg_ptr->data_type] !=
          SNS_DDF_SENSOR__NONE) )
    {
      uint32_t size = sizeof(smgr_sample_depot_s) +
                      sizeof(smgr_sample_s)*(cfg_ptr->total_bufs-1);
      smgr_sample_depot_s* depot_ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, size);
      if ( depot_ptr != NULL )
      {
        HEAP_STAT(__LINE__, size, cfg_ptr->total_bufs);
        SNS_OS_MEMZERO(depot_ptr, sizeof(smgr_sample_depot_s));
        depot_ptr->num_axes = cfg_ptr->num_axes;
        depot_ptr->max_bufs = cfg_ptr->total_bufs;
        depot_ptr->last_idx = depot_ptr->max_bufs - 1;
        sensor_ptr->ddf_sensor_ptr[cfg_ptr->data_type]->depot_ptr = depot_ptr;
      }
      else
      {
        /* no point continuing with no sample depot */
        sensor_ptr->init_state = SENSOR_INIT_FAIL;
        break;
      }
    }
  }

  if ( sensor_ptr->init_state == SENSOR_INIT_FAIL )
  {
    for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
    {
      SNS_OS_FREE(sensor_ptr->ddf_sensor_ptr[i]->depot_ptr);
      sensor_ptr->ddf_sensor_ptr[i]->depot_ptr = NULL;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_odr_supported

===========================================================================*/
/*!
  @brief Initializes odr_supported field of given sensor.

  @param sensor_ptr the sensor of interest
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_init_odr_supported(smgr_sensor_s* sensor_ptr)
{
  uint32_t len;
  void* odr_ptr;
  sns_ddf_status_e ddf_status;
  ddf_status = sns_smgr_get_attr( sensor_ptr,
                     SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                     SNS_DDF_ATTRIB_ODR,
                     &odr_ptr,
                     &len);
  if (SNS_DDF_SUCCESS == ddf_status )
  {
    SMGR_BIT_SET(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B);
    sns_ddf_memhandler_free(&sensor_ptr->memhandler);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_lpf_table

===========================================================================*/
/*!
  @brief Initializes lpf_table field of given sensor.

  @param sensor_ptr the sensor of interest
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_init_lpf_table(smgr_sensor_s* sensor_ptr)
{
  uint32_t len;
  sns_ddf_lowpass_freq_t* lpf_ptr;
  sns_ddf_status_e ddf_status;
  /* Load definition of low-pass filters if they exist for this device.
     Some device drivers use LOWPASS attribute to pass internal sampling
     rate, but the effect is to control filtering */
  sensor_ptr->num_lpf = 0;
  SNS_SMGR_PRINTF1(HIGH, "sns_smgr_init_lpf_table - range_sensor=%d", sensor_ptr->const_ptr->range_sensor);
  ddf_status = sns_smgr_get_attr( sensor_ptr,
                     SMGR_SENSOR_TYPE(sensor_ptr,
                                      sensor_ptr->const_ptr->range_sensor),
                     SNS_DDF_ATTRIB_LOWPASS,
                     (void**)&lpf_ptr,
                     &len);
  if (SNS_DDF_SUCCESS == ddf_status )
  {
    uint8_t ij;
    if ( (len <= ARR_SIZE(sensor_ptr->lpf_table)) && (len > 0) )
    {
      int16_t  delta;
      for ( ij = 0; ij < len; ij++, lpf_ptr++ )
      {
        sensor_ptr->lpf_table[ij] = *lpf_ptr;
      }
      /* Ensure table changes monotonically. Disregard portion of table
         that doesn't */
      delta = (int16_t)(sensor_ptr->lpf_table[1] - sensor_ptr->lpf_table[0]);
      for ( ij = 1; ij < len; ij++ )
      {
        int16_t delta_ij =
          (int16_t)(sensor_ptr->lpf_table[ij] - sensor_ptr->lpf_table[ij-1]);
        if ( (delta_ij * delta) <= 0 )
        {
          /* Reached a point in the table where delta is not monotonic.
             Limit length of usable table */
          break;
        }
      }
      sensor_ptr->num_lpf = (uint8_t)len;
    }
    sns_ddf_memhandler_free(&sensor_ptr->memhandler);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_range

===========================================================================*/
/*!
  @brief Initializes range field of given sensor.

  @param sensor_ptr the sensor of interest
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_init_range(smgr_sensor_s* sensor_ptr)
{
  uint32_t len;
  sns_ddf_range_s* range_ptr;
  sns_ddf_status_e ddf_status;

  sensor_ptr->range.num_ranges = 0;
  SNS_SMGR_PRINTF1(HIGH, "sns_smgr_init_range - range_sensor=%d", sensor_ptr->const_ptr->range_sensor);
  ddf_status = sns_smgr_get_attr( sensor_ptr,
                     SMGR_SENSOR_TYPE(sensor_ptr,
                                      sensor_ptr->const_ptr->range_sensor),
                     SNS_DDF_ATTRIB_RANGE,
                     (void**)&range_ptr,
                     &len);
  if (SNS_DDF_SUCCESS == ddf_status )
  {
    uint8_t ij;
    if ( (len <= SMGR_MAX_RANGES_IN_SENSOR) && (len != 0) )
    {
      for ( ij = 0; ij < len; ij++, range_ptr++ )
      {
        sensor_ptr->range.ranges[ij].up_thresh = (range_ptr->max *
                         (uint32_t)(SMGR_RANGE_UP_THRESH * 128)) >> 7;
        sensor_ptr->range.ranges[ij].down_thresh = (range_ptr->max *
                         (uint32_t)(SMGR_RANGE_DOWN_THRESH * 128)) >> 7;
      }
      /* Set to fixed range if automatic range switching is disabled */

      if ( 0 != (sensor_ptr->const_ptr->flags & SNS_SMGR_NO_SENSITIVITY) )
      {
        uint32_t range_set;
        uint32_t set_data = (uint32_t)SNS_DDF_POWERSTATE_ACTIVE;
        sns_smgr_set_attr( sensor_ptr,
                           SNS_DDF_SENSOR__ALL,
                           SNS_DDF_ATTRIB_POWER_STATE,
                           &set_data );

        range_set = (uint32_t)sensor_ptr->const_ptr->sensitivity_default;
        sns_smgr_set_attr( sensor_ptr,
                           SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                           SNS_DDF_ATTRIB_RANGE,
                           &range_set );
        sensor_ptr->range.range_now = range_set;

        set_data = (uint32_t)SNS_DDF_POWERSTATE_LOWPOWER;
        sns_smgr_set_attr( sensor_ptr,
                           SNS_DDF_SENSOR__ALL,
                           SNS_DDF_ATTRIB_POWER_STATE,
                           &set_data );
      }
      sensor_ptr->range.num_ranges = (uint8_t)len;
    }
    sns_ddf_memhandler_free(&sensor_ptr->memhandler);
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_build_odr_list

===========================================================================*/
/*!
  @brief Builds the list of ODRs the given sensor can support

  @param[i] sensor_ptr - the sensor of interest
  @param[o] odr_list   - destination for the ODR list

  @return  number of ODRs on the list
 */
/*=========================================================================*/
static uint32_t smgr_build_odr_list(smgr_sensor_s* sensor_ptr, sns_ddf_odr_t* odr_list)
{
  uint32_t len = 0;

  if ( SMGR_BIT_CLEAR_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B) )
  {
    /* build ODR table from LPF table */
    for ( len=0; len<sensor_ptr->num_lpf && len<ARR_SIZE(sensor_ptr->lpf_table); len++)
    {
      if ( SMGR_BIT_CLEAR_TEST(sensor_ptr->const_ptr->flags,
                               SNS_SMGR_SENSOR_LPF_FROM_RATE) )
      {
        odr_list[len] = FX_FIXTOFLT_Q16(sensor_ptr->lpf_table[len] << 1); /* BW * 2 */
      }
      else
      {
        odr_list[len] = FX_FIXTOFLT_Q16(sensor_ptr->lpf_table[len]);
      }
      SNS_SMGR_PRINTF3(LOW, "build_odr_list - lpf[%u]=%x odr=%u", 
                       len, sensor_ptr->lpf_table[len], odr_list[len]);
    }
  }
  else
  {
    /* build ODR table by trial-and-error */
    uint32_t odr, dont_care;
    uint32_t* set_odr_ptr;
    uint32_t set_data = (uint32_t)SNS_DDF_POWERSTATE_ACTIVE;
    sns_smgr_set_attr(sensor_ptr,
                      SNS_DDF_SENSOR__ALL,
                      SNS_DDF_ATTRIB_POWER_STATE,
                      &set_data);
    odr = 1;
    while ( odr <= sensor_ptr->ddf_sensor_ptr[0]->max_supported_freq_hz )
    {
      /* set ODR then get back the ODR actually set by driver */
      if ( (sns_smgr_set_attr(sensor_ptr,
                              SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                              SNS_DDF_ATTRIB_ODR,
                              &odr) == SNS_DDF_SUCCESS) &&
           (sns_smgr_get_attr(sensor_ptr,
                              SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                              SNS_DDF_ATTRIB_ODR,
                              (void**)&set_odr_ptr,
                              &dont_care) == SNS_DDF_SUCCESS) &&
           ((len == 0) || (*set_odr_ptr > odr_list[len-1])) )
      {
        odr_list[len++] = *set_odr_ptr;
        odr = (*set_odr_ptr) + 1;
      }
      else
      {
        break;
      }
    }
    set_data = (uint32_t)SNS_DDF_POWERSTATE_LOWPOWER;
    sns_smgr_set_attr(sensor_ptr,
                      SNS_DDF_SENSOR__ALL,
                      SNS_DDF_ATTRIB_POWER_STATE,
                      &set_data);
  }
  return len;
}


/*===========================================================================

  FUNCTION:   sns_smgr_init_odr_tables

===========================================================================*/
/*!
  @brief Initializes hp_odr and normal_odr fields of given sensor.

  @param sensor_ptr - the sensor of interest
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_init_odr_tables(smgr_sensor_s* sensor_ptr)
{
  uint32_t       len = 0;
  sns_ddf_odr_t* odr_list = NULL;
  sns_ddf_odr_t* alloc_odr_list = NULL;

  if ( (sensor_ptr->num_lpf == 0) &&
       SMGR_BIT_CLEAR_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_ODR_SUPPORTED_B) )
  {
    
      return;
  }

  if ( sns_smgr_get_attr(sensor_ptr,
                         SMGR_SENSOR_TYPE_PRIMARY(sensor_ptr),
                         SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST,
                         (void**)&odr_list,
                         &len) != SNS_DDF_SUCCESS )
  {
    uint32_t size =
      sizeof(sns_ddf_odr_t) * sensor_ptr->ddf_sensor_ptr[0]->max_supported_freq_hz;
    alloc_odr_list = odr_list = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, size);
  }
  if ( (odr_list != NULL) && (len == 0) )
  {
    /* SUPPORTED_ODR_LIST not supported; build the list by other means */
    len = smgr_build_odr_list(sensor_ptr, odr_list);
  }
  if ( (odr_list != NULL) && (len != 0) )
  {
    uint32_t  i, j;
    /* convert ODRs into bit positions and store them in normal_odr table */
    for ( i=0, j = 0; i<len && j < ARR_SIZE(sensor_ptr->normal_odr); i++ )
    {
      j = odr_list[i] >> 3;
      if ( j < ARR_SIZE(sensor_ptr->normal_odr) )
      {
        sensor_ptr->normal_odr[j] |= 1 << (odr_list[i] - (j << 3));
      }
    }
    /* initialize hp_odr table with defaults */
    sensor_ptr->hp_odr[0] = 50;
    for ( i=1; i<ARR_SIZE(sensor_ptr->hp_odr); i++ )
    {
      /* each ODR in hp_ODR table is double its previous neighbor */
      sensor_ptr->hp_odr[i] = sensor_ptr->hp_odr[i-1] << 1;
    }
    /* update hp_odr table with equivalent supported ODRs */
    for ( i=0; i<ARR_SIZE(sensor_ptr->hp_odr); i++ )
    {
      uint32_t adjacent_odrs[2];
      sns_smgr_get_adjacent_odrs(sensor_ptr, sensor_ptr->hp_odr[i], adjacent_odrs);
      sensor_ptr->hp_odr[i] = adjacent_odrs[1];
    }
    /* make sure max supported frequency falls within the table */
    for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
    {
      if ( sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__NONE )
      {
        sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz = 
          MIN(sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz, odr_list[len-1]);
      }
    }
  }
  /* clean up */
  if ( alloc_odr_list != NULL )
  {
    SNS_OS_FREE(alloc_odr_list);
  }
  sns_ddf_memhandler_free(&sensor_ptr->memhandler);
}

/*===========================================================================

  FUNCTION:   sns_smgr_init_max_frequency

===========================================================================*/
/*!
  @brief Initializes max_freq field of given sensor.

  @param sensor_ptr - the sensor of interest
  @return
   none
 */
/*=========================================================================*/
void sns_smgr_init_max_frequency(smgr_sensor_s *sensor_ptr)
{
  uint8_t i, j;
  for ( i=0; i<ARR_SIZE(sensor_ptr->const_ptr->data_types); i++ )
  {
    uint32_t len;
    sns_ddf_status_e ddf_status;
    sns_ddf_resolution_adc_s* resolution_adc_ptr;

    if ( sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__NONE )
    {
      ddf_status = sns_smgr_get_attr(sensor_ptr,
                                     SMGR_SENSOR_TYPE(sensor_ptr, i),
                                     SNS_DDF_ATTRIB_RESOLUTION_ADC,
                                     (void**)&resolution_adc_ptr,
                                     &len);
      if ( SNS_DDF_SUCCESS == ddf_status )
      {
        for ( j=0; j<len; j++ )
        {
          sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz =
            MAX(sensor_ptr->ddf_sensor_ptr[i]->max_supported_freq_hz,
                resolution_adc_ptr[j].max_freq);
        }
        sns_ddf_memhandler_free(&sensor_ptr->memhandler);
      }
    }
  }
}


/*===========================================================================

  FUNCTION:   sns_smgr_init_fifo_cfg

===========================================================================*/
/*!
  @brief Initializes the fifo configuration of given sensor.
  @param[in/out] sensor_ptr - the sensor of interest
  @return none
 */
/*=========================================================================*/
void sns_smgr_init_fifo_cfg(smgr_sensor_s *sensor_ptr)
{
  sns_smgr_fifo_init_fifo_cfg( sensor_ptr );
}

/*===========================================================================

  FUNCTION:   smgr_send_reg_message_for_fac_cal

===========================================================================*/
/*!
  @brief Send registry message for factory calibration

  @Detail

  @param[i] NONE

  @return
   NONE
*/
/*=========================================================================*/
void smgr_send_reg_message_for_fac_cal ( void )
{
  smgr_sensor_s       *sensor_ptr;
  smgr_sensor_cfg_s   *sensor_cfg_ptr;
  uint32_t            ix;

  for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    sensor_cfg_ptr = sensor_ptr->const_ptr;

    /* skip any non-existent sensor */
    if ( NULL == SMGR_DRV_FN_PTR(sensor_ptr) )
    {
      continue;
    }

    if ( SNS_SMGR_REG_ITEM_TYPE_NONE != sensor_cfg_ptr->primary_cal_reg_type )
    {
      sns_err_code_e  err;
      err = sns_smgr_req_reg_data(sensor_cfg_ptr->primary_cal_reg_id,
                                  sensor_cfg_ptr->primary_cal_reg_type);
      SNS_ASSERT( SNS_SUCCESS == err);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_sensor_init_post_cfg

===========================================================================*/
/*!
  @brief continue initialize smgr_sensor_s structure

  @Detail

  @param[i] NONE

  @return
   NONE
*/
/*=========================================================================*/
void sns_smgr_sensor_init_post_cfg ( void )
{
  smgr_sensor_s       *sensor_ptr;
  uint32_t            ix, i;

  for ( ix = 0; ix < ARR_SIZE(sns_smgr.sensor); ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    sensor_ptr->const_ptr = &smgr_sensor_cfg[ix];
    sensor_ptr->init_state = SENSOR_INIT_NOT_STARTED;
    sensor_ptr->reg_item_param.nvitem_grp = SNS_SMGR_REG_ITEM_TYPE_NONE;
    sensor_ptr->reg_item_param.status = SNS_DDF_EFAIL;


    for ( i=0; i<SMGR_MAX_DATA_TYPES_PER_DEVICE; i++ )
    {
      if ( sensor_ptr->const_ptr->data_types[i] != SNS_DDF_SENSOR__NONE )
      {
        smgr_ddf_sensor_s* ddf_sensor_ptr;

        sensor_ptr->ddf_sensor_ptr[i] =
          SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, sizeof(smgr_ddf_sensor_s));
        SNS_ASSERT(sensor_ptr->ddf_sensor_ptr[i] != NULL);
        HEAP_STAT(__LINE__, sizeof(smgr_ddf_sensor_s),
                  sensor_ptr->const_ptr->sensor_id);

        ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
        SNS_OS_MEMZERO(ddf_sensor_ptr, sizeof(smgr_ddf_sensor_s));
        ddf_sensor_ptr->data_type  = i;
        ddf_sensor_ptr->sensor_ptr = sensor_ptr;
        ddf_sensor_ptr->sensor_status.ddf_sensor_ptr = ddf_sensor_ptr;
        ddf_sensor_ptr->device_sampling_factor = 1;
        sns_q_link( ddf_sensor_ptr, &ddf_sensor_ptr->sched_link );

        smgr_load_default_cal( &ddf_sensor_ptr->factory_cal );
        smgr_load_default_cal( &ddf_sensor_ptr->auto_cal );
        smgr_load_default_cal( &ddf_sensor_ptr->full_cal );
      }
    }
  }

  sensor_ptr = sns_smgr_find_sensor(SNS_SMGR_ID_ACCEL_V01);
  if ( sensor_ptr != NULL )
  {
    smgr_sensor_s* sibling_sensor_ptr = sns_smgr_find_sensor(SNS_SMGR_ID_GYRO_V01);
    if ( (sibling_sensor_ptr != NULL) &&
         (sensor_ptr->const_ptr->drv_fn_ptr == sibling_sensor_ptr->const_ptr->drv_fn_ptr) )
    {
      sensor_ptr->sibling_ptr = sibling_sensor_ptr;
      sibling_sensor_ptr->sibling_ptr = sensor_ptr;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_dd_init

===========================================================================*/
/*!
  @brief Initialize/install all device drivers

  @detail  Any device driver may be waiting
    for registry data or timer part way through the initialization process, in
    which case, the next driver initialization may be started. A device driver
    issues a sns_ddf_event_init when finished, the status indicates success
    or failure. Timeout of the multi-device initialization process causes
    failure of any devices that have not completed initialization.
  @param Cal_p Calibration pointer
  @return
   none
 */
/*=========================================================================*/
void sns_smgr_dd_init( void )
{
  smgr_sensor_s                *sensor_ptr;
  smgr_sensor_cfg_s            *sensor_cfg_ptr;
  sns_ddf_sensor_e             *sensor_type_ptr;
  sns_ddf_sensor_e              sensor_type[SMGR_MAX_DATA_TYPES_PER_DEVICE];
  sns_ddf_status_e              drv_status;
  static uint32_t               ix;
  uint32_t                      ij;
  uint32_t                      attrib_length;
  sns_ddf_i2c_config_s          i2c_config;
  sns_ddf_device_access_s       ddf_device;

  SMGR_ASSERT_AT_COMPILE(ARR_SIZE(smgr_ssi_cfg)*SNS_REG_SSI_SMGR_CFG_NUM_SENSORS >=
                         ARR_SIZE(smgr_ssi_devinfo));

#ifndef SNS_EXCLUDE_POWER
  sns_hw_power_rail_config( SNS_SMGR_POWER_HIGH );
#endif
  sns_hw_set_qup_clk(true);

  switch ( sns_smgr.all_init_state )
  {
    case SENSOR_ALL_INIT_NOT_STARTED:
      SNS_SMGR_PRINTF0(LOW, "sns_smgr_dd_init SENSOR_ALL_INIT_NOT_STARTED");
#ifdef ADSP_STANDALONE
      sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
      SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
#endif /* ADSP_STANDALONE */
      return;
    case SENSOR_ALL_INIT_DONE:
      SNS_SMGR_PRINTF0(LOW, "sns_smgr_dd_init SENSOR_ALL_INIT_DONE");
      return;
    case SENSOR_ALL_INIT_WAITING_CFG:
      SNS_SMGR_PRINTF0(LOW, "sns_smgr_dd_init SENSOR_ALL_INIT_WAITING_CONFIG");
      return;
    case SENSOR_ALL_INIT_WAITING_AUTODETECT:
      SNS_SMGR_PRINTF0(LOW, "sns_smgr_dd_init SENSOR_ALL_INIT_WAITING_AUTODETECT");
      return;
    case SENSOR_ALL_INIT_AUTODETECT_DONE:
      SNS_SMGR_PRINTF0(LOW, "sns_smgr_dd_init SENSOR_ALL_INIT_AUTODETECT_DONE");
      return;
    case SENSOR_ALL_INIT_CONFIGURED:
    {
       uint32_t i;
       uint16_t off_to_idle_max = 0;
       SNS_SMGR_PRINTF0(LOW, "sns_smgr_dd_init SENSOR_ALL_INIT_CONFIGURED");
       sns_smgr.all_init_state = SENSOR_ALL_INIT_IN_PROGRESS;
       sns_smgr.init_start_tick = sns_em_get_timestamp();
       sns_smgr_sensor_init_post_cfg();
       ix = 0;
#ifndef ADSP_STANDALONE
       /* as all configuration info is available now, request the calibration data to REG module */
       smgr_send_reg_message_for_fac_cal();
#endif /* ADSP_STANDALONE */
      /* calculate max off_to_idle delay */
      for ( i = 0; i < SNS_SMGR_NUM_SENSORS_DEFINED; i++ )
      {
        sensor_ptr = &sns_smgr.sensor[i];
        sensor_cfg_ptr = sensor_ptr->const_ptr;

        off_to_idle_max = MAX(sensor_cfg_ptr->off_to_idle_time, off_to_idle_max);
      }

      /* wait for Max of all the OFF_TO_IDLE delays */
      SMGR_DELAY_US(sns_em_convert_dspstick_to_usec(off_to_idle_max));
      break;
    }
    default:
      break;
  }


  for ( ; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    sensor_cfg_ptr = sensor_ptr->const_ptr;
    sensor_type_ptr = sensor_type;

    /* Setup access to driver function list */
    if ( (NULL == SMGR_DRV_FN_PTR(sensor_ptr)) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->init) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->reset) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->get_data) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->set_attrib) ||
         (NULL == SMGR_DRV_FN_PTR(sensor_ptr)->get_attrib) )
    {
      /* Driver function list not defined */
      sensor_ptr->init_state = SENSOR_INIT_FAIL;
      continue;
    }

    attrib_length = 0;
    sensor_ptr->num_data_types = 0;
    for ( ij=0; ij<ARR_SIZE(sensor_ptr->const_ptr->data_types); ij++ )
    {
      if ( (sensor_ptr->const_ptr->data_types[ij] != SNS_DDF_SENSOR__NONE) &&
           (sensor_ptr->const_ptr->data_types[ij] != SNS_DDF_SENSOR__ALL) )
      {
        attrib_length++;
        sensor_type[ij] = sensor_ptr->const_ptr->data_types[ij];
        sensor_ptr->num_data_types++;
      }
    }
    if ( SENSOR_INIT_NOT_STARTED == sensor_ptr->init_state ||
         SENSOR_INIT_WAITING_TIMER == sensor_ptr->init_state ||
         SENSOR_INIT_REG_READY == sensor_ptr->init_state )
    {
#if !defined SNS_PCSIM && !defined ADSP_STANDALONE
      /* Request registry data for driver if not already requested.
         If driver registry data does not arrive by timeout, the init fails.
       */
      if ( SNS_SMGR_REG_ITEM_TYPE_NONE != sensor_cfg_ptr->driver_reg_type &&
           SMGR_BIT_CLEAR_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_REG_REQ_DRIVER_B) )
      {
         sns_err_code_e err = SNS_ERR_FAILED;
         uint32_t i;

         for (i=0; i<SMGR_RETRY_CNT_DD_REG_REQ && err!=SNS_SUCCESS; i++)
         {
            err = sns_smgr_req_reg_data(sensor_cfg_ptr->driver_reg_id,
                                      sensor_cfg_ptr->driver_reg_type);
            if ( SNS_SUCCESS != err )
            {
              SMGR_DELAY_US(1000); /* 1 ms */
            }
         }

         if ( SNS_SUCCESS != err )
         {
           SNS_SMGR_PRINTF0(FATAL, "sns_smgr_dd_init - error reading registry!");
           sensor_ptr->init_state = SENSOR_INIT_FAIL;
         }
         else
         {
            /* Request message sent */
            SMGR_BIT_SET(sensor_ptr->flags, SMGR_SENSOR_FLAGS_REG_REQ_DRIVER_B);
            sensor_ptr->init_state = SENSOR_INIT_WAITING_REG;
         }
      }
#endif /* SNS_PCSIM  || ADSP_STANDALONE */
      if ( SENSOR_INIT_WAITING_REG == sensor_ptr->init_state ||
           SENSOR_INIT_FAIL == sensor_ptr->init_state )
      {
        /* Skip init if waiting for registry or failed. We will loop through
           here again */
        continue;
      }
#ifdef ADSP_HWCONFIG_L
      sns_hw_int_gpio_inactive_config(sensor_cfg_ptr->first_gpio);
#endif
      ddf_device.device_select = sensor_cfg_ptr->device_select;
      ddf_device.port_config.bus = SNS_DDF_BUS_I2C;
      ddf_device.port_config.bus_config.i2c  = &i2c_config;
      ddf_device.first_gpio = sensor_cfg_ptr->first_gpio;
      ddf_device.second_gpio = sensor_cfg_ptr->second_gpio;
      ddf_device.port_config.bus_instance = sensor_cfg_ptr->bus_instance;
#ifdef ADSP_HWCONFIG_L
      ddf_device.flags = sensor_cfg_ptr->flags;
#endif
      i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
      i2c_config.bus_acq_timeout = -1;
      i2c_config.bus_freq = 400;
      i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
      i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
      i2c_config.slave_addr = sensor_cfg_ptr->bus_addr;
      i2c_config.xfer_timeout = -1;
      i2c_config.reg_addr_type = SNS_DDF_I2C_REG_ADDR_8BIT; /* default to 8-bit register
                                                               address. Driver can override
                                                               for 16-bit addressing. */
      SNS_SMGR_PRINTF2(LOW, "calling init: bus_instance: %d slave_addr:0x%x",
                       ddf_device.port_config.bus_instance,
                       ddf_device.port_config.bus_config.i2c->slave_addr);

      sns_ddf_memhandler_init( &(sensor_ptr->memhandler) );

      drv_status = SMGR_DRV_FN_PTR(sensor_ptr)->init(
                                      &sensor_ptr->dd_handle,
                                      sensor_ptr,
                                      &(sensor_ptr->reg_item_param),
                                      &ddf_device,
                                      1,        /* Num devices on this driver */
                                      &(sensor_ptr->memhandler),
                                      &sensor_type_ptr,
                                      &attrib_length );
      if ( 0 != sensor_ptr->reg_item_param.data )
      {
        /* Registry data received for device driver. Release copy */
        SNS_OS_FREE( sensor_ptr->reg_item_param.data );
        sensor_ptr->reg_item_param.data = 0;
      }

      if ( (SNS_DDF_PENDING != drv_status) && ( SNS_DDF_SUCCESS != drv_status ) )
      {
        SNS_SMGR_PRINTF1(FATAL, "sns_smgr_dd_init - bad drv_status %u!", drv_status);
        sensor_ptr->init_state = SENSOR_INIT_FAIL;
        continue;

      }
      /* Setup for get_data to retrieve all data types (known to DD as sensors) */
      /* Limit num_sensors to space available */
      if ( sensor_ptr->num_data_types > (uint8_t)attrib_length )
      {
        /* device supports fewer types than configured */
        SNS_SMGR_PRINTF2(FATAL, "sns_smgr_dd_init - device expects more types (%d/%d)!",
                         sensor_ptr->num_data_types, attrib_length);
        sensor_ptr->init_state = SENSOR_INIT_FAIL;
        continue;
      }
      for ( ij=0; ij<sensor_ptr->num_data_types &&
                  sensor_ptr->init_state != SENSOR_INIT_FAIL; ij++ )
      {
        if ( sensor_ptr->const_ptr->data_types[ij] != sensor_type_ptr[ij] )
        {
          SNS_SMGR_PRINTF0(FATAL, "sns_smgr_dd_init - error!");
          sensor_ptr->init_state = SENSOR_INIT_FAIL;
        }
      }
      /* TODO: If the sensor supports temperature, add that here even if it's
       * not in the initial configuration */
      if ( sensor_ptr->init_state == SENSOR_INIT_FAIL )
      {
        continue;
      }
      if ( SNS_DDF_PENDING == drv_status )
      {
        /* Come back again if driver is pending */
        sensor_ptr->init_state = SENSOR_INIT_WAITING_TIMER;
        continue;
      }
      if ( SNS_DDF_SUCCESS != drv_status )
      {
        SNS_SMGR_PRINTF1(FATAL, "sns_smgr_dd_init - drv_status %u!", drv_status);
        sensor_ptr->init_state = SENSOR_INIT_FAIL;
        continue;
      }
      sensor_ptr->init_state = SENSOR_INIT_SUCCESS;

      sns_smgr_create_sample_depot(sensor_ptr);

    }   /* END - if ( SENSOR_INIT_NOT_STARTED == sensor_ptr->init_state || */
  }   /* END - for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ ) */

  /* Assume all drivers have completed init */
  sns_smgr.all_init_state = SENSOR_ALL_INIT_DONE;
  /* Now look for exceptions */

  for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ )
  {
    sensor_ptr = &sns_smgr.sensor[ix];
    if ( SENSOR_INIT_SUCCESS != sensor_ptr->init_state  &&
         (SMGR_MAX_TICKS < ((sns_smgr.init_start_tick + SMGR_DD_INIT_TIMEOUT_TICK) -
                         sns_em_get_timestamp())) )
    {
      /* Timeout and device has not reached success. Set driver failed */
      sensor_ptr->init_state = SENSOR_INIT_FAIL;
    }
    if ( SENSOR_INIT_SUCCESS != sensor_ptr->init_state &&
         SENSOR_INIT_FAIL != sensor_ptr->init_state )
    {
      /* Some device is neither done nor failed, must be in progress */
      sns_smgr.all_init_state = SENSOR_ALL_INIT_IN_PROGRESS;
      break;
    }
  }
  if ( SENSOR_ALL_INIT_IN_PROGRESS == sns_smgr.all_init_state )
  {
    /* Init not all done. Make scheduler try init again */
    SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
  }
  else
  {
    sns_smgr.max_off2idle_time = 0;
    for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ )
    {
      sensor_ptr = &sns_smgr.sensor[ix];
      sensor_cfg_ptr = sensor_ptr->const_ptr;

      if ( SENSOR_INIT_SUCCESS == sensor_ptr->init_state )
      {
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_IDLE);
        SMGR_BIT_CLEAR(sensor_ptr->flags, SMGR_SENSOR_FLAGS_REG_REQ_DRIVER_B);
        sns_smgr.max_off2idle_time = MAX(sns_smgr.max_off2idle_time,
                                         sensor_cfg_ptr->off_to_idle_time);

        sns_smgr_init_max_frequency(sensor_ptr);
        sns_smgr_init_odr_supported(sensor_ptr);
        sns_smgr_init_lpf_table(sensor_ptr);
        sns_smgr_init_range(sensor_ptr);
        sns_smgr_init_odr_tables(sensor_ptr);
        sns_smgr_init_fifo_cfg(sensor_ptr);
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_OFF);
      }
      else if ( SENSOR_INIT_FAIL == sensor_ptr->init_state )
      {
        sns_smgr_set_sensor_state(sensor_ptr, SENSOR_STATE_FAILED);
      }
      sns_ddf_memhandler_free( &sensor_ptr->memhandler );

    }   /* END - for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ ) */
    SMGR_BIT_CLEAR(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
    /* disable QUP clock to save power*/
    sns_hw_set_qup_clk(false);
    if (sns_smgr.md.total_rpt_num==0)
    {
      sns_hw_power_rail_config(SNS_SMGR_POWER_OFF);
      sns_smgr_send_power_vote(SNS_PM_ST_INACTIVE_V01);
    }
  }   /* END - if ( SENSOR_ALL_INIT_IN_PROGRESS == sns_smgr.all_init_state ) - else */
}

/*===========================================================================

  FUNCTION:   sns_smgr_uuid_to_fn_ptr

===========================================================================*/
/*!
  @brief find driver function pointer from UUID.

  @detail
  @param
   UUId - driver UUID

  @return
    driver function pointer
 */
/*=========================================================================*/
static sns_ddf_driver_if_s * sns_smgr_uuid_to_fn_ptr( const uint8_t * uuid)
{
  uint8_t i;

  for ( i=0; i<SMGR_SENSOR_FN_PTR_MAP_TBLE_SIZE; i++ )
  {
    if(SNS_OS_MEMCMP(uuid, smgr_sensor_fn_ptr_map[i].drvuuid, 16 )==0)
      return smgr_sensor_fn_ptr_map[i].fun_ptr ;
  }
  return NULL ;
}

/*===========================================================================

  FUNCTION:   sns_smgr_fn_ptr_to_uuid

===========================================================================*/
/*!
  @brief find driver function pointer from UUID.

  @detail
  @param
   fn_ptr - Pointer to driver function interface

  @return
    Pointer to static const UUID
 */
/*=========================================================================*/
static const uint8_t * sns_smgr_fn_ptr_to_uuid( const sns_ddf_driver_if_s *fn_ptr)
{
  uint8_t i;

  for ( i=0; i<SMGR_SENSOR_FN_PTR_MAP_TBLE_SIZE; i++ )
  {
    if(smgr_sensor_fn_ptr_map[i].fun_ptr == fn_ptr)
    {
      return smgr_sensor_fn_ptr_map[i].drvuuid;
    }
  }
  return NULL ;
}

/*===========================================================================

  FUNCTION:   smgr_is_valid_fac_cal

===========================================================================*/
/*!
  @brief check if factory calibration data is within the valid range

  @detail treat the value invalid when all biases values are 0
  @param
  @return
 */
/*=========================================================================*/
static boolean smgr_is_valid_fac_cal(q16_t *cal_data_ptr)
{
  /* biases data validity */
  if ((0==cal_data_ptr[0]) && (0==cal_data_ptr[1]) &&  (0==cal_data_ptr[2]))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_cfg_idx

===========================================================================*/
/*!
  @brief  Checks if 'id' is a valid SSI CFG ID

  @param  id  : CFG ID
  @return   -1  : if 'id' does not exist in smgr_ssi_cfg[]
          >= 0  : 'id' exists, and value corresponds to the column index.
 */
/*=========================================================================*/
int sns_smgr_ssi_get_cfg_idx(uint16_t id)
{
  int i, rv = -1;

  for(i = 0; i < ARR_SIZE(smgr_ssi_cfg); i++)
  {
    if(smgr_ssi_cfg[i] == id)
    {
      rv = i;
      break;
    }
  }

  return rv;
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_next_cfg_id

===========================================================================*/
/*!
  @brief  Gets the next CFG group ID

  @param  curr_id  : the current CFG ID
  @return    -1 : if 'id' is the last CFG ID, or 'id' does not exist.
          >= 0  : the next CFG group ID
 */
/*=========================================================================*/
static int32_t sns_smgr_ssi_get_next_cfg_id(uint16_t curr_id)
{
  int32_t next_id = -1;
  int idx = sns_smgr_ssi_get_cfg_idx(curr_id);

  if( (idx>=0) && ((idx+1) < ARR_SIZE(smgr_ssi_cfg) ) )
  {
    next_id = (int32_t) smgr_ssi_cfg[idx+1];
  }

  return next_id;
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_is_last_cfg

===========================================================================*/
/*!
  @brief  Checks if id is the last SMGR CFG Id expected.

  @param  id  : SMGR CFG ID
  @return 'true' if so, 'false' otherwise
 */
/*=========================================================================*/
bool sns_smgr_ssi_is_last_cfg(uint16_t id)
{
  return ( smgr_ssi_cfg[ARR_SIZE(smgr_ssi_cfg)-1] == (id) );
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_cfg_id

===========================================================================*/
/*!
  @brief  Returns the 'idx'-th SMGR CFG Id in smgr_ssi_cfg table.

  @param  idx  : index
  @return   >= 0 : a valid Id
              -1 : otherwise
 */
/*=========================================================================*/
int32_t sns_smgr_ssi_get_cfg_id(uint8_t idx)
{
  return ( (idx < ARR_SIZE(smgr_ssi_cfg)) ? (int32_t)smgr_ssi_cfg[idx] : -1 );
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_devinfo_idx

===========================================================================*/
/*!
  @brief  Checks if 'id' is a valid SSI device info ID

  @param  id  : device info ID

  @return   -1  : if 'id' does not exist in smgr_ssi_devinfo[]
          >= 0  : 'id' exists, and value corresponds to the column index
 */
/*=========================================================================*/
static int sns_smgr_ssi_get_devinfo_idx(uint16_t id)
{
  int i, rv = -1;

  for(i = 0; i < ARR_SIZE(smgr_ssi_devinfo); i++)
  {
    if(smgr_ssi_devinfo[i] == id)
    {
      rv = i;
      break;
    }
  }

  return rv;
}

/*===========================================================================

  FUNCTION:   sns_smgr_ssi_get_next_devinfo_id

===========================================================================*/
/*!
  @brief  Gets the next DEVINFO group ID

  @param  curr_id  : the current DEVINFO ID
  @return   -1  : if 'id' is the last ID, or 'id' does not exist.
          >= 0  : the next CFG group ID
 */
/*=========================================================================*/
static int32_t sns_smgr_ssi_get_next_devinfo_id(uint16_t curr_id)
{
  int32_t next_id = -1;
  int idx = sns_smgr_ssi_get_devinfo_idx(curr_id);

  if(!SNS_SMGR_SSI_IS_LAST_DEVINFO(curr_id) && (idx >= 0) &&
     ((idx+1) < ARR_SIZE(smgr_ssi_devinfo)))
  {
    next_id = (int32_t) smgr_ssi_devinfo[idx+1];
  }

  return next_id;
}

/*===========================================================================

  FUNCTION:   sns_smgr_populate_cfg_from_devinfo

===========================================================================*/
/*!
  @brief Fills in a smgr_sensor_cfg_s based on registry device info

  @detail
  @param
   Id - Registry DEVINFO group ID.
   sensor_cfg_ptr - pointer to entry in smgr_sensor_cfg
   devinfo - pointer to sns_reg_ssi_devinfo_group_s
   Ix - index into devinfo
   num_sensors - number of sensors in the sensors_list
   sensors_list - Array of sensors supported by this device
   device_select - device_select value returned by probe function
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_populate_cfg_from_devinfo ( uint16_t Id,
                                                 smgr_sensor_cfg_s *sensor_cfg_ptr,
                                                 const sns_reg_ssi_devinfo_group_s *devinfo,
                                                 uint8_t Ix,
                                                 uint32_t num_sensors,
                                                 const sns_ddf_sensor_e *sensors_list,
                                                 uint8_t device_select )
{
  int i;
  sensor_cfg_ptr->drv_fn_ptr = sns_smgr_uuid_to_fn_ptr(devinfo->uuid_cfg[Ix].drvuuid );
  SNS_OS_MEMCOPY(sensor_cfg_ptr->uuid, devinfo->uuid_cfg[Ix].drvuuid, sizeof(sensor_cfg_ptr->uuid));
  sensor_cfg_ptr->off_to_idle_time =
    (uint16_t)((devinfo->uuid_cfg[Ix].off_to_idle/SNS_SMGR_USEC_PER_TICK)+1);
  sensor_cfg_ptr->idle_to_ready_time =
    (uint16_t)((devinfo->uuid_cfg[Ix].idle_to_ready/SNS_SMGR_USEC_PER_TICK)+1);
  sensor_cfg_ptr->bus_instance = devinfo->uuid_cfg[Ix].i2c_bus;
  sensor_cfg_ptr->device_select = device_select;
  if(  devinfo->uuid_cfg[Ix].reg_group_id == 0xFFFF ) {
    sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
    sensor_cfg_ptr->driver_reg_id = 0;
  }
  else
  {
    sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
    sensor_cfg_ptr->driver_reg_id = devinfo->uuid_cfg[Ix].reg_group_id;
  }
  if( devinfo->uuid_cfg[Ix].cal_pri_group_id == 0xFFFF ) {
    sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
    sensor_cfg_ptr->primary_cal_reg_id = 0;
  } else {
    sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
    sensor_cfg_ptr->primary_cal_reg_id = devinfo->uuid_cfg[Ix].cal_pri_group_id;
  }
  sensor_cfg_ptr->first_gpio = devinfo->uuid_cfg[Ix].gpio1;
  sensor_cfg_ptr->second_gpio = -1;
  sensor_cfg_ptr->bus_addr = devinfo->uuid_cfg[Ix].i2c_address;
  sensor_cfg_ptr->sensitivity_default = devinfo->uuid_cfg[Ix].sensitivity_default;
  sensor_cfg_ptr->flags = devinfo->uuid_cfg[Ix].flags;

  sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_NONE;
  sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_NONE;
  sensor_cfg_ptr->range_sensor = 0;
  switch( Id ) {
    case SNS_REG_GROUP_SSI_DEVINFO_ACCEL_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_ACCEL_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] == SNS_REG_SSI_DATA_TYPE_ACCEL ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_ACCEL;
        }
        if( sensors_list[i] == SNS_REG_SSI_DATA_TYPE_TEMP) {
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_GYRO_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_GYRO_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] == SNS_REG_SSI_DATA_TYPE_GYRO ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_GYRO;
        }
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_MAG_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_MAG_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_MAG ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_MAG;
        }
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_PROX_LIGHT_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_PROX_LIGHT_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_PROXIMITY ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_PROXIMITY;
        }
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_AMBIENT ) {
          sensor_cfg_ptr->range_sensor = 1;
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_AMBIENT;
        }
      }
      SNS_SMGR_PRINTF1(HIGH, "range_sensor=%d", sensor_cfg_ptr->range_sensor);
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_PRESSURE_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_PRESSURE_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_PRESSURE ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_PRESSURE;
        }
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_TEMP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_IR_GESTURE_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_IR_GESTURE_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_IR_GESTURE ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_IR_GESTURE;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_TAP_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_TAP_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_DOUBLETAP ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_DOUBLETAP;
        }
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_SINGLETAP ) {
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_SINGLETAP;
        }
      }
      break;
    case SNS_REG_GROUP_SSI_DEVINFO_HUMIDITY_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_HUMIDITY_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_HUMIDITY ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_HUMIDITY;
        }
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_AMBIENT_TEMP ) {
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_AMBIENT_TEMP;
        }
      }
      break;

    case SNS_REG_GROUP_SSI_DEVINFO_RGB_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_RGB_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_RGB ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_RGB;
        }
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_CT_C ) {
          sensor_cfg_ptr->data_types[1] = SNS_REG_SSI_DATA_TYPE_CT_C;
        }
      }
      break;

    case SNS_REG_GROUP_SSI_DEVINFO_SAR_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_SAR_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_SAR ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_SAR;
        }
      }
      break;

    case SNS_REG_GROUP_SSI_DEVINFO_HALL_EFFECT_V02:
      sensor_cfg_ptr->sensor_id = SNS_SMGR_ID_HALL_EFFECT_V01;
      for( i = 0; i < num_sensors; i++ ) {
        if( sensors_list[i] ==  SNS_REG_SSI_DATA_TYPE_HALL_EFFECT ) {
          sensor_cfg_ptr->data_types[0] = SNS_REG_SSI_DATA_TYPE_HALL_EFFECT;
        }
      }
      break;

    default:
      break;
  }

}

/*===========================================================================

  FUNCTION:   sns_smgr_parse_reg_devinfo_resp

===========================================================================*/
/*!
  @brief Process SSI devinfo response for sensor autodetect

  @detail
  @param
   Id - Group ID or single item ID
   devinfo - pointer to sns_reg_ssi_devinfo_group_s
  @return
   none
 */
/*=========================================================================*/
void sns_smgr_parse_reg_devinfo_resp( uint16_t Id,
                                      const sns_reg_ssi_devinfo_group_s *devinfo )
{
  int i;
  uint16_t devinfo_idx;

  devinfo_idx = sns_smgr_ssi_get_devinfo_idx(Id);

  if ( devinfo->min_ver_no != 1 ||
       SNS_SMGR_NUM_SENSORS_DEFINED <= devinfo_idx )
  {
    SNS_SMGR_PRINTF2(ERROR, "ssi: wrong devinfo->min_ver_no: %u, devinfo_idx: %u", devinfo->min_ver_no, devinfo_idx);
    return;
  }

  for ( i = 0; i < SNS_REG_SSI_DEVINFO_NUM_CFGS &&
               i < devinfo->num_uuid_dev_info_valid; i++ )
  {
    sns_ddf_driver_if_s    *drv_fn_ptr;
    sns_ddf_device_access_s dev_access;
    uint32_t                num_sensors;
    sns_ddf_sensor_e       *sensor_list;
    sns_ddf_i2c_config_s    i2c_config;
    sns_ddf_memhandler_s    memhandler;
    sns_ddf_status_e        status;
    smgr_sensor_cfg_s      *sensor_cfg_ptr;
    drv_fn_ptr = sns_smgr_uuid_to_fn_ptr(devinfo->uuid_cfg[i].drvuuid );
    if( drv_fn_ptr == NULL )
    {
      SNS_SMGR_PRINTF2(ERROR, "ssi: No UUID for devinfo_idx[i]: %u[%u]", devinfo_idx, i);
      continue;
    }

    if( drv_fn_ptr->probe != NULL )
    {
      dev_access.device_select = 0;
      dev_access.port_config.bus = SNS_DDF_BUS_I2C;
      dev_access.port_config.bus_config.i2c = &i2c_config;
      dev_access.port_config.bus_instance = devinfo->uuid_cfg[i].i2c_bus;
      dev_access.first_gpio = devinfo->uuid_cfg[i].gpio1;
      dev_access.second_gpio = 0;

      i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
      i2c_config.bus_acq_timeout = -1;
      i2c_config.bus_freq = 400;
      i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
      i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
      i2c_config.slave_addr = devinfo->uuid_cfg[i].i2c_address;
      i2c_config.xfer_timeout = -1;
      i2c_config.reg_addr_type = SNS_DDF_I2C_REG_ADDR_8BIT; /* default to 8-bit register
                                                               address. Driver can override
                                                               */
      sns_ddf_memhandler_init( &memhandler );
      SNS_SMGR_PRINTF2(LOW, "ssi: probing devinfo_idx[i]: %u[%u]", devinfo_idx, i);
      SNS_SMGR_PRINTF3(LOW, "ssi: bus_instance:%u gpio1:%u slave_addr:0x%x", devinfo->uuid_cfg[i].i2c_bus, devinfo->uuid_cfg[i].gpio1,
                       devinfo->uuid_cfg[i].i2c_address );
      status = drv_fn_ptr->probe( &dev_access, &memhandler,
                                  &num_sensors, &sensor_list );
      if( status == SNS_DDF_SUCCESS  && num_sensors != 0 )
      {
        SNS_SMGR_PRINTF2(HIGH, "ssi: devinfo_idx[i]: %u[%u] probe success", devinfo_idx, i);
        /* Populate the smgr_sensor_cfg[] array in the same order as the DEVINFO
           entries in the registry */
        sensor_cfg_ptr = &smgr_sensor_cfg[devinfo_idx];
        sns_smgr_populate_cfg_from_devinfo( Id, sensor_cfg_ptr, devinfo, i,
                                            num_sensors, sensor_list,
                                            dev_access.device_select );
        sns_ddf_memhandler_free( &memhandler );
        /* Only one sensor per type is currently supported, so return here now
           that one has been found */
        return;
      }
      SNS_SMGR_PRINTF2(HIGH, "ssi: devinfo_idx[i]: %u[%u] probe failed", devinfo_idx, i);

      sns_ddf_memhandler_free( &memhandler );
    } /* end if( drv_fn_ptr->probe != NULL ) block */
    else
    {
      /* Device does not support the probe function. Assume it's connected */
      const uint8_t default_device_select = 0;
      sns_ddf_sensor_e sensor_type[SMGR_MAX_DATA_TYPES_PER_DEVICE];

      SNS_SMGR_PRINTF2(MED, "ssi: devinfo_idx[i]: %u[%u] probe function not supported", devinfo_idx, i);

      sensor_cfg_ptr = &smgr_sensor_cfg[devinfo_idx];
      num_sensors = 1;
      sensor_list = sensor_type;
      switch( Id )
      {
        case SNS_REG_GROUP_SSI_DEVINFO_ACCEL_V02:
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_ACCEL;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_GYRO_V02:
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_GYRO;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_MAG_V02:
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_MAG;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_PROX_LIGHT_V02:
          num_sensors = 2;
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_PROXIMITY;
          sensor_type[1] = SNS_REG_SSI_DATA_TYPE_AMBIENT;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_PRESSURE_V02:
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_PRESSURE;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_IR_GESTURE_V02:
          num_sensors = 1;
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_IR_GESTURE;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_TAP_V02:
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_DOUBLETAP;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_HUMIDITY_V02:
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_HUMIDITY;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_RGB_V02:
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_RGB;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_SAR_V02:
          num_sensors = 1;
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_SAR;
          break;
        case SNS_REG_GROUP_SSI_DEVINFO_HALL_EFFECT_V02:
          num_sensors = 1;
          sensor_type[0] = SNS_REG_SSI_DATA_TYPE_HALL_EFFECT;
          break;

        default:
          num_sensors = 0;
          break;
      }
      sns_smgr_populate_cfg_from_devinfo( Id, sensor_cfg_ptr, devinfo, i,
                                          num_sensors, sensor_list,
                                          default_device_select );

      /* Only one sensor per type is currently supported, so return here now
         that one has been found */
      return;
    } /* end if( drv_fn_ptr->probe != NULL ) else block*/
  } /* end for */
}

/*===========================================================================

  FUNCTION:   sns_smgr_populate_ssi_cfg_from_smgr_cfg

===========================================================================*/
/*!
  @brief Fills in a sns_reg_ssi_smgr_cfg_group_s from a SMGR configuration

  @detail
  @param
   ssi_cfg_ptr - Pointer to an SSI SMGR configuration
   sensor_cfg_ptr - pointer to entry in smgr_sensor_cfg
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_populate_ssi_cfg_from_smgr_cfg(
  sns_reg_ssi_smgr_cfg_group_s *ssi_cfg_ptr,
  smgr_sensor_cfg_s            *sensors_cfg_ptr,
  int                          cfg_index)
{
  int i, os;
  const uint8_t *uuid;

  SNS_OS_MEMSET(ssi_cfg_ptr,
                0,
                sizeof(sns_reg_ssi_smgr_cfg_group_s));

  ssi_cfg_ptr->maj_ver_no = 1;
  ssi_cfg_ptr->min_ver_no = 1;
  ssi_cfg_ptr->reserved1 = 0;
  ssi_cfg_ptr->reserved2 = 0;
  ssi_cfg_ptr->reserved3 = 0;
  ssi_cfg_ptr->reserved4 = 0;


  os = 0;
  i = cfg_index * SNS_REG_SSI_SMGR_CFG_NUM_SENSORS;

  while(  os < SNS_REG_SSI_SMGR_CFG_NUM_SENSORS &&
          i < SNS_SMGR_NUM_SENSORS_DEFINED)
  {
    sns_reg_ssi_smgr_cfg_group_drvcfg_s *drv_ptr = &ssi_cfg_ptr->drv_cfg[os];

    uuid = sns_smgr_fn_ptr_to_uuid(sensors_cfg_ptr[i].drv_fn_ptr);
    if( uuid != NULL )
    {
      SNS_OS_MEMCOPY(drv_ptr->drvuuid, uuid, 16);
      drv_ptr->off_to_idle =
        sns_em_convert_dspstick_to_usec( sensors_cfg_ptr[i].off_to_idle_time );
      drv_ptr->idle_to_ready =
        sns_em_convert_dspstick_to_usec( sensors_cfg_ptr[i].idle_to_ready_time );
      drv_ptr->i2c_bus = sensors_cfg_ptr[i].bus_instance;
      if( sensors_cfg_ptr[i].driver_reg_type == SNS_SMGR_REG_ITEM_TYPE_NONE ) {
        drv_ptr->reg_group_id = 0xFFFF;
      } else {
        drv_ptr->reg_group_id = sensors_cfg_ptr[i].driver_reg_id;
      }
      if( sensors_cfg_ptr[i].primary_cal_reg_type == SNS_SMGR_REG_ITEM_TYPE_NONE ) {
        drv_ptr->cal_pri_group_id = 0xFFFF;
      } else {
        drv_ptr->cal_pri_group_id = sensors_cfg_ptr[i].primary_cal_reg_id;
      }
      drv_ptr->gpio1 = sensors_cfg_ptr[i].first_gpio;
      drv_ptr->gpio2 = sensors_cfg_ptr[i].second_gpio;
      drv_ptr->sensor_id = sensors_cfg_ptr[i].sensor_id;
      drv_ptr->i2c_address = sensors_cfg_ptr[i].bus_addr;
      drv_ptr->data_type1 = sensors_cfg_ptr[i].data_types[0];
      drv_ptr->data_type2 = sensors_cfg_ptr[i].data_types[1];
      drv_ptr->related_sensor_index = -1;
      drv_ptr->sensitivity_default = sensors_cfg_ptr[i].sensitivity_default;
      drv_ptr->flags = sensors_cfg_ptr[i].flags;
      drv_ptr->device_select = sensors_cfg_ptr[i].device_select;
      drv_ptr->reserved2 = 0;
      drv_ptr->reserved3 = 0;
    }

    os++;
    i++;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_sensor_dep_reg_data

===========================================================================*/
/*!
  @brief Configures default values for sensor dependent registry items after
   autodetect is complete
  @detail
  @param
   cfg_group_ptr - Pointer to a sns_reg_ssi_sensor_dep_reg_group_s struct
  @return
   none
 */
/*=========================================================================*/
static void sns_smgr_process_sensor_dep_reg_data(sns_reg_ssi_sensor_dep_reg_group_s * cfg_group_ptr)
{
  uint8_t sensor_type = cfg_group_ptr->sensor_type - 1;
  uint8_t i, j;
  if( sensor_type < SNS_SMGR_NUM_SENSORS_DEFINED )
  {
    // Get autodetected sensor of this type
    const uint8_t * uuid = sns_smgr_fn_ptr_to_uuid(smgr_sensor_cfg[sensor_type].drv_fn_ptr);
    if( uuid != NULL )
    {
      // Find default values for this sensor
      for( i = 0; i < SNS_REG_MAX_SENSORS_WITH_DEP_REG_ITEMS; ++i )
      {
        if( SNS_OS_MEMCMP( cfg_group_ptr->uuid_reg_cfg[i].drvuuid, uuid, sizeof(cfg_group_ptr->uuid_reg_cfg[i].drvuuid) ) == 0 )
        {
          for( j = 0; j < SNS_REG_MAX_SENSOR_DEP_REG_ITEMS; ++j )
          {
            if( cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].reg_item_id != 0 &&
                cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].size != 0 )
            {
              // Update reg item with new value
              sns_smgr_update_reg_data( cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].reg_item_id,
                                SNS_SMGR_REG_ITEM_TYPE_SINGLE,
                                cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].size,
                                (uint8_t*)cfg_group_ptr->uuid_reg_cfg[i].reg_items[j].value );
            }
          }
        }
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_reg_devinfo

===========================================================================*/
/*!
  @brief Process a retrieved registry devinfo group.

  @detail
  @param
   Type - Group or Single item registry
   Id - Group ID or single item ID
   Length - bytes
   data_ptr - pointer to received data
   sns_resp - response error
  @return
   none
 */
/*=========================================================================*/
static void 
sns_smgr_process_reg_devinfo( sns_smgr_RegItemType_e Type, uint16_t Id,
                              uint32_t Length, uint8_t* data_ptr, sns_common_resp_s_v01 sns_resp )
{
  int             devinfo_idx;
  uint32_t        ix;
  sns_err_code_e  err;

  if( Id == 0 )
  {
    Id = sns_smgr.last_received_reg_group_id;
  }

  devinfo_idx = sns_smgr_ssi_get_devinfo_idx(Id);
  if(sns_resp.sns_err_t == SENSOR1_SUCCESS)
  {
    sns_smgr_parse_reg_devinfo_resp( Id, (sns_reg_ssi_devinfo_group_s*)data_ptr );
  }
  else
  {
    SNS_SMGR_PRINTF3(HIGH, "ssi: resp_err=%u, id=%u, last_id=%u", sns_resp.sns_err_t, Id, sns_smgr.last_received_reg_group_id);
  }

  if( SNS_SMGR_SSI_IS_LAST_DEVINFO(Id) )
  {
    sns_reg_ssi_smgr_cfg_group_s ssi_cfg;

    SNS_SMGR_PRINTF0(HIGH, "ssi: populating SSI config.");
      
    for( ix = 0; ix < ARR_SIZE(smgr_ssi_cfg); ix++ )
    {
      sns_smgr_populate_ssi_cfg_from_smgr_cfg( &ssi_cfg,
                                               smgr_sensor_cfg,
                                               ix );

      err = sns_smgr_update_reg_data( (uint16_t) sns_smgr_ssi_get_cfg_id(ix),
                                      SNS_SMGR_REG_ITEM_TYPE_GROUP,
                                      sizeof( ssi_cfg ), (uint8_t*)&ssi_cfg );
      SNS_SMGR_PRINTF3(MED, "ssi: populating SSI config. ix:%u cfg_id:%u err:%u", ix, sns_smgr_ssi_get_cfg_id(ix), err);
      sns_hw_update_ssi_reg_items(sns_smgr_ssi_get_cfg_id(ix), (uint8_t*)&ssi_cfg);
    }
    
    SNS_SMGR_PRINTF0(LOW, "ssi: last devinfo. setting intit state to ALL_INIT_AUTODETECT_DONE");
    sns_smgr.all_init_state = SENSOR_ALL_INIT_AUTODETECT_DONE;
    err = sns_smgr_req_reg_data( SNS_REG_GROUP_SSI_SENSOR_DEP_CFG0_V02, SNS_SMGR_REG_ITEM_TYPE_GROUP );
    sns_smgr.last_requested_sensor_dep_reg_group_id = SNS_REG_GROUP_SSI_SENSOR_DEP_CFG0_V02;
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_reg_ssi_config

===========================================================================*/
/*!
  @brief Process an SSI SMGR Config registry group.

  @detail
  @param
   Type - Group or Single item registry
   Id - Group ID or single item ID
   Length - bytes
   data_ptr - pointer to received data
   sns_resp - response error
  @return
   none
 */
/*=========================================================================*/
static void
sns_smgr_process_reg_ssi_config( sns_smgr_RegItemType_e Type, uint16_t Id,
                                 uint32_t Length, uint8_t* data_ptr, sns_common_resp_s_v01 sns_resp )
{
  int                            cfg_idx;
  uint16_t                       curr_id;
  uint32_t                       ix;
  sns_err_code_e                 err;
  sns_reg_ssi_smgr_cfg_group_s * cfg_group_ptr = (sns_reg_ssi_smgr_cfg_group_s *)data_ptr;
  smgr_sensor_cfg_s            * sensor_cfg_ptr;

  static bool                    valid_cfg = false;

  if( Id == 0 )
  {
    Id = sns_smgr.last_received_reg_group_id;
  }

  cfg_idx = sns_smgr_ssi_get_cfg_idx(Id);

  SNS_SMGR_PRINTF2(HIGH, "ssi: proc ssi id %i cfg %i", Id, cfg_idx);
  if(sns_resp.sns_err_t != SENSOR1_SUCCESS)
  {
    SNS_SMGR_PRINTF3(LOW, "ssi: resp_err=%u, id=%u, last_id=%u", sns_resp.sns_err_t, Id, sns_smgr.last_received_reg_group_id);
    cfg_idx = sns_smgr_ssi_get_cfg_idx(sns_smgr.last_received_reg_group_id);
    if( sns_smgr_ssi_is_last_cfg(sns_smgr.last_received_reg_group_id) )
    {
      SNS_SMGR_PRINTF0(LOW, "ssi: Setting init state to ALL_INIT_CONFIGURED");
      sns_hw_set_smgr_ssi_recv(true);
      sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
      SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
    }
  }
  else if ( cfg_group_ptr->maj_ver_no != 1 && !valid_cfg )
  {
    /* Only use the configuration if the major version is 1. Otherwise autodetect
     * sensors */
    SNS_SMGR_PRINTF1(MED, "ssi: proc ssi maj ver no = %i", cfg_group_ptr->maj_ver_no);
    sns_smgr.all_init_state = SENSOR_ALL_INIT_WAITING_AUTODETECT;

    curr_id = SNS_SMGR_SSI_GET_FIRST_DEVINFO_ID();
    err = sns_smgr_req_reg_data( curr_id, SNS_SMGR_REG_ITEM_TYPE_GROUP );
    do
    {
      curr_id = (uint16_t) sns_smgr_ssi_get_next_devinfo_id(curr_id);
      err = sns_smgr_req_reg_data( curr_id, SNS_SMGR_REG_ITEM_TYPE_GROUP );
    } while( ! SNS_SMGR_SSI_IS_LAST_DEVINFO(curr_id) );
  }
  else
  {
    sns_hw_update_ssi_reg_items(Id, data_ptr);

    for ( ix = 0; smgr_sensor_cfg_cnt < SNS_SMGR_NUM_SENSORS_DEFINED &&
            ix < SNS_REG_SSI_SMGR_CFG_NUM_SENSORS; ix++ )
    {
      sensor_cfg_ptr = &smgr_sensor_cfg[smgr_sensor_cfg_cnt];
      smgr_sensor_cfg_cnt++;

      sensor_cfg_ptr->drv_fn_ptr = sns_smgr_uuid_to_fn_ptr(cfg_group_ptr->drv_cfg[ix].drvuuid );
      SNS_OS_MEMCOPY(sensor_cfg_ptr->uuid, cfg_group_ptr->drv_cfg[ix].drvuuid, sizeof(sensor_cfg_ptr->uuid));
      sensor_cfg_ptr->off_to_idle_time = (uint16_t)((cfg_group_ptr->drv_cfg[ix].off_to_idle/SNS_SMGR_USEC_PER_TICK)+1);
      sensor_cfg_ptr->idle_to_ready_time = (uint16_t)((cfg_group_ptr->drv_cfg[ix].idle_to_ready/SNS_SMGR_USEC_PER_TICK)+1);
      sensor_cfg_ptr->bus_instance = cfg_group_ptr->drv_cfg[ix].i2c_bus;
      if(  cfg_group_ptr->drv_cfg[ix].reg_group_id == 0xFFFF ) {
        sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
        sensor_cfg_ptr->driver_reg_id = 0;
      } else {
        sensor_cfg_ptr->driver_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
        sensor_cfg_ptr->driver_reg_id = cfg_group_ptr->drv_cfg[ix].reg_group_id;
      }
      if( cfg_group_ptr->drv_cfg[ix].cal_pri_group_id == 0xFFFF ) {
        sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_NONE;
        sensor_cfg_ptr->primary_cal_reg_id = 0;
      } else {
        sensor_cfg_ptr->primary_cal_reg_type = SNS_SMGR_REG_ITEM_TYPE_GROUP;
        sensor_cfg_ptr->primary_cal_reg_id = cfg_group_ptr->drv_cfg[ix].cal_pri_group_id;
      }
      sensor_cfg_ptr->first_gpio = cfg_group_ptr->drv_cfg[ix].gpio1;
      sensor_cfg_ptr->second_gpio = cfg_group_ptr->drv_cfg[ix].gpio2;
      sensor_cfg_ptr->sensor_id = cfg_group_ptr->drv_cfg[ix].sensor_id;
      sensor_cfg_ptr->bus_addr = cfg_group_ptr->drv_cfg[ix].i2c_address;
      sensor_cfg_ptr->data_types[0] = cfg_group_ptr->drv_cfg[ix].data_type1;
      sensor_cfg_ptr->data_types[1] = cfg_group_ptr->drv_cfg[ix].data_type2;
      sensor_cfg_ptr->sensitivity_default = cfg_group_ptr->drv_cfg[ix].sensitivity_default;
      sensor_cfg_ptr->flags = cfg_group_ptr->drv_cfg[ix].flags;

      if( cfg_group_ptr->min_ver_no > 0 ) {
        sensor_cfg_ptr->device_select = cfg_group_ptr->drv_cfg[ix].device_select;
      }
      if( sensor_cfg_ptr->drv_fn_ptr != NULL )
      {
        valid_cfg = true;
      }
    }

    if ( !sns_smgr_ssi_is_last_cfg(Id) )
    {
      curr_id = Id;
      do
      {
        curr_id = (uint16_t) sns_smgr_ssi_get_next_cfg_id(curr_id);
        err = sns_smgr_req_reg_data( curr_id, SNS_SMGR_REG_ITEM_TYPE_GROUP );
      } while( ! sns_smgr_ssi_is_last_cfg(curr_id) );
    }
    else
    {
      sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
      SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
    }
  }  // else (no auto-detect)
}

/*===========================================================================

  FUNCTION:   sns_smgr_process_reg_data

===========================================================================*/
/*!
  @brief Receive registry data. Find where it was requested and apply it.

  @detail
  @param
   Type - Group or Single item registry
   Id - Group ID or single item ID
   Length - bytes
   data_ptr - pointer to received data
   sns_resp - response error
  @return
   none
 */
/*=========================================================================*/
void sns_smgr_process_reg_data( sns_smgr_RegItemType_e Type, uint16_t Id,
                                uint32_t Length, uint8_t* data_ptr, sns_common_resp_s_v01 sns_resp )
{
  smgr_sensor_s             *sensor_ptr;
  smgr_sensor_cfg_s         *sensor_cfg_ptr;
  uint32_t                  ix, iy;

  SNS_SMGR_PRINTF1(LOW, "proc reg data: Id:%u", Id );

  if( sns_smgr_ssi_get_devinfo_idx(Id) != -1 ||
      (sns_resp.sns_err_t != SENSOR1_SUCCESS
       && sns_smgr_ssi_get_devinfo_idx(sns_smgr.last_received_reg_group_id) != -1) )
  {
    sns_smgr_process_reg_devinfo( Type, Id, Length, data_ptr, sns_resp );
    return;
  }

  if( sns_smgr_ssi_get_cfg_idx(Id) != -1 ||
      (sns_resp.sns_err_t != SENSOR1_SUCCESS
       && sns_smgr_ssi_get_cfg_idx(sns_smgr.last_received_reg_group_id) != -1) )
  {
    sns_smgr_process_reg_ssi_config( Type, Id, Length, data_ptr, sns_resp );
    return;
  }

  if(sns_resp.sns_err_t != SENSOR1_SUCCESS)
  {
    SNS_SMGR_PRINTF3(ERROR, "resp_err=%u, id=%u, last_id=%u", sns_resp.sns_err_t, Id, sns_smgr.last_received_reg_group_id);

    if( Id == sns_smgr.last_requested_sensor_dep_reg_group_id )
    {
      SNS_SMGR_PRINTF1(ERROR, "invalid DEP reg id=%u", sns_smgr.last_requested_sensor_dep_reg_group_id);
    }
    else
    {
      SNS_SMGR_PRINTF1(ERROR, "Unknown reg id=%u", sns_smgr.last_received_reg_group_id);
    }
  }
  else if ( Id == sns_smgr.last_requested_sensor_dep_reg_group_id )
  {
    sns_reg_ssi_sensor_dep_reg_group_s * cfg_group_ptr = (sns_reg_ssi_sensor_dep_reg_group_s*)data_ptr;
    SNS_SMGR_PRINTF1(LOW, "SMGR processing sensor dep reg item group %d", Id);

    if( cfg_group_ptr->ver_no == 0 )
    {
      SNS_SMGR_PRINTF0(HIGH, "Registry group is invalid");
      sns_smgr.last_requested_sensor_dep_reg_group_id = 0xFFFF;
      sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
      SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
    }
    else
    {
      sns_smgr_process_sensor_dep_reg_data( cfg_group_ptr );
      if( cfg_group_ptr->next_group_id != 0 )
      {
        sns_smgr_req_reg_data( cfg_group_ptr->next_group_id, SNS_SMGR_REG_ITEM_TYPE_GROUP );
        sns_smgr.last_requested_sensor_dep_reg_group_id = cfg_group_ptr->next_group_id;
      }
      else
      {
        SNS_SMGR_PRINTF0(HIGH, "Setting init_state to ALL_INIT_CONFIGURED");
        sns_smgr.last_requested_sensor_dep_reg_group_id = 0xFFFF;
        sns_smgr.all_init_state = SENSOR_ALL_INIT_CONFIGURED;
        SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_DD_INIT_B);
      }
    }
  }
  else if( Id == SNS_REG_GROUP_SSI_GPIO_CFG_V02 )
  {
    sns_hw_update_ssi_reg_items(Id, data_ptr);
  }
  else
  {
    for ( ix = 0; ix < SNS_SMGR_NUM_SENSORS_DEFINED; ix++ )
    {
      sensor_ptr = &sns_smgr.sensor[ix];
      sensor_cfg_ptr = sensor_ptr->const_ptr;

      /* skip any non-existent sensor */
      if ( NULL == SMGR_DRV_FN_PTR(sensor_ptr) )
      {
        continue;
      }

      if ( SMGR_BIT_TEST(sensor_ptr->flags, SMGR_SENSOR_FLAGS_REG_REQ_DRIVER_B) &&
           sensor_cfg_ptr->driver_reg_type == Type &&
           sensor_cfg_ptr->driver_reg_id == Id &&
           SENSOR_INIT_WAITING_REG == sensor_ptr->init_state)
      {
        /* Need to copy data for driver because response message will be freed
           before init can happen */
        if ( 0 != (sensor_ptr->reg_item_param.data = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_SMGR, Length )))
        {
          HEAP_STAT(__LINE__, Length, ix);

          for ( iy = 0; iy < Length; iy++ )
          {
            sensor_ptr->reg_item_param.data[iy] = data_ptr[iy];
          }
          sensor_ptr->reg_item_param.nvitem_grp = Type;
          sensor_ptr->reg_item_param.nvitem_id = Id;
          sensor_ptr->reg_item_param.status = SNS_DDF_SUCCESS;
          sensor_ptr->reg_item_param.data_len = Length;
          sensor_ptr->init_state = SENSOR_INIT_REG_READY;
        }
        else
        {
           /* No memory. At timeout we will discover registry request failed,
              thus init will fail. Very unlikely */
          SNS_PRINTF_STRING_ID_HIGH_2(SNS_DBG_MOD_DSPS_SMGR,
                                      DBG_SMGR_GENERIC_STRING2,
                                      ix, sensor_cfg_ptr->driver_reg_id);
        }
      }
      else if ( (sensor_cfg_ptr->primary_cal_reg_type == Type) &&
                (sensor_cfg_ptr->primary_cal_reg_id == Id) )
      {
        q16_t *reg_data_ptr = (q16_t *)data_ptr;
        smgr_ddf_sensor_s* ddf_sensor_ptr =
              sensor_ptr->ddf_sensor_ptr[SNS_SMGR_DATA_TYPE_PRIMARY_V01];
        SNS_ASSERT(Length >= (sizeof(q16_t) * SNS_SMGR_SENSOR_DIMENSION_V01 * 2));    /* scale factors and biases */

        if ( Id == SNS_REG_SCM_GROUP_MAG_FAC_CAL_PARAMS_V02 )
        {
          smgr_cal_s* cal_db_ptr;
          sns_reg_mag_fac_cal_params_data_group_s *mag_reg_data_ptr =
            (sns_reg_mag_fac_cal_params_data_group_s *)data_ptr;

          /* Reset all autocal data */
          ddf_sensor_ptr->auto_cal.used = FALSE;
          smgr_load_default_cal( &(ddf_sensor_ptr->auto_cal) );
          ddf_sensor_ptr->factory_cal.used = TRUE;
          ddf_sensor_ptr->full_cal.used = TRUE;

          cal_db_ptr = &ddf_sensor_ptr->factory_cal;

          if ( mag_reg_data_ptr->bias_valid )
          {
            cal_db_ptr->zero_bias_len = SNS_SMGR_SENSOR_DIMENSION_V01;

            cal_db_ptr->zero_bias[0] = ddf_sensor_ptr->full_cal.zero_bias[0] =
              mag_reg_data_ptr->x_fac_bias;
            cal_db_ptr->zero_bias[1] = ddf_sensor_ptr->full_cal.zero_bias[1] =
              mag_reg_data_ptr->y_fac_bias;
            cal_db_ptr->zero_bias[2] = ddf_sensor_ptr->full_cal.zero_bias[2] =
              mag_reg_data_ptr->z_fac_bias;

            SNS_SMGR_PRINTF3(LOW, "mag fac cal zero_bias [0]:%d, [1]:%d, [2]:%d",
                             cal_db_ptr->zero_bias[0],
                             cal_db_ptr->zero_bias[1],
                             cal_db_ptr->zero_bias[2]);
          }

          if ( mag_reg_data_ptr->cal_mat_valid )
          {
            cal_db_ptr->compensation_matrix_valid = ddf_sensor_ptr->full_cal.compensation_matrix_valid = true;

            cal_db_ptr->compensation_matrix[0] = ddf_sensor_ptr->full_cal.compensation_matrix[0] =
              mag_reg_data_ptr->compensation_matrix[0][0];
            cal_db_ptr->compensation_matrix[1]  = ddf_sensor_ptr->full_cal.compensation_matrix[1] =
              mag_reg_data_ptr->compensation_matrix[0][1];
            cal_db_ptr->compensation_matrix[2] = ddf_sensor_ptr->full_cal.compensation_matrix[2] =
              mag_reg_data_ptr->compensation_matrix[0][2];
            cal_db_ptr->compensation_matrix[3] = ddf_sensor_ptr->full_cal.compensation_matrix[3] =
              mag_reg_data_ptr->compensation_matrix[1][0];
            cal_db_ptr->compensation_matrix[4] = ddf_sensor_ptr->full_cal.compensation_matrix[4] =
              mag_reg_data_ptr->compensation_matrix[1][1];
            cal_db_ptr->compensation_matrix[5] = ddf_sensor_ptr->full_cal.compensation_matrix[5] =
              mag_reg_data_ptr->compensation_matrix[1][2];
            cal_db_ptr->compensation_matrix[6] = ddf_sensor_ptr->full_cal.compensation_matrix[6] =
              mag_reg_data_ptr->compensation_matrix[2][0];
            cal_db_ptr->compensation_matrix[7] = ddf_sensor_ptr->full_cal.compensation_matrix[7] =
              mag_reg_data_ptr->compensation_matrix[2][1];
            cal_db_ptr->compensation_matrix[8] = ddf_sensor_ptr->full_cal.compensation_matrix[8] =
              mag_reg_data_ptr->compensation_matrix[2][2];

            SNS_SMGR_PRINTF3(LOW, "mag fac cal compensation_matrix: [0]:%d, [1]:%d, [2]:%d",
                   cal_db_ptr->compensation_matrix[0],
                   cal_db_ptr->compensation_matrix[1],
                   cal_db_ptr->compensation_matrix[2]);
            SNS_SMGR_PRINTF3(LOW, "mag fac cal compensation_matrix: [0]:%d, [1]:%d, [2]:%d",
                   cal_db_ptr->compensation_matrix[3],
                   cal_db_ptr->compensation_matrix[4],
                   cal_db_ptr->compensation_matrix[5]);
            SNS_SMGR_PRINTF3(LOW, "mag fac cal compensation_matrix: [0]:%d, [1]:%d, [2]:%d",
                   cal_db_ptr->compensation_matrix[6],
                   cal_db_ptr->compensation_matrix[7],
                   cal_db_ptr->compensation_matrix[8]);
          }          
        }
        else
        {
        /* Check the validity of data */
        if (TRUE == smgr_is_valid_fac_cal(reg_data_ptr))
        {
          smgr_cal_s* cal_db_ptr;
          uint32_t i;
					sns_ddf_status_e err;

          /* Reset all autocal data */
          ddf_sensor_ptr->auto_cal.used = FALSE;
          smgr_load_default_cal( &(ddf_sensor_ptr->auto_cal) );
          ddf_sensor_ptr->factory_cal.used = TRUE;
          ddf_sensor_ptr->full_cal.used = TRUE;

          cal_db_ptr = &ddf_sensor_ptr->factory_cal;
          cal_db_ptr->scale_factor_len = cal_db_ptr->zero_bias_len = SNS_SMGR_SENSOR_DIMENSION_V01;
          for ( i = 0; i < SNS_SMGR_SENSOR_DIMENSION_V01; i++)
          {
            cal_db_ptr->zero_bias[i] = ddf_sensor_ptr->full_cal.zero_bias[i] = reg_data_ptr[i];
            cal_db_ptr->scale_factor[i] = ddf_sensor_ptr->full_cal.scale_factor[i] =
                                          reg_data_ptr[i+SNS_SMGR_SENSOR_DIMENSION_V01];
          }

          /* DD support: writing calibration values during initialization */
          err = sns_smgr_set_attr(ddf_sensor_ptr->sensor_ptr,
                            SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
                            SNS_DDF_ATTRIB_BIAS,
                            ddf_sensor_ptr->full_cal.zero_bias);

          if(err != SNS_DDF_SUCCESS && err != SNS_DDF_EINVALID_PARAM)
          {
             SNS_SMGR_PRINTF2(ERROR, "failed to init bias to DD (ddf_sensor=%d, err=%d)",
                              SMGR_SENSOR_TYPE(ddf_sensor_ptr->sensor_ptr, ddf_sensor_ptr->data_type),
                              err);
          }
        }
      }
    }
  }
}
}

/*===========================================================================

  FUNCTION:   sns_smgr_req_reg_data

===========================================================================*/
/*!
  @brief This function requests data from registry

  @detail
  @param
   Id - identifier of single item or group
   Type - option to request single item or group
  @return
   error code
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_req_reg_data( const uint16_t Id, const uint8_t Type )
{
  void*            read_req_ptr = NULL;
  void*            read_resp_ptr;
  uint16_t         resp_msg_size;
  sns_smr_header_s msg_hdr;
#if 0
  msg_hdr.dst_module = SNS_MODULE_APPS_REG;
  msg_hdr.src_module = SNS_MODULE_DSPS_SMGR;
  msg_hdr.priority = SNS_SMR_MSG_PRI_HIGH;
  msg_hdr.txn_id = 0;
  msg_hdr.ext_clnt_id = 0;
  msg_hdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
  msg_hdr.svc_num = SNS_REG2_SVC_ID_V01;
#endif

  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == Type )
  {
    msg_hdr.msg_id = SNS_REG_GROUP_READ_REQ_V02;
    msg_hdr.body_len = sizeof( sns_reg_group_read_req_msg_v02 );
    resp_msg_size = sizeof( sns_reg_group_read_resp_msg_v02 );
  }
  else if ( SNS_SMGR_REG_ITEM_TYPE_SINGLE == Type )
  {
    msg_hdr.msg_id = SNS_REG_SINGLE_READ_REQ_V02;
    msg_hdr.body_len = sizeof( sns_reg_single_read_req_msg_v02 );
    resp_msg_size = sizeof ( sns_reg_single_read_resp_msg_v02 );
  }
  else
  {
    return SNS_ERR_FAILED;
  }

  read_req_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, msg_hdr.body_len );
  read_resp_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, resp_msg_size);

  if( (NULL == read_req_ptr) || (NULL == read_resp_ptr) )
  {
    return SNS_ERR_NOMEM;
  }

  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == Type )
  {
    ((sns_reg_group_read_req_msg_v02*)read_req_ptr)->group_id = Id;
  }
  else
  {
    ((sns_reg_single_read_req_msg_v02*)read_req_ptr)->item_id = Id;
  }

  sns_smgr_send_req(&msg_hdr, read_req_ptr, read_resp_ptr, smgr_reg_cl_user_handle);

  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smgr_update_reg_data

===========================================================================*/
/*!
  @brief This function updates the registry data

  @detail
  @param
   Id - identifier of single item or group
   Type - option to request single item or group
   Length - bytes
   data_ptr - pointer to data that will update the registry
  @return
   error code
 */
/*=========================================================================*/
sns_err_code_e sns_smgr_update_reg_data( const uint16_t Id, const uint8_t Type,
                                         uint32_t Length, uint8_t* data_ptr )
{
#ifndef SNS_PCSIM
  void*            write_req_ptr = NULL;
  void*            write_resp_ptr;
  uint16_t         resp_msg_size;
  sns_smr_header_s msg_hdr;
#if 0
  msg_hdr.dst_module = SNS_MODULE_APPS_REG;
  msg_hdr.src_module = SNS_MODULE_DSPS_SMGR;
  msg_hdr.priority = SNS_SMR_MSG_PRI_LOW;
  msg_hdr.txn_id = 0;
  msg_hdr.ext_clnt_id = 0;
  msg_hdr.msg_type = SNS_SMR_MSG_TYPE_REQ;
  msg_hdr.svc_num = SNS_REG2_SVC_ID_V01;
#endif

  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == Type )
  {
    if (Length > SNS_REG_MAX_GROUP_BYTE_COUNT_V02)
    {
      return SNS_ERR_BAD_PARM;
    }
    msg_hdr.msg_id = SNS_REG_GROUP_WRITE_REQ_V02;
    msg_hdr.body_len = sizeof( sns_reg_group_write_req_msg_v02 );
    resp_msg_size = sizeof( sns_reg_group_write_resp_msg_v02 );
  }
  else if ( SNS_SMGR_REG_ITEM_TYPE_SINGLE == Type )
  {
    if (Length >= SNS_REG_MAX_ITEM_BYTE_COUNT_V02)
    {
      return SNS_ERR_BAD_PARM;
    }
    msg_hdr.msg_id = SNS_REG_SINGLE_WRITE_REQ_V02;
    msg_hdr.body_len = sizeof( sns_reg_single_write_req_msg_v02 );
    resp_msg_size = sizeof( sns_reg_single_write_resp_msg_v02 );
  }
  else
  {
    return SNS_ERR_FAILED;
  }

  write_req_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, msg_hdr.body_len );
  if ( NULL == write_req_ptr)
  {
    return SNS_ERR_NOMEM;
  }

  write_resp_ptr = sns_smr_msg_alloc(SNS_DBG_MOD_DSPS_SMGR, resp_msg_size);
  if ( NULL == write_resp_ptr )
  {
    sns_smr_msg_free(write_req_ptr);
    return SNS_ERR_NOMEM;
  }

  if ( SNS_SMGR_REG_ITEM_TYPE_GROUP == Type )
  {
    ((sns_reg_group_write_req_msg_v02*)write_req_ptr)->group_id = Id;
    ((sns_reg_group_write_req_msg_v02*)write_req_ptr)->data_len = Length;
    SNS_OS_MEMCOPY( ((sns_reg_group_write_req_msg_v02*)write_req_ptr)->data,
                     data_ptr, Length );
  }
  else
  {
    ((sns_reg_single_write_req_msg_v02*)write_req_ptr)->item_id = Id;
    ((sns_reg_single_write_req_msg_v02*)write_req_ptr)->data_len = Length;
    SNS_OS_MEMCOPY( ((sns_reg_single_write_req_msg_v02*)write_req_ptr)->data,
                    data_ptr, Length );
  }

  sns_smgr_send_req(&msg_hdr, write_req_ptr, write_resp_ptr, smgr_reg_cl_user_handle);
  return SNS_SUCCESS;
#else
  return SNS_ERR_FAILED;
#endif
}

