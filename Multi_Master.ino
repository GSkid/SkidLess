// ********** INCLUDES **********
#include <SPI.h>
#include <EEPROM.h>
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <printf.h>

/**** Configure the Radio ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);


/**** GLOBALS ****/
#define LED 2

uint16_t nodeID = 0;    // 0 = master
uint32_t runningTimer = 0;
uint8_t pingDat = 1;


void setup() {
  Serial.begin(115200);
  //  printf_begin();
  //Set the LED as an output
  pinMode(LED, OUTPUT);

  // Set this node as the master node
  mesh.setNodeID(nodeID);
  Serial.println(mesh.getNodeID());

  // Initialize & connect to the mesh
  mesh.begin();
}

void loop() {

  // Keep the network updated
  mesh.update();

  // Since this is the master node, we always want to be dynamically assigning
  //    addresses the new nodes
  mesh.DHCP();

  /**** Network Data Loop ****/

  // Check for incoming data from other nodes
  if (network.available()) {

    // Create a header var to store incoming network header
    RF24NetworkHeader header;
    // Get the data from the current header
    network.peek(header);


    // Use this var to store the header data
    uint32_t dat = 0;

    // Switch on the header type, we only want the data if addressed to the master
    switch (header.type) {
      // Retrieve the data from the header and print out the data
      case 'M': network.read(header, &dat, sizeof(dat));
        Serial.print("Received Incoming Data: ");
        Serial.println(dat);
        break;

      // Do not read the header data, instead print the address inidicated by the header type
      default: network.read(header, 0, 0);
        Serial.println(header.type);
        break;
    }

    // Based on the data values, turn on or off the LED
    if (dat == 1) {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
    }
  }

  /**** IO Menu Control ****/


  /**** Ping Data Nodes ****/

  // Sends out a ping message to a new node every second
  if ((millis() - runningTimer) >= 1000) {
    // Reset the runningTimer
    runningTimer = millis();

    // Consecutively send pings to nodes in the network address list to get sensor data
    uint8_t addrIndex = 0;
    RF24NetworkHeader p_header(mesh.addrList[addrIndex].address,'P');
    if (network.write(p_header, &pingDat, sizeof(pingDat))) {  
      Serial.print("******* Sent Ping To: "); Serial.print(mesh.addrList[addrIndex].nodeID);
      Serial.println(" *******\r\n");
      if (addrIndex < mesh.addrListTop) {
        addrIndex++;
      } else {
        addrIndex = 0;
      }
    } else {
      Serial.print("Failed Send; Attempted to send to: ");
      Serial.println(mesh.addrList[addrIndex].address);
//      Serial.println(F("********Assigned Addresses********"));
//      for (int i = 0; i < mesh.addrListTop; i++) {
//        Serial.print("NodeID: ");
//        Serial.print(mesh.addrList[i].nodeID);
//        Serial.print(" RF24Network Address: 0");
//        Serial.println(mesh.addrList[i].address, OCT);
//      }
      Serial.println(F("**********************************"));
    }
  }
}
