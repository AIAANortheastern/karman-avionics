/**
 * @file Spi_bg_task.h
 *
 * @brief SPI Backround API
 *
 * Created: 11/19/2016 1:45:09 PM
 *  Author: Andrew
 */ 


#ifndef SPI_BG_TASK_H_
#define SPI_BG_TASK_H_

#include "Spi_service.h"

/** How many SPI interfaces we are using */
#define MAX_SPI_MASTER_MODULES (3)

/** Array of pointers to each SPI master in the system. */
extern spi_master_t *gSpiMasters[MAX_SPI_MASTER_MODULES];

void spi_bg_task(void);

Bool spi_bg_add_master(spi_master_t *master);

#endif /* SPI_BG_TASK_H_ */
