#ifndef ASSERT_H
#define ASSERT_H
/*==========================================================================

            A S S E R T   S E R V I C E S   H E A D E R   F I L E

DESCRIPTION
  Definitions for ASSERT macro.
  
Copyright (c) 1998,1999 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/assert.h_v   1.0   Aug 21 2000 11:51:02   lpetrucc  $
$Header: //components/rel/core.adsp/2.2/api/services/amssassert.h#1 $ $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/98    ms     Removed spurious tabs.
02/19/98   jct     Created

===========================================================================*/


/*===========================================================================
 
                            INCLUDE FILES

===========================================================================*/


/*===========================================================================
 
                            CONSTANT DEFINITIONS

===========================================================================*/

/* -----------------------------------------------------------------------
** As a compiler definition, define ASSERT to be one of:
**    /DASSERT=ASSERT_FATAL
**    /DASSERT=ASSERT_WARN
** ASSERT_FATAL:
**   Invokes ERR_FATAL when the ASSERTion fails
** ASSERT_WARN:
**   Invokes ERR when the ASSERTion fails
** Others or no definition:
**   No processing
** ----------------------------------------------------------------------- */
#define ASSERT_FATAL 1
#define ASSERT_WARN  2


/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

MACRO ASSERT

DESCRIPTION
  Given a boolean expression, ASSERT will verify the condition is TRUE.  If
  the condition is FALSE, then ASSERT will perform the following action:
  
    For ASSERT_FATAL: invokes ERR_FATAL processing
    For ASSERT_WARN:  invokes ERR processing
    For all others:   is empty

  Example:
    ASSERT will trigger since i is equal to 5 and ASSERTion says i must
    NOT be 5

        int i=5;
        ASSERT( i != 5 );
   
    ASSERT will NOT trigger since i is not equal to 5 and ASSERTion says
    i must NOT be 5

        int i=4;
        ASSERT( i != 5 );
  
DEPENDENCIES
  For ASSERT_FATAL and ASSERT_WARN: err.h, err.c 
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#if   (ASSERT==ASSERT_FATAL)

  #undef ASSERT 

  #ifndef ERR_H
    #include "err.h"
  #endif
  
  #define ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR_FATAL( "Assertion " #xx_exp " failed", 0, 0, 0 ); \
     }
     
#elif (ASSERT==ASSERT_WARN)

  #undef ASSERT

  #ifndef ERR_H
    #include "err.h"
  #endif
  
  #define ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "Assertion " #xx_exp " failed",0,0,0 ); \
     }
          
#else

  #define ASSERT( xx_exp ) ((void)0)
  
#endif

#endif /* ASSERT.H */

