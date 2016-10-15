/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#ifndef MMPM2_TEST_UTILS_H
#define MMPM2_TEST_UTILS_H


#include "ULog.h"
#include "ULogFront.h"
#include "DALSys.h"


//Logging levels

#define ADSPPMTEST_LOG_LEVEL_RESULT 0 /** TEST CASE RESULT*/
#define ADSPPMTEST_LOG_LEVEL_ERROR 1 /** Error log messages*/
#define ADSPPMTEST_LOG_LEVEL_WARNING 2 /** Warnings */
#define ADSPPMTEST_LOG_LEVEL_INFO 3 /** Critical info messages */
#define ADSPPMTEST_LOG_LEVEL_INFO_EXT 4 /** Extended info messages */
#define ADSPPMTEST_LOG_LEVEL_FN_ENTRY 5 /** Function entry/exit messages */
#define ADSPPMTEST_LOG_LEVEL_DEBUG 6 /** All debug messages */
#define ADSPPMTEST_LOG_LEVEL_MAX 7

__inline ULogHandle GetTestUlogHandle(void);
__inline uint32 GetTestDebugLevel(void);

#ifndef NO_ULOG
#define ADSPPM_TESTLOG_PRINTF_0( level, formatStr) \
	if(GetTestDebugLevel() >= level) {ULOG_RT_PRINTF_0(GetTestUlogHandle(), formatStr);}
#define ADSPPM_TESTLOG_PRINTF_1( level, formatStr, p1) \
	if(GetTestDebugLevel() >= level) {ULOG_RT_PRINTF_1(GetTestUlogHandle(), formatStr, p1);}
#define ADSPPM_TESTLOG_PRINTF_2( level, formatStr, p1,p2) \
	if(GetTestDebugLevel() >= level) {ULOG_RT_PRINTF_2(GetTestUlogHandle(), formatStr, p1,p2);}
#define ADSPPM_TESTLOG_PRINTF_3( level, formatStr, p1,p2,p3) \
	if(GetTestDebugLevel() >= level)	{ULOG_RT_PRINTF_3(GetTestUlogHandle(), formatStr, p1,p2,p3);}
#define ADSPPM_TESTLOG_PRINTF_4( level, formatStr, p1,p2,p3,p4) \
	if(GetTestDebugLevel() >= level)	{ULOG_RT_PRINTF_4(GetTestUlogHandle(), formatStr, p1,p2,p3,p4);}
#define ADSPPM_TESTLOG_PRINTF_5( level, formatStr, p1,p2,p3,p4, p5) \
	if(GetTestDebugLevel() >= level)	{ULOG_RT_PRINTF_5(GetTestUlogHandle(), formatStr, p1,p2,p3,p4,p5);}
#else
#define ADSPPM_TESTLOG_PRINTF_0( level, formatStr)
#define ADSPPM_TESTLOG_PRINTF_1( level, formatStr, p1)
#define ADSPPM_TESTLOG_PRINTF_2( level, formatStr, p1,p2)
#define ADSPPM_TESTLOG_PRINTF_3( level, formatStr, p1,p2,p3)
#define ADSPPM_TESTLOG_PRINTF_4( level, formatStr, p1,p2,p3,p4)
#define ADSPPM_TESTLOG_PRINTF_5( level, formatStr, p1,p2,p3,p4,p5)

#endif


#endif
