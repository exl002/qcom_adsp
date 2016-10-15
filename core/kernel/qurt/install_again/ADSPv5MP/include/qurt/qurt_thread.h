#ifndef QURT_THREAD_H
#define QURT_THREAD_H
/**
  @file qurt_thread.h 
  @brief  Prototypes of Thread API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2010 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/qurt/include/public/qurt_thread.h#8 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/06/11   op      Add header file, Fix Doxygen warning messages
12/17/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/

#include <string.h>


/*=============================================================================
												CONSTANTS AND MACROS
=============================================================================*/


/*
  Bitmask configuration is for selecting DSP hardware threads. 
  To select all the hardware threads, using QURT_THREAD_CFG_BITMASK_ALL 
  and the following: \n
  - For QDSP6 V2/V3, all 6 hardware threads are selected \n
  - For QDSP6 V3L, all 4 hardware threads are selected \n
  - For QDSP6 V4, all 3 hardware threads are selected
 */  
/** @addtogroup thread_macros
@{ */
/**   @xreflabel{sec:qurt_thread_cfg} */  

#define QURT_THREAD_CFG_BITMASK_HT0      0x00000001   /**< */
#define QURT_THREAD_CFG_BITMASK_HT1      0x00000002   /**< */
#define QURT_THREAD_CFG_BITMASK_HT2      0x00000004   /**< */ 
#define QURT_THREAD_CFG_BITMASK_HT3      0x00000008   /**< */
#define QURT_THREAD_CFG_BITMASK_HT4      0x00000010   /**< */
#define QURT_THREAD_CFG_BITMASK_HT5      0x00000020   /**< */
#define QURT_THREAD_CFG_BITMASK_ALL      0x000000ff   /**< */

#define QURT_THREAD_CFG_USE_RAM          0x00000000   /**< */
#define QURT_THREAD_CFG_USE_TCM          0x00000100   /**< */

/*
   Macros for QuRT thread attributes.   
 */

#define QURT_HTHREAD_L1I_PREFETCH      0x1     /**< Enables hardware L1 instruction cache prefetching. */
#define QURT_HTHREAD_L1D_PREFETCH      0x2     /**< Enables hardware L1 data cache prefetching. */
#define QURT_HTHREAD_L2I_PREFETCH      0x4     /**< Enables hardware L2 instruction cache prefetching. */
#define QURT_HTHREAD_L2D_PREFETCH      0x8     /**< Enables hardware L2 data cache prefetching. */
#define QURT_HTHREAD_DCFETCH           0x10    /**< Enables DC fetch to the provided virtual address. 
                                     DC fetch instructs the hardware that a data memory access is likely. 
                                     Instructions are dropped in the case of high bus utilization. */



#define QURT_THREAD_ATTR_NAME_MAXLEN            16  /**< */
#define QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT  0  /**< */
#define QURT_THREAD_ATTR_PRIORITY_DEFAULT       256  /**< */
#define QURT_THREAD_ATTR_ASID_DEFAULT           0  /**< */
#define QURT_THREAD_ATTR_AFFINITY_DEFAULT      (-1)  /**< */
#define QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT   (-2)  /**< */
/** @} */ /* end_addtogroup thread_macros */
/*=============================================================================
												TYPEDEFS
=============================================================================*/
/** @addtogroup thread_types
@{ */
typedef enum {
    CCCC_PARTITION = 0,     /**< Use the CCCC page attribute bits to determine the main or auxiliary partition. */
    MAIN_PARTITION = 1,     /**< Use the main partition. */
    AUX_PARTITION = 2,      /**< Use the auxiliary partition. */
    MINIMUM_PARTITION = 3   /**< Use the minimum. Allocates the least amount of cache (no-allocate policy possible) for this thread. */
} qurt_cache_partition_t;

/** Thread ID type */
typedef unsigned int qurt_thread_t;

/** Thread attributes */
typedef struct _qurt_thread_attr {
    /** @cond */
    char name[QURT_THREAD_ATTR_NAME_MAXLEN]; /**< Thread name. */
    unsigned char tcb_partition;  /**< Should the thread TCB reside in RAM or
                                       on chip memory (i.e. TCM). */
    unsigned char affinity;       /**< Hardware bitmask indicating the threads it
                                       can run on. */
    unsigned short priority;      /**< Thread priority. */
    unsigned short asid;          /**< Address space ID. */
    unsigned short timetest_id;   /**< Timetest ID. */
    unsigned int stack_size;      /**< Thread stack size. */
    void *stack_addr;             /**< Stack address base, the range of the stack is
                                       (stack_addr, stack_addr+stack_size-1). */
    /** @endcond */
} qurt_thread_attr_t;

