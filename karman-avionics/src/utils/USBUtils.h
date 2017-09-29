/**
 * @file USBUtils.h
 *
 * @brief USB App-facing API 
 *
 * Created: 4/13/2017 6:23:00 PM
 * Author: Andrew Kaster
 */


#ifndef USBUTILS_H_
#define USBUTILS_H_

#include <asf.h>
#include "FlashMem.h"

/** Header to be sent before each packet */
typedef struct
{
    uint32_t packet_magic; /**< Magic number */
    uint16_t packet_id;    /**< Packet ID */
    uint16_t message_len;  /**< Number of bytes in message */
} usb_packet_header_t;

/** Size in bytes of USB packet header */
#define USB_PACKET_HDR_SIZE (8)
/** Size in bytes of USB checksum */
#define USB_PACKET_CHKSUM_SIZE (2)

/** USB packet. 8 bytes for header, 2 for checksum, plus message */
typedef struct
{
    usb_packet_header_t hdr; /**< Packet header */
    uint8_t *message;        /**< Pointer to the message */
    uint16_t checksum;       /**< Stores the checksum */
} usb_packet_t;

/** High level state machine for the USB */
typedef enum
{
    USB_STATE_INITIAL,              /**< Start condition */
    USB_STATE_WAIT_INIT_ACK,        /**< Wait for the ack from host */
    USB_STATE_WAIT_RECV_MODE,       /**< Wait for command */
    USB_STATE_WAIT_ACK_MODE_RESP,   /**< Wait for ack of command */
    USB_STATE_TRANSMIT_FLASH,       /**< Upload flash contents to host */
    USB_STATE_EJECTIONTEST,         /**< Perform ejection test */
    USB_STATE_DO_ACQ,               /**< Perform data acquistion */
} usb_utils_state_t;

/** Message parsing state machine */
typedef enum
{
    MSG_STATE_READ_HEADER, /**< Read the header */
    MSG_STATE_WAIT_MSG,    /**< Wait for the num bytes in the message */
} usb_utils_messageparse_state_t;

/** Possible messages over USB */
typedef enum
{
    USB_ID_INIT = 0,       /**< Intialization message */
    USB_ID_INIT_ACK,       /**< Intialization ACK from Host */
    USB_ID_REQ_MODE,       /**< Mode request from host */
    USB_ID_RECV_MODE,      /**< Mode reciept from mcu to host */
    USB_ID_ACK_MODE,       /**< ACK of the mode  */
    USB_ID_ACK_MODE_RESP,  /**< Response to the ACK */
    USB_ID_FLASHENTRY,     /**< Message contains a flash entry */
    USB_ID_EJTEST_MAIN,    /**< Request for Main ejection test */
    USB_ID_EJTEST_DROG,    /**< Request for Drogue ejection test */
    USB_ID_EJTEST_END,     /**< End of ejection tests */
    USB_ID_MSG_NACK,       /**< NACK message */
    NUM_USB_MSG_ID,        /**< Not an actual message, # of messages */
} usb_id_t;

/**
 * @brief NACK types
 * A nack message will be sent to the host in the event that
 * a packet is recieved in an unexpected order, or with anything invalid
 * in the message (checksum, id, payload value, etc)
*/
typedef enum
{
    NACK_UNEXP_HOST_MSG, /**< Host sent message out of order */
    NACK_TIMEOUT,        /**< Timeout expored */
    NACK_INVALID_PAYLD,  /**< Message contained incorrect info */
    NACK_FLASH_HDR_ERR,  /**< Flash header invalid */
    NACK_UNKNOWN,        /**< All other errors */
} nack_error_t;

/** Size of Initializatin string */
#define USB_INIT_STR_SIZE (16)
/** Initialization string to confirm with host that usb serial is working */
#define USB_INIT_STR ("The quick brown")

/** Intialization message */
typedef struct
{
    char init_str[USB_INIT_STR_SIZE]; /**< String payload */
} usb_msg_init_t;


/** Size of expected return string */
#define USB_INIT_ACK_STR_SIZE (16)
/** Expected return string from host after sending initialization string */
#define USB_INIT_ACK_STR ("fox jumped over")

/** Intialization acknowledgement from host */
typedef struct
{
    char init_ack_str[USB_INIT_ACK_STR_SIZE]; /**< String payload */
} usb_msg_init_ack_t;

/** Size of mode request message */
#define USB_REQ_MODE_SIZE (2)
/** Instructs host to display CLI prompt that asks the user to select a mode */
#define USB_REQ_MODE_MAGIC (0xCAFE)

/** Mode request message */
typedef struct
{
    uint16_t mode_req_magicnum; /**< Magic number payload */
} usb_msg_req_mode_t;

/** Size of Message from host to MCU containing the requested mode */
#define USB_MODE_MSG_SIZE (2)

/** Execution mode enumeration */
typedef enum
{
    USB_EXEC_MODE_DACQ = 0x111,     /**< Data acquistion */
    USB_EXEC_MODE_DNLD = 0x222,     /**< Download data */
    USB_EXEC_MODE_EJTEST = 0x999,   /**< Ejection test */
} usb_execution_mode_t;

/** Host message with initial mode for handshake */
typedef struct
{
    uint16_t execution_mode; /**< Mode enum, 16 bits tho */
} usb_msg_recv_mode_t;

/** Proper response */
#define USB_MODE_ACK_NUM (0xF00D)

/** Message from MCU to host to acknwoledge the mode */
typedef struct
{
    uint16_t ack_num;         /**< Contains 0xF00D */
    uint16_t execution_mode;  /**< Execution mode */
} usb_msg_ack_mode_t;

/** Proper response to response Host to MCU */
#define USB_MODE_ACKRESP_NUM (0xBEEF)

/** Final mode handshake message with confirmed mode */
typedef struct
{
    uint16_t ackresp_num;       /**< Contains 0xBEEF */
    uint16_t execution_mode;    /**< Exection mode */
} usb_msg_ack_mode_resp_t;

/** Data packet for download mode */
typedef struct
{
    uint32_t num_entries;       /**< Total number of entries to be sent */
    uint32_t entry_num;         /**< Sequence number */
    flash_data_entry_t entry;   /**< The actual packet */
} usb_msg_flashentry_t;

/** Not-Acknowlege message */
typedef struct
{
    nack_error_t error_code;    /**< NACK message */
} usb_msg_nack_t;

/* TODO The rest of the message definitions */

/* Computes checksum for message and fills packet pointer */
Bool usb_utils_create_packet(uint16_t id, uint16_t len, uint8_t *message, usb_packet_t *packet);

/* Takes in message and computes checksum */
Bool usb_utils_calculate_checksum(uint16_t *checksum, uint8_t *message, uint16_t len);

void dump_to_usb(void);

bool usb_utils_cdc_enabled(uint8_t port);

void usb_utils_cdc_disabled(uint8_t port);

void usb_utils_state_mach(void);

void init_usb(void);

bool usb_utils_check_for_message(usb_packet_t *packet_out); 

void usb_utils_send_nack(nack_error_t error_code);

extern Bool gIsUSBActive;
extern volatile Bool gIsUSBConnected;
extern volatile uint32_t gUSBConnectTime; /* For debounce */

#endif /* USBUTILS_H_ */
