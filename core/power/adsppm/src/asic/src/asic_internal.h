/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/**
 *  @file: asic_internal.h contains internal Asic Config Manager's definitions
 *
 *  Created on: Mar 23, 2012
 *  @author: yrusakov
 */

#ifndef ASIC_INTERNAL_H_
#define ASIC_INTERNAL_H_

#include "asic.h"
#include "DDIChipInfo.h"


#define PERCENTS_IN_A_WHOLE  256

#define ASIC_CONFIG_DEVICE_NAME "/core/power/adsppm"

typedef struct{
    int numElements;
    AsicClkDescriptorType *pClocks;
} AsicClockDescriptorArrayType;

typedef struct{
    int numElements;
    AsicBusPortDescriptorType *pPorts;
} AsicBusPortDescriptorArrayType;

typedef struct{
    int numElements;
    AsicPowerDomainDescriptorType *pPwrDomains;
} AsicPwrDescriptorArrayType;

typedef struct{
    int numElements;
    AsicCoreDescType *pCores;
} AsicCoreDescriptorArrayType;

typedef struct{
    int numElements;
    AsicMemDescFullType pMemories[Adsppm_Mem_Max];
} AsicMemDescriptorArrayType;

typedef struct{
    int numElements;
    AdsppmBusRouteType *pRoutes;
} AsicBusRouteArrayType;

/**
 *  Enumeration to indicate function set used for this chip
 */
typedef enum{
    AsicFnSetId_Default = 0//!< AsicFnID_Default
} AsicFnSetIdType;

typedef struct{
    DalChipInfoFamilyType     asicFamily;
    DalChipInfoIdType         asicId;
    DalChipInfoVersionType    asicVersion;
    AsicFnSetIdType functionSet;
    uint32  debugLevel;
    uint32  adspHwThreadNumber;
    uint64  adsppmLprTimeoutValue; //!<LPR timeout value in QTimer ticks (19.2Mhz)
    AsicCoreDescriptorArrayType cores;
    AsicClockDescriptorArrayType clocks;
    AsicMemDescriptorArrayType memories;
    AsicBusPortDescriptorArrayType busPorts;
    AsicBusRouteArrayType extBusRoutes;
    AsicBusRouteArrayType mipsBusRoutes;
    AsicPwrDescriptorArrayType pwrDomains;
    Adsppm_Status (*pFn_GetBWFromMips)(AdsppmMIPSToBWAggaregateType *pMipsAggregateData);
    Adsppm_Status (*pFn_GetClockFromMips)(AdsppmMIPSToClockAggaregateType *pMipsAggregateData);
    Adsppm_Status (*pFn_GetClockFromBW)(uint32 *pClocks, AdsppmBusBWDataIbAbType *pAHBBwData);
    Adsppm_Status (*pFn_BusBWAggregate)(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue);
    AsicFeatureDescType features[AsicFeatureId_enum_max]; //!< Feature enablement status
} AsicConfigType;


typedef struct{
    uint32 numClients;
    uint32 factor;
} AsicFactorType;

#define ADSPPM_ARRAY(array) (sizeof(array)/sizeof(array[0]), &array[0])
#define ADSPPM_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

/**
 * @fn asicGetAsicConfig - fill in ASIC configuration from Device Config image
 * @param pConfig - [OUT] pointer to ASIC configuration structure
 * @return completion status
 */
Adsppm_Status asicGetAsicConfig(AsicConfigType *pConfig);

#endif /* ASIC_INTERNAL_H_ */
