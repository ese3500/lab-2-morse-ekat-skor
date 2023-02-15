//
// PART A
// Created by Ekaterina Skorniakova on 2/10/23.
//

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000UL

void Initialize()
{
    DDRB |= (1<<DDB1);
    DDRB |= (1<<DDB2);
    DDRB |= (1<<DDB3);
    DDRB |= (1<<DDB4);
    DDRC |= (1<<DDD7);
}

int main(void)
{
    Initialize();
    /* Replace with your application code */
    int count = 0;
    while (1) {
            if ((PIND & (1<<PIND7))) {
                if (count == 0){
                    PORTB |= (1 << PORTB1);
                    PORTB &= ~(1 << PORTB2);
                    PORTB &= ~(1 << PORTB3);
                    PORTB &= ~(1 << PORTB4);
                    count +=1;
                    _delay_ms(500);
                }
                else if (count == 1) {
                    PORTB &= ~(1 << PORTB1);
                    PORTB |= (1 << PORTB2);
                    PORTB &= ~(1 << PORTB3);
                    PORTB &= ~(1 << PORTB4);
                    count +=1;
                    _delay_ms(500);
                }
                else if (count == 2) {
                    PORTB &= ~(1 << PORTB1);
                    PORTB &= ~(1 << PORTB2);
                    PORTB |= (1 << PORTB3);
                    PORTB &= ~(1 << PORTB4);
                    count +=1;
                    _delay_ms(500);
                }
                else if (count == 3) {
                    PORTB &= ~(1 << PORTB1);
                    PORTB &= ~(1 << PORTB2);
                    PORTB &= ~(1 << PORTB3);
                    PORTB |= (1 << PORTB4);
                    count = 0;
                    _delay_ms(500);
                }

            }


    }



}
