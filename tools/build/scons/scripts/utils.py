#===============================================================================
#
# Common utils
#
# GENERAL DESCRIPTION
#    Common utilities used across the build system
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/images/scons/qc/rel/1.0/tools/build/scons/scripts/utils.py#13 $
#  $DateTime: 2013/12/12 14:08:10 $
#  $Author: coresvc $
#  $Change: 4951991 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/15/09   wd      Create
#
#===============================================================================
import os
import glob
import re
import hashlib
import subprocess
import string
import fnmatch
import shutil
import textwrap
import imp
import SCons.Action
#import SCons.Errors
#import SCons.Tool
from SCons.Script import *
from datetime import datetime
import stat
import inspect

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('utils')

def generate(env):
   
   # init utils methods   
   env.AddMethod(check_alias, "CheckAlias")
   env.AddMethod(bind_aliases_to_targets, "BindAliasesToTargets")
   env.AddMethod(get_delete_action, "GetDeleteAction")
   
   # add libs, objs, files, artifacts methods   
   env.AddMethod(is_key_enable, "IsKeyEnable")
   env.AddMethod(is_key_enable, "IsTargetEnable")
   env.AddMethod(add_file, "AddFile")
   env.AddMethod(add_library, "AddLibrary")
   env.AddMethod(add_bin_library, "AddBinaryLibrary")
   env.AddMethod(add_oem_library, "AddOEMLibrary")
   env.AddMethod(add_object, "AddObject")
   env.AddMethod(add_bin_object, "AddBinaryObject")
   env.AddMethod(add_oem_object, "AddOEMObject")
   env.AddMethod(add_document, "AddDocument")
   env.AddMethod(add_listfile, "AddListFile")
   env.AddMethod(add_artifact, "AddArtifact")
   env.AddMethod(add_program, "AddProgram")

   # add libs, objs (deprecated soon to be removed)
   env.AddMethod(add_libs_to_image, "AddLibsToImage")
   env.AddMethod(add_objs_to_image, "AddObjsToImage")
   env.AddMethod(add_libs_to_image, "AddLibs")
   env.AddMethod(add_objs_to_image, "AddObjs")
   env.AddMethod(add_docs_to_image, "AddDocs")
   
   # init print/text utils methods   
   env.AddMethod(print_stage_banner, "PrintStageBanner")
   env.AddMethod(print_image_banner, "PrintImageBanner")
   env.AddMethod(print_error, "PrintError")
   env.AddMethod(print_warning, "PrintWarning")
   env.AddMethod(print_info, "PrintInfo")
   env.AddMethod(print_debug_info, "PrintDebugInfo")
   env.AddMethod(print_data, "Print")
   env.AddMethod(create_file_banner, "CreateFileBanner")

   # init file/path utils methods
   env.AddMethod(find_files, "FindFiles")
   env.AddMethod(find_source_files, "FindSourceFiles")   
   env.AddMethod(glob_files, "GlobFiles")
   env.AddMethod(glob_source_files, "GlobSourceFiles")   
   env.AddMethod(add_explicit_deps, "AddExplicitDeps")
   env.AddMethod(remove_file, "RemoveFiles")
   env.AddMethod(md5file, "GetMD5SumFile")
   env.AddMethod(md5file_compare, "CompareMD5File")
   env.AddMethod(md5file_compare_and_update, "CompareAndUpdateMD5File")
   env.AddMethod(md5file_create, "CreateMD5SumFile")
   env.AddMethod(path_to_dict_tree, "PathToDictTree")
   env.AddMethod(filter_match, "FilterMatch")
   env.AddMethod(norm_path, "NormPath")
   env.AddMethod(path_exists, "PathExists")
   env.AddMethod(real_path, "RealPath")
   env.AddMethod(clean_pack, "CleanPack")
   
   # init file/path utils methods (deprecated soon to be removed)
   env.AddMethod(norm_path, "SubstNormPath")
   env.AddMethod(path_exists, "SubstPathExists")
   env.AddMethod(real_path, "SubstRealPath")
   
   # init and load software units methods
   env.AddMethod(load_scons_scripts, "LoadSConscripts")
   env.AddMethod(load_tool_script, "LoadToolScript")
   env.AddMethod(load_image_units, "LoadImageUnits")
   env.AddMethod(load_api_units, "LoadAPIUnits")
   env.AddMethod(get_au_sw_unit_names, "GetAreaSoftwareUnitNames")   
   env.AddMethod(load_au_sw_units, "LoadAreaSoftwareUnits")
   env.AddMethod(load_au_sw_units, "GetAreaSoftwareUnits")   
   env.AddMethod(load_sw_units, "LoadSoftwareUnits")
   env.AddMethod(get_sw_units, "GetSoftwareUnits")

   # init and load software units methods (deprecated soon to be removed)
   env.AddMethod(load_cbsp_lib_scripts_deprecated, "LoadCoreBSPLibScripts")
   env.AddMethod(load_ext_lib_scripts_deprecated, "LoadExternalLibScripts")
   env.AddMethod(load_au_sw_units_deprecated, "LoadAUSoftwareUnits")   

   # init run,execute,process methods
   env.AddMethod(exec_cmds, "ExecCmds")
   env.AddMethod(run_cmds, "RunCmds")

   return None

#===============================================================================
# add_items_to_image
# Add libraries to image environment depending on the image name given and
# the image name(s) defined in the environment
#===============================================================================
def add_items_to_image(env, dst_list, targets, items):
   #import pdb; pdb.set_trace()

   if type(targets) is not list:
      targets = [targets]

   if (type(items) is not list) and (type(items) is not SCons.Node.NodeList):
      items = [items]

   intersection = set(targets) & set(env.Dictionary())
   if len(intersection) > 0:
      item_list = env.Flatten(items)
      dst_list.extend(item_list)
      for item in item_list:
         env.PrintDebugInfo('bindist', "Adding file using deprecated api: " +
                            os.path.join(os.path.relpath(os.curdir, env.subst('${BUILD_ROOT}')), str(item)))
   
   return None

def add_nodes_to_image(env, dst_list, nodes):
   #import pdb; pdb.set_trace()

   if (type(nodes) is not list) and (type(nodes) is not SCons.Node.NodeList):
      nodes = [nodes]

   node_list = env.Flatten(nodes)
   dst_list.extend(node_list)
   
   return None
   
def add_libs_to_image(env, targets, libs):
   #import pdb; pdb.set_trace()
   root_env = env.get('ROOT_ENV')
   
   # get list container for current AU or global 
   # if an AU container doesn't exist
   lib_list = root_env.get('AU_LIBS')
   if lib_list is None:
      lib_list = root_env.get('LIBS')

   if 'USES_BINARY_DIST' in env:
      if type(libs) is not list:
         libs = [libs]
      for lib in libs:
         env.PrintDebugInfo('bindist', "Ignoring libraries added using deprecated API: " +
                            os.path.join(os.path.relpath(os.curdir, env.subst('${BUILD_ROOT}')), str(lib)))
   else:
      add_items_to_image(env, lib_list, targets, libs)
      
   return None

def add_objs_to_image(env, targets, objs):   
   #import pdb; pdb.set_trace()
   root_env = env.get('ROOT_ENV')
   
   # get list container for current AU or global 
   # if an AU container doesn't exist
   obj_list = root_env.get('AU_OBJS')
   if obj_list is None:
      obj_list = root_env.get('OBJS')
   
   if 'USES_BINARY_DIST' in env:
      for obj in objs:
         env.PrintDebugInfo('bindist', "Ignoring objects added using deprecated API: " +
                            os.path.join(os.path.relpath(os.curdir, env.subst('${BUILD_ROOT}')), str(obj)))
   else:
      add_items_to_image(env, obj_list, targets, objs)

   return None

def add_docs_to_image(env, targets, docs):   
   #import pdb; pdb.set_trace()
   root_env = env.get('ROOT_ENV')
   
   # get list container for current AU or global 
   # if an AU container doesn't exist
   doc_list = root_env.get('AU_DOCS')
   if doc_list is None:
      env.PrintError("NO AU_DOCS in root env..")
      raise RuntimeError, 'Invalid AU_DOCS variable'
   
   add_items_to_image(env, doc_list, targets, docs)
   return None

