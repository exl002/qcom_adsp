#ifndef   QURTOS_INTERNAL_H
#define   QURTOS_INTERNAL_H

/*
||  qurtos_internal.h
||
||  QuRT OS has both a kernel portion which runs in monitor
||   mode and a qurtos portion which runs in guest-OS mode.
||
||  This include file contains macros, types, definitions,
||   declarations, etc., which are specific to the qurtos
||   portion of QuRT.
||
||  Nothing in this file is intended to be a public API and
||   everything in this file is to be considered subject to
||   change at any time.
*/

#include <qurt_config_vars.h>

#include <stddef.h>
#include <qurtk_stddef.h>
#include <space.h>
#include <consts.h>
#include <context.h>
#include <tlb.h>
#include <qurt_util_macros.h>

//#define _SIZE_T unsigned

#include <qurt_alloc.h>
#include <qurt_atomic_ops.h>
#include <qurt_defs_internal.h>
#include <qurt_memory.h>
#include <qurt_mutex.h>
#include <qurt_pimutex.h>
#include <qurt_qdi_internal.h>
#include <qurt_qdi_driver.h>
#include <qurt_space.h>
#include <qurt_process.h>
#include <qurt_types.h>
#include <qurt_utcb.h>
#include <qurt_lifo.h>
#include <qurt_int.h>
#include <qurt_sem.h>
#include <qurt_event.h>
#include <qurt_thread_context.h>
#include <qurtos.h>
#include <qurtos_methods.h>
#include <qurtos_constants.h>
#include <qurtos_ktraps.h>
#include <qurtos_shmem.h>
#include <qurtos_timer_libs.h>
#include <qurt_timer_hw.h>
#include <qurt_sclk.h>
#include <obj_cache.h>
#include <memheap_lite.h>
#include <heap.h>
#include <qurtos_sclk.h>
#include <debug_monitor.h>
#include <stringK.h>

#ifdef __QDSP6_ARCH__
#include <q6protos.h>
#else
#include <hexagon_protos.h>
#endif

struct qurtos_generic_method_handler {
   int method __attribute__((aligned(8)));
   unsigned handler;
};

struct qurtos_client_info {
   unsigned gp_value;
   unsigned ssr_value;
   unsigned ccr_value;
   unsigned usr_value;
   struct space *space;   /* Pointer to space structure for this client */
};

struct qurtos_user_client {
   qurt_qdi_obj_t qdiobj;
   int handle;
   unsigned asid;
   struct qurtos_client_info client_info;
   QURT_utcb_t *user_utcb;
   int exitstatus;

   qurt_mutex_t threadlist_mutex;
   struct qurtos_thread_info *threadlist;
   qurt_mutex_t usermalloc_mutex;
   unsigned usermalloc_base;     /* Virtual address of base of usermalloc heap */
   unsigned usermalloc_len;      /* Size of usermalloc heap */
   unsigned usermalloc_corrupt;
   union memhdr *first_memhdr;
   unsigned usermalloc_block_count;
   char name[64];

   qurt_qdi_obj_t **htable;
   int hmax;
   int htoffset;
   void *err_thread_info;
   void *memfree;
   phys_pool_t hlos_pool;
   struct phys_mem_pool_config hlos_pool_config;
};

struct qurtos_thread_info {
   qurt_qdi_obj_t qdiobj __attribute__((aligned(8)));    /* Forces entire structure to be 8-byte aligned */
   QURTK_thread_context *pTcb;
   QURTK_thread_context *pTcbParent;
   struct qurtos_thread_info *exited_children;
   struct qurtos_thread_info *pNext;
   struct qurtos_thread_info *pJoiner;
   struct qurtos_thread_info *pClientNext;
   int thread_handle;
   int exit_status;
   int asid;
   int join_status;
   int remote_client_handle;
   void (*pfnCleanup)(void *);
   void *pArgCleanup;
   struct qurtos_thread_info *reaper_prev;
   struct qurtos_thread_info *reaper_next;
};

void qurtos_kernel_set_space_main(unsigned asid, QURTK_thread_context *main);
qurt_mem_pool_t qurtos_kernel_get_user_pool(void);

void qurtos_thread_id_list_init(void);
int qurtos_thread_id_list_add(unsigned int handle);
int qurtos_thread_id_list_remove(unsigned int handle);

void qurtos_thread_init(void);
struct qurtos_thread_info *qurtos_thread_info_init(struct qurtos_thread_info *pInfo, QURTK_thread_context *pTcb, int client_handle);

void qurtos_rand_late_init(void);
void qurtos_app_heap_late_init(void);
void qurtos_space_early_init(void);
void qurtos_qdi_early_init(void);
void qurt_qdi_initialize(void *td);
void qurt_qdi_local_client_init(void);
void qurtos_memory_init(void);
int qurt_qdi_client_signal_group_new(int, int *, int *);
void qurtos_thread_generic_init(void);
void qurtos_space_generic_init(void);
void qurtos_user_client_generic_init(void);
void qurtos_reaper_generic_init(void);
void qurtos_thread_context_generic_init(void);
void qurtos_timer_server_generic_init(void);
void qurtos_sclk_generic_init(void);
void qurtos_memory_generic_init(void);
void qurtos_debugger_generic_init(void);

int qurtos_user_client_constructor(struct qurtos_user_client *me, unsigned asid, struct qurtos_client_info *, const char *name, int handle, void *memfree);
int qurtos_user_client_destroy(struct qurtos_user_client *me);

