/*===========================================================================
  FILE: tracer_diag.c

  OVERVIEW:     Tracer's interface to diag packet service

  DEPENDENCIES:

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/tracer_diag.c#3 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <stringl/stringl.h>   // memset (not available Venus, LPASS)
//#include <string.h>

#include "diagpkt.h"
#include "diagcmd.h"

#include "qdss.h"
#include "tracer_entity.h"
#include "tracer_event_ids.h"
#include "tracer_config_int.h"
#include "tracer_diag.h"
#include "tracer_cfgrpmevt.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
#define MODEM  1
#define ADSP   2
#define PRONTO 3
#define VENUS  4
#define RPM    5
#define APPS   6
#define SENSOR 7
#define AUDIO  8
#if (EVENT_SUBSYSTEM == 1)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_MODEM
#elif (EVENT_SUBSYSTEM == 2)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_LPASS
#elif (EVENT_SUBSYSTEM == 3)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_WCNSS
#elif (EVENT_SUBSYSTEM == 4)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_VENUS
#elif (EVENT_SUBSYSTEM == 5)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_RPM
#elif (EVENT_SUBSYSTEM == 6)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_APPS
#elif (EVENT_SUBSYSTEM == 7)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_SENSOR
#elif (EVENT_SUBSYSTEM == 8)
   #define TRACER_DIAG_PROC_ID   QDSSDIAG_PROCESSOR_AUDIO
#else
   #define TRACER_DIAG_PROC_ID   0x0
#endif
#undef MODEM
#undef ADSP
#undef PRONTO
#undef VENUS
#undef RPM
#undef APPS
#undef SENSOR
#undef AUDIO


/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
PACK(void *) tracer_diag_pkt_handler(PACK(void *) pRqstPkt, uint16 pkt_len);
PACK(void *) tracer_diag_pkt_handler_ext(PACK(void *) pRqstPkt, uint16 pkt_len);

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

// Diag packet service - callback table.
static const diagpkt_user_table_entry_type tracer_diag_pkt_tbl[] =
{
   { TRACER_DIAG_PROC_ID | TRACER_DIAG_QRY_TRACER_STATUS,// range: from
     TRACER_DIAG_PROC_ID | TRACER_DIAG_CTRL_SWE,         // range: to
     tracer_diag_pkt_handler },                          // handler fcn
   { TRACER_DIAG_PROC_ID | TRACER_DIAG_CTRL_RPMSWE,      // range: from
     TRACER_DIAG_PROC_ID | TRACER_DIAG_SET_SYSTESTCTRL,  // range: to
     tracer_diag_pkt_handler_ext }                       // handler fcn
};

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/
#define TRACER_RESP_PKT_SET(_result) \
            pRespPkt = (tracer_diag_pkt_resp *) diagpkt_subsys_alloc( \
                  DIAG_SUBSYS_QDSS, \
                  pHdrPkt->subsysCmdCode, \
                  sizeof(tracer_diag_pkt_resp)); \
            if (NULL != pRespPkt) \
            { \
               pRespPkt->result = _result; \
            }

#define TRACER_ALLOCDIAGRESP(_resp_t) \
         (_resp_t *) diagpkt_subsys_alloc(DIAG_SUBSYS_QDSS, \
         pHdrPkt->subsysCmdCode, sizeof(_resp_t))

/*-------------------------------------------------------------------------*/
PACK(void *) tracer_diag_pkt_handler(PACK(void *) pRqstPkt, uint16 pkt_len)
{
   tracer_diag_pkt_hdr *pHdrPkt;
   tracer_diag_pkt_resp *pRespPkt = NULL;

   tracer_diag_resp_tracer_status *pRespPkt_QTracerStatus;
   tracer_diag_resp_entity_state *pRespPkt_QEntityState;
   tracer_diag_resp_swe_tag *pRespPkt_QSweTag;
   tracer_diag_resp_swe_state *pRespPkt_QSweState;

   PACK(void *)pResp = NULL;

   tracer_cmdresp_t trc_ret;
   tracer_ost_entity_id_enum_t nEntity_id;
   tracer_event_id_t nEvent_id;

   if (NULL != pRqstPkt)
   {
      pHdrPkt = (tracer_diag_pkt_hdr *)pRqstPkt;

      switch (pHdrPkt->subsysCmdCode & 0x0FF)
      {
         case TRACER_DIAG_QRY_TRACER_STATUS:
            pRespPkt_QTracerStatus = TRACER_ALLOCDIAGRESP(
                                     tracer_diag_resp_tracer_status);
            if (NULL != pRespPkt_QTracerStatus)
            {
               pRespPkt_QTracerStatus->status =
                  (tcfg_is_op_enabled() ? 0x01 : 0) |
                  (QDSSisInUse() ? 0x02 : 0) |
                  (tcfg_is_event_initialized() ? 0x04 : 0) |
                  (tcfg_is_tracer_initialized() ? 0x08 : 0);
               pRespPkt_QTracerStatus->entityCnt = tcfg_get_entity_count();
               pRespPkt_QTracerStatus->eventCnt = tcfg_get_event_count();

            }
            pResp = (void *)pRespPkt_QTracerStatus;
         break;

         case TRACER_DIAG_CTRL_OP:
            trc_ret = tracer_cfg_op_ctrl(
               ((0 == (((tracer_diag_op_ctrl *)pRqstPkt)->state)) ?
               TRACER_OUTPUT_DISABLE : TRACER_OUTPUT_ENABLE));
            TRACER_RESP_PKT_SET(trc_ret);
            pResp = (void *)pRespPkt;
         break;

         case TRACER_DIAG_QRY_ENTITY_STATE:
            pRespPkt_QEntityState = TRACER_ALLOCDIAGRESP(
                                    tracer_diag_resp_entity_state);
            if (NULL != pRespPkt_QEntityState)
            {
               tcfg_cpy_entity_op_ctrl(pRespPkt_QEntityState->mask,
                       sizeof (pRespPkt_QEntityState->mask));
            }
            pResp = (void *)pRespPkt_QEntityState;
         break;

         case TRACER_DIAG_CTRL_ENTITY:
            nEntity_id = (tracer_ost_entity_id_enum_t)
               (((tracer_diag_entity_ctrl *)pRqstPkt)->entityId);
            if (0xFF != nEntity_id)
            {
               trc_ret = tracer_cfg_entity_ctrl(nEntity_id,
                  (0 == (((tracer_diag_entity_ctrl *)pRqstPkt)->state)) ?
                  TRACER_ENTITY_OFF : TRACER_ENTITY_ON);
            }
            else
            {
               trc_ret = tracer_cfg_entity_ctrl_all(
                  (0 == (((tracer_diag_entity_ctrl *)pRqstPkt)->state)) ?
                  TRACER_ENTITY_OFF : TRACER_ENTITY_ON);
            }
            TRACER_RESP_PKT_SET(trc_ret);
            pResp = (void *)pRespPkt;
         break;

         case TRACER_DIAG_QRY_SWE_TAG:
            pRespPkt_QSweTag = TRACER_ALLOCDIAGRESP(
                               tracer_diag_resp_swe_tag);
            if (NULL != pRespPkt_QSweTag)
            {
               tcfg_cpy_event_tag(pRespPkt_QSweTag->tag,
                                  sizeof(pRespPkt_QSweTag->tag));
            }
            pResp = (void *)pRespPkt_QSweTag;
            break;

         case TRACER_DIAG_QRY_SWE_STATE:
            pRespPkt_QSweState = TRACER_ALLOCDIAGRESP(
                                 tracer_diag_resp_swe_state);
            if (NULL != pRespPkt_QSweState)
            {
               pRespPkt_QSweState->group =
                  ((tracer_diag_qry_swe_state *)pRqstPkt)->group;
               tcfg_cpy_event_op_ctrl (pRespPkt_QSweState->mask,
                                       pRespPkt_QSweState->group,
                                       sizeof(pRespPkt_QSweState->mask));
            }
            pResp = (void *)pRespPkt_QSweState;
         break;

         case TRACER_DIAG_CTRL_SWE:
            nEvent_id = (tracer_event_id_t)
                        (((tracer_diag_swe_ctrl *)pRqstPkt)->eventId);
            if (0xFFFFFFFF != nEvent_id)
            {
               trc_ret = tracer_event_ctrl(nEvent_id,
                  (0 == (((tracer_diag_swe_ctrl *)pRqstPkt)->state)) ?
                  TRACER_EVENT_OFF: TRACER_EVENT_ON
                  );
            }
            else
            {
               trc_ret = tracer_event_ctrl_all(
                  (0 == (((tracer_diag_swe_ctrl *)pRqstPkt)->state)) ?
                  TRACER_EVENT_OFF: TRACER_EVENT_ON
                  );
            }
            TRACER_RESP_PKT_SET(trc_ret);
            pResp = (void *)pRespPkt;
         break;

         default:
            pResp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pRqstPkt, pkt_len);
         break;
      }

      if (NULL != pResp)
      {
         diagpkt_commit(pResp);
         pResp = NULL;
      }
   }
   return (pResp);
}
/*-------------------------------------------------------------------------*/
PACK(void *) tracer_diag_pkt_handler_ext(PACK(void *) pRqstPkt, uint16 pkt_len)
{
   tracer_diag_pkt_hdr *pHdrPkt;
   tracer_diag_pkt_resp *pRespPkt = NULL;
   PACK(void *)pResp = NULL;
   tracer_cmdresp_t trc_ret;

   if (NULL != pRqstPkt)
   {
      pHdrPkt = (tracer_diag_pkt_hdr *)pRqstPkt;

      switch (pHdrPkt->subsysCmdCode & 0x0FF)
      {
         case TRACER_DIAG_CTRL_RPMSWE:
            _rpm_swevent_cfg_send(
               (uint32)(((tracer_diag_rpmswe_ctrl *)pRqstPkt)->sink),
               (uint32)(((tracer_diag_rpmswe_ctrl *)pRqstPkt)->evtGroup),
               ((tracer_diag_rpmswe_ctrl *)pRqstPkt)->evtBitmask
               );
            TRACER_RESP_PKT_SET(TRACER_CMDRESP_SUCCESS);
            pResp = (void *)pRespPkt;
         break;

         case TRACER_DIAG_SET_SYSTESTCTRL:
            trc_ret = tcfg_set_systest_ctl(
               ((tracer_diag_set_systestctrl *)pRqstPkt)->value);
            TRACER_RESP_PKT_SET(trc_ret);
            pResp = (void *)pRespPkt;
         break;

         default:
            pResp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pRqstPkt, pkt_len);
         break;
      }

      if (NULL != pResp)
      {
         diagpkt_commit(pResp);
         pResp = NULL;
      }
   }
   return (pResp);
}

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

// Initialize interface with diag packet service
void tracer_diag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (
     (diagpkt_subsys_id_type) DIAG_SUBSYS_QDSS, tracer_diag_pkt_tbl);
  return;
}

