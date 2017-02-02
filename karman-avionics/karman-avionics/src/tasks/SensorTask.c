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


uint32_t gSensorInvokeCount = 0;

spi_master_t sensorSpiMaster;


/* Initialize all things the sensor task needs. TODO add TWI to this. */
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

    /* altimeter */
    ms5607_02ba03_init(&sensorSpiMaster);
}

/* This task runs all sensor getData functions and passes the updated ones to the radio*/
/* TODO: Think about using the timer's count instead of an invoke count. It will be more accurate */
void sensor_task_func(void)
{
    /* This function should be called every 1.5ms */
    /* check mod counts of invokeCount */
    /* Mod counts are a way to ensure that each sensor gets polled when it needs to be. */
    uint8_t mod2Count = gSensorInvokeCount & 1;
    uint8_t mod4Count = gSensorInvokeCount & 3;

    if(mod4Count == 3)
    {
        /* Do stuff that needs to be run every 6ms at the 4.5ms mark */
    }

    if(mod2Count == 1)
    {
        /* Do stuff that needs to happen every 3ms */
    }

    /* Do more stuff that happens every time */

    /* Do radio pzaz */

    gSensorInvokeCount++;
}

/* Interrupt service routine for the SPI interrupt on port C. */
ISR(SPIC_INT_vect)
{
    spi_master_ISR(&sensorSpiMaster);   
}
