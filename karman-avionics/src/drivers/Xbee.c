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


void xbee_init()
{
    gXbeeCtrl.is_tx_complete = true;
}



/** @brief Custom ISR for Xbee
 *
 * The interrupt service routine should be called on a data receive interrupt.
 * NOTE: This function must be re-entrant from multiple iterrupts. If one
 * SPI interrupt is higher than another, it could pre-empt in the middle
 * Of processing. (But please don't do that, keep them the same level...)
 */
void xbee_RX_ISR(void)
{
    static uint8_t index =0;

    gXbeeCtrl.rx_buffer[index] = RADIO_USART.DATA;

    /* The buffer is 256 bytes, so it can safely be indexed with a constantly
     * incrementing 8 bit integer. If the buffer size changes this logic MUST change,
     * since it relies on unsigned integer "overflow".
     */
    index++;
}


Bool is_xbee_pkt_rdy(void)
{ 
    return true;
}


/** @return false on failure, true on success */
Bool xbee_tx_payload(void *buf, uint16_t len)
{
    if(!buf || len < 0)
    {
        return false;
    }

    uint16_t i = 0;

    while(i < len)
    {
        //TODO do this without using blocking function usart_putchar
        //TODO investigate setting up DMA transfer
        usart_putchar(&RADIO_USART, ((uint8_t *)buf)[i]);
        i++;
    }
    return true;
}
