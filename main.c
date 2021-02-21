#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>

static uint8_t key_state = 0;
static uint8_t key_pressed = 0;
/*
 *  Used for PORTD input (see `debouncing`)
 */

static uint8_t EEMEM ee_led = 1;
/*
 *  Persistent memory for led state
 *  (initial value of 1 when programmed, check the Makefile)
 */

uint8_t get_key_press (uint8_t mask);
int main (void);

uint8_t
get_key_press (uint8_t mask)
{
  cli ();
  mask &= key_pressed;
  key_pressed ^= mask;
  sei ();
  return mask;
}

int
main (void)
{
  uint8_t led;

  // get led state from eeprom
  led = eeprom_read_byte(&ee_led);

  /* Init I/O */
  DDRB= 0xff;			// PORTB is out
  PORTB = led;
  DDRD = 0x00;			// PORTD is in
  PORTD = 0xff;			// pulled high

  /* Timer Init */
  TCNT0 = 0x00;
  TCCR0B = _BV (CS00) | _BV (CS02);	// 1024 prescaler
  TIMSK0 |= _BV (TOIE0);

  /* Start main loop */
  sei ();
  _delay_ms (1000);
  for (;;)
    {
      if (get_key_press (_BV (PORTD2)))
	{			// key on PORTD2 was pressed
	  if (led)
	    led = 0;
	  else
	    led = 0xff;

	  PORTB = led;
	  eeprom_write_byte(&ee_led, led);
	}
    }
}

ISR (TIMER0_OVF_vect)
{
static uint8_t ct0 = 0x00, ct1 = 0x00;
uint8_t delta, real;

  // carry bit toggle counts
  real = ~PIND;
  delta = key_state ^ real;
  key_state ^= delta & ct1;
  ct1 = ct0 & delta;
  ct0 = delta;
  key_pressed |= real & ct1 & key_state;
}
