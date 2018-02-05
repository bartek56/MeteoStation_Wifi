/*
 * DHT11.h
 *
 * Created by: Micha³ Smalera
 * 
 * Modyfikacje: Bartosz Brzozowski
 *
 *
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h> //do funkcji strcat
#include "DHT11.h"


uint8_t FetchData(uint8_t* arr)
{
	uint8_t data[5];
	uint8_t cnt, check;
	int8_t i, j;

	/******************* Inicjalizacja  *******************/

	/* Ustaw pin danych jako wyjœcie */
	DHT_DIR |= (1 << DHT_PIN);

	
	/* stan niski na 20 ms */
	DHT_PORT &= ~(1 << DHT_PIN);
	
	
	/* czekaj oko³o 20 ms */
	_delay_ms(20);
	/* Teraz ustaw Timer0 z preskalerem clk/8 */
	
	
	TCCR0 = 0; //wyzerowanie TCCR0
	TCCR0 |= (1 << WGM12); // ustawienie trybu CTC
	TCCR0 |= (1 << CS11); //prescaler 8
	TCNT0 = 0;
	/* Stan wysoki */
	DHT_PORT ^= (1 << DHT_PIN);

	/* Ustaw pin danych jako wejœcie */
	DHT_DIR &= ~(1 << DHT_PIN);

	/* Czekaj na odpowiedŸ z sensora 20-40µs */
	while (DHT_PIN_STATE & (1 << DHT_PIN)) {
		if (TCNT0 >= 120)
			return 0;    // 60us --> 
	}
	/************************* Rozpoczêcie transmisji *************************/

	TCNT0 = 0;

	/* Czekamy a¿ sensor odpowie. Najpierw stan niski ~80µs */
	while (!(DHT_PIN_STATE & (1 << DHT_PIN))) {
		if (TCNT0 >= 200)
			return 0;    //jeœli wiêcej ni¿ 100us, fail
	}

	TCNT0 = 0;

	/* Teraz stan wysoki: ~80µs */
	while (DHT_PIN_STATE & (1 << DHT_PIN)) {
		if (TCNT0 >= 200)
			return 0; //if more than 100us - fail
	}

	/********************* Przesy³ danych **********************/

	for (i = 0; i < 5; ++i) {
		for (j = 7; j >= 0; --j) {
			TCNT0 = 0;

			/* Najpierw 50µs stanu niskiego */
			while (!(DHT_PIN_STATE & (1 << DHT_PIN))) {
				if (TCNT0 >= 140)
					return 0;            //70us
			}

			TCNT0 = 0;

			/* Teraz dane. 26 to 28µs stanu wysokiego wskazuje
			 zero, a oko³o 70µs to wys³ana jedynka */

			while (DHT_PIN_STATE & (1 << DHT_PIN)) {
				if (TCNT0 >= 180)
					return 0;            //90us
			}

			/* ¯eby nie psuæ zmiennej TCNT0, skopiujemy j¹ sobie */
			cnt = TCNT0;

			if (cnt >= 40 && cnt <= 70) {       //pomiêdzy 20 i 35us
				data[i] &= ~(1 << j); //ustaw zero
			}
			else if (cnt >= 120 && cnt <= 160) { //pomiêdzy 60 and 80us
				data[i] |= (1 << j); //jedynka
			}

			else
				return 0;
		}
	}

	/********************* Zakoñczenie komunikacji, CRC *********************/

	check = (data[0] + data[1] + data[2] + data[3]) & 0xFF;

	if (check != data[4])
		return 0;

	for (i = 0; i < 4; ++i) {
		arr[i] = data[i];
	}

	return 1;
}

