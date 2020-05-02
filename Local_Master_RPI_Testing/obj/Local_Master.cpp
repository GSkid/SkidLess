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

#define MAX_ELEMENTS 19
#define MOISTURE 0
#define SUNLIGHT 1
#define TEMP  2


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

// Data Vars
D_Struct D_Dat;
D_Struct Test_Data[MAX_ELEMENTS];
uint8_t dFlag = 0;
uint8_t dataDat = 1;
uint8_t column_flag = 0;

// Timers
uint32_t dTimer = 0;
uint32_t frt = 0;

// Timer Support
uint8_t pingFlag = 0;

// RF24 Vars
uint8_t nodeID = 0;    // 0 = master

/**** Helper Fxn Prototypes ****/
int Timer(uint32_t, uint32_t);
void setup(void);
int printGrid(int16_t x0, int16_t x1, int16_t y0, int16_t y1, int16_t xtics, int16_t ytics);
int plotSampleData(D_Struct data[], uint8_t dataType, int16_t size);


/**  Testing OLED Initialization Code***/
void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_ModuleExit();

    exit(0);
}


/**** Void Setup ****/
void setup(void) {
  //Init the GPIO Library
  
  //bcm2835_init();
  
  //bcm2835_spi_begin();  
  
  signal(SIGINT, Handler);
  DEV_ModuleInit();
  Device_Init();
 
    
  // Set this node as the master node
  //printf("I am here \n");
  mesh.setNodeID(nodeID);
  printf("Node ID: %d\n", nodeID);
  radio.setPALevel(RF24_PA_MAX);
  
/*  OLED TESTING COMMENTED here
  // Initialize the mesh and check for proper chip connection
 if (mesh.begin()) {
    printf("\nInitialized: %d\n", radio.isChipConnected());
  }
  
  radio.printDetails();

*/ //    OLED TESTING COMMENTED here
  return;
}

