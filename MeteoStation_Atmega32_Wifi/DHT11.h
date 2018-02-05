/*
 * DHT11.h
 *
 * Created by: Micha³ Smalera
 * 
 * Modyfikacje: Bartosz Brzozowski
 *
 *
 */ 

#ifndef DHT11_H_
#define DHT11_H_

#define DHT_PORT 			PORTD
#define DHT_PIN 			PD6
#define DHT_DIR 			DDRD
#define DHT_PIN_STATE 		PIND

uint8_t FetchData(uint8_t* arr);


#endif

