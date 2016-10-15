#ifndef SNS_BUFFER_H
#define SNS_BUFFER_H

/*============================================================================
  @file sns_buffer.h

  Circular buffer utility header file

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/common/sam/algo/common/inc/sns_buffer.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-01-06  ad   initial version

============================================================================*/

#include <stdint.h>
#include <stdbool.h>

/*
 * since BUF_SIZE is a power of 2,
 * (x & (BUF_SIZE-1)) == (x % BUF_SIZE)
 */
#define SNS_BUF_INDEX(index,bufSize) ((index) & (bufSize-1))

#define SNS_BUF_ELEM(buf,i,j) (*((buf)->data + i*(buf)->numDataAxes + j))

/* Structure for circular buffer of data stored as floating point */
typedef struct
{
   float *data;
   uint32_t numDataAxes;
   uint32_t numDataValues;
   uint32_t windowSize;
   uint32_t end;
   uint32_t count;
} sns_buffer_type;

/*=======================================================================*/
/*!
    @brief Returns size round up to the nearest power of 2

    @param[i] windowSize: size of window (number of data elements)

    @return size of window round up to nearest power of 2
*/
/*=======================================================================*/
uint32_t sns_buffer_size(
   uint32_t windowSize);

/*=========================================================================
  FUNCTION:  sns_buffer_mem_req
  =======================================================================*/
/*!
    @brief Returns memory needed for buffer with specified params

    @param[i] windowSize: size of window (number of data elements)
    @param[i] numDataAxes: number of axes for each data element

    @return Size of memory required
*/
/*=======================================================================*/
uint32_t sns_buffer_mem_req(
   uint32_t windowSize, 
   uint32_t numDataAxes);

/*=========================================================================
  FUNCTION:  sns_buffer_reset
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[io] buffer: pointer to buffer
    @param[i] windowSize: size of window (number of data elements)
    @param[i] numDataAxes: number of axes for each data element
    @param[i] mem: memory provided for data buffer

    @return None
*/
/*=======================================================================*/
void sns_buffer_reset(
   sns_buffer_type* buffer, 
   uint32_t windowSize, 
   uint32_t numDataAxes, 
   void* mem);

/*=========================================================================
  FUNCTION:  sns_buffer_full
  =======================================================================*/
/*!
    @brief Indicate if buffer is full

    @param[i] buffer: pointer to buffer

    @return true - if buffer full, false - otherwise
*/
/*=======================================================================*/
bool sns_buffer_full(
   const sns_buffer_type* buffer);

/*=========================================================================
  FUNCTION:  sns_buffer_insert
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[io] buffer: pointer to buffer
    @param[i] data: pointer to data to be inserted in buffer

    @return None
*/
/*=======================================================================*/
void sns_buffer_insert(
   sns_buffer_type* buffer, 
   const float* data);

/*=========================================================================
  FUNCTION:  sns_buffer_compute_variance
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[i] buffer: pointer to buffer
    @param[o] sumData: sum of data elements for variance computation
    @param[o] sumSqData: sum of square data elements for variance computation

    @return variance
*/
/*=======================================================================*/
double sns_buffer_compute_variance(
   const sns_buffer_type* buffer, 
   double* sumData, 
   double* sumSqData);

#endif /* SNS_BUFFER_H */
