#ifndef TMS_UTILS_H
#define TMS_UTILS_H
/** vi: tw=128 ts=3 sw=3 et
@file tms_utils.h
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
$Header: //components/rel/core.adsp/2.2/debugtools/api/tms_utils.h#1 $
$DateTime: 2014/07/24 05:52:16 $
$Change: 6312319 $
$Author: pwbldsvc $
===============================================================================*/

/**

All function implementations as part of this API must remain reentrant. The
routines specifically do not use any NHLOS API which will block, or otherwise
serialize access to the data parameters.

It is the caller's responsiblity to handle data resource contention issues
as necessary. No routine within this API will internally supply resource
management for supplied data parameters.

*/

#include "ctype.h"
#include "stdarg.h"
#include "stddef.h"
#include "stdint.h"
#include "stdlib.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** @addtogroup tms_utils_api @{ */

/**
API, Typedefs and Enumerations
*/

typedef enum
{
   TMS_UTILS_STATUS_SUCCESS = 0,                                                 /**< SUCCESS INDICATION, evaluates as >= TMS_UTILS_STATUS_SUCCESS */
   TMS_UTILS_STATUS_ERROR = -1                                                   /**< FAILURE INDICATION, evaluates as < TMS_UTILS_STATUS_SUCCESS */

} TMS_UTILS_STATUS;

/**
Used to Ignore Warnings on Unused Results, NOT ENCOURAGED
*/

#define TMS_UTILS_STATUS_IGNORE(expr) do { if (0 < expr) {;} } while (0)         /**< Caller Can Ignore Return Status, NOT ENCOURAGED */

/**
API, NULL Terminated Buffer Length

int8_t const buf[] = "string initializer";
size_t length = tms_utils_length(buf);

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
size_t tms_utils_chr_length(int8_t const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Compare

int8_t const buf[] = "string initializer";
if (0 == tms_utils_chr_compare(buf, sizeof(buf), ...)) match;
if (0 > tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;
if (0 < tms_utils_chr_compare(buf, sizeof(buf), ...)) lexical_compare;

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
int32_t tms_utils_chr_compare(int8_t const* in_buf_p, size_t in_buf_sz, int8_t const* in_buf2_p, size_t in_buf2_sz);



/**
API, NULL Terminated Buffer Initializer, Result Buffer Always NULL Terminated

if (sizeof(buf) > tms_utils_chr_init(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) == tms_utils_chr_init(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     int8_t*                       Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     int8_t                        Output Buffer Initializer
@return        size_t                        Operation Result
*/
size_t tms_utils_chr_init(int8_t* out_buf_p, size_t out_buf_sz, const int8_t chr);

/**
API, NULL Terminated Buffer Copy, Result Buffer Always NULL Terminated

int8_t const buf[] = "string initializer";
if (sizeof(buf) > tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) == tms_utils_chr_copy(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     int8_t*                       Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1,3)))
#endif
size_t tms_utils_chr_copy(int8_t* out_buf_p, size_t out_buf_sz, int8_t const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Append, Result Buffer Always NULL Terminated

int8_t const buf[] = "string initializer";
if (sizeof(buf) > tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_underflow;
if (sizeof(buf) == tms_utils_chr_append(buf, sizeof(buf), ...)) buffer_overflow;

@param[in]     int8_t*                       Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        size_t                        Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3)))
#endif
size_t tms_utils_chr_append(int8_t* out_buf_p, size_t out_buf_sz, int8_t const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Search, First Occur of Param

int8_t const buf[] = "string initializer";
int8_t const* location = tms_utils_chr_first(buf, sizeof(buf), 'r');

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     int8_t                        Input Character
@return        int8_t*                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int8_t const* tms_utils_chr_first(int8_t const* in_buf_p, size_t in_buf_sz, const int8_t chr);

/**
API, NULL Terminated Buffer Search, Last Occur of Param

int8_t const buf[] = "string initializer";
int8_t const* location = tms_utils_chr_last(buf, sizeof(buf), 'r');

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@param[in]     int8_t                        Input Character
@return        int8_t*                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int8_t const* tms_utils_chr_last(int8_t const* in_buf_p, size_t in_buf_sz, const int8_t chr);

/**
API, Formatted Output to Sized Buffer

NOTE: Caller of routine supplies NHLOS protections to Output Buffer if
required. The routine itself does not carry any specific NHLOS details.
This routine supports only the following template expansions, nothing
more.

Supported Template Format Expansions

   %[-][0][ww][type]

   [-] left justification within field
   [0] zero pad numeric output
   [www] field width
   [type] field type
      c  character argument
      s  NULL terminated buffer pointer argument
      b  binary uint32_t argument
      d  decimal int32_t argument
      o  octal uint32_t argument
      x  hex uint32_t argument
      ld decimal int32_t argument
      lx hex uint32_t argument
      X  hex uint32_t argument (caps expansion)
      lX hex uint32_t argument (caps expansion)

Use Example

   int32_t count;
   int8_t buffer[256];

   if (0 < (count = tms_utils_fmt(buffer, sizeof(buffer), "%d", 12345)))
   {
      PRINT("%d, %d == strlen(\"%s\")\n", count, strlen(buffer), buffer);
   }
   else
   {
      ERROR("no output\n");
   }

@param[in]     int8_t*                       Output Buffer Pointer
@param[in]     size_t                        Output Buffer Size
@param[in]     int8_t*                       Format Template Pointer
@param[in]     ...                           Format Template Expansion Arguments
@return        int32_t                       Operation Result
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1, 3), format(printf, 3, 4)))
#endif
int32_t tms_utils_fmt(int8_t* out_buf_p, size_t out_buf_sz, char const* fmt_str, ...);

/**
API, NULL Terminated Buffer Scan as Decimal, Return Numeric Representation

int8_t const buf[] = "665";
int32_t number = tms_utils_num_int32(buf, sizeof(buf));

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int32_t                       Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int32_t tms_utils_num_int32(int8_t const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Hex, Return Numeric Representation

int8_t const buf[] = "12345678";
uint32_t number = tms_utils_num_uint32(buf, sizeof(buf));

The scanner does not validate the content of the buffer. The
scanner does not parse leading 0x|0X. All buffer validation must
be performed before use of this routine.

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint32_t                      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
uint32_t tms_utils_num_uint32(int8_t const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Decimal, Return Numeric Representation

int8_t const buf[] = "50000";
int64_t number = tms_utils_num_int64(buf, sizeof(buf));

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        int64_t                       Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
int64_t tms_utils_num_int64(int8_t const* in_buf_p, size_t in_buf_sz);

/**
API, NULL Terminated Buffer Scan as Hex, Return Numeric Representation

int8_t const buf[] = "aabbccdd";
uint64_t number = tms_utils_num_uint64(buf, sizeof(buf));

The scanner does not validate the content of the buffer. The
scanner does not parse leading 0x|0X. All buffer validation must
be performed before use of this routine.

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint64_t                      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
uint64_t tms_utils_num_uint64(int8_t const* in_buf_p, size_t in_buf_sz);

/**
API, Utility Hash Function

int8_t const buf[] = "string initializer";
uint32_t hash = tms_utils_hash(buf);

@param[in]     int8_t*                       Input Buffer Pointer
@param[in]     size_t                        Input Buffer Size
@return        uint32_t                      Numeric Representation
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result, nonnull(1)))
#endif
uint32_t tms_utils_hash(int8_t const* in_buf_p, size_t in_buf_sz);

/** @} end_addtogroup tms_utils_api */

#if defined(__cplusplus)
}
#endif

#endif
