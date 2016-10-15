
#=================================================================================
#   File Name: build.py
#
#   Python Script to call the QC-SCons build System for Badger
#
#   Copyright (c) 2013 QUALCOMM Technologies Incorporated.
#   All rights reserved. Qualcomm Proprietary and Confidential.
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/2.2/build.py#18 $
#  $DateTime: 2014/03/05 04:51:11 $
#  $Change: 5394000 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#   when     who            what, where, why
# --------   ---        ---------------------------------------------------------
# 03/04/13   Haseeb Khan    Script to call the QC-SCons build System for Badger    
# 06/12/13   corinc      Moved Build.py to adsp_proc\build\
#=================================================================================


import os
import re
import subprocess
import sys
import time
import optparse
import fnmatch
import shutil, errno



if sys.version:
   print "\n\nPython Version::\n", sys.version
   if sys.version_info[0] != 2:
       print "ERROR:: You are not using Python 2.x. Please use 2.x, preferably 2.6.2"
       sys.exit(0)
else:
   print '\n\nERROR: Python not installed!!!'
   print 'If installed already, please verify if respective path added to PATH environment variable!!!\n\n'
   sys.exit(0)

# Checking current working directory.
cwd_dir = os.getcwd()
adspproc_dir = os.getcwd()
print "\n\n checking adsp_proc, current working directory: %s" % adspproc_dir

# Now changing the directory to adsp_proc
adsp_dir = "adsp_proc"
bsp_dir  = "bsp"
ms_dir   = "ms"

if os.path.exists(adsp_dir):
    os.chdir(adsp_dir)
    print "1. Changed to \\adsp_proc\n\n"
    pass
elif os.path.exists(bsp_dir) and os.path.exists(ms_dir):
    os.chdir('../')
    print "2. in \\adsp_proc\\build, moved up to \\adsp_proc \n\n"
    pass
else:
    print "3. Your are already in \\adsp_proc\n\n"
    pass # do nothing!

     
#=================================================================================
#=================================================================================
#                  Software Paths & other definitions are below
#                    (edit these based on requirement)
#=================================================================================
#=================================================================================

#Q6 Version
q6_version = 'v5'

#RTOS tools version
q6_rtos_tools_version = '5.0.10'

#=============   For Windows Environment (Recommended locations)   =============
#Hexagon tools path for windows
#Donot specify path till version folder, path will be appended later using 'q6_rtos_tools_version'
q6_tools_path_win = r"C:/Qualcomm/HEXAGON_Tools"

#Python installation path for windows
#This python path is for internal builds in Qualcomm
#User need not modify it as python path will be directly taken from local path
python_win = r"C:/CRMapps/Apps/Python262"

if not os.environ.has_key('OPENSSL_DIR'):
   os.environ['OPENSSL_DIR'] = r"C:\utils\cygwin\bin"

#=============   End of Windows Environment (Recommended locations)   =============


#=============   For Linux Environment (Recommended locations)   =============
#Hexagon tools from Local Linux machine
#Note: Hexagon tools from local Linux machine will be preferred if both(Local and Network paths) are present
q6_tools_path_linux = 'None'
if os.environ.get('HOME', None):   
   home_dir = os.environ.get('HOME', None)
   print '\nHOME directory is:', home_dir
   #Donot specify path till version folder, path will be appended later using 'q6_rtos_tools_version'
   q6_tools_path_linux = home_dir + "/Qualcomm/HEXAGON_Tools"

#Hexagon tools from Network Location
#Donot specify path till version folder, path will be appended later using 'q6_rtos_tools_version'
q6_tools_netpath_linux = '/pkg/qct/software/hexagon/releases/tools'

#Python installation path for Linux
#This python path is for internal builds in Qualcomm
#User need not modify it as python path will be directly taken from local path
python_linux = "/pkg/python/2.6.2/bin"




#=============   End of Linux Environment (Recommended locations)   =============

#=================================================================================
#=================================================================================
#                  Software Paths & other definitions ends here
#                    (edit these based on requirement)
#=================================================================================
#=================================================================================










#===================================================================================
#=================     Required Versions        ====================================
#                   (edit these based on requirement)
#===================================================================================
#=== Please update, if you wish to change to new hexagon toolset and q6 version. ===
#=== This would reflect current required versions for build system               ===
#===    and compilation would error out if the required versions are changed.    ===
#===================================================================================
if os.environ.get('HEXAGON_REQD_Q6VERSION', None):
   print 'HEXAGON_REQD_Q6VERSION is already set: ', os.environ.get('HEXAGON_REQD_Q6VERSION', None)
else:
   os.environ['HEXAGON_REQD_Q6VERSION'] = 'v5'

if os.environ.get('HEXAGON_REQD_RTOS_RELEASE', None):   
   print 'HEXAGON_REQD_RTOS_RELEASE is already set: ', os.environ.get('HEXAGON_REQD_RTOS_RELEASE', None)
else:
   os.environ['HEXAGON_REQD_RTOS_RELEASE'] = '5.0.10'




#=================================================================================
#=================   End fof Required Versions   =================================
#                 (edit these based on requirement)
#=================================================================================










#=================================================================================
#=================================================================================
#                  Function definitions are below
#=================================================================================
#=================================================================================

# Function definition 'set_chipset_target' is here
def set_chipset_target(str, chipset_param, opts_chipset):
# Setting CHIPSET and TARGET here
   
   if str=='M8974AAAAAAAZL':
     os.environ['CHIPSET'] = "msm8974"
     os.environ['TARGET'] = "8974"
     
   if str=='M9625AAAAANAZL':
     os.environ['CHIPSET'] = "mdm9x25"
     os.environ['TARGET'] = "9x25"
   
   if str=='A8094AAAAAAAZL':
     os.environ['CHIPSET'] = "apq8094"
     os.environ['TARGET'] = "8094"
     
   if str=='M8626AAAAAAAZL':
     os.environ['CHIPSET'] = "msm8x26"
     os.environ['TARGET'] = "8x26"
     
   if str=='M8092AAAAAAAZL':
     os.environ['CHIPSET'] = "mpq8092"
     os.environ['TARGET'] = "8092" 
     
   if str=='M8610AAAAAAAZL':
     os.environ['CHIPSET'] = "msm8x10"
     os.environ['TARGET'] = "8x10"
     
   if str=='A8084AAAAAAAZL':
     os.environ['CHIPSET'] = "apq8084"
     os.environ['TARGET'] = "8084"

   if str=='M8962AAAAAAAZL':
     os.environ['CHIPSET'] = "msm8962"
     os.environ['TARGET'] = "8962"

	 
   if str=='M9635AAAAANAZL':
     os.environ['CHIPSET'] = "mdm9x35"
     os.environ['TARGET'] = "9x35"
	 
   if str is None or opts_chipset:
        # -c option provided from command-line over-rides chipset extraction from BLD_ENV_BUILD_ID
        os.environ['CHIPSET'] = chipset_param
	chipset_match = re.search('...(\d.\d\d)', chipset_param)
        os.environ['TARGET'] = chipset_match.group(1)        

   
   print 'CHIPSET: ', os.environ['CHIPSET']
   print 'TARGET: ', os.environ['TARGET']
   print '\n'
   
   return;


# Function definition 'cosim_tfw' is here
def cosim_tfw():
# only cosim build test      
      os.environ['VS90COMNTOOLS'] = "C:/Program Files (x86)/Microsoft Visual Studio 9.0/Common7/Tools/"
      print 'Build Cosim...'
      cosim_build_cmd = ''.join(['adsptest-build.cmd '])
      print cosim_build_cmd
      proc = subprocess.Popen(cosim_build_cmd, shell=True)
      (out, err) = proc.communicate()
      
      return;
   
