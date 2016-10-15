#===============================================================================
#
# AVS Library Builder
#
# GENERAL DESCRIPTION
#    This builder will create one of three library types based on USES flags
#    set in the AVS image scons script file.
#       1) A normal library with symbols (env.AddLibrary)
#       2) A stub library (env.AddLibrary)
#       3) A stripped library (obfuscated and symbols stripped)
#
# Copyright (c) 2009-2012 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/build/avs_library_builder.py#16 $
#  $DateTime: 2013/09/13 07:16:29 $
#  $Author: coresvc $
#  $Change: 4427964 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 08/01/12   ps      Create Builder
#
#===============================================================================

import re
import os

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Builder function definitions
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('avs_prop_library_builder')

   
def generate(env):
   # Generate function for Proprietary Library builder:  Sets up the 
   # pseudo builder, action and builder function to be called by
   # clients to pass library details to the builder.
   
   env.Replace(USES_AVS_LIBRARY_BUILDER = True)

   # Add Obfuscate tool to construction environment
   if env['PLATFORM'] == 'linux':
      env.Replace(OBFUSCATE = '${BUILD_ROOT}/avs/build/sbin/linux64/hexagon-obfuscate')
   else:
      env.Replace(OBFUSCATE ='${BUILD_ROOT}/avs/build/sbin/win32/hexagon-obfuscate.exe')
      
   # Get verbose option
   env.Replace(VERBOSE = int(env.GetOption('verbose')))
   
   # Add pseudo-builder name (wrapper) function
   env.AddMethod(add_avs_library, 'AddAvsLibrary')
   
   # ----------------------------------
   # Setup the AVS proprietary builders
   # ----------------------------------
   
   # AVS Proprietary Symbol Builder
   avs_prop_symbol_act = env.GetBuilderAction(avs_prop_symbol_builder)
   avs_prop_symbol_bld = env.Builder(action = avs_prop_symbol_act)
   env.Append(BUILDERS = {'AvsPropSymbolBuilder' : avs_prop_symbol_bld})
   
   # AVS Proprietary Link Builder
   avs_prop_link_act = env.GetBuilderAction(avs_prop_link_builder)
   avs_prop_link_bld = env.Builder(action = avs_prop_link_act)
   env.Append(BUILDERS = {'AvsLinkBuilder' : avs_prop_link_bld})
   
   # AVS Proprietary Link Builder
   avs_prop_strip_link_act = env.GetBuilderAction(avs_prop_strip_link_builder)
   avs_prop_strip_link_bld = env.Builder(action = avs_prop_strip_link_act)
   env.Append(BUILDERS = {'AvsPropLinkBuilder' : avs_prop_strip_link_bld})
   
   # AVS Proprietary Obfuscate Builder
   avs_prop_obfus_act = env.GetBuilderAction(avs_prop_obfus_builder)
   avs_prop_obfus_bld = env.Builder(action = avs_prop_obfus_act,
                                    emitter= avs_prop_obfus_emitter)
   env.Append(BUILDERS = {'AvsPropObBuilder' : avs_prop_obfus_bld})
   
   # AVS Proprietary Strip Builder
   avs_prop_strip_act = env.GetBuilderAction(avs_prop_strip_builder)
   avs_prop_strip_bld = env.Builder(action = avs_prop_strip_act)
   env.Append(BUILDERS = {'AvsPropStripBuilder' : avs_prop_strip_bld})
 
   return
   
# When prop list is empty strip and donot obfuscate regular libraries
def prop_list_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip):
      
      if build_type == 'LEAF':
         # Generate object files from regular source files
         reg_objs = env.AddObject(build_tags, reg_source_list)
        
         # Generate object files from stub source files
         stub_objs = env.AddObject(build_tags, stub_source_list)

      if build_type == 'MOD':
         reg_objs = list(reg_source_list)
         stub_objs = list(stub_source_list)
            
      if uses_lib_strip in env:
          
         # Use 'ld' to partially link all proprietary objects and regular
         # objects.  This will resolve all local symbols and produce a 
         # single relocatable file.
         link_reloc = env.AvsPropLinkBuilder(library + '_link.reloc', 
            [reg_objs])
         
                    
         # Use 'strip' to remove all non-global symbols from obfuscated 
         # relocatable object
         prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', link_reloc)

         # Create strip library file (e.g., Quartet_strip.lib) from the stripped
         # relocatable object.
         library_node = env.AddLibrary(build_tags, library + "_strip", prop_strip)
         
         
      elif uses_lib_stub in env:
          # Use 'ld' to partially link stub sources.
         link_reloc = env.AvsLinkBuilder(library + '_link.reloc', 
                [stub_objs])
         library_node = env.AddLibrary(build_tags, library + "_stub", link_reloc)

      else:
          # Use 'ld' to partially link all proprietary objects and regular
          # objects.  This will resolve all local symbols and produce a 
          # single relocatable file.
         link_reloc = env.AvsLinkBuilder(library + '_link.reloc', 
             [reg_objs])

         library_node = env.AddLibrary(build_tags, library, link_reloc)   

