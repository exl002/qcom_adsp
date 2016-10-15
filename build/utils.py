#===============================================================================
#                    Copyright 2009 QUALCOMM Technologies Incorporated.
#                           All Rights Reserved.
#                         QUALCOMM Proprietary/GTDR
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/2.2/utils.py#15 $
#  $DateTime: 2013/09/11 17:01:25 $
#  $Change: 4415963 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#   when       who            what, where, why
# --------   ---        ---------------------------------------------------------
#   06/12/13   corinc    Script to call the QC-SCons build System for Badger    

#===============================================================================
import os
import fnmatch
import sys
import datetime

import os
import subprocess
def create_timestamp(env):
   print "==================Creating timestamp and q6_build_version header files=================="
   print env['BUILD_VERSION']
   str_build_version = "#define QDSP6_BUILD_VERSION " + env.subst('$BUILD_VERSION') 
   file = open('./q6_build_version.h', 'w')
   file.write(str_build_version)
   file.close()
   
   timestamp_dir = os.getcwd()
   print "\n\n Time Stamp working directory is %s" % timestamp_dir

   current_time = datetime.datetime.now()
   str_timestamp_qcom = "const char qcom_timestampstring[] = \"QCOM time:" + current_time.strftime("Q6_BUILD_TS_%a_%b_%d_%H:%M:%S_PST_%Y_") + env.subst('$BUILD_VERSION') + "\";"
   if os.path.exists('./qcom_timestamp.h'):
      print "qcom_timestamp header file already generated from CRM build, check the build version from existing header file"
   else:
      file = open('./qcom_timestamp.h', 'w')
      file.write(str_timestamp_qcom)
      file.close()
   
   str_timestamp_engg = "const char engg_timestampstring[] = \"ENGG time:" + current_time.strftime("Q6_BUILD_TS_%a_%b_%d_%H:%M:%S_PST_%Y_") + env.subst('$BUILD_VERSION') + "\";"   
   file = open('./engg_timestamp.h', 'w')
   file.write(str_timestamp_engg)
   file.close()
   
#   current_time = date | sed 's/ /_/g'
#   file.write(current_time)

