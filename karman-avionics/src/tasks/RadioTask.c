/**
 * @file RadioTask.c
 *
 * @brief Radio Task Function and Initialization
 *
 * Created: 12/4/2016 10:19:36 PM
 *  Author: Andrew Kaster
 */

#include "RadioTask.h"
#include <compiler.h>
#include <asf.h>
#include "Spi_service.h"
#include "Spi_bg_task.h"
#include "Xbee.h"

/* See XMEGA AU manual page 146 and XMEGA 128A4U datasheet page 59*/
/*#define RADIO_SPI_CTRL_VALUE (SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc | SPI_ENABLE_bm | SPI_MASTER_bm)
 Using USART in SPI master mode instead */

#define SPI_BAUD_RATE (1000000) /**< 1MHz */

#ifndef F_CPU
#define F_CPU (sysclk_get_per_hz()) /**< System clock speed */
#endif

/**
 * @brief Control value to write to USART baud control regs
 *
 *  Ref https://github.com/abcminiuser/lufa/blob/master/LUFA/Drivers/Peripheral/XMEGA/SerialSPI_XMEGA.h
 */
#define SPI_BAUDCTRLVAL(Baud)       ((Baud < (F_CPU / 2)) ? ((F_CPU / (2 * Baud)) - 1) : 0)

/** SPI Master object for the radio bus */
spi_master_t radioSpiMaster;


/** 
 * @brief Initialize all things the radio task needs
 * 
 * Setup USART in SPI Master Mode.
 * Setup SPI master
 * Initialize radio driver
 *
 */
void init_radio_task(void)
{
    /* Initialize SPI interface on port E*/
    /* See XMEGA AU Manual page 146, page 280 */
    /* NOTE PINS ARE SETUP TO USE USART IN SPI MASTER MODE! */

    uint16_t baudrate = SPI_BAUDCTRLVAL(SPI_BAUD_RATE);

    sysclk_enable_peripheral_clock(&RADIO_SPI);
    RADIO_SPI.BAUDCTRLB = (uint8_t)((baudrate) >> 8); /* MSBs of Baud rate value. */
    RADIO_SPI.BAUDCTRLA = (uint8_t)(baudrate & 0xFF); /* LSBs of Baud rate value. */
    RADIO_SPI.CTRLA = 0x10; /* RXCINTLVL = 1, other 2 disabled */
    RADIO_SPI.CTRLB = 0x18; /* Enable RX and TX */
    RADIO_SPI.CTRLC = 0xC0; /* MSB first, mode 0. PMODE, SBMODE, CHSIZE ignored by SPI */

    init_spi_master_service(&radioSpiMaster, &RADIO_SPI, &RADIO_SPI_PORT, spi_bg_task);
    spi_bg_add_master(&radioSpiMaster);

    /* run initialization for radio driver */
    xbee_init(&radioSpiMaster);
}

/**
 * Runs high level Radio state machine and responsibilities
 *
 * This task receives messages from the sensors and from the control loop, and
 * sends them to the radio. It also receives messages from the radio and 
 * transfers them to the control loop.
 */
void radio_task_func(void)
{
    static uint32_t radio_timer = 0;
    static uint8_t radio_debug_payload[12] = "Hello World";

   if(is_xbee_pkt_rdy())
   {
        xbee_handleRxAPIFrame();
   }

   /* Debug!!! */
   if(radio_timer % 100 == 0)
   {
       xbee_tx_payload((void *)radio_debug_payload, 12);
   }
   /* Handle sensor tx queue */
   /* Handle control loop tx queue */
}

/** Interrupt service routine for the USART RXC interrupt on port E. */
ISR(RADIO_SPI_INT)
{
    xbee_SPI_ISR(&radioSpiMaster);
}
