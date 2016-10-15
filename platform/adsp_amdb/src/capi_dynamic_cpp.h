#ifndef CAPI_DYNAMIC_CPP_H
#define CAPI_DYNAMIC_CPP_H
/*==============================================================================

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

#include "Elite_CAPI.h"

class capi_dynamic_cpp : public ICAPI {

private:
  capi_dynamic_cpp(void);

  int private_variable;

public:

  capi_dynamic_cpp(uint32_t format, uint32_t bps, ADSPResult* result);
  ~capi_dynamic_cpp(void);

  virtual int Init(CAPI_Buf_t* pParams);
  virtual int ReInit(CAPI_Buf_t* pParams);
  virtual int End(void);
  virtual int GetParam(int nParamIdx, int* pnParamVal);
  virtual int SetParam(int nParamIdx, int nParamVal);
  virtual int Process(const CAPI_BufList_t* pInBitStream,
                      CAPI_BufList_t* pOutSamples, CAPI_Buf_t* pOutParams);

  int public_variable;
};

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

ADSPResult capi_dynamic_cpp_getsize(int32_t format, uint32_t bps,
                                    uint32_t* size_ptr);
ADSPResult capi_dynamic_cpp_ctor(ICAPI* capi_ptr, uint32_t format,
                                 uint32_t bps);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* CAPI_DYNAMIC_CPP_H */

