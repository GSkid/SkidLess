/*
   File: SolenoidButtonControl.ino
   Author: Brian Naranjo
   Date: 1/18/20

*/


/*******************************************************************************
  MODULE #INCLUDES
******************************************************************************/



#include <Arduino.h>
#include <U8x8lib.h>


#ifdef U8X8_HAVE_HW_SPI
#define U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif



/*******************************************************************************
   MODULE #DEFINES
 ******************************************************************************/
#define TRUE 1
#define FALSE 0

#define UP 1
#define DOWN 0


 


  /**************************************************************************
    *****************Set Up of 8X8 Sceen. Do not MOVE***********************/
U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

/************************************************************************/

//Initialize Pin Variables

//static int solenoidOutput = 3;
static int enterButtonInput = 4;
static int backButtonInput = 5;
static int downButtonInput = 3;
static int upButtonInput = 6;


//Button Variables
static int lastUpButtonState, lastDownButtonState, lastBackButtonState, lastEnterButtonState,
upButtonValue, downButtonValue, backButtonValue, enterButtonValue,
upButtonValue2, downButtonValue2, backButtonValue2, enterButtonValue2,
ENTER_PRESSED, UP_PRESSED, DOWN_PRESSED, BACK_PRESSED = 0;

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
    HOSE_VIEW,
    HOSE_EDIT,
    HOSE_MANUAL,
    OPTIONS,
    OPTIONS_RECAL,
    OPTIONS_SLEEP,
    OPTIONS_RESET,
    SLEEP,
} UI_States;

static UI_States currentPage = WELCOME_PAGE;

//Setup Pins and Serial Monitor

//void setup() {
    // put your setup code here, to run once:
    //  pinMode(solenoidOutput, OUTPUT);
  //  pinMode(enterButtonInput, INPUT);
    //pinMode(backButtonInput, INPUT);
   // pinMode(downButtonInput, INPUT);
   // pinMode(upButtonInput, INPUT);



   // Serial.begin(115200);

    //  lastButtonState = digitalRead(buttonInput); //Read initial value
   // u8x8.begin();
   // u8x8.setPowerSave(0);
   // u8x8.setFont(u8x8_font_chroma48medium8_r);
   // LCD_Welcome();

//}


