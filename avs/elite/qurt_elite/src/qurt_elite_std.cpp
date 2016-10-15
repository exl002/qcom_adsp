/*========================================================================*/
/** @file qurt_elite_std.cpp
This file contains standard C functions which are missing from HEXAGON tools

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/
/*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/elite/qurt_elite/src/qurt_elite_std.cpp#10 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/07/11   sivapava   Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite_std.h"
#include <stringl/stringl.h>

#ifdef __qdsp6__
#include "q6protos.h"
#endif //__qdsp6__

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/**
  Copies the source string to destination

  @param[in] dest_ptr - destination string ptr
  @param[in] src_ptr - source string ptr
  @param[in] dest_len - destination buffer length

  @return
  src_len - source string size

  @dependencies
  None.
*/
uint32_t qurt_elite_strl_cpy(char_t *dest_ptr, const char_t *src_ptr, uint32_t dest_len)
{
   return strlcpy (dest_ptr, src_ptr, dest_len);
}