#-------------------------------------------------------------------------------
# is Key or Target Enable
#-------------------------------------------------------------------------------
def is_key_enable (env, keys):

   tgt_enable = False
   
   if type(keys) is not list:
      keys = [keys]

   # check that at least one of the targets will be build
   intersection = set(keys) & set(env.Dictionary())
   if len(intersection) > 0:
      tgt_enable = True

   return tgt_enable

#-------------------------------------------------------------------------------
# convert a list of source files with end with any of CPPSUFFIXES 
# to a list of obj
#-------------------------------------------------------------------------------
def convert_src_to_obj_list (env, list):
   cppsuffixes = env.get('CPPSUFFIXES')

   if cppsuffixes is not None:
      re_pattern_cppsuffixes = "|".join([re.escape(x) for x in cppsuffixes])
      re_pattern_cppsuffixes = re_pattern_cppsuffixes.replace(".", "")      
      re_pattern_cppsuffixes = "^(.*)\.(%s)$" % re_pattern_cppsuffixes
      re_replace_cppsuffix = r"\1" + env.subst("${OBJSUFFIX}")
      
      list_obj = [re.sub(re_pattern_cppsuffixes, re_replace_cppsuffix, str(source)) for source in list]
   else:
      list_obj = list
      
   return list_obj
   
#-------------------------------------------------------------------------------
# check if item (library/object) is an exception to binary distribution
#-------------------------------------------------------------------------------
def is_binary_exception (env, library):
   exceptions = env['BINEXCEPT_OPT']
   if exceptions:
      builddir = os.path.relpath(os.curdir, env.subst('${BUILD_ROOT}')).replace(os.sep, '/')
      for exc in exceptions:
         if builddir.startswith(exc):
            return True
   return False
   
#-------------------------------------------------------------------------------
# Add File wrapper for building env.File (optimizations)
#-------------------------------------------------------------------------------
def add_file (env, targets, sources, use_file_list="LIBS", **args):

   file_nodes = None

   if is_key_enable(env, targets):   
   
      if type(sources) is not list:
         sources = [sources]
         
      sources = env.Flatten(sources) 
   
      file_nodes = [env.File(env.RealPath(str(src))) for src in sources]

      # now actually add lib
      root_env = env.get('ROOT_ENV')

      # get list container for current AU or global 
      # if an AU container dosen't exist
      file_list = root_env.get("AU_" + use_file_list)
      
      if file_list is None:
         file_list = root_env.get("IMAGE_" + use_file_list)
         
         if file_list is None:      
            file_list = root_env.get(use_file_list)
      
      add_nodes_to_image(env, file_list, file_nodes)

   return file_nodes

#-------------------------------------------------------------------------------
# Add Library wrapper for building env.Library (optimizations)
#-------------------------------------------------------------------------------
def add_library (env, targets, library, sources, pack=False, add_to_list=True, **args):

   library_node = None
   
   if is_key_enable(env, targets):
   
      if type(sources) is not list:
         sources = [sources]

      sources = env.Flatten(sources)

      if 'USES_BINARY_DIST' in env:
         sconsdir = os.path.relpath(os.curdir, env['BUILD_ROOT']).replace(os.sep, '/')
         if pack:
            library = str(library).replace("${BUILDPATH}", "${BINARY_BUILDPATH}")
         if is_binary_exception(env, sconsdir + '/' + library):
            # normal case, regular library
            env.PrintDebugInfo('bindist', "Binary dist exception for: " + sconsdir + '/' + library)
            library_node = env.Library(library, sources)
         if not library_node:
            # no exception, so use binary distribution
            # check if libsuffix is present
            if not library.endswith(env.subst("${LIBSUFFIX}")):
               library += "${LIBSUFFIX}"
            env.PrintDebugInfo('bindist', "Binary dist for: " + sconsdir + '/' + library)
            library_node = env.File(env.subst(library))
         
      elif pack:
         # make sure to use binary buildpath for binary deliverables,
         # OEM can change their build path, we need to use what was
         # originally used to build the binary node.
         library = str(library).replace("${BUILDPATH}", "${BINARY_BUILDPATH}")
         
         cleanpack = env.GetOption('cleanpack')
         
         pack_exception = is_key_enable(env, args.get('PACK_EXCEPTION'))
         
         if (('USES_NO_STRIP_NO_ODM' in env) and ('USES_CLEAN_PACK' not in env) and not cleanpack) or pack_exception:
            library_node = env.Library(library, sources)
         elif cleanpack:
            library_node = env.Library(library+"_cleanpack", sources)         
         else:
            library_node = env.File(env.subst(library+"${LIBSUFFIX}"))

         if cleanpack and not pack_exception:
            if (type(library_node) is list) or (type(library_node) is SCons.Node.NodeList):
               library_node = library_node[0]
               
            no_exec = env.GetOption('no_exec')
            
            if no_exec or env.PathExists(library+"${LIBSUFFIX}"):
               env.PrintInfo("CleanPack binary library [%s]" % library_node.path)
               for src in sources:
                  node = env.RealPath(str(env.GetBuildPath(src)))
                  env.PrintInfo("CleanPack for library [%s] will remove file [%s]" % (library_node.name, node))
               
                  if not no_exec:                  
                     # make sure files are writeable so they can be removed
                     if env.PathExists(node):
                        os.chmod(node, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)

                     # remove file
                     #os.unlink(env.RealPath(str(node)))
                     # cannot remove file here since SCons may have this files open for 
                     # dependency analysts resulting in OS errors as cannot delete file 
                     # is being used by another program.

                  # add to SCons clena list
                  env.Clean(library_node, node)
            else:
               env.PrintError ("-------------------------------------------------------------------------------")
               env.PrintError ("Required binary library missing -->")
               env.PrintError ("  [%s]" % env.RealPath(library+"${LIBSUFFIX}"))
               env.PrintError ("Image name [%s]" % env.get('IAMGE_NAME', "Unknown"))
               env.PrintError ("Lib name from script [%s]" % env.subst(library))
               env.PrintError ("CleanPack processing will not be performed")
               env.PrintError ("-------------------------------------------------------------------------------")
               
               # Set an error level that can be checked and exited on later after all add_library calls are done
               # check keep-on-going flag and exit only if not set
               Exit(1)
               
      else:
         # normal case, regular library
         library_node = env.Library(library, sources)

      # add node(s) to appropriate list (bucket)
      if add_to_list:
         # now actually add lib
         root_env = env.get('ROOT_ENV')

         # get list container for current AU or global 
         # if an AU container doesn't exist
         lib_list = root_env.get('AU_LIBS')
         
         if lib_list is None:
            lib_list = root_env.get('IMAGE_LIBS')
            
            if lib_list is None:      
               lib_list = root_env.get('LIBS')
         
         add_nodes_to_image(env, lib_list, library_node)

   return library_node
   
def add_bin_library (env, targets, library, sources, pack_exception=None, **kargs):
   return add_library (env, targets, library, sources, pack=True, PACK_EXCEPTION=pack_exception, **kargs)

def add_oem_library (env, targets, library, **kargs):
   return add_file (env, targets, library, use_file_list="LIBS", **kargs)

