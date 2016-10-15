/*
||  Global variables for the qurtos directory.
||
||  When included with the macro GLOBALS set to 1,
||   define the global variables within (except for
||   weak references).  Otherwise, declare the global
||   variables but do not define them.
||
||  Each global variable is defined using one or more
||    standard macros.  Examples:
||
||  Declare a ZI integer called foobar and a ZI array of
||   100 void pointers called ptrbase:
||
||      GLOBAL int foobar;
||      GLOBAL void *ptrbase[100];
||
||  Declare an initialized const integer array of the
||   first ten prime numbers:
||
||      GLOBAL const int primes[] INIT({2,3,5,7,11,13,17,19,23,29});
||
||  Declare an initialized variable into a particular segment:
||
||      GLOBAL unsigned foo SEGMENT("foo_data") INIT(12345);
||
||  Declare a weak reference to an external array:
||
||      WEAKREF int appinfo[8];
||
||  Declare an extern reference to a variable barfoo (must be
||   declared externally to qurtos directory):
||
||      EXTERN int barfoo;
*/

#if GLOBALS == 1
#define GLOBAL
#define INIT(x,y...) = x,y
#define SEGMENT(x) __attribute__((section(x)))
#define WEAKREF extern __attribute__((weak))
#define EXTERN extern
#else
#define GLOBAL extern
#define INIT(x,...)
#define SEGMENT(x)
#define WEAKREF extern __attribute__((weak))
#define EXTERN extern
#endif

extern mem_pool_t kernel_virt_pool;
extern qurt_mutex_t space_lock;
extern spinlock_t shmem_lock;
extern qurt_timers_group_type timer_groups[QURT_TIMER_MAX_GROUPS];
extern qurtos_mem_heap_type qurtos_heap_struct;
extern u32_t max_heap_used;
extern u32_t max_regions_alloced;
extern unsigned char *DL_swap_pool0;
extern unsigned char *DL_swap_pool1;
extern mem_t *swapping_pools[2];
extern phys_mem_region_t *qurtos_region_allocate_list;
extern shmem_t *qurtos_shmem_allocate_list;
extern struct qurtos_thread_info timerIST_thread_info;
extern struct QURT_ugp_ptr timerIST_ugp_area;
extern int default_pool_index;
extern mem_pool_t default_virt_pool;
extern space_t *g_spaces[MAX_ASIDS];
extern unsigned image_pend;
extern unsigned image_pstart;
extern unsigned image_vend;
extern unsigned image_vstart;
extern space_t *kernel_space;
extern qurt_mutex_t mem_lock;
extern qurt_mutex_t print_lock;
extern void *QURTK_pagetables[];
extern u64_t QURTK_tlb_dump[];
extern int QURTK_tlb_first_replaceable;
extern u32_t QURTK_trace_buffers[];
extern u32_t QURTK_trace_mask_save;
extern qurtos_obj_cache_t qurtos_memory_cache;
extern u32_t qurtos_pgsize_encode_to_size[7];
extern qurtos_obj_cache_t qurtos_pgtbl_cache;
extern qurtos_obj_cache_t qurtos_region_cache;
extern qurtos_obj_cache_t qurtos_shmem_cache;
extern qurtos_obj_cache_t qurtos_space_cache;
extern qurt_mutex_t qurtos_thread_lock;
extern qurtos_obj_cache_t qurtos_vma_cache;
extern int qurt_timer_base;
extern int qurt_timer_int_num;
extern qurt_mutex_t qurt_timer_lock;
extern int qurt_tlb_idx;
extern unsigned _SDA_BASE_;
extern void *stackTop;
extern unsigned QURTK_tlb_last_replaceable;
extern QURTK_thread_context QURTK_fastint_contexts[MAX_HTHREADS];
extern struct {
   unsigned executable_address;
   unsigned executable_end;
   unsigned readwrite_address;
   unsigned readwrite_end;
   unsigned entry_point;
   unsigned stack_pointer;
   unsigned gp_value;
   unsigned asid_number;
} user_start[1] __attribute__((weak));
extern char start[];
extern int DL_SWAP_POOL0_SIZE __attribute__((weak));
extern int DL_SWAP_POOL1_SIZE __attribute__((weak));
extern uint32_t __stack_chk_guard __attribute__((weak));
extern uint32_t image_size_in_pages;
extern uint32_t image_pageno_virt_first;
extern uint32_t image_pageno_virt_last;
extern uint32_t image_pageno_phys_first;
extern uint32_t image_pageno_phys_last;
extern phys_pool_t *qurtos_phys_pool_list_head;
extern qurt_mutex_t qurtos_phys_pool_list_lock;
