/*
 * SensorTask.c
 *
 * Created: 12/1/2016 7:19:19 PM
 *  Author: Andrew Kaster
 */ 

#include <compiler.h>
#include <asf.h>
#include "SensorTask.h"
#include "Spi_service.h"
#include "Spi_bg_task.h"
#include "Tasks.h"

#include "ms5607-02ba03.h"

/* See XMEGA AU manual page 146 and XMEGA 128A4U datasheet page 59*/
/*#define SENSOR_SPI_CTRL_VALUE (SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc | SPI_ENABLE_bm | SPI_MASTER_bm)
 Using USART in SPI master mode instead */

#define SPI_BAUD_RATE (1000000) /* 1MHz */

#ifndef F_CPU
#define F_CPU (sysclk_get_per_hz())
#endif

/* https://github.com/abcminiuser/lufa/blob/master/LUFA/Drivers/Peripheral/XMEGA/SerialSPI_XMEGA.h */
#define SPI_BAUDCTRLVAL(Baud)       ((Baud < (F_CPU / 2)) ? ((F_CPU / (2 * Baud)) - 1) : 0)

spi_master_t sensorSpiMaster;


/* Initialize all things the sensor task needs.*/
void init_sensor_task(void)
{
    /* Initialize SPI interface on port D*/
    /* See XMEGA AU Manual page 146, page 280 */
    /* NOTE PINS ARE SETUP TO USE USART IN SPI MASTER MODE! */

    uint16_t baudrate = SPI_BAUDCTRLVAL(SPI_BAUD_RATE);

    sysclk_enable_peripheral_clock(&SENSOR_SPI);
    SENSOR_SPI.BAUDCTRLB = (uint8_t)((baudrate) >> 8); /* MSBs of Baud rate value. */
    SENSOR_SPI.BAUDCTRLA = (uint8_t)(baudrate & 0xFF); /* LSBs of Baud rate value. */
    SENSOR_SPI.CTRLA = 0x10; /* RXCINTLVL = 1, other 2 disabled */
    SENSOR_SPI.CTRLB = 0x18; /* Enable RX and TX */
    SENSOR_SPI.CTRLC = 0xC6; /* MSB first, mode 0. PMODE, SBMODE, CHSIZE ignored by SPI */

    init_spi_master_service(&sensorSpiMaster, &SENSOR_SPI, &SENSOR_SPI_PORT, spi_bg_task);
    spi_bg_add_master(&sensorSpiMaster);

    /* run initialization for all sensors */

    /* 1 axis accelerometer */
    /* init_mma6855bkcw() */

    /* 3 axis accelerometer*/
    /* init_fxls8471qr1() */

    /* 3 axis gyro */
    /* init_i3g4250d() */

    /* temp/humidity */
    /* init_si7021-a20() */

    /* altimeter/pressure */
    ms5607_02ba03_init(&sensorSpiMaster);
}

/* This task runs all sensor getData functions and passes the updated ones to the radio*/
/* TODO: Think about using the timer's count instead of an invoke count. It will be more accurate */
void sensor_task_func(void)
{

    ms5607_02ba03_get_data();

}

/* Interrupt service routine for the SPI interrupt on port D. */
// USARTD0_RXC_vect
// CTRLA = 0x10; // RXCINTLVL = 1
// CTRLB = 0x18; 
// CTRLC = 0xC6; // MSB first, mode 0. PMODE, SBMODE, CHSIZE ignored by SPI
// BAUDB = (1000000 >> 8); //use SPI_BAUD_RATE instead ie #define SPI_BAUD_RATE (1000000)
// BAUDA = (1000000 & 0xFF);
ISR(SENSOR_SPI_INT)
{
    spi_master_ISR(&sensorSpiMaster);   
}
