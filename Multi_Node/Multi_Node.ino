// ********** INCLUDES **********
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include "Adafruit_BMP085.h"
#include <printf.h>

/**** Configure the Radio ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**** #Defines ****/
#define time_Thresh ((millis() - D_Struct.timeStamp) >= C_Thresh.time_thresh)

/**** GLOBALS ****/
#define LED 2
#define pushButton A0
#define MOISTURE_PIN A1
#define LIGHT_PIN A2

// Timers
uint32_t runningTimer = 0;

// Sensor Vars
uint16_t miso_soup = 0;
uint16_t bread;
Adafruit_BMP085 bmp;

// RF24 Vars
uint8_t nodeID = 2;    // Set this to a different number for each node in the mesh network
uint16_t meshAddr = 0;

// Use these vars to store the header data
uint32_t P_Dat = 0;
uint32_t D_Dat = 0;
uint32_t C_Dat = 0;

// C_Struct stores relevant thresholds
typedef struct {
  uint16_t sM_thresh;
  uint16_t sM_thresh_00;
  uint16_t bP_thresh;
  uint16_t lL_thresh;
  uint16_t tC_thresh;
  uint16_t time_thresh;
} C_Struct;

// D_Struct stores the relevant sensor data
typedef struct {
  uint16_t soilMoisture;
  uint16_t baroPressure;
  uint16_t lightLevel;
  uint16_t temp_C;
  uint16_t temp_F;
  uint16_t timeStamp;
} D_Struct;

// C and D type structs
C_Struct Thresholds;
D_Struct Data_Struct;

/**** Function Prototypes ****/
void D_Struct_Serial_print(D_Struct);
void C_Struct_Serial_print(C_Struct);
uint16_t pullSensor(int);
void activateSensor(int);
void de_activateSensor(int);
int Timer(uint32_t, uint32_t);
int run_DeepOcean(D_Struct, C_Struct);


