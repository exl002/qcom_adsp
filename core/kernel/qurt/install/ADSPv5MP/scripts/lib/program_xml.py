##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2007, Open Kernel Labs, Inc.
# 
# All rights reserved. 
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by National ICT Australia, free of charge, to
# any person obtaining a copy of this software and any associated
# documentation files (the "Software") to deal with the Software without
# restriction, including (without limitation) the rights to use, copy,
# modify, adapt, merge, publish, distribute, communicate to the public,
# sublicense, and/or sell, lend or rent out copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimers.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimers in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of National ICT Australia, nor the names of its
#       contributors, may be used to endorse or promote products derived
#       from this Software without specific prior written permission.
# 
# EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
# PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
# NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
# WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
# REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
# THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
# ERRORS, WHETHER OR NOT DISCOVERABLE.
# 
# TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
# NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
# THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
# LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
# OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
# OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
# OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
# CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
# CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
# DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
# CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
# DAMAGES OR OTHER LIABILITY.
# 
# If applicable legislation implies representations, warranties, or
# conditions, or imposes obligations or liability on National ICT
# Australia or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of National ICT Australia or the contributor is limited, to
# the full extent permitted by the applicable legislation, at its
# option, to:
# a.  in the case of goods, any one or more of the following:
# i.  the replacement of the goods or the supply of equivalent goods;
# ii.  the repair of the goods;
# iii. the payment of the cost of replacing the goods or of acquiring
#  equivalent goods;
# iv.  the payment of the cost of having the goods repaired; or
# b.  in the case of services:
# i.  the supplying of the services again; or
# ii.  the payment of the cost of having the services supplied again.
# 
# The construction, validity and performance of this licence is governed
# by the laws in force in New South Wales, Australia.
"""
Processing of the program XML element.
"""

from ezxml import Element, long_attr, str_attr, size_attr
import re
import sys

Osam_stack_fill_count_el = Element("osam_stack_fill_count",
                    value = (size_attr, "optional"))

Stack_size_el = Element("stack_size",
                    value = (size_attr, "required"))

App_heap_size_el = Element("heap_size",
                    name = (str_attr, "required"),
                    type = (str_attr, "optional"),
                    value = (size_attr, "required"))

MainPrio_el = Element("main_priority",
                    value = (str_attr, "required"))

MainBitmask_el = Element("main_bitmask",
                    value = (str_attr, "optional"))

ReaperPrio_el = Element("reaper_priority",
                    value = (str_attr, "required"))

ReaperTcbPartition_el = Element("reaper_tcb_partition",
                    value = (str_attr, "required"))

ReaperStackSize_el = Element("reaper_stack_size",
                             value = (size_attr, "optional"))

MaxPiPrio_el = Element("max_pimutex_prio",
                    value = (str_attr, "optional"))

TlbRepl_el = Element("tlb_first_replaceable",
                    value = (str_attr, "optional"))
                    
Memsection_el = Element("memsection",
                    glob = (str_attr, "optional"),
                    tlb_lock = (str_attr, "optional"),
                    direct = (str_attr, "optional"),
                    attach = (str_attr, "required"),
                    physpool = (str_attr, "optional"),
                    virt_addr = (long_attr, "required"),
                    phys_addr = (long_attr, "optional"),
                    user_mode = (str_attr, "optional"),
                    size = (size_attr, "required"),
                    cache_policy = (str_attr, "optional"))

Irq_el = Element("irq",
                    num = (str_attr, "required"),
                    trigger = (str_attr, "required"),
                    polarity = (str_attr, "optional"),
                    l2base = (str_attr, "optional"),
                    l2trigger = (str_attr, "optional"),
                    l2polarity = (str_attr, "optional"))
Interrupt_el = Element("interrupt",
                    Irq_el)

Program_el = Element("program",
                    Osam_stack_fill_count_el,
                    Stack_size_el,
                    App_heap_size_el,
                    MainPrio_el,
                    MainBitmask_el,
                    ReaperPrio_el,
                    ReaperTcbPartition_el,
                    ReaperStackSize_el,
                    MaxPiPrio_el,
                    TlbRepl_el,
                    Memsection_el,
                    Interrupt_el,
                    name = (str_attr, "required"))
                      
