/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

/*** RADIO ***/
#define RADIO_GPIO_PORT (PORTF)
#define RADIO_CTS   (1 << 7) /* Input */
#define RADIO_RTS   (1 << 6) /* Output */ /* https://www.digi.com/support/forum/3314/hardware-flow-control-of-xbee-module */
#define RADIO_CS    (1 << 5) /* Output */
#define RADIO_ATTEN (1 << 4) /* Input */

#define RADIO_SPI_PORT (PORTE)
#define RADIO_SPI (SPIE) // USARTE1
#define RADIO_MOSI (1 << 7) /* Output */
#define RADIO_MISO (1 << 6) /* Input  */
#define RADIO_SCLK (1 << 5) /* Output */ 

/*** FLASH MEMORY ***/
#define FLASH_PORT (PORTC)
#define FLASH_SPI (SPIC) // USARTC1
#define FLASH_MOSI (1 << 7) /* Output */
#define FLASH_MISO (1 << 6) /* Input  */
#define FLASH_SCLK (1 << 5) /* Output */
#define FLASH_CS   (1 << 0) /* Output */

/*** SENSORS ***/
#define SENSOR_SPI_PORT (PORTD)
#define SENSOR_SPI (SPID) //USARTD0
#define SENSOR_MOSI (1 << 3) /* Output */
#define SENSOR_MISO (1 << 2) /* Input  */
#define SENSOR_SCLK (1 << 1) /* Output */

/*** HIGH-G ACCELEROMTERS ***/
#define HIGHG_ACC1_PORT (PORTC)
#define HIGHG_ACC1_CS  (1 << 4) /* Output */
#define HIGHG_ACC1_INT (1 << 3) /* Input  */

#define HIGHG_ACC2_PORT (PORTE)
#define HIGHG_ACC2_CS  (1 << 3) /* Output */
#define HIGHG_ACC2_INT (1 << 2) /* Input  */

/*** GPS ***/
#define GPS_PORT (PORTC)
#define GPS_INT (1 << 2) /* Input  */
#define GPS_CS  (1 << 1) /* Output */

/*** IMU -- GPS/ACC/MAG ***/
#define IMU_ACC1_CS_PORT (PORTD)
#define IMU_ACC1_CS   (1 << 4) /* Output */
#define IMU_ACC1_INT_PORT (PORTE)
#define IMU_ACC1_INT (1 << 0) /* Input  */

#define IMU_ACC2_PORT (PORTF)
#define IMU_ACC2_INT  (1 << 2) /* Input  */
#define IMU_ACC2_CS   (1 << 1) /* Output */

#define IMU_GYRO1_PORT (PORTD)
#define IMU_GYRO1_CS (1 << 0) /* Output */

#define IMU_GYRO2_PORT (PORTE)
#define IMU_GYRO2_CS (1 << 4) /* Output */

#define IMU_MAG1_PORT (PORTE)
#define IMU_MAG1_CS (1 << 1) /* Output */

#define IMU_MAG2_PORT (PORTF)
#define IMU_MAG2_CS (1 << 3) /* Output */

/*** ALTIMETER ***/
#define ALTIMETER_PORT (PORTF)
#define ALTIMETER_CS   (1 << 0) /* Output */

/*** PYROTECHNICS ***/
#define PYRO_12_PORT (PORTA)

#define PYRO_SENSE2B (1 << 7) /* Input  */
#define PYRO_2A      (1 << 6) /* Output */ 
#define PYRO_2B      (1 << 5) /* Output */
#define PYRO_SENSE2A (1 << 4) /* Input  */
#define PYRO_SENSE1B (1 << 3) /* Input  */
#define PYRO_1A      (1 << 2) /* Output */
#define PYRO_1B      (1 << 1) /* Output */
#define PYRO_SENSE1A (1 << 0) /* Input  */

#define PYRO_34_PORT (PORTB)

#define PYRO_SENSE4B (1 << 7) /* Input  */
#define PYRO_4A      (1 << 6) /* Output */
#define PYRO_4B      (1 << 5) /* Output */
#define PYRO_SENSE4A (1 << 4) /* Input  */
#define PYRO_SENSE3B (1 << 3) /* Input  */
#define PYRO_3A      (1 << 2) /* Output */
#define PYRO_3B      (1 << 1) /* Output */
#define PYRO_SENSE3A (1 << 0) /* Input  */

#endif // CONF_BOARD_H