int main(int argc, char **argv) {
  setup();
  while(1) {
    
    //Testing for OLED, commented out RF Code 
/*    OLED TESTING COMMENTED here
    // Keep the network updated
    
     mesh.update();

    // Since this is the master node, we always want to be dynamically assigning
    //    addresses the new nodes
    mesh.DHCP();
 */   //OLED TESTING COMMENTED here

    /**** Check For Available Network Data ****/
/* OLED TESTING COMMENTED here
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
*/ //OLED TESTING COMMENTED here

    /**** 'P' Type Evaluation ****/
 /* OLED TESTING COMMENTED here
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
/* OLED TESTING COMMENTED here
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

/* OLED TESTING COMMENTED here
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
          //fprintf(out, "%19d    ", D_Dat.baroPressure); // prints out 1st member of the data vector to the file.
          fprintf(out, "%13f    ", D_Dat.lightLevel); // prints out 2nd member of the data vector to the file.
          fprintf(out, "%19d    ", D_Dat.temp_C); // prints out 3rd member of the data vector to the file.
          fprintf(out, "%25d    ", D_Dat.digitalOut); // prints out 4th member of the data vector to the file.
          //fprintf(out, "%10d    ", D_Dat.timeStamp); // prints out 5th member of the data vector to the file.
          fprintf(out, "%7d\n", D_Dat.nodeID); // prints out 6th member of the data vector to the file.
          fclose(out);
      }
*/ //OLED TESTING COMMENTED here

      /**** 'S' and 'C' Type Message Responses ****/
/* OLED TESTING COMMENTED here
      // Here we condition on if the node should be sent a configure message instead

      // Send to the message stored in the fromNode nodeID, message type 'S'
      RF24NetworkHeader p_header(mesh.getAddress(D_Dat.nodeID), 'S');
      // Data_Dat is just a 1 telling the node to go to sleep
      if (network.write(p_header, &dataDat, sizeof(dataDat))) {
        printf("Message Returned to %d\n\n", D_Dat.nodeID);
      }
    }

*/ //OLED TESTING COMMENTED here

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
    Set_Color(WHITE);
    
    printf("Testing");
    
    
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
    
      
      //printf("Testing Value 5: %f", Test_Data[5].soilMoisture);
      
     // plotSampleData(Test_Data, MOISTURE, MAX_ELEMENTS);
      
      plotSampleData(Test_Data, SUNLIGHT, MAX_ELEMENTS);
    
    
/*
  
    
    print_String(0,60, (const uint8_t*)"Water%", FONT_5X8);
    print_String(65,120, (const uint8_t*)"Hours", FONT_5X8);
    
    Set_Color(RED);
    //Write_Line(110, 15 , 20, 115);
    
    print_String(10,0, (const uint8_t*)"Moisture Level", FONT_5X8);
    print_String(10,10, (const uint8_t*)"(Node 1)", FONT_5X8);
    
  
    DEV_Delay_ms(30000); //Wait 30 Seconds
*/ 
    
    
    
    
    //Clear_Screen();
    
    //Replot Grid
    
    Set_Color(WHITE);
    
    printGrid(20,120,20,120,10,10);
    
    Set_Color(YELLOW);
    
    //Hard Coded Sunlight Data
    
    
   /* 
    Draw_Pixel(30, 35);
    
    Draw_Pixel(35, 37);
    
    Draw_Pixel(40, 36);
    
    Draw_Pixel(45, 37);    
      
    Draw_Pixel(50, 35);
       
    Draw_Pixel(55, 38);
    
    Draw_Pixel(60, 69);
    
    Draw_Pixel(65, 96);
    
    Draw_Pixel(70, 98);
  
    Draw_Pixel(75, 100);
    
    Draw_Pixel(80, 105);
    
    Draw_Pixel(90, 102);
    
    Draw_Pixel(95, 103);
    
    Draw_Pixel(100, 100);
    
    Draw_Pixel(105, 56);
    
    Draw_Pixel(105, 42);
    
    Draw_Pixel(110, 40);
    
    Draw_Pixel(115, 38);
    
    Draw_Pixel(120, 36);
    
    */
    
    print_String(0,60, (const uint8_t*)"Light%", FONT_5X8);
    print_String(65,120, (const uint8_t*)"Hours", FONT_5X8);
    
    Set_Color(RED);
    //Write_Line(110, 15 , 20, 115);
    
    print_String(10,0, (const uint8_t*)"Light Level", FONT_5X8);
    print_String(10,10, (const uint8_t*)"(Node 1)", FONT_5X8);
    
    DEV_Delay_ms(30000); //Wait 30 Seconds
    
    
    
    
    
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
  
  int incrementX =  ( x1 - x0 ) / xtics ;
  int incrementY =  ( y1 - y0 ) / ytics ;
  
  printf("Xspaces: %d", incrementX);
  printf("Yspaces: %d", incrementY);
  
  //print x-axis
  Write_Line(x0, y1, x1, y1);
  
  //print y-axis  
  Write_Line(x0, y0, x0, y1);
  
  return 0;
  
}

/* @name: plotSampleData 
   @param: dataType 
   @param:  
   
   @return: TRUE/FALSE depending if data was successfully printed
*/

int plotSampleData( D_Struct TestData[], uint8_t dataType, int16_t size){
  
  int i = 0;
  int16_t x_Value = 20; 
  int16_t mapped_y_Value = 0;
  
  if( dataType == MOISTURE){
    
    printf("Plotting Moisture \r\n ");
    
    for(i = 0; i <= size ; i++){
      
      mapped_y_Value = (int16_t)TestData[i].soilMoisture;
   
      printf("Element: %d \r\n", i);
      
      printf("Moisture Value: %f \r\n", TestData[i].soilMoisture);
      
      Draw_Pixel(x_Value, mapped_y_Value);
      
      x_Value += 5;
      
    }
    
  } else if( dataType == SUNLIGHT){
    printf("Plotting Sunlight \r\n ");
    
    for(i = 0; i <= size ; i++){
      
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

/*
    Write_Line(20, 120 , 400, 120);
    
    Write_Line(20, 20, 20, 120);
    
    
    Write_Line(17, 20 , 23, 20);
    
    Write_Line(17, 30 , 23, 30);
    
    Write_Line(17, 40 , 23, 40);
    
    Write_Line(17, 50 , 23, 50);
    
    Write_Line(17, 60 , 23, 60);
    
    Write_Line(17, 70 , 23, 70);
    
    Write_Line(17, 80 , 23, 80);
    
    Write_Line(17, 90 , 23, 90);
    
    Write_Line(17, 100 , 23, 100);
    
    Write_Line(17, 110 , 23, 110);
    

    
    Write_Line(25, 117 , 25, 123);
    
    Write_Line(35, 117 , 35, 123);
    
    Write_Line(45, 117 , 45, 123);
    
    Write_Line(55, 117 , 55, 123);
    
    Write_Line(65, 117 , 65, 123);
    
    Write_Line(75, 117 , 75, 123);
    
    Write_Line(85, 117 , 85, 123);
    
    Write_Line(95, 117 , 95, 123);
    
    Write_Line(105, 117 , 105, 123);
    
    Write_Line(115, 117 , 115, 123);
*/