class MemSection:
    """
    Represents a memsection. Derives attributes from the parsed element.
    Consists of methods to perform consistency checks and obtain optional
    attributes.
    """
    VALID_PAGE_SIZES=[0x1000, 0x4000, 0x10000, 0x40000, 0x100000, 0x400000, 0x1000000]
    def __init__(self, memsec_el):
        self.glob = '0'
        self.lock = '0'
        self.direct = 'false'
        self.virt_addr = memsec_el.virt_addr
        self.user_mode = 'true'
        self.size = memsec_el.size
        self.attach = memsec_el.attach

        if hasattr (memsec_el, 'glob'):
            self.glob = memsec_el.glob
        if hasattr (memsec_el, 'tlb_lock'):
            self.lock = memsec_el.tlb_lock
        if hasattr (memsec_el, 'physpool'):
            self.pool= memsec_el.physpool
        if hasattr (memsec_el, 'direct'):
           self.direct = memsec_el.direct
        if hasattr (memsec_el, 'phys_addr'):
           self.phys_addr = memsec_el.phys_addr
        if hasattr (memsec_el, 'user_mode'):
           self.user_mode = memsec_el.user_mode
       
        if hasattr (memsec_el, 'cache_policy'):
           self.cache_policyname = memsec_el.cache_policy

    def setCachePolicy(self, policy_list):
        if hasattr (self, 'cache_policyname'):
            for (value, name) in policy_list:
                if name == self.cache_policyname:
                    self.cache_policy = value
                    break
        else:
            self.cache_policy = '0x7'

    def isTlbLock (self):
        if self.lock == '1':
            return True
        else:
            return False

    def queryUserMode (self):
        if self.user_mode == 'false':
            return '0'
        else:
            return '1'

    def setPhysAddr(self):
        if self.direct == 'true':
           self.phys_addr = self.virt_addr

    def isValidSize(self):
        if self.size in MemSection.VALID_PAGE_SIZES:
            return True
        else:
            return False

    def isValidAlign(self):
        phys_offset = self.phys_addr & (self.size - 1)
        virt_offset = self.virt_addr & (self.size - 1)
        if (phys_offset != 0)  or (virt_offset != 0):
            return False
        return True

    def isValidPhysPool(self, regions):
        for (base, size) in regions:
            if self.phys_addr >= base and self.phys_addr <= (base + size - 1) and (self.phys_addr + self.size - 1) >= base and (self.phys_addr + self.size - 1) <= (base + size - 1):
                return True
        return False

    def checkConsistency(self):
        if not hasattr (self, 'phys_addr'):
            print 'No phys_addr'
            return False

        if not hasattr (self, 'cache_policy'):
            print 'Invalid cache_policy'
            return False

        if not self.isValidSize():
            print 'Invalid size'
            return False

        #import pdb; pdb.set_trace()
        if not self.isValidAlign():
            print "Improper alignment of phys addr: %s or virt addr: %s" % (hex(self.phys_addr), hex(self.virt_addr))
            return False

        # Every thing looks good for this memsection
        return True

def  checkMemOverlap (mem_objs):
    if len (mem_objs) < 2:
        return True

    # Check if overlapping virtual addresses and conflicting cache attributes exist
    for i in range(len(mem_objs) - 1):
        x = mem_objs[i]
        for j in range (i+1, len(mem_objs)):
            y = mem_objs[j]
            # Overlapping virtual addresses ?
            if x.virt_addr >= y.virt_addr and x.virt_addr <= (y.virt_addr + y.size - 1):
                print "Virt addr, size : (%s, %s) Virt addr, size:  (%s, %s) overlap" % (hex(x.virt_addr), hex(x.size), hex(y.virt_addr), hex(y.size))
                return False
            if x.virt_addr <= y.virt_addr and (x.virt_addr + x.size - 1) >= y.virt_addr:
                print "Virt addr, size : (%s, %s) Virt addr, size:  (%s, %s) overlap" % (hex(x.virt_addr), hex(x.size), hex(y.virt_addr), hex(y.size))
                return False

            # Conflicting cache attributes ?
            if x.phys_addr >= y.phys_addr and x.phys_addr <= (y.phys_addr + y.size - 1):
                if x.cache_policy != y.cache_policy:
                    print "Virt addr, cache policy : (%s, %s) Virt addr, cache policy:  (%s, %s) conflict" % (hex(x.virt_addr), hex(x.cache_policy), hex(y.virt_addr), hex(y.cache_policy))
                    return False
            if x.phys_addr <= y.phys_addr and (x.phys_addr + x.size - 1) >= y.phys_addr:
                if x.cache_policy != y.cache_policy:
                    print "Virt addr, cache policy : (%s, %s) Virt addr, cache policy:  (%s, %s) conflict" % (hex(x.virt_addr), hex(x.cache_policy), hex(y.virt_addr), hex(y.cache_policy))
                    return False

    return True

