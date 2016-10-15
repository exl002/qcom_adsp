#!/bin/sh
#===============================================================================
#
#  SCons + QCT SCons Extentions buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/build_common.sh#1 $
# $DateTime: 2012/05/21 14:17:25 $
# $Author: coresvc $
# $Change: 2437655 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
#
#===============================================================================

#===============================================================================
# Call Set environment script
#===============================================================================
if [ -e setenv.sh ]; then
source setenv.sh
fi

#===============================================================================
# Call SCons
#===============================================================================
export SCONS_BUILD_ROOT=../..
export SCONS_TARGET_ROOT=$SCONS_BUILD_ROOT/..
export TOOLS_SCONS_ROOT=$SCONS_BUILD_ROOT/tools/build/scons

echo Calling rename-log...
$TOOLS_SCONS_ROOT/build/rename-log.sh
$TOOLS_SCONS_ROOT/build/build.sh -f $TOOLS_SCONS_ROOT/build/start.scons $BUILD_CMD
build_result=$?
if [ "${build_result}" != "0" ] ; then
    exit ${build_result}
fi
