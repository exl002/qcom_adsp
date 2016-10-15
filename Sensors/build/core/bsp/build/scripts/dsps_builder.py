#===============================================================================
#
# DSPS builders
#
# GENERAL DESCRIPTION
#    build rules script
#
# Copyright (c) 20011 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/build/core/bsp/build/scripts/dsps_builder.py#1 $
#  $DateTime: 2014/05/16 10:56:56 $
#  $Author: coresvc $
#  $Change: 5907808 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when        who     what, where, why
# --------    ---     ---------------------------------------------------------
# 03/31/2011  ag      Handle target's clientspec mapping
# 03/14/2011  ag      Change path to elf_modify. Issue on CRM builds alone.
# 03/10/2011  ag      Don't use bash
# 03/10/2011  ag      Handle compiling on Linux machines
# 02/14/2011  ag      Generate BuildProducts.txt
# 02/03/2011  ag      Initial version of build scripts for DSPS
#===============================================================================
import os
import subprocess
import string
import re
import shutil

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('dsps_builder')

def generate(env):
   dsps_clean_generate(env)
   dsps_mod_generate(env)
   dsps_pilify_generate(env)
   dsps_product_list_generate(env)

#===============================================================================
# Clean build rules
#===============================================================================
def dsps_clean_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for elf cleanup
   #
   dsps_clean_act = env.GetBuilderAction(dsps_clean_builder)
   dsps_clean_bld = env.Builder(action = dsps_clean_act,
                                suffix = '.clean.elf')

   env.Append(BUILDERS = {'DspsCleanBuilder' : dsps_clean_bld})

#===============================================================================
# Mod build rules
#===============================================================================
def dsps_mod_generate(env):
   #-------------------------------------------------------------------------------
   # def builder for shortcut address translation
   #
   dsps_mod_act = env.GetBuilderAction(dsps_mod_builder)
   dsps_mod_bld = env.Builder(action = dsps_mod_act,
                              suffix = '.mod.elf')

   env.Append(BUILDERS = {'DspsModBuilder' : dsps_mod_bld})

#===============================================================================
# Pilify build rules
#===============================================================================
def dsps_pilify_generate(env):
   #-------------------------------------------------------------------------------
   # def builder to generate binaries for PIL driver
   #
   dsps_pilify_act = env.GetBuilderAction(dsps_pilify_builder)
   dsps_pilify_bld = env.Builder(action = dsps_pilify_act,
                                 suffix = '.mdt')

   env.Append(BUILDERS = {'DspsPilifyBuilder' : dsps_pilify_bld})

#===============================================================================
# product_list build rules
#===============================================================================
def dsps_product_list_generate(env):
   #-------------------------------------------------------------------------------
   # def builder to generate binaries for PIL driver
   #
   dsps_product_list_act = env.GetBuilderAction(dsps_product_list_builder)
   dsps_product_list_bld = env.Builder(action = dsps_product_list_act,
                                       suffix = '.txt')

   env.Append(BUILDERS = {'DspsProductListBuilder' : dsps_product_list_bld})

#-------------------------------------------------------------------------------
# Emitter builder to add clean actions
#
def dsps_emitter(target, source, env):
   # add dependencies (if it needs to be build) here if needed
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))
   #target_base, target_fname = os.path.split(target_str)
   target_base, target_fname_ext = os.path.extsplit(target_str)
   env.Clean(target_base+".b00")
   env.Clean(target_base+".b01")
   env.Clean(target_base+".b02")
   env.Clean("${BUILD_ROOT}/../BuildProducts.txt")
   return (target, source)

#-------------------------------------------------------------------------------
# def builder for elf cleanup
#
def dsps_clean_builder(target, source, env):
   # init variables
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))

   cmds = ["$BINTOOL ${ARMBIN_NODEBUG_CMD} ${ARMBIN_NOLINKVIEW_CMD} ${ARMBIN_ELF_CMD} ${ARMBIN_OUTPUT_CMD} " + target_str + " " + source_str]

   data, err, rv = env.RunCmds(target, source, cmds)
   
   return None

#-------------------------------------------------------------------------------
# def builder for shortcut address translation
#
def dsps_mod_builder(target, source, env):
   # init variables
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))

   if env['PLATFORM'] == 'linux':
      env.Replace(ELF_MOD_CMD = "./elf_modify_linux.exe")
   else:
      env.Replace(ELF_MOD_CMD = "./elf_modify.exe")

   exec_dir = "${BUILD_ROOT}/core/bsp/build/"
   cmds = ["$ELF_MOD_CMD -i " + source_str + " -o " + target_str]

   data, err, rv = env.ExecCmds(cmds, dir=exec_dir)
   
   return None

#-------------------------------------------------------------------------------
# def builder to generate binaries for PIL driver
#
def dsps_pilify_builder(target, source, env):
   # init variables
   source_str = os.path.abspath(str(source[0]))
   target_str = os.path.abspath(str(target[0]))

   if env['PLATFORM'] == 'linux':
      env.Replace(PILIFY_CMD = "./pilify_dsps.sh")
   else:
      env.Replace(PILIFY_CMD = "bash pilify_dsps.sh")

   exec_dir = "${BUILD_ROOT}/core/bsp/build/"
   cmds = ["$PILIFY_CMD "+ source_str]

   data, err, rv = env.ExecCmds(cmds, dir=exec_dir, shell=True)
   
   return None
#-------------------------------------------------------------------------------
# def builder to generate build product list
#
def dsps_product_list_builder(target, source, env):

   if env['PLATFORM'] == 'linux':
      env.Replace(PROD_LIST_CMD = "./gen_product_list_dsps.sh")
   else:
      env.Replace(PROD_LIST_CMD = "bash gen_product_list_dsps.sh")

   exec_dir = "${BUILD_ROOT}/core/bsp/build/"
   cmds = ["$PROD_LIST_CMD"]

   data, err, rv = env.ExecCmds(cmds, dir=exec_dir, shell=True)

   return None
