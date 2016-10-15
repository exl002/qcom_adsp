/*============================================================================
@file ULogFront.h

Frontend interfaces for ULog Core.  Simplified access to the logging mechanism.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.adsp/2.2/api/power/ULogFront.h#2 $
============================================================================*/
#ifndef ULOGFRONT_H
#define ULOGFRONT_H

#include "ULog.h"
#include "stdarg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_RAW
 * -------------------------------------------------------------------------*/

/**
 * <!-- ULogFront_RawInit -->
 *
 * @brief Create a new log in the RAW format.
 * 
 * Create a new raw log, See ULogCore_LogCreate for details.
 * 
 * @param h : See ULogCore_LogCreate.
 * @param name : See ULogCore_LogCreate.
 * @param logBufSize : See ULogCore_LogCreate.
 * @param logBufMemType : See ULogCore_LogCreate.
 * @param logLockType : See ULogCore_LogCreate.
 *
 * @return See ULogCore_LogCreate.
*/
ULogResult ULogFront_RawInit(ULogHandle * h, 
                             const char * name,
                             uint32 logBufSize,
                             uint32 logBufMemType,
                             ULOG_LOCK_TYPE logLockType);

/**
 * <!-- ULogFront_RawLog -->
 *
 * @brief Log data to a raw log.
 *
 * @param h  : A ULog handle.
 * @param dataArray : A pointer to the data to log.
 * @param dataCount : The amount of data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RawLog(ULogHandle h, 
                            const char * dataArray,
                            uint32 dataCount);

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_REALTIME
 * -------------------------------------------------------------------------*/

/** The value for x MUST be a 4 byte "float" variable.  It cannot be immediate data or an 8 byte "double" */
#define ULOG_RT_FLOAT(x) (*((uint32 *)((void *)&x)))

/** Macros for splitting up a 64 bit value */
#define ULOG64_LOWWORD(x) (uint32)x
#define ULOG64_HIGHWORD(x) (uint32)((uint64)x>>32)
#define ULOG64_DATA(x) (uint32)x, (uint32)((uint64)x>>32)


/**
 * <!-- ULogFront_RealTimeInit -->
 *
 * @brief Create a new log in the RealTime format.
 *
 * Create a new RealTime log, See ULogCore_LogCreate for details.
 *
 * @param h : See ULogCore_LogCreate.
 * @param logName : See ULogCore_LogCreate.
 * @param bufferSize : See ULogCore_LogCreate.
 * @param memoryType : See ULogCore_LogCreate.
 * @param lockType : See ULogCore_LogCreate.
 *
 * @return See ULogCore_LogCreate.
*/
ULogResult ULogFront_RealTimeInit(ULogHandle * h, 
                             const char * name,
                             uint32 logBufSize,
                             uint32 logBufMemType,
                             ULOG_LOCK_TYPE logLockType);

/**
 * <!-- ULogFront_RealTimePrintf -->
 *
 * @brief Log data in the printf format.
 *
 * Log printf data to a RealTime log.  The printf formating is
 * not executed until the log is read. This makes a very performant call,
 * but also means all strings must be present when the log is read.
 * This function does the most work for ULog logging.  It's been optimized to 
 * write directly to the buffer if possible. 
 *
 * @param h : A ULog handle.
 * @param dataArray : The number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimePrintf(ULogHandle h, 
                                    uint32 dataCount, 
                                    const char * formatStr,
                                    ...);
/**
 * <!-- ULOG_RT_PRINTF_N -->
 *
 * @brief Macros to provide some parameter count safety to the ULog_RealTimePrintf function.
 *
 * These macros provide parameter count safety to the
 * ULog_RealTimePrintf function.  They can be used instead of
 * calling the function directly.
 *
 * @param h : A handle indicating where to log.
 * @param formatStr : The format string for the printf.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_PRINTF_0(h, formatStr)    \
             ULogFront_RealTimePrintf(h, 0, formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1)    \
             ULogFront_RealTimePrintf(h, 1, formatStr, p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1, p2)    \
             ULogFront_RealTimePrintf(h, 2, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)    \
             ULogFront_RealTimePrintf(h, 3, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)    \
             ULogFront_RealTimePrintf(h, 4, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)    \
             ULogFront_RealTimePrintf(h, 5, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULogFront_RealTimePrintf(h, 6, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULogFront_RealTimePrintf(h, 7, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULogFront_RealTimePrintf(h, 8, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULogFront_RealTimePrintf(h, 9, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULogFront_RealTimePrintf(h, 10, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- ULogFront_RealTimeVprintf -->
 * 
 * @brief Log data in the vprintf format.
 *
 * Log printf data to a RealTime log.  The printf formativ is not
 * executed until the log is pulled.  This makes a very performant call,
 * but also means all strings must be present when the log is read.
 *
 * @note This function serves identical purpose to ULogFront_RealTimePrintf,
 *       except it takes a va_list argument instead of being variadic.
 *
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ap : A va_list of the parameters being printed.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeVprintf(ULogHandle h, 
                                     uint32 dataCount, 
                                     const char * formatStr,
                                     va_list ap);

/**
 * <!-- ULogFront_RealTimeData -->
 *
 * @brief Log uint32 parameters.
 *
 * Log uint32 data to a RealTime log.  Data is provided as
 * variatic arguments to this function.
 *  
 * @param h: A ULog handle.
 * @param dataCount: the number of parameters being logged.  Can
 *                  be a maximum of 10.
 * @param ... : The parameters to log.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeData(ULogHandle h, uint32 dataCount, ...);


/**
 * <!-- ULOG_RT_DATA_N -->
 *
 * @brief Macros to provide some parameter count safety to the ULogFront_RealTimeData function.
 * 
 * These macros provide parameter count safety to the
 * ULogFront_RealTimeData function.  They can be used instead of
 * calling the function directly.
 *
 * @param h : A handle indicating where to log.
 * @param p1 - pN : The data to log.  These must be (u)int32 in
 *           length.  Smaller data types will be automatically
 *           promoted.
 */
