#===============================================================================
#
# Target Linux Build Script
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009-2013 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/dspbuild.adsp/2.2/ms/build.sh#5 $
# $DateTime: 2014/06/17 21:41:55 $
# $Author: pwbldsvc $
# $Change: 6105840 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
#
#===============================================================================

# Command Line Usage
# ------------------
#   build.sh $1 $2 $3 $4 $5
# $1 - Build Chipset.  
#      msm8974, mdm9x25, msm8x26, msm8x10, mpq8092, mpq8094
# $2 - Image to Build.
#      adsp_mpd_core_images - CoreBSP Standalone Image, MPD
#      adsp_mpd_images - CRM, MPD
#      adsp_core_images - CoreBSP Standalone Image, SPD
#      adsp_images - CRM, SPD
# $3 - Build ID
# $4 - Revision ID
# $5 - Additional Arguments
#
#  Note:  The Build Chipset must be the first argument.  A shift command is 
#         performed on the $* string, which truncates the first argument before
#         passing the remainder of the arguments to the SCons Build System.

export USES_FLAGS=USES_SPD
for i in $*
do
     if [ "$i" = "adsp_mpd_core_images" ]
     then
         export USES_FLAGS=CORE_KERNEL_CONFIGURATION,USES_MULTI_PD
     elif [ "$i" = "adsp_mpd_images" ]
     then
         export USES_FLAGS=USES_MULTI_PD
     elif [ "$i" = "adsp_core_images" ]
     then
         export USES_FLAGS=CORE_KERNEL_CONFIGURATION,SPD
     elif [ "$i" = "msm8974_MPD" ]
     then
         export USES_FLAGS=USES_MULTI_PD,USES_FULL_ADSP_BUILD
	 elif [ "$i" = "mdm9x25_MPD" ]
     then
         export USES_FLAGS=USES_MULTI_PD,USES_FULL_ADSP_BUILD
	 elif [ "$i" = "msm8x10_MPD" ]
     then
         export USES_FLAGS=USES_MULTI_PD,USES_FULL_ADSP_BUILD
	 elif [ "$i" = "msm8x26_MPD" ]
     then
         export USES_FLAGS=USES_MULTI_PD,USES_FULL_ADSP_BUILD
	 elif [ "$i" = "mpq8092_MPD" ]
     then
         export USES_FLAGS=USES_MULTI_PD,USES_FULL_ADSP_BUILD
	 elif [ "$i" = "mpq8094_MPD" ]
     then
         export USES_FLAGS=USES_MULTI_PD,USES_FULL_ADSP_BUILD	 
     fi
	 if [ "$i" = "msm8974" ]
	 then
		 export BUILD_ASIC=8974
		 export MSM_ID=8974
		 export HAL_PLATFORM=8974
		 export TARGET_FAMILY=8974
		 export CHIPSET=msm8974
	 elif [ "$i" = "mdm9x25" ]
	 then
		 export BUILD_ASIC=9x25A
		 export MSM_ID=9x25
		 export HAL_PLATFORM=9x25
		 export TARGET_FAMILY=9x25
		 export CHIPSET=mdm9x25
	 elif [ "$i" = "msm8x10" ]
	 then
		 export BUILD_ASIC=8x10
		 export MSM_ID=8x10
		 export HAL_PLATFORM=8x10
		 export TARGET_FAMILY=8x10
		 export CHIPSET=msm8x10
	 elif [ "$i" = "msm8x26" ]
	 then
		 export BUILD_ASIC=8x26
		 export MSM_ID=8974
		 export HAL_PLATFORM=8974
		 export TARGET_FAMILY=8974
		 export CHIPSET=msm8974
	 elif [ "$i" = "mpq8092" ]
	 then
		 export BUILD_ASIC=8092
		 export MSM_ID=8092
		 export HAL_PLATFORM=8092
		 export TARGET_FAMILY=8092
		 export CHIPSET=mpq8092
	 elif [ "$i" = "mpq8094" ]
	 then
		 export BUILD_ASIC=8094
		 export MSM_ID=8094
		 export HAL_PLATFORM=8094
		 export TARGET_FAMILY=8094
		 export CHIPSET=mpq8094
	 fi
