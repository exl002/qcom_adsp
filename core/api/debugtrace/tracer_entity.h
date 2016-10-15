#ifndef TRACER_ENTITY_H
#define TRACER_ENTITY_H
/**==========================================================================
  @file tracer_entity.h

  @brief Defined entity identifiers for encapsulation of output stream and
         used by OST base protocol.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/api/debugtrace/tracer_entity.h#1 $ */

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief OST base layer entity values, unique per target system.
 */
#undef DEFINE_ENTITY
#define DEFINE_ENTITY(txt, val, cmt) TRACER_ENTITY_##txt=val, cmt
typedef enum
{
   #include "tracer_entity_defn.h"

   // 200-239 Reserved for assignment to 3rd parties.

   TRACER_ENTITY_MGMT     = 240, /**< OST system level manager */
   // 241 to 254 Reserved for future OST use.
   TRACER_ENTITY_ALL      = 255  /**< Broadcast to all entities */
} tracer_ost_entity_id_enum_t;

#define TRACER_ENTITY_MAX   256  /**< 8-bit entity id field, max # of values*/

typedef uint8 tracer_ost_entity_id_t;

/*-------------------------------------------------------------------------*/
/**
  @brief OST base layer protocol version values, unique per entity.
  Current protocol version for each defined entity.
 */
#define TRACER_PROTOCOL_NONE     0  /**< Not applicable, unknown, undefined */
#define TRACER_PROTOCOL_TDS      1  /**< Tracer data stream version */
#define TRACER_PROTOCOL_SWEVT    1  /**< SW Event w/params version */

typedef uint8 tracer_ost_protocol_id_t;

#define TRACER_ENTITY_DEFAULT    TRACER_ENTITY_TDS    /**< Default entity */
#define TRACER_PROTOCOL_DEFAULT  TRACER_PROTOCOL_TDS  /**< Default version */

#endif /* #ifndef TRACER_ENTITY_H */


