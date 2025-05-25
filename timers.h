#ifndef TIMERS_H_
#define TIMERS_H_

#include <avr/io.h>


/**
 * Initialize Timer 1 (16-bit) in CTC mode.
 */
void Timer1_init_ctc();

void enable_PB1_pwm();
void disable_PB1_pwm();
void set_PB1_freq(uint16_t freq);

void Timer2_init_systicks();

/** Declares the system ticks counter (milliseconds since boot) for use within
 * other .c modules! */
extern volatile unsigned int systicks;

/** Useful macro to check whether a specific amount of time passed since some
 * given last event */
#define SYSTICKS_PASSED(last_event, diff_amount) \
	((systicks - (last_event)) >= (diff_amount))

#endif // TIMERS_H_

