#ifndef MPMINTI_H
#define MPMINTI_H

/**
@file mpminti.h
 
@brief Internal definitions for MPM interrupt management.

*/

/*-----------------------------------------------------------------------------
             Copyright © 2011-2012 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
             QUALCOMM Confidential and Proprietary.
-----------------------------------------------------------------------------*/

/*=============================================================================

$Header: //components/rel/core.adsp/2.2/power/mpm/src/mpminti.h#1 $

=============================================================================*/
#include "comdef.h"
#include "HALmpmint.h"
#include "mpmint_target.h"

/*=========================================================================
 *                     INTERNAL OS SPECIFIC DEFINITIONS
 *=========================================================================*/
/* The macros defined below are usually OS depedent. New definitions must be
 * added as support for new OS comes or it will result into default value
 * that may not work as expected on all OSes. */

#if defined(USES_QNX) && defined(QNP_POWERMAN_PROC)

  /* QNX powerman process specific implementation */

  #define VMPM_MSG_RAM_LOCK()
  #define VMPM_MSG_RAM_UNLOCK()

#else

  /* Default implementation for message ram lock/unlock. */

  #include "DALSys.h"

  extern DALSYSSyncHandle mpm_shared_msg_ram_lock;

  #define VMPM_MSG_RAM_LOCK()   DALSYS_SyncEnter(mpm_shared_msg_ram_lock)
  #define VMPM_MSG_RAM_UNLOCK() DALSYS_SyncLeave(mpm_shared_msg_ram_lock)

#endif

/*
 * Following macros assume that number of interrupts on mpm and
 * master are reasonable enough not to exceed 16 bit int range. If that
 * changes, we will need to switch to 32 bit values. It is not the first 
 * choice due to preference of reduced memory footprint for interrupt 
 * mapping table.
 */

/**
 * Indicates invalid interrupt id at master (local) level. 
 */
#define MPMINT_NULL_IRQ  (uint16)(-1)

/**
 * Use this macro to indicate unmapped entry in interrupt mapping table
 * (IMT).
 *
 * @see mpmint_irq_data_type
 */
#define MPMINT_IMT_UNMAPPED_HAL_IRQ  (int16)(-1)

/**
 * Indicates end of sequence in mpm interrupt mapping table (IMT). 
 * Also, negative values for mpm id indicates invalid mapping.
 *
 * @see mpmint_irq_data_type
 */
#define MPMINT_IMT_EOS_HAL_IRQ    (int16)(-2)

/**
 * This macro is used with MPMINT_EOS_HAL_IRQ in interrupt mapping table
 * (IMT) to mark its end.
 */
#define MPMINT_IMT_EOS_LOCAL_IRQ  MPMINT_NULL_IRQ
    
/*===========================================================================
 *                         INTERNAL TYPE DEFINTIONS
 *==========================================================================*/

/**
 * Mapping of master's interrupts id to the mpm hardware interrupts ids.
 *
 * @Note:
 * For the mapped interrupts/gpios, both the fields of this structure 
 * are non-negative (>=0). A negative value for the first field indicates 
 * invalid entry - either unmapped entry or end of table.
 */
typedef struct
{
  int16  hal_irq;
  uint16 local_irq;
} mpmint_irq_data_type;

/*===========================================================================
 *                       EXTERNAL VARIABLE DECLARATIONS
 *===========================================================================*/
/**
 * Number of interrupts master wants MPM to monitor during deep sleep.
 */
extern uint32 mpmint_num_mapped_irqs;

/**
 * Pointer to the table containing mapping of master interrupt ids to mpm
 * interrupt ids. Depending on particular target, this table may contain
 * intermediate invalid/unmapped entries.
 * 
 * @Note:
 * This pointer can be set either directly or via querying devcfg.
 */
extern mpmint_irq_data_type *mpmint_isr_tbl;

/**
 * Number of entries in mapping table.
 *
 * @Note:
 * This variable can be set directly or during init time if we were
 * using devcfg.
 */
extern uint32 mpmint_isr_tbl_size;

/**
 * Address in message ram for master's mpm configuration registers base.
 */
extern uint32 mpmint_irq_cfg_reg_base_addr;

/**
 * Id for rpm wakeup interrupt to this master. Ideally type of this 
 * variable would be some typedef but those are not allowed in DevCfg xml
 * files which is one of the ways to obtain this value.
 */
extern uint32 mpmint_rpm_wakeup_irq;

/**
 * Flags for rpm wakeup interrupt. As of now this flag just contains
 * trigger type information.
 */
extern uint32 mpmint_rpm_wakeup_irq_flags;

/*===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================*/

/**
 * Gets the table index corresponding to in_id for mpmint_isr_tbl[].
 * 
 * @param in_id[in]    Interrupt for which the table index is required.
 * @param out_id[out]  Table index (only valid if return value of this function
 *                     is TRUE).
 * 
 * @return boolean TRUE if valid index was found; FALSE otherwise.
 */
bool32 mpmint_get_isr_tbl_index
(
  mpmint_isr_type in_id,
  uint32          *out_id
);

/**
 * Triggers the GPIO passed as argument on the master. This function usually
 * called by mpmint_trigger_interrupts while coming out of sleep if it finds
 * particular GPIO occurred at MPM during sleep.
 *
 * Depending on the implementation, validity of gpio_num may or may not be
 * checked. It is meant for vMPM internal use only.
 *
 * @see mpmint_trigger_interrupts
 *
 * @param gpio_num: Actual gpio number that will be triggered on the master.
 */
void mpmint_retrigger_gpio
(
  uint32 gpio_num
);

/**
 * Trigger the interrupt passed as argument on the master. This function 
 * usually called by mpmint_trigger_interrupts while coming out of sleep if
 * it finds particular interrupt occurred at MPM during sleep.
 *
 * Depending on the implementation, validity of gpio_num may or may not be
 * checked. It is meant for vMPM internal use only.
 *
 * @see mpmint_trigger_interrupts
 *
 * @param irq_num: Local IRQ number for the interrupt that will be triggered
 *                 on the master.
 */
void mpmint_retrigger_interrupt
(
  uint32 irq_num
);

/**
 * Initializes all target specific data for common mpm driver to use. 
 * Depending on the OS, this function can query DevCfg APIs to obtain
 * and then intialize target specific data or can have direct assignment.
 * Currently it is expected to initializ following variables
 *   - mpmint_isr_tbl
 *   - mpmint_isr_tbl_size
 *   - mpmint_irq_cfg_reg_base_addr
 *   - mpmint_rpm_wakeup_irq
 *   - mpmint_rpm_wakeup_irq_flags
 *
 * @return none
 */
void mpmint_target_data_init( void );

/**
 * Maps the MPM HW to memory and sets a global variable to contain that
 * virtual address.
 *
 * @return none
 */
void mpm_map_hw_reg_base( void );

/**
 * Initializes log for mpm. This function must be called before using
 * any log functionalities for mpm.
 */
void mpmint_log_init( void );

/**
 * Logs the mpm driver messages in Ulog. A valid ulog handle should be passed
 * as there will not be any checking in this function. For low memory targets
 * such as sensor, this could be a stub function.
 *
 * @Note
 * mpmint_log_init must be called before calling this function.
 *
 * @param arg_count: Number of parameters that are to be logged
 * @param format: The formate string to print logs. Similar to one used in 
 *                printf statements.
 */
void mpmint_log_printf( int arg_count, const char * format, ... );

#endif /* !MPMINTI_H */

