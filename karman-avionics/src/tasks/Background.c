/**
 * @file Background.c
 *
 * @brief Background Function API
 *
 *  Created: 11/5/2016 6:15:29 PM
 *  Author: Andrew Kaster 
 *  
 *  This is used to run various functions in the background.
 *  Check if a function is a background function
 *  Create a new background function with a function sent to it
 */ 


#include "Background.h"

/**
 * @brief Holds currently registered background functions
 *
 * Since this is an uninitialized array, it will go in the .bss segment and be
 * initialized to 0 by the C standard library at runtime
 */
static background_func_t backgroundFuncArry[MAX_BACKGROUND_FUNCS];

 /**
  * @brief Number of background functions that are currently registered
  *
  * Declared volatile to make sure for loop is not optimized away
  */
static volatile uint8_t numBackgroundFunc = 0;

/** 
 * @brief Runs all the critial tasks every loop of scheduler
 */
void background_task_func(void){
    /* Run everybody's background stuff here,
     * This includes polling, calculations, etc */
    static uint8_t funcArryIdx = 0;
    for(funcArryIdx = 0; funcArryIdx < numBackgroundFunc; funcArryIdx++)
    {
        if(backgroundFuncArry[funcArryIdx] == NULL)
        {
            /* Ignore NULL function pointers. */
            /* Based on current implementation, this should really be an error. */
            continue;
        }
        else
        {
            /* Call background functions that have been registered */
            backgroundFuncArry[funcArryIdx]();
        }
    }
}

/** 
 * @brief Utility to register a background function
 *
 * @param function funciton pointer to be registered
 * @returns a background_status_t enum to indicate success or failure
 *
 * The function will be added to the list of background functions.
 * Communicate with your background function via global variables,
 * i.e. a mailbox: A structure with a "doOperation" flag, a "operationDone" flag
 * and possibly some information for the background function to use.
 * background functions should be registered inside some init function.
 */
uint8_t add_background_function(background_func_t function){
    uint8_t retVal = BKGND_FUNC_SUCCESS;
    
    if(numBackgroundFunc < MAX_BACKGROUND_FUNCS)
    {
        backgroundFuncArry[numBackgroundFunc] = function;
        numBackgroundFunc++;
    }
    else
    {
        retVal = BKGND_FUNC_FAILURE;    
    }
    return retVal;
}

/**
 * @brief Checks if the given function is registered
 *
 * @param key The backround funciton to check
 * @returns true if found, false if not found
 *
 */
Bool is_background_function(background_func_t key)
{
    uint8_t idx;
    Bool found = false;
    for (idx = 0; idx < numBackgroundFunc; idx++)
    {
        if(backgroundFuncArry[idx] == key)
        {
            found = true;
			break;
        }
    }
    return found;
}
