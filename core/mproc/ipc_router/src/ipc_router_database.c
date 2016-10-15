/*===========================================================================

                    I P C    R O U T E R    D A T A B A S E

   This file provides routing data managment interface.

 Copyright (c) 2007-2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_database.c#2 $ $DateTime: 2013/07/11 18:00:48 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/07   sa      Initial version

===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_database.h"
#include "ipc_router_log.h"

/*===========================================================================
                CONSTANT / MACRO DACLARATIONS
===========================================================================*/
#ifdef __CC_ARM
#pragma diag_suppress 1293
#endif

#define IPC_ROUTER_PROC_TABLE_SIZE         4
#define IPC_ROUTER_SERVER_TABLE_SIZE       32
#define IPC_ROUTER_CLIENT_TABLE_SIZE       32


/* Quota for the number of messages that can be 
   received from a single processor. A confirmation
   control message will have to be received by the
   sending processor before it resumes sending messages
*/
#define IPC_ROUTER_DEFAULT_RX_QUOTA 5

#define CALLOC_BAIL(ptr, num, size, err_label) \
        if(NULL == ((ptr) = ipc_router_os_calloc((num), (size)))) goto err_label;
#define MALLOC_BAIL(ptr, size, err_label) \
        if(NULL == ((ptr) = ipc_router_os_malloc((size)))) goto err_label;

#define LOG_MEM_ERROR() \
  IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR, "%s:%d malloc() error", __FUNCTION__, __LINE__, 0)

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
typedef struct server_record
{
  ipc_router_service_type       service;
  ipc_router_address_type       server;
  ipc_router_xport_type         *xport;
  LINK(struct server_record, link);
} ipc_router_server_record_type;

typedef struct client_record
{
  uint32                        port_id;
  ipc_router_client_type        *client;
  int                           ref_count;
  LINK(struct client_record, link);
} ipc_router_client_record_type;

typedef struct xport_record
{
  ipc_router_xport_type         *xport;
  LINK(struct xport_record, link);
} ipc_router_xport_record_type;

typedef LIST(ipc_router_client_record_type, ipc_router_client_table_type)[IPC_ROUTER_CLIENT_TABLE_SIZE];

typedef struct proc_record
{
  uint32                        processor_id;
  ipc_router_xport_type         *xport;
  ipc_router_client_table_type  clients;
  LINK(struct proc_record, link);
} ipc_router_proc_record_type;

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
extern uint32 ipc_router_local_processor_id;

static LIST(ipc_router_proc_record_type,   g_client_tbls)[IPC_ROUTER_PROC_TABLE_SIZE];
static LIST(ipc_router_server_record_type, g_server_tbl)[IPC_ROUTER_SERVER_TABLE_SIZE];
static LIST(ipc_router_xport_record_type,  g_xport_list);

static ipc_router_os_sem client_table_semaphore;
static ipc_router_os_sem server_table_semaphore;
static ipc_router_os_sem xport_table_semaphore;

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/
static ipc_router_client_record_type *ipc_router_remove_client
(
 ipc_router_client_table_type *client_table,
 uint32 port_id
 );

static void ipc_router_cleanup_client(ipc_router_client_record_type *i);

/*===========================================================================
FUNCTION      find_slot

DESCRIPTION   Hash function for inserting/retrieving from a hash table. 
              table size must be a power of 2

RETURN VALUE  a hash used to index into a hash table.

SIDE EFFECTS  None
===========================================================================*/
static uint32 find_slot
(
 uint32    table_size,
 uint32    key
)
{
  return(key & (table_size-1));
}
/*===========================================================================
FUNCTION      default_compare

DESCRIPTION   Default instance compare routine if the user does not provide
              one.

RETURN VALUE  Returns TRUE or FALSE based on inst1 and inst2

SIDE EFFECTS  None
===========================================================================*/
static boolean default_compare(uint32 inst1, uint32 inst2, void *compare_data)
{
  return inst1 == inst2 ? TRUE : FALSE;
}

