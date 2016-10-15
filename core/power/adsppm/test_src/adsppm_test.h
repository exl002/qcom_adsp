/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#ifndef MMPM2_TEST_H
#define MMPM2_TEST_H

#include "mmpm.h"

// Define a macro to use for conditional builds for
// ARM (real target) vs. X86 (simulator) builds.
// Typically the simulator won't run the CRM,
// just the mmpm2_client_test.
#if defined(__X86__)
#define	MMPM2TEST_X86_BUILD
#endif

#include "adsppm_test_main.h"
#include "adsppm_test_utils.h"



uint32 Test_Register(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId);
MMPM_STATUS Test_RequestClk(uint32 clientId, ClkTestType clk[], uint32 numClk);
MMPM_STATUS Test_RequestClkDomain(uint32 clientId, ClkDomainTestType clk[], uint32 numClk);
MMPM_STATUS Test_RequestRegProg(uint32 clientId, uint32 match);
MMPM_STATUS Test_RequestBw(uint32 clientId, BwReqTestType *bw, uint32 numBw);
MMPM_STATUS Test_RequestSleepLatency(uint32 clientId, uint32 microSec);
MMPM_STATUS Test_RequestVreg(uint32 clientId, uint32 milliVol);
MMPM_STATUS Test_RequestMips(uint32 clientId, uint32 mips);
MMPM_STATUS Test_RequestPwr(uint32 clientId);
MMPM_STATUS Test_ReleaseClk(uint32 clientId, uint32 relClkId[], uint32 numClk);
MMPM_STATUS Test_ReleaseClkDomain(uint32 clientId, uint32 relClkId[], uint32 numClk);
MMPM_STATUS Test_ReleaseRegProg(uint32 clientId);
MMPM_STATUS Test_ReleaseBw(uint32 clientId);
MMPM_STATUS Test_ReleaseSleepLatency(uint32 clientId);
MMPM_STATUS Test_ReleaseVreg(uint32 clientId);
MMPM_STATUS Test_ReleaseMips(uint32 clientId);
MMPM_STATUS Test_ReleasePwr(uint32 clientId);
MMPM_STATUS Test_InfoClk(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 clkId, uint32 *clkFreq);
MMPM_STATUS Test_InfoMips(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *mipsInfo);
MMPM_STATUS Test_InfoBW(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint64 *bwInfo);
MMPM_STATUS Test_InfoClkPerfMon(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 clkId, uint32 *clkFreq);
MMPM_STATUS Test_InfoPwr(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *bInfoPower);
MMPM_STATUS Test_Thermal(void);
MMPM_STATUS Test_Deregister(uint32 clientId);
MMPM_STATUS Test_SetParam(uint32 clientId,OcmemSetParamTestType *pOcmemSetParam);
MMPM_STATUS Test_RequestMemPwr(uint32 clientId, uint32 *memType, uint32 *powerState);
MMPM_STATUS Test_ReleaseMemPwr(uint32 clientId);
MMPM_STATUS InvokeTest(AdsppmTestType test[], uint32 numTest, MMPM_STATUS testSts[][MAX_TEST_SEQUENCE]);


#endif
