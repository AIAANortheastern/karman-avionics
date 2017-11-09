/**
 * @file Spi_service.h
 *
 * Created: 11/18/2016 7:33:29 PM
 * Author: Andrew Kaster
 *
 * @brief SPI App-facing API
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

/** Max number of entries in the queue */
#define SPI_MASTER_QUEUE_DEPTH (10)


/** Information about the chip select pin for the device to be contacted */
typedef struct
{
    PORT_t      *csPort;        /**< The port that the chip select pin is on */
    uint8_t     pinBitMask;     /**< The bitmask for the pin. (1 << pinNum) */
} chip_select_info_t;

/** Structure to define parameters to give to the SPI service */
typedef struct  
{
  chip_select_info_t    csInfo;     /**< Information about chip select pin */
  volatile void         *sendBuff;  /**< Buffer to send data from */
  uint16_t               sendLen;   /**< How many bytes to send */
  volatile uint8_t      bytesSent;  /**< How many bytes have already been sent */
  volatile void         *recvBuff;  /**< Buffer to store the result in */
  uint16_t               recvLen;   /**< How many bytes to expect from the device */
  volatile uint8_t      bytesRecv;  /**< How many bytes have actually been received */
  volatile Bool         *complete;  /**< Complete flag */
  Bool                  valid;      /**< Valid flag. Is this a valid request? */
  Bool                  raise_cs;   /**< */
} spi_request_t;

/** @brief Struct to define the SPI interface to use. 
 *
 * Note that there needs to exist one
 * for every port that needs a Master. Note that for best effect, this should be declared
 * globally. We're in C, so the idea of a singleton doesn't really work, but that's intent.
 */
typedef struct
{
    USART_t *master; /**< The USART module associated with this master */
    PORT_t *port;    /**< The port the master is on */
    /* The fields necessary to implement a queue. An array, and two indecies */
    volatile spi_request_t  requestQueue[SPI_MASTER_QUEUE_DEPTH]; /**< Array to hold queue items */
    volatile uint8_t        front; /**< Index of the front of the queue */
    volatile uint8_t        back;  /**< Index of the back of the queue */
    volatile Bool           masterBusy; /**< Flag to indicate if the master is busy or not */
} spi_master_t;

/** 
 * @brief Intialize an SPI master object
 * @return bool - Whether or not it initialized successfully.
 * 
 * @param masterObj -  Spi master object for a given SPI bus
 * @param regSet - The hardware peripheral assocaited with the SPI bus
 * @param port - The port this is being initialized on.
 * @param taskName - A function to be run in the background that process its queue
 * initializes an SPI master servicer object
 */
Bool init_spi_master_service(spi_master_t *master,
                             USART_t *regSet,
                             PORT_t *port,
                             background_func_t taskName);

/**
 * @brief Push function for queue.
 *
 * @param spi_interface The SPI master object to use
 * @param csInfo Chip select information for the hardware device to contact
 * @param sendBuff Caller's buffer containing the data to be sent out
 * @param sendLen Number of bytes to send
 * @param[out] recvBuff Caller's buffer to store response from device into
 * @param recvLen Number of bytes to receive
 * @param complete Flag to set true when the transaction is complete
 * @param keep_cs_low Flag to determine if we should disable pulling the CS high after the transaction is finished. WARNING: The caller will be required to pull the CS high again or the SPI interface will be broken!!!
 * @return True on success, false on failure
 *
 * The queue is wrapping, and so the array acts like a ring buffer
 * This makes detecting collisions a lot more fun. A lot of care is taken.
 * If front and back are the same, this can cause issues. Adding a new entry
 * when there is still an old entry there is bad. So, we check the valid flag
 * of the entry indexed by back. If it's valid, we can't add a new one. back 
 * will always point to the last entry in the queue upon entry to this function.
 * If there are no entries, back and front are the same. If there are three entries, 
 * then back will be the index for the last entry. So to create a new entry at the
 * back of the queue, we check if back + 1 is empty. We only want to update back
 * if we successfully add an entry.
 */
Bool spi_master_enqueue(spi_master_t *spi_interface,
                        chip_select_info_t *csInfo,
                        volatile void *sendBuff,
                        uint16_t sendLen,
                        volatile void *recvBuff,
                        uint16_t recvLen,
                        volatile Bool *complete,
                        Bool keep_cs_low);

/** 
 * @brief Dequeue an item from an SPI master's queue
 * 
 * @param spi_interface The SPI master to dequeue from
 * @return True on success, false on failure 
 *
 * Pop an item from the request queue by invalidating the entry, then fix up the
 * front and back indexes
*/
Bool spi_master_dequeue(spi_master_t *spi_interface);

/**
 * @brief Generic SPI Interrupt Service Routine
 *
 * @param spi_interface The SPI master object that controls the bus.
 *
 * The interrupt service routine should be called on a data receive interrupt.
 * NOTE: This function must be re-entrant from multiple iterrupts. If one
 * SPI interrupt is higher than another, it could pre-empt in the middle
 * Of processing. (But please don't do that, keep them the same level...)
 */
void spi_master_ISR(spi_master_t *spi_interface);

/**
 * @brief Start a transaction on an SPI bus
 *
 * @param spi_interface The SPI master object that controls the desired interface
 * @return True on success, false on failure
 *
 * Instructs the SPI interface to start the request at the beginning of its queue.
 * Also pulls the chip select line low for the enqueued request.
 */
Bool spi_master_initate_request(spi_master_t *spi_interface);

/**
 * @brief Send a request, but block the whole time while waiting for it to finish
 *
 * @param spi_interface The SPI master object to use
 * @param csInfo Chip select information for the hardware device to contact
 * @param sendBuff Caller's buffer containing the data to be sent out
 * @param sendLen Number of bytes to send
 * @param[out] recvBuff Caller's buffer to store response from device into
 * @param recvLen Number of bytes to receive
 * @param complete Flag to set true when the transaction is complete
 * @param keep_cs_low Flag to determine if we should disable pulling the CS high after the transaction is finished. WARNING: The caller will be required to pull the CS high again or the SPI interface will be broken!!!
 * @return True on success, false on failure
 *
 *
 * Instead of enqueueing a request and waiting for the SPI Background routine to
 * start it up, enqueue a request, start it up yourself, and then sit and wait for the
 * complete boolean to indicate it's done.
*/
Bool spi_master_blocking_send_request(spi_master_t *spi_interface,
                                 chip_select_info_t *csInfo,
                                 volatile void *sendBuff,
                                 uint16_t sendLen,
                                 volatile void *recvBuff,
                                 uint16_t recvLen,
                                 volatile Bool *complete,
                                 Bool keep_cs_low);

/** Pull the chip select pin high to de-select the device */
#define spi_master_finish_request(reqPtr)       (reqPtr->csInfo.csPort->OUTSET = reqPtr->csInfo.pinBitMask)
/** Set the master's complete flag to true so it is ready to initiate a new request */
#define spi_master_request_complete(master)     (*(master->requestQueue[master->front].complete) = true)

#endif /* SPI_SERVICE_H_ */
