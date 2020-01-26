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


#include <Arduino.h>
#include <U8x8lib.h>
//#include "LCD_Print_Commands.h"
//#include "Button_Functions.h"


#ifdef U8X8_HAVE_HW_SPI
#define U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif



/*******************************************************************************
   MODULE #DEFINES
 ******************************************************************************/
#define TRUE 1
#define FALSE 0


/*******************************************************************************
   PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
/**
   @Function checkButtons(void)
   @param None
   @return None
   @brief This function checksButtons and sets appropriate flag
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void checkButtons(void);

/**
   @Function LCD_Init(void)
   @param None
   @return None
   @brief This function sets up 8x8 LCD display
   @note
   @author Brian Naranjo, 1/19/20
   @editor   */

void LCD_Init(void);


/**
   @Function LCD_HomePage(void)
   @param None
   @return None
   @brief This function prints Home page on LCD screen
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_HomePage(void);


/**
   @Function LCD_PrintArrow(int x, int y)
   @param None
   @return None
   @brief This function prints arrow in desired x,y coordinate
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */
void LCD_PrintArrow(int x, int y);

/**
   @Function LCD_HoseSetup(int state)
   @param int state, Used to determine position of arrow
   @return None
   @brief This function prints Hose Setup Page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_HoseSetup(int state);

/**
   @Function LCD_Options(int state)
   @param int state, Used to determine position of arrow
   @return None
   @brief This function prints Options Page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_Options(int state);


/**
   @Function LCD_Sleep(int state)
   @param int state, Used to determine position of arrow
   @return None
   @brief This function prints Sleep Page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_Sleep(int state);

/**
   @Function LCD_print_water_on(void)
   @param None
   @return None
   @brief This function prints out display for the turning on water
   @note
   @author Brian Naranjo, 1/19/20
   @editor   */

void LCD_print_water_on(void);

/**
   @Function LCD_print_water_off(void)
   @param None
   @return None
   @brief This function prints out display for the turning off water
   @note
   @author Brian Naranjo, 1/19/20
   @editor   */
void LCD_print_water_off(void);

///**
//   @Function int button_press_detected(int x, int y)
//   @param buttonValue. Taken from input pin , converted to int value.
//   @return TRUE if pressed. FALSE if not.
//   @brief This function checks to see if button press is detected
//   @note
//   @author Brian Naranjo, 1/20/20
//   @editor   */
//
//int button_press_detected(int butVal, int debounceButVal);



/**************************************************************************
  *****************Set Up of 8X8 Sceen. Do not MOVE***********************/
U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

/************************************************************************/









//Initialize Variables
//static int solenoidOutput = 3;
static int buttonInput = 4;
static int downButtonInput = 3;



static int lastButtonState, lastDownButtonState, buttonValue,
       downButtonValue, BUTTON_PRESSED, DOWN_PRESSED  = 0;

static int LCD_x, LCD_y, arrowState = 0;




typedef enum {
  WELCOME_PAGE,
  HOME_PAGE,
  SENSOR_DATA,
  SENSOR_SINGLE,
  SENSOR_MOISTURE,
  SENSOR_SUNLIGHT,
  SENSOR_WEATHER,
  HOSE_SETUP,
  OPTIONS,
  SLEEP,
  Page_10,
  Page_11,
  Page_12,
} UI_States;

static UI_States currentPage = WELCOME_PAGE;

//Setup Pins and Serial Monitor

