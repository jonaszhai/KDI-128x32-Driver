/*
 * atmega644_defines.h
 *
 * Created: 3/2/2012 6:09:58 PM
 *  Author: Jonas
 */ 


#ifndef ATMEGA644_DEFINES_H_
#define ATMEGA644_DEFINES_H_



//#define F_CPU 8000000
#define F_CPU 10000000UL // Clock Speed

#define VDD		   PB0
#define DnC_DRV    PB1
#define nCS_DRV    PB6
#define nRESET_DRV PB3
#define UCLK       PB4
#define SIMO	   PB5
#define BUSY_DRV   PB2
#define TEST	   PA0

#define BUSY_DRV_INPUT PINB2
#define VDD_INPUT PINB0

#include "defines.h"  

void KDI_Init();

#endif /* ATMEGA644_DEFINES_H_ */