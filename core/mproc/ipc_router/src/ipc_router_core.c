/*===========================================================================

                    I P C    R O U T E R   C O R E

   This file describes the OS-independent interface to the IPC Router.
   This file must be compatible with all operating systems on which the 
   router will be used.

 Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_core.c#2 $
$DateTime: 2013/07/11 18:00:48 $
$Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_database.h"
#include "ipc_router_packet.h"
#include "ipc_router_protocol.h"
#include "ipc_router_log.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
#define FORWARD_MSG_TO_XPORT(xport, msg) \
  send_and_log(xport, msg)

#define FORWARD_MSG_TO_CLIENT(client, msg) \
  FORWARD_MSG_TO_XPORT(client->xport, msg)

#define IPC_ROUTER_POLICY_CAN_SEND(scope, proc, xp_scope) \
  (ipc_router_policy.send ? ipc_router_policy.send(scope, proc, xp_scope) : 1)

#define IPC_ROUTER_POLICY_CAN_RECV(xp_scope, proc, scope) \
  (ipc_router_policy.recv ? ipc_router_policy.recv(xp_scope, proc, scope) : 1)

#define IPC_ROUTER_POLICY_GET_SCOPE(type, instance, host, scope) \
  (ipc_router_policy.get_scope ? ipc_router_policy.get_scope(type, instance, host, scope) : 1)

#define IPC_ROUTER_POLICY_GET_LINK_SCOPE(xport_tag) \
  (ipc_router_policy.get_xport_scope && xport_tag ? ipc_router_policy.get_xport_scope(xport_tag) : IPC_ROUTER_DEFAULT_SCOPE)

#define ADJUST_CLIENT_MODE_ON_SEND(client, dest_xport) \
  do { \
    ipc_router_os_sem_lock(&((client)->mode_sem));  \
    if((client)->mode == IPC_ROUTER_EP_MODE_NULL) \
    { \
      (client)->mode = IPC_ROUTER_EP_MODE_SINGLE; \
      (client)->connected_xport = dest_xport; \
    } \
    else if( (client)->mode == IPC_ROUTER_EP_MODE_SINGLE && \
            (client)->connected_xport != dest_xport) \
    { \
      (client)->mode = IPC_ROUTER_EP_MODE_MULTI; \
      (client)->connected_xport = NULL; \
    } \
    ipc_router_os_sem_unlock(&((client)->mode_sem));  \
  } while(0)

#define IPC_ROUTER_MAX_TRIES 0xfffeL

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
uint32 ipc_router_local_processor_id = 0xffffffff;

/* port semaphore and auto port are used to assign port numbers */
static ipc_router_os_sem port_semaphore;
static uint16 ipc_router_auto_port;
ipc_router_policy_ops_type ipc_router_policy = {NULL, NULL, NULL, NULL};

/* protocol semaphore is used to enforce compliance with protocol in concurrent
 * scenarios such as receiving a hello message while a server is being 
 * registered.
 */
static ipc_router_os_sem protocol_semaphore;

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/
static int ipc_router_send_cntl_msg
(
 ipc_router_control_msg_body   *cntl_msg,
 ipc_router_xport_type         *xport,
 ipc_router_address_type       *addr
 );

static int ipc_router_bcast_cntl_msg
(
 ipc_router_control_msg_body *cntl_msg,
 uint32 net_id_mask,
 uint32 priority_pref,
 uint64 scope
 );

static void ipc_router_bcast_to_other_xport
(
 ipc_router_control_msg_body *cntl_msg,
 ipc_router_xport_type *xport,
 uint64 scope
 );

static int send_and_log
(
 ipc_router_xport_type *xport,
 ipc_router_packet_type *msg
 )
{
  ipc_router_log_tx(xport, msg);
  return xport->ops->xport_write(xport->handle, msg);
}

static uint64 get_client_scope(ipc_router_address_type *addr)
{
  uint64 scope = 0;
  ipc_router_client_type *client;
  client = ipc_router_find_client(addr);
  if(client)
  {
    scope = client->scope;
    ipc_router_unlock_client(client);
  }
  return scope;
}

static void ipc_router_send_remove_client
(
  ipc_router_address_type *addr,
  ipc_router_xport_type   *xport
)
{
  ipc_router_control_msg_body cntl_msg = {0};
  ipc_router_client_type *client;

  client = ipc_router_find_client(addr);
  if(!client)
  {
    return;
  }

  /* construct a remove client message for other xports */
  cntl_msg.command = IPC_ROUTER_CONTROL_REMOVE_CLIENT;
  cntl_msg.arguments.client_arg = *addr;

  if(client->mode == IPC_ROUTER_EP_MODE_MULTI)
  {
    if(xport)
    {
      ipc_router_bcast_to_other_xport(&cntl_msg, xport, client->scope);
    }
    else
    {
      ipc_router_bcast_cntl_msg(&cntl_msg, IPC_ROUTER_XPORT_NET_ALL,
          client->priority, client->scope);
    }
  }
  else if(client->mode == IPC_ROUTER_EP_MODE_SINGLE && 
          client->connected_xport != NULL)
  {
    if(IPC_ROUTER_STATUS_SUCCESS == ipc_router_lock_xport(client->connected_xport))
    {
      if(IPC_ROUTER_POLICY_CAN_SEND(&client->scope, client->connected_xport->remote_router.processor_id, &client->connected_xport->xport_scope))
      {
        if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_send_cntl_msg(&cntl_msg, client->connected_xport, NULL))
        {
          IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_WARNING,
            "ipc_router_core: Could not send remote client message\n",0,0,0);
        }
      }
      ipc_router_unlock_xport(client->connected_xport);
    }
  }
  ipc_router_unlock_client(client);
}

/*===========================================================================
FUNCTION      get_next_port

DESCRIPTION   Get the next available port to use

RETURN VALUE  Next port number
===========================================================================*/
static uint32 get_next_port(void)
{
  uint32 port;
  ipc_router_os_sem_lock(&port_semaphore);
  ipc_router_auto_port++;
  /* skip over port 0 */
  if(ipc_router_auto_port == 0)
    ipc_router_auto_port = 1;
  port = (uint32)ipc_router_auto_port;
  ipc_router_os_sem_unlock(&port_semaphore);
  
  return port;
}

