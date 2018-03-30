/*
 * main.c
 *
 * Created: 2016-09-21 10:35:23
 * Author : Bartosz Brzozowski
 */ 

#ifndef DS3231_H_
#define DS3231_H_


#define DS3231_ADDR 0xD0
#define _24C04_ADDR 0xA8

#define eventsSize 16
#define eventsMask (eventsSize-1)



extern volatile int eventHead;
extern volatile int eventTail;
extern volatile int WirelessSet;

char* getLastEvent();
int checkEvent();
void checkClock();
void newEvent(char* event);
int DecToBcd(uint8_t dec);
int BcdToDec(uint8_t bcd);
void ShowClock();
void SetClock(uint8_t godz, uint8_t min, uint8_t sek);
void SetClockOnlyHour(uint8_t hr);
void SetDate(uint8_t weekDay, uint8_t day, uint8_t month, uint8_t year);
void ShowDate();


#endif /* UART_H_ */
