/*=============================================================================
  @file sns_smgr_hw.c

  This file contains hw related features used by SMGR.

*******************************************************************************
* Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
********************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_hw.c#2 $ */
/* $DateTime: 2014/05/29 07:31:11 $ */
/* $Author: rpoliset $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-05-27  dc   Pass in correct parameter to                    DalTlmm_SetInactiveConfig to fix compilation error when ADSP_HWCONFIG_L is enabled.
  2014-05-12  ak   Removed DalTlmm_SetInactiveConfig 
  2014-02-20  hw   Add L2 cache retention voting for sensors' usecase
  2014-02-03  TC   Added support for Accels 2 through 5, SAR 2, and RGB 2
  2013-12-18  MW   Added support for Hall Effect sensor
  2013-12-03  jhh  Remove vdd NPA node
  2013-11-22  sc   Fixed init error that caused GPIO0 to be wrongly configured
  2013-11-18  jhh  Configure reset pin to be low when no client on SMGR
  2013-10-23  MW   Added support for SAR sensor
  2013-10-18  jhh  Use sync client for latency node when sampling
  2013-10-18  pn   Completes GPIO config before starting SSI SMGR config
  2013-09-23  pn   Simplified sns_hw_power_rail_config()
  2013-09-12  MW   Added support for RGB sensor type
  2013-09-03  jhh  Update PNOC BW value to achieve Fmax SVS
  2013-08-21  jhh  Separate LDO and LVS control for 8x26
  2013-08-13  sc   Added check for GPIO number; don't configure if invalid
  2013-08-09  ps   Eliminate compilier warnings when ADSP_STANDALONE is defined
  2013-07-12  lka  Added compile-time configurations
  2013-07-15  dc   Support for humidity sensor type
  2013-07-12  lka  Added compile-time configurations
  2013-06-23  lka  Increased I2C bus instances based on device info count.
  2013-06-12  agk  Enabled async latency node voting mechanism
  2013-06-11  ae   Added QDSP SIM playback support
  2013-06-03  br   Inserted PNOC clock voting
  2013-04-17  dc   Read the I2C Bus information from registry
  2013-04-10  dc   Read the GPIO and I2C information from registry
  2013-04-12  ag   Reverted change that separates control for LDO and LVS temporaily for 8x26 only
  2013-03-29  vh   Changing the power-rail status conditionally
  2013-03-08  ag   Enable e-2-e without power manager and RPM communication
  2013-03-06  jhh  Add separate control for LDO and LVS
  2013-03-05  ag   Update accel gpio # for 8x26
  2013-02-26  ag   Keep track of sns_smgr_hw.i2c_clk.qupclk_on on QDSP6 targets
  2013-02-25  ps   Added this file under 8x26 directory, defined 8x26 bus instances
  2013-02-14  ag   Changes for I2C optimizations to reduce clock ON/OFF time
  2013-01-08  sd   change sensor state off when turning off power rail
  2013-01-03  jhh  Add pmic control API back to SMGR
  2012-12-26  pn   Updated message macros
  2012-12-03  pn   Added support for Buffering feature
  2012-08-31  sc   Updated GPIO configuration for 8974 target
  2012-08-29  ag   Disabled NPA calls until they are fixed
  2012-06-27  sd   Fixed Klocwork error/warning 
  2012-05-25  rp   Removed redundant latency input in sns_hw_init_power  
  2012-04-30  br   Added latency node support
  2012-03-06  pn   added sns_hw_gpio_out_config() and sns_hw_gpio_out()
  2012-03-05  sd   moved DRI gpio control to DD and inserted latency node API
  2012-02-15  br   Changed default inactive config to NO_PULL for GPIOs for I2C and interrupts
  2012-02-13  br   Changed default inactive configuration for GPIOs for I2C and interrupts
  2012-02-08  sd   Add MD support for MPU6050
  2012-01-16  sd   Enable/Disable cc_gsbi12_hclk/gpio44/gpio45 when enable/disable qup clock
  2011-12-27  sd   Added sns_hw_qup_clck_status
  2011-10-27  sd   Put gpio53 to high to make pressure sensor to low power mode
  2011-09-14  sd   set sns_smgr_hw.powerrail_timer_pending = false if register time failed.
  2011-09-13  sd   updated some debug message
  2011-09-09  sd   removed extra msg to change power rail state before setting hysteresis  timer
                   added some comments.
  2011-08-30  sd   changed GPIO10_ACCEL_INT1 from 10 to 67 for LIS3DH using int2
  2011-08-29  sd   Set reset pin for BMP085 high at DSPS initialization
  2011-08-26  sc   Fixed compilation error for PCSIM
  2011-08-23  sd   added LD09 state transition
  2011-07-05  sc   Fixed compilation warnings
  2011-06-17  sd   smgr code clean up
  2011-06-02  SD   Use DalTlmm_ConfigGpio to config gpio low/high power
  2011-05-16  DC   Changed DDITLMM.h to DDITlmm.h to compile on linux(case sensitivity)
  2011-05-10  SD   dont turn power rail off until smgr can handle reinitialize DD after turning it
                  on again
  2011-05-06  SD   added sns_hw_init_i2c_clk in sns_hw_bd_config
  2011-05-05  sc   Commented out DAL-related functions for PCSIM build
  2011-05-03  SD   Initial version for 8960
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_memmgr.h"
#include "sns_smgr_define.h"
#include "sns_smgr_hw.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_common.h"
#include "sns_reg_common.h"

#if !defined(SNS_PCSIM)
#include "DDITlmm.h"
#include "DALDeviceId.h"
#include "npa.h"
#include "npa_resource.h"
#include <DDIClock.h>
#include "pm.h"
#include "DalDevice.h"
#include "ddii2c.h"
#include "mmpm.h"

//#define SNS_DSPS_LATENCY_NODE_ENABLED
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SNS_SMGR_MAX_GSBI_INSTANCE ARR_SIZE(sns_i2c_bus_table)
#define SNS_SMGR_HW_INVALID_GPIO (0xFFFF)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#if !defined(SNS_PCSIM)
typedef struct
{
  npa_client_handle         npa_handle;
  npa_client_handle         reqWakeUpClient;
  npa_client_handle         reqSyncLatencyClient; /* Use this for all latency voting for use cases */
  npa_client_handle         reqL2Client; /* Use this for L2 cache retention voting */
  npa_client_handle         reqSampleLatencyClient; /* Use this client to enable/disable PC only */
  npa_client_handle         pmic_client_ldo;
  npa_client_handle         pmic_client_lvs;
  uint32                    mmpm_client_id;
  uint32_t                  latency_us;     /* 0 means no request for latency */ 
} sns_power_s;
#endif

typedef struct
{
  uint16_t  i2c_sda_1;
  uint16_t  i2c_scl_1;
  uint16_t  i2c_sda_2;
  uint16_t  i2c_scl_2;
  uint16_t  sensor_reset;  
  uint16_t  gyro_int;
  uint16_t  mag_int;
  uint16_t  als_int;
  uint16_t  prx_int;
  uint16_t  press_int;
  uint16_t  accel_int2;
  uint16_t  accel_int1;
  uint16_t  ir_gesture_int;
  uint16_t  tap_int;
  uint16_t  humidity_int;
  uint16_t  rgb_int;
  uint16_t  sar_int;
  uint16_t  hall_effect_int;
  uint16_t  accel2_int2;
  uint16_t  accel2_int1;
  uint16_t  accel3_int2;
  uint16_t  accel3_int1;
  uint16_t  accel4_int2;
  uint16_t  accel4_int1;
  uint16_t  accel5_int2;
  uint16_t  accel5_int1;
  uint16_t  rgb2_int;
  uint16_t  sar2_int;
}sns_int_config_s;

