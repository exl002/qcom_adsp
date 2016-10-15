@REM Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008 The SCons Foundation
@REM src/script/scons.bat 3842 2008/12/20 22:59:52 scons
@echo off
set SCONS_ERRORLEVEL=
if "%OS%" == "Windows_NT" goto WinNT

@REM for 9x/Me you better not have more than 9 args
python -c "from os.path import join; import sys; sys.path = [ join(abspath('../../bsp/tools'))] + sys.path; import SCons.Script; SCons.Script.main()" %1 %2 %3 %4 %5 %6 %7 %8 %9
@REM no way to set exit status of this script for 9x/Me
goto endscons

@REM Credit where credit is due:  we return the exit code despite our
@REM use of setlocal+endlocal using a technique from Bear's Journal:
@REM http://code-bear.com/bearlog/2007/06/01/getting-the-exit-code-from-a-batch-file-that-is-run-from-a-python-program/

:WinNT
setlocal
@REM ensure the script will be executed with the Python it was installed for
set path=%~dp0;%~dp0..;%path%
set SCONS_ROOT=%~dp0..

@REM Looking for python install
if defined PYTHONBIN (goto pythonbin_defined)

@REM PYTHONPATH is defined as an array and is intended to look for packages,
@REM it is used incorrectly here and should be removed. Left in this version
@REM for backward compatibility.
if defined PYTHONPATH (goto pythonpath_defined)

@REM check known hard-coded paths
set CRM_PYTHON_1=C:\CRMApps\Apps\Python262
set CRM_PYTHON_2=C:\Apps\Python262

if exist %CRM_PYTHON_1%\nul (goto crm_python_1)
if exist %CRM_PYTHON_2%\nul (goto crm_python_2)

goto no_match:

:pythonbin_defined
set PYTHONCMD=%PYTHONBIN%\python.exe
goto done

:pythonpath_defined
set PYTHONCMD=%PYTHONPATH%\python.exe
goto done

:crm_python_1
set PYTHONCMD=%CRM_PYTHON_1%\python.exe
goto done

:crm_python_2
set PYTHONCMD=%CRM_PYTHON_2%\python.exe
goto done

:no_match
set PYTHONCMD=python.exe

:done
if not exist %PYTHONCMD% (set PYTHONCMD=python.exe)

echo PYTHONCMD=%PYTHONCMD%

%PYTHONCMD% -c "from os import environ; from os.path import join, abspath; import sys; sys.path = [ join(abspath(environ.get('SCONS_ROOT','..')))] + sys.path; import SCons.Script; SCons.Script.main()" %*
endlocal & set SCONS_ERRORLEVEL=%ERRORLEVEL%

if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto returncode
if errorlevel 9009 echo you do not have python in your PATH
goto endscons

:returncode
exit /B %SCONS_ERRORLEVEL%

:endscons
call :returncode %SCONS_ERRORLEVEL%