/*===========================================================================
FUNCTION      ipc_router_core_send_hello

DESCRIPTION   Send hello message to xport

ARGUMENTS     xport - transport to send the message to

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h
===========================================================================*/
static int ipc_router_core_send_hello
(
  ipc_router_xport_type *xport
)
{
  ipc_router_control_msg_body      cntl_msg;

  cntl_msg.command = IPC_ROUTER_CONTROL_HELLO;

  return ipc_router_send_cntl_msg(&cntl_msg, xport, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_core_send_msg_local

DESCRIPTION   Send message to local client

ARGUMENTS     client    - pointer to local client
              msg       - pointer to message NOT including header
              header    - pointer to header

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  Client's Rx callback will be invoked, packet is not freed 
===========================================================================*/
static int ipc_router_core_send_msg_local
(
 ipc_router_client_type *client,
 ipc_router_packet_type *msg,
 ipc_router_header_type *header
)
{
  if(!client->xport && client->rx_cb) 
  {
    /* remove alignment padding */
    ipc_router_packet_trim(&msg, header->msg_size);
    client->rx_cb(client, client->rx_cb_data, msg, &header->src_addr, 
        header->msg_size, (boolean)header->confirm_rx); 
    return IPC_ROUTER_STATUS_SUCCESS;
  }
  ipc_router_packet_free(&msg);
  return IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      ipc_router_core_build_packet

DESCRIPTION   Build packet from header and payload

ARGUMENTS     Header, pointer to payload and length of payload

RETURN VALUE  Packet pointer

SIDE EFFECTS  None
===========================================================================*/
static ipc_router_packet_type *ipc_router_core_build_packet
(
 ipc_router_protocol_ops_type *protocol_ops,
 ipc_router_header_type *header,
 ipc_router_packet_type *msg,
 unsigned char *payload,
 uint32 len
 )
{
  uint32 pad_data = 0;
  uint32 pad_len = (4 - (len & 3)) & 3;

  if(msg)
  {
    uint32 real_len = ipc_router_packet_length(msg);

    if(real_len != len && real_len != (len + pad_len))
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR, 
          "ipc_router_core: Length mismatch\n",0,0,0);
      return NULL;
    }

    /* Set len to be the padded len (if) so we do not
     * re-pad the packet */
    len = real_len;
    pad_len = (4 - (len & 3)) & 3;

    if(protocol_ops->inject(header, &msg) != IPC_ROUTER_STATUS_SUCCESS)
    {
      return NULL;
    }
  } 
  else
  {
    msg = ipc_router_packet_new_headroom(IPC_ROUTER_MAX_HEADER_SIZE);
    if(!msg)
      return NULL;

    if(protocol_ops->inject(header, &msg) != IPC_ROUTER_STATUS_SUCCESS)
    {
      ipc_router_packet_free(&msg);
      return NULL;
    }

    if(ipc_router_packet_copy_payload(&msg, payload, len) != len)
    {
      ipc_router_packet_free(&msg);
      return NULL;
    }
  }

  if(pad_len)
  {
    if(ipc_router_packet_copy_tail(&msg, &pad_data, pad_len) != pad_len)
    {
      ipc_router_packet_free(&msg);
      return NULL;
    }
  }

  return msg;
}

/*===========================================================================
FUNCTION      ipc_router_xport_up

DESCRIPTION   This function brings up a transport and send list of servers
              to the remote router.

ARGUMENTS     

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_xport_up(ipc_router_xport_type *xport)
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_server_list_type *server_list;
  ipc_router_control_msg_body cntl_msg = {0};

  /* Mark transport as up now we've heard from the other router */
  xport->up = 1;

  /* If we haven't sent a hello message, send one as in the
     case of modem restart
   */
  if(!(xport->hello_sent)) {
    xport->hello_sent = 1;
    if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_core_send_hello(xport))
    {
      xport->hello_sent = 0;
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
          "ipc_router_core: failed to send hello message.\n",0,0,0);
      return;
    }
  }

  /* Get a list of known servers */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_servers(&cr, &results);

  /* if have any servers to advertise, send them one at a time */
  cntl_msg.command = IPC_ROUTER_CONTROL_NEW_SERVER;

  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    ipc_router_client_type *client;
    /* Use the same control message we received, after updating the fields */
    cntl_msg.arguments.server_arg.addr = server_list->addr;
    cntl_msg.arguments.server_arg.service = server_list->service;

    client = ipc_router_find_client(&server_list->addr);

    if(client)
    {
      /* Servers on this processor, or servers on processors of a different net,
       * and if the priority matches our requirement */
      if((client->xport == NULL || (client->xport->net_id & xport->net_id) == 0) &&
         (client->priority == 0 || (client->priority >= xport->priority)) &&
         IPC_ROUTER_POLICY_CAN_SEND(&client->scope, xport->remote_router.processor_id, &xport->xport_scope))
      {
        if(IPC_ROUTER_STATUS_SUCCESS != 
          ipc_router_send_cntl_msg(&cntl_msg, xport, NULL))
        {
          IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
            "ipc_router_core: ipc_router_send_cntl_msg failure.\n",0,0,0);
        }   
      }
      ipc_router_unlock_client(client);
    }

    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_xport_down

DESCRIPTION   This function removes the clients and servers associated with
              the xport.
ARGUMENTS     

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_xport_down(ipc_router_xport_type *xport)
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_server_list_type *server_list;
  ipc_router_client_list_type *client_list;
  ipc_router_proc_list_type *proc_list;
  ipc_router_control_msg_body cntl_msg = {0};

  xport->hello_sent = 0;
  xport->up = 0;

  /* remove all clients and servers known over this transport */
  cr.crit = IPC_ROUTER_CRITERIA_XPORT;
  cr.arguments.xport_arg = xport;

  /* remove all servers known over this transport */
  ipc_router_list_servers(&cr, &results);

  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    /* construct a remove server message for other xports */
    cntl_msg.command = IPC_ROUTER_CONTROL_REMOVE_SERVER;
    cntl_msg.arguments.server_arg.addr = server_list->addr;
    cntl_msg.arguments.server_arg.service = server_list->service;
    ipc_router_bcast_to_other_xport(&cntl_msg, xport, get_client_scope(&server_list->addr));

    if(IPC_ROUTER_STATUS_SUCCESS != 
        ipc_router_unregister_server(&(server_list->service), 
          &(server_list->addr))
      )
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: unregister server failure.\n",0,0,0);
    }
    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }

  /* Get a list of all clients known over this transport */
  ipc_router_list_clients(&cr, &results);

  /* remove all clients known over this transport */
  while(NULL != (client_list = LIST_HEAD(results.clients)))
  {
    ipc_router_send_remove_client(&client_list->addr, xport);

    /* Remove the client from the routing table */
    if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_unregister_client(&(client_list->addr)))
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: unregister client failure.\n",0,0,0);
    }
    LIST_REMOVE(results.clients, client_list, link);
    ipc_router_os_free(client_list);
  }

  /* remove all processors known over this transport */
  ipc_router_list_procs(&cr, &results);

  while(NULL != (proc_list = LIST_HEAD(results.procs)))
  {
    /* construct a remove proc message for other xports */
    if(IPC_ROUTER_STATUS_SUCCESS != 
        ipc_router_unregister_proc(proc_list->processor_id))
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: unregister processor failure.\n",0,0,0);
    }
    LIST_REMOVE(results.procs, proc_list, link);
    ipc_router_os_free(proc_list);
  }
}

/*===========================================================================
FUNCTION      ipc_router_remove_client

DESCRIPTION   This function removes the clients at a given address

ARGUMENTS     

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_remove_client(ipc_router_address_type *addr)
{
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;
  ipc_router_server_list_type *server_list;
  int ret;

  /* try to remove client */
  ret = ipc_router_unregister_client(addr);

  if(IPC_ROUTER_STATUS_SUCCESS != ret)
  {
    return ret;
  }

  /* Get a list of servers from this client */
  cr.crit = IPC_ROUTER_CRITERIA_ADDRESS;
  cr.arguments.addr_arg = addr;
  ipc_router_list_servers(&cr, &results);

  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_unregister_server(&(server_list->service), &(server_list->addr)))
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: unregister server failure.\n",0,0,0);
    }

    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_resume_tx

DESCRIPTION   This function removes the clients at a given address

