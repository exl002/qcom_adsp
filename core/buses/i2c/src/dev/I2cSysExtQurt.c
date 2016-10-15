/*=============================================================================
  FILE:   I2cSysExt.c

  OVERVIEW: This file contains the implementation for the QUP OS Services 
 
            Copyright (c) 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary

  ===========================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/buses/i2c/src/dev/I2cSysExtQurt.c#1 $
  $DateTime: 2013/04/03 17:22:53 $$Author: coresvc $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  09/26/11 LK     Created

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cSys.h"
#include "qurt.h"

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/**
   @brief Translates from virtual address to physical
 
   @param[in] pVirtMem  Pointer to virtual memory.
   @param[in] uSize     Size of memory buffer.
 
   @return 0 if error, physical address otherwise.
 */
uint32 I2CSYS_VirtToPhys
(
   uint8       *pVirtMem,
   uint32       uSize
)
{
  return (uint32) qurt_lookup_physaddr ((uint32)pVirtMem); 
}