/*===========================================================================
FUNCTION      get_client_table

DESCRIPTION   Return cilent table given a processor ID

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_table_type *get_client_table(uint32 processor_id)
{
  ipc_router_proc_record_type *proc; 
  LIST_FIND(g_client_tbls[find_slot(IPC_ROUTER_PROC_TABLE_SIZE, processor_id)],
            proc, link, proc->processor_id == processor_id);
  return proc ? &proc->clients : NULL;
}

/*===========================================================================
FUNCTION      add_client_table

DESCRIPTION   Add a new client table for processor and return pointer to it

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_table_type *add_client_table
(
 uint32 processor_id,
 ipc_router_xport_type *xport
 )
{
  ipc_router_proc_record_type *proc;
  proc = ipc_router_os_calloc(1, sizeof(ipc_router_proc_record_type));
  if(!proc)
    return NULL;

  proc->processor_id = processor_id;  
  proc->xport = xport;
  LIST_ADD(g_client_tbls[find_slot(IPC_ROUTER_PROC_TABLE_SIZE, processor_id)], proc, link);

  return &proc->clients;
}

/*===========================================================================
FUNCTION      remove_proc_record

DESCRIPTION   Remove processor record

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static int remove_proc_record(uint32 processor_id)
{
  int rc = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_proc_record_type *proc;

  LIST_FIND(g_client_tbls[find_slot(IPC_ROUTER_PROC_TABLE_SIZE, processor_id)],
            proc, link, proc->processor_id == processor_id);
  if(proc)
  {
    int j;
    for(j = 0; j < IPC_ROUTER_CLIENT_TABLE_SIZE; j++)
    {
      while(proc->clients[j].head)
      {
        ipc_router_client_record_type *record = 
           ipc_router_remove_client(&proc->clients, 
                    proc->clients[j].head->port_id);
        if(record && record->ref_count < 0)
          ipc_router_cleanup_client(record);
      }
    }

    LIST_REMOVE(g_client_tbls[find_slot(IPC_ROUTER_PROC_TABLE_SIZE, processor_id)], proc, link);

    ipc_router_os_free(proc);
    rc = IPC_ROUTER_STATUS_SUCCESS;
  }
  return rc;
}

/*===========================================================================
FUNCTION      proc_table_deinit

DESCRIPTION   Deinit processor table and free memory

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void proc_table_deinit(void)
{
  int i, j;
  ipc_router_proc_record_type *proc;
  for(i = 0; i < IPC_ROUTER_PROC_TABLE_SIZE; i++)
  {
    while(NULL != (proc = LIST_HEAD(g_client_tbls[i])))
    {
      for(j = 0; j < IPC_ROUTER_CLIENT_TABLE_SIZE; j++)
      {
        while(NULL != LIST_HEAD(proc->clients[j]))
        {
          ipc_router_client_record_type *record =
            ipc_router_remove_client(&proc->clients, 
                proc->clients[j].head->port_id);
          if(record && record->ref_count < 0)
            ipc_router_cleanup_client(record);
        }
      }
      LIST_REMOVE(g_client_tbls[i], proc, link);
      ipc_router_os_free(proc);
    }
  }
}

/*===========================================================================
FUNCTION      create_server_record

DESCRIPTION   creates a server struct and initializes it

ARGUMENTS     

RETURN VALUE  1 on success

SIDE EFFECTS  None
===========================================================================*/
static int create_server_record
(
   ipc_router_server_record_type **server_record,
   ipc_router_address_type *addr,
   ipc_router_service_type *service,
   ipc_router_xport_type *xport
)
{

  if(!server_record){
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  CALLOC_BAIL(*server_record, 1, sizeof(ipc_router_server_record_type),
              create_server_err0);

  (*server_record)->xport = xport;

  (*server_record)->server = *addr;
  (*server_record)->service = *service;

  return IPC_ROUTER_STATUS_SUCCESS;

create_server_err0:
    LOG_MEM_ERROR();
    return IPC_ROUTER_STATUS_NO_MEM;
}

/*===========================================================================
FUNCTION      ipc_router_put_server

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_put_server
(
 ipc_router_address_type *addr,
 ipc_router_service_type *service,
 ipc_router_xport_type *xport
 )
{
  uint32 index = 0;
  ipc_router_server_record_type *i = NULL;
  ipc_router_server_record_type *new;

  index = find_slot(IPC_ROUTER_SERVER_TABLE_SIZE, service->type);
  LIST_FIND(g_server_tbl[index], i, link,
            i->service.type == service->type &&
            i->service.instance == service->instance &&
            i->server.processor_id == addr->processor_id &&
            i->server.port_id == addr->port_id);
  if(i)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO,
          "ipc_router_database: Dupe server reg from same addr. Prog: %x, Ver: %x, Proc: %x.\r\n",
          i->service.type,
          i->service.instance,
          addr->processor_id);
    return IPC_ROUTER_STATUS_FAILURE;
  }

  if(create_server_record(&new, addr, service, xport) != IPC_ROUTER_STATUS_SUCCESS)
  {
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  LIST_FIND(g_server_tbl[index], i, link,
                 i->service.type == service->type);

  if(i)
  {
    if(!i->xport)
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO, 
          "Prioritizing dup server below already registered server: type:%x, instance: %x\n",
          i->service.type, i->service.instance, 0);
      LIST_ADD(g_server_tbl[index], new, link);
    }
    else
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_INFO, 
          "Prioritizing dup server above already registered server: type:%x, instance: %x\n",
          i->service.type, i->service.instance, 0);
      LIST_PREPEND(g_server_tbl[index], new, link);
    }
  }
  else
  {
    LIST_ADD(g_server_tbl[index], new, link);
  }

  return IPC_ROUTER_STATUS_SUCCESS;

}

/*===========================================================================
FUNCTION      ipc_router_get_server

DESCRIPTION   Finds the destination of a server if known to the  router.
              Uses the program number and the major component of the version
              to match the requested server. 

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_get_server
(
 ipc_router_service_type         *service,
 ipc_router_address_type         *addr,
 ipc_router_instance_compare_type compare,
 void                            *compare_data
 )
{ 
  uint32                     index = 0;
  ipc_router_server_record_type *i = NULL;

  if (!addr || !service){
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  if(!compare)
  {
    compare = default_compare;
  }

  index = find_slot(IPC_ROUTER_SERVER_TABLE_SIZE, service->type);

  LIST_FIND(g_server_tbl[index], i, link,
            i->service.type == service->type &&
            compare(service->instance, i->service.instance, compare_data));
  if(i)
  {
     *addr  = i->server;
     return IPC_ROUTER_STATUS_SUCCESS;
  }

  /* Server not found - clients need to listen for name service events */
  return IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      ipc_router_remove_server

DESCRIPTION   remove server with type and address

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static int  ipc_router_remove_server
(
 ipc_router_service_type *service,
 ipc_router_address_type *addr
 )
{ 
  uint32 index = 0;
  ipc_router_server_record_type *i = NULL;

  index = find_slot(IPC_ROUTER_SERVER_TABLE_SIZE, service->type);

  LIST_FIND(g_server_tbl[index], i, link,
            i->service.type == service->type && 
            i->service.instance == service->instance &&
            i->server.processor_id == addr->processor_id &&
            i->server.port_id == addr->port_id);
  if(i)
  {
    LIST_REMOVE(g_server_tbl[index], i, link);
    ipc_router_os_free(i);
    return IPC_ROUTER_STATUS_SUCCESS;
  }
  return IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      create_client

DESCRIPTION   creates a client struct and initializes it

ARGUMENTS     port_id - id of the client

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success

SIDE EFFECTS  None
===========================================================================*/
static int create_client
(
 ipc_router_client_type **pclient,
 uint32                   port_id
)
{
  if(!pclient)
    return IPC_ROUTER_STATUS_FAILURE;

  CALLOC_BAIL(*pclient, 1, sizeof(ipc_router_client_type), create_client_err0);

  (*pclient)->port_id = port_id;

  return IPC_ROUTER_STATUS_SUCCESS;   

create_client_err0:
  LOG_MEM_ERROR();
  *pclient = NULL;
  return IPC_ROUTER_STATUS_NO_MEM;
}

/*===========================================================================
FUNCTION      delete_client

DESCRIPTION   deletes a client struct and deinitializes it

ARGUMENTS     pclient - a pointer to a pointer to the client struct

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
static void delete_client
(
 ipc_router_client_type **pclient
 )
{
  ipc_router_os_sem_deinit(&(*pclient)->tx_cb_q_sem);
  ipc_router_os_sem_deinit(&(*pclient)->mode_sem);
  ipc_router_os_free(*pclient);

  /* Invalidate the client's handle */
  *pclient = NULL;
}

/*===========================================================================
FUNCTION      ipc_router_put_client

DESCRIPTION   finds the destination of a server if known to the router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_put_client
(
 ipc_router_client_table_type   *client_table,
 uint32                          port_id,
 ipc_router_xport_type          *xport,
 ipc_router_client_type         *client
 )
{  
  uint32 index = 0;
  ipc_router_client_record_type *i;

  index = find_slot(IPC_ROUTER_CLIENT_TABLE_SIZE, port_id);
  LIST_FIND((*client_table)[index], i, link, (uint32)i->port_id == port_id);
  if(i)
  {
    return IPC_ROUTER_STATUS_BUSY;
  }

  i = ipc_router_os_calloc(1, sizeof(ipc_router_client_record_type));
  if(!i)
  {
    LOG_MEM_ERROR();
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  client->prv = i;
  i->client = client;
  i->port_id = port_id;
  LIST_ADD((*client_table)[index], i, link);

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_get_client

DESCRIPTION   finds the destination of a server if known to the router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_record_type *ipc_router_get_client
(
 ipc_router_client_table_type   *client_table,
 uint32                          port_id
 )
{
  uint32 index = 0;
  ipc_router_client_record_type *i = NULL;

  index = find_slot(IPC_ROUTER_CLIENT_TABLE_SIZE, port_id);
  LIST_FIND((*client_table)[index], i, link, i->port_id == port_id);
  return i;
}

/*===========================================================================
FUNCTION      ipc_router_notify_tx_resume

DESCRIPTION   Notify clients that are waiting for a resume Tx event

RETURN VALUE  None

SIDE EFFECTS  Waiting clients will be notified with client lock held
===========================================================================*/
static void ipc_router_notify_tx_resume
(
 ipc_router_client_type *client,
 ipc_router_address_type *addr
)
{
  ipc_router_event_info_type info;
  ipc_router_cb_q_type *node;

  /* resume transmission */
  ipc_router_os_sem_lock(&client->tx_cb_q_sem);

  client->tx_count = 0;
  client->empty_bucket_count=0;

  node = LIST_HEAD(client->tx_cb_q);
  LIST_INIT(client->tx_cb_q);

  ipc_router_os_sem_unlock(&client->tx_cb_q_sem);

  /* notify client of resume tx event and free the node */
  info.client = *addr;
  while(node)
  {
    ipc_router_cb_q_type *to_delete = node;
    ipc_router_client_type *sender = ipc_router_find_client(&node->sender);;

    if(sender)
    {
      if(sender->event_cb)
      {
        sender->event_cb(
            sender, 
            sender->event_cb_data,
            IPC_ROUTER_EVENT_RESUME_TX,
            &info
            );
      }
      /* unlock sender's client record */
      ipc_router_unlock_client(sender);
    }
    node = node->link.next;
    ipc_router_os_free(to_delete);
  }
}

/*===========================================================================
FUNCTION      ipc_router_dec_ref_count

DESCRIPTION   Decrease the reference count of a client record.

              Client table must be locked

RETURN VALUE  Reference count

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_dec_ref_count(ipc_router_client_record_type *i)
{
  i->ref_count--;
  return i->ref_count;
}

/*===========================================================================
FUNCTION      ipc_router_cleanup_client

DESCRIPTION   Clean up when client reference count reaches 0.

              Client table needs not be locked

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_cleanup_client(ipc_router_client_record_type *i)
{
  ipc_router_address_type addr;
  ipc_router_event_type event = IPC_ROUTER_EVENT_CLOSED;
  ipc_router_event_info_type info;

  /* wake up clients waiting to write */
  if(i->client->xport)
    addr.processor_id = i->client->xport->remote_router.processor_id;
  else
    addr.processor_id = ipc_router_local_processor_id;

  addr.port_id = i->client->port_id;
  ipc_router_notify_tx_resume(i->client, &addr);

  /* call client's event callback with the IPC_ROUTER_EVENT_CLOSED event */
  if(i->client->event_cb)
  {
    info.client.processor_id = ipc_router_local_processor_id;
    info.client.port_id = i->client->port_id;
    i->client->event_cb(i->client, i->client->event_cb_data, event, &info);
  }
  delete_client(&i->client);
  ipc_router_os_free(i);
}

