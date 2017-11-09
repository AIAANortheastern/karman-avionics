/**
 * @file Spi_service.c
 *
 * Created: 11/18/2016 7:33:16 PM
 * Author: Andrew Kaster
 *
 * @brief SPI App-facing API
 *
 * Description: This is the implementation of the SPI service
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

#include "Spi_service.h"
#include "Background.h"
#include <stdint.h>
#include <string.h>

/** The size of every SPI master's queue */
#define SPI_MASTER_QUEUE_SIZE (SPI_MASTER_QUEUE_DEPTH*sizeof(spi_request_t))

/** 
 * @brief Initialize an SPI master object
 * @return bool - Whether or not it initialized successfully.
 * 
 * @param masterObj -  Spi master object for a given SPI bus
 * @param regSet - The hardware peripheral associated with the SPI bus
 * @param port - The port this is being initialized on.
 * @param taskName - A function to be run in the background that process its queue
 * initializes an SPI master servicer object
 */
Bool init_spi_master_service(spi_master_t *masterObj, USART_t *regSet, PORT_t *port, background_func_t taskName)
{
    Bool initSuccess = true;

    masterObj->master = regSet;
    masterObj->port = port;

    /* clear out the queue, to make sure it's empty */
    memset((void *)(masterObj->requestQueue), 0, SPI_MASTER_QUEUE_SIZE);
    masterObj->front = 0;
    masterObj->back = 0;

    if(!is_background_function(taskName))
    {
        if (add_background_function(taskName) == BKGND_FUNC_FAILURE)
        {
            initSuccess = false;
        }
    }

    return initSuccess;
}

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
                            Bool keep_cs_low)
{
    Bool createStatus = true;
    uint8_t newIndex = spi_interface->back;
    volatile spi_request_t *newRequest = NULL;

    /** first check, Are front and back the same? This should only be true if a) the queue
     * is empty, or b) the queue is full. If they are, don't modify the new index.
     * otherwise, add one.
     */
    newIndex += (spi_interface->front == spi_interface->back) ? 0 : 1;

    /** Next check for index wrapping */
    if(newIndex == SPI_MASTER_QUEUE_DEPTH)
    {
        newIndex = 0;
    }

    /** Check for collisions on addition */
    if((spi_interface->requestQueue[newIndex].valid == true) &&
        (spi_interface->back == spi_interface->front))
    {
        createStatus = false;
    }

    /** If it's safe, add the new entry */
    if (createStatus == true)
    {
        newRequest = &spi_interface->requestQueue[newIndex];

        newRequest->csInfo.csPort = csInfo->csPort;
        newRequest->csInfo.pinBitMask = csInfo->pinBitMask;

        if(keep_cs_low) {
            newRequest->raise_cs = false;
        }else {
            newRequest->raise_cs = true;
        }

        newRequest->sendBuff = sendBuff;
        newRequest->sendLen = sendLen;
        newRequest->bytesSent = 0;

        newRequest->recvBuff = recvBuff;
        newRequest->recvLen = recvLen;
        newRequest->bytesRecv = 0;

        newRequest->complete = complete;
        *(newRequest->complete) = false;
        newRequest->valid = true;

        spi_interface->back = newIndex;
    }

    return createStatus;
}

/** 
 * @brief Dequeue an item from an SPI master's queue
 * 
 * @param spi_interface The SPI master to dequeue from
 * @return True on success, false on failure 
 *
 * Pop an item from the request queue by invalidating the entry, then fix up the
 * front and back indexes
*/
Bool spi_master_dequeue(spi_master_t *spi_interface)
{
    Bool popStatus = true;
    uint8_t oldFront = spi_interface->front;
    /* If front == back, there's one entry or no entries */
    uint8_t newFront = (oldFront == spi_interface->back) ? (oldFront) : (oldFront + 1);

    /* Handle wrap-around */
    if(newFront >= SPI_MASTER_QUEUE_DEPTH)
    {
       newFront = 0;
    }

    /* If there wasn't an entry to pop */
    if(spi_interface->requestQueue[oldFront].valid == false)
    {
        popStatus = false;
    }
    else
    {
        /* Invalidate the old front */
        spi_interface->requestQueue[oldFront].valid = false;
        /* Change the new front index to be one further in the queue (assuming no wrapping, else 0) */
        spi_interface->front = newFront;
    }

    return popStatus;
}

