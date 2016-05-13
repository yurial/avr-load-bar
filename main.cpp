#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PIN_IDLE    0
#define PIN_DATA    1
#define PIN_CLK     2
#define PIN_LOAD    3

class max7219
    {
    public:
    enum cmd
        {
        noop        = 0x00,
        digit0      = 0x01,
        digit1      = 0x02,
        digit2      = 0x03,
        digit3      = 0x04,
        digit4      = 0x05,
        digit5      = 0x06,
        digit6      = 0x07,
        digit7      = 0x08,
        decodeMode  = 0x09,
        intensity   = 0x0a,
        scanLimit   = 0x0b,
        shutdown    = 0x0c,
        displayTest = 0x0f
        };
    };

void shiftOut(uint8_t data)
    {
    for (uint8_t i = 0; i < 8; i++)
        {
        if ( data & 0x80 )
            {
            PORTB = _BV( PIN_DATA );
            PORTB = _BV( PIN_DATA ) | _BV( PIN_CLK );
            }
        else
            {
            PORTB = 0; 
            PORTB = _BV( PIN_CLK );
            }
        data <<= 1;
        PORTB = 0; 
        }
    }

void send_cmd(uint8_t cmd, uint8_t val)
    {
    PORTB = 0;
    shiftOut( cmd );
    shiftOut( val );
    PORTB = _BV( PIN_LOAD );
    }

void print(uint8_t n3, uint8_t n2, uint8_t n1, uint8_t n0)
    {
    send_cmd( max7219::shutdown, 0x00 );
    send_cmd( max7219::intensity, 0x0F );
    send_cmd( max7219::decodeMode, 0xFF );
    send_cmd( max7219::scanLimit, 0x04 );
    send_cmd( max7219::digit0, n0 );
    send_cmd( max7219::digit1, n1 );
    send_cmd( max7219::digit2, n2 );
    send_cmd( max7219::digit3, n3 );
    send_cmd( max7219::shutdown, 0x01 );
    }

void print(uint16_t v)
    {
    print( v%10, v/10%10, v/100%10, v/1000%10 );
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
    uint16_t clock_val = TCNT1>>3;
    TCNT1 = 0;
    if ( clock_val > clock_max )
        clock_max = next_pow2( clock_val );
    print( ceil(static_cast<float>(clock_max-clock_val)/clock_max*100.0) ); //16383
    }

int main()
    {
    DDRB = _BV(PIN_IDLE) | _BV(PIN_DATA) | _BV(PIN_CLK) | _BV(PIN_LOAD);
    PORTB = _BV(PIN_LOAD);
    sei();
    //TIMER1
    TCCR1B = _BV(CS12) | _BV(CS11); //External clock source on T1 pin. Clock on falling edge
    //TIMER0
    TCCR0 = _BV(CS02) | _BV(CS00); //prescaller = 1024
    TCNT0 = 0;
    TCNT1 = 0;
    TIMSK |= _BV(TOIE0); //overflow interrupt
    for (;;)
        {
        PORTB = _BV(PIN_LOAD) | _BV(PIN_IDLE);
        PORTB = _BV(PIN_LOAD);
        }

    return 0;
    }
