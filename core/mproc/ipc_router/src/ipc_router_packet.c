/*===========================================================================

                    I P C    R O U T E R   P A C K E T

        This file provides an abstraction to the packet buffer manager

 Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_packet.c#2 $
$DateTime: 2013/07/11 18:00:48 $
$Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_os.h"
#include "ipc_router_packet.h"

/* Packets will be at least this big */
#define IPC_ROUTER_PACKET_DEFAULT_SIZE (128)

/* Packets are always aligned so the sizes should be a multiple of 4 */
#define WORD_ALIGNED(size) (((size) + 3) & (~3UL))

#define IPC_ROUTER_MIN(a,b) ((a) < (b) ? (a) : (b))

#define IPC_ROUTER_PACKET_MAGIC_START 0xdeadbeef
#define IPC_ROUTER_PACKET_MAGIC_END   0xbeefdead

#define MAGIC_END_PTR(pkt) \
  ((uint32 *)(((uint8 *)(pkt)) + sizeof(ipc_router_packet_type) + (pkt)->allocated_size))


/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/

/* Notes: Unless something changes in smd_lite, (ipc_router_packet_type *) 
 * can be cast as (smdl_iovec_type *) as the first three members are the
 * same. Obviously the other way around is not possible!
 */
struct ipc_router_packet_s
{
  ipc_router_packet_type *next;
  uint32                 used;
  void                   *data_ptr;
  size_t                 size;
  size_t                 allocated_size;
  ipc_router_packet_type *pkt_next;
  uint32                 magic_start;
};

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/

static ipc_router_packet_type *ipc_router_packet_alloc(uint32 size)
{
  ipc_router_packet_type *ret = NULL;

  size = WORD_ALIGNED(size);

  ret = ipc_router_os_malloc(sizeof(ipc_router_packet_type) + size + sizeof(uint32));
  if(!ret)
    return NULL;
  ipc_router_os_mem_set(ret, 0, sizeof(ipc_router_packet_type));
  ret->next = NULL;
  ret->used = 0;
  ret->allocated_size = ret->size = size;
  ret->data_ptr = ((uint8 *)ret) + sizeof(ipc_router_packet_type);
  ret->pkt_next = NULL;
  ret->magic_start = IPC_ROUTER_PACKET_MAGIC_START;
  *MAGIC_END_PTR(ret) = IPC_ROUTER_PACKET_MAGIC_END;
  
  return ret;
}

static void ipc_router_packet_release(ipc_router_packet_type *pkt)
{
  if(pkt)
  {
    if(pkt->magic_start != IPC_ROUTER_PACKET_MAGIC_START ||
        *MAGIC_END_PTR(pkt) != IPC_ROUTER_PACKET_MAGIC_END)
    {
      IPC_ROUTER_OS_MESSAGE(IPC_ROUTER_MSG_ERR, "ipc_router_packet: WARNING memory corruption detected!\n", 0, 0, 0);
    }
    ipc_router_os_free(pkt);
  }
}

/*===========================================================================
                        EXPORTED FUNCTIONS
===========================================================================*/

void ipc_router_packet_init(void)
{
  /* Nothing to be initialized */
}

ipc_router_packet_type *ipc_router_packet_new(void)
{
  return ipc_router_packet_alloc(IPC_ROUTER_PACKET_DEFAULT_SIZE);
}

void ipc_router_packet_free(ipc_router_packet_type **pkt)
{
  ipc_router_packet_type *p;
  if(!pkt)
  {
    return;
  }
  p = *pkt;
  while(p)
  {
    ipc_router_packet_type *to_free = p;
    p = p->next;
    ipc_router_packet_release(to_free);
  }
  *pkt =  NULL;
}

ipc_router_packet_type *ipc_router_packet_new_headroom(uint32 headroom)
{
  ipc_router_packet_type *pkt = NULL;
  uint32 packet_size = headroom;

  packet_size = headroom = WORD_ALIGNED(headroom);
  if(packet_size < IPC_ROUTER_PACKET_DEFAULT_SIZE)
  {
    packet_size = IPC_ROUTER_PACKET_DEFAULT_SIZE;
  }

  pkt = ipc_router_packet_alloc(packet_size);
  if(!pkt)
  {
    return NULL;
  }

  /* Create headroom */
  pkt->data_ptr = (uint8 *)pkt->data_ptr + headroom;
  pkt->size -= headroom;

  return pkt;
}
/*===========================================================================
FUNCTION      ipc_router_packet_expand

DESCRIPTION   Expands a packet to the required size.
              (If the packet is already larger than the required bytes, then
              it wont be modified).

              pkt   - The packet to expand.
              bytes - The size of the expanded (final) packet.

RETURN VALUE  The total size of the returned packet.

SIDE EFFECTS  Input packet will be modified.
===========================================================================*/
uint32 ipc_router_packet_expand(ipc_router_packet_type **pkt, uint32 bytes)
{
  uint32 allocated_bytes = 0;
  ipc_router_packet_type *in_pkt = NULL, *ret = NULL, *prev = NULL;

  if(!pkt)
  {
    return 0;
  }

  in_pkt = *pkt;

  while(allocated_bytes < bytes)
  {
    uint32 this_pkt_len = bytes - allocated_bytes;
    ipc_router_packet_type *item = NULL;
    if(in_pkt)
    {
      item = in_pkt;
      in_pkt = in_pkt->next;
    }
    else
    {
      item = ipc_router_packet_alloc(this_pkt_len);
    }

    if(!item)
    {
      break;
    }

    this_pkt_len = IPC_ROUTER_MIN(this_pkt_len, item->size);
    item->used = this_pkt_len;
    allocated_bytes += this_pkt_len;

    if(!ret)
    {
      ret = prev = item;
    }
    else
    {
      prev->next = item;
      prev = item;
    }
  }
  *pkt = ret;

  return allocated_bytes;
}

