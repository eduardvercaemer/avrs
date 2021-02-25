#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main (void);

int
main (void)
{
  uint8_t led;

  led = 0x00;
  DDRB = 0xff;
  PORTB = led;

  for (;;)
    {
      _delay_ms(500);
      led = (led<<1)+1;
      if (led == 31) {
	  led = 0;
      }
      PORTB = led;
    }
}
