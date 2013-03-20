/*
 * dht11.h
 *
 *  Created on: Mar 20, 2013
 *      Author: roxxor
 */

#ifndef DHT11_H_
#define DHT11_H_

#include <stdint.h>

typedef union {
	uint64_t rawData;
	struct {
		uint8_t checkSum;
		uint8_t decTemperature;
		uint8_t intTemperature;
		uint8_t decHumidity;
		uint8_t intHumidity;
	} formated;
} dht11Data_t;

#endif /* DHT11_H_ */
