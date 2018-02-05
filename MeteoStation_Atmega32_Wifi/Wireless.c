/*
 * Wireless.c
 *
 * Created: 2017-04-08 15:24:20
 *  Author: Bartek
 */ 
#include <util/delay.h>
#include <string.h> //strcat
#include <stdio.h> //sprintf
#include <avr/wdt.h> // watchdog

#include "Wireless.h"
#include "UART.h"
#include "lcd44780.h"


char CLIENTMODE[]="AT+CWMODE=1\r\n";
char CONNECTWITHNETWORK[]="AT+CWJAP=\"TP-LINK\",\"pizda204\"\r\n";
char CONNECTWITHSERVER[] = "AT+CIPSTART=\"TCP\",\"meteostation.y0.pl\",80\r\n";
char CHECKSTATUS[]="AT+CIPSTATUS\r\n";


int CountMark(char *HTTP)
{
	int countMark=0;
	while(HTTP[countMark]!=0x00)
	{
		countMark++;
	}
	return countMark-10;
}

char *CheckStatus()
{
	ClearBufor();
	Send(CHECKSTATUS);
	//_delay_ms(300);
	while(bufferReceive[21]!=':');
	_delay_ms(1);

	switch (bufferReceive[22])
	{
		case '1' :	return "1";	break;
		case '2' : return "Connected";		break;
		case '3' : return "TCP ON";		break;
		case '4' : return "TCP OFF";		break;
		case '5' : return "Disconnected";		break;
	}
	
	return "Error";
}

void WirelessWebSite()
{
	Send(CONNECTWITHSERVER);
}

void WirelessGetOther(char* pressure, int humidity, int rainFlag, int lightState)
{
	char HTTP[150];
	sprintf(HTTP,"GET http://www.meteostation.y0.pl/GSMOther.php?sp=0&dir=0&pres=%s&hum=%i&ra=%i&li=%iHTTP/1.0\r\nHost:meteostation.y0.pl",pressure, humidity, rainFlag, lightState );
	
	char command[20];
	sprintf(command,"AT+CIPSEND=%i\r\n",CountMark(HTTP));
	
	_delay_ms(100);
	Send(command);
	_delay_ms(300);
	Send(HTTP);
}



void WirelessGetTemp(char* temp1,char* temp2,char* temp3,char* temp4,char* temp5)
{
	char HTTP[150];
	sprintf(HTTP,"GET http://www.meteostation.y0.pl/GSMTemp.php?temp1=%s&temp2=%s&temp3=%s&temp4=%s&temp5=%sHTTP/1.0\r\nHost:meteostation.y0.pl",temp1,temp2,temp3,temp4,temp5);
	
	char command[20];
	sprintf(command,"AT+CIPSEND=%i\r\n",CountMark(HTTP));
		
	_delay_ms(500);
	Send(command);
	_delay_ms(100);
	Send(HTTP);
}