typedef struct  
{
  bool                       qupclk_on;
#if !defined (SNS_PCSIM) && !defined (QDSP6)
  /* clock handle */
  DalDeviceHandle           *h_clock;
  /* cc_gsbi12_qup_apps_clk - 8974*/
  /* cc_gsbi2_qup_apps_clk - 8x26*/
  ClockIdType                gsbi_qup_apps_clk_id;  
  /* cc_gsbi12_hclk - 8974*/
  /* cc_gsbi2_hclk - 8x26*/
  ClockIdType                gsbi_hclk_id;
#endif
}sns_hw_i2c_clk;

typedef struct {
  int           i2c_bus;
  DALDEVICEID   i2c_bus_id;
} sns_i2c_bus_s;

static sns_i2c_bus_s sns_i2c_bus_table[] = 
{
  {  0, 0x0 },
  {  1, DALDEVICEID_I2C_DEVICE_1},
  {  2, DALDEVICEID_I2C_DEVICE_2},
  {  3, DALDEVICEID_I2C_DEVICE_3},
  {  4, DALDEVICEID_I2C_DEVICE_4},
  {  5, DALDEVICEID_I2C_DEVICE_5},
  {  6, DALDEVICEID_I2C_DEVICE_6},
  {  7, DALDEVICEID_I2C_DEVICE_7},
  {  8, DALDEVICEID_I2C_DEVICE_8},
  {  9, DALDEVICEID_I2C_DEVICE_9},
  { 10, DALDEVICEID_I2C_DEVICE_10},
  { 11, DALDEVICEID_I2C_DEVICE_11},
  { 12, DALDEVICEID_I2C_DEVICE_12}
};

typedef struct
{
  smgr_power_state_type_e    powerrail_state;
  bool                       powerrail_timer_pending;
  smgr_power_state_type_e    powerrail_pending_state;
  uint32_t                   powerail_timer_value_in_tick;
#if !defined(SNS_PCSIM)
  DalDeviceHandle           *gpio_handle;
  /* this is for MD interrupt, to be moved to DD control */
  uint32_t                   gpio_md_pin_cfg;
  /* gpio 6 is for BLSP3 I2C bus on 8226*/
  /* gpio 10 is for BLSP3 I2C bus on 8974*/
  uint32_t                   sda_1_pin;
  /* gpio 7 is for BLSP3 I2C bus on 8226*/
  /* gpio 11 is for BLSP3 I2C bus on 8974 */
  uint32_t                   scl_1_pin;
  /* only 1 i2c bus on 8226 */
  /* gpio 87 is for BLSP12 I2C bus on 8974 */
  uint32_t                   sda_2_pin;
  /* only 1 i2c bus on 8226 */
  /* gpio 88 is for BLSP12 I2C bus on 8974 */
  uint32_t                   scl_2_pin;
  uint32_t                   sensor_reset_gpio_pin_cfg;
  sns_power_s                power;
#endif
  sns_hw_i2c_clk             i2c_clk;
  sns_em_timer_obj_t         tmr_obj;
  DalDeviceHandle            *bus_handle[SNS_SMGR_MAX_GSBI_INSTANCE];
  bool                       b_smgr_ssi_recv;
  bool                       b_gpio_ssi_recv;
  sns_i2c_bus_s              sns_i2c_bus_instances[SNS_REG_SSI_SMGR_CFG_NUM_SENSORS*SNS_REG_SSI_NUM_SMGR_CFG];
  int                        sns_i2c_bus_count;
} sns_hw_s;

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
sns_hw_s   sns_smgr_hw;

/* initialize the gpios to INVALID */
sns_int_config_s sns_smgr_hw_gpio_cfg =
{
  SNS_SMGR_HW_INVALID_GPIO,  //i2c_sda_1;
  SNS_SMGR_HW_INVALID_GPIO,  //i2c_scl_1;
  SNS_SMGR_HW_INVALID_GPIO,  //i2c_sda_2;
  SNS_SMGR_HW_INVALID_GPIO,  //i2c_scl_2;
  SNS_SMGR_HW_INVALID_GPIO,  //sensor_reset;  
  SNS_SMGR_HW_INVALID_GPIO,  //gyro_int;
  SNS_SMGR_HW_INVALID_GPIO,  //mag_int;
  SNS_SMGR_HW_INVALID_GPIO,  //als_int;
  SNS_SMGR_HW_INVALID_GPIO,  //prx_int;
  SNS_SMGR_HW_INVALID_GPIO,  //press_int;
  SNS_SMGR_HW_INVALID_GPIO,  //accel_int2;
  SNS_SMGR_HW_INVALID_GPIO,  //accel_int1;
  SNS_SMGR_HW_INVALID_GPIO,  //ir_gesture_int;
  SNS_SMGR_HW_INVALID_GPIO,  //tap_int;
  SNS_SMGR_HW_INVALID_GPIO,  //humidity_int;
  SNS_SMGR_HW_INVALID_GPIO,  //rgb_int;
  SNS_SMGR_HW_INVALID_GPIO,  //sar_int;
  SNS_SMGR_HW_INVALID_GPIO,  //hall_effect_int;
  SNS_SMGR_HW_INVALID_GPIO,  //accel2_int2;
  SNS_SMGR_HW_INVALID_GPIO,  //accel2_int1;
  SNS_SMGR_HW_INVALID_GPIO,  //accel3_int2;
  SNS_SMGR_HW_INVALID_GPIO,  //accel3_int1;
  SNS_SMGR_HW_INVALID_GPIO,  //accel4_int2;
  SNS_SMGR_HW_INVALID_GPIO,  //accel4_int1;
  SNS_SMGR_HW_INVALID_GPIO,  //accel5_int2;
  SNS_SMGR_HW_INVALID_GPIO,  //accel5_int1;
  SNS_SMGR_HW_INVALID_GPIO,  //rgb2_int;
  SNS_SMGR_HW_INVALID_GPIO   //sar2_int;
};

#if defined(QDSP6) && !defined(SNS_QDSP_SIM)
/*===========================================================================

  FUNCTION:   sns_hw_powerrail_state_chng_condn

===========================================================================*/
/*!
  @brief Checks to change the power rail state or not

  @detail

  @param[in] None
  @return true or false
 */
/*=========================================================================*/

static bool sns_hw_powerrail_state_chng_condn(void)
{
  smgr_sensor_s* sensor_ptr;
  uint8_t        i;
  int            reports_in_queue;
  bool           state_chng_cndn = true;

  reports_in_queue = sns_q_cnt(&sns_smgr.report_queue);
  if(reports_in_queue > 0)
  {
    SNS_SMGR_PRINTF1(HIGH, "not changing state, reports_in_queue=%d", reports_in_queue);
    state_chng_cndn = false;
  }
  else
  {
    for ( i=0; i<SNS_SMGR_NUM_SENSORS_DEFINED ;i++ )
    {
      sensor_ptr = &sns_smgr.sensor[i];
      if(sensor_ptr->sensor_state == SENSOR_STATE_TESTING)
      {
        SNS_SMGR_PRINTF1(HIGH, "not changing state, testing sensor=%d", i);
        state_chng_cndn = false;
        break;
      }
    }
  }

  return state_chng_cndn;
}
#endif

#if !defined(SNS_PCSIM)
/*===========================================================================

  FUNCTION:   sns_hw_gpio_config_i2c_sda_scl

===========================================================================*/
/*!
  @brief Configuring GPIO pins for I2C buses

  @detail

  @param enable_flag  true : enable GPIO pins; false: disable GPIO pins
  @return
   DAL_SUCCESS -- GPIO was successfully configured as requested.\n
   DAL_ERROR -- Either an invalid GPIO number in the configuration or ownership
   cannot be obtained from a secure root.
 */
