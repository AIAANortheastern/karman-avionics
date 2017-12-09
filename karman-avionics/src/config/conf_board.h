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
#define RADIO_GPIO_PORT (PORTF) /**< See schematic */
#define RADIO_CTS   (1 << 7) /**< Input */
#define RADIO_RTS   (1 << 6) /**< Output. https://www.digi.com/support/forum/3314/hardware-flow-control-of-xbee-module */
#define RADIO_CS    (1 << 5) /**< Output */
#define RADIO_ATTEN (1 << 4) /**< Input */
#define RADIO_ATTEN_PINCTRL PIN5CTRL /**< Config w/rising/falling edge. DO NOT add parentheses! */

#define RADIO_SPI_PORT (PORTE) /**< See schematic */
#define RADIO_USART (USARTE1) /**< USARTE1 */
#define RADIO_MOSI (1 << 7) /**< Output */
#define RADIO_MISO (1 << 6) /**< Input  */
#define RADIO_SCLK (1 << 5) /**< Output */ 
#define RADIO_SPI_INT (USARTE1_RXC_vect) /**< Data RX interrupt */

/*** FLASH MEMORY ***/
#define FLASH_PORT (PORTC) /**< See schematic */
#define FLASH_SPI (USARTC1) /**< USARTC1 */
#define FLASH_MOSI (1 << 7) /**< Output */
#define FLASH_MISO (1 << 6) /**< Input  */
#define FLASH_SCLK (1 << 5) /**< Output */
#define FLASH_CS   (1 << 0) /**< Output */
#define FLASH_SPI_INT (USARTC1_RXC_vect) /**< Data RX interrupt */

/*** SENSORS ***/
#define SENSOR_SPI_PORT (PORTD) /**< See schematic */
#define SENSOR_SPI (USARTD0) /**< USARTD0 */
#define SENSOR_MOSI (1 << 3) /**< Output */
#define SENSOR_MISO (1 << 2) /**< Input  */
#define SENSOR_SCLK (1 << 1) /**< Output */
#define SENSOR_SPI_INT (USARTD0_RXC_vect) /**< Data RX interrupt */

/* USB */
#define USB_PORT (PORTD) /**< See schematic */
#define USB_SENSE (1 << 5) /**< Input  */
#define USB_SENSE_PINCTRL PIN5CTRL /**< Config w/rising/falling edge. DO NOT add parentheses! */

/*** HIGH-G ACCELEROMTERS ***/
#define HIGHG_ACC1_PORT (PORTC) /**< See schematic */
#define HIGHG_ACC1_CS  (1 << 4) /**< Output */
#define HIGHG_ACC1_INT (1 << 3) /**< Input  */
#define HIGHG_ACC1_INT_PINCTRL PIN3CTRL /**< Config w/rising/falling edge. DO NOT add parentheses! */

#define HIGHG_ACC2_PORT (PORTE) /**< See schematic */
#define HIGHG_ACC2_CS  (1 << 3) /**< Output */
#define HIGHG_ACC2_INT (1 << 2) /**< Input  */
#define HIGHG_ACC2_INT_PINCTRL PIN2CTRL /**< Config w/rising/falling edge. DO NOT add parentheses! */

/*** GPS ***/
#define GPS_PORT (PORTC) /**< See schematic */
#define GPS_INT (1 << 2) /**< Input  */
#define GPS_CS  (1 << 1) /**< Output */

/*** IMU -- GPS/ACC/MAG ***/
#define IMU_ACC1_CS_PORT (PORTD) /**< See schematic */
#define IMU_ACC1_CS   (1 << 4) /**< Output */
#define IMU_ACC1_INT_PORT (PORTE) /**< See schematic */
#define IMU_ACC1_INT (1 << 0) /**< Input  */
#define IMU_ACC1_INT_PINCTRL PIN0CTRL /**< Config w/rising/falling edge. DO NOT add parentheses! */

#define IMU_ACC2_PORT (PORTF) /**< See schematic */
#define IMU_ACC2_INT  (1 << 2) /**< Input  */
#define IMU_ACC2_INT_PINCTRL PIN2CTRL /**< Config w/rising/falling edge. DO NOT add parentheses! */
#define IMU_ACC2_CS   (1 << 1) /**< Output */

#define IMU_GYRO1_PORT (PORTD) /**< See schematic */
#define IMU_GYRO1_CS (1 << 0) /**< Output */

#define IMU_GYRO2_PORT (PORTE) /**< See schematic */
#define IMU_GYRO2_CS (1 << 4) /**< Output */

#define IMU_MAG1_PORT (PORTE) /**< See schematic */
#define IMU_MAG1_CS (1 << 1) /**< Output */

#define IMU_MAG2_PORT (PORTF) /**< See schematic */
#define IMU_MAG2_CS (1 << 3) /**< Output */

/*** ALTIMETER ***/
#define ALTIMETER_PORT (PORTF) /**< See schematic */
#define ALTIMETER_CS   (1 << 0) /**< Output */

/*** PYROTECHNICS ***/
#define PYRO_12_PORT (PORTA) /**< See schematic */

#define PYRO_SENSE2B (1 << 7) /**< Input  */
#define PYRO_2A      (1 << 6) /**< Output */ 
#define PYRO_2B      (1 << 5) /**< Output */
#define PYRO_SENSE2A (1 << 4) /**< Input  */
#define PYRO_SENSE1B (1 << 3) /**< Input  */
#define PYRO_1A      (1 << 2) /**< Output */
#define PYRO_1B      (1 << 1) /**< Output */
#define PYRO_SENSE1A (1 << 0) /**< Input  */

#define PYRO_34_PORT (PORTB) /**< See schematic */

#define PYRO_SENSE4B (1 << 7) /**< Input  */
#define PYRO_4A      (1 << 6) /**< Output */
#define PYRO_4B      (1 << 5) /**< Output */
#define PYRO_SENSE4A (1 << 4) /**< Input  */
#define PYRO_SENSE3B (1 << 3) /**< Input  */
#define PYRO_3A      (1 << 2) /**< Output */
#define PYRO_3B      (1 << 1) /**< Output */
#define PYRO_SENSE3A (1 << 0) /**< Input  */

#endif // CONF_BOARD_H
