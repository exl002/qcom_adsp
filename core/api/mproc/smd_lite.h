#ifndef SMD_LITE_H
#define SMD_LITE_H

/**
 * @file smd_lite.h
 *
 * Public API for the lightweight shared memory driver.
 */

/** \defgroup smd_lite SMD Lite
 * \ingroup SMD
 *
 * SMD Lite provides reliable, in-order, datagram-based interprocessor
 * communication over shared memory. Clients receive notification via a
 * callback when the port's status changes and may call the read or write
 * functions from the callback or notify their own threads for further
 * processing.
 *
 * All ports preserve message boundaries across the interprocessor channel; one
 * write into the port exactly matches one read from the port.
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2009-2013 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/api/mproc/smd_lite.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/13   hwu     Added partial packet read support.
10/03/12   bt      Add smd_is_port_open API.
08/02/12   pa      Added new SMDL open API and associated params struct
07/31/12   pa      Added SMDL_EVENT_LAST
07/26/11   tl      Added SMD Lite IST priority
05/03/11   tl      Include smd_type.h for smd_channel_type definition
05/02/11   tl      Added SMDL_EVENT_CLOSE_COMPLETE and
                   SMDL_EVENT_REMOTE_CLOSE. Clarified documentation on closing
                   ports.
04/18/11   tl      Clarified documentation on SMDL_EVENT_OPEN
03/25/11   bt      Added events and signals for CD and RI signals.
02/14/11   tl      Updated smdl_readv() documentation for long packets
02/08/11   tl      Added flags for long-message read and write
01/27/11   tl      Added vector write function smdl_writev()
01/25/11   bt      Added RS-232 related signals, events, and functions.
01/24/11   tl      Added vector read function smdl_readv()
01/12/11   bt      Added enum and smdl_open flags for transfer mode.  Updated
                   function headers to address packet and streaming mode.
01/06/11   bt      Corrected header comment to leave 4 bytes free, not 1. 
10/07/10   tl      Clarified return value of smdl_tx_peek()
07/21/10   tl      Added SMD Lite API for SMD priority.
02/24/10   tl      Updated API to SMD Lite v2
04/20/09   tl      Initial release of SMD Lite
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "smd_type.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/**
 * This preprocessor symbol can be used to test for the minor differences
 * between SMD Lite API v1 and SMD Lite API v2 at compile time. The only
 * difference is smdl_open(): In v2, the argument smd_channel_type edge has
 * been added.
 */
#define SMD_LITE_API_V2

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

/**
 * Internal data structure for SMD Lite ports.
 *
 * This structure should not be used directly by clients; they should treat
 * smdl_handle_type as an opaque handle.
 */
struct smd_stream_info_struct;

/**
 * Opaque handle pointing to the internal data structures.
 *
 * Clients must not take advantage of the internal structure of this handle.
 */
typedef struct smd_stream_info_struct * smdl_handle_type;

/**
 * I/O vector type
 *
 * Clients using the vector read and write functions, smdl_readv() and
 * smdl_writev(), must create a structure of this type for each buffer that
 * makes up the vector.
 */
typedef struct smdl_iovec_struct {
  /** Pointer to the next I/O vector structure, or NULL if this is the last
   * buffer in the chain. */
  struct smdl_iovec_struct *next;

  /** The length, in bytes, of this element of the vector. */
  uint32  length;

  /** The data pointer of this element of the vector. */
  void   *buffer;
} smdl_iovec_type;

