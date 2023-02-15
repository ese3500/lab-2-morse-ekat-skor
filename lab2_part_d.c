//
// Created by Ekaterina Skorniakova on 2/13/23.
//
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
#include "string.h"
#include "uart.h"

/*--------------------Variables---------------------------*/
char String[25];
volatile int rise = 0;
volatile int fall = 0;
volatile int pulse = 0;
volatile int prevTime = 0;
volatile int currTime = 0;
bool space = false;
char entry[6]="";
int i;
char dot[] = ".";
char dash[] = "-";

// arrays of pointers
static const char *morse[]= {
        ".-",   //A
        "-...", //B
        "-.-.", //C
        "-..",  //D
        ".",    //E
        "..-.", //F
        "--.",  //G
        "....", //H
        "..",   //I
        ".---", //J
        "-.-",  //K
        ".-..", //L
        "--",   //M
        "-.",   //N
        "---",  //O
        ".--.", //P
        "--.-", //Q
        ".-.",  //R
        "...",  //S
        "-",    //T
        "..-",  //U
        "...-", //V
        ".--",  //W
        "-..-", //X
        "-.--", //Y
        "--..", //Z
        "-----", //0
        ".----", //1
        "..---", //2
        "...--", //3
        "....-", //4
        ".....", //5
        "-....", //6
        "--...", //7
        "---..", //8
        "----." //9
};


static const char *ascii[] = {
        "A",
        "B",
        "C",
        "D",
        "E",
        "F",
        "G",
        "H",
        "I",
        "J",
        "K",
        "L",
        "M",
        "N",
        "O",
        "P",
        "Q",
        "R",
        "S",
        "T",
        "U",
        "V",
        "W",
        "X",
        "Y",
        "Z",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9"
};

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
    DDRD |=(1<<DDD6); // dot led (yellow)
    DDRD |=(1<<DDD7); // dash led (blue)



    // internal clock / 1024
    // 1 tick =1/2M second

    // TCCR1B : wave generators lie here
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
    PORTD &=~(1<<PORTD6);
    PORTD &=~(1<<PORTD7);
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
        //480000/1024   -> 400000/1024     : 3200000/1024 -> 8000000/1024
        if (pulse >= (400000/1024) && pulse < (8000000/1024))
        {
            // yellow LED = dot
            PORTD ^=(1<<PORTD6);
            strcat(entry,dot);
            _delay_ms(1000);
            PORTD &=~(1<<PORTD6);

            prevTime = ICR1; // store time of last-performed action (prev)
            space = false;
        }

        // dash 6400000/1024 -> 24000000/1024
        else if (pulse >= (8000000/1024) && pulse < (24000000/1024))
        {
            // blue LED = dash
            PORTD ^=(1<<PORTD7);
            strcat(entry,dash);
            _delay_ms(1000);
            PORTD &=~(1<<PORTD7);


            prevTime =ICR1; // save time for when the last action took place
            space = false; // allowed to print space again
        }

        // space
        else
        {
//            sprintf(String, " ");
//            UART_putstring(String);
//            _delay_ms(1000);
//            space = true;
        }

    }
}



int translate()
{

        sprintf(String, "%s ", entry, strlen(entry));
        UART_putstring(String);

        i=0;
        for (i=0; i < sizeof(morse); i++)
        {
            if (strcmp(morse[i],entry)==0)
            {
                sprintf(String, "%s", ascii[i]);
                UART_putstring(String);
                memset(entry, 0, strlen(entry));
                i += 100;

                sprintf(String, "K   A   T ");
                UART_putstring(String);
                _delay_ms(1000);
            }
            else;
        }
        space = false;

}


int main(void)
{
    Initialize();
    UART_init(BAUD_PRESCALER);

    while(1) {

        currTime = ICR1;
        if ((currTime - prevTime) >= (24000000/1024))
        {
            if (space == false)
            {
                translate();
                memset(entry, 0, strlen(entry)); //clear entry

                sprintf(String, "  ");
                UART_putstring(String);

                space = true;
            }
            else;
        }
        else;

    }
}
