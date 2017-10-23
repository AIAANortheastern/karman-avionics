/**
 * @file Timer.c
 *
 * @brief Timer API
 *
 *  Created: 11/5/2016 5:24:55 PM
 *  Author: Andrew Kaster
 */ 

#include "Timer.h"
#include <asf.h>

/**
    @brief The current timer count as set by the TCC0 interrupt. 

    Made static here so that it will only be accessed
    in an interrupt-safe way. 
*/
static volatile uint32_t timerCount = 0;

/**
    @brief Callback for the TCC0 interrupt

    Increments ::timerCount every time TCC0 overflows
*/
void timer0_callback(void){
    timerCount++;
}

/**
    @brief Callback for the TCC0 interrupt

    Increments #timerCount every time TCC0 overflows
*/
void timer_init(void){
    /** Enable Timer Counter Compare 0, register timer0_callback with the ASF
     *  interrupt, set the timer to standard mode, and the overflow period to
     *  16000 counts. At 32MHz, that gives us a period of 500us. 
     *  Finally, set the interrupt level to Low.
    */
    tc_enable(&TCC0);
    tc_set_overflow_interrupt_callback(&TCC0, timer0_callback);
    tc_set_wgm(&TCC0, TC_WG_NORMAL);
    tc_write_period(&TCC0, 16000); /* Trigger interrupt when timer hits 16000 */
    tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
}

/**
    @brief Provides interrupt-safe access to the current time count.
    @return The current time count
*/
inline uint32_t get_timer_count(void){
    /** Save the current IRQ status and disable interrupts. Then, save our timer
     * variable locally, and re-enable interrupts before returning the current time.
    */
    irqflags_t flags = cpu_irq_save();
    uint32_t timerVal = timerCount;
    cpu_irq_restore(flags);
    return timerVal;
}

/**
    @brief Sleep for \a millis \a milliseconds
    @param millis The number of milliseconds to wait.
*/
void timer_delay_ms(uint8_t millis)
{
    /** Get the start time and calcualte the duration in counts.
     *  Then, twiddle thumbs until \a millis \a milliseconds have
     *  expired.
     */
    uint32_t timer_begin = get_timer_count();
    uint32_t wait_len = 2*millis;
    while((get_timer_count() - timer_begin) < wait_len)
    {
        /* Do nothing */
    }
}

