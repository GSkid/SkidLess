#ifndef __cplusplus
#define __cplusplus
#endif

// ********** INCLUDES **********
//#include <OLED/Fonts/ASCII_Font.h>
//#include <obj/OLED_Driver.h>
//#include <Fonts/ASCII_Font.h>
//#include <obj/OLED_GFX.h>
#include "OLED_GFX.h"
#include "OLED_Driver.h"
//#include "DEV_Config.h"
#include <stdio.h>
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <math.h>
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>
#include <RF24/utility/RPi/bcm2835.h>
#include<iostream>
#include<cstdio>
#include<vector>

/**** GLOBALS ****/
#define LED RPI_BPLUS_GPIO_J8_07
#define pushButton RPI_BPLUS_GPIO_J8_29

#define SPI_SPEED_2MHZ 2000000

#define TRUE  1
#define FALSE 0

#define MAX_ELEMENTS 20
#define MOISTURE 0
#define SUNLIGHT 1
#define TEMP  2

#define WATER_OFF 0
#define WATER_ON  1
#define PMOS_ON 0
#define PMOS_OFF 1
#define NMOS_ON 1
#define NMOS_OFF 0

#define LPMOS_Pin 6
#define LNMOS_Pin 13
#define RPMOS_Pin 19
#define RNMOS_Pin 26

#define ENTER_Pin  26 //16 
#define BACK_Pin  20
#define DOWN_Pin 12
#define UP_Pin 21

#define FIVE_SECONDS 5000
#define ONE_SECOND 1000
#define HUNDRED_MILLI 100

/* Avialable Colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
*/


/**** Configure the Radio ****/
/* Radio Pins:
 *            CE: 22
 *            CSN: 24
 *            MOSI: 19
 *            MISO: 21
 *            CLK: 23   */
RF24 radio(RPI_BPLUS_GPIO_J8_22, RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ);
RF24Network network(radio);
RF24Mesh mesh(radio, network);




// C_Struct stores relevant thresholds
typedef struct {
  float sM_thresh;
  float sM_thresh_00;
  //uint16_t bP_thresh;
  float lL_thresh;
  uint16_t tC_thresh;
  uint16_t time_thresh;
} C_Struct;

// D_Struct stores the relevant sensor data
typedef struct {
  float soilMoisture;
  //uint16_t baroPressure;
  float lightLevel;
  uint16_t temp_C;
  uint8_t digitalOut;
  //uint32_t timeStamp;
  uint8_t nodeID;
} D_Struct;

//States for Water Delivery SM
typedef enum{
  HOSE_IDLE,
  HOSE_ON_S1,
  HOSE_ON_S2,
  HOSE_ON_S3,
  HOSE_ON_S4,
  HOSE_OFF_S1,
  HOSE_OFF_S2, 
  HOSE_OFF_S3, 
  HOSE_OFF_S4,
} w_State;

//States for OLED SM
typedef enum{
  SLEEP,
  HOME_PAGE,
  SENSORS_HOME,
  SENSORS_PLOT,
  HOSES_HOME, 
  HOSES_STATUS,
  HOSES_WATER,
  HOSES_REMAP,
  SETTINGS_HOME,
  SETTINGS_SLEEP,
  SETTINGS_CAL,
  SETTINGS_RESET,
} OLED_State;


// Data Vars
D_Struct D_Dat;
D_Struct Test_Data[MAX_ELEMENTS];
OLED_State oledState = SLEEP;
static w_State waterState = HOSE_IDLE; //Water Deliver SM state var
static int HOSE_ONE = WATER_OFF; //flags used to monitor which Hoses are on
//static int HOSE_TWO = WATER_OFF;
//static int HOSE_THREE = WATER_OFF;
static uint8_t dataType = 0;
static int testVar = 3;
static char testBuf[9];
static char testBuf2[100];

static float testFloat = 64.757065;

//static char floatSign[9] = " ";

static float temp_wholeVal = 0;
  
static int wholeVal = 0;

static float temp_decimalVal = 0;

static int decimalVal = 0;


//Button Variables
static int prevArrowState, arrowState = 0;
//static int lastUpButtonState, lastDownButtonState, lastBackButtonState, lastEnterButtonState,
 //      upButtonValue, downButtonValue, backButtonValue, enterButtonValue,
 //      upButtonValue2, downButtonValue2, backButtonValue2, enterButtonValue2,
  //     ENTER_PRESSED, UP_PRESSED, DOWN_PRESSED, BACK_PRESSED = 0;
  