# Function to Verify if watchdog timeout happened during CoSim test
def verify_test_log(logfile_name_temp):
       result_flag = 0
       logfile = open(logfile_name_temp, 'r')
       
       for line in logfile:        
          test_match = re.search('WATCHDOG TIMEOUT EXPIRED!', line)
          if test_match:
             result_flag = 1             
       logfile.close()
       
       if os.path.exists(logfile_name_temp):
             try:
                     os.remove(logfile_name_temp)                             
             except:
                     print "Exception: ",str(sys.exc_info())
       else:
             print "File '%s' not found" % logfile_name_temp
             
       return result_flag
       
   
# Function definition 'cosim_tfw_run' is here
def oemroot_cosim_tfw_run():
# cosim build and test      
      print 'Run CoSim Test...'
      print '\n\nRunning \'elite_examples.qtfw\' Test...\n'
      cosim_run_cmd = ''.join(['adsptest-run.cmd --test elite_examples.qtfw ', '--watchdog-timeout 2000 --- --dsp_clock 83 > ../../opendsp_elite_examples.log 2>&1'])    
      print cosim_run_cmd
      proc = subprocess.Popen(cosim_run_cmd, shell=True)
      #waiting for 10 mins before terminating the tfwk process
      #exits if process completes before time-out
      elite_time_out = 600.0
      elapsed = 0
      flag_elapsed = 0
      wait_b4_poll = 120  # wait for 2 mins
      while proc.poll() is None:
             time.sleep(wait_b4_poll)
             elapsed = elapsed + wait_b4_poll
             #print "elapsed = %s" % elapsed
             logfile_name = '../../opendsp_elite_examples.log'
             logfile_name_temp = '../../opendsp_elite_examples_temp.log'
             if os.path.exists(logfile_name):
                 shutil.copy(logfile_name, logfile_name_temp)               
                 result_flag = verify_test_log(logfile_name_temp)   #Verify if watchdog timeout happened during CoSim test             
             if elapsed > elite_time_out or result_flag == 1:
                  subprocess.Popen("start taskkill /F /T /PID %i"%proc.pid , shell=True)
                  if elapsed > elite_time_out:
                     print "elite_examples Test process is killed as it exceeded time-out of (in secs): ", elite_time_out
                  if result_flag == 1:
                     print "elite_examples Test process is killed as watchdog timer (2 Secs) is expired !!!"                  
                  flag_elapsed = 1
      if flag_elapsed == 0:
         print "elite_examples Test process completed (in secs) before time-out: ", elapsed   
      
      
      print '\n\nRunning \'example_capi.qtfw\' Test...\n'
      cosim_run_cmd = ''.join(['adsptest-run.cmd --test example_capi.qtfw ', '--watchdog-timeout 2000 --- --dsp_clock 83 > ../../opendsp_example_capi.log 2>&1'])    
      print cosim_run_cmd
      proc = subprocess.Popen(cosim_run_cmd, shell=True)
      #waiting for 10 mins before terminating the tfwk process
      #exits if process completes before time-out
      capi_time_out = 600.0
      elapsed = 0
      flag_elapsed = 0
      while proc.poll() is None:
             time.sleep(wait_b4_poll)
             elapsed = elapsed + wait_b4_poll
             #print "elapsed = %s" % elapsed
             logfile_name = '../../opendsp_example_capi.log'
             logfile_name_temp = '../../opendsp_example_capi_temp.log'
             if os.path.exists(logfile_name):
                 shutil.copy(logfile_name, logfile_name_temp)               
                 result_flag = verify_test_log(logfile_name_temp)   #Verify if watchdog timeout happened during CoSim test
             if elapsed > capi_time_out or result_flag == 1:
                  subprocess.Popen("start taskkill /F /T /PID %i"%proc.pid , shell=True)
                  if elapsed > capi_time_out:
                     print "example_capi Test process is killed as it exceeded time-out of (in secs): ", capi_time_out
                  if result_flag == 1:
                     print "example_capi Test process is killed as watchdog timer (2 Secs) is expired !!!"                  
                  flag_elapsed = 1
      if flag_elapsed == 0:
         print "example_capi Test process completed (in secs) before time-out: ", elapsed    
      
      
      print '\n\nRunning \'examples.qtfw\' Test...\n'
      cosim_run_cmd = ''.join(['adsptest-run.cmd --test examples.qtfw ', '--watchdog-timeout 2000 --- --dsp_clock 83 > ../../opendsp_examples.log 2>&1'])    
      print cosim_run_cmd
      proc = subprocess.Popen(cosim_run_cmd, shell=True)
      #waiting for 10 mins before terminating the tfwk process
      #exits if process completes before time-out
      examples_time_out = 600.0
      elapsed = 0
      flag_elapsed = 0
      while proc.poll() is None:
             time.sleep(wait_b4_poll)
             elapsed = elapsed + wait_b4_poll
             #print "elapsed = %s" % elapsed
             logfile_name = '../../opendsp_examples.log'
             logfile_name_temp = '../../opendsp_examples_temp.log'
             if os.path.exists(logfile_name):
                 shutil.copy(logfile_name, logfile_name_temp)               
                 result_flag = verify_test_log(logfile_name_temp)   #Verify if watchdog timeout happened during CoSim test           
             if elapsed > examples_time_out or result_flag == 1:
                  subprocess.Popen("start taskkill /F /T /PID %i"%proc.pid , shell=True)
                  if elapsed > examples_time_out:
                     print "examples Test process is killed as it exceeded time-out of (in secs): ", examples_time_out
                  if result_flag == 1:
                     print "examples Test process is killed as watchdog timer (2 Secs) is expired !!!"
                  flag_elapsed = 1
      if flag_elapsed == 0:
         print "examples Test process completed (in secs) before time-out: ", elapsed
      
      
      adsp_dir = "../.."
      try:
           os.chdir(adsp_dir)
           cwd_dir = os.getcwd()
           print "\n\nCurrent working directory now changed to %s" % cwd_dir
      except os.error:
           print "Your are already in 'adsp_proc' or not able to change directory to this directory"
           pass # do nothing!
         
      return;      


   
