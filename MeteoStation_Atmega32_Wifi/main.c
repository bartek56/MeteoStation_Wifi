/*
 * MeteoStation_Atmega32_Wifi.c
 *
 * Created: 2017-04-08 15:20:17
 * Author : Bartek
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h> //strcat
#include <stdio.h> //sprintf
#include <avr/wdt.h> // watchdog
#include "Wireless.h"
#include "UART.h"
#include "lcd44780.h"
#include "ds18b20.h"
#include "DHT11.h"
#include "i2c_twi.h"
#include "DS3231.h"
#include "BMP085.h"
#include "ADC.h"

// help variable
uint8_t bufferIndex=0;

volatile uint8_t timer1Flag=0;
volatile int8_t wait=0;

//variable button
uint8_t showMessageLcd=0;
uint8_t numberButton=0;

char *lastEvent;


// variable sensors
char pressure[8];
char temperature[6];
char humidity;
uint8_t dataDht11[4];
long tempBMP, pressBMP;
double altitude;


char light[15];


ISR (TIMER1_COMPA_vect)
{
	timer1Flag=1;
}


int main(void)
{
  	
	Lcd_init();
	UsartInit();
	I2CInit();
	Bmp085Init();
	AdcInit();
	checkTemp();
	
	sprintf(light, "%s", Light());
	//SetClock(21,8,00);
	//SetDate(6,8,4,17);
	lcd_cls();
	lcd_locate(5,1);
	lcd_str(light);
	ShowClock();
	lcd_locate(4,2);
	lcd_str("Meteo Station");
	ShowDate();
	
	//timer to clock and wind
	TCCR1B |= (1 << WGM12); // set CLC mode
	TCCR1B |= (1 << CS12) | (1 << CS10); //prescaler 1024
	TCNT1 = 0; // start timer count
	OCR1A=15625; //1s
	TIMSK |= (1 << OCIE1A); // interrupt
		
	WDTCR|= (1<<WDE);
	wdt_enable(WDTO_2S);
		
	sei();
	
	tempBMP = Bmp085GetTemperature();
	pressBMP = Bmp085GetPressure();
	//altitude = Bmp085GetAltitude();
	
	if (pressBMP%100<10)
	sprintf(pressure, "%ld.0%ld", (pressBMP/100), (pressBMP%100));
	else
	sprintf(pressure, "%ld.%ld", (pressBMP/100), (pressBMP%100));
	
	if (tempBMP%100<10)
	sprintf(temperature, "%ld.0%ld", (tempBMP/10), (tempBMP%10));
	else
	sprintf(temperature, "%ld.%ld", (tempBMP/10), (tempBMP%10));
	
	
    /* Replace with your application code */
    while (1) 
    {
		
	// calculation wind speed after 1sec and show clock
	if(timer1Flag==1)
	{
		timer1Flag=0;
		if(showMessageLcd==0)
			ShowClock();
		else
		{
			wait++;
			checkClock();
		}
			
	}
	
	if(lcdFlagClear==1)
	{
		lcdFlagClear=0;
		wait=2;
	}
		
	
	// end show button or communicate
	if(wait==2)
	{
		lcd_cls();
		ShowClock();
		
		wait=0;
		showMessageLcd=0;
		
		lcd_locate(3,1);
		lcd_str(light);
		lcd_locate(4,2);
		lcd_str("Meteo Station");
		ShowDate();			
	}
	
		
	// buttons
	if(Survey(0)>100)
	{	
		wait=0;
		if(showMessageLcd==0)
		{
			lcd_cls();
		}
		
		showMessageLcd=1;
		
		if(Survey(0)>1000) // button 4
		{
			if(numberButton!=4)
			{
				lcd_cls();
				numberButton=4;
			}
				lcd_locate(3,0);
				lcd_str("Wireless Status");
				lcd_locate(0,1);
				lcd_str(CheckStatus());
		}
		else if(Survey(0)>650 && Survey(0)<850) // button 3
		{
			if(numberButton!=3)
			{
				lcd_cls();
				numberButton=3;
			}
			
			lcd_locate(0,0);
			lcd_str("Wind speed:");
		}
		else if(Survey(0)>350 && Survey(0)<650) // button 1
		{
			if(numberButton!=1)
			{
				lcd_cls();
				numberButton=1;
			}
						
			lcd_locate(0,4);
			lcd_str("TEMPERATURE");
			lcd_locate(4,2);
			lcd_str(temp1);
			lcd_char(0xDF);
			lcd_char(0x43);

			lcd_locate(12,2);
			lcd_str(temp2);
			lcd_char(0xDF);
			lcd_char(0x43);

			lcd_locate(0,3);
			lcd_str(temp3);
			lcd_char(0xDF);
			lcd_char(0x43);

			lcd_locate(7,3);
			lcd_str(temp4);
			lcd_char(0xDF);
			lcd_char(0x43);

			lcd_locate(14,3);
			lcd_str(temp5);
			lcd_char(0xDF);
			lcd_char(0x43);
		}
		else if(Survey(0)>100 && Survey(0)<350) // button 2
		{
			if(numberButton!=2)
			{
				lcd_cls();
				numberButton=2;
			}
			tempBMP = Bmp085GetTemperature();
			pressBMP = Bmp085GetPressure();
			//altitude = Bmp085GetAltitude();
			
			if (pressBMP%100<10)
			sprintf(pressure, "%ld.0%ld", (pressBMP/100), (pressBMP%100));
			else
			sprintf(pressure, "%ld.%ld", (pressBMP/100), (pressBMP%100));
			
			if (tempBMP%100<10)
			sprintf(temperature, "%ld.0%ld", (tempBMP/10), (tempBMP%10));
			else
			sprintf(temperature, "%ld.%ld", (tempBMP/10), (tempBMP%10));
			
			if (FetchData(dataDht11))
			{
				humidity=dataDht11[0];
			}
			
			lcd_locate(0,0);
			lcd_str("Pressure: ");
			lcd_str(pressure);
			lcd_str("hPa");
			
			
			lcd_locate(0,1);
			lcd_str("Humidity: ");
			lcd_int(humidity); // humidity
			lcd_str("%");
			
			lcd_locate(0,2);
			lcd_str("Temperature: ");
			lcd_str(temperature);
			lcd_char(0xDF);
			lcd_char(0x43);
			/*
			lcd_locate(5,3);
			lcd_int(altitude);
			lcd_str(" m a.s.l.");
			*/
			
		}		
	}// end buttons
	
	if(bufferReceiveIndex>1)
	{
		_delay_ms(100); // take all characters with USART
		bufferReceiveIndex = 0;
		/*
		wait=-1;
		if(showMessageLcd==0)
		{
			lcd_cls();
		}
		
		showMessageLcd=1;
		lcd_locate(0,0);
		lcd_char(bufferReceive[0]);
		lcd_char(bufferReceive[1]);
		lcd_char(bufferReceive[2]);
		lcd_char(bufferReceive[3]);
		lcd_char(bufferReceive[4]);
		lcd_char(bufferReceive[5]);
		lcd_char(bufferReceive[6]);
		lcd_char(bufferReceive[7]);
		lcd_char(bufferReceive[8]);
		lcd_char(bufferReceive[9]);
		lcd_char(bufferReceive[10]);
		lcd_char(bufferReceive[11]);
		lcd_char(bufferReceive[12]);
		lcd_char(bufferReceive[13]);
		lcd_char(bufferReceive[14]);
		lcd_char(bufferReceive[15]);
		lcd_char(bufferReceive[16]);
		lcd_char(bufferReceive[17]);
		lcd_char(bufferReceive[18]);
		lcd_char(bufferReceive[19]);
		lcd_char(bufferReceive[20]);
		lcd_locate(0,1);
		lcd_char(bufferReceive[21]);
		lcd_char(bufferReceive[22]);
		lcd_char(bufferReceive[23]);
		lcd_char(bufferReceive[24]);
		lcd_char(bufferReceive[25]);
		lcd_char(bufferReceive[26]);
		lcd_char(bufferReceive[27]);
		lcd_char(bufferReceive[28]);
		lcd_char(bufferReceive[29]);
		lcd_char(bufferReceive[30]);
		lcd_char(bufferReceive[31]);
		lcd_char(bufferReceive[32]);
		lcd_char(bufferReceive[33]);
		lcd_char(bufferReceive[34]);
		lcd_char(bufferReceive[35]);
		lcd_char(bufferReceive[36]);
		lcd_char(bufferReceive[37]);
		lcd_char(bufferReceive[38]);
		lcd_char(bufferReceive[39]);
		lcd_char(bufferReceive[40]);
		lcd_locate(0,2);
		lcd_char(bufferReceive[41]);
		lcd_char(bufferReceive[42]);
		lcd_char(bufferReceive[43]);
		lcd_char(bufferReceive[44]);
		lcd_char(bufferReceive[45]);
		lcd_char(bufferReceive[46]);
		lcd_char(bufferReceive[47]);
		lcd_char(bufferReceive[48]);
		lcd_char(bufferReceive[49]);
		lcd_char(bufferReceive[50]);
		lcd_char(bufferReceive[51]);
		lcd_char(bufferReceive[52]);
		lcd_char(bufferReceive[53]);
		lcd_char(bufferReceive[54]);
		lcd_char(bufferReceive[55]);
		lcd_char(bufferReceive[56]);
		lcd_char(bufferReceive[57]);
		lcd_char(bufferReceive[58]);
		lcd_char(bufferReceive[59]);
		lcd_char(bufferReceive[60]);
		lcd_locate(0,3);
		lcd_char(bufferReceive[71]);
		lcd_char(bufferReceive[72]);
		lcd_char(bufferReceive[73]);
		lcd_char(bufferReceive[74]);
		lcd_char(bufferReceive[75]);
		lcd_char(bufferReceive[76]);
		lcd_char(bufferReceive[77]);
		lcd_char(bufferReceive[78]);
		lcd_char(bufferReceive[79]);
		lcd_char(bufferReceive[80]);
		lcd_char(bufferReceive[81]);
		lcd_char(bufferReceive[82]);
		lcd_char(bufferReceive[83]);
		lcd_char(bufferReceive[84]);
		lcd_char(bufferReceive[85]);
		lcd_char(bufferReceive[86]);
		lcd_char(bufferReceive[87]);
		lcd_char(bufferReceive[88]);
		lcd_char(bufferReceive[89]);
		lcd_char(bufferReceive[90]);
		*/
		
		
		if(bufferReceive[0]=='W' && bufferReceive[1]=='I' && bufferReceive[5]=='G' && bufferReceive[6]=='O' )
		{
			wirelessReady=1;
		}
		if(bufferReceive[0]=='W' && bufferReceive[1]=='I' && bufferReceive[5]=='D' && bufferReceive[6]=='I' )
		{
			wirelessReady=0;
		}
		
	}
	
	if(checkEvent())
	{
		wdt_reset();
		lastEvent=getLastEvent();
		
		if(strcmp(lastEvent,"OTHER")==0)
		{
			tempBMP = Bmp085GetTemperature();
			pressBMP = Bmp085GetPressure();
			altitude = Bmp085GetAltitude();
			
			if (pressBMP%100<10)
			sprintf(pressure, "%ld.0%ld", (pressBMP/100), (pressBMP%100));
			else
			sprintf(pressure, "%ld.%ld", (pressBMP/100), (pressBMP%100));
			
			if (tempBMP%100<10)
			sprintf(temperature, "%ld.0%ld", (tempBMP/10), (tempBMP%10));
			else
			sprintf(temperature, "%ld.%ld", (tempBMP/10), (tempBMP%10));
			

			
			if (FetchData(dataDht11))
			{
				humidity=dataDht11[0];
				//lcd_int(dataDht11[2])
			}
		}
		else if(strcmp(lastEvent,"TEMP")==0)
		{
			checkTemp();
		}
		
		else if(strcmp(lastEvent,"WIFIWEB")==0)
		{
			bufferReceiveIndex = 0;
			WirelessWebSite();
		}
		else if(strcmp(lastEvent,"WIFIGETOTHER")==0)
		{
			bufferReceiveIndex = 0;
			WirelessGetOther(pressure,humidity,rainFlag,lightState);
		}
		else if(strcmp(lastEvent,"WIFIGETTEMP")==0)
		{
			bufferReceiveIndex = 0;			
			WirelessGetTemp(temp1,temp2,temp3,temp4,temp5);
		}				
		
		
	} // end events
		
	
	wdt_reset();	
    } // end while
	
}

