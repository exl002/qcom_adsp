#===============================================================================
#
# ARM tool rules
#
# GENERAL DESCRIPTION
#    ARM 5.0 and greater Tools definitions
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/scons/scripts/arm.py#10 $
#  $DateTime: 2013/10/02 14:00:14 $
#  $Change: 4531440 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import os
import string
import SCons.Action
from SCons.Script import *

#------------------------------------------------------------------------------
# RDirs
# Makes sure the inc paths are as provided, don't let SCons mess with them.
#------------------------------------------------------------------------------
def RDirs (path_list):
   #path_list = [str(path).replace('\\', '/') for path in path_list]
   return path_list

#------------------------------------------------------------------------------
# Chain emitter for C files
# Adds explicit dep
#------------------------------------------------------------------------------
def C_static_obj_emitter(target, source, env):
   #import pdb; pdb.set_trace()
   
   # call default static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)
   
   preprocess = env.GetOption('preprocess')
   mixasm = env.GetOption('mixasm')
   hdeptree = env.GetOption('hdeptree')
   
   if preprocess or hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
      env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))
      
   if mixasm:
      env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))
      
   if hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".dep"]))
      
      
   if 'EXPLICIT_DEPS' in env:
      env.Depends(tgt, env['EXPLICIT_DEPS'])
   
   return (tgt, src)

#------------------------------------------------------------------------------
# Chain emitter for asm files
# Adds explicit dep
#------------------------------------------------------------------------------
def S_static_obj_emitter(target, source, env):
   #import pdb; pdb.set_trace()
   
   # call default static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)
   
   preprocess = env.GetOption('preprocess')
   hdeptree = env.GetOption('hdeptree')
   mixasm = env.GetOption('mixasm')

   # add cleaners for assembling side-effects, that SCons can't auto detect
   env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
   env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))
   env.Clean(tgt, "".join([str(tgt[0]), ".lst"]))
   
   if hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".dep"]))

   if mixasm:
      env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))
   
   if 'EXPLICIT_DEPS' in env:
      env.Depends(tgt, env['EXPLICIT_DEPS'])
   
   return (tgt, src)
   
#-------------------------------------------------------------------------------
# Add Program (Link into exe, elf, etc).
#-------------------------------------------------------------------------------
def add_program (env, target=None, source=None, **kargs):

   #----------------------------------------------------------------------------
   # Init vars and environment needed to use Program builder
   image_env = env.get('IMAGE_ENV')   
   
   # variables for Program builder
   if image_env is not None:
      if target is None:
         target = image_env.get('TARGET_NAME')
         
      if source is None:
         objs = image_env.get('IMAGE_OBJS')
      else:
         objs = source
         
      if 'LIBS' not in kargs:
         libs = image_env.get('IMAGE_LIBS')
         #kargs['LIBS'] = image_env.get('IMAGE_LIBS')
      else:
         libs = kargs['LIBS']
         
      del kargs['LIBS']

   # list file extraction
   list_files = []
   if 'LISTFILES' in kargs:
      list_files = kargs.get('LISTFILES')
      if list_files is None:
         pass
      elif type(list_files) is not list:
         list_files = [list_files]
   elif image_env is not None:
      list_files = image_env.get('IMAGE_LISTFILES')
      
   #----------------------------------------------------------------------------
   # Ready for Program builder...
   target_elf = env.Program(target, source=[], **kargs)
   
   # get target base name
   target_base, target_ext = os.path.splitext(target_elf[0].get_abspath())
   build_dir = env.Dir('#').get_abspath()
   
   #import pdb;pdb.set_trace()
   
   # create list of files used during link.
   if len(objs) > 0:
      objs_lf_fname = env.subst("%s_objs.lf" % target_base)
      objs_linker_file = env.ListFileBuilder(objs_lf_fname, objs, 
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, objs_linker_file)      
      env.Append(LISTFILES = [objs_linker_file[0].get_abspath()])

   if len(libs) > 0:
      libs_lf_fname = env.subst("%s_libs.lf" % target_base)
      libs_linker_file = env.ListFileBuilder(libs_lf_fname, libs, 
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, libs_linker_file)
      env.Append(LISTFILES = [libs_linker_file[0].get_abspath()])
      
   if 'USES_ANALYTIC_DATA' in env:
      srcs_lf_fname = env.subst("%s_src.lf" % target_base)
      sources_list_file = env.SourceListBuilder(srcs_lf_fname, [objs, libs], 
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, sources_list_file)
      
   # add dependency on external build system files
   exbldsys_files = None
   if len(list_files) > 0:
      exbldsys_list_fname = env.subst("%s_exbldsys.lf" % target_base)
      exbldsys_files = env.ListFileExtractor(exbldsys_list_fname, list_files)
      env.Depends(target_elf, exbldsys_files)
      env.Append(LISTFILES = [exbldsys_files[0].path])
   
   # side effects of creating a program...
   #import pdb;pdb.set_trace()
   map_file = env.subst("%s.map" % target_base)
   
   env.SideEffect(map_file, target_elf)
   env.Clean(target_elf, map_file)
   
   sym_file = env.subst("%s.sym" % target_base)
   env.SideEffect(sym_file, target_elf)
   env.Clean(target_elf, sym_file)
   
   callg_file = env.subst("%s.htm"  % target_base)
   env.SideEffect(callg_file, target_elf)
   env.Clean(target_elf, callg_file)

   return target_elf

