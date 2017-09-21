/**
 * @file Scheduler.h
 *
 * @brief Cooperative Scheduler
 *
 * Created: 11/5/2016 4:42:56 PM
 *  Author: Andrew Kaster
 */ 

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <compiler.h>

/**  
    @brief Main Scheduler function.

    Runs in an infinite loop, calling each task at its requested interval,
    When possible. When there is time, if not possible.
*/
void run_scheduler(void) __attribute__((noreturn));

/**
    @brief Intialize the scheduler.

    Sets up the scheduler's global data.
*/
void init_scheduler(void);

#endif /* SCHEDULER_H_ */
