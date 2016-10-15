/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * mipsmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef MIPSMGR_H_
#define MIPSMGR_H_

#include "requestmgr.h"

/** 
*@struct MIPS data structure for Aggregate
*/
typedef struct
{
    AdsppmMIPSToClockAggaregateType mipsToClockData;
    AdsppmMIPSToBWAggaregateType  mipsToBWAggregateDataDdr;
    AdsppmMIPSToBWAggaregateType  mipsToBWAggregateDataOcmem;
}MIPSAggregateDataType;



typedef struct
{
       DALSYSSyncHandle       MipsmgrCtxLock;
}MIPSCtxType;


 /**
 *@fn MIPS_Init - init MIPS management data structure
 */

Adsppm_Status MIPS_Init(void);

/**
*@fn MIPS_ProcessRequest - process clients MIPS request 
*@param input RMCtxType *gpRMCtx - MIPS request data from clients 
*@return Adsppm_Status
*/

Adsppm_Status MIPS_ProcessRequest(coreUtils_Q_Type *pMipsReqQ);



#endif /* MIPSMGR_H_ */
