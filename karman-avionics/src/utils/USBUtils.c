 /**
 * @file USBUtils.c
 * @brief Controls USB Port
 *
 * The utility to control the USB port to the main board.
 *
 * TODO list: Finish 2 state machines.
 *            Create functions to make each type of packet.
 *
 * Created: 4/16/2017 12:51:00 AM
 *  Author: Andrew Kaster
 */

/** http://www.atmel.com/Images/Atmel-42336-ASF-USB-Stack-Manual_ApplicationNote_AT09331.pdf */

#include "USBUtils.h"
#include "conf_usb.h"
#include "FlashMem.h"
#include "Timer.h"
#include <compiler.h>

/** Size of the USB message buffer. TX/RX? TODO */
#define USB_MSG_BUF_SIZE (200)

/* Global Variables */
Bool gIsUSBActive; /**< Flag to know if USB is ready for communication */
volatile Bool gIsUSBConnected = false; /**< Flag to know if USB sense pin is high */
volatile uint32_t gUSBConnectTime; /**< Time for debouncing USB sense pin */
uint8_t gUSBMsgBuf[USB_MSG_BUF_SIZE]; /**< For holding USB data */

usb_utils_state_t gUsbUtilsState; /**< Main state machine for USB */
usb_utils_messageparse_state_t gUSBUtilsMessageState; /**< TX message parsing state machine */

/** 
 * @brief Initialize the USB driver
 *
 * Setup Interrupt on the usb sense pin to allow usb connection
 * and avoid needless processsing when its disconnected. 
 * Also start the ASF usb stack.
 */
void init_usb(void)
{
    gIsUSBActive = false;
    gUsbUtilsState = USB_STATE_INITIAL;

    /* setup both edges interrupt on USB_SENSE pin (see conf_board.h) */
    /* setup USB_SENSE with both direction interrupt, totem configuration (external pullup and pulldown)
     * No slew rate enable, and no invert Input/output 
     */
    USB_PORT.USB_SENSE_PINCTRL = 0x00;
    /* Enable INT0 with LOW priority, disable INT1 */
    USB_PORT.INTCTRL = 0x01;
    /* Setup USB_SENSE as interrupt pin */
    USB_PORT.INT0MASK = USB_SENSE;

    /* Start the USB device stack */
    udc_start();
}

/** 
 * @brief Custom VBUS monitoring Interrupt
 *
 * Set variable to true if we have a connection, record connection time.
 * Debounced in the USB task.
 *
 * Ref http://www.microchip.com/forums/m616629.aspx
*/
ISR(PORTD_INT0_vect)
{
    uint8_t pinval = USB_PORT.IN & USB_SENSE;
    /* pinval will be 0x20 if bit 5 is set or 0 if it is not. */

    if(pinval)
    {
        gIsUSBConnected = true;
        gUSBConnectTime = get_timer_count();
    }
    else
    {
        gIsUSBConnected = false;
    }
}

/** 
 * @brief OS callback from udc_attach
 *
 * @param port Ignored, only one USB peripheral on chip
 * @return True always.
 *
 * Called once udc_attach is finished. Initializes state machine and
 * Confirms that USB is ready for communication.
 */
bool usb_utils_cdc_enabled(uint8_t port)
{
    gIsUSBActive= true;
    gUsbUtilsState = USB_STATE_INITIAL;
    return true;
}

/**
 * @brief OS callback from udc_detach
 *
 * @param port Ignored, only one USB peripheral on chip
 *
 * Called once udc_detach is finished. Informs USB thread that
 * USB is fully detached.
 */
void usb_utils_cdc_disabled(uint8_t port)
{
    gIsUSBActive = false;
}

/**
 * @brief Form packet in caller's pointer
 *
 * @param id The ID of the message to create
 * @param len The message length of the intended message
 * @param message Pointer to the payload. Will be copied to packet.
 * @param[out] packet Will be filled with message.
 *
 * @return True on failure, false on success
 *
 * Computes checksum for message and fills packet pointer
 */