def init_chipset(env):
  
  #print env.Dump()
  print "current PLATFORM is " + env['PLATFORM']
  chipset=env.subst('$CHIPSET')
  print chipset
  build_flavor=env.subst('$BUILD_FLAVOR')
  print build_flavor
  build_act=env.subst('$BUILD_ACT')
  print build_act
  build_flags=env.subst('$BUILD_FLAGS')
  print build_flags


  if env['PLATFORM'] == 'posix':
     env.Replace(SCONS_CMD_SCRIPT = "./build.sh ${CHIPSET}")
  else:
     if chipset=='msm8974':
        env.Replace(SCONS_CMD_SCRIPT = "build.cmd")
     elif chipset=='mdm9x25':
        env.Replace(SCONS_CMD_SCRIPT = "build_9x25.cmd")
     elif chipset=='msm8x26':
        env.Replace(SCONS_CMD_SCRIPT = "build_8x26.cmd")
     elif chipset=='apq8084':
        env.Replace(SCONS_CMD_SCRIPT = "build_8084.cmd")
     elif chipset=='msm8962':
        env.Replace(SCONS_CMD_SCRIPT = "build_8962.cmd")
     elif chipset=='msm8x10':
        env.Replace(SCONS_CMD_SCRIPT = "build_8x10.cmd")
     else:
        print "Unknown CHIPSET!!!"
        sys.exit(0)   

  if build_flavor=='spd':
     if chipset=='msm8x26':
        env.Replace(IMAGE_ALIAS = "msm8974_SPD")
     else:   
        env.Replace(IMAGE_ALIAS = "${CHIPSET}_SPD")
  elif build_flavor=='mpd':
     if chipset=='msm8x26':
        env.Replace(IMAGE_ALIAS = "msm8974_MPD")
     else:
        env.Replace(IMAGE_ALIAS = "${CHIPSET}_MPD")   
  else:
     print "Unknown BUILD_FLAVOR!!!"
     sys.exit(0)

  env.Replace(SCONS_CMD = "${SCONS_CMD_SCRIPT} ${IMAGE_ALIAS} BUILD_ID=AAAAAAAA BUILD_VER=1234 -j 8")
  env.Replace(SCONS_FLAVOR_CMD = "cd build/ms && ${SCONS_CMD} --verbose=2")

  if build_act=='clean':
     env.Replace(SCONS_FLAVOR_ACT_CMD = "${SCONS_FLAVOR_CMD} -c")
     print "clean engg_timestamp"
     engg_timestamp_file = './engg_timestamp.h'
     os.remove(engg_timestamp_file)
     print "clean split images"
     splitelf_dir = './obj/qdsp6v5_ReleaseG/LA/system/etc/firmware/'
     if os.path.exists(splitelf_dir):
        splitelf_files = os.listdir(splitelf_dir)
        print splitelf_files
        os.chdir(splitelf_dir)     
        for file in splitelf_files:
           os.remove(file)
        os.chdir('../../../../../..')
     splitelf_dir = './obj/qdsp6v5_ReleaseG/non_reloc/LA/system/etc/firmware/'
     if os.path.exists(splitelf_dir):
        splitelf_files = os.listdir(splitelf_dir)
        print splitelf_files
        os.chdir(splitelf_dir)     
        for file in splitelf_files:
           os.remove(file)
        os.chdir('../../../../../../..')
  elif build_act=='SIM':
     print "for build_act is SIM, pass SIM=1 to SCons build cmd"
     env.Replace(SCONS_FLAVOR_ACT_CMD = "${SCONS_FLAVOR_CMD} SIM=1")

     print "generating osam cfg file"
     #str_osam_path = "../core/kernel/qurt/install_again/ADSPv5MP/debugger/lnx64/qurt_model.so"
     #import pdb; pdb.set_trace() 
     if build_flags != 'CHECKSIMBOOT':
        if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"] :
           #This is a hack as pw works on windows and testing is done on linux
           str_osam_path = "../core/kernel/qurt/install_again/ADSPv5MP/debugger/lnx64/qurt_model.so"       
        else:
           print "sds"
           str_osam_path = "../core/kernel/qurt/install_again/ADSPv5MP/debugger/lnx64/qurt_model.so"
     else:
        if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"] :
           str_osam_path = "..\core\kernel\qurt\install_again\ADSPv5MP\debugger\cygwin\qurt_model.dll"       
        else:
           str_osam_path = "../core/kernel/qurt/install_again/ADSPv5MP/debugger/lnx64/qurt_model.so"
     print str_osam_path      
     #import pdb; pdb.set_trace()       
     dir_obj_ReleaseG = './obj/qdsp6v5_ReleaseG/'
     if not os.path.exists(dir_obj_ReleaseG):
        os.makedirs(dir_obj_ReleaseG)
     file = open(dir_obj_ReleaseG + 'osam.cfg', 'w')
     file.write(str_osam_path)
     file.close()
  elif build_act=='klocwork':
     env.Replace(SCONS_FLAVOR_ACT_CMD = "${SCONS_FLAVOR_CMD}")
  else:
     env.Replace(SCONS_FLAVOR_ACT_CMD = "${SCONS_FLAVOR_CMD}")
  
  if build_flags:
     #env.Replace(SCONS_FLAVOR_ACT_CMD_FLAGS = "${SCONS_FLAVOR_ACT_CMD} USES_FLAGS="+build_flags)
     env.Replace(SCONS_FLAVOR_ACT_CMD_FLAGS = "${SCONS_FLAVOR_ACT_CMD}")
     scons_flavor_act_cmd=env.subst('$SCONS_FLAVOR_ACT_CMD_FLAGS')
     env.Replace(SCONS_FLAVOR_ACT_CMD = scons_flavor_act_cmd)
 