ARGUMENTS     

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_resume_tx(ipc_router_address_type *addr)
{
  ipc_router_client_type *client;
  client = ipc_router_find_client(addr);
  if(!client)
  {
    IPC_ROUTER_OS_MESSAGE(
        IPC_ROUTER_MSG_ERR,
        "ipc_router_core: resume tx for unknown client proc=0x%x port: 0x%x <- \n", 
        addr->processor_id, addr->port_id, 0);
    return IPC_ROUTER_STATUS_FAILURE;
  }
  ipc_router_resume_client_tx(client, addr);
  ipc_router_unlock_client(client);
  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_handle_control_msg

DESCRIPTION   This function handles router control messages
        
ARGUMENTS     cntl_msg - pointer to control message body
              xport    - pointer to transport where the message was received
              header   - pointer to header

RETURN VALUE  None

SIDE EFFECTS  Messages can be forwarded if income link type != outgoing link
===========================================================================*/
static void ipc_router_handle_control_msg
(
 ipc_router_control_msg_body *cntl_msg,
 ipc_router_xport_type *xport,
 ipc_router_header_type *rx_header 
)
{
  int                               ret;
  uint64                            scope = IPC_ROUTER_DEFAULT_SCOPE;
  ipc_router_server_type            *server;


  switch(cntl_msg->command) {
    case IPC_ROUTER_CONTROL_NEW_SERVER:
      server = &cntl_msg->arguments.server_arg;
      if(!IPC_ROUTER_POLICY_GET_SCOPE(server->service.type,
                                      server->service.instance,
                                      server->addr.processor_id,
                                       &scope))
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
                             "ipc_router_core: register server 0x%x:0x%x from proc:%d not allowed\n", 
                             server->service.type, server->service.instance,
                             server->addr.processor_id);
        break;
      }

      if(!IPC_ROUTER_POLICY_CAN_RECV(NULL, ipc_router_local_processor_id,
                                     &scope))
      {
        break;
      }

      /* if the server is unknown, add it to the routing table */
      if(IPC_ROUTER_STATUS_SUCCESS != (ret = 
            ipc_router_register_client_ext(
              &server->addr, xport,
              NULL, 0, NULL, NULL, 
              NULL, NULL, 0, scope,
              IPC_ROUTER_EP_MODE_MULTI)
            ) 
          && IPC_ROUTER_STATUS_BUSY != ret)
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: register server type=0x%x client addr=0x%x:0x%x failure.\n",
                               server->service.type, 
                               server->addr.processor_id, 
                               server->addr.port_id);
        break;
      }

      if(IPC_ROUTER_STATUS_SUCCESS != 
          ipc_router_register_server(
            &(cntl_msg->arguments.server_arg.service),
            &(cntl_msg->arguments.server_arg.addr),
            xport)
        )
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: register server 0x%x:0x%x failure.\n",
                              cntl_msg->arguments.server_arg.service.type,
                              cntl_msg->arguments.server_arg.service.instance,0);
        break;
      }
      /* Scope of endpoint might be different after a client register */
      scope = get_client_scope(&server->addr);
      ipc_router_bcast_to_other_xport(cntl_msg, xport, scope);
      break;

    case IPC_ROUTER_CONTROL_REMOVE_SERVER:
      /* remove the server from the servers table */
      scope = get_client_scope(&cntl_msg->arguments.server_arg.addr);
      if(IPC_ROUTER_STATUS_SUCCESS != 
          ipc_router_unregister_server(
          &(cntl_msg->arguments.server_arg.service), 
          &(cntl_msg->arguments.server_arg.addr))
        )
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: unregister server 0x%x:0x%x failure.\n",
                              cntl_msg->arguments.server_arg.service.type,
                              cntl_msg->arguments.server_arg.service.instance, 0);
        break;
      }
      ipc_router_bcast_to_other_xport(cntl_msg, xport, scope);

      break;

    case IPC_ROUTER_CONTROL_REMOVE_CLIENT:
      ipc_router_send_remove_client(&cntl_msg->arguments.client_arg, xport);
      ipc_router_remove_client(&(cntl_msg->arguments.client_arg));
      break;

    case IPC_ROUTER_CONTROL_HELLO:
      /* Save the address of the router on the other end of the transport */
      xport->remote_router = rx_header->src_addr;

      /* this step needs to be atomic with server registration */
      ipc_router_os_sem_lock(&protocol_semaphore);
      ipc_router_xport_up(xport);
      ipc_router_os_sem_unlock(&protocol_semaphore);
      break;

    case IPC_ROUTER_CONTROL_BYE:
      ipc_router_xport_down(xport);
      break;

    case IPC_ROUTER_CONTROL_RESUME_TX:
      ipc_router_resume_tx(&(cntl_msg->arguments.client_arg));
      break;

    default:
      break;
  }
}

/*===========================================================================
FUNCTION      ipc_router_send_cntl_msg

DESCRIPTION   Sends a router control message over a given transport.  

ARGUMENTS     cntl_msg (in)       - Buffer containing the control message to 
                                    be sent

              xport (in)         - the transport over which the 
                                    control message to be sent.

              dest_addr (in)      - address of the destination. If NULL the
                                    address of the other router on the other
                                    end of the xport is used
              
RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_send_cntl_msg
(
 ipc_router_control_msg_body   *cntl_msg,
 ipc_router_xport_type         *xport,
 ipc_router_address_type       *dest_addr
 )
{
  ipc_router_header_type tx_header; 
  ipc_router_packet_type *msg;

  if(!cntl_msg || !xport)
    return IPC_ROUTER_STATUS_INVALID_PARAM;;

  /* hello message not received before a control message is sent - skip over
   * this transport
   */
  if(cntl_msg->command != IPC_ROUTER_CONTROL_HELLO && !xport->up)
  {
    return IPC_ROUTER_STATUS_SUCCESS;
  }

  /* Compose control message */
  BUILD_HEADER(
      tx_header,
      cntl_msg->command,
      ipc_router_local_processor_id,
      IPC_ROUTER_ROUTER_PORT,
      0,
      sizeof(ipc_router_control_msg_body),
      0, 0);

  if(dest_addr)
  {
    tx_header.dest_addr.processor_id = dest_addr->processor_id;
    tx_header.dest_addr.port_id    = IPC_ROUTER_ROUTER_PORT;
  }
  else
  {
    tx_header.dest_addr = xport->remote_router;
  }

  msg = ipc_router_core_build_packet(xport->protocol_ops, &tx_header, NULL, 
                    (unsigned char *)cntl_msg, sizeof(ipc_router_control_msg_body));

  if(!msg)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_core: failed to build packet.\n",0,0,0);
    return IPC_ROUTER_STATUS_NO_MEM;
  }

  /* Write the message */
  if(IPC_ROUTER_STATUS_SUCCESS != FORWARD_MSG_TO_XPORT(xport, msg))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_core: failed to write control message.\n",0,0,0);
    return IPC_ROUTER_STATUS_IO_ERROR;
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_bcast_cntl_msg

DESCRIPTION   Sends a router control message over a given transport.  

ARGUMENTS     cntl_msg (in)       - Buffer containing the control message to 
                                    be sent

              net_id_mask         - mask of the type of link to send msg to
              priority_pref       - priority preference
              
RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_bcast_cntl_msg
(
 ipc_router_control_msg_body *cntl_msg,
 uint32 net_id_mask,
 uint32 priority_pref,
 uint64 scope
 )
{
  ipc_router_xport_type *xport;
  ipc_router_xport_list_type *xport_list;
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;

  if(priority_pref == 0)
    priority_pref = IPC_ROUTER_XPORT_PRIORITY_MASK;

  /* Get a list of unique tranports */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_xports(&cr, &results);

  while(NULL != (xport_list = LIST_HEAD(results.xports)))
  {
    xport = xport_list->xport;

    /* Try to lock the xport - it may have been unregistered and freed by
     * another thread, so the xport handle is invalid.
     */
    if(IPC_ROUTER_STATUS_SUCCESS == ipc_router_lock_xport(xport))
    {
      if((xport->net_id & net_id_mask) && 
          xport->priority <= priority_pref &&
          IPC_ROUTER_POLICY_CAN_SEND(&scope, 
          xport->remote_router.processor_id, &xport->xport_scope))
      {
        if(IPC_ROUTER_STATUS_SUCCESS != 
            ipc_router_send_cntl_msg(cntl_msg, xport, NULL))
        {
          IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
              "ipc_router_core: bcast cntl msg to xport: %p.\n",
              xport_list->xport,0,0); 
        }
      }
      ipc_router_unlock_xport(xport);
    }
    LIST_REMOVE(results.xports, xport_list, link);
    ipc_router_os_free(xport_list);
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_bcast_to_other_xport

DESCRIPTION   This function broadcast router control messages to other links
              of a different type
        
ARGUMENTS     cntl_msg - incoming message
              xport - pointer to transport where the message was received

RETURN VALUE  None

SIDE EFFECTS  Messages can be forwarded if income link type != outgoing link
===========================================================================*/
static void ipc_router_bcast_to_other_xport
(
 ipc_router_control_msg_body *cntl_msg,
 ipc_router_xport_type *xport,
 uint64 scope
 )
{
  /* if we received the message from a transport, forward it to others of
   * different types.
   */
  if(xport)
  {
    if(ipc_router_bcast_cntl_msg(cntl_msg, 
          IPC_ROUTER_XPORT_NET_ALL & (IPC_ROUTER_XPORT_NET_ALL ^ xport->net_id),
          0, scope) != IPC_ROUTER_STATUS_SUCCESS)
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
          "ipc_router_core: unable to broadcast control.\n",0,0,0);
    }
  }
}

/*===========================================================================
FUNCTION      ipc_router_forward_msg

DESCRIPTION   This function forwards messages (data & control)
        
ARGUMENTS     msg - message NOT including header
              xport - xport of where the message came from 
              rx_header - header to the message

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_forward_msg
(
 ipc_router_packet_type *msg,
 ipc_router_xport_type *xport,
 ipc_router_header_type *rx_header
)
{
  int ret;
  ipc_router_xport_type  *dest_xport;
  ipc_router_packet_type *src_msg = msg;

  if(rx_header->msg_type == IPC_ROUTER_CONTROL_DATA)
  {
    ipc_router_client_type *client;
    client = ipc_router_find_client(&rx_header->dest_addr);
    if(!client)
    {
      IPC_ROUTER_OS_MESSAGE(
        IPC_ROUTER_MSG_ERR,
        "ipc_router_core: No client to forward type=%d to 0x%x:0x%x <-\n", 
        rx_header->msg_type,
        rx_header->dest_addr.processor_id,
        rx_header->dest_addr.port_id);
      goto forward_free_msg_bail;
    }

    dest_xport = client->xport;
    ipc_router_unlock_client(client);

    /* Find (and register if not present) the source endpoint.
       Adjust its mode if necessary */
    client = ipc_router_find_client(&rx_header->src_addr);
    if(!client)
    {
      /* This is not a service, we just came to know about it, so
       * being the first packet, create a client in single mode */
      ret = ipc_router_register_client(&rx_header->src_addr, xport, NULL, 0, 
                                      IPC_ROUTER_EP_MODE_SINGLE);
      if(ret != IPC_ROUTER_STATUS_SUCCESS || 
          NULL == (client = ipc_router_find_client(&rx_header->src_addr)))
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR, 
               "ipc_router_core: Failed to forward due to source client register failure\n", 0, 0, 0);
        goto forward_free_msg_bail;
      }
      client->connected_xport = dest_xport;
    }

    /* Adjust the src_client mode if required */
    ADJUST_CLIENT_MODE_ON_SEND(client, dest_xport);
    ipc_router_unlock_client(client);
  }
  else
  {
    dest_xport = ipc_router_find_xport(rx_header->dest_addr.processor_id);
  }

  if(!dest_xport || 
      IPC_ROUTER_STATUS_SUCCESS != ipc_router_lock_xport(dest_xport))
  {
    IPC_ROUTER_OS_MESSAGE(
        IPC_ROUTER_MSG_ERR,
        "ipc_router_core: No route to forward type=%d to 0x%x:0x%x <-\n", 
        rx_header->msg_type,
        rx_header->dest_addr.processor_id,
        rx_header->dest_addr.port_id);
      goto forward_free_msg_bail;
  }

  /* only forward if link types are different */
  if((dest_xport->net_id & xport->net_id) & IPC_ROUTER_XPORT_NET_ALL) 
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_core: forwarding disallowed. net1=0x%x net2=0x%x.\n",
        dest_xport->net_id, xport->net_id, 0);
    goto foward_unlock_xport_bail;
  }

  msg = ipc_router_core_build_packet(dest_xport->protocol_ops, rx_header, msg, 
                                      NULL, rx_header->msg_size);
  if(!msg)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_core: build forwarded packet failed\n",0,0,0);
    ipc_router_packet_free(&src_msg);
    goto foward_unlock_xport_bail;
  }

  if(IPC_ROUTER_STATUS_SUCCESS == FORWARD_MSG_TO_XPORT(dest_xport, msg))
  {
    /* set msg to NULL so it does not get double-freed */
    msg = NULL;
  }
  else
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_core: failed to forward data message from proc:0x%x port:0x%x",
        rx_header->dest_addr.processor_id, rx_header->dest_addr.port_id, 0);
  }

foward_unlock_xport_bail:
  ipc_router_unlock_xport(dest_xport);
forward_free_msg_bail:
  if(msg)
    ipc_router_packet_free(&msg);

  return;
}

