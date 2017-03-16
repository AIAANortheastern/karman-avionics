/*
 * main.c
 *
 * Created: 11/5/2016 9:49:00 PM
 *  Author: Andrew Kaster
 */ 

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "Scheduler.h"
#include "Timer.h"
#include "tc_fixes.h"

__attribute__((OS_main)) int main(void)
{
    /* Insert system clock initialization code here (sysclk_init()). */
    pmic_init(); /* Enable the Programmable Multiple Interrupt Controller */
    sysclk_init(); /* Enable the system clock (32MHz). See conf_clock.h */

    /* Enable global interrupts and enable timer */
    cpu_irq_enable();
    tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
    timer_init(); /* Initialize timer. DO NO REMOVE*/

    /* TODO x a million. Need to put code in this function*/
    board_init(); /* Do board initialization steps. */
                  /* Function defined in src/ASF/common/boards/user_board/init.c */

    /* Insert application code here, after the board has been initialized. */
    init_scheduler();
    /* Start the task scheduler */
    run_scheduler();

    while(1)
    {
        ; /* Should never get here, run_scheduler is noreturn */
    }
}