#When prop list is not empty obfuscate them
def prop_list_not_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip,prop_source_list,library_path):
	
      if build_type == 'LEAF':
         # Generate object files from regular source files
         reg_objs = env.AddObject(build_tags, reg_source_list)
        
         # Generate object files from proprietary source files
         prop_objs = env.AddObject(build_tags, prop_source_list)

         # Generate object files from stub source files
         stub_objs = env.AddObject(build_tags, stub_source_list)

      if build_type == 'MOD':
         reg_objs = list(reg_source_list)
         prop_objs = list(prop_source_list)   
         stub_objs = list(stub_source_list)
            
      if uses_lib_strip in env:
          
         # Use 'nm' (Name list) to produce list of proprietary symbols 
         # from the proprietary object files (*.o)
         prop_sym_fn = library_path + '/proprietary_symbols.txt'
         prop_sym_file = env.AvsPropSymbolBuilder(prop_sym_fn, prop_objs)
         
         # Put symbol map filename into environment so it can be cleaned 
         # later.  SCons doesn't know about the map file, so we have to 
         # explicitly clean with an env.Clean() call.
         env.Replace(PROP_MAP_FN = library_path + '/proprietary_symbols.map')

         #Use 'ld' to partially link all proprietary objects and regular
         # objects.  This will resolve all local symbols and produce a 
         # single relocatable file.
         link_reloc = env.AvsPropLinkBuilder(library + '_link.reloc', 
            [prop_objs, reg_objs])
         
         # Use 'Obfuscate' program (Qualcomm) to encrypt the proprietary 
         # symbols in the relocatable object
         prop_obfus = env.AvsPropObBuilder(library + '_obfuscate.reloc', 
            [prop_sym_file, link_reloc])
              
         # Use 'strip' to remove all non-global symbols from obfuscated 
         # relocatable object
         prop_strip = env.AvsPropStripBuilder(library + '_strip.reloc', prop_obfus)

         # Create strip library file (e.g., Quartet_strip.lib) from the stripped
         # relocatable object.
         library_node = env.AddLibrary(build_tags, library + "_strip", prop_strip)
         
         
      elif uses_lib_stub in env:
          # Use 'ld' to partially link stub sources.
         link_reloc = env.AvsLinkBuilder(library + '_link.reloc', 
            [stub_objs])
         library_node = env.AddLibrary(build_tags, library + "_stub", link_reloc)

      else:
          # Use 'ld' to partially link all proprietary objects and regular
          # objects.  This will resolve all local symbols and produce a 
          # single relocatable file.
         link_reloc = env.AvsLinkBuilder(library + '_link.reloc', 
            [prop_objs, reg_objs])

         library_node = env.AddLibrary(build_tags, library, link_reloc) 

def add_avs_library(env, build_tags, library, sources,build_type='LEAF'):
   # Input parameters:
   #   'build_tags' and 'library' are the same as add_library
   #   'sources' is a list of three (3) source file lists:
   #      sources[0] --> list of regular source files
   #      sources[1] --> list of stub source files
   #      sources[2] --> list of proprietary source files
   #
   # This function checks USES flags as follows and builds the correct libraries:
   #
   #    USES_<Library>_STRIP  --
   #       - Compile and build library using 'strip' and regular source lists
   #       - Use NM to create a list of proprietary symbols
   #       - Use 'ld' to partially link all proprietary objects and regular objects
   #       - Use 'Obfuscate' program to encrypt the proprietary symbols in the 
   #         relocatable object
   #       - Use 'strip' to remove all non-global symbols from the obfuscated 
   #         relocatable object
   #       - Create strip library file (e.g., Quartet_strip.lib) from the stripped
   #         relocatable object.
   #      
   #    USES_<Library>_STUB --
   #       - Compile and build library using 'stub' source list
   #
   #    No USES Flags set -- (default)
   #       - Compile and build library using 'regular' source list

   library_node = []
      
   # Unpack regular, stub and propriety source lists from 'sources'
   reg_source_list = sources[0]
   stub_source_list = sources[1]
   prop_source_list = sources[2]

   # Get library name and create USES_<library>_<type> strings
   library_path, library_fn = os.path.split(library)
   uses_lib_stub  = ''.join(['USES_', library_fn.upper(), '_STUB'])
   uses_lib_strip = ''.join(['USES_', library_fn.upper(), '_STRIP'])

   if 'USES_CREATE_ONLY_STUBS' in env.get('USES_FLAGS'):
      if stub_source_list and not uses_lib_stub in env:
         library_node = env.AddBinaryLibrary('AVS_ADSP_STUBS', library + "_stub", stub_source_list)
      return library_node

   if not env.IsTargetEnable(build_tags):
      return library_node
      
   cleanpack = env.GetOption('cleanpack')
   if ('USES_CLEAN_PACK' in env) or cleanpack:
      if uses_lib_strip in env:
         library_node = env.AddBinaryLibrary(build_tags, library + "_strip", prop_source_list)
               
      elif uses_lib_stub in env:
          # Use 'ld' to partially link stub sources.
         #link_reloc = env.AvsLinkBuilder(library + '_link.reloc', 
             #[stub_source_list])
         library_node = env.AddBinaryLibrary(build_tags, library + "_stub", stub_source_list)

        
      else:
         library_node = env.AddBinaryLibrary(build_tags, library, reg_source_list)
   
		 
   else:
      if prop_source_list == []:
         prop_list_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip)  
      else:
         prop_list_not_empty(env, build_tags, build_type, reg_source_list, stub_source_list, library,uses_lib_stub,uses_lib_strip,prop_source_list,library_path)
   return library_node
   
   
