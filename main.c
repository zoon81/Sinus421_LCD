/*
 * main.c
 *
 *  Created on: 2014.03.28.
 *      Author: zoon
 */

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/iom88.h>

#include "LedControll.h"
#include "i2c.h"
#include "LCDControll.h"


int main(void)  {

	_delay_ms(200); //LCD WAKE UP TIME

LCD_init();
LCD_clearall();

while (1) {
		LedBlink(1, 200);
	}
}

