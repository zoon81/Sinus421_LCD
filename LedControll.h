/*
 * LedControll.h
 *
 *  Created on: 2014.03.29.
 *      Author: zoon
 */

#ifndef LEDCONTROLL_H_
#define LEDCONTROLL_H_

#ifndef __HAS_DELAY_CYCLES
#include <util/delay.h>
#endif

#ifndef _AVR_IO_H_
#include <avr/io.h>
#endif

void delay_ms(uint16_t delay) {
	while (delay) {
		_delay_ms(1);
		delay--;

	}
}
void LedBlink(uint8_t NumberOfBlinks, uint16_t delay) {
	DDRB |= 1;
	while (NumberOfBlinks) {
		PORTB |= 1;
		delay_ms(delay);
		PORTB &= ~1;
		delay_ms(delay);
	}

}

#endif /* LEDCONTROLL_H_ */
