#===============================================================================
# Copyrigha (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================

import sys
import os
import string
import SCons.Action
from SCons.Script import *

gList_File = '/platform/exports/exposed_symbols.txt'
gBaseSymbols_File = '/platform/exports/base_symbols.lst'
gDynsyms_File = '/platform/exports/dynsymbols.lst'
gExterns_File = '/platform/exports/externs.lst'

def dl_exposeFile(env, filename):
  if env.GetOption('clean'):
    print 'DLExposeFile - Not exposing symbols. Building clean'
    return

  global gList_File
  list_file = env['BUILD_ROOT'] + gList_File

  if not os.path.exists(filename):
    print 'DLExposeFile: Error: File %s does not exist. (ignored)' % filename

  print 'DLExposeFile: Adding symbols from file %s' % filename
  with FileLock(list_file):
    f = open(list_file, "a")
    for line in file(filename):
      name = line.strip()
      if name: #avoid empty lines
        f.write("%s\n" % name)
    f.close()

def dl_expose(env, name):
   if env.GetOption('clean'):
    print 'DLExpose - Not exposing symbols. Building clean'
    return

   global gList_File
   list_file = env['BUILD_ROOT'] + gList_File

   names = [n.strip() for n in name.split(',')]
   with FileLock(list_file):
      f = open(list_file, "a")
      f.write("%s\n" % "\n".join(names))
      f.close()

def dlexpose_generate_builder(target, source, env):
   if env.GetOption('clean'):
      print 'DLExposeBuilder - Not exposing symbols. Building clean'
      return
   # Read the list file into a set
   global gList_File
   global gBaseSymbols_File
   global gDynsyms_File
   global gExterns_File
   print 'DLExposeGenerateBuilder: Called'

   list_file = env['BUILD_ROOT'] + gList_File
   basesyms_file = env['BUILD_ROOT'] + gBaseSymbols_File
   dynsymbols_file = env['BUILD_ROOT'] + gDynsyms_File
   #dynsymbols_file = str(target[0])
   externs_file = env['BUILD_ROOT'] + gExterns_File

   f = open(list_file, "r")
   lines = [l.strip() for l in f.readlines()]
   f.close()

   #Remove duplicates
   libs = list(set(lines))

   # Create the dynsymbols file
   fdyns = open(dynsymbols_file, "w")
   fexterns = open(externs_file, "w")
   try:
    # Write the headers
    fdyns.write('{\n/* Base symbols */\n')
    fexterns.write('EXTERN (')

    # Utility function to reuse code for writing to dynsym and extern list
    def writeSymbol(symbol):
      fdyns.write('  %s;\n' % symbol)
      fexterns.write('%s\n' % symbol)

    print 'DLExposeGenerateBuilder: Writing base symbols'

    # Write only if minimal is not enabled
    if not 'USES_PLATFORM_MINIMAL' in env:
        for line in file(basesyms_file):
          line = line.strip()
          if line not in libs: # Avoid duplicates
            writeSymbol(line)

    print 'DLExposeGenerateBuilder: Writing DLExposed symbols'
    fdyns.write ('\n  /* DL Expose symbols */\n')
    for name in libs:
      writeSymbol(name)

    # write footer
    fdyns.write('};\n')
    fexterns.write(');\n')

   finally:
    fdyns.close()
    fexterns.close()

   # Delete the list file so it gets recreated the next time
   os.remove(list_file)

def dlexpose_generate(env, name):
   global gList_File
   global gBaseSymbols_File
   global gDynsyms_File
   global gExterns_File

   list_file = env['BUILD_ROOT'] + gList_File
   basesyms_file = env['BUILD_ROOT'] + gBaseSymbols_File
   dynsymbols_file = env['BUILD_ROOT'] + gDynsyms_File
   externs_file = env['BUILD_ROOT'] + gExterns_File

   platform_exports_file = env.DLExposeGenerateBuilder(target=name, source = None)
   #env.Depends(platform_exports_file, list_file)
   #env.Depends(platform_exports_file, basesyms_file)
   install_platform_exports_file = env.InstallAs(dynsymbols_file, platform_exports_file)
   env.Depends(install_platform_exports_file, platform_exports_file)

   print 'DLExposeGenerate: Install file: ', install_platform_exports_file
   env.AddArtifact('ADSP_PROC_IMG', install_platform_exports_file)
   #env.AddArtifact('QDSP6_SW_IMAGE', install_platform_exports_file)
   env.AddArtifact('MULTI_PD_IMG', install_platform_exports_file)
   #env.AddArtifact('SINGLE_IMAGE', install_platform_exports_file)
   #env.AddArtifact('MODEM_IMAGE', install_platform_exports_file)
   #env.AddArtifact('CBSP_QDSP6_SW_IMAGE', install_platform_exports_file)
   print "DLExposeGenerate: platform_exports_file", install_platform_exports_file
   return env.AlwaysBuild(platform_exports_file)

