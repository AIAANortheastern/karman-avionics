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

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
     /* TODO Enable IO Pins */
     /* TODO Enable peripheral clocks (TWI, SPI, USART)*/

     /* Initalizes SPI for sensors, and sets up CS pins for all sensors */
     init_sensor_task();

     /* Initailizes SPI for radio module, sets up cs pin for it */
     init_radio_task();
}
