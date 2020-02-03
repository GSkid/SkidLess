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
  uint16_t temp_C;
  uint16_t temp_F;
  uint32_t timeStamp;
} D_Struct;

// P_Struct stores the from_node nodeID and data value
typedef struct {
  uint16_t fromNode;
  uint8_t digitalDat;
} P_Struct;

// Data Vars
P_Struct P_Dat;
D_Struct D_Dat;
uint8_t pFlag = 0;
uint8_t dataDat = 1;

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

/**** Helper Fxn Prototypes ****/
void D_Struct_Serial_print(D_Struct);
void C_Struct_Serial_print(C_Struct);
int Timer(uint32_t, uint32_t);

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
          // Use this ping struct to store data and resend the sleep ping
          network.read(header, &P_Dat, sizeof(P_Dat));
          Serial.print("Received 'P' Type Message ");
          Serial.print("From Node: "); Serial.println(P_Dat.fromNode, OCT);
          Serial.print("Digital Data Value: "); Serial.println(P_Dat.digitalDat);
          pFlag = 1;
          break;
          
        // Retrieve the data struct for D type messages
        case 'D':
          // Use the data struct to store data messages and print out the result
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


  /**** 'P' Type Evaluation ****/

  if (pFlag && !network.available()) {
    pFlag = 0;
    /* Based on the data values, turn on or off the LED */
    if (P_Dat.digitalDat) {
      Serial.println("Data Transmission -HIGH-; LED Going -ON-\r\n");
      digitalWrite(LED, HIGH);
    } else {
      Serial.println("Data Transmission -LOW-; LED Going -OFF-\r\n");
      digitalWrite(LED, LOW);
    }

    /**** 'S' and 'C' Type Message Responses ****/
    
    Serial.print("Sending Sleep Message Back to: "); Serial.println(P_Dat.fromNode, OCT);
    // Here we condition on if the node should be sent a configure message instead
    
    // Send to the message stored in the fromNode nodeID, message type 'S'
    RF24NetworkHeader p_header(mesh.getAddress(P_Dat.fromNode), 'S');
    // Data_Dat is just a 1 telling the node to go to sleep
    if (network.write(p_header, &dataDat, sizeof(dataDat))) {
      Serial.println("Sleep Message Sent");
    }
      Serial.println("Sleep Message Failed To Send");
    Serial.println(F("**********************************\r\n"));
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
      Serial.println(F("**********************************"));
      Serial.print("Failed Send; Attempted to send to: ");
      Serial.println(mesh.addrList[addrIndex].address);
    }
  }


  /**** UI Menu Control ****/
  
} // Loop


/****  HELPER FXNS ****/

void C_Struct_Serial_print(C_Struct sct) {
  Serial.print("Soil Moisture Threshold: "); Serial.println(sct.sM_thresh);
  Serial.print("Barometric Pressure Threshold: "); Serial.println(sct.bP_thresh);
  Serial.print("Ambient Light Level Threshold: "); Serial.println(sct.lL_thresh);
  Serial.print("Ambient Temperature Threshold: "); Serial.println(sct.t_thresh);
}

void D_Struct_Serial_print(D_Struct sct) {
  Serial.print("Soil Moisture Level (V ): "); Serial.println(sct.soilMoisture);
  Serial.print("Barometric Pressure (Pa): "); Serial.println(sct.baroPressure);
  Serial.print("Ambient Light Level (V ): "); Serial.println(sct.lightLevel);
  Serial.print("Ambient Temperature (C ): "); Serial.println(sct.temp_C);
  Serial.print("Ambient Temperature (F ): "); Serial.println(sct.temp_F);
  Serial.print("Time Stamp (ms): "); Serial.println(sct.timeStamp);
}


/* @name: Timer
   @param: delayThresh - timer duration
   @param: prevDelay - time in millis() when the timer started
   @return: digital high/low depending if timer elapsed or not
   This is a non-blocking timer that handles uint32_t overflow,
   it works off the internal function millis() as reference
*/
int Timer(uint32_t delayThresh, uint32_t prevDelay) {
  // Checks if the current time is at or beyond the set timer
  if ((millis() - prevDelay) >= delayThresh) {
    return 1;
  } else if (millis() < prevDelay) {
    //Checks and responds to overflow of the millis() timer
    if (((4294967296 - prevDelay) + millis()) >= delayThresh) {
      return 1;
    }
  }
  return 0;
}
