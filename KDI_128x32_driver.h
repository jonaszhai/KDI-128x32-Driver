/*
 * KDI_128x32_driver.h
 *
 * Created: 3/3/2012 5:25:06 PM
 *  Author: Jonas
 */ 


#ifndef KDI_128X32_DRIVER_H_
#define KDI_128X32_DRIVER_H_

#include "defines.h"
#include "font.h"

#define CHAR_PER_PAGE (BYTES_PER_PAGE/BYTES_PER_CHAR)
#define CHAR_PER_IMAGE (CHAR_PER_PAGE*N_PAGES)

/************************************************************************/
/* Print all 4 pages.                                                   */
/************************************************************************/
void printFullScreen(uint8_t* str);

/************************************************************************/
/* Print a page.                                                         */
/************************************************************************/
void printPage(uint8_t pageIndex, uint8_t* str);



#endif /* KDI_128X32_DRIVER_H_ */