/*===========================================================================
FUNCTION      ipc_router_unlock_client

DESCRIPTION   Decrease the reference count of a client record. Clean up
              when it reaches 0.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_unlock_client(ipc_router_client_type *client)
{
  int ref_count;
  ipc_router_os_sem_lock(&client_table_semaphore);
  ref_count = ipc_router_dec_ref_count((ipc_router_client_record_type *)client->prv);
  ipc_router_os_sem_unlock(&client_table_semaphore);
  if(ref_count < 0)
    ipc_router_cleanup_client((ipc_router_client_record_type *)client->prv);
}

/*===========================================================================
FUNCTION      ipc_router_remove_client

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_record_type *ipc_router_remove_client
(
 ipc_router_client_table_type   *client_table,
 uint32                          port_id
 )
{ 
  uint32 index = 0;
  ipc_router_client_record_type *i = NULL;

  index = find_slot(IPC_ROUTER_CLIENT_TABLE_SIZE, port_id);
  LIST_FIND((*client_table)[index], i, link, i->port_id == port_id);
  if(i)
  {
    LIST_REMOVE((*client_table)[index], i, link);
    ipc_router_dec_ref_count(i);
  }
  return i;
}

/*===========================================================================
FUNCTION      ipc_router_event_notify

DESCRIPTION   Notify clients on the local processor of events (new server,
              remove server, resume tx). This is a broadcast too all clients.

RETURN VALUE  Invoke event callback for each client.

SIDE EFFECTS  Client record is locked during callbacks
===========================================================================*/
static void ipc_router_event_notify
(
 ipc_router_event_type event,
 ipc_router_event_info_type *info
 )
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_client_list_type *client_list;

  /* We must not be holding the client table lock when making callbacks to
   * prevent deadlocks - get a list of clients and access them individually
   */

  cr.crit = IPC_ROUTER_CRITERIA_PROCESSOR;
  cr.arguments.proc_arg = ipc_router_local_processor_id;
  ipc_router_list_clients(&cr, &results);

  while(NULL != (client_list = LIST_HEAD(results.clients)))
  {
    ipc_router_client_type *client;
    
    client = ipc_router_find_client(&client_list->addr);

    if(client)
    {
      if(client->event_cb)
      {
        client->event_cb(client, client->event_cb_data, event, info);
      }
      ipc_router_unlock_client(client);
    }
    LIST_REMOVE(results.clients, client_list, link);
    ipc_router_os_free(client_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_database_init

DESCRIPTION   Initialize router database.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_database_init(void)
{  
  ipc_router_os_sem_init(&client_table_semaphore);
  ipc_router_os_sem_init(&server_table_semaphore);
  ipc_router_os_sem_init(&xport_table_semaphore);

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_database_deinit

DESCRIPTION   Deinitialize router database.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_database_deinit
(
)
{
  int i;
  ipc_router_server_record_type *server_to_delete = NULL;

  /* cleanup proc table */
  ipc_router_os_sem_lock(&client_table_semaphore);
  proc_table_deinit();
  ipc_router_os_sem_unlock(&client_table_semaphore);

  /* cleanup servers table */
  ipc_router_os_sem_lock(&server_table_semaphore);
  for(i = 0 ; i < IPC_ROUTER_SERVER_TABLE_SIZE ; i++)
  {
    while((server_to_delete = LIST_HEAD(g_server_tbl[i])))
    {
      ipc_router_remove_server(&server_to_delete->service, 
          &server_to_delete->server);
    }
  }
  ipc_router_os_sem_unlock(&server_table_semaphore);

  /* Cleanup xports table */
  while(LIST_HEAD(g_xport_list))
  {
    ipc_router_unregister_xport(LIST_HEAD(g_xport_list)->xport);
  }

  /* cleanup global semaphores */
  ipc_router_os_sem_deinit(&client_table_semaphore);
  ipc_router_os_sem_deinit(&server_table_semaphore);
  ipc_router_os_sem_deinit(&xport_table_semaphore);

  return IPC_ROUTER_STATUS_SUCCESS;;
}

/*===========================================================================
FUNCTION      ipc_router_register_client

DESCRIPTION   Initialize router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_register_client_ext
(
 ipc_router_address_type       *addr,
 ipc_router_xport_type         *xport,
 ipc_router_client_type       **client,
 int                           rx_quota,
 ipc_router_rx_cb_type         rx_cb,
 void                         *rx_cb_data,
 ipc_router_event_cb_type      event_cb,
 void                         *event_cb_data,
 uint32                        priority,
 uint64                        xport_scope,
 ipc_router_client_mode        client_mode
 )
{
  int ret = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_client_type *new_client;
  ipc_router_client_table_type *tbl;
  ipc_router_client_record_type *record;

  ipc_router_os_sem_lock(&client_table_semaphore);

  tbl = get_client_table(addr->processor_id);

  if(tbl && (NULL != (record = ipc_router_get_client(tbl, addr->port_id))))
  {
    /* Client record already exists */
    ret = IPC_ROUTER_STATUS_BUSY;
    if(xport && record->client->xport && 
        xport->priority > record->client->xport->priority)
    {
      /* update xport */
      record->client->xport = xport;
      /* Adjust client's scope to be the lowest 
       * common denominator */
      record->client->scope &= xport_scope;
      ret = IPC_ROUTER_STATUS_SUCCESS;
    }
    ipc_router_os_sem_unlock(&client_table_semaphore);
    return ret;
  }

  ret = create_client(&new_client, addr->port_id);
  if(IPC_ROUTER_STATUS_SUCCESS != ret){
    goto register_client_unlock_bail;
  }

  if(client)
    *client = new_client;

  new_client->rx_cb = rx_cb;
  new_client->rx_cb_data = rx_cb_data;
  new_client->event_cb = event_cb;
  new_client->event_cb_data = event_cb_data;
  new_client->xport = xport;
  new_client->rx_quota = rx_quota ? rx_quota : IPC_ROUTER_DEFAULT_RX_QUOTA;  
  new_client->priority = priority;
  new_client->scope = xport_scope;
  new_client->mode = client_mode;
  new_client->connected_xport = NULL;
  ipc_router_os_sem_init(&new_client->tx_cb_q_sem);
  ipc_router_os_sem_init(&new_client->mode_sem);

  /* if this is the first new_client to register on that processor, 
     allocate a new_client table for it*/
  if(!tbl)
  {
    tbl = add_client_table(addr->processor_id, xport);
    if(!tbl)
    {
      LOG_MEM_ERROR();
      ret = IPC_ROUTER_STATUS_NO_MEM;
      delete_client(&new_client);
      goto register_client_unlock_bail;
    }
  }

  ret = ipc_router_put_client(tbl, addr->port_id, xport, new_client);

  if(ret != IPC_ROUTER_STATUS_SUCCESS)
  {
    delete_client(&new_client);
    goto register_client_unlock_bail;
  }

register_client_unlock_bail:
  ipc_router_os_sem_unlock(&client_table_semaphore);

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_register_server

DESCRIPTION   Register a server with the router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients will be notified of the event
===========================================================================*/
int ipc_router_register_server
(   
 ipc_router_service_type *service,
 ipc_router_address_type *addr,
 ipc_router_xport_type   *xport
 )
{
  int ret = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_event_info_type info;

  if(!service || !addr){
    ret = IPC_ROUTER_STATUS_INVALID_PARAM;
    goto register_server_bail;
  }

  ipc_router_os_sem_lock(&server_table_semaphore);

  ret = ipc_router_put_server(addr, service, xport);

  ipc_router_os_sem_unlock(&server_table_semaphore);

  if(ret == IPC_ROUTER_STATUS_SUCCESS)
  {
    info.server.service = *service;
    info.server.addr = *addr;

    ipc_router_event_notify(
        IPC_ROUTER_EVENT_NEW_SERVER,
        &info
        );
  }

register_server_bail:
  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_unregister_client

DESCRIPTION   Initialize router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_client
(
 ipc_router_address_type *addr
 )
{
  ipc_router_event_info_type info;
  ipc_router_client_table_type *tbl;
  ipc_router_client_record_type *record = NULL;
  int ref_count = 0;

  info.client = *addr;

  ipc_router_os_sem_lock(&client_table_semaphore);

  tbl = get_client_table(addr->processor_id);
  if(tbl)
  {
    record = ipc_router_remove_client(tbl, addr->port_id);

    /* save ref_count before unlocking table */
    if(record)
      ref_count = record->ref_count;
  }

  ipc_router_os_sem_unlock(&client_table_semaphore);

  /* clean up client with client_table_semaphore unlocked because it invokes
   * client's event callback
   */
  if(ref_count < 0)
    ipc_router_cleanup_client(record);

  /* Notify everyone of the event */
  ipc_router_event_notify(IPC_ROUTER_EVENT_REMOVE_CLIENT, &info);

  return record ? IPC_ROUTER_STATUS_SUCCESS : IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      ipc_router_unregister_server

DESCRIPTION   Register a server with the router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_server
(   
 ipc_router_service_type *service,
 ipc_router_address_type *addr
 )
{
  ipc_router_event_info_type info;

  info.server.service = *service;
  info.server.addr = *addr;

  ipc_router_os_sem_lock(&server_table_semaphore);

  if(ipc_router_remove_server(service, addr) != 
      IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_os_sem_unlock(&server_table_semaphore);
    return IPC_ROUTER_STATUS_FAILURE;
  }

  ipc_router_os_sem_unlock(&server_table_semaphore);

  ipc_router_event_notify(
      IPC_ROUTER_EVENT_REMOVE_SERVER,
      &info
      );

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_find_server

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_find_server
(   
 ipc_router_service_type *service,
 ipc_router_address_type *addr,
 ipc_router_instance_compare_type compare,
 void *compare_data
 )
{
  int ret = IPC_ROUTER_STATUS_FAILURE;

  ipc_router_os_sem_lock(&server_table_semaphore);
  ret = ipc_router_get_server(service, addr, compare, compare_data);
  ipc_router_os_sem_unlock(&server_table_semaphore);

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_find_client

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Record of the client is locked
===========================================================================*/
ipc_router_client_type *ipc_router_find_client
(   
 ipc_router_address_type *addr
 )
{
  ipc_router_client_record_type *temp = NULL;
  ipc_router_client_table_type *tbl;

  ipc_router_os_sem_lock(&client_table_semaphore);

  tbl = get_client_table(addr->processor_id);
  if(tbl)
    temp = ipc_router_get_client(tbl, addr->port_id);

  /* Increase reference count so we don't nuke it */
  if(temp)
    temp->ref_count++;

  ipc_router_os_sem_unlock(&client_table_semaphore);

  return( temp ? temp->client : NULL );
}

/*===========================================================================
FUNCTION      ipc_router_register_xport

DESCRIPTION   Registers a transport.  

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_register_xport
(
 ipc_router_xport_type *xport
 )
{
  ipc_router_xport_record_type *xport_record;

  CALLOC_BAIL(xport_record, 1, sizeof(ipc_router_xport_record_type),
      register_xport_err0);

  xport_record->xport = xport;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  LIST_ADD(g_xport_list, xport_record, link);
  ipc_router_os_sem_unlock(&xport_table_semaphore);

  return IPC_ROUTER_STATUS_SUCCESS;

register_xport_err0:
  LOG_MEM_ERROR();
  return IPC_ROUTER_STATUS_NO_MEM;
}

/*===========================================================================
FUNCTION      ipc_router_unregister_xport

DESCRIPTION   Unregister the xport from the database but not free the xport

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_xport
(
 ipc_router_xport_type *xport
 )
{
  ipc_router_xport_record_type *i;
  int rc = IPC_ROUTER_STATUS_INVALID_PARAM;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  LIST_FIND(g_xport_list, i, link, i->xport == xport);
  if(i)
  {
    LIST_REMOVE(g_xport_list, i, link);
    ipc_router_os_free(i);
    rc = IPC_ROUTER_STATUS_SUCCESS;
  }
  ipc_router_os_sem_unlock(&xport_table_semaphore);
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_xport_dec_ref

DESCRIPTION   Unregisters a transport and free the xport data structure when
              the reference count goes to 0.  

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_xport_dec_ref
(
 ipc_router_xport_type *xport
 )
{
  if(xport->ref_count)
    xport->ref_count--;
  else
    ipc_router_os_free(xport);
}

/*===========================================================================
FUNCTION      ipc_router_free_xport

DESCRIPTION   Unregisters a transport and free the xport data structure when
              the reference count goes to 0.  

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_free_xport
(
 ipc_router_xport_type *xport
 )
{
  if(!xport)
    return;

  ipc_router_unregister_xport(xport);

  ipc_router_os_sem_lock(&xport_table_semaphore);
  ipc_router_xport_dec_ref(xport);
  ipc_router_os_sem_unlock(&xport_table_semaphore);
}

/*===========================================================================
FUNCTION      ipc_router_find_xport

DESCRIPTION   finds the destination xport of a given processor

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
ipc_router_xport_type *ipc_router_find_xport
(   
 uint32 processor_id
)
{
  ipc_router_xport_type *xport = NULL;
  ipc_router_proc_record_type *proc; 
  uint32 index = find_slot(IPC_ROUTER_PROC_TABLE_SIZE, processor_id);

  ipc_router_os_sem_lock(&client_table_semaphore);
  LIST_FIND(g_client_tbls[index], proc, link, proc->processor_id == processor_id);
  if(proc)
  {
    xport = proc->xport;
  }
  ipc_router_os_sem_unlock(&client_table_semaphore);

  return xport;
}

/*===========================================================================
FUNCTION      ipc_router_lock_xport

DESCRIPTION   Verify a xport handle and lock it 

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_lock_xport
(
 ipc_router_xport_type *xport
)
{
  ipc_router_xport_record_type * i;
  int rc = IPC_ROUTER_STATUS_INVALID_PARAM;

  if(!xport)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  LIST_FIND(g_xport_list, i, link, i->xport == xport);
  if(i)
  {
    rc = IPC_ROUTER_STATUS_SUCCESS;
    xport->ref_count++;
  }
  ipc_router_os_sem_unlock(&xport_table_semaphore);
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_unlock_xport

DESCRIPTION   Unlock a xport structure. xport must be previously locked.

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_unlock_xport
(
 ipc_router_xport_type *xport
)
{
  if(!xport)
    return;

  ipc_router_os_sem_lock(&xport_table_semaphore);
  ipc_router_xport_dec_ref(xport);
  ipc_router_os_sem_unlock(&xport_table_semaphore);
}

/*===========================================================================
FUNCTION      ipc_router_unregister_proc

DESCRIPTION   Unregister the processor from the database and free all memory

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_unregister_proc
(
 uint32 processor_id
 )
{
  int rc;
  ipc_router_os_sem_lock(&client_table_semaphore);
  rc = remove_proc_record(processor_id);
  ipc_router_os_sem_unlock(&client_table_semaphore);
  return rc;
}

/*===========================================================================
FUNCTION      create_client_list_node

DESCRIPTION   allocate and initialize client list node

RETURN VALUE  pointer to new client list node

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_client_list_type* create_client_list_node
(
 uint32 processor_id,
 uint32 port_id
)
{
  ipc_router_client_list_type *node;
  node = ipc_router_os_calloc(1, sizeof(ipc_router_client_list_type));
  if(node)
  {
    node->addr.processor_id = processor_id;
    node->addr.port_id = port_id;
  }
  return node;
}

/*===========================================================================
FUNCTION      ipc_router_list_clients

DESCRIPTION     

RETURN VALUE  returns a link list of client addresses matching the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_clients
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  uint32 j,k;
  ipc_router_client_record_type *i = NULL;
  ipc_router_client_list_type *node = NULL;
  ipc_router_client_table_type *tbl;
  ipc_router_proc_record_type *proc;

  LIST_INIT(results->clients);

  ipc_router_os_sem_lock(&client_table_semaphore);

  switch(cr->crit)
  {
    case IPC_ROUTER_CRITERIA_ADDRESS:
      tbl = get_client_table(cr->arguments.addr_arg->processor_id);
      if(tbl)
      {
        i = ipc_router_get_client(tbl, cr->arguments.addr_arg->port_id);
        if(i)
        {
          node = create_client_list_node(cr->arguments.addr_arg->processor_id,
              cr->arguments.addr_arg->port_id);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_clients_bail;
          }
        }
      }
      LIST_ADD(results->clients, node, link);

      break;

    case IPC_ROUTER_CRITERIA_PROCESSOR:
      tbl = get_client_table(cr->arguments.proc_arg);
      if(!tbl)
      {
        goto list_clients_bail;
      }

      for(k = 0 ; k < IPC_ROUTER_CLIENT_TABLE_SIZE ; k++)
      {
        i = (*tbl)[k].head;
        while(i)
        {
          node = create_client_list_node(cr->arguments.proc_arg, 
              i->port_id);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_clients_bail;
          }
          LIST_ADD(results->clients, node, link);
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_XPORT:
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          for(k = 0 ; k < IPC_ROUTER_CLIENT_TABLE_SIZE ; k++)
          {
            i = proc->clients[k].head;
            while(i)
            {
              if(i->client->xport == cr->arguments.xport_arg)
              {
                node = create_client_list_node(proc->processor_id, i->port_id);
                if(!node)
                {
                  LOG_MEM_ERROR();
                  goto list_clients_bail;
                }
                LIST_ADD(results->clients, node, link);
              }
              i = i->link.next;
            }
          }
          proc = proc->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_ALL:
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE ; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          for(k = 0 ; k < IPC_ROUTER_CLIENT_TABLE_SIZE ; k++)
          {
            i = proc->clients[k].head;
            while(i)
            {
              node = create_client_list_node(proc->processor_id, i->port_id);
              
              if(!node)
              {
                LOG_MEM_ERROR();
                goto list_clients_bail;
              }
              LIST_ADD(results->clients, node, link);
              i = i->link.next;
            }
          }
          proc = proc->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_INVALID:   /* Fall through */
    default:
      break;
  }

list_clients_bail:
  ipc_router_os_sem_unlock(&client_table_semaphore);
}

/*===========================================================================
FUNCTION      create_server_list_node

DESCRIPTION   allocate and initialize server list node

RETURN VALUE  pointer to new server list node

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_server_list_type *create_server_list_node
(
 ipc_router_address_type *addr,
 ipc_router_service_type *service
)
{
  ipc_router_server_list_type *node;
  node = ipc_router_os_calloc(1, sizeof(ipc_router_server_list_type));
  if(node)
  {
    node->addr = *addr;
    node->service = *service;
  }
  return node;
}

/*===========================================================================
FUNCTION      ipc_router_list_servers

DESCRIPTION     

RETURN VALUE  returns a link list of server's program,version structs matching
              the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_servers
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  uint32                            k;
  ipc_router_server_record_type    *i            = NULL;
  ipc_router_server_list_type      *node         = NULL;
  ipc_router_instance_compare_type compare       = NULL;
  void                             *compare_data = NULL;

  LIST_INIT(results->servers);

  ipc_router_os_sem_lock(&server_table_semaphore);
  switch(cr->crit)
  {
    case IPC_ROUTER_CRITERIA_PROCESSOR:

      for(k = 0 ; k < IPC_ROUTER_SERVER_TABLE_SIZE ; k++)
      {
        i = g_server_tbl[k].head;
        while(i)
        {
          if(i->server.processor_id == cr->arguments.proc_arg)
          {
            node = create_server_list_node(&i->server, &i->service);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_servers_bail;
            }
            LIST_ADD(results->servers, node, link);
          }
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_ADDRESS:
      for(k = 0 ; k < IPC_ROUTER_SERVER_TABLE_SIZE ; k++)
      {
        i = g_server_tbl[k].head;
        while(i)
        {
          if((i->server.processor_id == cr->arguments.addr_arg->processor_id) &&
              (i->server.port_id == cr->arguments.addr_arg->port_id))
          {
            node = create_server_list_node(&i->server, &i->service);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_servers_bail;
            }
            LIST_ADD(results->servers, node, link);
          }
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_SERVICE:
      compare      = cr->arguments.svc_arg.compare;
      compare_data = cr->arguments.svc_arg.compare_data;
      if(!compare)
      {
        compare = default_compare;
      }
      i = LIST_HEAD(g_server_tbl[find_slot(IPC_ROUTER_SERVER_TABLE_SIZE, cr->arguments.svc_arg.service->type)]);
      while(i)
      {
        if(i->service.type == cr->arguments.svc_arg.service->type &&
            compare(cr->arguments.svc_arg.service->instance, i->service.instance,
              compare_data))
        {
          node = create_server_list_node(&i->server, &i->service);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_servers_bail;
          }
          LIST_ADD(results->servers, node, link);
        }
        i = i->link.next;
      }
      break;

    case IPC_ROUTER_CRITERIA_XPORT:
      for(k = 0 ; k < IPC_ROUTER_SERVER_TABLE_SIZE ; k++)
      {
        i = g_server_tbl[k].head;
        while(i)
        {
          /* If this is a dummy record, skip it */
          if(!(i->xport))
          {
            i = i->link.next;
            continue;
          }

          if(i->xport == cr->arguments.xport_arg) 
          {
            node = create_server_list_node(&i->server, &i->service);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_servers_bail;
            }
            LIST_ADD(results->servers, node, link);
          }
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_ALL:
      for(k = 0 ; k < IPC_ROUTER_SERVER_TABLE_SIZE ; k++)
      {
        i = g_server_tbl[k].head;
        while(i)
        {
          node = create_server_list_node(&i->server, &i->service);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_servers_bail;
          }
          LIST_ADD(results->servers, node, link);
          i = i->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_INVALID:   /* Fall through */
    default:
      break;
  }

list_servers_bail:
  ipc_router_os_sem_unlock(&server_table_semaphore);
}

/*===========================================================================
FUNCTION      ipc_router_list_xports

DESCRIPTION     

RETURN VALUE  returns a link list of xport address structs matching
              the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_xports
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  ipc_router_xport_record_type *i = NULL;
  ipc_router_xport_list_type   *node = NULL;

  LIST_INIT(results->xports);

  switch(cr->crit)
  {
    case IPC_ROUTER_CRITERIA_ALL:
      ipc_router_os_sem_lock(&xport_table_semaphore);
      i = g_xport_list.head;
      while(i)
      {
        node = ipc_router_os_calloc(1, sizeof(ipc_router_xport_list_type));
        if(!node)
        {
          LOG_MEM_ERROR();
          goto list_xports_bail;
        }
        node->xport = i->xport;
        LIST_ADD(results->xports, node, link);
        i = i->link.next;
      }
      break;

    case IPC_ROUTER_CRITERIA_PROCESSOR:   /* Fall through */
    case IPC_ROUTER_CRITERIA_ADDRESS:     /* Fall through */
    case IPC_ROUTER_CRITERIA_XPORT:       /* Fall through */
    case IPC_ROUTER_CRITERIA_INVALID:     /* Fall through */
    default:
      break;
  }
list_xports_bail:
  ipc_router_os_sem_unlock(&xport_table_semaphore);
}

