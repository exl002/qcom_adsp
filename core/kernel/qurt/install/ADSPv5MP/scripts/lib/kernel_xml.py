from ezxml import Element, str_attr, size_attr, long_attr
import re

'''    XML tag             AttrType   Req/Opt       C template regex       Default value (or None)    '''
kernel_db = [
    ["hthread_mask",       str_attr,  "required",    "HTHREADMASK",           None   ],
    ["heap_size",          size_attr, "optional", r"\bKERNELHEAPSIZE\b",      0x1C000],
    ["tlb_debug_level",    long_attr, "optional",    "TLBDEBUGLEVEL",         0      ],
    ["kernel_separation",  long_attr, "optional", r"\bKERNELSEP\b",           0      ],
    ["split_virtual_pool", long_attr, "optional", r"\bSPLITVIRTPOOL\b",       0      ],
    ["max_threads",        str_attr,  "required", r"\bMAXTHREADS\b",          None   ],
    ["max_threads_in_tcm", str_attr,  "optional",    "MAXTHREADSINTCM",       "0"    ],
    ["max_futexes",        str_attr,  "required",    "MAXFUTEXES",            None   ],
    ["qurtos_qdi_handles", str_attr,  "optional",    "QURTOSQDIHANDLES",      "1024" ],
    ["trace_mask",         str_attr,  "required",    "QURTKDEFAULTTRACEMASK", None   ],
    ["trace_size",         size_attr, "required",    "TRACESIZE",             None   ],
    ["fastint_stack_size", size_attr, "optional",    "FASTINTSTACKSIZE",      384    ],
    ["qurtos_stack_size",  size_attr, "optional", r"\bQURTOSSTACKSIZE\b",     0      ],
    ["tcm_size",           size_attr, "optional", r"\bTCMSIZE\b",             32     ],
    ["page_table_size",    size_attr, "optional", r"\bPAGETABLESIZE\b",       0x24000],
    ["debug_buf_size",     size_attr, "optional", r"\bDEBUGBUFSIZE\b",        8192   ]
]

# FIXME: support "fastint_stack size= " until cust_config.xml can move to "fastint_stack_size value= " 
Fastint_stack_el = Element("fastint_stack",
                      size = (str_attr,"optional"))
Kernel_el = Element("kernel", Fastint_stack_el, *[Element(e[0], value=(e[1],e[2])) for e in kernel_db]) 

''' Collect the attributes of the kernel element '''

def collect_kernel_element(parsed, ignore_name, asic):
    k = parsed.find_child("kernel")
    if not k:
        return
    symbols = {}
    for e in kernel_db:
        try:
            v = k.find_child(e[0]).value
        except AttributeError:
            v = e[4]
            if v == None:
                print('Cannot parse kernel attribute ' + e[0])
                raise
        if e[1] == size_attr or e[1] == long_attr:
            v = hex(v)
        symbols[re.compile(e[3])] = v

    # FIXME: support "fastint_stack size= " until cust_config.xml can move to "fastint_stack_size value= " 
    try:
        symbols[re.compile("FASTINTSTACKSIZE")] = k.find_child("fastint_stack").size
    except AttributeError:
        pass
    return symbols