/** The notification events clients receive from SMD */
typedef enum
{
  /** No event. */
  SMDL_EVENT_NONE = 0,

  /** Called when the remote processor asserts its Data Terminal Ready (DTR)
   * signal, which normally means that the port is open. See smdl_open(). */
  SMDL_EVENT_OPEN,

  /** Called when data is available to be read from the FIFO.
   *
   * SMD Lite will send this event to the client when there is new data
   * available to be read, but might not explicitly notify the client for each
   * packet received. When clients receive this event, they must read as much
   * data as is available by calling smdl_read() or smdl_readv() in a loop to
   * avoid leaving data in the receive FIFO when multiple packets are received
   * at once. See smdl_read(), smdl_readv(), and smdl_rx_peek(). */
  SMDL_EVENT_READ,

  /** Called when space is available in the FIFO after a failed write.
   * See smdl_write() and smdl_writev(). */
  SMDL_EVENT_WRITE,

  /** Called when SMD Lite has finished closing the port after the client
   * calls smdl_close(). After the client receives this event, the port may be
   * immediately reopened.
   * See smdl_close(). */
  SMDL_EVENT_CLOSE_COMPLETE,

  /** Called when SMD Lite has finished closing the port after the client
   * calls smdl_close(). After the client receives this event, the port may be
   * immediately reopened. This event is provided for backward compatability
   * with existing code.
   * See smdl_close(). */
  SMDL_EVENT_CLOSE = SMDL_EVENT_CLOSE_COMPLETE,
  
  /** Event generated when the remote processor changes its Data Terminal
   * Ready (DTR) signal.
   *
   * Most SMD clients use the DTR signal to indicate that they have opened the
   * port and are ready to transmit and receive data. smdl_open() asserts the
   * DTR signal when the port is opened, but clients using the SMD memcpy API
   * or the SIO API must call an ioctl to assert the DTR signal.
   *
   * When receiving this event, clients may use smdl_sig_get() to query the
   * current state of the remote processor's DTR signal. Reading the remote
   * processor's DTR signal is considered equivalent to reading the channel's
   * Data Set Ready (DSR) signal.
   *
   * See smdl_sig_get() and smdl_sig_set(). */
  SMDL_EVENT_DSR,
  
  /** Event generated when the remote processor changes its Clear To Send
   * (CTS) signal.
   *
   * Before SMD can transmit data on a channel, the remote processor must
   * assert its CTS signal, which is referred to as "enabling inbound flow".
   * smdl_open() asserts the CTS signal when the port is opened, but clients
   * using the SMD memcpy API or the SIO API must call an ioctl to assert the
   * CTS signal.
   *
   * When receiving this event, clients may use smdl_sig_get() to query the
   * current state of the remote processor's CTS signal. Reading the remote
   * processor's CTS signal is considered equivalent to reading the channel's
   * Ready To Send (RTS) signal.
   *
   * See smdl_sig_get() and smdl_sig_set(). */
  SMDL_EVENT_CTS,
  
  /** Event generated when the remote processor changes its Carrier Detect
   * (CD) signal.
   *
   * When receiving this event, clients may use smdl_sig_get() to query the
   * current state of the remote processor's CD signal.
   *
   * SMD does not use the CD signal but does allow clients to use the legacy
   * RS-232 signal. smdl_open() asserts the CD signal when the port is opened,
   * but clients using the SMD memcpy API or the SIO API must call an ioctl to
   * assert the CD signal.
   *
   * See smdl_sig_get() and smdl_sig_set(). */
  SMDL_EVENT_CD,
  
  /** Event generated when the remote processor changes its Ring Indicator
   * (RI) signal. 
   *
   * When receiving this event, clients may use smdl_sig_get() to query the
   * current state of the remote processor's RI signal.
   *
   * SMD does not use the RI signal but does allow clients to use the legacy
   * RS-232 signal.
   *
   * See smdl_sig_get() and smdl_sig_set(). */
  SMDL_EVENT_RI,

  /** Event generated when the remote processor closes the port. After the
   * remote processor closes the port, no further data may be transferred on
   * the port. Clients may leave the port half-opened and wait for the remote
   * processor to reconnect, at which point SMD Lite will generate
   * #SMDL_EVENT_OPEN.
   */
  SMDL_EVENT_REMOTE_CLOSE,

  /** Called in packet mode, when partial packet is available to be read.
   *
   * SMD Lite will send this event to the client when there is partial data 
   * available in packet mode. When clients receive this event, they must read as 
   * much data as is available by calling smdl_readv() in a loop to read data. 
   * See smdl_read(), smdl_readv(), and smdl_rx_peek(). */
  SMDL_EVENT_READ_PARTIAL,
 
  /** Last event in the enum */
  SMDL_EVENT_LAST = SMDL_EVENT_READ_PARTIAL,
} smdl_event_type;

/** The type of RS-232 signal to set, clear, or query. */
typedef enum {
  /** The DTR/DSR signals. */
  SMDL_SIG_DTR_DSR,
  
  /** The RTS/CTS signals. */
  SMDL_SIG_RTS_CTS,
  
  /** The CD signal. */
  SMDL_SIG_CD,
  
  /** The RI signal. */
  SMDL_SIG_RI,
} smdl_sig_type;