static int lastUpButtonState, lastDownButtonState, lastBackButtonState, lastEnterButtonState,
       upButtonValue, downButtonValue, backButtonValue, enterButtonValue,
       ENTER_PRESSED, UP_PRESSED, DOWN_PRESSED, BACK_PRESSED = 0;

uint8_t dFlag = 0;
uint8_t dataDat = 1;
uint8_t column_flag = 0;

// Timers
uint32_t dTimer = 0;
uint32_t wTimer = 0; //Timer used for driving Water Delivery
uint32_t oledTimer = 0; //Timer used for updating OLED
uint32_t frt = 0;

// Timer Support
uint8_t pingFlag = 0;

// RF24 Vars
uint8_t nodeID = 0;    // 0 = master

/**** Helper Fxn Prototypes ****/
int Timer(uint32_t, uint32_t);
void setup(void);
void checkButtons(void);
int printGrid(int16_t x0, int16_t x1, int16_t y0, int16_t y1, int16_t xtics, int16_t ytics);
int plotSampleData(D_Struct data[], uint8_t dataType, int16_t size);
int WaterDeliverySM(uint8_t status, uint32_t delayP_N, uint32_t pulseTime);
void OLED_PrintArrow(int x, int y);
void OLED_SM(uint16_t color);
void LPMOS_Set(uint8_t status);
void RPMOS_Set(uint8_t status);
void LNMOS_Set(uint8_t status);
void RNMOS_Set(uint8_t status);
int convertFloat_String(float in, char buffer[100]); 


/*
//  Testing OLED Initialization Code
*
* 
void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_ModuleExit();

    exit(0);
}
*/


/**** Void Setup ****/
void setup(void) {
  //Init the GPIO Library
  
   
  
  //signal(SIGINT, Handler);
  DEV_ModuleInit();
  Device_Init();
  
  bcm2835_init();
  bcm2835_spi_begin(); 
  
  //Set Pins to Output
  //DEV_GPIO_Mode(LPMOS_Pin, 1);
  //DEV_GPIO_Mode(RPMOS_Pin, 1);
  //DEV_GPIO_Mode(LNMOS_Pin, 1);
  //DEV_GPIO_Mode(RNMOS_Pin, 1);
  
  //Set Pins to Input
  DEV_GPIO_Mode(ENTER_Pin, 0);
  DEV_GPIO_Mode(BACK_Pin, 0);
  DEV_GPIO_Mode(DOWN_Pin, 0);
  DEV_GPIO_Mode(UP_Pin, 0);
    
  //LPMOS_Set(PMOS_OFF); //Initial States for MOS devices 
  //RPMOS_Set(PMOS_OFF);
  //LNMOS_Set(NMOS_OFF); 
  //RNMOS_Set(NMOS_OFF);
  
  
    
  oledTimer = bcm2835_millis();  //Initialize Oled Timer 
  // Set this node as the master node
  //printf("I am here \n");
  mesh.setNodeID(nodeID);
  printf("Node ID: %d\n", nodeID);
  radio.setPALevel(RF24_PA_MAX);
  

  // Initialize the mesh and check for proper chip connection
 
 if (mesh.begin()) {
    printf("\nInitialized: %d\n", radio.isChipConnected());
  }
  
  radio.printDetails();
  
  Clear_Screen();
  
  
 // if (testFloat < 0) {
  //  floatSign = "-";
  //} else {
  //  floatSign = " ";
  //}

  //temp_wholeVal = testFloat;
  
  //if (testFloat < 0){
  //  temp_wholeVal = -testFloat;
  //} 

  //wholeVal = temp_wholeVal;

  //temp_decimalVal = temp_wholeVal - wholeVal;
  
  //decimalVal = trunc(temp_decimalVal*10000);

  convertFloat_String(testFloat, testBuf2);

  return;
}

