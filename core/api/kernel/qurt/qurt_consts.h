#ifndef QURT_CONSTS_H
#define QURT_CONSTS_H

/*======================================================================

                        qurt_defs.h 

GENERAL DESCRIPTION
  QURT constant and definitions 

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 
											 EDIT HISTORY FOR FILE
 
	 This section contains comments describing changes made to the
	 module. Notice that changes are listed in reverse chronological
	 order.
 
	$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_consts.h#11 $ 
	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
	$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_consts.h#11 $ 
	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
	when				who 		what, where, why
	----------	--- 		------------------------------------------------
	2011-02-25	op			Add Header file
    2012-12-14  cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/

/*=====================================================================
 Constants and macros
 ======================================================================*/

/* Definitions of system events. System events will cause 
   a thread to be suspended and put into suspending_list.
   The system event number will be saved in CONTEXT::error::cause field
   of the suspended thread. An event handler thread such as
   page fault handler, system error handler may wake up the suspended
   thread.
 */
#define QURT_EVENT_PAGEFAULT      0x1 /* Page fault event. */
#define QURT_EVENT_SYSTEM_ERR     0x2 /* System Error event. */
#define QURT_EVENT_SUSPEND        0x3
#define QURT_EVENT_PROCESS_EXIT   0x4 /* Process termination event.*/

#define QURT_SYSENV_MAX_THREADS_TYPE           1 /* Maximum threads object. */
#define QURT_SYSENV_PROCNAME_TYPE              2 /* Process name object. */
#define QURT_SYSENV_MAX_PI_PRIO_TYPE           3 /* Maximum pi priority object. */
#define QURT_SYSENV_VERSION_TYPE               4 /* Architecture version object. */
#define QURT_SYSENV_APP_HEAP_TYPE              5 /* Application heap object. */
#define QURT_SYSENV_TIMER_HW_ATTR_TYPE         6 /* Timer object. */

/** @cond RTOS_user_guide */
/** @addtogroup perf_monitor_const
@{ */

/** 
 PMU  
*/
#define QURT_PMUCNT0    0  /**< */
#define QURT_PMUCNT1    1  /**< */
#define QURT_PMUCNT2    2  /**< */
#define QURT_PMUCNT3    3  /**< */
#define QURT_PMUCFG     4  /**< */
#define QURT_PMUEVTCFG  5  /**< */
/** @} */ /* end_addtogroup perf_monitor_const */
/** @endcond */

/*
 Power collapse operation 
*/
#define QURT_POWER_SHUTDOWN       0
#define QURT_TCXO_SHUTDOWN        1
#define QURT_POWER_CMD_PREPARE    0
#define QURT_POWER_CMD_PERFORM    1
#define QURT_POWER_CMD_EXIT       2
#define QURT_POWER_CMD_FAIL_EXIT  3
#define QURT_POWER_CMD_PERFORM_L2_RETENTION 4
#define QURT_POWER_CMD_PERFORM_SAVE_TCM     5

/** @ingroup thread_macros
 The limit on the maximum number of hardware threads supported by QuRT for any
 Hexagon version. This definition can be used to define arrays, etc., in
 target independent code.
 */
#define QURT_MAX_HTHREAD_LIMIT    6

/*
  L2 cache retention mode
*/
#define QURT_POWER_SHUTDOWN_TYPE_L2NORET QURT_POWER_CMD_PERFORM
#define QURT_POWER_SHUTDOWN_TYPE_L2RET   QURT_POWER_CMD_PERFORM_L2_RETENTION
#define QURT_POWER_SHUTDOWN_TYPE_SAVETCM QURT_POWER_CMD_PERFORM_SAVE_TCM

