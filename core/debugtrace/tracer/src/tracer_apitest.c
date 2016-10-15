/*===========================================================================
  FILE: tracer_apitest.c

  OVERVIEW:     QUALCOMM Debug Subsystem (QDSS) Tracer - Test stub

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/tracer_apitest.c#2 $
===========================================================================*/
#ifndef TRACER_TESTING_ON
#define TRACER_TESTING_ON 0   // Development testing
#endif
#if TRACER_TESTING_ON

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer.h"
#include "tracer_config.h"
#include "tracer_config_int.h"
#include "tracer_event_ids.h"

/*---------------------------------------------------------------------------
 * Unit test sanity check.
 * ------------------------------------------------------------------------*/
int tracer_test(void)
{
   tracer_client_handle_t client_handle;

   if (TRACER_RETURN_SUCCESS != tracer_register(&client_handle,
       TRACER_ENTITY_DEFAULT, TRACER_PROTOCOL_DEFAULT))
   { return 0; }

   // Exercise control functions and report.
   tracer_cfg_entity_ctrl(TRACER_ENTITY_DEFAULT, TRACER_ENTITY_ON);
   tracer_cfg_entity_ctrl_all(TRACER_ENTITY_ON);

   tracer_cfg_op_ctrl(TRACER_OUTPUT_ENABLE);
   tracer_data(client_handle, TRACER_OPT_GUARANTEE,
               0, "Events enabled by default: %d", tcfg_get_event_count());

   tracer_data(client_handle, TRACER_OPT_GUARANTEE | TRACER_OPT_TIMESTAMP,
       0, "Hello, World!");

   tracer_set_owner(client_handle, 1234);
   tracer_data(client_handle, TRACER_OPT_NONE,
       0, "My thread 1234");

   tracer_event_ctrl(TRCEVT_1_INIT, TRACER_EVENT_ON);
   tracer_event_ctrl(TRCEVT_2_ARG3, TRACER_EVENT_ON);

   tracer_data(client_handle, TRACER_OPT_TIMESTAMP,
               0, "Events enabled for test: %d", tcfg_get_event_count());

   tracer_event(client_handle, TRACER_EVENT_RESERVE_0); // should not print
   tracer_event(client_handle, TRCEVT_1_INIT);
   tracer_event_simple(TRCEVT_1_INIT);
   tracer_event_simple_vargs(TRCEVT_1_INIT, 2, 0x0496, 4112);
   tracer_event_simple_vargs(TRCEVT_2_ARG3, 3, 0x53534451, 0x63617254, 0x21217265);

   // Return to default startup state.
#ifdef QDSS_STANDALONE_MODE
   tracer_cfg_entity_ctrl_all(TRACER_ENTITY_ON);
   tracer_event_ctrl_all(TRACER_EVENT_ON);
   tracer_cfg_op_ctrl(TRACER_OUTPUT_ENABLE);
#else
   tracer_cfg_op_ctrl(TRACER_OUTPUT_DISABLE);
   tracer_cfg_entity_ctrl_all(TRACER_ENTITY_OFF);
   tcfg_event_set_default();
#endif

   tracer_unregister(&client_handle);

   return 0;
}
#endif //TRACER_TESTING_ON

