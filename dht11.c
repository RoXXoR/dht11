/*
 * dht11.c
 *
 *  Created on: Mar 20, 2013
 *      Author: roxxor
 */


#include "dht11.h"


uint8_t verifyChecksum(dht11Data_t* recvData){
	uint8_t calcChecksum;
	calcChecksum = recvData->formated.decHumidity + recvData->formated.decTemperature + recvData->formated.intHumidity + recvData->formated.intTemperature;
	if (calcChecksum^receivedData->formated.checkSum == 0){

	}
}
