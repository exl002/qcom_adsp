/*===============================================================================
#
# DSPS custfile
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/build/core/bsp/sensorsimg/inc/custsensorsimg.h#1 $
#  $DateTime: 2014/05/16 10:56:56 $
#  $Author: coresvc $
#  $Change: 5907808 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 07/11/11    DC     Enable power management. Enable SNS_PM_TEST thread
# 07/07/11    DC     Disable power management. App proc Bus driver not yet ready.
# 07/05/11    DC     Enable power management.
# 06/20/11    JH     Moved CONFIG_USE_LIS3DH and CONFIG_USE_ISL29028 to arm7.scons
# 06/16/11    AG     Defined flag to select ISL29028 driver
# 06/14/11    DC     Disabled Power management
# 06/03/11    JH     Enable power management features
# 05/25/11    YK     Renamed CONFIG_USE_STM to CONFIG_USE_LIS3DH.
# 05/25/11    RK     defined STM driver
# 05/25/11    RK     undefined M8660_V2
# 05/12/11    RK     Added CONFIG_USE_STM UNDEF
# 04/29/11    JH     Updated shared RAM address range for 8960
# 02/08/11    AG     Remove reference to unrelated features
# 11/17/10    JH     Define V2 as default MSM version
# 10/29/10    AG     Added #define for 8660 version
#
#===============================================================================*/
#ifndef CUSTSENSORSIMG_H
#define CUSTSENSORSIMG_H

/* Include the target cust file for this build flavor */
#ifdef AMSS_CUST_H
    #include AMSS_CUST_H
#endif

#include SNS_DSPS_H

#ifndef SCL_SHARED_RAM_BASE
    #ifdef FEATURE_MSM8660
        #define SCL_SHARED_RAM_BASE 0x40000000
        #define SCL_SHARED_RAM_SIZE 0x00100000

    #elif FEATURE_MSM8960
        #define SCL_SHARED_RAM_BASE 0x80000000
        #define SCL_SHARED_RAM_SIZE 0x00200000
    #endif
#endif

#define FEATURE_SMDL
#undef FEATURE_SMDLITE_TEST
//#define SNS_EXCLUDE_POWER
#define SNS_PM_TEST

#ifdef FEATURE_SMDL
    #define IMAGE_DSPS_PROC
    #define FEATURE_IGUANA
    #define FEATURE_PPSS_DEBUG 
    #undef  FEATURE_SPINLOCK 
    #undef  FEATURE_ARM_ASM_SPINLOCK
    #define SNS_SHORTCUT_ENABLED
    #undef  CONFIG_USE_ADXL
    #undef  M8660_V2
#endif

#endif /* CUSTSENSORSIMG_H */