done
shift
unamestr=`uname`
if [ "$unamestr" = "Linux" ]; then
   
   # set up local environment
   export_armlmd_license()
   {
     # know where the host is located
     mdb $(echo `hostname`) return site > __temp.out

     # in boulder?
     grep -q "site: boulder" __temp.out
     if [ $? -eq 0 ]
     then
       echo "in boulder"
       export ARMLMD_LICENSE_FILE=8224@redcloud:8224@swiftly:7117@license-wan-arm1
     else
       # in rtp?
       grep -q "site: rtp" __temp.out
       if [ $? -eq 0 ]
       then
         echo "in rtp"
         export ARMLMD_LICENSE_FILE=8224@license-wan-rtp1
       else
         # in hyderabad?
         grep -q "site: hyderabad" __temp.out
         if [ $? -eq 0 ]
         then
           echo "in hyderabad"
           export ARMLMD_LICENSE_FILE=7117@license-hyd1:7117@license-hyd2:7117@license-hyd3
         else
           # in sandiego and others
           echo "in sandiego"
           export ARMLMD_LICENSE_FILE=7117@license-wan-arm1
         fi
       fi
     fi

     rm -f __temp.out
   }

   ARM_COMPILER_PATH=/pkg/qct/software/arm/RVDS/2.2BLD593/RVCT/Programs/2.2/593/linux-pentium
   #PYTHON_PATH=/pkg/qct/software/python/2.6.4/bin
   MAKE_PATH=/pkg/gnu/make/3.81/bin
   export ARMTOOLS=RVCT221
   export ARMROOT=/pkg/qct/software/arm/RVDS/2.2BLD593
   export ARMLIB=$ARMROOT/RVCT/Data/2.2/349/lib
   export ARMINCLUDE=$ARMROOT/RVCT/Data/2.2/349/include/unix
   export ARMINC=$ARMINCLUDE
   export ARMCONF=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMDLL=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMBIN=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export PATH=$MAKE_PATH:$ARM_COMPILER_PATH:$PATH
   export ARMHOME=$ARMROOT
   export_armlmd_license

   # Hexagon Setup
   if [ $HEXAGON_ROOT ]; then
   echo "HEXAGON_ROOT already set: $HEXAGON_ROOT"
   else
   export HEXAGON_ROOT=/pkg/qct/software/hexagon/releases/tools
   fi

   if [ $HEXAGON_RTOS_RELEASE ]; then
   echo "HEXAGON_RTOS_RELEASE already set: $HEXAGON_RTOS_RELEASE"
   else
   export HEXAGON_RTOS_RELEASE=5.0.10
   fi

   if [ $HEXAGON_REQD_RTOS_RELEASE ]; then
   echo "HEXAGON_REQD_RTOS_RELEASE already set: $HEXAGON_REQD_RTOS_RELEASE"
   else
   export HEXAGON_REQD_RTOS_RELEASE=5.0.10
   fi
      
   if [ $HEXAGON_Q6VERSION ]; then
   echo "HEXAGON_Q6VERSION already set: $HEXAGON_Q6VERSION"
   else
   export HEXAGON_Q6VERSION=v5
   fi
   
   if [ $HEXAGON_REQD_Q6VERSION ]; then
   echo "HEXAGON_REQD_Q6VERSION already set: $HEXAGON_REQD_Q6VERSION"
   else
   export HEXAGON_REQD_Q6VERSION=v5
   fi

fi

#===============================================================================
# Set target enviroment
#===============================================================================
export BUILD_ID=AAAAAAAA
export BUILD_VER=0002

if [ $BUILD_FLAGS ]; then
export BUILD_CMD="BUILD_ID=$BUILD_ID BUILD_VER=$BUILD_VER MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET USES_FLAGS=$USES_FLAGS,$BUILD_FLAGS $*"
else
export BUILD_CMD="BUILD_ID=$BUILD_ID BUILD_VER=$BUILD_VER MSM_ID=$MSM_ID HAL_PLATFORM=$HAL_PLATFORM TARGET_FAMILY=$TARGET_FAMILY BUILD_ASIC=$BUILD_ASIC CHIPSET=$CHIPSET USES_FLAGS=$USES_FLAGS $*"
fi

build_dir=`dirname $0`
cd $build_dir
export SCONS_BUILD_ROOT=../..
export SCONS_TARGET_ROOT=$SCONS_BUILD_ROOT/..
export TOOLS_SCONS_ROOT=$SCONS_BUILD_ROOT/tools/build/scons

$TOOLS_SCONS_ROOT/build/rename-log.sh
$TOOLS_SCONS_ROOT/build/build.sh -f $TOOLS_SCONS_ROOT/build/start.scons $BUILD_CMD