int main(int argc, char **argv) {
  setup();
  
  while(1) {
    
    //Testing for OLED, commented out RF Code 

    // Keep the network updated
    
     mesh.update();

    // Since this is the master node, we always want to be dynamically assigning
    //    addresses the new nodes
    mesh.DHCP();

    /**** Check For Available Network Data ****/

    // Check for incoming data from other nodes
    if (network.available()) {

      // Create a header var to store incoming network header
      RF24NetworkHeader header;
      // Get the data from the current header
      network.peek(header);

      // First ensure the message is actually addressed to the master
      if (header.to_node == 0) {

        // Switch on the header type to sort out different message types
        switch (header.type) {

          // Retrieve the data struct for D type messages
          case 'D':
            printf("Message Received\n");
            // Use the data struct to store data messages and print out the result
            //while (network.available() ) {
              network.read(header, &D_Dat, sizeof(D_Dat));
            //}
            dFlag = 1;
            break;

          // Do not read the header data, instead print the address inidicated by the header type
          default:
            break;
        }
      } else {
        network.read(header, 0, 0);
      }
    }

    /**** 'P' Type Evaluation ****/

    if (dFlag) {
      dFlag = 0;

      /*
      printf("Celebration Time\n");
      // Based on the data values, turn on or off the LED
      if (D_Dat.digitalOut) {
        //bcm2835_gpio_set(LED);
        printf("Celebration Time\n");
      } else {
        //bcm2835_gpio_clr(LED);
        printf("Celebration Time\n");
      }
      */

      /**** Write Data Values to SD Card ****/

      {
          FILE* out = fopen("Data_Log.txt", "a");

          
/*
          if (argc < 2)
          {
              printf("No output file specified.\n");
              exit(1);
          }

          out = fopen(argv[1], "w");

          if (out == NULL)
          {
              printf("Unable to open or generate file.\n");
              exit(1);
          }
*/


          // this is the imported data vector
          // std::vector<int> data = { 0, 1, 2, 3, 4, 5, 6 };

          // prints out main column headers for the data file.
          // conditional here: output if first loop, dont afterward, controlled by column_flag
          if (column_flag == 0)
          {
              fprintf(out, "Soil Moisture:   Ambient Light:   Ambient Temperature:   Calculated Digital Output:   Node ID:   \n");
              //Barometric Pressure:   
              //Time Stamp:   
              column_flag = 1;
          }

          printf("%f, %f, %d, %d, %d\n", D_Dat.soilMoisture, D_Dat.lightLevel, D_Dat.temp_C, D_Dat.digitalOut, D_Dat.nodeID);
          //%d, D_Dat.baroPressure, 
          //%d, D_Dat.timeStamp, 

          fprintf(out, "%13f    ", D_Dat.soilMoisture); // prints out 0th member of the data vector to the file.
          convertFloat_String(D_Dat.soilMoisture,testBuf2);
          //fprintf(out, "%19d    ", D_Dat.baroPressure); // prints out 1st member of the data vector to the file.
          fprintf(out, "%13f    ", D_Dat.lightLevel); // prints out 2nd member of the data vector to the file.
          fprintf(out, "%19d    ", D_Dat.temp_C); // prints out 3rd member of the data vector to the file.
          fprintf(out, "%25d    ", D_Dat.digitalOut); // prints out 4th member of the data vector to the file.
          //fprintf(out, "%10d    ", D_Dat.timeStamp); // prints out 5th member of the data vector to the file.
          fprintf(out, "%7d\n", D_Dat.nodeID); // prints out 6th member of the data vector to the file.
          fclose(out);
      }


      /**** 'S' and 'C' Type Message Responses ****/
      // Here we condition on if the node should be sent a configure message instead

      // Send to the message stored in the fromNode nodeID, message type 'S'
      RF24NetworkHeader p_header(mesh.getAddress(D_Dat.nodeID), 'S');
      // Data_Dat is just a 1 telling the node to go to sleep
      if (network.write(p_header, &dataDat, sizeof(dataDat))) {
        printf("Message Returned to %d\n\n", D_Dat.nodeID);
      }
    }


    /**** Get Node Data Values ****/
/*
    // Every 2 seconds, if the pushbutton is pressed, the master will send a 'D' message to
    // the current indexed node
    if (bcm2835_gpio_lev(pushButton) && ((bcm2835_millis() - dTimer) >= 2000)) {
      // Reset the dTimer
      dTimer = bcm2835_millis();

      //Prepare the data to be sent
      RF24NetworkHeader d_header(mesh.addrList[1].address, 'D');
      // addrIndex will be changed to reflect the a selectable option from the UI
      if (network.write(d_header, &dataDat, sizeof(dataDat))) {
        // Print out successful message sent
      } 
      else {
        //Print out error message
      }
    }
*/


    /**** UI Menu Control ****/

   // Draw_Line(0, 0, 100, SSD1351_HEIGHT - 1);
    //DEV_Delay_ms(20);
    
   // Draw_Line(, 0, 100, SSD1351_HEIGHT - 1);

      
      
    //Testing Hardcoded D_Struct Moisture Data  
      
      
    Test_Data[0].soilMoisture = 50;
    Test_Data[1].soilMoisture = 53;
    Test_Data[2].soilMoisture = 54;
    Test_Data[3].soilMoisture = 46;
    Test_Data[4].soilMoisture = 43;
    Test_Data[5].soilMoisture = 35;
    Test_Data[6].soilMoisture = 38;
    Test_Data[7].soilMoisture = 42;
    Test_Data[8].soilMoisture = 46;
    Test_Data[9].soilMoisture = 50;
    Test_Data[10].soilMoisture = 50;
    Test_Data[11].soilMoisture = 53;
    Test_Data[12].soilMoisture = 54;
    Test_Data[13].soilMoisture = 46;
    Test_Data[14].soilMoisture = 43;
    Test_Data[15].soilMoisture = 35;
    Test_Data[16].soilMoisture = 38;
    Test_Data[17].soilMoisture = 42;
    Test_Data[18].soilMoisture = 46;
    Test_Data[19].soilMoisture = 50;
      
      
    //Testing Hardcoded D_Struct Sunlight Data  
      
      
    Test_Data[0].lightLevel = 35;
    Test_Data[1].lightLevel = 37;
    Test_Data[2].lightLevel = 36;
    Test_Data[3].lightLevel = 37;
    Test_Data[4].lightLevel = 35;
    Test_Data[5].lightLevel = 38;
    Test_Data[6].lightLevel = 69;
    Test_Data[7].lightLevel = 96;
    Test_Data[8].lightLevel = 98;
    Test_Data[9].lightLevel = 100;
    Test_Data[10].lightLevel = 105;
    Test_Data[11].lightLevel = 102;
    Test_Data[12].lightLevel = 103;
    Test_Data[13].lightLevel = 100;
    Test_Data[14].lightLevel = 56;
    Test_Data[15].lightLevel = 42;
    Test_Data[16].lightLevel = 40;
    Test_Data[17].lightLevel = 38;
    Test_Data[18].lightLevel = 36;
    Test_Data[19].lightLevel = 35;
    
      
  //printf("Testing Hose 1 Status Before: %d \r \n", HOSE_ONE);    
 
 /*     
  if( !HOSE_ONE ) {
      WaterDeliverySM(WATER_ON, ONE_SECOND, FIVE_SECONDS);
  }
  
  //printf("Testing Hose 1 Status After On: %d \r \n", HOSE_ONE);
  
  if( HOSE_ONE ) {
    WaterDeliverySM(WATER_OFF, ONE_SECOND, FIVE_SECONDS);
  }
  
  */
  
  //printf("Testing Hose 1 Status After Off: %d \r \n", HOSE_ONE);
    
    
  /** Testing User Interface **/
    
  //User Input
    
  checkButtons();
    
  //Plot Grid
    
  //Set_Color(WHITE);
    
  //printGrid(20,120,20,120,10,10);
    
  // plotSampleData(Test_Data, MOISTURE, MAX_ELEMENTS);
      
        
  OLED_SM(WHITE);
  
/*      
  if (Timer(FIVE_SECONDS, oledTimer)){
    plotSampleData(Test_Data, MOISTURE, MAX_ELEMENTS);
    oledTimer = bcm2835_millis();
  }    
  */
      
 /*
  if (plotSampleData(Test_Data, MOISTURE, MAX_ELEMENTS)){
    printf("Sample Data Plotted \r \n");
    
  }   
    
  DEV_Delay_ms(500);
  
  */ 
    
    
  }  // Loop
  
  // Should NEVER get here
  return(1);
}