void setup() {
  // put your setup code here, to run once:
  //  pinMode(solenoidOutput, OUTPUT);
  pinMode(buttonInput, INPUT);
  pinMode(downButtonInput, INPUT);

  Serial.begin(115200);

  lastButtonState = digitalRead(buttonInput); //Read initial value
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  LCD_Welcome();
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonValue = digitalRead(buttonInput);
  //  delay(20);        //to check for unsteady signals
  downButtonValue = digitalRead(downButtonInput);

  //Check input of Button.

  /**************************Want to Replace w helper functs

   *****************************************************/
  //
  //  if (buttonValue == debounceButtonValue) { //Steady Signal
  //    if (buttonValue != lastButtonState) { //Change in State
  //      if (buttonValue == LOW) { //Flipped, Low is pressed
  //        BUTTON_PRESSED = TRUE; //set flag TRUE
  //
  ////        digitalWrite(solenoidOutput, HIGH);
  //        //        u8x8.drawString(0, 6, "Button: Pressed      =");
  //
  //      } else {
  //        BUTTON_PRESSED = FALSE; //set flag FALSE
  ////        digitalWrite(solenoidOutput, LOW);
  //        //        u8x8.drawString(0, 6, "Button: Not Pressed    ");
  //
  //      }
  //
  //
  //    }
  //    lastButtonState = buttonValue;
  //  }

  checkButtons();

  /***************************************************
  *******************************************************/


  //UI State Machine



  switch (currentPage) {
    case WELCOME_PAGE:
      if (BUTTON_PRESSED) {
        currentPage = HOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_HomePage(arrowState); //Start Home Page with cursor at first line
        LCD_PrintArrow(LCD_x, LCD_y);
        BUTTON_PRESSED = FALSE;
      }
      break;

    case HOME_PAGE:
      if (DOWN_PRESSED) {
        if (arrowState >= 3) {
          arrowState = 0;
        } else {
          arrowState++;
        }
        u8x8.clearDisplay();
        LCD_HomePage(arrowState); //Start Home Page with cursor at first line
        LCD_PrintArrow(LCD_x, LCD_y);
        DOWN_PRESSED = FALSE;
        //        BUTTON_PRESSED = FALSE;
      } else if (BUTTON_PRESSED) {
        //        currentPage = SENSOR_DATA;
        //        arrowState = 0;
        //        u8x8.clearDisplay();
        //        LCD_SensorData(arrowState); //Start Home Page with cursor at first line
        //        LCD_PrintArrow(LCD_x, LCD_y);
        //        BUTTON_PRESSED = FALSE;

        u8x8.clearDisplay(); //To allow for new print

        if (arrowState == 0) {
          currentPage = SENSOR_DATA;
          arrowState = 0;
          LCD_SensorData(arrowState); //Start Home Page with cursor at first line
          LCD_PrintArrow(LCD_x, LCD_y);
        } else if (arrowState == 1) {
          currentPage = HOSE_SETUP;
          arrowState = 0;
          LCD_HoseSetup(arrowState); //Start Home Page with cursor at first line
          LCD_PrintArrow(LCD_x, LCD_y);

        } else if (arrowState == 2) {
          currentPage = OPTIONS;
          arrowState = 0;
          LCD_Options(arrowState); //Start Home Page with cursor at first line
          LCD_PrintArrow(LCD_x, LCD_y);

        } else {
          currentPage = SLEEP;
          arrowState = 0;
          LCD_Sleep(arrowState); //Start Home Page with cursor at first line
          LCD_PrintArrow(LCD_x, LCD_y);
        }
        arrowState = 0;
        BUTTON_PRESSED = FALSE;
        
      }
      //        currentState = Page_2;
      //        u8x8.drawString(4, 7, "Page 2");
      //        BUTTON_PRESSED = FALSE;
      //      }else if(DOWN_PRESSED){
      //       u8x8.clearDisplay();
      //       LCD_HomePage(0); //Start Home Page with cursor at first line
      //       LCD_PrintArrow(LCD_x,LCD_y);
      //      }
      break;

    case SENSOR_DATA:
      if (DOWN_PRESSED) {
        if (arrowState >= 3) {
          arrowState = 0;
        } else {
          arrowState++;
        }
        u8x8.clearDisplay();
        LCD_SensorData(arrowState); //Start Home Page with cursor at first line
        LCD_PrintArrow(LCD_x, LCD_y);
        DOWN_PRESSED =  FALSE;
      } else if (BUTTON_PRESSED) {
        u8x8.clearDisplay();
        if (arrowState == 0) {
          currentPage = SENSOR_SINGLE;
          u8x8.drawString(1, 7, "SINGLE SENSOR");

        } else if (arrowState == 1) {
          u8x8.drawString(4, 7, "MOISTURE");
          currentPage = SENSOR_MOISTURE;

        } else if (arrowState == 2) {
          u8x8.drawString(4, 7, "SUNLIGHT");
          currentPage = SENSOR_SUNLIGHT;

        } else {
          u8x8.drawString(4, 7, "WEATHER");
          currentPage = SENSOR_WEATHER;
        }

        BUTTON_PRESSED = FALSE;
        arrowState = 0;
      }
      break;

    case SENSOR_SINGLE:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_Welcome();
        //        u8x8.drawString(4, 7, "Page 4");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case SENSOR_MOISTURE:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_Welcome();
        //        u8x8.drawString(4, 7, "Page 5");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case SENSOR_SUNLIGHT:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_Welcome();
        //        u8x8.drawString(4, 7, "Page 6");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case SENSOR_WEATHER:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_Welcome();
        //        u8x8.drawString(4, 7, "Page 7");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case HOSE_SETUP:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_Welcome();
        BUTTON_PRESSED = FALSE;
      }
      break;

    case OPTIONS:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_Welcome();
        BUTTON_PRESSED = FALSE;
      }
      break;

    case SLEEP:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        arrowState = 0;
        u8x8.clearDisplay();
        LCD_Welcome();
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_10:
      if (BUTTON_PRESSED) {
        currentPage = Page_11;
        u8x8.drawString(4, 7, "Page 11");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_11:
      if (BUTTON_PRESSED) {
        currentPage = Page_12;
        u8x8.drawString(4, 7, "Page 12");
        BUTTON_PRESSED = FALSE;
      }
      break;

    case Page_12:
      if (BUTTON_PRESSED) {
        currentPage = WELCOME_PAGE;
        u8x8.clearDisplay();
        BUTTON_PRESSED = FALSE;
      }
      break;

  }


  delay(20);

}



