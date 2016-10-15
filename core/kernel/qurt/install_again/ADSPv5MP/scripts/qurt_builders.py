from SCons.Script import *
import re
import subprocess
import signal
from threading import Timer

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('qurt_builders')

def generate(env):
   # load builder into enviroment
   cust_config_generate(env)
   obj_dump_generate(env)
   sim_generate(env)
   test_sim_generate(env)
   reloc_generate(env)
   qurt_image_generate(env)
   build_param_generate(env)
   config_vars_generate(env)
   const_autogen_generate(env)
   ppend_cust_config_generate(env)
   traptab_generate(env)
   hex_cfg_generate(env)
   return None

#------------------------------------------------------------------------------
# Customer Configuration definition
#------------------------------------------------------------------------------
def cust_config_generate(env):
   CustConfigBuilder = Builder(action = 'python ${Q6_RTOS_ROOT}/scripts/qurt_config.py update -T ${Q6_TOOLS_ROOT} -o $TARGET $SOURCE', 
                       emitter = cust_config_emitter, 
                       suffix = '.o', 
                       src_suffix = '.xml')
   env['BUILDERS']['CustConfigObj'] = CustConfigBuilder

def cust_config_emitter(target, source, env):
   # Add dependencies on scripts
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/Input/cust_config.c")
   env.Depends(target, "${Q6_RTOS_ROOT}/scripts/qurt_config.py")
   # clean side effects
   env.Clean(target, "build/qurt_config.c")
   return (target, source)

#------------------------------------------------------------------------------
# Object Dump
#------------------------------------------------------------------------------
def obj_dump_generate(env):
   ObjDumpBuilder = Builder(action = '${OBJDUMP} ${OBJDUMPOPT} $SOURCE > $TARGET', 
                       suffix = '.dump', 
                       src_suffix = '.pbn')
   env['BUILDERS']['ObjectDump'] = ObjDumpBuilder

#------------------------------------------------------------------------------
# Simulator
#------------------------------------------------------------------------------
def sim_generate(env):
   SimBuilder = Builder(action = "${SIM} -S $TARGET --simulated_returnval $SOURCE ${SIM_RTOS} ${SYMFILE} ${SIM_COSIM} ${SIMOUT} ${SIMERR}", 
                       suffix = '.txt')
   env['BUILDERS']['Simulator'] = SimBuilder

#------------------------------------------------------------------------------
# Simulator for testing, timeout after
#------------------------------------------------------------------------------
def test_sim_generate(env):
    TestSimBuilder = Builder(action = test_sim_builder)
    env['BUILDERS']['TestSimulator'] = TestSimBuilder

def kill_proc(proc, timeout):
    timeout["value"] = True
    if ((sys.version_info[0] == 2) and (sys.version_info[1] >= 6)) or (sys.version_info[0] > 2):
        proc.kill()
    else:
        os.kill(proc.pid, signal.SIGKILL)

def test_sim_builder(target, source, env):
    cmd = "%s -S %s --simulated_returnval %s --rtos %s --symfile %s --cosim_file %s" % (env.subst("${SIM}"), 
          str(target[1]), str(source[0]), str(source[1]), str(source[2]), str(source[3]))
    print cmd;
    command = cmd.split()

    simproc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    timeout = {"value": False}
    timer = Timer(float(env.subst("${TEST_TIMEOUT}")), kill_proc, [simproc, timeout])
    timer.start()
    stdout, stderr = simproc.communicate()
    timer.cancel()

    outfile = open(str(target[2]), 'w+')
    outfile.write(stdout)
    outfile.write(stderr)
    outfile.close

    if str(timeout["value"]) == "True":
        return "TIMEOUT"
    else:
        file = open(str(target[0]), 'w+')
        if simproc.returncode == 0:
            file.write('PASS')
        else:
            file.write('FAIL')
        file.close
        return simproc.returncode

#------------------------------------------------------------------------------
# Simulator for testing, timeout after
#------------------------------------------------------------------------------
def hex_cfg_generate(env):
    HexCfgBuilder = Builder(action = hex_cfg_builder)
    env['BUILDERS']['HexCfg'] = HexCfgBuilder

def hex_cfg_builder(target, source, env):
    if os.name == 'posix':
        timerlib = env.subst('${Q6_TOOLS_ROOT}/qc/lib/iss/qtimer.so')
        l2viclib = env.subst('${Q6_TOOLS_ROOT}/qc/lib/iss/l2vic.so')
    elif os.name == 'nt':
        timerlib = env.subst('${Q6_TOOLS_ROOT}/qc/lib/iss/qtimer.dll')
        l2viclib = env.subst('${Q6_TOOLS_ROOT}/qc/lib/iss/l2vic.dll')

    srcfile = open(str(source[0]),'r')
    tempstr = srcfile.read()
    srcfile.close()

    tgtfile = open(str(target[0]),'w+')
    tempstr = tempstr.replace('qtimer.so', timerlib)
    tempstr = tempstr.replace('l2vic.so', l2viclib)
    tgtfile.write(tempstr)
    tgtfile.close()

