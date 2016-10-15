#===============================================================================
#
# Defaults
#
# GENERAL DESCRIPTION
#    Defualt values added to build system environment
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/scons/scripts/defaults.py#2 $
#  $DateTime: 2013/06/27 10:00:54 $
#  $Author: coresvc $
#  $Change: 3997127 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 07/28/10   wd      For QAPI 1.0.2
#                    * Automatic Cloning, sub scripts no longer need to clone 
#                      the environment it is done for them before loading the 
#                      script.
#                    * Added APIs for AddFileList(), AddArtifacts().
#                    * Added initial for external build system builder.
#                    * Changed config.py to use our won wrapper when loading 
#                      .py files to get better debugging info.
#                     
# 04/24/09   wd      Create
#
#===============================================================================
import os
import atexit
import re
from SCons.Script import *
from datetime import datetime

#------------------------------------------------------------------------------
# Global to keep time info
#------------------------------------------------------------------------------
global build_start_time
global default_env

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('defaults')

def generate(env):
   global build_start_time
   global default_env
   
   build_start_time = datetime.now()
   default_env = env
   
   env['BUILD_TIME_SATRT'] = build_start_time
   # ensure minimum version sof python and SCons
   env.EnsurePythonVersion(2, 5)
   env.EnsureSConsVersion(1, 0)

   
   #============================================================================
   # Changes to SCONS_QAPI
   #============================================================================
   # 1.0.3 - Added ability to call external build system builders.
   #         Complete implementation to AddLibrary, AddBinaryLibrary.
   # 1.0.4 - Added capability to control SCONS num of jobs trough OS enviroment.
   # 1.0.5 - Added capability clean pack
   # 1.0.6 - Changed to AddBinaryLibrary, add optional exception tags
   env.Replace(SCONS_QAPI  = "1.0.6")

   #============================================================================
   # Get QC SCons ext reelase info
   #============================================================================
   ''' Find version information.
   Expected version info:
      Release label: @SCONS.QC.X
   
   Expected date format:
      $DateTime: 2013/06/27 10:00:54 $      
   '''
   
   release_text = env.subst('${BUILD_ROOT}/tools/build/scons/release.txt')
   if os.path.exists(release_text):
      rfile = open(release_text, 'r')
      rfile_data = rfile.read()
      
      rfile_ver_re = re.search('\@SCONS\.QC\.(.*)', rfile_data)
      rfile_date_re = re.search('DateTime\: (.* .*) ', rfile_data)
      
      if rfile_ver_re is not None:
         scons_qc_ver = rfile_ver_re.group(1)
      else:
         scons_qc_ver = "uknown"
         
      if rfile_date_re is not None:
         scons_qc_date = rfile_date_re.group(1)
         scons_qc_ver_txt = "%s (%s)" % (scons_qc_ver, scons_qc_date)
      else:
         scons_qc_ver_txt = scons_qc_ver
         
      env.Replace(SCONS_QC_VER = scons_qc_ver_txt)
  
   #============================================================================
   # Load essentials
   #============================================================================
   if 'BUILD_SCRIPTS_ROOT' not in env:
      build_scripts_root = os.environ.get('BUILD_SCRIPTS_ROOT')
      if build_scripts_root is None:
         build_scripts_root = '${BUILD_ROOT}/tools/build/scons/scripts'
      env.Replace(BUILD_SCRIPTS_ROOT = build_scripts_root)

   if 'BUILD_SCRIPTS_OEM_ROOT' not in env:
      build_scripts_oem_root = os.environ.get('BUILD_SCRIPTS_OEM_ROOT')
      if build_scripts_oem_root is None:
         build_scripts_oem_root = '${BUILD_ROOT}/tools/build/scons/scripts'
      env.Replace(BUILD_SCRIPTS_OEM_ROOT = build_scripts_oem_root)

   # load minimum required scripts to enable most of Qualcomm SCons infrastructure
   env.Tool('utils', toolpath = ['${BUILD_SCRIPTS_ROOT}'])
   env.Tool('cmdline', toolpath = ['${BUILD_SCRIPTS_ROOT}']) 
   
   env.PrintDebugInfo('scripts', "Loading DEFAULT scripts...")
   
   env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/formatting.py")
      
   #============================================================================
   # Load alternate OEM options
   #============================================================================
   defaults_oem = "${BUILD_SCRIPTS_OEM_ROOT}/defaults_oem.py"
   
   if env.PathExists(defaults_oem):
      env.LoadToolScript(defaults_oem)
      
   #============================================================================
   # Init defaults options
   #============================================================================
   set_db_name(env)
   set_num_jobs(env)
   reg_on_exit()
   
   # optimize scons, cache MD5, don't MD5 every single build
   # unless file time chnages
   env.SetOption('max_drift', 1) 
   env.Decider('MD5-timestamp')
   #env.SetOption('implicit_cache', 1)
   
   #if GetOption('clean'):
   #   env.SetOption('implicit-deps-unchanged', 1) 
   #   env.SetOption('implicit_cache', 0)

   # init dir and file name patterns
   image_root_dir_patterns = env.get('IMAGE_ROOT_DIR_PATTERNS')
   if image_root_dir_patterns is None:
      image_root_dir_patterns = ['${BUILD_ROOT}/*/bsp/*/build']
      env.Replace(IMAGE_ROOT_DIR_PATTERNS = image_root_dir_patterns)
      
   image_dir_patterns = env.get('IMAGE_DIR_PATTERNS')
   if image_dir_patterns is None:
      image_dir_patterns = ['*']
      env.Replace(IMAGE_DIR_PATTERNS = image_dir_patterns)
      
   au_root_dir_patterns = env.get('AU_ROOT_DIR_PATTERNS')
   if au_root_dir_patterns is None:
      au_root_dir_patterns = ['${BUILD_ROOT}/*/build']
      env.Replace(AU_ROOT_DIR_PATTERNS = au_root_dir_patterns)
      
   au_dir_patterns = env.get('AU_DIR_PATTERNS')
   if au_dir_patterns is None:
      au_dir_patterns = ['${BUILD_ROOT}/${AU_NAME}/build']
      env.Replace(AU_DIR_PATTERNS = au_dir_patterns)
      
   script_dir_patterns = env.get('SCRIPT_DIR_PATTERNS')
   if script_dir_patterns is None:
      script_dir_patterns = ['../*/build']
      env.Replace(SCRIPT_DIR_PATTERNS = script_dir_patterns)
      
   script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')
   if script_name_patterns is None:
      script_name_patterns = ['SConscript', '*.scons']
      env.Replace(SCRIPT_NAME_PATTERNS = script_name_patterns)
      
   api_script_name_patterns = env.get('API_SCRIPT_NAME_PATTERNS')
   if api_script_name_patterns is None:
      api_script_name_patterns = ['paths.sconscript', '*.api']
      env.Replace(API_SCRIPT_NAME_PATTERNS = api_script_name_patterns)
      
   build_warnings_for_oem = env.get('ENABLE_BUILD_WARNINGS_FOR_OEM')
   if build_warnings_for_oem is None:
      env.Replace(ENABLE_BUILD_WARNINGS_FOR_OEM = False)
      

   # Add command text formatting and utils to enviroment
   env.LoadToolScript("${BUILD_SCRIPTS_ROOT}/generic_builders.py")   
   env.LoadToolScript('${BUILD_SCRIPTS_ROOT}/scons_mod.py')
   env.LoadToolScript('${BUILD_SCRIPTS_ROOT}/config.py')

   return None