/****  HELPER FXNS ****/


/* @name: Timer
   @param: delayThresh - timer duration
   @param: prevDelay - time in millis() when the timer started
   @return: digital high/low depending if timer elapsed or not
   This is a non-blocking timer that handles uint32_t overflow,
   it works off the internal function millis() as reference
*/

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

  enterButtonValue = DEV_Digital_Read(ENTER_Pin);
  downButtonValue = DEV_Digital_Read(DOWN_Pin);
  backButtonValue = DEV_Digital_Read(BACK_Pin);
  upButtonValue = DEV_Digital_Read(UP_Pin);

  //DEV_Delay_ms(5);
  
  //enterButtonValue2 = DEV_Digital_Read(ENTER_Pin);
  //downButtonValue2 = DEV_Digital_Read(DOWN_Pin);
  //backButtonValue2 = DEV_Digital_Read(BACK_Pin);
  //upButtonValue2 = DEV_Digital_Read(UP_Pin);

  //if (enterButtonValue == enterButtonValue2) {
    if (enterButtonValue != lastEnterButtonState) { //Change in State
      if (enterButtonValue == LOW) { //Flipped, Low is pressed
        ENTER_PRESSED = TRUE; //set flag TRUE
        printf("Enter Pressed \r \n ");
      } else {
        ENTER_PRESSED = FALSE; //set flag FALSE
      }
      lastEnterButtonState = enterButtonValue;
    }
  //} if (downButtonValue == downButtonValue2) {
    if (downButtonValue != lastDownButtonState) { //Change in State
      if (downButtonValue == LOW) { //Flipped, Low is pressed
        DOWN_PRESSED = TRUE; //set flag TRUE
        printf("Down Pressed \r \n ");
      } else {
        DOWN_PRESSED = FALSE; //set flag FALSE
      }
      lastDownButtonState = downButtonValue;
    }
 // } if (upButtonValue == upButtonValue2) {
    if (upButtonValue != lastUpButtonState) { //Change in State
      if (upButtonValue == LOW) { //Flipped, Low is pressed
        UP_PRESSED = TRUE; //set flag TRUE
        printf("Up Pressed \r \n ");
      } else {
        UP_PRESSED = FALSE; //set flag FALSE
      }
      lastUpButtonState = upButtonValue;
    }
  //} if (upButtonValue == upButtonValue2) {
    if (backButtonValue != lastBackButtonState) { //Change in State
      if (backButtonValue == LOW) { //Flipped, Low is pressed
        BACK_PRESSED = TRUE; //set flag TRUE
        printf("Back Pressed \r \n ");
      } else {
        BACK_PRESSED = FALSE; //set flag FALSE
      }
      lastBackButtonState = backButtonValue;
    //}
  }
}

