/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * mempwrmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef OCMEMPWRMGR_H_
#define OCMEMPWRMGR_H_

#include "adsppm.h"
#include "requestmgr.h"
#include "asic.h"
#include "hal_rpm.h"

/**
 * Definition of the ocmem Power Manager Context.
 */
typedef struct{
    DALSYSSyncHandle       ctxLock;//
    AdsppmMemPowerStateType ocmemPwrSts[ASIC_OCMEM_MAX_REGIONNUM][ASIC_OCMEM_MAX_BANKNUM][2];
    AdsppmMemPowerStateType memPwrState[Adsppm_Mem_Max -Adsppm_Mem_Lpass_LPM][2]; // only for internal memories
    uint32 activeStateIndex;
    HalOcmemPwrMgrStsType updateSts[ASIC_OCMEM_MAX_REGIONNUM];
} memPwrMgrContextType;


Adsppm_Status MEMPWR_Init(void);

Adsppm_Status MEMPWR_ProcessRequest(coreUtils_Q_Type *pMemPwrReqQ);


#endif /* OCMEMPWRMGR_H_ */
