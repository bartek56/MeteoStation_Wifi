/*
 * BMP085.h
 *
 *  Created on: 28 kwi 2014
 *  Autor: £ukasz £uczko
 *
 *	Modyfikacje: Bartosz Brzozowski
 *
 */

#ifndef BMP085_H_
#define BMP085_H_

#define bmo085Adr 0xEE 	// Adres I2C
#define bmp085Mode 3		// oversampling setting (0-3)

void Bmp085Init( void );	// Inicjalizacja czujnika
void Bmp085GetUTemperature(void);	// Odczyt temperatury bez kompensacji
void Bmp085GetUPressure(void);	// odczyt ciœnienia bez kompensacji
long Bmp085GetTemperature(void);		// Wylicza temperaturê
long Bmp085GetPressure(void);		// Wylicza ciœnienie
double Bmp085GetAltitude();

#endif /* BMP180_H_ */
