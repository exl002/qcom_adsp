#ifndef SPACE_H
#define SPACE_H
/*=============================================================================
                space.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
		  QuRT OS address space (Protection Domain) functions and definitions
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/qurtos/include/space.h#9 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
=============================================================================*/

#include <qurtos_memory.h>
#include <alloc.h>
#include <consts.h>
#include <err_hndlr.h>

struct space {
    int             id;     //ASID
    vma_node_t      *vma_list; //george: shall we keep this? yes if 1. virt_address for shared memory are all same in each processes. 2.  

    mem_pool_t      *default_virt_pool;  

    void *          pdir;   //page table directory pointer
    uint16_t        l2pgtbl_usage_counter[1024];    //each entry track usage of a L2 page table (1024 L2 pagetables, each has 1024 entries)
    
    uint32_t        entry;
    uint32_t        app_vstart;
    uint32_t        app_vend;
    
    uint32_t        usermalloc_addr;
    uint32_t        usermalloc_size;

    void            *rwdatasave;

    char            name[QURT_MAX_NAME_LEN];
} __attribute__((aligned(8))) ;

typedef struct space space_t;



struct pgent
{
    union {
        struct {
            /* For speed, same bits in same order as TLB */
            u32_t addr            : 22; //this includes one S bit. So only have 21 bit for PPN. So only support 33 bits Paddr
            u32_t global          : 1;
            u32_t trust           : 1;
            u32_t cfield          : 4;
            u32_t usr             : 1;
            u32_t xwr             : 3;
        } X;
        u32_t               raw;
    };
};

#define EMPTY_ENTRY (0xffffffff)

typedef struct pgent pgent_t;

/* memory coherency attributes (map to C field EntryLo bits (see sect 15.3) ) */
#if 0
typedef enum pgattr_e {
    write_back          = 7,
    write_through       = 5,
    uncached            = 4,
	zero_cfield = 0
} pgattr_t;
#endif /* 0 */
typedef mem_cache_mode_t pgattr_t;

static inline pgent_t PGENT( u32_t addr, pgsize_t pgsize, int global, int cfield, perm_t perm ) {
    pgent_t pg;
    pg.X.addr      = (addr<<1) | pgsize; 
    pg.X.trust     = 0;
    pg.X.global    = perm.X.global;
    pg.X.cfield    = cfield;      
    pg.X.xwr       = perm.X.rwx;      
    pg.X.usr       = perm.X.mode;
    return pg;
}

//extern u32_t   kube_tcm_phys_start;
//
//extern u32_t __phys_addr_ram;
#define ALIGN_UP_4K(addr) ((addr)&0xfff ? ((addr) + 0x1000) & 0xfffff000 : (addr)&0xfffff000 )  

#define V2P(addr) ((unsigned int)(addr) - KERNEL_VIRT_START + __phys_addr_ram )
#define P2V(addr) ((unsigned int)(addr) - __phys_addr_ram + KERNEL_VIRT_START  )

void qurtos_space_init(void);

int qurtos_munmap_pages_ext(space_t *space, u32_t pageno_virt, u32_t pageno_phys, u32_t page_count);
int qurtos_munmap_pages(space_t *space, u32_t pageno_virt, u32_t pageno_phys, u32_t page_count);

int qurtos_mmap_pages_ext(space_t *space, u32_t pageno_virt, u32_t pageno_phys, u32_t page_count,
                          perm_t perm, pgattr_t attr);
int qurtos_mmap_pages(space_t *space, u32_t pageno_virt, u32_t pageno_phys, u32_t page_count,
                      perm_t perm, pgattr_t attr);
int qurtos_mmap(space_t *space, u32_t vaddr, u32_t paddr, u32_t size,
                perm_t perm, pgattr_t attr);

int qurtos_munmap(space_t *space, u32_t vaddr, u32_t paddr, u32_t size);
int qurtos_munmap_64(space_t *space, u32_t vaddr, u32_t paddr_lo, u32_t paddr_hi, u32_t size);

int qurtos_mmap_ext (space_t *space, u32_t vaddr, u32_t paddr, u32_t size,perm_t perm, pgattr_t attr);
int qurtos_mmap_ext_64 (space_t *space, u32_t vaddr, u32_t paddr_lo, u32_t paddr_hi, u32_t size,
                      perm_t perm, pgattr_t attr);

int qurtos_munmap_ext (space_t *space, u32_t vaddr, u32_t paddr, u32_t size);

u32_t qurtos_v2p(int asid, u32_t v_addr);
int qurtos_exec(const char *);
space_t *qurtos_fork(void);
void qurtos_set_user_app_mapping(void);
void qurtos_finalize_user_app_mapping(void);
#endif
