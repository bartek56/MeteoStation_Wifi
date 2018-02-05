/*
 * UART.h
 *
 * Created: 2016-04-23 18:36:03
 *  Author: Bartek
 */ 

#ifndef UART_H_
#define UART_H_

extern volatile char bufferReceive[150];
extern uint8_t bufferReceiveIndex;

void UsartInit(void);
void Send(char data[]);
void ClearBufor();

#endif /* UART_H_ */
