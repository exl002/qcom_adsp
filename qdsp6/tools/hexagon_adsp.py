#===============================================================================
#
# CoreBSP HEXAGON tool rules
#
# GENERAL DESCRIPTION
#    HEXAGON Tools definitions
#
# Copyright (c) 2009-2013 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspcore.adsp/2.2/tools/hexagon_adsp.py#9 $
#  $DateTime: 2013/11/22 20:50:06 $
#  $Author: coresvc $
#  $Change: 4846146 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import sys
import os
import string
import SCons.Action
from SCons.Script import *


   
#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('hexagon_adsp')

def generate(env):
   #===============================================================================
   # load hexagon.py to environment
   #===============================================================================
   env.Tool('hexagon', toolpath = ['${BUILD_ROOT}/tools/build/scons/scripts'])

   #===============================================================================
   # figure out what tools set will be using
   #===============================================================================
   # The following environment variables must be defined prior to using this make
   # file: GNUPATH, QCTOOLS. In addition the PATH must be updated for
   # the GCC tools.

   hexagon_root = os.environ.get('HEXAGON_ROOT', None)   
   hexagon_image_entry = os.environ.get('HEXAGON_IMAGE_ENTRY', '0xf0000000')   
   hexagon_rtos_release = os.environ.get('HEXAGON_RTOS_RELEASE', None)
   q6version = os.environ.get('HEXAGON_Q6VERSION', None)
   q6_reqdversion = os.environ.get('HEXAGON_REQD_Q6VERSION', None)
   hexagon_rtos_reqdrelease = os.environ.get('HEXAGON_REQD_RTOS_RELEASE', None)

      
   # init variables to environment
   env.Replace(QDSP6_RELEASE_DIR = "${HEXAGON_ROOT}/${HEXAGON_RTOS_RELEASE}")
   env.Replace(Q6_REQDVERSION = q6_reqdversion)
   env.Replace(HEXAGON_RTOS_REQDRELEASE = hexagon_rtos_reqdrelease)
   
   if not env.PathExists("${QDSP6_RELEASE_DIR}"):
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("*** HEXAGON tools are not present")
      env.PrintError("*** Please install HEXAGON tools")
      env.PrintError("*** Currently setup for RTOS: ${HEXAGON_RTOS_RELEASE}")
      env.PrintError("*** Missing RTOS PATH: ${QDSP6_RELEASE_DIR}")
      env.PrintError("*** Recommended locations:")
      env.PrintError("***      Windows: C:\Qualcomm\HEXAGON_Tools")
      env.PrintError("***      Linux: HOME/Qualcomm/HEXAGON_Tools (OR) /pkg/qct/software/hexagon/releases/tools")
      env.PrintError("***      Note: If installed at other location, please update \'Software Paths & other definitions\' section in \'build.py\' accordingly")
      env.PrintError("***            OR set HEXAGON_ROOT and HEXAGON_RTOS_RELEASE environment variables accordingly")
      env.PrintError("***               Eg: set HEXAGON_ROOT=C:\Qualtools\Hexagon")
      env.PrintError("***                   set HEXAGON_RTOS_RELEASE=5.0.09")
      env.PrintError("-------------------------------------------------------------------------------")
      Exit(1)


   if q6version != os.environ.get('HEXAGON_REQD_Q6VERSION'):
      env.PrintError("-------------------------------------------------------------------------------")  
      env.PrintError("*** You are using Hexagon Q6 Version: ${Q6VERSION}")
      env.PrintError("*** Please use Required Hexagon Q6 Version: ${Q6_REQDVERSION}")
      env.PrintError("*** If it is intentional, please update HEXAGON_REQD_Q6VERSION at 'Required Versions' section in build.py accordingly!!!")
      env.PrintError("***   OR set HEXAGON_REQD_Q6VERSION environment variable accordingly!!!")
      env.PrintError("-------------------------------------------------------------------------------")      
      Exit(1)

   if hexagon_rtos_release != os.environ.get('HEXAGON_REQD_RTOS_RELEASE'):
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("*** You are using Hexagon Tools Version: ${HEXAGON_RTOS_RELEASE}")
      env.PrintError("*** Please install Required Hexagon Tools Version: ${HEXAGON_RTOS_REQDRELEASE}")
      env.PrintError("*** If it is intentional, please update HEXAGON_REQD_RTOS_RELEASE at 'Required Versions' section in build.py")
      env.PrintError("***   OR set HEXAGON_REQD_RTOS_RELEASE environment variable accordingly!!!")
      env.PrintError("-------------------------------------------------------------------------------")      
      Exit(1)
   
   
   #-------------------------------------------------------------------------------
   # Software tool and environment definitions
   #-------------------------------------------------------------------------------

   env.PrintInfo("Print from hexagon_adsp.py version")
   env.PrintInfo("HEXAGON_TOOLS       = ${HEXAGON_TOOLS}")
   env.PrintInfo("HEXAGON_RTOS_REL    = ${HEXAGON_RTOS_RELEASE}")
   env.PrintInfo("Q6VERSION           = ${Q6VERSION}")      
   env.PrintInfo("HEXAGON_ROOT        = %s" % env.RealPath("${HEXAGON_ROOT}"))
   env.PrintInfo("QDSP6BIN            = ${QDSP6BIN}")
   env.PrintInfo("HEXAGON_IMAGE_ENTRY = ${HEXAGON_IMAGE_ENTRY}")   

   

   #----------------------------------------------------------------------------
   # common defines
   #----------------------------------------------------------------------------

   env.Replace(BUILD_CCFLAGS = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" '\
      '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" '\
      '-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 '\
      '-DGPL_ENVIRONMENT=2 -D__FILENAME__=\\"$SOURCE.file\\" '\
      '-D__qdsp6__=1')
      
   env.Replace(BUILD_ASMFLAGS = '-DCUST_H=\\"${CUST_H}\\" -DBUILD_TARGET=\\"${BUILD_ID}\\" '\
      '-DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\" -D__FILENAME__=\\"$SOURCE.file\\" '\
      '-D__qdsp6__=1')
      
   #-------------------------------------------------------------------------------
   # HEXAGON Flags Def.
   #-------------------------------------------------------------------------------
   env.Replace(BUILD_FLAGS = "-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 -DGPL_ENVIRONMENT=2 -D__qdsp6__=1")

   #-------------------------------------------------------------------------------
   # HEXAGON LINK, very special procedure
   #-------------------------------------------------------------------------------
   #env.Replace(LINKFLAGS = "-m${Q6VERSION} -nostdlib --section-start .start=${HEXAGON_IMAGE_ENTRY} ${HEXAGONLD_MAP_CMD} ${TARGET.posix}.map --entry=${HEXAGON_IMAGE_ENTRY} ${LFLAGS} ")
   env.Replace(LINKFLAGS = "-m${Q6VERSION} --gc-sections --wrap malloc --wrap calloc --wrap free --wrap realloc --wrap memalign --wrap __stack_chk_fail -g -T${BUILD_ROOT}/build/chipset/${CHIPSET}/elite.linker -G8 -nostdlib --section-start .start=${HEXAGON_IMAGE_ENTRY} ${LFLAGS}")

   # flags to support dynamic linking
   if not 'USES_PLATFORM_MINIMAL' in env:
      env.LoadToolScript('platform_builders', toolpath = ['${BUILD_ROOT}/platform/build'])
      env.Replace(DYNLINKFLAGS = "--force-dynamic --dynamic-linker= --dynamic-list=${BUILD_ROOT}/platform/exports/dynsymbols.lst -T${BUILD_ROOT}/platform/exports/externs.lst")
  

   # LINK special case in hexagon world
   env.Replace(QDSP6OBJS3  = [            
      '${QDSP6_RELEASE_LIB_DIR}/libc.a',
      '${QDSP6_RELEASE_LIB_DIR}/libqcc.a',
      '${QDSP6_RELEASE_LIB_DIR}/libstdc++.a',
      ])

   #-------------------------------------------------------------------------------------
   # if $SOURCES doesn't have any items in it, python is throwing an error $SOURCES.posix.
   # When linking there should always be at least one obj it is the entry point.
   # SOURCES should never be emty if it is someone added objects to library list instead 
   # of object list. Fix that isntead of trying to get creative here.
   #-------------------------------------------------------------------------------------
   env.Replace(LINKOBJGRP = "${QDSP6OBJS1} " \
      "--start-group " \
      "$_LIBFLAGS_POSIX " \
      "$SOURCES.posix $_LISTFILES ${QDSP6OBJS3} --end-group " \
      "${QDSP6OBJS4} "
   )  


   env.Replace(TARGET_MAP = "${TARGET.posix}.map")  
   env.Replace(LINKCOM = "${TEMPFILE('$LINK $LINKFLAGS $LINK_DIRS $LINKOBJGRP $DYNLINKFLAGS -Map $TARGET_MAP $HEXAGON_OUTPUT_CMD $TARGET.posix $LINKFLAGS_END')}")

   #============================================================================
   #print env.Dump()

