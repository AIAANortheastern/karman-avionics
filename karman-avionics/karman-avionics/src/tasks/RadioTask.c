/*
 * RadioTask.c
 *
 * Created: 12/4/2016 10:19:36 PM
 *  Author: Andrew Kaster
 */ 
#include "RadioTask.h"
#include <compiler.h>
#include <asf.h>
#include "Spi_service.h"
#include "Spi_bg_task.h"

/* See XMEGA AU manual page 146 and XMEGA 128A4U datasheet page 59*/
#define SPIE_MOSI (1 << 5) /* 0x20 */
#define SPIE_MISO (1 << 6) /* 0x40 */
#define SPIE_SCK  (1 << 7) /* 0x80 */
#define RADIO_SPI (SPID) /* Possible options are SPIC and SPID */
#define RADIO_SPI_PORT (PORTD)
#define RADIO_SPI_CTRL_VALUE (SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc | SPI_ENABLE_bm | SPI_MASTER_bm)

spi_master_t radioSpiMaster;


/* Initialize all things the radio task needs.*/
void init_radio_task(void)
{
    /* Initialize SPI interface on port D*/
    /* See XMEGA AU Manual page 146, page 276 */
    sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
    RADIO_SPI_PORT.DIRSET = SPIE_MOSI;
    RADIO_SPI_PORT.DIRSET = SPIE_SCK;
    RADIO_SPI_PORT.DIRCLR = SPIE_MISO;
    RADIO_SPI.CTRL = RADIO_SPI_CTRL_VALUE;
    RADIO_SPI.INTCTRL = SPI_INTLVL_LO_gc;

    init_spi_master_service(&radioSpiMaster, &RADIO_SPI, &RADIO_SPI_PORT, spi_bg_task);
    spi_bg_add_master(&radioSpiMaster);

    /* run initialization for radio driver */
    /* init_cc2500(); */
}

/* This task receives messages from the sensors and from the control loop, and sends them to the radio.
 * It also receives messages from the radio and transfers them to the control loop.
 */
void radio_task_func(void)
{
   /* Do the stuff*/
}

/* Interrupt service routine for the SPI interrupt on port E. */
ISR(SPID_INT_vect)
{
    spi_master_ISR(&radioSpiMaster);
}