def collect_memsections (parsed, asic):
    machine_el = parsed.find_child("machine")
    program_el = parsed.find_child("program")
    physpool_el = parsed.find_children("physical_pool")

    # Extract Cache policy definitions
    cache_policies = [] # [(value, name), ....]
    cache_policies = [(cache_el.value, cache_el.name) for cache_el in machine_el.find_children("cache_policy")]
    # Extract memsection information
    mem_sections = []
    mem_obj_list = []
    for el in program_el.find_children("memsection"):
        mem_obj = MemSection(el)
        # Set cache policy
        mem_obj.setCachePolicy(cache_policies)
        # Set Phys addr
        mem_obj.setPhysAddr()
        # Sanity check the object
        if not mem_obj.checkConsistency():
            sys.exit ("!!! In consistent memsection: \n %s \n" % (el))

        # Make the phys pool is valid
        if hasattr(mem_obj, 'pool'):
            pool_list = None
            for pool in physpool_el:
                if pool.name == mem_obj.pool:
                    pool_list = [(region_el.base, region_el.size) for region_el in pool.find_children("region")]
                    if not mem_obj.isValidPhysPool(pool_list):
                        sys.exit ("!!! Pool in memsection doesn't match phypool: \n %s \n" % (el))
                    break;
            if pool_list is None:
                sys.exit ("!!! Invalid Pool name in memsection: \n %s \n" % (el))

        mem_obj_list.append(mem_obj)

    if not checkMemOverlap(mem_obj_list):
         sys.exit ("!!! Memsections and Physpool fail in overall consistency \n")
    return mem_obj_list

def collect_program_element(parsed, ignore_name, asic):
    """Collect the attributes of the program element."""

    symbols = {}
    symbols = collect_program_interrupts(parsed)

    program_el = parsed.find_child("program")

    pgsize = {}
    str_tlblock = ""
    str = ""
    tlb_entries=0

    mem_sections = collect_memsections(parsed, asic)

    if asic == "v2" or asic == "v3":
        pgsize = {0x1000: "0", 0x4000: "1", 0x10000: "2", 0x40000: "3", 0x100000: "4", 0x400000: "5", 0x1000000: "6"}
    else:
        pgsize = {0x1000: "1", 0x4000: "2", 0x10000: "4", 0x40000: "8", 0x100000: "16", 0x400000: "32", 0x1000000: "64"}

    for obj in mem_sections:
        if obj.isTlbLock ():
            tlb_entries = tlb_entries + 1
            str_tlblock += "\\\n   MEMORY_MAP(" + obj.glob + ", 0, " + hex(obj.virt_addr>>12) + ", "
            str_tlblock += obj.queryUserMode() + ", "
            str_tlblock += obj.attach.upper() + ", " + obj.cache_policy + ", " + pgsize[obj.size] + ", MAIN, " + hex(obj.phys_addr>>12) + "),"
        else:
            str += "\\\n   MEMORY_MAP(" + obj.glob + ", 0, " + hex(obj.virt_addr>>12) + ", "
            str += obj.queryUserMode() + ", "
            str += obj.attach.upper() + ", " + obj.cache_policy + ", " + pgsize[obj.size] + ", MAIN, " + hex(obj.phys_addr>>12) + "),"
      
    if tlb_entries > 63:
        sys.exit("Number of tlb entries exceed the uper limit")
    str=str_tlblock+str
    symbols[re.compile("MEMORY_MAPS")] = str
    symbols[re.compile("TLBLOCKNUM")] = repr(tlb_entries)

    #QURTK_main_priority
    symbols[re.compile("QURTKMAINPRIORITY")] = program_el.find_child("main_priority").value

    bitmask = hex (0xff)
    if hasattr(program_el, 'main_bitmask'):
        bitmask = program_el.find_child("main_bitmask").value

    #QURTK_main_bitmask
    symbols[re.compile("QURTKMAINBITMASK")] = bitmask
   
    #QURT_reaper_priority
    param = program_el.find_child("reaper_priority")
    if param is None:
        symbols[re.compile("QURTOS_REAPER_PRIORITY")] = "32"
    else:
        symbols[re.compile("QURTOS_REAPER_PRIORITY")] = param.value

    #QURT_reaper_tcb_partition
    param = program_el.find_child("reaper_tcb_partition")
    if param is None:
        symbols[re.compile("QURTOS_REAPER_TCB_PARTITION")] = "0"
    else:
        symbols[re.compile("QURTOS_REAPER_TCB_PARTITION")] = param.value

    param = program_el.find_child("reaper_stack_size")
    if param is None:
        symbols[re.compile("QURTOSREAPERSTACKSIZE")] = "4096"
    else:
        symbols[re.compile("QURTOSREAPERSTACKSIZE")] = "%u" % param.value

    #QURTK_max_pimutex_prio
    try:
        pimutex_prio = program_el.find_child("max_pimutex_prio").value
    except AttributeError:
        pimutex_prio = "0"
    symbols[re.compile("MAXPIMUTEXPRIO")] = pimutex_prio
    
    try:
        first_repl = program_el.find_child("tlb_first_replaceable").value
        if (int(first_repl) > 128) or (int(first_repl) > 64 and asic != "v5"):
          sys.exit ("!!! First replaceable entry beyond limit \n")
    except AttributeError: 
        first_repl = "0"
        
    #QURTK_tlb_first_replaceable
    symbols[re.compile("TLBFIRSTREPL")] = first_repl;
    
    try:
        osam_count = program_el.find_child("osam_stack_fill_count").value
    except AttributeError:
        osam_count = 128

    #qurt_osam_stack_fill_count
    symbols[re.compile("OSAMSTACKFILLCOUNT")] = hex(osam_count)

    #QURTK_main_stack_size
    symbols[re.compile("MAINSTACKSIZE")] = hex(program_el.find_child("stack_size").value)
    #QURTK_app_heap_size
    symbols[re.compile(r"\bHEAPSIZE\b")] = hex(program_el.find_child("heap_size").value)
    #application heap type
    try:
        heap_type = 0
        heaptypedict = {'static':0, 'dynamic':1, 'aslr':2, 'appregion':3}
        heap_type = heaptypedict[program_el.find_child("heap_size").type.lower()]
    except Exception:
        pass
    symbols[re.compile(r"\bQURTOSAPPHEAPTYPE\b")] = ("%d" % heap_type)

    return symbols