#-------------------------------------------------------------------------------
# Add Object wrapper for building env.Object (optimizations)
#-------------------------------------------------------------------------------
def add_object (env, targets, sources, pack=False, add_to_list=True, **args):

   object_nodes = []
   
   if is_key_enable(env, targets):
   
      if type(sources) is not list:
         sources = [sources]
         
      sources = env.Flatten(sources)
   
      if 'USES_BINARY_DIST' in env:
         sconsdir = os.path.relpath(os.curdir, env['BUILD_ROOT']).replace(os.sep, '/')
         if pack:
            sources = [str(source).replace("${BUILDPATH}", "${BINARY_BUILDPATH}") for source in sources]
         sources_obj = convert_src_to_obj_list(env, sources)
         for obj in sources_obj:
            if is_binary_exception(env, sconsdir + '/' + obj):
               # normal case, regular object
               env.PrintDebugInfo('bindist', "Binary dist exception for: " + sconsdir + '/' + obj)
               object_nodes.append(env.Object(obj))
            else:
               # no exception, so use binary distribution
               # check if libsuffix is present
               if not obj.endswith(env.subst("${OBJSUFFIX}")):
                  env.PrintError("Object doesn't end in correct suffix: " + obj)
               env.PrintDebugInfo('bindist', "Binary dist for: " + sconsdir + '/' + obj)
               object_nodes.append(env.File(env.subst(obj)))

      elif pack:
         # make sure to use binary buildpath for binary deliverables,
         # OEM can change their build path, we need to use what was
         # originally used to build the binary node.
         sources = [str(source).replace("${BUILDPATH}", "${BINARY_BUILDPATH}") for source in sources]

         cleanpack = env.GetOption('cleanpack')
         
         pack_exception = is_key_enable(env, args.get('PACK_EXCEPTION'))
         
         if (('USES_NO_STRIP_NO_ODM' in env) and ('USES_CLEAN_PACK' not in env) and not cleanpack) or pack_exception:
            object_nodes = env.Object(sources)
         elif cleanpack:
            cp_sources = [str(source).replace("BUILDPATH}", "BUILDPATH}/cleanpack") for source in sources]
            object_nodes = env.Object(cp_sources)
         else:
            sources_obj = convert_src_to_obj_list(env, sources)
            object_nodes = [env.File(env.RealPath(obj)) for obj in sources_obj]
            
         if cleanpack and not pack_exception:         
            no_exec = env.GetOption('no_exec')
            
            if not no_exec:
               # check binary objects exit before we delete the sources
               sources_obj = convert_src_to_obj_list(env, sources)
               
               for obj in sources_obj:
                  if not env.PathExists(str(obj)):
                     env.PrintError ("-------------------------------------------------------------------------------")
                     env.PrintError ("Required binary object missing -->")
                     env.PrintError ("  [%s]" % env.RealPath(str(obj)))
                     env.PrintError ("Image name [%s]" % env.get('IAMGE_NAME', "Unknown"))
                     env.PrintError ("Object name from script [%s]" % env.subst(str(obj)))
                     env.PrintError ("CleanPack processing will not be performed")
                     env.PrintError ("-------------------------------------------------------------------------------")
                  
                     # Set an error level that can be checked and exited on later after all add_library calls are done
                     # check keep-on-going flag and exit only if not set
                     Exit(1)

            for src in sources:
               node = env.RealPath(str(env.GetBuildPath(src)))
               env.PrintInfo("CleanPack will remove file [%s]" % node)
            
               if not no_exec:
                  # make sure files are writeable so they can be removed
                  if env.PathExists(node):
                     os.chmod(node, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)

                  # remove file
                  #os.unlink(env.RealPath(str(node)))
                  # cannot remove file here since SCons may have this files open for 
                  # dependency analysts resulting in OS errors as cannot delete file 
                  # is being used by another program.

               # add to SCons clena list
               env.Clean(object_nodes, node)
      else:
         # normal case, regular object
         object_nodes = env.Object(sources)

      # add node(s) to appropriate list (bucket)
      if add_to_list:
         # now actually add lib
         root_env = env.get('ROOT_ENV')

         # get list container for current AU or global 
         # if an AU container doesn't exist
         obj_list = root_env.get('AU_OBJS')
         
         if obj_list is None:
            obj_list = root_env.get('IMAGE_OBJS')
            
            if obj_list is None:      
               obj_list = root_env.get('OBJS')
         
         add_nodes_to_image(env, obj_list, object_nodes)

   return object_nodes

def add_bin_object (env, targets, sources, pack_exception=None, **kargs):
   return add_object (env, targets, sources, pack=True, PACK_EXCEPTION=pack_exception, **kargs)
   
def add_oem_object (env, targets, object, **kargs):
   return add_file (env, targets, object, use_file_list="OBJS", **kargs)
   
#-------------------------------------------------------------------------------
# Add Document wrapper for building env.Document
#-------------------------------------------------------------------------------
def add_document (env, targets, document, sources, **args):

   document_nodes = None
   
   if is_key_enable(env, targets):
      document_nodes = env.PdfDocument(document, sources)

      # now actually add lib
      root_env = env.get('ROOT_ENV')

      # get list container for current AU or global 
      # if an AU container doesn't exist
      doc_list = root_env.get('AU_DOCS')
      
      if doc_list is None:
         doc_list = root_env.get('IMAGE_DOCS')
         
         if doc_list is None:
            env.PrintError("NO AU_DOCS or IMAGE_DOCS in root env..")
            raise RuntimeError, 'Invalid or missing AU_DOCS, IMAGE_DOCS variables'
      
      add_nodes_to_image(env, doc_list, document_nodes)

   return document_nodes
   
#-------------------------------------------------------------------------------
# Add list files (link nodes listed in a text file) pass the linker 
# for example ARM uses --via.
#-------------------------------------------------------------------------------
def add_listfile (env, targets, sources, **args):

   listfile_node = None
   
   if is_key_enable(env, targets):

      if type(sources) is not list:
         sources = [sources]

      listfile_node = env.Flatten(sources)
         
      # now actually add lib
      root_env = env.get('ROOT_ENV')

      # get list container for current AU or global 
      # if an AU container doesn't exist
      listfile_list = root_env.get('AU_LISTFILES')
      
      if listfile_list is None:
         listfile_list = root_env.get('IMAGE_LISTFILES')
         
         if listfile_list is None:
            env.PrintError("NO AU_LISTFILES or IMAGE_LISTFILES in root env..")
            raise RuntimeError, 'Invalid or missing AU_LISTFILES, IMAGE_LISTFILES variables'
      
      add_nodes_to_image(env, listfile_list, listfile_node)
      
      # list files are special case since we cannot added directly as a 
      # dependency of the Program. For a correct Solution we might have 
      # to add our own AddProgram Wrapper. In the mean time add an alias
      # form the aliases to this node.
      #aliases = env.get('IMAGE_ALIASES')
      #if aliases is not None:
      #   [env.Alias(alias, node) for alias in aliases for node in listfile_node]

   return listfile_node

#-------------------------------------------------------------------------------
# Add Artefact (Non-Link Node) for example scripts, etc.
#-------------------------------------------------------------------------------
def add_artifact (env, targets, sources, **args):
   
   artifact_node = None
   
   if is_key_enable(env, targets):

      if type(sources) is not list:
         sources = [sources]

      artifact_node = env.Flatten(sources)
   
      # now actually add lib
      root_env = env.get('ROOT_ENV')

      # get list container for current AU or global 
      # if an AU container dosen't exist
      artifacts_list = root_env.get('AU_ARTIFACTS')
      
      if artifacts_list is None:
         artifacts_list = root_env.get('IMAGE_ARTIFACTS')
         
         if artifacts_list is None:
            env.PrintError("NO AU_ARTIFACTS or IMAGE_ARTIFACTS in root env..")
            raise RuntimeError, 'Invalid or missing AU_ARTIFACTS, IMAGE_ARTIFACTS variables'
      
      add_nodes_to_image(env, artifacts_list, artifact_node)

      # artifacts are special case since we cannot added directly as a 
      # dependency of the Program. For a correct Solution we might have 
      # to add our own AddProgram Wrapper. In the mean time add an alias
      # form the aliases to this node.
      aliases = env.get('IMAGE_ALIASES')
      if aliases is not None:
         [env.Alias(alias, node) for alias in aliases for node in artifact_node]

   return artifact_node
   
