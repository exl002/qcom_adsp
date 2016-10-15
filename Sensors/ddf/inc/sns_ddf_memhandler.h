/** 
 * @file sns_ddf_memhandler.h 
 * @brief A memory allocator object with built-in garbage collection.
 *  
 * A helper object used to allocate and cleanup memory easily. The object keeps 
 * track of allocations as they are made and provides a function to free them 
 * all at once. This allows objects containing complex nested structures to be 
 * constructed in such a way that freeing the memory is done simply via a single 
 * function call. This is especially useful for function output parameters, 
 * since the caller does not need to know how the parameter was constructed.
 * 
 * Usage example: 
 *  
 *   typedef struct
 *   { 
 *       uint32* int_array;
 *       float*  float_array;
 *   } struct_s; 
 *    
 *   // The object that will manage the required memory. It can be passed around
 *   // safely, re-used as needed, and freed when no longer needed.
 *   sns_ddf_memhandler_s  memhandler;
 *   sns_ddf_memhandler_init(&memhandler);
 *  
 *   // Allocate memory for the structure.
 *   struct_s* s = sns_ddf_memhandler_malloc(&memhandler, sizeof(struct_s));
 *  
 *   // Allocate memory for its internals.
 *   s->int_array = sns_ddf_memhandler_malloc(&memhandler, sizeof(int) * 10);
 *   s->float_array = sns_ddf_memhandler_malloc(&memhandler, sizeof(float) * 5);
 *  
 *   // Do something with the structure.
 *   ...
 *  
 *   // Free all associated memory automagically.
 *   sns_ddf_memhandler_free(&memhandler);
 *  
 * Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/ddf/inc/sns_ddf_memhandler.h#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2010-09-02 yk   Initial revision
==============================================================================*/

#ifndef SNS_DDF_MEMHANDLER_H
#define SNS_DDF_MEMHANDLER_H

#include <stdint.h>


/**
 * Memory handler object. This must be initialized by the user via the init() 
 * function before being used.
 *  
 * @warning Do not access the fields in this structure directly. Use the 
 *          associated functions instead. 
 */
typedef struct sns_ddf_memhandler
{
    void* buff;                      /**< Buffer referenced by this object. */
    struct sns_ddf_memhandler* next; /**< Link to the next buffer. */

} sns_ddf_memhandler_s;


/**
 * Initializes the memory handler object before use.
 *  
 * @warning Do not attempt to initialize this object again before calling @a 
 *          sns_ddf_memhandler_free() first. Doing so may cause a memory leak. 
 * 
 * @param[in] memhandler  Memory memhandler object.
 */
void sns_ddf_memhandler_init(sns_ddf_memhandler_s* memhandler);


/**
 * Allocate the specified number of bytes. The memhandler object keeps track of 
 * these allocations internally so they are all freed at once when 
 * @a sns_ddf_memhandler_free() is called.
 *  
 * @param[in] memhandler  Memory handler object.
 * @param[in] size        Number of bytes to allocate.
 * 
 * @return Pointer to the allocated memory location, or NULL on error.
 */
void* sns_ddf_memhandler_malloc(
    sns_ddf_memhandler_s* memhandler, uint32_t size);


/**
 * Frees all memory allocations made until now. Note that since the underlying 
 * memory is freed, all previously returned pointers are invalidated and should 
 * therefore no longer be used. 
 * 
 * @param[in] memhandler  Memory handler object.
 */
void sns_ddf_memhandler_free(sns_ddf_memhandler_s* memhandler);

#endif

