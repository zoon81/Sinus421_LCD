/*
 * main.c
 *
 *  Created on: 2014.03.28.
 *      Author: zoon
 */
/*************************************************************************
* Title:    I2C master library using hardware TWI interface
* Author:   Peter Fleury <<a href="mailto:pfleury@gmx.ch">pfleury@gmx.ch</a>>  <a href="http://jump.to/fleury">http://jump.to/fleury</a>
**************************************************************************/
/*
ADC + UART
Hardware: ATmega8 @ 7.372800MHz
PC Software:Hyper terminal @ 9600 baud, Nincs Paritas Bit, 1 Stop Bit, Flow Control = NONE
*/

#define SCL_CLOCK  100000L // I2C sebessege Hz-ben
#define I2C_READ    1    // adatiranyt jelzo bit - olvasas
#define I2C_WRITE   0    // adatiranyt jelzo bit - iras

#include <inttypes.h>
#include <util/delay.h>
#include <compat/twi.h>
#include <avr/iom88.h>

#include "LedControll.h"


/*************************************************************************
I2C periferia inicializalasa
*************************************************************************/
void i2c_init(void)
{
  TWSR = 0;
  TWBR = ((F_CPU/SCL_CLOCK)-16)/2;
}
/*************************************************************************
Startjel kiadasa, majd az eszkozazonosito cim es adatirany elkuldese
Fuggv. parameter: I2C eszkozazonosito cim, adatirany
Fuggv. altal visszadott ertek: 0 = eszkoz elerheto, 1= eszkoz elerese sikertelen
*************************************************************************/
unsigned char i2c_start(unsigned char address)
{
    uint8_t   twst;
 // START jel kuldese
 TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
 // varakozas amig az atvitel be nem fejezodik
 while(!(TWCR & (1<<TWINT)));
 // TWI Statusz Register ertekenek ellenorzese, elooszto bitek kimaszkolasa.
 twst = TW_STATUS & 0xF8;
 if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;
 // eszkozazonosito cim kuldese
 TWDR = address;
 TWCR = (1<<TWINT) | (1<<TWEN);
 // varakozas amig az atvitel be nem fejezodik es az ACK/NACK nyugtazas be nem erkezik
 while(!(TWCR & (1<<TWINT)));
 // TWI Statusz Register ertekenek ellenorzese, elooszto bitek kimaszkolasa.
 twst = TW_STATUS & 0xF8;
 if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
 return 0;
}
/*************************************************************************
Startjel kiadasa, majd az eszkozazonosito cim es adatirany elkuldese
Ha az eszkoz foglalt, varakozas amig kesz nem lesz (ack polling)
Fuggv. parameter: I2C eszkozazonosito cim, adatirany
*************************************************************************/
void i2c_start_wait(unsigned char address)
{
    uint8_t   twst;
    while ( 1 )
    {
     // START jel kuldese
     TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
     // varakozas amig az atvitel be nem fejezodik
     while(!(TWCR & (1<<TWINT)));
     // TWI Statusz Register ertekenek ellenorzese, elooszto bitek kimaszkolasa.
     twst = TW_STATUS & 0xF8;
     if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
     // eszkozazonosito cim kuldese
     TWDR = address;
     TWCR = (1<<TWINT) | (1<<TWEN);
     // varakozas amig az atvitel be nem fejezodik
     while(!(TWCR & (1<<TWINT)));
     // TWI Statusz Register ertekenek ellenorzese, elooszto bitek kimaszkolasa.
     twst = TW_STATUS & 0xF8;
     if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) )
     {
         // az eszkoz foglalt, Stop jel kuldese, irasi muvelet befejezese
         TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
         //  varakozas amig a Stop utasitas vegre nem hajtodik es az I2C busz szabad nem lesz
         while(TWCR & (1<<TWSTO));
         continue;
     }
     //if( twst != TW_MT_SLA_ACK) return 1;
     break;
     }
}

/*************************************************************************
Ismetelt Startjel kiadasa, majd az eszkozazonosito cim es adatirany elkuldese
Fuggv. parameter: I2C eszkozazonosito cim, adatirany
Fuggv. altal visszadott ertek: 0 = eszkoz elerheto, 1= eszkoz elerese sikertelen
*************************************************************************/
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );
}
/*************************************************************************
Stopjel kuldese, kommunikacio lezarasa ,I2C bus szabadda tetele *************************************************************************/
void i2c_stop(void)
{
 // STOP jel kuldese
 TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
 //  varakozas amig a Stop utasitas vegre nem hajtodik es az I2C busz szabad nem lesz
 while(TWCR & (1<<TWSTO));
}
/*************************************************************************
Egy bajt kuldese az I2C eszkozre
Fuggv. parameter:    byte to be transfered
Fuggv. altal visszaadott ertek: 0 iras sikeres, 1 iras sikertelen *************************************************************************/
unsigned char i2c_write( unsigned char data )
{
  uint8_t   twst;
 //  adat irasa a korabban megcimzett eszkozbe
 TWDR = data;
 TWCR = (1<<TWINT) | (1<<TWEN);
 // varakozas amig az atvitel be nem fejezodik
 while(!(TWCR & (1<<TWINT)));
// TWI Statusz Register ertekenek ellenorzese, elooszto bitek kimaszkolasa.
 twst = TW_STATUS & 0xF8;
 if( twst != TW_MT_DATA_ACK) return 1;
 return 0;
}
/*************************************************************************
Egy bajt kiolvasasa az I2C eszkozbol, tovabbi adatok kerese
Fuggv. altal visszaadott ertek:  az I2C eszkozbol kiolvasott bajt *************************************************************************/
unsigned char i2c_readAck(void)
{
 TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
 while(!(TWCR & (1<<TWINT)));
      return TWDR;
}
/*************************************************************************
Egy bajt kiolvasasa az I2C eszkozbol, a kiolvasast Stop bit koveti
Fuggv. altal visszaadott ertek:  az I2C eszkozbol kiolvasott bajt *************************************************************************/
unsigned char i2c_readNak(void)
{
 TWCR = (1<<TWINT) | (1<<TWEN);
 while(!(TWCR & (1<<TWINT)));
    return TWDR;
}



int main(void)  // Foprogram
{
DDRB = 1;
i2c_init();
i2c_start_wait(0x78);
i2c_write(0x00);
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

i2c_start_wait(0x78);
i2c_write(0x00);
i2c_write(0x0c);
i2c_stop();

i2c_start_wait(0x78);
i2c_write(0xff);
i2c_write(0xff);
i2c_write(0xff);
i2c_write(0xff);
i2c_write(0xff);
i2c_stop();






	while(1)
	{
		LedBlink(1,200);
	}
}