/** \name Open flags
 *
 * These flags may be used to control the behavior of the port opened by
 * smdl_open().
 *
 * @{
 */

/** Empty flags definition for smdl_open().
 *
 * This may be passed as the flags argument of smdl_open() to indicate no
 * special operations are requested.
 */
#define SMDL_OPEN_FLAGS_NONE    0x00000000

/** Bitmask of the smdl_open() flags for specifying priority.
 *
 * How the priority parameters are interpreted varies depending on the
 * particular implementation of SMD Lite. If SMD Lite is running on a
 * processor with SMD, SMD Lite ports are processed from the SMD tasks. If SMD
 * is implemented using multiple tasks for ports of different priorities, the
 * port priority requested here will control which SMD task the SMD Lite port
 * uses. High-priority ports are processed from the high-priority SMD task,
 * default-priority ports are processed from the default-priority SMD task,
 * and low-priority ports are processed from the low-priority SMD task.  See
 * \ref smd_priority_type for further discussion regarding SMD priorities.
 *
 * If SMD Lite is running on a processor with SMD, but without multiple SMD
 * tasks, the priority will be ignored and all SMD Lite ports will be
 * processed from the one SMD task.
 *
 * If SMD Lite is running on a processor without SMD, the priority will be
 * ignored, and all SMD Lite ports will be processed from the SMD Lite ISR.
 */
#define SMDL_OPEN_FLAGS_PRIORITY_MASK           0x00000003

/** SMD Lite open flag for default priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_DEFAULT        0x00000000

/** SMD Lite open flag for low priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_LOW            0x00000001

/** SMD Lite open flag for high priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_HIGH           0x00000002

/** SMD Lite open flag for IST priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_IST            0x00000003

/** Bitmask of the smdl_open() flags for specifying transfer mode.
 * 
 * The mode can be selected as either Packet or Streaming mode.  Packet mode  
 * is the default.
 */
#define SMDL_OPEN_FLAGS_MODE_MASK               0x00000004

/** Open the port in packet mode.  This is the default mode.
 * 
 * Data is sent in packets of no more than a max size, each preceded by a 
 * packet header.
 */
#define SMDL_OPEN_FLAGS_MODE_PACKET             0x00000000

/** Open the port in streaming mode.
 * 
 * Allows an arbitrary amount of data to be read or written without packet 
 * headers, while message boundaries are not preserved.
 */
#define SMDL_OPEN_FLAGS_MODE_STREAMING          0x00000004

/*@}*/

/** \name Read flags
 *
 * These flags may be used to control the behavior of smdl_read(). No flags
 * are defined at this time; clients may pass 0 to the flags argument or use
 * the empty-flag value #SMDL_READ_FLAGS_NONE.
 *
 * @{
 */

/** Empty flags definition for smdl_read().
 *
 * This may be passed as the flags argument of smdl_read() to indicate no
 * special operations are requested.
 */
#define SMDL_READ_FLAGS_NONE    0x00000000

/** Allows smdl_readv() to read more data than will fit in the FIFO.
 *
 * This flag may be bitwise-ored together with other read flags.
 */
#define SMDL_READ_FLAG_LONG     0x00000002

/*@}*/

/** \name Write flags
 *
 * These flags may be used to control the behavior of smdl_write().
 *
 * @{
 */

/** Empty flags definition for smdl_write().
 *
 * This may be passed as the flags argument of smdl_write() to indicate no
 * special operations are requested.
 */
#define SMDL_WRITE_FLAGS_NONE   0x00000000

/** Does not send an interrupt to the remote processor on write.
 *
 * Clients may use this flag when they are writing a long sequence of packets
 * and they wish to avoid waking up the remote processor until all the data is
 * written. No interrupt will be sent when this flag is set, but the remote
 * processor may read data from the channel.
 *
 * This flag may be bitwise-ored together with other write flags.
 */
#define SMDL_WRITE_FLAG_NOINT   0x00000001

/** Allows smdl_writev() to write more data than will fit in the FIFO.
 *
 * This flag may be bitwise-ored together with other write flags.
 */
