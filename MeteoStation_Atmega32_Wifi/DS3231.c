/*
 * main.c
 *
 * Created: 2016-09-21 10:35:23
 * Author : Bartosz Brzozowski
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h> //strcat
#include <stdio.h> //sprintf
#include "lcd44780.h"
#include "i2c_twi.h"
#include "DS3231.h"
#include "Wireless.h"


volatile int WirelessSet=0;
int year;
volatile int eventHead;
volatile int eventTail;
//volatile char events[eventsSize][150];
volatile char events[eventsSize][16];

int gsmOn=0;

char ret[32];

uint8_t buforI2C[10];

int seconds;
int minutes;
int hours;

// converting decimal number to BCD
int DecToBcd(uint8_t dec) {
return ((dec / 10)<<4) | (dec % 10);
}

// converting BCD number to decimal
int BcdToDec(uint8_t bcd) {
    return ((((bcd) >> 4) & 0x0F) * 10) + ((bcd) & 0x0F);
}


void newEvent(char* event)
{
	int tmpEvent;
	tmpEvent = (eventHead+1) & eventsMask;
		
	if(tmpEvent != eventTail) // if is free memory in buffer 
	{
		eventHead = tmpEvent;
		for(int i=0; i<16;i++)
			events[tmpEvent][i]=event[i];
	}	
}

char* getLastEvent()
{
	
		eventTail = (eventTail+1) & eventsMask;
		
		for(int i=0;i<16;i++)
			ret[i]=events[eventTail][i];
		return ret;			
	
}

int checkEvent()
{
	if(eventHead!=eventTail)
		return 1; 
	else
		return 0; // buffer is empty
}


void SetClock(uint8_t hr, uint8_t min, uint8_t sec)
{
	buforI2C[0] = DecToBcd(sec);	// seconds
	buforI2C[1] = DecToBcd(min);	// minutes
	buforI2C[0] = DecToBcd(hr);	// hours
	I2CWriteBuf( DS3231_ADDR, 0x02, 1, buforI2C );
}

void SetClockOnlyHour(uint8_t hr)
{
	buforI2C[0] = DecToBcd(hr);		// hours
	I2CWriteBuf( DS3231_ADDR, 0x02, 1, buforI2C );
}

void SetDate(uint8_t weekDay, uint8_t day, uint8_t month, uint8_t year)
{
	buforI2C[0] = DecToBcd(weekDay);
	buforI2C[1] = DecToBcd(day);
	buforI2C[2] = DecToBcd(month);
	buforI2C[3] = DecToBcd(year);
	I2CWriteBuf( DS3231_ADDR, 0x03, 4, buforI2C );
}


void checkClock()
{
	
	I2CReadBuf( DS3231_ADDR, 0x00, 4, buforI2C );
	
	seconds = BcdToDec( buforI2C[0] );
	minutes = BcdToDec( buforI2C[1] );
	hours = BcdToDec( buforI2C[2] );
	
	if(seconds==7)
	{
		newEvent("TEMP");
	}
	if(seconds==30)
	{
		newEvent("OTHER");
	}
	
	
	if(wirelessReady==1)
	{
		if(minutes%28==27 && seconds==58)
		{
			newEvent("WIFIWEB");
		}
		else if(minutes%28==0 && seconds==00)
		{
			newEvent("WIFIGETTEMP");
		}
		
		
		else if(minutes%28==00 && seconds==58)
		{
			newEvent("WIFIWEB");
		}
		else if(minutes%28==1 && seconds==00)
		{
			newEvent("WIFIGETOTHER");
		}
		
	}
}


void ShowClock()
{
	
	checkClock();
	// show time
	lcd_locate(0,0);
	if( hours < 10 ) lcd_str("0");
	lcd_int(hours);
	lcd_str(":");
	if( minutes < 10 ) lcd_str("0");
	lcd_int(minutes);
	lcd_str(":");
	if( seconds < 10 ) lcd_str("0");
	lcd_int(seconds);
}

void ShowDate()
{
	int month;
	int day;
	char weekDay;
	
	I2CReadBuf( DS3231_ADDR, 0x03, 4, buforI2C );	
	 
	weekDay = BcdToDec( buforI2C[0] );
	day = BcdToDec( buforI2C[1] );
	month = BcdToDec( buforI2C[2] );
	year = BcdToDec( buforI2C[3] );

	//show date
	lcd_locate(10,0);
	if( day < 10 )
	lcd_str("0");
	lcd_int(day);
	lcd_str("/");
	if( month < 10 )
	lcd_str("0");
	lcd_int(month);
	lcd_str("/");
	lcd_str("20");
	lcd_int(year);
	
	switch(weekDay)
	{
	case 1: lcd_locate(7,3); lcd_str("MONDAY"); break;
	case 2: lcd_locate(7,3); lcd_str("TUESDAY"); break;
	case 3: lcd_locate(6,3); lcd_str("WEDNESDAY"); break;
	case 4: lcd_locate(6,3); lcd_str("THURSDAY"); break;
	case 5: lcd_locate(7,3); lcd_str("FRIDAY"); break;
	case 6: lcd_locate(6,3); lcd_str("SATURDAY"); break;
	case 7: lcd_locate(7,3); lcd_str("SUNDAY"); break;
	}

}