/** @} */ /* end_addtogroup thread_types */

/*=============================================================================
												FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_thread_attr_init
  Initializes the structure used to set the thread attributes when a thread is created.
  After an attribute structure is initialized, the individual attributes in the structure can be
  explicitly set using the thread attribute operations.

  The default attribute values set the by the initialize operation are the following: \n
  - Name -- Null string \n
  - Timetest ID -- QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT \n
  - Priority -- QURT_THREAD_ATTR_PRIORITY_DEFAULT \n
  - Affinity -- QURT_THREAD_ATTR_AFFINITY_DEFAULT \n
  - TCB partition -- QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Thread attribute structure.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_init (qurt_thread_attr_t *attr)
{

    attr->name[0] = 0;
    attr->tcb_partition = QURT_THREAD_ATTR_TCB_PARTITION_DEFAULT;
    attr->priority = QURT_THREAD_ATTR_PRIORITY_DEFAULT;
    attr->asid = QURT_THREAD_ATTR_ASID_DEFAULT;
    attr->affinity = QURT_THREAD_ATTR_AFFINITY_DEFAULT;
    attr->timetest_id = QURT_THREAD_ATTR_TIMETEST_ID_DEFAULT;
    attr->stack_size = 0;
    attr->stack_addr = 0;
}

/**@ingroup func_qurt_thread_attr_set_name
  Sets the thread name attribute.\n
  This function specifies the name to be used by a thread.
Thread names are used to uniquely identify a thread during debugging or profiling. \n
@note1hang Thread names differ from the kernel-generated thread identifiers used to
specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t

@param[in,out] attr Thread attribute structure.
@param[in] name Character string containing the thread name.

@return
None.

@dependencies
None.
*/
static inline void qurt_thread_attr_set_name (qurt_thread_attr_t *attr, char *name)
{
    strlcpy (attr->name, name, QURT_THREAD_ATTR_NAME_MAXLEN);
    attr->name[QURT_THREAD_ATTR_NAME_MAXLEN - 1] = 0;
}


/**@ingroup func_qurt_thread_attr_set_tcb_partition
  Sets the thread TCB partition attribute.
  Specifies the memory type where a thread's thread control block (TCB) is allocated.
  TCBs can be allocated in RAM or TCM/LPM.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr  Thread attribute structure.
  @param[in] tcb_partition TCB partition. Values:\n
                     0 - TCB resides in RAM \n
                     1 - TCB resides in TCM/LCM

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_tcb_partition (qurt_thread_attr_t *attr, unsigned char tcb_partition)
{
    attr->tcb_partition = tcb_partition;
}

/**@ingroup func_qurt_thread_attr_set_priority
  Sets the thread priority to be assigned to a thread.
  Thread priorities are specified as numeric values in the range 0-255, with 0 representing
  the highest priority.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] priority Thread priority.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_priority (qurt_thread_attr_t *attr, unsigned short priority)
{
    attr->priority = priority;
}

/**@ingroup func_qurt_thread_attr_set_affinity
  Specifies the Hexagon processor hardware threads that a QuRT thread can execute on.\n
  This function sets the thread affinity attribute.
  The affinity value specifies a bitmask value which identifies the hardware threads to be used.\n
  Bits 0-5 in the 8-bit mask value specify hardware threads 0-5 respectively. If a bit is set to
  1, the thread is eligible to run on the corresponding hardware thread.\n
  Mask bit values are specified using the predefined bitmask symbols QURT_THREAD_CFG_*
  (Section @xref{sec:qurt_thread_cfg}). These symbols can be ORed together to specify more than one hardware
  thread, or the symbol QURT_THREAD_CFG_BITMASK_ALL can be used to specify all the
  threads. \n
  @note1hang QURT_THREAD_CFG_BITMASK_ALL is defined to specify the proper set of
             hardware threads regardless of the Hexagon processor version (since the
             versions support different numbers of hardware threads).

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Thread attribute structure.
  @param[in] affinity Bitmask indicating hardware threads used.

  @return
  None.

  @dependencies
  None.
  
*/
static inline void qurt_thread_attr_set_affinity (qurt_thread_attr_t *attr, unsigned char affinity)
{
    attr->affinity = affinity;
}