def collect_program_interrupts(parsed):
    """ Collect the interrupt attributes of the program element."""
       
    int_type = [0xffffffff for i in range(32) ]
    int_enable = [0 for i in range(32*16)]
    
    int_symbols = {}
    program_count = 0
    program_names = ""
    interrupt_scnt = 0
    max_int_num = 0
    int_privilege = 0

    for program_el in parsed.find_children("program"):
        if( program_count ==0 ):
            program_names += ( "\"" + program_el.name )
            program_names += ( "\"")
        else:
            program_names += (",\"" + program_el.name)
            program_names += ( "\"")
        program_count = program_count + 1
        interrupt_el = program_el.find_child("interrupt")
        if interrupt_el is not None:
            interrupt_scnt = interrupt_scnt + 1
            for el in interrupt_el.find_children("irq"):
                el_no = int(el.num)
                element_no = el_no/32
                shift_no = el_no%32
                trigger = int_type[element_no]         
                irq_val = 1
                if el.trigger == "edge":
                    trigger = trigger | (irq_val << shift_no)
                else:
                    trigger = ~(~trigger | (irq_val << shift_no))          
                int_type[element_no] = trigger
                enable = int_enable[(program_count-1)*32 + element_no]         
                enable = enable | (irq_val << shift_no)        
                int_enable[(program_count-1)*32 + element_no] = enable
                if(el_no > max_int_num):
                    max_int_num = el_no

    if(interrupt_scnt > 0):
         machine_el = parsed.find_child("machine")
         if machine_el is not None:
             mint_el = machine_el.find_child("interrupt")
             if mint_el is not None:
                 sys.exit ("!!! Interrupt cannot be defined under both Machine and Program sections \n")

         int_privilege = 1
         str_type = ""
         str_type = hex(int_type[0])
         str_enable = ""
         str_enable = hex(int_enable[0])
         count = 1
         while (count < 32):
             str_type += ("," + hex(int_type[count]))
             str_enable += ("," + hex(int_enable[count]))
             count = count + 1
          
         while (count < 32*program_count):   
             str_enable += ("," + hex(int_enable[count]))
             count = count + 1
          
         int_symbols[re.compile("QURTKSIRCTYPE")] = str_type
         int_symbols[re.compile("QURTKL2VICENABLE")] = str_enable
         int_symbols[re.compile("MAXINT")] = repr(max_int_num)

    int_symbols[re.compile("MAXPROGRAMS")] = repr(program_count)
    int_symbols[re.compile("QURTKPROGNAMES")] = program_names
    int_symbols[re.compile("INTPRIV")] = repr(int_privilege)

    return int_symbols



