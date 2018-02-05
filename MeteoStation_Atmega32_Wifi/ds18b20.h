/*
 *
 * Created by: xyz.isgreat.org
 * 
 * Modyfikacje: Bartosz Brzozowski
 *  
 *  
*/


#ifndef DS18B20_H
#define DS18B20_H

/* DS18B20 przylaczony do portu  PD7 AVRa  */
#define SET_ONEWIRE_PORT     PORTD  |=  _BV(7)
#define CLR_ONEWIRE_PORT     PORTD  &= ~_BV(7)
#define IS_SET_ONEWIRE_PIN   PIND   &   _BV(7)
#define SET_OUT_ONEWIRE_DDR  DDRD   |=  _BV(7)
#define SET_IN_ONEWIRE_DDR   DDRD   &= ~_BV(7)

extern char temp1[7];
extern char temp2[7];
extern char temp3[7];
extern char temp4[7];
extern char temp5[7];

char ds18b20_ConvertT(void);
char ds18b20_Read(unsigned char []);
void OneWireStrong(char);
unsigned char OneWireReset(void);
void OneWireWriteByte(unsigned char);
unsigned char OneWireReadByte(void);
unsigned char OneWireReadTimeSlot(void);
void checkTemp();


#endif