#------------------------------------------------------------------------------
# Register function for on exit event
#------------------------------------------------------------------------------
def print_build_failures():
   from SCons.Script import GetBuildFailures
   build_failures = GetBuildFailures()
   if build_failures:
      print '** Build errors...'
      for build_failure in build_failures:
         print "%s failed: %s" % (build_failure.node, build_failure.errstr)
      print ''
      
def on_exit():
   print
   print "=============================================================================="
   print "   SCons build summary"
   print "=============================================================================="

   print_build_failures()

   global build_start_time
   build_end_time = datetime.now()
   elapsed_time = build_end_time - build_start_time
   
   # computation per python instructions for timedelta.total_seconds() 
   # whihc is new in python 2.7
   #import pdb; pdb.set_trace()
   td = elapsed_time
   total_microseconds = td.microseconds + (td.seconds + td.days *  24 * 3600) * 10**6
   seconds, microseconds = divmod(total_microseconds, 10**6)
   minutes, seconds = divmod(seconds, 60)
   hours, minutes = divmod(minutes, 60)
   
   # always round up, as build start time and end time will alwasy round up, 
   # this way the values alway match (delta vs manually calculating from end-start).
   if microseconds > 0:
      seconds = seconds + 1

   print "** Build time..."
   print " Build start  : %s" % build_start_time.ctime()
   print " Build end    : %s" % build_end_time.ctime()
   print " Elapsed time : %d:%.2d:%.2d" % (hours, minutes, seconds)

