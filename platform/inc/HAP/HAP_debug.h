#ifndef HAP_DEBUG_H
#define HAP_DEBUG_H
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAP_LEVEL_LOW 0
#define HAP_LEVEL_MEDIUM 1
#define HAP_LEVEL_HIGH 2
#define HAP_LEVEL_ERROR 3
#define HAP_LEVEL_FATAL 4

void HAP_debug(const char *msg, int level, const char *filename, int line);

#define HAP_DEBUG_TRACEME 0

long HAP_debug_ptrace(int req, unsigned int pid, void* addr, void* data);

#ifdef __cplusplus
}
#endif

#endif // HAP_DEBUG_H

