/**
 * @file Background.h
 *
 * @brief Background Function API
 *
 *  Created: 11/5/2016 6:16:17 PM
 *  Author: Andrew Kaster
 */ 


#ifndef BACKGROUND_H_
#define BACKGROUND_H_
#include <compiler.h>
#define MAX_BACKGROUND_FUNCS (20)

typedef enum {
    BKGND_FUNC_SUCCESS,
    BKGND_FUNC_FAILURE,     
} background_status_t;

typedef void (*background_func_t)(void);

void background_task_func(void);

/* Utility to register a background function */
/* Communicate with your background function via global variables, */
/* i.e. a mailbox: A structure with a "doOperation" flag, a "operationDone" flag */
/* and possibly some information for the background function to use. */
/* background functions should be registered inside some init function*/
uint8_t add_background_function(background_func_t function);

Bool is_background_function(background_func_t key);

#endif /* BACKGROUND_H_ */
