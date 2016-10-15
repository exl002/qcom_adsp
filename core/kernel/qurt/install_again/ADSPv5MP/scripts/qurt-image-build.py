#! /usr/bin/python

import getopt
import os
import string
import struct
import sys

''' Define keys used in dictionaries '''
APPNAME='appname'
FILENAME='filename'
OS_IMAGE='os_image'
FILE='file'
FMT='fmt'
RESTART='restart'

OUTPUT_OFFSET='output_offset'
COPY_BOT='copy_bot'
COPY_TOP='copy_top'
FILEALIGN='filealign'
OUTPUT_PHYSADDR='output_physaddr'
VMEM_BOT='vmem_bot'
VMEM_TOP='vmem_top'
MEMALIGN='memalign'
OUTCOPY='outcopy'
SEGS='segs'

EHDR='ehdr'
E_IDENT='e_ident'
E_TYPE='e_type'
E_MACHINE='e_machine'
E_VERSION='e_version'
E_ENTRY='e_entry'
E_PHOFF='e_phoff'
E_SHOFF='e_shoff'
E_FLAGS='e_flags'
E_EHSIZE='e_ehsize'
E_PHENTSIZE='e_phentsize'
E_PHNUM='e_phnum'
E_SHENTSIZE='e_shentsize'
E_SHNUM='e_shnum'
E_SHSTRNDX='e_shstrndx'

PHDR='phdr'
P_TYPE='p_type'
P_OFFSET='p_offset'
P_VADDR='p_vaddr'
P_PADDR='p_paddr'
P_FILESZ='p_filesz'
P_MEMSZ='p_memsz'
P_FLAGS='p_flags'
P_ALIGN='p_align'

SHDR='shdr'
SH_NAME='sh_name'
SH_TYPE='sh_type'
SH_FLAGS='sh_flags'
SH_ADDR='sh_addr'
SH_OFFSET='sh_offset'
SH_SIZE='sh_size'
SH_LINK='sh_link'
SH_INFO='sh_info'
SH_ADDRALIGN='sh_addralign'
SH_ENTSIZE='sh_entsize'

STRTAB='strtab'

''' Define formats of ELF structures '''
ehdr_format = [[E_IDENT    , '16s', '%r'  ],
               [E_TYPE     , 'H'  , '%u'  ],
               [E_MACHINE  , 'H'  , '%u'  ],
               [E_VERSION  , 'L'  , '%u'  ],
               [E_ENTRY    , 'L'  , '0x%X'],
               [E_PHOFF    , 'L'  , '%u'  ],
               [E_SHOFF    , 'L'  , '%u'  ],
               [E_FLAGS    , 'L'  , '0x%X'],
               [E_EHSIZE   , 'H'  , '%u'  ],
               [E_PHENTSIZE, 'H'  , '%u'  ],
               [E_PHNUM    , 'H'  , '%u'  ],
               [E_SHENTSIZE, 'H'  , '%u'  ],
               [E_SHNUM    , 'H'  , '%u'  ],
               [E_SHSTRNDX , 'H'  , '%u'  ]]

phdr_format = [[P_TYPE  , 'L', '0x%X'],
               [P_OFFSET, 'L', '0x%X'],
               [P_VADDR , 'L', '0x%X'],
               [P_PADDR , 'L', '0x%X'],
               [P_FILESZ, 'L', '0x%X'],
               [P_MEMSZ , 'L', '0x%X'],
               [P_FLAGS , 'L', '0x%X'],
               [P_ALIGN , 'L', '0x%X']]

shdr_format = [[SH_NAME     , 'L', '0x%X'],
               [SH_TYPE     , 'L', '0x%X'],
               [SH_FLAGS    , 'L', '0x%X'],
               [SH_ADDR     , 'L', '0x%X'],
               [SH_OFFSET   , 'L', '0x%X'],
               [SH_SIZE     , 'L', '0x%X'],
               [SH_LINK     , 'L', '0x%X'],
               [SH_INFO     , 'L', '0x%X'],
               [SH_ADDRALIGN, 'L', '0x%X'],
               [SH_ENTSIZE  , 'L', '0x%X']]

old_eip_mode = None
no_adjust = None

# zstr = str(bytearray(0x10000))
# Can't use above line with Python 2.4, try this:
zstr = '\0' * 0x10000

def padzero(outfile, length):
  while length > len(zstr):
    outfile.write(zstr)
    length -= len(zstr)
  if length > 0:
    outfile.write(zstr[0:length])

