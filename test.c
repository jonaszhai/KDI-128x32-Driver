/*
 * KentDisplay_atmega644_zjn.c
 *
 * Created: 3/2/2012 9:56:56 AM
 *  Author: Jonas
 */ 

#include "atmega644_defines.h"

int main(void)
{	
	KDI_Init();
	

	/*
	uint8_t* s0 = "Hello Jason -+ Page1!";
	printPage(0, s0);
	
	uint8_t* s1 = "Hello Jason -+ Page2!***";
	printPage(1, s1);
	
	uint8_t* s2 = "Hello Jason -+ Page3!";
	printPage(2, s2);

	uint8_t* s3 = "Hello Jason";
	printPage(3, s3);
	*/	
	uint8_t* s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ0123456789 ~!@#$%^&*()_-+=[]{};:'\",.<>/?\\";
	//uint8_t* s = "$GPRMC,000125.006,A,3723.2475,N,121158.3416,W,309.62,120598,080810,W,A,N*4C";
	printFullScreen(s);
	
	for (;;) {}              // Loop forever.  
    
}
