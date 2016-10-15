#ifndef ERR_DIAGPKT_H
#define ERR_DIAGPKT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Defnitions for diagpkts used by various err modules

GENERAL DESCRIPTION
  Common internal defniitions for diag command handling by error services

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.adsp/2.2/debugtools/err/src/err_diagpkt.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/01/11   mcg     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "diagpkt.h"
#include "erri.h"   // maps ERR_DIAG_PROC_BASE via err_img_*.h


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* registration values for diagpkt callbacks*/
/*   Packet values are 2 bytes.
     We use high nibble of high byte to specify proc. */

enum
{
  ERR_DIAG_PROC_MPSS  = 0x0000,
  ERR_DIAG_PROC_APSS  = 0x1000,
  ERR_DIAG_PROC_WCNSS = 0x2000,
  ERR_DIAG_PROC_LPASS = 0x3000,
  ERR_DIAG_PROC_ADSP  = 0x3000,
  ERR_DIAG_PROC_DSPS  = 0x4000
};

// DO NOT ARBITRARILY RESIZE THIS ENUMERATION : LEGACY OPERATION REQUIRES THAT
// UNTIL DIAG_F3_TRACE IS IT'S OWN DISTINCT SUBSYSTEM, ITS COMMANDS ARE INTERLEAVED
// WITH THOSE OF ERR.

enum
{
  ERR_DIAGPKT_DEPRECATED_0 = ERR_DIAG_PROC_BASE, // IMPORTANT TO RETAIN ERR_DIAG_PROC_BASE, PRIOR ENUM, MAPPED VIA ERR_IMG*.H
  ERR_DIAGPKT_DEPRECATED_1,
  ERR_DIAGPKT_DEPRECATED_2,   /* Delete type trace file          */
  ERR_DIAGPKT_CORE_DUMP,
  ERR_DIAGPKT_DEPRECATED_3,   /* Get number of type saved        */
  ERR_DIAGPKT_DEPRECATED_4,   /* Get next type saved             */
  ERR_DIAGPKT_DEPRECATED_5,   /* Get names of all trace files       */
  ERR_DIAGPKT_DEPRECATED_6,   /* Get number of type saved from file */
  ERR_DIAGPKT_DEPRECATED_7,   /* Get next type saved from file      */
  ERR_DIAGPKT_DEPRECATED_8,   /* Delete specified trace file        */
  ERR_DIAGPKT_DEPRECATED_9
};

#endif