def copydata(outfile, infile, seekloc, length):
  infile.seek(seekloc)
  while length > 0x10000:
    outfile.write(infile.read(0x10000))
    length -= 0x10000
  if length > 0:
    outfile.write(infile.read(length))

def new_strtab_entry(f, s):
  s = s + '\0'
  f[STRTAB] = f[STRTAB] + s
  return len(f[STRTAB])-len(s)

def aligndown(n, a):
  return n & ~(a-1)

def alignup(n, a):
  return (n+(a-1)) & ~(a-1)

def string_lookup(f, n):
  s = f[STRTAB]
  e = s.index('\0', n)
  return s[n:e]

''' Old version of write_eip_info, wrote 80 bytes per image unconditionally '''
def old_write_eip_info(f, infiles):
  s = ''
  for i in infiles:
    s += struct.pack('<LLLL64s', *(i[OUTPUT_OFFSET], i[VMEM_BOT], i[OUTPUT_PHYSADDR], i[VMEM_TOP], i[APPNAME]))
  f[FILE].write(s)
  return len(s)

''' Look through the list for segments which overlap or touch '''
''' Elements are sorted by the start address '''
''' If they overlap, combine them and combine permissions '''
''' If they touch without overlapping, only combine them if permissions are identical '''
''' Edit the list and return True if we were able to optimize '''
''' Leave the list and return False if we were not able to optimize '''
def seg_optimize(segs):
  for n in range(len(segs)-1):
    if segs[n][1] > segs[n+1][0]:
      ''' n and n+1 overlap '''
      if segs[n][2] != segs[n+1][2]:
        raise Exception('Overlapping segments with different memory maps')
      ''' n and n+1 overlap with the same offset '''
      ''' Combine them, including permissions, and return '''
      combined = (segs[n][0],
                  max(segs[n][1], segs[n+1][1]),
                  segs[n][2],
                  segs[n][3] or segs[n+1][3],
                  segs[n][4] or segs[n+1][4])
      segs = segs[:n] + [combined] + segs[n+2:]
      return (False, segs)
    elif segs[n][1] == segs[n+1][0]:
      ''' n and n+1 touch '''
      if segs[n][2:] == segs[n+1][2:]:
        ''' n and n+1 touch and have the same offset and permissions '''
        ''' combine them and return '''
        segs = segs[:n] + [(segs[n][0], segs[n+1][1]) + segs[n][2:]] + segs[n+2:]
        return (False, segs)
    else:
      ''' n and n+1 have separation between them '''
      pass
  return (True, segs)

''' New version of write_eip_info, writes header, variable length records, and string table '''
'''
    Top level header (fixed length, one only):
      uint32_t magic_number (0x32504945)
      uint16_t version_number
      uint16_t number_of_apps
    Second level header (fixed length, one per app):
      uint16_t first_memsection_number
      uint16_t number_of_memsections
      uint16_t entry_memsection_number
      uint16_t string_table_offset_of_app_name
    Third level header (fixed length, one per memsection):
      uint32_t virtual_address_start
      uint32_t physical_address_start
      uint32_t length_in_bytes
      uint32_t permissions
    String table:
      Zero terminated strings for app names
      (App names which are restartable are prepended with '>' greater than sign)
      (App names which are not-restartable are not prepended)
'''
def write_eip_info(f, infiles):
  for i in infiles:
    segs = []
    for q in i[PHDR]:
      p = q[OUTCOPY]
      ''' Add the memory described in this program header '''
      virt_start = aligndown(p[P_VADDR], 0x1000)
      virt_end = alignup(p[P_VADDR] + p[P_MEMSZ], 0x1000)
      virt_offset = p[P_PADDR] - p[P_VADDR]
      virt_exec = ((p[P_FLAGS] & 1) != 0)
      virt_write = ((p[P_FLAGS] & 2) != 0)
      if virt_start != virt_end and p[P_TYPE] == 1:
        segs += [(virt_start, virt_end, virt_offset, virt_exec, virt_write)]
    done, segs = (False, sorted(segs))
    while not done:
      done, segs = seg_optimize(segs)
    i[SEGS] = segs
  top_hdr = (0x32504945,1,len(infiles))
  eip_data = struct.pack('<LHH', *top_hdr)
  strtab = ''
  secno = 0
  for i in infiles:
    segs = i[SEGS]
    second_hdr = (secno, len(segs), secno, len(strtab))
    eip_data += struct.pack('<HHHH', *second_hdr)
    secno += len(segs)
    if i[RESTART]:
      strtab += '>'
    strtab += i[APPNAME] + '\0'
  for i in infiles:
    segs = i[SEGS]
    for s in segs:
      perms = 0
      ''' Add in exec and write permissions '''
      if s[3]:
        perms += 1
      if s[4]:
        perms += 2
      third_hdr = (s[0], s[0]+s[2], s[1]-s[0], perms)
      eip_data += struct.pack('<LLLL', *third_hdr)
  eip_data += strtab
  f[FILE].write(eip_data)
  return len(eip_data)

