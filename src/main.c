#include <msp430.h> 
#include "dht11.h"
/*
 * main.c
 */
int main(void) {
	dht11Data_t bla;

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	initClock();
	while (1) {
		getData(&bla);
	}
	return 0;
}
