/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * asyncmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef ASYNCMGR_H_
#define ASYNCMGR_H_

#include "adsppm.h"
#include "qurt_pipe.h"

#define  ASYNCMGR_REQ_MASK 0
#define  ASYNCMGR_TAG_MASK 32
#define  ASYNCMGR_CLIENTID_MASK  64
#define  ASYNCMGR_RESULT_MASK    96


typedef struct
{
		uint16 reqQMask;
		uint16 reqTag;
		uint16 clientId;
		uint16 result;
}AsyncMgrPipeDataType;


typedef union
{
	qurt_pipe_data_t pipeData;
	AsyncMgrPipeDataType asyncPipeData;
}AsyncMgrMessageType;


/** Async management call back thread*/

void AM_callBackThread(void);

/** Async management work loop thread*/

void AM_WorkLoopThread(void);

Adsppm_Status AM_AsyncInit(void);

#endif /* ASYNCMGR_H_ */
