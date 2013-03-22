/*
 * dht11.c
 *
 *  Created on: Mar 20, 2013
 *      Author: roxxor
 *
 *      _               ___
 *       |_____________|
 *        <- 18ms ->    <- 20-40us ->
 *
 *
 *        START: 18ms low; 20-40us high (drive)
 *        INIT: 80us low; 80us high (read)
 *
 */

#include "dht11.h"

enum state currentState;

uint8_t verifyChecksum(dht11Data_t* recvData) {
	uint8_t calcChecksum;
	calcChecksum = recvData->formated.decHumidity
			+ recvData->formated.decTemperature + recvData->formated.intHumidity
			+ recvData->formated.intTemperature;
	return calcChecksum ^ recvData->formated.checkSum; // should be 0 in case of success otherwise the wrong bits are set to 1
}

uint8_t getData(dht11Data_t* recvData) {
	DHTPOUT &= ~DHTPIN;
	currentState = start;

	// Config Timer
	TB0CCTL0 = CCIE;                         	// TRCCR3 interrupt enabled
	TB0CCR0 = 36000;							// 18ms @ 2MHz
	TB0CTL = TBSSEL__ACLK + MC_1 + TBCLR;      	// ACLK, upmode, clear TBR
	DHTPDIR |= DHTPIN;							// set to output

	__bis_SR_register(LPM0_bits + GIE);
	// Enter LPM3, enable interrupts
	__no_operation();
	__no_operation();
	__no_operation();
	//return verifyChecksum(recvData); 			// data is only valid if checkSum is OK
	return 0;
}

void initClock() {

	// ACLK = XT2/2
	UCSCTL4 |= SELA__XT2CLK;
	UCSCTL5 |= DIVA__2;

	// enable XT2
	P5SEL |= BIT2 | BIT3 | BIT4 | BIT5;

	UCSCTL6 &= ~XT2OFF;
	while (SFRIFG1 & OFIFG) {
		UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG);
		SFRIFG1 &= ~OFIFG;
	}
}

uint16_t dT = 0;
uint8_t receivedBit = 0;
uint64_t receivedData = 0;

#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER_BO_ISR() {
	enum state nextState;
	uint8_t tmp;
	tmp = (uint16_t)TB0IV;
	switch (currentState) {
	case start:
		DHTPOUT |= DHTPIN;							// drive DHT to high
		TB0CTL = 0;									// stop timer
		DHTPDIR &= ~DHTPIN;							// set to input
		DHTPSEL |= DHTPIN;							// enable CCIxA
		TB0CCTL3 = CM_2 | CAP | CCIE | CCIS_0;		// capture on falling edge
		TB0CTL = TBSSEL__ACLK | MC_2 | TBCLR;		// ACLK, cont., clear TBR
		nextState = receive;
		break;
	case receive:
		dT = TB0CCR3 - dT;
		TB0CCTL3 &= ~CCIFG;
		if (receivedBit == 0 && (320 < dT < 360)) {	// DHT Init signal
			receivedBit++;
		} else if (receivedBit == 40) {				// End signal
			TB0CTL = 0;		// stop timer
			nextState = idle;
			__bic_SR_register(LPM4_bits + GIE);
		} else {
			receivedData <<= (receivedBit - 1);
			if (230 < dT < 270) {
				receivedData |= 1;
			}
			receivedBit++;
		}
		__no_operation();
		break;

	}
	currentState = nextState;
}
