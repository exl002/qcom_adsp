/** vi: tw=128 ts=3 sw=3 et
@file tms_utils_hash.c
@brief This file contains the API for the TMS Utilities, API 0.x
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All rights reserved.
QUALCOMM Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.adsp/2.2/debugtools/utils/src/tms_utils_hash.c#1 $
$DateTime: 2014/07/24 05:52:16 $
$Change: 6312319 $
$Author: pwbldsvc $
===============================================================================*/

#if defined(TMS_UTILS_TRACER_SWEVT)
#include "tracer.h"
#include "tms_utils_tracer_swe.h"
#endif

#if 0

// Code Size: 0Bytes
// Stack Resource: 0Bytes
// This implementation is primarily for utilization in C++11 for compile time instansiation.

// Function must remain reentrant and not utilize NHLOS or external library calls which
// are not reentrant or potentially cause any type of NHLOS blocking to occur.

#include "tms_utils.h"

constexpr uint32_t tms_utils_hash_internal(int8_t const* in_buf_p, const size_t in_buf_sz, uint32_t hash_acc)
{
   return in_buf_sz && in_buf_p && *in_buf_p
         ? tms_utils_hash_internal(in_buf_p + 1, in_buf_sz - 1,
                                   (0 != (((hash_acc << 4) + (uint32_t)(*in_buf_p)) & 0xf0000000)
                                    ? (((hash_acc << 4) + (uint32_t)(*in_buf_p)) ^ ((((hash_acc << 4) + (uint32_t)(*in_buf_p)) & 0xf0000000) >> 24))
                                    : (((hash_acc << 4) + (uint32_t)(*in_buf_p)))) & (~(((hash_acc << 4) + (uint32_t)(*in_buf_p)) & 0xf0000000)))
         : hash_acc;
}

#else

// Code Size: ~92Byte
// Stack Resource: ~16Byte

// Function must remain reentrant and not utilize NHLOS or external library calls which
// are not reentrant or potentially cause any type of NHLOS blocking to occur.

#include "tms_utils.h"

uint32_t tms_utils_hash_internal(int8_t const* in_buf_p, const size_t in_buf_sz, uint32_t hash_acc)
{
   size_t sz = in_buf_sz;

   while (sz && in_buf_p && *in_buf_p)
   {
      uint32_t temp;

      hash_acc = (hash_acc << 4) + (uint32_t)(*in_buf_p);

      temp = hash_acc & 0xf0000000;

      if (temp)
      {
         hash_acc ^= (temp >> 24);
      }

      hash_acc &= (~temp);

      sz--;

      in_buf_p++;
   }

   return hash_acc;
}

#endif

uint32_t tms_utils_hash(int8_t const* in_buf_p, const size_t in_buf_sz)
{
   return tms_utils_hash_internal(in_buf_p, in_buf_sz, 0);
}
