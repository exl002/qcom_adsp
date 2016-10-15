/**
@file stackCheck.c
 
@brief This file contains the user level boot code for LPASS

*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     ------------------------------------------------------- 
 
03/01/12   corinc  StackCheck.c created
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include "StackCheck.h"
#include "qurt.h"
//#include "msm.h"
#include "qurt_elite.h"
//#include "common.h"
#include "err.h"
#include "PrngML.h"


//void *__stack_chk_guard;

void init_stack_chk_guard(void)
{
	uint32 seedRandom = 0xfaceb00c;  //aka: facebook

    // get random number
    if (PRNGML_ERROR_NONE != PrngML_getdata_lite((uint8*)&seedRandom, (uint16) sizeof(seedRandom)))
{
        ERR_FATAL( "ADSP QDSP6: __stack_chk_guard cannot get random number", 0, 0, 0 );
    }
    /* Get random number */
    __stack_chk_guard = (void*)seedRandom;
//    qurt_printf("seedRandom = %d \n", seedRandom);
    qurt_printf("***===*** __stack_chk_guard initialized ***===***\n");

    return;
}

void __wrap___stack_chk_fail(void)
{
    qurt_printf("__stack_chk_fail: stack check failed!\n");
	ERR_FATAL("LPASS QDSP6: Stack Check failed",0,0,0);
//	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "The stack protection check failed!");
}