uint32 ipc_router_packet_copy_header(ipc_router_packet_type **pkt, void *_buf, uint32 len)
{
  uint32 headroom = 0;
  uint8 *buf = (uint8 *)_buf;
  ipc_router_packet_type *p;

  if(!pkt || !buf || !len)
  {
    return 0;
  }

  p = *pkt;
  if(p)
  {
    headroom = (uint8 *)p->data_ptr - ((uint8 *)p + sizeof(ipc_router_packet_type));

    /* See if we can adjust the new data in the existing packet's headroom */
    if(headroom >= len)
    {
      p->data_ptr = (uint8 *)p->data_ptr - len;
      p->size += len;
      p->used += len;
      ipc_router_os_mem_copy(p->data_ptr, buf, len);
      return len;
    }
  }

  p = ipc_router_packet_alloc(len);
  if(!p)
  {
    return 0;
  }

  ipc_router_os_mem_copy(p->data_ptr, buf, len);
  p->used = len;
  p->next = *pkt;
  *pkt = p;

  return len;
}

uint32 ipc_router_packet_copy_payload(ipc_router_packet_type **pkt, void *_buf, uint32 len)
{
  uint8 *buf = (uint8 *)_buf;
  ipc_router_packet_type *p = NULL, *last = NULL;
  uint32 ret = 0;
  if(!pkt || !buf || !len)
  {
    return 0;
  }
  last = *pkt;
  if(last)
  {
    while(last->next != NULL)
    {
      last = last->next;
    }
    /* Some space is remaining */
    if(last->size > last->used)
    {
      uint32 to_put = IPC_ROUTER_MIN(last->size - last->used, len);
      ipc_router_os_mem_copy((uint8 *)last->data_ptr + last->used, buf, to_put);
      buf += to_put;
      last->used += to_put;
      len -= to_put;
      ret = to_put;
    }
  }

  if(len)
  {
    p = ipc_router_packet_alloc(len);
    if(!p)
    {
      return ret;
    }
    ipc_router_os_mem_copy(p->data_ptr, buf, len);
    p->used = len;
    p->next = NULL;
    ret += len;
    if(last)
    {
      last->next = p;
    }
    else
    {
      *pkt = p;
    }
  }

  return ret;
}

uint32 ipc_router_packet_copy_tail(ipc_router_packet_type **pkt, void *buf, uint32 len)
{
  return ipc_router_packet_copy_payload(pkt, buf, len);
}


uint32 ipc_router_packet_length(ipc_router_packet_type *pkt)
{
  uint32 len = 0;
  while(pkt)
  {
    len += pkt->used;
    pkt = pkt->next;
  }
  return len;
}

uint32 ipc_router_payload_length(ipc_router_packet_type *pkt)
{
  return ipc_router_packet_length(pkt);
}

void *ipc_router_packet_peek_header(ipc_router_packet_type *pkt)
{
  if(!pkt)
  {
    return NULL;
  }

  return pkt->data_ptr;
}

void ipc_router_packet_trim(ipc_router_packet_type **pkt, uint32 len)
{
  ipc_router_packet_type *p;
  uint32 pkt_len;
  uint32 cur_pkt_size = 0;

  if(!pkt)
  {
    return;
  }

  if(len == 0)
  {
    ipc_router_packet_free(pkt);
    return;
  }

  p = *pkt;
  pkt_len = ipc_router_packet_length(p);

  /* Nothing to trim. Packet smaller than or
   * equal to the required len */
  if(pkt_len <= len)
  {
    return;
  }

  while(p)
  {
    cur_pkt_size += p->used;
    if(cur_pkt_size >= len)
    {
      p->used -= (cur_pkt_size - len);
      ipc_router_packet_free(&p->next);
      p->next = NULL;
      break;
    }
    p = p->next;
  }
}

