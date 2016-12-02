/*
 * Background.c
 *
 * Created: 11/5/2016 6:15:29 PM
 *  Author: Andrew Kaster 
 */ 


#include "Background.h"

/* Since this is an uninitialized array, it will go in the .bss segment and be
 * initialized to 0 by the C standard library at runtime.*/
static background_func_t backgroundFuncArry[MAX_BACKGROUND_FUNCS];
/* Holds how many background functions are currently registered.
 * Declared volatile to make sure for loop is not optimized away. */
static volatile uint8_t numBackgroundFunc = 0;

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

/* Use this function to register a background function. The function will be added to the list
 * of background functions.
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

Bool is_background_function(background_func_t key)
{
    uint8_t idx;
    Bool found = false;
    for (idx = 0; idx < numBackgroundFunc; idx++)
    {
        if(backgroundFuncArry[idx] == key)
        {
            found = true;
        }
    }
    return found;
}
