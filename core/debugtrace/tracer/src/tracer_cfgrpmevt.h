#ifndef TRACER_CFGRPMEVT_H
#define TRACER_CFGRPMEVT_H
/** =========================================================================
  @file tracer_cfgrpmevt.h

  @brief QUALCOMM Debug Subsystem (QDSS) Tracer - Configure SW
         events on RPM processor


               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/tracer_cfgrpmevt.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "comdef.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief Wrapper to configure sw events on RPM processor.
 */
void _rpm_swevent_cfg_send(uint32 setting, uint32 type, uint64 bitmask);


#endif /* #ifndef TRACER_CFGRMPEVT_H */