/**@ingroup func_qurt_thread_attr_set_timetest_id
  Sets the thread timetest attribute.\n
  Specifies the timetest identifier to be used by a thread.

  Timetest identifiers are used to uniquely identify a thread during debugging or profiling. \n
  @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
             specify threads in the API thread operations.

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr   Thread attribute structure.
  @param[in] timetest_id Timetest identifier value.

  @return
  None.

  @dependencies
  None.
  */
static inline void qurt_thread_attr_set_timetest_id (qurt_thread_attr_t *attr, unsigned short timetest_id)
{
    attr->timetest_id = timetest_id;
}

/**@ingroup func_qurt_thread_attr_set_stack_size
  @xreflabel{sec:set_stack_size}
  Sets the thread stack size attribute.\n
  Specifies the size of the memory area to be used for a thread's call stack.

  The thread stack address (Section @xref{sec:set_stack_addr}) and stack size specify the memory area used as a
  call stack for the thread. The user is responsible for allocating the memory area used for
  the stack.

  @datatypes
  #qurt_thread_attr_t

  @param[in,out] attr Thread attribute structure.
  @param[in] stack_size Size (in bytes) of the thread stack.

  @return
  None.

  @dependencies
  None.
*/

static inline void qurt_thread_attr_set_stack_size (qurt_thread_attr_t *attr, unsigned int stack_size)
{
    attr->stack_size = stack_size;
}

/**@ingroup func_qurt_thread_attr_set_stack_addr
  @xreflabel{sec:set_stack_addr}
  Sets the thread stack address attribute. \n
  Specifies the base address of the memory area to be used for a thread's call stack.

  stack_addr must contain an address value that is 8-byte aligned.

  The thread stack address and stack size (Section @xref{sec:set_stack_size}) specify the memory area used as a
  call stack for the thread. \n
  @note1hang The user is responsible for allocating the memory area used for the thread
             stack. The memory area must be large enough to contain the stack that is
             created by the thread.

  @datatypes
  #qurt_thread_attr_t
  
  @param[in,out] attr Thread attribute structure.
  @param[in] stack_addr  8-byte aligned address of thread stack.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_thread_attr_set_stack_addr (qurt_thread_attr_t *attr, void *stack_addr)
{
    attr->stack_addr = stack_addr;
}

/**@ingroup func_qurt_thread_get_name
Gets the thread name of current thread.\n
  Returns the thread name of the current thread. 
  Thread names are assigned to threads as thread attributes (Section @xref{sec:threads}). They are used to
uniquely identify a thread during debugging or profiling.

  @param[out] name Character string. Specifies the address where the returned thread name is stored.
@param[in] max_len Maximum string length. Specifies the maximum length of the character string that can be returned.

@return
None.

@dependencies
None.
*/
void qurt_thread_get_name (char *name, unsigned char max_len);

/**@ingroup func_qurt_thread_create
  Creates a new thread with the specified attributes, and makes it executable.

  @note1hang This function fails (with an error result) if the set of hardware threads
             specified in the thread attributes is invalid for the target processor version.

  @datatypes
  #qurt_thread_t \n
  #qurt_thread_attr_t
  
  @param[out]  thread_id    Returns a pointer to the thread identifier if the thread was 
                             successfully created.
  @param[in]   attr 	     Pointer to the initialized thread attribute structure specifying 
                             the attributes of the created thread.
  @param[in]   entrypoint   C function pointer specifying the thread's main function.
  @param[in]   arg  	     Pointer to a thread-specific argument structure
  
   
   @return 
  QURT_EOK -- Thread created. \n
  QURT_EFAILED -- Thread not created. 

   @dependencies
   None.
 */
int qurt_thread_create (qurt_thread_t *thread_id, qurt_thread_attr_t *attr, void (*entrypoint) (void *), void *arg);

/*
   Stops the current thread, frees the kernel TCB, and yields to the next highest ready thread. 
  
   @return
   void 

   @dependencies
   None.
 */
void qurt_thread_stop(void);


/**@ingroup func_qurt_thread_resume
   Resumes the execution of a suspended thread.
  
   @param[in]  thread_id Thread identifier.

   @return 
   QURT_EOK -- Thread successfully resumed. \n
   QURT_EFATAL -- Resume operation failed.

   @dependencies
   None.
 */
int qurt_thread_resume(unsigned int thread_id);

/**@ingroup func_qurt_thread_get_id
   Gets the identifier of the current thread.\n
   Returns the thread identifier for the current thread.
  
   @return 
   Thread identifier -- Identifier of the current thread. 

   @dependencies
   None.
 */
qurt_thread_t qurt_thread_get_id (void);