uint16_t UI_StateLoop(void) {
    // put your main code here, to run repeatedly:

    uint16_t viewedSensor = 0;

    //Check input of Button.

    checkButtons();


    //UI State Machine

    switch (currentPage) {
    case WELCOME_PAGE:
        if (ENTER_PRESSED) {
            currentPage = HOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HomePage(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        break;

    case HOME_PAGE:
        if (DOWN_PRESSED) {
            LCD_ToggleArrow(DOWN);
            u8x8.clearDisplay();
            LCD_HomePage(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (UP_PRESSED) {
            LCD_ToggleArrow(UP);
            u8x8.clearDisplay();
            LCD_HomePage(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (BACK_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (ENTER_PRESSED) {

            u8x8.clearDisplay(); //To allow for new print
            if (arrowState == 0) {
                currentPage = SENSOR_DATA;
                arrowState = 0;
                LCD_SensorData(arrowState); //Start Home Page with cursor at first line
                LCD_PrintArrow(LCD_x, LCD_y);

            }
            else if (arrowState == 1) {
                currentPage = HOSE_SETUP;
                arrowState = 0;
                LCD_HoseSetup(arrowState); //Start Home Page with cursor at first line
                LCD_PrintArrow(LCD_x, LCD_y);

            }
            else if (arrowState == 2) {
                currentPage = OPTIONS;
                arrowState = 0;
                LCD_Options(arrowState); //Start Home Page with cursor at first line
                LCD_PrintArrow(LCD_x, LCD_y);

            }
            else {
                currentPage = SLEEP;
                arrowState = 0;
                LCD_Sleep(); //Start Home Page with cursor at first line
            }
            arrowState = 0;

        }
        break;

        /***********************************************************    SENSORS PAGES  ************************************************************/

    case SENSOR_DATA:
        if (DOWN_PRESSED) {
            LCD_ToggleArrow(DOWN);
            u8x8.clearDisplay();
            LCD_SensorData(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (UP_PRESSED) {
            LCD_ToggleArrow(UP);
            u8x8.clearDisplay();
            LCD_SensorData(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (BACK_PRESSED) {
            currentPage = HOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HomePage(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (ENTER_PRESSED) {
            u8x8.clearDisplay();
            if (arrowState == 0) {
                currentPage = SENSOR_SINGLE;
                u8x8.drawString(1, 7, "SINGLE SENSOR");

            }
            else if (arrowState == 1) {
                u8x8.drawString(4, 7, "MOISTURE");
                currentPage = SENSOR_MOISTURE;

            }
            else if (arrowState == 2) {
                u8x8.drawString(4, 7, "SUNLIGHT");
                currentPage = SENSOR_SUNLIGHT;

            }
            else {
                u8x8.drawString(4, 7, "WEATHER");
                currentPage = SENSOR_WEATHER;
            }
            arrowState = 0;

        }
        break;

    case SENSOR_SINGLE:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = SENSOR_DATA;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_SensorData(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        break;

    case SENSOR_MOISTURE:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = SENSOR_DATA;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_SensorData(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        break;

    case SENSOR_SUNLIGHT:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = SENSOR_DATA;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_SensorData(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        break;

    case SENSOR_WEATHER:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = SENSOR_DATA;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_SensorData(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        break;



        /***********************************************************    HOSE PAGES  ************************************************************/

    case HOSE_SETUP:
        if (DOWN_PRESSED) {
            LCD_ToggleArrow(DOWN);
            u8x8.clearDisplay();
            LCD_HoseSetup(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (UP_PRESSED) {
            LCD_ToggleArrow(UP);
            u8x8.clearDisplay();
            LCD_HoseSetup(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (BACK_PRESSED) {
            currentPage = HOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HomePage(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (ENTER_PRESSED) {
            u8x8.clearDisplay();
            if (arrowState == 0) {
                currentPage = HOSE_VIEW;
                u8x8.drawString(3, 7, "View Hoses");

            }
            else if (arrowState == 1) {
                currentPage = HOSE_EDIT;
                u8x8.drawString(3, 7, "Edit Hoses");

            }
            else if (arrowState == 2) {
                u8x8.drawString(1, 7, "Manual On/Off");
                currentPage = HOSE_MANUAL;
            }
            else {
                u8x8.drawString(3, 7, "View Hoses");
                currentPage = HOSE_VIEW;
            }
            arrowState = 0;

        }
        break;

    case HOSE_VIEW:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = HOSE_SETUP;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HoseSetup(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }

        break;

    case HOSE_EDIT:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = HOSE_SETUP;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HoseSetup(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }

        break;

    case HOSE_MANUAL:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = HOSE_SETUP;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HoseSetup(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }

        break;

        /***********************************************************    OPTIONS PAGES  ************************************************************/

    case OPTIONS:
        if (DOWN_PRESSED) {
            LCD_ToggleArrow(DOWN);
            u8x8.clearDisplay();
            LCD_Options(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (UP_PRESSED) {
            LCD_ToggleArrow(UP);
            u8x8.clearDisplay();
            LCD_Options(arrowState); //Update Home with arrow
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (BACK_PRESSED) {
            currentPage = HOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HomePage(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        else if (ENTER_PRESSED) {
            u8x8.clearDisplay();
            if (arrowState == 0) {
                currentPage = OPTIONS_RECAL;
                u8x8.drawString(1, 7, "Recalibration");

            }
            else if (arrowState == 1) {
                currentPage = OPTIONS_SLEEP;
                u8x8.drawString(2, 7, "Sleep Timer");

            }
            else if (arrowState == 2) {
                u8x8.drawString(3, 7, "Full Reset");
                currentPage = OPTIONS_RESET;
            }
            else {
                u8x8.drawString(2, 7, "Recalibration");
                currentPage = OPTIONS_RECAL;
            }
            arrowState = 0;

        }
        break;

    case OPTIONS_RECAL:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = OPTIONS;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Options(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }

        break;
    case OPTIONS_SLEEP:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = OPTIONS;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Options(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }

        break;
    case OPTIONS_RESET:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = OPTIONS;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Options(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }

        break;

        /***********************************************************    SLEEP PAGES  ************************************************************/

    case SLEEP:
        if (ENTER_PRESSED) {
            currentPage = WELCOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_Welcome();

        }
        else if (BACK_PRESSED) {
            currentPage = HOME_PAGE;
            arrowState = 0;
            u8x8.clearDisplay();
            LCD_HomePage(arrowState); //Start Home Page with cursor at first line
            LCD_PrintArrow(LCD_x, LCD_y);

        }
        break;

    }

    /***********************************************************    END OF PAGES  ************************************************************/
    // Return the nodeID of the sensor currently looking at for Sensors Page, otherwise return 0
    return viewedSensor;

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

    ENTER_PRESSED = FALSE;
    BACK_PRESSED = FALSE;
    DOWN_PRESSED = FALSE;
    UP_PRESSED = FALSE;

    enterButtonValue = digitalRead(enterButtonInput);
    downButtonValue = digitalRead(downButtonInput);
    backButtonValue = digitalRead(backButtonInput);
    upButtonValue = digitalRead(upButtonInput);

    delay(1);
    enterButtonValue2 = digitalRead(enterButtonInput);
    downButtonValue2 = digitalRead(downButtonInput);
    backButtonValue2 = digitalRead(backButtonInput);
    upButtonValue2 = digitalRead(upButtonInput);

    if (enterButtonValue == enterButtonValue2) {
        if (enterButtonValue != lastEnterButtonState) { //Change in State
            if (enterButtonValue == LOW) { //Flipped, Low is pressed
                ENTER_PRESSED = TRUE; //set flag TRUE
            }
            else {
                ENTER_PRESSED = FALSE; //set flag FALSE
            }
            lastEnterButtonState = enterButtonValue;
        }
    } if (downButtonValue == downButtonValue2) {
        if (downButtonValue != lastDownButtonState) { //Change in State
            if (downButtonValue == LOW) { //Flipped, Low is pressed
                DOWN_PRESSED = TRUE; //set flag TRUE
            }
            else {
                DOWN_PRESSED = FALSE; //set flag FALSE
            }
            lastDownButtonState = downButtonValue;
        }
    } if (upButtonValue == upButtonValue2) {
        if (upButtonValue != lastUpButtonState) { //Change in State
            if (upButtonValue == LOW) { //Flipped, Low is pressed
                UP_PRESSED = TRUE; //set flag TRUE
            }
            else {
                UP_PRESSED = FALSE; //set flag FALSE
            }
            lastUpButtonState = upButtonValue;
        }
    } if (upButtonValue == upButtonValue2) {
        if (backButtonValue != lastBackButtonState) { //Change in State
            if (backButtonValue == LOW) { //Flipped, Low is pressed
                BACK_PRESSED = TRUE; //set flag TRUE
            }
            else {
                BACK_PRESSED = FALSE; //set flag FALSE
            }
            lastBackButtonState = backButtonValue;
        }
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
   @Function LCD_ToggleArrow(int state)
   @param int Dircetion used to determine moving arrow up or down
   @return None
   @brief This function increment/decrements row location of arrow
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_ToggleArrow(int Direction) {
    if (Direction == DOWN) { //if down, increment arrowstate.
        if (arrowState >= 3) {
            arrowState = 0;
        }
        else {
            arrowState++;
        }
    }
    else {
        if (arrowState <= 0) { //otherwise, decrement arrowstate.
            arrowState = 3;
        }
        else {
            arrowState--;
        }
    }
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
    }
    else if (state == 1) {
        LCD_x = 11;
        LCD_y = 3;
    }
    else if (state == 2) {
        LCD_x = 8;
        LCD_y = 4;
    }
    else {
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
    }
    else if (state == 1) {
        LCD_x = 10;
        LCD_y = 3;
    }
    else if (state == 2) {
        LCD_x = 9;
        LCD_y = 4;
    }
    else {
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
    u8x8.drawString(0, 2, "View Hoses");
    u8x8.drawString(0, 3, "Edit Hoses");
    u8x8.drawString(0, 4, "Manual On/Off");

    //determines what row to print arrow on
    if (state == 0) {
        LCD_x = 11;
        LCD_y = 2;
    }
    else if (state == 1) {
        LCD_x = 11;
        LCD_y = 3;
    }
    else if (state == 2) {
        LCD_x = 14;
        LCD_y = 4;
    }
    else {
        LCD_x = 11;
        LCD_y = 2;
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
    u8x8.drawString(0, 2, "Recalibration ");
    u8x8.drawString(0, 3, "Sleep Timer");
    u8x8.drawString(0, 4, "Full Reset");

    //determines what row to print arrow on
    if (state == 0) {
        LCD_x = 14;
        LCD_y = 2;
    }
    else if (state == 1) {
        LCD_x = 12;
        LCD_y = 3;
    }
    else if (state == 2) {
        LCD_x = 11;
        LCD_y = 4;
    }
    else {
        LCD_x = 14;
        LCD_y = 2;
    }
}


/**
   @Function LCD_Sleep(void)
   @param None
   @return None
   @brief This function prints Sleep Page on LCD
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void LCD_Sleep(void) {
    u8x8.drawString(6, 4, "Zzzz");
    //determines what row to print arrow on

}