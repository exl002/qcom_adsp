#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "qurt.h"
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"

#define QMI_SVC_WAIT_SIG 0x00010000
#define QMI_PING_SERVICE_STACK_SIZE  (2048)
qurt_anysignal_t     qmi_ping_service_sig;
qurt_thread_t        qmi_ping_service_tcb;
qurt_thread_attr_t   qmi_ping_service_attr;
char qmi_ping_service_stack[QMI_PING_SERVICE_STACK_SIZE];

extern void *qmi_ping_register_service(qmi_csi_os_params *os_params);

static void qmi_ping_service_thread(void *unused)
{
  qmi_csi_os_params os_params;
  void *sp;

  os_params.sig = QMI_SVC_WAIT_SIG;
  os_params.signal = &qmi_ping_service_sig;

  sp = qmi_ping_register_service(&os_params);

  if(!sp)
  {
#ifdef FEATURE_QMI_MSG
    ERR_FATAL("Unable to start ping service!", 0,0,0);
#endif
    return;
  }

  while(1)
  {
    qurt_anysignal_wait(os_params.signal, QMI_SVC_WAIT_SIG);
    qurt_anysignal_clear(os_params.signal, QMI_SVC_WAIT_SIG);
    qmi_csi_handle_event(sp, &os_params);
  }
}

void qmi_ping_service_start(void)
{
  qurt_anysignal_init(&qmi_ping_service_sig);
  qurt_thread_attr_init(&qmi_ping_service_attr);
  qurt_thread_attr_set_name(&qmi_ping_service_attr, "QMI_PING_SVC");
  qurt_thread_attr_set_priority(&qmi_ping_service_attr, 10);
  qurt_thread_attr_set_stack_size(&qmi_ping_service_attr, QMI_PING_SERVICE_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&qmi_ping_service_attr, qmi_ping_service_stack);
  qurt_thread_create(&qmi_ping_service_tcb, &qmi_ping_service_attr, 
      qmi_ping_service_thread, NULL);
}