#------------------------------------------------------------------------------
# Relocation
#------------------------------------------------------------------------------
def reloc_generate(env):
   RelocBuilder = Builder(action = "${OBJCOPY} --change-start +${RELOC_ADDR} " +
                                 "--change-section-lma .start+${RELOC_ADDR} " +
                                 "--change-section-lma .init+${RELOC_ADDR} " +
                                 "--change-section-lma .text+${RELOC_ADDR} " +
                                 "--change-section-lma .fini+${RELOC_ADDR} " +
                                 "--change-section-lma .rodata+${RELOC_ADDR} " +
                                 "--change-section-lma .data+${RELOC_ADDR} " +
                                 "--change-section-lma .eh_frame+${RELOC_ADDR} " +
                                 "--change-section-lma .ctors+${RELOC_ADDR} " +
                                 "--change-section-lma .dtors+${RELOC_ADDR} " +
                                 "--change-section-lma .bss+${RELOC_ADDR} " +
                                 "--change-section-lma .sdata+${RELOC_ADDR} " +
                                 "--change-section-lma .sbss+${RELOC_ADDR} " +
                                 "$SOURCE $TARGET")
   env['BUILDERS']['Relocate'] = RelocBuilder

#------------------------------------------------------------------------------
# Qurt Image
#------------------------------------------------------------------------------
#guest os image needs to be first in source list
def qurt_image_generate(env):
   QurtImageBuilder = Builder(action = "python ${Q6_RTOS_ROOT}/scripts/qurt-image-build.py $SOURCES -o $TARGET -p ${PADDR} -t ${Q6_TOOLS_ROOT}")
   env['BUILDERS']['QurtImage'] = QurtImageBuilder

#------------------------------------------------------------------------------
# Builder: Deriving build_params.txt
#------------------------------------------------------------------------------
def build_param_generate(env):
   build_param_bld = Builder(action = gen_build_param)
   env['BUILDERS']['build_param_builder'] = build_param_bld

def gen_build_param(target, source, env):
   target = str(target[0])   
   fileHandle = open (target, 'w')
   fileHandle.write("Q6VERSION="         +env.subst('$Q6VERSION')+"\n")
   fileHandle.write("BUILD_CONFIG_FLAGS="+env.subst('$BUILD_CONFIG_FLAGS')+"\n")
   fileHandle.write("Q6_TOOLS_ROOT="     +os.path.realpath(env.subst('$Q6_TOOLS_ROOT')))
   fileHandle.close()

#------------------------------------------------------------------------------
# Builder: Prepending cust_config.c
#------------------------------------------------------------------------------
def ppend_cust_config_generate(env):
   cust_config_bld = Builder(action = ppend_cust_config)
   env['BUILDERS']['cust_config_builder'] = cust_config_bld

def ppend_cust_config(target, source, env):
   target = str(target[0])
   tmplt  = str(source[0])
   mcr    = str(source[1])
   
   fileTarget = open (target, 'w')
   fileTmplt  = open (tmplt, 'r')
   fileMcr    = open (mcr, 'r')
   
   for line in fileMcr:
      if line.find('THREAD_CONTEXT_TOTALSIZE') != -1:
         fileTarget.write(line)
      elif line.find('QURT_CONFIG_MAX_HTHREAD') != -1:
         fileTarget.write(line)
   
   for line in fileTmplt:
      fileTarget.write(line)

   fileTarget.close()

#------------------------------------------------------------------------------
# Builder: qurt_config_vars.h
#------------------------------------------------------------------------------
def config_vars_generate(env):
   env['BUILDERS']['ConfigVarsGen'] = Builder(action = config_vars_builder)

def config_vars_builder(target, source, env):
    arglist = env.subst("$CC -E -P -DGEN_CONFIG_HEADER").split(" ")
    for w in source:
        if re.match('.*\\.h', str(w)):
            arglist += ["-include", str(w)]
    for w in source:
        if re.match('.*\\.c', str(w)):
            arglist += [str(w)]
    outinc_file = open(str(target[0]), 'w')
    outinc_file.write('/* This file is automatically generated. */\n\n')
    outinc_file.write('#ifndef GEN_CONFIG_HEADER_USED\n')
    outinc_file.write('#define GEN_CONFIG_HEADER_USED\n')
    outinc_file.flush()
    print arglist
    cpp = subprocess.Popen(arglist, stdout=outinc_file)
    val = cpp.wait()
    if val == 0:
        outinc_file.write('#endif /* GEN_CONFIG_HEADER_USED */\n')
    outinc_file.close()
    if val != 0:
        os.remove(str(target[0]))
    return val


#------------------------------------------------------------------------------
# Builder: qurt_config_vars.h
#------------------------------------------------------------------------------
def const_autogen_generate (env):
   const_autogen_bld = Builder(action = '${SIM} --quiet $SOURCE > $TARGET')
   env['BUILDERS']['const_autogen_builder'] = const_autogen_bld

#------------------------------------------------------------------------------
# xml config with paddr option 
#------------------------------------------------------------------------------
def qurt_image_generate(env):
   QurtImageBuilder = Builder(action = "python ${Q6_RTOS_ROOT}/scripts/qurt-image-build.py $SOURCES -o $TARGET -p ${PADDR} -t ${Q6_TOOLS_ROOT}")
   env['BUILDERS']['QurtImage'] = QurtImageBuilder

#------------------------------------------------------------------------------
# traptab builder
#------------------------------------------------------------------------------
def traptab_generate(env):
   env['BUILDERS']['TrapTabGen'] = Builder(generator = traptab_generator)
def traptab_generator(source, target, env, for_signature):
   return ' '.join(['python ', str(source[0]),'-outtab', str(target[0]),'-outinc', str(target[1]),env['CC'],env.subst('$CCFLAGS'),' '.join("-I%s" % w for w in env['CPPPATH']),'-E -x c', str(source[1])])