/*******************************************************************************
   PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/
/**
   @Function checkButtons(void)
   @param None
   @return None
   @brief This function checksButtons and sets appropriate flag
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void checkButtons(void) {
  if (buttonValue != lastButtonState) { //Change in State
    if (buttonValue == HIGH) { //Flipped, Low is pressed
      BUTTON_PRESSED = TRUE; //set flag TRUE
    } else {
      BUTTON_PRESSED = FALSE; //set flag FALSE
    }
    lastButtonState = buttonValue;
  } else if (downButtonValue != lastDownButtonState) { //Change in State
    if (downButtonValue == HIGH) { //Flipped, Low is pressed
      DOWN_PRESSED = TRUE; //set flag TRUE
    } else {
      DOWN_PRESSED = FALSE; //set flag FALSE
    }
    lastDownButtonState = downButtonValue;
  }

}

/**
   @Function LCD_Init(void)
   @param None
   @return None
   @brief This function sets up 8x8 LCD display
   @note
   @author Brian Naranjo, 1/19/20
   @editor   */

void LCD_Init(void) {
  //Serial.begin(115200);
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

/**
   @Function LCD_Welcome(void)
   @param None
   @return None
   @brief This function sprints Welcome Screen
   @note
   @author Brian Naranjo, 1/19/20
   @editor   */

void LCD_Welcome(void) {
  u8x8.drawString(0, 0, "--------------");
  u8x8.drawString(2, 1, "Welcome to");
  u8x8.drawString(0, 2, "Intuitive Auto");
  u8x8.drawString(2, 3, "Irrigation");
  u8x8.drawString(0, 4, "--------------");
}


/**
   @Function LCD_PrintArrow(int state)
   @param int x, int y Used to determine x,y position of arrow
   @return None
   @brief This function prints Arrow on LCD at x,y coordinates
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */
   
void LCD_PrintArrow(int x, int y) {
  u8x8.drawString(x, y, "<");
}


/**
   @Function LCD_HomePage(int state)
   @param int state, Used to determine what state screen is on.
   @return None
   @brief This function prints Home page on LCD screen
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_HomePage(int state) {
  u8x8.drawString(4, 0, "Home Page");
  u8x8.drawString(0, 2, "Sensor Data");
  u8x8.drawString(0, 3, "Hose Setup");
  u8x8.drawString(0, 4, "Options");
  u8x8.drawString(0, 5, "Sleep");

  //determines what row to print arrow on
  if (state == 0) {
    LCD_x = 12;
    LCD_y = 2;
  } else if (state == 1) {
    LCD_x = 11;
    LCD_y = 3;
  } else if (state == 2) {
    LCD_x = 8;
    LCD_y = 4;
  } else {
    LCD_x = 6;
    LCD_y = 5;
  }
}

/**
   @Function LCD_SensorData(int state)
   @param int state, Used to determine position of arrow
   @return None
   @brief This function prints sensor data page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_SensorData(int state) {
  u8x8.drawString(2, 0, "Sensor Data");
  u8x8.drawString(0, 2, "Single Sensor");
  u8x8.drawString(0, 3, "Moisture");
  u8x8.drawString(0, 4, "Sunlight");
  u8x8.drawString(0, 5, "Weather");

  //determines what row to print arrow on
  if (state == 0) {
    LCD_x = 14;
    LCD_y = 2;
  } else if (state == 1) {
    LCD_x = 10;
    LCD_y = 3;
  } else if (state == 2) {
    LCD_x = 9;
    LCD_y = 4;
  } else {
    LCD_x = 8;
    LCD_y = 5;
  }
}


/**
   @Function LCD_HoseSetup(int state)
   @param int state, Used to determine position of arrow
   @return None
   @brief This function prints Hose Setup Page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_HoseSetup(int state) {
  u8x8.drawString(2, 0, "Hose Setup");
  u8x8.drawString(0, 2, "Current Hoses");
  u8x8.drawString(0, 3, "Add/Remove Hose");

  //determines what row to print arrow on
  if (state == 0) {
    LCD_x = 11;
    LCD_y = 0;
  } else if (state) {
    LCD_x = 3;
    LCD_y = 3;
  } else if (state == 2) {
    LCD_x = 3;
    LCD_y = 4;
  } else {
    LCD_x = 3;
    LCD_y = 5;
  }
}

/**
   @Function LCD_Options(int state)
   @param int state, Used to determine position of arrow
   @return None
   @brief This function prints Options Page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_Options(int state) {
  u8x8.drawString(4, 0, "Options");

  //determines what row to print arrow on
  if (state == 0) {
     LCD_x = 11;
    LCD_y = 0;
  } else if (state == 1) {
    LCD_x = 1;
    LCD_y = 3;
  } else if (state == 2) {
    LCD_x = 1;
    LCD_y = 4;
  } else {
    LCD_x = 1;
    LCD_y = 5;
  }
}


/**
   @Function LCD_Sleep(int state)
   @param int state, Used to determine position of arrow
   @return None
   @brief This function prints Sleep Page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_Sleep(int state) {
  u8x8.drawString(4, 0, "Sleep");
  

  //determines what row to print arrow on
  if (state == 0) {
    LCD_x = 11;
    LCD_y = 0;
  } else if (state == 1) {
    LCD_x = 6;
    LCD_y = 3;
  } else if (state == 2) {
    LCD_x = 6;
    LCD_y = 4;
  } else {
    LCD_x = 6;
    LCD_y = 5;
  }
}


/**
   @Function LCD_print_water_on(void)
   @param None
   @return None
   @brief This function prints out display for the turning on water
   @note
   @author Brian Naranjo, 1/19/20
   @editor   */

void LCD_print_water_on(void) {
  Serial.println("-------------------------");
  Serial.println("Button Pressed: ON");
  Serial.println("------------------------- \n \n \n ");
  Serial.println("Button Count: %d");
  u8x8.drawString(2, 6, "Water ON   ");
}

/**
   @Function LCD_print_water_off(void)
   @param None
   @return None
   @brief This function prints out display for the turning off water
   @note
   @author Brian Naranjo, 1/19/20
   @editor   */
void LCD_print_water_off(void) {
  Serial.println("-------------------------");
  Serial.println("Button Pressed: OFF");
  Serial.println("------------------------- \n \n \n ");
  Serial.println("Button Count: %d");
  u8x8.drawString(2, 6, "Water ON   ");
}

//int button_press_detected(int butVal, int debounceButVal) {
//  int PRESSED_BUTTON = 0;
//
//  if (butVal == debounceButVal) { //Steady Signal
//    if (butVal != lastButtonState) { //Change in State
//      if (butVal == HIGH) { //Flipped, Low is pressed
//        PRESSED_BUTTON = TRUE; //set flag TRUE
//
//      } else {
//
//        PRESSED_BUTTON = FALSE; //set flag FALSE
//      }
//
//    }
//    lastButtonState = butVal;
//  }
//  return PRESSED_BUTTON;
//}
