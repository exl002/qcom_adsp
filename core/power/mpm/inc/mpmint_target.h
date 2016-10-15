#ifndef MPMINT_TARGET_H
#define MPMINT_TARGET_H

/*===========================================================================
  FILE: mpmint_target.h

  DESCRIPTION: This files defines the enumeration which contains exhaustive
               list of interrupts and gpios that are mapped to MPM on various
               B family targets.

  DEPENDENCIES: Requires updating this list when new targets are added which
                has interrupts/gpios mapped to mpm that are not present in
                below list.

                Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.  
                All Rights Reserved.
                QUALCOMM Proprietary

  $Header: //components/rel/core.adsp/2.2/power/mpm/inc/mpmint_target.h#2 $ 
===========================================================================*/


/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

/** 
 * A list of all of the supported MPM interrupts for B family targets.
 * 
 * Add any new enum at the end so that it does not affect existing mapping
 * tables based on the order before the new enum.
 *
 * Notes:
 * The hardware does not necessarily support all of these interrupts.
 *
 * Sometimes there are some unused interrupt lines marked as MPM_BIT_n
 * which we are not listing here. If a specific IRQ/GPIO wants to be
 * mapped at that line, provide appropriate enum for it and mapping to
 * hal layer.
 */                                         
typedef enum                                                                    
{                                                                               
  MPM_QTIMER_ISR,                      /* 0 */
  MPM_PEN_DOWN_ISR,
  MPM_TSENS_UPPER_LOWER_ISR,
  MPM_GPIO102_ISR,
  MPM_GPIO1_ISR,
  MPM_GPIO5_ISR,                       /* 5 */
  MPM_GPIO9_ISR,
  MPM_GPIO18_ISR,
  MPM_GPIO20_ISR,
  MPM_GPIO24_ISR,
  MPM_GPIO27_ISR,                      /* 10 */
  MPM_GPIO28_ISR,
  MPM_GPIO34_ISR,
  MPM_GPIO35_ISR,
  MPM_GPIO37_ISR,
  MPM_GPIO42_ISR,                      /* 15 */
  MPM_GPIO44_ISR,
  MPM_GPIO46_ISR,   
  MPM_GPIO50_ISR,   
  MPM_GPIO54_ISR,
  MPM_GPIO59_ISR,                      /* 20 */
  MPM_GPIO61_ISR,  
  MPM_GPIO62_ISR,    
  MPM_GPIO64_ISR,
  MPM_GPIO65_ISR,
  MPM_GPIO66_ISR,                      /* 25 */ 
  MPM_GPIO67_ISR,     
  MPM_GPIO68_ISR,   
  MPM_GPIO71_ISR,
  MPM_GPIO72_ISR,         
  MPM_GPIO73_ISR,                      /* 30 */ 
  MPM_GPIO74_ISR,
  MPM_GPIO75_ISR,
  MPM_GPIO77_ISR,
  MPM_GPIO79_ISR,
  MPM_GPIO80_ISR,                      /* 35 */
  MPM_GPIO82_ISR,
  MPM_GPIO86_ISR,
  MPM_GPIO92_ISR,
  MPM_GPIO93_ISR,   
  MPM_GPIO95_ISR,                      /* 40 */
  MPM_HSIC_STROBE_CORE_ISR,
  MPM_SDC1_DATA1_ISR,
  MPM_SDC1_DATA3_ISR,
  MPM_SDC2_DATA1_ISR,
  MPM_SDC2_DATA3_ISR,                  /* 45 */
  MPM_SRST_N_ISR,
  MPM_USB2_PHY_OTG_0_ISR,
  MPM_USB2_ID_HV_0_ISR,
  MPM_USB2_PHY_DPSE_0_ISR,
  MPM_USB2_PHY_OTG_1_ISR,              /* 50 */ 
  MPM_USB2_ID_HV_1_ISR,
  MPM_USB2_PHY_DPSE_1_ISR,
  MPM_HDMI_PHY_RCV_ISR,
  MPM_USB2_PHY_DMSE_0_ISR,
  MPM_USB2_PHY_DMSE_1_ISR,             /* 55 */
  MPM_UIM_CTL_CARD_ISR,
  MPM_UIM_CTL_BATT_ISR,
  MPM_MPM_SPMI_WAKE_ISR,
  MPM_GPIO0_ISR,
  MPM_GPIO2_ISR,                       /* 60 */
  MPM_GPIO3_ISR,
  MPM_GPIO4_ISR,
  MPM_GPIO6_ISR,
  MPM_GPIO7_ISR,
  MPM_GPIO8_ISR,                       /* 65 */
  MPM_GPIO10_ISR,
  MPM_GPIO11_ISR,
  MPM_GPIO12_ISR,
  MPM_GPIO13_ISR,
  MPM_GPIO14_ISR,                      /* 70 */
  MPM_GPIO15_ISR,
  MPM_GPIO16_ISR,
  MPM_GPIO17_ISR,
  MPM_GPIO19_ISR,
  MPM_GPIO21_ISR,                      /* 75 */
  MPM_GPIO25_ISR,
  MPM_GPIO51_ISR,
  MPM_GPIO63_ISR,
  MPM_GPIO69_ISR,
  MPM_GPIO29_ISR,                      /* 80 */
  MPM_GPIO31_ISR,
  MPM_GPIO33_ISR,
  MPM_GPIO38_ISR,
  MPM_GPIO39_ISR,
  MPM_GPIO41_ISR,                      /* 85 */
  MPM_GPIO48_ISR,
  MPM_GPIO49_ISR,  
  MPM_GPIO52_ISR,
  MPM_GPIO106_ISR,
  MPM_GPIO107_ISR,                     /* 90 */
  MPM_GPIO108_ISR,
  MPM_GPIO109_ISR,
  MPM_GPIO110_ISR,
  MPM_GPIO111_ISR,
  MPM_GPIO113_ISR,                     /* 95 */
  MPM_GPIO32_ISR,
  MPM_GPIO76_ISR,
  MPM_GPIO78_ISR,
  MPM_GPIO81_ISR,
  MPM_GPIO83_ISR,                     /* 100 */
  MPM_GPIO84_ISR,  
  MPM_GPIO85_ISR,
  MPM_GPIO87_ISR,
  MPM_GPIO90_ISR,
  MPM_GPIO91_ISR,                     /* 105 */
  MPM_GPIO94_ISR,
  MPM_GPIO96_ISR,
  MPM_GPIO97_ISR,
  MPM_GPIO98_ISR,
  MPM_GPIO99_ISR,                     /* 110 */
  MPM_GPIO100_ISR,
  MPM_GPIO101_ISR,
  MPM_HELICON_CDC_ISR,
  MPM_USB3_PWR_EVENT_0_ISR,
  MPM_GPIO40_ISR,                     /* 115 */
  MPM_GPIO55_ISR,
  MPM_GPIO56_ISR,
  MPM_GPIO58_ISR,
  MPM_GPIO60_ISR,
  MPM_GPIO103_ISR,                    /* 120 */
  MPM_GPIO104_ISR,
  MPM_GPIO105_ISR,
  MPM_GPIO121_ISR,
  MPM_GPIO122_ISR,
  MPM_GPIO123_ISR,                    /* 125 */
  MPM_GPIO131_ISR,
  MPM_GPIO139_ISR,
  MPM_GPIO141_ISR,
  MPM_SDC_CMD_ISR,
  MPM_USB2_PHY_DPSE_2_ISR,            /* 130 */
  MPM_USB2_PHY_DMSE_2_ISR,
  MPM_USB2_PHY_DPSE_3_ISR,
  MPM_USB2_PHY_DMSE_3_ISR,
  MPM_USB3_PWR_EVENT_1_ISR,
  MPM_SPSS_WAKE_ISR,                  /* 135 */

  /* Add any new enums for ISR here */

  MPMINT_NUM_INTS,                         
} mpmint_isr_type;                         

/*===========================================================================   
                      PUBLIC FUNCTION DECLARATIONS                              
===========================================================================*/   
                                                                                
#endif  /* MPMINT_TARGET_H */
 
