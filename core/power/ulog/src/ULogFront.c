/*============================================================================
@file ULogFront.c

This file implements the front end interfaces to ULog.  

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.adsp/2.2/power/ulog/src/ULogFront.c#2 $
============================================================================*/
#include "ULogBuildFeatures.h"
#include "stdarg.h"
#include "DALSys.h"
#include "ULog.h"
#include "ULogFront.h"
#include "ULogCoreInternal.h"
#include "ULogInlines.h"
#include "CoreTime.h"
#include "DALStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFEATURE_ULOG_FRONT_REALTIME
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
ULogResult ULogFront_RealTimeInit( ULogHandle * h,
                                   const char * name,
                                   uint32 logBufSize,
                                   uint32 logBufMemType,
                                   ULOG_LOCK_TYPE logLockType )
{
  return(ULogCore_LogCreate(h, name, logBufSize, logBufMemType, logLockType, ULOG_INTERFACE_REALTIME));
}


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
ULogResult ULogFront_RealTimeVprintf( ULogHandle h,
                                      uint32 dataCount,
                                      const char * formatStr,
                                      va_list ap )
{
  uint32 total_size;
  uint32 dataArray[15];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32* wordPtr;
  uint32 i;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;


  if ((dataCount>10) || (h==NULL)){
    return(DAL_ERROR);
  }

  // msgSize is len/fmt(4) + format string(4) + TS + data
  total_size = 8 + ULOG_TIMESTAMP_SIZE + dataCount*4; 

  switch(ULogInternal_FastFmtSizeTS(h, total_size, ULOG_REALTIME_SUBTYPE_PRINTF)){

    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg. 

      //pointer to the format string
      wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
      *wordPtr = (uint32) formatStr;
      log->writerCore->write+=4;

      //any arguments
      for(i = 0; i < dataCount; i++)
      {
        wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        *wordPtr = (uint32)va_arg(ap, uint32);
        log->writerCore->write+=4;
      }

      //fill in the size
      wordPtr = (uint32*)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK;

      ULogInternal_LockRelease(log);

      return (DAL_SUCCESS);


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes. 
      //Use the virtualized core->write to handle Tee, STM, or Alternate writes. 
      if(ULogCore_IsEnabled(h, &core) == TRUE)
      {
        *dataPtr++ = ULOG_REALTIME_SUBTYPE_PRINTF;
#ifdef ULOG_64BIT_TIME
        timestamp = CoreTimetick_Get64();
        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp>>32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif
        *dataPtr++ = (uint32)formatStr;

        for(i = 0; i < dataCount; i++)
        {
          *dataPtr++ = va_arg(ap, uint32);
        }

        retval = core->write (h, total_size, (char *)dataArray,  0, NULL, ULOG_INTERFACE_REALTIME);
      }
      return retval;


      case ULOG_FAST_TO_RAM_FAIL:
        //the log is disabled, or some argument is wrong. 
        return(DAL_ERROR);
  }

  return(DAL_ERROR);
}


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
 * @param dataArray : the number of parameters being printed (Not including
 *                    the formatStr).  Limited to 10 parameters.
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimePrintf(ULogHandle h, uint32 dataCount, const char * formatStr, ...)
{
  uint32 total_size;
  uint32 dataArray[14];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32* wordPtr;
  uint32 i;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;
  va_list    ap;


  if ((dataCount>10) || (h==NULL)){
    return(DAL_ERROR);
  }

  // msgSize is len/fmt(4) + format string(4) + TS + data
  total_size = 8 + ULOG_TIMESTAMP_SIZE + dataCount*4; 

  switch(ULogInternal_FastFmtSizeTS(h, total_size, ULOG_REALTIME_SUBTYPE_PRINTF)){

    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg. 
      va_start(ap, formatStr);
      //pointer to the format string
      wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
      *wordPtr = (uint32) formatStr;
      log->writerCore->write+=4;

      //any arguments
      for(i = 0; i < dataCount; i++)
      {
        wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        *wordPtr = (uint32)va_arg(ap, uint32);
        log->writerCore->write+=4;
      }
      va_end(ap);

      //fill in the size
      wordPtr = (uint32*)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK;

      ULogInternal_LockRelease(log);
      return (DAL_SUCCESS);


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes. 
      //Use the virtualized core->write to handle Tee, STM or Alternate writes. 
      va_start(ap, formatStr);

      if(ULogCore_IsEnabled(h, &core) == TRUE)
      {
        *dataPtr++ = ULOG_REALTIME_SUBTYPE_PRINTF;
#ifdef ULOG_64BIT_TIME
        timestamp = CoreTimetick_Get64();
        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp>>32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif
        *dataPtr++ = (uint32)formatStr;

        for(i = 0; i < dataCount; i++)
        {
          *dataPtr++ = va_arg(ap, uint32);
        }

        retval = core->write (h, total_size, (char *)dataArray,  0, NULL, ULOG_INTERFACE_REALTIME);
      }
      va_end(ap);
      return retval;


      case ULOG_FAST_TO_RAM_FAIL:
        //the log is disabled, or some argument is wrong. 
        return(DAL_ERROR);
  }

  return(DAL_ERROR);
}


