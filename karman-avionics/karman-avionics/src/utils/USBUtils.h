/*
 * USBUtils.h
 *
 * Created: 4/13/2017 6:23:00 PM
 *  Author: Andrew Kaster
 */ 


#ifndef USBUTILS_H_
#define USBUTILS_H_

#include <asf.h>

typedef struct
{
	uint16_t packet_id;
	uint16_t packet_len;
} usb_packet_header_t;

typedef struct
{
	usb_packet_header_t hdr;
	uint8_t *message;
	uint16_t checksum;
} usb_packet_t;

/* Computes checksum for message and fills packet pointer */
Bool usb_utils_create_packet(uint16_t id, uint16_t len, uint8_t *message, usb_packet_t *packet);

/* Takes in message and computes checksum */
Bool usb_utils_calculate_checksum(uint16_t *checksum, uint8_t *message, uint16_t len);


#endif /* USBUTILS_H_ */