#-------------------------------------------------------------------------------
# clean_pack - Remove the pack files during the clean process
#-------------------------------------------------------------------------------
def clean_pack (env, targets, sources, **args):
   if is_key_enable(env, targets):
      cleanpack = env.GetOption('cleanpack')  
      no_exec = env.GetOption('no_exec')      
      
      pack_exception = (is_key_enable(env, args.get('PACK_EXCEPTION')) or
                        is_key_enable(env, args.get('pack_exception')))
      
      if cleanpack and not pack_exception:
      
         if type(sources) is not list:
            sources = [sources]
            
         # create a dummy builder just for this clean action
         env_cleanpack = env.Clone()
         
         def cleanpack_fake_action(target, source, env):
         # fake function to use command builder
            return None
         
         # use like         
         #env_cleanpack.Command('foo.out', 'foo.in', cleanpack_fake_action)
           
         aliases = env_cleanpack.get('IMAGE_ALIASES')

         for src in sources:
            node = env.RealPath(str(env_cleanpack.GetBuildPath(src)))
            env_cleanpack.PrintInfo("CleanPack will remove file [%s]" % node)
             
            if not no_exec:                                       
               # make sure files are writeable so they can be removed
               if env_cleanpack.PathExists(node):
                  os.chmod(node, stat.S_IRWXU | stat.S_IRWXG | stat.S_IRWXO)

               # remove file
               #if not no_exec:                  
               #   os.unlink(env_cleanpack.RealPath(str(node)))
               # cannot remove file here since SCons may have this files open for 
               # dependency analysts resulting in OS errors as cannot delete file 
               # is being used by another program.
               
            # add to SCons clean list
            cp_node = env_cleanpack.Command(node, None, cleanpack_fake_action)
            
            # cleanpack nodes are special case since we cannot directly connect 
            # to a target for now aliases this node to current aliases
            if aliases is not None:
               [env_cleanpack.Alias(alias, cp_node) for alias in aliases]
               

   return None
  
#-------------------------------------------------------------------------------
# Add Program (Link into exe, elf, etc).
#-------------------------------------------------------------------------------
def add_program (env, target=None, source=None, **kargs):
   env.PrintError('')
   env.PrintError("-------------------------------------------------------------------------------")
   env.PrintError("This version of AddProgram is deprecated, please contact the SCons arch team")
   env.PrintError("for further assistance. The tool sets should define the AddProgram method.") 
   env.PrintError("-------------------------------------------------------------------------------")
   env.PrintError('')
   Exit(1)

#===============================================================================
# check_alias
# Check if script should be loaded depending on alias, also used to list
# the target name and aliases
#===============================================================================
def check_alias(env, image_name="${IMAGE_NAME}", alias_list=None, always_true=False):
   # init vars
   alias_math = False
   cmdline_targets = map(str, COMMAND_LINE_TARGETS)

   listtargets = env.GetOption('listtargets')
   listapis = env.GetOption('listapis')
   help = env.GetOption('help')

   if alias_list is None:
      alias_list = env.get('IMAGE_ALIASES')
   elif type(alias_list) is not list:
      alias_list = [alias_list]

   if listtargets or listapis or help:
   # if help - print short image info (also overide all others)
   # if listtargets - print detail info per image
   # if listapis - do nothing
      if help:
         print string.ljust(env.subst(image_name + " ${PROC} "), 22) + str(alias_list)
      elif listtargets:
         if len(cmdline_targets) == 0 or len(set(cmdline_targets) & set(alias_list)) > 0:
            wrapper = textwrap.TextWrapper()
            wrapper.width = 80
            wrapper.subsequent_indent = "     "
         
            print " "
            print "IMAGE: %s" % env.subst(image_name)
            
            # proc info
            wrapper.initial_indent = "  proc:        "
            for print_line in wrapper.wrap(env.subst("${PROC}")):
               print print_line

            wrapper.initial_indent = "  proc config: "
            for print_line in wrapper.wrap(env.subst("${PROC_CONFIG}")):
               print print_line
            
            # print aliases
            wrapper.initial_indent = "  aliases:     "
            for print_line in wrapper.wrap(env.subst("${IMAGE_ALIASES}").replace(" ",", ")):
               print print_line

            # print build tags
            wrapper.initial_indent = "  build tags:  "
            for print_line in wrapper.wrap(env.subst("${BUILD_TAGS}").replace(" ",", ")):
               print print_line
               
            # print deprecated build tags
            wrapper.initial_indent = "  deprecated build tags: "
            for print_line in wrapper.wrap(env.subst("${DEPRECATED_BUILD_TAGS}").replace(" ",", ")):
               print print_line
               
            # print image path location
            wrapper.initial_indent = "  image path:  "
            path_from_root = "".join(["${BUILD_ROOT}", env.RealPath(inspect.stack()[2][1], posix=True).replace(env.RealPath('${BUILD_ROOT}', posix=True),"")])
            for print_line in wrapper.wrap(path_from_root):
               print print_line
            
      return alias_math

   if len(cmdline_targets) > 0:
      for target in cmdline_targets:
         if ('\\' in target) or ('/' in target):
            if str(target).find(alias_list[0]) != -1:
               alias_math = True
               break;
         elif target in alias_list:
            alias_math = True
            break
      
   if always_true:
      filter_opt = env.get('FILTER_OPT')
      if filter_opt is not None and not alias_math:
         alias_math = False
      else:
         alias_math = True

   # last but not least print image banner (only if alias matches).
   if alias_math:
      env.PrintImageBanner(image_name)
      
   return alias_math

#===============================================================================
# bind_aliases_to_targets
# utility to bind image targets to image aliases
#===============================================================================
def bind_aliases_to_targets(env, target, alias_list=None):
   if alias_list is None:
      alias_list = env.get('IMAGE_ALIASES')
   elif type(alias_list) is not list:
      alias_list = [alias_list]

   for alias in alias_list:
      env.Alias(alias, target)

#===============================================================================
# print_stage_banner
# print standard stage
#===============================================================================
def print_stage_banner(env, stage_name):
   print "\n"
   print "------------------------------------------------------------------------------"
   print "   %s" % str(stage_name)
   print "------------------------------------------------------------------------------"

#===============================================================================
# print_image_banner
# print standard image banner
#===============================================================================
def print_image_banner(env, image_name="${IMAGE_NAME}"):
   print string.ljust(env.subst(image_name + " ${PROC}"), 22) + \
      env.subst("BUILD_ID=${BUILD_ID}, TOOL_ENV=${PLATFORM}")
      
#===============================================================================
# print_error
# print standard error message
#===============================================================================
def print_error(env, message):
   print '!! ERROR: ' + env.subst(str(message))

#===============================================================================
# print_warning
# print standard warning message
#===============================================================================
def print_warning(env, message, width=80, raw=True):
   #wrapper = textwrap.TextWrapper()
   #wrapper.width = width
   #wrapper.initial_indent =    "!! WARNING: "
   #wrapper.subsequent_indent = "   "
   #for print_line in wrapper.wrap(env.subst(str(message))):
   #   print print_line
   
   if not env.get('DISABLE_BUILD_WARNINGS', False):
      print '!! WARNING: ' + env.subst(str(message), raw)

#===============================================================================
# print_info
# print standard info message
#===============================================================================
def print_info(env, message, raw=True):
   print '** INFO: ' + env.subst(str(message), raw)

#===============================================================================
# print_debug_info
# print standard debug info message, depeding on debug info type
#===============================================================================
def print_debug_info(env, debuginfo_type, message, raw=True):
   debuginfo_type = str(debuginfo_type)
   debuginfo_opt = env.get('DEBUGINFO_OPT')

   if debuginfo_type in debuginfo_opt:
      print '** DBGINF: ' + env.subst(str(message), raw)

#===============================================================================
# print_data
# print standard info message
#===============================================================================
def print_data(env, message, raw=True):
   print env.subst(str(message), raw)

#===============================================================================
# create_file_banner
# creates a string that can be use as a banner for auto generated files
#===============================================================================
def create_file_banner(env, fname, description="None", start_comment="#", 
   end_comment="", start_block="", end_block="", style='none'):
   banner_str = \
