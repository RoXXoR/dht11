/*
 * dht11.c
 *
 *  Created on: Mar 20, 2013
 *      Author: roxxor
 */

#include "dht11.h"

uint8_t verifyChecksum(dht11Data_t* recvData) {
	uint8_t calcChecksum;
	calcChecksum = recvData->formated.decHumidity
			+ recvData->formated.decTemperature + recvData->formated.intHumidity
			+ recvData->formated.intTemperature;
	return calcChecksum ^ recvData->formated.checkSum;// should be 0 in case of success otherwise the wrong bits are set to 1
}

uint8_t getData(dht11Data_t* recvData) {

	return verifyChecksum(recvData); // data is only valid if checkSum is OK
}

void initPort() {

}

void initClock() {
	// enable XT2
	UCSCTL6 &= ~XT2OFF;
	while (SFRIFG1 & OFIFG){
		UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
		SFRIFG1 &= ~OFIFG;
	}

	// ACLK = XT2
	UCSCTL4 |= SELA__XT2CLK;
}


#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER_BO_ISR(){

}