# Function definition 'cosim_tfw_run' is here
def cosim_tfw_run():
# cosim build and test      
      print 'Run CoSim Test...'
      print '\n\nRunning Sanity Test...\n'
      cosim_run_cmd = ''.join(['adsptest-run.cmd --test sanity.qtfw ', '--watchdog-timeout 2000 --- --dsp_clock 83 > ../../opendsp_sanity.log 2>&1'])    
      print cosim_run_cmd
      proc = subprocess.Popen(cosim_run_cmd, shell=True)
      #waiting for 30 mins before terminating the tfwk process
      #exits if process completes before time-out      
      sanity_time_out = 1800.0
      elapsed = 0
      flag_elapsed = 0
      wait_b4_poll = 120  # wait for 2 mins
      while proc.poll() is None:
             time.sleep(wait_b4_poll)
             elapsed = elapsed + wait_b4_poll
             #print "elapsed = %s" % elapsed
             logfile_name = '../../opendsp_sanity.log'
             logfile_name_temp = '../../opendsp_sanity_temp.log'
             if os.path.exists(logfile_name):
                 shutil.copy(logfile_name, logfile_name_temp)               
                 result_flag = verify_test_log(logfile_name_temp)   #Verify if watchdog timeout happened during CoSim test             
             if elapsed > sanity_time_out or result_flag == 1:
                  subprocess.Popen("start taskkill /F /T /PID %i"%proc.pid , shell=True)
                  if elapsed > sanity_time_out:
                     print "Sanity Test process is killed as it exceeded time-out of (in secs): ", sanity_time_out
                  if result_flag == 1:
                     print "Sanity Test process is killed as watchdog timer (2 Secs) is expired !!!"                  
                  flag_elapsed = 1
      if flag_elapsed == 0:
         print "Sanity Test process completed (in secs) before time-out: ", elapsed
       
            
      print '\n\nRunning Lua-Sanity Test...\n'
      cosim_run_cmd = ''.join(['adsptest-run.cmd --test lua-sanity.qtfw ', '--watchdog-timeout 2000 --- --dsp_clock 83 > ../../opendsp_lua_sanity.log 2>&1'])    
      print cosim_run_cmd
      proc = subprocess.Popen(cosim_run_cmd, shell=True)
      #waiting for 30 mins before terminating the tfwk process
      #exits if process completes before time-out
      lua_time_out = 1800.0
      elapsed = 0
      flag_elapsed = 0
      while proc.poll() is None:
             time.sleep(wait_b4_poll)
             elapsed = elapsed + wait_b4_poll
             #print "elapsed = %s" % elapsed
             logfile_name = '../../opendsp_lua_sanity.log'
             logfile_name_temp = '../../opendsp_lua_sanity_temp.log'
             if os.path.exists(logfile_name):
                 shutil.copy(logfile_name, logfile_name_temp)               
                 result_flag = verify_test_log(logfile_name_temp)   #Verify if watchdog timeout happened during CoSim test             
             if elapsed > lua_time_out or result_flag == 1:
                  subprocess.Popen("start taskkill /F /T /PID %i"%proc.pid , shell=True)
                  if elapsed > lua_time_out:
                     print "Lua-Sanity Test process is killed as it exceeded time-out of (in secs): ", lua_time_out
                  if result_flag == 1:
                     print "Lua-Sanity Test process is killed as watchdog timer (2 Secs) is expired !!!"                  
                  flag_elapsed = 1
      if flag_elapsed == 0:
         print "Lua-Sanity Test process completed (in secs) before time-out: ", elapsed
      
      
      adsp_dir = "../.."
      try:
           os.chdir(adsp_dir)
           cwd_dir = os.getcwd()
           print "\n\nCurrent working directory now changed to %s" % cwd_dir
      except os.error:
           print "Your are already in 'adsp_proc' or not able to change directory to this directory"
           pass # do nothing!
         
      return;      


# Function definition 'check_success' is here
def check_success():
# check if success file present
      file_name="success"
      if os.path.exists(file_name):
          try:
              os.remove(file_name)
          except:
              print "Exception: ",str(sys.exc_info())
      else:
          print "File '%s' not found" % file_name


      return;

# Function definition 'verify_args' is here
def verify_args(str, array_var):
      arg_flag = 0         
      for each_element in array_var:         
         match = re.search(str, each_element, re.I)
         if match: arg_flag = 1         

      return arg_flag;      

# Function definition 'other_options_cb' is here
def other_options_cb(option, opt_str, value, parser):
        args=[]
        for arg in parser.rargs:
                if arg[0] != "-":
                        args.append(arg)
                else:
                        del parser.rargs[:len(args)]
                        break
        if getattr(parser.values, option.dest):
                args.extend(getattr(parser.values, option.dest))
        setattr(parser.values, option.dest, args)


# Function definition 'summary_build' is here
def summary_build(default_buildid, default_chipset, default_pid, buildid_flag, chipset_flag, protection_domain_flag, klocwork_flag, flags_param):
      print '\n\n********************************************************'
      print '*************** Summary Build Environment **************'
      print '********************************************************'      
      tool_version = ''.join(['uname -a'])      
      proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)      
      (out, err) = proc.communicate()
      if out:
         print "Operating System::", out
      else:
         print "uname is not recognized!!!"
      
      if sys.version:
         print "\n\nPython Version::\n", sys.version

         if sys.version_info[0] != 2:
             print "ERROR:: You are not using Python 2.x. Please use 2.x, preferably 2.6.2"
             sys.exit(0)
      else:
         print '\n\nERROR: Python not installed!!!'
         print 'If installed already, please verify if respective path added to PATH environment variable!!!\n\n'
         sys.exit(0)
      
      tool_version = ''.join(['perl -v'])
      proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)
      (out, err) = proc.communicate()
      if out:
         print "Perl Version::", out
      else:
         print '\n\nWARNING: Perl not installed to execute Post Build Step (Memory Profiling Scripts)!!!\n\n'

      
      tool_version = ''.join(['hexagon-sim --version'])
      proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)

      (out, err) = proc.communicate()      
      if out:         


         tool_version = ''.join(['hexagon-sim --version > ./build/toolsver.txt 2>&1'])
         proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)
         (out, err) = proc.communicate()
         toolsver_filelist = find_files('build', 'toolsver.txt')
	 if toolsver_filelist:
	    toolsver_file = open( "build/toolsver.txt", "r" )
	    lines = []
	    for line in toolsver_file:	       
	       toolsver_match = re.search('Hexagon Build Version .(\d.\d.\d\d)', line)
	       if toolsver_match:
	          tools_ver = toolsver_match.group(1)
	          print 'You are using Hexagon Tools Version: ', tools_ver
	    toolsver_file.close()
	    os.remove('build/toolsver.txt')
	    if tools_ver != (os.environ['HEXAGON_REQD_RTOS_RELEASE']):	       
	       print '\nERROR: Please install Required Hexagon Tools Version: ', os.environ['HEXAGON_REQD_RTOS_RELEASE']
               print '       If it is intentional, please update HEXAGON_REQD_RTOS_RELEASE at \'Required Versions\' section in \'build.py\' accordingly!!!'
               print '         (OR) set HEXAGON_REQD_RTOS_RELEASE environment variable accordingly!!!'
               sys.exit(0)
      else:         
         print '\n\nERROR: Hexagon Tools not installed!!!'
         print 'Recommended locations:'
         print '     Windows: C:\Qualcomm\HEXAGON_Tools'
         print '     Linux: $HOME/Qualcomm/HEXAGON_Tools (OR) /pkg/qct/software/hexagon/releases/tools'
         print '     Note: If installed at other locations, please update \'Software Paths & other definitions\' section in \'build.py\' accordingly'
         print '           (OR) set HEXAGON_ROOT and HEXAGON_RTOS_RELEASE environment variables accordingly'
         print '                 Eg: set HEXAGON_ROOT=C:\Qualtools\Hexagon'
         print '                     set HEXAGON_RTOS_RELEASE=5.0.10'
         sys.exit(0)
      
      print 'Q6VERSION=', os.environ['Q6VERSION']
      print 'Q6_TOOLS_ROOT=', os.environ['Q6_TOOLS_ROOT']
      if buildid_flag:
         print 'QDSP6_BUILD_VERSION=', os.environ.get('QDSP6_BUILD_VERSION', None)
      elif default_buildid:
         print "QDSP6_BUILD_VERSION= [Default Value Taken from file 'adsp_proc/hap/default_pack.txt']:", os.environ.get('QDSP6_BUILD_VERSION', None)         
      else:
         print 'QDSP6_BUILD_VERSION= [Default Value Taken]', os.environ.get('QDSP6_BUILD_VERSION', None)
      
      if chipset_flag:
         print 'CHIPSET=', os.environ.get('CHIPSET', None)
         print 'TARGET=',  os.environ.get('TARGET', None)
      elif default_chipset:
         print "CHIPSET= [Default Value Taken from file 'adsp_proc/hap/default_pack.txt']:", os.environ.get('CHIPSET', None)         
         print "TARGET= [Default Value Taken from file 'adsp_proc/hap/default_pack.txt']:",  os.environ.get('TARGET', None)
      else:
         print 'CHIPSET= [Default Value Taken]', os.environ.get('CHIPSET', None)
         print 'TARGET= [Default Value Taken]',  os.environ.get('TARGET', None)
      
      if protection_domain_flag:
         print 'BUILD_FLAVOR=',  os.environ.get('BUILD_FLAVOR', None)
      elif default_pid:
         print "BUILD_FLAVOR= [Default Value Taken from file 'adsp_proc/hap/default_pack.txt']:", os.environ.get('BUILD_FLAVOR', None)
      else:   
         print 'BUILD_FLAVOR= [Default Value Taken]',  os.environ.get('BUILD_FLAVOR', None)   
      
      print 'BUILD_ACT=',  os.environ.get('BUILD_ACT', None)
      
      arg_flag = 0
      if other_option_flag:
         arg_flag = verify_args('\Aklocwork\Z', opts.other_option)         
         if arg_flag or klocwork_flag:
            print 'Klocwork is Enabled!!!'
         else:
            print 'Klocwork is NOT Enabled!!!'   
      
      arg_flag = 0
      if other_option_flag:
         arg_flag = verify_args('cosim', opts.other_option)         
         if arg_flag:
            arg_flag = 0
            arg_flag = verify_args('\Acosim\Z', opts.other_option)         
            if arg_flag:      
               print 'Build & Run of CoSim Test is Enabled!!!'
            arg_flag = 0            
            arg_flag = verify_args('\Acosim_run\Z', opts.other_option)         
            if arg_flag:      
               print 'Only Run of CoSim Test is Enabled!!!'      
         else:
            print 'Build & Run of CoSim Test is NOT Enabled!!!'
                  
      print 'Build Flags enabled are:', flags_param
      print "\nFor Build Command help, use -h option: python build.py -h"
      print '********************************************************'      
      print '************** End of Build Environment ****************'
      print '********************************************************'
      print '\n'
 

