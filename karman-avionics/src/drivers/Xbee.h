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

#include <asf.h>

#define XBEEBUFSIZE (256)

typedef struct
{
    volatile uint16_t           bytesRecv;
    Bool                        pkt_rdy;
    uint8_t                     tx_buffer[XBEEBUFSIZE];
    uint8_t                     rx_buffer[XBEEBUFSIZE];
    volatile Bool               is_tx_complete;
} xbee_ctrl_t;

extern xbee_ctrl_t gXbeeCtrl;

void xbee_init(void);

void xbee_RX_ISR(void);

Bool is_xbee_pkt_rdy(void);

Bool xbee_tx_payload(void *buf, uint16_t len);

Bool is_xbee_tx_done(void);

#endif /* XBEE_H_ */