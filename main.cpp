#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PIN_T1      5 //counter1 pin
#define PIN_IDLE    0
#define PIN_DATA    1
#define PIN_CLK     2
#define PIN_LOAD    3

inline void bar_line(uint8_t v)
    {
    PORTC = (v>6)? 0xFF : (0xFF << (6 - v));
    PORTB = 0xFF << (12 - v);
    }

inline void bar_red()
    {
    PORTC = 0;
    PORTB = _BV( 2 ) | _BV( 3 ) | _BV( 4 );
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

    if ( 0 == TCNT1 ) //special case
        {
        bar_red();
        return;
        }

    uint16_t clock_val = TCNT1>>3;
    TCNT1 = 0;
    if ( clock_val > clock_max )
        {
        clock_max = next_pow2( clock_val )-1;
        clock_10percent = static_cast<float>( clock_max )/10.0;
        }
    bar_line( round(static_cast<float>(clock_max-clock_val)/clock_10percent) );
    }

int main()
    {
    set_sleep_mode( SLEEP_MODE_IDLE );
    sleep_enable();

    DDRD = 0;
    DDRB = 0xFF;
    DDRC = 0xFF;
    PORTD = _BV( PIN_T1 );
    for (int i = 0; i < 11; ++i)
        {
        bar_line( i );
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
        sleep_cpu();

    return 0;
    }
