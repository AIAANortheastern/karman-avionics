/**
 * @file SensorTask.c
 *
 * @brief Sensor Task Function and Intialization
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
#include "ADXL375.h"

/* See XMEGA AU manual page 146 and XMEGA 128A4U datasheet page 59*/
/*#define SENSOR_SPI_CTRL_VALUE (SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc | SPI_ENABLE_bm | SPI_MASTER_bm)
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

/** SPI Master object for the sensor bus */
spi_master_t sensorSpiMaster;

/* external control data structures for each sensor */
extern gAltimeterControl;
extern gHighGAccelerometer;

/** 
 * @brief Initialize all things the radio task needs
 * 
 * Setup USART in SPI Master Mode.
 * Setup SPI master
 * Intialize all sensor drivers
 */
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

    /* run initialization for all sensors. Most of these names are out of date */

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
	
	ADXL375_init(&sensorSpiMaster);
}

/**
 * @brief High level sensor operations
 *
 * This task runs all sensor state machine functions and passes the updated data to
 * the main control loop and the radio.
 */
void sensor_task_func(void)
{
    sensor_status_t curr_status;

    curr_status = ms5607_02ba03_get_data();

    if (curr_status == SENSOR_COMPLETE)
    {
        /* Do fancy things with current temp/pressure data */
    }

	curr_status = ADXL375_get_data();

	if (curr_status == SENSOR_COMPLETE)
	{
		volatile uint16_t a = 0;
		while(!a);
		/* Do fancy things with current high g accel data */
	}

    /* ----TEMPLATE----
     * curr_status = <foo>_get_data();
     * if (curr_status == SENSOR_COMPLETE)
     * {
     *    Do fancy things with current sensor's data
     * }
     *
     */

}

/** Interrupt service routine for the USART RXC interrupt on port D. */
ISR(SENSOR_SPI_INT)
{
    spi_master_ISR(&sensorSpiMaster);   
}
