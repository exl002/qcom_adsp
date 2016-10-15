/* ===========================================================================
FILE:         MPMIntHalDevCfgData.c

DESCRIPTION:  This is the platform hardware abstraction layer implementation for the
              MPM interrupt controller block.
              This platform is for the RPM on 8x26.

                 Copyright © 2013 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.adsp/2.2/power/mpm/config/asic/8x26/MPMIntHalDevCfgData.c#2 $
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
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_QTIMER_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_PEN_ISR                     */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_TSENS_UPPER_LOWER_ISR       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  1                       ), /* HAL_MPMINT_GPIO1_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  4                       ), /* HAL_MPMINT_GPIO4_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  5                       ), /* HAL_MPMINT_GPIO5_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  9                       ), /* HAL_MPMINT_GPIO9_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  13                      ), /* HAL_MPMINT_GPIO13_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  17                      ), /* HAL_MPMINT_GPIO17_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  21                      ), /* HAL_MPMINT_GPIO21_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  27                      ), /* HAL_MPMINT_GPIO27_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  29                      ), /* HAL_MPMINT_GPIO29_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  31                      ), /* HAL_MPMINT_GPIO31_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  33                      ), /* HAL_MPMINT_GPIO33_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  35                      ), /* HAL_MPMINT_GPIO35_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  37                      ), /* HAL_MPMINT_GPIO37_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  38                      ), /* HAL_MPMINT_GPIO38_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  39                      ), /* HAL_MPMINT_GPIO39_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  41                      ), /* HAL_MPMINT_GPIO41_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  46                      ), /* HAL_MPMINT_GPIO46_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  48                      ), /* HAL_MPMINT_GPIO48_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  49                      ), /* HAL_MPMINT_GPIO49_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  50                      ), /* HAL_MPMINT_GPIO50_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  51                      ), /* HAL_MPMINT_GPIO51_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  52                      ), /* HAL_MPMINT_GPIO52_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  54                      ), /* HAL_MPMINT_GPIO54_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  62                      ), /* HAL_MPMINT_GPIO62_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  63                      ), /* HAL_MPMINT_GPIO63_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  64                      ), /* HAL_MPMINT_GPIO64_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  65                      ), /* HAL_MPMINT_GPIO65_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  66                      ), /* HAL_MPMINT_GPIO66_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  67                      ), /* HAL_MPMINT_GPIO67_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  68                      ), /* HAL_MPMINT_GPIO68_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  69                      ), /* HAL_MPMINT_GPIO69_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  71                      ), /* HAL_MPMINT_GPIO71_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  72                      ), /* HAL_MPMINT_GPIO72_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  106                     ), /* HAL_MPMINT_GPIO106_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  107                     ), /* HAL_MPMINT_GPIO107_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  108                     ), /* HAL_MPMINT_GPIO108_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  109                     ), /* HAL_MPMINT_GPIO109_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  110                     ), /* HAL_MPMINT_GPIO110_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  115                     ), /* HAL_MPMINT_HSIC_STROBE_ISR             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DAT1_ISR               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DAT3_ISR               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC2_DAT1_ISR               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC2_DAT3_ISR               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SRST_N_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_OTG_VDDPX3_0_IS    */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( LOW,     HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_ID_VDDPX3_0_ISR    */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DPSE_0_ISR         */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_OTG_VDDPX3_1_ISR   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( LOW,     HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_ID_VDDPX3_1_ISR    */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DPSE_1_ISR         */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_53_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  111                     ), /* HAL_MPMINT_GPIO111_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  113                     ), /* HAL_MPMINT_GPIO113_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_56_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_57_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DMSE_0_ISR         */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_59_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_CARD_ISR                */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_BATT_ISR                */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SPMI_WAKE_ISR               */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_63_ISR                      */

  /*64 - special entry to mark end of the table */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( INVALID, HAL_MPMINT_INVALID_GPIO )
};           
