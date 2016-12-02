/*
 * Tasks.h
 *
 * Created: 11/5/2016 4:50:45 PM
 *  Author: Andrew Kaster
 */ 


#ifndef TASKS_H_
#define TASKS_H_
#include <compiler.h>

/* With default settings, system clock runs at 2MHz, and our timer interrupt occurs every */
/* 1000 cycles (see Timer.c). So, Our timer interrupt occurs every 2KHz. This has a period of 1/2KHz = 500us*/
/* Background (continuous) tasks run every 500us/while no one else is running. */
/* 10ms tasks run every 10ms, etc. Task priority/order is determined by ordering in the task list.*/
typedef enum task_freq_e {
    TASK_FREQ_BACKGROUND = 0,   /* Run every loop */
    TASK_FREQ_1500us = 3,       /* Run every 1.5ms */
    TASK_FREQ_10ms = 20,        /* Run every 10ms */
    TASK_FREQ_100ms = 200,      /* Run every 100ms */
    TASK_FREQ_1s = 2000,        /* Run every second */
} task_freq_enum_t;

/* Structure to define tasks.*/
typedef struct simple_task_s{
    task_freq_enum_t taskFreq; /* How often should this task be run? */
    uint32_t lastCount; /* The last timer count we ran at */
    void(*task)(void); /* Pointer to task function */
} simple_task_t;

/* Inline function that returns a pointer to the task list */
simple_task_t *get_task_list(void);

/* Inline function that returns the number of tasks in the list */
uint8_t get_num_tasks(void);

#endif /* TASKS_H_ */