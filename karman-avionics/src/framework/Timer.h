/**
 * @file Timer.h
 *
 * @brief Timer API
 *
 * Created: 11/5/2016 5:21:10 PM
 *  Author: Andrew Kaster
 */ 


#ifndef TIMER_H_
#define TIMER_H_
#include <compiler.h>
#include <asf.h>

/** 
Eight MS -- Timer is a 500us interrupt 
*/
#define EIGHT_MS (16) 

/**
    @brief Callback for the TCC0 interrupt

    Increments #timerCount every time TCC0 overflows
*/
void timer0_callback(void);

/**
    @brief Intialize timer interrupt

    Enables the TCC0 module and sets the overflow frequency
    to 2KHz. Probably a bit on the fast side...
*/
void timer_init(void);

/**
    @brief Provides interrupt-safe access to the current time count.
    @return The current time count
*/
uint32_t get_timer_count(void);

/**
    @brief Sleep for \a millis \a milliseconds
    @param millis The number of milliseconds to wait.
*/
void timer_delay_ms(uint8_t millis);

#endif /* TIMER_H_ */
