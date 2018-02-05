/*
 * ADC.h
 *
 * Created: 2016-09-21 10:35:23
 * Author : Bartosz Brzozowski
 */ 

#ifndef ADC_H_
#define ADC_H_


extern int rainFlag;
extern int lightState;

void AdcInit();
int Survey(int channel);
char* Light();

#endif /* ADC_H_ */
