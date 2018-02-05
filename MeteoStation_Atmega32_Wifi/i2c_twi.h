/*
 * i2c_twi.h
 *
 *  Created on: 2010-09-07
 *       Autor: Miros³aw Kardaœ
 *	
 *
 *	Modyfikacje: Bartosz Brzozowski
 *
 */

#ifndef I2C_TWI_H_
#define I2C_TWI_H_

#define ACK 1
#define NACK 0
#define I2CBUSCLOCK		100000UL


// funkcje

void I2CStart(void);
void I2CStop(void);
void I2CWrite(uint8_t bajt);
uint8_t I2CRead(uint8_t ack);

void I2CWriteBuf( uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf );
void I2CReadBuf(uint8_t SLA, uint8_t adr, uint8_t len, uint8_t *buf);
void I2C_SetBusSpeed(uint16_t speed);
void I2CInit();

#endif /* I2C_TWI_H_ */