#define SMDL_WRITE_FLAG_LONG    0x00000002

/*@}*/

/** Return code signaling continued read or write
 *
 * This value is returned by smdl_writev() and smdl_readv() when the whole
 * message could not be written to or read from shared memory and the client
 * must retry the write or read after receiving the appropriate event.
 */
#define SMDL_CONTINUE           (int32)(0x80000000)

/**
 * Notification callback when SMD needs to communicate asynchronously with
 * the client.
 *
 * This callback function may be called from interrupt context; clients must
 * not block or call any functions that block. Clients may call any of the
 * following SMD Lite functions from the callback:
 *
 * @li smdl_read()
 * @li smdl_readv()
 * @li smdl_write()
 * @li smdl_writev()
 * @li smdl_rx_peek()
 * @li smdl_tx_peek()
 * @li smdl_sig_set()
 * @li smdl_sig_get()
 * @li smdl_close()
 *
 * @param[in] port   The port on which the event occurred
 * @param[in] event  The event that occurred
 * @param[in] data   The user-supplied data provided to smdl_open()
 *
 * @return void
 */
typedef void (*smdl_callback_t) (
                                  smdl_handle_type port,
                                  smdl_event_type  event,
                                  void            *data
                                );

/** Type for passing parameters to smdl_open_ex */
typedef struct
{
  /* Name of the port. */
  const char       *name;

  /* The edge, identifying both processors, on which this channel communicates.
   * The edge must include the current processor. */
  smd_channel_type  edge;

  /* A bitfield of flags controlling the behavior of the port.
   * #SMDL_OPEN_FLAGS_NONE
   * #SMDL_OPEN_FLAGS_PRIORITY_DEFAULT
   * #SMDL_OPEN_FLAGS_PRIORITY_LOW
   * #SMDL_OPEN_FLAGS_PRIORITY_HIGH
   * #SMDL_OPEN_FLAGS_PRIORITY_IST
   * #SMDL_OPEN_FLAGS_MODE_PACKET
   * #SMDL_OPEN_FLAGS_MODE_STREAMING */
  uint32            flags;

  /* The size of the FIFO. This is the physical size in memory, which is always
   * four bytes greater than the total capacity of the FIFO. In packet mode, the
   * SMD packet header adds 20 bytes per packet, further reducing the capacity.
   * The FIFO size must be a multiple of 32 and must not be less than
   * SMD_MIN_FIFO or greater than SMD_MAX_FIFO.
   * If this parameter is 0, the default size (SMD_STANDARD_FIFO) will be used.
   */
  uint32            fsize;

  /* The callback function called when SMD needs to notify the client of a
   * remote event on the SMD port. */
  smdl_callback_t   notify;

  /* The data passed to the notify function. */
  void             *cb_data;

  /* Handle returned to client after the port is allocated. */
  smdl_handle_type  handle;

  /* Extended status information in case of failure.
   * When SMD_STATUS_SIZE_MISMATCH is returned, this field contains the
   * existing fifo size. */
  uint32            ext_status;

} smdl_open_params_type;

/*=========================================================================== 
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/** 
 * Initializes the SMD Lite system.
 *
 * This function must be called by the system startup code before any other SMD
 * Lite functions are called.
 *
 * This function initializes local and shared data structures for SMD Lite and
 * registers the necessary interrupt handlers.
 *
 * @param[in]    reserved  Must be 0
 *
 * @return       0 if successful
 * @return       A negative error code if an error occurred.
 *
 * @sideeffects  Registers interrupt handlers, writes to shared memory, and
 *               signals the remote processor.
 */
int32 smdl_init
(
  uint32 reserved
);

/** 
 * Shuts down the SMD Lite system.
 *
 * This function is provided to terminate the SMD Lite system in test
 * environments. It is not intended to be used on production systems. After
 * this function is called, all handles will be invalidated and clients must
 * call smdl_init() to initialize the system.
 *
 * This function MUST NOT be called if a call to smdl_init() is outstanding in
 * another thread.
 *
 * @param[in]    reserved  Must be 0
 *
 * @return       0 if successful
 * @return       A negative error code if an error occurred.
 */
int32 smdl_term
(
  uint32 reserved
);

