/*
 * LEDTask.c
 *
 * Created: 9/9/2017 5:38:26 PM
 *  Author: Andrew
 */ 

#include "LEDTask.h"
#include <asf.h>

#define LEDPORT (PYRO_12_PORT)
#define LEDPIN  (PYRO_1A)

void led_task_func(void)
{
    static Bool led_status = false;

    if(led_status)
    {
        LEDPORT.OUTSET = LEDPIN;
    }
    else
    {
        LEDPORT.OUTCLR = LEDPIN;
    }
}