/*===========================================================================
FUNCTION      ipc_router_handle_data_msg

DESCRIPTION   This function handles data messages
        
ARGUMENTS     msg - message NOT including header
              xport - xport of where the message came from 
              rx_header - header to the message

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_handle_data_msg
(
 ipc_router_packet_type *msg,
 ipc_router_xport_type *xport,
 ipc_router_header_type *rx_header 
)
{
  int ret;
  ipc_router_client_type *client;
  ipc_router_client_type *src_client;

  src_client = ipc_router_find_client(&(rx_header->src_addr));
  if(!src_client)
  {
    /* Unknown client, regiser as a uni-connected client */
    ret = ipc_router_register_client(&rx_header->src_addr, xport, NULL, 0, 
                                     IPC_ROUTER_EP_MODE_SINGLE);
    if(ret != IPC_ROUTER_STATUS_SUCCESS || 
        NULL == (src_client = ipc_router_find_client(&rx_header->src_addr)))
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR, 
          "ipc_router_core: regiser_client 0x%x:0x%x failure!\n",
          rx_header->src_addr.processor_id, rx_header->src_addr.port_id, 0);
      ipc_router_packet_free(&msg);
      return;
    }
    src_client->connected_xport = NULL;
  }

  /* Receive a packet on the endpoint on the current processor (xport == NULL) */
  ADJUST_CLIENT_MODE_ON_SEND(src_client, NULL);

  ipc_router_unlock_client(src_client);

  client = ipc_router_find_client(&(rx_header->dest_addr));

  if(!client)
  {
    IPC_ROUTER_OS_MESSAGE(
        IPC_ROUTER_MSG_ERR,
        "ipc_router_core: Unknown client 0x%x, processor=0x%x <- \n", 
        rx_header->dest_addr.port_id, 
        rx_header->dest_addr.processor_id,
        0
        );
    ipc_router_packet_free(&msg);
    return; 
  }

  if(!IPC_ROUTER_POLICY_CAN_RECV(NULL, 
           rx_header->src_addr.processor_id, &client->scope))
  {
     IPC_ROUTER_OS_MESSAGE(
        IPC_ROUTER_MSG_ERR,
        "ipc_router_core: Client 0x%x:0x%x does not accept messages from proc:%d\n", 
        rx_header->dest_addr.processor_id, 
        rx_header->dest_addr.port_id,
        rx_header->src_addr.processor_id
        );
    ipc_router_packet_free(&msg);
    goto bail;
  }

  if(!client->xport)
  {
    ipc_router_core_send_msg_local(client, msg, rx_header);
  }
  else
  {
    IPC_ROUTER_OS_MESSAGE(
        IPC_ROUTER_MSG_ERR,
        "ipc_router_core: Non-local client 0x%x, processor=0x%x <- \n", 
        rx_header->dest_addr.port_id, 
        rx_header->dest_addr.processor_id,
        0
        );
    ipc_router_packet_free(&msg);
  }
bail:
  ipc_router_unlock_client(client);
}

/*===========================================================================
                        EXPORTED XAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      ipc_router_xal_recv

DESCRIPTION   function for transports to call when there's a packet to be 
              received.
              - this function can only be called by a single thread
                from any given transport

ARGUMENTS     
              cb_handle (in) - identifier for the transport
===========================================================================*/
void ipc_router_xal_recv
(
 ipc_router_packet_type *msg,
 void *cb_handle
)
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;
  ipc_router_header_type rx_header; 
  uint32 pkt_len;

  ipc_router_log_rx(xport, msg);

  if(xport->protocol_ops->extract(&msg, &rx_header) != IPC_ROUTER_STATUS_SUCCESS)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
         "ipc_router_core: illegible message received from proc:%d\n",
          xport->remote_router.processor_id,0,0);
    ipc_router_packet_free(&msg);
    return; 
  }

  pkt_len = ipc_router_packet_length(msg);

  /* Validate packet length else drop it. The packet cannot be smaller than the
   * expected size, and cannot be larger than expected size accounting
   * for any bytes added for padding */
  if(pkt_len < rx_header.msg_size || pkt_len > (rx_header.msg_size + 4))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
             "ipc_router_core: Unexpected msg of size %d instead of %d received from proc:%d\n",
              pkt_len, rx_header.msg_size, xport->remote_router.processor_id);
    ipc_router_packet_free(&msg);
    return;
  }

  /* Check if the xport can receive this message */
  if(!IPC_ROUTER_POLICY_CAN_RECV(&xport->xport_scope, 
              rx_header.src_addr.processor_id, NULL))
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
             "ipc_router_core: Xport cannot receive from processor: %d\n",
              rx_header.src_addr.processor_id,0,0);
    ipc_router_packet_free(&msg);
    return;
  }

  if(rx_header.msg_type != IPC_ROUTER_CONTROL_HELLO &&
     rx_header.dest_addr.processor_id != ipc_router_local_processor_id)
  {
    /* packet does not belong on this node, forward it */
    ipc_router_forward_msg(msg, xport, &rx_header);
  }
  else if(rx_header.msg_type == IPC_ROUTER_CONTROL_DATA)
  {
    /* process data packet */
    ipc_router_handle_data_msg(msg, xport, &rx_header);
  }
  else
  {
    ipc_router_control_msg_body cntl_data;

    /* process control message */
    if(ipc_router_packet_read_payload(&msg, &cntl_data, 
          sizeof(ipc_router_control_msg_body))
        == sizeof(ipc_router_control_msg_body))
    {
      ipc_router_handle_control_msg(&cntl_data, xport, &rx_header);
    }
    else
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
          "ipc_router_core: control message too short!\n",0,0,0);
    }
    ipc_router_packet_free(&msg);
  }
}

/*===========================================================================
FUNCTION      ipc_router_xal_error

DESCRIPTION   A callback function for transports to call in the case of I/O
              errors.  

ARGUMENTS     
              cb_handle (in) - identifier for the transport
===========================================================================*/
void ipc_router_xal_error
(   
 void *cb_handle
 )
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;

  /* Is the transport even opened? */
  if(!(xport->up)){
    /* Just ignore the problem */
    return;
  }

  /* Disable the transport */
  xport->up = 0;
  xport->hello_sent = 0;

  ipc_router_xport_down(xport);
}

/*===========================================================================
FUNCTION      ipc_router_xal_resume

DESCRIPTION   A callback function for transports to call to resume I/O after
              errors.  

ARGUMENTS     
              cb_handle (in) - identifier for the transport
===========================================================================*/
void ipc_router_xal_resume
(
  void *cb_handle 
)
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;
  if(xport->up)
  {
    return;
  }
  /* Send a hello in case the link comes back up. The
   * XAL should queue it up, if it cannot be sent
   */
  xport->hello_sent = 1;
  if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_core_send_hello(xport))
  {
    xport->hello_sent = 0;
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_core: failed to send hello message.\n",0,0,0);
    return;
  }
}

/*===========================================================================
FUNCTION      ipc_router_xal_closed

DESCRIPTION   Signaling the transport has fully closed.

ARGUMENTS     cb_handle - handle to xport
===========================================================================*/
void ipc_router_xal_closed
(
 void *cb_handle
 )
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)cb_handle;
  ipc_router_free_xport(xport);
}