/*=========================================================================*/
int sns_hw_gpio_config_i2c_sda_scl(boolean enable_flag)
{
  int status;
  if ( true == enable_flag )
  {
    status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.sda_1_pin, DAL_TLMM_GPIO_ENABLE );
    if ( DAL_ERROR == status )
    {
      return status;
    }  
    status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.scl_1_pin, DAL_TLMM_GPIO_ENABLE );
    if ( DAL_ERROR == status )
    {
      return status;
    }  
    status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.sda_2_pin, DAL_TLMM_GPIO_ENABLE );
    if ( DAL_ERROR == status )
    {
      return status;
    }  
    return DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.scl_2_pin, DAL_TLMM_GPIO_ENABLE );
  }
  else
  {
    status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.sda_1_pin, DAL_TLMM_GPIO_DISABLE );
    if ( DAL_ERROR == status )
    {
      return status;
    }  
    status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.scl_1_pin, DAL_TLMM_GPIO_DISABLE );
    if ( DAL_ERROR == status )
    {
      return status;
    }  
    status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.sda_2_pin, DAL_TLMM_GPIO_DISABLE );
    if ( DAL_ERROR == status )
    {
      return status;
    }  
    return DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.scl_2_pin, DAL_TLMM_GPIO_DISABLE );
  }
}
#endif  /* ifndef SNS_PCSIM */

/*===========================================================================

  FUNCTION:   sns_hw_gpio_out_config

===========================================================================*/
/*!
  @brief Configures GPIO pin for the given GPIO number as output.

  @detail

  @param [in] gpio_num : the GPIO number for the pin to be configured
  @return
   DAL_SUCCESS -- GPIO was successfully configured as output.\n
   DAL_ERROR   -- the GPIO pin is not usable as ouput
 */
/*=========================================================================*/
int sns_hw_gpio_out_config(uint16_t gpio_num)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
  int result = DAL_ERROR;
  DALGpioSignalType gpio_cfg;

  if (gpio_num == SNS_SMGR_HW_INVALID_GPIO)
  {
    return result;
  }

  gpio_cfg = DAL_GPIO_CFG_OUT(gpio_num, 
                              0, 
                              DAL_GPIO_OUTPUT,
                              DAL_GPIO_NO_PULL, 
                              DAL_GPIO_2MA,
                              DAL_GPIO_LOW_VALUE);
  result = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle,
                              gpio_cfg, 
                              DAL_TLMM_GPIO_ENABLE);
  if( DAL_SUCCESS != result )
  {
    SNS_SMGR_DEBUG1(ERROR, DBG_SMGR_GENERIC_STRING1, gpio_num);
  }
  return result;
#else
  return 0;
#endif

}

/*===========================================================================

  FUNCTION:   sns_hw_gpio_out

===========================================================================*/
/*!
  @brief Pulls the GPIO pin up/down depending on input.

  @detail

  @param [in] gpio_num  : the GPIO number for the pin to be triggered
  @param [in] b_to_high : TRUE to go high, FALSE low
  @return
   DAL_SUCCESS -- GPIO was successfully triggered.\n
   DAL_ERROR   -- the GPIO pin is not usable as ouput
 */
/*=========================================================================*/
int sns_hw_gpio_out(uint16_t gpio_num, boolean b_to_high)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
  int result = DAL_ERROR;
  DALGpioSignalType gpio_cfg;

  if (gpio_num == SNS_SMGR_HW_INVALID_GPIO)
  {
    return result;
  }

  gpio_cfg = DAL_GPIO_CFG_OUT(gpio_num, 
                              0, 
                              DAL_GPIO_OUTPUT,
                              DAL_GPIO_NO_PULL, 
                              DAL_GPIO_2MA,
                              DAL_GPIO_LOW_VALUE);
  result = DalTlmm_GpioOut(sns_smgr_hw.gpio_handle, 
                           gpio_cfg, 
                           b_to_high? DAL_GPIO_HIGH_VALUE : DAL_GPIO_LOW_VALUE);
  if( DAL_SUCCESS != result )
  {
    SNS_SMGR_DEBUG2(ERROR, DBG_SMGR_GENERIC_STRING2, gpio_num, b_to_high);
  }

  return result;
#else
  return 0;
#endif
}

#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
/*===========================================================================

  FUNCTION:   sns_hw_gpio_inactive_config

===========================================================================*/
/*!
  @brief Configuring GPIO pins for inactive at DSPS initialization 

  @detail

  @param [in] gpio_handle_ptr : DAL GPIO handle
  @param [in] gpio_num : GPIO number
  @return
   none
 */
/*=========================================================================*/
static void sns_hw_gpio_inactive_config(DalDeviceHandle *gpio_handle_ptr, uint16_t gpio_num)
{
  DALResult status;
  uint32_t port_config;

  if (gpio_num != SNS_SMGR_HW_INVALID_GPIO)
  {
    port_config = DAL_GPIO_CFG(gpio_num , 0, DAL_GPIO_INPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);
    DalTlmm_SetInactiveConfig(gpio_handle_ptr, gpio_num , port_config);
    status = DalTlmm_ConfigGpio(gpio_handle_ptr, port_config, DAL_TLMM_GPIO_DISABLE);
    if ( DAL_ERROR == status )
    {
      SNS_SMGR_PRINTF1(FATAL, "inactive_config - gpio %d failed", gpio_num);
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_hw_int_and_i2c_gpio_inactive_config

===========================================================================*/
/*!
  @brief Configuring GPIO pins for inactive for interrupt/I2C bus,etc at SSC
         initialization
  @detail

  @param none
  @return
   none
 */
/*=========================================================================*/
void sns_hw_int_and_i2c_gpio_inactive_config(DalDeviceHandle *gpio_handle_ptr)
{
  /* I2C ports */
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.i2c_sda_1);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.i2c_scl_1);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.i2c_sda_2);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.i2c_scl_2);

#ifndef ADSP_HWCONFIG_L
  /* Interrupt GPIO pins */
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.accel_int2);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.gyro_int);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.mag_int);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.als_int);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.accel_int1);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.prx_int);
  sns_hw_gpio_inactive_config(gpio_handle_ptr, sns_smgr_hw_gpio_cfg.press_int);
#endif /* ADSP_HWCONFIG_L */
}
#endif

#ifdef ADSP_HWCONFIG_L
void sns_hw_int_gpio_inactive_config(uint16_t gpio_num)
{
  DALResult status;
  uint32_t port_config;

  /* Interrupt GPIO pins */
  port_config = DAL_GPIO_CFG(gpio_num, 0, DAL_GPIO_INPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);
  DalTlmm_SetInactiveConfig(sns_smgr_hw.gpio_handle, gpio_num , port_config);
  status = DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, port_config, DAL_TLMM_GPIO_DISABLE);
  if ( DAL_ERROR == status )
  {
    SNS_SMGR_PRINTF1(FATAL, "inactive_config - GPIO65_DFLT_ACCEL2_INT2 failed %d", 9972);
  }
}
#endif

/*===========================================================================

  FUNCTION:   sns_hw_int_and_i2c_gpio_config

===========================================================================*/
/*!
  @brief Configuring GPIO pins for motion interrpt/I2C bus,etc at DSPS initialization

  @detail

  @param none
  @return
   none
 */
