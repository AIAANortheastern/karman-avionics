/*
 * Spi_bg_task.c
 *
 * Created: 11/19/2016 1:53:38 PM
 *  Author: Andrew Kaster
 */ 
#include <asf.h>
#include "Spi_bg_task.h"
#include "Spi_service.h"
#include "Background.h"

spi_master_t *gSpiMasters[MAX_SPI_MASTER_MODULES];

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


/* This is a background task that loops over the array of SPI master modules and */
/* attempts to initiate a request on the idle ones. If there is no request in the queue */
/* for the currently indexed master, it should do nothing, and simply move onto the next one. */
void spi_bg_task(void)
{
    static uint8_t idx = 0;
    static spi_master_t *currMaster = NULL;

    for(idx = 0; idx < MAX_SPI_MASTER_MODULES; idx++)
    {
        currMaster = gSpiMasters[idx];

        if(currMaster != NULL)
        {
            if(!currMaster->masterBusy)
            {
                (void)spi_master_initate_request(currMaster);
            } /* End of check on busy */
        } /* End of NULL check */
    } /* End of loop over master modules */
}