/** 
 * Opens a port to the specified remote processor.
 *
 * Opens the port named \c name between the processors specified by \c edge.
 * (The current processor must be one of the two processors connected by \c
 * edge.) This function allocates the data structures for the ports in local
 * and shared memory and allocates FIFOs for receive and transmit, each of
 * size \c fsize.
 *
 * Clients may not read from or write to the port until it is opened on both
 * ends. When the other processor opens the port and asserts its Data Terminal
 * Ready (DTR) signal the \c notify callback will be called with argument
 * #SMDL_EVENT_OPEN. (The DTR signal is asserted by default when the port is
 * opened using smdl_open(), but if the other end of the port is opened using
 * the SMD memcpy API or the SIO API, the remote client must call an ioctl to
 * assert its DTR signal.) If the other end is already open and has already
 * asserted its DTR signal when this function is called, the open notification
 * callback will be called immediately, and may be called before this function
 * returns.
 *
 * If a client wishes to know when the SMD port is fully open, and the other
 * end asserts its DTR signal when opening the port, the client should wait
 * for #SMDL_EVENT_OPEN. If a client wishes to know when the SMD port is fully
 * open, and the other end does not assert its DTR signal when opening the
 * port, the client should wait for #SMDL_EVENT_CTS, taking care that the
 * other end may set and clear its Clear To Send (CTS) signal to enable and
 * disable inbound flow during normal operation. If a client doesn't need to
 * know when the port is open, and merely wishes to transmit data, the client
 * should send data after opening the port. If the port is fully open, SMD
 * Lite will transmit the data immediately. If the port is not fully open, SMD
 * Lite will notify the client when data can be written (when the remote
 * processor asserts its CTS signal) by sending #SMDL_EVENT_WRITE.
 *
 * Port names uniquely identify the port connecting two processors across a
 * single edge. The same name may be used along different edges. The special
 * name "LOOPBACK" will cause the opposite end to echo data back as it is 
 * written to the port. Only one loopback port may be opened on a single edge.
 * Loopback ports are only intended for testing, not for general use.
 *
 * @param[in]    name     The name for the port.
 * @param[in]    edge     The edge, identifying both processors, on which this
 *                        channel communicates. The edge must include the
 *                        current processor.
 * @param[in]    flags    A bitfield of flags controlling the behavior of the
 *                        port. See #SMDL_OPEN_FLAGS_NONE,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_DEFAULT,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_LOW,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_HIGH,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_IST,
 *                        #SMDL_OPEN_FLAGS_MODE_PACKET, and
 *                        #SMDL_OPEN_FLAGS_MODE_STREAMING.
 * @param[in]    fsize    The size of the FIFO. This is the physical
 *                        size in memory, which is always four bytes greater 
 *                        than the total capacity of the FIFO. In packet mode, 
 *                        the SMD packet header adds 20 bytes per packet, 
 *                        further reducing the capacity. The FIFO size must be
 *                        a multiple of 32 and must not be less than 
 *                        SMD_MIN_FIFO or greater than SMD_MAX_FIFO. If this 
 *                        parameter is 0, the default size (SMD_STANDARD_FIFO) 
 *                        will be used.
 * @param[in]    notify   The callback called when SMD needs to notify the
 *                        client of a remote event on the SMD port.
 * @param[in]    cb_data  The data passed to the notify function.
 *
 * @return       An opaque SMD handle for the opened port, or NULL if an
 *               error occured while opening the port.
 *
 * @sideeffects  Allocates the local and shared data structures for the SMD
 *               port. Signals the remote processor.
 *               Error fatal occurs when SMDL port is already open (by other
 *               processor) with a different fifo size.
 */
smdl_handle_type smdl_open
(
  const char       *name,
  smd_channel_type  edge,
  uint32            flags,
  uint32            fsize,
  smdl_callback_t   notify,
  void             *cb_data
);

/**
 * Opens a port to the specified remote processor.
 *
 * This function is the underlying function of smdl_open() and has the same
 * behavior, but with a more extensible and flexible API.
 * Currently, the only behavioral difference between this function and
 * smdl_open is that this function does not fatal error when specified fifo size
 * does not match the existing size in SMEM.
 *
 * @param[inout] params   Parameter structure.  See definition in smd_lite.h.
 *                        smd_lite_?handle type is returned in the param struct
 *                        and that the client should save it in order to do 
 *                        further operations on the port.
 *
 * @return       SMD_STATUS_SUCCESS if successful.
 * @return       SMD_STATUS_INVALID_PARAM if input params are incorrect.
 *               No work is done in this case.
 * @return       SMD_STATUS_SIZE_MISMATCH if fifo size does not match currently
 *               allocated fifo in SMEM.  Param field ext_status is populated
 *               with the existing size in this case.
 *               No work is done in this case.
 * @return       SMD_STATUS_ERROR if an error occurred.
 */
