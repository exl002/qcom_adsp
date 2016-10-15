#===============================================================================
#
# CoreBSP DSPS tool rules
#
# GENERAL DESCRIPTION
#    rules build script
#
# Copyright (c) 20011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/build/core/bsp/build/scripts/dsps_defs.py#1 $
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
# 2/3/2011   ag      Initial version for DSPS. Remove modem proc definition
#===============================================================================
import sys
import os
import string

# Assembly MODEM compile flags (note first pass is through armcc using -E option then passed to armas, see 
# build rule below
dsps_asm_dflags = '-DCUST_H=\\"${CUST_H}\\" -DCORE_SPS'

# standard DSPS compile flags
dsps_cc_dflags = '-DCUST_H=\\"${CUST_H}\\" -DCORE_SPS ' \
      '-DBUILD_TARGET=\\"${BUILD_ID}\\" -DBUILD_VER=\\"${BUILD_VER}\\" -DBUILD_ASIC=\\"${BUILD_ASIC}\\"'
      
#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('dspstools_defs')

def generate(env):
   # Assembly common flags
   env.Replace(ASM_DFLAGS = dsps_asm_dflags)
   
   # CC (apps) common compile flags
   env.Replace(CC_DFLAGS = dsps_cc_dflags)
   

