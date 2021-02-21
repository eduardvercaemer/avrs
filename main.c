#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 *  Implements a debounced key press polled system with PORTD
 *
 *  Uses timer0 and the overflow interrupt to constantly check the status
 *  of the input pins, if a change is persistent for 3 consecutive checks,
 *  it is interpreted as the stable value and put in `key_state`
 *
 *  We also add the key presses in `key_pressed` so then we can use the
 *  function `get_key_press` to 'consume' the pending presses.
 */

static uint8_t key_state = 0;	// stable pressed state
static uint8_t key_pressed = 0;	// pending jey presses

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
uint8_t count = 0;

  /* Init I/O */
  DDRB= 0xff;			// PORTB is out
  PORTB = 0x00;
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
	  count++;
	  PORTB = count;
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
