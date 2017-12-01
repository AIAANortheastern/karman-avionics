/*
 * Xbee.c
 *
 * Created: 11/19/2017 6:00:24 PM
 *  Author: Andrew Kaster
 */ 

#include "Xbee.h"
#include <conf_board.h>
#include "Spi_service.h"

volatile Bool gXbeeAttenAsserted ;
xbee_ctrl_t gXbeeCtrl;
uint8_t xbeePktCnt;

static volatile Bool xb_dummyBool = false;
static volatile uint8_t xb_dummyInt = false;

#ifndef XBEE_TX_ADDRESS
    #define XBEE_TX_ADDRESS (0x000000000000FFFF) /**< Broadcast address is 0xFFFF */
#endif

typedef struct 
{
    uint8_t reserved : 4;
    uint8_t int1     : 2;
    uint8_t int0     : 2;
} port_intctrl_t;

void xbee_init(spi_master_t *master)
{
    uint8_t temp;

    xbee_ringbuf_init(&(gXbeeCtrl.curr_pkt.framebuf));

    gXbeeCtrl.master = master;
    gXbeeCtrl.cs_info.csPort = &RADIO_GPIO_PORT;
    gXbeeCtrl.cs_info.pinBitMask = RADIO_CS;

    /* setup both edges interrupt on RADIO_ATTEN  pin (see conf_board.h) */
    /* setup RADIO_ATTEN with both direction interrupt, totem configuration (external pullup and pulldown)
     * No slew rate enable, and no invert Input/output 
     */
    RADIO_GPIO_PORT.RADIO_ATTEN_PINCTRL = 0x00;

    /* Set INT0 to low priority. Top 4 bits reserved, 0. Ignore INT1 */
    temp = RADIO_GPIO_PORT.INTCTRL;
    (*((port_intctrl_t *)&temp)).int0 = PORT_INT0LVL_LO_gc;
    RADIO_GPIO_PORT.INTCTRL = temp;

    /* Setup RADIO_ATTEN as interrupt pin */
    RADIO_GPIO_PORT.INT0MASK = RADIO_ATTEN;
}

/* RADIO_ATTEN interrupt. Active low */
ISR(PORTF_INT0_vect)
{
    uint8_t pinval = RADIO_GPIO_PORT.IN & RADIO_ATTEN;
    /* pinval will be 0x10 if bit 4 is set or 0 if it is not. */

    if(pinval)
    {
        gXbeeAttenAsserted  = false;
    }
    else
    {
        /* ATTEN is active low */
        gXbeeAttenAsserted  = true;
        /* Xbee has something to say, let's hear it */
        if(!(gXbeeCtrl.master->masterBusy))
        {
            /* If there's nothing in the request queue, just send some garbage */
            if(gXbeeCtrl.master->requestQueue[gXbeeCtrl.master->front].valid == false)
            {
                spi_master_enqueue(gXbeeCtrl.master,
                                   &(gXbeeCtrl.cs_info),
                                   &xb_dummyInt,
                                   0,
                                   NULL,
                                   0,
                                   &xb_dummyBool);
            }
            /* Begin the current request so we can  */
            spi_master_initate_request(gXbeeCtrl.master);
        }
    }
}

/** @brief Custom ISR for Xbee
 * @param spi_interface The SPI master object that controls the bus.
 *
 * The interrupt service routine should be called on a data receive interrupt.
 * NOTE: This function must be re-entrant from multiple iterrupts. If one
 * SPI interrupt is higher than another, it could pre-empt in the middle
 * Of processing. (But please don't do that, keep them the same level...)
 */
void xbee_SPI_ISR(spi_master_t *spi_interface)
{
    volatile uint8_t *dataSent;
    uint16_t dataToSend;
    Bool moreToDo;
    uint8_t currByte;

    /** Look at the front of the queue for transmit data */
    volatile spi_request_t *currRequest = &spi_interface->requestQueue[spi_interface->front];
    dataSent = &(currRequest->bytesSent);
    dataToSend = currRequest->sendLen;

    currByte = spi_interface->master->DATA;

    if(gXbeeAttenAsserted)
    {
        xbee_rx_statemach_run(currByte);
    }

    /** If there's still bytes to send, keep sending them*/
    if((*dataSent) < dataToSend)
    {
        spi_interface->master->DATA = ((uint8_t *)(currRequest->sendBuff))[(*dataSent)];
        (*dataSent)++;
    }
    /** Otherwise, if  want to receive more data but we don't have any more to send... */
    else if(gXbeeAttenAsserted)
    {
        /** Send a dummy byte to clock out the next bits of data. */
        spi_interface->master->DATA = 0x00;
    }

    /** Check if are we done */
    moreToDo = (*dataSent < dataToSend) ? true : false;
    moreToDo |= gXbeeAttenAsserted;

    if(!moreToDo)
    {
        /** If we're done, raise chip select again. NOTE: This is enabled by default. 
         * There are a few special cases (i.e. Altimeter Reset procedure) that
         * require it to be held low.
        */
        if(currRequest->raise_cs){
            spi_master_finish_request(currRequest);
        }
        /** Inform the initiator that the request has completed*/
        spi_interface->masterBusy = false;
        spi_master_request_complete(spi_interface);
        /** Dequeue the request from the list*/
        spi_master_dequeue(spi_interface);
    }
}