def find_files(base, pattern):
    '''Return list of files matching pattern in base folder.'''
    return [n for n in fnmatch.filter(os.listdir(base), pattern) if
        os.path.isfile(os.path.join(base, n))]

def test_framework(opts, other_option_flag, new_path):
   arg_flag = 0
   if other_option_flag:
      arg_flag = verify_args('cosim', opts.other_option)         
   if arg_flag or cosim_flag or cosim_run_flag:
         new_cosim_path = ''.join([new_path, ';', local_path])
         os.environ['PATH'] = new_cosim_path
	 print 'New PATH before cosim/cosim_run:\n', os.environ['PATH']
         time.sleep(3)
         
         test_dir = "avs/aDSPSim"
         try:
              os.chdir(test_dir)
              cwd_dir = os.getcwd()
              print "\n\nCurrent working directory now changed to %s" % cwd_dir
         except os.error:
              print "Your are already in avs/aDSPSim or not able to change directory to this directory"
              pass # do nothing!
         
         arg_flag = 0
         if other_option_flag:
            arg_flag = verify_args('\Acosim\Z', opts.other_option)         
         if arg_flag or cosim_flag:
               cosim_tfw()   #Build cosim
               check_success()
               cosim_tfw_run()   #Test Cosim
       
         arg_flag = 0
         if other_option_flag:
            arg_flag = verify_args('\Acosim_run\Z', opts.other_option)         
         if arg_flag or cosim_run_flag: 
               check_success()
               cosim_tfw_run()   #Test Cosim
       
         arg_flag = 0
         if other_option_flag:
            arg_flag = verify_args('\Aoemroot_cosim_run\Z', opts.other_option)         
         if arg_flag or oemroot_cosim_run_flag: 
               check_success()
               oemroot_cosim_tfw_run()   #OEM_ROOT Test Cosim               
               
 

def postprocess_command_options(opts, args):
   #For windows: hypen('-') or double-dash('--') is converted to character 0x96 if build command is copy-pasted from outlook.
   #For Linux: hypen('-') or double-dash('--') is removed if build command is copy-pasted from outlook.
   
   #This function avoids script taking default value for the above cases by:
   #   a. replacing character 0x96 back to '-'.
   #   b. recognizing option without '-' or '--'.
   
   j = 0
   for i in args:
      temp = i.replace('\x96', '-')      
      args[j] = temp      
      if args[j] == '-b' or args[j] == 'b' or args[j] == '-buildid':
         opts.buildid = args[j+1]   
      if args[j] == '-c' or args[j] == 'c' or args[j] == '-chipset':
         opts.chipset = args[j+1]
      if args[j] == '-p' or args[j] == 'p' or args[j] == '-pid':
         opts.protection_domain = args[j+1]   
      if args[j] == '-o' or args[j] == 'o' or args[j] == '-others':
         opts.other_option = args[j+1:]
         opts.other_option = process_array(opts.other_option)
      if args[j] == '-f' or args[j] == 'f' or args[j] == '-flags':
         opts.flags = args[j+1:]            
         opts.flags = process_array(opts.flags)
      if args[j] == '-k' or args[j] == 'k' or args[j] == '-kloc':
         opts.kloc = True   
      if args[j] == '-h' or args[j] == 'h' or args[j] == '-help':         
         parser.print_help()
         print "\n\n"
         sys.exit(0)
      j = j+1
   return (opts, args)

def process_array(array):
      n = 0
      for m in array:         
         temp = m.replace('\x96', '-')         
         match = re.search('(-)', temp)         
         if match:            
            break
         else:            
            array[n] = temp
            n = n + 1
      array = array[:n]      
      return array;



def search_exe_tool(search_tool):
   if search_tool == 'which':
     q6_tools_path_linux = 'None'
     q6_tools_linux = 'None'
     tools_find = ''.join([search_tool, ' hexagon-sim'])
     proc = subprocess.Popen(tools_find, stdout=subprocess.PIPE, shell=True)
     (out, err) = proc.communicate()
     tools_match = re.search('(.*)(\d.\d.\d\d)', out)
     if tools_match:
        tools_path = tools_match.group(1).replace('\\', '/').rstrip('/')        
        if os.path.exists(tools_path):
           print 'For Linux: Hexagon tools taken from local path and not from recommended path: ', tools_path
           q6_tools_path_linux = tools_path
           q6_tools_linux = q6_tools_path_linux + "/" + q6_rtos_tools_version
     return (q6_tools_path_linux, q6_tools_linux)
   
   if search_tool == 'where':
     q6_tools_path_win = 'None'
     tools_find = ''.join(['where hexagon-sim'])
     proc = subprocess.Popen(tools_find, stdout=subprocess.PIPE, shell=True)
     (out, err) = proc.communicate()     
     tools_match = re.search('(.*)(\d.\d.\d\d)', out)   
     if tools_match:
        tools_path = tools_match.group(1).replace('\\', '/').rstrip('/')        
        if os.path.exists(tools_path):
           print 'For Windows: Hexagon tools taken from local path and not from recommended path: ', tools_path
           q6_tools_path_win = tools_path          
     return (q6_tools_path_win)
   
   
#=================================================================================
#=================================================================================
#                  Function definitions ends here
#=================================================================================
#=================================================================================



#=================================================================================
#=================================================================================
#                  Main build.py starts here
#=================================================================================
#=================================================================================

#For internal builds in Qualcomm
kw_path = "\\\\qctdfsrt\\prj\\qct\\asw\\StaticAnalysis\\Windows\\Klocwork\\Server\\bin"
sagen_path = "\\\\stone\\aswcrm\\smart\\nt\\bin"

q6_tools_path_win = q6_tools_path_win.replace('\\', '/').rstrip('/')
python_win = python_win.replace('\\', '/').rstrip('/')
q6_tools_path_linux = q6_tools_path_linux.rstrip('/')
q6_tools_netpath_linux = q6_tools_netpath_linux.rstrip('/')
python_linux = python_linux.rstrip('/')

tool_version = ''.join(['uname -a'])
proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)
(out, err) = proc.communicate()
if out:
   platform_match = re.search('(\w\w\w\w\w)', out)
   match = platform_match.group(1)
   print "\nOperating System::", out
