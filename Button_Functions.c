/*
 * File: Button.c
 * Author: Brian Naranjo
 * Date: 1/20/20
 *
 */



/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/
#include <Arduino.h>
//#include "Button_Functions.h"
//#include <U8x8lib.h>
//#include "LCD_Print_Commands.h"


//#ifdef U8X8_HAVE_HW_SPI
//#include <SPI.h>
//#endif

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define TRUE 1
#define FALSE 0
 
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

 
/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/*Helper functions for checking button presses for user interface
*/





/**
 * @Function button_press_detected(void)
 * @param buttonValue. Taken from input pin , converted to int value. 
 * @return TRUE if pressed. FALSE if not. 
 * @brief This function checks to see if button press is detected
 * @note  
 * @author Brian Naranjo, 1/20/20
 * @editor   */


static int lastState = FALSE;



int button_press_detected(int butVal,int debounceButVal){
  
  int PRESSED_BUTTON = 0;

  if(butVal == debounceButVal){ //Steady Signal
    if(butVal != lastState){ //Change in State
      if(butVal == LOW){ //Flipped, Low is pressed
        PRESSED_BUTTON = TRUE; //set flag TRUE
       
      } else {
 
        PRESSED_BUTTON = FALSE; //set flag FALSE
      }
    
    }
    lastState = butVal;
  }
  return PRESSED_BUTTON;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/
 
