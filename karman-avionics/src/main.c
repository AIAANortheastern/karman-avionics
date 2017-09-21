/**
 * @file main.c
 *
 * @brief Startup Function
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

/** @brief It's main. 
 *
 * Gets called after we have fun in .init0 through .init9
 * Intialize all the ASF boring stuff. Intialize the timer, and the
 * board pins and such. Intialize all tasks and drivers.
 * Then initialize the scheduler, never to return. Otherwise we've got problems.
 *
*/
int main(void) __attribute__((OS_main)) 
{
    pmic_init(); /** Enable the Programmable Multiple Interrupt Controller */
    sysclk_init(); /** Enable the system clock (32MHz). See conf_clock.h */

    /** Enable global interrupts and enable timer */
    cpu_irq_enable();
    timer_init(); /** Initialize timer. DO NO REMOVE*/
    tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc); /** start the timer. */

    board_init(); /** Do board initialization steps. */
                  /** Function defined in src/ASF/common/boards/user_board/init.c */

    /** Initalize, then start the task scheduler */
    init_scheduler();
    run_scheduler();

    while(1)
    {
        ; /* Should never get here, run_scheduler is noreturn */
    }
}
