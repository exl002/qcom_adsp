/**==========================================================================
  @file tracer_entity_defn.h

  @brief For specifying entities recognized by tracer. Will be built
  into an enum and reference tables.

  Obtain entity ids from tracer_entity.h. Do not include
  tracer_entity_defn.h in your files.

               Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/api/debugtrace/tracer_entity_defn.h#2 $ */

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
#ifndef DEFINE_ENTITY
#define DEFINE_ENTITY(x,y,z) z
#endif

/**
  @brief OST base layer entity ids, unique per target system.

  DEFINE_ENTITY(name, value, comment)
      "name" is a maximum of 10 alphanumeric characters.
      "comment" must be properly delimited as a C-style comment.

  Reference entity id name is TRACER_ENTITY_<name>
 */
DEFINE_ENTITY(NONE,  0, /**< Raw output stream */)
DEFINE_ENTITY(TDS,   1, /**< Tracer output stream (w/ TDS header) */)
DEFINE_ENTITY(SWEVT, 2, /**< Software event with parameters */)
// IDs 3-10 are reserved for TRACER use.

// All others are independent output streams (OST header and framing only)
DEFINE_ENTITY(ULOG, 11, /**< Ulog */)
DEFINE_ENTITY(PROF, 12, /**< PMU and TP profiling */)

// 200-239 Reserved for assignment to 3rd parties.
// 240 to 254 Reserved for OST use.


#undef DEFINE_ENTITY
