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
rem $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/build_common.cmd#1 $
rem $DateTime: 2012/05/21 14:17:25 $
rem $Author: coresvc $
rem $Change: 2437655 $
rem
rem ==========================================================================

setlocal

rem ===============================================================================
rem  Call Set environment script
rem ===============================================================================
IF EXIST setenv.cmd CALL setenv.cmd

rem ===============================================================================
rem Call SCons
rem ===============================================================================
SET SCONS_BUILD_ROOT=%~dp0..\..
SET SCONS_TARGET_ROOT=%SCONS_BUILD_ROOT%\..
SET TOOLS_SCONS_ROOT=%SCONS_BUILD_ROOT%\tools\build\scons

echo Calling rename-log...
CALL %TOOLS_SCONS_ROOT%\build\rename-log.cmd
CALL %TOOLS_SCONS_ROOT%\build\build.cmd -f %TOOLS_SCONS_ROOT%\build\start.scons %BUILD_CMD%
@exit /B %ERRORLEVEL%