void setup() {
  Serial.begin(115200);
  //  printf_begin();

  // Set the LED as an output
  pinMode(LED, OUTPUT);
  pinMode(pushButton, INPUT);
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(LIGHT_PIN, INPUT);

  // Begin the Barometric Pressure Sensor
  // Pin out: Vin->5V, SCL->A5, SDA->A4
  bmp.begin();

  // Set this node as the master node
  mesh.setNodeID(nodeID);
  Serial.print("Mesh Network ID: ");
  Serial.println(mesh.getNodeID());

  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
  //  network.multicastRelay = 1;
  meshAddr = mesh.getAddress(nodeID);
  radio.setPALevel(RF24_PA_MAX);
  Serial.println(F("**********************************\r\n"));
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
      // For some reason, the mesh addr was not updated properly
      mesh.renewAddress();
      meshAddr = mesh.getAddress(nodeID);
      Serial.println("Re-initializing the network ID...");
      Serial.print("New network ID: "); Serial.println(mesh.getNodeID());
      Serial.println(F("**********************************\r\n"));
    }
  }


  /**** Read Sensors ****/

  if (D_Dat | P_Dat) {

    // Read all sensors
    Data_Struct.soilMoisture = pullSensor(MOISTURE_PIN, 33);
    Data_Struct.baroPressure = bmp.readPressure();
    Data_Struct.lightLevel = pullSensor(LIGHT_PIN, 33);
    Data_Struct.temp_C = bmp.readTemperature();
    Data_Struct.temp_F = ((Data_Struct.temp_C * 9) / 5) + 32;
    Data_Struct.timeStamp = millis();

    // Sets a variable to the voltage of the pushbutton and maps it into mV
    miso_soup = pullSensor(pushButton, 33);
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

    // Send the D_Struct to M
    // Sends the data up through the mesh to the master node to be evaluated
    if (!mesh.write(&Data_Struct, 'D', sizeof(Data_Struct), 0)) {
      Serial.println("Send failed; checking network connection.");
      if (!mesh.checkConnection()) {
        mesh.renewAddress();
        meshAddr = mesh.getAddress(nodeID);
        Serial.println("Re-initializing the network ID...");
        Serial.print("New network ID: "); Serial.println(mesh.getNodeID());
      } else {
        Serial.println("Network connection good.");
        Serial.println(F("**********************************\r\n"));
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

    // Resets P_Dat to 0 to indicate the data has been consumed
    P_Dat = 0;

    // Sends the data up through the mesh to the master node to be evaluated
    if (!mesh.write(&bread, 'P', sizeof(bread), 0)) {
      Serial.println("Send failed; checking network connection.");
      if (!mesh.checkConnection()) {
        mesh.renewAddress();
        meshAddr = mesh.getAddress(nodeID);
        Serial.println("Re-initializing the network ID...");
        Serial.print("New network ID: "); Serial.println(mesh.getNodeID());
      } else {
        Serial.println("Network connection good.");
        Serial.println(F("**********************************\r\n"));
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
  Serial.print("Soil Moisture Danger Threshold: "); Serial.println(sct.sM_thresh_00);
  Serial.print("Barometric Pressure Threshold: "); Serial.println(sct.bP_thresh);
  Serial.print("Ambient Light Level Threshold: "); Serial.println(sct.lL_thresh);
  Serial.print("Ambient Temperature Threshold: "); Serial.println(sct.tC_thresh);
  Serial.print("Maximum TimeStamp Threshold: "); Serial.println(sct.time_thresh);
}

void D_Struct_Serial_print(D_Struct sct) {
  Serial.print("Soil Moisture Level (V ): "); Serial.println(sct.soilMoisture);
  Serial.print("Barometric Pressure (Pa): "); Serial.println(sct.baroPressure);
  Serial.print("Ambient Light Level (V ): "); Serial.println(sct.lightLevel);
  Serial.print("Ambient Temperature (C ): "); Serial.println(sct.temp_C);
  Serial.print("Ambient Temperature (F ): "); Serial.println(sct.temp_F);
  Serial.print("Time Stamp (ms): "); Serial.println(sct.timeStamp);
}

/* @name: pullSensor
 * @param: sensor - pin you want to read from
 * @param: toMax - maximum value you want the result mapped to
 * @return: value of the mapped sensor value
 */
uint16_t pullSensor(int sensor, int toMax) {
  // Returns the mapped analog value
  return (map(analogRead(sensor), 0, 900, 0, toMax));
}

/* @name: activateSensor
 * @param: activePin - pin you want to turn on
 * @return: if the pin was turned on successfully
 */
void activateSensor(int activePin) {
  // Turns on the argument pin
  return (digitalWrite(activePin, 1));
}

/* @name: de_activateSensor
 * @param: inactivePin - pin you want to turn off
 * @return: if the pin was turned off successfully
 */
void de_activateSensor(int inactivePin) {
  // Turns off the arugment pin
  return (digitalWrite(inactivePin, 0));
}

/* @name: Timer
 * @param: delayThresh - timer duration
 * @param: prevDelay - time in millis() when the timer started
 * @return: digital high/low depending if timer elapsed or not
 * This is a non-blocking timer that handles uint32_t overflow,
 * it works off the internal function millis() as reference
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
 * @param: D_Struct - struct that holds sensor data
 * @param: C_Struct - struct that holds thresholds
 * @return: HydroHomie - digital high/low telling the system to
 *                        turn on or off the water
 */
int run_DeepOcean(D_Struct D_Struct, C_Struct C_Thresh) {
  int HydroHomie = 0;
  
  // Chcek the soil moisture agains the first threshold
  if ((D_Struct.soilMoisture < C_Thresh.sM_thresh) && time_Thresh) {
    // If its light, then don't water unless it has been a long time
    if (D_Struct.lightLevel <= C_Thresh.lL_thresh) {
      HydroHomie = 1;
    }
  }
  
  // Water immediately if soilMoisture goes below a certain level
  if (D_Struct.soilMoisture < C_Thresh.sM_thresh_00) {
    HydroHomie = 1;
  }

  // Check temperature to prevent freezing
  // Also make sure you only water once in a while so water is not 
  // always on when its cold
  if ((D_Struct.temp_C <= C_Thresh.tC_thresh) && time_Thresh) {
    HydroHomie = 1;
  }

  // In main, make sure you check the result of HydroHomie to reset the timestamp
  return HydroHomie;
}
