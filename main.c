#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t eeprom_read (uint16_t addr);
void eeprom_write (uint16_t addr, uint8_t b);
int main (void);

uint8_t
eeprom_read (uint16_t addr)
{
  // wait for completion of prev. write
  while (EECR & _BV(EEPE))
    ;

  // eeprom address
  EEAR = addr;

  // read value
  EECR |= _BV(EERE);
  return EEDR;
}

void
eeprom_write (uint16_t addr, uint8_t b)
{
  // wait for completion of prev. write
  while (EECR & _BV (EEPE))
    ;

  // eeprom address
  EEAR = addr;
  // eeprom data
  EEDR = b;

  // start write
  EECR |= _BV (EEMPE);
  EECR |= _BV (EEPE);
}

int
main (void)
{
  uint8_t led;

  // get led state from eeprom
  led = eeprom_read(0x0000);

  for (;;);
}