#define ULOG_RT_DATA_1(h, p1)    \
             ULogFront_RealTimeData(h, 1, p1)
#define ULOG_RT_DATA_2(h, p1, p2)    \
             ULogFront_RealTimeData(h, 2, p1, p2)
#define ULOG_RT_DATA_3(h, p1, p2, p3)    \
             ULogFront_RealTimeData(h, 3, p1, p2, p3)
#define ULOG_RT_DATA_4(h, p1, p2, p3, p4)    \
             ULogFront_RealTimeData(h, 4, p1, p2, p3, p4)
#define ULOG_RT_DATA_5(h, p1, p2, p3, p4, p5)    \
             ULogFront_RealTimeData(h, 5, p1, p2, p3, p4, p5)
#define ULOG_RT_DATA_6(h, p1, p2, p3, p4, p5, p6)    \
             ULogFront_RealTimeData(h, 6, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_DATA_7(h, p1, p2, p3, p4, p5, p6, p7)    \
             ULogFront_RealTimeData(h, 7, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_DATA_8(h, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULogFront_RealTimeData(h, 8, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_DATA_9(h, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULogFront_RealTimeData(h, 9, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_DATA_10(h, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULogFront_RealTimeData(h, 10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 * <!-- ULogFront_RealTimeCharArray -->
 *
 * @brief Log byte data to a RealTime log.  
 * 
 * @param h : A ULog handle.
 * @param byteCount : the number of bytes to log.
 * @param byteData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeCharArray(ULogHandle h, 
                                       uint32 byteCount, 
                                       char * byteData);


/**
 * <!-- ULogFront_RealTimeString -->
 *
 * @brief Log string data to a RealTime log.
 *
 * This function copies the full string to the log.
 *
 * @param h : A ULog handle.
 * @param string : The string to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeString(ULogHandle h, char * cStr);
 

/**
 * <!-- ULogFront_RealTimeWordArray -->
 *
 * @brief Log word data to a RealTime log.
 *
 * @param h : A ULog handle.
 * @param wordCount : The number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeWordArray(ULogHandle h, 
                                       uint32 wordCount, 
                                       const uint32 * wordData);


/**
 * <!-- ULogFront_RealTimeCsv -->
 *
 * @brief Log word data to a RealTime log. Output will be a CSV compatible format.
 *
 * @param h  : A ULog handle.
 * @param wordCount : the number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeCsv(ULogHandle h, 
                                 uint32 wordCount, 
                                 const uint32 * wordData);


/**
 * <!-- ULogFront_RealTimeVector -->
 *
 * @brief Log vector data to a RealTime log.
 *
 * Output will be a vector with the provided format. No newlines will be inserted
 *
 * @param h : A ULog handle.
 * @param formatStr : A format string with a single %x, %d, %i, or %u 
 * @param entryByteCount : 1, 2 or 4 byte size data 
 * @param vectorLength :  How man elements to store. 
 * @param vector : Pointer to the array of elements.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeVector(ULogHandle h, 
                                    const char * formatStr,
                                    unsigned short entryByteCount,
                                    unsigned short vectorLength,
                                    const void * vector);


/**
 * <!-- ULogFront_RealTimeMultipartMsgBegin -->
 *
 * @brief Begin a multipart message.
 *
 * The log will be locked for the duration of the multipart message.  To
 * free the lock, call ULogFront_RealTimeMultipartMsgEnd. The first message of a 
 * MPM is always a ULogFront_RealTimePrintf with %m specifiers for the parts to 
 * add in the subsequent logging before calling ULogFront_RealTimeMultipartMsgEnd.
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 *
 * @return TRUE if the mpm was successfully started. False if it wasn't and
 *         the multipart message should not be continued further
 */
DALBOOL ULogFront_RealTimeMultipartMsgBegin (ULogHandle h);


/**
 * <!-- ULogFront_RealTimeMultipartMsgEnd -->
 *
 * @brief End a multipart message
 *
 * The log will be locked for the duration of the multipart message.  
 * This function frees the lock, ends the MPM message and completes the MPM bookeeping.
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 */
void ULogFront_RealTimeMultipartMsgEnd (ULogHandle h);

#ifdef __cplusplus
}
#endif

#endif // ULOGFRONT_H
