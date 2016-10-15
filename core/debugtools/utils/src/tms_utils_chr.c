/** vi: tw=128 ts=3 sw=3 et
@file tms_utils_chr.c
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
$Header: //components/rel/core.adsp/2.2/debugtools/utils/src/tms_utils_chr.c#1 $
$DateTime: 2014/07/24 05:52:16 $
$Change: 6312319 $
$Author: pwbldsvc $
===============================================================================*/

#if defined(TMS_UTILS_TRACER_SWEVT)
#include "tracer.h"
#include "tms_utils_tracer_swe.h"
#endif

// Code Size: ~0.3KB Optimized
// Stack Resource: ~32B Maximum Single Routine

// Function must remain reentrant and not utilize NHLOS or external library calls which
// are not reentrant or potentially cause any type of NHLOS blocking to occur.

#include "stdint.h"
#include "tms_utils.h"

size_t tms_utils_chr_length(int8_t const* in_buf_p, size_t in_buf_sz)
{
   int8_t const* in_p = in_buf_p;

   while (in_buf_sz && in_p && *in_p)
   {
      in_buf_sz--, in_p++;
   }

   return in_buf_p ? in_p - in_buf_p : 0;
}

int32_t tms_utils_chr_compare(int8_t const* in_buf_p, size_t in_buf_sz, int8_t const* in_buf2_p, size_t in_buf2_sz)
{
   while (in_buf_sz && in_buf_p && in_buf2_sz && in_buf2_p)
   {
      if (*in_buf_p != *in_buf2_p)
      {
         return *in_buf_p - *in_buf2_p;
      }

      in_buf_sz--, in_buf_p++;

      in_buf2_sz--, in_buf2_p++;
   }

   return 0;
}

size_t tms_utils_chr_init(int8_t* out_buf_p, size_t out_buf_sz, const int8_t chr)
{
   int8_t* out_p = out_buf_p;

   while (out_buf_sz && out_p)
   {
      *out_buf_p = chr;

      out_buf_sz--, out_p++;
   }

   if (out_buf_sz && out_p)
   {
      *out_p = '\0';

      return out_p - out_buf_p;
   }

   if (out_p && out_p - out_buf_p)
   {
      *(out_p - 1) = '\0';

      return out_p - out_buf_p;
   }

   return 0;
}

size_t tms_utils_chr_copy(int8_t* out_buf_p, size_t out_buf_sz, int8_t const* in_buf_p, size_t in_buf_sz)
{
   int8_t* out_p = out_buf_p;
   int8_t const* in_p = in_buf_p;

   while (out_buf_sz && out_p && in_buf_sz && in_p && *in_p)
   {
      *out_p = *in_p;

      out_buf_sz--, out_p++;

      in_buf_sz--, in_p++;
   }

   if (out_buf_sz && out_p)
   {
      *out_p = '\0';

      return out_p - out_buf_p;
   }

   if (out_p && out_p - out_buf_p)
   {
      *(out_p - 1) = '\0';

      return out_p - out_buf_p;
   }

   return 0;
}

size_t tms_utils_chr_append(int8_t* out_buf_p, size_t out_buf_sz, int8_t const* in_buf_p, size_t in_buf_sz)
{
   int8_t* out_p = out_buf_p;
   int8_t const* in_p = in_buf_p;

   while (out_buf_sz && out_p && *out_p)
   {
      out_buf_sz--, out_p++;
   }

   while (out_buf_sz && out_p && in_buf_sz && in_p && *in_p)
   {
      *out_p = *in_p;

      out_buf_sz--, out_p++;

      in_buf_sz--, in_p++;
   }

   if (out_buf_sz && out_p)
   {
      *out_p = '\0';

      return out_p - out_buf_p;
   }

   if (out_p && out_p - out_buf_p)
   {
      *(out_p - 1) = '\0';

      return out_p - out_buf_p;
   }

   return 0;
}

int8_t const* tms_utils_chr_first(int8_t const* in_buf_p, size_t in_buf_sz, const int8_t chr)
{
   int8_t const* rc = NULL;

   if (in_buf_p && in_buf_sz)
   {
      size_t sz = in_buf_sz;

      do
      {
         if (*in_buf_p == chr)
         {
            rc = in_buf_p;

            break;
         }

      } while (*in_buf_p++ && sz--);
   }

   return rc;
}

int8_t const* tms_utils_chr_last(int8_t const* in_buf_p, const size_t in_buf_sz, const int8_t chr)
{
   int8_t const* rc = NULL;

   if (in_buf_p && in_buf_sz)
   {
      size_t sz = in_buf_sz;

      do
      {
         if (*in_buf_p == chr)
         {
            rc = in_buf_p;
         }

      } while (*in_buf_p++ && sz--);
   }

   return rc;
}
