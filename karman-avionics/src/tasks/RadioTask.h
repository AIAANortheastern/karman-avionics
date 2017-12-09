/**
 * @file RadioTask.h
 *
 * @brief Radio Task Function and Intialization
 *
 * Created: 12/4/2016 10:19:49 PM
 *  Author: Andrew Kaster
 */ 


#ifndef RADIOTASK_H_
#define RADIOTASK_H_

#include "Spi_service.h"
#include "SensorDefs.h"

typedef struct  
{
	uint8_t header;
	sensor_data_t sensorData;
	uint8_t checksum;
}xbee_tx_data_t;

void radio_task_func(void);

void init_radio_task(void);



#endif /* RADIOTASK_H_ */
