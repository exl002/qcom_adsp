#ifndef MPMINT_TYPES_H
#define MPMINT_TYPES_H

/*===========================================================================
               Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/* The PIC hardware can support the setting of edge or level
 * triggering on some interrupts. 
 */
typedef enum
{
  MPMINT_LEVEL_DETECT = 0,
  MPMINT_EDGE_DETECT
} mpmint_detect_type;

/* The PIC hardware can support the setting of polarity
 * triggering on some interrupts.
 */
typedef enum
{
  MPMINT_ACTIVE_LOW     = 0,
  MPMINT_ACTIVE_FALLING = 0,
  MPMINT_ACTIVE_HIGH    = 1,
  MPMINT_ACTIVE_RISING  = 1,
  MPMINT_ACTIVE_DUAL    = 2,
} mpmint_polarity_type;

/* For dynamically configured interrupts which processor 
 * handles normally during active time.
 */
typedef enum
{
  MPMINT_TRIGGER_LEVEL_LOW      = 0,
  MPMINT_TRIGGER_EDGE_FALLING   = 1,
  MPMINT_TRIGGER_EDGE_RISING    = 2,
  MPMINT_TRIGGER_EDGE_DUAL      = 3,
  MPMINT_TRIGGER_LEVEL_HIGH     = 4,
}mpmint_trigger_type;

typedef enum
{
  MPMINT_RPM = 0,
  MPMINT_APPS,
  MPMINT_MODEM,
  MPMINT_LPASS
} mpmint_processor_type;


/* Stores various information about an interrupt. This structure is mainly
 * used when there is a need to configure/enable/disable more than one
 * mpm mapped interrupts.
 *
 * @Note
 * First two fields are supposed to be filled in with query/mapping
 * functions like mpmint_map_interrupts_with_master (see below).
 *
 * For Trigger,
 * Refer to mpmint_trigger_type enum.
 *
 * For Status,
 * 0  =  Disable
 * 1  =  Enable
 */
typedef struct
{
  uint32 mpm_hw_int_id;            /* Actual interrupt id at MPM hardware */
  uint32 master_hw_int_id;         /* Interrupt id at master hardware */
  uint8 trigger;                   /* Trigger for the interrupt at mpm */
  uint8 status;                    /* Interrupt status */
}mpmint_config_info_type;

/* Pointer to an isr function that is passed within tramp services */
typedef void (*mpm_isr_ptr_type) (void);
                                                                                 
#endif  /* MPMINT_TYPES_H */