int32 smdl_open_ex
(
  smdl_open_params_type * params
);

/** 
 * Closes the port to the remote processor.
 *
 * Closes the port and signals the remote processor to close the port. No
 * more data may be transferred over the port after it is closed.
 *
 * After calling this function, clients must wait for
 * #SMDL_EVENT_CLOSE_COMPLETE before reopening the port.
 *
 * @param[in]    port  The port to close
 *
 * @return       0 if the port is closed
 * @return       A negative error code if an error occurred.
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
int32 smdl_close
(
  smdl_handle_type port
);

/** 
 * Returns the number of bytes available to be read from the port.
 *
 * Clients may use this function to quickly determine whether to read data from
 * the port, and to allocate the correct buffer size.
 *
 * @param[in]  port  The port to check for data.
 *
 * @return       Packet mode: The number of bytes to be read from the next 
 *               packet in the queue, or 0 if no packets are available.
 * @return       Streaming mode: The number of bytes available to be read from 
 *               the port.  (Determined by how many bytes have been written 
 *               so far and not yet read.)
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  None
 */
int32 smdl_rx_peek
(
  smdl_handle_type port
);

/** 
 * Peek the FIFO to see if there is data or packet pending.
 *
 * Clients may use this function to quickly determine whether to read data from
 * the port, and to allocate the correct buffer size.
 *
 * @param[in]  port         The port to check for data.
 * @param[out] pkt_len      Pointer to the packet length.
 * @param[out] bytes_avail  Packet Mode: Pointer to the number of bytes of the next 
 *                                       packet that is available in the FIFO.
 *                          Stream Mode: Pointer to the number of bytes available to 
 *                                       be read from the FIFO. 
 *
 * @return       Return 0 if success. Otherwise, return a negative error code. 
 *
 * @sideeffects  None
 */
int32 smdl_rx_peek_ext
(
  smdl_handle_type port,
  uint32          *pkt_len,
  uint32          *byte_avail
);

/** 
 * Returns the number of bytes that can be written to a port.
 *
 * Clients may use this function to quickly determine whether the packet they
 * wish to send will fit in the outgoing buffer.
 *
 * When the port is not fully opened, or the receiver has disabled inbound
 * flow, this function will return 0, as no data can be written to the port.
 *
 * @param[in]  port  The port to check for data.
 *
 * @return       The length, in bytes, of largest amount of data that can be 
 *               written to the port.
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  None
 */
int32 smdl_tx_peek
(
  smdl_handle_type port
);

/** 
 * Reads data from the SMD port into the provided buffer.
 *
 * Packet mode: This function will read up to \c len bytes of the next packet 
 * in the port into the provided buffer. If the packet is larger than the 
 * buffer, as much data as possible will be copied to the buffer and remainder 
 * will be discarded. Clients concerned about dropping data should call 
 * smdl_rx_peek() to determine the size of the next packet.
 *
 * Streaming mode: This function will read up to \c len bytes from the port 
 * into the provided buffer, limited by the value returned by smdl_rx_peek(). 
 * Any remaining bytes in the port will be left there for future reads.
 *
 * This function does not block; it will return 0 immediately if no data is
 * available.
 *
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]   port    The port to read data from
 * @param[in]   len     The number of bytes available in \c buffer
 * @param[out]  buffer  The buffer into which the data from the port should
 *                      be copied.
 * @param[in]   flags   Bitwise-or of flags controlling SMD's behavior. See
 *                      #SMDL_READ_FLAGS_NONE.
 *
 * @return       If data was read: The number of bytes read into \c buffer
 * @return       If no data was available: 0
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Reads from shared memory and signals the remote processor.
 */
int32 smdl_read
(
  smdl_handle_type port,
  uint32           len,
  void            *buffer,
  uint32           flags
);

