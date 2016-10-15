/*============================================================================
@file CoreWorkQueue.h

Core Work Queue API.

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.adsp/2.2/power/utils/inc/CoreWorkQueue.h#2 $
============================================================================*/
#ifndef COREWORKQUEUE_H
#define COREWORKQUEUE_H

#include "CoreWorkContainer.h"
#include "CorePool.h"

/** @brief The elements that make up a CoreWorkQueue linked list */
typedef struct CoreWorkQueueElemType
{
  struct CoreWorkQueueElemType *pNext;
  unsigned int                  uPersist;
  char                          pElemData[];
} CoreWorkQueueElemType;


/**
 * @brief Holds the head and tail of a work queue as well as the pool info. 
 * Typically stored in the pCntrData of a 
 * CoreWorkContainerHandle / CoreWorkContainerType
 */
typedef struct CoreWorkQueueType
{
  /* the actual queue */
  CoreWorkQueueElemType *pHead;
  CoreWorkQueueElemType *pTail;

  /* work queue element memory pool */
  CorePoolType           ElemPool;
  unsigned int           NumElems;
} CoreWorkQueueType;


/*****************************************************************************
 * Public functions.
 ****************************************************************************/

/**
 * Core_WorkQueueCreate
 *
 * @brief Allocate and initalize a work queue.
 * WorkQueues are passed around by their CoreWorkContainerHandles. This function
 * allocates the work container/handle, the work queue, and the Core Pool.
 *
 * @param phContainer: Handle for the work queue returned here. 
 * @param pfnProcess:  Function to be called to process queue elements. 
 * @param uElemNum:    Initial size of the queue.
 * @param uElemNumSubsequent: Size (in number of elems) of subsequent pool
 *                     block allocations.
 * @param uDataSize:   Size of data stored in queue. Elements in the linked list
 *                     end up being this size + sizeof(CoreWorkQueueElemType)
 *  
 * @return Success or Failure 
 */
DALResult Core_WorkQueueCreate(CoreWorkContainerHandle *phContainer,
                               CoreContainerProcessFcn  pfnProcess, 
                               uint32                   uElemNum,
                               uint32                   uElemNumSubsequent,
                               uint32                   uDataSize);


/**
 * Core_WorkQueueAlloc
 *
 * @brief Get a new WorkQueueElemType from the memory pool. 
 *
 * @param ppData: Pointer to the work queue  pElemData memory returned.
 * @param hContainer: The work queue to get the element from. 
 *  
 * @return Success or Failure 
 */
DALResult Core_WorkQueueAlloc(CoreWorkContainerHandle  hContainer,
                              void                     **ppData);


/**
 * Core_WorkQueuePut
 * 
 * @brief Insert at the tail of the queue. 
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      A pointer to a CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueuePut(CoreWorkContainerHandle  hContainer,
                            void                     *pData);


/**
 * Core_WorkQueuePutAtHead
 *
 * @brief Insert at the start of the queue.
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      A pointer to a CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueuePutAtHead(CoreWorkContainerHandle  hContainer,
                            void                     *pData);


/**
 * Core_WorkQueueIsEmpty
 *
 * Return values from IsEmpty and Length routines may be incorrect if
 * the workqueue is subject to concurrent access from multiple threads 
 *
 * @param hContainer: The work queue to check.
 *
 * @return True or False
 */
DALBOOL Core_WorkQueueIsEmpty(CoreWorkContainerHandle hContainer);


/**
 * Core_WorkQueueLength
 *
 * Return values from IsEmpty and Length routines may be incorrect if
 * the workqueue is subject to concurrent access from multiple threads 
 *
 * @param hContainer: The work queue to check.
 *
 * @return CoreWorkQueueType->NumElems
 */
unsigned int Core_WorkQueueLength(CoreWorkContainerHandle hContainer);


/**
 * Core_WorkQueuePutNoDups
 *
 * @brief Insert at the tail of the queue with the NoDups/uPersist status.
 *
 * Insert into workqueue (at tail) if not already in it. Must be invoked
 * with workqueue locked 
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      A pointer to a CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueuePutNoDups( CoreWorkContainerHandle  hContainer,
                                   void                    *pData );


/**
 * Core_WorkQueuePutAtHeadNoDups
 *
 * @brief Insert at the start of the queue with the NoDups/uPersist status.
 *
 * Insert into workqueue (at head) if not already in it. Must be invoked
 * with workqueue locked 
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      A pointer to a CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueuePutAtHeadNoDups( CoreWorkContainerHandle  hContainer,
                                         void                    *pData );


/**
 * Core_WorkQueueLock
 *
 * @brief Call DALSYS_SyncEnter on the WorkQueue container's lock
 *
 * Must be invoked before Core_WorkQueuePut(AtHead)NoDups 
 *
 * @param hContainer: The work queue to lock.
 */
void Core_WorkQueueLock( CoreWorkContainerHandle hContainer );


/**
 * Core_WorkQueueUnlock
 *
 * @brief Call DALSYS_SyncLeave on the WorkQueue container's lock.
 *
 * Must be invoked after Core_WorkQueuePut(AtHead)NoDups
 *
 * @param hContainer: The work queue to unlock.
 */
void Core_WorkQueueUnlock( CoreWorkContainerHandle hContainer );

/*****************************************************************************
 * Container internal functions -- you probably shouldn't call these.
 ****************************************************************************/

/**
 * Core_WorkQueueFree
 *
 * @brief Return non persistant CoreWorkQueueElemTypes to the memory pool.
 *
 * pData is a pointer to the data area of CoreWorkQueueElemType.  Backs 
 * the ptr up to the start of the CoreWorkQueueElemType and then adds it back
 * to the Core Pool
 *
 * @param hContainer: The work queue the element came from. 
 * @param pData: A pointer to the CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */ 
DALResult Core_WorkQueueFree(CoreWorkContainerHandle  hContainer,
                             void                     *pData);


/**
 * Core_WorkQueueGet
 *
 * @brief Return an element from the head of the queue. 
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      Returns pointer to a CoreWorkQueueElemType pElemData area
 *                    or NULL.
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueueGet(CoreWorkContainerHandle  hContainer,
                            void                     **ppData);

#endif /* COREWORKQUEUE_H */
