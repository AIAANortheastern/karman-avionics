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

void run_scheduler(void) __attribute__((noreturn));

void init_scheduler(void);

#endif /* SCHEDULER_H_ */
