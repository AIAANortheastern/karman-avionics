/**
 * @file Tasks.h
 *
 * @brief Task Defintions
 *
 * Created: 11/5/2016 4:50:45 PM
 *  Author: Andrew Kaster
 */ 


#ifndef TASKS_H_
#define TASKS_H_
#include <compiler.h>

#include "Timer.h"

/** Structure to define a task. */
typedef struct simple_task_s {
    task_freq_enum_t taskFreq; /**< How often should this task be run? */
    uint32_t lastCount;        /**< The last timer count we ran at */
    void(*task)(void);         /**< Pointer to task function */
} simple_task_t;

/** Inline function that returns a pointer to the task list */
simple_task_t *get_task_list(void);

/** Inline function that returns the number of tasks in the list */
uint8_t get_num_tasks(void);

#endif /* TASKS_H_ */