/*===========================================================================
FUNCTION      create_proc_list_node

DESCRIPTION   allocate and initialize proc list node

RETURN VALUE  pointer to new proc list node

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_proc_list_type *create_proc_list_node
(
 int32 processor_id
)
{
  ipc_router_proc_list_type *node; 
  node = ipc_router_os_calloc(1, sizeof(ipc_router_proc_list_type));
  if(node)
  {
    node->processor_id = processor_id;
  }
  return node;
}

/*===========================================================================
FUNCTION      ipc_router_list_procs

DESCRIPTION     

RETURN VALUE  returns a link list of processor address structs matching
              the given criteria

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_list_procs
(
 ipc_router_search_criteria_type  *cr,
 ipc_router_search_results_type   *results
 )
{
  uint32 j;
  ipc_router_proc_record_type *proc;
  ipc_router_proc_list_type *node; 

  LIST_INIT(results->procs);

  ipc_router_os_sem_lock(&client_table_semaphore);

  switch(cr->crit){
    case IPC_ROUTER_CRITERIA_ALL:
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE ; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          node = create_proc_list_node(proc->processor_id);
          if(!node)
          {
            LOG_MEM_ERROR();
            goto list_procs_bail;
          }
          LIST_ADD(results->procs, node, link);
          proc = proc->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_PROCESSOR:   /* Fall through */
    case IPC_ROUTER_CRITERIA_ADDRESS:     /* Fall through */
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE ; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          if(proc->processor_id == cr->arguments.addr_arg->processor_id)
          {
            node = create_proc_list_node(proc->processor_id);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_procs_bail;
            }
            LIST_ADD(results->procs, node, link);
          }
          proc = proc->link.next;
        }
      }
      break;
    case IPC_ROUTER_CRITERIA_XPORT:       /* Fall through */
      for(j = 0 ; j < IPC_ROUTER_PROC_TABLE_SIZE ; j++)
      {
        proc = g_client_tbls[j].head;
        while(proc)
        {
          if(proc->xport == cr->arguments.xport_arg)
          {
            node = create_proc_list_node(proc->processor_id);
            if(!node)
            {
              LOG_MEM_ERROR();
              goto list_procs_bail;
            }
            LIST_ADD(results->procs, node, link);
          }
          proc = proc->link.next;
        }
      }
      break;

    case IPC_ROUTER_CRITERIA_INVALID:     /* Fall through */
    default:
      break;
  }