/* @name: printGrid
   @param: x0 - initial x position for grid
   @param: x1 - final x position for grid
   @param: y0 - initial y position for grid
   @param: y1 - final y position for grid
   @param: xtics - # of lines on x line
   @param: ytics - # of lines on y line
   @return: TRUE/FALSE depending if grid was successfully printed
*/

int printGrid(int16_t x0, int16_t x1, int16_t y0, int16_t y1, int16_t xtics, int16_t ytics){
  int i = 0;
  int xTic = 0;
  int yTic = 0;
  int incrementX =  ( x1 - x0 ) / xtics ;
  int incrementY =  ( y1 - y0 ) / ytics ;
  
  //printf("Xspaces: %d", incrementX); //Testing incrementX/Y
  //printf("Yspaces: %d", incrementY);
  
  //print x-axis
  Write_Line(x0, y1, x1, y1);
  
  xTic = x0 + incrementX;
  
  for(i=0;i <= xtics-1; i++){
    Write_Line(xTic, y1-2, xTic, y1+2 );
    xTic += incrementX;
  }
  
  //print y-axis  
  Write_Line(x0, y0, x0, y1);
  
  yTic = y1 - incrementY; 
  
  for(i=0;i <= ytics-1; i++){
    Write_Line(x0-2, yTic, x0+2, yTic );
    yTic -= incrementY;
  }
  
  return 0;
  
}

/* @name: plotSampleData 
   @param: TestData - array of structs used for plotting
   @param: dataType - type of sensor data to display
   @param: size - # of elements in array
   
   @return: TRUE/FALSE depending if data was successfully printed
*/