Bool usb_utils_create_packet(uint16_t id, uint16_t len, uint8_t *message, usb_packet_t *packet)
{
    Bool retVal = false; /* Nothing wrong */

    if(id < NUM_USB_MSG_ID)
    {
        packet->hdr.packet_magic = 0xDEADBEEF;
        packet->hdr.packet_id = id;
        packet->hdr.message_len = len;
        packet->message = message;
        retVal = usb_utils_calculate_checksum(&(packet->checksum), message, len);
    }
    else
    {
        retVal = true;
    }
    return retVal;
}

/**
 * @brief Calculate 8-bit checksum
 *
 * @param[out] checksum The checksum of the message
 * @param message Pointer to where the message is
 * @param len The length of the message
 *
 * @return True on failure, false on success
 *
 * Takes in message and computes checksum.
 */
Bool usb_utils_calculate_checksum(uint16_t *checksum, uint8_t *message, uint16_t len)
{
    Bool retVal = false;
    uint16_t word_idx = 0;
    uint8_t byte_idx = 0;
    
    if(NULL == checksum || NULL == message)
    {
        return retVal;
    }
    
    *checksum = 0;

    if(len < 1)
    {
        retVal = true;
        *checksum = 0xFFFF;
    }
    else /* Message length greater than 0 */
    {
        /* Iterate over 2 byte words */
        while(word_idx < (len >> 1))
        {
            *checksum += ((uint16_t *)message)[word_idx];
            word_idx++;
        }
        /* handle last word if odd number of bytes*/
        if(len & 1) /* if(len%2 == 1) */
        {
            byte_idx = len - 1;
            *checksum+= (((uint16_t)(message[byte_idx])) << 8);
        }
    }

    return retVal;
}

/** 
 * @brief Transfers flash memory contents to host
 *
 * Checks the Flash memory for a valid header, and then copies the
 * contents over USB to the host, one entry at a time.
 *
 * Does not transfer the header, only the packets. Each packet contains
 * a packet number and the total number of packets.
 */
void dump_to_usb(void) {
  usb_packet_t packet;
  usb_msg_flashentry_t payload;
  flash_data_entry_t *entry = &(payload.entry);

  flash_data_hdr_t header;
  flashmem_hdrstatus_t hdr_status = flashmem_verify_header(&header);

  switch(hdr_status) {
    case HDR_VALID:
      payload.num_entries = header.num_entries;
      for (int i = 0; i < header.num_entries; i++) {
        flashmem_read_entry(entry, i);
        payload.entry_num = i;
        usb_utils_create_packet(USB_ID_FLASHENTRY,
                                sizeof(usb_msg_flashentry_t),
                                (uint8_t *)&payload,
                                &packet);
        while (!udi_cdc_is_tx_ready()) {
          asm volatile ("nop \n\t");
        }
        udi_cdc_write_buf(&packet, sizeof(packet));
        //udi_cdc_putc, getc, write_buff, read_buff
      }
      break;
    case HDR_INVALID:
      break;
    case HDR_ZERO:
      break;
    case HDR_READFAIL:
      break;
  }
}

/**
 * @brief Main USB state machine
 *
 * Runs in the USB thread if the USB cable is connected and
 * the ASF USB stack is ready. Starts off with a lengthy intialization
 * handshake to make sure host and application are both ready to begin 
 * communcation. Then, the Host will request a mode. The App will acknowledge
 * the mode, and wait for the host to confirm it. If at any time a bad or
 * out of order message is rx, the app will revert to Initial state to redo 
 * the handshake. Once the mode is confirmed, the app will either transmit the
 * contents of the external flash memory, continue on to data acquistion mode,
 * or begin an ejection test. After transmitting flash or finishing an ejection
 * test, it will transition to an idle mode while waiting for a new mode 
 * request. While in ejection test mode, it will wait for a request to eject 
 * the main or drogue. As with the mode request, a response will be transmitted
 * to the host, which will send an acknowledgement of the response. On receipt
 * of the acknowledgement of the Main/Drogue test, it will wait 2 seconds
 * before triggering the corresponding pyrotechnics. The test is ended with an
 * end test message. While in data acquistion mode, the OS will keep
 * collecting data and keep running the main application. In the other three 
 * modes, the main app is not running. 
 *
 */
