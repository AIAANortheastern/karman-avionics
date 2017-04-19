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
    uint32_t packet_magic;
    uint16_t packet_id;
    uint16_t message_len;
} usb_packet_header_t;

#define USB_PACKET_HDR_SIZE (8)

typedef struct
{
    usb_packet_header_t hdr;
    uint8_t *message;
    uint16_t checksum;
} usb_packet_t;

/* A nack message will be sent to the host in the event that */
/* a packet is recieved in an unexpected order, or with anything invalid */
/* in the message (checksum, id, payload value, etc) */

typedef enum 
{
    USB_ID_INIT = 0,
    USB_ID_INIT_ACK,
    USB_ID_REQ_MODE,
    USB_ID_RECV_MODE,
    USB_ID_ACK_MODE,
    USB_ID_ACK_MODE_RESP,
    USB_ID_EJTEST_MAIN,
    USB_ID_EJTEST_DROG,
    USB_ID_EJTEST_END,
    USB_ID_MSG_NACK,
    NUM_USB_MSG_ID,
} usb_id_t;

/* Initialization string to confirm with host that usb serial is working */
#define USB_INIT_STR_SIZE (16)
#define USB_INIT_STR ("The quick brown")

typedef struct 
{
    char init_str[USB_INIT_STR_SIZE];
} usb_msg_init_t;


/* Expected return string from host after sending initialization string */
#define USB_INIT_ACK_STR_SIZE (16)
#define USB_INIT_ACK_STR ("fox jumped over")

typedef struct
{
    char init_ack_str[USB_INIT_ACK_STR_SIZE];
} usb_msg_init_ack_t;

/* Instructs host to display CLI prompt that asks the user to select a mode */
#define USB_REQ_MODE_SIZE (2)
#define USB_REQ_MODE_MAGIC (0xCAFE) 

typedef struct
{
    uint16_t mode_req_magicnum;
} usb_msg_req_mode_t;

/* Message from host to MCU containing the requested mode */
#define USB_MODE_MSG_SIZE (2)

typedef enum
{
    USB_EXEC_MODE_DACQ = 0x111,
    USB_EXEC_MODE_DNLD = 0x222,
    USB_EXEC_MODE_EJTEST = 0x999,
} usb_execution_mode_t;

/* Host message with initial mode for handshake */
typedef struct
{
    uint16_t execution_mode;
} usb_msg_recv_mode;

#define USB_MODE_ACK_NUM (0xF00D)

/* Message from MCU to host to acknwoledge the mode */
typedef struct
{
    uint16_t ack_num;
    uint16_t execution_mode;
} usb_msg_ack_mode;

#define USB_MODE_ACKRESP_NUM (0xBEEF)

/* Final mode handshake message with confirmed mode */
typedef struct
{
    uint16_t ackresp_num;
    uint16_t execution_mode;
} usb_msg_ack_mode_resp;

/* TODO The rest of the message definitions */

/* Computes checksum for message and fills packet pointer */
Bool usb_utils_create_packet(uint16_t id, uint16_t len, uint8_t *message, usb_packet_t *packet);

/* Takes in message and computes checksum */
Bool usb_utils_calculate_checksum(uint16_t *checksum, uint8_t *message, uint16_t len);


#endif /* USBUTILS_H_ */