/**
 * @brief Start a transaction on an SPI bus
 *
 * @param spi_interface The SPI master object that controls the desired interface
 * @return True on success, false on failure
 *
 * Instructs the SPI interface to start the request at the beginning of its queue.
 * Also pulls the chip select line low for the enqueued request.
 */
Bool spi_master_initate_request(spi_master_t *spi_interface)
{
    Bool initiateSuccess = true;
    volatile spi_request_t *frontQueue = &spi_interface->requestQueue[spi_interface->front];
    
    if(frontQueue->valid == false)
    {
        initiateSuccess = false;
    }
    else
    {
        /** Start the request */
        *(frontQueue->complete) = false;
        frontQueue->bytesRecv = 0;
        frontQueue->bytesSent = 0;
        
        /** Mark this device as "busy" */
        spi_interface->masterBusy = true;
        /** Enable chip select for the device in this request */
        frontQueue->csInfo.csPort->OUTCLR = frontQueue->csInfo.pinBitMask;

        /** Write to the spi master data. this will send the first byte. */
        spi_interface->master->DATA = ((uint8_t *)(frontQueue->sendBuff))[0];
        frontQueue->bytesSent++;

    }
    return initiateSuccess;
}

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
void spi_master_ISR(spi_master_t *spi_interface)
{
    volatile uint8_t *dataSent, *dataRecv;
    uint16_t dataToSend, dataToRecv, dummyByte;
    Bool moreToDo;

    /** Look at the front of the queue */
    volatile spi_request_t *currRequest = &spi_interface->requestQueue[spi_interface->front];
    dataSent = &(currRequest->bytesSent);
    dataRecv = &(currRequest->bytesRecv);
    dataToSend = currRequest->sendLen;
    dataToRecv = currRequest->recvLen;

    /** If there's still bytes to receive, keep receiving them. */
    if ((*dataRecv) < dataToRecv)
    {
        ((uint8_t *)(currRequest->recvBuff))[(*dataRecv)] = spi_interface->master->DATA;
        (*dataRecv)++;
    }
    /** If we don't care what's in the buffer, read the register then ignore the value. */
    /** NOTE AS WE ARE USING THE RXC INTERRUPT DATA MUST BE READ TO CLEAR THE INTERRUPT */
    else
    {
        dummyByte = spi_interface->master->DATA;
    }

    /** If there's still bytes to send, keep sending them*/
    if((*dataSent) < dataToSend)
    {
        spi_interface->master->DATA = ((uint8_t *)(currRequest->sendBuff))[(*dataSent)];
        (*dataSent)++;
    }
    /** Otherwise, if  want to recieve more data but we don't have any more to send... */
    else if((*dataRecv) < dataToRecv)
    {
        dummyByte = 0x00;
        /** Send a dummy byte to clock out the next bits of data. */
        spi_interface->master->DATA = dummyByte;
    }

    /** Check if are we done */
    moreToDo = (*dataSent < dataToSend) ? true : false;
    moreToDo |= (*dataRecv < dataToRecv) ? true : false;

    if(!moreToDo)
    {
        /** If we're done, raise chip select again. NOTE: This is enabled by default. 
         * There are a few special cases (i.e. Altimeter Reset procedure) that
         * require it to be held low.
        */
        if(currRequest->raise_cs){
            spi_master_finish_request(currRequest);
        }
        /** Inform the initiator that the request has completed*/
        spi_interface->masterBusy = false;
        spi_master_request_complete(spi_interface);
        /** Dequeue the request from the list*/
        spi_master_dequeue(spi_interface);
    }
}

/*****************************************************************************/
/*                      BEGIN BLOCKING FUNCTIONS                             */
/*              Only use these during startup! Do NOT use after              */
/*              the scheduler has started...unless you know what you're doing*/
/*****************************************************************************/


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
                                 Bool keep_cs_low)
{
    /** In the future we might add a timeout..? */
    Bool retVal = true;

    spi_master_enqueue(spi_interface, csInfo, sendBuff, sendLen, recvBuff, recvLen, complete, keep_cs_low);
    spi_master_initate_request(spi_interface);

    while((*complete) != true)
    {
        asm("");/** Do nothing while waitng. empty Asm here just to make sure we keep the loop as a loop.
                 * The fact that complete is a pointer to a volatile variable should be enough though.
                 */
    }
    
    /** The ISR routine dequeues the request */

    return retVal;
}
