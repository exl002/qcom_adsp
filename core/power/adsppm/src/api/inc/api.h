/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * api.h
 *
 *  Created on: Jun 22, 2012
 *      Author: yrusakov
 */

#ifndef API_H_
#define API_H_

#include "asyncmgr.h"

void MMPM_Init_Ext(void);
uint32 API_doClientCallback(AsyncMgrPipeDataType* receiveData);

#endif /* API_H_ */
