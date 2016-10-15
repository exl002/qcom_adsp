#####################################################
# To use
# Returns string consisting of the flags
# import derive_flags
# build_flags.get_flags(directory of build_config.def)
#####################################################
import re
import os
import sys
import string

#
# Read the .def file line by line and contruct a list of lists
# Ignore - Empty lines and comments. Comments are lines starting with "#"
# Error - if more than one "=" sign in a line
#         if the build flag is not defined by "y" or "n"
#
def get_flags(file_name):
   flags =[]
   
   for line in file(file_name):
      line_list = []
      line = line.strip(string.whitespace) # strips all kinds of whitespace including
                                           # carriage returns for linux
      if not line:
         # empty line. skip it
         continue
      if line.startswith('#'): # Is this a comment ?
         continue

      # replace all whitespace with empty chars by picking up characters not in string.whitespace
      line = ''.join(s for s in line if s not in string.whitespace)

      line_list = str.split(line,'=')
      if len(line_list) == 2:
         flag_value = line_list[1] 
         if (flag_value != 'y') and (flag_value != 'n'):
            print >> sys.stderr, 'Error- Invalid build flag:(%s) at line:(%s)' % (repr(flag_value), repr(line))
            sys.exit(1) # Error
      else:
         print >> sys.stderr, 'Error- Invalid line:(%s)' % (repr(line))
         sys.exit(1) # Error

      # Error checking is done
      flags.append(line_list)

   return flags

#
# Computes the build flags based on the configuration.
# If "CONFIG_STATIC=y" is found in the configuration all
# the configuration specification upto that point are ignored
# and default build flags are set based on the definitions from
# Input/static_build_config.def
#
def compute_build_flags (default_dir, config_dir):
   config_flags = get_flags(config_dir+"/build_config.def")

   is_config_static_defined = False
   for i in range (0, len(config_flags)):
      (elem_name, elem_value) = config_flags[i]
      if (elem_name == 'CONFIG_STATIC') and (elem_value == 'y'):
         is_config_static_defined = True
         config_flags.pop(i)
         break

   if is_config_static_defined == True: 
      default_flags = get_flags(default_dir+"/static_build_config.def")
      final_flags = default_flags
   else:
      default_flags = get_flags(default_dir+"/default_build_config.def")
      final_flags = default_flags

   for (elem_name, elem_value) in config_flags:
      for i in range (0, len(final_flags)):
         (flag_name, flag_value) = final_flags[i]
         if (elem_name == flag_name):
            #import pdb; pdb.set_trace()
            final_flags[i] = (elem_name, elem_value)
            break;

   return final_flags

#
# Main function that prints build flags with "-D" prefix
#
if __name__ == "__main__":
   script_path = os.path.dirname(sys.argv[0])
   input_path = script_path+"/Input"
   config_path = sys.argv[1]
   #print compute_build_flags(input_path, config_path)
   build_flags = compute_build_flags(input_path, config_path)
   s = ""

   for (elem_name, elem_value) in build_flags:
      if elem_value == 'y':
         s = s + " -D" + elem_name

   print s

