/**
 * @file Timer.c
 *
 * @brief Timer API
 *
 * Created: 11/5/2016 5:24:55 PM
 *  Author: Andrew Kaster
 */ 

#include "Timer.h"
#include <asf.h>


static volatile uint32_t timerCount = 0;

void timer0_callback(void){
    timerCount++;
}

void timer_init(void){
    tc_enable(&TCC0);
    tc_set_overflow_interrupt_callback(&TCC0, timer0_callback);
    tc_set_wgm(&TCC0, TC_WG_NORMAL);
    tc_write_period(&TCC0, 16000); /* Trigger interrupt when timer hits 16000 */
    tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
}

inline uint32_t get_timer_count(void){
    irqflags_t flags = cpu_irq_save();
    uint32_t timerVal = timerCount;
    cpu_irq_restore(flags);
    return timerVal;
}

inline void timer_delay_ms(uint8_t millis)
{
    uint32_t timer_begin = get_timer_count();
    uint32_t timer_end = timer_begin + 2*millis;
    while(get_timer_count() < timer_end)
    {
        asm("");
    }
}

