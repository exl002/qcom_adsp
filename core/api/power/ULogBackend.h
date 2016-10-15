/*============================================================================
@file ULogBackend.h

Frontend interfaces for ULog Core.  Simplified access to the logging mechanism.

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.adsp/2.2/api/power/ULogBackend.h#2 $ 
============================================================================*/
#ifndef ULOGBACKEND_H
#define ULOGBACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * <!-- ULogBackend_Init -->
 *
 * @brief Initialize all the ULog Backends.
 * 
 * Initialize any and all ULog backends.  This function will set
 * up and enable all backend methods for data retrieval from the
 * ULog system.
*/
void ULogBackend_Init (void);

#ifdef __cplusplus
}
#endif

#endif // ULOGBACKEND_H