'''$SB$SCM============================================================================$ECM
$SCM  Name:                                                                     $ECM
$SCM    $FILE_NAME $ECM
$SCM
$SCM  Description:                                                              $ECM
$SCM    $DESCRIPTION $ECM
$SCM                                                                            $ECM
$SCM Copyright (c) $YEAR by QUALCOMM, Incorporated.  All Rights Reserved.        $ECM
$SCM============================================================================$ECM
$SCM                                                                            $ECM
$SCM *** AUTO GENERATED FILE - DO NOT EDIT                                      $ECM
$SCM                                                                            $ECM
$SCM GENERATED: $DATE $ECM
$SCM============================================================================$ECM$EB
'''   
   if style == 'C':
      start_comment = "#"
      end_comment = ""
      start_block = "/*\n"
      end_block = "\n*/"
   
   elif style == 'C++':
      start_comment = "//"
      end_comment = ""
      start_block = ""
      end_block = ""
      
   elif style == 'asm':
      start_comment = ";"
      end_comment = ""
      start_block = ""
      end_block = ""
    
   elif style == 'make' or style == 'shell':
      start_comment = "#"
      end_comment = ""
      start_block = ""
      end_block = ""
   
   elif style == 'dos':
      start_comment = "REM "
      end_comment = ""
      start_block = ""
      end_block = ""
      
   banner_str = banner_str.replace('$SCM', start_comment)
   banner_str = banner_str.replace('$ECM', end_comment)
   banner_str = banner_str.replace('$SB', start_block)
   banner_str = banner_str.replace('$EB', end_block)
   banner_str = banner_str.replace('$YEAR', str(datetime.now().strftime('%Y')))
   banner_str = banner_str.replace('$DATE', str(datetime.now().ctime()))
   banner_str = banner_str.replace('$FILE_NAME', fname)
   banner_str = banner_str.replace('$DESCRIPTION', description)

   return banner_str

   
#===============================================================================
# real_path
#===============================================================================
def real_path(env, paths, posix=False):
   env_subst = env.subst
   ospath_realpath = os.path.realpath

   if env.get('PLATFORM', 'none') == 'cygwin':
      posix = True

   if type(paths) is list:
      paths = [env_subst(path) for path in paths]
      paths = [ospath_realpath(path) for path in paths]
      
      if posix:
         paths = [path.replace('\\', '/') for path in paths]
   else:
      paths = env_subst(paths)
      paths = ospath_realpath(paths)
      
      if posix:
         paths = paths.replace('\\', '/')

   return paths

#===============================================================================
# norm_path
#===============================================================================
def norm_path(env, paths, posix=False):
   env_subst = env.subst
   ospath_normpath = os.path.normpath

   if env.get('PLATFORM', 'none') == 'cygwin':
      posix = True

   if type(paths) is list:
      paths = [env_subst(path) for path in paths]
      paths = [ospath_normpath(path) for path in paths]
      
      if posix:
         paths = [path.replace('\\', '/') for path in paths]
   else:
      paths = env_subst(paths)
      paths = ospath_normpath(paths)
      
      if posix:
         paths = paths.replace('\\', '/')

   return paths

#===============================================================================
# path_exists
# Substitue envirometn variables and check if path exists
#===============================================================================
def path_exists(env, path):
   path = env.subst(path)
   return os.path.exists(path)

#===============================================================================
# path_to_dict_tree
# Converst a list of paths into a dictionary used as a tree data structure. 
# leafs point to empty dictionaries
#===============================================================================
def path_to_dict_tree(env, path_list, path_sep='/'):
   dict_tree = dict()

   if type(path_list) is not list:
      path_list = [path_list]

   # iterate filters and decompose each filter into its parts 
   for path in path_list:
      path_parts = path.split(path_sep)
      
      # now create tree structure from the filter parts
      current_dict = dict_tree
      for part in path_parts:
         dict_key = part
         sub_dict = current_dict.get(dict_key)
         
         # if key is not present create new entry
         if sub_dict is None:
            current_dict[dict_key] = dict()
            sub_dict = current_dict.get(dict_key)

         current_dict = sub_dict

   return dict_tree
#===============================================================================   
# filter_match
#
#===============================================================================
def filter_match(env, path=""):
   found_match = False
   filter_opt = env.get('FILTER_OPT')
   
   if filter_opt is not None and len(path) > 0:
   # for the most part filters are part of the path after ${BUILD_ROOT}
   # but since this is configurable it is not guranteed. So for speed and 
   # sanity purposes, the only job is to see if we can find a filter within
   # the provided path. Filter should be posix like so make sure our path is 
   # also posix like
      path = env.NormPath(path, posix=True)
      for filter in filter_opt:
         if path.find(filter) != -1:
            found_match = True
            break
   
   return found_match


#===============================================================================
# DeleteAction / get_delete_action
# return an action that can be used to delete files when a builder call it's
# actions, also follows the general text formatting rules
#===============================================================================
class DeleteAction:
   def __init__(self, file):
      self.file = file
      self.verbose = int(GetOption('verbose'))

   def delete(self, target, source, env):
      file = self.file

      if '${TARGET}' in self.file:
         file = file.replace('${TARGET}', target[0].path)
      elif '$TARGET' in self.file:
         file = file.replace('$TARGET', target[0].path)

      #import pdb; pdb.set_trace()

      if self.file is not None:
         if os.path.exists(file):
            os.unlink(file)
         else:
            print "*** Can't delete file: " + file

      return 0

def get_delete_action(env, file):
   verbose = int(GetOption('verbose'))

   if verbose >= 2:
      verbose_str = "<== Deleting \nFile: "+ file +"\n==>"
   elif verbose == 1:
      verbose_str = "=== Deleting   "+ file
   else:
      verbose_str = "=== Deleting   ${BUILDPATH}/"+ file.replace('TARGET', 'TARGET.name')

   da = DeleteAction(file)
   delete_act = SCons.Action.Action(da.delete, verbose_str)

   return delete_act

#===============================================================================
# Run CMDS
# Runs commands passed in from builders, and respects the verbose levels.
# (deprecated, use ExecCmds, this functions take parameters that are not used
# bt must be passed in)
#===============================================================================
def run_cmds(env, target, source, cmds, dir=None, silent=None):
   if type(target) is list:
      target=str(target[0])
   return exec_cmds (env, cmds, dir, silent, target)

#===============================================================================
# Exec CMDS
# Executes/Runs comdas passed in from builders, and respects the verbose levels.
# it returns a tuple (stdout, stderr, rv).
#
# Note: because of changes to capture command output on the fly, the stderr 
# will always be an empty string stdout and stderr are now mixed in stdout.
#===============================================================================
def exec_cmds(env, cmds, dir=None, silent=None, target=None, shell=False, buffer=True):
   # call cmds
   #silent = GetOption('silent')
   if type(target) is list:
      target=str(target[0])
   
   verbose = int(GetOption('verbose'))
   all_data = ''
   all_err = ''
   all_rv = 0
   org_cwd = None
   cmd_env = None
   use_shell = shell
   stdout_data = ''
   
   # clean command environment, it must contain only strings otherwise popen will fail
   if env.has_key('ENV'):
      org_env = env['ENV']
      cmd_env = {}
      for key in org_env.keys():
         cmd_env[key] = str(org_env[key])

   if verbose > 1:
      if target == None:
         target_str = ""
      else:
         target_str = ", Creating [" + target + "]"

   # calculate steps, and make sure cmds is a list other wise make it a list
   current_step = 1;
   if type(cmds) is not list:
      cmds = [cmds]

   total_steps = str(len(cmds))

   # if dir is not None, change directory to dir
   if dir is not None:
      dir = env.NormPath(dir)

      if verbose == 3:
         print "cd " + dir
      elif verbose == 2:
         print "== Change Directory: " + dir + " =="

      org_cwd = os.getcwd()
   else:
      dir = os.getcwd()
      
   # go trough list of commands !!cmds must be a list and not an single string
   for cmd in cmds:
      cmd = env.subst(cmd)
      
      if verbose == 3:
         print cmd
      elif verbose == 2:
         print '<== Step '+ str(current_step) +' of '+ total_steps + target_str +' \nCommand:\n'+ cmd +'\n==>'

      startupinfo = None
      if os.name == 'nt':
         startupinfo = subprocess.STARTUPINFO()
      else:
         use_shell = True

      # check for shell operations
      if re.search(r"(<|>|&&|\|)", cmd):
         use_shell = True
         
      try:
         #pathdirs = os.environ["PATH"]
         proc = subprocess.Popen(
            cmd,
            #stdin=PIPE,                     # don't take imput away from tools
            stdout=subprocess.PIPE,
            #stderr=subprocess.PIPE,
            stderr=subprocess.STDOUT,        # need std<out & err> combined
            startupinfo=startupinfo,
            shell = use_shell,
            cwd = dir
            #env=cmd_env
            #env={"PATH":pathdirs}
         )
         
         stdout_line = proc.stdout.readline()
         
         if buffer:
            stdout_data = stdout_line
         
         while(stdout_line):
            # Print the line out (remove trailing whitespace)
            if not silent:
               print stdout_line.rstrip()
            
            # Read the next line
            stdout_line = proc.stdout.readline()
            
            if buffer:
               stdout_data = "".join([stdout_data, stdout_line])

         # Wait for the process to exit and get exit code
         rv = proc.wait()
         #import pdb;pdb.set_trace()

      except:
         env.PrintError("-------------------------------------------------------------------------------")
         env.PrintError("*** Popen exception.")
         env.PrintError("*** CMD:%s" % cmd)
         env.PrintError("*** OS Name=%s" % os.name)
         env.PrintError("*** startupinfo = %s" % str(startupinfo))
         env.PrintError("*** sys exec info:")
         env.PrintError("%s" % str(sys.exc_info()))
         env.PrintError("-------------------------------------------------------------------------------")
         raise RuntimeError, 'Exception from subprocess.Popen'

      current_step += 1
      
      if len(stdout_data) > 0:
         all_data = "".join([all_data, stdout_data])

   if org_cwd is not None:
      if verbose == 3:
         print "cd " + org_cwd
      elif verbose == 2:
         print "== Change Directory: " + org_cwd + " =="

   return [all_data, all_err, rv]

