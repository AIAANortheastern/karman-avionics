/**
 * @file SensorTask.h
 *
 * @brief Sensor Task Function and Intialization
 *
 * Created: 12/1/2016 7:19:55 PM
 *  Author: Andrew Kaster
 */ 


#ifndef SENSORTASK_H_
#define SENSORTASK_H_

#include "Spi_service.h"

/** Return for all sensor state machines */
typedef enum
{
    SENSOR_WAITING,     /**< In a sleep state */
    SENSOR_BUSY,        /**< Transaction in progress */
    SENSOR_COMPLETE,    /**< New data available */
} sensor_status_t;

void sensor_task_func(void);

void init_sensor_task(void);



#endif /* SENSORTASK_H_ */
