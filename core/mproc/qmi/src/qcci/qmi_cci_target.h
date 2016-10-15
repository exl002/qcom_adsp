#ifndef QMI_CCI_TARGET_H
#define QMI_CCI_TARGET_H
/******************************************************************************
  @file    qmi_cci_target.h
  @brief   OS Specific routines internal to QCCI.

  DESCRIPTION
  This header provides an OS abstraction to QCCI.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "qurt.h"
#include "smem_log.h"
#include "err.h"
#include "assert.h"
#include "qmi_common.h"
#include "qmi_cci_target_ext.h"
#include "qmi_idl_lib_internal.h"

typedef qurt_mutex_t qmi_cci_lock_type;
#define LOCK(ptr)        qurt_mutex_lock(ptr)
#define UNLOCK(ptr)      qurt_mutex_unlock(ptr)
#define LOCK_INIT(ptr)   qurt_mutex_init(ptr)
#define LOCK_DEINIT(ptr) qurt_mutex_destroy(ptr)

#define MALLOC(size)      malloc(size)
#define CALLOC(num, size) calloc(num, size)
#define FREE(ptr)         free(ptr)
#define REALLOC(ptr,size) realloc(ptr, size)

#define QMI_CCI_LOG_EVENT_TX            (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x00)
#define QMI_CCI_LOG_EVENT_TX_EXT        (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x04)
#define QMI_CCI_LOG_EVENT_RX            (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x01)
#define QMI_CCI_LOG_EVENT_RX_EXT        (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x05)
#define QMI_CCI_LOG_EVENT_ERROR         (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x03)

#define QMI_CCI_OS_EXT_SIGNAL_INIT(ptr, os_params) \
  do { \
    ptr = os_params; \
    (ptr)->timed_out = 0; \
    (ptr)->timer_inited = FALSE; \
    if((ptr)->sig == 0 || !is_pow_2((ptr)->sig)) {  \
      (ptr)->sig = 0x1; \
    } \
    if((ptr)->ext_signal == NULL) { \
      qurt_anysignal_init(&((ptr)->signal));  \
    } \
    if((ptr)->timer_sig != 0 && (ptr)->timer_sig != (ptr)->sig && is_pow_2((ptr)->timer_sig)) { \
      (ptr)->timer_inited = TRUE; \
      qurt_timer_attr_init(&((ptr)->timer_attr)); \
      qurt_timer_attr_set_type(&((ptr)->timer_attr), QURT_TIMER_ONESHOT); \
      qurt_timer_attr_set_duration(&((ptr)->timer_attr), QURT_TIMER_MAX_DURATION); \
      qurt_timer_attr_set_group(&((ptr)->timer_attr), 0); \
      qurt_timer_create(&((ptr)->timer), &((ptr)->timer_attr), (ptr)->ext_signal ? (ptr)->ext_signal : &((ptr)->signal), (ptr)->timer_sig); \
      qurt_timer_stop((ptr)->timer); \
    } \
  } while(0)

#ifdef FEATURE_QMI_SMEM_LOG

#define QMI_CCI_OS_LOG_TX(header) \
  do { \
    uint8_t cntl_flag; \
    uint16_t txn_id, msg_id, msg_len; \
    decode_header(header, &cntl_flag, &txn_id, &msg_id, &msg_len); \
    SMEM_LOG_EVENT(QMI_CCI_LOG_EVENT_TX, cntl_flag << 16 | txn_id, msg_id << 16 | msg_len, 0); \
  } while(0)

#define QMI_CCI_OS_LOG_RX(header) \
  do { \
    uint8_t cntl_flag; \
    uint16_t txn_id, msg_id, msg_len; \
    decode_header(header, &cntl_flag, &txn_id, &msg_id, &msg_len); \
    SMEM_LOG_EVENT(QMI_CCI_LOG_EVENT_RX, cntl_flag << 16 | txn_id, msg_id << 16 | msg_len, 0); \
  } while(0)

#define QMI_CCI_OS_LOG_TX_EXT(header, clnt) \
  do { \
    uint8_t cntl_flag; \
    uint16_t txn_id, msg_id, msg_len; \
    decode_header(header, &cntl_flag, &txn_id, &msg_id, &msg_len); \
    if(clnt) {  \
      uint32_t *addr = (uint32_t *)clnt->server_addr;  \
      SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_TX_EXT, cntl_flag << 16 | txn_id, msg_id << 16 | msg_len, clnt->service_obj->service_id, addr[0], addr[1], addr[2]); \
    } else {  \
      SMEM_LOG_EVENT(QMI_CCI_LOG_EVENT_TX, cntl_flag << 16 | txn_id, msg_id << 16 | msg_len, 0); \
    } \
  } while(0)

#define QMI_CCI_OS_LOG_RX_EXT(header, clnt) \
  do { \
    uint8_t cntl_flag; \
    uint16_t txn_id, msg_id, msg_len; \
    decode_header(header, &cntl_flag, &txn_id, &msg_id, &msg_len); \
    if(clnt) {  \
      uint32_t *addr = (uint32_t *)clnt->server_addr;  \
      SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_RX_EXT, cntl_flag << 16 | txn_id, msg_id << 16 | msg_len, clnt->service_obj->service_id, addr[0], addr[1], addr[2]); \
    } else {  \
      SMEM_LOG_EVENT(QMI_CCI_LOG_EVENT_RX, cntl_flag << 16 | txn_id, msg_id << 16 | msg_len, 0); \
    } \
  } while(0)

#else
#define QMI_CCI_OS_LOG_TX(header) 
#define QMI_CCI_OS_LOG_RX(header) 

/* Sensors does not have ASSERT defined */
#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(cond)

#endif

#define QMI_CCI_OS_LOG_ERROR() qcci_log_error(__FILE__, __LINE__)

size_t strlcpy(char *dst, const char *src, size_t siz);
static __inline void qcci_log_error(char *filename, unsigned int line)
{
  uint32 name[5];
  char *last;
  last = strrchr(filename, '/');
  if(!last)
    last = strrchr(filename, '\\');
  last = last ? (last+1) : filename;
  strlcpy((char *)name, last, sizeof(name));
#ifdef FEATURE_QMI_SMEM_LOG
  SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
#endif
#ifdef FEATURE_QMI_MSG
  MSG_ERROR("Runtime error. File 0x%s, Line: %d", filename, line, 0);
#endif
}

#endif