#===============================================================================
# load_scons_scripts
#
#===============================================================================
def load_scons_scripts(env, scripts):
   # init input vars
   if type(scripts) is not list:
      scripts = [scripts]

   for script in scripts:
      script = env.subst(script)
      env.PrintDebugInfo('scripts', "Loading script: " + env.RealPath(script))
      env.SConscript(script, exports='env')
      
   return None

#===============================================================================
# load_tool_script
#
#===============================================================================
def load_tool_script(env, tool, toolpath=[]):
   
   (tool_base, tool_ext) = os.path.splitext(tool)
   
   if env.PathExists(tool) and tool_ext.lower() == ".py":
      (toolpath, tool) = os.path.split(tool_base)
      toolpath = [env.NormPath(toolpath)]
   else:
      if type(toolpath) is not list:
         toolpath = [toolpath]

      toolpath = [env.NormPath(dir) for dir in toolpath]
   
   m_fp, m_path, m_desc = imp.find_module(tool, toolpath)
   
   if m_fp is None:
      env.PrintWarning("Tool ["+ tool +"] not found in "+ str(toolpath))
   else:   
      env.PrintDebugInfo('scripts', "Loading tool script: " + env.RealPath(m_path))
      try:
        m = imp.load_module(tool, m_fp, m_path, m_desc)
      finally:
        # Since we may exit via an exception, close fp explicitly.
        if m_fp:
            m_fp.close()
            
      # module loaded, now generate
      m.exists(env)
      m.generate(env)
      
   return None
   
#===============================================================================
# load_image_units
#
#===============================================================================
def load_image_units(env, dir_patterns=None):
   #import pdb; pdb.set_trace()
   if dir_patterns is None:
      dir_patterns = env.get('IMAGE_DIR_PATTERNS')

   if type(dir_patterns) is not list:
      dir_patterns = [dir_patterns]
      
   script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')
   
   load_sub_scripts(env, dir_patterns, script_name_patterns, "image", clone_env=True)
   
   return None
   
#===============================================================================
# load_api_units
#
#===============================================================================
def load_api_units(env, dir_patterns=None):
   #import pdb; pdb.set_trace()
   if dir_patterns is None:
      dir_patterns = env.get('SCRIPT_DIR_PATTERNS')
      
   if type(dir_patterns) is not list:
      dir_patterns = [dir_patterns]

   script_name_patterns = env.get('API_SCRIPT_NAME_PATTERNS')
   
   load_sub_scripts(env, dir_patterns, script_name_patterns, "api")
   
   return None
   
#===============================================================================
# load_sw_units
# Level param is not used any longer it will be obsoleated eventually, for the 
# mean time mantain backward compatibility
#===============================================================================
def load_sw_units (env, level=0, dir_patterns=None):
   if dir_patterns is None:
      dir_patterns = env.get('SCRIPT_DIR_PATTERNS')
      
   if type(dir_patterns) is not list:
      dir_patterns = [dir_patterns]

   node = SoftwareUnitsNode(env)
   node.load_sw_units(dir_patterns)   
   return None

#===============================================================================
# get_sw_units
# same bheaivor as load_sw_units, but will create the units buckets as AU does
# (load_au_sw_units) allowing for the user of this to collect the units. And do
# soemthing else before passing them up to the iamge.
#===============================================================================
def get_sw_units (env, dir_patterns=None):
   env = env.Clone()
   env.Replace(ROOT_ENV = env)
   env.Replace(AU_ENV = env)
   env.Replace(AU_LIBS = [])
   env.Replace(AU_OBJS = [])
   env.Replace(AU_DOCS = [])
   env.Replace(AU_LISTFILES = [])
   env.Replace(AU_ARTIFACTS = [])

   load_sw_units(env, dir_patterns=dir_patterns)
   
   # after loading scripts get list of libs and objs
   #import pdb; pdb.set_trace()
   items = dict()
   items['LIBS'] = env.get('AU_LIBS')
   items['OBJS'] = env.get('AU_OBJS')
   items['DOCS'] = env.get('AU_DOCS')
   items['LISTFILES'] = env.get('AU_LISTFILES')
   items['ARTIFACTS'] = env.get('AU_ARTIFACTS')
   items['ENV'] = env.Clone()

   return items

#===============================================================================
# get_au_sw_unit_names
#
#===============================================================================
def get_au_sw_unit_names(env):
   au_root_script_pattern = env.get('AU_ROOT_DIR_PATTERNS')
   script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')

   # compine dir and file patterns
   patterns = ["".join([dir,"/",file]) for dir in au_root_script_pattern for file in script_name_patterns]

   # glob files matching patterns
   au_root_scripts = env.GlobFiles(patterns, posix=True)

   #import pdb; pdb.set_trace()
   # if BUILD_ROOT is not the base pattern for all, this will not work
   # and a multiple matches must be done one for each pattern, most probably
   # replace '*' with '(\w+)' to each au_root_dir_pattern to come up with
   # the matching reg-ex
   au_current_list = []
   au_name_re = re.compile(r'^' + env.subst('${BUILD_ROOT}/') + '(\w+)/')

   for au_root_script in au_root_scripts:
      re_match = au_name_re.match(au_root_script)
      if re_match:
         au_current_list.append(re_match.group(1))
         
   # update global enviroment so other images can use this information
   # and not have to re-calculate
   root_glocal_env = env.get('ROOT_GLOBAL_ENV')
   if root_glocal_env is not None:
      root_glocal_env.Replace(AU_CURRENT_LIST = au_current_list)
      
   return au_current_list

