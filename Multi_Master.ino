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

// Data Vars
uint32_t P_Dat = 0;
uint16_t pFlag = 0;
uint32_t dataDat = 1;
uint32_t C_Dat = 0;

// Timers
uint32_t runningTimer = 0;
uint32_t delayTimer = 0;
uint32_t pingTimer = 0;
uint32_t dTimer = 0;

// Timer Support
uint8_t pingFlag = 0;

// RF24 Vars
uint16_t nodeID = 0;    // 0 = master
uint8_t pingDat = 1;
uint8_t addrIndex = 0;

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
  uint32_t timeStamp;
} D_Struct;

/**** Helper Fxn Prototypes ****/
void D_Struct_Serial_print(D_Struct);
void C_Struct_Serial_print(C_Struct);
uint16_t pullSensor(int);
void activateSensor(int);
void de_activateSensor(int);
int myDelay(uint32_t, uint32_t*);

/**** Void Setup ****/
void setup() {
  Serial.begin(115200);
  //  printf_begin();
  //Set the LED as an output
  pinMode(LED, OUTPUT);
  pinMode(pushButton, INPUT);

  // Set this node as the master node
  mesh.setNodeID(nodeID);
  Serial.print("Mesh Network ID: "); Serial.println(mesh.getNodeID());
  Serial.println("Mesh Created Using This Node As Master\r\n**********************************\r\n");

  // Initialize & connect to the mesh
  mesh.begin();
}

void loop() {

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

        // Retrieve the ping data from the header and print out the data
        case 'P':
          network.read(header, &P_Dat, sizeof(P_Dat));
          Serial.print("Received 'P' Type Data: "); Serial.println(P_Dat);
          pFlag = 1;
          break;

        // Retrieve ack message that C thresholds have been changed/configured
        case 'C':
          break;

        // Retrieve the data struct for D type messages
        case 'D':
          // Use this data struct to store all the incoming data
          D_Struct D_Dat;
          network.read(header, &D_Dat, sizeof(D_Dat));
          Serial.println("Received 'D' Type Data"); D_Struct_Serial_print(D_Dat);
          Serial.println(F("**********************************\r\n"));
          break;

        // Do not read the header data, instead print the address inidicated by the header type
        default:
          break;
      }
    } else {
      network.read(header, 0, 0);
      Serial.print("Message received, to_node: "); Serial.println(header.to_node);
    }
  }


  /**** 'P' Type Data Evaluation ****/

  if (pFlag) {
    pFlag = 0;
    // Based on the data values, turn on or off the LED
    if (P_Dat) {
      Serial.println("Data Transmission -HIGH-; LED Going -ON-");
      Serial.println(F("**********************************\r\n"));
      digitalWrite(LED, HIGH);
    } else {
      Serial.println("Data Transmission -LOW-; LED Going -OFF-");
      Serial.println(F("**********************************\r\n"));
      digitalWrite(LED, LOW);
    }
  }


  /**** Get Node Data Values ****/

  // Every 2 seconds, if the pushbutton is pressed, the master will send a 'D' message to
  // the current indexed node
  if ((map(analogRead(pushButton), 0, 1023, 0, 33) >= 13) && ((millis() - dTimer) >= 2000)) {
    // Reset the dTimer
    dTimer = millis();

    //Prepare the data to be sent
    RF24NetworkHeader d_header(mesh.addrList[addrIndex].address, 'D');
    // addrIndex will be changed to reflect the a selectable option from the UI
    if (network.write(d_header, &dataDat, sizeof(dataDat))) {
      Serial.println(F("**********************************"));
      Serial.print("Sent 'D' Message To: "); Serial.println(mesh.addrList[addrIndex].nodeID);
    } else {
      if (network.write(d_header, &dataDat, sizeof(dataDat))) {
        Serial.println(F("**********************************"));
        Serial.print("Sent 'D' Message To: "); Serial.println(mesh.addrList[addrIndex].nodeID);
      } else {
        Serial.println(F("**********************************"));
        Serial.print("Failed Send; Attempted to send to: ");
        Serial.println(mesh.addrList[addrIndex].address);
      }
    }
  }


  /**** UI Menu Control ****/


  /**** Ping Data Nodes ****/

  // Tells the master to send out pings every 1 mins
  if ((millis() - pingTimer) >= 6000) {
    pingFlag = 1;
  }

  // Sends out a ping message to a new node every 2.5s
  if ((pingFlag == 1) && ((millis() - runningTimer) >= 2500)) {
    // Reset the runningTimer
    runningTimer = millis();

    // Consecutively send pings to nodes in the network address list to get sensor data
    RF24NetworkHeader p_header(mesh.addrList[addrIndex].address, 'P');
    if (network.write(p_header, &pingDat, sizeof(pingDat))) {
      Serial.println(F("**********************************"));
      Serial.print("Sent Ping To: "); Serial.println(mesh.addrList[addrIndex].nodeID);
      if ((addrIndex + 1) < mesh.addrListTop) {
        Serial.println(mesh.addrListTop);
        addrIndex++;
      } else {
        // Resets all the control variables
        addrIndex = 0;
        pingFlag = 0;
        pingTimer = millis();
      }
    } else {
      if (network.write(p_header, &pingDat, sizeof(pingDat))) {
        Serial.println(F("**********************************"));
        Serial.print("Sent Ping To: "); Serial.println(mesh.addrList[addrIndex].nodeID);
        if ((addrIndex + 1) < mesh.addrListTop) {
          addrIndex++;
        } else {
          // Resets all the control variables
          addrIndex = 0;
          pingFlag = 0;
          pingTimer = millis();
        }
      } else {
        Serial.println(F("**********************************"));
        Serial.print("Failed Send; Attempted to send to: ");
        Serial.println(mesh.addrList[addrIndex].address);
        if ((addrIndex + 1) < mesh.addrListTop) {
          addrIndex++;
        } else {
          // Resets all the control variables
          addrIndex = 0;
          pingFlag = 0;
          pingTimer = millis();
        }
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
} // Loop


/****  HELPER FXNS ****/

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

uint16_t pullSensor(int sensor, int toMax) {
  return (map(analogRead(sensor), 0, 1023, 0, toMax));
}

void activateSensor(int activePin) {
  return (digitalWrite(activePin, 1));
}

void de_activateSensor(int inactivePin) {
  return (digitalWrite(inactivePin, 0));
}

int myDelay(uint32_t delayThresh, uint32_t *prevDelay) {
  uint32_t capn_crunch = prevDelay;
  Serial.print("Capn Crunch: "); Serial.println(capn_crunch);
  if ((millis() - capn_crunch) >= delayThresh) {
    prevDelay = millis();
    return 1;
  } else if ((millis() - capn_crunch) < 0) {
    if (((4294967296 - capn_crunch) + millis()) >= delayThresh) {
      prevDelay = millis();
      return 1;
    }
  }
  return 0;
}
