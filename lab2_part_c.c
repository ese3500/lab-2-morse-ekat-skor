//
// PART C
// Created by Ekaterina Skorniakova on 2/11/23.
//

/*--------------------Notes---------------------------*/

// dot =  30ms to 200ms
// dash = 200ms to 400ms
// space = button not pressed for > 400ms

// ticks = (clock frequency) * (time in seconds)
// 30ms: 16 x 10^6 * (30 x 10^-3) = 480,000 ticks (dot)
// 200ms: 16 x 10^6 * (200 x 10^-3) = 3,200,000 ticks (dash)
// 400ms: 16 x 10^6 * (400 x 10^-3) = 6,400,0000 ticks (space)

// 8 bit timer counts 256 ticks
// 16 bit timer counts 65536 ticks

// TCNT1 register will overflow approximately every 4ms.
// timing for events longer than 4ms => timer overflow interrupt (enabled in the TIMSK1 register) or prescaler in TCCR1B


// have the microcontroller print out:
// 1) timer variables
// 2) transition times on each press and release of the button


/*--------------------Definitions---------------------------*/


#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)


/*--------------------Libraries---------------------------*/

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
//#include <stdio.h>
#include "uart.h"

/*--------------------Variables---------------------------*/
char String[25];
volatile int rise = 0;
volatile int fall = 0;
volatile int pulse = 0;
volatile int prevTime = 0;
volatile int currTime = 0;
volatile long thirty_ms = 480000; // 30ms = 480,000 ticks
volatile long two_h_ms = 3200000; // 200ms = 3,200,000 ticks
volatile long four_h_ms = 6400000; // 400ms = 6,400,000 ticks
bool space = false;

/*--------------------Serial Monitor for CLion---------------------------*/
// terminal serial monitor access:  pio device monitor
/*-----------------------------------------------------------*/


void Initialize()
{
    cli(); // disable global interrupts

    // set input capture pin
    DDRB &= ~(1 << DDB0); // set PB0 = ICP1  (input capture pin) : [D8]
    DDRB |= (1<<DDB5);// set PB5 as output pin (1) : [onboard LED]
    PORTB &= ~(1<<PORTB5); // disable pull up resistor on B5 for LED

    // internal clock / 1024
    // 1 tick =1/2M second

    // TCCR1B : wave generator
    TCCR1B |= (1<<CS10);
    TCCR1B &= ~(1<<CS11);
    TCCR1B |= (1<<CS12);

    // activate timer 1 to normal mode of operation (0)
    TCCR1A &= ~(1 << WGM10);
    TCCR1A &= ~(1 << WGM11);
    TCCR1A &= ~(1 << WGM12);
    TCCR1A &= ~(1 << WGM13);

    TCCR1B |= (1<<ICES1);// looking for rising edge
    TIMSK1 |= (1<<ICIE1);// enable input capture interrupt
    TIFR1 |= (1<<ICF1);// clear interrupt flag for input capture

    sei(); // re-enable global interrupts

}

// Interrupt Service Routine
ISR(TIMER1_CAPT_vect) {

    // LED button
    PORTB ^=(1<<PORTB5);
    TCCR1B ^= (1<<ICES1);// looking for rising/falling edge

    // if rising edge detected (button pressed)
    if (PINB & (1<<PORTB5))
    {
        rise = ICR1; //rise time
    }
    else
    {
        fall = ICR1; //fall time
        pulse = fall - rise;

        // dot
        if (pulse >= (thirty_ms/1024) && pulse < (two_h_ms/1024))
        {
            sprintf(String, ".");
            UART_putstring(String);
            _delay_ms(1000);
            prevTime = ICR1; // store time of last-performed action (prev)
            space = false;
        }
        // dash
        else if (pulse >= (two_h_ms/1024) && pulse < (four_h_ms/1024))
        {
            sprintf(String, "_");
            UART_putstring(String);
            _delay_ms(1000);
            prevTime =ICR1; // save time for when the last action took place
            space = false; // allowed to print space again
            }
            // space
            else
            {
                sprintf(String, "_");
                UART_putstring(String);
                _delay_ms(1000);
            }

    }
}


int main(void)
{
    Initialize();
    UART_init(BAUD_PRESCALER);

    while(1) {

        currTime = ICR1;
        if ((currTime - prevTime) >= (four_h_ms/1024))
        {
            if (space == false)
            {
                sprintf(String, " ");
                UART_putstring(String);
                space = true;
            }
            else;
        }
        else;

    }
}
