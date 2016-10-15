#ifndef QURT_PIPE_H
#define QURT_PIPE_H
/**
  @file qurt_pipe.h 

  @brief   Prototypes of the pipe interface API  
	 This is a pipe or message queue
	 It will block if too full (send) or empty (receive)
	 Unless a nonblocking option is used, all datagrams are 64 bits.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2010 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc. 

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/qurt/include/public/qurt_pipe.h#8 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
12/16/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/
#include <qurt_mutex.h>
#include <qurt_sem.h>
/** @addtogroup pipe_types
@{ */
/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/
#define QURT_PIPE_MAGIC  0xF1FEF1FE


/*=============================================================================
                        TYPEDEFS
=============================================================================*/
/** Represents pipe data values. */
typedef unsigned long long int qurt_pipe_data_t;

/** Represents pipes.*/
typedef struct {
    /** @cond */
	qurt_mutex_t pipe_lock;
	qurt_sem_t senders;
	qurt_sem_t receiver;
	unsigned int size;
	unsigned int sendidx;
	unsigned int recvidx;
	unsigned int magic;
	qurt_pipe_data_t *data;
    /** @endcond */
} qurt_pipe_t;

/**  Represents pipe attributes. */
typedef struct {
  /** @cond */
  qurt_pipe_data_t *buffer;
  unsigned int elements;
  /** @endcond */
} qurt_pipe_attr_t;

/** @} */ /* end_addtogroup pipe_types */
/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**@ingroup func_qurt_pipe_attr_init
  Initializes the structure that is used to set the pipe attributes when a pipe is created.

  After an attribute structure is initialized, the individual attributes in the structure are
  explicitly set using the pipe attribute operations.

  @datatypes
  #qurt_pipe_attr_t
 
  @param[in,out] attr Pipe attribute structure.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_pipe_attr_init(qurt_pipe_attr_t *attr)
{
  attr->buffer = 0;
  attr->elements = 0;
}

/**@ingroup func_qurt_pipe_attr_set_buffer
  @xreflabel{sec:qurt_pipe_attr_set_buffer}
  Sets the pipe buffer address attribute.\n
  Specifies the base address of the memory area to be used for a pipe's data buffer.

  The base address and size (Section @xref{sec:qurt_pipe_attr_set_elements}) specify the 
  memory area used as a pipe data buffer. The user is responsible for allocating the 
  memory area used for the buffer.

  @datatypes
  #qurt_pipe_attr_t \n
  #qurt_pipe_data_t

  @param[in,out] attr Pipe attribute structure.
  @param[in] buffer   Buffer base address.

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_pipe_attr_set_buffer(qurt_pipe_attr_t *attr, qurt_pipe_data_t *buffer)
{
  attr->buffer = buffer;
}

/**@ingroup func_qurt_pipe_attr_set_elements
  @xreflabel{sec:qurt_pipe_attr_set_elements}
  Specifies the length of the memory area to be used for a pipe's data buffer. 
  
  The length is expressed in terms of the number of 64-bit data elements that 
  can be stored in the buffer. 
  
  The base address (Section @xref{sec:qurt_pipe_attr_set_buffer}) and size specify 
  the memory area used as a pipe data buffer. The user is responsible for 
  allocating the memory area used for the buffer.

  @datatypes
  #qurt_pipe_attr_t

  @param[in,out] attr Pipe attribute structure.
  @param[in] elements Pipe length (64 bit elements). 

  @return
  None.

  @dependencies
  None.
*/
static inline void qurt_pipe_attr_set_elements(qurt_pipe_attr_t *attr, unsigned int elements)
{
  attr->elements = elements;
}

/**@ingroup func_qurt_pipe_create
  Creates a pipe.\n
  Allocates a pipe object and its associated data buffer, and initializes the pipe object.

  @note1hang The buffer address and size stored in the attribute structure specify how the
             pipe data buffer is allocated.
  
  @datatypes
  #qurt_pipe_t \n
  #qurt_pipe_attr_t
  
  @param[out] pipe  Pointer to the created pipe object.
  @param[in]  attr  Pointer to the attribute structure used to create the pipe.

  @return 
  QURT_EOK -- Pipe created. \n
  QURT_EFAILED -- Pipe not created.

  @dependencies
  None.
 */
int qurt_pipe_create(qurt_pipe_t **pipe, qurt_pipe_attr_t *attr);

/**@ingroup func_qurt_pipe_init
  Initializes a pipe object using an existing data buffer.

  @note1hang The buffer address and size stored in the attribute structure must 
             specify a data buffer that has already been allocated by the user.

  @datatypes
  #qurt_pipe_t \n
  #qurt_pipe_attr_t
  
  @param[out] pipe Pointer to the pipe object to initialize.
  @param[in] attr  Pointer to the pipe attribute structure used to initialize the pipe.

  @return 
  QURT_EOK -- Success. \n
  QURT_EFAILED -- Failure.

  @dependencies
  None.
 */
