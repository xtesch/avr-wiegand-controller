/*

Input pins :

PD2 : listen D0
PD3 : listen D1

Output pins :

PD6 : garage door
PD7 : Buzzer

*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint64_t bits = 0;
volatile uint8_t bitCount = 0;

void checkId() {
  bits >>= 1;
  uint32_t result = 0;
  result |= bits;

  if(0x737B47 == bits) {
    PORTD |= (1 << PIND6);
    PORTD |= (1 << PIND7);
    _delay_ms(300);
    PORTD &= ~(1 << PIND6);
    PORTD &= ~(1 << PIND7);
  }
}

void write1() {
    PORTD |= (1 << PIND6);
    PORTD |= (1 << PIND7);

    _delay_ms(2);
    PORTD &= ~(1 << PIND6);
    PORTD &= ~(1 << PIND7);
    _delay_ms(1);
}

void write0() {
    
    PORTD |= (1 << PIND6);
    PORTD |= (1 << PIND7);

    _delay_ms(1);
    PORTD &= ~(1 << PIND6);
    PORTD &= ~(1 << PIND7);
    _delay_ms(1);
}

void writeNumber(uint8_t number) {
    
    for(int i=0; i < number; i++) {
        write0();
    }

    _delay_ms(100);
}

void writeResult() {
    uint64_t buffer = bits;

    //writeNumber(bitCount);

    for(int i=0; i<bitCount; i++) {
        if(1 & buffer) {
          write1();
        } else {
          write0();
        }
        buffer >>= 1;
    }
}

void addBitToCardId(uint8_t bit) {
  bits <<= 1;
  bits |= bit;
  bitCount++;

  if(bitCount == 26) {
    checkId();
    bitCount = 0;
    bits = 0;
  }
}

ISR(INT0_vect) {
  addBitToCardId(0);
}

ISR(INT1_vect) {
  addBitToCardId(1);
}

void initInterrupts(void) {
  EIMSK |= (1 << INT0);           /* enable INT0 */
  EICRA |= (1 << ISC01);          /* trigger INT0 on falling */
  EICRA &= ~(1 << ISC00);          /* trigger INT0 on falling */

  EIMSK |= (1 << INT1);           /* enable INT1 */
  EICRA |= (1 << ISC11);          /* trigger INT1 on falling */
  EICRA &= ~(1 << ISC10);         /* trigger INT1 on falling */

  sei();                          /* set (global) interrupt enable bit */
}

int main(void) {

  DDRD &= ~(1 << PIND2);
  DDRD &= ~(1 << PIND3);
  DDRD |= (1 << PIND6);
  DDRD |= (1 << PIND7);

  PORTD = 0x00;
  PORTD |= (1 << PORTD2);
  PORTD |= (1 << PORTD3);

  initInterrupts();

  while (1) {
    _delay_ms(10000);
    //writeResult();
    bitCount = 0;
    bits = 0;
  }
  return 0;
}
