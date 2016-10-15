/* ===========================================================================
FILE:         MPMIntHalDevCfgData.c

DESCRIPTION:  This is the platform hardware abstraction layer implementation for the
              MPM interrupt controller block.
              This platform is for the RPM on 8x26.

              Copyright © 2012-2013 QUALCOMM Technologies Incorporated.
              All Rights Reserved.
              QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.adsp/2.2/power/mpm/config/asic/8x10/MPMIntHalDevCfgData.c#2 $
$DateTime: 2013/05/21 18:53:46 $
$Author: coresvc $
========================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <HALmpmint.h>
#include "HALmpmintInternal.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/*
 * Target-specific interrupt configurations
 */
HAL_mpmint_PlatformIntType devcfgMpmIntHalTable[] = 
{
  /*                             Trigger            GPIO          */
  /*                             -------  ----------------------- */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_QTIMER_ISR                               00*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_PEN_ISR                                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_TSENS_UPPER_LOWER_ISR                    */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  1                       ), /* HAL_MPMINT_GPIO1_ISR                UART RX wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  5                       ), /* HAL_MPMINT_GPIO5_ISR                UART RX wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  9                       ), /* HAL_MPMINT_GPIO9_ISR                UART RX wake up 05*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  14                      ), /* HAL_MPMINT_GPIO14_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  15                      ), /* HAL_MPMINT_GPIO15_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  32                      ), /* HAL_MPMINT_GPIO32_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  33                      ), /* HAL_MPMINT_GPIO33_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  34                      ), /* HAL_MPMINT_GPIO34_ISR                               10*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  35                      ), /* HAL_MPMINT_GPIO35_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  41                      ), /* HAL_MPMINT_GPIO41_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  42                      ), /* HAL_MPMINT_GPIO42_ISR             SDCC card wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  72                      ), /* HAL_MPMINT_GPIO72_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  73                      ), /* HAL_MPMINT_GPIO73_ISR                               15*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  74                      ), /* HAL_MPMINT_GPIO74_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  75                      ), /* HAL_MPMINT_GPIO75_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  76                      ), /* HAL_MPMINT_GPIO76_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  77                      ), /* HAL_MPMINT_GPIO77_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  78                      ), /* HAL_MPMINT_GPIO78_ISR                               20*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  79                      ), /* HAL_MPMINT_GPIO79_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  80                      ), /* HAL_MPMINT_GPIO80_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  81                      ), /* HAL_MPMINT_GPIO81_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  82                      ), /* HAL_MPMINT_GPIO82_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  83                      ), /* HAL_MPMINT_GPIO83_ISR                               25*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  84                      ), /* HAL_MPMINT_GPIO84_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  85                      ), /* HAL_MPMINT_GPIO85_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  87                      ), /* HAL_MPMINT_GPIO87_ISR               UART RX wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  90                      ), /* HAL_MPMINT_GPIO90_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  91                      ), /* HAL_MPMINT_GPIO91_ISR               UART RX wake up 30*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  92                      ), /* HAL_MPMINT_GPIO92_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  93                      ), /* HAL_MPMINT_GPIO93_ISR               UART RX wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  94                      ), /* HAL_MPMINT_GPIO94_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  95                      ), /* HAL_MPMINT_GPIO95_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  96                      ), /* HAL_MPMINT_GPIO96_ISR                               35*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  97                      ), /* HAL_MPMINT_GPIO97_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  98                      ), /* HAL_MPMINT_GPIO98_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  99                      ), /* HAL_MPMINT_GPIO99_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  100                     ), /* HAL_MPMINT_GPIO100_ISR                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  101                     ), /* HAL_MPMINT_GPIO101_ISR                              40*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_HELICON_CDC_ISR    Helicon codec wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DAT1_ISR               SDIO wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DAT3_ISR   SDCC card detect wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC2_DAT1_ISR               SDIO wake up */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC2_DAT3_ISR   SDCC card detect wake up 45*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SRST_N_ISR                               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_OTG_VDDPX3_0_ISR                */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_ID_VDDPX3_0_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DPSE_0_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              50*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              55*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DMSE_0                          */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_CARD_ISR                             60*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_BATT_ISR                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SPMI_WAKE_ISR                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* UNUSED                                              */

  /*64 - special entry to mark end of the table */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( INVALID, HAL_MPMINT_INVALID_GPIO )
};           
