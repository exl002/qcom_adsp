#ifndef QURTOS_MEMORY_H
#define QURTOS_MEMORY_H
/*=============================================================================
                qurtos_memory.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  QuRT OS memory structures and typedefs
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009 - 2013
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/qurtos/include/qurtos_memory.h#9 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/

#include <qurtk_stddef.h>
#include <stringK.h>
#include <qurt_qdi_driver.h>

/**
 * Cache type 
 *
 * Specifying data cache or instruction cache
 */
typedef enum {
        MEM_ICACHE,
        MEM_DCACHE,
} mem_cache_type_t;
/**
 * Cache operation type
 */
typedef enum { 
    MEM_CACHE_FLUSH,
    MEM_CACHE_INVALIDATE,
    MEM_CACHE_FLUSH_INVALIDATE,
} mem_cache_op_t;


/**
 * Cache flush operation argument structure
 */
typedef struct {
    mem_cache_op_t         operation;
    mem_cache_type_t       type;
    void                   *addr; 
    size_t                 size;
} flush_attr_t;

/**
 * Cache OPT structure is an "ioctl" like structure. It is a union of all
 * possible cache control operations.  Currently it only supports cache flush
 * and will be supporting L2 config in the future 
 */
typedef union{
        flush_attr_t flush;
} mem_cache_ctrl_t;

/**
 * Mapping type
 * 
 * MEM_MAPPING_VIRTUAL is the default mode, in which system will
 * pick up an available range of virtual address, and mapped to
 * contiguous or separated physical address;    
 *
 * In MEM_MAPPING_PHYS_CONTIGUOUS mode, the virtual address might
 * not be  same as the physical address. But physical address of the
 * memory region is guaranteed to be contiguous. 
 */
typedef enum {
        MEM_MAPPING_VIRTUAL=0,
        MEM_MAPPING_PHYS_CONTIGUOUS = 1,
        MEM_MAPPING_IDEMPOTENT=2,
        MEM_MAPPING_VIRTUAL_FIXED=3,
        MEM_MAPPING_NONE=4,
        MEM_MAPPING_VIRTUAL_FIXED_ADDR=5,
        MEM_MAPPING_INVALID=6,
        MEM_MAPPING_VIRTUAL_RANDOM=7,
        MEM_MAPPING_UNKNOWN=0xff,
} mem_mapping_t;

/**
 * memory region type
 *
 * Local region is only accessible within a PD and shared memory region can be
 * shared with other PD. 
 * User access region means that the region is allocated and owned by code
 *  running in the guest-OS, but the memory is accessible from user-mode.
 */ 
typedef enum {
    MEM_REGION_LOCAL=0,
    MEM_REGION_SHARED,
    MEM_REGION_USER_ACCESS,
    MEM_REGION_FS = 4
} mem_region_type_t;

/**
 * Memory attribute
 *
 * Users can specify physaddr in and MEM_MAPPING_PHYS_CONTIGUOUS mode.  Users
 * can not specify virtaddr for a memory region, it can only be queried by
 * mem_attr_getvirtaddr function 
 */
typedef struct {
    mem_mapping_t      mapping_type;
    mem_cache_mode_t   cache_mode;
    unsigned int       ppn;
    unsigned int       virtaddr;
    mem_region_type_t  type;
    size_t             size;
} mem_region_attr_t;

#include "tailq.h"

struct memory {
   uint32_t pageno;
   uint32_t page_count;
   TAILQ_ENTRY(memory) mem_list;
   TAILQ_ENTRY(memory) glob_list;
   uint32_t tf_bit;
};

typedef struct memory mem_t;

TAILQ_HEAD(mem_list, memory);
TAILQ_HEAD(glob_list, memory);

#ifndef CHAR_BIT
#define CHAR_BIT                8
#endif
#define MEM_MIN_BITS            12
#define MEM_MIN_SIZE            (1UL << MEM_MIN_BITS)
#define MEM_MAX_IDX             (sizeof(addr_t) * CHAR_BIT - MEM_MIN_BITS)
#define POOL_UNINITIALIZED      0xFFFFFFFF

struct mem_pool {
    struct mem_list free_list[MEM_MAX_IDX + 1];
    struct glob_list global_list; 
    int coalesced;
};


//
//Memory region
//
typedef struct mem_pool mem_pool_t;
#if 0
typedef struct memory mem_t;
#endif

struct phys_mem_region;

typedef struct vma_node {
    struct vma_node             * next;     //A linked list of all VMAs per address space
    unsigned int                flags;
    int (*pfnRelease)(int, void *); //pointer to release-delete method
    int                         qdi_handle;  //QDI handle which wraps this object (or -1 if none)
    mem_region_type_t           vma_flags;   //indicates it's a shared or file backed VMA
    mem_t                       *virt_mem;
    u32_t                       owner_thread;
    struct phys_mem_region      * phys_region_list;
    struct vma_node             * next_shared;   //linklist of shared VMAs that mapped to one region
    u32_t                       fd;  // file descriptor where vma is mapped from
    u32_t                       offset; // offset within the file vma starts
} vma_node_t;

