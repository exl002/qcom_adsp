#ifndef QURT_TYPES_H
#define QURT_TYPES_H
/**
  @file qurt_types.h 
  @brief  Contains types common to all configurations

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_types.h#11 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
12/12/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/

/*=============================================================================
												TYPEDEFS
=============================================================================*/
//#include <stddef.h>
#include <qurt_consts.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=====================================================================
 typedefs
======================================================================*/
/** @addtogroup memory_management_types
@{ */
typedef unsigned int qurt_addr_t;           /**< */
typedef unsigned int qurt_paddr_t;         /* Used in TLB APIs */ 
typedef unsigned long long qurt_paddr_64_t; 
typedef unsigned int qurt_mem_region_t;     /**< Memory regions are represented as objects of this type. */
typedef unsigned int qurt_mem_fs_region_t; 
typedef unsigned int qurt_mem_pool_t;       /**< Memory pools are represented in QuRT as objects of this type.*/
typedef unsigned int qurt_size_t;           /**< */

/*
 * Mapping type
 *
 * QMEM_MAPPING_VIRTUAL is the default mode, in which system will
 * pick up available range of virtual address, and map it to 
 * available contiguous physical addresses. Physical-to-virtual
 * is not guaranteed to be 1:1; both virtual and phys memory is 
 * contiguous.
 *
 * In QMEM_MAPPING_IDEMPOTENT mode, user shall provide physical address;
 * kernel will allocate 1:1 physical-to-virtual memory. Primary usage of 
 * of this mapping is to allocate physical-to-virtual memory 1:1.
 *
 * In QMEM_MAPPING_PHYS_CONTIGUOUS mode, the virtual address might
 * not be the same as the physical address. But physical address of the
 * memory region is guaranteed to be contiguous starting at the provided
 * address, it is REQUIRED to provide a fixed physical address. Primary 
 * usage of this mapping is to allocate physical memory from a particular 
 * address, where 1:1 physical-to-virtual is not required.
 *
 * QMEM_MAPPING_NONE mode should be used to reserve a Virtual Memory
 * Area (VMA); no physical memory is reserved or mapped to this virtual
 * space; all standard qmem_region APIs apply to a VMA, however physical
 * address is always INVALID_ADDR. qmem_region_create() in this mode
 * will return a handle to the VMA, both virt_addr and phys_addr need to
 * be set to INVALID_ADDR, kernel will allocate any available virtual
 * memory of the specified size. Starting virtual address of VMA can be
 * obtained through qmem_region_attr_getvirtaddr().
 * Primary purpose of this mapping mode is to provide a mechanism for
 * "delayed binding" in QURT, i.e. reserve virtual memory and map it at
 * some later time to possibly discontiguous physical blocks. Thus, a
 * single VMA can be partitioned among several physical-virtual mappings
 * created via qmem_region_create() with QMEM_VIRTUAL_FIXED mapping mode.
 * Each VMA keeps track of associated mapped regions.
 * Deletion of VMA will succeed only if all associated "virtual_fixed"
 * regions are freed prior to VMA deletion.
 *
 * QMEM_MAPPING_VIRTUAL_FIXED mode should be used to create a region
 * from virtual space that has been reserved via qmem_region_create()
 * with QMEM_MAPPING_NONE mapping. A valid virt_add is required, if
 * phys_addr is specified, kernel will make attempt to map it accordingly,
 * if no phys_addr is specified, kernel will map any available physical
 * memory. All standard qmem_region APIs apply to such region. Remapping
 * a virtual range without prior freeing of the region is not permitted.
 * When such region is deleted its corresponding VMA remains intact.
 *
 * QURT_MEM_MAPPING_VIRTUAL_FIXED_ADDR attribute is similar to 
 * QMEM_MAPPING_VIRTUAL except that the the virtual memory is mapped 
 * at the client specified virtual address. The memory can be mapped
 * to contiguous or separated physical address range.
 */

