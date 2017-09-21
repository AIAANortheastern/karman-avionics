/**
 * @file Tasks.c
 *
 * @brief Task List Defintion
 *
 *  Created: 11/5/2016 4:42:56 PM
 *  Author: Andrew Kaster
 *  
 *  This initializes the tasts that the program
 *  will loop through from scheduler.c
 *  Each task has a specific job or "task" if you will
 *  that it will do as the scheduler loops through this list.
 */ 

#include "Tasks.h"
#include "Background.h"
#include "Pyrotechnics.h"
#include "SensorTask.h"
#include "RadioTask.h"
#include "USBTask.h"

#define INITIAL_COUNT (0)

/** List of tasks to be run. Tasks must have defined a Frequency, last_count 
 * field, and a task function. */
simple_task_t TaskList[] =
{
    /** Task to maintain connection with USB host */
    {
        .taskFreq = TASK_FREQ_1500us,
        .lastCount = INITIAL_COUNT,
        .task = USB_task_func,
    },
    /** Task to check status of global sensor data to see if it's time to
    * perform Pyrotechnics activities */
    { 
        .taskFreq = TASK_FREQ_10ms,
        .lastCount = INITIAL_COUNT,
        .task = check_pyro_task_func,
    },
    /** Radio task to manage reciept and transfer of messages to and from the RF modules */
    {
        .taskFreq = TASK_FREQ_10ms,
        .lastCount = INITIAL_COUNT,
        .task = radio_task_func,
    },
    /** Sensor task to keep track of timings for all sensors and when they need called */
    {
        .taskFreq = TASK_FREQ_1500us,
        .lastCount = INITIAL_COUNT,
        .task = sensor_task_func,
    },
     /** ALWAYS Keep background task last! */
    { 
        .taskFreq = TASK_FREQ_BACKGROUND,
        .lastCount = INITIAL_COUNT,
        .task =  background_task_func,
    },
};

/** Inline function that returns a pointer to the task list */
inline simple_task_t *get_task_list(void){
    return TaskList;
}

/** Inline function that returns the number of tasks in the list */
inline uint8_t get_num_tasks(void){
    return sizeof(TaskList)/sizeof(simple_task_t);
}
