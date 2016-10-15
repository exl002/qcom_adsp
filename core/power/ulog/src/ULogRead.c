/*============================================================================
@file ULogRead.c

Log Read interface for ULog.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.adsp/2.2/power/ulog/src/ULogRead.c#2 $
============================================================================*/
#include "ULogBuildFeatures.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFEATURE_ULOG_READ
#include "DALSys.h"
#include "CoreVerify.h"
#include "MemBarrier.h"
#include "ULog.h"
#include "ULogCoreInternal.h"
#include "CoreString.h"


/**
 * <!-- ReadIsValid -->
 *
 * @brief Is the read pointer somewhere between the current beginning and end of the log? 
 *
 * @param log : Pointer to the ULog to check.
 *
 * @return TRUE (1) if the read pointer is valid, FALSE (0) if not. 
 */
static uint32 ReadIsValid(ULOG_TYPE * log)
{
  CORE_MEM_BARRIER();

  // Return false if the read pointer required update.
  //if (((int32)log->readerCore->read - (int32)log->writerCore->readWriter) >= (int32)0)
  if ((int32)(log->readerCore->read - log->writerCore->readWriter) >=  0)
  {
    // The read index is ahead of the write index, so we're ok.
    return(TRUE);
  }
  log->readerCore->read = log->writerCore->readWriter;
  return(FALSE);
}



