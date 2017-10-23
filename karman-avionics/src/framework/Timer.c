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

/** Number of scheduler ticks per millisecond */
#define TICKS_PER_MS (5)
/** Number of scheduler microseconds per tick */
#define US_PER_TICK (200)

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
     *  6400 counts. At 32MHz, that gives us a period of 200us. 
     *  Finally, set the interrupt level to Low.
    */
    tc_enable(&TCC0);
    tc_set_overflow_interrupt_callback(&TCC0, timer0_callback);
    tc_set_wgm(&TCC0, TC_WG_NORMAL);
    tc_write_period(&TCC0, 6400); /* Trigger interrupt when timer hits 6400, 200us */
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
    /** Get the start time and calculate the duration in counts.
     *  Then, twiddle thumbs until \a millis \a milliseconds have
     *  expired.
     */
    uint32_t timer_begin = get_timer_count();
    uint32_t wait_len = TICKS_PER_MS*millis;
    while((get_timer_count() - timer_begin) < wait_len)
    {
        /* Do nothing */
    }
}

/**
    @brief Sleep for \a micros \a microseconds
    @param micros The number of microseconds to wait.
*/
void timer_delay_us(uint32_t micros)
{
    /** Get the start time and calculate the duration in counts.
     *  Then, twiddle thumbs until \a millis \a milliseconds have
     *  expired.
     */
    uint32_t timer_begin = get_timer_count();
    uint32_t wait_len = micros/US_PER_TICK;
    while((get_timer_count() - timer_begin) < wait_len)
    {
        /* Do nothing */
    }
}