def reg_on_exit():
   atexit.register(on_exit)

#------------------------------------------------------------------------------
# Set detabase name to be used... BUILD_ID must be defined
#------------------------------------------------------------------------------
def set_db_name(env):
   # create file name
   oem_bid = env.get('OEM_BUILD_ID')

   # OS environment overrides any other settings.
   oem_bid = os.environ.get('SCONS_OEM_BUILD_ID', oem_bid)
   
   db_ext = ".dblite"
   
   bid = str(env.get('BUILD_ID'))
   
   if oem_bid is not None:
      oem_bid = str(oem_bid)
      env.Replace(OEM_BUILD_ID = oem_bid)
      bid = "%s_%s" % (oem_bid, bid)
      
   db_name = ".sconsign_%s" % bid

   # remove db first file if force build option is enabled
   force_build = env.GetOption('force_build')
      
   if force_build:
      db_del_file = "%s%s" % (db_name, db_ext) 
      #import pdb; pdb.set_trace()
      
      if os.path.exists(db_del_file):
         os.unlink(db_del_file) 
         print
         env.PrintInfo("FORCE BUILD, cache db deleted [%s]" % db_del_file)         

   # set db filename
   SCons.SConsign.File(db_name)

#------------------------------------------------------------------------------
# Figure out number of jobs to use
#------------------------------------------------------------------------------
def set_num_jobs(env):
   default_max_num_jobs = 8
   default_num_jobs_factor = 2
   default_override_num_jobs = 0
   max_num_jobs = env.get('MAX_NUM_JOBS', default_max_num_jobs)
   num_jobs_factor = env.get('NUM_JOBS_FACTOR', default_num_jobs_factor)   
   override_num_jobs = env.get('OVERRIDE_NUM_JOBS', default_override_num_jobs)
   
   # OS enviroments overrides any other settings.
   max_num_jobs = int(os.environ.get('SCONS_MAX_NUM_JOBS', max_num_jobs))
   num_jobs_factor = int(os.environ.get('SCONS_NUM_JOBS_FACTOR', num_jobs_factor))
   override_num_jobs = int(os.environ.get('SCONS_OVERRIDE_NUM_JOBS', override_num_jobs))
   
   num_cpu = 1
   if hasattr( os, "sysconf" ): 
      # Linux
      if os.sysconf_names.has_key( "SC_NPROCESSORS_ONLN" ): 
         num_cpu = os.sysconf( "SC_NPROCESSORS_ONLN" ) 
   else:
      # Windows: 
      if os.environ.has_key( "NUMBER_OF_PROCESSORS" ): 
         num_cpu = int( os.environ[ "NUMBER_OF_PROCESSORS" ] ); 
         
   # figure out how many jobs to process
   num_jobs = num_cpu * num_jobs_factor

   if num_jobs > max_num_jobs:
      num_jobs = max_num_jobs
   
   if override_num_jobs != 0:
      num_jobs = override_num_jobs

   env.SetOption('num_jobs', num_jobs)

   print
   #print ("*** Found %d CPUs, running with -j %d" % (num_cpu, env.GetOption('num_jobs')))
   env.PrintInfo("Found %d CPUs, running with -j %d" % (num_cpu, env.GetOption('num_jobs')))
