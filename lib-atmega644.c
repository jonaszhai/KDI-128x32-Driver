/*
 * sysconfig_atmega168.c
 *
 * Created: 2/29/2012 Wednesday 11:32:48 PM
 *  Author: Heyang
 */ 

#include "atmega644_defines.h"




/*
typedef signed char int8_t;        // signed 8- bit data type 
typedef signed int  int16_t ;       // signed 16- bit data type 
typedef unsigned char uint8_t;     // unsigned 8- bit data type 
typedef unsigned int  uint16_t ;    //   unsigned 16- bit data type
*/

//void Delay(uint16_t ticks);


void KDI_Init(){
	DDRA = 0b11111111;
	PORTA = 0b00000000;
	
	DDRB = 0b11111011;
	PORTB = 0b01000100;
}

void Delay(uint16_t ticks)
{
	uint16_t counter = ticks/TICKS_PER_MS;
	while(counter>0){
		_delay_ms(1);
		counter--;
	}
}


uint8_t isr_flag;
void WaitNonBusy(void)
{
	cli();
		
	EIMSK |= _BV(INT2);
	EICRA |= _BV(ISC21); //Trigger on falling edge of INT0
	EICRA &= ~(1<<ISC20);
    sei();
	
	isr_flag = 1;
	while(isr_flag){
		_delay_ms(1);		
	}
}

ISR(INT2_vect)
{
	// Stop waiting
	isr_flag = 0;
}

void Assert_nRESET_DRV(void){
	PORTB &= ~(1<<nRESET_DRV);
}

void Deassert_nRESET_DRV(void){
	PORTB |= (1<<nRESET_DRV);
}

uint8_t getBUSY_DRV(void){	
	if (BUSY_DRV_INPUT>0){
		return 1;
	}
	else
		return 0;
}

void SelectDisplay(void){
	PORTB &= ~(1 << nCS_DRV);
}

void DeselectDisplay(void){
	PORTB |= (1 << nCS_DRV);
}

uint8_t VDDEnabled(void){
	if (VDD_INPUT>0){
		return 1;
	}
	else{
		return 0;
	}		
}

void VDDOn(void){
	PORTB |= (1 << VDD);
}

void VDDOff(void){
	PORTB &= ~(1 << VDD);
}


void Assert_D_DRV(void){
	PORTB |= (1 << DnC_DRV);
}

void Assert_nC_DRV(void){
	PORTB &= ~(1 << DnC_DRV);
}

void setSIMO(void){
	PORTB |= (1 << SIMO);
}

void clearSIMO(void){
	PORTB &= ~(1 << SIMO);
}

void setSCLK(void){
	PORTB |= (1 << UCLK);
}

void clearSCLK(void){
	PORTB &= ~(1 << UCLK);
}

void ZeroSPILines(void){
	PORTB &= ~(1 << UCLK);
	PORTB &= ~(1 << SIMO);
	PORTB &= ~(1 << DnC_DRV);
}

void ZeroDriverLines(void){
	PORTB &= ~(1 << nCS_DRV);
	PORTB &= ~(1 << nRESET_DRV);
}


int16_t  GetTemperature(void){
	return 10;
}

