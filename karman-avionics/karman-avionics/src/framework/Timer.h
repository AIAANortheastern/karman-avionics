/*
 * Timer.h
 *
 * Created: 11/5/2016 5:21:10 PM
 *  Author: Andrew Kaster
 */ 


#ifndef TIMER_H_
#define TIMER_H_
#include <compiler.h>
#include <asf.h>

void timer0_callback(void);

void timer_init(void);

uint32_t get_timer_count(void);

#endif /* TIMER_H_ */
