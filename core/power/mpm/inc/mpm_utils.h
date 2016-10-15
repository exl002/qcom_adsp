#ifndef MPM_UTILS_H
#define MPM_UTILS_H

/*=============================================================================


GENERAL DESCRIPTION

  This module contains routines for the multiprocessor power manager (MPM).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

===============================================================================

                           EDIT HISTORY FOR MODULE
                           
$Header: //components/rel/core.adsp/2.2/power/mpm/inc/mpm_utils.h#1 $

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include <stdint.h>
#include "HALcomdef.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

=============================================================================*/

/**
 * Returns the time in sclk unit from MPM counter. MPM counter is expected to
 * be running reliably even in low power modes.
 *
 * @Note:
 * This API is just for legacy code. From B family onwards clients should use
 * QTimer base time which is also expected to work in low power modes.
 *
 * @return Time in sclk at MPM hardware.
 */
uint32 mpm_get_timetick( void );

/**
 * Writes a wake up tick for this master in to memory region reserved for it
 * in the shared message ram during rpm assisted low power modes. RPM 
 * processor uses it to determine wakeup time for that master. It must be
 * noted that during each rpm assited Sleep cycle, master needs to upate 
 * this value.
 *
 * @param wakeup_tick: Time (in ticks) when this master wants rpm to wake
 *                     it up.
 * @return nothing.
 */
void mpm_set_wakeup_timetick(uint64_t wakeup_tick);

#endif /* MPM_UTILS_H */

