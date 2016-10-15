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
rem $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/setenv.cmd#1 $
rem $DateTime: 2012/05/21 14:17:25 $
rem $Author: coresvc $
rem $Change: 2437655 $
rem
rem ==========================================================================

rem ==================================================================
rem     Setup ARM Environment
rem ==================================================================
call \\stone\aswcrm\smart\nt\bin\RVCT221P593.cmd 

rem ==================================================================
rem     Setup Hexagon Environment
rem ==================================================================
set HEXAGON_RTOS_RELEASE=3.0.10
set HEXAGON_Q6VERSION=v4
set HEXAGON_IMAGE_ENTRY=0x41800000

REM SET BUILDSPEC=KLOCWORK