/* 
  QURT_system_state
  Use for debugging the shutdown/startup process.

  State transition for cold boot:
  QURT_BOOT_SETUP_ISDB --> QURT_CBOOT_BSP_INIT --> 
  QURT_CBOOT_END_CLEAN_INIT --> QURT_CBOOT_END_OS_INIT --> 
  QURT_CBOOT_KERNEL_INIT_DONE --> QURT_CBOOT_PLAT_CONFIG_DONE -->
  QURT_CBOOT_ROOT_TASK_STARTED

  State transition for power collapse:
  QURT_PREPARE_SINGLE_MODE --> QURT_PERFORM_IPEND -->
  QURT_PERFORM_SAVE_TLB --> QURT_PERFORM_SWITCH_PC -->
  cache flush states (dependent on L2 retention config)

  State transition for warm boot:
  QURT_BOOT_SETUP_ISDB --> QURT_WBOOT_INIT_TLB -->
  QURT_WBOOT_SET_1TO1_MAP --> QURT_WBOOT_REMOVE_1TO1_MAP -->
  QURT_CBOOT_END_CLEAN_INIT --> QURT_CBOOT_END_OS_INIT
*/
#define QURT_PREPARE_SINGLE_MODE 1
#define QURT_PREPARE_END 2
#define QURT_PERFORM_IPEND 3
#define QURT_PERFORM_SAVE_ISDP 4
#define QURT_PERFORM_SAVE_PMU 5
#define QURT_PERFORM_SAVE_TLB 6
#define QURT_PERFORM_SWITCH_PC 7
#define QURT_PERFORM_EXIT 8
#define QURT_FLUSH_L1CACHE 9
#define QURT_FLUSH_L2CACHE 0xA
#define QURT_FLUSH_CACHE_DONE 0xB
#define QURT_SWITCH_PC_DONE 0xC
#define QURT_BOOT_SETUP_ISDB 0xD
#define QURT_WBOOT_INIT_TLB 0xE
#define QURT_WBOOT_SET_1TO1_MAP 0xF
#define QURT_WBOOT_CFG_ADV_SYSCFG 0x10
#define QURT_WBOOT_REMOVE_1TO1_MAP 0x11
#define QURT_CBOOT_BSP_INIT 0x12
#define QURT_CBOOT_END_CLEAN_L1CACHE 0x13
#define QURT_CBOOT_END_CLEAN_INIT 0x14
#define QURT_CBOOT_END_OS_INIT 0x15
#define QURT_CBOOT_TLB_DUMP_LOAD 0x16
#define QURT_CBOOT_TLB_STATIC_LOAD 0x17
#define QURT_CBOOT_KERNEL_INIT_DONE 0x18
#define QURT_CBOOT_PLAT_CONFIG_DONE 0x19
#define QURT_CBOOT_ROOT_TASK_STARTED 0x1A
#define QURT_IMPRECISE_EXCEPTION 0x1B
#define QURT_WBOOT_DEBUG_L2_START 0x1C
#define QURT_WBOOT_DEBUG_L2_END   0x1D
#define QURT_NMI_SAVE_L2VIC_COMPLETE   0x1E
#define QURT_NMI_HANDLER_COMPLETE   0x1F
#define QURT_NMI_AFTER_SAVE_GLOBAL 0x20
/*
  Thread attributes
*/

#define QURT_THREAD_ATTR_GP              0x00000002
#define QURT_THREAD_ATTR_UGP             0x00000003
#define QURT_THREAD_ATTR_PREFETCH        0x00000004
#define QURT_THREAD_ATTR_TID             0x00000005
#define QURT_THREAD_ATTR_L1IC_PART       0x00000007
#define QURT_THREAD_ATTR_L1DC_PART       0x00000008
#define QURT_THREAD_ATTR_L2C_PART        0x00000009
#define QURT_THREAD_ATTR_COPROCESSOR     0x0000000a

/**  Cache operations*/
#define QURT_DCCLEAN       0   /* Clean Dcache. */ 
#define QURT_DCINV         1   /* Invalidate Dcache. */
#define QURT_DCCLEANINV    2   /* Clean and invalidate Dcache. */
#define QURT_ICINV         3   /* Invalidate Icache. */
#define QURT_DUMP_DCTAGS   4  /* This is for testing purpose. */
#define QURT_FLUSH_ALL     5  /* Flush L1 and L2 cache. */
#define QURT_TABLE_FLUSH   6  /* Flush based on table of physical pages */
#define QURT_CLEAN_INVALIDATE_ALL  7  /* Flush and invalidate L1 and L2 cache. */


/**@ingroup chapter_prefined_symbols */
#define QURT_API_VERSION    5 /**< QURT API version. */

/* Process state. */
#define QURT_UPDATE_PROCESS_STATE   0
#define QURT_MP_INIT        1
#define QURT_MP_RUNNING     2
#define QURT_MP_STOPPED     3

/* QuRT reset reason. */
#define QURT_NORMAL_BOOT               0  /* Normal boot. */
#define QURT_WARM_BOOT                 1  /* Power collapse warm boot. */
#define QURT_WARM_BOOT_L2_RETENTION    2  /* Power collapse with L2 retention warm boot. */ 
#define QURT_WARM_BOOT_SAVE_TCM        3  /* Power collapse with saving TCM. */ 

#define QURT_MAX_NAME_LEN   64

#define MAX_POOL_RANGES     16
#define MAX_TLB_ENTRIES     128 

/*
  Microarchitecture version
 */

#define  V5_1         0x1  //128KB
#define  V5_2         0x2  //256KB
#define  V5_4         0x4  //512KB
#define  V5_6         0x6  //768KB (512K L2$ + additional 256K L2TCM

#endif /* QURT_CONSTS_H */
