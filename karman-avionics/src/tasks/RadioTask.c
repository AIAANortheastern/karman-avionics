/**
 * @file RadioTask.c
 *
 * @brief Radio Task Function and Initialization
 *
 * Created: 12/4/2016 10:19:36 PM
 *  Author: Andrew Kaster
 */

#include "RadioTask.h"
#include <compiler.h>
#include <asf.h>
#include "Xbee.h"
#include "SensorDefs.h"

#define USART_SERIAL                     &RADIO_USART
#define USART_SERIAL_BAUDRATE            9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

extern sensor_data_t gCurrSensorValues;
xbee_tx_data_t xbeeSensorData;



/** 
 * @brief Initialize all things the radio task needs
 * 
 * Setup USART
 * Initialize radio driver
 *
 */
void init_radio_task(void)
{
    /* Initialize USART interface on port E*/
    /* See XMEGA AU Manual page 146, page 280 */

    static usart_rs232_options_t USART_SERIAL_OPTIONS = {
        .baudrate = USART_SERIAL_BAUDRATE,
        .charlength = USART_SERIAL_CHAR_LENGTH,
        .paritytype = USART_SERIAL_PARITY,
        .stopbits = USART_SERIAL_STOP_BIT
    };

    sysclk_enable_module(SYSCLK_PORT_E, PR_USART1_bm); 
    usart_init_rs232(USART_SERIAL, &USART_SERIAL_OPTIONS);
    usart_set_rx_interrupt_level(USART_SERIAL, USART_INT_LVL_LO);
    
    /* run initialization for radio driver */
    xbee_init();
}

/**
 * Runs high level Radio state machine and responsibilities
 *
 * This task receives messages from the sensors and from the control loop, and
 * sends them to the radio. It also receives messages from the radio and 
 * transfers them to the control loop.
 */
void radio_task_func(void)
{
    static uint32_t radio_timer = 0;
    //static uint8_t radio_debug_payload[13] = "Hello World\n\r";
	xbeeSensorData.header = 'A';
	xbeeSensorData.sensorData = *(&gCurrSensorValues);
	xbeeSensorData.checksum = '\n';

   /* Debug!!! */
   if(radio_timer % 200 == 0)
   {
       xbee_tx_payload((void *)(&xbeeSensorData), sizeof(xbeeSensorData));
   }

   radio_timer++;
   /* Handle sensor tx queue */
   /* Handle control loop tx queue */ 
}

/** Interrupt service routine for the USART RXC interrupt on port E. */
ISR(RADIO_SPI_INT)
{
    xbee_RX_ISR();
}
