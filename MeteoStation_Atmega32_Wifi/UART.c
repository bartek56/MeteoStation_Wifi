/*
 * UART.c
 *
 * Created: 2016-04-23 18:37:33
 *  Author: Bartek
 */

#include <avr/interrupt.h>
#include "UART.h"

volatile char bufferReceive[150];
uint8_t bufferReceiveIndex;
char* buforSend;

void ClearBufor()
{
	for(int i=0; i<150; i++)
	{
	bufferReceive[i]='\0';
	}
}

//interrupt generated after receiving the byte
ISR(USART_RXC_vect)
{
	
	bufferReceive [bufferReceiveIndex] = UDR;
	
	/* 
	0x0D, \r  CR
	0x0A, \n  LF
	0x4B  K
	0x4F  O
	*/

	if(bufferReceive[bufferReceiveIndex]=='\r' || bufferReceive[bufferReceiveIndex]=='\n')
	{
		bufferReceive[bufferReceiveIndex]=0x5F;
	}

	bufferReceiveIndex++;
}

//przerwanie generowane, gdy bufor nadawania jest ju¿ pusty
ISR(USART_UDRE_vect)
{
	static int bufferSendIndex = 0;
	
	//czy bufor nadawania nie ma ostatniego znaku
	if(buforSend[bufferSendIndex]!= 0)
	{
		//za³aduj znak do rejestru wysy³ki i ustaw indeks na nastêpny znak
		UDR = buforSend[bufferSendIndex++];
	}
	else
	{
		//osi¹gniêto koniec napisu w tablicy bufor_nad
		UCSRB &= ~(1<<UDRIE); //wy³¹cz przerwania pustego bufora nadawania
		bufferSendIndex = 0; 
	}
}

//definiowanie parametrów transmisji
void UsartInit(void)
{
	#define BAUD 19200       //wartoœæ prêdkoœæ transmisji
	#include <util/setbaud.h>
	
	//obliczone przez makro wartoœci
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	
	UCSRA &= ~(1<<U2X);
	
	//modu³ GSM wymaga 8 bitów stopu, brak bitu parzystoœci, 1 bit stopu
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); 
	//bitów danych: 8
	//bity stopu:  1
	//parzystoœæ:  brak
	
	//w³¹czenie nadajnika i odbiornika oraz przerwanie odbiornika
	UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);
}

//wysy³anie danych
void Send(char data[])
{
	buforSend=data;
	while (!(UCSRA & (1<<UDRE)));
	UCSRB |= (1<<UDRIE);
}

