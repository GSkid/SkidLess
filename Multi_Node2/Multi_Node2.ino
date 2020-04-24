// ********** INCLUDES **********
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RF24.h"
#include "nRF24L01.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include "Adafruit_BMP085.h"
#include <printf.h>
#include <avr/sleep.h>
#include <avr/power.h>

/**** Configure the Radio ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**** #Defines ****/
#define time_Thresh Timer(C_Thresh.time_thresh, 10)//D_Struct.timeStamp)

/**** GLOBALS ****/
#define nodeID 3 // Set this to a different number for each node in the mesh network
#define MOISTURE_PIN A1
#define LIGHT_PIN A2
#define LIQUID_SENSE 10000
#define INTERRUPT_MASK 0b01000000

// C_Struct stores relevant thresholds
typedef struct {
  float sM_thresh;
  float sM_thresh_00;
  //uint8_t bP_thresh;
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
  uint8_t node_ID;
} D_Struct;

// Timers
uint32_t sleepTimer = 0;
uint32_t messageTimer = 0;
uint32_t witchTimer = 60000;

// Timer Support
uint8_t timerFlag = 0;
uint8_t message_Flag = 0;

// Sensor Vars
Adafruit_BMP085 bmp;

// RF24 Vars
uint8_t sleepFlag = 0;

// Use these vars to store the header data
uint8_t M_Dat = 0;

// C and D type structs
C_Struct Thresholds;
D_Struct Data_Struct;

/**** Function Prototypes ****/
void D_Struct_Serial_print(D_Struct);
void C_Struct_Serial_print(C_Struct);
void initC_Struct(C_Struct*);
float pullMoistureSensor(void);
float pullLightSensor(void);
int Timer(uint32_t, uint32_t);
int run_DeepOcean(D_Struct, C_Struct);


void setup() {
  Serial.begin(115200);
  printf_begin();

  // Set the IO
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(LIGHT_PIN, INPUT);

  // Begin the Barometric Pressure Sensor
  // Pin out: Vin->5V, SCL->A5, SDA->A4
  bmp.begin();

  // Set this node as the master node
  mesh.setNodeID(nodeID);

  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();

  // Print out the mesh addr
  Serial.print(F("Mesh Network ID: "));
  Serial.println(mesh.getNodeID());
  Serial.print(F("Mesh Address: ")); Serial.println(mesh.getAddress(nodeID));
  radio.setPALevel(RF24_PA_MAX);

  //  radio.printDetails();
  Serial.println(F("**********************************\r\n"));

  // initialize the thresholds
  initC_Struct(&Thresholds);
  C_Struct_Serial_print(Thresholds);
  Serial.print(F("\n"));

  // Setting the watchdog timer
  set_sleep_mode(SLEEP_MODE_IDLE);
  network.setup_watchdog(9);
  sleepFlag = 1;
}

void loop() {

  // Keep the network updated
  mesh.update();



  /**** Network Data Loop ****/
  // Check for incoming data from other nodes
  if (network.available()) {

    // Create a header var to store incoming network header
    RF24NetworkHeader header;
    // Get the data from the current header
    network.peek(header);

    // Switch on the header type, we only want the data if addressed to the master
    switch (header.type) {

      // 'S' Type messages ask the sensor to read and send sensor data after evals
      case 'S':
        network.read(header, &M_Dat, sizeof(M_Dat));
        Serial.print(F("\r\n"));
        Serial.print(F("Received 'S' Type Message: ")); Serial.println(M_Dat);
        break;

      // 'C' Type messages tell the sensor to calibrate or change its thresholds
      case 'C':
        network.read(header, &M_Dat, sizeof(M_Dat));
        Serial.print(F("\r\n"));
        Serial.print(F("Received 'C' Type Message: ")); Serial.println(M_Dat);
    }
  }

  /**** Read Sensors ****/

  if (sleepFlag) {
    sleepFlag = 0;

    // Read all sensors
    Data_Struct.soilMoisture = pullMoistureSensor();
    //Data_Struct.baroPressure = 0; //change to bmp.readPressure();
    Data_Struct.lightLevel = pullLightSensor();
    Data_Struct.temp_C = bmp.readTemperature();
    Data_Struct.digitalOut = run_DeepOcean(Data_Struct, Thresholds); // will be replaced by DeepOcean
    //    if (Data_Struct.digitalOut) {
    //      Data_Struct.timeStamp = millis();
    //    }
    Data_Struct.node_ID = nodeID;


    /**** Data Transmission ****/

    if (mesh.checkConnection()) {
      // Send the D_Struct to Master
      // Sends the data up through the mesh to the master node to be evaluated
      if (!mesh.write(&Data_Struct, 'D', sizeof(Data_Struct), 0)) {
        Serial.println(F("Send failed; checking network connection."));
        // Check if still connected
        if (!mesh.checkConnection()) {
          // Reconnect to the network if disconnected and no send
          Serial.println(F("Re-initializing the Network Address..."));
          mesh.renewAddress();
          Serial.print(F("New Network Addr: ")); Serial.println(mesh.getAddress(nodeID));
        } else {
          Serial.println(F("Network connection good."));
          Serial.println(F("**********************************\r\n"));
          sleepFlag = 1;
        }
      } else {
        Serial.println(F("**********************************"));
        Serial.println(F("Sending Data to Master")); D_Struct_Serial_print(Data_Struct);
        // Set the flag to check for a failed message response
        message_Flag = 1; messageTimer = millis();
      }
    } else {
      // Reconnect to the mesh if disconnected
      Serial.println(F("Re-initializing the Network Address..."));
      mesh.renewAddress();
      Serial.print(F("New Network Addr: ")); Serial.println(mesh.getAddress(nodeID));
    }
  }


  /**** No Message Response ****/

  // Reset the mesh connection
  if (message_Flag && Timer(1000, messageTimer)) {
    message_Flag = 0;
    // Reconnect to the network
    if (!mesh.checkConnection()) {
      Serial.println(F("Re-initializing the network ID..."));
      mesh.renewAddress();
      Serial.print(F("New Network Address: ")); Serial.println(mesh.getAddress(nodeID));
    }
    network.sleepNode(8, 255);
    sleepFlag = 1;
  }

  /**** 'C' Type Data Evaluation ****/

  // Based on the 'C' type data, re-configure the thresholds


  /**** 'D' Type Data Evaluation ****/

  // Responding to the S or C type message from the master
  if (M_Dat && message_Flag) {
    // Turn off the message response flag
    message_Flag = 0;
    // If M_Dat == 2, reconfig the thresholds
    // Reset the data variables
    M_Dat = 0;
    // Go to sleep
    Serial.println(F("Received Sleep Instructions From Master"));
    network.sleepNode(8, 255);
    sleepFlag = 1;
  }

  /**** Config Options ****/

} // Loop


