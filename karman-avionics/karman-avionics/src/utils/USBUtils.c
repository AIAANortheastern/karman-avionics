/*
 * USBUtils.c
 *
 * Created: 4/16/2017 12:51:00 AM
 *  Author: Andrew Kaster
 */

/* http://www.atmel.com/Images/Atmel-42336-ASF-USB-Stack-Manual_ApplicationNote_AT09331.pdf */

#include "USBUtils.h"
#include "conf_usb.h"
#include "FlashMem.h"
#include "Timer.h"
#include <compiler.h>

Bool gIsUSBActive;
volatile Bool gIsUSBConnected = false;
volatile uint32_t gUSBConnectTime; /* For debounce */

usb_utils_state_t gUsbUtilsState;

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

/* Custom VBUS monitoring */
/* http://www.microchip.com/forums/m616629.aspx */
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

/* Called once udc_attach is finished */
bool usb_utils_cdc_enabled(void)
{
    gIsUSBActive= true;
    gUsbUtilsState = USB_STATE_INITIAL;
    return true;
}

/* called once udc_detach is finsihed */
void usb_utils_cdc_disabled(void)
{
    gIsUSBActive = false;
}

/* Computes checksum for message and fills packet pointer */
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

/* Takes in message and computes checksum */
Bool usb_utils_calculate_checksum(uint16_t *checksum, uint8_t *message, uint16_t len)
{
    Bool retVal = false;
    uint16_t word_idx = 0;
    uint8_t byte_idx = 0;

    checksum = 0;

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
        udi_cdc_write_buf(&packet, sizeof(entry));
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

/* this function runs a state machine */
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
        case USB_STATE_WAIT_ACK_MODE:
            /* wait for message usb_msg_recv_mode */
            /* on success, send usb_msg_ack_mode */
            /* on timeout/failure, send usb_msg_nack */
            break;
        default:
            /* ERROR */
            break;
    }

    if(is_nack_required)
    {
        /* send nack with error code error_code */
    }
}