else:
   match = 0

        
if os.environ.get('HEXAGON_RTOS_RELEASE', None):
   q6_rtos_tools_version = os.environ['HEXAGON_RTOS_RELEASE']   
else:   
   os.environ['HEXAGON_RTOS_RELEASE'] = q6_rtos_tools_version
   
if os.environ.get('HEXAGON_Q6VERSION', None):
   q6_version = os.environ.get('HEXAGON_Q6VERSION', None)
else:
   os.environ['HEXAGON_Q6VERSION'] = q6_version      
   
q6_tools_linux = 'None'
if os.environ.get('HOME', None):   
   q6_tools_linux = q6_tools_path_linux + "/" + q6_rtos_tools_version   
if not os.path.exists(q6_tools_linux): 
   q6_tools_path_linux = q6_tools_netpath_linux
   q6_tools_linux = q6_tools_path_linux + "/" + q6_rtos_tools_version

if match == 'Linux':
   #For Linux: If Hexagon tools installed at location other than Recommended location
   #then this code would try and search hexagon tools in local path
   if not os.path.exists(q6_tools_linux):
      (q6_tools_path_linux, q6_tools_linux) = search_exe_tool('which')
else:
   #For Windows: If Hexagon tools installed at location other than Recommended location
   #then this code would try and search hexagon tools in local path 
   where_tools_path = q6_tools_path_win  + "/" + q6_rtos_tools_version
   if not os.path.exists(where_tools_path):      
      q6_tools_path_win = search_exe_tool('where')    
         
#=================================================================================


print '\nbuild.py usage help...'
usage = "Usage: python %prog -b <buildid> -c <chipset> -p <protection_domain> -o <others> -f <flags> -k \n \
                 --Command for build/clean of dsp image \n \
                   cd adsp_proc \n \
                      To Build dsp image:           python build.py -b 0x8fffffff -c msm8974 -o all   (by default -p mapped to 'mpd') \n \
                      To Build sim-only dsp image:  python build.py -b 0x8fffffff -c msm8974 -o sim   (by default -p mapped to 'mpd') \n \
                      To Clean dsp image:           python build.py -b 0x8fffffff -c msm8974 -o clean (by default -p mapped to 'mpd') \n\n \
                 --Command for build/clean of dsp image with default parameters \n \
                   cd adsp_proc \n \
                      To Build dsp image:           python build.py -o all    (this will take default parameters as: -b 0x8fffffff, -c msm8974 and -p mpd) \n \
                      To Build sim-only dsp image:  python build.py -o sim    (this will take default parameters as: -b 0x8fffffff, -c msm8974 and -p mpd) \n \
                      To Clean dsp image:           python build.py -o clean  (this will take default parameters as: -b 0x8fffffff, -c msm8974 and -p mpd) \n\n \
                 --Command for Only CoSim (Assuming the dsp image is already built) \n \
                   cd adsp_proc \n \
                      Only Build CoSim & Run CoSim:  python build.py -o cosim \n \
                      Only Run CoSim:                python build.py -o cosim_run \n\n \
                 --Command for build/clean of dsp image & CoSim \n \
                   cd adsp_proc \n \
                      To Build dsp image & Build CoSim, Run CoSim:           python build.py -o all cosim \n \
                      To Build dsp image & Run CoSim:                        python build.py -o all cosim_run \n \
                      To Build sim-only dsp image & Build CoSim, Run CoSim:  python build.py -o sim cosim \n \
                      To Build sim-only dsp image & Run CoSim:               python build.py -o sim cosim_run \n \
                      To Build dsp image & Run CoSim for OEM_ROOT:           python build.py -o all oemroot_cosim_run \n \
                      To Clean dsp image:                                    python build.py -o clean \n\n \
                 --Command to create HAP packages \n \
                   cd adsp_proc \n \
                      To create HK11 package:        python build.py -o pkg_hk11 \n \
                      To create HY11/HY31 package:   python build.py -o pkg_oem \n \
                      To create HD11 package:        python build.py -o pkg_hd11 \n \
                      To create HY22 package:        python build.py -o pkg_isv \n \
                      To create HCBSP_Test package:  python build.py -o pkg_hcbsp \n\n \
                 --For help:  python build.py -h"
parser = optparse.OptionParser(usage=usage)
parser.add_option('-b', '--buildid' , help='buildid option [Default: 0x8fffffff]', dest='buildid', action='store')
parser.add_option('-c', '--chipset' , help='chipset option [Default: msm8974]', dest='chipset', action='store')
parser.add_option('-p', '--pid' , help='protection domain option [Default: mpd]', dest='protection_domain', action='store')
parser.add_option("-o", "--others", help='other variable number of options: <ARG1> <ARG2> ... \
                   Eg: all sim clean cosim cosim_run oemroot_cosim_run ... \
                   To create packages: pkg_hk11, pkg_oem, pkg_hd11, \
                   pkg_isv, pkg_hcbsp              \
                   all: Build the dsp image.           \
                   sim: Build the sim-only dsp image.           \
                   clean: Clean the dsp image.                \
                   cosim: Build CoSim & Run CoSim will be Enabled. \
                   cosim_run: Only Run CoSim will be Enabled. \
                   oemroot_cosim_run: Only Run CoSim for OEM_ROOT will be Enabled. \
                   Any new arguments, in future, can be added here. ', \
                   dest='other_option', action="callback", callback=other_options_cb)
parser.add_option('-f', '--flags' , help='Flags to pass to the build system: <FLAG1> <FLAG2> ... \
                   Any new flags, in future, can be added here', \
                   dest='flags', action="callback", callback=other_options_cb)
parser.add_option('-k', '--kloc' , help='To enable klocwork [Default: Disabled]', dest='kloc', action='store_true')
parser.add_option('-t', '--test' , help='To automate testing of opendsp packages [Default: Disabled]', dest='test', action='store_true')


(opts, args) = parser.parse_args()
print opts
print args
(opts, args) = postprocess_command_options(opts, args)
print opts
print args

parser.print_help()
print "\n\n"

alloptions = ['buildid', 'chipset', 'protection_domain', 'other_option', 'flags', 'kloc'] 

all_flag = 0
spd_flag = 0
mpd_flag = 0
clean_flag = 0
klocwork_flag = 0
cosim_flag = 0
cosim_run_flag = 0
oemroot_cosim_run_flag = 0
sim_flag = 0
sim_check = 0
pkg_hk11_flag = 0
pkg_hk22_flag = 0
pkg_oem_flag = 0
pkg_hd11_flag = 0
pkg_isv_flag = 0
pkg_hcbsp_flag = 0
pkg_hevc_flag = 0
check_dsp_flag = 0
other_option_flag = 0
build_flags = 0

buildid_flag = 0
chipset_flag = 0
chipset_file = 0
protection_domain_flag = 0

############################################################################################
######## If 'hap/default_pack.txt' present then extract default parameters from this file ##
########       If user options specified, will override these default values              ##
############################################################################################
default_buildid = 0
default_chipset = 0
default_pid = 0

default_params_filelist = find_files('hap', 'default_pack.txt')
if default_params_filelist:
   default_params_file = open( "hap/default_pack.txt", "r" )
   lines = []
   for line in default_params_file:      
      default_chipset_match = re.search('--chipset (\w\w\w\w\w\w\w)', line)
      default_pid_match = re.search('--pid (\w\w\w)', line)
      default_buildid_match = re.search('--buildid (\w\w\w\w\w\w\w\w\w\w)', line)
      if default_chipset_match:
         default_chipset = default_chipset_match.group(1)
      if default_pid_match:
         default_pid = default_pid_match.group(1)
      if default_buildid_match:
         default_buildid = default_buildid_match.group(1)   
   default_params_file.close()   

############################################################################################      