/*===========================================================================
FUNCTION      ipc_router_xal_start_xport

DESCRIPTION   Initializes and opens a transport.  

ARGUMENTS     ops - table of xport operations
              xport_params - parameters to the xport
              flags - xport specific flags
              desc - description of the network interface

              FLAGS
               31                            0
               |__TAG__|_VERS_|_PRIO_|_NET_ID_|
                   16     4      4       8

RETURN VALUE  Handle to xport on success

SIDE EFFECTS  None
===========================================================================*/
void *ipc_router_xal_start_xport
(
 ipc_router_xal_ops_type *ops,
 void *xport_params,
 uint32 flags,
 char *desc
 )
{
  ipc_router_xport_type *xport;
  uint32 net_id = flags & IPC_ROUTER_XPORT_NET_ALL;
  uint8 priority = (flags >> 8) & IPC_ROUTER_XPORT_PRIORITY_MASK;
  uint32 version = (flags >> 12) & 0xf;
  uint32 tag = (uint32)((flags >> 16) & 0xffff);

  if(!ops)
    return NULL;

  xport = ipc_router_os_calloc(1, sizeof(ipc_router_xport_type)); 
  if(!xport)
  { 
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: memory allocation failure.\n",0,0,0);
    return NULL;
  }

  /* Configure the net_id of the link before we do anything to elminate race */
  if(net_id == 0)
    net_id = IPC_ROUTER_XPORT_NET_DEFAULT;

  /* If bundle ID is specified but priority is not, set it to medium */
  if(priority == 0)
    priority = IPC_ROUTER_XPORT_PRIORITY_MEDIUM;

  /* Default to IPC Router */
  if(version == 0)
    version = IPC_ROUTER_V1_VERSION;

  if(version >= IPC_ROUTER_VERSION_TBL_SIZE || 
      ipc_router_protocol_ops[version].inject == NULL ||
      ipc_router_protocol_ops[version].extract == NULL)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: Unsupported version %d.\n",version,0,0);
    ipc_router_os_free(xport);
    return NULL;
  }

  xport->desc = desc;
  xport->ops = ops;
  xport->net_id = net_id;
  xport->priority = priority;
  xport->xport_scope = IPC_ROUTER_POLICY_GET_LINK_SCOPE(tag);
  xport->protocol_ops = &ipc_router_protocol_ops[version];

  /* default remote router to the broadcast address */
  xport->remote_router.processor_id = IPC_ROUTER_BROADCAST_ADDRESS;
  xport->remote_router.port_id = IPC_ROUTER_ROUTER_PORT;

  /* mark hello message flag as sent so that the rx callback doesn't send a 
   * second hello message
   */
  xport->hello_sent = 1;

  if(ipc_router_register_xport(xport) != IPC_ROUTER_STATUS_SUCCESS)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: Unable to register xport!\n", 0,0,0);
    ipc_router_os_free(xport);
    return NULL;
  }

  /* Try to open and start the transport */
  xport->handle = xport->ops->xport_open(xport_params, xport);
  if(!xport->handle || 
      xport->ops->xport_start(xport->handle) != IPC_ROUTER_STATUS_SUCCESS)
  {   
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,"ipc_router_core: Unable to open xport!\n", 0,0,0);
    ipc_router_free_xport(xport);
    return NULL;
  }

  /* Send hello message */
  if(ipc_router_core_send_hello(xport) != IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_free_xport(xport);
    return NULL;
  }

  return xport;
}

/*===========================================================================
FUNCTION      ipc_router_xal_stop_xport

DESCRIPTION   Stops and deinitializes a transport.  

ARGUMENTS     handle - handle to xport

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_xal_stop_xport
(
 void *handle
 )
{
  ipc_router_xport_type *xport = (ipc_router_xport_type *)handle;    
  ipc_router_control_msg_body     cntl_msg = {0};

  if(!xport)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  /* Send bye message to the other router */
  cntl_msg.command = IPC_ROUTER_CONTROL_BYE;
  ipc_router_send_cntl_msg(&cntl_msg, xport, NULL);

  /* Send bye message locally to handle the event */
  ipc_router_xport_down(xport);

  ipc_router_unregister_xport(xport);

  xport->ops->xport_close(xport->handle);

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_policy_register

DESCRIPTION   Register a policy manager's methods

ARGUMENTS     ops - Operation table of the external policy manager

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_policy_register(ipc_router_policy_ops_type *ops)
{
  if(ops)
  {
    ipc_router_policy = *ops;
  }
}


/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      ipc_router_core_send_conf

DESCRIPTION   This function sends a confirmation message to a transport
        
ARGUMENTS     client - pointer to client record where the conf_msg originates
              addr   - pointer to the destination address

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_send_conf
(
 ipc_router_client_type *client,
 ipc_router_address_type *addr
)
{
  ipc_router_xport_type *xport;
  ipc_router_control_msg_body cntl_msg = {0};
  int ret;

  if(!client || !addr)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }
 
  cntl_msg.arguments.client_arg.processor_id = ipc_router_local_processor_id;
  cntl_msg.arguments.client_arg.port_id = client->port_id;

  if(addr->processor_id == ipc_router_local_processor_id)
  {
    ret = ipc_router_resume_tx(&cntl_msg.arguments.client_arg);
  }
  else
  {
    cntl_msg.command = IPC_ROUTER_CONTROL_RESUME_TX;
    ret = IPC_ROUTER_STATUS_IO_ERROR;
    xport = ipc_router_find_xport(addr->processor_id);
    if(xport)
    {
      if(IPC_ROUTER_STATUS_SUCCESS == ipc_router_lock_xport(xport))
      {
        ret = ipc_router_send_cntl_msg(&cntl_msg, xport, addr);
        ipc_router_unlock_xport(xport);
      }
    }
  }

  return ret;
}


