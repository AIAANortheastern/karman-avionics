/*
 * LEDTask.c
 *
 * Created: 9/9/2017 5:38:26 PM
 *  Author: Andrew
 */ 

#include "LEDTask.h"
#include <asf.h>

#define LEDPORT (PYRO_34_PORT)
#define LEDPIN  (PYRO_SENSE4A)

void led_task_func(void)
{
    LEDPORT.OUTTGL = LEDPIN;
}
