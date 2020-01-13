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
  Serial.print("Mesh Network ID: "); Serial.println(mesh.getNodeID());
  Serial.println("\r\n Mesh Created Using This Node As Master\r\n**********************************\r\n");

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
    switch (header.to_node) {
      // Retrieve the data from the header and print out the data
      case 0: network.read(header, &dat, sizeof(dat));
        Serial.print("Received Incoming Data: "); Serial.println(dat);
        Serial.print("Message Type: "); Serial.println(header.type);
        break;

      // Do not read the header data, instead print the address inidicated by the header type
      default: network.read(header, 0, 0);
        Serial.print("Message received, type: "); Serial.println(header.type);
        break;
    }

    // Based on the data values, turn on or off the LED
    if (dat == 1) {
      Serial.println("Data Transmission -HIGH-; LED Going -ON-");
      Serial.println(F("**********************************\r\n"));
      digitalWrite(LED, HIGH);
    } else {
      Serial.println("Data Transmission -LOW-; LED Going -OFF-");
      Serial.println(F("**********************************\r\n"));
      digitalWrite(LED, LOW);
    }
  }

  /**** IO Menu Control ****/


  /**** Ping Data Nodes ****/

  // Sends out a ping message to a new node every second
  if ((millis() - runningTimer) >= 2500) {
    // Reset the runningTimer
    runningTimer = millis();

    // Consecutively send pings to nodes in the network address list to get sensor data
    uint8_t addrIndex = 0;
    RF24NetworkHeader p_header(mesh.addrList[addrIndex].address,'P');
    if (network.write(p_header, &pingDat, sizeof(pingDat))) {  
      Serial.println(F("**********************************"));
      Serial.print("Sent Ping To: "); Serial.println(mesh.addrList[addrIndex].nodeID);
      if (addrIndex < mesh.addrListTop) {
        addrIndex++;
      } else {
        addrIndex = 0;
      }
    } else {
      Serial.println(F("**********************************"));
      Serial.print("Failed Send; Attempted to send to: ");
      Serial.println(mesh.addrList[addrIndex].address);
//      Serial.println(F("********Assigned Addresses********"));
//      for (int i = 0; i < mesh.addrListTop; i++) {
//        Serial.print("NodeID: ");
//        Serial.print(mesh.addrList[i].nodeID);
//        Serial.print(" RF24Network Address: 0");
//        Serial.println(mesh.addrList[i].address, OCT);
//      }
//      Serial.println(F("**********************************\r\n"));
    }
  }
}
