/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

  #ifndef MMPM2_TEST_PARAM_H
#define MMPM2_TEST_PARAM_H

#include "mmpm.h"
#include "adsppm_test_main.h"

/* request memory power parameter table*/
typedef struct{
	uint32  memType;
	uint32  powerState;
}Mmpm2ReqMemPwrTestParamTableType;

/* Request clock parameter table */
typedef struct {
    uint32       numClks;
    ClkTestType  clk[MAX_CLK_REQUEST];
} Mmpm2ReqClkTestParamTableType;

/* Request clock parameter table */
typedef struct {
    uint32       numClks;
    ClkDomainTestType  clk[MAX_CLK_REQUEST];
} Mmpm2ReqClkDomainTestParamTableType;


/* Request bandwidth parameter table */
typedef struct {
    uint32          numBw;
    BwReqTestType   bw[MAX_BW_REQUEST];
} Mmpm2ReqBwTestParamTableType;

/* Release clock parameter table */
typedef struct {
    uint32    numClks;
    uint32    clk[MAX_CLK_REQUEST];
} Mmpm2RelClkTestParamTableType;


/* Frequency plan table */
typedef struct {
    uint32   numFreq;
    uint32   clkId;
    uint32   freq[16];
} FreqPlanParam;

typedef struct {
    uint32           numClks;
    FreqPlanParam    clkParam[MAX_CLK_REQUEST];  
} Mmpm2FreqPlanTestParamTableType;

void GetReqClkParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, ClkTestType clk[], uint32 *numClks);
void GetReqClkDomainParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, ClkDomainTestType clk[], uint32 *numClks);
void GetReqMipsParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *mips);
void GetReqRegProgParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *match);
void GetReqVregParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *milliVol);
void GetReqBWParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, BwReqTestType bw[], uint32 *numBw);
void GetReqSleepLatencyParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *microSec);
void GetReqGpioParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId);
void GetRelClkParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 clk[], uint32 *numClks);
void GetInfoClkParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *clkId);
void GetSetParamData(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, OcmemSetParamTestType *pOcmemSetParam);
void GetReqMemPwrParam(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId, uint32 *memType, uint32 *powerState);

#endif