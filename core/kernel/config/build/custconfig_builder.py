#===============================================================================
#
# CBSP Kernel Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.adsp/2.2/kernel/config/build/custconfig_builder.py#1 $
#  $DateTime: 2013/04/03 17:22:53 $
#  $Author: coresvc $
#  $Change: 3569480 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 03/24/11   sho     Generate Blast cust config
#
#===============================================================================
import os
import subprocess
import string
import re
from SCons.Script import *

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('custconfig_builder')

def generate(env):
   # load builder into enviroment
   cust_config_generate(env)

#-------------------------------------------------------------------------------
# Customer Configuration definition
#
def cust_config_generate(env):
   cust_config_act = env.GetBuilderAction(cust_config_builder)
   cust_config_bld = env.Builder(action = cust_config_act, 
      emitter = cust_config_emitter
      )
   env.Append(BUILDERS = {'CustConfigBuilder' : cust_config_bld})

#-------------------------------------------------------------------------------
# Cust Config build Emitter to add clean actions and otehr misc
#
def cust_config_emitter(target, source, env):
   # Add dependencies on scripts
   env.Depends(target, "${QDSP6_SCRIPT_DIR}/Input/cust_config.c")
   env.Depends(target, "${QDSP6_SCRIPT_DIR}/qurt_config.py")

   # clean side effects
   env.Clean(target, "${BUILDPATH}/qurt_config.c")

   return (target, source)

#-------------------------------------------------------------------------------
# Cust Config build Action
#
def cust_config_builder(target, source, env):
   cust_config_xml = env.RealPath(str(source[0]),posix=True)
   cust_config_obj = env.RealPath(str(target[0]),posix=True)

   cmd = "${PYTHONCMD} ${QDSP6_SCRIPT_DIR}/qurt_config.py update -T ${QDSP6_RELEASE_DIR} -o " + cust_config_obj + " " + cust_config_xml

   data, err, rv = env.ExecCmds(cmd)
   return None
