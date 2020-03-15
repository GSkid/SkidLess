/*
   File: SolenoidButtonControl.ino
   Author: Brian Naranjo
   Date: 1/18/20

*/


/*******************************************************************************
  MODULE #INCLUDES
******************************************************************************/

//
//#include <U8g2lib.h>
//#include <U8x8lib.h>
////
//#include <U8g2_for_Adafruit_GFX.h>
//#include <u8g2_fonts.h>

//#include <U8g2lib.h>
//#include <U8x8lib.h>


//#include <Arduino.h>

#include "cppsrc/U8x8lib.h"

//#include "LCD_Print_Commands.h"
//#include "Button_Functions.h"

//#include <csrc/U8g2.h>

//#include <U8g2.h>

/*
#ifdef U8X8_HAVE_HW_SPI
#define U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
*/
#include <cstdio>


/*******************************************************************************
   MODULE #DEFINES
 ******************************************************************************/
#define TRUE 1
#define FALSE 0

#define UP 1
#define DOWN 0


/*******************************************************************************
   PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
///**

/*
int Timer(uint32_t, uint32_t);
*/
void setup(void);

/**************************************************************************
  *****************Set Up of 8X8 Sceen. Do not MOVE***********************/
U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

/************************************************************************/

//Setup Pins and Serial Monitor

void setup(void) {
  // put your setup code here, to run once:

  //bcm2835_init();
  
  //bcm2835_spi_begin();
  //bcm2835_spi_set_speed_hz(SPI_SPEED_2MHZ);

  //Serial.begin(115200);

   
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  //LCD_Welcome();

	return;
}


int main(void) {
  // put your main code here, to run repeatedly:

  //Check input of Button.
	setup();
  


  //UI State Machine

  printf("Hello World \r\n");
  
  while(1);


  // delay(20);
  return 0;
}



/*******************************************************************************
   PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
 /*
int Timer(uint32_t delayThresh, uint32_t prevDelay) {
  // Checks if the current time is at or beyond the set timer
  if ((bcm2835_millis() - prevDelay) >= delayThresh) {
    return 1;
  } else if (millis() < prevDelay) {
    //Checks and responds to overflow of the millis() timer
    if (((4294967296 - prevDelay) + bcm2835_millis()) >= delayThresh) {
      return 1;
    }
  }
  return 0;
}
*/
