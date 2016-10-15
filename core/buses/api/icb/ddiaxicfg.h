#ifndef __DDIAXICFG_H__
#define __DDIAXICFG_H__
/**
 * @file ddiaxicfg.h
 * @note External header file. API 2.0 for Bus Configuration. Not compatible with API 1.0
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.adsp/2.2/buses/api/icb/ddiaxicfg.h#1 $ 
 * $DateTime: 2013/04/03 17:22:53 $ 
 * $Author: coresvc $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 11/28/2011   dj      Branched from rev 1.0 and updated to rev 2.0 
 *  
 *  
 * Copyright (c) 2011-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 */ 
#include "icbid.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Clock gating type.
 *  Note: Not all fields apply to all ports.
 */
typedef struct
{
  bool bCoreClockGateEn; /**< Gating of the registers on the clock of the core behind the port. */
  bool bArbClockGateEn;  /**< Gating of the arbiter on the port. */ 
  bool bPortClockGateEn; /**< Gating of the registers on the bus core clock. */
} AxiCfg_ClockGatingType;

/**
  @brief Configure the clock gating for the indicated master.

  @param[in] eMaster    The master port identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetMasterClockGating( ICBId_MasterType eMaster, AxiCfg_ClockGatingType * pGating);

/**
  @brief Configure the clock gating for the indicated slave.

  @param[in] eSlave     The slave way identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetSlaveClockGating( ICBId_SlaveType eSlave, AxiCfg_ClockGatingType * pGating);



#ifdef __cplusplus
}
#endif

#endif /* __DDIAXICFG_H__ */
