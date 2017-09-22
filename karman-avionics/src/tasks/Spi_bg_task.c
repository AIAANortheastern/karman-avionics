/**
 * Spi_bg_task.c
 *
 * Created: 11/19/2016 1:53:38 PM
 *  Author: Andrew Kaster
 *  
 *  @file Spi_bg_task.c
 *  @brief Manages all SPI Masters
 *
 * 
 */ 
#include <asf.h>
#include "Spi_bg_task.h"
#include "Spi_service.h"
#include "Background.h"

/** Array of pointers to each SPI master in the system. */
spi_master_t *gSpiMasters[MAX_SPI_MASTER_MODULES];

/**
 * @brief Adds an SPI Master object to the array 
 *
 * @param master The SPI master to add to the processing list
 * @return True on success, false on failure
 */
Bool spi_bg_add_master(spi_master_t *master)
{
    static uint8_t currMasterIdx = 0;
    Bool addMasterStatus = true;

    if(!(currMasterIdx < MAX_SPI_MASTER_MODULES))
    {
        addMasterStatus = false;
    }
    else
    {
        gSpiMasters[currMasterIdx] = master;
        currMasterIdx++;
    }
    return addMasterStatus;
}


/**
 * @brief Background task for servicing SPI Master queues
 *
 * This is a background task that loops over the array of SPI master modules and
 * attempts to initiate a request from the front of the queue on the idle ones. 
 * If there is no request in the queue for the currently indexed master, it
 * should do nothing, and simply move onto the next one. 
*/
void spi_bg_task(void)
{
    uint8_t idx = 0;
    spi_master_t *currMaster = gSpiMasters[0];

    for(idx = 0; idx < MAX_SPI_MASTER_MODULES; idx++, currMaster++)
    {
        if(currMaster != NULL)
        {
            if(!currMaster->masterBusy)
            {
                (void)spi_master_initate_request(currMaster);
            } /* End of check on busy */
        } /* End of NULL check */
    } /* End of loop over master modules */
}
