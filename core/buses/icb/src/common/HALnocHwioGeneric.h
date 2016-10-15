#ifndef __HALNOCHWIOGENERIC_H__
#define __HALNOCHWIOGENERIC_H__
/*
===========================================================================
*/
/**
  @file HALnocHwioGeneric.h
  @brief Generic hwio for the NOC QOS block.

*/
/*
  ===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.adsp/2.2/buses/icb/src/common/HALnocHwioGeneric.h#1 $
  $DateTime: 2013/04/03 17:22:53 $
  $Author: coresvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: NOC_QOS (generic)
 *--------------------------------------------------------------------------*/
#define NOC_QOS_REG_BASE(b)                                ((b)      + 0x00003000)

/*------------------------------ QOS_ID_COREIDn -----------------------------*/
#define NOC_QOS_ID_COREIDn_ADDR(b,n)                       (NOC_QOS_REG_BASE(b) + 0x80 * (n))
#define NOC_QOS_ID_COREIDn_RMSK                            0xffffffff
#define NOC_QOS_ID_COREIDn_MAXn                                    32
#define NOC_QOS_ID_COREIDn_INI(b,n)        \
        in_dword_masked(NOC_QOS_ID_COREIDn_ADDR(b,n), NOC_QOS_ID_COREIDn_RMSK)
#define NOC_QOS_ID_COREIDn_INMI(b,n,mask)    \
        in_dword_masked(NOC_QOS_ID_COREIDn_ADDR(b,n), (mask))
#define NOC_QOS_ID_COREIDn_OUTI(b,n,val)    \
        out_dword(NOC_QOS_ID_COREIDn_ADDR(b,n),val)
#define NOC_QOS_ID_COREIDn_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(NOC_QOS_ID_COREIDn_ADDR(b,n),mask,val,NOC_QOS_ID_COREIDn_INI(b,n))
#define NOC_QOS_ID_COREIDn_CORECHECKSUM_BMSK               0xffffff00
#define NOC_QOS_ID_COREIDn_CORECHECKSUM_SHFT                      0x8
#define NOC_QOS_ID_COREIDn_CORETYPEID_BMSK                       0xff
#define NOC_QOS_ID_COREIDn_CORETYPEID_SHFT                        0x0


/*---------------------------- QOS_ID_REVISIONIDn ----------------------------*/
#define NOC_QOS_ID_REVISIONIDn_ADDR(b,n)                   (NOC_QOS_REG_BASE(b) + 0x4 + 0x80 * (n))
#define NOC_QOS_ID_REVISIONIDn_RMSK                        0xffffffff
#define NOC_QOS_ID_REVISIONIDn_MAXn                                32
#define NOC_QOS_ID_REVISIONIDn_INI(b,n)        \
        in_dword_masked(NOC_QOS_ID_REVISIONIDn_ADDR(b,n), NOC_QOS_ID_REVISIONIDn_RMSK)
#define NOC_QOS_ID_REVISIONIDn_INMI(b,n,mask)    \
        in_dword_masked(NOC_QOS_ID_REVISIONIDn_ADDR(b,n), (mask))
#define NOC_QOS_ID_REVISIONIDn_OUTI(b,n,val)    \
        out_dword(NOC_QOS_ID_REVISIONIDn_ADDR(b,n),val)
#define NOC_QOS_ID_REVISIONIDn_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(NOC_QOS_ID_REVISIONIDn_ADDR(b,n),mask,val,NOC_QOS_ID_REVISIONIDn_INI(b,n))
#define NOC_QOS_ID_REVISIONIDn_FLEXNOCID_BMSK              0xffffff00
#define NOC_QOS_ID_REVISIONIDn_FLEXNOCID_SHFT                     0x8
#define NOC_QOS_ID_REVISIONIDn_USERID_BMSK                       0xff
#define NOC_QOS_ID_REVISIONIDn_USERID_SHFT                        0x0


/*------------------------------- QOS_PRIORITYn ------------------------------*/
#define NOC_QOS_PRIORITYn_ADDR(b,n)                    (NOC_QOS_REG_BASE(b) + 0x8  + 0x80 * (n))
#define NOC_QOS_PRIORITYn_RMSK                         0x0000000f
#define NOC_QOS_PRIORITYn_MAXn                                 32
#define NOC_QOS_PRIORITYn_INI(b,n)        \
        in_dword_masked(NOC_QOS_PRIORITYn_ADDR(b,n), NOC_QOS_PRIORITYn_RMSK)
#define NOC_QOS_PRIORITYn_INMI(b,n,mask)    \
        in_dword_masked(NOC_QOS_PRIORITYn_ADDR(b,n), (mask))
