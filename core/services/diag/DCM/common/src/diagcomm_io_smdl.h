#ifndef DIAGCOMM_IO_SMDL_H 
#define DIAGCOMM_IO_SMDL_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.adsp/2.2/services/diag/DCM/common/src/diagcomm_io_smdl.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/06/11   hvm     Added new api declaration for T-put improv
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/
#ifndef FEATURE_WINCE
  #include "customer.h"
#endif

#ifdef DIAG_SMDL_SUPPORT 

#include "diagcomm_io.h"         /* For diagcomm_io_conn_type */

void diagcomm_smdl_open_init( diagcomm_io_conn_type * conn );
void diagcomm_io_smdl_send ( diagcomm_io_conn_type * conn );

#endif /* DIAG_SMDL_SUPPORT */

#endif //DIAGCOMM_IO_SMDL_H


