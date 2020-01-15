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

// Timers
uint32_t runningTimer = 0;

// Sensor Vars
uint16_t miso_soup = 0;
uint16_t bread;

// RF24 Vars
uint8_t nodeID = 1;    // Set this to a different number for each node in the mesh network
uint8_t dataFlag = 0;
uint16_t meshAddr = 0;

// Use these vars to store the header data
uint32_t P_Dat = 0;
uint32_t D_Dat = 0;
uint32_t C_Dat = 0;

// C_Struct stores relevant thresholds
typedef struct {
  uint16_t sM_thresh;
  uint16_t bP_thresh;
  uint16_t lL_thresh;
  uint16_t t_thresh;
  uint8_t thresh_configure;
} C_Struct;

// D_Struct stores the relevant sensor data
typedef struct {
  uint16_t soilMoisture;
  uint16_t baroPressure;
  uint16_t lightLevel;
  uint16_t temp;
  uint16_t timeStamp;
} D_Struct;

// C and D type structs
C_Struct Thresholds;
D_Struct Data_Struct;

/**** Function Prototypes ****/
void D_Struct_Serial_print(D_Struct);
void C_Struct_Serial_print(C_Struct);


void setup() {
  Serial.begin(115200);
  //  printf_begin();

  //Set the LED as an output
  pinMode(LED, OUTPUT);
  pinMode(pushButton, INPUT);

  // Set this node as the master node
  mesh.setNodeID(nodeID);
  Serial.print("Mesh Network ID: ");
  Serial.println(mesh.getNodeID());

  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
  meshAddr = mesh.getAddress(nodeID);
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
    //    Serial.println(header.type);

    // Ensure the message is addressed to this node
    if (header.to_node == meshAddr) {

      // Switch on the header type, we only want the data if addressed to the master
      switch (header.type) {

        // 'P' Type messages ask the sensor to read and send sensor data after evals
        case 'P':
          network.read(header, &P_Dat, sizeof(P_Dat));
          Serial.println(F("**********************************"));
          Serial.print("Received 'P' Type Message: "); Serial.println(P_Dat);
          break;

        // 'C' Type messages tell the sensor to calibrate or change its thresholds
        case 'C':
          network.read(header, &C_Dat, sizeof(C_Dat));
          Serial.println(F("**********************************"));
          Serial.print("Received 'C' Type Message: "); Serial.println(C_Dat);
          Serial.println(F("**********************************\r\n"));

        // 'D' Type messages query the node to send back all sensor data as a D_Struct
        case 'D':
          network.read(header, &D_Dat, sizeof(D_Dat));
          Serial.println(F("**********************************"));
          Serial.print("Received 'D' Type Message: "); Serial.println(D_Dat);
        default:
          break;
      }
    } else {
      // Do not read the header data
      // Instead print the address inidicated by the header type
      network.read(header, 0, 0);
      Serial.print("Received data for node: ");
      Serial.println(header.to_node);
      Serial.println(F("**********************************\r\n"));
    }
  }


  /**** Read Sensors ****/

  if (D_Dat | P_Dat) {
    // Read all sensors
    // Sets a variable to the voltage of the pushbutton and maps it into mV
    miso_soup = map(analogRead(pushButton), 0, 1023, 0, 33);
    Serial.print("Mapped Miso Soup: ");
    Serial.println(miso_soup);

    // Sets a high or low variable based on the input of the pushbutton
    // We get this bread if miso_soup is high, otherwise we let someone else get this bread
    if (miso_soup > 12) {
      bread = 1;
    } else {
      bread = 0;
    }
  }

  /**** 'C' Type Data Evaluation ****/

  // Based on the 'C' type data, re-configure the thresholds


  /**** 'D' Type Data Evaluation ****/

  // Based on the 'D' type data, send the data to M
  if (D_Dat) {
    // Reset D_Dat
    D_Dat = 0;

    // Read and store all data types
    Data_Struct.timeStamp = millis();

    // Send the D_Struct to M
    // Sends the data up through the mesh to the master node to be evaluated
    if (!mesh.write(&Data_Struct, 'D', sizeof(Data_Struct), 0)) {
      Serial.println("Send failed; preparing for second attempt");
      if (mesh.write(&Data_Struct, 'D', sizeof(Data_Struct), 0)) {
        Serial.println("Sending Data to Master"); D_Struct_Serial_print(Data_Struct);
        Serial.println(F("**********************************\r\n"));
      } else {
        Serial.println("Second attempt failed. Aborting...");
        if (mesh.write(&Data_Struct, 'D', sizeof(Data_Struct), 0)) {
          Serial.println("Sending Data to Master"); D_Struct_Serial_print(Data_Struct);
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
      Serial.println("Sending Data to Master"); D_Struct_Serial_print(Data_Struct);
      Serial.println(F("**********************************\r\n"));
    }
  }

  /**** Config Options ****/



  /**** Sensor Data Transmission ****/

  // Only sends data when it receives a P_Dat signal
  if (P_Dat) {

    // Resets the dataFlag to 0 to indicate the data has been consumed
    P_Dat = 0;

    // Sends the data up through the mesh to the master node to be evaluated
    if (!mesh.write(&bread, 'P', sizeof(bread), 0)) {
      Serial.println("Send failed; preparing for second attempt");
      if (mesh.write(&bread, 'P', sizeof(bread), 0)) {
        Serial.print("Sending Bread to Master: "); Serial.println(bread);
        Serial.println(F("**********************************\r\n"));
      } else {
        Serial.println("Second attempt failed. Aborting...");
        if (mesh.write(&bread, 'P', sizeof(bread), 0)) {
          Serial.print("Sending Bread to Master: "); Serial.println(bread);
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
      Serial.println(F("**********************************\r\n"));
    }
  }
} // Loop


/**** Helper Functions ****/

void C_Struct_Serial_print(C_Struct sct) {
  Serial.print("Soil Moisture Threshold: "); Serial.println(sct.sM_thresh);
  Serial.print("Barometric Pressure Threshold: "); Serial.println(sct.bP_thresh);
  Serial.print("Ambient Light Level Threshold: "); Serial.println(sct.lL_thresh);
  Serial.print("Ambient Temperature Threshold: "); Serial.println(sct.t_thresh);
}

void D_Struct_Serial_print(D_Struct sct) {
  Serial.print("Soil Moisture Level: "); Serial.println(sct.soilMoisture);
  Serial.print("Barometric Pressure: "); Serial.println(sct.baroPressure);
  Serial.print("Ambient Light Level: "); Serial.println(sct.lightLevel);
  Serial.print("Ambient Temperature: "); Serial.println(sct.temp);
  Serial.print("Time Stamp: "); Serial.println(sct.timeStamp);
}
