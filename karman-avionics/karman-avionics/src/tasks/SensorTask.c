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
#define SENSOR_SPI_CTRL_VALUE (SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc | SPI_ENABLE_bm | SPI_MASTER_bm)

spi_master_t sensorSpiMaster;


/* Initialize all things the sensor task needs.*/
void init_sensor_task(void)
{
    /* Initialize SPI interface on port D*/
    /* See XMEGA AU Manual page 146, page 276 */
    sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_SPI);
    SENSOR_SPI_PORT.DIRSET = SENSOR_MOSI;
    SENSOR_SPI_PORT.DIRSET = SENSOR_SCLK;
    SENSOR_SPI_PORT.DIRCLR = SENSOR_MISO;
    SENSOR_SPI.CTRL = SENSOR_SPI_CTRL_VALUE;
    SENSOR_SPI.INTCTRL = SPI_INTLVL_LO_gc;

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

/* Interrupt service routine for the SPI interrupt on port C. */
ISR(SPIC_INT_vect)
{
    spi_master_ISR(&sensorSpiMaster);   
}