int plotSampleData( D_Struct TestData[], uint8_t dataType, int16_t size){
  
  int gridPlotted = FALSE;
  int i = 0;
  int16_t x_Value = 20; 
  int16_t mapped_y_Value = 0;
  
  if( dataType == MOISTURE){
    
    printf("Plotting Moisture \r\n ");
    
    Set_Color(RED);
    print_String(10,0, (const uint8_t*)"Moisture Level", FONT_5X8);
    print_String(10,10, (const uint8_t*)"(Node 1)", FONT_5X8);
    
    Set_Color(BLUE);
    print_String(0,60, (const uint8_t*)"Water%", FONT_5X8);
    print_String(65,120, (const uint8_t*)"Hours", FONT_5X8);
    
    
    for(i = 0; i <= (size-1) ; i++){
      
      mapped_y_Value = (int16_t)TestData[i].soilMoisture;
   
      //printf("Element: %d \r\n", i); //Testing Struct Elements
      
      //printf("Moisture Value: %f \r\n", TestData[i].soilMoisture);
      
      Draw_Pixel(x_Value, mapped_y_Value);
      
      x_Value += 5;
    }
    
    gridPlotted = TRUE;
    
  } else if( dataType == SUNLIGHT){
    printf("Plotting Sunlight \r\n ");
    
    Set_Color(RED);
    print_String(10,0, (const uint8_t*)"Light Level", FONT_5X8);
    print_String(10,10, (const uint8_t*)"(Node 1)", FONT_5X8);
    
    
    Set_Color(YELLOW);
    print_String(0,60, (const uint8_t*)"Light%", FONT_5X8);
    print_String(65,120, (const uint8_t*)"Hours", FONT_5X8);
    
    
    
    for(i = 0; i <= (size-1) ; i++){
      
      mapped_y_Value = (int16_t)TestData[i].lightLevel;
   
      //printf("Element: %d \r\n", i);  //Testing Struct Elements
      
      //printf("Light Level Value: %f \r\n", TestData[i].lightLevel); 
      
      Draw_Pixel(x_Value, mapped_y_Value);
      
      x_Value += 5;
    }
    
    gridPlotted = TRUE;
    
  } else if( dataType == TEMP){
    printf("Plotting Temperature \r\n ");
    
    for(i = 0; i <= size ; i++){
      
      mapped_y_Value = (int16_t)TestData[i].temp_C;
   
      //printf("Element: %d \r\n", i); //Testing Struct Elements
      
      //printf("Temp Value: %d \r\n", TestData[i].temp_C);
      
      Draw_Pixel(x_Value, mapped_y_Value);
      
      x_Value += 5;
    }
    
    gridPlotted = TRUE;
    
  } else {
    printf(" No Plot Selected \r\n ");
    
    gridPlotted = FALSE;
  }  

  return gridPlotted;
  
}

/* @name: LPMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void LPMOS_Set(uint8_t status){
  DEV_Digital_Write(LPMOS_Pin, status);
}

/* @name: RPMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void RPMOS_Set(uint8_t status){
  DEV_Digital_Write(RPMOS_Pin, status);
}

/* @name: LNMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void LNMOS_Set(uint8_t status){
  DEV_Digital_Write(LNMOS_Pin, status);
}

/* @name: RNMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void RNMOS_Set(uint8_t status){
  DEV_Digital_Write(RNMOS_Pin, status);
}


/* @name: WaterDeliverSM 
   @param: status - whether to turn on or off WD
   @param: delayP_N - delay time between turning ON/OFF PFET and NFET
   @param: pulseTime - Time for +/-5V Pulse, Delays time between ON and OFF 
   @return: 1/0 depending on whether drive was completed
*/
int WaterDeliverySM(uint8_t status, uint32_t delayP_N, uint32_t pulseTime){
  w_State nextState = waterState; //initialize var to current state
  int hoseSet = FALSE; // Set to TRUE(1) once done Driving 
  //printf("Test Next State1 = %d \r \n", nextState);
  //printf("Testing State1 = %d \r \n ", waterState);

  
  switch(waterState){
    case HOSE_IDLE:
      if ( (status == WATER_ON) && (HOSE_ONE == WATER_OFF) ){
        nextState = HOSE_ON_S1;
        wTimer = bcm2835_millis();
        hoseSet = FALSE; 
        //printf("Test Next State2 = %d", nextState);
        //printf("Test State2 = %d", waterState);
        printf("Leaving Hose Idle: On  \r \n");
      } else if ( (status == WATER_OFF) && (HOSE_ONE == WATER_ON) ){
        nextState = HOSE_OFF_S1;
        wTimer = bcm2835_millis();
        hoseSet = FALSE; 
        printf("Leaving Hose Idle: off  \r \n");
      }
      break;
      
    case HOSE_ON_S1:
      LNMOS_Set(NMOS_ON);
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S2;
        printf("Leaving Hose On S1 \r \n");
      }
      break;
    
    case HOSE_ON_S2:
      RPMOS_Set(PMOS_ON); 
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S3;
        printf("Leaving Hose On S2 \r \n");
      }
      break;
      
    case HOSE_ON_S3:
      RPMOS_Set(PMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S4;
        printf("Leaving Hose On S3 \r \n");
      }
      break;
      
    case HOSE_ON_S4:
      LNMOS_Set(NMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        nextState = HOSE_IDLE;
        printf("Leaving Hose On S4 \r \n");
        HOSE_ONE = WATER_ON;
        hoseSet = TRUE;
      }
      break;
    
    case HOSE_OFF_S1:
      RNMOS_Set(NMOS_ON);
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S2;
        printf("Leaving Hose Off S1 \r \n");
      }
      break;
    
    case HOSE_OFF_S2:
      LPMOS_Set(PMOS_ON); 
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S3;
        printf("Leaving Hose Off S2 \r \n");
      }
      break;
      
    case HOSE_OFF_S3:
      LPMOS_Set(PMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S4;
        printf("Leaving Hose Off S3 \r \n");
      }
      break;
      
    case HOSE_OFF_S4:
      RNMOS_Set(NMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        nextState = HOSE_IDLE;
        HOSE_ONE = WATER_OFF;
        hoseSet = TRUE;
        printf("Leaving Hose Off S4 \r \n");
      }
      break;
    
  }
    
  waterState = nextState;
  //printf("State3: %d \r \n", waterState);
  //printf("Next State3: %d \r \n ", nextState);
  return hoseSet;  //1 if set, 0 if still in S1-4

}

