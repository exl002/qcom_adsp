/** vi: tw=128 ts=3 sw=3 et
@file tms_utils_num.c
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
$Header: //components/rel/core.adsp/2.2/debugtools/utils/src/tms_utils_num.c#1 $
$DateTime: 2014/07/24 05:52:16 $
$Change: 6312319 $
$Author: pwbldsvc $
===============================================================================*/

#if defined(TMS_UTILS_TRACER_SWEVT)
#include "tracer.h"
#include "tms_utils_tracer_swe.h"
#endif

// Code Size: ~1.5KB Optimized
// Stack Resource: ~80Byte + 4Byte * Vargs_Template_Expansion_Arguments

// Function must remain reentrant and not utilize NHLOS or external library calls which
// are not reentrant or potentially cause any type of NHLOS blocking to occur.

#include "tms_utils.h"

#define ASCII_ZERO      '0'         // ASCII ZERO
#define ASCII_SIGN      '-'         // ASCII
#define ASCII_PLUS      '+'         // ASCII

#define ASCII_A         'a'         // ASCII

int64_t tms_utils_num_int64(int8_t const* in_buf_p, size_t in_buf_sz)
{
   int64_t rc = 0;
   int8_t sign;
   int8_t num_chr;
   int8_t const* num_str = in_buf_p;

   // Skip leading space

   while (num_str - in_buf_p < in_buf_sz && isspace(*num_str))
   {
      num_str++;
   }

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Record presense of a SIGN

   sign = num_chr = *num_str++;

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Skip over any SIGN presence

   if (ASCII_SIGN == num_chr || ASCII_PLUS == num_chr)
   {
      num_chr = *num_str++;
   }

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Loop Across Digits Obtain Value

   while (num_str - in_buf_p < in_buf_sz && isdigit(num_chr))
   {
      rc = 10 * rc + (num_chr - ASCII_ZERO);

      num_chr = *num_str++;
   }

   // If SIGN presence then adjust return

   if (ASCII_SIGN == sign)
   {
      return 0 - rc;
   }

   else
   {
      return rc;
   }
}

uint64_t tms_utils_num_uint64(int8_t const* in_buf_p, size_t in_buf_sz)
{
   uint64_t rc = 0;
   int8_t num_chr;
   int8_t const* num_str = in_buf_p;

   // Skip leading space

   while (num_str - in_buf_p < in_buf_sz && isspace(*num_str))
   {
      num_str++;
   }

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // First Non Space; Does Not Check For Leading '0x'

   num_chr = *num_str++;

   // Bail out check

   if (num_str - in_buf_p >= in_buf_sz)
   {
      return rc;
   }

   // Loop Across Digits Obtain Value

   while (num_str - in_buf_p < in_buf_sz && (isdigit(num_chr) || isalpha(num_chr)))
   {
      if (isdigit(num_chr))
      {
         rc = 16 * rc + (num_chr - ASCII_ZERO);
      }

      else if (isalpha(num_chr))
      {
         rc = 16 * rc + (tolower(num_chr) - ASCII_A + 10);
      }

      num_chr = *num_str++;
   }

   return rc;
}

int32_t tms_utils_num_int32(int8_t const* in_buf_p, size_t in_buf_sz)
{
   return (int32_t)tms_utils_num_int64(in_buf_p, in_buf_sz);
}

uint32_t tms_utils_num_uint32(int8_t const* in_buf_p, size_t in_buf_sz)
{
   return (uint32_t)tms_utils_num_uint64(in_buf_p, in_buf_sz);
}
