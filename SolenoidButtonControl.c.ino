//Brian Naranjo
//1/18/20

//#include <U8g2lib.h>
//#include <U8x8lib.h>
//
//#include <U8g2_for_Adafruit_GFX.h>
//#include <u8g2_fonts.h>
//
//#include <U8g2lib.h>
//#include <U8x8lib.h>


#include <Arduino.h>
#include <U8x8lib.h>
#include "LCD_Print_Commands.h"
#include "Button_Functions.h"
//#include "Button.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif



/*******************************************************************************
   MODULE #DEFINES
 ******************************************************************************/
#define TRUE 1
#define FALSE 0


/**************************************************************************
  Set Up of 8X8 Sceen. Do not MOVE
************************************************************************/

U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

//Initialize Variables
static int solenoidOutput = 3;
static int buttonInput = 4;



static int lastButtonState, buttonValue, currentFunction,
       debounceButtonValue, BUTTON_PRESSED  = 0;

typedef enum {
  WELCOME_PAGE,
  Page_1,
  Page_2,
  Page_3,
  Page_4,
  Page_5,
  Page_6,
  Page_7,
  Page_8,
  Page_9,
  Page_10,
  Page_11,
  Page_12,
} UI_States;

static UI_States currentState = WELCOME_PAGE;

//Setup Pins and Serial Monitor

void setup() {
  // put your setup code here, to run once:
  pinMode(solenoidOutput, OUTPUT);
  pinMode(buttonInput, INPUT);

  Serial.begin(9600);

  lastButtonState = digitalRead(buttonInput); //Read initial value
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);



  //declare static variables

  //  static char BUTTON_PRESSED = FALSE;

  u8x8.drawString(0, 0, "--------------");
  u8x8.drawString(2, 1, "Welcome to");
  u8x8.drawString(0, 2, "Intuitive Auto");
  u8x8.drawString(2, 3, "Irrigation");
  u8x8.drawString(0, 4, "--------------");


}

void loop() {
  // put your main code here, to run repeatedly:
  buttonValue = digitalRead(buttonInput);
  delay(20);        //to check for unsteady signals
  debounceButtonValue = digitalRead(buttonInput);

  //Check input of Button.

  /**************************Want to Replace w helper functs

   *****************************************************/

  if (buttonValue == debounceButtonValue) { //Steady Signal
    if (buttonValue != lastButtonState) { //Change in State
      if (buttonValue == LOW) { //Flipped, Low is pressed
        BUTTON_PRESSED = TRUE; //set flag TRUE

        digitalWrite(solenoidOutput, HIGH);
        //        u8x8.drawString(0, 6, "Button: Pressed      =");

      } else {
        BUTTON_PRESSED = FALSE; //set flag FALSE
        digitalWrite(solenoidOutput, LOW);
        //        u8x8.drawString(0, 6, "Button: Not Pressed    ");

      }


    }
    lastButtonState = buttonValue;
  }

  /***************************************************
  *******************************************************/
  //  if (button_press_detected(buttonValue,debounceButtonValue)){
  //    currentFunction = print_water_on();
  //    digitalWrite(solenoidOutput,HIGH);
  //  } else {
  //    currentFunction = print_water_off();
  //    digitalWrite(solenoidOutput,LOW);
  //  }



  //UI State Machine



  switch (currentState) {
    case WELCOME_PAGE:
      if (BUTTON_PRESSED) {
        currentState = Page_1;
        u8x8.drawString(4, 7, "Page 1");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_1:
      if (BUTTON_PRESSED) {
        currentState = Page_2;
        u8x8.drawString(4, 7, "Page 2");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_2:
      if (BUTTON_PRESSED) {
        currentState = Page_3;
        u8x8.drawString(4, 7, "Page 3");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_3:
      if (BUTTON_PRESSED) {
        currentState = Page_4;
        u8x8.drawString(4, 7, "Page 4");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_4:
      if (BUTTON_PRESSED) {
        currentState = Page_5;
        u8x8.drawString(4, 7, "Page 5");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_5:
      if (BUTTON_PRESSED) {
        currentState = Page_6;
        u8x8.drawString(4, 7, "Page 6");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_6:
      if (BUTTON_PRESSED) {
        currentState = Page_7;
        u8x8.drawString(4, 7, "Page 7");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_7:
      if (BUTTON_PRESSED) {
        currentState = Page_8;
        u8x8.drawString(4, 7, "Page 8");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_8:
      if (BUTTON_PRESSED) {
        currentState = Page_9;
        u8x8.drawString(4, 7, "Page 9");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_9:
      if (BUTTON_PRESSED) {
        currentState = Page_10;
        u8x8.drawString(4, 7, "Page 10");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_10:
      if (BUTTON_PRESSED) {
        currentState = Page_11;
        u8x8.drawString(4, 7, "Page 11");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_11:
      if (BUTTON_PRESSED) {
        currentState = Page_12;
        u8x8.drawString(4, 7, "Page 12");
        BUTTON_PRESSED = FALSE;
      }
      break;
      
    case Page_12:
      if (BUTTON_PRESSED) {
        currentState = WELCOME_PAGE;
        u8x8.drawString(0, 6, "              ");
        u8x8.drawString(0, 7, "              ");
        BUTTON_PRESSED = FALSE;
      }
      break;

  }


  delay(20);

}