/* Read len bytes from head  into buf and free the read buffers */
uint32 ipc_router_packet_read_header(ipc_router_packet_type **pkt, void *_buf, uint32 len)
{
  uint32 read_bytes = 0;
  uint8 *buf = (uint8 *)_buf;
  ipc_router_packet_type *p;

  if(!pkt || !buf || !len)
  {
    return 0;
  }

  p = *pkt;

  while(read_bytes < len)
  {
    uint32 to_read = len - read_bytes;
    ipc_router_packet_type *to_free;
    if(!p)
    {
      return read_bytes;
    }
    to_free = p;
    to_read = IPC_ROUTER_MIN(to_read, p->used);
    ipc_router_os_mem_copy(buf, p->data_ptr, to_read);

    p->used -= to_read;
    p->data_ptr = (uint8 *)p->data_ptr + to_read;
    read_bytes += to_read;
    buf += to_read;

    p = p->next;

    /* If this packet is empty free it, and set the new head */
    if(to_free->used == 0)
    {
      ipc_router_packet_release(to_free);
      *pkt = p;
    }
  }
  return read_bytes;
}

/* Read len bytes from tail into buf and free the read buffers */
uint32 ipc_router_packet_read_payload(ipc_router_packet_type **pkt, void *_buf, uint32 len)
{
  uint32 pkt_len = 0;
  uint32 read_bytes = 0;
  uint8 *buf = (uint8 *)_buf;
  ipc_router_packet_type *p = NULL;
  ipc_router_packet_type *prev = NULL;
  uint32 skip_len = 0;
  uint32 skipped_len = 0;

  if(!pkt || !buf || !len)
  {
    return 0;
  }

  pkt_len = ipc_router_packet_length(*pkt);

  /* We cannot read more than this */
  len = IPC_ROUTER_MIN(pkt_len, len);
  p = *pkt;
  skip_len = pkt_len - len;


  while(skipped_len < skip_len)
  {
    skipped_len += p->used;
    if(skipped_len <= skip_len)
    {
      prev = p;
      p = p->next; /* No need to check */
    }
  }

  /* The above while loop can skip more than required */
  if(skipped_len > skip_len)
  {
    uint32 read_from_pkt = skipped_len - skip_len;
    uint8 *ptr = (uint8 *)p->data_ptr + p->used - read_from_pkt;
    read_bytes += read_from_pkt;
    ipc_router_os_mem_copy(buf, ptr, read_from_pkt);
    buf += read_from_pkt;
    p->used -= read_from_pkt;
    len -= read_from_pkt;
    prev = p;
    p = p->next;
  }

  /* Cut the existing packet */
  if(prev)
  {
    prev->next = NULL;
  }

  while(p)
  {
    ipc_router_packet_type *to_free = p;
    ipc_router_os_mem_copy(buf, p->data_ptr, p->used);
    buf += p->used;
    read_bytes += p->used;
    p = p->next;
    ipc_router_packet_release(to_free);
  }

  /* One condition which wont appropriately set *ptr = NULL */
  if(skip_len == 0)
  {
    *pkt = NULL;
  }

  return read_bytes;
}

void ipc_router_packet_queue_init(ipc_router_packet_queue_type *queue)
{
  ipc_router_os_sem_init(&queue->lock);
  queue->head = queue->tail = NULL;
  queue->cnt = 0;
}

void ipc_router_packet_queue_deinit(ipc_router_packet_queue_type *queue)
{
  ipc_router_packet_type *pkt;
  ipc_router_os_sem_lock(&queue->lock);
  pkt = queue->head;
  while(pkt)
  {
    ipc_router_packet_type *to_free = pkt;
    pkt = pkt->pkt_next;
    ipc_router_packet_free(&to_free);
  }
  queue->head = queue->tail = NULL;
  ipc_router_os_sem_unlock(&queue->lock);
  ipc_router_os_sem_deinit(&queue->lock);
}

void ipc_router_packet_queue_put(ipc_router_packet_queue_type *queue, ipc_router_packet_type *pkt)
{
  if(!queue || !pkt)
  {
    return;
  }
  ipc_router_os_sem_lock(&queue->lock);
  pkt->pkt_next = NULL;
  if(!queue->tail)
  {
    queue->head = queue->tail = pkt;
  }
  else
  {
    queue->tail->pkt_next = pkt;
    queue->tail = pkt;
  }
  queue->cnt++;
  ipc_router_os_sem_unlock(&queue->lock);
}

ipc_router_packet_type *ipc_router_packet_queue_get(ipc_router_packet_queue_type *queue)
{
  ipc_router_packet_type *ret = NULL;

  if(!queue)
  {
    return NULL;
  }

  ipc_router_os_sem_lock(&queue->lock);
  if(queue->head)
  {
    ret = queue->head;
    queue->head = queue->head->pkt_next;
    ret->pkt_next = NULL;
    queue->cnt--;
    if(queue->cnt == 0)
    {
      queue->tail = NULL;
    }
  }
  ipc_router_os_sem_unlock(&queue->lock);
  return ret;
}

