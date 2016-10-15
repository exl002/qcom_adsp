/*========================================================================

*//** @file lpm_cfg_8926.h
This file contains LPM configuration for MSM8926

Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: 

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/03/13   aprasad Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "lpm_prop.h"

/* LPM configuration for MSM8926 */
/* LPA heap sizes are a multiple of 4KB so that it can be placed in OCMEM */
const lpm_prop_struct_t lpm_prop = {0xFE090000, 64*1024, 52*1024, 12*1024, 0, 0, 128*1024, 44*1024};