#include "DALSysInt.h" 

#define DEVCFG_TCSR_SOC_HW_ADDR	0xFD4A8000

extern unsigned int __8x26_PLATFORM_QRD_devcfg_data_addr_base__;
extern unsigned int __8x26_PLATFORM_MTP_MSM_devcfg_data_addr_base__;
extern unsigned int __8x26_PLATFORM_CDP_devcfg_data_addr_base__;

DEVCFG_PLATFORM_SEG_INFO devcfg_target_platform_info_8x26[3] =
{
	{ DALPLATFORMINFO_TYPE_QRD, (DALProps *)&__8x26_PLATFORM_QRD_devcfg_data_addr_base__},
	{ DALPLATFORMINFO_TYPE_MTP_MSM, (DALProps *)&__8x26_PLATFORM_MTP_MSM_devcfg_data_addr_base__},
	{ DALPLATFORMINFO_TYPE_CDP, (DALProps *)&__8x26_PLATFORM_CDP_devcfg_data_addr_base__},
};
extern unsigned int __8x26_devcfg_data_addr_base__;
extern unsigned int __8974_devcfg_data_addr_base__;

DEVCFG_TARGET_INFO devcfg_target_soc_info[ ] =
{
	{ 2, (DALProps *)&__8x26_devcfg_data_addr_base__, devcfg_target_platform_info_8x26, 3},
	{ 4, (DALProps *)&__8x26_devcfg_data_addr_base__, devcfg_target_platform_info_8x26, 3},
	{ 0, (DALProps *)&__8974_devcfg_data_addr_base__, NULL, 0},
	{ 8, (DALProps *)&__8974_devcfg_data_addr_base__, NULL, 0},
	{0, NULL, NULL, 0}
};
