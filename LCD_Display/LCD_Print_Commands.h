/*
 * File: LCD_Print_Commands.h
 * Author: Brian Naranjo
 * Date: 1/18/20
 *
 */



/*******************************************************************************
 * MODULE #INCLUDES                                                             *
 ******************************************************************************/
//#include <Arduino.h>
//#include <U8x8lib.h>

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
/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/
 
/*Helper functions for printing on LCD Via PushButton, 
Button pressed is considered equivalent to 'water on'
*/

void LCD_Init(void);


/**
 * @Function print_water_on(void)
 * @param None
 * @return None
 * @brief This function prints out display for the turning on water
 * @note  
 * @author Brian Naranjo, 1/19/20
 * @editor   */
 
int LCD_print_water_on(U8X8_SH1106_128X64_NONAME_4W_HW_SPI);
/**
 * @Function print_water_off(void)
 * @param None
 * @return None
 * @brief This function prints out display for the turning off water
 * @note  
 * @author Brian Naranjo, 1/19/20
 * @editor   */

int LCD_print_water_off(U8X8_SH1106_128X64_NONAME_4W_HW_SPI);
