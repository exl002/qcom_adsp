/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/


/*
 * adsspm_ConfigData.c - contains all 8974-specific data definitions for Config Manager
 *
 *  Created on: Sep 07, 2012
 *      Author: yrusakov/rli
 */
#include "adsppm.h"
#include "asic.h"
#include "asic_internal.h"
#include "adsppm_utils.h"
#include "icbid.h"
#include "DDIIPCInt.h"


/**
 * Array of 8974 core descriptors. Arranged according to core ID enum
 */
const AsicCoreDescType cores_8974[Adsppm_Core_Id_Max] = {
        {   //Adsppm_Core_Id_None
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None  //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_ADSP
                Adsppm_Core_Id_ADSP, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_ADSP, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_Adsp, // Power Domain
                AdsppmBusPort_Adsp_Master, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_LPM
                Adsppm_Core_Id_LPM, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Mem, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_Lpm_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_DML
                Adsppm_Core_Id_DML, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_Dml_Master, // Master port
                AdsppmBusPort_Dml_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_AIF
                Adsppm_Core_Id_AIF, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_Aif_Master, // Master port
                AdsppmBusPort_Aif_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_SlimBus
                Adsppm_Core_Id_SlimBus, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_Slimbus_Master, // Master port
                AdsppmBusPort_Slimbus_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_Midi
                Adsppm_Core_Id_Midi, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_Midi_Master, // Master port
                AdsppmBusPort_Midi_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_AVsync
                Adsppm_Core_Id_AVsync, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_AvSync_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_HWRSMP
                Adsppm_Core_Id_HWRSMP, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                AdsppmBusPort_HwRsmp_Master, // Master port
                AdsppmBusPort_HwRsmp_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_SRam
                Adsppm_Core_Id_SRam, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Mem, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_SRam, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_Sram_Slave  // Slave port
        },
        {   //Adsppm_Core_Id_DCodec
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                AdsppmBusPort_None, // Master port
                AdsppmBusPort_None  // Slave port
        },
        {   //Adsppm_Core_Id_Spdif
                Adsppm_Core_Id_None, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                 AdsppmBusPort_None, // Master port
                 AdsppmBusPort_None  // Slave port
        }
};

const AsicMemDescType memories_8974[Adsppm_Mem_Max] = {
        {   //Adsppm_Mem_None
                Adsppm_Mem_None, // Mem ID
                AsicPowerDomain_AON // Power Domain
        },
        {   //Adsppm_Mem_Ocmem
                Adsppm_Mem_Ocmem, // Core ID
                AsicPowerDomain_Ocmem // Power Domain
        },
        {   //Adsppm_Mem_Lpass_LPM
                Adsppm_Mem_Lpass_LPM, // Core ID
                AsicPowerDomain_Lpm // Power Domain
        },
        {   //Adsppm_Mem_Sram
                Adsppm_Mem_Sram, // Core ID
                AsicPowerDomain_SRam // Power Domain
        }
};


/**
 * Array of 8974 core clock descriptors. Arranged by core clock ID
 */
