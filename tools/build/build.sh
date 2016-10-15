#===============================================================================
#
# SCons + QCT SCons Extentions buils system
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
# $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/build.sh#1 $
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

cd `dirname $0`

# ==================================================================
#     exportup Target-Specific Variables
# ==================================================================

# exportup Default Values
export BUILD_ID=ABCDEFGH

# Common
export BUILD_ASIC=9xxxA
export MSM_ID=9xxx
export HAL_PLATFORM=9xxx
export TARGET_FAMILY=9xxx
export CHIPexport=mdm9x15
export T_CFLAGS=-mno-pullup

# export BUILD_CMD="BUILD_ID=$BUILD_ID MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET T_CFLAGS=$T_CFLAGS $*"
export BUILD_CMD="BUILD_ID=%BUILD_ID% BUILD_ASIC=%BUILD_ASIC% CHIPSET=%CHIPSET% TARGET_FAMILY=%TARGET_FAMILY% HAL_PLATFORM=%HAL_PLATFORM% $*"


#==================================================================
#    Call Common Build Command
#==================================================================

source build_common.sh $*
build_result=$?
if [ "${build_result}" != "0" ] ; then
    exit ${build_result}
fi
