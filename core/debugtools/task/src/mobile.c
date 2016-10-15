/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M O B I L E   M O D U L E

GENERAL DESCRIPTION
  This module contains the Static Version Data for AMSS Mobile software.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1992-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/debugtools/task/src/mobile.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/15   c_dninga Version ADSP.BF.2.2.c24-00003-M8626AAAAAAAZL-1 Used for creating ADSP 8x26 image

===========================================================================*/

//#include <ctype.h>
//#include <locale.h>
//#include "armasm.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
//#include "rex.h"
//#include "task.h"

//#ifdef FEATURE_QDSP6
//#ifdef FEATURE_BLAST
//#include "blast.h"
//#endif
//#include "qube.h"
//#include "qmutex.h"
//#endif

//#include "tmc.h"
#include "mobile.h"
//#include "msg.h"
//#include "rexlog.h"

//#ifdef FEATURE_QUARTZ_3X
//#include "kernel.h"
//#endif

/* ==========================================================================
** Version Information
**   Version Date and Time - when compiled
**   Version Directory     - QCT CRM four letter build id
**   Version Number        - QCT CRM four number build version
**   Release Date          - Same as version
**   Release Time          - Same as version
**   Copyright             - Copyright info placed in ROM
**   Authors               - Tribute to the development teams
**   Version String        - QCT CRM fully qualified version
** ========================================================================== */

NOTICE_TYPE ver_date[] = __DATE__;
NOTICE_TYPE ver_time[] = __TIME__;
NOTICE_TYPE ver_dir[]  = BUILD_TARGET BUILD_VER;

NOTICE_TYPE rel_date[] = "Mar 17 2015";
NOTICE_TYPE rel_time[] = "21:00:00";

NOTICE_TYPE copyright[] =
 "Copyright (c) 1992,1993,1994,1995,1996,1997,1998,1999,2000,"
 "2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,"
 "2011 "
 "by QUALCOMM, Incorporated. "
 "All Rights Reserved.";

NOTICE_TYPE authors[] = "Authors: QCT";

/* Mobile Firmware Marketing Software Revision
**
** THE CRM BUILD TOOLS REQUIREMENT FORCES THIS
** STRING TO BE "SET_BY_CM" AS IT IS ASSIGNED
** DURING THE CRM RELEASE PROCESS.
**
** BUILD_ASIC "-" BUILD_TARGET "-" BUILD_VER
** CANNOT BE USED TO POPULATE THIS SYMBOL
*/

NOTICE_TYPE mob_sw_rev[] = "ADSP.BF.2.2.c24-00003-M8626AAAAAAAZL-1";

/* GUID
*/
#ifndef BUILD_GUID
// #warning Specify a BUILD_GUID in .BUILDS file, Value Supplied By Target Team
#define BUILD_GUID 0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10
#endif

mob_guid_type mob_guid = { BUILD_GUID };

/* ==========================================================================
** Air Standard Identifiers
** ========================================================================== */
// BEGIN: Air Standard Identifiers

/* Station Class Mark
*/
const byte NOTICE_SEG_TYPE station_class_mark =
  SCM_RSVD_80_V |                       /* Reserved                    */
#ifdef FEATURE_ACP
  SCM_DUAL_MODE_V |                     /* Dual Mode phone             */
#else
  SCM_CDMA_ONLY_V |                     /* CDMA Only phone             */
#endif
#ifdef T_SLEEP
  SCM_SLOTTED_V |                       /* Slotted                     */
#else
  SCM_NON_SLOTTED_V |                   /* Non-Slotted                 */
#endif
  SCM_IS54_POWER_CLASS_V |              /* IS-54 Power class           */
  SCM_25_MHZ_BANDWIDTH_V |              /* 25 MHz bandwidth            */
  SCM_CONTINOUS_V |                     /* Continuous transmission     */
  SCM_POWER_CLASS_III_V                 /* Power Class III (portable)  */
  ;

/* Slot Cycle Index (CAI Sections 6.6.2.1.1.3, 6.7.1.3.2.1, and 6.7.4.7)
*/
const byte NOTICE_SEG_TYPE slot_cycle_index = 0x01;

#ifdef FEATURE_CDMA
/* Mobile CAI Revision (CAI Section 6.7.4.7)
*/
const byte NOTICE_SEG_TYPE mob_cai_rev = MOB_P_REV;
#endif

/* Mobile Firmware Revision (CAI Section 6.7.4.7)
*/
const word NOTICE_SEG_TYPE mob_firm_rev = 100;

#ifdef FEATURE_GPSONE
/* Device manufacturer id defaults to QCOM, It should be overwritten by
** individual phone manufacturer.Only first 4 char's are used.
*/
const char mob_gpsone_device_manufacturer[] = "QCOM";

/* Revision Id should also be updated by phone manufacturer.
*/
dword mob_gpsone_device_rev_id = 0;
#endif /* GPSONE */

/* Mobile Model (CAI Section 6.7.4.7)
**    Value provided by cust*.h file or .BUILDS file
*/
#ifndef CUST_MOB_MODEL
// #warning Specify a CUST_MOB_MODEL in .BUILDS file, Value Supplied By Target Team
#define CUST_MOB_MODEL 0
#endif

const byte NOTICE_SEG_TYPE mob_model = (byte)CUST_MOB_MODEL; // 8 bits. do not modify size.

/* Mobile Model (Extension)
**    Value provided by cust*.h file or .BUILDS file
*/
#ifndef CUST_MOB_MODEL_EXT
// #warning Specify a CUST_MOB_MODEL_EXT in .BUILDS file, Value Supplied By Target Team
#define CUST_MOB_MODEL_EXT 0
#endif

const uint32 NOTICE_SEG_TYPE mob_model_ext = (uint32)CUST_MOB_MODEL_EXT; // 32 bits. do not modify size.

// END: Air Standard Identifiers

// BEGIN: timetest_port

unsigned long timetest_port; // software provided timetest_port memory location

// END: timetest_port