int qurtos_usermalloc_init(struct qurtos_user_client *);
int qurtos_usermalloc_imp(struct qurtos_user_client *, unsigned, void **);
int qurtos_userfree_imp(struct qurtos_user_client *, void *);

void qurtos_space_inform_process_exit(int asid, int status);
space_t *get_space_from_client_handle(int handle);
space_t * get_space_from_asid(int asid);
void qurtos_mark_trace_memory(unsigned, unsigned);
void zero_mem_range_c(unsigned, unsigned);
int tlb_remove_mapping(unsigned);
void qurtos_timer_init(void);
int wrap_mem_region(int client_handle, vma_node_t *vma, bool is_static);
int mem_pool_attachK(int, size_t, const char *name, uint32_t *pool);
int mem_pool_attr_get(uint32_t pool_obj, struct phys_mem_pool_config *attr); 
int mem_region_createK(u32_t *obj,u32_t size,mem_pool_t *pool,mem_region_attr_t *attr, int);
int mem_region_queryK(int client_handle, u32_t *vaddr, u32_t ppn_start, u32_t ppn_end, mem_cache_mode_t cache_attribs, rwx_t perm);
int mem_create_mappingK (int client_handle, u32_t vaddr, u32_t ppn, u32_t p_size, mem_cache_mode_t cache_attribs, rwx_t perm);
int mem_remove_mappingK (int client_handle, u32_t vaddr, u32_t ppn, u32_t p_size);
int mem_lookup_physaddrK (int client_handle, u32_t vaddr, u64_t* paddr);
int qurtos_mem_region_queryK (int client_handle, u32_t *region_handle, u32_t vaddr, u32_t ppn);
int qurtos_mem_fs_change_page_attr ( u32_t addr, u32_t len, rwx_t perm, int client_handle);
int qurtos_mem_fs_region_create (u32_t *region_handle, u32_t addr, size_t size, int flags, int client_handle);
int qurtos_mem_fs_region_get_virtaddr (unsigned int *vaddr, unsigned int region_obj);
int qurtos_mem_fs_region_delete (unsigned int region_handle, int client_handle);
int qurtos_mem_fs_region_query (u32_t *region_handle, unsigned int addr, int client_handle);
int qurtos_timer_create(const qurt_timer_attr_t *attr, const qurt_anysignal_t *signal, unsigned int mask);
int qurtos_timer_stop(qurt_timer_t timer);
int qurtos_timer_delete(qurt_timer_t timer);
int qurtos_timer_restart(qurt_timer_t timer, qurt_timer_duration_t duration);
int qurtos_timer_sleep(qurt_timer_duration_t duration, qurt_anysignal_t *signal, unsigned int mask);
int qurtos_timer_get_attr(qurt_timer_t timer, qurt_timer_attr_t *attr);
int qurtos_timer_get_ticks(qurt_timetick_type *ticks);
int qurtos_timer_group_enable(unsigned int group);
int qurtos_timer_group_disable(unsigned int group);
void qurtos_timer_recover_pc(void);
int qurtos_sysclock_get_hw_ticks(unsigned long long *ticks);
int do_printf(const char *format, va_list args);
u64_t hw_timer_match_val(void);
void qurt_sysclock_init(void);
unsigned qurtos_rand32(void);

void shmem_region_create_from_static_mapping(
                mem_pool_t*,
                vma_node_t **vma, 
                space_t *space, 
                u32_t page_count, 
                u32_t pageno_phys,
                u32_t pageno_virt, 
                mem_cache_mode_t cache_mode, 
                perm_t perm);

u32_t lookup_user_mapping (space_t *space, u32_t vaddr);
qurt_qdi_obj_t *qurtos_get_local_client(void);

int qurt_qdi_state_local_new_handle_from_obj(qurt_qdi_obj_t **table,
                                             int max,
                                             qurt_qdi_obj_t *objptr);
int qurt_qdi_state_local_release_handle(qurt_qdi_obj_t **table,
                                        int max,
                                        int h);
int qurt_qdi_obj_ref_inc(qurt_qdi_obj_t *);
int qurt_qdi_obj_ref_dec(qurt_qdi_obj_t *);

void qurtos_start_reaper(unsigned long long *stack,
                         unsigned stack_size,
                         struct qurtos_thread_info *info,
                         struct QURT_ugp_ptr *ugp);

int qurtos_qdi_generic_entry(int client_handle, qurt_qdi_obj_t *objptr, int method,
                             qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3,
                             qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                             qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9);

void qurtos_qdi_generic_early_init(void);

void qurtos_qdi_generic_register_methods(const struct qurtos_generic_method_handler *, unsigned);
int qurtos_getasid(void);
void QURTK_zero_mem_range(void *mem_start, void *mem_end);

#define QURTOS_ASIZE(x) (sizeof(x)/sizeof((x)[0]))
#define QURTOS_GENERIC_FN(pfn) ((unsigned)(pfn))
#define QURTOS_GENERIC_OBJ(objptr) ((unsigned)(objptr)+1)
#define QURTOS_ERROR(e) (-(int)(e))

#include <qurtos_global_vars.h>

#ifdef CONFIG_PRIORITY_INHERITANCE
#define qurtos_timer_lock(lock)     qurt_pimutex_lock(lock)
#define qurtos_timer_unlock(lock)   qurt_pimutex_unlock(lock)
#else
#define qurtos_timer_lock(lock)     qurt_rmutex_lock(lock)
#define qurtos_timer_unlock(lock)     qurt_rmutex_unlock(lock)
#endif


#endif // QURTOS_INTERNAL_H