#===============================================================================
# load_au_sw_units
#
# Returns dictionary with data accessed by keys:
#    LIBS, libraries nodes
#    OBJS, objects files nodes
#    DOCS, document files nodes
#    ENV,  AU enviroment.
#
#===============================================================================
def load_au_sw_units (env, au_name, filter_list=[]):
   #import pdb; pdb.set_trace()
   t_start = time.clock()
   
   #env.PrintDebugInfo('time', "Loading %s scripts..." % au_name)
   env.PrintInfo("Loading %s scripts..." % str(au_name).upper())
   
   # init parameter vars
   if type(filter_list) is not list:
      filter_list = [filter_list]
   
   # get current au list (or build it) and check for valid AU
   au_current_list = env.get('AU_CURRENT_LIST')
   script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')
   
   if au_current_list is None:
      au_current_list = env.GetAreaSoftwareUnitNames()
   
   if au_name not in au_current_list:
      env.PrintError("[" + au_name +"] is not a valid AU name.")
      raise RuntimeError, 'Invalid AU given'

   # init enviroment variables
   root_env = env.get('ROOT_ENV')
   env = env.Clone()
   
   if root_env is not None:
      root_env["au_" + au_name + '_env'] = env
   
   env.Replace(ROOT_ENV = env)
   env.Replace(AU_ENV = env)
   env.Replace(AU_NAME = au_name)
   env.Replace(AU_LIBS = [])
   env.Replace(AU_OBJS = [])
   env.Replace(AU_DOCS = [])
   env.Replace(AU_LISTFILES = [])
   env.Replace(AU_ARTIFACTS = [])
   
   # init restricted inc paths
   env.InitAUIncPaths()
   
   #debug filter
   #filter_list=['debugtools/tmc', 'services', 'storage/efs']
   
   # init this AU filter list and default to load au
   au_filter_list = []
   do_load_au = True
   
   # get command line filter options
   cmd_filter_opt = env.get('FILTER_OPT')
   #import pdb; pdb.set_trace()
   if cmd_filter_opt is not None:
      filter_opt_list = cmd_filter_opt
      
      # if there are both command line filters and AU filters 
      # these need to be merged.
      # the rule is AU filters are the only ones we can use as
      # a base to come up with final filter list. 
      #  examples:
      #  cmd filter     AU filter      Final Filter
      #  X/Y/Z          X/Y            X/Y/Z
      #  X              X/Y            X/Y
      #  A/B/C          X/Y            nothing to build
      #  A/B/C          X/Y,A          A/B/C
      #  A/B/C          X/Y,A/D        nothing to build
      if len(filter_list) and do_load_au:
         for au_filter in filter_list:
            au_filter = "".join([au_name, env.get('FILTER_SEP', '/'), au_filter])
            for filter_opt in filter_opt_list:
               if len(filter_opt) >= len(au_filter):
                  if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
                     if string.lower(au_filter) == string.lower(filter_opt[:len(au_filter)]):
                        au_filter_list.append(filter_opt)
                  else:
                     if au_filter == filter_opt[:len(au_filter)]:
                        au_filter_list.append(filter_opt)
               else:
                  if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
                     if string.lower(filter_opt) == string.lower(au_filter[:len(filter_opt)]):
                        au_filter_list.append(au_filter)
                  else:
                     if filter_opt == au_filter[:len(filter_opt)]:
                        au_filter_list.append(au_filter)

      # no AU filters, process only cmd line filters
      else:
         au_filter_list = filter_opt_list

      # use only this au filters.
      if len(au_filter_list):
         au_filters = env.PathToDictTree(au_filter_list, env.get('FILTER_SEP', '/'))
         au_filters = au_filters.get(au_name)

         if au_filters is not None:
            env.Replace(AU_FILTERS = au_filters)
         else:
            do_load_au = False
         
      # filter nuke each other out nothing to build
      else:
         do_load_au = False
      
   # there are no command line filter options      
   else:
      if len(filter_list):
         au_filters = env.PathToDictTree(filter_list, env.get('FILTER_SEP', '/'))
         env.Replace(AU_FILTERS = au_filters)         
      
   # time to load root script
   if do_load_au:
      #import pdb; pdb.set_trace()
      # get au root script
      au_dir_patterns = env.get('AU_DIR_PATTERNS')
      patterns = ["".join([dir,"/",file]) for dir in au_dir_patterns for file in script_name_patterns]
      au_root_script = env.GlobFiles(patterns)[0]
   
      env.PrintDebugInfo('scripts', "Loading " + au_name.upper() + " scripts... ")
      env.PrintDebugInfo('scripts', "Loading script: " + env.RealPath(au_root_script))
      env.SConscript(au_root_script, exports='env')
  
   # after loading scripts get list of libs and objs
   #import pdb; pdb.set_trace()
   items = dict()
   items['LIBS'] = env.get('AU_LIBS')
   items['OBJS'] = env.get('AU_OBJS')
   items['DOCS'] = env.get('AU_DOCS')
   items['LISTFILES'] = env.get('AU_LISTFILES')
   items['ARTIFACTS'] = env.get('AU_ARTIFACTS')
   items['ENV'] = env.Clone()
   
   # add items to IMAGE lists
   image_env = env.get('IMAGE_ENV')
   
   if image_env is not None:
      image_libs = image_env.get('IMAGE_LIBS')
      if image_libs is not None:
         image_libs.extend(env.get('AU_LIBS'))

      image_objs = image_env.get('IMAGE_OBJS')
      if image_libs is not None:   
         image_objs.extend(env.get('AU_OBJS'))

      image_docs = image_env.get('IMAGE_DOCS')
      if image_libs is not None:   
         image_docs.extend(env.get('AU_DOCS'))

      image_listfiles = image_env.get('IMAGE_LISTFILES')
      if image_libs is not None:   
         image_listfiles.extend(env.get('AU_LISTFILES'))

      image_artifacts = image_env.get('IMAGE_ARTIFACTS')
      if image_libs is not None:   
         image_artifacts.extend(env.get('AU_ARTIFACTS'))

   t_end = time.clock()   
   #print "** Load AU Elapsed time = " + str(round(t_end - t_start, 3))
   #env.PrintDebugInfo('time', "Load %s scripts, elapsed time %s" % (au_name, str(round(t_end - t_start, 3))))
   env.PrintInfo("Loaded %s scripts, elapsed time %s sec." % (str(au_name).upper(), str(round(t_end - t_start, 3))))
   
   return items
   
#===============================================================================
# load_sub_units
# This fucntion is intended to load non library scripts such as image and
# API definition scripts
#===============================================================================
def load_sub_scripts(env, script_dir_patterns, script_name_patterns, script_type, clone_env=False):
   #import pdb; pdb.set_trace()
   node = SoftwareUnitsNode(env)
   script_list = node.get_files(script_dir_patterns, script_name_patterns)
   script_type = "".join([script_type, " "])
   node.load_scripts(script_list, script_type, clone_env=clone_env)
   return None
   
#===============================================================================
# SoftwareUnitsNode
#   this class deal with loading the correct sripts at any script node that
#   has sub nodes, since the scripts need to be read and they may have 
#   different rules, more nodes needs to be created from with in the SCons
#   scripts.
#===============================================================================
class SoftwareUnitsNode:
   def __init__(self, env):
      self.verbose = int(GetOption('verbose'))
      self.debuginfo_opt = env.get('DEBUGINFO_OPT')
      self.env = env

   def get_files(self, dir_patterns, file_patterns, posix=False):
      # init vars
      env = self.env
      
      if type(dir_patterns) is not list:
         dir_patterns = [dir_patterns]
         
      if type(file_patterns) is not list:
         file_patterns = [file_patterns]

      # combine dir and file patterns
      patterns = ["".join([dir,"/",file]) for dir in dir_patterns for file in file_patterns]

      # glob files from dir/file patterns
      file_list = env.GlobFiles(patterns, posix)
      
      return file_list
      
   def load_scripts(self, scripts, script_type="", clone_env=False):
      # init vars
      env = self.env
      
      if type(scripts) is list:
         for script in scripts:
            org_env = env
            if clone_env:
               env = env.Clone()
         
            env.PrintDebugInfo('scripts', "Loading "+ script_type + "script: " + env.RealPath(script))
            env.SConscript(script, exports='env')
            env = org_env            
      else:
         org_env = env
         if clone_env:
            env = env.Clone()
            
         env.PrintDebugInfo('scripts', "Loading "+ script_type + "script: " + env.RealPath(scripts))
         env.SConscript(scripts, exports='env')
         env = org_env
         
      return None
   
   def load_sw_units(self, dir_patterns):
      #import pdb; pdb.set_trace()
      
      # init filters and list of script vars
      env = self.env
      filters = env.get('AU_FILTERS')
      script_name_patterns = env.get('SCRIPT_NAME_PATTERNS')
      filter_script_list = []
      
      # find all matching scripts at this level
      script_list = self.get_files(dir_patterns, script_name_patterns)
         
      # if no matching scripts where fund return
      if len(script_list) == 0:
         if 'scripts' in self.debuginfo_opt:
            env.PrintWarning("No scripts found from [" + env.RealPath('.') + "],\n" + \
               "   with dir pattern " + str(dir_patterns) + ".")
      
         return None

      # clean list depending on filters, and add filters to list
      # this filters will get pass down to sub nodes with the
      # AU_FILTERS env var. The trick is that each sub node gets 
      # only the part of the filter it cares about and not all of it.
      if filters is not None and len(filters.keys()) > 0:
         for key in filters.keys():
            filter_match = False
            dir_viable_list = [dir.replace('*',key) for dir in dir_patterns]
            for dir in dir_viable_list:
               for script in script_list:      
                  dir_path = env.NormPath(dir, posix=True)
                  script_path = env.NormPath(script, posix=True)
                  
                  # check for windows and lower case
                  if env['PLATFORM'] in ['windows', 'win32', 'cygwin']:
                     dir_path = string.lower(dir_path)
                     script_path = string.lower(script_path)

                  if dir_path == script_path[:len(dir_path)]:
                     filter_script_list.append([script, filters[key]])
                     filter_match = True
                     
            if not filter_match:
               #import pdb; pdb.set_trace()
               env.PrintWarning("No scripts found mathicng filter [" + key +"],\n" + \
                  "   from [" + env.RealPath('.') + "],\n" + \
                  "   with dir pattern " + str(dir_patterns) + ".")
      
      # no filters, load all scripts found
      else:
         filter_script_list = [(script, None) for script in script_list]
            
      #import pdb; pdb.set_trace()

      for script_filter in filter_script_list:
         script = script_filter[0]
         filter = script_filter[1]
         
         # if there are filters set variable accordingly
         if filter is not None:
            env.Replace(AU_FILTERS = filter)

         self.load_scripts(script, clone_env=True)
         #env.SConscript(script, exports='env')

