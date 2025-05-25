#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "timers.h"

/** Allocates the system ticks counter (milliseconds since boot) */
volatile unsigned int systicks = 0;

void enable_PB1_pwm() {
    DDRB |= (1 << PB1);
    TCCR1A |= (1 << COM1A0);
}

void disable_PB1_pwm() {
    TCCR1A &= ~(1 << COM1A0);
}

void Timer1_init_ctc()
{
    TCCR1A = 0;
    TCCR1B = 0;

    disable_PB1_pwm();
    TCCR1B |= (1 << WGM12); // enable CTC

    /* Use 64 prescaler: CS1[2:0] = 0b011 */
    TCCR1B |= (1 << CS10) | (1 << CS11);
}

void set_PB1_freq(uint16_t freq) {
    float result = 125000 / freq; // 16MHz / 2 / 64 / freq
    OCR1A = (uint16_t)result;
}

void Timer2_init_systicks() {
    TCCR2A = (1 << WGM21); /* WGM => CTC mode */
    TCCR2B = (1 << CS22); /* Prescaler => 64 */
    OCR2A  = 250; /* 16MHz / 64 => 250 kHz */
    TIMSK2 = (1 << OCIE2A); /* interrupt when counter reaches OCR2A == 250 */
}

ISR(TIMER2_COMPA_vect)
{
    /* Will get called once every 1ms! */
    systicks++;
    /* Note: the timer is in Clear Timer on Compare Match mode, so it will
     * automatically reset itself back to 0! */
}