/**@ingroup func_qurt_thread_set_timetest_id
   Sets the timetest identifier of the current thread.
   Timetest identifiers are used to uniquely identify a thread during debugging or profiling.\n
   @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
              specify threads in the API thread operations.
  
   @param[in]  tid  Timetest identifier.

   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_timetest_id (unsigned short tid);


#if 0
/**
   Sets the prefetch value in ssr.
  
   @param   settings	The least significant 5 bits are used to set the prefetchability of the calling
                        thread. See @xhyperref{Q5,[Q5]} for hardware meanings.
    
   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_set_prefetch(unsigned int settings);
#endif


/**@ingroup func_qurt_thread_get_timetest_id
   Gets the timetest identifier of the current thread.\n
   Returns the timetest identifier of the current thread.\n
   Timetest identifiers are used to uniquely identify a thread during debugging or profiling. \n
   @note1hang Timetest identifiers differ from the kernel-generated thread identifiers used to
              specify threads in the API thread operations.

   @return 
   Integer -- Timetest identifier. 

   @dependencies
   None.
 */
unsigned short qurt_thread_get_timetest_id (void);

/**@ingroup func_qurt_thread_exit
   @xreflabel{sec:qurt_thread_exit}
   Stops the current thread and awakens any threads joined to it, then destroys the stopped
   thread.

   Any thread that has been suspended on the current thread (by performing a thread join 
   -- Section @xref{sec:thread_join}) is awakened and passed a user-defined status value 
   indicating the status of the stopped thread.\n

   @note1hang Exit must be called in the context of the thread to be stopped.
  
   @param[in]   status User-defined thread exit status value.

   @return
   None.

   @dependencies
   None.
 */
void qurt_thread_exit(int status);

/**@ingroup func_qurt_thread_join
   @xreflabel{sec:thread_join}
   Waits for a specified thread to finish.
   The specified thread should be another thread within the same process.
   The caller thread is suspended until the specified thread exits. When this happens the
   caller thread is awakened. \n
   @note1hang If the specified thread has already exited, this function returns immediately
              with the result value ENO_THREAD. \n
   @note1cont Two threads cannot call qurt_thread_join to wait for the same thread to finish.
              If this happens QuRT generates an exception (see Section @xref{sec:exceptionHandling}).
  
   @param[in]   tid     Thread identifier.
   @param[out]  status  Destination variable for thread exit status. Returns a application-defined 
                        value indicating the termination status of the specified thread. 
  
   @return  
   ENO_THREAD -- Thread has already exited. \n
   QURT_EOK -- Thread successfully joined with valid status value. 

   @dependencies
   None.
 */
int qurt_thread_join(unsigned int tid, int *status);

/**@ingroup func_qurt_thread_get_anysignal
   Gets the signal of the current thread.
   Returns the RTOS-assigned signal of the current thread.

   QuRT assigns every thread a signal to support communication between threads.
   
   @return  
   Signal object address -- Any-signal object assigned to the current thread.

   @dependencies
   None.
  
 */
unsigned int qurt_thread_get_anysignal(void);

/**@ingroup func_qurt_thread_get_priority 
   Gets the priority of the specified thread. \n 
   Returns the thread priority of the specified thread.\n
   Thread priorities are specified as numeric values in a range as large as 0-255, with lower
   values representing higher priorities. 0 represents the highest possible thread priority. \n
   @note1hang QuRT can be configured to have different priority ranges.

   @datatypes
   #qurt_thread_t
  
   @param[in]  threadid	   Thread identifier.	

   @return
   -1 -- Invalid thread identifier. \n
   0-255 -- Thread priority value.

   @dependencies
   None.
 */
int qurt_thread_get_priority (qurt_thread_t threadid);

/**@ingroup func_qurt_thread_set_priority
   Sets the priority of the specified thread.\n
   Thread priorities are specified as numeric values in a range as large as 0-255, with lower
   values representing higher priorities. 0 represents the highest possible thread priority.

   @note1hang QuRT can be configured to have different priority ranges. For more
              information see Section @xref{sec:AppDev}.
  
   @datatypes
   #qurt_thread_t

   @param[in] threadid	    Thread identifier.	
   @param[in] newprio 	    New thread priority value.

   @return
   0 -- Priority successfully set. \n
   -1 -- Invalid thread identifier. \n 
   
   @dependencies
   None.
 */
int qurt_thread_set_priority (qurt_thread_t threadid, unsigned short newprio);

/*
   Gets the QuRT API version.
 
  @return
  Qurt API version.

  @dependencies
  None.
 */
unsigned int qurt_api_version(void);


#endif /* QURT_THREAD_H */

