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
# $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/setenv.sh#1 $
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


   PYTHON_PATH=/pkg/qct/software/python/2.5.2/bin
   MAKE_PATH=/pkg/gnu/make/3.81/bin
   
   # ==================================================================
   #    Setup ARM Environment
   # ==================================================================
   ARM_COMPILER_PATH=/pkg/qct/software/arm/RVDS/2.2BLD593/RVCT/Programs/2.2/593/linux-pentium   
   export ARMTOOLS=RVCT221
   export ARMROOT=/pkg/qct/software/arm/RVDS/2.2BLD593
   export ARMLIB=$ARMROOT/RVCT/Data/2.2/349/lib
   export ARMINCLUDE=$ARMROOT/RVCT/Data/2.2/349/include/unix
   export ARMINC=$ARMINCLUDE
   export ARMCONF=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMDLL=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export ARMBIN=$ARMROOT/RVCT/Programs/2.2/593/linux-pentium
   export PATH=$MAKE_PATH:$PYTHON_PATH:$ARM_COMPILER_PATH:$PATH
   export ARMHOME=$ARMROOT
   export_armlmd_license

   # ==================================================================
   #   Setup Hexagon Environment
   # ==================================================================
   export HEXAGON_RTOS_RELEASE=3.0.10
   export HEXAGON_Q6VERSION=v4
   export HEXAGON_IMAGE_ENTRY=0x41800000
   export HEXAGON_ROOT=/pkg/qct/software/hexagon/releases/tools

fi