def execute_buildcmd(env):
   print "==================delete the elfs from previous build if exist=================="
   dspelf_file = './dsp.elf'
   if os.path.exists(dspelf_file):
      os.remove(dspelf_file)
   adsp_link_dir = './build/bsp/adsp_link/build/AAAAAAAA/'
   if os.path.exists(adsp_link_dir):
      adsp_link_files = os.listdir(adsp_link_dir)
      print adsp_link_files
      os.chdir(adsp_link_dir)
      for file in adsp_link_files:
         os.remove(file)
      os.chdir('../../../../..')

   print "==================execute the build startup command=================="
   print "print the SCONS_FLAVOR_ACT_CMD in env"
   print env['SCONS_FLAVOR_ACT_CMD']
   scons_flavor_act_cmd=env.subst('$SCONS_FLAVOR_ACT_CMD')
   print "print the SCONS_FLAVOR_ACT_CMD command when use it"
   print scons_flavor_act_cmd
   cwd_dir = os.getcwd()
   print "\n\nexecute the build working directory is %s" % cwd_dir
   os.system(scons_flavor_act_cmd)
   cwd_dir = os.getcwd()
   print "\n\nCurrent working directory is %s" % cwd_dir

def split_proc(env):
   print "================== Running splitter ==================="
   if os.path.exists('./dsp.elf'):
     split_directory = os.path.dirname('./obj/qdsp6v5_ReleaseG/LA/system/etc/firmware/')
     split_directory_non_reloc = os.path.dirname('./obj/qdsp6v5_ReleaseG/non_reloc/LA/system/etc/firmware/')
     if not os.path.exists(split_directory):
         os.makedirs(split_directory)
     if not os.path.exists(split_directory_non_reloc):
         os.makedirs(split_directory_non_reloc)
     adsp_proc_root = os.getcwd() 
     SPLIT_CMD = "cd "+adsp_proc_root+"/obj/qdsp6v5_ReleaseG/LA/system/etc/firmware && python "+adsp_proc_root+"/qdsp6/scripts/pil-splitter.py "+adsp_proc_root+"/dsp.elf adsp"
     os.system(SPLIT_CMD)
     SPLIT_CMD = "cd "+adsp_proc_root+"/obj/qdsp6v5_ReleaseG/non_reloc/LA/system/etc/firmware && python "+adsp_proc_root+"/qdsp6/scripts/pil-splitter.py "+adsp_proc_root+"/dsp_non_reloc.elf adsp"
     os.system(SPLIT_CMD)
   else:
     print "dsp.elf is not generated!!!"
     sys.exit(0)
	 
def elf_extractor_proc(env):
   print "================== Running hexagon elf extractor script ==================="
   env.Replace(ELF_EXTRACTOR_PROC_CMD = "python ./qdsp6/scripts/hexagon_elf_extractor.py --target=adsp --elf=./build/bsp/adsp_link/build/AAAAAAAA/ADSP_PROC_IMG_AAAAAAAAQ.elf")
   elf_extractor_proc_cmd=env.subst('$ELF_EXTRACTOR_PROC_CMD')
   os.system(elf_extractor_proc_cmd)

