/*
 * LoadData.c
 *
 * Created: 3/2/2012 6:03:54 PM
 *  Author: Jonas
 */ 

#include "defines.h" 
 
///////////////////////////////////////////////////////////////////////////////  
// 
// D e f i n i t i o n s 
// 
///////////////////////////////////////////////////////////////////////////////  
#define COL_LSN       0x00  // Set Lower Column Address 
#define COL_MSN       0x10  // Set Higher Column Address  
#define PAGE_ADD      0xB0  // Set Page Address  
 
/////////////////////////////////////////////////////////////////////////////// 
// 
// P u b l i c   F u n c t i o n s  
// 
///////////////////////////////////////////////////////////////////////////////  
/////////////////////////////////////////////////////////////////// ////////////  
// Function:  LoadData 
// Purpose:   Load a portion of display RAM with image data. 
// Inputs:    Index     -   starting position within display RAM for data (0 to   
//                       BYTES_PER_IMAGE- 1) 
//            NumBytes  -   number of by tes of image data to load  
//            pData     -   pointer to the image data to load 
// Outputs:   None. 
// Notes:     Index is adjusted to properly map into display RAM.  
//            If Vdd is off, this function turns it on and leaves it on,  
//             presuming the display routine will turn it off later. 
/////////////////////////////////////////////////////////////////////////////// 
void LoadData(uint16_t   Index, uint16_t   NumBytes, const uint8_t *pData)  
{  
  uint8_t  colAddr, page;  
  uint16_t   i; 
 
  // Enable power to display and reset driver if it's not been done already.  
  if (!VDDEnabled())  
    EnableVDD();  
 
  // Segment remapping must be done when loading data.  
  #ifdef REMAP_IMAGE  
    PutCharSPI( 0xA1, CMD_MASK );       // Remap segs.  
  #else  
      PutCharSPI( 0xA0, CMD_MASK );       // Don't remap segs.  
  #endif 
     
  // Loop through the data to write.  
  for (i = Index; i < (Index + NumBytes); i++) 
  { 
    colAddr = (i % N_COLS) + 2;         // Add two to skip unused & frame.  
 
    // Set page an d column address as needed.  
    if ((colAddr == 2) || (i == Index) )  
    { 
      page = (i/N_COLS) + 2;            // Add two to skip unused & frame. 
      PutCharSPI( (COL_MSN | (colAddr / 16)) , CMD_MASK );  
      PutCharSPI( (COL_LSN | (colAddr % 16)) ,  CMD_MASK ); 
      PutCharSPI( (PAGE_ADD | page) , CMD_MASK );  
    } 
 
    // Write a data byte.  
    PutCharSPI( *pData++, DATA_MASK );  
  }
  // NOP command is required after writing data before chip select goes high. 
  PutCharSPI( 0xE3, CMD_MASK ); 
}  
 
///////////////////////////////////////////////////////////////////////////////  
// Function:  LoadBorder 
// Purpose:   Sets the display RAM corresponding to the active frame to the 
//            given value. 
// Inputs:    border  -  0: dark, 0xFF: previous,  else: bright  
// Outputs:   None. 
// Notes:     If Vdd is off, this function turns it on and leaves it on,  
//            presuming the display routine will turn it off later.  
///////////////////////////////////////////////////////////////////////////////  
void LoadBorder(uint8_t  border)  
{
	
  static uint8_t  Previous = 0;          // Previous border setting 
  uint8_t  data, i, colAddr, page;  
 
  // Enable power to display and reset driver if it's not been done already.  
  if (!VDDEnabled())  
    EnableVDD();  
     
  // Segment remapping must be done when loading data.  
  #ifdef REMAP_IMAGE  
    PutCharSPI( 0xA1, CMD_MASK );       // Remap segs.  
  #else  
    PutCharSPI( 0xA0, CMD_MASK );       // Don't remap segs.  
  #endif 
  // Use previous border if specified.  Else, save the given border as 
  // the previous.  
  if (border == PREVIOUS_BORDER)  
    border = Previous;  
  else   
    Previous = border;  
 
  // Initialize the image memory data value for the border. 
  if (border == 0)  
    data = 0;  
  else 
    data = 0xff;  
 
  // Write the top border. 
  colAddr = 1;  
  page = 1;  
  PutCharSPI( (COL_MSN | (colAddr / 16)) , CMD_MASK );  
  PutCharSPI( (COL_LSN | (colAddr % 16)) , CMD_MASK );  
  PutCharSPI( (PAGE_ADD | page) , CMD_MASK );  
  for ( i = 0; i < N_COLS+2; i++)  
    PutCharSPI( data, DATA_MASK );  
     
  // Write the bottom border.  
  colAddr = 1;  
  page = N_PAGES + 2; 
  PutCharSPI( (COL_MSN | (colAddr / 16)) , CMD_MASK );  
  PutCharSPI( (COL_LSN | (colAddr % 16)) , CMD_MASK );  
  PutCharSPI( (PAGE_ADD | page) , CMD_MASK ); 
  for ( i = 0; i < N_COLS+2; i++) 
    PutCharSPI( data, DATA_MASK );  
	// Write the side borders for the 4 image pages. 
  for (page = 2; page < 2+N_PAGES; page++) 
  { 
    // Write the left side border.    
    colAddr = 1; 
    PutCharSPI( (COL_MSN | (colAddr / 16)) , CMD_MASK );  
    PutCharSPI( (COL_LSN | (colAddr % 16)) , CMD_MASK );  
    PutCharSPI( (PAGE_ADD | page) , CMD_MASK );  
    PutCharSPI( data, DATA_MASK );  
 
    // Write the right side border.   
    colAddr = N_COLS+2;  
    PutCharSPI( (COL_MSN | (colAddr / 16)) , CMD_MASK );  
    PutCharSPI( (COL_LSN | (colAddr % 16)) , CMD_MASK );  
    PutCharSPI( data, DATA_MASK );  
  } 
} 
