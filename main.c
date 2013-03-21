#include <msp430.h> 
#include "dht11.h"
/*
 * main.c
 */
int main(void) {
	volatile dht11Data_t bla;


    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    bla.rawData = 0x0123456789ABCDEF;
    initClock();

	return 0;
}