void usb_utils_state_mach(void)
{
    nack_error_t error_code = NACK_UNKNOWN;
    Bool is_nack_required = false;

    switch(gUsbUtilsState)
    {
        case USB_STATE_INITIAL:
            /* Send usb_msg_init */
            break;
        case USB_STATE_WAIT_INIT_ACK:
            /* wait for usb_msg_init_ack */
            /* on success, send usb_req_mode */
            /* on timeout/failure, send usb_msg_nack */
            error_code = NACK_TIMEOUT; /* for example */
            break;
        case USB_STATE_WAIT_RECV_MODE:
            /* wait for message usb_msg_recv_mode */
            /* on success, send usb_msg_ack_mode */
            /* on timeout/failure, send usb_msg_nack */
            break;
        case USB_STATE_WAIT_ACK_MODE_RESP:
            /* wait for message ack_mode_resp */
            /* on success, set next mode based on message */
            /* on timeout/failure, send usb_msg_nack */
            break;
        case USB_STATE_TRANSMIT_FLASH:
            /* call dump_to_usb */
            /* on failure, send usb_msg_nack */
            break;
        case USB_STATE_EJECTIONTEST:
            /* wait for ejection test messages */
            /* trigger pyrotechnics when requested */
            break;
        case USB_STATE_DO_ACQ:
            /*  */
            break;
        default:
            /* ERROR */
            break;
    }

    if(is_nack_required)
    {
        /* send nack with error code error_code */
        usb_utils_send_nack(error_code);
    }
}

/** 
 * @brief Sends nack packet over USB
 *
 * @param error_code the specific NACK to send
 *
 */
void usb_utils_send_nack(nack_error_t error_code)
{
    /* Send nack message over usb */
}

/**
 * @brief Message parsing state machine
 *
 * @param[out] packet_out Rx'd packet is copied into this pointer
 * @returns True if packet RX, false otherwise
 *
 * TODO
 * uses global variable gUSBMsgBuf. Think about buffer of packets
 */
bool usb_utils_check_for_message(usb_packet_t *packet_out)
{
    /* Check how much data is ready to be processed */
    iram_size_t num_bytes_recv = udi_cdc_get_nb_received_data();
    iram_size_t num_rem_bytes;

    switch(gUSBUtilsMessageState)
    {
        case MSG_STATE_READ_HEADER:
            if(num_bytes_recv >= USB_PACKET_HDR_SIZE)
            {
                num_rem_bytes = udi_cdc_read_buf((uint8_t *)&(packet_out->hdr), USB_PACKET_HDR_SIZE);
                if(num_rem_bytes >= packet_out->hdr.packet_id)
                {
                    num_rem_bytes = udi_cdc_read_buf(&(packet_out->message), (packet_out->hdr.message_len + USB_PACKET_CHKSUM_SIZE));
                }
                else
                {
                    gUSBUtilsMessageState = MSG_STATE_WAIT_MSG;
                }
            }
            break;
        case MSG_STATE_WAIT_MSG:
            break;
        default:
            break;
    }
    // state: read header
    //      If there are USB_PACKET_HDR_SIZE bytes to read
    //          Read USB_PACKET_HDR_SIZE bytes
    //          If there are message_len bytes to read
    //              Read message_len bytes
    //              parse message
    //          else
    //              Next state = wait message          
    //
    // state: wait message
    //      If there are message_len bytes
    //          Read mesage_len bytes
    //          parse message

    return false;
}