/*=========================================================================*/
void sns_hw_int_and_i2c_gpio_config(void)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
  if(sns_smgr_hw.gpio_handle == NULL)
  {
    if( DAL_DeviceAttach(DALDEVICEID_TLMM, &sns_smgr_hw.gpio_handle) != DAL_SUCCESS)
    {
      SNS_SMGR_PRINTF1(FATAL, "gpio_config - DeviceAttach failed %d", 9920);
      return;
    }
  }

  /* configure inactive configuration the gpio pins */
  sns_hw_int_and_i2c_gpio_inactive_config(sns_smgr_hw.gpio_handle);

  sns_smgr_hw.sda_1_pin = DAL_GPIO_CFG(sns_smgr_hw_gpio_cfg.i2c_sda_1,1,DAL_GPIO_OUTPUT,DAL_GPIO_PULL_UP,
                                                     DAL_GPIO_2MA); 
  sns_smgr_hw.scl_1_pin = DAL_GPIO_CFG(sns_smgr_hw_gpio_cfg.i2c_scl_1,1,DAL_GPIO_OUTPUT,DAL_GPIO_PULL_UP,
                                                     DAL_GPIO_2MA);
  sns_smgr_hw.sda_2_pin = DAL_GPIO_CFG(sns_smgr_hw_gpio_cfg.i2c_sda_2,1,DAL_GPIO_OUTPUT,DAL_GPIO_PULL_UP,
                                                     DAL_GPIO_2MA); 
  sns_smgr_hw.scl_2_pin = DAL_GPIO_CFG(sns_smgr_hw_gpio_cfg.i2c_scl_2,1,DAL_GPIO_OUTPUT,DAL_GPIO_PULL_UP,
                                                     DAL_GPIO_2MA);

#ifndef ADSP_HWCONFIG_L
  /* configuring sensor reset pin, it will stay high during LD09 and DSPS power on/off, this also makes it in low power mode */
  if (sns_smgr_hw_gpio_cfg.sensor_reset != SNS_SMGR_HW_INVALID_GPIO)
  {
    sns_smgr_hw.sensor_reset_gpio_pin_cfg= DAL_GPIO_CFG_OUT(sns_smgr_hw_gpio_cfg.sensor_reset, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, 1);
    DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.sensor_reset_gpio_pin_cfg, DAL_TLMM_GPIO_ENABLE );
    if(sns_hw_gpio_config_i2c_sda_scl(false) == DAL_ERROR)
    {
      SNS_SMGR_PRINTF1(FATAL, "gpio_config - sns_hw_gpio_config_i2c_sda_scl failed %d", 9922);
    }
  }
#endif /* ADSP_HWCONFIG_L */

#endif
}

/*===========================================================================

  FUNCTION:   sns_hw_init_i2c_clk

===========================================================================*/
/*!
  @brief Initialize I2C related clocks

  @detail

  @param none
  @return
   none
 */
/*=========================================================================*/

void sns_hw_init_i2c_clk(void)
{
#if !defined(SNS_PCSIM) && !defined(QDSP6) && !defined(SNS_QDSP_SIM)
#ifndef SNS_EXCLUDE_POWER
  DALResult eResult = DAL_ERROR;
  /* Attach to the clock DAL */
  eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &sns_smgr_hw.i2c_clk.h_clock);
  if(eResult != DAL_SUCCESS)
  {
     SNS_SMGR_PRINTF1(FATAL, "init_i2c_clk - DAL_DeviceAttach failed %d", 9950);
     return;
  }
  eResult = DalClock_GetClockId(sns_smgr_hw.i2c_clk.h_clock, "cc_gsbi12_qup_apps_clk", &sns_smgr_hw.i2c_clk.gsbi_qup_apps_clk_id);

  if(eResult != DAL_SUCCESS)
  {
     SNS_SMGR_PRINTF1(FATAL, "init_i2c_clk - GetClockId for gsbi12_qup_apps failed %d", 
                      9951);
     return;
  }

  eResult = DalClock_GetClockId(sns_smgr_hw.i2c_clk.h_clock, "cc_gsbi12_hclk", &sns_smgr_hw.i2c_clk.gsbi_hclk_id);

  if(eResult != DAL_SUCCESS)
  {
     SNS_SMGR_PRINTF1(FATAL, "init_i2c_clk - GetClockId for gsbi12 failed %d", 9952);
     return;
  }
#endif
#endif
}


/*===========================================================================

  FUNCTION:   sns_hw_set_qup_clk

===========================================================================*/
/*!
  @brief Enable/Disable i2c related clocks & gpio pins

  @detail

  @param[in] enable_flag:  enable/disable flag
  @return
   none
 */
/*=========================================================================*/

void sns_hw_set_qup_clk(bool  enable_flag)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)

#if defined(QDSP6)

  uint8_t     i;
  DALResult   dal_result;
  if (sns_smgr_hw.i2c_clk.qupclk_on == enable_flag)
  {
     return;
  }

  if ( enable_flag )
  {
    /* open each bus instance to turn ON clock */
    /* this could be further optimized to turn ON only the clock for the right GSBI instance */
    for ( i=0; i<sns_smgr_hw.sns_i2c_bus_count; i++ )
    {
      dal_result = DalDevice_Open(sns_smgr_hw.bus_handle[i], DAL_OPEN_SHARED );
      if (DAL_SUCCESS != dal_result)
      {
        SNS_SMGR_PRINTF2(FATAL, "sns_hw_set_qup_clk - Bus %d Open failed %d", i, dal_result);
        return;
      }
    }
    sns_smgr_hw.i2c_clk.qupclk_on= true;
  }
  else
  { 
    /* close I2C device */
    for ( i=0; i<sns_smgr_hw.sns_i2c_bus_count; i++ )
    {
      DalDevice_Close(sns_smgr_hw.bus_handle[i]);
    }
    sns_smgr_hw.i2c_clk.qupclk_on= false;
  }
  return;

#else
#ifndef SNS_EXCLUDE_POWER
  if (sns_smgr_hw.i2c_clk.qupclk_on == enable_flag)
  {
     return;
  }

  if ( enable_flag )
  {
     /* Enable gsbi12_hclk Clock */
     if (DAL_SUCCESS != DalClock_EnableClock(sns_smgr_hw.i2c_clk.h_clock, sns_smgr_hw.i2c_clk.gsbi_hclk_id))
     {
        SNS_SMGR_PRINTF1(FATAL, "set_qup_clk - EnableClock for gsbi12 failed %d",
                         9960);
        return;
     }
     /* Enable QUP Clock */
     if (DAL_SUCCESS != DalClock_EnableClock(sns_smgr_hw.i2c_clk.h_clock, sns_smgr_hw.i2c_clk.gsbi_qup_apps_clk_id))
     {
        SNS_SMGR_PRINTF1(FATAL, "set_qup_clk - EnableClock for gsbi12_qup_apps failed",
                         9961);
        return;
     }
     sns_smgr_hw.i2c_clk.qupclk_on= true;
  }
  else
  {
     /* Disable QUP Clock */
     if (DAL_SUCCESS != DalClock_DisableClock(sns_smgr_hw.i2c_clk.h_clock, sns_smgr_hw.i2c_clk.gsbi_qup_apps_clk_id))
     {
       SNS_SMGR_PRINTF1(FATAL, "set_qup_clk - DisableClock for gsbi12_qup_apps failed %d",
                        9962);
       return;
     }
     /* Disable gsbi12_hclk */
     if (DAL_SUCCESS != DalClock_DisableClock(sns_smgr_hw.i2c_clk.h_clock, sns_smgr_hw.i2c_clk.gsbi_hclk_id))
     {
       SNS_SMGR_PRINTF1(FATAL, "set_qup_clk - DisableClock for gsbi12 failed %d",
                        9963);
       return ;
     } 
     sns_smgr_hw.i2c_clk.qupclk_on= false;
  }
#endif /* SNS_EXCLUDE_POWER */
#endif /* QDSP6 */
#endif  /* SNS_PCSIM */
}


/*===========================================================================

  FUNCTION:   sns_hw_init_i2c_bus_config

===========================================================================*/
/*!
  @brief Attach to DAL i2c device for each bus

  @detail

  @param
   none
 
  @return
   none
 */
/*=========================================================================*/
void sns_hw_init_i2c_bus_config(void)
{
  uint8_t     i;
  DALResult   dal_result;
  /* Attach to each bus instance */
  for ( i=0; i<sns_smgr_hw.sns_i2c_bus_count; i++ )
  {
    dal_result = DAL_I2CDeviceAttach(sns_smgr_hw.sns_i2c_bus_instances[i].i2c_bus_id, &(sns_smgr_hw.bus_handle[i]));
    if (DAL_SUCCESS != dal_result)
    {
      SNS_SMGR_PRINTF1(FATAL, "sns_hw_init_i2c_bus_config - GSBI %d attach failed.", i);
      return;
    }
  }
}