def dlclean(env):
   global gList_File
   global gDynsyms_File
   global gExterns_File
   print 'DLExposeGenerateBuilder: Called to clean'

   list_file = env['BUILD_ROOT'] + gList_File
   dynsymbols_file = env['BUILD_ROOT'] + gDynsyms_File
   externs_file = env['BUILD_ROOT'] + gExterns_File

   def cleanFile(filename):
     if os.path.exists(filename):
       #print 'Removed %s' % filename
       #os.remove(filename)
       print 'In cleanFile'
   map(cleanFile, [list_file, dynsymbols_file, externs_file])


def exists(env):
   return env.Detect('dlinker_symbols')

def generate(env):
   env.AddMethod(dl_expose, "DLExpose")
   env.AddMethod(dl_exposeFile, "DLExposeFile")
   env.AddMethod(dlexpose_generate, "DLExposeGenerate")

   # use standard action wrapper provided by QCT extensions, provides debug
   # levels and correct text output during build. Not using this will result
   # in function pointers and mangle names displayed instead of a nice string
   # such as "=== Generating  <your target>"
   dlinker_action = env.GetBuilderAction(dlexpose_generate_builder)

   #env['BUILDERS']['DLExposeGenerateBuilder'] = Builder(name = "DLExposeGenerateBuilder",
   #                                         action = dlexpose_generate_builder)
   dlinker_bld = env.Builder(
      action = dlinker_action,
      #emitter=skeleton_emitter,
      #source_scanner=skeleton_source_scanner,
      #target_scanner=skeleton_target_scanner,
      #suffix = '.my_extention'
   )
   #env.Append(BUILDERS = {'DLExposeGenerateBuilder' : dlinker_bld})


# BSD Licensed Cross-platform file locking support in Python
# See http://www.evanfosmark.com/2009/01/cross-platform-file-locking-support-in-python/
# License information - http://www.evanfosmark.com/2009/01/cross-platform-file-locking-support-in-python/#comment-1054

class FileLockException(Exception):
    pass

class FileLock(object):
    """ A file locking mechanism that has context-manager support so
        you can use it in a with statement. This should be relatively cross
        compatible as it doesn't rely on msvcrt or fcntl for the locking.
    """

    def __init__(self, file_name, timeout=10, delay=.05):
        """ Prepare the file locker. Specify the file to lock and optionally
            the maximum timeout and the delay between each attempt to lock.
        """
        self.is_locked = False
        self.lockfile = os.path.join(os.getcwd(), "%s.lock" % file_name)
        self.file_name = file_name
        self.timeout = timeout
        self.delay = delay


    def acquire(self):
        """ Acquire the lock, if possible. If the lock is in use, it check again
            every `wait` seconds. It does this until it either gets the lock or
            exceeds `timeout` number of seconds, in which case it throws
            an exception.
        """
        start_time = time.time()
        while True:
            try:
                self.fd = os.open(self.lockfile, os.O_CREAT|os.O_EXCL|os.O_RDWR)
                break;
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise
                if (time.time() - start_time) >= self.timeout:
                    raise FileLockException("Timeout occured.")
                time.sleep(self.delay)
        self.is_locked = True


    def release(self):
        """ Get rid of the lock by deleting the lockfile.
            When working in a `with` statement, this gets automatically
            called at the end.
        """
        if self.is_locked:
            os.close(self.fd)
            os.unlink(self.lockfile)
            self.is_locked = False


    def __enter__(self):
        """ Activated when used in the with statement.
            Should automatically acquire a lock to be used in the with block.
        """
        if not self.is_locked:
            self.acquire()
        return self


    def __exit__(self, type, value, traceback):
        """ Activated at the end of the with statement.
            It automatically releases the lock if it isn't locked.
        """
        if self.is_locked:
            self.release()


    def __del__(self):
        """ Make sure that the FileLock instance doesn't leave a lockfile
            lying around.
        """
        self.release()
