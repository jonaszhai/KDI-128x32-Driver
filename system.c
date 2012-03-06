/*
 * system.c
 *
 * Created: 3/2/2012 10:13:26 AM
 *  Author: Jonas
 */ 

#include "defines.h" 

///////////////////////////////////////////////////////////////////////////////  
// Function:  PutCharSPI 
// Purpose:   Sends a byte to the SPI bus.  
// Inputs:    data -   byte to send  
//            Data_nCmd -   command (0) or data (else) attribute of byte to send  
// Outputs:   None. 
/////////////////////////////////////////////////////////////////////////////// 

void PutCharSPI(uint8_t data, uint8_t Data_nCmd) { 
	uint8_t mask; 
	
	// Assert chip select to display. 
	// The driver seems to require a new chip select to start each byte. 
	
	SelectDisplay(); // Signal byte type to driver.
	
	 // Signal byte type to driver.  
	if (Data_nCmd)
		Assert_D_DRV(); 
	else 
		Assert_nC_DRV();  
		
	// Bit- bang data out, MSB first.  Data changes on falling edge and 
  // is latched on rising.  Clock is inactive low. 
	for (mask = 0x80; mask != 0; mask = mask >> 1) 
	{ 
	
		if (data & mask)  
			setSIMO(); 
		else 
			clearSIMO();        
    
			setSCLK(); 
			clearSCLK();  
	}
	
	  // De -assert chip select to display. 
	DeselectDisplay();
}	

///////////////////////////////////////////////////////////////////////////////  
// Function:  EnableVDD  
// Purpose:   Enable power to the display. 
// Inputs:     None.  
// Outputs:   None. 
///////////////////////////////////////////////////////////////////////////////  
void EnableVDD(void) 
{  
  // Enable power to driver and temperature circuit(s). 
  VDDOn(); 
   
  // Delay ~2 msec for power to come on w/ reset still asserted low.  
  Delay(MS2TCK(2)); 
   
  // Bring logic signals to driver to active levels and release reset. 
  DeselectDisplay();  
  Deassert_nRESET_DRV(); 
}  
 
///////////////////////////////////////////////////////////////////////////////  
// Function:   DisableVDD 
// Purpose:   Remove power from the display. 
// Inputs:    None. 
// Outputs:   None. 
// Notes:     The reference design removes power by switching off a FET,  
//            essentially disconnecting the driver from its supply voltage. 
//            So the code below sets all interface and control lines to the  
//            driver to zero in order to avoid sourcing current through 
//            the interface lines.  In designs which do not remove power,   
//            relying instead on the driver ¡¯s lower power sleep mode, do not 
//            assert the reset or chip select outputs, as doing so will cause 
//            extra current demand. 
///////////////////////////////////////////////////////////////////////////////  
void DisableVDD(void)  
{  
  //  Set driver interface signals low. 
  ZeroDriverLines();  
 
  // Set driver SPI communications signals low.  
  ZeroSPILines(); 
   
  // Cut power to driver and temp circuits.  
  VDDOff();  
 
  Delay(MS2TCK(25));  
}  


///////////////////////////////////////////////////////////////////////////////  
// Function:  HardResetDisplay  
// Purpose:   Asserts the reset line to the display.  Systems which leave 
//            the driver powered between updates should perform a hard reset  
//            before a data load / update cycle. 
// Inputs:    None. 
// Outputs:   None. 
///////////////////////////////////////////////////////////////////////////////  
void HardResetDisplay(void)  
{  
  // Assert reset line.  
  Assert_nRESET_DRV(); 
   
    // Delay ~2 msec (only 20 uS req'd).  
  Delay(MS2TCK(2)); 
   
  // De -assert reset line.  
  Deassert_nRESET_DRV(); 
} 