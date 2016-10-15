/*======================================================================

                DALQdiFwdr.S

GENERAL DESCRIPTION
		  QDI DAL FORWARDING FUNCTIONS in USER-PD
			
EXTERNAL FUNCTIONS
        qurt_qdi_get_dal_fwd_vtable

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2012
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=======================================================================*/

/*======================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/dal/framework/src/qdi/DALQdiFwdr.S#1 $
$DateTime: 2013/05/03 17:59:48 $

when       who     what, where, why
--------   ---     -----------------------------------------------------
06/08/12   aa      Updated for QDI2.0.
06/01/12   aa      Tuned for DAL, Bug fix and added comments for release.
04/03/12   jack    Initial Version
=======================================================================*/
#include "qurt_qdi_constants.h"

/*
 A DAL forwarding object in user pd's looks like this:
 DalDeviceHandle
     dwDalHandleId == used for identify if the handle is abstraction
                      for remote object via QDI call.
     pVtbl         == points at qurt_qdi_dal_fwd_vtable, which dal 
                      driver methods jump into for any DDI calls
     pClientCtxt   == points at the word where the QDI handle is stored
                      (can be the immediately following word)
     dwQdiHandle   == QDI handle associated with this forwarding object
*/

	.equ QDI_DAL_FWD, 128          /* 0-128 method id reserved for QDI FW */

	.altmacro

/*
 MACRO vtentry generate assembly output with as many as elements(dal_fwd_x)
 defined in macro definition.(0,99). Its a recursive calls that expands 
 for a vtable containing function pointers until (HI-LO) is greater then 0
 .word dal_fwd_0
 .word dal_fwd_1
 .word dal_fwd_2
 .
 .
 .
 .word dal_fwd_99
*/
	.macro vtentry LO, HI
	.word dal_fwd_\LO
	.if \HI-\LO
	vtentry %(\LO+1),\HI
	.endif
	.endm

/*
 MACRO vtforward generate assembly output with as many as elements(dal_fwd_x)
 definition in macro definition.(0,99). Its a recursive calls that expands 
 until (HI-LO) is greater for function definitions.
 dal_fwd_0 {}
 dal_fwd_1 {}
 dal_fwd_2 {}
 .
 .
 .
 dal_fwd_99 {}
     
*/
	.macro vtforward LO, HI
dal_fwd_\LO:
	{
	r1 = memw(r0+#8)                /* R1 := DalDeviceHandle->pClientCtxt */
	r2 = #(QDI_DAL_FWD+\LO)         /* R2 := method number for forwarding */
   r7:6 = combine(r2,r1)           /* save user args R2, R1 in R7, R6 */
	jump dal_fwd_generic
	}
	.if \HI-\LO
	vtforward %(\LO+1),\HI
	.endif
	.endm

/*
 qurt_qdi_dal_fwd_vtable contains a macro vtentry which a pre-processor
 unwraps with function table of dal forwarders.
 .word dal_fwd_0
 .word dal_fwd_1
 .word dal_fwd_2
 .
 .
 .
 .word dal_fwd_99
*/
	.section .rodata
	.p2align 5
qurt_qdi_dal_fwd_vtable:
	vtentry 0,99

/*
 Pre-Proccessor expands vtforward with macro defintion as defined above
 from 2-99. dal_fwd_1 and dal_fwd_2 explicitly defined.
 dal_fwd_2 {}
 dal_fwd_3 {}
 dal_fwd_4 {}
 .
 .
 .
 dal_fwd_99 {}
*/
	.text
	.p2align 5
	vtforward 2,99


/*
 dal_fwd_generic : Loads the qdi handle in r0 and invoke qurt_qdi_handle_invoke
 to trap in qurtos.
*/   

dal_fwd_generic:
	{
   C1:0 = R7:6                      /* Cl:0 specify the LC0/SA0 pair, expect first source register to be of higher index */
	r1 = memw(r1)                    /* Load QDI handle from address in r1 */                                      
   r0 = #QDI_HANDLE_LOCAL_CLIENT    /* R0 := QDI handle for denote its a local qdi client */
	jump ##qurt_qdi_qhi12            /* Go invoke the QDI handle, will explicitly pass all 12 args */
	}

/* 
 We do not support forwarding the first/second entry (Attach/Detach), 
 using forwarder so return -1 unconditionally.  They are handled via 
 DALFWK. 
*/

dal_fwd_0:
dal_fwd_1:

	{
	r0 = #-1
	jumpr r31
	}

/*
 void *qurt_qdi_get_dal_fwd_vtable(void);
 Returns a pointer to the DAL QDI forwarding vtable which is appropriate for 
 use to initialize the pVtbl field of a DAL QDI forwarding object.
*/

	.globl qurt_qdi_get_dal_fwd_vtable
qurt_qdi_get_dal_fwd_vtable:
	{
	r0 = ##qurt_qdi_dal_fwd_vtable
	jumpr r31
	}