def repr_generic(dict):
  return string.join([('%s: %s' % (e[0],e[2])) % (dict[e[0]]) for e in dict[FMT]],' ')

def elf_format(dict):
  return struct.pack(string.join(['<']+[e[1] for e in dict[FMT]],''), *tuple([dict[e[0]] for e in dict[FMT]]))

def parse_generic(data, fmt):
  tmp = struct.unpack(string.join(['<']+[e[1] for e in fmt],''), data)
  tmp = dict((fmt[j][0], tmp[j]) for j in range(len(fmt)))
  tmp[FMT] = fmt
  return tmp

def parse_elf_header(f):
  try:
    f[FILE].seek(0)
    f[EHDR] = parse_generic(f[FILE].read(52), ehdr_format)
    if f[EHDR][E_EHSIZE] != 52:
      raise
    if f[EHDR][E_PHENTSIZE] != 32:
      raise
    if f[EHDR][E_SHENTSIZE] != 40:
      raise
  except:
    raise Exception('Error in ELF header for file %s' % (f[FILENAME]))

def parse_program_headers(f):
  try:
    f[FILE].seek(f[EHDR][E_PHOFF])
    f[PHDR] = []
    for j in range(f[EHDR][E_PHNUM]):
      f[PHDR] += [parse_generic(f[FILE].read(f[EHDR][E_PHENTSIZE]), phdr_format)]
  except:
    raise Exception('Error in ELF program headers for file %s' % (f[FILENAME]))

def parse_section_headers(f):
  try:
    f[FILE].seek(f[EHDR][E_SHOFF])
    f[SHDR] = []
    for j in range(f[EHDR][E_SHNUM]):
      f[SHDR] += [parse_generic(f[FILE].read(f[EHDR][E_SHENTSIZE]), shdr_format)]
  except:
    raise Exception('Error in ELF section headers for file %s' % (f[FILENAME]))

def read_string_table(f):
  try:
    f[FILE].seek(f[SHDR][f[EHDR][E_SHSTRNDX]][SH_OFFSET])
    f[STRTAB] = f[FILE].read(f[SHDR][f[EHDR][E_SHSTRNDX]][SH_SIZE])
    if len(f[STRTAB]) != f[SHDR][f[EHDR][E_SHSTRNDX]][SH_SIZE]:
      raise
  except:
    raise Exception('Error in ELF string table for file %s' % (f[FILENAME]))

def read_elf_info(f):
  parse_elf_header(f)
  parse_program_headers(f)
  parse_section_headers(f)
  read_string_table(f)

def process_input_files(infiles, physaddr):
  curloc = infiles[0][EHDR][E_EHSIZE]
  for f in infiles:
    for p in f[PHDR]:
      curloc += f[EHDR][E_PHENTSIZE]
  for f in infiles:
    fbot = 0
    ftop = 0
    falign = 0
    for p in f[PHDR]:
      if p[P_TYPE] == 1 and p[P_FILESZ] > 0:
        if ftop == 0:
          fbot = p[P_OFFSET]
          ftop = p[P_OFFSET]+p[P_FILESZ]
          falign = p[P_ALIGN]
        else:
          fbot = min(fbot, p[P_OFFSET])
          ftop = max(ftop, p[P_OFFSET]+p[P_FILESZ])
          falign = max(falign, p[P_ALIGN])
    if falign == 0:
      falign = 1
    ''' Need to increase curloc until it is congruent to fbot modulo falign '''
    curloc += ((fbot%falign)+falign-(curloc%falign))%falign
    f[OUTPUT_OFFSET] = curloc
    f[COPY_BOT] = fbot
    f[COPY_TOP] = ftop
    f[FILEALIGN] = falign
    curloc += (ftop-fbot)
  ''' Above figured out where we put all of the data in the output file '''
  ''' Now figure out where we put it in memory '''
  curloc = physaddr
  for f in infiles:
    mbot = 0
    mtop = 0
    malign = 0
    for p in f[PHDR]:
      ''' Only examining program headers of type LOAD and non-zero size '''
      if p[P_TYPE] == 1 and p[P_MEMSZ] > 0:
        if mtop == 0:
          mbot = p[P_VADDR]
          mtop = p[P_VADDR]+p[P_MEMSZ]
          malign = p[P_ALIGN]
        else:
          mbot = min(mbot, p[P_VADDR])
          mtop = max(mtop, p[P_VADDR]+p[P_MEMSZ])
          malign = max(malign,p[P_ALIGN])
    if malign == 0:
      malign = 1
    ''' Need to increase curloc until it is congruent to mbot modulo malign '''
    curloc += ((mbot%malign)+malign-(curloc%malign))%malign
    f[OUTPUT_PHYSADDR] = curloc
    f[VMEM_BOT] = mbot
    f[VMEM_TOP] = mtop
    f[MEMALIGN] = malign
    curloc += (mtop-mbot)
    ''' Round up to next 1MB boundary '''
    curloc = (curloc+0xFFFFF) & ~0xFFFFF

