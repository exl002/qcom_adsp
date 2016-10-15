#ifndef _ELITE_MAIN_H_
#define _ELITE_MAIN_H_

/**
@file EliteMain.h

@brief This file provides an API wrapper for high level elite frame work functions
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The descriptions that appera in the PDF are maintained in the
      Elite_Core_mainpage.dox file. Contact Tech Pubs for support.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013 Qualcomm Technologies, Inc.  All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/20/13   sw      (Tech Pubs) Edited Doxygen markup/comments for 2.2.
02/04/10   mspk      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** @ingroup elitemain_func_init
  Initializes the Elite framework.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_init(void);

/** @ingroup elitemain_func_deinit
  De-initializes the Elite framework.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_deinit(void);


/** @ingroup elitemain_func_start
  Starts the Elite framework, including static services.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_start(void);

/** @ingroup elitemain_func_stop
  Stops the Elite framework, including static services.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_stop(void);


/** @ingroup elitemain_func_tests
  Performs the framework unit test cases and also tests the framework test
  case.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_unit_tsts(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_ELITE_MAIN_H_