const AsicClkDescriptorType clocks_8974[AdsppmClk_EnumMax] = {
        {
                AdsppmClk_None, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Core, //Clk ID
                AsicClk_TypeNpa, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "/clk/cpu",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Ahb_Root, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_ixfabric_clk",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "q6ss_ahbm_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_HwRsp_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_resampler_lfabif_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Dml_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_dml_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Aif_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_lpaif_dma_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Aif_Csr_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_lpaif_csr_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_DCG, // Cntl Type
                AsicClk_MemCntlAlwaysRetain, // MemCtrl Type
                "audio_core_slimbus_lfabif_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Midi_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_midi_lfabif_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_avsync_csr_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Lpm_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_lpm_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_Lpass_LPM //Memory ID
        },
        {
                AdsppmClk_Sram_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_wrapper_smem_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Lcc_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_wrapper_lcc_csr_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbE_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_wrapper_efabric_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbI_Hclk, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlAlwaysON_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_ixfabric_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbX_Hclk, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlAlwaysON_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_ixfabric_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Csr_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_csr_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Security_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_security_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Wrapper_Security_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlOff, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_wrapper_security_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Wrapper_Br_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON_DCG, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_wrapper_br_clk",//Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Dcodec_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Spdif_Hmclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Spdif_Hsclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
       {
                AdsppmClk_Smmu_Adsp_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Smmu_Lpass_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Smmu_Sysnoc_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_HwRsp_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_resampler_core_clk",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Midi_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_midi_core_clk",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Xo, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_avsync_xo_clk",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Bt, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_avsync_bt_xo_clk",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Fm, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_avsync_fm_xo_clk",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type disabled until ClkRegime implements it
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "audio_core_slimbus_core_clk",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
	{
                AdsppmClk_Avtimer_core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
	{
                AdsppmClk_Atime_core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                "",//Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        }
};

/**
 * Array of 8974 Bus port descriptors arranged by Bus port ID
 */
const AsicBusPortDescriptorType busPorts_8974[AdsppmBusPort_EnumMax] = {
        {
                AdsppmBusPort_None, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Adsp_Master, //ID
                AsicBusPort_AhbE_M | AsicBusPort_Ext_M,  //Connection
                AdsppmClk_Adsp_Hclk, //Bus clock
                AdsppmClk_Adsp_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_LPASS_PROC}, // icbarb ID
                AdsppmBusPort_Adsp_Master // Access port
        },
        {
                AdsppmBusPort_Dml_Master, //ID
                AsicBusPort_AhbI_M | AsicBusPort_AhbX_M, //Connection
                AdsppmClk_Dml_Hclk, //Bus clock
                AdsppmClk_Dml_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Aif_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Aif_Hclk, //Bus clock
                AdsppmClk_Aif_Csr_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Slimbus_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Slimbus_Hclk, //Bus clock
                AdsppmClk_Slimbus_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Midi_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Midi_Hclk, //Bus clock
                AdsppmClk_Midi_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_HwRsmp_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_HwRsp_Hclk, //Bus clock
                AdsppmClk_HwRsp_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //ID
                AsicBusPort_Ext_M, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                AdsppmClk_AhbX_Hclk, //Csr clock
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Spdif_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Dml_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Dml_Hclk, //Bus clock
                AdsppmClk_Dml_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Aif_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Aif_Csr_Hclk, //Bus clock
                AdsppmClk_Aif_Csr_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Slimbus_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Slimbus_Hclk, //Bus clock
                AdsppmClk_Slimbus_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Midi_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Midi_Hclk, //Bus clock
                AdsppmClk_Midi_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_HwRsmp_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_HwRsp_Hclk, //Bus clock
                AdsppmClk_HwRsp_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_AvSync_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_AvSync_Hclk, //Bus clock
                AdsppmClk_AvSync_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Lpm_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Lpm_Hclk, //Bus clock
                AdsppmClk_Lpm_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Sram_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Sram_Hclk, //Bus clock
                AdsppmClk_Sram_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                AdsppmClk_AhbX_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Ddr_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                AdsppmClk_AhbX_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_EBI1}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Ocmem_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                AdsppmClk_AhbX_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_OCMEM}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_PerifNoc_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                AdsppmClk_AhbX_Hclk, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_BLSP_1}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        {
                AdsppmBusPort_Spdif_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                AdsppmClk_None, //Csr clock
                {.icbarbSlave = ICBID_SLAVE_LPASS}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
};

/**
 * List of supported external bus routes
 * Note: Must sync with NUMBER_OF_EXTERNAL_BUS_ROUTES in adsppm.xml
 */
const AdsppmBusRouteType extBusRoutes_8974[] = {
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ocmem_Slave //slave
        },
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_PerifNoc_Slave //slave
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        }
};

