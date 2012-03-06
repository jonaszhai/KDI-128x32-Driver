/*
 * defines.h
 *
 * Created: 3/2/2012 9:57:39 AM
 *  Author: Jonas
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

// Implementation-specific definitions are given in mydefines.h. This user- 
// written file may include definitions and function prototypes for the user's 
// program. It must define the integral data types, either by including 
// <stdint.h> if it is available, or by explicitly defining the integral types 
// as in the example below. This may vary by compiler: 
// 
// typedef signed char int8_t; // 8-bit signed integer 
// typedef signed int int16_t; // 16-bit signed integer 
// typedef unsigned char uint8_t; // 8-bit unsigned integer 
// typedef unsigned int uint16_t; // 16-bit unsigned integer 
// 
// The file must also provide an appropriate definition for the following 
// token to define the number of hardware timer ticks in a millisecond. \
// 
// TICKS_PER_MS 
//

// Prototypes for functions to abstract I/O and timer hardware. These control 
// or read individual I/O lines at the display interface. The functions must 
// be implemented in a user-written file.

#define TICKS_PER_MS      (F_CPU/1000)          // Platform- specific ticks per millisecond.

#include <stdint.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>



// nRESET_DRV manipulation. 
void Assert_nRESET_DRV(void); // nRESET_DRV = 'L' 
void Deassert_nRESET_DRV(void); // nRESET_DRV = 'H'

// BUSY_DRV monitoring 
uint8_t getBUSY_DRV(void); // Read BUSY_DRV, returning 0 or 1

// Display selection/de-selection. 
void SelectDisplay(void); // nCS_DRV = 'L' void DeselectDisplay(void); // nCS_DRV = 'H'

// VDD Monitoring and control. 
uint8_t VDDEnabled(void); // Read VDD status, 1 => on, 0 => off. 
void VDDOn(void); // EN_VDD = 'H' 
void VDDOff(void); // EN_VDD = 'L'

// D_nC_DRV control. 
void Assert_D_DRV(void); // D/nC_DRV = 'H' 
void Assert_nC_DRV(void); // D/nC_DRV = 'L'

// SPI bit manipulations. 
void setSIMO(void); // SIMO = 'H' 
void clearSIMO(void); // SIMO = 'L' 
void setSCLK(void); // SCLK = 'H' 
void clearSCLK(void); // SCLK = 'L'

// Functions to zero outputs to the driver before powering it down. 
void ZeroSPILines(void); // SCLK, SIMO, D_nc_DRV = 'L' 
void ZeroDriverLines(void); // nCS_DRV, nRESET_DRV = 'L'

// Image characteristics (excluding frame). 
#define N_ROWS 32 
#define N_COLS 128 
#define BYTES_PER_IMAGE (N_ROWS*N_COLS/8) 
#define N_PAGES (N_ROWS/8) 
#define BYTES_PER_PAGE (BYTES_PER_IMAGE/N_PAGES)
#define DUMMY_ROWS			4

// Image remapping: A normal image's top is at the furthest edge from the 
// flex cable. A remapped image's top is nearest the flex cable. 
// 
// REMAP_IMAGE Image Top of image. 
// ------------ -------- --------------- 
// undefined Normal Away from flex. 
// defined Remapped Near flex. 
// 
#undef REMAP_IMAGE // Define or undefine this token for 
					// desired image orientation.
					
// Conversion from ms to ticks for input to Delay(). 
#define MS2TCK(msec) (TICKS_PER_MS * (msec))			


// Function Prototypes 
 
// display.c  
void Display(uint8_t  firstRow, uint8_t  numRows);  
 
// loaddata.c 
void LoadData(uint16_t   Index, uint16_t   NumBytes, const uint8_t *pData); 
 
// To use previous border when current one is unspecified.  For LoadBorder(); 
#define PREVIOUS_BORDER   0xFF      
 
void LoadBorder(uint8_t  border); 
 
// system.c 
 
// Tokens for selecting command byte or data byte.  For PutCharSPI(). 
#define CMD_MASK          0  
#define DATA_MASK         1  
 
void PutCharSPI(uint8_t  data, uint8_t  Data_nCmd); 
void EnableVDD(void);  
void DisableVDD(void); 
void HardResetDisplay(void); 
void Delay(uint16_t ticks);  

// Temperature measurement.  
int16_t  GetTemperature(void);



#endif /* DEFINES_H_ */