def populate_output_file(f, infiles):
  firstfile = infiles[0]
  lastfile = infiles[len(infiles)-1]
  f[PHDR] = []
  for i in infiles:
    for p in i[PHDR]:
      np = p.copy()
      p[OUTCOPY] = np
      if np[P_TYPE] == 6:
        ''' If the program header is of type PHDR, leave it alone '''
        pass
      else:
        if np[P_OFFSET] >= i[COPY_BOT] and np[P_OFFSET] <= i[COPY_TOP]:
          np[P_OFFSET] = i[OUTPUT_OFFSET] + (np[P_OFFSET] - i[COPY_BOT])
        if np[P_VADDR] >= i[VMEM_BOT] and np[P_VADDR] <= i[VMEM_TOP]:
          np[P_PADDR] = i[OUTPUT_PHYSADDR] + (np[P_VADDR] - i[VMEM_BOT])
      f[PHDR] = f[PHDR] + [np]
  f[SHDR] = [firstfile[SHDR][0]]
  f[STRTAB] = ''
  new_strtab_entry(f, '\0.shstrtab')
  for i in infiles:
    for s in i[SHDR]:
      if s[SH_TYPE] != 0:
        if (s[SH_FLAGS] & 2) != 0 or string_lookup(i, s[SH_NAME]) == '.comment':
          sp = s.copy()
          if sp[SH_OFFSET] >= i[COPY_BOT] and sp[SH_OFFSET] <= i[COPY_TOP]:
            sp[SH_OFFSET] = i[OUTPUT_OFFSET] + (sp[SH_OFFSET] - i[COPY_BOT])
          if i[OS_IMAGE]:
            ''' Don't rename sections in the OS image '''
            sp[SH_NAME] = new_strtab_entry(f, string_lookup(i, s[SH_NAME]))
          else:
            sp[SH_NAME] = new_strtab_entry(f, string_lookup(i, s[SH_NAME])+'.'+i[APPNAME])
          f[SHDR] = f[SHDR] + [sp]
        else:
          pass
  new_strtab_entry(f, '\0')
  f[SHDR] += [firstfile[SHDR][firstfile[EHDR][E_SHSTRNDX]]]
  f[SHDR][len(f[SHDR])-1][SH_NAME] = 1
  f[EHDR] = firstfile[EHDR].copy()
  f[EHDR][E_PHOFF] = 52
  f[EHDR][E_ENTRY] = firstfile[OUTPUT_PHYSADDR]
  f[EHDR][E_SHOFF] = alignup(lastfile[OUTPUT_OFFSET] + (lastfile[COPY_TOP] - lastfile[COPY_BOT]), 4)
  f[EHDR][E_PHNUM] = len(f[PHDR])
  f[EHDR][E_SHNUM] = len(f[SHDR])
  f[EHDR][E_SHSTRNDX] = len(f[SHDR])-1

def dump_output_file(f, infiles):
  f[FILE].write(elf_format(f[EHDR]))
  for p in f[PHDR]:
    f[FILE].write(elf_format(p))
  for i in infiles:
    padzero(f[FILE], i[OUTPUT_OFFSET]-f[FILE].tell())
    if i[OS_IMAGE]:
      ''' Need to copy 64 bytes, then replace some bytes with our EIP info '''
      ''' Then finish from the input file '''
      copydata(f[FILE], i[FILE], i[COPY_BOT], 64)
      ''' If the next 8 bytes read from the input file are all zero, then '''
      '''  we have to use old EIP mode.  The OS isn't prepared for a new '''
      '''  EIP format '''
      tmpstring = i[FILE].read(8)
      if old_eip_mode or tmpstring == zstr[0:8]:
        z = old_write_eip_info(f, infiles)
      else:
        z = write_eip_info(f, infiles)
      copydata(f[FILE], i[FILE], i[COPY_BOT] + 64 + z, i[COPY_TOP]-i[COPY_BOT] - 64 - z)
    else:
      copydata(f[FILE], i[FILE], i[COPY_BOT], i[COPY_TOP]-i[COPY_BOT])
  padzero(f[FILE], f[EHDR][E_SHOFF]-f[FILE].tell())
  f[SHDR][len(f[SHDR])-1][SH_OFFSET] = f[FILE].tell()+len(f[SHDR])*f[EHDR][E_SHENTSIZE]
  f[SHDR][len(f[SHDR])-1][SH_SIZE] = len(f[STRTAB])
  for s in f[SHDR]:
    f[FILE].write(elf_format(s))
  f[FILE].write(f[STRTAB])
  f[FILE].flush()
  f[FILE].close()