const AdsppmBusRouteType mipsBwRoutes_8974[] =
{
      {
              AdsppmBusPort_Adsp_Master,
              AdsppmBusPort_Ocmem_Slave
      },
      {
              AdsppmBusPort_Adsp_Master,
              AdsppmBusPort_Ddr_Slave
      }
};


/**
* Array of all clock, which need to be enabled for TZ handshake (LPASS Core power damain)
*/
const AdsppmClkIdType securityClocks_LPASSCore_8974[] = 
{
    AdsppmClk_Security_Hclk, 
    AdsppmClk_Aif_Csr_Hclk, 
    AdsppmClk_Lpm_Hclk, 
    AdsppmClk_Slimbus_Hclk, 
    AdsppmClk_HwRsp_Hclk
};
/**
 * Array of power domain descriptors
 */
const AsicPowerDomainDescriptorType pwrDomains_8974[AsicPowerDomain_EnumMax] = {
        {
                AsicPowerDomain_AON, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                0,
                NULL
        },
        {
                AsicPowerDomain_Adsp, //Id
                "/core/cpu/vdd", //Name
                AsicRsc_Power_ADSP, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                0,
                NULL        
        },
        {
                AsicPowerDomain_LpassCore, //Id
                "LPASS_AUDIO_CORE", //Name
                AsicRsc_Power_Core, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_GP_3, //TZ notify
                46, //TZ Done
                sizeof(securityClocks_LPASSCore_8974)/sizeof(AdsppmClkIdType),
                &securityClocks_LPASSCore_8974[0]
        },
        {
                AsicPowerDomain_Lpm, //Id
                "", //Name
                AsicRsc_Power_Mem, //Type
                AdsppmClk_Lpm_Hclk, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                0,
                NULL
        },
        {
                AsicPowerDomain_SRam, //Id
                "", //Name
                AsicRsc_Power_Mem, //Type
                AdsppmClk_Sram_Hclk, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                0,
                NULL
        },
        {
                AsicPowerDomain_Ocmem, //Id
                "", //Name
                AsicRsc_Power_Ocmem, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                0,
                NULL
        }
};

const AsicFeatureDescType features_8974 [AsicFeatureId_enum_max] =
{
        //AsicFeatureId_Adsp_Clock_Scaling
        {
                AsicFeatureState_Enabled,
                80, //min MIPS
                700, //max MIPS
        },
        //AsicFeatureId_Adsp_LowTemp_Voltage_Restriction
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Adsp_PC
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_Scaling
        {
                AsicFeatureState_Enabled,
                19000000*4, //min bytes per second corresponds to 19Mhz clock
                561736000, //max bytes per second
        },
        //AsicFeatureId_Ahb_Sw_CG
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_DCG
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC_TZ_Handshake
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Bus_Scaling
        {
                AsicFeatureState_Enabled,
                32768, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_CoreClk_Scaling
        {
                AsicFeatureState_Enabled,
                0, //min Hz
                140434000, //max Hz
        },
        //AsicFeatureId_Min_Adsp_BW_Vote
        {
                AsicFeatureState_Enabled,
                10000000, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_InitialState
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_TimelineOptimisationMips
        {
                AsicFeatureState_Enabled,
                240, //min MIPS for periodic clients
                600  //max MIPS value
        },
        //AsicFeatureId_TimelineOptimisationBw
        {
                AsicFeatureState_Enabled,
                76000000*8, //min ext BW in bytes per second (targeting above 76Mhz SNOC clk)
                0x7FFFFFFF, //max ext BW in bytes per second
        },
        //AsicFeatureId_TimelineOptimisationAhb
        {
                AsicFeatureState_Enabled,
                60000000, //min AHB clock in Hz
                141000000, //max AHB clock in Hz
        },
        //AsicFeatureId_LpassClkSleepOptimization
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        }
};