/**
 * <!-- ULogFront_RealTimeData -->
 *
 * @brief Log uint32 data to a RealTime log.
 *
 * Data is provided as variatic arguments to this function.
 *  
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being logged.  Can
 *                  be a maximum of 10.
 * @param ... : The parameters to log.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeData(ULogHandle h, uint32 dataCount, ...)
{
  uint32 total_size;
  uint32 dataArray[14];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  va_list ap;
  uint32 i;
  uint32* wordPtr;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
 
  if ((dataCount > 10) ||
      (dataCount == 0) ||
      (h == NULL))
  {
    return(DAL_ERROR);
  }

  //size/fmt + TS + data
  total_size = 4 + ULOG_TIMESTAMP_SIZE + dataCount*4; 

  switch(ULogInternal_FastFmtSizeTS(h, total_size, ULOG_REALTIME_SUBTYPE_WORDDATA)){

    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg. 
      va_start(ap, dataCount);
      for(i = 0; i < dataCount; i++)
      {
        wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        *wordPtr = (uint32)va_arg(ap, uint32);
        log->writerCore->write+=4;
      }
      va_end(ap);

      //fill in the size
      wordPtr = (uint32*)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK;

      ULogInternal_LockRelease(log);
      return (DAL_SUCCESS);


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes. 
      if(ULogCore_IsEnabled(h, &core) == TRUE)   
      {
        *dataPtr++ = ULOG_REALTIME_SUBTYPE_WORDDATA; 
#ifdef ULOG_64BIT_TIME
    	  timestamp = CoreTimetick_Get64();
        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp>>32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif
        va_start(ap, dataCount);
        for(i = 0; i < dataCount; i++)
        {
          *dataPtr++ = va_arg(ap, uint32);
        }
        va_end(ap);

        retval = core->write (h, total_size, (char *)dataArray, 0, NULL, ULOG_INTERFACE_REALTIME);
      }
      return retval;


      case ULOG_FAST_TO_RAM_FAIL:
        //the log is disabled, or some argument is wrong. 
        return(DAL_ERROR);
  }

  return(DAL_ERROR);
}


/**
 * <!-- ULogFront_RealTimeCharArray -->
 *
 * @brief Log byte data to a RealTime log.  
 *
 * core->write will use memcpy on the passed in array so we're not doing the 
 * ULogInternal_FastFmtSizeTS type optimizations we do in other functions. 
 * 
 * @param h : A ULog handle.
 * @param byteCount : the number of bytes to log.
 * @param byteData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeCharArray( ULogHandle h,
                                        uint32 byteCount,
                                        char * byteData )
{
  uint32 logData[3];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;


  if ((byteCount == 0) ||
      (byteCount > (1020-ULOG_TIMESTAMP_SIZE)) ||
      (h==NULL)){
    return(DAL_ERROR);
  }

  if((byteData != NULL) && (ULogCore_IsEnabled(h, &core) == TRUE))
  {
    logData[0] = ULOG_REALTIME_SUBTYPE_BYTEDATA;

#ifdef ULOG_64BIT_TIME
    timestamp = CoreTimetick_Get64();
    logData[1] = (uint32)timestamp;
    logData[2] = (uint32)((uint64)timestamp>>32);
#else
    logData[1] = CoreTimetick_Get();
#endif
    //len/format, and time, and then bytes
    retval = core->write(h, 4 + ULOG_TIMESTAMP_SIZE , (char *)logData,  byteCount, byteData, ULOG_INTERFACE_REALTIME);

  }
  return retval;
}


/**
 * <!-- ULogFront_RealTimeString -->
 *
 * @brief Log string data to a RealTime log.
 *
 * This function copies the full string to the log.
 * core->write will use memcpy on the passed in array so we're not doing the 
 * ULogInternal_FastFmtSizeTS type optimizations we do in other functions. 
 *
 * @param h : A ULog handle.
 * @param string : The string to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeString(ULogHandle h, char * cStr)
{
  uint32 logData[3];
  uint32 length;
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;

  if ((cStr==NULL) || (h==NULL)){
    return(DAL_ERROR);
  }

  length = strlen(cStr);  //Note: we don't include the NULL termination in the stored string. 

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    logData[0] = ULOG_REALTIME_SUBTYPE_STRINGDATA;
    
#ifdef ULOG_64BIT_TIME
    timestamp = CoreTimetick_Get64();
    logData[1] = (uint32)timestamp;
    logData[2] = (uint32)((uint64)timestamp>>32);
#else
    logData[1] = CoreTimetick_Get();
#endif

    retval = core->write (h, 4 + ULOG_TIMESTAMP_SIZE, (char *)logData, length, cStr, ULOG_INTERFACE_REALTIME);
  }

  return retval;
}


/**
 * <!-- ULogFront_RealTimeWordBaseFcn -->
 *
 * @brief A generic function to write word data into the log.
 *
 * core->write will use memcpy on the passed in array so we're not doing the 
 * ULogInternal_FastFmtSizeTS type optimizations we do in other functions. 
 *
 */
