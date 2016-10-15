/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_rpm.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef HAL_RPM_H_
#define HAL_RPM_H_

#include "adsppm.h"
#include "asic.h"

//OCMEM state definitions for RPM : OFF = 0, RETENTION =1, ACTIVE = 2
#define RPM_OCMEM_OFF 0
#define RPM_OCMEM_RETENTION 1
#define RPM_OCMEM_ACTIVE 2

typedef struct{
	uint32 regionId;
	uint32 stsValue[ASIC_OCMEM_MAX_BANKNUM];
}HalOcmemPwrMgrStsType;


Adsppm_Status Rpm_Init(void);

Adsppm_Status RPM_RequestOcmemPwr(uint32 updateNum, HalOcmemPwrMgrStsType *pOcmemStsArray);

#endif /* halrpm_H_ */


