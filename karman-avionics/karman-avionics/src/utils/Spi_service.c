/*
 * Filename: Spi_service.h
 *
 * Created: 11/18/2016 7:33:16 PM
 * Author: Andrew Kaster
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

#define SPI_MASTER_QUEUE_SIZE (SPI_MASTER_QUEUE_DEPTH*sizeof(spi_request_t))

/* initalizes an SPI master service */
Bool init_spi_master_service(spi_master_t *masterObj, SPI_t *regSet, PORT_t *port, background_func_t taskName)
{
    Bool initSuccess = false;

    masterObj->master = regSet;
    masterObj->port = port;

    /* clear out the queue, to make sure it's empty */
    memset(masterObj->requestQueue, 0, SPI_MASTER_QUEUE_SIZE);
    masterObj->front = 0;
    masterObj->back = 0;

    if (!(add_background_function(taskName) == BKGND_FUNC_FAILURE))
    {
        initSuccess = true;
    }

    return initSuccess;
}

/* Push function for queue. The queue is wrapping, and so the array acts like a ring buffer
 * This makes detecting collisions a lot more fun. A lot of care is taken.
 * If front and back are the same, this can cause issues. Adding a new entry
 * when there is still an old entry there is bad. So, we check the valid flag
 * of the entry indexed by back. If it's valid, we can't add a new one. back 
 * will always point to the last entry in the queue upon entry to this function.
 * If there are no entries, back and front are the same. If there are three entries, 
 * then back will be the index for the last entry. So to create a new entry at the
 * back of the queue, we check if back + 1 is empty. We only want to update back
 * if we sucessfully add an entry.
 */
Bool spi_master_enqueue(spi_master_t *spi_interface,
                            chip_select_info_t *csInfo,
                            volatile void *sendBuff,
                            uint8_t sendLen,
                            volatile void *recvBuff,
                            uint8_t recvLen,
                            volatile Bool *complete)
{
    Bool createStatus = true;
    uint8_t newIndex = spi_interface->back;
    spi_request_t *newRequest = NULL;

    /* Are front and back the same? This should only be true if a) the queue
     * is empty, or b) the queue is full. If they are, don't modify the new index.
     * otherwise, add one.
     */
    newIndex += (spi_interface->front == spi_interface->back) ? 0 : 1;

    /* Check for index wrapping */
    if(newIndex == SPI_MASTER_QUEUE_DEPTH)
    {
        newIndex = 0;
    }

    /* Check for collisions on addition */
    if((spi_interface->requestQueue[newIndex].valid == true) &&
        (spi_interface->back == spi_interface->front))
    {
        createStatus = false;
    }

    /* we're safe to add the new entry */
    if (createStatus == true)
    {
        newRequest = &spi_interface->requestQueue[newIndex];

        newRequest->csInfo.csPort = csInfo->csPort;
        newRequest->csInfo.pinBitMask = csInfo->pinBitMask;

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

/* pop an item from the request queue by invalidating the entry */
Bool spi_master_dequeue(spi_master_t *spi_interface)
{
    Bool popStatus = true;
    uint8_t oldFront = spi_interface->front;
    uint8_t newFront = (oldFront == spi_interface->back) ? (oldFront) : (oldFront + 1);

    if(newFront >= SPI_MASTER_QUEUE_DEPTH)
    {
       newFront = 0;
    }

    if(spi_interface->requestQueue[oldFront].valid == false)
    {
        popStatus = false;
    }
    else
    {
        spi_interface->requestQueue[oldFront].valid = false;
        spi_interface->front = newFront;
    }

    return popStatus;
}

/* Instructs the SPI interface to start the request at the beginning of its queue */
Bool spi_master_initate_request(spi_master_t *spi_interface)
{
    Bool initiateSuccess = true;
    spi_request_t *frontQueue = &spi_interface->requestQueue[spi_interface->front];
    
    if(frontQueue->valid == false)
    {
        initiateSuccess = false;
    }
    else
    {
        /* Start the request */
        frontQueue->complete = false;
        frontQueue->bytesRecv = 0;
        frontQueue->bytesSent = 0;

        /* Enable chip select for the device in this request */
        frontQueue->csInfo.csPort->OUTSET = frontQueue->csInfo.pinBitMask;

        /* Write to the spi master data. this will send the first byte. */
        spi_interface->master->DATA = ((uint8_t *)(frontQueue->sendBuff))[0];
        frontQueue->bytesSent++;

    }
    return initiateSuccess;
}

/* The interrupt service routine should trigger when the first byte is sent back
 * or it's time to send the next byte.
 */
void spi_master_ISR(spi_master_t *spi_interface)
{
    static uint8_t dataSent, dataRecv, dataToSend, dataToRecv;
    static Bool moreToDo;

    /* Look at the front of the queue */
    spi_request_t *currRequest = &spi_interface->requestQueue[spi_interface->front];
    dataSent = currRequest->bytesSent;
    dataRecv = currRequest->bytesRecv;
    dataToSend = currRequest->sendLen;
    dataToRecv = currRequest->recvLen;

    /* If there's still bytes to recieve, keep recieving them. */
    if (dataRecv < dataToRecv)
    {
        ((uint8_t *)(currRequest->recvBuff))[dataRecv] = spi_interface->master->DATA;
        dataRecv++;
    }

    /* If there's still bytes to send, keep sending them*/
    if(dataSent < dataToSend)
    {
        spi_interface->master->DATA = ((uint8_t *)(currRequest->sendBuff))[dataSent];
        dataSent++;
    }

    /* are we done? */
    moreToDo = (dataSent < dataToSend) ? true : false;
    moreToDo = (dataRecv < dataToRecv) ? true : false;

    if(!moreToDo)
    {
        /* If we're done, raise chip select again*/
        spi_master_finish_request(currRequest);
        /* Inform the owner of the task that the request has completed*/
        spi_master_request_complete(spi_interface);
        /* Dequeue the task from the list*/
        spi_master_dequeue(spi_interface);
    }
}