ULogResult ULogFront_RealTimeWordBaseFcn( ULogHandle h,
                                          uint32 wordCount,
                                          const uint32 * wordData,
                                          uint32 subformatId )
{
  uint32 logData[3];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;

  if ((wordData==NULL)  ||
      (wordCount == 0)  ||
      (wordCount > 200) || /*Setting a large max value here.*/
      (h==NULL))
  {
    return(DAL_ERROR);
  }

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    logData[0] = subformatId;
#ifdef ULOG_64BIT_TIME
    timestamp = CoreTimetick_Get64();
    logData[1] = (uint32)timestamp;
    logData[2] = (uint32)((uint64)timestamp>>32);
#else
    logData[1] = CoreTimetick_Get();
#endif
    retval = core->write (h, 4 + ULOG_TIMESTAMP_SIZE, (char *)logData, wordCount*4, (char *)wordData, ULOG_INTERFACE_REALTIME);
  }

  return retval;
}


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
ULogResult ULogFront_RealTimeWordArray( ULogHandle h,
                                        uint32 wordCount,
                                        const uint32 * wordData )
{
  return(ULogFront_RealTimeWordBaseFcn(h, wordCount, wordData, ULOG_REALTIME_SUBTYPE_WORDDATA));
}


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
ULogResult ULogFront_RealTimeCsv( ULogHandle h,
                                  uint32 wordCount,
                                  const uint32 * wordData )
{
  return(ULogFront_RealTimeWordBaseFcn(h, wordCount, wordData, ULOG_REALTIME_SUBTYPE_CSVDATA));
}


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
ULogResult ULogFront_RealTimeVector( ULogHandle h,
                                     const char * formatStr,
                                     unsigned short entryByteCount,
                                     unsigned short vectorLength,
                                     const void * vector )
{
  ULogResult retval = DAL_ERROR;
  uint32 logData[5];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULOG_CORE_VTABLE * core;

  if ((vector == NULL)     ||
      (formatStr ==  NULL) ||
      (vectorLength == 0)   || 
      (vectorLength > 200) || /*Setting a large max value here.*/
      (h==NULL))
  {
    return(DAL_ERROR);
  }
  
  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    // We only handle these byte entry values.
    if ((entryByteCount == 4) ||
        (entryByteCount == 1) ||
        (entryByteCount == 2))
    {
      logData[0] = ULOG_REALTIME_SUBTYPE_VECTOR;
#ifdef ULOG_64BIT_TIME
      timestamp = CoreTimetick_Get64();
      logData[1] = (uint32)timestamp;
      logData[2] = (uint32)((uint64)timestamp>>32);
      logData[3] = (uint32)formatStr;
      logData[4] = entryByteCount << 16 | vectorLength;
#else
      logData[1] = CoreTimetick_Get();
      logData[2] = (uint32)formatStr;
      logData[3] = entryByteCount << 16 | vectorLength;
#endif
      retval = core->write (h,  12 + ULOG_TIMESTAMP_SIZE, (char *)logData, vectorLength * entryByteCount, (char *)vector, ULOG_INTERFACE_REALTIME);
    }
  }
  return retval;
}


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
DALBOOL ULogFront_RealTimeMultipartMsgBegin(ULogHandle h)
{
  ULOG_CORE_VTABLE * core;
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (h==NULL)
  {
    return(FALSE);
  }

#ifndef DEFEATURE_ULOG_TEE
  if (HANDLE_IS_A_TEE(log))
  {
    ULOG_TEE_LINK_TYPE * link;
    ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)log;
    link = tee->link;
    while (link != NULL)
    {
      if ((link->log != NULL) &&
          (link->log->sharedHeader != NULL))
      {
        ULogInternal_LockGet(link->log);      
      }
      link = link->next;
    }
  }