def memory_proc(env):
   tool_version = ''.join(['perl -v'])
   proc = subprocess.Popen(tool_version, stdout=subprocess.PIPE, shell=True)
   (out, err) = proc.communicate()
      
   if out:
      print "================== Running memory profile script ==================="
      build_flavor=env.subst('$BUILD_FLAVOR')
      if build_flavor=='spd':
          env.Replace(MEMORY_PROC_CMD2 = "perl ./qdsp6/scripts/perl/show_memory_qurt.pl ./build/bsp/adsp_link/build/AAAAAAAA/ADSP_PROC_IMG_AAAAAAAAQ.elf ${CHIPSET} ${BUILD_FLAVOR}")   
          memory_proc_cmd2=env.subst('$MEMORY_PROC_CMD2')
          show_memory_qurt_flag = os.system(memory_proc_cmd2)     
          if show_memory_qurt_flag:
             print '\nWARNING: Perl not installed to execute Post Build Step (Memory Profiling Script) \'show_memory_qurt.pl\'\n'
      elif build_flavor=='mpd':
          env.Replace(MEMORY_PROC_CMD2 = "perl ./qdsp6/scripts/perl/show_memory_qurt.pl ./build/bsp/multi_pd_img/build/AAAAAAAA/bootimage.pbn ${CHIPSET} ${BUILD_FLAVOR}")       
          memory_proc_cmd2=env.subst('$MEMORY_PROC_CMD2')
          os.system(memory_proc_cmd2)

      env.Replace(MEMORY_PROC_CMD1 = "perl ./qdsp6/scripts/perl/Image_Break_Down.pl ${CHIPSET} ./build/bsp/adsp_link/build/AAAAAAAA/ADSP_PROC_IMG_AAAAAAAAQ.elf.map CommonPD")   
      memory_proc_cmd1=env.subst('$MEMORY_PROC_CMD1')
      os.system(memory_proc_cmd1)
      if os.path.exists('./build/bsp/sensor_img/build/AAAAAAAA/SENSOR_IMG_AAAAAAAAQ.elf.map'):
        env.Replace(MEMORY_PROC_CMD1 = "perl ./qdsp6/scripts/perl/Image_Break_Down.pl ${CHIPSET} ./build/bsp/sensor_img/build/AAAAAAAA/SENSOR_IMG_AAAAAAAAQ.elf.map SensorsPD")   
        memory_proc_cmd2=env.subst('$MEMORY_PROC_CMD1')
        os.system(memory_proc_cmd2)          
   else:
      print '\n\nWARNING: Perl not installed to execute Post Build Step (Memory Profiling Scripts)!!!\n\n'

#Initialize environmentn 
def Init(env):

   os.chdir('../')
   env_init_dir = os.getcwd()
   print "\n\n Env init working directory is %s" % env_init_dir

   #InitChipset method
   env.AddMethod(init_chipset,"InitChipset")

   #create timestamp
   env.AddMethod(create_timestamp, "CreateTimestamp")

   #execute the build startup command
   env.AddMethod(execute_buildcmd, "ExecuteBuildCmd")

   #execute the split_proc command
   env.AddMethod(split_proc, "SplitProc")

   #execute the memory_proc command
   env.AddMethod(memory_proc, "MemoryProc")

   #execute the hexagon_elf_extractor command
   env.AddMethod(elf_extractor_proc, "ElfExtractorProc")
    
   env.AddMethod(check_sim_boot, "CheckSimBoot")
def check_sim():
   print "Starting sim command"
   sim_cmd="cd avs && qdsp6-sim --simulated_returnval --mv5c_256 ../dsp.elf --simulated_returnval --rtos ../obj/qdsp6v5_ReleaseG/osam.cfg --symfile ../build/ms/M8974AAAAAAAAQ1234_reloc.elf --symfile ../build/ms/M8974AAAAAAAAQ1234_SENSOR_reloc.elf --cosim_file ./q6ss.cfg"
   os.system(sim_cmd)
   sys.exit(0)


def check_sim_boot(env):
   path =".."
   name="check_sim_boot.txt"
   full_path = os.path.join(path, name)
   print "Starting sim command"
   sim_cmd='cd avs && qdsp6-sim --simulated_returnval --mv5c_256 ../dsp.elf --simulated_returnval --rtos ../obj/qdsp6v5_ReleaseG/osam.cfg --symfile ../build/ms/M8974AAAAAAAAQ1234_reloc.elf --symfile ../build/ms/M8974AAAAAAAAQ1234_SENSOR_reloc.elf --cosim_file ./q6ss.cfg  > %s 2>&1' % full_path
   os.system(sim_cmd)
   if os.path.isfile('check_sim_boot.txt'):
       f=open('check_sim_boot.txt', 'r')
       check_msgs=['CreateEliteStaticService Begin','CreateEliteStaticService End','StartEliteStaticService start','StartEliteStaticService end','Ratio to Real Time']
       count=0
       for msg in check_msgs:
          f.seek(0, 0)
          count =0
          for line in f:
             if msg in line:
                count=count+1
          if count == 0:
             print msg+"not created"
             print "Failed in testing the sim image, check check_sim_boot.txt for more info"
             sys.exit(-1)
       print "======================= Simulation Tests Passed  =========================="
   else:
       print "======== check_sim_boot.txt is not found ================"
       sys.exit(-1)
  