/** Memory region mappings are specified as values of this type. */
typedef enum {
        QURT_MEM_MAPPING_VIRTUAL=0,  /**< Default mode. The system picks up the
                                     available range of the virtual address, and maps it to 
                                     available contiguous physical addresses.*/
        QURT_MEM_MAPPING_PHYS_CONTIGUOUS = 1,  /**< The physical address of the
                                               memory region is guaranteed to be contiguous 
                                               starting at the provided address, and it is required 
                                               to provide a fixed physical address.*/
        QURT_MEM_MAPPING_IDEMPOTENT=2,  /**< Allocates physical-to-virtual memory 1:1. */ 
        QURT_MEM_MAPPING_VIRTUAL_FIXED=3,  /**< Creates a region from virtual 
                                           space that has been reserved via 
                                           qmem_region_create() with QMEM_MAPPING_NONE 
                                           mapping. */
        QURT_MEM_MAPPING_NONE=4,  /**< Reserves a virtual memory area (VMA). */
        QURT_MEM_MAPPING_VIRTUAL_RANDOM=7,  /**< The system chooses a random virtual address and
                                            maps it to available contiguous physical addresses.*/
        QURT_MEM_MAPPING_INVALID=10,        /**< Reserved as an invalid mapping type. */
} qurt_mem_mapping_t;

/** Cache modes are specified as values of this type. */
typedef enum {
        QURT_MEM_CACHE_WRITEBACK=7,     /**< */
        QURT_MEM_CACHE_NONE_SHARED=6,   /**< */
        QURT_MEM_CACHE_WRITETHROUGH=5,  /**< */
        QURT_MEM_CACHE_WRITEBACK_NONL2CACHEABLE=0,    /**< */
        QURT_MEM_CACHE_WRITETHROUGH_NONL2CACHEABLE=1,  /**< */
        QURT_MEM_CACHE_WRITEBACK_L2CACHEABLE=QURT_MEM_CACHE_WRITEBACK,  /**< */
        QURT_MEM_CACHE_WRITETHROUGH_L2CACHEABLE=QURT_MEM_CACHE_WRITETHROUGH,  /**< */
        QURT_MEM_CACHE_NONE = 4,  /**< */
        QURT_MEM_CACHE_INVALID=10,  /**< Reserved as an invalid cache type. */
} qurt_mem_cache_mode_t;


/** Memory access permission. */
typedef enum {
        QURT_PERM_READ=0x1, /**< */
        QURT_PERM_WRITE=0x2,  /**< */
        QURT_PERM_EXECUTE=0x4,  /**< */
        QURT_PERM_FULL=QURT_PERM_READ|QURT_PERM_WRITE|QURT_PERM_EXECUTE,  /**< */
} qurt_perm_t;

/** Cache types are specified as values of this type, specifying data cache or instruction cache. */
typedef enum {
        QURT_MEM_ICACHE, /**< */
        QURT_MEM_DCACHE  /**< */
} qurt_mem_cache_type_t;

/**  Cache operation codes are specified as values of this type. */
typedef enum {
    QURT_MEM_CACHE_FLUSH, /**< */
    QURT_MEM_CACHE_INVALIDATE, /**< */
    QURT_MEM_CACHE_FLUSH_INVALIDATE, /**< */
    QURT_MEM_CACHE_FLUSH_ALL, /**< */
    QURT_MEM_CACHE_FLUSH_INVALIDATE_ALL, /**< */
    QURT_MEM_CACHE_TABLE_FLUSH_INVALIDATE, /**< */
} qurt_mem_cache_op_t;

/** Memory region types are stored as values of this type. */
typedef enum {
        QURT_MEM_REGION_LOCAL=0,  /**< */
        QURT_MEM_REGION_SHARED=1,  /**< */
        QURT_MEM_REGION_USER_ACCESS=2,  /**< */
        QURT_MEM_REGION_FS=4,  /**< */
        QURT_MEM_REGION_INVALID=10,  /**< Reserved as an invalid region type. */
} qurt_mem_region_type_t;

/**
   Memory region attributes are stored in structures of this type.  
   physaddr can by specified in QMEM_MAPPING_IDEMPOTENT mode and
   QMEM_MAPPING_PHYS_CONTIGUOUS mode. virtaddr cannot be specified for a
   memory region, it can only be queried by the qmem_attr_getvirtaddr function.
 */
typedef struct {
    /** @cond */
    qurt_mem_mapping_t     mapping_type; 
    qurt_mem_cache_mode_t  cache_mode;  
    unsigned               ppn; //physical page number (physical>>12)
    qurt_addr_t            virtaddr;
    qurt_mem_region_type_t type;   
    qurt_size_t            size;
    /** @endcond */
} qurt_mem_region_attr_t;

/** Definition of the user physical memory pool. */
typedef struct {
    /** @cond */
    char name[32];
    struct ranges{
        unsigned int start;
        unsigned int size;
    } ranges[MAX_POOL_RANGES];   
    /** @endcond */
} qurt_mem_pool_attr_t;


/** @} */ /* end_addtogroup memory_management_types */
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_TYPES_H */
