@echo off
rem ==========================================================================
rem
rem  SCons + QCT SCons Extentions buils system
rem
rem  General Description
rem     build batch file.
rem
rem Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/build.cmd#1 $
rem $DateTime: 2012/05/21 14:17:25 $
rem $Author: coresvc $
rem $Change: 2437655 $
rem                      EDIT HISTORY FOR FILE
rem
rem  This section contains comments describing changes made to the module.
rem  Notice that changes are listed in reverse chronological order.
rem
rem when       who     what, where, why
rem --------   ---     ---------------------------------------------------------
rem
rem ==========================================================================

setlocal

rem ==================================================================
rem     Setup Target-Specific Variables
rem ==================================================================

rem Setup Default Values
SET BUILD_ID=ABCDEFGH

rem Common
SET BUILD_ASIC=9xxxA
SET MSM_ID=9xxx
SET HAL_PLATFORM=9xxx
SET TARGET_FAMILY=9xxx
SET CHIPSET=mdm9x15
SET T_CFLAGS=-mno-pullup

rem SET BUILD_CMD=BUILD_ID=%BUILD_ID% MSM_ID=%MSM_ID% HAL_PLATFORM=%HAL_PLATFORM% TARGET_FAMILY=%TARGET_FAMILY% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% T_CFLAGS=%T_CFLAGS% %*
SET BUILD_CMD=BUILD_ID=%BUILD_ID% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% TARGET_FAMILY=%TARGET_FAMILY% HAL_PLATFORM=%HAL_PLATFORM% %*

rem ==================================================================
rem     Call Common Build Command
rem ==================================================================

call build_common.cmd 

@exit /B %ERRORLEVEL%