void xbee_rx_statemach_run(uint8_t currByte)
{
    switch(gXbeeCtrl.rx_state)
    {
    case XBEE_NO_START:
        if(currByte == XBEE_FRAME_DELIM)
        {
            gXbeeCtrl.rx_state = XBEE_LEN1;
        }
        break;
    case XBEE_LEN1:
        gXbeeCtrl.curr_pkt.len = ((uint16_t)currByte << 8);
        gXbeeCtrl.rx_state = XBEE_LEN2;
        break;
    case XBEE_LEN2:
        gXbeeCtrl.curr_pkt.len |= currByte;
        if(gXbeeCtrl.curr_pkt.len <= 0 || gXbeeCtrl.curr_pkt.len >= MAX_FRAME_SIZE)
        {
            gXbeeCtrl.rx_state = XBEE_NO_START;
        }
        else
        {
            ringbuf_put((volatile ringbuf_t *)&(gXbeeCtrl.curr_pkt.framebuf),
                        ((gXbeeCtrl.curr_pkt.len & 0xFF00) >> 8));
            ringbuf_put((volatile ringbuf_t *)&(gXbeeCtrl.curr_pkt.framebuf),
                        (gXbeeCtrl.curr_pkt.len & 0xFF));
            gXbeeCtrl.rx_state = XBEE_PAYLOAD;
            gXbeeCtrl.bytesRecv = 0;
        }
        break;
    case XBEE_PAYLOAD:
        if(gXbeeCtrl.curr_pkt.len < gXbeeCtrl.bytesRecv)
        {
            ringbuf_put((volatile ringbuf_t *)&(gXbeeCtrl.curr_pkt.framebuf), currByte);
            gXbeeCtrl.bytesRecv++;
        }
        else
        {
            /** put the checksum at the end */
            ringbuf_put((volatile ringbuf_t *)&(gXbeeCtrl.curr_pkt.framebuf), currByte);
            gXbeeCtrl.pkt_rdy = true;
            gXbeeCtrl.rx_state = XBEE_NO_START;
        }
        break;
    default:
        /* Error case, results in dropping a byte */
        gXbeeCtrl.rx_state = XBEE_NO_START;
        break;
    }

    return;
}

Bool is_xbee_pkt_rdy(void)
{ 
    return gXbeeCtrl.pkt_rdy;
}

/**
 * @brief handles a received XBee API frame
 *
 * @param frame the frame to handle
 */
Bool xbee_handleRxAPIFrame(xbee_rx_frame_u *frame)
{
    Bool retVal = false;
    switch (frame->frame_type)
    {
        case XBEE_RX_IND:
            /* Handle frame->rx */
    	    break;
        case XBEE_TX_STS:
            /* Handle frame->tx_sts */
            break;
        default:
            /* Ignore unhandled frames */
            break;
    }
    gXbeeCtrl.pkt_rdy = false;
    return retVal;
}

/** @return false on failure, true on success */
Bool xbee_tx_payload(void *buf, uint16_t len)
{
    if(!buf || (len > (MAX_FRAME_SIZE - TX_HDR_SIZE)))
    {
        return false;
    }

    xbee_tx_req_t *req = (xbee_tx_req_t *)(&(gXbeeCtrl.tx_buffer[XBEE_PREFRAME_SIZE]));
    uint8_t checksum = 0;

    gXbeeCtrl.tx_buffer[0] = XBEE_FRAME_DELIM;
    gXbeeCtrl.tx_buffer[1] = len << 8;
    gXbeeCtrl.tx_buffer[2] = len & 0xFF;

    xbeePktCnt++; /* Frame ID 0 will not return a TX status message, so pre-increment count */

    req->frame_type = 0x10;
    req->frame_id = xbeePktCnt;

    /* Avoid ID 0 */
    if(xbeePktCnt == 0xFF)
    {
        xbeePktCnt = 0;
    }

    req->dest_addr.qword = XBEE_TX_ADDRESS;
    req->reserved = 0xFFFE;
    req->bcast_radius = 0;
    req->tx_options = 0;

    memcpy((void *)(req->payload), buf, len);

    checksum = xbee_calculate_checksum(&(gXbeeCtrl.tx_buffer[XBEE_PREFRAME_SIZE]), len);

    gXbeeCtrl.tx_buffer[XBEE_PREFRAME_SIZE + len] = checksum;

    return spi_master_enqueue(gXbeeCtrl.master,
                              &(gXbeeCtrl.cs_info),
                              gXbeeCtrl.tx_buffer,
                              len,
                              NULL,
                              0,
                              &(gXbeeCtrl.is_tx_complete));
}

/** See page 63 of datasheet */
uint8_t xbee_calculate_checksum(uint8_t *buf, uint16_t len)
{
    if(!buf || (len > (MAX_FRAME_SIZE - TX_HDR_SIZE)))
    {
        return 0;
    }

    uint16_t i;
    uint8_t sum;

    for(i = 0; i < len; i++)
    {
        sum += buf[i];
    }

    return 0xFF - sum;
}