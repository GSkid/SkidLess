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

// Data Array


/**** Helper Fxn Prototypes ****/
int Timer(uint32_t, uint32_t);
void setup(void);

/**** Void Setup ****/
void setup(void) {
  //Init the GPIO Library
  
  bcm2835_init();
  
  bcm2835_spi_begin();  

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