/*===========================================================================

  FUNCTION:   smgr_hw_pnoc_vote

===========================================================================*/
/*!
  @brief Voting PNOC ON/OFF using MMPM(Multi Media Power Management) client identifier

  @detail

  @param[in] mmpm_client_id: MMPM(Multi Media Power Management) client identifier
  @param[in] on_flag: vote flag if PNOC is ON
  @return
   none
 */
/*=========================================================================*/
void smgr_hw_pnoc_vote (uint32_t mmpm_client_id, bool on_flag)
{
  MmpmRscExtParamType req_rsc_param;
  MMPM_STATUS result_status[1];
  MmpmRscParamType req_param[1];
  MmpmGenBwValType bw_value[1];
  MmpmBwReqType bw_param;
  MMPM_STATUS req_result;

  SNS_SMGR_PRINTF2(MED, "PNOC vote, client_id=%d, on_flag=%d", mmpm_client_id, on_flag);

  if (!mmpm_client_id)
  {
    SNS_SMGR_PRINTF1(FATAL, "MMPM client_id is 0, on_flag=%d", on_flag);
    return;
  }

  /*sensors make a BW request from ADSP to PNOC */
  if (on_flag)
  {
    bw_value[0].bwValue.busBwValue.bwBytePerSec = 163577856;
	// Highest SVS Clock-rates upto 37.5, 50 MHz. Voting for 39 MHz so as to automatically get 50 MHz clock.
  }
  else
  {
    bw_value[0].bwValue.busBwValue.bwBytePerSec = 0;
  }
  bw_value[0].bwValue.busBwValue.usagePercentage = 100;
  bw_value[0].bwValue.busBwValue.usageType = MMPM_BW_USAGE_LPASS_DSP;
  bw_value[0].busRoute.masterPort = MMPM_BW_PORT_ID_ADSP_MASTER;
  bw_value[0].busRoute.slavePort = MMPM_BW_PORT_ID_PERIFNOC_SLAVE;
  bw_param.numOfBw = 1;
  bw_param.pBandWidthArray = (MmpmBwValType *)bw_value;

  req_param[0].rscId = MMPM_RSC_ID_GENERIC_BW;
  req_param[0].rscParam.pBwReq = &bw_param;

  req_rsc_param.apiType = MMPM_API_TYPE_SYNC;
  req_rsc_param.numOfReq = 1;
  req_rsc_param.reqTag = 0;
  req_rsc_param.pStsArray = result_status;
  req_rsc_param.pReqArray = req_param;
  req_result = MMPM_Request_Ext(mmpm_client_id, &req_rsc_param);
  if(MMPM_STATUS_SUCCESS != req_result)
  {
    SNS_SMGR_PRINTF1(FATAL, "MMPM_Request_Ext request failed, on_flag=%d", on_flag);
  }
}

/*===========================================================================

  FUNCTION:   sns_hw_send_powerrail_msg

===========================================================================*/
/*!
  @brief Sending RMP message to change the power state of power rail

  @detail

  @param[in] state:  the state of power rail needs to be updated to.
  @return
   none
 */
/*=========================================================================*/

void sns_hw_send_powerrail_msg(smgr_power_state_type_e state )
{
#if defined(QDSP6) && !defined(SNS_QDSP_SIM)
  if(state == SNS_SMGR_POWER_OFF)
  {
    if(false == sns_hw_powerrail_state_chng_condn())
    {
      return;
    }
  }


  // power rail may set up before calling sns_hw_int_and_i2c_gpio_config
  // initialize gpio handle with TLMM in case it reaches first
  if(sns_smgr_hw.gpio_handle == NULL)
  {
    if( DAL_DeviceAttach(DALDEVICEID_TLMM, &sns_smgr_hw.gpio_handle) != DAL_SUCCESS)
    {
      SNS_SMGR_PRINTF1(FATAL, "gpio_config - DeviceAttach failed %d", 9920);
      return; 
    }
  }


  sns_smgr_hw.powerrail_state = state;
  if( SNS_SMGR_POWER_OFF == sns_smgr_hw.powerrail_state )
  {
#ifndef SNS_EXCLUDE_POWER
    SNS_SMGR_PRINTF0(MED, "PMIC_NPA_MODE_ID_SENSOR_POWER_OFF");

    smgr_hw_pnoc_vote(sns_smgr_hw.power.mmpm_client_id, false);   /* PNOC(Peripheral NOC) vote for off*/
    npa_issue_required_request (sns_smgr_hw.power.pmic_client_lvs, PMIC_NPA_MODE_ID_SENSOR_POWER_OFF);
    npa_issue_required_request (sns_smgr_hw.power.pmic_client_ldo, PMIC_NPA_MODE_ID_SENSOR_POWER_OFF);
    /* sns_hw_send_powerrail_msg is called from timer cb in case of turning off
           hence change the state only when turning off */

    sns_smgr_hw.sensor_reset_gpio_pin_cfg= DAL_GPIO_CFG_OUT(sns_smgr_hw_gpio_cfg.sensor_reset, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, 0);
	DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.sensor_reset_gpio_pin_cfg, DAL_TLMM_GPIO_ENABLE );

#endif /* SNS_EXCLUDE_POWER */
    sns_smgr_change_state(SENSOR_STATE_OFF);
  }
  else
  {
#ifndef SNS_EXCLUDE_POWER
    /* power rail needs to be on right away for sensor access */
    SNS_SMGR_PRINTF0(MED, "PMIC_NPA_MODE_ID_SENSOR_LPM");

    npa_issue_required_request (sns_smgr_hw.power.pmic_client_ldo, PMIC_NPA_MODE_ID_SENSOR_POWER_ON);
    npa_issue_required_request (sns_smgr_hw.power.pmic_client_lvs, PMIC_NPA_MODE_ID_SENSOR_POWER_ON);
    smgr_hw_pnoc_vote(sns_smgr_hw.power.mmpm_client_id, true);   /* PNOC(Peripheral NOC) vote for on */

    sns_smgr_hw.sensor_reset_gpio_pin_cfg= DAL_GPIO_CFG_OUT(sns_smgr_hw_gpio_cfg.sensor_reset, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, 1);
    DalTlmm_ConfigGpio(sns_smgr_hw.gpio_handle, sns_smgr_hw.sensor_reset_gpio_pin_cfg, DAL_TLMM_GPIO_ENABLE );

#endif /* SNS_EXCLUDE_POWER */
  }
