/**
  @file bamconfig_props.c
  @brief
  This file contains implementation of the interfaces that provide
  target specific base addresses & irq vector data to the BAM driver.

*/
/*
===============================================================================

                             Edit History

 $Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/06/12   SS      Added support for tfgcfg binary compatibility
10/15/12   SS      Created

===============================================================================
                   Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#ifdef DAL_CONFIG_IMAGE_APPS
#ifndef BAM_APPS_CONFIG
#define BAM_APPS_CONFIG
#endif
#endif

#include "bamconfig_props.h"
#include "bamtgtcfg.h"
#ifdef BAM_XML_PROPS
#ifdef BAM_ADSP_CONFIG
#include "bamtgtcfgdata_lpass.h"
#elif BAM_MODEM_CONFIG
#include "bamtgtcfgdata_mpss.h"
#elif BAM_TZOS_CONFIG
#include "bamtgtcfgdata_tz.h"
#endif
#endif


#ifdef BAM_APPS_CONFIG
const bamclient_config_info_type  bam_bamdma_baseirq_info[] =
	 {
	    { BAM_BAMDMA_BASE, BAM_BAMDMA_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_slimbus_baseirq_info[] =
	 {
	    { BAM_LITE_SLIMBUS_BASE, BAM_LITE_SLIMBUS_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
	 {
	    { BAM_LITE_BLSP1_BASE, BAM_LITE_BLSP1_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_crypto1_baseirq_info[] =
	 {
	    { BAM_NDP_CRYPTO1_BASE, BAM_NDP_CRYPTO1_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_qdss_baseirq_info[] =
	 {
	    { BAM_NDP_QDSS_BASE, BAM_NDP_QDSS_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_usb2_baseirq_info[] =
	 {
	    { BAM_USB2_BASE, BAM_USB2_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_sdc1_baseirq_info[] =
	 {
	    { BAM_SDC1_BASE, BAM_SDC1_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_sdc2_baseirq_info[] =
	 {
	    { BAM_SDC2_BASE, BAM_SDC2_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_sdc3_baseirq_info[] =
	 {
	    { BAM_SDC3_BASE, BAM_SDC3_IRQVECTOR }
	 };

#elif BAM_MODEM_CONFIG
const bamclient_config_info_type  bam_bamdma_baseirq_info[] =
	 {
	    { BAM_BAMDMA_BASE, BAM_BAMDMA_IRQVECTOR }
	 };
const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
	 {
	    { BAM_LITE_BLSP1_BASE, BAM_LITE_BLSP1_IRQVECTOR }
	 };
const bamclient_config_info_type  bam_slimbus_baseirq_info[] =
	 {
	    { BAM_LITE_SLIMBUS_BASE, BAM_LITE_SLIMBUS_IRQVECTOR }
	 };
const bamclient_config_info_type  bam_crypto_baseirq_info[] =
	 {
	    { BAM_NDP_MSS_CRYPTO_BASE, BAM_NDP_MSS_CRYPTO_IRQVECTOR }
	 };
const bamclient_config_info_type  bam_mssa2_baseirq_info[] =
	 {
	    { BAM_NDP_MSS_A2_BASE, BAM_MSS_A2_IRQVECTOR }
	 };
#elif BAM_TZOS_CONFIG
const bamclient_config_info_type  bam_bamdma_baseirq_info[] =
	 {
	    { BAM_BAMDMA_BASE, BAM_IRQVECTOR_NONE }
	 };

const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
	 {
	    { BAM_LITE_BLSP1_BASE, BAM_IRQVECTOR_NONE }
	 };

const bamclient_config_info_type  bam_crypto1_baseirq_info[] =
	 {
	    { BAM_NDP_CRYPTO1_BASE, BAM_IRQVECTOR_NONE }
	 };

#elif BAM_ADSP_CONFIG
const bamclient_config_info_type  bam_bamdma_baseirq_info[] =
	 {
	    { BAM_BAMDMA_BASE, BAM_BAMDMA_IRQVECTOR}
	 };

const bamclient_config_info_type  bam_slimbus_baseirq_info[] =
	 {
	    { BAM_LITE_SLIMBUS_BASE, BAM_LITE_SLIMBUS_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
	 {
	    { BAM_LITE_BLSP1_BASE, BAM_LITE_BLSP1_IRQVECTOR }
	 };

const bamclient_config_info_type  bam_crypto1_baseirq_info[] =
	 {
	    { BAM_NDP_CRYPTO1_BASE, BAM_NDP_CRYPTO1_IRQVECTOR }
	 };

#endif