/*===========================================================================
FUNCTION      ipc_router_core_open_with_options

DESCRIPTION   Opens a handle to the router. This handle is to be use in all 
              subsequent operations on the IPC router for this port.

ARGUMENTS     handle (out)   - a handle returned to identify this client in 
                               later operations.

              port_id (in) - a locally unique identifier for the end-point.
                             If 0, a port is dynamically assigned (recommended)

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_open_with_options
(
 ipc_router_client_type **handle,
 uint32 port_id,
 ipc_router_rx_cb_type rx_cb,
 void *rx_cb_data,
 ipc_router_event_cb_type event_cb,
 void *event_cb_data,
 ipc_router_core_options_type *options
)
{
  ipc_router_client_type *client = NULL;
  ipc_router_address_type ClientAddr;
  uint64 scope = IPC_ROUTER_DEFAULT_SCOPE;
  uint32 priority = IPC_ROUTER_PORT_PRIORITY_DEFAULT;
  int to_return = IPC_ROUTER_STATUS_UNKNOWN;
  int i;

  if(!handle || port_id >= IPC_ROUTER_RESERVED_PORTS_START || !rx_cb || 
      !event_cb)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  *handle = NULL;

  if(options)
  {
    if(options->options_set & IPC_ROUTER_CORE_PORT_OPTIONS_PRIORITY_VALID)
    {
      if(options->priority <= IPC_ROUTER_PORT_PRIORITY_HIGH)
      {
        priority = options->priority;
      }
      else
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_WARNING,
            "ipc_router_core_open_with_options: Invalid priority option: %d using default value\n",
            options->priority, 0, 0);
      }
    }

    if(options->options_set & IPC_ROUTER_CORE_PORT_OPTIONS_SCOPE_VALID)
    {
      scope = options->scope;
    }
  }

  ClientAddr.processor_id = ipc_router_local_processor_id;
  ClientAddr.port_id = port_id ?  port_id : get_next_port(); 

  priority = priority ? priority : IPC_ROUTER_PORT_PRIORITY_MEDIUM;

  for(i = 0; i < IPC_ROUTER_MAX_TRIES; i++)
  {
    to_return = ipc_router_register_client_ext(
                    &ClientAddr, 
                    NULL, 
                    &client,
                    0,
                    rx_cb,
                    rx_cb_data,
                    event_cb,
                    event_cb_data, 
                    priority,
                    scope,
                    IPC_ROUTER_EP_MODE_NULL);
    if(to_return != IPC_ROUTER_STATUS_BUSY || port_id != 0)
    {
      break;
    }
    ClientAddr.port_id = get_next_port();
  }

  if(IPC_ROUTER_STATUS_SUCCESS == to_return)
  {
    *handle = client;
  }

  return to_return;
}

/*===========================================================================
FUNCTION      ipc_router_core_open

DESCRIPTION   Opens a handle to the router. This handle is to be use in all 
              subsequent operations on the IPC router for this port.

ARGUMENTS     handle (out)   - a handle returned to identify this client in 
                               later operations.

              port_id (in) - a locally unique identifier for the end-point.
                             If 0, a port is dynamically assigned (recommended)

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_open
(
 ipc_router_client_type **handle,
 uint32 port_id,
 ipc_router_rx_cb_type rx_cb,
 void *rx_cb_data,
 ipc_router_event_cb_type event_cb,
 void *event_cb_data
)
{
  return ipc_router_core_open_with_options(handle, port_id, rx_cb, rx_cb_data, 
      event_cb, event_cb_data, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_core_send_internal

DESCRIPTION   Send buffer or packet to destination
              

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              dest (in)         - pointer to destination address
              buf (in)          - pointer to the data to be written
              len (in)          - lenth of the message to send
              msg (in)          - pointer to packet

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_BUSY if Tx quota exceeded. Wait for event cb.
              Other failures

SIDE EFFECTS  
===========================================================================*/
static int ipc_router_core_send_internal
(
 ipc_router_client_type *client,
 ipc_router_address_type *dest,
 const unsigned char *buf,
 uint32 len,
 ipc_router_packet_type *msg
)
{
  ipc_router_header_type header;
  ipc_router_client_type *dest_client = NULL;
  ipc_router_write_token_type token;
  int ret = IPC_ROUTER_STATUS_UNKNOWN;
   
  if(!client || (!buf && !msg)) 
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  if(msg)
    len = ipc_router_payload_length(msg);

  /* Find client record */
  dest_client = ipc_router_find_client(dest);

  /* The client record is now locked so it doesn't get freed/modified while
   * we are using it.
   */

  if(!dest_client)
  {
    IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
        "ipc_router_send: unknown client: Proc:0x%08x, client:0x%08x.\n",
        dest->processor_id, dest->port_id, 0);
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  ADJUST_CLIENT_MODE_ON_SEND(client, dest_client->xport);

  /* Get write token. If none avail, an event callback will be queued */
  token = ipc_router_get_write_token(dest_client, client);
  
  if(token == IPC_ROUTER_WRITE_TOKEN_NONE)
  {
    ret = IPC_ROUTER_STATUS_BUSY;
    goto ipc_router_send_bail;
  }

  BUILD_HEADER(
      header, 
      IPC_ROUTER_CONTROL_DATA, 
      ipc_router_local_processor_id,
      client->port_id, 
      (token == IPC_ROUTER_WRITE_TOKEN_LAST), 
      len, 
      dest->processor_id,
      dest->port_id);

  if(!dest_client->xport)
  {
    /* Client is local, build a packet without a header if we have to */
    if(!msg)
    {
      msg = ipc_router_packet_new();
      if(!msg || ipc_router_packet_copy_payload(&msg, (void *)buf, len) != len)
      {
        IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
            "ipc_router_core: failed to build packet.\n",0,0,0);
        ret = IPC_ROUTER_STATUS_NO_MEM;
        goto ipc_router_send_bail;
      }
    }
    ipc_router_core_send_msg_local(dest_client, msg, &header);
  }
  else
  {
    /* Remote client, send on transport */
    msg = ipc_router_core_build_packet(dest_client->xport->protocol_ops, &header, 
                                        msg, (unsigned char *)buf, len);
    if(!msg)
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
          "ipc_router_core: failed to build packet.\n",0,0,0);
      ret = IPC_ROUTER_STATUS_NO_MEM;
      goto ipc_router_send_bail;
    }

    if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_lock_xport(dest_client->xport))
    {
      /* unable to find and lock xport */
      ret = IPC_ROUTER_STATUS_IO_ERROR;
      goto ipc_router_send_bail;
    }

    if(!IPC_ROUTER_POLICY_CAN_SEND(&client->scope, dest->processor_id, 
                                  &dest_client->xport->xport_scope))
    {
      ret = IPC_ROUTER_STATUS_NOPERM;
      ipc_router_unlock_xport(dest_client->xport);
      goto ipc_router_send_bail;
    }

    if(IPC_ROUTER_STATUS_SUCCESS != FORWARD_MSG_TO_CLIENT(dest_client, msg))
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR,
          "ipc_router_core: failed to send data message.\n",0,0,0);
      ret = IPC_ROUTER_STATUS_IO_ERROR;
      ipc_router_unlock_xport(dest_client->xport);
      goto ipc_router_send_bail;
    }
    ipc_router_unlock_xport(dest_client->xport);
  }

  ret = IPC_ROUTER_STATUS_SUCCESS;

ipc_router_send_bail:
  ipc_router_unlock_client(dest_client);
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
    ipc_router_packet_free(&msg);

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_core_send

DESCRIPTION   Send message to destination 
             

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              dest (in)         - pointer to destination address
              buf (in)          - pointer to the data to be written
              len (in)          - lenth of the message to send

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_BUSY if Tx quota exceeded. Wait for event cb.
              Other failures