#endif /* QDSP6 */
#if !defined(SNS_PCSIM) && !defined(QDSP6)
  static uint8_t pwr_off_cnt=0;
  RPM_Message                             active_st_msg;
  DALResult                               send_msg_err_code=0;

  SNS_SMGR_PRINTF3(MED, "send_powerrail_msg - %d new=%d old=%d",
                   9992, state, sns_smgr_hw.powerrail_state );
  /* Trigger LDO9 with RPM Driver */
  if ( state == SNS_SMGR_POWER_HIGH )
  {
    active_st_msg.msg_type = LDO9_High;
    if(sns_hw_gpio_config_i2c_sda_scl(true) == DAL_ERROR)
    {
      SNS_SMGR_PRINTF1(FATAL, "send_powerrail_msg - LDO9_High failed %d", 9931);
    }
  }
  else if ( state == SNS_SMGR_POWER_LOW )
  {
    active_st_msg.msg_type = LDO9_Low;
    if(sns_hw_gpio_config_i2c_sda_scl(true) == DAL_ERROR)
    {
      SNS_SMGR_PRINTF1(FATAL, "send_powerrail_msg - LDO9_Low failed %d", 9931);
    }
  }
  else if ( state == SNS_SMGR_POWER_OFF )
  {
    active_st_msg.msg_type = LDO9_Off;
  }

  send_msg_err_code = RPM_Send_Message(&active_st_msg);

  if (send_msg_err_code != DAL_SUCCESS)
  {
    SNS_SMGR_PRINTF2(FATAL, "send_powerrail_msg - %d Send_Message err=%d", 
                     9922, send_msg_err_code );
  }
  else
  {
    sns_smgr_hw.powerrail_state = state;
    SNS_SMGR_PRINTF2(MED, "send_powerrail_msg - %d state=%d", 8833, state);
  }
  if ( SNS_SMGR_POWER_OFF ==  sns_smgr_hw.powerrail_state )
  {
     /* change all sensor state to off */
    pwr_off_cnt++;
    sns_smgr_change_state(SENSOR_STATE_OFF);
    if(sns_hw_gpio_config_i2c_sda_scl(false) == DAL_ERROR)
    {
      SNS_SMGR_PRINTF1(FATAL, "send_powerrail_msg - all sensors off failed %d",
                       9931);
    }
  }
  else
  {
    if(sns_hw_gpio_config_i2c_sda_scl(true) == DAL_ERROR)
    {
      SNS_SMGR_PRINTF1(FATAL, "send_powerrail_msg - power on failed %d", 9932);
    }
  }
  SNS_SMGR_PRINTF2(MED, "send_powerrail_msg - %d count=%d", 8835, pwr_off_cnt);
#endif  /* SNS_PCSIM */
}

/*===========================================================================

  FUNCTION:   sns_hw_send_powerrail_msg_tmr_cb_proc

===========================================================================*/
/*!
  @brief timer call back process function

  @detail

  @param[i]  cb_arg  callback argument
  @return   none
 */
/*=========================================================================*/
void sns_hw_send_powerrail_msg_tmr_cb_proc()
{
  /* validate the timer happens in valid state */
  if ( sns_smgr_hw.powerrail_timer_pending)
  {
    sns_smgr_hw.powerrail_timer_pending = false;
    sns_hw_send_powerrail_msg(sns_smgr_hw.powerrail_pending_state);
  }
}

/*===========================================================================

  FUNCTION:   sns_hw_timer_cb

===========================================================================*/
/*!
  @brief timer call back. Entered on interrupt

  @detail

  @param[i]  cb_arg  callback argument
  @return   none
 */
/*=========================================================================*/
void sns_hw_timer_cb( void *cb_arg )
{
  uint8_t       err;
  sns_os_sigs_post(sns_smgr.sig_grp, SNS_SMGR_LDO9_FLAG, OS_FLAG_SET, &err);
}

/*===========================================================================

  FUNCTION:   sns_hw_power_rail_config

===========================================================================*/
/*!
  @brief Power rail state check before sending RPM message

  @detail

  @param[in] state:  the state of power rail needs to be updated to.
  @return
   none
 */
/*=========================================================================*/

void sns_hw_power_rail_config(smgr_power_state_type_e state )
{
#ifndef SNS_EXCLUDE_POWER
  SNS_SMGR_PRINTF3(
     MED, "power_rail_config - curr/want=0x%02x timer_pending=%d pending_state=%d",
     (sns_smgr_hw.powerrail_state << 4) | state, 
     sns_smgr_hw.powerrail_timer_pending, sns_smgr_hw.powerrail_pending_state );

  if ( sns_smgr_hw.powerrail_timer_pending &&
       (sns_smgr_hw.powerrail_pending_state != state) )
  {
    sns_em_cancel_timer(sns_smgr_hw.tmr_obj);
    sns_smgr_hw.powerrail_timer_pending = false;
  }

  if ( !sns_smgr_hw.powerrail_timer_pending )
  {
    if ( sns_smgr_hw.powerrail_state > state )
    {
      /* higher power needed now */
      sns_hw_send_powerrail_msg(state);
    }
    else if ( sns_smgr_hw.powerrail_state < state )
    {
      /* lower power needed, can wait */
      if ( SNS_SUCCESS == 
           sns_em_register_timer(sns_smgr_hw.tmr_obj, 
                                 sns_smgr_hw.powerail_timer_value_in_tick))
      {
        sns_smgr_hw.powerrail_timer_pending = true;
        sns_smgr_hw.powerrail_pending_state = state;
      }
      else
      {
        SNS_SMGR_PRINTF0(FATAL, "power_rail_config - failed new timer register");
      }
    }
    /* else, already at the right power state */
  }
  /* else, already waiting to change power to 'state' */
#endif /* SNS_EXCLUDE_POWER */
}

/*===========================================================================

  FUNCTION:   sns_hw_powerrail_status

===========================================================================*/
/*!
  @brief Returns current power rail status

  @detail

  @param  none
  @return current power rail status
 */
/*=========================================================================*/
smgr_power_state_type_e sns_hw_powerrail_status(void)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
  return sns_smgr_hw.powerrail_state;
#else
  return SNS_SMGR_POWER_HIGH;
#endif
}


/*===========================================================================

  FUNCTION:   sns_hw_qup_clck_status

===========================================================================*/
/*!
  @brief Returns current qup clock status

  @detail

  @param  none
  @return current qup clock  status
 */
/*=========================================================================*/
boolean sns_hw_qup_clck_status(void)
{
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)
  return sns_smgr_hw.i2c_clk.qupclk_on ;
#else
  return true;
#endif
}

/*===========================================================================

  FUNCTION:   sns_hw_init_power

===========================================================================*/
/*!
  @brief initialize to use power related API 

  @detail create client handle for latency node

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_hw_init_power (void)
{ 
#if !defined(ADSP_STANDALONE) && !defined(SNS_EXCLUDE_POWER) && !defined(SNS_QDSP_SIM)
  /* MMPM(Multi Media Power Management) client */
  MmpmRegParamType  mmpmRegParam;
#ifdef SNS_DSPS_LATENCY_NODE_ENABLED
  sns_smgr_hw.power.npa_handle = npa_create_sync_client(
               "/core/cpu/latency/usec",      // The path to the resource
               "DSPS_SNS",                    // The client name 
               NPA_CLIENT_REQUIRED);          // Your client Type
  SNS_ASSERT(NULL != sns_smgr_hw.power.npa_handle);
  sns_smgr_hw.power.latency_us = 0;           // No latency is requested
