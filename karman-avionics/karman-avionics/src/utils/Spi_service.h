/*
 * Filename: Spi_service.h
 *
 * Created: 11/18/2016 7:33:29 PM
 * Author: Andrew Kaster
 *
 * Description: This is the header for the SPI service utility.
 *              This utility registers a background function, and
 *              handles requests on the SPI bus. 
 *
 *              NOTE: Any pins used by this utility MUST be
 *              configured before any functions are called. 
 *              
 *              MOSI -- Output, Totem pole
 *              MISO -- Input, Totem pole
 *              SCK  -- Output, Totem pole
 *              
 *              Every chip select pin: Output, Wired AND.
 *
 *     This code is loosely based off of AVR1309, with many modifications for our use case.
 *
 */ 

#ifndef SPI_SERVICE_H_
#define SPI_SERVICE_H_

#include <compiler.h>
#include <asf.h>
#include "Background.h"

#define SPI_MASTER_QUEUE_DEPTH (10)


/* Information about the chip select pin for the device to be contacted */
typedef struct
{
    PORT_t      *csPort;        /* The port that the chip select pin is on */
    uint8_t     pinBitMask;     /* The bitmask for the pin. (1 << pinNum) */
} chip_select_info_t;

/* Structure to define parameters to give to the SPI service */
typedef struct  
{
  chip_select_info_t    csInfo;     /* Information about chip select pin */
  volatile void         *sendBuff;  /* Buffer to send data from */
  uint16_t               sendLen;    /* How many bytes to send */
  volatile uint8_t      bytesSent;  /* How many bytes have already been sent */
  volatile void         *recvBuff;  /* Buffer to store the result in */
  uint16_t               recvLen;    /* How many bytes to expect from the device */
  volatile uint8_t      bytesRecv;  /* How many bytes have actually been recieved */
  volatile Bool         *complete;   /* Complete flag */
  Bool                  valid;      /* Valid flag. Is this a valid request? */
} spi_request_t;

/* Struct to define the SPI interface to use. Note that there needs to exist one
 * for every port that needs a Master. Note that for best effect, this should be declared
 * globally. We're in C, so the idea of a singleton doesn't really work, but that's intent.
 */
typedef struct
{
    SPI_t *master; /* The SPI module associated with this struct */
    PORT_t *port; /* The port the master is on */

    /* The fields necessary to implement a queue. An array, and two indecies */
    volatile spi_request_t  requestQueue[SPI_MASTER_QUEUE_DEPTH];
    volatile uint8_t        front;
    volatile uint8_t        back;

    /* Flag to indicate if the master is busy or not */
    volatile Bool           masterBusy;
} spi_master_t;

Bool init_spi_master_service(spi_master_t *master,
                             SPI_t *regSet,
                             PORT_t *port,
                             background_func_t taskName);

Bool spi_master_enqueue(spi_master_t *spi_interface,
                        chip_select_info_t *csInfo,
                        volatile void *sendBuff,
                        uint16_t sendLen,
                        volatile void *recvBuff,
                        uint16_t recvLen,
                        volatile Bool *complete);

Bool spi_master_dequeue(spi_master_t *spi_interface);

void spi_master_ISR(spi_master_t *spi_interface);

Bool spi_master_initate_request(spi_master_t *spi_interface);

Bool spi_master_blocking_send_request(spi_master_t *spi_interface,
                                      chip_select_info_t *csInfo,
                                      volatile void *sendBuff,
                                      uint16_t sendLen,
                                      volatile void *recvBuff,
                                      uint16_t recvLen,
                                      volatile Bool *complete);


#define spi_master_finish_request(reqPtr)       (reqPtr->csInfo.csPort->OUTCLR = reqPtr->csInfo.pinBitMask)
#define spi_master_request_complete(master)     (*(master->requestQueue[master->front].complete) = true)

#endif /* SPI_SERVICE_H_ */
