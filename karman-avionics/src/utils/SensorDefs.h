/**
 * @file SensorDefs.h
 *
 * @brief Put all common sensor declarations here
 *
 * Should hold all data structures that depends on every sensor,
 * or on multiple sensors. 
 *
 * Created: 2/15/2017 8:14:23 PM
 *  Author: Andrew
 */ 


#ifndef SENSORDEFS_H_
#define SENSORDEFS_H_

#include "ms5607-02ba03.h"

/** contains data for every sensor */
typedef struct
{
    ms5607_02ba03_data_t altimeter; /**< Temp and pressure */
    /* TODO add all sensors' data */
} sensor_data_t;



#endif /* SENSORDEFS_H_ */