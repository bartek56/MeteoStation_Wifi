/*
 * ADC.c
 *
 * Created: 2016-09-21 10:35:23
 * Author : Bartosz Brzozowski
 */ 

#include <avr/io.h>
#include <stdio.h> //sprintf
#include "ADC.h"
#include "lcd44780.h"


int lightState=0;
int rainFlag=0;
char state[20];


void AdcInit()
{
	ADMUX |= (1<<REFS0); // voltage comaprison to VCC
	ADCSRA |= (1<<ADEN) | (1<<ADPS2); // switch on converter and set prescaler on 16
}

int Survey(int channel)
{
	ADMUX=0;
	ADMUX |= (1<<REFS0) | (ADMUX & 0xf8) | channel;
	ADCSRA |= (1<<ADSC); // start conversion
	while(ADCSRA & (1<<ADSC));
	return ADCW;
}


char* Light()
{
	
	if(Survey(2)<400)
		{
			if(rainFlag==0)
			{
				lcdFlagClear=1;
				rainFlag=1;
				sprintf(state, "%s", "         Rain");
			}
		}
		else
		{
			if(Survey(1)>710)
			{	
				if(lightState!=3 || rainFlag==1)
				{
					rainFlag=0;
					lcdFlagClear=1;
					lightState=3;
					sprintf(state, "%s", "        Sunny");
				}
			}
			else if(Survey(1)>180 && Survey(1)<=710)
			{	
				if(lightState!=2 || rainFlag==1)
				{
					rainFlag=0;
					lcdFlagClear=1;
					lightState=2;
					sprintf(state, "%s", "       Cloudy");
				}
			}		
			else if(Survey(1)<=180)
			{
				if(lightState!=1 || rainFlag==1)
				{
					rainFlag=0;
					lcdFlagClear=1;
					lightState=1;
					sprintf(state, "%s", "        Night");
				}
			}
		}
		return state;
}

