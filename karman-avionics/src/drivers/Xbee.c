/*
 * Xbee.c
 *
 * Created: 11/19/2017 6:00:24 PM
 *  Author: Andrew Kaster
 */ 

#include "Xbee.h"
#include <conf_board.h>

volatile xbee_ringbuf_t xbee_RingBuf;

volatile Bool xbee_atten_asserted;

void xbee_init(void)
{
    xbee_ringbuf_init(&xbee_RingBuf);

    /* setup both edges interrupt on RADIO_ATTEN  pin (see conf_board.h) */
    /* setup RADIO_ATTEN with both direction interrupt, totem configuration (external pullup and pulldown)
     * No slew rate enable, and no invert Input/output 
     */
    RADIO_GPIO_PORT.RADIO_ATTEN_PINCTRL = 0x00;
    /* Enable INT0 with LOW priority, disable INT1. */
    /* NOTE! If we use IMU_ACC2_INT this will need to change!*/
    RADIO_GPIO_PORT.INTCTRL = 0x01;
    /* Setup RADIO_ATTEN as interrupt pin */
    RADIO_GPIO_PORT.INT0MASK = RADIO_ATTEN;
}

ISR(PORTF_INT0_vect)
{
    uint8_t pinval = RADIO_GPIO_PORT.IN & RADIO_ATTEN;
    /* pinval will be 0x10 if bit 4 is set or 0 if it is not. */

    if(pinval)
    {
        xbee_atten_asserted = false;
    }
    else
    {
        /* ATTEN is active low */
        xbee_atten_asserted = true;
    }
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

