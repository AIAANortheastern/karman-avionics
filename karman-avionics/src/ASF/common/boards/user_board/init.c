/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>
#include "SensorTask.h"
#include "RadioTask.h"
#include "FlashMem.h"
#include "USBUtils.h"

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
    */

    /* setup Pin directions on all GPIO pins */
    /* FOR DIRECTION: 0 = input, 1 = output */
    /* SET --> OUTPUT, CLEAR --> INPUT */

    RADIO_GPIO_PORT.DIRCLR = RADIO_CTS | RADIO_ATTEN;
    RADIO_GPIO_PORT.DIRSET = RADIO_RTS | RADIO_CS;
    RADIO_SPI_PORT.DIRCLR = RADIO_MISO;
    RADIO_SPI_PORT.DIRSET = RADIO_MOSI | RADIO_SCLK;

    FLASH_PORT.DIRCLR = FLASH_MISO;
    FLASH_PORT.DIRSET = FLASH_MOSI | FLASH_SCLK | FLASH_CS;

    SENSOR_SPI_PORT.DIRCLR = SENSOR_MISO;
    SENSOR_SPI_PORT.DIRSET = SENSOR_MOSI | SENSOR_SCLK;

    USB_PORT.DIRCLR = USB_SENSE;

    HIGHG_ACC1_PORT.DIRCLR = HIGHG_ACC1_INT;
    HIGHG_ACC1_PORT.DIRSET = HIGHG_ACC1_CS;

    HIGHG_ACC2_PORT.DIRCLR = HIGHG_ACC2_INT;
    HIGHG_ACC2_PORT.DIRSET = HIGHG_ACC2_CS;

    GPS_PORT.DIRCLR = GPS_INT;
    GPS_PORT.DIRSET = GPS_CS;

    IMU_ACC1_INT_PORT.DIRCLR = IMU_ACC1_INT;
    IMU_ACC1_CS_PORT.DIRSET = IMU_ACC1_CS;

    IMU_ACC2_PORT.DIRCLR = IMU_ACC2_INT;
    IMU_ACC2_PORT.DIRSET = IMU_ACC2_CS;

    IMU_GYRO1_PORT.DIRSET = IMU_GYRO1_CS;

    IMU_GYRO2_PORT.DIRSET = IMU_GYRO2_CS;

    IMU_MAG1_PORT.DIRSET = IMU_MAG1_CS;

    IMU_MAG2_PORT.DIRSET = IMU_MAG2_CS;

    ALTIMETER_PORT.DIRSET = ALTIMETER_CS;

    PYRO_12_PORT.DIRCLR = PYRO_SENSE2B | PYRO_SENSE2A | PYRO_SENSE1B | PYRO_SENSE1A;
    PYRO_12_PORT.DIRSET = PYRO_2A | PYRO_2B | PYRO_1A | PYRO_1B;

    PYRO_34_PORT.DIRCLR = PYRO_SENSE4B | PYRO_SENSE4A | PYRO_SENSE3B | PYRO_SENSE3A;
    PYRO_34_PORT.DIRSET = PYRO_4A | PYRO_4B | PYRO_3A | PYRO_3B;

    /* Pull all CS pins high */
    /* Radio and flash memory */
    RADIO_GPIO_PORT.OUTSET = RADIO_CS;
    FLASH_PORT.OUTSET = FLASH_CS;
    /* Sensors */
    HIGHG_ACC1_PORT.OUTSET = HIGHG_ACC1_CS;
    HIGHG_ACC2_PORT.OUTSET = HIGHG_ACC2_CS;
    GPS_PORT.OUTSET = GPS_CS;
    IMU_ACC1_CS_PORT.OUTSET = IMU_ACC1_CS;
    IMU_ACC2_PORT.OUTSET = IMU_ACC2_CS;
    IMU_GYRO1_PORT.OUTSET = IMU_GYRO1_CS;
    IMU_GYRO2_PORT.OUTSET = IMU_GYRO2_CS;
    IMU_MAG1_PORT.OUTSET = IMU_MAG1_CS;
    IMU_MAG2_PORT.OUTSET = IMU_MAG2_CS;
    ALTIMETER_PORT.OUTSET = ALTIMETER_CS;

    /* Pull all txd pins high */
    RADIO_SPI_PORT.OUTSET = RADIO_MOSI;
    SENSOR_SPI_PORT.OUTSET = SENSOR_MOSI;
    FLASH_PORT.OUTSET = FLASH_MOSI;


    /* Initializes SPI for sensors, and sets up CS pins for all sensors */
    //init_sensor_task();

    /* Initializes SPI for radio module, sets up cs pin for it */
    //init_radio_task();

    /* Initializes SPI for External Flash Memory, sets up CS pin for it */
    //init_flashmem();

    /* Initializes the USB CDC interface */
    //init_usb();
}