############################################################################################
######## If 'hap/default_test.txt' present then extract default parameters from this file ##
######## File 'hap/default_test.txt' used for testing purposes on CRM                     ##
######## This is ONLY for CRM i.e., with -k option ONLY                                   ##
######## Also, enables only for opendsp packages as it needs 'hap/default_test.txt'       ##
########       If user options specified, will override these default values              ##
############################################################################################
default_testothers = 0

default_testparams_filelist = find_files('hap', 'default_test.txt')
if ((default_testparams_filelist and opts.kloc) or opts.test):
   default_testparams_file = open( "hap/default_test.txt", "r" )
   lines = []
   for line in default_testparams_file:      
      default_testothers_match = re.search('--others (.+)', line)
      if default_testothers_match:
         default_testothers = default_testothers_match.group(1)   
   default_testparams_file.close()   

############################################################################################

for m in alloptions:
    if m == 'chipset':
       if not opts.chipset:
          chipset_param = 'msm8974'                 
	  if default_chipset:
             chipset_param = default_chipset
             print "chipset option not specified, default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % chipset_param
          else:
             print 'chipset option not specified, setting default:', chipset_param  
       else:  
          chipset_param = getattr(opts, m)
          chipset_flag = 1
       chipset_param = chipset_param.lower()       
    if m == 'buildid':
       if not opts.buildid:
          buildid_param = '0x8fffffff'
	  if default_buildid:
             buildid_param = default_buildid
             print "buildid option not specified, default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % buildid_param
          else:
             print 'buildid option not specified, setting default:', buildid_param
       else:
          buildid_param = getattr(opts, m)
          buildid_flag = 1
       buildid_param = buildid_param.lower()       
    if m == 'protection_domain':  
       if not opts.protection_domain:
          protection_domain_param = 'mpd'
	  if default_pid:
             protection_domain_param = default_pid
             print "protection domain option not specified, default value taken from file: 'adsp_proc/hap/default_pack.txt': %s" % protection_domain_param
          else:          
             print 'protection domain option not specified, setting default:', protection_domain_param
       else:
          protection_domain_param = getattr(opts, m)
          protection_domain_flag = 1
       protection_domain_param = protection_domain_param.lower()       
       if protection_domain_param == 'mpd':
          mpd_flag = 1
       if protection_domain_param == 'spd':
          spd_flag = 1
    if m == 'other_option':
       print 'other options:', opts.other_option
       if opts.other_option:

          other_option_flag = 1
          if ((default_testothers and opts.kloc) or opts.test):
             #This is ONLY for CRM i.e., with -k option ONLY
             #Also, enables only for opendsp packages as it needs 'hap/default_test.txt'
             default_testarray = default_testothers.split()
             print 'other options from \'default_test.txt\': ', default_testarray
             all_flag = verify_args('\Aall\Z', default_testarray) 
             clean_flag = verify_args('\Aclean\Z', default_testarray)
             klocwork_flag = verify_args('\Aklocwork\Z', default_testarray)
             cosim_flag = verify_args('\Acosim\Z', default_testarray)
             cosim_run_flag = verify_args('\Acosim_run\Z', default_testarray)
             oemroot_cosim_run_flag = verify_args('\Aoemroot_cosim_run\Z', default_testarray)
             sim_flag = verify_args('\ASIM\Z', default_testarray)
             sim_check = verify_args('\Achecksim\Z', default_testarray)
             check_dsp_flag = verify_args('\Acheck_dsp\Z', default_testarray)
          else:
             all_flag = verify_args('\Aall\Z', opts.other_option)
             clean_flag = verify_args('\Aclean\Z', opts.other_option)
             klocwork_flag = verify_args('\Aklocwork\Z', opts.other_option)
             cosim_flag = verify_args('\Acosim\Z', opts.other_option)
             cosim_run_flag = verify_args('\Acosim_run\Z', opts.other_option)
             oemroot_cosim_run_flag = verify_args('\Aoemroot_cosim_run\Z', opts.other_option)
             sim_flag = verify_args('\ASIM\Z', opts.other_option)
             sim_check = verify_args('\Achecksim\Z', opts.other_option)
             pkg_hk11_flag = verify_args('\Apkg_hk11\Z', opts.other_option)
             pkg_hk22_flag = verify_args('\Apkg_hk22\Z', opts.other_option)
             pkg_oem_flag = verify_args('\Apkg_oem\Z', opts.other_option)
             pkg_hd11_flag = verify_args('\Apkg_hd11\Z', opts.other_option)
             pkg_isv_flag = verify_args('\Apkg_isv\Z', opts.other_option)
             pkg_hcbsp_flag = verify_args('\Apkg_hcbsp\Z', opts.other_option)
             pkg_hevc_flag = verify_args('\Apkg_hevc\Z', opts.other_option)
             check_dsp_flag = verify_args('\Acheck_dsp\Z', opts.other_option)
       else:

         if ((default_testothers and opts.kloc) or opts.test):
             #This is ONLY for CRM i.e., with -k option ONLY
             #Also, enables only for opendsp packages as it needs 'hap/default_test.txt'
             default_testarray = default_testothers.split()
             print 'other options from \'default_test.txt\': ', default_testarray
	     all_flag = verify_args('\Aall\Z', default_testarray)
	     clean_flag = verify_args('\Aclean\Z', default_testarray)
	     klocwork_flag = verify_args('\Aklocwork\Z', default_testarray)
	     cosim_flag = verify_args('\Acosim\Z', default_testarray)
	     cosim_run_flag = verify_args('\Acosim_run\Z', default_testarray)
	     oemroot_cosim_run_flag = verify_args('\Aoemroot_cosim_run\Z', default_testarray)
	     sim_flag = verify_args('\ASIM\Z', default_testarray)
	     sim_check = verify_args('\Achecksim\Z', default_testarray)
             check_dsp_flag = verify_args('\Acheck_dsp\Z', default_testarray)
         else:    
             all_flag = 1          
    
    opts_flags = ''
    if m == 'flags':
       if opts.flags:
          flags_param = ''
          build_flags = 1
          opts_flags = opts.flags
          for n in opts.flags:    
              print "flags options are:", n
              if n == 'OEM_ROOT':
                 os.environ['OEM_ROOT'] = '1'
                 os.environ['HAP_AUDIO_EXAMPLES'] ='1'
                 print 'OEM_ROOT is set for customization!!!'
              flags_param = ''.join([n, ',' , flags_param])          
          flags_param = flags_param.rstrip(',')
          print flags_param
    if m == 'kloc':
       if opts.kloc:
          klocwork_flag = 1          

print "\n\n"


# try to find BLD_ENV_BUILD_ID enviroment variable
env_bld_id = os.environ.get('BLD_ENV_BUILD_ID', None)
if env_bld_id and not opts.chipset:
   print 'Extracting CHIPSET from BLD_ENV_BUILD_ID'   
   set_chipset_target(env_bld_id, chipset_param, opts.chipset)       
   chipset_flag = 1
else:
   if chipset_param:
      #print 'Chipset Parameter:', chipset_param
      if not env_bld_id:
         print 'BLD_ENV_BUILD_ID enviroment variable is None'
      else:
         print '-c option provided from command-line over-rides chipset extraction from BLD_ENV_BUILD_ID', chipset_param
      set_chipset_target(env_bld_id, chipset_param, opts.chipset)
   
  

# try to find CRM_BUILDID enviroment variable
env_bld_ver = os.environ.get('CRM_BUILDID', None)
if env_bld_ver:
    print 'Extracting QDSP6_BUILD_VERSION from CRM_BUILDID'
    build_ver = os.environ['CRM_BUILDID']
    build_ver_match = re.search(r'([\w\.\d-]+?)(AAAA\w+)', build_ver)
    os.environ['QDSP6_BUILD_VERSION'] = build_ver_match.group(1)
    buildid_flag = 1
