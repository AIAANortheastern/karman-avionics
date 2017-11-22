/*
 * Xbee.h
 *
 * Created: 11/19/2017 3:40:06 PM
 *  Author: Andrew Kaster
 *
 * https://cdn.sparkfun.com/datasheets/Wireless/Zigbee/90002173_N.pdf
 */ 


#ifndef XBEE_H_
#define XBEE_H_

#include "ringbuf.h"
#include <asf.h>
#include "Spi_service.h"

/** List of TX Frame names */
#define XBEE_API_CMD (0x08)
#define XBEE_AT_CMD (0x09)
#define XBEE_TX_REQ (0x10)
#define XBEE_EXP_TX_REQ (0x11)
#define XBEE_REM_CMD_REQ (0x17)

/** List of RX Frame Names */
#define XBEE_AT_CMD_RESP (0x88)
#define XBEE_MODEM_STS (0x8A)
#define XBEE_TX_STS (0x8B)
#define XBEE_RX_IND (0x90)
#define XBEE_EXP_RX_IND (0x91)
#define XBEE_NODE_ID_IND (0x95)
#define XBEE_REM_CMD_RESP (0x97)

#define XBEE_FRAME_DELIM (0x7E) /**< Start of frame byte */

#define MAX_FRAME_SIZE (256)

//
// Frame structure:
//    Frame ID = 1 byte
//    Frame specific data = len-1 bytes
//

#define TX_HDR_SIZE (14)

typedef struct
{
    uint8_t frame_type; /**< Frame type == 0x10 */
    uint8_t frame_id;   /**< incrementing per packet */
    union
    {
        struct
        {
            uint8_t byte7;
            uint8_t byte6;
            uint8_t byte5;
            uint8_t byte4;
            uint8_t byte3;
            uint8_t byte2;
            uint8_t byte1;
            uint8_t byte0;
        } bytes;
        uint64_t qword;  
    } dest_addr; /**< IPv6 address */
    uint16_t reserved; /**< 0xFFFE */
    uint8_t bcast_radius; /**< 0 = max hops */
    uint8_t tx_options;   /**< 0x00 = use TO config param */
    uint8_t payload[MAX_FRAME_SIZE - TX_HDR_SIZE]; /**< Payload goes here */
} xbee_tx_req;

typedef enum
{
    XBEE_STS_SUCCESS = 0x00,        /**< Success */
    XBEE_STS_MAC_ACK_FAIL = 0x01,   /**< MAC ACK Failure */
    XBEE_STS_COLLISION_FAIL = 0x02, /**< Collision Avoidance Failure */
    XBEE_STS_NTWK_ACK_FAIL = 0x21,  /**< Network ACK Failure */
    XBEE_STS_RTE_NOT_FND = 0x25,    /**< Route Not Found */
    XBEE_STS_INT_RES_ERR = 0x31,    /**< Internal Resource Error */
    XBEE_STS_INT_ERROR = 0x32,      /**< Internal Error */
    XBEE_STS_TOO_LARGE = 0x74,      /**< Payload too large */
    XBEE_STS_NOT_REQ = 0x75         /**< Indirect message unrequested */
} xbee_deliv_sts_t;

typedef struct  
{
    uint8_t frame_type; /**< 0x8B */
    uint8_t frame_id; /**< frame_id for the just-sent frame */
    uint16_t reserved; /**< 0xFFFE */
    xbee_deliv_sts_t deliv_status; /**< see xbee_deliv_sts_t */
    uint8_t discov_sts; /**< 0 = none, 2 = discovery */
} xbee_tx_sts_t;

#define RX_HDR_SIZE (12)
typedef struct
{
    uint8_t ack      : 1; /**< packet was acknowledged */
    uint8_t bcast    : 1; /**< broadcasted packet */
    uint8_t reserved : 4; /**< Other bits should be ignored */
    uint8_t mode     : 2; /**< 1 = point/multi point, 2 = repeater, 3 = digiMesh */
} xbee_rx_opt_t;

typedef struct  
{
    uint8_t frame_type; /**< 0x90 */
    union
    {
        struct bytes
        {
            uint8_t byte7;
            uint8_t byte6;
            uint8_t byte5;
            uint8_t byte4;
            uint8_t byte3;
            uint8_t byte2;
            uint8_t byte1;
            uint8_t byte0;
        } bytes;
        uint64_t qword;
    } src_addr; /**< IPv6 address */
    uint16_t reserved; /**< 0xFFFE */
    xbee_rx_opt_t rx_options; /**< see xbee_rx_opt_t */
    uint8_t payload[MAX_FRAME_SIZE - RX_HDR_SIZE]; /**< Payload goes here */
} xbee_rx_inidcator_t;

typedef union
{
    uint8_t frame_type;
    xbee_rx_inidcator_t rx;
    xbee_tx_sts_t tx_sts;
} xbee_rx_frame_u;

typedef enum
{
    XBEE_NO_START,
    XBEE_LEN1,
    XBEE_LEN2,
    XBEE_PAYLOAD
} xbee_rx_state_t;

RINGBUF_DECL(xbee, (2*MAX_FRAME_SIZE + 6));

typedef struct
{
    uint16_t        len; /**< length of frame */
    uint8_t         checksum; /**< Actually from the END of the packet! :) */
    xbee_ringbuf_t  framebuf; /**< frame goes here */
} xbee_rx_packet_t;

typedef struct
{
    volatile uint16_t bytesRecv;
    xbee_rx_state_t   rx_state;
    volatile xbee_rx_packet_t  curr_pkt;
    Bool              pkt_rdy;
} xbee_ctrl_t;

extern xbee_ctrl_t gXbeeCtrl;

void xbee_init(void);

void xbee_SPI_ISR(spi_master_t *spi_interface);

Bool is_xbee_pkt_rdy(void);

Bool xbee_handleRxAPIFrame(xbee_rx_frame_u *frame);

#endif /* XBEE_H_ */