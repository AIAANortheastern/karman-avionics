/*
 * LEDTask.c
 *
 * Created: 9/9/2017 5:38:26 PM
 *  Author: Andrew
 */ 

#include "LEDTask.h"
#include <asf.h>

#define LEDPORT (PYRO_34_PORT)
#define LEDPIN  (PYRO_4A)

void led_task_func(void)
{
    static Bool led_status = false;

    if(led_status)
    {
        LEDPORT.OUTCLR = LEDPIN;
        led_status = false;
    }
    else
    {
        LEDPORT.OUTSET = LEDPIN;
        led_status = true;
    }
}