#------------------------------------------------------------------------------
# Hooks for SCons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('arm')

def generate(env):
   #===============================================================================
   # Special init for our tools and special dependency in CUST_H
   #===============================================================================
   env.Replace(ARM_TOOLS_ENV = env)
   
   env.AddMethod(add_program, "AddProgram")

   # Replace Static Object Emitter
   env.Object.builder.add_emitter(suffix='.c', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.cpp', emitter=C_static_obj_emitter)
   env.Object.builder.add_emitter(suffix='.s', emitter=S_static_obj_emitter)
   
   # Standard include paths always searched
   env.Append(CPPPATH = [
      ".",
      "${INC_ROOT}/build/cust",
      "${INC_ROOT}/build/ms",
   ])
   
   # special dependencies in CUST_H
   cust_scanner = SCons.Scanner.C.CScanner()
   cust_path = cust_scanner.path(env)
   
   if 'CUST_H' in env:
      cust_h_file = env.FindFile(env['CUST_H'], env['CPPPATH'])
      if cust_h_file:
         env.Append(EXPLICIT_DEPS = [cust_h_file])
         deps = cust_scanner(cust_h_file, env, cust_path)
         if deps:
            env.Append(EXPLICIT_DEPS = deps)
      
   if 'AMSS_CUST_H' in env:
      cust_h_file = env.FindFile(env['AMSS_CUST_H'], env['CPPPATH'])
      if cust_h_file:
         env.Append(EXPLICIT_DEPS = [cust_h_file])
         deps = cust_scanner(cust_h_file, env, cust_path)
         if deps:
            env.Append(EXPLICIT_DEPS = deps)
    
   # Replace RDirs for a don't mess with it version
   #env.Replace(RDirs = RDirs)
   #env.Replace(_CPPINCFLAGS = '$( ${_concat(INCPREFIX, CPPPATH, INCSUFFIX, __env__, RDirs, TARGET, SOURCE)} $)')

   
   #============================================================================
   #                             TOOL DEFINITIONS
   #============================================================================
   # The following environment variables must be defined prior to using this make
   # file: ARMHOME, ARMINC, QCTOOLS. In addition the PATH must be updated for
   # the ARM tools.
   

   #----------------------------------------------------------------------------
   # Tool path definitions
   #----------------------------------------------------------------------------

   # Get environment info for ARMCT versions beginning with
   # Major ARM version 5
   armtools = os.environ.get('ARMTOOLS', None)
   arm_ver  = 5
   
   if armtools is not None:
      env.Replace(ARMTOOLS = armtools)
      env.Replace(ARM_TOOLS = armtools)      
      if 'ARMCT5' in armtools:
         arm_ver = 5
      #elif 'ARMCT6' in armtools:
         #arm_ver = 6
   else:
      env.Replace(ARMTOOLS = "ARMCT5")
      env.Replace(ARM_TOOLS = "ARMCT5")
      armtools = 'Assuming ARMCT5'
      
   # Setup the ARM major version number
   env.Replace(ARMTOOLSVERSION = arm_ver)

   if os.environ.has_key('ARMCC5BIN'):
      env.Replace(ARMBIN = os.environ['ARMCC5BIN'])
      env.Replace(ARMLIB = os.environ['ARMCC5LIB'])
      env.Replace(ARMINC = os.environ['ARMCC5INC'])
   elif os.environ.has_key('ARMBIN'):
      env.Replace(ARMBIN = os.environ['ARMBIN'])
      env.Replace(ARMLIB = os.environ['ARMLIB'])
      env.Replace(ARMINC = os.environ['ARMINC'])
   else:
      env.PrintError('')
      env.PrintError('-------------------------------------------------------------------------------')
      env.PrintError('*** ARMCC5BIN or ARMBIN OS variable not defined!!')
      env.PrintError('*** Please verify that ARM Toolchain is setup correctly')
      env.PrintError('-------------------------------------------------------------------------------')
      env.PrintError('')
      Exit(1)
      
   env.PrintInfo('ARMTOOLS        = ${ARMTOOLS}')
   env.PrintInfo('ARMTOOLSVERSION = ${ARMTOOLSVERSION}')
   env.PrintInfo('ARMBIN          = ${ARMBIN}')
   env.PrintInfo('ARMLIB          = ${ARMLIB}')
   env.PrintInfo('ARMINC          = ${ARMINC}')

   #-------------------------------------------------------------------------------
   # Software tool and environment definitions
   #-------------------------------------------------------------------------------
   # Check ARMBIN path otherwise clear it to use system environment
   if env.PathExists("${ARMBIN}"):
      ARMBIN = env.RealPath("${ARMBIN}", posix=False)
      if env['PLATFORM'] in ['windows', 'win32']:
         ARMBIN = ARMBIN + "\\"
      else:
         ARMBIN = ARMBIN + "/"
   else:
      env.PrintError('')
      env.PrintError('-------------------------------------------------------------------------------')      
      env.PrintError('*** ARMBIN variable set, but directory does not exist')
      env.PrintError('*** ARMBIN = ${ARMBIN}')
      env.PrintError('*** Please verify that ARM Toolchain is setup correctly')
      env.PrintError('-------------------------------------------------------------------------------')      
      env.PrintError('')
      Exit(1)
   

   # ARM SDT Thumb 16-bit inst. set ANSI C compiler
   env.Replace(TCC = ARMBIN + "armcc${EXE_EXT} --thumb")

   # ARM SDT ARM 32-bit inst. set ANSI C compiler
   env.Replace(ARMCC = ARMBIN + "armcc${EXE_EXT}")

   env.Replace(CC = env['TCC'])

   # ARM SDT Thumb 16-bit inst. set ANSI C++ compiler
   env.Replace(TCPP = ARMBIN + "armcc${EXE_EXT} --thumb  --cpp")

   env.Replace(CXX = env['TCPP'])

   # ARM SDT ARM 32-bit inst. set ANSI C++ compiler
   env.Replace(ARMCPP = ARMBIN + "armcc${EXE_EXT} --cpp")

   env.Replace(ARMCXX = env['ARMCPP'])

   # ARM SDT assembler
   env.Replace(AS = ARMBIN + "armasm${EXE_EXT}")

   # ARM SDT library archiver
   env.Replace(AR = ARMBIN + "armar${EXE_EXT}")

   # ARM SDT linker
   env.Replace(LINK = ARMBIN + "armlink${EXE_EXT}")

   # ARM SDT utility to create hex file from image
   env.Replace(HEXTOOL = ARMBIN + "fromelf${EXE_EXT}")

   # ARM SDT utility to create binary file from image
   env.Replace(BINTOOL = ARMBIN + "fromelf${EXE_EXT}")

   # Preprocess assembly cleaner
   env.Replace(PPASM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))   
   env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE}")  
   
   # Header dependency tree generator
   env.Replace(HDEPTREE_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/hdeptree.py"))   
   env.Replace(HDEPTREE = "${PYTHONCMD} ${HDEPTREE_FILE}")  

   #----------------------------------------------------------------------------
   # Processor architecture options
   #----------------------------------------------------------------------------
   env.Replace(ARM_CPU_CMD = "--cpu")
   env.Replace(ARM_ASM = "-D_ARM_ASM_")                 # ARM assembly language selection
   env.Replace(ARM = "-DT_ARM -D__ARMEL__ -D__RVCT__")  # ARM processor target
   env.Replace(ARM_CPU_ARM7 = "ARM7TDMI-S")
   env.Replace(ARM_CPU_ARM9 = "ARM926EJ-S")
   env.Replace(ARM_CPU_ARM11 = "ARM1136J-S")
   env.Replace(ARM_CPU_SPARROW = "Cortex-A5")
   
   # TODO: no unaligned access belongs on the code generation; can it be done regardless of cpu??   
   #env.Replace(ARM_CPU_SCORPION = "QSP")
   env.Replace(ARM_CPU_SCORPION = "QSP.no_neon.no_vfp --no_unaligned_access")
      
   if 'PROC' in env:
      if env['PROC'] in ['arm7']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_ARM7")
      elif env['PROC'] in ['arm9']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_ARM9")
      elif env['PROC'] in ['arm11']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_ARM11")
      elif env['PROC'] in ['scorpion']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_SCORPION")
      elif env['PROC'] in ['sparrow']:
         env.Replace(ARM_CPU = "$ARM_CPU_CMD $ARM_CPU_SPARROW")
   else:
      env.Replace(ARM_CPU = "$ARM_CPU_CMD CPU_UKNOWN")

   env.Replace(ARMCC_CPU = "${ARM_CPU} ${ARM}")
   env.Replace(ARMASM_CPU = "${ARM_CPU} ${ARM} ${ARM_ASM}")

   #----------------------------------------------------------------------------
   # NOTE:
   # If software stack checking support is enabled, FEATURE_STACK_CHECK must be
   # #defined in the cust*.h file to enable the required common services support.
   #
   # The following APCS options are obsolete in the ARM SDT 2.50 tools:
   #   PCSZ
   #   FRAMEPTR
   #   REENTRNT
   #----------------------------------------------------------------------------

   #----------------------------------------------------------------------------
   # Compiler/Linker/Tools output options
   #----------------------------------------------------------------------------
   env.Replace(ARMCC_OUTPUT_CMD = "-o")
   env.Replace(ARMCC_COMPILE_CMD = "-c")
   env.Replace(ARMCC_PREPROCESS_CMD = "-E")
   env.Replace(ARMCC_ASMOUT_CMD = "-S")
   env.Replace(ARMCC_ASM_CMD = "--asm")
   env.Replace(ARMCC_INTERLEAVE_CMD = "--interleave")
   env.Replace(ARMAS_OUTPUT_CMD = "-o")
   env.Replace(ARMAS_LIST_CMD = "--list")
   env.Replace(ARMAR_LIB_REPLACE = "-r")
   env.Replace(ARMAR_LIB_QUITE = "-c")
   env.Replace(ARMLD_OUTPUT_CMD = "-o")
   env.Replace(ARMLD_ELF_CMD = "--elf")
   env.Replace(ARMLD_MAP_CMD = "--map")
   env.Replace(ARMLD_INFO_CMD = "--info")
   env.Replace(ARMLD_LIST_CMD = "--list")
   env.Replace(ARMLD_SYMBOLS_CMD = "--symbols")
   env.Replace(ARMLD_SYMDEFS_CMD = "--symdefs")
   env.Replace(ARMLD_SCATTER_CMD = "--scatter")
   env.Replace(ARMLD_REMOVE_CMD = "--remove")
   env.Replace(ARMLD_NOREMOVE_CMD = "--noremove")
   env.Replace(ARMLD_CALLGRAPH_CMD = "--callgraph")   
   env.Replace(ARMBIN_BIN_CMD = "--bin")
   env.Replace(ARMBIN_ELF_CMD = "--elf")
   env.Replace(ARMBIN_OUTPUT_CMD = "-o")
   env.Replace(ARMBIN_NODEBUG_CMD = "--nodebug")
   env.Replace(ARMBIN_NOLINKVIEW_CMD = "--nolinkview")
   env.Replace(ARMHEX_OUTPUT_CMD = "-o")
   env.Replace(ARMHEX_I32_CMD = "--i32")
   

   #----------------------------------------------------------------------------
   # Compiler/assembler debug options
   #----------------------------------------------------------------------------
   if 'USES_NO_DEBUG' in env:
      env.Replace(ARM_DBG = "")
   else:
      env.Replace(ARM_DBG = "-g --dwarf2")            # Enable DWARF2 format debug tables
  
   #----------------------------------------------------------------------------
   # ARM Procedure Call Standard (APCS) options
   #----------------------------------------------------------------------------

   env.Replace(ARM_PCSZ = "32")                       # Program counter (PC) size (bits)

   if "USES_STACKCHK" in env:
      env.Replace(ARM_STACKCHK = "swst")              # software stack checking support
   else:
      env.Replace(ARM_STACKCHK = "noswst")            # No software stack checking support

   env.Replace(ARM_FRAMEPTR = "nofp")                 # No frame pointer
   env.Replace(ARM_FLOATPNT = "softfp")               # Use software floating point library
   env.Replace(ARM_INTRWORK = "interwork")            # ARM/Thumb interworking support
   env.Replace(ARM_NARROW   = "narrow")               # Use caller parameter narrowing
   env.Replace(ARM_REENTRNT = "nonreentrant")         # No support for re-entrancy
   env.Replace(ARM_NOROPI = "noropi")
   env.Replace(ARM_NORWPI = "norwpi")

   if env['MSM_ID'] in ['6695']:
      # --apcs /interwork/noropi/norwpi/noswst
      env.Replace(ARMCC_APCS = "--apcs /${ARM_INTRWORK}/${ARM_NOROPI}/${ARM_NORWPI}/${ARM_STACKCHK}")
      
   else:
      env.Replace(ARMCC_APCS = "--apcs /${ARM_STACKCHK}/${ARM_INTRWORK}")
      
   env.Replace(ARMCXX_APCS = "--apcs /${ARM_INTRWORK}/${ARM_NOROPI}/${ARM_NORWPI}/${ARM_STACKCHK}")
   
   #----------------------------------------------------------------------------
   # Compiler optimization options
   #----------------------------------------------------------------------------
   env.Replace(ARM_OPT_TIME = "-Otime")
   env.Replace(ARM_OPT_SPACE = "-Ospace")
   env.Replace(ARM_OPT_3 = "-O3")
   env.Replace(ARM_OPT_2 = "-O2")
   env.Replace(ARM_OPT_1 = "-O1")
   
      
   # NOTE: An optimization issue that may cause unpredictable software behavior 
   # exists with RVCT 2.2 Patch 593 and Patch 616 compilers. When -O2 optimization 
   # level is used, the R14 (link register) can become corrupted, leading to 
   # erratic software operation. To avoid this issue, -O2 optimization 
   # must not be used; instead, -O1 can be utilized.
   env.Replace(ARMCC_OPT = "${ARM_OPT_TIME} ${ARM_OPT_1}")

   # The other opts found
   env.Replace(ARMCXX_OPT = "${ARM_OPT_SPACE} ${ARM_OPT_3} ${ARM_OPT_2}")

   env.Replace(ARMASM_OPT = "${ARM_OPT_1}")

   #----------------------------------------------------------------------------
   # Compiler code generation options
   #----------------------------------------------------------------------------
   env.Replace(ARM_ALIGN = "--memaccess -UL41")
   env.Replace(ARM_NOINLINE = "--no_inline")
   env.Replace(ARM_NOAUTOINLINE = "--no_autoinline")
   env.Replace(ARM_NO_UNALIGNED_ACCESS = "--no_unaligned_access")
   env.Replace(ARM_NOPT = "--force_new_nothrow")      # this is added for CR 139116
   env.Replace(ARM_END = "--littleend")               # Compile for little endian memory architecture
   env.Replace(ARM_ZA  = "-za1")                      # LDR may only access 32-bit aligned addresses
   env.Replace(ARM_ZAS = "-zas4")                     # Min byte alignment for structures
   env.Replace(ARM_ZAP = "-zap0")                     # Struct ptrs NOT assumed to be aligned per -zas
   env.Replace(ARM_ZAT = "-zat1")                     # Min byte alignment for top-level static objects
   env.Replace(ARM_ZZT = "-zzt0")                     # Force uninitialized variables to ZI data area
   env.Replace(ARM_SPLIT_SECTIONS = "--split_sections")
   env.Replace(ARM_FPMODE_FAST = "--fpmode fast")
   env.Replace(ARM_DATA_REORDER = "--data_reorder")
   env.Replace(ARM_LOOSE_IMPLICIT_CAST = "--loose_implicit_cast")
   
   if ('BUILD_BOOT_CHAIN' in env) or ('BUILD_TOOL_CHAIN' in env):
      env.Replace(ARMCC_CODE = "${ARM_END} ${ARM_ALIGN}")
      
   elif env['MSM_ID'] in ['6695']:
      # --fpmode fast --littleend --no_inline --no_autoinline --split_sections --data_reorder
      env.Replace(ARMCC_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_NOINLINE} " \
         "${ARM_NOAUTOINLINE} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER}")
         
   else:
      env.Replace(ARMCC_CODE = "${ARM_END} ${ARM_NOPT}")

   env.Replace(ARMCXX_CODE = "${ARM_FPMODE_FAST} ${ARM_END} ${ARM_NOINLINE} " \
      "${ARM_NOAUTOINLINE} ${ARM_SPLIT_SECTIONS} ${ARM_DATA_REORDER} ${ARM_LOOSE_IMPLICIT_CAST}")
      
   #----------------------------------------------------------------------------
   #  Stack Protection
   #----------------------------------------------------------------------------
   if 'USES_STACK_PROTECTOR' in env:
      env.Replace(RVCT_STKPRO = "--protect_stack")
   else:
      env.Replace(RVCT_STKPRO = "")

   #----------------------------------------------------------------------------
   # common defines
   #----------------------------------------------------------------------------
   if 'CUST_H' in env:
      env.Replace(CUST_H_CPPDEFINE = '-DCUST_H=\\"${CUST_H}\\"')
      
   if 'BUILD_ID' in env:
      env.Replace(BUILD_TARGET_CPPDEFINE = '-DBUILD_TARGET=\\"${BUILD_ID}\\"')
      
   if 'BUILD_VER' in env:
      env.Replace(BUILD_VER_CPPDEFINE = '-DBUILD_VER=\\"${BUILD_VER}\\"')

   if 'BUILD_ASIC' in env:
      env.Replace(DBUILD_ASIC_CPPDEFINE = '-DBUILD_ASIC=\\"${BUILD_ASIC}\\"')
      
   
   env.Replace(BUILD_CCFLAGS = '${CUST_H_CPPDEFINE} ${BUILD_TARGET_CPPDEFINE} '\
      '${BUILD_VER_CPPDEFINE} ${DBUILD_ASIC_CPPDEFINE} '\
      '-DBUILD_LICENSE_ENVIRONMENT=NON_GPL_ENVIRONMENT -DNON_GPL_ENVIRONMENT=1 '\
      '-DGPL_ENVIRONMENT=2 -D__FILENAME__=\\"$SOURCE.file\\" ')
      
   env.Replace(BUILD_ASMFLAGS = '${CUST_H_CPPDEFINE} ${BUILD_TARGET_CPPDEFINE} '\
      '${BUILD_VER_CPPDEFINE} ${DBUILD_ASIC_CPPDEFINE} -D__FILENAME__=\\"$SOURCE.file\\" ')
   
   #----------------------------------------------------------------------------
   # Linker options
   #----------------------------------------------------------------------------


   #----------------------------------------------------------------------------
   # NOTE:
   # The -zap0 option is required due to a known ARM 2.50 compiler bug with
   # pointers to struct sub-fields within __packed structs
   #----------------------------------------------------------------------------

   env.Replace(INCPREFIX = "-I")
   env.Replace(CPPDEFPREFIX  = "-D")
   env.Replace(OBJSUFFIX = ".o")

   env.Replace(LIBPREFIX = "")
   env.Replace(LIBSUFFIX = ".lib")
   env.Replace(LIBSUFFIXES = ['$LIBSUFFIX', '.l', '.a'])
   env.Replace(LIBLIKNPREFIX = "")
   env.Replace(DO_NOT_USE_LIBLINKSUFFIX = "")
   env.Replace(LIBDIRPREFIX = "--userlibpath ")

   env.Replace(PROGPREFIX = "")
   env.Replace(PROGSUFFIX = ".elf")

   # max cmd line lenght
   # on a cmd sh for XP or newer max comand line is 8191, on 2000 and NT is 2047
   # usig CreateProcess (our internal spawn) 32766.
   # POSIX OSs is no issue command line limit usually in the 128k range.
   # cygwin has the added restriction of max 32k including OS env variables
   # we have seen a lot of issues with OS environment grater then 20k 
   # (What is going on from MAKE world why is environment so large??)
   if env['PLATFORM'] in ['windows', 'win32']:
      env.Replace(MAXLINELENGTH = 32000)
   elif env['PLATFORM'] in ['cygwin']:
      dyn_maxlinelength = 32000 - len(str(os.environ))
      if dyn_maxlinelength < 2000:
         dyn_maxlinelength = 2000
      env.Replace(MAXLINELENGTH = dyn_maxlinelength)
   else:
      env.Replace(MAXLINELENGTH = 100000)
      
   # WARNING!! cmdn can't have any shell operation such as "<", ">" or "|"
   # if it does we will default back os shell and its cmd lenght restrictions 
   # will be in effect.

   # Ecapsulate within quotes so we retain the space after --via
   env.Replace(TEMPFILEPREFIX = "\"--via \"")
   env.Replace(TEMPFILESUFFIX = "")
   
   # Add a way to expand link files from a list...
   env.Replace(LISTFILEPREFIX = "\"--via \"")
   env.Replace(LISTFILESUFFIX = "")
   env.Replace(_LISTFILES = '${_concat(LISTFILEPREFIX, LISTFILES, LISTFILESUFFIX, __env__)}')     
      
   # Init FLAGS      
   env.Replace(CCFLAGS = "$ARMCC_CPU $ARMCC_APCS $ARMCC_CODE $ARM_DBG $ARMCC_OPT $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS $RVCT_STKPRO ")
   env.Replace(CXXFLAGS = "$ARMCC_CPU $ARMCXX_APCS $ARMCXX_CODE $ARM_DBG $ARMCXX_OPT $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS $RVCT_STKPRO ")
   env.Replace(ASMFLAGS = "$ARMASM_CPU $ARMCC_APCS $ARMASM_OPT $BUILDFLAGS $ASM_DFLAGS $BUILD_ASMFLAGS $RVCT_STKPRO ")
   env.Replace(ASFLAGS = "$ARM_CPU $ARMCC_APCS $ARM_DBG ")
   env.Replace(ARFLAGS = "$ARMAR_LIB_REPLACE $ARMAR_LIB_QUITE")
   env.Replace(LINKFLAGS = "$ARMLD_ELF_CMD $ARMLD_MAP_CMD $ARMLD_INFO_CMD sizes,totals $ARMLD_LIST_CMD " \
      "${TARGET.base}.map $ARMLD_SYMBOLS_CMD $ARMLD_SYMDEFS_CMD ${TARGET.base}.sym $ARMLD_SCATTER_CMD " \
      "${TARGET.base}.scl $ARMLD_NOREMOVE_CMD $LFLAGS")

   CCCOM = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(CCCOM =       CCCOM)
   env.Replace(CCCOMCPY =    CCCOM)
   env.Replace(CCPPCOM =     CCCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CCMIXCOM =    CCCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES -o $TARGET")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET}.pp -o ${TARGET}.i")
   env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")

   # TODO: need to remove CCFALGS from CXXCOM it is couseing a lot of warnings, a lot of files in MULTIMEDIA need to be fixed first
   #CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $CCFLAGS $_CCCOMCOM $INCPATHS $ARMCC_COMPILE_CMD $SOURCES $ARMCC_OUTPUT_CMD $TARGET')}"
   env.Replace(CXXCOM =       CXXCOM)
   env.Replace(CXXCOMCPY =    CXXCOM)
   env.Replace(CXXPPCOM =     CXXCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_PREPROCESS_CMD'))
   env.Replace(CXXMIXCOM =    CXXCOM.replace('$ARMCC_COMPILE_CMD','$ARMCC_ASMOUT_CMD $ARMCC_ASM_CMD $ARMCC_INTERLEAVE_CMD'))
   env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
   env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])
   
   env.Replace(ARCOM =      "${TEMPFILE('$AR $ARFLAGS $TARGET $SOURCES')}")
   env.Replace(LINKCOM =    "${TEMPFILE('$LINK $LINKFLAGS $_LIBDIRFLAGS $_LISTFILES $_LIBFLAGS  $SOURCES $ARMLD_OUTPUT_CMD $TARGET')}")

   env.Replace(ASCPPCOM =  "$CC $ASMFLAGS $AFLAGS $_CCCOMCOM $INCPATHS $ARMCC_PREPROCESS_CMD $SOURCES $ARMCC_OUTPUT_CMD ${TARGET}.pp")
   
   ASASCOM = "$AS $ASFLAGS $_CPPINCFLAGS $INCPATHS $ARMAS_LIST_CMD ${TARGET}.lst ${TARGET}.i $ARMAS_OUTPUT_CMD $TARGET"
   env.Replace(ASASCOM = ASASCOM)
   
   AsmPreprocessAction = SCons.Action.Action('${TEMPFILE("$ASCPPCOM")}', '$ASCPPCOMSTR')
   AsmCleanAction = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
   AsmAssemblyAction = SCons.Action.Action('${TEMPFILE("$ASASCOM")}', '$ASASCOMSTR')

   env.Replace(ASCOM = [
      AsmPreprocessAction,                   # C pre-process assembly files
      AsmCleanAction,                        # Removed junck generated by C preprocess
      AsmAssemblyAction,                     # Do the actuall assymbly
      #env.GetDeleteAction('${TARGET}.i'),   # Do not delete here needed to debug on target
      #env.GetDeleteAction('${TARGET}.pp'),
      #env.GetDeleteAction('${TARGET}.lst'),
   ])
   
   env.Replace(ASCOMCPY = ASASCOM)
   
   # avoid having a rallib run for no reason, scons defaults does this, but is maybe casing issues
   # as there is no actual ranlibcom
   env.Replace(RANLIBCOM = "")
   env.Replace(RANLIBCOMSTR = "")

   #============================================================================
   # COMMON CC ADDITIONS
   #============================================================================
   comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

   if env.PathExists(comcc_addt):
      env.LoadToolScript(comcc_addt)
      
   #============================================================================
   # Other options
   #============================================================================
   callgraph = env.GetOption('callgraph')
   
   if callgraph:
      env.Append(LINKFLAGS = " $ARMLD_CALLGRAPH_CMD")

   #============================================================================
   # ALTERNATE PRODUCT OPTIONS
   #============================================================================
   arm_product = None
   
   if env.PathExists("${BUILD_SCRIPTS_ROOT}/arm_product.py"):
      arm_product = "${BUILD_SCRIPTS_ROOT}/arm_product.py"
   elif env.PathExists("${BUILD_MS_ROOT}/arm_product.py"):
      arm_product = "${BUILD_MS_ROOT}/arm_product.py"
   
   if arm_product is not None:
      env.LoadToolScript(arm_product)
   
   #============================================================================
   # ALTERNATE OEM OPTIONS
   #============================================================================
   arm_oem = "${BUILD_SCRIPTS_OEM_ROOT}/arm_oem.py"
   
   if env.PathExists(arm_oem):
      env.LoadToolScript(arm_oem)
   

   #print env.Dump()

   
   '''Possible TARGET.attribute or SOURCE.attributes
   
   dictSpecialAttrs = { "base"     : __get_base_path,
                        "posix"    : __get_posix_path,
                        "windows"  : __get_windows_path,
                        "win32"    : __get_windows_path,
                        "srcpath"  : __get_srcnode,
                        "srcdir"   : __get_srcdir,
                        "dir"      : __get_dir,
                        "abspath"  : __get_abspath,
                        "filebase" : __get_filebase,
                        "suffix"   : __get_suffix,
                        "file"     : __get_file,
                        "rsrcpath" : __get_rsrcnode,
                        "rsrcdir"  : __get_rsrcdir,
                      }   
   '''