list_procs_bail:
  ipc_router_os_sem_unlock(&client_table_semaphore);
}
/*===========================================================================
FUNCTION      ipc_router_get_write_token

DESCRIPTION   Gives write permission to a given client.


ARGUMENTS     client (in)  - A pointer to client structure to which a token
                             is requested
              sender (in)  - client structure of the sender

RETURN VALUE  returns ipc_router_write_token_type.

SIDE EFFECTS  Requires exclusive access to client structure
===========================================================================*/
ipc_router_write_token_type ipc_router_get_write_token
(   
 ipc_router_client_type *client, 
 ipc_router_client_type *sender 
 )
{
  ipc_router_write_token_type ret;
  ipc_router_cb_q_type *node;

  if(!client || client->rx_quota <= 0)
  {
    return IPC_ROUTER_WRITE_TOKEN_INVALID;
  }

  ipc_router_os_sem_lock(&client->tx_cb_q_sem);

  /* Check to see if we've exceeded the client's rx quota */
  if((int)client->tx_count < client->rx_quota)
  {
    client->tx_count++;
    if((int)client->tx_count == client->rx_quota)
    {
      ret = IPC_ROUTER_WRITE_TOKEN_LAST;
    }
    else
    {
      ret = IPC_ROUTER_WRITE_TOKEN_NORMAL;
    }
  }
  else
  {
    ret = IPC_ROUTER_WRITE_TOKEN_NONE;
    client->empty_bucket_count++;

    node = ipc_router_os_calloc(1, sizeof(ipc_router_cb_q_type));
    if(!node)
    {
      LOG_MEM_ERROR();
    }
    else
    {
      /* Use sender's address since the client reference can be freed up by the
       * time we get the confirmation
       */
      node->sender.processor_id = ipc_router_local_processor_id;
      node->sender.port_id      = sender->port_id;

      /* Add client to destination client's callback queue */
      LIST_ADD(client->tx_cb_q, node, link);
    }
  }

  ipc_router_os_sem_unlock(&client->tx_cb_q_sem);
  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_resume_client_tx

DESCRIPTION   Signals waiting writers to proceed with with sending messages.

ARGUMENTS     addr (in)  - A pointer to client address for which tx
                                  to be resumed

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_FAILURE if client is not found locally

SIDE EFFECTS  Client record locked during event callback
===========================================================================*/
void ipc_router_resume_client_tx
(   
 ipc_router_client_type *client,
 ipc_router_address_type *addr
 )
{
  if(!client)
    return;

  ipc_router_notify_tx_resume(client, addr);
  return;
}