def adjust_input_file(i, newnamefunc):
  outname = newnamefunc(i[FILENAME])
  inplace = (outname == i[FILENAME])
  # filemode = ('r+b' if inplace else 'wb')
  # Can't use above line with Python 2.4, try this:
  filemode = ('wb','r+b')[inplace]
  nfile = open(outname, filemode)
  i[EHDR][E_ENTRY] = i[OUTPUT_PHYSADDR]
  nfile.write(elf_format(i[EHDR]))
  for p in i[PHDR]:
    p[P_PADDR] = p[OUTCOPY][P_PADDR]
    nfile.write(elf_format(p))
  if not inplace:
    ''' Copy rest of input file. '''
    # i[FILE].seek(0, os.SEEK_END)
    # Can't use above line with Python 2.4, try this:
    i[FILE].seek(0, 2)
    copydata(nfile, i[FILE], nfile.tell(), i[FILE].tell() - nfile.tell())
  nfile.flush()
  nfile.close()

def splitname(s, restart):
  names = s.split('=')
  if len(names) == 1:
    return (os.path.basename(s).replace('elf','pbn'), s, restart)
  if len(names) == 2:
    return (names[0], names[1], restart)
  raise Exception("input file argument cannot contain multiple '=' characters")

def parse_args(argv):
  global old_eip_mode
  global no_adjust
  opts, args = getopt.gnu_getopt(argv[1:], "eno:p:t:R:")
  optdict = dict()
  for opt, arg in opts:
    optdict[opt] = optdict.get(opt,[]) + [arg]
  old_eip_mode = '-e' in optdict
  no_adjust = '-n' in optdict
  outfilename = optdict.get('-o',[])
  physaddr = optdict.get('-p',[])
  restarts = optdict.get('-R', [])
  args = args+restarts
  if len(args) == 0:
    raise Exception('must provide at least one input file')
  if len(outfilename) != 1:
    raise Exception('must provide exactly one output file (-o)')
  if len(physaddr) != 1:
    raise Exception('must provide exactly one physical address (-p)')
  try:
    physaddr = long(physaddr[0], 0)
    if physaddr < 0 or physaddr >= pow(2,32):
      raise
    if physaddr & 0xFFF:
      raise
  except:
    raise Exception('physical address (-p) must be a 32-bit unsigned integer on a 4K boundary')
  args = [splitname(arg, arg in restarts) for arg in args]
  return (args, outfilename[0], physaddr)

def rename_input_file(ifilename):
  ''' Replace the last dot (.) in the name with _reloc. '''
  ''' If no dot, this returns unchanged... '''
  return '_reloc.'.join(ifilename.rsplit('.',1))

def qurt_image_build(argv):
  try:
    infilenames, outfilename, physaddr = parse_args(argv)
    ''' Create a list of dictionaries, one for each of the input files '''
    infiles=[{APPNAME: f[0], FILENAME: f[1], OS_IMAGE: False, FILE:open(f[1],'rb'), RESTART:f[2]} for f in infilenames]
    infiles[0][OS_IMAGE] = True
    for f in infiles:
      read_elf_info(f)
    ''' Now figure out our basic relocation information '''
    process_input_files(infiles, physaddr)
    outfile = {APPNAME: '', FILENAME: outfilename, OS_IMAGE: True, FILE:open(outfilename, 'wb')}
    ''' Populate the output file structure '''
    populate_output_file(outfile, infiles)
    dump_output_file(outfile, infiles)
    if not no_adjust:
      for i in infiles:
        adjust_input_file(i, rename_input_file)
    return 0
  except Exception, err:
    print >> sys.stderr, "%s: %s" % (argv[0], str(err))
    return 1

sys.exit(qurt_image_build(sys.argv))
