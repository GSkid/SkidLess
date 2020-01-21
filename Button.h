/*
 * File: Button.h
 * Author: Brian Naranjo
 * Date: 1/20/20
 *
 */



/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/
#include <Arduino.h>
//#include <U8x8lib.h>
//#include "LCD_Print_Commands.h"


//#ifdef U8X8_HAVE_HW_SPI
//#include <SPI.h>
//#endif


 
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


int buttonpressdetected(void);

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/
