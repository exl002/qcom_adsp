import sys
import os
import re
import shutil
import subprocess
from subprocess import call

def Qshrink_execution(elf_path_name, env):
   elf_name=env.subst(elf_path_name)
   ElfNamePathWin=re.search('(.+)\\\M(\w+\.\w+)', elf_name)
   if ElfNamePathWin:
         ElfPath=ElfNamePathWin.group(1)
         ElfName=ElfNamePathWin.group(2)
   ElfNamePathLin=re.search('(.+)\/\M(\w+\.\w+)', elf_name)		 
   if ElfNamePathLin:
         ElfPath=ElfNamePathLin.group(1)
         ElfName=ElfNamePathLin.group(2)		 
   TargetName=ElfName[0:4]
   ElfFileName = 'M' + ElfName  
   if (TargetName == '9x25' or TargetName == '8x10'):
      elf_path=env.subst(elf_path_name)      
      qshrinkelfname="".join(['qsr_',ElfFileName])
      qshrinkelfpath=os.path.join(ElfPath,qshrinkelfname)
      QSHRINKPATH = '../../qdsp6/qshrink/src/Qshrink20.py'
      ELFFILEPATH = elf_path_name
      ELFOUTFILEPATH = qshrinkelfpath
      if (TargetName == '9x25'):
         HASHFILEPATH = '../../qdsp6/qshrink/src/msg_hash_9x25.txt'
      if (TargetName == '8x10'):
         HASHFILEPATH = '../../qdsp6/qshrink/src/msg_hash.txt'	  
      LOGFILEPATH = '../../qdsp6/qshrink/src/qsr_.txt'
      RetCode = call(['python',
                       QSHRINKPATH,
                       ELFFILEPATH,
                       '--output=' + ELFOUTFILEPATH,
                       '--hashfile=' + HASHFILEPATH,
                       '--log=' + LOGFILEPATH])
      os.remove(ELFFILEPATH)
      os.rename(ELFOUTFILEPATH, ELFFILEPATH)
   return