/** 
 * Writes data from the provided buffer to the SMD port.
 *
 * Packet mode: The buffer is treated as a single packet to be sent to the 
 * remote processor. The packet will be sent in its entirety if space is 
 * available in the outgoing FIFO or will be dropped (with an appropriate error 
 * code) if space is not available. Attempting to send packets larger than the 
 * size of the outgoing FIFO will always fail.
 * 
 * Streaming mode: Up to \c len bytes from \c buffer will be written into the 
 * port, limited by the value returned by smdl_tx_peek(). This function will 
 * return the number of successfully written bytes. Any bytes that cannot fit 
 * in the outgoing FIFO will be truncated.
 *
 * Writing 0 bytes to the port will not change shared memory and will not
 * signal the remote processor.
 *
 * Clients interested in determining the maximum packet size that can be
 * written to the port may call smdl_tx_peek().
 *
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]  port    The port to write data to
 * @param[in]  len     The number of bytes to write to the port
 * @param[in]  buffer  The buffer to write to the port
 * @param[in]  flags   Bitwise-or of flags controlling SMD's behavior. See
 *                     #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
 *
 * @return       Return behavior depends on the transfer mode used.
 *               -Packet mode: 
 *                 -If the port has sufficient space to hold the 
 *                  provided packet: The length of the packet written to the 
 *                  port.
 *                 -If the port does not have sufficient space to 
 *                  hold the provided packet: 0.
 *               -Streaming mode: The number of bytes that were successfully 
 *                written to the port. If truncated, will be less than \c len.
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
int32 smdl_write
(
  smdl_handle_type port,
  uint32           len,
  const void      *buffer,
  uint32           flags
);

/** 
 * Reads data from the SMD port into the provided vector.
 *
 * Packet mode: This function will read the next packet in the port into the
 * provided buffer or buffers in the linked list of vectors \c iovec. Each
 * element of the I/O vector describes a buffer size and length, and a pointer
 * to the next buffer in the list. This function will copy as much of the
 * packet as possible into the first buffer in the chain, then continue
 * copying data until either the packet is complete or the buffers are
 * exhausted. If the packet is larger than the vector, as much data as
 * possible will be copied to the vector and remainder will be discarded.
 * Clients concerned about dropping data should call smdl_rx_peek() to
 * determine the size of the next packet.
 *
 * If the packet to be received is larger than the FIFO (specified when
 * opening the port using smdl_open()), and the #SMDL_READ_FLAG_LONG flag in
 * \c flags is not set, this function will read as much of the packet as is
 * possible in a single read, then discard the remainder of the packet. If the
 * #SMDL_READ_FLAG_LONG flag is set, this function will read as much data as
 * is available at a time, then return #SMDL_CONTINUE if more data must be
 * received before the read is complete. The caller is responsible for waiting
 * until it receives the #SMDL_EVENT_READ event callback, at which point the
 * caller must call smdl_readv() again, using the same arguments. This
 * function will read as much data as is possible, repeating as many times as
 * is necessary to read the entire packet, and ultimately return the size of
 * the entire packet when the entire packet has been read.
 *
 * Streaming mode: This function will read as much data as will fit into the
 * provided linked list of vector \c iovec from the port into the provided
 * buffer, limited by the value returned by smdl_rx_peek().  Any remaining
 * bytes in the port will be left there for future reads.
 *
 * Setting the #SMDL_READ_FLAG_LONG flag when a port has been opened in
 * streaming mode has no effect. This function will always read as much data
 * as is available and return the number of bytes read.
 *
 * This function does not block; it will return 0 immediately if no data is
 * available.
 *
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]   port    The port to read data from
 * @param[out]  iovec   Pointer to a linked list of I/O vectors for the packet
 *                      to be copied into.
 * @param[in]   flags   Bitwise-or of flags controlling SMD's behavior. See
 *                      #SMDL_READ_FLAGS_NONE and #SMDL_READ_FLAG_LONG.
 *
 * @return       If data was read: The total number of bytes read into \c iovec
 * @return       If no data was available: 0
 * @return       If #SMDL_READ_FLAG_LONG is specified in packet mode and the
 *               whole packet was not read: #SMDL_CONTINUE
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Reads from shared memory and signals the remote processor.
 */
