/*==============================================================================

  D A L I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
 This file Contains the Interrupt configuration data for all the interrupts for 
 this platform.

REFERENCES

        Copyright � 2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.adsp/2.2/systemdrivers/InterruptController/config/8092/InterruptConfigData.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALReg.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DALInterruptControllerConfig.h"



/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*
 * Interrupt_Configs
 * BSP data for this target's interrupts.
 * The data layout is the following :
 * {  l2VIC interrupt vector number , Interrupt name }
 *
 */
static InterruptConfigType InterruptConfigs[] =
{
 {0 ,    "qdsp6_etm_irq"},
 {1 ,    "qdsp6_isdb_irq"},
 {2 ,    "qtmr_phy_irq[0]"},
 {3 ,    "qtmr_phy_irq[1]"},
 {5 ,    "rpm_ipc[9]"},
 {7 ,    "rpm_ipc[8]"},
 {8 ,    "APCS_adspIPCInterrupt[1]"},
 {11 ,    "slimbus_core_ee0_irq"},
 {12 ,    "bam_irq[0]"},
 {13 ,    "rpm_ipc[10]"},
 {14 ,    "rpm_ipc[11]"},
 {18 ,    "APCS_adspIPCInterrupt[0]"},
 {19 ,    "audio_out2_irq"},
 {20 ,    "bus_irq"},
 {23 ,    "dm_irq"},
 {24 ,    "dir_conn_irq_lpa_dsp[0]"},
 {25 ,    "dir_conn_irq_lpa_dsp[1]"},
 {26 ,    "dir_conn_irq_lpa_dsp[2]"},
 {27 ,    "dir_conn_irq_lpa_dsp[3]"},
 {28 ,    "dir_conn_irq_lpa_dsp[4]"},
 {29 ,    "dir_conn_irq_lpa_dsp[5]"},
 {30 ,    "q6ss_idle_timer_irq"},
 {31 ,    "q6ss_cti_irq"},
 {32 ,    "q6ss_wdog_irq"},
 {33 ,    "APCS_adspIPCInterrupt[2]"},
 {37 ,    "adsp_ext_vfr_irq"},
 {38 ,    "summary_irq_lpa_dsp"},
 {41 ,    "q6ss_avs_irq"},
 {42 ,    "q6ss_avs_cpu_up_irq"},
 {43 ,    "q6ss_avs_cpu_dn_irq"},
 {44 ,    "usb30_power_event_irq"},
 {45 ,    "usb30_ee1_irq"},
 {46 ,    "APCS_adspIPCInterrupt[3]"},
 {47 ,    "usb30_ee2_irq"},
 {48 ,    "ee3_lpass_spmi_periph_irq"},
 {49 ,    "channel3_lpass_trans_done_irq"},
 {50 ,    "avtimer_int_0"},
 {51 ,    "lpass_irq_out_wcss[5]"},
 {53 ,    "resampler_irq[1]"},
 {54 ,    "usb1_hs_irq"},
 {55 ,    "blsp1_bam_irq[1]"},
 {56 ,    "blsp2_bam_irq[1]"},
 {59 ,    "bam_dma_irq[1]"},
 {60 ,    "ao_pen_irq"},
 {61 ,    "spdm_realtime_irq[2]"},
 {62 ,    "summary_irq_sensors"},
 {63 ,    "q6ss_avs_sw_done_irq"},
 {64 ,    "blsp1_qup_1_irq"},
 {65 ,    "blsp1_qup_2_irq"},
 {66 ,    "blsp1_qup_3_irq"},
 {67 ,    "blsp1_qup_4_irq"},
 {68 ,    "blsp1_qup_5_irq"},
 {69 ,    "blsp1_qup_6_irq"},
 {70 ,    "dir_conn_irq_sensors[0]"},
 {71 ,    "dir_conn_irq_sensors[1]"},
 {72 ,    "dir_conn_irq_sensors[2]"},
 {73 ,    "dir_conn_irq_sensors[3]"},
 {74 ,    "dir_conn_irq_sensors[4]"},
 {75 ,    "dir_conn_irq_sensors[5]"},
 {76 ,    "dir_conn_irq_sensors[6]"},
 {77 ,    "dir_conn_irq_sensors[7]"},
 {78 ,    "vpu_maple_qdsp_3_irq_out"},
 {80 ,    "blsp2_qup_1_irq"},
 {81 ,    "blsp2_qup_2_irq"},
 {82 ,    "blsp2_qup_3_irq"},
 {83 ,    "blsp2_qup_4_irq"},
 {84 ,    "blsp2_qup_5_irq"},
 {85 ,    "blsp2_qup_6_irq"},
 {86 ,    "usb1_hs_irq"},
 {88 ,    "qtmr_phy_irq[2]"},
 {89 ,    "crypto_core_irq[0]"},
 {90 ,    "o_timeout_slave_lpass_summary_intr"},
 {95 ,    "vpu_maple_qdsp_2_irq_out"},
 {96 ,    "crypto_core_irq[0]"},
 {97 ,    "crypto_bam_irq[0]"},
 {98 ,    "crypto_core_irq[0]"},
 {99 ,    "crypto_bam_irq[0]"},
 {101 ,    "usb3_hs_irq"},
 {101 ,    "usb3_hs_irq"},
 {102 ,    "dir_conn_irq_sensors[8]"},
 {103 ,    "dir_conn_irq_sensors[9]"},
 {104 ,    "phss_uart_lpass_int[0]"},
 {105 ,    "phss_uart_lpass_int[1]"},
 {106 ,    "spss_cec_intr"},
 {107 ,    "spss_mpm_intr_krait"},
 {108 ,    "spss_ir_irq"},
 {109 ,    "spss_ir_wake_irq_krait"},
 {110 ,    "vpu_maple_qdsp_0_irq_out"},
 {111 ,    "vpu_maple_qdsp_1_irq_out"},
 {120 ,    "crypto_bam_irq[0]"},
 {121 ,    "lpass_hdmirx_interrupt"},
 {122 ,    "spdif_tx_irq"},
 {123 ,    "sif_aud_dec_out_irq"},
 {INVALID_INTERRUPT , ""}
};

/*
 * InterruptPlatformDataType 
 * This is used by the Interrupt controller platform to query platform specific data. 
 */
InterruptPlatformDataType  pInterruptControllerConfigData[] =
{
  {
    InterruptConfigs,           /* pIRQConfigs */
    124
  }
};




  
