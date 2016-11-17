/*
 * Timer.c
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
    tc_write_period(&TCC0, 1000); /* Trigger interrupt when timer hits 1000 */
    tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
}

inline uint32_t get_timer_count(void){
    return timerCount;
}