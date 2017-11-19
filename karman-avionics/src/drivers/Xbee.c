/*
 * Xbee.c
 *
 * Created: 11/19/2017 6:00:24 PM
 *  Author: Andrew Kaster
 */ 

#include "Xbee.h"

volatile xbee_ringbuf_t xbee_RingBuf;

void xbee_init(void)
{
    xbee_ringbuf_init(&xbee_RingBuf);
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
    return retVal;
}

