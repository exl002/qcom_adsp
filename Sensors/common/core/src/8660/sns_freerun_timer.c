/*============================================================================

  @file sns_freerun_timer.c

  @brief
    This API implements for a free-run-timer

            Copyright (c) 2010 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary

============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/common/core/src/8660/sns_freerun_timer.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
2010-10-05   br      Initial Release

============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sensor1.h"
#include "sns_freerun_timer.h"

/*===========================================================================

  FUNCTION:   sns_free_timer_enable

===========================================================================*/
/*!
  @brief This function enables the platform specific free-run timer
   
  @detail
  
  @param[i] timer_id to be enabled. The value can be either 0 or 1 for 9660 as an example.

  @return
   None
*/
/*=========================================================================*/
void sns_freerun_timer_enable ( uint32_t timer_id )
{
  HWIO_OUT(PPSS_TIMER0_CLK_CTL, HWIO_FMSK(PPSS_TIMER0_CLK_CTL, CLK_BRANCH_ENA) | 
           HWIO_FMSK(PPSS_TIMER0_CLK_CTL, PXO_SRC_BRANCH_ENA));
  HWIO_OUTI(TCXO_TMRn_CONTROL, timer_id, HWIO_FMSK(TCXO_TMRn_CONTROL, EN));
}

/*===========================================================================

  FUNCTION:   sns_free_timer_disable

===========================================================================*/
/*!
  @brief This function disables the platform specific free-run timer
   
  @detail
  
  @param[i] timer_id to be enabled. The value can be either 0 or 1 for 9660 as an example

  @return
   None

*/
/*=========================================================================*/
void sns_freerun_timer_disable ( uint32_t timer_id )
{
  HWIO_OUTI(TCXO_TMRn_CLEAR_CNT, timer_id, 1);
  HWIO_OUTF(PPSS_TIMER0_CLK_CTL, CLK_BRANCH_ENA, 0);
}

/*===========================================================================

  FUNCTION:   sns_freerun_timer_get_usec

===========================================================================*/
/*!
  @brief This function returns a gab between two ticks with usec value
   
  @detail
  
  @param[i] start_val the start tick value
  @param[i] now_val the now tick value

  @return
   None

*/
/*=========================================================================*/
uint32_t sns_freerun_timer_get_usec ( uint32_t timer_id, uint32_t start_val, uint32_t now_val )
{
  uint64_t diff;
  diff = (now_val - start_val + PXO_24_576_MHZ / 2000000);  /* add 0.5 usec tick */
  return ( (diff * 1000000) / PXO_24_576_MHZ );
}

/*===========================================================================

  FUNCTION:   sns_freerun_timer_get_hz

===========================================================================*/
/*!
  @brief This function returns the timer hz.
   
  @detail
  
  @param[i] timer_id the timer id on which this function returns the rate

  @return
   None

*/
/*=========================================================================*/
uint32_t sns_freerun_timer_get_hz ( uint32_t timer_id )
{
    return ( PXO_24_576_MHZ );
}
