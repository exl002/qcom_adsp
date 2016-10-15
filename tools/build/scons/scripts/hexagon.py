#===============================================================================
#
# CoreBSP HEXAGON tool rules
#
# GENERAL DESCRIPTION
#    HEXAGON Tools definitions
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/scons/scripts/hexagon.py#12 $
#  $DateTime: 2013/10/10 15:27:07 $
#  $Change: 4582710 $
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

RDirsSCons = ""
RelPath = ""
NormPath = ""

#------------------------------------------------------------------------------
# RDirs_rel
# Create relative paths.
#------------------------------------------------------------------------------
def RDirs_rel (path_list):
   global RDirsSCons
   global RelPath
   #import pdb; pdb.set_trace()
   #path_list = RDirsSCons(path_list)
   path_list = [RelPath(str(p), os.getcwd()) for p in path_list]
   return path_list

#------------------------------------------------------------------------------
# RDirs_posix
# Create relative paths.
#------------------------------------------------------------------------------
def RDirs_posix (path_list):
   global RDirsSCons
   global NormPath
   #import pdb; pdb.set_trace()
   #path_list = RDirsSCons(path_list)
   path_list = [NormPath(str(p), posix=True) for p in path_list]
   return path_list
   
#------------------------------------------------------------------------------
# ESCAPE
# escape hack to check for temp files since qdsp6 tools don't have a way to
# add temp files, so we use `cat file` to get this working; unfortunately SCons
# will add quotes ("") around args with spaces :(. So this hack allows us
# to intercept such changes and fix it before spawn executes the command
#------------------------------------------------------------------------------
class hexagon_cyg:
   def __init__(self, env):
      self.env = env
      self.get_cygwin_cmd = env['GET_CYGWIN_CMD']

   def get_cmd(self, sh, escape, cmd, args, env = None):
      #import pdb; pdb.set_trace()
      cmdline, env = self.get_cygwin_cmd(sh, escape, cmd, args, env)
      
      cmdline = cmdline.replace("\\\"`", "`")
      cmdline = cmdline.replace("`\\\"","`")
      
      return cmdline, env
   
#------------------------------------------------------------------------------
# Chain emitter for C files
# Adds explicit dep
#------------------------------------------------------------------------------
def C_static_obj_emitter(target, source, env):
   #import pdb; pdb.set_trace()
   
   # call defualt static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)
   
   preprocess = env.GetOption('preprocess')
   hdeptree = env.GetOption('hdeptree')
   mixasm = env.GetOption('mixasm')
   
   if preprocess or hdeptree:
      env.Clean(tgt, "".join([str(tgt[0]), ".i"]))
      env.Clean(tgt, "".join([str(tgt[0]), ".pp"]))
      
   if hdeptree:      
      env.Clean(tgt, "".join([str(tgt[0]), ".dep"]))
      
   if mixasm:
      env.Clean(tgt, "".join([str(tgt[0]), ".mix"]))
      
   if 'EXPLICIT_DEPS' in env:
      env.Depends(tgt, env['EXPLICIT_DEPS'])
   
   return (tgt, src)

#------------------------------------------------------------------------------
# Chain emitter for asm files
# Adds explicit dep
#------------------------------------------------------------------------------
def S_static_obj_emitter(target, source, env):
   #import pdb; pdb.set_trace()
   
   # call defualt static object emitter
   tgt, src = SCons.Defaults.StaticObjectEmitter(target, source, env)

   preprocess = env.GetOption('preprocess')
   hdeptree = env.GetOption('hdeptree')
   mixasm = env.GetOption('mixasm')

   # add cleaners for assembling side-effects, taht scons can't auto detect
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
   # Init vars and eviroment needed to use Program builder
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
   target_elf = env.Program(target, source, **kargs)
   
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
      #env.Append(LISTFILES = [objs_linker_file[0].get_abspath()])

   if len(libs) > 0:
      libs_lf_fname = env.subst("%s_libs.lf" % target_base)
      libs_linker_file = env.ListFileBuilder(libs_lf_fname, libs, 
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, libs_linker_file)
      #env.Append(LISTFILES = [libs_linker_file[0].get_abspath()])
      
   if 'USES_ANALYTIC_DATA' in env:
      srcs_lf_fname = env.subst("%s_src.lf" % target_base)
      sources_list_file = env.SourceListBuilder(srcs_lf_fname, [objs, libs],
         add_header=True, relative_path=build_dir, posix=True)
      env.Depends(target_elf, sources_list_file)
      
   # add dependency on external build-system files
   exbldsys_files = None
   if len(list_files) > 0:
      exbldsys_list_fname = env.subst("%s_exbldsys.lf" % target_base)
      exbldsys_files = env.ListFileExtractor(exbldsys_list_fname, list_files)
      env.Depends(target_elf, exbldsys_files)
      #env.Append(LISTFILES = [exbldsys_files[0].path])
      
   # other special dependencies for ELF come from the special QDSP5OBJ link groups
   for obj_group_num in range(1,5):
      obj_group = env.GetHexagonLinkGroup(obj_group_num)
      for i in obj_group:
         env.Depends(target_elf, env.RealPath(i))
   
   # side effects of creating a program...
   #import pdb;pdb.set_trace()
   map_file = env.subst("%s.elf.map" % target_base)

   env.SideEffect(map_file, target_elf)
   env.Clean(target_elf, map_file)
   
   sym_file = env.subst("%s.sym" % target_base)
   env.SideEffect(sym_file, target_elf)
   env.Clean(target_elf, sym_file)
   
   callg_file = env.subst("%s.htm"  % target_base)
   env.SideEffect(callg_file, target_elf)
   env.Clean(target_elf, callg_file)

   return target_elf

   
