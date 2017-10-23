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

/** With default settings, system clock runs at 2MHz, and our timer interrupt occurs every
  * 1000 cycles (see Timer.c). So, Our timer interrupt occurs at 5KHz. This has a period of 1/5KHz = 200us
  * Background (continuous) tasks run every 200us/while no one else is running.
  * 10ms tasks run every 10ms, etc. Task priority/order is determined by ordering in the task list.
*/
typedef enum task_freq_e {
    TASK_FREQ_BACKGROUND = 0,   /**< Run every loop */
    TASK_FREQ_2ms = 10,         /**< Run every 2ms */
    TASK_FREQ_10ms = 50,        /**< Run every 10ms */
    TASK_FREQ_100ms = 500,      /**< Run every 100ms */
    TASK_FREQ_1s = 5000,        /**< Run every second */
} task_freq_enum_t;

/** 
Eight MS -- Timer is a 200us interrupt 
*/
#define EIGHT_MS (40) 

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

/**
    @brief Sleep for \a micros \a microseconds
    @param micros The number of microseconds to wait.
*/
void timer_delay_us(uint32_t micros);

#endif /* TIMER_H_ */
