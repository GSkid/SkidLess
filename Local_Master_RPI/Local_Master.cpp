#ifndef __cplusplus
#define __cplusplus
#endif

// ********** INCLUDES **********
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

/**** GLOBALS ****/
#define LED RPI_BPLUS_GPIO_J8_07
#define pushButton RPI_BPLUS_GPIO_J8_29
#define SPI_SPEED_2MHZ 2000000
#define FORECAST_CALL 1800000

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

#define ONE_SECOND 1000
#define HUNDRED_MILLI 100

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

typedef struct{
    int precipProb;
    int temperature;
    int humidity;
    int pressure;
    int windSpeed;
    int windBearing;
} Forecast;

//States for Water Delivery SM
typedef enum {
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


// Data Vars
D_Struct D_Dat;
D_Struct Test_Data[MAX_ELEMENTS];
uint8_t dFlag = 0;
uint8_t dataDat = 1;
uint8_t column_flag = 0;

// Timers
uint32_t dTimer = 0;
uint32_t wTimer = 0; //Timer for driving Water Delivery
uint32_t frt = 0;
uint32_t forecastTimer = 0;

// Timer Support
uint8_t pingFlag = 0;

// RF24 Vars
uint8_t nodeID = 0;    // 0 = master

// Forecast Support
Forecast Forecast1;
char buffer[10];
double data[6];
FILE* fp;


/**** Helper Fxn Prototypes ****/
int Timer(uint32_t, uint32_t);
void setup(void);
int printGrid(int16_t x0, int16_t x1, int16_t y0, int16_t y1, int16_t xtics, int16_t ytics);
int plotSampleData(D_Struct data[], uint8_t dataType, int16_t size);
int WaterDeliverySM(w_State state, uint8_t status, uint32_t delayP_N, uint32_t pulseTime);
void LPMOS_Set(uint8_t status);
void RPMOS_Set(uint8_t status);
void LNMOS_Set(uint8_t status);
void RNMOS_Set(uint8_t status);



/**** Void Setup ****/
void setup(void) {
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

      /**** Write Data Values to SD Card ****/

      {
          FILE* out = fopen("Data_Log.txt", "a");
 
          // prints out main column headers for the data file.
          // conditional here: output if first loop, dont afterward, controlled by column_flag
          if (column_flag == 0)
          {
              fprintf(out, "Soil Moisture,   Ambient Light,   Ambient Temperature,   Calculated Digital Output,   Node ID,   \n");
              //Barometric Pressure:   
              //Time Stamp:   
              column_flag = 1;
          }

          printf("%f, %f, %d, %d, %d,\n", D_Dat.soilMoisture, D_Dat.lightLevel, D_Dat.temp_C, D_Dat.digitalOut, D_Dat.nodeID);
          //%d, D_Dat.baroPressure, 
          //%d, D_Dat.timeStamp, 

          fprintf(out, "%13f,   ", D_Dat.soilMoisture); // prints out 0th member of the data vector to the file.
          //fprintf(out, "%19d,    ", D_Dat.baroPressure); // prints out 1st member of the data vector to the file.
          fprintf(out, "%13f,   ", D_Dat.lightLevel); // prints out 2nd member of the data vector to the file.
          fprintf(out, "%19d,   ", D_Dat.temp_C); // prints out 3rd member of the data vector to the file.
          fprintf(out, "%25d,   ", D_Dat.digitalOut); // prints out 4th member of the data vector to the file.
          //fprintf(out, "%10d,    ", D_Dat.timeStamp); // prints out 5th member of the data vector to the file.
          fprintf(out, "%7d,\n", D_Dat.nodeID); // prints out 6th member of the data vector to the file.
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

    /**** Forecast Data API Call ****/

    if (Timer(FORECAST_CALL, forecastTimer)) {
        printf("Opening call to forecast API...\n");
        forecastTimer = millis();
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
  
  printf("Xspaces: %d", incrementX);
  printf("Yspaces: %d", incrementY);
  
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
   
      printf("Element: %d \r\n", i);
      
      printf("Moisture Value: %f \r\n", TestData[i].soilMoisture);
      
      Draw_Pixel(x_Value, mapped_y_Value);
      
      x_Value += 5;
      
    }
    
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
   
      printf("Element: %d \r\n", i);
      
      printf("Light Level Value: %f \r\n", TestData[i].lightLevel);
      
      Draw_Pixel(x_Value, mapped_y_Value);
      
      x_Value += 5;
      
    }
    
  } else if( dataType == TEMP){
    printf("Plotting Temperature \r\n ");
    
    for(i = 0; i <= size ; i++){
      
      mapped_y_Value = (int16_t)TestData[i].temp_C;
   
      printf("Element: %d \r\n", i);
      
      printf("Temp Value: %d \r\n", TestData[i].temp_C);
      
      Draw_Pixel(x_Value, mapped_y_Value);
      
      x_Value += 5;
      
    }
    
  } else {
    printf(" No Plot Selected \r\n ");
    
    
  }  

  return 0;
  
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
   @param: state - current state of waterDelivery Driving
   @param: status - whether to turn on or off WD
   @param: delayP_N - delay time between turning ON/OFF PFET and NFET
   @param: pulseTime - Time for +/-5V Pulse, Delays time between ON and OFF 
   @return: 1/0 depending on whether drive was completed
*/
int WaterDeliverySM(w_State state, uint8_t status, uint32_t delayP_N, uint32_t pulseTime){
  w_State nextState = state;
  int hoseSet = 0; // Set to 1 once done Driving 
  
  switch(state){
    case HOSE_IDLE:
      if (status == WATER_ON){
        nextState = HOSE_ON_S1;
        wTimer = bcm2835_millis();
        hoseSet = 0; 
      }
      break;
      
    case HOSE_ON_S1:
      LNMOS_Set(NMOS_ON);
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S2;
      }
      break;
    
    case HOSE_ON_S2:
      RPMOS_Set(PMOS_ON); 
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S3;
      }
      break;
      
    case HOSE_ON_S3:
      RPMOS_Set(PMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S4;
      }
      break;
      
    case HOSE_ON_S4:
      LNMOS_Set(NMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        nextState = HOSE_IDLE;
        hoseSet = 1;
      }
      break;
    
    case HOSE_OFF_S1:
      RNMOS_Set(NMOS_ON);
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S2;
      }
      break;
    
    case HOSE_OFF_S2:
      LPMOS_Set(PMOS_ON); 
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S3;
      }
      break;
      
    case HOSE_OFF_S3:
      LPMOS_Set(PMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S4;
      }
      break;
      
    case HOSE_OFF_S4:
      RNMOS_Set(NMOS_OFF); 
      if (Timer(delayP_N, wTimer)){
        nextState = HOSE_IDLE;
        hoseSet = 1;
      }
      break;
    
  }
    
  state = nextState;
  return hoseSet;  //1 if set, 0 if still in S1-4

}

