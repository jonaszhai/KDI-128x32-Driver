/*
 * display.c
 *
 * Created: 3/2/2012 10:05:32 AM
 *  Author: Jonas
 */ 

#include "defines.h"

///////////////////////////////////////////////////////////////////////////////  
// 
// D e f i n i t i o n s 
// 
///////////////////////////////////////////////////////////////////////////////  
 
// Temperature compensation data element definition.  
typedef struct  
{  
  int8_t  Temp;  
  uint8_t  VAClearPW; 
  uint8_t  VAIdle;  
  uint8_t  AAClearPW; 
  uint8_t  AAIdle;  
  uint8_t  DrivePW; 
  uint8_t  Voltage; 
} tcTableEntry; 
 
#define TEMP_PTS (sizeof(tcTable)/sizeof(tcTable[0]))   // # of table entries. 
#define TEMP_MAX ( tcTable[0].Temp+5)                    // Max update temp (C). 


///////////////////////////////////////////////////////////////////////////////  
// 
// I n t r a m o d u l a r   G l o b a l s 
// 
/////////////////////////////////////////////////////////////////////////////// 
 
// Temp Comp Data 
// 
// White/Blue KLC- 19.  
// 
const tcTableEntry tcTable[] =  
{  
  // 
  //  Temperatures are in degrees Celsius.  All other values (in hex) are   
  //  encoded for the display driver.  Within the comments, the values 
  //  are durations in milliseconds, except for voltage.  
  // 
  //  Temp  VAClear VAIdle  AAclear AAIdle  Drive   Voltage 
  //  ----  -------   ------  -------   ------  -----     -------  
  //  45    50      2       20      6       10      24  
  {   45,   0x12,   0x05,     0x0D,   0x07,   0x09,   0x40}, 
  //  ----  -------   ------  -------   ------  -----     -------  
  //  40    60      2       25      6       12      24    
  {   40,   0x13,   0x05,   0x0E,   0x07,   0x0A,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  35    80      2       30      6       14      24  
  {   35,   0x14,   0x05,   0x0F,   0x07,   0x0B,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  30    100     2       40      6       20      24  
  {   30,   0x15,   0x05,   0x11,   0x07,   0x0D,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  25    100     2       40      6       20      24      
  {   25,   0x15,   0x05,   0x11,   0x07,   0x0D,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  20    150     2       50      6       25      24  
  {   20,   0x16,   0x05,   0x12,   0x07,   0x0E,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  15    150     2       60      6       35      24  
  {   15,   0x16,   0x05,   0x13,   0x07,   0x10,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  10    250     2       100     6       50      24  
  {   10,   0x18,   0x05,   0x15,   0x07,   0x12,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  5     350     4       150     10      80      24  
  {   5,    0x19,   0x06,   0x16,   0x09,   0x14,   0x40},  
  //  ----  -------   ------  -------   ------  -----     -------  
  //  0     500     6       200     18      100     24 
  {   0,    0x1A,   0x07,   0x17,   0x0C,   0x15,   0x40} 
  //  ----  -------   ------  -------   ------  -----     -------  
}; 
 
 
 ///////////////////////////////////////////////////////////////////////////////  
// 
// P r i v a t e   F u n c t i o n s  
// 
///////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////////////////////////////////////////  
// Function: DriveImage  
// Purpose:  Updates display with image data in driver RAM using   
//            supplied drive parameters. 
// Inputs:   firstRow   -   first physical display row to update  
//           numRows    -   number of rows to update 
//           VAClearPW  -   VA Clear  Pulse width  (driver encoding) 
//           VAIdle      -   VA Idle Duration (driver encoding)  
//           AAClearPW  -   AA Clear  Pulse width  (driver encoding) 
//           AAIdle     -   AA Idle Duration (driver encoding)  
//           DrivePW    -   Scan  Pulse width  (driver encoding)   
//           ClearV     -   VA/AA Clear Voltage (driver encoding) 
//           DriveV     -   Scan Voltage (driver encoding) 
//           VAClearRpt -   Number of VA Clear Cycles  
//           VAIdleRpt  -   Number of VA Idles  
//           AAClearRpt -   Number of AA Clear Cycles  
//            AAIdleRpt  -   Number of AA Idles  
//           DriveRpt   -   Number of Driver Cycles  
//           Bias       -   Drive Voltage Bias (driver encoding)  
// Outputs:  None. 
// Notes:    Full- screen updates include additional dummy rows (DUMMY_ROWS) to 
//            let last image row see some non- select rows.  
//           Driver must already be powered with image loaded. 
///////////////////////////////////////////////////////////////////////////////  
void DriveImage(uint8_t firstRow, uint8_t numRows,  
                 uint8_t VAClearPW, uint8_t VAIdle,  
                 uint8_t AAClearPW, uint8_t AAIdle,  
                 uint8_t DrivePW, uint8_t ClearV, uint8_t DriveV, 
                 uint8_t VAClearRpt, uint8_t VAIdleRpt,  
                 uint8_t AAClearRpt, uint8_t AAIdleRpt,  
                 uint8_t DriveRpt, 
                 uint8_t Bias) 
{  
  uint8_t offset, startLineCmd; 
 
  // Map first row to first driver COMMON. 
  if ((firstRow == 0) && (numRows == 32))  // Full screen?  
  { 
    // Include  top/bottom active frame in update area.  Thus the image spans  
    // 34 COM lines, COM15 to COM48.  
    firstRow = 15;  
    numRows = 34;       
  } 
  else // Partial screen.  
  { 
    // First image row is on the COM line that is 16 inward from the edge 
    //  COM16 or COM47. 
    firstRow += 16; 
  } 
  
  
  // Compute image location parameters. 
  #ifdef REMAP_IMAGE  
    offset = 0x40  -  firstRow;     // Location on display. 
  #else  
    offset = firstRow + numRows;  // Location on display. 
    if (firstRow == 15)             // Offset needs special treatment when we  
      offset += DUMMY_ROWS;       // do a full image in non- remapped  mode.  
  #endif 
  startLineCmd = 0x40 + ((firstRow==15) ? 15 : 16); // Location in display RAM. 
   
  // Initialization code.  
  PutCharSPI( 0xA3, CMD_MASK );   // Enable band gap  and other analog control.  
  PutCharSPI( 0x18, CMD_MASK ); 
  PutCharSPI( 0xF6, CMD_MASK );   // Enable oscillator. 
  PutCharSPI( 0x40, CMD_MASK ); 
  PutCharSPI( 0xAE, CMD_MASK );   // Set auto charge pump threshold value. 
  PutCharSPI( 0x00, CMD_MASK ); 
 
  PutCharSPI( 0xA2, CMD_MASK );   // Set Bias level 
  PutCharSPI( Bias, CMD_MASK ); 
   
  // Set drive parameters. 
  PutCharSPI( 0x80, CMD_MASK );  
  PutCharSPI( 0x00, CMD_MASK ); 
  PutCharSPI( VAClearPW, CMD_MASK );  // VA Clear Duration  
  PutCharSPI( VAIdle, CMD_MASK );  // VA Idle  Duration.  Stir time. 
  PutCharSPI( AAClearPW, CMD_MASK );  // AA Clear Duration  
  PutCharSPI( AAIdle, CMD_MASK );  // AA Idle  Duration 
  PutCharSPI( DrivePW, CMD_MASK );  // Drive    Duration  
  PutCharSPI( ClearV, CMD_MASK );  // Clear  Voltage 
  PutCharSPI( DriveV, CMD_MASK );  // Drive  Voltage 
 
  // Set dummy waveform for supply initialization. 
  PutCharSPI( 0x93, CMD_MASK ); 
  PutCharSPI( 0x00, CMD_MASK );  // Skip VA Clear. 
  PutCharSPI( 0x94, CMD_MASK ); 
  PutCharSPI( 0x00, CMD_MASK );  // Skip VA Idle.  
  PutCharSPI( 0x95, CMD_MASK ); 
  PutCharSPI( 0x00, CMD_MASK );  // Skip AA Clear. 
  PutCharSPI( 0x96, CMD_MASK ); 
  PutCharSPI( 0x00, CMD_MASK );  // Skip AA Idle.  
  PutCharSPI( 0x97, CMD_MASK ); 
  PutCharSPI( 0x00, CMD_MASK );  // Skip Drive.  
 
  // Dummy update results in future supply initialization to Clear Voltage. 
  PutCharSPI(  0x31, CMD_MASK );  // Dummy update. 
  
  WaitNonBusy();  
 
  // More driver initialization code. 
  PutCharSPI( 0xA3, CMD_MASK);  // Enable other analog control.  
  PutCharSPI( 0x1A, CMD_MASK); 
  PutCharSPI( 0xE9, CMD_MASK);  // Enable bias driven.  
  PutCharSPI( 0x84, CMD_MASK);  
  PutCharSPI( 0x2F, CMD_MASK);  // Enable booster and high voltage buffer.  
 
  // Delay to allow dc/dc reach voltage.  
  // Rise time to 25V with Vin = 2.4V is 280 msec. 
  // Rise time to 25V with Vin = 3.3V is 160 msec. 
  
  Delay(MS2TCK(300)); 
  
  // Set up update. 
  PutCharSPI(       0x93, CMD_MASK ); 
  PutCharSPI( VAClearRpt, CMD_MASK );     // VA Clear Repeats  
  PutCharSPI(       0x94, CMD_MASK ); 
  PutCharSPI(  VAIdleRpt, CMD_MASK );     // VA Idle  Repeats  
  PutCharSPI(       0x95, CMD_MASK );  
  PutCharSPI( AAClearRpt, CMD_MASK );     // AA Clear Repeats  
  PutCharSPI(       0x96, CMD_MASK ); 
  PutCharSPI(  AAIdleRpt, CMD_MASK );     // AA Idle  Repeats  
  PutCharSPI(       0x97, CMD_MASK ); 
  PutCharSPI(   DriveRpt, CMD_MASK );     // Drive    Repeats  
 
  PutCharSPI( 0x32, CMD_MASK );           // Drive scheme:  
  PutCharSPI( 0x32, CMD_MASK );           // Clear to bright.  
   
  // Configure update area.  
  PutCharSPI( 0xa8, CMD_MASK ); 
  if (numRows == 34)  
    PutCharSPI( numRows+DUMMY_ROWS,  CMD_MASK ); // Mux ratio, full screen.  
  else 
    PutCharSPI( numRows, CMD_MASK );            // Mux ratio, partial screen. 
 
  PutCharSPI( 0xd3, CMD_MASK ); 
  PutCharSPI( offset, CMD_MASK );               // Physical location on screen. 
  PutCharSPI( startLineCmd, CMD_MASK );         // Starting location in RAM. 
   
  // Set AA - idle- to- drive delay to 8 msec (coincidentally, 0x08 in driver   
  // encoding).  Shorter values may cause image problems due to chip logic,   
  // especially at high operating frequenci es.  The problem manifests 
  // as one or more spurious bright lines during a partial update.   
  PutCharSPI( 0xAA, CMD_MASK );     
  PutCharSPI( 0x08, CMD_MASK );     
 
  // Set COMs to remap or not remap the image.  Segs are remapped during    
   // display RAM loading. 
 #ifdef REMAP_IMAGE  
   PutCharSPI( 0xC0, CMD_MASK );         // Scan COMs normally  
 #else  
   PutCharSPI( 0xC8, CMD_MASK );         // Scan COMs in reverse. 
 #endif 
 // Perform Update.  
 PutCharSPI( 0x31, CMD_MASK); 
 
 WaitNonBusy();  
 
  // Put display to sleep. 
  PutCharSPI( 0x2A, CMD_MASK); 
  PutCharSPI( 0xE9, CMD_MASK); 
  PutCharSPI( 0x04, CMD_MASK); 
  PutCharSPI( 0xF6, CMD_MASK); 
  PutCharSPI( 0x00, CMD_MASK); 
  PutCharSPI( 0xA3, CMD_MASK); 
  PutCharSPI( 0x00, CMD_MASK); 
  PutCharSPI( 0xAB, CMD_MASK);  
  PutCharSPI( 0x00, CMD_MASK); 
   
  // Power down display/driver. 
  DisableVDD(); 
  
} 

///////////////////////////////////////////////////////////////////////////////  
// 
// P u b l i c   F u n c t i o n s  
// 
/////////////////////////////////////////////////////////////////////////////// 
 
///////////////////////////////////////////////////////////////////////////////  
// Function: Display 
// Purpose:  Updates display with image data in driver RAM using   
//           temperature compensation table drive parameters. 
// Inputs:   firstRow -   first physical display row to update  
//           numRows  -  number of rows to update 
// Outputs:  None 
///////////////////////////////////////////////////////////////////////////////  
void Display(uint8_t  firstRow, uint8_t  numRows) 
{  
  uint8_t  VAClearPW, VAIdle, AAClearPW, AAIdle, DrivePW, ClearV;  
  uint8_t  DriveV, VAClearRpt, VAIdleRpt, AAClearRpt, AAIdleRpt, DriveRpt, Bias; 
  uint8_t  i; 
  int16_t  temperature;  
 
  // Read the temperature in degrees C. 
  temperature = GetTemperature();  
 
  // Update display if the temperature is within limits.  
  if ((temperature >= tcTable[TEMP_PTS - 1].Temp) && (temperature <= TEMP_MAX))  
  { 
    // Search TC table to find the drive parameters. 
    for (i = 0; i < TEMP_PTS; i++)  
    { 
		
      if ( temperature >= tcTable[i].Temp ) // Within range of table entry? 
      { 
		  
		  //{   20,   0x16,   0x05,   0x12,   0x07,   0x0E,   0x40},  
		 
        // Read parameters from table. 
		DrivePW    = tcTable[i].DrivePW;  
		DriveV     = tcTable[i].Voltage;  
        VAClearPW  = tcTable[i].VAClearPW;  
        VAIdle     = tcTable[i].VAIdle;      // Stir time between VA & AA clear.  
        AAClearPW  = tcTable[i].AAClearPW;  
		AAIdle     = tcTable[i].AAIdle; 
        ClearV     = DriveV; 
   
        // Set fixed parameters  
        VAClearRpt = 1; 
        VAIdleRpt  = 1; 
        AAClearRpt = 1; 
        AAIdleRpt  = 1; 
        DriveRpt   = 1; 
        Bias       = 0x02;                  // 1/7 
 
        // Display the image and break out of the search loop.  
        DriveImage( firstRow, numRows, 
                     VAClearPW, VAIdle, AAClearPW, AAIdle, 
                    DrivePW, ClearV, DriveV, 
                    VAClearRpt, VAIdleRpt,   
                    AAClearRpt, AAIdleRpt, 
                    DriveRpt, Bias);  
		
        break; 
      } 
    } 
	
  }
 
  // Turn VDD off if not already done.  This is done in DriveImage(),  
  // but that won't be called if the temperature is out of range.  
  if (VDDEnabled()) 
    DisableVDD(); 
	 
} 