#-------------------------------------------------------------------------------
# Append Hexagon Link Group
#-------------------------------------------------------------------------------
def append_hexagon_link_group (env, group_num, val):
   hexagon_tool_env = env.get('HEXAGON_TOOLS_ENV')
   link_group_str = 'QDSP6OBJS%s' % group_num
   
   if hexagon_tool_env is not None:
      link_group_list = hexagon_tool_env.get(link_group_str, [])
      link_group_list.append(val)
      hexagon_tool_env[link_group_str] = env.Flatten(link_group_list)
      return hexagon_tool_env.get(link_group_str)

#-------------------------------------------------------------------------------
# Prepend Hexagon Link Group
#-------------------------------------------------------------------------------
def prepend_hexagon_link_group (env, group_num, val):
   hexagon_tool_env = env.get('HEXAGON_TOOLS_ENV')
   link_group_str = 'QDSP6OBJS%s' % group_num
   
   if hexagon_tool_env is not None:
      link_group_list = hexagon_tool_env.get(link_group_str, [])
      link_group_list.insert(0, val)
      hexagon_tool_env[link_group_str] = env.Flatten(link_group_list)
      return hexagon_tool_env.get(link_group_str)

#-------------------------------------------------------------------------------
# Replace Hexagon Link Group
#-------------------------------------------------------------------------------
def replace_hexagon_link_group (env, group_num, val):
   hexagon_tool_env = env.get('HEXAGON_TOOLS_ENV')
   link_group_str = 'QDSP6OBJS%s' % group_num   
   
   if hexagon_tool_env is not None:
      hexagon_tool_env[link_group_str] = env.Flatten(val)
      return hexagon_tool_env.get(link_group_str)

#-------------------------------------------------------------------------------
# Get Hexagon Link Group
#-------------------------------------------------------------------------------
def get_hexagon_link_group (env, group_num):
   hexagon_tool_env = env.get('HEXAGON_TOOLS_ENV')
   link_group_str = 'QDSP6OBJS%s' % group_num
   
   if hexagon_tool_env is not None:
      return hexagon_tool_env.get(link_group_str)

#------------------------------------------------------------------------------
# Hooks for SCons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('hexagon')

