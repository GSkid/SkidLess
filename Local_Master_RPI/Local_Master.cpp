#ifndef __cplusplus
#define __cplusplus
#endif

// ********** INCLUDES ***********
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <RF24Mesh/RF24Mesh.h>
#include <RF24/utility/RPi/bcm2835.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include "OLED_GFX.h"
#include "OLED_Driver.h"
#include <stdio.h>
#include <stdlib.h>		//exit()
#include <signal.h>     //signal()
#include <math.h>

/**** GLOBALS ****/
#define LED RPI_BPLUS_GPIO_J8_07
#define pushButton RPI_BPLUS_GPIO_J8_29
#define SPI_SPEED_2MHZ 2000000
#define FORECAST_CALL 1800000

#define TRUE  1
#define FALSE 0

#define MAX_ELEMENTS 100
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

#define ENTER_Pin  16 
#define BACK_Pin  20
#define DOWN_Pin 12
#define UP_Pin 21

#define FIVE_SECONDS 5000
#define MIN_3 180000
#define MIN_2 120000
#define ONE_SECOND 1000
#define PULSE_DURATION 1500
#define FET_DELAY 5
#define HUNDRED_MILLI 100
#define MAX_SENSORS 20
#define HOURS_36 129600000
#define MIN_10 600000

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
  float lL_thresh;
  uint16_t tC_thresh;
  uint16_t time_thresh;
} C_Struct;

// D_Struct stores the relevant sensor data
typedef struct {
  float soilMoisture;
  float lightLevel;
  uint16_t temp_C;
  uint8_t digitalOut;
  uint8_t nodeID;
  uint8_t battLevel;
} D_Struct;

typedef struct{
    int precipProb;
    int temperature;
    int humidity;
    int pressure;
    int windSpeed;
    int windBearing;
} Forecast;

typedef struct {
    uint8_t status;
    uint8_t sensors[MAX_SENSORS];
    uint8_t waterLevel;
    uint8_t tally;
    uint8_t flowRate;
    uint8_t rainFlag;
    uint32_t rainTimer;
    uint8_t control;
}Hoses;

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

// Enum for hsoe specification
typedef enum {
    HOSE0,
    HOSE1,
    HOSE2,
}HOSE_NUM;

// Enum for control types
enum {
    OFF,
    ON,
    AUTOMATIC,
};


// Data Vars
D_Struct D_Dat;
D_Struct sensor_data[MAX_ELEMENTS];
uint8_t dFlag = 0;
uint8_t dataDat = 1;
uint8_t column_flag = 0;
uint8_t sd_index = -1;

D_Struct Test_Data[MAX_ELEMENTS];
OLED_State oledState = SLEEP;
static w_State waterState = HOSE_IDLE; //Water Deliver SM state var
static int HOSE_ONE = WATER_OFF; //flags used to monitor which Hoses are on
//static int HOSE_TWO = WATER_OFF;
//static int HOSE_THREE = WATER_OFF;
static uint8_t dataType = 0;

//Button Variables
static int prevArrowState, arrowState = 0;
static int lastUpButtonState, lastDownButtonState, lastBackButtonState, lastEnterButtonState,
       upButtonValue, downButtonValue, backButtonValue, enterButtonValue,
       upButtonValue2, downButtonValue2, backButtonValue2, enterButtonValue2,
       ENTER_PRESSED, UP_PRESSED, DOWN_PRESSED, BACK_PRESSED = 0;

// Timers
uint32_t dTimer = 0;
uint32_t frt = 0;
uint32_t forecastTimer = 0;
uint32_t waterDeliveryTimer = 0;
uint32_t wTimer = 0; //Timer used for driving Water Delivery testing
uint32_t oledTimer = 0; //Timer used for updating OLED testing
uint32_t connectionTimer = 0;

// Timer Support
uint8_t pingFlag = 0;

// RF24 Vars
uint8_t nodeID = 0;    // 0 = master
uint8_t num_nodes = 0;

// Forecast Support
Forecast Forecast1;
char buffer[10];
double data[6];
FILE* fp;
uint8_t rainFlag = 0;

// Water Delivery Support
Hoses Hose0, Hose1, Hose2;
Hoses Hose[3];
uint8_t hose_statuses = 0;


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
uint8_t WaterDelivery(HOSE_NUM);