#===============================================================================
# load_au_sw_units_deprecated /* DEPRECATED */
#
# Retunds two tuplet (libs, objs)
#   libs, libs nodes
#   objs, object files nodes
#
#===============================================================================
def load_au_sw_units_deprecated (env, au_name, filter_list=[]):
   items = env.LoadAreaSoftwareUnits(au_name, filter_list)
   return items.get('LIBS'), items.get('OBJS')

#===============================================================================
# load Core BSP Lib Scripts /* DEPRECATED */
#===============================================================================
def load_cbsp_lib_scripts_deprecated (env):
   # deprecated don't use

   items = env.LoadAreaSoftwareUnits("core")

   #import pdb; pdb.set_trace()
   root_env = env.get('ROOT_ENV')
   libs = root_env.get('LIBS')
   libs += items.get('OBJS')   
   libs += items.get('LIBS')
   
#===============================================================================
# load external Lib Scripts /* DEPRECATED */
#===============================================================================
def load_ext_lib_scripts_deprecated (env):
   # deprecated don't use
   return None

#===============================================================================
# MD5 sum for files
# returns hex digist
#===============================================================================
def md5file(env, file_names):
   md5data = hashlib.md5()

   # if file_names is not a list make it a list
   if type(file_names) != list:
      file_names = [file_names]

   for filename in file_names:
      file = open(filename)
      while 1:
         data = file.read(4096)
         if data == "":
            break
         md5data.update(data)
      file.close()
   return md5data.hexdigest()

#===============================================================================
# MD5 sum create md5 data file
# MD5 sums file and creates a md5 file
#===============================================================================
def md5file_create(env, file_names, md5_file_name):
   md5sum_data = md5file(env, file_names)

   md5_file_name = env.NormPath(md5_file_name)

   md5_file = open(md5_file_name, "w")
   md5_file.write(md5sum_data)
   md5_file.close()
   return md5sum_data

#===============================================================================
# MD5 sum compare and update md5 data file
#
#===============================================================================
def md5file_compare(env, file_name, md5_file_name):
   md5sum_data = 0

   md5_file_name = env.NormPath(md5_file_name)

   if os.path.exists(md5_file_name):
      md5_file = open(md5_file_name, "r")
      md5sum_data = md5_file.read(4096)
      md5_file.close()

   md5sum_new_data = md5file(env, file_name)

   if md5sum_data == md5sum_new_data:
      result = True
   else:
      result = False

   return result

#===============================================================================
# MD5 sum compare and update md5 data file
#
#===============================================================================
def md5file_compare_and_update(env, file_name, md5_file_name):
   result = env.CompareMD5File(file_name, md5_file_name)
   
   if not result:
      env.CreateMD5SumFile(file_name, md5_file_name)

   return result

#===============================================================================
# Remove Files
#
#===============================================================================
def remove_file (env, file_names):
   # if file_names is not a list make it a list
   if type(file_names) != list:
      file_names = [file_names]

   for filename in file_names:
      fname = env.NormPath(filename)
      if os.path.exists(fname):
         os.unlink(fname)
         print "Removed " + fname

   return None

#===============================================================================
# Find Files
#
#===============================================================================
def find_files (env, file_name_patterns='*', root_dir='.', posix=False, sort=True):
   # initialize variables
   ospath_join = os.path.join
   env_subst = env.subst
   aggregate_file_list = []
   
   # expand any SCons variables
   root_dir = env_subst(root_dir)   
   
   # make sure patterns is a list
   if type(file_name_patterns) is not list:
      file_name_patterns = [file_name_patterns]

   # walk trough each directory and try each pattern, aggregate results.
   for dirpath, dirs, files in os.walk(root_dir):
   
      for file_name_pattern in file_name_patterns:
   
         file_name_pattern = env_subst(file_name_pattern)

         files_match = fnmatch.filter(files, file_name_pattern)

         match_file_list = [ospath_join(dirpath, file) for file in files_match]
         match_file_list = env.RealPath(match_file_list)
         aggregate_file_list.extend(match_file_list)
   
   # remove any duplicate matches from aggregate list.
   aggregate_file_list = list(set(aggregate_file_list))

   if posix:
      aggregate_file_list = [file_item.replace('\\', '/') for file_item in aggregate_file_list]
   
   if sort:
      aggregate_file_list = sorted(aggregate_file_list)

   return aggregate_file_list

#===============================================================================
# Find Source Files
#
#===============================================================================
def find_source_files (env, file_name_patterns, srcpath, posix=False, sort=True):

   real_srcpath = env.RealPath(srcpath, posix)
   
   file_list = find_files(env, file_name_patterns, real_srcpath, posix, sort)
   file_list = [file.replace(real_srcpath, "${BUILDPATH}") for file in file_list]
   #import pdb; pdb.set_trace()
   
   return file_list


#===============================================================================
# Glob Files
#
#===============================================================================
def glob_files (env, glob_patterns=["*"], posix=False, sort=True):
   if env.get('PLATFORM', 'none') == 'cygwin':
      posix=True

   #import pdb; pdb.set_trace()
   # initialize variables
   env_subst = env.subst
   glob_glob = glob.glob
   glob_list = []
   glob_list_extend = glob_list.extend

   # make sure patterns is a list   
   if type(glob_patterns) is not list:
      glob_patterns = [glob_patterns]
   
   glob_patterns = [env_subst(pattern) for pattern in glob_patterns]
   [glob_list_extend(glob_glob(pattern)) for pattern in glob_patterns]
   
   # remove any duplicate matches from aggregate list.
   glob_list = list(set(glob_list))

   if posix:
      glob_list = [glob_item.replace('\\', '/') for glob_item in glob_list]

   if sort:
      glob_list = sorted(glob_list)
      
   return glob_list
   
#===============================================================================
# Glob Source Files
#
#===============================================================================
def glob_source_files (env, glob_patterns, srcpath, posix=False, sort=True):

   def norm_path(path, posix):
      npath = os.path.normpath(path)
      if posix:
         npath = npath.replace('\\','/')
      return npath

   if type(glob_patterns) is not list:
      glob_patterns = [glob_patterns]
   
   real_srcpath = env.RealPath(srcpath, posix)
   real_glob_patterns = ["".join([real_srcpath, "/", pattern]) for pattern in glob_patterns]
   
   file_list = glob_files(env, real_glob_patterns, posix, sort)
   file_list = [norm_path(file.replace(real_srcpath, "${BUILDPATH}"), posix) for file in file_list]
   #import pdb; pdb.set_trace()   
   
   return file_list
   

#===============================================================================
# add_explicit_deps
# adds explicit dependencies to the enviroment, the different compiler tools
# scripts will add this explicit dependencies to each .c or .cpp
#===============================================================================
def add_explicit_deps (env, files):
   env.Append(EXPLICIT_DEPS = [files])
