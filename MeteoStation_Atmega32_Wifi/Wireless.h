/*
 * Wireless.h
 *
 * Created: 2017-04-08 15:24:36
 *  Author: Bartek
 */ 


#ifndef WIRELESS_H_
#define WIRELESS_H_

void WirelessWebSite();
void WirelessGetTemp(char *temp1,char *temp2,char *temp3, char *temp4, char *temp5);
void WirelessGetOther(char *pressure, int humidity, int rainFlag, int lightState);
void ConnectToNewNetwork(char* name, char* password);
char *CheckStatus();
uint8_t wirelessReady;


#endif /* WIRELESS_H_ */