#endif

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    return(core->multipartMsgBegin(h));
  }

  return(FALSE);
}


/**
 * <!-- ULogFront_RealTimeMultipartMsgEnd -->
 *
 * @brief End a multipart message
 *
 * Write the size into the start of the MPM and release the MPM lock 
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 */
void ULogFront_RealTimeMultipartMsgEnd(ULogHandle h)
{
  ULOG_CORE_VTABLE * core;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  
  if (h==NULL)
  {
    return;
  }

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    core->multipartMsgEnd(h);
  }

#ifndef DEFEATURE_ULOG_TEE
  if (HANDLE_IS_A_TEE(log))
  {
    ULOG_TEE_LINK_TYPE * link;
    ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)log;
    link = tee->link;
    while (link != NULL)
    {
      if ((link->log != NULL) &&
          (link->log->sharedHeader != NULL))
      {
        ULogInternal_LockRelease(link->log);      
      }
      link = link->next;
    }
  } 
#endif
}
#endif /* DEFEATURE_ULOG_FRONT_REALTIME */


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
                             ULOG_LOCK_TYPE logLockType)
{
    return(ULogCore_LogCreate(h, name, logBufSize, 
                              logBufMemType, 
                              logLockType, 
                              ULOG_INTERFACE_RAW));
}


/**
 * <!-- ULogFront_RawLog -->
 *
 * @brief Log data to a raw log.
 *
 * @param h : A ULog handle.
 * @param dataArray : A pointer to the data to log.
 * @param dataCount : The amount of data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RawLog( ULogHandle h,
                             const char * dataArray,
                             uint32 dataCount )
{
  uint32 total_size;
  uint32* wordPtr;

  unsigned int padding=0;
  unsigned int bytesToEnd, copy1Count, copy2Count;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  ULOG_CORE_VTABLE * core;

  if ((dataArray == NULL) ||
      (h == NULL))
  {
    return(DAL_ERROR);
  }

  //len/fmt word (4 bytes) + data(at least 1 byte)
  total_size = 4 + dataCount;

  if (total_size < MIN_RAW_MSG_LEN){
    return(DAL_ERROR);
  }

  if (ULogInternal_UseFastRam(log)){
    
    ULogInternal_LockGet(log);

    if(ULogInternal_ReadyForMessage(log, total_size)==FALSE)
    {
      ULogInternal_LockRelease(log);
      return(DAL_ERROR);
    }

    // len/fmt word  (Raw logs set format to 0)
    wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
    //save this important index and size to use at the end of the write
    log->transactionWriteStart = log->writerCore->write;
    *wordPtr = 0;  //keep the size zero until the msg is completely written.

    //advance the write pointer past the fmt/size word we just wrote
    log->writerCore->write+=4;

    //memcpy the rest of the bytes into the buffer.
    bytesToEnd = log->bufSize - (log->writerCore->write & log->bufSizeMask);

    if (dataCount <= bytesToEnd)
    {
      memcpy(&(log->buffer[log->writerCore->write & log->bufSizeMask]), dataArray, dataCount);
    }
    else
    {
      copy1Count = bytesToEnd;
      copy2Count = dataCount - bytesToEnd;
      memcpy(&log->buffer[log->writerCore->write & log->bufSizeMask], dataArray, copy1Count);
      memcpy(&log->buffer[0], &dataArray[copy1Count], copy2Count);
    }
  
    //make sure the write pointer ends on a word boundary
    if ((dataCount & 0x03) != 0)
    {
       padding = 4 - (dataCount & 0x03);
    }

    // Increment the write pointer
    log->writerCore->write += dataCount + padding;
    
    //write the final size for this message into the buffer.
    wordPtr = (uint32 *)&log->buffer[log->transactionWriteStart & log->bufSizeMask];
    *wordPtr |= (total_size << ULOG_LENGTH_SHIFT) & ULOG_LENGTH_MASK;

    ULogInternal_LockRelease(log);
    return(DAL_SUCCESS);
  } else {
    //it's an STM or other type of write.  Use the generic write function. 
    padding = 0;  //the format is zero, the write will fill in the size. 
    if(ULogCore_IsEnabled(h, &core) == TRUE)
    {
      return(core->write(h, 4, (char*)&padding, dataCount, dataArray, ULOG_INTERFACE_RAW));
    }
  }

  return(DAL_ERROR);
}

#ifdef __cplusplus
}
#endif
