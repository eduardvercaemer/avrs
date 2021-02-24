#include <avr/io.h>
#include <util/delay.h>

int main (void);

int
main (void)
{
  // PORTB as output
  DDRB = 0xff;
  // PORTB low
  PORTB = 0x00;

  for (;;)
    {
      _delay_ms(1000);
      // Writing to PINB toggles the pins on PORTB
      PINB = 0xff;
    }
}
