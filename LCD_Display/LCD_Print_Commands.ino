/*
 * File: LCD_Print_Commands.c
 * Author: Brian Naranjo
 * Date: 1/18/20
 *
 */



/*******************************************************************************
 * MODULE #INCLUDES                                                             *
 ******************************************************************************/
#include "LCD_Print_Commands.h"
#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif



 
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

 
/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/
/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/
 
/*Helper functions for printing on LCD Via PushButton, 
Button pressed is considered equivalent to 'water on'
*/


//U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

/**
 * @Function print_water_on(void)
 * @param None
 * @return None
 * @brief This function prints out display for the turning on water
 * @note  
 * @author Brian Naranjo, 1/19/20
 * @editor   */

int print_water_on(void){
        Serial.println("-------------------------");
        Serial.println("Button Pressed: ON");
        Serial.println("------------------------- \n \n \n ");
        //        Serial.println("Button Count: %d");
        u8x8.drawString(2,6,"Water ON   ");

        return 0;
}

/**
 * @Function print_water_off(void)
 * @param None
 * @return None
 * @brief This function prints out display for the turning off water
 * @note  
 * @author Brian Naranjo, 1/19/20
 * @editor   */

int print_water_off(void){
        Serial.println("-------------------------");
        Serial.println("Button Pressed: OFF");
        Serial.println("------------------------- \n \n \n ");
        //        Serial.println("Button Count: %d");
        //u8x8.drawString(2,6,"Water OFF");

        return 1;
}