else:    
    if buildid_param:       
        print 'CRM_BUILDID enviroment variable is None'
        os.environ['QDSP6_BUILD_VERSION'] = chipset_param.upper() + "_" + buildid_param
        build_ver_match = re.search('(..)\w', os.environ['QDSP6_BUILD_VERSION'])
        # if build_ver_match.group(1) != '0x':
          # print 'ERROR: QDSP6_BUILD_VERSION should be a 32-bit hexa-decimal value with 0x \n       Eg: 0x8fffffff'
          # sys.exit(0)

if build_flags:
      print 'flags Parameter:', flags_param
      os.environ['BUILD_FLAGS'] = flags_param
else:
      os.environ['BUILD_FLAGS'] = ''
      flags_param = ''

    
os.environ['BUILD_FLAVOR'] = 'mpd'
os.environ['BUILD_ACT'] = ''
if mpd_flag:
   os.environ['BUILD_FLAVOR'] = 'mpd'
if spd_flag:
   os.environ['BUILD_FLAVOR'] = 'spd'
if clean_flag:
   os.environ['BUILD_ACT'] = 'clean'
if sim_flag:
   os.environ['BUILD_ACT'] = 'SIM'
if all_flag:
   os.environ['BUILD_ACT'] = ''
if sim_check:
   os.system("python -c 'from utils import check_sim;check_sim()'")   
   sys.exit(0)

tool_version = ''.join(['uname -a'])
proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)
(out, err) = proc.communicate()
if out:
   platform_match = re.search('(\w\w\w\w\w)', out)
   match = platform_match.group(1)
   print "\nOperating System::", out
else:
   match = 0


os.environ['Q6VERSION'] = q6_version

if match == 'Linux':
   print '\n\nIt is Linux environment!!!\n\n'
   if os.environ.get('HEXAGON_ROOT', None):
      q6_tools_path_linux = os.environ.get('HEXAGON_ROOT', None)
   else:
      os.environ['HEXAGON_ROOT'] = q6_tools_path_linux  
   
   os.environ['Q6_TOOLS_ROOT'] =  q6_tools_linux
   os.environ['Q6_ROOT'] = os.environ.get('Q6_TOOLS_ROOT', None)
   q6_root = os.environ['Q6_ROOT']
   q6_root_eclipse = ''.join([q6_root, '/eclipse'])
   q6_root_gnu_bin = ''.join([q6_root, '/gnu/bin'])
   q6_root_qc_lib = ''.join([q6_root, '/qc/lib/iss/v5'])
   q6_root_qc_bin = ''.join([q6_root, '/qc/bin'])
   os.environ['PYTHON_ROOT'] = python_linux
   python_root = os.environ['PYTHON_ROOT']
else:
   if os.environ.get('HEXAGON_ROOT', None):
      q6_tools_path_win = os.environ.get('HEXAGON_ROOT', None)
      q6_tools_path_win = q6_tools_path_win.replace('\\', '/')
      os.environ['HEXAGON_ROOT'] = q6_tools_path_win
   else:
      q6_tools_path_win = q6_tools_path_win.replace('\\', '/')
      os.environ['HEXAGON_ROOT'] = q6_tools_path_win
   
   # HEXAGON_TOOLS_ROOT is not used in build system. Added to support TestFramework as it uses HEXAGON_TOOLS_ROOT
   if os.environ.get('HEXAGON_TOOLS_ROOT', None):
      q6_tools_4tfwk = os.environ.get('HEXAGON_TOOLS_ROOT', None)
      q6_tools_4tfwk = q6_tools_4tfwk.replace('\\', '/')
      os.environ['HEXAGON_TOOLS_ROOT'] = q6_tools_4tfwk
   else:
      os.environ['HEXAGON_TOOLS_ROOT'] = q6_tools_path_win + "/" + q6_rtos_tools_version      
   
      
   q6_tools_win = q6_tools_path_win + "/" + q6_rtos_tools_version
   os.environ['Q6_TOOLS_ROOT'] = q6_tools_win
   q6_tools_root = os.environ['Q6_TOOLS_ROOT']
   os.environ['Q6_ROOT'] = os.environ.get('Q6_TOOLS_ROOT', None)
   q6_root = os.environ['Q6_ROOT']
   q6_root_eclipse = ''.join([q6_root, '/eclipse'])
   q6_root_gnu_bin = ''.join([q6_root, '/gnu/bin'])
   q6_root_qc_lib = ''.join([q6_root, '/qc/lib/iss/v5'])
   q6_root_qc_bin = ''.join([q6_root, '/qc/bin'])


   os.environ['PYTHON_ROOT'] = python_win
   python_root = os.environ['PYTHON_ROOT']   


print "Current OS Environment:"
print os.environ
print '\n'
local_path = os.environ['PATH']
print '\nOld PATH is:\n', os.environ['PATH']
print '\n\n'

if match != 'Linux':
   cwd_dir = os.getcwd()
   print "\n\nCurrent working directory is %s" % cwd_dir
   path_remove_cmd = ''.join([cwd_dir + "/hap/remove_cygwin.cmd"])
   if os.path.isfile(path_remove_cmd):   
      print path_remove_cmd
      proc = subprocess.Popen(path_remove_cmd, shell=True)
      (out, err) = proc.communicate()
      file_withpath = open("path_with_cygwin_removed.txt", "r")
      line_path = file_withpath.readline()
      local_path = line_path.replace('PATH=', '')
      file_withpath.close()
      os.remove('path_with_cygwin_removed.txt')

cmddbg_dir = os.getcwd()
print "\n\n build command directory is %s \n" % cmddbg_dir

if os.path.isfile("./build/build.py"):
   build_cmd = ''.join(['.\\tools\\build\\scons\\SCons\\scons --directory=./build ' , '--chipset=', os.environ.get('CHIPSET', None), ' ', \
                     '--build_version=', os.environ.get('QDSP6_BUILD_VERSION', None), ' ', '--build_flavor=', os.environ.get('BUILD_FLAVOR', None), ' ', \
                     '--build_act=', os.environ.get('BUILD_ACT'), ' ', '--build_flags=', os.environ.get('BUILD_FLAGS', None)])
   linux_build_cmd = ''.join(['./tools/build/scons/SCons/scons --directory=./build ', '--chipset=', os.environ.get('CHIPSET', None), ' ', \
                     '--build_version=', os.environ.get('QDSP6_BUILD_VERSION', None), ' ', '--build_flavor=', os.environ.get('BUILD_FLAVOR', None), ' ', \
                     '--build_act=', os.environ.get('BUILD_ACT'), ' ', '--build_flags=', os.environ.get('BUILD_FLAGS', None)])
   kw_build_cmd = ''.join([kw_path, '\\', 'kwinject -T kw_trace.txt ', build_cmd])
else:
   build_cmd = ''.join(['.\\tools\\build\\scons\\SCons\\scons ', '--chipset=', os.environ.get('CHIPSET', None), ' ', \
                     '--build_version=', os.environ.get('QDSP6_BUILD_VERSION', None), ' ', '--build_flavor=', os.environ.get('BUILD_FLAVOR', None), ' ', \
                     '--build_act=', os.environ.get('BUILD_ACT'), ' ', '--build_flags=', os.environ.get('BUILD_FLAGS', None)])
   linux_build_cmd = ''.join(['./tools/build/scons/SCons/scons ', '--chipset=', os.environ.get('CHIPSET', None), ' ', \
                     '--build_version=', os.environ.get('QDSP6_BUILD_VERSION', None), ' ', '--build_flavor=', os.environ.get('BUILD_FLAVOR', None), ' ', \
                     '--build_act=', os.environ.get('BUILD_ACT'), ' ', '--build_flags=', os.environ.get('BUILD_FLAGS', None)])
   kw_build_cmd = ''.join([kw_path, '\\', 'kwinject -T kw_trace.txt ', build_cmd])