#define NOC_QOS_PRIORITYn_OUTI(b,n,val)    \
        out_dword(NOC_QOS_PRIORITYn_ADDR(b,n),val)
#define NOC_QOS_PRIORITYn_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(NOC_QOS_PRIORITYn_ADDR(b,n),mask,val,NOC_QOS_PRIORITYn_INI(b,n))
#define NOC_QOS_PRIORITYn_P1_BMSK                             0xc
#define NOC_QOS_PRIORITYn_P1_SHFT                             0x2
#define NOC_QOS_PRIORITYn_P0_BMSK                             0x3
#define NOC_QOS_PRIORITYn_P0_SHFT                             0x0


/*--------------------------------- QOS_MODEn --------------------------------*/
#define NOC_QOS_MODEn_ADDR(b,n)                        (NOC_QOS_REG_BASE(b) + 0xC + 0x80 * (n))
#define NOC_QOS_MODEn_RMSK                             0x00000003
#define NOC_QOS_MODEn_MAXn                                     32
#define NOC_QOS_MODEn_INI(b,n)        \
        in_dword_masked(NOC_QOS_MODEn_ADDR(b,n), NOC_QOS_MODEn_RMSK)
#define NOC_QOS_MODEn_INMI(b,n,mask)    \
        in_dword_masked(NOC_QOS_MODEn_ADDR(b,n), (mask))
#define NOC_QOS_MODEn_OUTI(b,n,val)    \
        out_dword(NOC_QOS_MODEn_ADDR(b,n),val)
#define NOC_QOS_MODEn_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(NOC_QOS_MODEn_ADDR(b,n),mask,val,NOC_QOS_MODEn_INI(b,n))
#define NOC_QOS_MODEn_MODE_BMSK                            0x3
#define NOC_QOS_MODEn_MODE_SHFT                            0x0


/*------------------------------ QOS_BANDWIDTHn ------------------------------*/
#define NOC_QOS_BANDWIDTHn_ADDR(b,n)                   (NOC_QOS_REG_BASE(b) + 0x10 + 0x80 * (n))
#define NOC_QOS_BANDWIDTHn_RMSK                        0x0000ffff
#define NOC_QOS_BANDWIDTHn_MAXn                                32
#define NOC_QOS_BANDWIDTHn_INI(b,n)        \
        in_dword_masked(NOC_QOS_BANDWIDTHn_ADDR(b,n), NOC_QOS_BANDWIDTHn_RMSK)
#define NOC_QOS_BANDWIDTHn_INMI(b,n,mask)    \
        in_dword_masked(NOC_QOS_BANDWIDTHn_ADDR(b,n), (mask))
#define NOC_QOS_BANDWIDTHn_OUTI(b,n,val)    \
        out_dword(NOC_QOS_BANDWIDTHn_ADDR(b,n),val)
#define NOC_QOS_BANDWIDTHn_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(NOC_QOS_BANDWIDTHn_ADDR(b,n),mask,val,NOC_QOS_BANDWIDTHn_INI(b,n))
#define NOC_QOS_BANDWIDTHn_BANDWIDTH_BMSK                 0xffff
#define NOC_QOS_BANDWIDTHn_BANDWIDTH_SHFT                    0x0


/*------------------------------ QOS_SATURATIONn ------------------------------*/
#define NOC_QOS_SATURATIONn_ADDR(b,n)                  (NOC_QOS_REG_BASE(b) + 0x14 + 0x80 * (n))
#define NOC_QOS_SATURATIONn_RMSK                       0x000003ff
#define NOC_QOS_SATURATIONn_MAXn                               32
#define NOC_QOS_SATURATIONn_INI(b,n)        \
        in_dword_masked(NOC_QOS_SATURATIONn_ADDR(b,n), NOC_QOS_SATURATIONn_RMSK)
#define NOC_QOS_SATURATIONn_INMI(b,n,mask)    \
        in_dword_masked(NOC_QOS_SATURATIONn_ADDR(b,n), (mask))
#define NOC_QOS_SATURATIONn_OUTI(b,n,val)    \
        out_dword(NOC_QOS_SATURATIONn_ADDR(b,n),val)
#define NOC_QOS_SATURATIONn_OUTMI(b,n,mask,val) \
        out_dword_masked_ns(NOC_QOS_SATURATIONn_ADDR(b,n),mask,val,NOC_QOS_SATURATIONn_INI(b,n))
#define NOC_QOS_SATURATIONn_SATURATION_BMSK                 0x3ff
#define NOC_QOS_SATURATIONn_SATURATION_SHFT                   0x0


#endif /* __HALNOCHWIOGENERIC_H__ */
