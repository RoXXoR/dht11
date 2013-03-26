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

uint16_t prevCCR = 0;
uint8_t receivedBit = 0;
uint64_t receivedData = 0;
uint8_t initReceived = 0;

void initGlobals() {
	prevCCR = 0;
	receivedBit = 0;
	receivedData = 0;
	initReceived = 0;
}


uint8_t getData(dht11Data_t* recvData) {
	DHTPOUT &= ~DHTPIN;

	initGlobals();
	// Config Timer
	TB0CCTL0 = CCIE;                         	// TRCCR3 interrupt enabled
	TB0CCR0 = 36000;							// 18ms @ 2MHz
	TB0CTL = TBSSEL__ACLK + MC_1 + TBCLR;      	// ACLK, upmode, clear TBR
	DHTPDIR |= DHTPIN;							// set to output

	__bis_SR_register(LPM0_bits + GIE);

	recvData->rawData = receivedData;
	__no_operation();
	__no_operation();
	__no_operation();
	return verifyChecksum(recvData); 			// data is only valid if checkSum is OK
}

void initClock() {

	// ACLK = XT2/2
	UCSCTL4 |= SELA__XT2CLK | SELM__XT2CLK | SELS_4;
	UCSCTL5 |= DIVA__2;

	// enable XT2
	P5SEL |= BIT2 | BIT3 | BIT4 | BIT5;

	UCSCTL6 &= ~XT2OFF;
	while (SFRIFG1 & OFIFG) {
		UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG);
		SFRIFG1 &= ~OFIFG;
	}
}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_BO_ISR() {
	DHTPOUT |= DHTPIN;							// drive DHT to high
	TB0CTL = 0;									// stop timer
	DHTPDIR &= ~DHTPIN;							// set to DHT pin to input
	DHTPSEL |= DHTPIN;							// enable CCIxA
	TB0CCTL3 = CM_2 | CAP | CCIE | CCIS_0;		// capture on falling edge
	TB0CTL = TBSSEL__ACLK | MC_2 | TBCLR;		// ACLK, cont., clear TBR
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR() {
	if (!initReceived) {	// DHT Init signal
		if (300 < (TBCCR3 - prevCCR)) {
			initReceived = 1;
		}
	} else {
		receivedData <<= 1;
		if (200 < (TBCCR3 - prevCCR)) {
			receivedData |= 1;
		}
		receivedBit++;
		if (receivedBit >= 40) {				// End signal
			TB0CTL = 0;		// stop timer
			__bic_SR_register_on_exit(LPM4_bits + GIE);
		}
	}
	prevCCR = TBCCR3;
	TB0CCTL3 &= ~CCIFG;
}
