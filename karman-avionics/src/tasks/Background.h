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

/** Size of backround function array */
#define MAX_BACKGROUND_FUNCS (20)

/** Custom return type for backround functions */
typedef enum {
    BKGND_FUNC_SUCCESS,
    BKGND_FUNC_FAILURE,     
} background_status_t;

/** Type defintion to keep all backround functions organized */
typedef void (*background_func_t)(void);

void background_task_func(void);

uint8_t add_background_function(background_func_t function);

Bool is_background_function(background_func_t key);

#endif /* BACKGROUND_H_ */