typedef struct phys_mem_region {
    struct phys_mem_region     * next;      //A linklist of all physical memory regions in system
    unsigned int               flags;      /* This is needed to verify the type of memory region*/
    int (*pfnRelease)(int, void *);
    int qdi_handle;                         //QDI handle which wraps this object (or -1 if none)
    mem_pool_t                 * phys_pool;
    mem_t                      * phys_mem;
    struct phys_mem_region     * next_scattered; //linked list of scattered regions that backs up one VMA 
    u32_t                      count;      // count how many VMA mapped to this phys_region
    mem_mapping_t              mapping_type;
    mem_cache_mode_t           cache_mode;
    vma_node_t		           * vma;
    u32_t                      vma_offset; //offset within the VMA, used by VIRTUAL_FIXED mapping to get the vaddr 
                                            //of the mapped region
} phys_mem_region_t;


typedef struct pgt_range {
    struct pgt_range * next;
    unsigned int vaddr;
    unsigned int paddr;
    u32_t size;
    perm_t perm;
    u32_t pgattr;
} pgt_range_t;

typedef struct mem_fs_region {
    struct mem_fs_region *next;
    vma_node_t *vma_node;
    pgt_range_t *pgt_range_list; //pertains only to this region, as a result of mprotect
} mem_fs_region_t;


typedef struct phys_pool {
    qurt_qdi_obj_t qdiobj;
    struct phys_pool *pNext;
    struct phys_mem_pool_config *pConfig;
    int qdi_handle;
    mem_pool_t pool;
}phys_pool_t;


#ifndef GEN_CONFIG_HEADER_USED
struct phys_mem_pool_config{
    char name[32];
    struct range {
        unsigned int start;
        unsigned int size;
    } ranges[16];
};
#endif
 
typedef struct shmem_str {
    struct shmem_str *next;
    unsigned int    magic;
    char name[32];
    phys_mem_region_t *phys_mem_region;
    vma_node_t *vma_share_list;
}shmem_t;


#define MEM_T_SHIFT_AMT 12

static inline uint32_t M_PAGE(mem_t *mem)
{
   return mem->pageno;
}

static inline void M_SETPAGE(mem_t *mem, uint32_t pageno)
{
   mem->pageno = pageno;
}

static inline uint32_t M_ADDR(mem_t *mem)
{
   return (mem->pageno << MEM_T_SHIFT_AMT);
}

static inline void M_SETADDR(mem_t *mem, uint32_t addr)
{
   mem->pageno = addr >> MEM_T_SHIFT_AMT;
}

static inline uint32_t M_ENDADDR(mem_t *mem)
{
   return ((mem->pageno + mem->page_count) << MEM_T_SHIFT_AMT) - 1;
}

static inline uint32_t M_BYTECNT(mem_t *mem)
{
   return (mem->page_count << MEM_T_SHIFT_AMT);
}

static inline uint32_t M_PGCNT(mem_t *mem)
{
   return (mem->page_count);
}

static inline void M_SETBYTECNT(mem_t *mem, uint32_t size)
{
   mem->page_count = size >> MEM_T_SHIFT_AMT;
}

static inline void M_SETPGCNT(mem_t *mem, uint32_t size)
{
   mem->page_count = size;
}

static inline void M_INCBYTECNT(mem_t *mem, uint32_t size)
{
   mem->page_count += size >> MEM_T_SHIFT_AMT;
}

static inline uint32_t M_BYTEOFFSET(mem_t *mem, phys_mem_region_t *region)
{
   return ((mem->pageno << MEM_T_SHIFT_AMT)+ region->vma_offset);
}

static inline void M_SETBYTEOFFSET(mem_t *mem, phys_mem_region_t *region, uint32_t addr)
{
   region->vma_offset = addr - (mem->pageno << MEM_T_SHIFT_AMT);
}

static inline void *M_PTR(mem_t *mem)
{
   return (void *)(mem->pageno << MEM_T_SHIFT_AMT);
}

static inline int M_CHKBYTE(mem_t *mem, uint32_t addr)
{
   return (((addr >> MEM_T_SHIFT_AMT) - mem->pageno) < mem->page_count);
}

static inline int M_CHKPAGE(mem_t *mem, uint32_t pageno)
{
   return ((pageno - mem->pageno) < mem->page_count);
}

phys_pool_t *mem_pool_alloc(phys_pool_t *, int, struct phys_mem_pool_config *);
phys_mem_region_t *mem_allocate_region(void);
void mem_free_region(phys_mem_region_t *region);
vma_node_t *mem_allocate_vma(vma_node_t **vma_list);
void mem_free_vma(vma_node_t **vma_list, vma_node_t *vma);

extern shmem_t *shmem_search(u32_t paddr, u32_t size, mem_cache_mode_t cache_mode);

#endif /* QURTOS_MEMORY_H */
