/*
 * i2c_twi.c
 *
 *  Created on: 2010-09-07
 *       Autor: Miros³aw Kardaœ
 *	
 *	Modyfikacje: Bartosz Brzozowski
 *
 */
#include <avr/io.h>

#include "i2c_twi.h"


void I2C_SetBusSpeed(uint16_t speed)
{
	speed=(F_CPU/speed/100-16)/2;		//speed=TWBR*4^TWPS
	uint8_t prescaler=0;
	while(speed>255)				//Oblicz wartoœæ preskalera
	 {
		prescaler++;
		speed=speed/4;
	 };
	TWSR=(TWSR & (_BV(TWPS1)|_BV(TWPS0))) | prescaler;
	TWBR=speed;
}

void I2CInit()
{
	TWCR =_BV(TWEA) | _BV(TWEN);		//W³¹cz interfejs I2C
	I2C_SetBusSpeed(I2CBUSCLOCK/100);
}

void I2CStart(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
	while (!(TWCR&(1<<TWINT)));
}

void I2CStop(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ( !(TWCR&(1<<TWSTO)));
}

void I2CWrite(uint8_t bajt) {
	TWDR = bajt;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ( !(TWCR&(1<<TWINT)));
}

uint8_t I2CRead(uint8_t ack) {
	TWCR = (1<<TWINT)|(ack<<TWEA)|(1<<TWEN);
	while ( !(TWCR & (1<<TWINT)));
	return TWDR;
}



void I2CWriteBuf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf ) {
	I2CStart();
	I2CWrite(SLA);
	I2CWrite(adr);
	while (len--) I2CWrite(*buf++);
	I2CStop();
}



void I2CReadBuf(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf) {
	I2CStart();
	I2CWrite(SLA);
	I2CWrite(adr);;
	I2CStart();
	I2CWrite(SLA + 1);
	while (len--) *buf++ = I2CRead( len ? ACK : NACK );
	I2CStop();
}
