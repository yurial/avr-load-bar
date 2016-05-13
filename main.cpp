#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PIN_IDLE    0
#define PIN_DATA    1
#define PIN_CLK     2
#define PIN_LOAD    3

void bar(uint8_t v)
    {
    PORTC = (v>6)? 0xFF : (0xFF << (6 - v));
    PORTB = 0xFF << (12 - v);
    }

uint16_t next_pow2(uint16_t x)
    {
    unsigned y = 1;
    if (!x) return 0;

    /* Keep doubling y until we are done */
    while (y <= x) y += y;
    
    return y;
    }

ISR(TIMER0_OVF_vect,ISR_BLOCK)
    {
    static uint16_t clock_max = 1;
    static float clock_10percent = 1.0;
    uint16_t clock_val = TCNT1>>3;
    TCNT1 = 0;
    if ( clock_val > clock_max )
        {
        clock_max = next_pow2( clock_val )-1;
        clock_10percent = static_cast<float>( clock_max )/10.0;
        }
    bar( round(static_cast<float>(clock_max-clock_val)/clock_10percent) );
    }

int main()
    {
    DDRB = 0xFF;
    DDRC = 0xFF;
    for (int i = 0; i < 11; ++i)
        {
        bar( i );
        _delay_ms( 100 );
        }
    //TIMER1
    TCCR1B = _BV(CS12) | _BV(CS11); //External clock source on T1 pin. Clock on falling edge
    //TIMER0
    TCCR0 = _BV(CS02) | _BV(CS00); //prescaller = 1024
    TCNT0 = 0;
    TCNT1 = 0;
    TIMSK |= _BV(TOIE0); //overflow interrupt
    sei();
    for (;;)
        {
        }

    return 0;
    }