SIDE EFFECTS  
===========================================================================*/
int ipc_router_core_send
(
 ipc_router_client_type *client,
 ipc_router_address_type *dest,
 const unsigned char *buf,
 uint32 len
)
{
  return ipc_router_core_send_internal(client, dest, buf, len, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_core_get_packet

DESCRIPTION   Get packet for transmission. 

ARGUMENTS     

RETURN VALUE  Pointer to packet or NULL on failure

SIDE EFFECTS  Packet is allocated with proper headroom reserved
===========================================================================*/
ipc_router_packet_type *ipc_router_core_get_packet(void)
{
  return ipc_router_packet_new_headroom(sizeof(ipc_router_header_type));
}

/*===========================================================================
FUNCTION      ipc_router_core_send_packet

DESCRIPTION   Send packet to destination

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              dest (in)         - pointer to destination address
              msg (in)          - pointer to packet

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_BUSY if Tx quota exceeded. Wait for event cb.
              Other failures

SIDE EFFECTS  
===========================================================================*/
int ipc_router_core_send_packet
(
 ipc_router_client_type *client,
 ipc_router_address_type *dest,
 ipc_router_packet_type *msg
)
{
  return ipc_router_core_send_internal(client, dest, NULL, 0, msg);
}

/*===========================================================================
FUNCTION      ipc_router_core_send_to_name

DESCRIPTION   Writes data from ipc client to the address contained in the 
              first 64 bits of the buffer.          

ARGUMENTS     client (in)       - client handle returned from ipc_router_open
              dest (in)         - pointer to destination server name
              buf (in)          - pointer to the data to be written
              len (in)          - lenth of the message to send

RETURN VALUE  IPC_ROUTER_STATUS_SUCCESS on success
              IPC_ROUTER_STATUS_BUSY if Tx quota exceeded. Wait for event cb.
              Other failures

SIDE EFFECTS  
===========================================================================*/
int ipc_router_core_send_to_name
(
 ipc_router_client_type *client,
 ipc_router_service_type *service,
 const unsigned char *buf,
 uint32 len
)
{
  int ret;
  ipc_router_address_type addr;

  ret = ipc_router_find_server(service, &addr, NULL, NULL);

  if(ret != IPC_ROUTER_STATUS_SUCCESS)
    return ret;

  return ipc_router_core_send(client, &addr, buf, len);
                              
}

/*===========================================================================
FUNCTION      ipc_router_reg_server

DESCRIPTION   Publish server

ARGUMENTS     client - client handle
              service - type and instance of the server

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients' server event callbacks will be invoked
===========================================================================*/
int ipc_router_core_reg_server
(
 ipc_router_client_type *client,
 ipc_router_service_type *service
)
{
  int ret;
  ipc_router_control_msg_body cntl_msg = {0};
  uint64 scope = IPC_ROUTER_DEFAULT_SCOPE;

  if(!client || !service)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  cntl_msg.command = IPC_ROUTER_CONTROL_NEW_SERVER;
  cntl_msg.arguments.server_arg.addr.processor_id = ipc_router_local_processor_id;
  cntl_msg.arguments.server_arg.addr.port_id = client->port_id;
  cntl_msg.arguments.server_arg.service.type = service->type;
  cntl_msg.arguments.server_arg.service.instance  = service->instance;

  ipc_router_os_sem_lock(&client->mode_sem);
  client->mode = IPC_ROUTER_EP_MODE_MULTI;
  ipc_router_os_sem_unlock(&client->mode_sem);

  if(!IPC_ROUTER_POLICY_GET_SCOPE(service->type, service->instance, 
                              ipc_router_local_processor_id, &scope))
  {
    return IPC_ROUTER_STATUS_NOPERM;
  }
  /* End-point's scope is the lowest common denominator. */
  client->scope &= scope;

  /* registration and broadcast must not be interrupted by a hello message */
  ipc_router_os_sem_lock(&protocol_semaphore);

  ret = ipc_router_register_server(
      &(cntl_msg.arguments.server_arg.service),
      &(cntl_msg.arguments.server_arg.addr),
      NULL);
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
    goto reg_bail;

  /* broadcast message over all transports */
  ret = ipc_router_bcast_cntl_msg(&cntl_msg, IPC_ROUTER_XPORT_NET_ALL, 
      client->priority, client->scope);

reg_bail:
  ipc_router_os_sem_unlock(&protocol_semaphore);
  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_core_dereg_server

DESCRIPTION   Withdraw server publication

ARGUMENTS     client - client handle
              service - type and instance of the server

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  Clients' server event callbacks will be invoked
===========================================================================*/
int ipc_router_core_dereg_server
(
 ipc_router_client_type *client,
 ipc_router_service_type *service
)
{
  ipc_router_control_msg_body cntl_msg = {0};
  int ret;

  if(!client || !service)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  cntl_msg.command = IPC_ROUTER_CONTROL_REMOVE_SERVER;
  cntl_msg.arguments.server_arg.addr.processor_id = ipc_router_local_processor_id;
  cntl_msg.arguments.server_arg.addr.port_id = client->port_id;
  cntl_msg.arguments.server_arg.service.type = service->type;
  cntl_msg.arguments.server_arg.service.instance  = service->instance;

  ret = ipc_router_unregister_server(
      &(cntl_msg.arguments.server_arg.service),
      &(cntl_msg.arguments.server_arg.addr));
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
    return ret;

  /* broadcast message over all transports */
  ret = ipc_router_bcast_cntl_msg(&cntl_msg, IPC_ROUTER_XPORT_NET_ALL, 0, client->scope);
  if(ret != IPC_ROUTER_STATUS_SUCCESS)
    return ret;

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_core_find_server

DESCRIPTION   finds the destination of a server if known to the  router.  

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_find_server
(   
 ipc_router_client_type  *client,
 ipc_router_service_type *service,
 ipc_router_address_type *addr,
 ipc_router_instance_compare_type compare,
 void *compare_data
)
{
  if(!service || !addr)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  return ipc_router_find_server(service, addr, compare, compare_data);
}

/*===========================================================================
FUNCTION      ipc_router_core_find_all_servers

DESCRIPTION   finds all the servers satisfying the type and instances based
              on the return value of the optional compare function

RETURN VALUE  returns IPC_ROUTER_STATUS_SUCCESS unless there is an error.

SIDE EFFECTS  None
===========================================================================*/
int ipc_router_core_find_all_servers
(   
 ipc_router_client_type  *client,
 ipc_router_service_type *service,
 ipc_router_server_type  *servers,
 uint32                  *num_entries,
 uint32                  *num_servers,
 ipc_router_instance_compare_type compare,
 void                    *compare_data

)
{
  ipc_router_search_criteria_type  cr;
  ipc_router_search_results_type   results;
  ipc_router_server_list_type     *server_list;
  uint32 to_fill = 0;

  if(!service || !num_servers)
    return IPC_ROUTER_STATUS_INVALID_PARAM;

  *num_servers = 0;
  if(num_entries && servers)
  {
    to_fill = *num_entries;
    *num_entries = 0;
  }

  /* Get a list of servers from this client */
  cr.crit = IPC_ROUTER_CRITERIA_SERVICE;
  cr.arguments.svc_arg.service = service;
  cr.arguments.svc_arg.compare = compare;
  cr.arguments.svc_arg.compare_data = compare_data;

  ipc_router_list_servers(&cr, &results);

  while(NULL != (server_list = LIST_HEAD(results.servers)))
  {
    if(*num_servers < to_fill)
    {
      /* fill output array */
      servers[*num_servers].service = server_list->service;
      servers[*num_servers].addr = server_list->addr;
      (*num_entries)++;
    }
    (*num_servers)++;

    LIST_REMOVE(results.servers, server_list, link);
    ipc_router_os_free(server_list);
  }

  return *num_servers ? IPC_ROUTER_STATUS_SUCCESS : IPC_ROUTER_STATUS_FAILURE;
}

/*===========================================================================
FUNCTION      ipc_router_core_close

DESCRIPTION   Closes an opened handle to the router.  

ARGUMENTS     handle (in) - client handle returned from ipc_router_open

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  When close get's called, it's assumed that no client threads
              will be running in the router core
===========================================================================*/
int ipc_router_core_close
(
 ipc_router_client_type *client
)
{
  ipc_router_address_type addr;

  if(!client)
  {
    return IPC_ROUTER_STATUS_INVALID_PARAM;
  }

  addr.processor_id = ipc_router_local_processor_id;
  addr.port_id = client->port_id;

  ipc_router_send_remove_client(&addr, NULL);

  return ipc_router_remove_client(&addr);
}

/*===========================================================================
FUNCTION      ipc_router_core_init

DESCRIPTION   Initializes the IPC router.  

ARGUMENTS     None

RETURN VALUE  an IPC router error code as defined in ipc_router_types.h

SIDE EFFECTS  By default, the local transport is started.
===========================================================================*/
int ipc_router_core_init
(
 uint32 processor_id
 )
{
  ipc_router_local_processor_id = processor_id;

  ipc_router_packet_init();
  ipc_router_os_sem_init(&port_semaphore);
  ipc_router_os_sem_init(&protocol_semaphore);

  if(IPC_ROUTER_STATUS_SUCCESS != ipc_router_database_init()){
    /* Failed to initialize router database error message */
    return IPC_ROUTER_STATUS_UNKNOWN;
  }

  return IPC_ROUTER_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      ipc_router_core_deinit

DESCRIPTION   deintializes the IPC router.  

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_core_deinit
(
 void
)
{   
  ipc_router_xport_list_type *xport_list;
  ipc_router_search_criteria_type cr;
  ipc_router_search_results_type results;

  /* Get a list of currently open tranports */
  cr.crit = IPC_ROUTER_CRITERIA_ALL;
  ipc_router_list_xports(&cr, &results);

  while(NULL != (xport_list = LIST_HEAD(results.xports)))
  {
    ipc_router_xal_stop_xport(xport_list->xport);
    LIST_REMOVE(results.xports, xport_list, link);
    ipc_router_os_free(xport_list);
  }

  ipc_router_database_deinit();

  ipc_router_os_sem_deinit(&port_semaphore);
  ipc_router_os_sem_deinit(&protocol_semaphore);
}
