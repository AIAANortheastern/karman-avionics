/*
 * USBUtils.c
 *
 * Created: 4/16/2017 12:51:00 AM
 *  Author: Andrew Kaster
 */ 

#include "USBUtils.h"

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

