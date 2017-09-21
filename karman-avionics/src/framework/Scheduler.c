/**
 * @file Scheduler.c
 *
 * @brief Cooperative Scheduler
 *
 * Created: 11/5/2016 4:42:56 PM
 *  Author: Andrew Kaster
 *  
 *  This is the master of the program that kicks 
 *  everything off. This is called by the main function
 *  and this just loops through tasks such that each task
 *  does its duty or at least part of it and waits for 
 *  the next call.
 */ 
#include "Scheduler.h"
#include "Tasks.h"
#include "Timer.h"

/** Pointer to the task array from Task.c */
static simple_task_t *taskArry;
/** The number of tasks in the task array from Task.c */
static uint8_t numTasks;

/**
    @brief Intialize the scheduler.

    Sets up the scheduler's global data.
*/
void init_scheduler(void){
    taskArry = get_task_list();
    numTasks = get_num_tasks();
}

/**  
    @brief Main Scheduler function.

    Runs in an infinite loop, calling each task at its requested interval,
    When possible. When there is time, if not possible.
*/
void run_scheduler(void){
    
    static uint8_t i = 0;
    static volatile uint32_t timeCount = 0;
    static volatile uint32_t prevTimeCount = 0;
    /** Loop infinitely, comparing every task's last time to the current time */
    for (;;){
        /** Wait for 500us interrupt */
        do {
            timeCount = get_timer_count();
        } while (timeCount == prevTimeCount);
        
        prevTimeCount = timeCount;

        for(i = 0; i < numTasks; i++)
        {
            /** First, check if the task is not a background task */
            /** Run the background task unconditionally, otherwise 
              * see if it's time to run the task or not 
            */
            if((taskArry[i].taskFreq == TASK_FREQ_BACKGROUND) ||
               ((timeCount - taskArry[i].lastCount) > taskArry[i].taskFreq))
            {
                taskArry[i].task();
                taskArry[i].lastCount = timeCount;
            }
        } /* End loop over tasks */
    } /* End infinite loop */
} /* End function run_scheduler */
