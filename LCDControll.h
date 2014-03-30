/*
 * LCDControll.h
 *
 *  Created on: 2014.03.30.
 *      Author: zoon
 */
//							Some basic functions for Sinus 421 ISDN phone LCD display
//							Unknown graphic LCD driver chip on glass device
//								it is 101x48 pixel LCD
//		left to right front view PIN 1,2,3:VCC  (3.3v)
//								 PIN 4,5  : SDA,SDC
//								 PIN 6	  : GND
//								 PIN7     :Vcontrast about 8.3v
#ifndef LCDCONTROLL_H_
#define LCDCONTROLL_H_
#ifndef I2C_H_
#include "i2c.h"
#endif
void LCD_init()
{
	i2c_init();

		i2c_start_wait(0x78);
		i2c_write(0x00);		//Nobody Know what is it...
		i2c_write(0x30);
		i2c_write(0x05);
		i2c_write(0x12);
		i2c_write(0x31);
		i2c_write(0x12);
		i2c_write(0xaf);
		i2c_write(0x07);
		i2c_write(0x30);
		i2c_write(0x08);
		i2c_write(0x80);
		i2c_write(0x40);
		i2c_stop();

		i2c_start_wait(0x78); 	//turn On Display
		i2c_write(0x00);
		i2c_write(0x0c);
		i2c_stop();
	}
void LCD_clearall()
{
		uint16_t a = 6*101;
		i2c_start_wait(0x78);  // 1. COLLUMN
			    i2c_write(0x80);
				i2c_write(0x80);
				i2c_write(0x80);
				i2c_write(0x40);
				i2c_write(0x40);
				while(a)			//WRITE PIXEL DATA
				{
					i2c_write(0x00	);
					a--;
				}

			i2c_stop();
}

#endif /* LCDCONTROLL_H_ */
