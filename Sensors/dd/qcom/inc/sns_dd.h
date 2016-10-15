#ifndef SNS_DD_H
#define SNS_DD_H

/*============================================================================

  @file sns_dd.h

  @brief
  This file supports interface between device drivers and Sensor Manager

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

============================================================================*/
/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/dd/qcom/inc/sns_dd.h#2 $ */
/* $DateTime: 2014/05/29 07:31:11 $ */
/* $Author: rpoliset $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-05-16   MW   Added HSPPAD038A
  2014-04-08   MW   Added LPS25H
  2014-02-28   MW   Added generic DD interfaces - for HD22
  2013-12-18   MW   Added BU52061NVX
  2013-10-11   yh   Added AK09911C  
  2013-11-29   yh   Added MC3410 
  2013-12-11   MW   Added MAX44006  
  2013-11-06   sd   Added BMG160
  2013-11-04   MW   Added LSM303D
  2013-10-23   MW   Added AD7146
  2013-09-23   yh   Added AL3320B
  2013-09-09   yh   Added CM36283
  2013-09-05   yh   Added KXTIK
  2013-10-09   RS   Added Memsic M34160PJ Mag
  2013-09-12   MW   Updated MAX88120 funciton list, added BH1721
  2013-09-05   rs   Added AKM09912
  2013-09-03   yh   Added ISL29044A
  2013-09-02   cj   Added ISL29147
  2013-08-09   yh   Added AP3216C
  2013-08-09   yh   Added MMA8452
  2013-07-25   yh   Added KXCJK
  2013-08-05   sc   Added AMS TMG399X
  2013-07-31   MW   Added APDS9950 and MAX88120
  2013-07-15   dc   Support for humidity sensor type  
  2013-06-17   pf   Added LTR55x
  2013-06-11   ae   Added QDSP SIM playback support
  2013-05-22  lka   Renamed MPU function pointer
  2013-05-01  lka   Added support for LIS3DSH
  2013-04-03   sc   Added support for MAXIM44009
  2013-02-27   ps   Added sns_dd_mag_hscdtd_if and sns_alsprx_tmd277x_driver_fn_list
  2013-01-09   sd   Added sns_mag_yas_driver_fn_list and sns_accel_bma2x2_driver_fn_list
  2012-06-20   dc   Extern sns_accel_bma250_driver_fn_list(Bosch BAM 250)
  2012-06-01   dc   Extern sns_dd_press_lps331ap_if(ST Pressure sensor)
  2012-05-18   pn   Support concurrent MAG drivers.
  2012-05-17   pn   Support concurrent APDS drivers.
  2012-02-03   ry   Added support for APDS99XX and APDS91XX
  2011-10-05   br   Added support for MPU6050
  2011-10-05   ry   Added support for AK8963
  2011-06-29   sd   Added ST mag/gyro support
  2011-06-10   ag   Added ISL 29028 for alsprx
  2011-05-25   rk   added the extern declaration for STM accel driver
  2011-02-22   as   Add BMP085 Pressure and Temperature sensor.
  2011-11-24   OF   AKM8975 Magnetometer added
  2011-01-18   as   Added ADXL 350/346 accelerometer driver.
  2010-10-26   AG   ALSPRX added
  2010-10-12   RK   Adding mag driver list
  2010-10-07   ad   added gyro driver function list
  2010-09-02   JB   Initial version
============================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Make device function lists sharable with SMGR */
extern sns_ddf_driver_if_s sns_dd_mpu6xxx_if;   // MPU6050, MPU6500
extern sns_ddf_driver_if_s sns_dd_mpu6515_if;
extern sns_ddf_driver_if_s sns_accel_driver_fn_list;
extern sns_ddf_driver_if_s sns_accel_adxl350_driver_fn_list;
extern sns_ddf_driver_if_s sns_gyro_driver_fn_list;
extern sns_ddf_driver_if_s sns_mag_driver_fn_list;
extern sns_ddf_driver_if_s sns_mag_ami306_driver_fn_list;
extern sns_ddf_driver_if_s sns_alsprx_driver_fn_list;
extern sns_ddf_driver_if_s sns_alsprx_isl29028_driver_fn_list;
extern sns_ddf_driver_if_s sns_alsprx_isl29147_driver_fn_list;
extern sns_ddf_driver_if_s sns_mag_akm8963_driver_fn_list;
extern sns_ddf_driver_if_s sns_mag_akm8975_driver_fn_list;
extern sns_ddf_driver_if_s sns_alt_bmp085_driver_fn_list;
extern sns_ddf_driver_if_s sns_dd_acc_lis3dh_if;
extern sns_ddf_driver_if_s sns_dd_acc_lis3dsh_if;
extern sns_ddf_driver_if_s sns_dd_mag_lsm303dlhc_if;
extern sns_ddf_driver_if_s sns_dd_gyr_if;
extern sns_ddf_driver_if_s sns_dd_gyr_l3g4200d_if;
extern sns_ddf_driver_if_s sns_dd_apds91xx_driver_if;
extern sns_ddf_driver_if_s sns_dd_apds99xx_driver_if;
extern sns_ddf_driver_if_s sns_dd_press_lps331ap_if;
extern sns_ddf_driver_if_s sns_accel_bma250_driver_fn_list;
extern sns_ddf_driver_if_s sns_accel_bma2x2_driver_fn_list;
extern sns_ddf_driver_if_s sns_mag_yas_driver_fn_list;
extern sns_ddf_driver_if_s sns_dd_mag_hscdtd_if;
extern sns_ddf_driver_if_s sns_alsprx_tmd277x_driver_fn_list;
extern sns_ddf_driver_if_s sns_alsprx_ltr55x_driver_fn_list;
extern sns_ddf_driver_if_s sns_alsprx_al3320b_driver_fn_list;
extern sns_ddf_driver_if_s sns_als_max44009_driver_if;
extern sns_ddf_driver_if_s sns_dd_qdsp_playback_if;
extern sns_ddf_driver_if_s sns_rht_shtc1_driver_fn_list;
extern sns_ddf_driver_if_s sns_dd_apds9950_driver_if;
extern sns_ddf_driver_if_s sns_ges_max88120_driver_if;
extern sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list;
extern sns_ddf_driver_if_s sns_accel_kxcjk_driver_fn_list;
extern sns_ddf_driver_if_s sns_dd_acc_mma8452_if;
extern sns_ddf_driver_if_s sns_alsprx_ap3216c_driver_fn_list;
extern sns_ddf_driver_if_s sns_alsprx_isl29044a_driver_fn_list;
extern sns_ddf_driver_if_s sns_mag_akm09912_driver_fn_list;
extern sns_ddf_driver_if_s sns_als_bh1721_driver_fn_list;
extern sns_ddf_driver_if_s sns_mmc3xxx_driver_fn_list;
extern sns_ddf_driver_if_s sns_alsprx_cm36283_driver_fn_list;
extern sns_ddf_driver_if_s sns_accel_kxtik_driver_fn_list;
extern sns_ddf_driver_if_s sns_dd_sar_ad7146_fn_list;
extern sns_ddf_driver_if_s sns_dd_lsm303d_if;
extern sns_ddf_driver_if_s sns_bmg160_driver_fn_list;
extern sns_ddf_driver_if_s sns_als_rgb_max44006_driver_if;
extern sns_ddf_driver_if_s sns_dd_acc_mc3410_if;
extern sns_ddf_driver_if_s sns_mag_akm_driver_fn_list;
extern sns_ddf_driver_if_s sns_hall_bu52061_driver_fn_list;
extern sns_ddf_driver_if_s sns_dd_press_lps25h_if;
extern sns_ddf_driver_if_s sns_dd_prs_hsppad_if;

/* Generic DD interfaces available for vendors */
extern sns_ddf_driver_if_s sns_dd_vendor_if_1;
extern sns_ddf_driver_if_s sns_dd_vendor_if_2;

#endif /* SNS_DD_H */