if match != 'Linux':

   print "Setting paths now..."   
   new_path = ''.join([python_root, ';', q6_root_gnu_bin, ';', q6_root_qc_lib, ';', q6_root_qc_bin]) 

if match == 'Linux':
   if all_flag or sim_flag or clean_flag:
      new_path = ''.join([python_root, ':', q6_root_gnu_bin, ':', q6_root_qc_lib, ':', q6_root_qc_bin, ':'])
      new_engg_path = ''.join([new_path, ':', local_path, ':'])
      os.environ['PATH'] = new_engg_path
      print 'New PATH w.r.t Linux:\n', os.environ['PATH']
      summary_build(default_buildid, default_chipset, default_pid, buildid_flag, chipset_flag, protection_domain_flag, klocwork_flag, flags_param)   
      print '\n\nBuild Command on Linux:\n', linux_build_cmd
      print "\n"   
      proc = subprocess.Popen(linux_build_cmd, shell=True)
      (out, err) = proc.communicate()   
else: #The complete code below till end is for non-linux case      
   if all_flag or sim_flag or clean_flag:      
      arg_flag = 0
      if other_option_flag:
         arg_flag = verify_args('\Aklocwork\Z', opts.other_option)         
      if arg_flag or klocwork_flag:	
         os.environ['BUILDSPEC'] = 'KLOCWORK'         
         new_kw_path = ''.join([new_path, ';', local_path, ';'])
         os.environ['PATH'] = new_kw_path
         print 'New PATH w.r.t klockwork:\n', os.environ['PATH']
         summary_build(default_buildid, default_chipset, default_pid, buildid_flag, chipset_flag, protection_domain_flag, klocwork_flag, flags_param)      
                  
         print '\n\nBuild Command with Klocwork:\n', kw_build_cmd
         print "\n"         
         proc = subprocess.Popen(kw_build_cmd, shell=True)
         (out, err) = proc.communicate()
               
         temp_dir = ".."
         os.chdir(temp_dir)
         cwd_dir = os.getcwd()      
         sagenconfig_run_cmd = ''.join([sagen_path, '\\', 'sagenconfig -r ', os.getcwd(), ' -t trace -f ./adsp_proc/kw_trace.txt'])      
         proc = subprocess.Popen(sagenconfig_run_cmd, shell=True)
         (out, err) = proc.communicate()
         adsp_dir = "adsp_proc"
         os.chdir(adsp_dir)
         cwd_dir = os.getcwd()      
      else:         
         new_engg_path = ''.join([new_path, ';', local_path])
         os.environ['PATH'] = new_engg_path
         print 'New PATH w.r.t engineering build:\n', os.environ['PATH']         
         summary_build(default_buildid, default_chipset, default_pid, buildid_flag, chipset_flag, protection_domain_flag, klocwork_flag, flags_param)         
               
         print '\n\nBuild Command for Engineering build:\n', build_cmd
         print "\n"      
         proc = subprocess.Popen(build_cmd, shell=True)
         (out, err) = proc.communicate()

# Bit-exactness Verification
   if check_dsp_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      compare_file_old = 'mpd_phdr.pbn' 
      compare_file_old2= 'M9x25AAAAAAAAAQ1234_phdr.pbn'
      compare_file_new = 'mpd_pre_dummy_seg_phdr.pbn'
      compare_file_new2 = 'adsp_phdr.pbn'
      where_compare_path = ''.join([cwd_dir + "/build/bsp/multi_pd_img/build/"])
      where_compare_path2 = ''.join([cwd_dir + "/build/bsp/devcfg_img/build/"])
      if os.environ['CHIPSET'] == "mdm9x25" and os.environ['BUILD_FLAVOR'] == 'spd':
         if os.path.exists(''.join([where_compare_path2 + compare_file_old2])):
            checkdsp_cmd = ''.join(["perl " + cwd_dir + "/../../adsp_proc/hap/scripts/HAP-DSP_verify.pl " + "../../adsp_proc/build/bsp/devcfg_img/build/" + compare_file_old2 + " ./build/bsp/devcfg_img/build/" + compare_file_old2, " > ./opendsp_bitmatch.log 2>&1"])
      elif os.path.exists(''.join([where_compare_path + compare_file_old])):
         checkdsp_cmd = ''.join(["perl " + cwd_dir + "/../../adsp_proc/hap/scripts/HAP-DSP_verify.pl " + "../../adsp_proc/build/bsp/multi_pd_img/build/" + compare_file_old + " ./build/bsp/multi_pd_img/build/" + compare_file_old, " > ./opendsp_bitmatch.log 2>&1"])
      elif os.path.exists(''.join([where_compare_path + compare_file_new])):
         checkdsp_cmd = ''.join(["perl " + cwd_dir + "/../../adsp_proc/hap/scripts/HAP-DSP_verify.pl " + "../../adsp_proc/build/bsp/multi_pd_img/build/" + compare_file_new + " ./build/bsp/multi_pd_img/build/" + compare_file_new, " > ./opendsp_bitmatch.log 2>&1"])
      elif os.path.exists(''.join([where_compare_path + compare_file_new2])):
         checkdsp_cmd = ''.join(["perl " + cwd_dir + "/../../adsp_proc/hap/scripts/HAP-DSP_verify.pl " + "../../adsp_proc/build/bsp/multi_pd_img/build/" + compare_file_new2 + " ./build/bsp/multi_pd_img/build/" + compare_file_new2, " > ./opendsp_bitmatch.log 2>&1"])
      else:
         checkdsp_cmd = ''.join(["perl " + cwd_dir + "/../../adsp_proc/hap/scripts/HAP-DSP_verify.pl " + "../../adsp_proc/dsp.elf" + " ./dsp.elf", " > ./opendsp_bitmatch.log 2>&1"])  
      print checkdsp_cmd
      proc = subprocess.Popen(checkdsp_cmd, shell=True)
      (out, err) = proc.communicate()
      where_success_file = ''.join([cwd_dir + "/success"])
      where_bitmatchfail_file = ''.join([cwd_dir + "/bitmatch_fail.txt"])      
      if os.path.exists(where_success_file):
         if os.path.exists(where_bitmatchfail_file):
            os.remove(where_success_file)
            print "       Bit-match Failed, 'success' file is removed to reflect the CRM pack status!!! \n"
      print "\n\n"

   
# Test framework build and run
   test_framework(opts, other_option_flag, new_path)

# HAP packages creation
   if pkg_hk11_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join([cwd_dir + "/hap/scripts/opendsp_hk11.cmd"])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()

   if pkg_hk22_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join([cwd_dir + "/hap/scripts/opendsp_hk22.cmd"])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()
       
   if pkg_oem_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join([cwd_dir + "/hap/scripts/opendsp.cmd"])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()

   if pkg_hd11_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join([cwd_dir + "/hap/scripts/opendsp_hd11.cmd"])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()
      
   if pkg_isv_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join([cwd_dir + "/hap/scripts/opendsp_isv.cmd"])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()      
      
   if pkg_hcbsp_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join([cwd_dir + "/hap/scripts/hcbsp.cmd"])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()
      
   if pkg_hevc_flag:
      cwd_dir = os.getcwd()
      print "\n\nCurrent working directory is %s" % cwd_dir
      pkg_cmd = ''.join([cwd_dir + "/hap/scripts/hevc.cmd"])
      print pkg_cmd
      proc = subprocess.Popen(pkg_cmd, shell=True)
      (out, err) = proc.communicate()      
      
#=================================================================================            
#=================================================================================
#                  Main build.py ends here
#=================================================================================
#=================================================================================            
            
            


