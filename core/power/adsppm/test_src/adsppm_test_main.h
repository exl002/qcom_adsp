/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/


#ifndef MMPM2_TEST_MAIN_H
#define MMPM2_TEST_MAIN_H

#include <stdio.h>
#include "mmpm.h"
#include <stdarg.h>
#include "ULog.h"
#include "ULogFront.h"

#define MAX_CLK_REQUEST      8
#define MAX_BW_REQUEST       16
#define MAX_TEST_CLIENT      300
#define ITOA_RADIX           10
#define MAX_LEN_CLIENT_NAME  32
#define PHY_MEM_EBI          0x40000000
#define PHY_MEM_IMEM         0x2E000000
#define MAX_TEST             25
#define MAX_TEST_SEQUENCE    25
#define MAX_LOG_STRING_LEN   256
#define MAX_AXI_PORT         2
#define MAX_AXI_CLK          3
#define MAX_AHB_CLK          2

typedef enum {
    NONE,
    LPASS_ADSP,                  /**< Q6SS HW inside of LPASS */
    LPASS_LPM,                  /**< Low Power Memory inside of LPASS */
    LPASS_DML,                  /**< DML (Data Mover Lite) HW inside of LPASS */
    LPASS_AIF,                  /**< AIF(Audio Intreface) HW inside of LPASS */
    LPASS_SLIMBUS,              /**< SLIMBus HW inside of LPASS */
    LPASS_MIDI,                 /**< MIDI HW inside of LPASS */
    LPASS_AVSYNC,               /**< AVT/ProntoSync HW inside of LPASS */
    LPASS_HWRSMP,               /**< HW Resampler HW inside of LPASS */
    LPASS_SRAM,                 /**< SRAM CORE */
    LPASS_DCODEC,               /** < Digital codec inside of LPASS */
    LPASS_SPDIF,                /** < SPDIF inside of LPASS */             
    MAX_CORE	                /**<LPASS CORE ID END */    
} CoreIdType;

typedef enum {
    REG,
    REQCLK,
    REQCLKDOMAIN,
    REQMIPS,
    REQREGP,
    REQVREG,
    REQBW,
    REQPWR,
    REQMEMPWR,
    REQSLATENCY,
    RELCLK,
    RELCLKDOMAIN,
    RELMIPS,
    RELREGP,
    RELVREG,
    RELSPATENCY,
    RELBW,
    RELPWR,
    RELMEMPWR,
    INFOCLK,
    INFOCLKP, //get info clk using perfmon and using h/w counter
    INFOPWR,
    THERML,
    DREG,
    VERPWR,
    SETLOG,
    SETPARAM,
    MAX_NUM_TEST
} TestSequenceType;

typedef enum
{
   MMPM_LOG_LEVEL_CRITICALFAIL = 1,
   MMPM_LOG_LEVEL_ERROR,
   MMPM_LOG_LEVEL_TESTSTART,
   MMPM_LOG_LEVEL_TESTEND,
   MMPM_LOG_LEVEL_WARNING,
   
   MMPM_LOG_LEVEL_DEBUG=10,
   MMPM_LOG_LEVEL_MAX 
}MmmpmTestloglevelType;


typedef struct
{
    uint32 clkId;
    uint32 freq;
    uint32 freqMatch;
} ClkTestType;

typedef struct
{
    uint32             clkId;
    uint32             clkFreqHz; 			 
    uint32             clkDomainSrc;
    /**< Clock domain source ID. */
}ClkDomainTestType;


typedef struct
{
    uint32 masterPort;
    uint32 slavePort;
    uint32 bwVal;
    uint32 usagePercent;
    uint32 usageType;
} BwReqTestType;

typedef struct
{
	uint32                 mipsTotal;                         
    uint32                 mipsPerThread;                
    uint32				   codeLocation;  
    uint32                 reqOperation;  
}MipsReqTestType;

typedef struct
{
	uint32     memType;
	uint32     powerState;
}MemPwrReqTestType;

typedef struct
{
	uint32 regionId;
	uint32 numKeys;
	uint32 pKey[4];
}OcmemRegionType;

typedef struct
{
	uint32 numRegions;
	OcmemRegionType pOcemRegion[3];	
}OcmemMapType;

typedef struct{
    uint32 paramId; //!< Parameter ID
	OcmemMapType ocmemMap;
}OcmemSetParamTestType;


/* Test are defined in this structure */
typedef struct {
    uint32            clientNum;
    uint32            numTestSeq;
    uint32            newThread;   //0 same thread; 1 new thread
    uint32            coreId;
    uint32            instanceId;
    TestSequenceType  testSeq[MAX_TEST];
    void              *testParam[MAX_TEST];
} AdsppmTestType;


typedef struct{
	ULogHandle test_hLog;
    uint32 test_log_buffer_size;
	uint32 testLogLevel;
}AdsppmTestGlbCtxType;


typedef MMPM_STATUS (VerifyFunc)(uint32);

//void adsppmtest_main(void);

void AdsppmTestDebugLog(char *format, ... );
void AdsppmTestDebugLogTestStart(char *name, int progmax);
void AdsppmTestDebugLogTestUpdate(int progress);
void AdsppmTestDebugLogTestDone(int sts);


/* MMPM2 tests */
void Test_Clock(void);
void Test_Power(void);
void Test_Bw(void);
void Test_Stress(void);
void Test_AxiAhb(void);
void Test_DomClock(void);
void Test_MipsSleep(void);
void Test_System(void);
void Test_Bundle(void);
void Test_MemPower(void);

#endif