def avs_prop_symbol_builder(target, source, env):
   prop_sym_fn = str(target[0])
   prop_objs_str = ''.join([ ''.join([str(src), ' ' ]) for src in source ])

   nm_cmd = ''.join([ '${NM} -f posix ', prop_objs_str ])
   #import pdb; pdb.set_trace()
   data, err, rv = env.ExecCmds (nm_cmd, silent=True, target='AVS Proprietary Symbol File')
   
   if env['VERBOSE'] == 2:
     env.PrintInfo('Removing lines matching regex - " \.CONST_"')
     env.PrintInfo('Finding lines matching regex - " [DdTt] "')
     
   # Remove '\r' chars from nm output
   nm_data1 = re.sub('\r', '', data)

   #import pdb; pdb.set_trace()

   m = re.search('.*\.CONST_.*',nm_data1,re.M)
   print m
   while True:
       if m==None:
           break;
       nm_data = re.sub('.*\.CONST_.*', '', nm_data1, re.M);
       m = re.search('.*\.CONST_.*',nm_data,re.M);
       nm_data1 = nm_data;

   #import pdb; pdb.set_trace()
   # Find all lines with regex of: ' [DdTt] '
   nm_list = re.findall('.* [DdTt] .*', nm_data1, re.M)

   if env['VERBOSE'] == 2:
     env.PrintInfo('Creating Symbol File - %s' % prop_sym_fn)

     # Create the proprietary symbol file
   try:
      prop_sym_fh = open(prop_sym_fn, 'w')
   except IOError:
      env.PrintError ('Error Could not create Proprietary Symbol file - ' + prop_sym_fn)
   
   for nm_line in nm_list:
      prop_sym_fh.write(nm_line + '\n')

   # Close proprietary symbol file
   prop_sym_fh.close()
   #import pdb; pdb.set_trace()
   return None
   
   
def avs_prop_link_builder(target, source, env):
   prop_objs_str = ''.join([ ''.join([str(src), ' ' ]) for src in source ])
   link_cmd = ''.join([ '${LINK} -r -g -m${Q6VERSION} --whole-archive -o ', str(target[0]), " ", prop_objs_str])
   data, err, rv = env.ExecCmds (link_cmd, silent=True, target='AVS Proprietary Linked Relocatable File')
   
   return None
   
def avs_prop_strip_link_builder(target, source, env):
   prop_objs_str = ''.join([ ''.join([str(src), ' ' ]) for src in source ])
   link_cmd = ''.join([ '${LINK} -r -g -m${Q6VERSION} --whole-archive --script ../../qdsp6/Partial_Linking/plink.linker -o ', str(target[0]), " ", prop_objs_str])
   data, err, rv = env.ExecCmds (link_cmd, silent=True, target='AVS Proprietary Linked Relocatable File')
   
   return None   
   
   
def avs_prop_obfus_builder(target, source, env):
   prop_sym_file = str(source[0])
   prop_link_reloc = str(source[1])
   
   # Derive map filename from symbol filename
   prop_sym_path, prop_sym_ext = os.path.splitext(prop_sym_file)
   prop_map_fn = ''.join([prop_sym_path, '.map'])
   
   obfus_cmd = ''.join([ '${OBFUSCATE} --symbols-file ', prop_sym_file, ' --map ', 
               prop_map_fn, ' -o ', str(target[0]), ' ', prop_link_reloc])
   
   data, err, rv = env.ExecCmds (obfus_cmd, silent=True, target='AVS Obfuscated Relocatable File')
   
   return None
   
   
def avs_prop_obfus_emitter(target, source, env):
   env.Clean(target, env['PROP_MAP_FN'])
   
   return (target, source)


def avs_prop_strip_builder(target, source, env):
   strip_cmd = ''.join([ '${STRIP} -x ', str(source[0]), ' -o ', str(target[0])])
   data, err, rv = env.ExecCmds (strip_cmd, silent=True, target='AVS Stripped Relocatable File')
   
   return None
