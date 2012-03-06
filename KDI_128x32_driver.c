/*
 * KDI_128x32_driver.c
 *
 * Created: 3/3/2012 5:29:22 PM
 *  Author: Jonas
 */ 

#include "KDI_128x32_driver.h"
#include "atmega644_defines.h"

//The cursor in the display image buffer.
static uint16_t cursor;
//Display buffer.
static uint8_t imageBuffer[BYTES_PER_IMAGE];

static void getCharFont(uint8_t ch) {
  uint8_t* charFont;
  if (ch < 32) ch = 32;
  if (ch > 128) ch = 128;
  charFont = font_6x8_data + BYTES_PER_CHAR * (ch-32);	//Find the font for this char.
  for(uint8_t i = 0; i < BYTES_PER_CHAR; i++){
	  imageBuffer[cursor + i] = pgm_read_byte(charFont + i);	//Copy the font to the display buffer.
  }
}
static void initImageBuffer(){
	//Move the cursor to the beginning of display buffer.	
	cursor = 0;
	//Initialize the display buffer to all 0xFF. Set all the dots to "white".
	for(uint16_t j = 0; j < BYTES_PER_IMAGE; j++){
		imageBuffer[j] = 0xFF;
	}
}
void printFullScreen(uint8_t* str){		
	initImageBuffer();
	int numberOfCharactors = strlen((char*)str);
	if(numberOfCharactors > CHAR_PER_IMAGE){
		numberOfCharactors = CHAR_PER_IMAGE;
	}	
	
	uint8_t pageIndex = 1;
	uint8_t n_charInPage = 1;
	for(uint8_t i = 0; i < numberOfCharactors; i++){
		getCharFont(*str);
		cursor += BYTES_PER_CHAR;		
		//If the bytes left is not enough for a char, add empty bytes to these locations.
		if(n_charInPage == pageIndex * CHAR_PER_PAGE){	
			for(uint8_t j = 0; j < (BYTES_PER_PAGE % BYTES_PER_CHAR); j++){
				imageBuffer[cursor++] = 0xFF;
			}
			n_charInPage = 0;
		}
		n_charInPage++;
		str++;
	}
	
	EnableVDD();             // Power- up the display driver. 	
	HardResetDisplay();      // Reset the driver 
	
	LoadData(0, BYTES_PER_IMAGE, imageBuffer);
	
	LoadBorder(1); 
	
	Display(0, N_ROWS);
	
}


void printPage(uint8_t pageIndex, uint8_t* str){
	initImageBuffer();
	int numberOfCharactors = strlen((char*)str);
	if(numberOfCharactors > CHAR_PER_PAGE){
		numberOfCharactors = CHAR_PER_PAGE;
	}
	
	for(uint8_t i = 0; i < numberOfCharactors; i++){
		getCharFont(*str);
		cursor += BYTES_PER_CHAR;
		str++;
	}

	EnableVDD();             // Power- up the display driver. 	
	HardResetDisplay();      // Reset the driver 
	
	//The display procedure fetches data starts from 0.
	LoadData(0, BYTES_PER_PAGE, imageBuffer);
	//LoadData(pageIndex * BYTES_PER_PAGE, BYTES_PER_PAGE, pageData);
	
	LoadBorder(1); 
	
	Display(pageIndex * 8, 8);
}