/**
 * <!-- ULogCore_Read -->
 *
 * @brief Read data from the log buffer.
 * 
 * Read data from the log buffer.  This data is unformatted. Call
 * ULogCore_MsgFormat to format each message.  Only full messages
 * are placed in the output buffer, thus it is incorrect to use
 * outputCount != outputSize as an indicator that the log buffer
 * is empty.
 *
 * An empty log buffer will yield a read of only status, and an
 * outputCount = 4.
 * 
 * @param h : A ULog handle of the log to read.
 * @param outputSize : Size of the output buffer.
 * @param outputMem : A pointer to the output buffer.
 * @param outputCount : The amount of data placed in the output buffer.
 *
 * @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_Read( ULogHandle h,
                          uint32 outputSize, 
                          char* outputMem,
                          uint32* outputCount )
{
  return(ULogCore_ReadEx(h, outputSize, outputMem, outputCount, 0));
}


/**
 * <!-- ULogCore_ReadEx -->
 *
 * @brief Read data from the log buffer.
 *
 * Read data from the log buffer.  See ULogCore_Read for
 * description.  Allows additional control of limited number of
 * messages read.
 *
 * @param h : A ULog handle of the log to read.
 * @param outputSize : Size of the output buffer.
 * @param outputMem : A pointer to the output buffer.
 * @param outputCount : The amount of data placed in the output buffer.
 * @param outputMessageLimit : The maximum number of messages to read from
 *                     the buffer.
 *
 * @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_ReadEx( ULogHandle h,
                            uint32 outputSize,
                            char *outputMem,
                            uint32* outputCount,
                            uint32 outputMessageLimit)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32 msgLength;
  char * copyPtr;
  char * statusLocation = outputMem;
  uint32 statusValue;
  uint32 outputMessagesRemaining = outputMessageLimit;

  if (log == NULL)
  {
    *outputCount = 0;
    return(ULOG_ERR_INVALIDPARAMETER);
  }
#ifndef DEFEATURE_ULOG_TEE
  else if (HANDLE_IS_A_TEE(log))
  {
    *outputCount = 0;
    return(ULOG_ERR_INVALIDHANDLE);
  }
#endif
  else if ((log->logStatus & ULOG_STATUS_DEFINED) == 0)
  {
    // The log is not yet defined.  Return no data, but don't error either.
    if (outputCount != NULL)
    {
      *outputCount = 0;
    }
    return(DAL_SUCCESS);
  }
  // Check for possible error conditions.
  else if ((outputMem == NULL) ||    //no valid memory location to put the data 
           (outputSize < 4) ||       //insufficient space to copy out the data
           (outputCount == NULL) ||  //no valid memory location to put the our copied bytes count. 
           ((log->bufMemType & ULOG_MEMORY_USAGE_READABLE) == 0))
  {
    // The log is disabled, doesn't exist, or the memory pointers looked invalid so the
    // output dataCount is always zero.
    if (outputCount != NULL)
    {
      *outputCount = 0;
    }
    return(ULOG_ERR_INVALIDPARAMETER);
  }
  else if ((log->sharedHeader != NULL) && 
           (log->sharedHeader->logStatus & ULOG_STATUS_ENABLED))
  {
    // Skip the location for the status word.
    *outputCount = 4;
    outputMem+=4;

    while ((log->readerCore->read != log->writerCore->write) &&
           (*outputCount < outputSize) &&
           ((outputMessageLimit == 0) || (outputMessagesRemaining != 0)))
    {
      copyPtr = outputMem;

      // Read the next message length
      memcpy(&msgLength, &log->buffer[log->readerCore->read & log->bufSizeMask], 4);
      msgLength = (msgLength & ULOG_LENGTH_MASK) >> ULOG_LENGTH_SHIFT;
      msgLength = ULOGINTERNAL_PADCOUNT(msgLength);

      // Make sure the current message is still valid.  If so,
      // then msgBytes should be valid, and we can read the
      // message data.
      // 
      if (ReadIsValid(log))
      {
        if (msgLength == 4)
        {
          // The next message isn't written yet.  We're done.
          break;
        }
        else if ((*outputCount + msgLength) <= outputSize)
        {
          uint32 i;
          // This message will fit into the output buffer.  Copy it.
          // Don't move the outBuf pointer until we are sure the
          // message data is good.
          //
          for(i = 0; i < msgLength; i++)
          {
            *copyPtr++ = log->buffer[(log->readerCore->read+i) & log->bufSizeMask];
          }
        }
        else if (msgLength > outputSize)
        {
          // The entire output buffer doesn't have enough memory to hold the
          // one message.  Throw the message away, or it will clog the pipeline.
          // 
          log->readerCore->read += msgLength;
          if (log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE)
          {
            log->writerCore->usageData |= ULOG_ERR_LARGEMSGOUT;
          }
          break;
        }
        else
        {
          // We have written all we can to the output buffer.
          break;
        }
        if (ReadIsValid(log))
        {
          // The full message is valid.  Update all the pointers
          // and byte counts as appropriate.
          // 
          // Advance the read record in the log to account for what we've just read out.
          log->readerCore->read += msgLength;
          // Update the number of bytes we've coppied so far into the caller's count.
          *outputCount += msgLength;
          // Advance the starting point for then next pass through this loop
          outputMem = copyPtr;   
          // The caller requested a number of messages to be read out.  Subtract 1 for the message we just completed. 
          outputMessagesRemaining--;
        }
      }
    }

    // Rewind the log if we have just read and found no data.
    // The only thing in the log is the status.
    if ((*outputCount == 4) &&
        (log->attributes & ULOG_ATTRIBUTE_READ_AUTOREWIND))
    {
      log->readerCore->read = log->writerCore->readWriter;
      log->readerCore->readFlags |= ULOG_RD_FLAG_REWIND;
    }

    // Write the status here.
    statusValue = log->writerCore->usageData;
    memcpy(statusLocation, &statusValue, 4);
    if (log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE)
    {
      // Note, there is a window of time in which an error could
      // take place that would be cleared by this operation.
      //
      log->writerCore->usageData = 0;
    }

    return(DAL_SUCCESS);
  }
  else
  {
    if (outputCount != NULL)
    {
      *outputCount = 0;
      return(DAL_SUCCESS);
    }
  }

  return(DAL_ERROR);
}


/**
 * <!-- ULogCore_HeaderRead -->
 * 
 * @brief Read the header from the supplied log.
 * 
 * @param h : A handle of the log to access.
 * @param statusString : The output string.
 * @param statusStringSize : The size of the output string
 * 
 * @return DAL_SUCCESS if the format was successful. Errors are
 *         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_HeaderRead(ULogHandle h,  
                               uint32 headerReadOffset,
                               char * headerString, 
                               uint32 headerStringSize,
                               uint32 * headerActualLength)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log == NULL)
  {
    return (DAL_ERROR);
  }

  if ((headerStringSize == 0) ||
      (headerActualLength == NULL))
  {
    return(DAL_ERROR);
  }

#ifndef DEFEATURE_ULOG_TEE
  else if (HANDLE_IS_A_TEE(log))
  {
    return(ULOG_ERR_INVALIDHANDLE);
  }
#endif
  else if (log->header == NULL)
  {
    *headerActualLength = 0;
    headerString[0] = 0;
  }
  else
  {
    if (headerReadOffset < strlen(log->header))
    {
      core_strlcpy(headerString, &log->header[headerReadOffset], headerStringSize);
    }
    else
    {
      headerString[0] = 0;
    }
    *headerActualLength = strlen(headerString);
  }

  return(DAL_SUCCESS);
}

#endif // DEFEATURE_ULOG_READ

#ifdef __cplusplus
}
#endif