/*********************************************************************************************/
/**** Void Setup ****/
void setup(void) {
  // Initialize the Hose array
  Hose[0] = Hose0; Hose[1] = Hose1; Hose[2] = Hose2;
  Hose[0].waterLevel = 1; Hose[1].waterLevel = 1; Hose[2].waterLevel = 1; 
  Hose[0].control = AUTOMATIC; Hose[1].control = OFF; Hose[2].control = OFF;
  
  //Init the GPIO Library
  
  bcm2835_init();
  
  bcm2835_spi_begin();  
  
  DEV_ModuleInit();
  Device_Init();
  
  //Set Pins to Output
  DEV_GPIO_Mode(LPMOS_Pin, 1);
  DEV_GPIO_Mode(RPMOS_Pin, 1);
  DEV_GPIO_Mode(LNMOS_Pin, 1);
  DEV_GPIO_Mode(RNMOS_Pin, 1);
  
  //Set Pins to Input
  DEV_GPIO_Mode(ENTER_Pin, 0);
  DEV_GPIO_Mode(BACK_Pin, 0);
  DEV_GPIO_Mode(DOWN_Pin, 0);
  DEV_GPIO_Mode(UP_Pin, 0);
    
  LPMOS_Set(PMOS_OFF); //Initial States for MOS devices 
  RPMOS_Set(PMOS_OFF);
  LNMOS_Set(NMOS_OFF); 
  RNMOS_Set(NMOS_OFF);
  
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
  return;
}


/********************************************************************************************/
int main(int argc, char **argv) {
  setup();
  while(1) {
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
            network.read(header, &D_Dat, sizeof(D_Dat));
            // Set the flag that indicates we need to respond to a new message
            dFlag = 1;

            // Here is where we add the sensor data to the sensor data array
            // But first we want to see if the sensor data array is full
            if (sd_index >= MAX_ELEMENTS) { // checks if the index is at the max # of elements
                int i, j = 0;
                // Now we transfer the 10 most recent data values to the bottom of the list
                for ((i = MAX_ELEMENTS - 10); i < MAX_ELEMENTS; i++) {
                    sensor_data[j] = sensor_data[i]; // j is the bottom, i is the top
                    j++;
                }
                // Reset the sensor data index
                sd_index = 10;
            }
            // Increment the sensor data index for the new value
            sd_index++;
            // Then place the new data into the array
            sensor_data[sd_index] = D_Dat;
            break;

          // Do not read the header data, instead print the address inidicated by the header type
          default:
            break;
        }
      } else {
        network.read(header, 0, 0);
      }
    }




    /**** Update List of Nodes ****/
    if (Timer(MIN_2, connectionTimer)) {
      connectionTimer = millis();
      // Other option is to create a dict after receiving a message
      if (num_nodes != mesh.addrListTop) {
        num_nodes = mesh.addrListTop;
        printf("\nConnected nodes: ");
        int i = 0;
        for (i = 0; i < mesh.addrListTop; i++) {
            // Add sensor nodes to the list of sensors mapped to the hose
            Hose[HOSE0].sensors[i] = mesh.addrList[i].nodeID;
            if (i == (mesh.addrListTop - 1)) {
              printf("%d\n\n", mesh.addrList[i].nodeID);
            } else {
              printf("%d, ", mesh.addrList[i].nodeID);
            }
        }
        Hose[HOSE0].waterLevel = i/2;
      }
    }




    /**** Data Logging ****/
  
    if (dFlag) {
        // This should be the last thing that gets done when data is received
      dFlag = 0;

      /**** Write Data Values to SD Card ****/
      {
          FILE* out = fopen("Data_Log.csv", "a");
 
          // prints out main column headers for the data file.
          // conditional here: output if first loop, dont afterward, controlled by column_flag
          if (column_flag == 0)
          {
              fprintf(out, "Soil_Moisture,Ambient_Light,Ambient_Temp,Barometric_Pressure,Precip_Prob,Digital_Output,Node_ID,Battery_Level,Hose_1,Hose_2,Hose_3\n");
              column_flag = 1;
          }

          printf("%f, %f, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", D_Dat.soilMoisture, D_Dat.lightLevel, D_Dat.temp_C, Forecast1.pressure, Forecast1.precipProb, D_Dat.digitalOut, D_Dat.nodeID, D_Dat.battLevel, Hose[0].status, Hose[1].status, Hose[2].status);
        
          fprintf(out, "%13f,   ", D_Dat.soilMoisture); // prints out 0th member of the data vector to the file.
          fprintf(out, "%13f,   ", D_Dat.lightLevel); // prints out 2nd member of the data vector to the file.
          fprintf(out, "%19d,   ", D_Dat.temp_C); // prints out 3rd member of the data vector to the file.
          fprintf(out, "%19d,   ", Forecast1.pressure);
          fprintf(out, "%11d,   ", Forecast1.precipProb);
          fprintf(out, "%14d,   ", D_Dat.digitalOut); // prints out 4th member of the data vector to the file.
          fprintf(out, "%7d,   ", D_Dat.nodeID); // prints out 6th member of the data vector to the file.
          fprintf(out, "%14d,   ", D_Dat.battLevel);
          fprintf(out, "%5d,   ", Hose[0].status);
          fprintf(out, "%5d,   ", Hose[1].status);
          fprintf(out, "%5d,\n", Hose[2].status);
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




    /**** UI Menu Control ****/

   


    /**** Water Delivery ****/

    if (Timer(MIN_3, waterDeliveryTimer)) {
        // reset the timer
        waterDeliveryTimer = millis();
        // Then call WaterDelivery to see if we need to turn on each hose
        if (Hose[0].control == AUTOMATIC) {
            hose_statuses = WaterDelivery(HOSE0);
        }
        if (Hose[1].control == AUTOMATIC) {
            hose_statuses = WaterDelivery(HOSE1);
        }
        if (Hose[2].control == AUTOMATIC) {
            hose_statuses = WaterDelivery(HOSE2);
        }
    }
    



    /**** Forecast Data API Call ****/

    if (Timer(FORECAST_CALL, forecastTimer)) {
        printf("Opening call to forecast API...\n");
        forecastTimer = millis();
        // Opens and runs the python script in the terminal
        fp = popen("python RFpython_test.py", "r");

        // error checking
        if (fp == NULL)
        {
            printf("Failed to run command.\n");
            break;
        }

        printf("Call to forecast API success.\n");
        int tmp = 0;

        // loop that extracts the outputted data from the shell and places it in an array
        while (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            sscanf(buffer, "%lf", &data[tmp]);
            ++tmp;
        }

        // moves the extracted data from the array to the struct
        Forecast1.precipProb = round(data[0]);
        printf("Forecast1.precipProb = %d.\n", Forecast1.precipProb);
        Forecast1.temperature = round(data[1]);
        printf("Forecast1.temperature = %d.\n", Forecast1.temperature);
        Forecast1.humidity = round(data[2]);
        printf("Forecast1.humidity = %d.\n", Forecast1.humidity);
        Forecast1.pressure = round(data[3]);
        printf("Forecast1.pressure = %d.\n", Forecast1.pressure);
        Forecast1.windSpeed = round(data[4]);
        printf("Forecast1.windSpeed = %d.\n", Forecast1.windSpeed);
        Forecast1.windBearing = round(data[5]);
        printf("Forecast1.windBearing = %d.\n", Forecast1.windBearing);

        pclose(fp);
    }
  }  // Loop
  
// Should NEVER get here
return(1);
}