#endif /* SNS_DSPS_LATENCY_NODE_ENABLED */

  sns_smgr_hw.power.reqWakeUpClient   =  npa_create_sync_client(
               "/core/cpu/wakeup",      // Connect to the /core/cpu/wakeup” resource
               "SNS_SMGR_WakeupNode",   // Name describing your client
               NPA_CLIENT_REQUIRED);    // Your client Type
  SNS_ASSERT(NULL != sns_smgr_hw.power.reqWakeUpClient);

  sns_smgr_hw.power.reqSyncLatencyClient  =  npa_create_sync_client(
               "/core/cpu/latency",     // Connect to the “/core/cpu/latency resource
               "SNS_SMGR_LatencyNode",  // Name describing your client
               NPA_CLIENT_REQUIRED );   // Your client Type
  SNS_ASSERT(NULL != sns_smgr_hw.power.reqSyncLatencyClient);

  sns_smgr_hw.power.reqL2Client  =  npa_create_sync_client(
               "/core/cpu/l2cache",     // Connect to the “/core/cpu/l2cache resource
               "SNS_SMGR_L2Node",  // Name describing your client
               NPA_CLIENT_REQUIRED );   // Your client Type
  SNS_ASSERT(NULL != sns_smgr_hw.power.reqL2Client);

  sns_smgr_hw.power.reqSampleLatencyClient  =  npa_create_sync_client(
               "/core/cpu/latency",     // Connect to the “/core/cpu/latency resource
               "SNS_SMGR_SampleLatencyNode",  // Name describing your client
               NPA_CLIENT_REQUIRED );   // Your client Type


  SNS_ASSERT(NULL != sns_smgr_hw.power.reqSampleLatencyClient);

  sns_smgr_hw.power.pmic_client_lvs = npa_create_sync_client (PMIC_NPA_GROUP_ID_SENSOR_VDDIO, "Sensors", NPA_CLIENT_REQUIRED);
  sns_smgr_hw.power.pmic_client_ldo = npa_create_sync_client (PMIC_NPA_GROUP_ID_SENSOR_VDD, "Sensors", NPA_CLIENT_REQUIRED);

  /* Create MMPM(Multi Media Power Management) client for controlling PNOC(Peripheral NOC) */
  mmpmRegParam.rev = MMPM_REVISION;
  mmpmRegParam.coreId = MMPM_CORE_ID_LPASS_ADSP;
  mmpmRegParam.instanceId = MMPM_CORE_INSTANCE_0;
  mmpmRegParam.MMPM_Callback = NULL;
  mmpmRegParam.pClientName = "SENSORS SMGR_MMPM";
  mmpmRegParam.pwrCtrlFlag = PWR_CTRL_PERIODIC_CLIENT;
  mmpmRegParam.callBackFlag = CALLBACK_NONE;
  mmpmRegParam.cbFcnStackSize = 0;
  sns_smgr_hw.power.mmpm_client_id = MMPM_Register_Ext(&mmpmRegParam);
  
  SNS_ASSERT(NULL != sns_smgr_hw.power.pmic_client_lvs);
  SNS_ASSERT(NULL != sns_smgr_hw.power.pmic_client_ldo);
  SNS_ASSERT(0 != sns_smgr_hw.power.mmpm_client_id);
#endif /* !ADSP_STANDALONE */
}

/*===========================================================================

  FUNCTION:   sns_hw_power_set_latency_node_us

===========================================================================*/
/*!
  @brief request a maximum allowable latency or cancel the request 

  @detail

  @param  latency_us the tolerable or allowable latency

  @return none
 */
/*=========================================================================*/
void sns_hw_power_set_latency_node_us(uint32_t latency_us)
{
#if !defined(ADSP_STANDALONE) && !defined(SNS_QDSP_SIM) && defined(SNS_LATENCY_NODE_ENABLED) && !defined(SNS_EXCLUDE_POWER)
  if (sns_smgr_hw.power.latency_us != latency_us)
  {
    if (latency_us)
    {
      npa_issue_required_request(
              sns_smgr_hw.power.npa_handle,   // npa client handle
              latency_us);                    // the tolerable maximum latency
    }
    else
    {
      npa_complete_request(sns_smgr_hw.power.npa_handle);   /* cancel the previos request */
    }
    sns_smgr_hw.power.latency_us = latency_us;
  }
#endif
}


/*===========================================================================

  FUNCTION:   sns_hw_power_npa_vote_wakeup
 
===========================================================================*/
/*!
  @brief Call the NPA(Node Power Architecture) voting API

  @param  wake_state
  @param  latency_state

  @return none
 */
/*=========================================================================*/
void sns_hw_power_npa_vote_wakeup(uint32_t wakeup_state)
{
#if !defined(ADSP_STANDALONE) && !defined(SNS_QDSP_SIM) && !defined(SNS_EXCLUDE_POWER)
  if ( wakeup_state == 0 )
  {
    npa_complete_request(sns_smgr_hw.power.reqWakeUpClient);
  }
  else
  {
    npa_issue_required_request(sns_smgr_hw.power.reqWakeUpClient, wakeup_state);
  }
#endif
}

void sns_hw_power_npa_vote_latency(smgr_npa_client_type_e latencyClientType, uint32_t latency_state)
{
#if !defined(ADSP_STANDALONE) && !defined(SNS_QDSP_SIM) && !defined(SNS_EXCLUDE_POWER)
  npa_client_handle latencyClient, L2CacheClient;

  latencyClient = ( latencyClientType == SNS_SMGR_NPA_CLIENT_SYNC ) ? 
                      sns_smgr_hw.power.reqSyncLatencyClient : sns_smgr_hw.power.reqSampleLatencyClient;
  L2CacheClient = sns_smgr_hw.power.reqL2Client;

  if ( latency_state == 0 )
  {
    npa_complete_request(L2CacheClient);
    npa_complete_request(latencyClient);
  }
  else
  {
    npa_issue_required_request(L2CacheClient, 1);
    npa_issue_required_request(latencyClient, latency_state);
  }
#endif
}

/*===========================================================================

  FUNCTION:   sns_get_i2c_bus_instance

===========================================================================*/
/*!
  @brief Read the I2C bus instances from registry and update the max instances
         on this platform.

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
static void sns_get_i2c_bus_instance(sns_reg_ssi_smgr_cfg_group_s *cfg_group_ptr)
{
  int i, j, new_bus_instance;
  
  for(i = 0; i < ARR_SIZE(cfg_group_ptr->drv_cfg); i++) 
  {
    for (j = 0, new_bus_instance = 1; 
         j < ARR_SIZE(sns_smgr_hw.sns_i2c_bus_instances) && sns_smgr_hw.sns_i2c_bus_instances[j].i2c_bus != 0; 
         j++) 
    {
      if(cfg_group_ptr->drv_cfg[i].i2c_bus == sns_smgr_hw.sns_i2c_bus_instances[j].i2c_bus) 
      {
        new_bus_instance = 0;
        break;
      }
    }
    if(new_bus_instance && j < ARR_SIZE(sns_smgr_hw.sns_i2c_bus_instances)) 
    {
      sns_smgr_hw.sns_i2c_bus_instances[j].i2c_bus = cfg_group_ptr->drv_cfg[i].i2c_bus;
      sns_smgr_hw.sns_i2c_bus_instances[j].i2c_bus_id = sns_i2c_bus_table[cfg_group_ptr->drv_cfg[i].i2c_bus].i2c_bus_id;
      if(sns_smgr_hw.sns_i2c_bus_instances[j].i2c_bus) 
      {
        sns_smgr_hw.sns_i2c_bus_count++;
      }
    }
  }
  return;
}

/*===========================================================================

  FUNCTION:   sns_hw_set_smgr_ssi_recv

===========================================================================*/
/*!
  @brief  Sets the SMGR SSI Receive variable to 'state'

  @param  state - desired state. 'true' means SMGR has received all info
                  related to SMGR SSI configuration. 'false' others.
  @return none
 */
/*=========================================================================*/
void sns_hw_set_smgr_ssi_recv(bool state)
{
  sns_smgr_hw.b_smgr_ssi_recv = state;
}

