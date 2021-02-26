#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main (void);

int
main (void)
{
  /* INIT OUTPUT */
  DDRB = 0xff;
  PORTB = 0x00;

  /* INIT THE ADC */
  ADMUX = (0 << REFS1) | (0 << REFS0)	// Use AVcc
    | _BV (ADLAR)			// Left adjusted
    | 0;				// Single input ADC0
  ADCSRA = _BV (ADEN)		  	// ADC Enable
    | _BV (ADPS2) | _BV (ADPS1) | _BV (ADPS0);	// 128 prescaler

  for (;;)
    {
      /* READ VALUE FROM ADC */
      uint8_t adc;
      ADCSRA |= _BV(ADSC); // start conversion
      while (ADCSRA & _BV(ADSC)) // wait result
	;
      adc = ADCH;
      adc = 0xff - adc;

      PORTB = ((adc >= 0) << 0)
       | ((adc >= 32) << 1)
       | ((adc >= 64) << 2)
       | ((adc >= 96) << 3)
       | ((adc >= 128) << 4)
       | ((adc >= 160) << 5)
       | ((adc >= 192) << 6)
       | ((adc >= 224) << 7);
    }
}
