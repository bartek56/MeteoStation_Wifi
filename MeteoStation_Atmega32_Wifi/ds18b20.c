/*
 *
 * Created by: xyz.isgreat.org
 * 
 * Modyfikacje: Bartosz Brzozowski
 *  
 *  
*/

#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h> //sprintf
#include "ds18b20.h"

unsigned char ds18b20_pad[9];
double  temp[5];
char temp1[7];
char temp2[7];
char temp3[7];
char temp4[7];
char temp5[7];
unsigned char rom_code[][8] = {	
								{0x28,0x90,0xE8,0x2D,0x05,0x00,0x00,0x93},
                                {0x28,0xFE,0xCD,0x63,0x04,0x00,0x00,0x37},
								{0x28,0xC9,0xC4,0x63,0x04,0x00,0x00,0xAC},						
								{0x28,0x41,0xD9,0x63,0x04,0x00,0x00,0x77},
								{0x28,0xF0,0x08,0x2D,0x05,0x00,0x00,0x8A}
								};

/**********************************************************/

char ds18b20_ConvertT(void)
{
  if (!OneWireReset()) return 0;

  OneWireWriteByte(0xcc); // SKIP ROM
  OneWireWriteByte(0x44); // CONVERT T

  return 0xff;
}

/***********************************************************/

char ds18b20_Read(unsigned char scratchpad[])
{
  unsigned char i;	

  if (!OneWireReset()) return 0;

  OneWireWriteByte(0xcc); // SKIP ROM
  OneWireWriteByte(0xbe); // READ SCRATCHPAD

  for(i=0; i<10; i++) scratchpad[i] = OneWireReadByte();
 
  return 0xff;
}

/**********************************************************/

void OneWireStrong(char s)
{
  if (s)
  {
     SET_ONEWIRE_PORT; 
     SET_OUT_ONEWIRE_DDR; 
  }
  else
  {
     SET_IN_ONEWIRE_DDR; 
  }
}

/**********************************************************/

unsigned char OneWireReset()
{
   SET_IN_ONEWIRE_DDR; 
  CLR_ONEWIRE_PORT; 

  if (!(IS_SET_ONEWIRE_PIN)) return 0;  

  SET_OUT_ONEWIRE_DDR; // stan niski
  _delay_us(500);
  SET_IN_ONEWIRE_DDR; 
  _delay_us(70);

  if(!(IS_SET_ONEWIRE_PIN))
  {
    _delay_us(500);
    return(1);
  }

  _delay_us(500);

return(0);
}

/**********************************************************/

void OneWireWriteByte(unsigned char byte)
{
   unsigned char i;

   SET_IN_ONEWIRE_DDR; 
   CLR_ONEWIRE_PORT; 

   for (i=0; i<8; i++)
   {
     SET_OUT_ONEWIRE_DDR; 

     if (byte & 0x01)
     {
       _delay_us(7);
       SET_IN_ONEWIRE_DDR; 
       _delay_us(70);
     }
     else
     {
        _delay_us(70);
        SET_IN_ONEWIRE_DDR; 
        _delay_us(7);
     }

     byte >>= 1;
   }
}

/***********************************************************/

unsigned char OneWireReadByte(void)
{
  unsigned char i, byte = 0;

  CLR_ONEWIRE_PORT; 
  SET_IN_ONEWIRE_DDR; 
  
  for (i=0; i<8; i++)
  {
     SET_OUT_ONEWIRE_DDR; 
     _delay_us(7);
     SET_IN_ONEWIRE_DDR; 
     _delay_us(7);
     byte >>= 1; // przesuniêcie w prawo i przypisanie
     
     if(IS_SET_ONEWIRE_PIN) byte |= 0x80;

     _delay_us(70);
  }

  return byte;
}

/***********************************************************/

unsigned char OneWireReadTimeSlot(void)
{
	unsigned char  byte = 0;

	CLR_ONEWIRE_PORT;
	SET_IN_ONEWIRE_DDR;
	
	SET_OUT_ONEWIRE_DDR;
	_delay_us(7);
	SET_IN_ONEWIRE_DDR;
	_delay_us(7);
	
	if(IS_SET_ONEWIRE_PIN) byte = 0xff;
	_delay_us(70);

	return byte;
}

/***********************************************************/

void checkTemp()
{
	while(!OneWireReset());
    OneWireWriteByte(0xCC); // SKIP ROM 
    OneWireWriteByte(0x44); // CONVERT T
    _delay_ms(750);
	
 	for(int j=0; j<5; j++)
    {
    	while(!OneWireReset());
        OneWireWriteByte(0x55); // MATCH ROM 
        for(int i=0; i<8; i++) OneWireWriteByte(rom_code[j][i]);
        OneWireWriteByte(0xbe); // READ SCRATCHPAD
        for(int i=0; i<9; i++) ds18b20_pad[i] = OneWireReadByte();
        temp[j] = ((ds18b20_pad[1] << 8) + ds18b20_pad[0])/16.0 ;
	}

	sprintf(temp1, "%4.1f", temp[0]);
	sprintf(temp2, "%4.1f", temp[1]);
	sprintf(temp3, "%4.1f", temp[2]);
	sprintf(temp4, "%4.1f", temp[3]);
	sprintf(temp5, "%4.1f", temp[4]);
}
