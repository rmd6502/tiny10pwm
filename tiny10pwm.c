#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

void setup();
void loop();

int main(void)
{
	setup();

    PRR = 1 << PRADC;
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
	while (1) {
        sleep_cpu();
	}
}

volatile uint8_t count = 0;
volatile int8_t dir = 1;

void setup()
{
	cli();
	// Enable internal pullup on the reset pin
	PUEB = 1 << PUEB3;
	// pin 2 to 0, pin 3 (reset) to 1
	// pins 0 and 1 at 1
	PORTB = 0xb;
	// set all pins but reset to output
	DDRB = 0x7;
	// Set OCR0A to 0, OCR0B to 255
	OCR0A = 0;
	OCR0B = 255;

	// Set the counter to 0
	TCNT0 = 0;

	// set the timer to top at 200, fast PWM
	TCCR0A = (2 << COM0A0) | (2 << COM0B0) | (1 << WGM00);
	// no prescaler, other half of fast PWM
	TCCR0B = (1 << WGM02) | (1 << CS00);

	// enable the overflow interrupt
	TIFR0 = (1 << TOV0);
	TIMSK0 = (1 << TOIE0);
	sei();
}

ISR(TIM0_OVF_vect)
{
	++count;
	if (count == 10) {
		OCR0A += dir;
		OCR0B -= dir;
		if (OCR0A == 0 || OCR0A == 255) {
			dir = -dir;
		}
		count = 0;
	}
}
