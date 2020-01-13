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
#define pushButton A0

uint8_t nodeID = 1;    // Set this to a different number for each node in the mesh network
uint32_t runningTimer = 0;
uint8_t dataFlag = 0;


void setup() {
  Serial.begin(115200);
  //  printf_begin();

  //Set the LED as an output
  pinMode(LED, OUTPUT);
  pinMode(pushButton, INPUT);

  // Set this node as the master node
  mesh.setNodeID(nodeID);
  Serial.println(mesh.getNodeID());

  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}

void loop() {

  // Keep the network updated
  mesh.update();

  //  if (!mesh.checkConnection()) {
  //    // Refresh network address
  //    mesh.renewAddress();
  //  }

  /**** Network Data Loop ****/
  // Check for incoming data from other nodes
  if (network.available()) {

    // Create a header var to store incoming network header
    RF24NetworkHeader header;
    // Get the data from the current header
    network.peek(header);

    // Use this var to store the header data
    uint32_t dat = 0;
    //    Serial.println(header.type);

    // Switch on the header type, we only want the data if addressed to the master
    switch (header.type) {
      // Retrieve the data from the header and print out the data
      case 'P': network.read(header, &dat, sizeof(dat));
        Serial.println(F("**********************************"));
        Serial.print("Received data: ");
        Serial.println(dat);
        dataFlag = dat;
        break;

      // Do not read the header data, instead print the address inidicated by the header type
      default: network.read(header, 0, 0);
        Serial.print("Received data for node: ");
        Serial.println(header.to_node);
        Serial.println(F("**********************************\r\n"));
        break;
    }

    // Based on the data values, turn on or off the LED
    if (dat > 12) {
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
    }
  }

  /**** IO Menu Control ****/



  /**** Sensor Data Transmission ****/

  // Only sends data every second
  if (dataFlag == 1) {

    // Resets the dataFlag to 0 to indicate the data has been consumed
    dataFlag = 0;

    // Sets a variable to the voltage of the pushbutton and maps it into mV
    uint16_t miso_soup = 0;
    miso_soup = map(analogRead(pushButton), 0, 1023, 0, 33);
    Serial.print("Mapped Miso Soup: ");
    Serial.println(miso_soup);

    // Sets a high or low variable based on the input of the pushbutton
    // We get this bread if miso_soup is high, otherwise we let someone else get this bread
    uint16_t bread;
    if (miso_soup > 12) {
      bread = 1;
    } else {
      bread = 0;
    }

    // Sends the data up through the mesh to the master node to be evaluated
    if (!mesh.write(&bread, '1', sizeof(bread), 0)) {
      Serial.println("Send failed; preparing for second attempt");
      if (mesh.write(&bread, '2', sizeof(bread), 0)) {
        Serial.print("Sending Bread to Master: "); Serial.println(bread);
        Serial.print("Message type successfully sent: "); Serial.println("50");
        Serial.println(F("**********************************\r\n"));
      } else {
        Serial.println("Second attempt failed. Aborting...");
        if (mesh.write(&bread, '3', sizeof(bread), 0)) {
          Serial.print("Sending Bread to Master: "); Serial.println(bread);
          Serial.print("Message type successfully sent: "); Serial.println("51");
          Serial.println(F("**********************************\r\n"));
        } else {
          if (!mesh.checkConnection()) {
            mesh.renewAddress();
            Serial.println("Re-initializing the network ID...");
            Serial.print("New network ID: "); Serial.println(mesh.getNodeID());
          }
        }
      }
    } else {
      Serial.print("Sending Bread to Master: "); Serial.println(bread);
      Serial.print("Message type successfully sent: "); Serial.println("49");
      Serial.println(F("**********************************\r\n"));
    }
  }
} // Loop
