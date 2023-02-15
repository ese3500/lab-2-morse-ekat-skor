//
// PART B
// Created by Ekaterina Skorniakova on 2/10/23.
//


//Bit 6 – ICES1: Input Capture Edge Select

// rising edge: 1
// falling edge: 0

// enable TCCR1B
// TIMSK = 1


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL

// button press = HIGH (on)
// button release = LOW (off)

void Initialize()
{

    cli(); // disable global interrupts

    DDRB &= ~(1 << DDB0); // set PB0 = ICP1  (input capture pin) : [D8]
    DDRB |= (1 << DDB5); // set PB5 as output pin (1) : [onboard LED]
    PORTB |= (1 << PORTB5); // enable pull up resistor on B5 for LED


    TCCR1B &= ~(1 << ICES1); //falling edge triggers the capture (0) (pressing button down)
    TIMSK1 |= (1 << ICIE1); // enable input capture (TIMSK sets which interrupt to enable)
    TIFR1 |= (1 << ICF1); // clear interrupt flag by writing 1

    sei(); // re-enable global interrupts

}


ISR(TIMER1_CAPT_vect) {
    PORTB ^= (1 << PORTB5); // toggle LED : if off then on and vice versa
    TCCR1B ^= (1 << ICES1); // toggle trigger capture: 0->1 (un-trigger capture)
    TIFR1 |= (1 << ICF1); // clear interrupt flag again (1)
}

int main(void)
{
    Initialize();
    while(1) ;
}