/**
   @Function LCD_PrintArrow(int state)
   @param int x, int y Used to determine x,y position of arrow
   @return None
   @brief This function prints Arrow on OLED at x,y coordinates
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void OLED_PrintArrow(int x, int y) {
  print_String(x,y, (const uint8_t*)"<", FONT_5X8);
}

/* @name: OLED_SM 
   @param: Color of Page Text
   @return: void
*/

void OLED_SM(uint16_t color){
  OLED_State nextPage = oledState;
  Set_Color(color);
  
  //Toggle Arrow
  if (DOWN_PRESSED) { //if down, increment arrowstate.
    if (arrowState >= 3) {
      arrowState = 0;
    } else {
      arrowState++;
    }
  } else if (UP_PRESSED){
    if (arrowState <= 0) { //otherwise, decrement arrowstate.
      arrowState = 3;
    } else {
      arrowState--;
    }
  }


  switch(oledState){
    case SLEEP:
      
      sprintf(testBuf,"%d", testVar);
      //sprintf(testBuf2,"%d.%04d", wholeVal, decimalVal );
      print_String(35,55, (const uint8_t*)"SLEEPING", FONT_8X16);
      print_String(35,5, (const uint8_t*)testBuf, FONT_8X16);
      print_String(10,90, (const uint8_t*)testBuf2, FONT_5X8);
      
      if (ENTER_PRESSED){
        nextPage = HOME_PAGE;
        
        Clear_Screen();
      }
      break;
    
    case HOME_PAGE:
      if (prevArrowState != arrowState){
        Clear_Screen();
      }  
        
      print_String(0,0, (const uint8_t*)"Home Page", FONT_8X16);
      print_String(0,30, (const uint8_t*)"Sensor Data", FONT_5X8);
      print_String(0,40, (const uint8_t*)"Hose Configuration", FONT_5X8);
      print_String(0,50, (const uint8_t*)"Settings", FONT_5X8);
      
      if(arrowState == 0){
        OLED_PrintArrow(70, 30);
      } else if (arrowState == 1){
        OLED_PrintArrow(110, 40);
      } else {
        OLED_PrintArrow(55, 50);
      } 
      
      
      if (ENTER_PRESSED){
        if(arrowState == 0){
          nextPage = SENSORS_HOME;
        } else if(arrowState == 1){
          nextPage = HOSES_HOME;
        } else{
          nextPage = SETTINGS_HOME;
        }
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = SLEEP;
        Clear_Screen();
      }
      break;
    
    case SENSORS_HOME:
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Sensors Home", FONT_8X16);
      print_String(0,30, (const uint8_t*)"Plot Moisture Data", FONT_5X8);
      print_String(0,40, (const uint8_t*)"Plot Sunlight Data", FONT_5X8);
      
      
      if(arrowState == 0){
        OLED_PrintArrow(110, 30);
      } else {
        OLED_PrintArrow(110, 40);
      } 
      
      if (ENTER_PRESSED){
        Clear_Screen();
        if(arrowState == 0){
          dataType = MOISTURE;
        } else {
          dataType = SUNLIGHT;
        }
        nextPage = SENSORS_PLOT;
        Clear_Screen();
        
      } else if (BACK_PRESSED){
        nextPage = HOME_PAGE;
        Clear_Screen();
      }
      break;
    
    case SENSORS_PLOT:
      printGrid(20,120,20,120,10,10);
      plotSampleData(Test_Data, dataType, MAX_ELEMENTS);
      
      if(ENTER_PRESSED){
        if(dataType == MOISTURE){
          dataType = SUNLIGHT;
        } else {
          dataType = MOISTURE;
        }
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = SENSORS_HOME;
        Clear_Screen();
      }
      
      break;
      
   
    
    case HOSES_HOME: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Hoses", FONT_8X16);
      
      print_String(0,30, (const uint8_t*)"Current Hose Status", FONT_5X8);
      print_String(0,40, (const uint8_t*)"Watering Log", FONT_5X8);
      print_String(0,50, (const uint8_t*)"Remap Sensors ", FONT_5X8);
      
      if(arrowState == 0){
        OLED_PrintArrow(112, 30);
      } else if (arrowState == 1) {
        OLED_PrintArrow(80, 40);
      } else {
        OLED_PrintArrow(85, 50);
      }
      
      if (ENTER_PRESSED){
        if(arrowState == 0){
          nextPage = HOSES_STATUS;
        } else if (arrowState == 1){
          nextPage = HOSES_WATER;
        } else {
          nextPage = HOSES_REMAP;
        }
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = HOME_PAGE;
        Clear_Screen();
      }
      break;
      
    case HOSES_STATUS: 
      print_String(0,0, (const uint8_t*)"Hoses Status", FONT_8X16);
      if (ENTER_PRESSED){
        nextPage = SLEEP;
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = HOSES_HOME;
        Clear_Screen();
      }
      break;
      
    case HOSES_WATER: 
      print_String(0,0, (const uint8_t*)"Watering Log", FONT_8X16);
      if (ENTER_PRESSED){
        nextPage = SLEEP;
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = HOSES_HOME;
        Clear_Screen();
      }
      break;
      
    case HOSES_REMAP: 
      print_String(0,0, (const uint8_t*)"Remap Hoses", FONT_8X16);
      if (ENTER_PRESSED){
        nextPage = SLEEP;
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = HOSES_HOME;
        Clear_Screen();
      }
      break;     
    
    
    case SETTINGS_HOME:
    
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Settings", FONT_8X16);
      
      print_String(0,30, (const uint8_t*)"Adjust Sleep Timer", FONT_5X8);
      print_String(0,40, (const uint8_t*)"Calibrate Sensors", FONT_5X8);
      print_String(0,50, (const uint8_t*)"Reset System", FONT_5X8);
      
      if(arrowState == 0){
        OLED_PrintArrow(112, 30);
      } else if (arrowState == 1) {
        OLED_PrintArrow(100, 40);
      } else {
        OLED_PrintArrow(85, 50);
      }
      
      if (ENTER_PRESSED){
        if(arrowState == 0){
          nextPage = SETTINGS_SLEEP;
        } else if (arrowState == 1){
          nextPage = SETTINGS_CAL;
        } else {
          nextPage = SETTINGS_RESET;
        }
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = HOME_PAGE;
        Clear_Screen();
      }
      break;
      
     case SETTINGS_SLEEP: 
      print_String(0,0, (const uint8_t*)"Sleep Settings", FONT_8X16);
      if (ENTER_PRESSED){
        nextPage = SLEEP;
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = SETTINGS_HOME;
        Clear_Screen();
      }
      break;  
      
     case SETTINGS_CAL: 
      print_String(0,0, (const uint8_t*)"Sensors Recal", FONT_8X16);
      if (ENTER_PRESSED){
        nextPage = SLEEP;
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = SETTINGS_HOME;
        Clear_Screen();
      }
      break; 
      
      case SETTINGS_RESET: 
      print_String(0,0, (const uint8_t*)"System Reset", FONT_8X16);
      if (ENTER_PRESSED){
        nextPage = SLEEP;
        Clear_Screen();
      } else if (BACK_PRESSED){
        nextPage = SETTINGS_HOME;
        Clear_Screen();
      }
      break;    
      
    }
    
    prevArrowState = arrowState;
    oledState = nextPage;
    
    return;
}

int convertFloat_String(float in, char buffer[100]){
    temp_wholeVal = in;
  
    //if (testFloat < 0){
    //  temp_wholeVal = -testFloat;
    //} 

    wholeVal = temp_wholeVal;

    temp_decimalVal = temp_wholeVal - wholeVal;
  
    decimalVal = trunc(temp_decimalVal*10000);
    
    sprintf(buffer,"%d.%04d", wholeVal, decimalVal );

    return 0;
} 