/*===========================================================================

  FUNCTION:   sns_hw_update_reg_items

===========================================================================*/
/*!
  @brief Updates the registry items for reg group SNS_REG_GROUP_SSI_GPIO_CFG_V02

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_hw_update_ssi_reg_items(uint16_t Id, uint8_t* data_ptr)
{
  if( sns_smgr_ssi_get_cfg_idx(Id) >= 0 )
  {
    sns_reg_ssi_smgr_cfg_group_s *cfg_group_ptr = (sns_reg_ssi_smgr_cfg_group_s*)data_ptr;
    if( cfg_group_ptr )
    {
      uint32_t ix;
      uint8_t null_uuid[16] = SNS_REG_UUID_NULL;
      for ( ix = 0; ix < ARR_SIZE(cfg_group_ptr->drv_cfg); ix++ )
      {
        /* skip the entry with NULL UUID */
        if( SNS_OS_MEMCMP(cfg_group_ptr->drv_cfg[ix].drvuuid, null_uuid, 16)==0 )
        {
           SNS_SMGR_PRINTF0(ERROR, "NULL UUID");
           continue;
        }

        switch( cfg_group_ptr->drv_cfg[ix].sensor_id )
        {
          case SNS_SMGR_ID_ACCEL_V01:
              SNS_SMGR_PRINTF0(MED, "SNS_SMGR_ID_ACCEL_V01");
            sns_smgr_hw_gpio_cfg.accel_int1 = cfg_group_ptr->drv_cfg[ix].gpio1;
            sns_smgr_hw_gpio_cfg.accel_int2 = cfg_group_ptr->drv_cfg[ix].gpio2;
            break;
          case SNS_SMGR_ID_ACCEL_2_V01:
            sns_smgr_hw_gpio_cfg.accel2_int1 = cfg_group_ptr->drv_cfg[ix].gpio1;
            sns_smgr_hw_gpio_cfg.accel2_int2 = cfg_group_ptr->drv_cfg[ix].gpio2;
            break;
          case SNS_SMGR_ID_ACCEL_3_V01:
            sns_smgr_hw_gpio_cfg.accel3_int1 = cfg_group_ptr->drv_cfg[ix].gpio1;
            sns_smgr_hw_gpio_cfg.accel3_int2 = cfg_group_ptr->drv_cfg[ix].gpio2;
            break;
          case SNS_SMGR_ID_ACCEL_4_V01:
            sns_smgr_hw_gpio_cfg.accel4_int1 = cfg_group_ptr->drv_cfg[ix].gpio1;
            sns_smgr_hw_gpio_cfg.accel4_int2 = cfg_group_ptr->drv_cfg[ix].gpio2;
            break;
          case SNS_SMGR_ID_ACCEL_5_V01:
            sns_smgr_hw_gpio_cfg.accel5_int1 = cfg_group_ptr->drv_cfg[ix].gpio1;
            sns_smgr_hw_gpio_cfg.accel5_int2 = cfg_group_ptr->drv_cfg[ix].gpio2;
            break;
          case SNS_SMGR_ID_GYRO_V01:
              SNS_SMGR_PRINTF0(MED, "SNS_SMGR_ID_PROX_LIGHT_V01");
            sns_smgr_hw_gpio_cfg.gyro_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_MAG_V01:
              SNS_SMGR_PRINTF0(MED, "SNS_SMGR_ID_MAG_V01");
            sns_smgr_hw_gpio_cfg.mag_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_PRESSURE_V01:
               SNS_SMGR_PRINTF0(MED, "SNS_SMGR_ID_PRESSURE_V01");
            sns_smgr_hw_gpio_cfg.press_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_PROX_LIGHT_V01:
              SNS_SMGR_PRINTF0(MED, "SNS_SMGR_ID_PROX_LIGHT_V01");
            sns_smgr_hw_gpio_cfg.prx_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            sns_smgr_hw_gpio_cfg.als_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_IR_GESTURE_V01:
            sns_smgr_hw_gpio_cfg.ir_gesture_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_TAP_V01:
            sns_smgr_hw_gpio_cfg.tap_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_HUMIDITY_V01:
            sns_smgr_hw_gpio_cfg.humidity_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_RGB_V01:
            sns_smgr_hw_gpio_cfg.rgb_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_RGB_2_V01:
            sns_smgr_hw_gpio_cfg.rgb2_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_SAR_V01:
            sns_smgr_hw_gpio_cfg.sar_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_SAR_2_V01:
            sns_smgr_hw_gpio_cfg.sar2_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          case SNS_SMGR_ID_HALL_EFFECT_V01:
            sns_smgr_hw_gpio_cfg.hall_effect_int = cfg_group_ptr->drv_cfg[ix].gpio1;
            break;
          default:
            break;
        }
      }
      sns_get_i2c_bus_instance(cfg_group_ptr);
    }

    if( sns_smgr_ssi_is_last_cfg(Id) )
    {
      sns_smgr_hw.b_smgr_ssi_recv = true;
      sns_hw_init_i2c_bus_config();
      SMGR_BIT_SET(sns_smgr.flags, SMGR_FLAGS_HW_INIT_B);
    }        
  }
  else if( Id == SNS_REG_GROUP_SSI_GPIO_CFG_V02 )
  {
    sns_reg_ssi_gpio_cfg_group_s *cfg_group_ptr = (sns_reg_ssi_gpio_cfg_group_s*)data_ptr;
    if( cfg_group_ptr )
    {
      sns_smgr_hw.b_gpio_ssi_recv = true;
      sns_smgr_hw_gpio_cfg.i2c_sda_1 = cfg_group_ptr->i2c_sda_1;
      sns_smgr_hw_gpio_cfg.i2c_scl_1 = cfg_group_ptr->i2c_scl_1;
      sns_smgr_hw_gpio_cfg.i2c_sda_2 = cfg_group_ptr->i2c_sda_2;
      sns_smgr_hw_gpio_cfg.i2c_scl_2 = cfg_group_ptr->i2c_scl_2;
      sns_smgr_hw_gpio_cfg.sensor_reset = cfg_group_ptr->sns_reset;
      sns_hw_int_and_i2c_gpio_config();
      sns_hw_init_i2c_clk();

      if ( SNS_SUCCESS != sns_smgr_req_reg_data( (uint16_t) sns_smgr_ssi_get_cfg_id(0), 
                                                 SNS_SMGR_REG_ITEM_TYPE_GROUP ) )
      {
        SNS_SMGR_PRINTF1(FATAL, "Failed reading registry group %d", 
                         (uint16_t) sns_smgr_ssi_get_cfg_id(0));
      }
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_smgr_hw_read_ssi_data_from_reg

===========================================================================*/
/*!
  @brief Reads the registry items for reg groups SNS_REG_GROUP_SSI_GPIO_CFG_V02 &
         SNS_REG_GROUP_SSI_SMGR_CFG_V02

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_smgr_hw_read_ssi_data_from_reg(void)
{
  int err;
  sns_smgr_hw.b_smgr_ssi_recv = false;
  sns_smgr_hw.b_gpio_ssi_recv = false;

  err = sns_smgr_req_reg_data( SNS_REG_GROUP_SSI_GPIO_CFG_V02, SNS_SMGR_REG_ITEM_TYPE_GROUP );
  if ( SNS_SUCCESS != err )
  {
    SNS_SMGR_PRINTF2(FATAL, "Failed reading registry. Group %d err %d", SNS_REG_GROUP_SSI_GPIO_CFG_V02, err);
  }
  sns_smgr.all_init_state = SENSOR_ALL_INIT_WAITING_CFG;
}

/*===========================================================================

  FUNCTION:   sns_hw_bd_config

===========================================================================*/
/*!
  @brief SMGR init HW related variables

  @detail

  @param  none
  @return none
 */
/*=========================================================================*/
void sns_hw_bd_config(void)
{
  static uint8_t cb_arg;
  sns_smgr_hw.gpio_handle = NULL;

  sns_em_create_timer_obj( &sns_hw_timer_cb, &cb_arg,SNS_EM_TIMER_TYPE_ONESHOT, &sns_smgr_hw.tmr_obj );
#if !defined(SNS_PCSIM) && !defined(SNS_QDSP_SIM)  
#ifdef QDSP6
  /* qup clock is turned ON when DALDevice_Open is called as part of sns_hw_set_qup_clk */
  sns_smgr_hw.i2c_clk.qupclk_on= false;
#else
  /* qup clock is turned on at i2c firstread/write atpower up*/
  sns_smgr_hw.i2c_clk.qupclk_on= true;
#endif
#endif
  /* It will be turned on in SMGR DD init */
  sns_smgr_hw.powerrail_state = SNS_SMGR_POWER_OFF;
  sns_smgr_hw.powerrail_timer_pending = false;
  sns_smgr_hw.powerail_timer_value_in_tick = SNS_SMGR_POWERRAIL_HYSTERESIS_TIMER_VALUE_TICK;
#ifndef ADSP_STANDALONE
  sns_smgr_hw_read_ssi_data_from_reg();
#endif /* ADSP_STANDALONE */
  sns_hw_init_power();
}