int32 smdl_readv
(
  smdl_handle_type port,
  smdl_iovec_type *iovec,
  uint32           flags
);

/**
 * Writes data from the provided vector to the SMD port.
 *
 * Packet mode: The vector is treated as a single packet to be sent to the
 * remote processor. By default, the packet will be sent in its entirety if
 * space is available in the outgoing FIFO or will be dropped (with an
 * appropriate error code) if space is not available.
 *
 * Streaming mode: This function will write as much of the provided vector as
 * is possible, limited by the value returned by smdl_tx_peek(). This function
 * will return the number of successfully written bytes. Any bytes that cannot
 * fit in the outgoing FIFO will be truncated.
 *
 * In both streaming and packet mode, clients may specify the
 * #SMDL_WRITE_FLAG_LONG bit of \c flags to enable writing messages longer
 * than the FIFO. In this mode, smdl_writev() will write as much of the
 * message as is possible, then return #SMDL_CONTINUE. It is the
 * responsibility of the client to wait until it receives #SMDL_EVENT_WRITE,
 * then call smdl_writev() with the same arguments. smdl_writev() will
 * again write as much data as can be written. Once smdl_writev() has written
 * the entire message, it will return the size of the whole message.
 *
 * Writing 0 bytes to the port will not change shared memory and will not
 * signal the remote processor.
 *
 * Clients interested in determining the maximum packet size that can be
 * written to the port (without specifying #SMDL_WRITE_FLAG_LONG) may call
 * smdl_tx_peek().
 *
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]  port    The port to write data to
 * @param[in]  iovec   Pointer to a linked list of I/O vectors to be written
 *                     to the port.
 * @param[in]  flags   Bitwise-or of flags controlling SMD's behavior. See
 *                     #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
 *
 * @return       Return behavior depends on the transfer mode used.
 *               -If #SMDL_WRITE_FLAG_LONG is specified:
 *                 -If the whole message has been written: The size of the
 *                  message.
 *                 -If the whole message has not been written: #SMDL_CONTINUE.
 *               -Packet mode: 
 *                 -If the port has sufficient space to hold the 
 *                  provided packet: The length of the packet written to the 
 *                  port.
 *                 -If the port does not have sufficient space to 
 *                  hold the provided packet: 0.
 *               -Streaming mode: The number of bytes that were successfully 
 *                written to the port. If truncated, will be less than the
 *                total amount of data available in the vector.
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
int32 smdl_writev
(
  smdl_handle_type       port,
  const smdl_iovec_type *iovec,
  uint32                 flags
);

/*
 * Set or clear an RS-232 signal. 
 *
 * @param[in]    port    The port to set or clear the signal from.
 * @param[in]    sig     The type of the signal.  Can be DTR/DSR or CTS/RTS.
 * @param[in]    value   Must be 1 for set, or 0 for clear. No other values are
 *                       allowed.
 *
 * @return       0 if successful.
 * @return       -1 if \c port is invalid or an error occurred.
 * 
 * @sideeffects  Writes the signal to shared memory and signals the remote 
 *               processor.
 */
int32 smdl_sig_set
(
  smdl_handle_type port,
  smdl_sig_type    sig,
  boolean          value
);

/**
 * Query the value of an RS-232 signal. 
 *
 * @param[in]    port    The port to query the signal from.
 * @param[in]    sig     The type of the signal.  Can be DTR/DSR or CTS/RTS.
 *
 * @return       The value of the signal (0 or 1) if successful.
 * @return       -1 if \c port is invalid or an error occurred.
 */
int32 smdl_sig_get
(
  smdl_handle_type port,
  smdl_sig_type    sig
);

/*===========================================================================
  FUNCTION  smd_is_port_open
===========================================================================*/
/**
  Specifies whether the port with the given name and channel_type has already
  been opened by any processor.
  
  This function doesn't differentiate based on which processor(s) opened the 
  port.  It also works whether or not the local host is coincident with the 
  channel_type.

  @param[in] name           Name of the port to look for.
  @param[in] channel_type   Channel type of the port to look for.

  @return
  TRUE if the port has already been opened.
  FALSE if the port has never been opened.
*/
/*=========================================================================*/
boolean smd_is_port_open
(
  const char       *name,
  smd_channel_type  channel_type
);

#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* SMD_LITE_H */