/**** Helper Functions ****/

void C_Struct_Serial_print(C_Struct sct) {
  Serial.print(F("Soil Moisture Threshold: ")); Serial.println(sct.sM_thresh);
  Serial.print(F("Soil Moisture Danger Threshold: ")); Serial.println(sct.sM_thresh_00);
  //Serial.print(F("Barometric Pressure Threshold: "); Serial.println(F(sct.bP_thresh);
  Serial.print(F("Ambient Light Level Threshold: ")); Serial.println(sct.lL_thresh);
  Serial.print(F("Ambient Temperature Threshold: ")); Serial.println(sct.tC_thresh);
  Serial.print(F("Maximum TimeStamp Threshold: ")); Serial.println(sct.time_thresh);
  return;
}

void D_Struct_Serial_print(D_Struct sct) {
  Serial.print(F("Soil Moisture Cont. (g%): ")); Serial.println(sct.soilMoisture);
  //Serial.print(F("Barometric Pressure (Pa): ")); Serial.println(sct.baroPressure);
  Serial.print(F("Ambient Lux Level   (lx): ")); Serial.println(sct.lightLevel);
  Serial.print(F("Ambient Temperature (C ): ")); Serial.println(sct.temp_C);
  Serial.print(F("Calucated Digital Output: ")); Serial.println(sct.digitalOut);
  //Serial.print(F("Previous Time Stamp (ms): ")); Serial.println(sct.timeStamp);
  Serial.print(F("Node ID: ")); Serial.println(sct.node_ID);
  return;
}

void initC_Struct(C_Struct* sct) {
  sct->sM_thresh = 3.8;
  sct->sM_thresh_00 = 1;
  //sct->bP_thresh = 20000;
  sct->lL_thresh = 12;
  sct->tC_thresh = 5;
  sct->time_thresh = 30000;
  return;
}

/* @name: pullMoistureSensor
   @param: none
   @return: value of the mapped sensor value
*/
float pullMoistureSensor(void) {
  // First map the voltage reading into a resistance
  float soilV = map(analogRead(MOISTURE_PIN), 0, 1023, 0, 500);
  // convert to soil resistance in kohms
  float R_probes = (500 / soilV);
  Serial.println(R_probes);
  R_probes -= 1;
  Serial.println(R_probes);
  R_probes *= 10;
  Serial.println(R_probes);
  // convert to percentage of gravimetric water content (gwc)
  R_probes = pow((R_probes / 2.81), -1 / 2.774) * 100;
  // Returns the mapped analog value
  // A voltage of 2.5V should return a gwc of 60-70%
  return R_probes;
}

/* @name: pullLightSensor
   @param: none
   @return: value of the mapped sensor value
*/
float pullLightSensor(void) {
  float b = -0.94;
  float c = 38.9;
  float a = 0.014;
  // First map the voltage reading
  float lightV = map(analogRead(LIGHT_PIN), 0, 1023, 0, 500);
  float mr_Lumen = lightV - b * c * 100;
  mr_Lumen /= c * 100;
  mr_Lumen = pow(mr_Lumen, 1 / a);
  // Returns the mapped analog value
  return (mr_Lumen);
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


/* @name: run_DeepOcean
   @param: D_Struct - struct that holds sensor data
   @param: C_Struct - struct that holds thresholds
   @return: digital high/low telling the system to
                      turn on or off the water
*/
int run_DeepOcean(D_Struct D_Struct, C_Struct C_Thresh) {
  int HydroHomie = 0;
  // Check for the time threshold
  if (time_Thresh) {

    // Chcek the soil moisture against the first threshold
    // If its light, then don't water unless it has been a long time
    if ((D_Struct.soilMoisture < C_Thresh.sM_thresh) && (D_Struct.lightLevel <= C_Thresh.lL_thresh)) { //
      HydroHomie = 1;
    }

    // Check temperature to prevent freezing
    // Also make sure you only water once in a while so water is not
    // always on when its cold
    else if (D_Struct.temp_C <= C_Thresh.tC_thresh) {
      HydroHomie = 1;
    }
  }

  // Water immediately if soilMoisture goes below a certain level
  if (D_Struct.soilMoisture < C_Thresh.sM_thresh_00) {
    return 2;
  }

  // In main, make sure you update the timestamp if the output is >0
  return HydroHomie;
}