def generate(env):
   #===============================================================================
   # Special init for our tools and special dependency in CUST_H
   #===============================================================================
   env.Replace(HEXAGON_TOOLS_ENV = env)
   
   env.AddMethod(add_program, "AddProgram")
   
   env.AddMethod(append_hexagon_link_group, "AppendHexagonLinkGroup")
   env.AddMethod(prepend_hexagon_link_group, "PrependHexagonLinkGroup")   
   env.AddMethod(replace_hexagon_link_group, "ReplaceHexagonLinkGroup")
   env.AddMethod(get_hexagon_link_group, "GetHexagonLinkGroup")

   #===============================================================================
   # figure out what tools set will be using
   #===============================================================================
   # The following environment variables must be defined prior to using this make
   # file: GNUPATH, QCTOOLS. In addition the PATH must be updated for
   # the GCC tools.

   hexagon_root = os.environ.get('HEXAGON_ROOT', 'C:/Qualcomm/HEXAGON_Tools')   
   hexagon_rtos_release = os.environ.get('HEXAGON_RTOS_RELEASE', '1.1')
   q6version = os.environ.get('HEXAGON_Q6VERSION', 'v2')
   
   if 'HEXAGON_IMAGE_ENTRY' in env:
      hexagon_image_entry = env.get('HEXAGON_IMAGE_ENTRY', '0x00c00000')
   else:
      hexagon_image_entry = os.environ.get('HEXAGON_IMAGE_ENTRY', '0x00c00000')

   hexagon_tools = "HEXAGON%s" % hexagon_rtos_release
   
   if 'HEXAGON1' in hexagon_tools:
      hexagon_ver = 1
   elif 'HEXAGON2' in hexagon_tools:
      hexagon_ver = 2
   elif 'HEXAGON3' in hexagon_tools:
      hexagon_ver = 3
   elif 'HEXAGON4' in hexagon_tools:
      hexagon_ver = 4
   elif 'HEXAGON5' in hexagon_tools:
      hexagon_ver = 5
   elif 'HEXAGON6' in hexagon_tools:
      hexagon_ver = 6
   elif 'HEXAGON7' in hexagon_tools:
      hexagon_ver = 7
   else:
      hexagon_ver = 5
      env.PrintWarning("Cannot decode HEXAGON version default to 5!")

   #===============================================================================
   # Special init to setup cygwin and change text formatting
   #===============================================================================
   if env['PLATFORM'] in ['windows', 'win32', 'cygwin'] and hexagon_rtos_release == '1.1':
      env.LoadToolScript('cygwin', toolpath = ['${BUILD_ROOT}/tools/build/scons/scripts'])
      
      # replace escape hack because of temp files
      hexcyg = hexagon_cyg(env)
      env['GET_CYGWIN_CMD'] = hexcyg.get_cmd
      
   # since we may have change to cygwin re init formatting
      env.LoadToolScript('formatting', toolpath = ['${BUILD_ROOT}/tools/build/scons/scripts'])       

   #===============================================================================
   # Special setup for Windows Hexagon Tools not cygwin
   #===============================================================================
   if (env['PLATFORM'] in ['windows', 'win32']):
      global RDirsSCons
      global RelPath
      global NormPath
      RDirsSCons = env.get('RDirs')
      RelPath = env.RelPath
      NormPath = env.NormPath      
      #env.Replace(RDirs = RDirs_rel)
      env.Replace(RDirs = RDirs_posix)
   
      
   # init variables to environment
   env.Replace(HEXAGON_ROOT = hexagon_root)
   env.Replace(HEXAGON_IMAGE_ENTRY = hexagon_image_entry)
   env.Replace(HEXAGON_RTOS_RELEASE = hexagon_rtos_release)
   env.Replace(HEXAGON_TOOLS = hexagon_tools)
   env.Replace(HEXAGONTOOLS = hexagon_tools)   
   env.Replace(QDSP6_RELEASE_DIR = env.RealPath("${HEXAGON_ROOT}/${HEXAGON_RTOS_RELEASE}", posix=True))
   env.Replace(Q6VERSION = q6version)
   env.Replace(L4Q6_ROOT = env.RealPath("${L4_ROOT}/../l4-qdsp6", posix=True))
   
   if not env.PathExists("${QDSP6_RELEASE_DIR}"):
      env.PrintError('')
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError("*** HEXAGON tools are not present")
      env.PrintError("*** Please install HEXAGON tools RTOS 1.1, 2.0Beta, etc.")
      env.PrintError("*** Currently setup for RTOS: ${HEXAGON_RTOS_RELEASE}")
      env.PrintError("*** Missing RTOS PATH: ${QDSP6_RELEASE_DIR}")
      env.PrintError("-------------------------------------------------------------------------------")
      env.PrintError('')
      Exit(1)
   
   #===============================================================================
   # Special init for our tools and special dependency in CUST_H
   #===============================================================================
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

   #===============================================================================
   #                             TOOL DEFINITIONS
   #===============================================================================
   env.Replace(QDSP6BIN = "${QDSP6_RELEASE_DIR}/gnu/bin")
   env.Replace(QDSP6_RELEASE_LIB_DIR = "${QDSP6_RELEASE_DIR}/dinkumware/lib/qdsp6${Q6VERSION}")
   if not env.PathExists("${QDSP6_RELEASE_LIB_DIR}"):
      env.Replace(QDSP6_RELEASE_LIB_DIR = "${QDSP6_RELEASE_DIR}/dinkumware/lib/${Q6VERSION}")
   env.Replace(QDSP6_BINDIR = "${L4Q6_ROOT}/bin")

   # try default QuRT path first, then legacy
   qurt_os = True
   if 'QDSP6_LIBDIR' in env:
      qdsp6_libdir = env.get('QDSP6_LIBDIR')
   else:
      qdsp6_libdir = "${COREBSP_ROOT}/kernel/qurt/install/modem${Q6VERSION}/lib"
   
   if not env.PathExists(qdsp6_libdir):
      qurt_os = False
      qdsp6_libdir = "${COREBSP_ROOT}/kernel/blast/install/modemSW${Q6VERSION}/lib"
      if not env.PathExists(qdsp6_libdir):
         qdsp6_libdir = "${BUILD_ROOT}/core/kernel/blast/install/lib"
         if not env.PathExists(qdsp6_libdir):
            # can't find path, use legacy path, or user defined as these could be build 
            # as part of the build process
            qurt_os = True
            if 'QDSP6_LIBDIR' in env:
               qdsp6_libdir = env.get('QDSP6_LIBDIR')
            else:
               qdsp6_libdir = "${COREBSP_ROOT}/kernel/qurt/install/modem${Q6VERSION}/lib"

            #env.PrintWarning("-------------------------------------------------------------------------------")
            #env.PrintWarning("*** Cannot find kernel install libs dir!!")
            #env.PrintWarning("-------------------------------------------------------------------------------")
            #env.PrintWarning('')

   env.Replace(QDSP6_LIBDIR = qdsp6_libdir)
   
   #-------------------------------------------------------------------------------
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   #-------------------------------------------------------------------------------
   env.Replace(QDSP6_CPPPATH = [])

   #-------------------------------------------------------------------------------
   # Defines to exapnd paths and libs with posix style even on a Windows env.
   #-------------------------------------------------------------------------------
   env.Replace(LIBFLAGS_NULL = '')
   env.Replace(_LIBFLAGS_POSIX = '$( ${_concat(LIBFLAGS_NULL, _LIBFLAGS, LIBFLAGS_NULL, __env__, RDirs)} $)')
   
   if env['PLATFORM'] in ['cygwin']:
      env.Replace(_QDSP6_CPPINCFLAGS = '$( ${_concat(INCPREFIX, QDSP6_CPPPATH, INCSUFFIX, __env__, PosixRDirs, TARGET, SOURCE)} $)')  
   else:
      env.Replace(_QDSP6_CPPINCFLAGS = '$( ${_concat(INCPREFIX, QDSP6_CPPPATH, INCSUFFIX, __env__, RDirs, TARGET, SOURCE)} $)')
      
   if env.has_key('ENV'):
      env['ENV']['PATH'] = env['ENV']['PATH'] + env.subst(";${QDSP6BIN}")
      #print env.Dump()
      
   
   #-------------------------------------------------------------------------------
   # this is needed for the bintools (fromelf) we probbaly should do soemthing 
   # like a hexagon equivelant different here
   # Tool path definitions (may be tool set dependent - ADS vs RVCT)
   # !!!!! THIS SHOULD NOT BE HERE, TODO: find out who needs this (what builder) 
   # have them deal with missing path or use different tools other then ARM for 
   # an image that builds for Q6. What happens when we build only Q6 images,
   # will custoemr be require to get ARM licensce?!! (non-sense)
   #-------------------------------------------------------------------------------
   if os.environ.get('ARMBIN') is not None:
      env.Replace(ARMBIN = os.environ['ARMBIN'])
      env.Replace(ARMLIB = os.environ['ARMLIB'])
         
   #-------------------------------------------------------------------------------
   # Software tool and environment definitions
   #-------------------------------------------------------------------------------
   # Check QDSP6BIN path other wise clear it to use system environment
   if env.PathExists("${QDSP6BIN}"):
      QDSP6BIN = env.RealPath("${QDSP6BIN}")
      if env['PLATFORM'] in ['windows', 'win32']:
         QDSP6BIN = QDSP6BIN + "\\"
      else:
         QDSP6BIN = QDSP6BIN + "/"
   else:
      env.PrintWarning("${QDSP6BIN} not found using system environment. Using system paths for tools.")
      QDSP6BIN = ""
   
   env.PrintInfo("HEXAGON_TOOLS       = ${HEXAGON_TOOLS}")
   env.PrintInfo("HEXAGON_RTOS_REL    = ${HEXAGON_RTOS_RELEASE}")
   env.PrintInfo("Q6VERSION           = ${Q6VERSION}")      
   env.PrintInfo("HEXAGON_ROOT        = %s" % env.RealPath("${HEXAGON_ROOT}"))
   env.PrintInfo("QDSP6BIN            = %s" % env.RealPath("${QDSP6BIN}"))
   env.PrintInfo("HEXAGON_IMAGE_ENTRY = ${HEXAGON_IMAGE_ENTRY}")   

   #import pdb; pdb.set_trace()      
   if env['PLATFORM'] in ['cygwin']:
      QDSP6BIN = string.replace("/cygdrive/%s" % QDSP6BIN, ":", "")
   
   # if env['platform'] == 'windows':

   # HEXAGON set ANSI C compiler
   #import pdb; pdb.set_trace()
   if hexagon_ver >= 6:
      env.Replace(CC = "${QDSP6_RELEASE_DIR}/qc/bin/hexagon-clang${EXE_EXT}")
   elif hexagon_ver == 5:
      env.Replace(CC = QDSP6BIN + "hexagon-gcc${EXE_EXT}")
   else:
      env.Replace(CC = QDSP6BIN + "qdsp6-gcc${EXE_EXT}")
      
   # HEXAGON set ANSI C++ compiler
   if hexagon_ver >= 6:
      env.Replace(CXX = "${QDSP6_RELEASE_DIR}/qc/bin/hexagon-clang++${EXE_EXT}")
   elif hexagon_ver == 5:
      env.Replace(CXX = QDSP6BIN + "hexagon-g++${EXE_EXT}")
   else:
      env.Replace(CXX = QDSP6BIN + "qdsp6-g++${EXE_EXT}")

   # HEXAGON assembler
   if hexagon_ver >= 5:
      env.Replace(AS = QDSP6BIN + "hexagon-as${EXE_EXT}")
   else:
      env.Replace(AS = QDSP6BIN + "qdsp6-as${EXE_EXT}")

   # HEXAGON library archiver
   if hexagon_ver >= 5:
      env.Replace(AR = QDSP6BIN + "hexagon-ar${EXE_EXT}")
   else:
      env.Replace(AR = QDSP6BIN + "qdsp6-ar${EXE_EXT}")

   # HEXAGON linker
   if hexagon_ver >= 5:
      env.Replace(LINK = QDSP6BIN + "hexagon-ld${EXE_EXT}")
   else:
      env.Replace(LINK = QDSP6BIN + "qdsp6-ld${EXE_EXT}")
   
   # HEXAGON library archiver
   if hexagon_ver >= 5:
      env.Replace(RANLIB = QDSP6BIN + "hexagon-ranlib${EXE_EXT}")
   else:
      env.Replace(RANLIB = QDSP6BIN + "qdsp6-ranlib${EXE_EXT}")
      
   # HEXAGON nm (name list) object files symbols utility
   if hexagon_ver >= 5:
      env.Replace(NM = QDSP6BIN + "hexagon-nm${EXE_EXT}")
   else:
      env.Replace(NM = QDSP6BIN + "qdsp6-nm${EXE_EXT}") 

   # HEXAGON object file strip utility
   if hexagon_ver >= 5:
      env.Replace(STRIP = QDSP6BIN + "hexagon-strip${EXE_EXT}")
   else:
      env.Replace(STRIP = QDSP6BIN + "qdsp6-strip${EXE_EXT}")

   # HEXAGON object copy
   if hexagon_ver >= 5:
      env.Replace(OBJCOPY = QDSP6BIN + "hexagon-objcopy${EXE_EXT}")
   else:
      env.Replace(OBJCOPY = QDSP6BIN + "qdsp6-objcopy${EXE_EXT}")

   # Preprocess assembly cleaner
   env.Replace(PPASM_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/ppasm.py"))   
   env.Replace(PPASM = "${PYTHONCMD} ${PPASM_FILE}")  
   
   # Header dependency tree generator
   env.Replace(HDEPTREE_FILE = env.RealPath("${BUILD_SCRIPTS_ROOT}/hdeptree.py"))   
   env.Replace(HDEPTREE = "${PYTHONCMD} ${HDEPTREE_FILE}")  
   
   #-------------------------------------------------------------------------------
   # Compiler output options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_COMPILE_CMD = "-c")
   env.Replace(HEXAGON_PREPROCESS_CMD = "-E -xc")
   env.Replace(HEXAGON_PREPROCESS_CC_CMD = "-E -xc")
   env.Replace(HEXAGON_PREPROCESS_CXX_CMD = "-E -xc++")
   env.Replace(HEXAGON_INHIBIT_WARN_CMD = "-w")
   env.Replace(HEXAGON_OUTPUT_CMD = "-o")
   env.Replace(HEXAGON_ASMOUT_CMD = "-S")   
   env.Replace(HEXAGONAR_LIB_REPLACE = "-r")
   env.Replace(HEXAGONAR_LIB_QUITE = "c")
   env.Replace(HEXAGONLD_MAP_CMD = "-Map")
   
   env.Replace(ARMBIN_BIN_CMD = "--bin")
   env.Replace(ARMBIN_ELF_CMD = "--elf")
   env.Replace(ARMBIN_OUTPUT_CMD = "-o")
   env.Replace(ARMBIN_NODEBUG_CMD = "--nodebug")
   env.Replace(ARMBIN_NOLINKVIEW_CMD = "--nolinkview")


   #-------------------------------------------------------------------------------
   # Compiler/assembler debug options
   #-------------------------------------------------------------------------------
   if env.has_key('USES_NO_DEBUG'):
      env.Replace(HEXAGON_DBG = "")
   else:
      env.Replace(HEXAGON_DBG = "-g ") # Enable DWARF2 format debug tables


   #-------------------------------------------------------------------------------
   # Compiler optimization options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_OPT_TIME = "-Otime")
   env.Replace(HEXAGON_OPT_SPACE = "-Ospace")
   env.Replace(HEXAGON_OPT_2 = "-O2")
   env.Replace(HEXAGON_OPT_1 = "-O1")
   env.Replace(HEXAGON_OPT_S = "-Os")

   # Let's optimize for time on the L4 build
   env.Replace(HEXAGONCC_OPT = "${HEXAGON_OPT_S}")

   # the other opts found
   env.Replace(HEXAGONCXX_OPT = "${HEXAGON_OPT_SPACE} ${HEXAGON_OPT_TIME} ${HEXAGON_OPT_S}")
   env.Replace(HEXAGONASM_OPT = "${HEXAGON_OPT_1}")

   #-------------------------------------------------------------------------------
   # HEXAGON Warnings Options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_WARN = "-Wall -Wpointer-arith")
   env.Replace(HEXAGONCC_WARN = "${HEXAGON_WARN} -Wstrict-prototypes -Wnested-externs")
   env.Replace(HEXAGONCXX_WARN = "${HEXAGON_WARN}")

   #-------------------------------------------------------------------------------
   # HEXAGON Machine Options
   #-------------------------------------------------------------------------------
   hexagon_machine = "-m${Q6VERSION}"
   if env.subst("$Q6VERSION") == "v1" or env.subst("$Q6VERSION") == "v2":
      hexagon_machine += " -mv1-v2-uncached-data"
   env.Replace(HEXAGON_MACHINE = hexagon_machine)

   #-------------------------------------------------------------------------------
   # HEXAGON Undef Options
   #-------------------------------------------------------------------------------
   env.Replace(HEXAGON_UNDEF = "-Uqdsp6 -Uq6sim -Uqdsp6r0")
   
   #-------------------------------------------------------------------------------
   # HEXAGON Lang independent options
   #-------------------------------------------------------------------------------
   if hexagon_ver >= 6:
      env.Replace(HEXAGON_LANIND = "-fdata-sections -ffunction-sections "
         "-nostdlib -fno-exceptions -fno-strict-aliasing ")
   elif hexagon_ver >= 2 and hexagon_ver <= 5:
      env.Replace(HEXAGON_LANIND = "-fdata-sections -ffunction-sections " \
         "-mno-literal-pool-addresses -nostdlib -fno-exceptions " \
         "-fno-merge-constants -fno-strict-aliasing ")
         
      # There was a bug in the hexagon tools earlier then 2.0.06 (-mno-extended-crossjumping acted as a temp patch)
      if hexagon_rtos_release < "2.0.06":  # This is old code -- beware, the comparison is ascii, not numeric.
         env.Append(HEXAGON_LANIND = "-mno-extended-crossjumping ")
   else:
      env.Replace(HEXAGON_LANIND = "-fdata-sections -ffunction-sections " \
         "-mno-literal-pool-addresses  -nostdlib -fno-exceptions " \
         "-fno-merge-constants ")

   #-------------------------------------------------------------------------------
   # HEXAGON Stack Checking
   #-------------------------------------------------------------------------------
   if hexagon_ver >= 3 and 'USES_STACK_PROTECTOR' in env:
      env.Replace(HEXAGON_STKPRO = "-fstack-protector")
   else:
      env.Replace(HEXAGON_STKPRO = "")

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
   
   #-------------------------------------------------------------------------------
   # HEXAGON Flags Def.
   #-------------------------------------------------------------------------------

   env.Replace(INCPREFIX = "-I")
   env.Replace(CPPDEFPREFIX  = "-D")
   env.Replace(OBJSUFFIX = ".o")

   env.Replace(LIBPREFIX = "")
   env.Replace(LIBSUFFIX = ".lib")
   env.Replace(LIBSUFFIXES = ['$LIBSUFFIX', '.l', '.a'])
   env.Replace(LIBLIKNPREFIX = "")
   env.Replace(DO_NOT_USE_LIBLINKSUFFIX = "")
   env.Replace(LIBDIRPREFIX = "-L")

   env.Replace(PROGPREFIX = "")
   env.Replace(PROGSUFFIX = '.elf')

   # max cmd line length
   # on a cmd sh for XP or newer max command line is 8191, on 2000 and NT is 2047
   # using CreateProcess (our internal spawn) 32766.
   # POSIX OSs is no issue command line limit usually in the 128k range.
   # cygwin has the added restriction of max 32k including OS env variables
   # we have seen a lot of issues with OS environment grater then 20k 
   # (what is going on from MAKE world why is environment so large??)
   if env['PLATFORM'] in ['windows', 'win32']:
      # hexagon uses gcc code under the hood, gcc uses the OS env extensively 
      # because of this we need to limit the command line as it is then added 
      # to the OS env by gcc. the safe value we have been using is 8k
      env.Replace(MAXLINELENGTH = 32000)
   elif env['PLATFORM'] in ['cygwin']:
      env.Replace(MAXLINELENGTH = 8000)
   else:
      env.Replace(MAXLINELENGTH = 100000)
   
   # WARNING!! cmdn can't have any shell operation such as "<", ">" or "|"
   # if it does we will default back os shell and its cmd lenght restrictions 
   # will be in effect (8191).

   if env['PLATFORM'] in ['cygwin']:
      env.Replace(TEMPFILEPREFIX = "`cat ")
      env.Replace(TEMPFILESUFFIX = "`")
   else:
      env.Replace(TEMPFILEPREFIX = "@")
      env.Replace(TEMPFILESUFFIX = "")
      
   # Add a way to expand link files from a list...
   env.Replace(LISTFILEPREFIX = "@")
   env.Replace(LISTFILESUFFIX = "")
   env.Replace(_LISTFILES = '${_concat(LISTFILEPREFIX, LISTFILES, LISTFILESUFFIX, __env__)}')     


   # Init FLAGS   
   env.Replace(CCFLAGS   = "$HEXAGONCC_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS ")
   env.Replace(CXXFLAGS  = "$HEXAGONCXX_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS ")
   env.Replace(ASMFLAGS  = "$HEXAGONCC_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS ")
   env.Replace(ASFLAGS   = "$HEXAGONCC_WARN $HEXAGON_MACHINE $HEXAGON_UNDEF $HEXAGONCC_OPT $HEXAGON_DBG $HEXAGON_LANIND $HEXAGON_STKPRO $BUILD_CCFLAGS $CC_DFLAGS $CFLAGS ")
   #env.Replace(ARFLAGS   = "$HEXAGONAR_LIB_QUITE $HEXAGONAR_LIB_REPLACE")
   env.Replace(ARFLAGS   = "$HEXAGONAR_LIB_REPLACE$HEXAGONAR_LIB_QUITE")

   CCCOM = "${TEMPFILE('$CC $CCFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $SOURCES.posix')}"
   env.Replace(CCCOM =       CCCOM)
   env.Replace(CCCOMCPY =    CCCOM)
   env.Replace(CCPPCOM =     CCCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_PREPROCESS_CC_CMD $PPFLAGS'))
   env.Replace(CCMIXCOM =    CCCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_ASMOUT_CMD'))
   env.Replace(CCPPCLCOM =   "$PPASM -f $SOURCES.posix -o $TARGET.posix")
   env.Replace(CCPPCL_ACOM = "$PPASM -f ${TARGET.posix}.pp -o ${TARGET.posix}.i")
   env.Replace(HDEPTREECOM = "$HDEPTREE -f ${TARGET}.pp -o ${TARGET}.dep")      
   
   CXXCOM = "${TEMPFILE('$CXX $CXXFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $SOURCES.posix')}"
   env.Replace(CXXCOM =       CXXCOM)
   env.Replace(CXXCOMCPY =    CXXCOM)
   env.Replace(CXXPPCOM =     CXXCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_PREPROCESS_CXX_CMD $PPFLAGS'))
   env.Replace(CXXMIXCOM =    CXXCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_ASMOUT_CMD'))
   env.Replace(CXXPPCLCOM =   env['CCPPCLCOM'])
   env.Replace(CXXPPCL_ACOM = env['CCPPCL_ACOM'])   
   
   env.Replace(ARCOM     =  "${TEMPFILE('$AR $ARFLAGS $TARGET.posix $SOURCES.posix')}")
   env.Replace(RANLIBCOM =  "${TEMPFILE('$RANLIB $TARGET.posix')}")
   
   ASASCOM = "${TEMPFILE('$CC $ASFLAGS $_CCCOMCOM $INCPATHS $_QDSP6_CPPINCFLAGS $HEXAGON_OUTPUT_CMD $TARGET.posix $HEXAGON_COMPILE_CMD $SOURCES.posix')}"
   env.Replace(ASCPPCOM  =  ASASCOM.replace('$HEXAGON_COMPILE_CMD','$HEXAGON_PREPROCESS_CC_CMD $PPFLAGS'))
   env.Replace(ASASCOM   =  ASASCOM)

   AsmPreprocessAction = SCons.Action.Action('${TEMPFILE("$ASCPPCOM")}', '$ASCPPCOMSTR')
   AsmCleanAction = SCons.Action.Action('$CCPPCL_ACOM', '$CCPPCLCOM_ASTR')
   AsmAssemblyAction = SCons.Action.Action('$ASASCOM', '$ASCOMSTR')

   env.Replace(ASCOM = [
   #   AsmPreprocessAction,          # C pre-process assembly files
   #   AsmCleanAction,               # Removed junck generated by C preprocess
      AsmAssemblyAction,            # Do the actuall assymbly
   #   env.GetDeleteAction('${TARGET.posix}.pp'),
   #   env.GetDeleteAction('${TARGET.posix}.lst'),
   ])
   env.Replace(ASCOMCPY = ASASCOM)
   
   #-------------------------------------------------------------------------------
   # HEXAGON LINK, very special procedure
   #-------------------------------------------------------------------------------
   env.Replace(LINKFLAGS = "-m${Q6VERSION} -nostdlib --section-start .start=${HEXAGON_IMAGE_ENTRY} ${HEXAGONLD_MAP_CMD} ${TARGET.posix}.map --entry=${HEXAGON_IMAGE_ENTRY} ${LFLAGS} ")
            
   if 'USES_LCS_FILE' in env:
      # The following linker control script (lcs) is generated 
      # by: lcs_builder.py
      env.Append(LINKFLAGS = "-T${TARGET.posix}.lcs ")
   
   env.Replace(LINK_DIRS = "-L${QDSP6_RELEASE_LIB_DIR}")

   # LINK special case in hexagon world
   env.Replace(QDSP6OBJS1  = [
      '${QDSP6_LIBDIR}/crt0.o',
      '${QDSP6_RELEASE_LIB_DIR}/init.o',
   ])
   
   env.Replace(QDSP6OBJS2  = [
   ])
   
   if qurt_os:
      env.Replace(QDSP6OBJS3  = [
         '${QDSP6_LIBDIR}/libqurt.a',
         '${QDSP6_LIBDIR}/libqurtcfs.a',
         '${QDSP6_LIBDIR}/libqurtkernel.a',
         '${QDSP6_LIBDIR}/libqube_compat.a',
         '${QDSP6_LIBDIR}/libtimer.a',
         '${QDSP6_LIBDIR}/libposix.a',
         '${QDSP6_RELEASE_LIB_DIR}/libc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libqcc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libstdc++.a',
         ])
   else:
      env.Replace(QDSP6OBJS3  = [
         '${QDSP6_LIBDIR}/libblast.a',
         '${QDSP6_LIBDIR}/libblastkernel.a',
         '${QDSP6_LIBDIR}/libqube_compat.a',
         '${QDSP6_LIBDIR}/libtimer.a',
         '${QDSP6_LIBDIR}/libposix.a',
         '${QDSP6_RELEASE_LIB_DIR}/libc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libqcc.a',
         '${QDSP6_RELEASE_LIB_DIR}/libstdc++.a',
         ])

   env.Replace(QDSP6OBJS4  = [
      '${QDSP6_RELEASE_LIB_DIR}/fini.o',\
   ])

   #-------------------------------------------------------------------------------------
   # if $SOURCES doesn't have any items in it, python is throwing an error $SOURCES.posix.
   # When linking there should always be at least one obj it is the entry point.
   # SOURCES should never be emty if it is someone added objects to library list instead 
   # of object list. Fix that isntead of trying to get creative here.
   #-------------------------------------------------------------------------------------
   env.Replace(LINKOBJGRP = "--undefined=main ${QDSP6OBJS1} " \
      "--start-group $_LIBFLAGS_POSIX ${QDSP6OBJS2} $SOURCES.posix $_LISTFILES --end-group " \
      "--start-group ${QDSP6OBJS3}  --end-group " \
      "${QDSP6OBJS4}"
   )
   
   #cd $(TARGET_NAME); $(LD) $(LINKFLAGS) $(LINK_DIRS) $(LINKOBJGRP) $(OBJ_CMD) $(TARGET_NAME).$(EXETYPE) -Map $(TARGET_NAME).map; mv $(TARGET_NAME).$(EXETYPE) ../$(TARGET_NAME).$(EXETYPE); mv $(TARGET_NAME).map ../$(TARGET_NAME).map
   #$(CC) -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -mv2 -moslib=qube -Xlinker -m$(Q6VERSION) -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01200000 $(QDSP6_BINDIR)/lib/qube/qdsp6v2/ig_server -o ig_server.reloc
   #$(CC) -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -mv2 -moslib=qube -Xlinker -m$(Q6VERSION) -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01300000 $(TARGET_NAME).$(EXETYPE) -o $(TARGET_NAME).reloc
   
   env.Replace(LINKCOM = "${TEMPFILE('$LINK $LINKFLAGS $LINK_DIRS $LINKOBJGRP $HEXAGON_OUTPUT_CMD $TARGET.posix $LINKFLAGS_END')}")
   env.Replace(KERNEL_RELOCCOM = "$CC -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -moslib=qube -Xlinker -m${Q6VERSION} -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01200000 $SOURCE.posix -o $TARGET.posix")
   env.Replace(AMSS_RELOCCOM = "$CC -nostdlib -nostartfiles -g -Xlinker --gc-sections -Xlinker -moslib=qube -Xlinker -m${Q6VERSION} -Xlinker --entry=_start -Xlinker --section-start -Xlinker .start=0x01300000 $SOURCE.posix -o $TARGET.posix")
   
   #-------------------------------------------------------------------------------
   # Reloc Builders
   #-------------------------------------------------------------------------------
   kreloc_act = env.GetBuilderAction('$KERNEL_RELOCCOM')
   kreloc_bld = env.Builder(action = kreloc_act, suffix = '.reloc')
   env.Append(BUILDERS = {'Q6_KernelRelocBuilder' : kreloc_bld})

   amssreloc_act = env.GetBuilderAction('$AMSS_RELOCCOM')
   amssreloc_bld = env.Builder(action = amssreloc_act, suffix = '.reloc')
   env.Append(BUILDERS = {'Q6_AmssRelocBuilder' : amssreloc_bld})

   #============================================================================
   # COMMON CC ADDITIONS
   #============================================================================
   comcc_addt = "${BUILD_SCRIPTS_ROOT}/comcc_addt.py"

   if env.PathExists(comcc_addt):
      env.LoadToolScript(comcc_addt)
   
   #============================================================================
   # ALTERNATE PRODUCT OPTIONS
   #============================================================================
   hexagon_product = None
   
   if env.PathExists("${BUILD_SCRIPTS_ROOT}/hexagon_product.py"):
      hexagon_product = "${BUILD_SCRIPTS_ROOT}/hexagon_product.py"
   elif env.PathExists("${BUILD_MS_ROOT}/hexagon_product.py"):
      hexagon_product = "${BUILD_MS_ROOT}/hexagon_product.py"
   
   if hexagon_product is not None:
      env.LoadToolScript(hexagon_product)

   #============================================================================
   # ALTERNATE OEM OPTIONS
   #============================================================================
   hexagon_oem = "${BUILD_SCRIPTS_OEM_ROOT}/hexagon_oem.py"
   
   if env.PathExists(hexagon_oem):
      env.LoadToolScript(hexagon_oem)

  
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