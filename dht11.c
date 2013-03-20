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
	return calcChecksum ^ receivedData->formated.checkSum;// should be 0 in case of success otherwise the wrong bits are set to 1
}
