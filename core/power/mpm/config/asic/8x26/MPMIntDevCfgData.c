/*===========================================================================
   FILE:         MPMIntDevCfgData.c
 
   DESCRIPTION:  MPM HAL <--> Tramp IRQ mapping info.

                 Copyright © 2013 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.adsp/2.2/power/mpm/config/asic/8x26/MPMIntDevCfgData.c#1 $
$DateTime: 2013/04/03 17:22:53 $
$Author: coresvc $
========================================================================*/


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "mpmint_target.h"
#include "mpminti.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */


/**
 * Table mapping the external enumeration's representation of interrupts to the
 * HAL's internal enumeration values. Any entries which are unused or reserved
 * in hal table, are skipped here.
 */
mpmint_irq_data_type devcfgMpmIntMappingTable[] =
{
  /* MPM_QTIMER_ISR             */  {0,                           MPMINT_NULL_IRQ},
  /* MPM_PEN_DOWN_ISR           */  {1,                           MPMINT_NULL_IRQ},
  /* MPM_TSENS_UPPER_LOWER_ISR  */  {2,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO102_ISR            */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO1_ISR              */  {3,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO5_ISR              */  {5,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO9_ISR              */  {6,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO18_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ}, 
  /* MPM_GPIO20_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO24_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO27_ISR             */  {10,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO28_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO34_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO35_ISR             */  {14,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO37_ISR             */  {15,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO42_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO44_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO46_ISR             */  {19,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO50_ISR             */  {22,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO54_ISR             */  {25,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO59_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO61_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO62_ISR             */  {26,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO64_ISR             */  {28,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO65_ISR             */  {29,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO66_ISR             */  {30,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO67_ISR             */  {31,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO68_ISR             */  {32,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO71_ISR             */  {34,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO72_ISR             */  {35,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO73_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO74_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO75_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO77_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO79_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO80_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO82_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO86_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO92_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO93_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO95_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_HSIC_STROBE_CORE_ISR   */  {41,                          MPMINT_NULL_IRQ},
  /* MPM_SDC1_DATA1_ISR         */  {42,                          MPMINT_NULL_IRQ},
  /* MPM_SDC1_DATA3_ISR         */  {43,                          MPMINT_NULL_IRQ},
  /* MPM_SDC2_DATA1_ISR         */  {44,                          MPMINT_NULL_IRQ},
  /* MPM_SDC2_DATA3_ISR         */  {45,                          MPMINT_NULL_IRQ},
  /* MPM_SRST_N_ISR             */  {46,                          MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_OTG_0_ISR     */  {47,                          MPMINT_NULL_IRQ},
  /* MPM_USB2_ID_HV_0_ISR       */  {48,                          MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DPSE_0_ISR    */  {49,                          MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_OTG_1_ISR     */  {50,                          MPMINT_NULL_IRQ},
  /* MPM_USB2_ID_HV_1_ISR       */  {51,                          MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DPSE_1_ISR    */  {52,                          MPMINT_NULL_IRQ},
  /* MPM_HDMI_PHY_RCV_ISR       */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DMSE_0_ISR    */  {58,                          MPMINT_NULL_IRQ},
  /* MPM_USB2_PHY_DMSE_1_ISR    */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_UIM_CTL_CARD_ISR       */  {60,                          MPMINT_NULL_IRQ},
  /* MPM_UIM_CTL_BATT_ISR       */  {61,                          MPMINT_NULL_IRQ},
  /* MPM_MPM_SPMI_WAKE_ISR      */  {62,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO0_ISR              */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO2_ISR              */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO3_ISR              */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO4_ISR              */  {4,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO6_ISR              */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO7_ISR              */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO8_ISR              */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO10_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO11_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO12_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO13_ISR             */  {7,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO14_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO15_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO16_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO17_ISR             */  {8,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO19_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO21_ISR             */  {9,                           MPMINT_NULL_IRQ},
  /* MPM_GPIO25_ISR             */  {MPMINT_IMT_UNMAPPED_HAL_IRQ, MPMINT_NULL_IRQ},
  /* MPM_GPIO51_ISR             */  {23,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO63_ISR             */  {27,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO69_ISR             */  {33,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO29_ISR             */  {11,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO31_ISR             */  {12,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO33_ISR             */  {13,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO38_ISR             */  {16,                          MPMINT_NULL_IRQ},  
  /* MPM_GPIO39_ISR             */  {17,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO41_ISR             */  {18,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO48_ISR             */  {20,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO49_ISR             */  {21,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO52_ISR             */  {24,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO106_ISR            */  {36,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO107_ISR            */  {37,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO108_ISR            */  {38,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO109_ISR            */  {39,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO110_ISR            */  {40,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO111_ISR            */  {54,                          MPMINT_NULL_IRQ},
  /* MPM_GPIO113_ISR            */  {55,                          MPMINT_NULL_IRQ},
  
  /* End of table Entry         */  {MPMINT_IMT_EOS_HAL_IRQ, MPMINT_IMT_EOS_LOCAL_IRQ},
};


