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
#define SPIC_MOSI (1 << 5) /* 0x20 */
#define SPIC_MISO (1 << 6) /* 0x40 */
#define SPIC_SCK  (1 << 7) /* 0x80 */
#define SENSOR_SPI (SPIC) /* Possible options are SPIC and SPID */
#define SENSOR_SPI_PORT (PORTC)
#define SENSOR_SPI_CTRL_VALUE (SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc | SPI_ENABLE_bm | SPI_MASTER_bm)

spi_master_t sensorSpiMaster;


/* Initialize all things the sensor task needs.*/
void init_sensor_task(void)
{
    /* Initialize SPI interface on port C*/
    /* See XMEGA AU Manual page 146, page 276 */
    sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_SPI);
    SENSOR_SPI_PORT.DIRSET = SPIC_MOSI;
    SENSOR_SPI_PORT.DIRSET = SPIC_SCK;
    SENSOR_SPI_PORT.DIRCLR = SPIC_MISO;
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
void sensor_task_func(void)
{
    sensor_status_t curr_status;

    curr_status = ms5607_02ba03_get_data();

    if (curr_status == SENSOR_COMPLETE)
    {
        /* Do fancy things with current temp/pressure data */
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

/* Interrupt service routine for the SPI interrupt on port C. */
ISR(SPIC_INT_vect)
{
    spi_master_ISR(&sensorSpiMaster);   
}