int qurt_pipe_init(qurt_pipe_t *pipe, qurt_pipe_attr_t *attr);

/**@ingroup func_qurt_pipe_destroy
  @xreflabel{sec:qurt_pipe_destroy}
  Destroys the specified pipe.

  @note1hang Pipes must be destroyed when they are no longer in use. Failure 
             to do this causes resource leaks in the QuRT kernel.
             Pipes must not be destroyed while they are still in use. If this 
             happens the behavior of QuRT is undefined.

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pointer to the pipe object to destroy.

  @return
  None.

  @dependencies
  None.
 */
void qurt_pipe_destroy(qurt_pipe_t *pipe); 

/**@ingroup func_qurt_pipe_delete
  Deletes the pipe.\n
  Destroys the specified pipe (Section @xref{sec:qurt_pipe_destroy}) and deallocates the pipe object and its
  associated data buffer.

  @note1hang Pipes should be deleted only if they were created using qurt_pipe_create
             (and not qurt_pipe_init). Otherwise the behavior of QuRT is undefined. \n
  @note1cont Pipes must be deleted when they are no longer in use. Failure to do this 
             causes resource leaks in the QuRT kernel.\n
  @note1cont Pipes must not be deleted while they are still in use. If this happens the
             behavior of QuRT is undefined. 

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pipe object to destroy.

  @return 
  None.

  @dependencies
  None.
 */
void qurt_pipe_delete(qurt_pipe_t *pipe);

/**@ingroup func_qurt_pipe_send
  Writes a data item to the specified pipe. \n
  If a thread writes to a full pipe, it is suspended on the pipe. When another thread reads
  from the pipe, the suspended thread is awakened and can then write data to the pipe.

  Pipe data items are defined as 64-bit values. Pipe writes are limited to transferring a single
  64-bit data item per operation.

  @note1hang Data items larger than 64 bits can be transferred by reading and writing
             pointers to the data, or by transferring the data in consecutive 64-bit chunks.

  @datatypes
  #qurt_pipe_t \n
  #qurt_pipe_data_t
  
  @param[in] pipe Pointer to the pipe object to write to.
  @param[in] data Data item to be written.

  @return
  None.

  @dependencies
  None.
*/
void qurt_pipe_send(qurt_pipe_t *pipe, qurt_pipe_data_t data);

/**@ingroup func_qurt_pipe_receive
  Reads a data item from the specified pipe.

  If a thread reads from an empty pipe, it is suspended on the pipe. When another thread
  writes to the pipe, the suspended thread is awakened and can then read data from the pipe.
  Pipe data items are defined as 64-bit values. Pipe reads are limited to transferring a single
  64-bit data item per operation.

  @note1hang Data items larger than 64 bits can be transferred by reading and writing
             pointers to the data, or by transferring the data in consecutive 64-bit chunks.

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe Pointer to the pipe object to read from.

  @return
  Integer containing 64-bit data item from pipe.

  @dependencies
  None.
*/
qurt_pipe_data_t qurt_pipe_receive(qurt_pipe_t *pipe);

/**@ingroup func_qurt_pipe_try_send
  Writes a data item to the specified pipe (without suspending the thread if the pipe is full).\n

  If a thread writes to a full pipe, the operation returns immediately with success set to -1.
  Otherwise, success is always set to 0 to indicate a successful write operation.

  Pipe data items are defined as 64-bit values. Pipe writes are limited to transferring a single
  64-bit data item per operation.

  @note1hang Data items larger than 64 bits can be transferred by reading and writing
             pointers to the data, or by transferring the data in consecutive 64-bit chunks.

  @datatypes
  #qurt_pipe_t \n
  #qurt_pipe_data_t
  
  @param[in] pipe Pointer to the pipe object to write to.
  @param[in] data Data item to be written.

  @return
  0 -- Success. \n
  -1 -- Failure (pipe full).

  @dependencies
  None.
*/ 
int qurt_pipe_try_send(qurt_pipe_t *pipe, qurt_pipe_data_t data);

/**@ingroup func_qurt_pipe_try_receive
  Reads a data item from the specified pipe (without suspending the thread if the pipe is
  empty).\n
  If a thread reads from an empty pipe, the operation returns immediately with success set
  to -1. Otherwise, success is always set to 0 to indicate a successful read operation.\n

  Pipe data items are defined as 64-bit values. Pipe reads are limited to transferring a single
  64-bit data item per operation.

  @note1hang Data items larger than 64 bits can be transferred by reading and writing
             pointers to the data, or by transferring the data in consecutive 64-bit chunks.

  @datatypes
  #qurt_pipe_t
  
  @param[in] pipe     Pointer to the pipe object to read from.
  @param[out] success Operation status result.

  @return
  Integer containing a 64-bit data item from pipe.

  @dependencies
  None.
*/
qurt_pipe_data_t qurt_pipe_try_receive(qurt_pipe_t *pipe, int *success);


#endif  /* QURT_PIPE_H */