/**************************************************************************************************/
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



/* @name: WaterDelivery
   @param: HOSE_NUM - an enum that specifies which hose to evaluate
   @return: uint8_t - a bit array of values that indicate which hoses are on/off
 */
uint8_t WaterDelivery(HOSE_NUM HOSE_IN)
{
    // First reset the hose tally
    Hose[HOSE_IN].tally = 0;
    int prevstatus = Hose[HOSE_IN].status;

    // Then need to tally up the digital outs on the hose
    int i, j = 0;
    for (i = 0; i <= MAX_SENSORS; i++) {
        // This just shuts down the for loop if the list of sensors is exhausted
        if ((Hose[HOSE_IN].sensors[i] <= 0) || (sd_index == -1)) {
            break;
        }
        for (j = sd_index; j >= 0; j--) {
            // Check if the data item is a sensor mapped to the hose
            if ((sensor_data[j].nodeID == Hose[HOSE_IN].sensors[i]) && (sensor_data[j].nodeID)) {
                // If it is, increase the tally
                Hose[HOSE_IN].tally += sensor_data[j].digitalOut;
                break;
            }
        }
    }

    // Next check if the tally is above the water level threshold
    if (Hose[HOSE_IN].tally > Hose[HOSE_IN].waterLevel) {
        // Check the forecast data
        if (Forecast1.precipProb <= 30) {
            rainFlag = 0;
            Hose[HOSE_IN].rainFlag = 0;
            // Go ahead and turn on the water
            Hose[HOSE_IN].status = WATER_ON;
        }
        else {
            if (!Hose[HOSE_IN].rainFlag) {
                Hose[HOSE_IN].rainFlag++;
                Hose[HOSE_IN].rainTimer = millis();
                Hose[HOSE_IN].status = WATER_OFF;
            }
            else if (Timer(HOURS_36, Hose[HOSE_IN].rainTimer)) {
                rainFlag = 0;
                // Go ahead and turn on the water
                Hose[HOSE_IN].status = WATER_ON;
            }
        }
    }// ...If the sensors indicate it is not dry enough to water
    else {
      Hose[HOSE_IN].status = WATER_OFF;
    }
    printf("Hose %d Water Delivery:\nHose Status: %d;  Prev State = %d\n\n", HOSE_IN, Hose[HOSE_IN].status, prevstatus);
    // Now we actually turn on or off the Hose
    if (prevstatus != Hose[HOSE_IN].status) {
        // Call the state machine to open the solenoid valve
        while (!WaterDeliverySM(Hose[HOSE_IN].status, FET_DELAY, PULSE_DURATION));
    }
    // Create a bit array of hose states to return
    uint8_t hose_status = Hose[2].status * 4 + Hose[1].status * 2 + Hose[0].status;

    return hose_status;
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
        hoseSet = 0; 
        //printf("Test Next State2 = %d", nextState);
        //printf("Test State2 = %d", waterState);
        printf("Leaving Hose Idle: On  \n");
      } else if ( (status == WATER_OFF) && (HOSE_ONE == WATER_ON) ){
        nextState = HOSE_OFF_S1;
        wTimer = bcm2835_millis();
        hoseSet = 0; 
        printf("Leaving Hose Idle: off  \n");
      }
      break;
      
    case HOSE_ON_S1:
      LNMOS_Set(NMOS_ON);
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S2;
        printf("Leaving Hose On S1 \n");
      }
      break;
    
    case HOSE_ON_S2:
      RPMOS_Set(PMOS_ON); 
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S3;
        printf("Leaving Hose On S2 \n");
      }
      break;
      
    case HOSE_ON_S3:
      RPMOS_Set(PMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S4;
        printf("Leaving Hose On S3 \n");
      }
      break;
      
    case HOSE_ON_S4:
      LNMOS_Set(NMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        nextState = HOSE_IDLE;
        printf("Leaving Hose On S4 \n");
        HOSE_ONE = WATER_ON;
        hoseSet = 1;
      }
      break;
    
    case HOSE_OFF_S1:
      RNMOS_Set(NMOS_ON);
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S2;
        printf("Leaving Hose Off S1 \n");
      }
      break;
    
    case HOSE_OFF_S2:
      LPMOS_Set(PMOS_ON); 
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S3;
        printf("Leaving Hose Off S2 \n");
      }
      break;
      
    case HOSE_OFF_S3:
      LPMOS_Set(PMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S4;
        printf("Leaving Hose Off S3 \n");
      }
      break;
      
    case HOSE_OFF_S4:
      RNMOS_Set(NMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        nextState = HOSE_IDLE;
        HOSE_ONE = WATER_OFF;
        hoseSet = 1;
        printf("Leaving Hose Off S4 \n");
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
      print_String(35,55, (const uint8_t*)"SLEEPING", FONT_8X16);
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

  DEV_Delay_ms(5);
  
  enterButtonValue2 = DEV_Digital_Read(ENTER_Pin);
  downButtonValue2 = DEV_Digital_Read(DOWN_Pin);
  backButtonValue2 = DEV_Digital_Read(BACK_Pin);
  upButtonValue2 = DEV_Digital_Read(UP_Pin);

  if (enterButtonValue == enterButtonValue2) {
    if (enterButtonValue != lastEnterButtonState) { //Change in State
      if (enterButtonValue == LOW) { //Flipped, Low is pressed
        ENTER_PRESSED = TRUE; //set flag TRUE
        printf("Enter Pressed \r \n ");
      } else {
        ENTER_PRESSED = FALSE; //set flag FALSE
      }
      lastEnterButtonState = enterButtonValue;
    }
  } if (downButtonValue == downButtonValue2) {
    if (downButtonValue != lastDownButtonState) { //Change in State
      if (downButtonValue == LOW) { //Flipped, Low is pressed
        DOWN_PRESSED = TRUE; //set flag TRUE
        printf("Down Pressed \r \n ");
      } else {
        DOWN_PRESSED = FALSE; //set flag FALSE
      }
      lastDownButtonState = downButtonValue;
    }
  } if (upButtonValue == upButtonValue2) {
    if (upButtonValue != lastUpButtonState) { //Change in State
      if (upButtonValue == LOW) { //Flipped, Low is pressed
        UP_PRESSED = TRUE; //set flag TRUE
        printf("Up Pressed \r \n ");
      } else {
        UP_PRESSED = FALSE; //set flag FALSE
      }
      lastUpButtonState = upButtonValue;
    }
  } if (upButtonValue == upButtonValue2) {
    if (backButtonValue != lastBackButtonState) { //Change in State
      if (backButtonValue == LOW) { //Flipped, Low is pressed
        BACK_PRESSED = TRUE; //set flag TRUE
        printf("Back Pressed \r \n ");
      } else {
        BACK_PRESSED = FALSE; //set flag FALSE
      }
      lastBackButtonState = backButtonValue;
    }
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

