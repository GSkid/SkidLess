// ********** INCLUDES **********
#include <SPI.h>
#include <EEPROM.h>
//#include <SD.h>
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <printf.h>

/**** Configure the Radio ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);


/**** GLOBALS ****/
//#define SD_CS 10
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
  uint8_t digitalOut;
  uint32_t timeStamp;
  uint8_t nodeID;
} D_Struct;

// Data Vars
D_Struct D_Dat;
uint8_t dFlag = 0;
uint8_t dataDat = 1;

// Timers
uint32_t dTimer = 0;

// Timer Support
uint8_t pingFlag = 0;

// RF24 Vars
uint16_t nodeID = 0;    // 0 = master

/**** Helper Fxn Prototypes ****/
//void D_Struct_DataLogger(D_Struct, File);
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

  // Setup the SD Card
  //  if (!SD.begin(10)) {
  //    Serial.println("SD Setup Failed");
  //  } else {
  //    Serial.println("SD Setup Success");
  //  }

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
    bool RPD = 0;

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
          // Use the data struct to store data messages and print out the result
          while (network.available() ) {
            //Serial.println( radio.testRPD() ? "Strong Signal > 64dBm" : "Weak Signal < 64dBm");
            RPD = radio.testRPD();
            network.read(header, &D_Dat, sizeof(D_Dat));
          }
          dFlag = 1;
          Serial.println(F("**********************************"));
          Serial.println( RPD ? "Strong Signal > 64dBm" : "Weak Signal < 64dBm");
          Serial.println("Received 'D' Type Data"); D_Struct_Serial_print(D_Dat);
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

  if (dFlag) {
    dFlag = 0;
    /* Based on the data values, turn on or off the LED */
    if (D_Dat.digitalOut) {
      Serial.println("...Data Transmission -HIGH-; LED Going -ON-\r\n");
      digitalWrite(LED, HIGH);
    } else {
      Serial.println("...Data Transmission -LOW-; LED Going -OFF-\r\n");
      digitalWrite(LED, LOW);
    }


    /**** Write Data Values to SD Card ****/

    //    if (!SD.begin(10)) {
    //      Serial.println("SD Setup Failed");
    //    } else {
    //      Serial.println("SD Setup Success");
    //    }
    //    File dataLog;
    //    dataLog = SD.open("DLOG.TXT", FILE_WRITE);
    //    if (dataLog) {
    //      Serial.println("Writing D_Struct To SD Card");
    //      D_Struct_Serial_print(D_Dat);
    //      D_Struct_DataLogger(D_Dat, dataLog);
    //      dataLog.close();
    //    } else {
    //      Serial.println("Error Writing To SD Card");
    //    }
    //    SD.end();


    /**** 'S' and 'C' Type Message Responses ****/

    Serial.print("Sending Sleep Message Back to: "); Serial.println(D_Dat.nodeID, OCT);
    // Here we condition on if the node should be sent a configure message instead

    // Send to the message stored in the fromNode nodeID, message type 'S'
    RF24NetworkHeader p_header(mesh.getAddress(D_Dat.nodeID), 'S');
    // Data_Dat is just a 1 telling the node to go to sleep
    if (network.write(p_header, &dataDat, sizeof(dataDat))) {
      Serial.println("Sleep Message Sent");
    } else {
      Serial.println("Sleep Message Failed To Send");
    }
    Serial.println(F("**********************************\r\n"));
  }


  /**** Get Node Data Values ****/

  // Every 2 seconds, if the pushbutton is pressed, the master will send a 'D' message to
  // the current indexed node
  if ((map(analogRead(pushButton), 0, 1023, 0, 33) >= 13) && ((millis() - dTimer) >= 2000)) {
    // Reset the dTimer
    dTimer = millis();

    //Prepare the data to be sent
    RF24NetworkHeader d_header(mesh.addrList[1].address, 'D');
    // addrIndex will be changed to reflect the a selectable option from the UI
    if (network.write(d_header, &dataDat, sizeof(dataDat))) {
      Serial.println(F("**********************************"));
      Serial.print("Sent 'D' Message To: "); Serial.println(mesh.addrList[1].nodeID);
    } else {
      Serial.println(F("**********************************"));
      Serial.print("Failed Send; Attempted to send to: ");
      Serial.println(mesh.addrList[1].address);
    }
  }


  /**** UI Menu Control ****/

} // Loop


/****  HELPER FXNS ****/

//void D_Struct_DataLogger(D_Struct sct, File dataFile) {
//  String dataString = "";
//  dataString += sct.soilMoisture; dataString += ",";
//  dataString += sct.baroPressure; dataString += ",";
//  dataString += sct.lightLevel; dataString += ",";
//  dataString += sct.temp_C; dataString += ",";
//  dataString += sct.digitalOut; dataString += ",";
//  dataString += sct.timeStamp; dataString += ",";
//  dataString += sct.nodeID;
//  dataFile.println(dataString);
//  return;
//}

void C_Struct_Serial_print(C_Struct sct) {
  Serial.print("Soil Moisture Threshold: "); Serial.println(sct.sM_thresh);
  Serial.print("Barometric Pressure Threshold: "); Serial.println(sct.bP_thresh);
  Serial.print("Ambient Light Level Threshold: "); Serial.println(sct.lL_thresh);
  Serial.print("Ambient Temperature Threshold: "); Serial.println(sct.t_thresh);
  return;
}

void D_Struct_Serial_print(D_Struct sct) {
  Serial.print("Soil Moisture Level (V ): "); Serial.println(sct.soilMoisture);
  Serial.print("Barometric Pressure (Pa): "); Serial.println(sct.baroPressure);
  Serial.print("Ambient Light Level (V ): "); Serial.println(sct.lightLevel);
  Serial.print("Ambient Temperature (C ): "); Serial.println(sct.temp_C);
  Serial.print("Calucated Digital Output: "); Serial.println(sct.digitalOut);
  Serial.print("Watering Time Stamp (ms): "); Serial.println(sct.timeStamp);
  Serial.print("Node ID: "); Serial.println(sct.nodeID);
  Serial.print("Master Time Stamp (ms): "); Serial.println(millis());
  return;
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
