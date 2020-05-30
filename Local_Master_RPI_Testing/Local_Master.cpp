#ifndef __cplusplus
#define __cplusplus
#endif

// ********** INCLUDES ***********
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
#include <math.h>
#include <time.h>

/**** GLOBALS ****/
#define LED RPI_BPLUS_GPIO_J8_07
#define pushButton RPI_BPLUS_GPIO_J8_29
#define SPI_SPEED_2MHZ 2000000

#define TRUE  1
#define FALSE 0
#define MAX_ELEMENTS 100
#define NUM_HOSES 3
#define MOISTURE 0
#define SUNLIGHT 1
#define TEMP  2

// Water Delivery
#define WATER_OFF 0
#define WATER_ON  1

#define PMOS_ON 0
#define PMOS_OFF 1
#define NMOS_ON 1
#define NMOS_OFF 0

#define DEMUX_OFF 1 //Set to Low Enable
#define DEMUX_ON  0

#define LPMOS_Pin 6
#define LNMOS_Pin 13
#define RPMOS_Pin 19
#define RNMOS_Pin 26

// Buttons
#define ENTER_Pin  12 
#define BACK_Pin  5
#define DOWN_Pin 20
#define UP_Pin 21

//Flow Sensor Pins 
#define FLOW_SENSOR_1_Pin  4                                                                   
#define FLOW_SENSOR_2_Pin  3
#define FLOW_SENSOR_3_Pin 2

//Select Pins
#define SEL_1_Pin 17
#define SEL_0_Pin 27

//Flow Sensor Conversions
#define FS_CAL_A 46.2 //Variables used for Characterized FS Regression
#define FS_CAL_B 40.8 
#define FS_CAL_STEADY 404  //Calibration factor used when FS signal is steady
#define LITERS_TO_GAL 0.264172

// Time
#define FORECAST_CALL 1800000
#define FIVE_SECONDS 5000
#define EIGHT_SECONDS 8000
#define MIN_5 300000
#define MIN_3 180000
#define MIN_2 120000
#define MIN_1 60000
#define ONE_SECOND 1000
#define PULSE_DURATION 3000
#define FET_DELAY 10
#define HUNDRED_MILLI 100
#define MAX_SENSORS 20
#define HOURS_36 129600000
#define MIN_10 600000
#define MUX_DELAY 50

/* Avialable Colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
*/


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
  float lL_thresh;
  uint16_t tC_thresh;
  uint16_t time_thresh;
} C_Struct;

// D_Struct stores the relevant sensor data
typedef struct {
  float soilMoisture;
  float lightLevel;
  uint16_t temp_C;
  uint8_t digitalOut;
  uint8_t nodeID;
  uint8_t battLevel;
} D_Struct;

typedef struct{
    float precipProb;
    int temperature;
    int humidity;
    int pressure;
    int windSpeed;
    int windBearing;
} Forecast;

typedef struct {
    uint8_t status;
    uint8_t sensors[MAX_SENSORS];
    uint8_t waterLevel;
    uint8_t tally;
    uint8_t flowRate;
    uint8_t rainFlag;
    uint32_t rainTimer;
    uint8_t control;
}Hoses;

//States for Water Delivery SM
typedef enum{
  HOSE_IDLE,
  HOSE_ON_S1,
  HOSE_ON_S2,
  HOSE_ON_S3,
  HOSE_OFF_S1,
  HOSE_OFF_S2, 
  HOSE_OFF_S3, 
} w_State;

//States for OLED SM
typedef enum{
  WELCOME_PAGE,
  SLEEP,
  HOME_PAGE,
  SENSORS_HOME,
  SENSORS_LIST,
  SENSORS_CURRENT,
  SENSORS_PLOT_START,
  SENSORS_PLOT,
  HOSES_HOME, 
  HOSES_STATUS,
  HOSES_CONTROL,
  HOSES_WATER,
  HOSES_MAP,
  HOSES_MAP_SELECT,
  SETTINGS_HOME,
  SETTINGS_SLEEP,
  SETTINGS_CAL,
  SETTINGS_COLOR,
  SETTINGS_RESET,
} OLED_State;

// Enum for hose specification
typedef enum {
    HOSE0,
    HOSE1,
    HOSE2,
}HOSE_NUM;

// Enum for control types
enum {
    OFF,
    ON,
    AUTOMATIC,
};


// Data Vars
D_Struct D_Dat;
D_Struct current_Dat_1;   //Most recent Sensor Data
D_Struct current_Dat_2;
D_Struct current_Dat_3;
//D_Struct current_Dat_4;
//D_Struct current_Dat_5;
D_Struct sensor_data[MAX_ELEMENTS]; 
D_Struct sensor1_data[MAX_ELEMENTS];  //Recent Sensor Data for Plotting
D_Struct sensor2_data[MAX_ELEMENTS];
D_Struct sensor3_data[MAX_ELEMENTS];
//D_Struct sensor4_data[MAX_ELEMENTS];
//D_Struct sensor5_data[MAX_ELEMENTS]

uint8_t dFlag = 0;
uint8_t dataDat = 1;
uint8_t column_flag = 0;
int sd_index = -1;
int sd_index_1 = -1;
int sd_index_2 = -1;
int sd_index_3 = -1;

//Struct Declarations 
OLED_State oledState = WELCOME_PAGE;
OLED_State nextPage = WELCOME_PAGE;
//OLED_State prevPage = SLEEP;
static w_State waterState = HOSE_IDLE; //Water Deliver SM state var

//Array Declarations
static int mappedSensors[MAX_SENSORS]; //Intialize Mapping Variables 
static int unmappedSensors[MAX_SENSORS]; 
//static uint8_t prev_waterLevel[3];
static char timeBuffer[20];   //Buffers used for format Variables into strings
static char intBuffer[20];
static char sensorIDBuffer[100];
static char hoseBuffer[100];
//static char testBuffer2[100];
//static char testBuffer3[100];
static char currentBuffer1[100];  //Used for printing current sensor readings 
static char currentBuffer2[100];
static char currentBuffer3[100];
static char currentBuffer4[100];
static char currentBuffer5[100];


//Variable Declarations
static uint8_t dataType = 0;    //Determines type of data to be  printed
static uint16_t oledColor;

static int oledHour;    //Store System Time
static int oledMinute;
//static int tempVal = 0;
static int wholeVal = 0;    //Converting 
static int decimalVal = 0;
static int hose0_elements = 0;  //keeps track of number of sensors mapped to hose
static int hose1_elements = 0;
static int hose2_elements = 0;
static int new_Data = FALSE; //Flag set when new data received to update OLED
static int selected_Node = 0; //Determine which set of Sensor node Data to plot
static int nodeUnmapped = TRUE; // Set True if at least one unmapped sensor
static int sensorToMap = 0; // Var keeping track of which sensor to map

//Flow Sensor Support 
static int tach_fs[4];  //Used for individually tracking flow rates 
static int prevTach_fs[4];  //and Water Output using YF-S201 Flow Sensors

static int pulseCount_fs[4];
static float water_liters_fs[4];
static float prev_Liters_fs[4];
static float water_gal_fs[4];

static float moisture_s_thresh[MAX_SENSORS]; //Moisture Thresholds


//static float testFloat = 64.757065;
static float temp_wholeVal = 0;
static float temp_decimalVal = 0;
static float test_Moisture = 0;



//Button Variables
static int prevArrowState, arrowState = 0;
static int lastUpButtonState, lastDownButtonState, lastBackButtonState, lastEnterButtonState,
       upButtonValue, downButtonValue, backButtonValue, enterButtonValue,
       upButtonValue2, downButtonValue2, backButtonValue2, enterButtonValue2,
       ENTER_PRESSED, UP_PRESSED, DOWN_PRESSED, BACK_PRESSED = 0;

// Timers
uint32_t forecastTimer = 0;
uint32_t waterDeliveryTimer = 0;
uint32_t wTimer = 0; //Timer used for driving Water Delivery testing
uint32_t oledSleepTimer = 0; //Timer used for updating OLED testing
uint32_t sleepTime = 0; //Variable to set time to wait for button press until SLEEP
uint32_t connectionTimer = 0;


// RF24 Vars
const static uint8_t nodeID = 0;    // 0 = master
uint8_t num_nodes = 0;

// Forecast Support
Forecast Forecast1;
char buffer[10];
double data[6];
FILE* fp;

// Water Delivery Support
Hoses Hose0, Hose1, Hose2;
Hoses Hose[3];
uint8_t hose_statuses = 0;
uint8_t prev_hose_statuses = 0;


/**** Helper Fxn Prototypes ****/
int Timer(uint32_t, uint32_t);
void setup(void);
void checkButtons(void);
void printHoseStatus(int16_t x, int16_t y, uint8_t status);
int printGrid(int16_t x0, int16_t x1, int16_t y0, int16_t y1, int16_t xtics, int16_t ytics);
int printAxesLabels(int16_t x0, int16_t y0);
int plotSampleData(D_Struct data[], uint8_t dataType, int16_t size);
int WaterDeliverySM(uint8_t status, uint32_t delayP_N, uint32_t pulseTime);
void OLED_PrintArrow(int x, int y);
void OLED_SM(uint16_t color);
void LPMOS_Set(uint8_t status);
void RPMOS_Set(uint8_t status);
void LNMOS_Set(uint8_t status);
void RNMOS_Set(uint8_t status);
void recordPulses_FS(int i);
float convertPulse_Liters(int pulseCount);
float convertLiters_Gals(float liters);
int convertFloat_String(float in, char buffer[100]); 
void Reset_System(void);
void Set_Select(uint8_t hose_selected);
uint8_t WaterDelivery(HOSE_NUM);




/*********************************************************************************************/
/**** Void Setup ****/
void setup(void) {
  // Initialize the Hose array
  Hose[0] = Hose0; Hose[1] = Hose1; Hose[2] = Hose2;
  Hose[0].waterLevel = 1; Hose[1].waterLevel = 1; Hose[2].waterLevel = 1; 
  Hose[0].control = AUTOMATIC; Hose[1].control = OFF; Hose[2].control = OFF;
  Hose[0].status = WATER_ON; Hose[1].status = WATER_OFF; Hose[2].status = WATER_OFF;
  
  // Init the GPIO Library
  DEV_ModuleInit();
  Device_Init();
  bcm2835_init();
  bcm2835_spi_begin();  
  
  
  // Set Pins to Output
  DEV_GPIO_Mode(LPMOS_Pin, 1);
  DEV_GPIO_Mode(RPMOS_Pin, 1);
  DEV_GPIO_Mode(LNMOS_Pin, 1);
  DEV_GPIO_Mode(RNMOS_Pin, 1);
  
  // Set SELECT Pins to Output
  DEV_GPIO_Mode(SEL_1_Pin, 1);
  DEV_GPIO_Mode(SEL_0_Pin, 1);
  
  
  
  // Set Pins to Input
  DEV_GPIO_Mode(ENTER_Pin, 0);
  DEV_GPIO_Mode(BACK_Pin, 0);
  DEV_GPIO_Mode(DOWN_Pin, 0);
  DEV_GPIO_Mode(UP_Pin, 0);
  
  // Set Flow Sensor Pins to Input
  DEV_GPIO_Mode(FLOW_SENSOR_1_Pin, 0);
  DEV_GPIO_Mode(FLOW_SENSOR_2_Pin, 0);
  DEV_GPIO_Mode(FLOW_SENSOR_3_Pin, 0);
  
  
  
  // Turn off the H-Bridge
  LPMOS_Set(PMOS_OFF); //Initial States for MOS devices 
  RPMOS_Set(PMOS_OFF);
  LNMOS_Set(NMOS_OFF); // Notice the diff b/t PMOS and NMOS states
  RNMOS_Set(NMOS_OFF);  //PMOS_OFF = 1, NMOS_OFF = 0
  
  
  //LPMOS_Set(DEMUX_OFF);   //Driving with De-Mux, Active Low
  //RPMOS_Set(DEMUX_OFF);     //DeMux_OFF = 1
  //LNMOS_Set(DEMUX_OFF);
  //RNMOS_Set(DEMUX_OFF);
  
  //Initialize OLED variables 
  oledColor = WHITE; //
  //oledTimer = bcm2835_millis(); //Reset Oled Timer
  sleepTime = MIN_3; //Set default sleep time to 3 minutes
  oledSleepTimer = bcm2835_millis(); //Start Oled Sleep Timer

  // Set this node as the master node
  mesh.setNodeID(nodeID);
  printf("Node ID: %d\n", nodeID);
  radio.setPALevel(RF24_PA_MAX);

  // Initialize the mesh and check for proper chip connection
 if (mesh.begin()) {
    printf("\nInitialized: %d\n", radio.isChipConnected());
  }
  // Print out debugging information
  radio.printDetails();


  //Testing Current Struct Plotting
  current_Dat_1.nodeID = 2;
  current_Dat_1.soilMoisture = 36.8;
  current_Dat_1.lightLevel = 90.2;
  current_Dat_1.temp_C = 85;
  
  current_Dat_2.nodeID = 5;
  current_Dat_2.soilMoisture = 67.4;
  current_Dat_2.lightLevel = 72.6;
  current_Dat_2.temp_C = 85;
  
  current_Dat_3.nodeID = 4;
  current_Dat_3.soilMoisture = 56.5;
  current_Dat_3.lightLevel = 76.3;
  current_Dat_3.temp_C = 85;
  selected_Node = 2;
  
  
  moisture_s_thresh[0] = 42;
  moisture_s_thresh[1] = 32;
  moisture_s_thresh[2] = 55;

  
  int i = 0;
  test_Moisture = 60; 
  
  //Plotting Moisture Testing 
  for(i=0;i<MAX_ELEMENTS; i++){
    sensor2_data[i].soilMoisture = test_Moisture;
    if( ((i > 30) && ( i < 35)) || ((i > 60) && ( i < 65)) ){
      test_Moisture += 3;
    } else if (((i > 36) && ( i < 40)) || ((i > 66) && ( i < 70))){
      test_Moisture -= 0.8;
    } else {
      test_Moisture -= 0.3;
    }
  }
  
  
  return;
}


/********************************************************************************************/
int main(void) {
  setup();
  
  //Access Local Time from RPi
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  
  //Store as variables for comparison
  oledHour = tm.tm_hour;
  oledMinute = tm.tm_min;
  
  //Store Time variables into strings for printing
  if(oledHour  == 0){ //Takes Care of 12:00 AM Error
    sprintf(timeBuffer,"%02d:%02d PM",(oledHour+12),oledMinute );
  } else if(oledHour < 12){
      sprintf(timeBuffer,"%02d:%02d PM",oledHour,oledMinute );
  } else if(oledHour == 12){         //Takes care of prev error w/ 12:00 PM
      sprintf(timeBuffer,"%02d:%02d PM",oledHour,oledMinute );
  } else {
      sprintf(timeBuffer,"%02d:%02d PM",(oledHour-12), oledMinute );
  }
  
  printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year +1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );
  
  
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
            network.read(header, &D_Dat, sizeof(D_Dat));
            // Set the flag that indicates we need to respond to a new message
            dFlag = 1;

            // Here is where we add the sensor data to the sensor data array
            // But first we want to see if the sensor data array is full
            if (sd_index >= MAX_ELEMENTS) { // checks if the index is at the max # of elements
                int i, j = 0;
                // Now we transfer the 10 most recent data values to the bottom of the list
                for ((i = MAX_ELEMENTS - 10); i < MAX_ELEMENTS; i++) {
                    sensor_data[j] = sensor_data[i]; // j is the bottom, i is the top
                    j++;
                }
                // Reset the sensor data index
                sd_index = 10;
            }
            // Increment the sensor data index for the new value
            sd_index++;
            // Then place the new data into the array
            sensor_data[sd_index] = D_Dat;
            break;

          // Do not read the header data, instead print the address inidicated by the header type
          default:
            break;
        }
      } else {
        // Generally will never get here
        // This basically just removes the message from the input buffer
        network.read(header, 0, 0);
      }
    }




    /**** Update List of Nodes ****/
    if (Timer(MIN_2, connectionTimer)) {
      connectionTimer = millis();
      // Other option is to create a dict after receiving a message
      if (num_nodes != mesh.addrListTop) {
        num_nodes = mesh.addrListTop;
        printf("\nConnected nodes: ");
        int i = 0;
        for (i = 0; i < mesh.addrListTop; i++) {
            // Add sensor nodes to the list of sensors mapped to the hose
            Hose[HOSE0].sensors[i] = mesh.addrList[i].nodeID; //Hard-coded for Testing
            if (i == (mesh.addrListTop - 1)) {
              printf("%d\n\n", mesh.addrList[i].nodeID);
            } else {
              printf("%d, ", mesh.addrList[i].nodeID);
            }
        }
        // Reset the water level threshold according to the # of sensors
        Hose[HOSE0].waterLevel = i/2;
        printf("Water Level Threshold: %d\n\n", Hose[HOSE0].waterLevel);
      }
    }




    /**** Data Logging ****/
  
    if (dFlag) {
        // This should be the last thing that gets done when data is received
      dFlag = 0;

      /**** Write Data Values to SD Card ****/
      {
          FILE* out = fopen("Data_Log.csv", "a");
 
          // prints out main column headers for the data file.
          // conditional here: output if first loop, dont afterward, controlled by column_flag
          if (column_flag == 0)
          {
              fprintf(out, "Soil_Moisture, Ambient_Light, Ambient_Temp, Barometric_Pressure, Precip_Prob, Digital_Output, Node_ID, Battery_Level, Hose_1, Hose_2, Hose_3\n");
              column_flag = 1;
          }

          printf("%f, %f, %d, %d, %f, %d, %d, %d, %d, %d, %d\n", D_Dat.soilMoisture, D_Dat.lightLevel, D_Dat.temp_C, Forecast1.pressure, Forecast1.precipProb, D_Dat.digitalOut, D_Dat.nodeID, D_Dat.battLevel, Hose[0].status, Hose[1].status, Hose[2].status);
        
          fprintf(out, "%13f,   ", D_Dat.soilMoisture); // prints out 0th member of the data vector to the file.
          fprintf(out, "%13f,   ", D_Dat.lightLevel); // prints out 2nd member of the data vector to the file.
          fprintf(out, "%19d,   ", D_Dat.temp_C); // prints out 3rd member of the data vector to the file.
          fprintf(out, "%19d,   ", Forecast1.pressure);
          fprintf(out, "%11f,   ", Forecast1.precipProb);
          fprintf(out, "%14d,   ", D_Dat.digitalOut); // prints out 4th member of the data vector to the file.
          fprintf(out, "%7d,   ", D_Dat.nodeID); // prints out 6th member of the data vector to the file.
          fprintf(out, "%14d,   ", D_Dat.battLevel);
          fprintf(out, "%5d,   ", Hose[0].status);
          fprintf(out, "%5d,   ", Hose[1].status);
          fprintf(out, "%5d,\n", Hose[2].status);
          fclose(out);
      }
      
      //Update Struct Variables for Data
      if (D_Dat.nodeID == 2){  // Update Recent Sensor Node Value
        if (sd_index_1 >= MAX_ELEMENTS) { // checks if the index is at the max # of elements
                int i, j = 0;
                // Now we transfer the 10 most recent data values to the bottom of the list
                for ((i = MAX_ELEMENTS - 10); i < MAX_ELEMENTS; i++) {
                    sensor1_data[j] = sensor1_data[i]; // j is the bottom, i is the top
                    j++;
                }
                // Reset the sensor data index
                sd_index_1 = 10;
            }
            // Increment the sensor data index for the new value
            sd_index_1++;
            // Then place the new data into the array
            sensor1_data[sd_index_1] = D_Dat;
            current_Dat_1 = D_Dat;
            new_Data = TRUE;
      } else if(D_Dat.nodeID == 5){
        if (sd_index_2 >= MAX_ELEMENTS) { // checks if the index is at the max # of elements
                int i, j = 0;
                // Now we transfer the 10 most recent data values to the bottom of the list
                for ((i = MAX_ELEMENTS - 10); i < MAX_ELEMENTS; i++) {
                    sensor2_data[j] = sensor2_data[i]; // j is the bottom, i is the top
                    j++;
                }
                // Reset the sensor data index
                sd_index_2 = 10;
            }
            // Increment the sensor data index for the new value
            sd_index_2++;
            // Then place the new data into the array
            sensor2_data[sd_index_2] = D_Dat;
            current_Dat_2 = D_Dat;
            new_Data = TRUE;
      } else if(D_Dat.nodeID == 4){
        if (sd_index_3 >= MAX_ELEMENTS) { // checks if the index is at the max # of elements
                int i, j = 0;
                // Now we transfer the 10 most recent data values to the bottom of the list
                for ((i = MAX_ELEMENTS - 10); i < MAX_ELEMENTS; i++) {
                    sensor3_data[j] = sensor3_data[i]; // j is the bottom, i is the top
                    j++;
                }
                // Reset the sensor data index
                sd_index_3 = 10;
            }
            // Increment the sensor data index for the new value
            sd_index_3++;
            // Then place the new data into the array
            sensor3_data[sd_index_3] = D_Dat;
            current_Dat_3 = D_Dat;
            new_Data = TRUE;
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




    /**** Water Delivery ****/

    if (Timer(MIN_1, waterDeliveryTimer)) {
        // reset the timer
        waterDeliveryTimer = millis();
        printf("Checking Water Delivery\n");
        // Then call WaterDelivery to see if we need to turn on each hose
        if (Hose[0].control == AUTOMATIC) {
            hose_statuses = WaterDelivery(HOSE0);
        } else if (Hose[0].control == ON){
         
          if (Hose[0].status == WATER_OFF){
            // Call the state machine to open the solenoid valve
            Set_Select(HOSE0);
            DEV_Delay_ms(MUX_DELAY);
            printf("Select to Hose 1\n");
            printf("Hose 1 Turned On\n");
            while (!WaterDeliverySM(WATER_ON, FET_DELAY, PULSE_DURATION));
          }
          Hose[0].status = WATER_ON;
          hose_statuses |= 0x01;
        } else {
          if (Hose[0].status == WATER_ON){
            Set_Select(HOSE0);
            DEV_Delay_ms(MUX_DELAY);
            printf("Select to Hose 1\n");
            printf("Hose 1 Turned Off\n");
            // Call the state machine to open the solenoid valve
            while (!WaterDeliverySM(WATER_OFF, FET_DELAY, PULSE_DURATION));
          }
          Hose[0].status = WATER_OFF;
          hose_statuses &= 0xFE; //Clear Hose Status
        }
        if (Hose[1].control == AUTOMATIC) {
            hose_statuses = WaterDelivery(HOSE1);
        }  else if (Hose[1].control == ON){
          
          // Call the state machine to open the solenoid valve
          if (Hose[1].status == WATER_OFF){
            Set_Select(HOSE1);
            DEV_Delay_ms(MUX_DELAY);
            printf("Select to Hose 2\n");
            printf("Hose 2 Turned On\n");
            while (!WaterDeliverySM(WATER_ON, FET_DELAY, PULSE_DURATION));
          }
          Hose[1].status = WATER_ON;
          hose_statuses |= 0x02;
        } else {
          if (Hose[1].status == WATER_ON){
            Set_Select(HOSE1);
            DEV_Delay_ms(MUX_DELAY);
            printf("Select to Hose 2\n");
            printf("Hose 2 Turned Off\n");
             // Call the state machine to open the solenoid valve
            while (!WaterDeliverySM(WATER_OFF, FET_DELAY, PULSE_DURATION));
          }
          Hose[1].status = WATER_OFF;
          hose_statuses &= 0xFC; //Clear Hose Status
                    
        }
        if (Hose[2].control == AUTOMATIC) {
            hose_statuses = WaterDelivery(HOSE2);
        } else if (Hose[2].control == ON){
         
          if (Hose[2].status == WATER_OFF){
            Set_Select(HOSE2);
            DEV_Delay_ms(MUX_DELAY);
            printf("Select to Hose 3\n");
            printf("Hose 3 Turned On\n");
            // Call the state machine to open the solenoid valve
            while (!WaterDeliverySM(WATER_ON, FET_DELAY, PULSE_DURATION));
             
          }
          Hose[2].status = WATER_ON;
          hose_statuses |= 0x04;
        } else {
          if (Hose[2].status == WATER_ON){
            Set_Select(HOSE2);
            DEV_Delay_ms(MUX_DELAY);
            printf("Select to Hose 3\n");
            printf("Hose 3 Turned Off\n");
            // Call the state machine to open the solenoid valve
            while (!WaterDeliverySM(WATER_OFF, FET_DELAY, PULSE_DURATION));
          }
          Hose[2].status = WATER_OFF;
          hose_statuses &= 0xFB; //Clear Hose Status
        }
    }
    



    /**** Forecast Data API Call ****/

    if (Timer(FORECAST_CALL, forecastTimer)) {
        printf("Opening call to forecast API...\n");
        forecastTimer = millis();
        // Opens and runs the python script in the terminal
        fp = popen("python RFpython_test.py", "r");

        // error checking
        if (fp == NULL)
        {
            printf("Failed to run command.\n");
            break;
        }

        printf("Call to forecast API success.\n");
        int tmp = 0;

        // loop that extracts the outputted data from the shell and places it in an array
        while (fgets(buffer, sizeof(buffer), fp) != NULL)
        {
            sscanf(buffer, "%lf", &data[tmp]);
            ++tmp;
        }

        // moves the extracted data from the array to the struct
        Forecast1.precipProb = data[0];
        printf("Forecast1.precipProb = %f.\n", Forecast1.precipProb);
        Forecast1.temperature = round(data[1]);
        printf("Forecast1.temperature = %d.\n", Forecast1.temperature);
        Forecast1.humidity = round(data[2]);
        printf("Forecast1.humidity = %d.\n", Forecast1.humidity);
        Forecast1.pressure = round(data[3]);
        printf("Forecast1.pressure = %d.\n", Forecast1.pressure);
        Forecast1.windSpeed = round(data[4]);
        printf("Forecast1.windSpeed = %d.\n", Forecast1.windSpeed);
        Forecast1.windBearing = round(data[5]);
        printf("Forecast1.windBearing = %d.\n\n", Forecast1.windBearing);

        pclose(fp);
    }
    
    
    
    /** Flow Sensor Management ****/
    
    
    int i;
    // pulseCount_fs2 = 100; // for testing
    for(i = 0; i > 3 ;i++){
      recordPulses_FS(i); //Record Flow Sensor Tach Signals 
      water_liters_fs[i] = convertPulse_Liters(pulseCount_fs[i]);
      water_gal_fs[i] = convertLiters_Gals(water_liters_fs[i]);
      prev_Liters_fs[i] = water_liters_fs[i];
    }
  
    /* Testing Current Data Plotting*/
    //convertFloat_String(76.5, currentBuffer1);
    //convertFloat_String(23.75, currentBuffer2);
    //sprintf(currentBuffer3, "%d", 45);
    //sprintf(currentBuffer4, "%d", 2);
    //new_Data = TRUE;
    
    
    /**** UI Menu Control ****/
    //Continously Update System Time each loop
    t = time(NULL);
    tm = *localtime(&t);
    
    if (tm.tm_min != oledMinute){ //if new minute, update time
      oledHour = tm.tm_hour;
      oledMinute = tm.tm_min;
      if(oledHour == 0){ //Takes care of error w/ 12:00AM
        sprintf(timeBuffer,"%02d:%02d AM",(oledHour+12),oledMinute );
      } else if(oledHour < 12){
        sprintf(timeBuffer,"%02d:%02d AM",oledHour,oledMinute );
      } else if(oledHour == 12){         //Takes care of prev error w/ 12:00 PM
        sprintf(timeBuffer,"%02d:%02d PM",oledHour,oledMinute );
      } else {
        sprintf(timeBuffer,"%02d:%02d PM",(oledHour-12), oledMinute );
      }
    }
    
    // First check the buttons to inform the oled 
    checkButtons();
    // Then call the oled function to operate the UI
    OLED_SM(oledColor);
    
    
  }  // Loop
  
// Should NEVER get here
return(1);
}


/**************************************************************************************************/
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



/* @name: WaterDelivery
   @param: HOSE_NUM - an enum that specifies which hose to evaluate
   @return: uint8_t - a bit array of values that indicate which hoses are on/off
   This function determines if a hose needs to be turned on or off based on sensor data.
   The function also handles the control of the water delivery SM to turn on/off the H-bridge
 */
uint8_t WaterDelivery(HOSE_NUM HOSE_IN){
    // First reset the hose tally
    Hose[HOSE_IN].tally = 0;
    int prevstatus = Hose[HOSE_IN].status;

    // Then need to tally up the digital outs on the hose
    int i, j = 0;
    for (i = 0; i <= MAX_SENSORS; i++) {
        // This just shuts down the for loop if the list of sensors is exhausted
        if ((Hose[HOSE_IN].sensors[i] <= 0) || (sd_index == -1)) {
            break;
        }
        for (j = sd_index; j >= 0; j--) {
            // Check if the data item is a sensor mapped to the hose
            if ((sensor_data[j].nodeID == Hose[HOSE_IN].sensors[i]) && (sensor_data[j].nodeID)) {
                // If it is, increase the tally
                Hose[HOSE_IN].tally += sensor_data[j].digitalOut;
                break;
            }
        }
    }
    
    // Next check if the tally is above the water level threshold
    if (Hose[HOSE_IN].tally > Hose[HOSE_IN].waterLevel) {
        // Check the forecast data
        if (Forecast1.precipProb <= 0.3) {
            Hose[HOSE_IN].rainFlag = 0;
            // Go ahead and turn on the water
            Hose[HOSE_IN].status = WATER_ON;
        }
        else {
            // Checks if the rain flag is set
            // This prevents the rainTimer from being set more than once
            if (!Hose[HOSE_IN].rainFlag) {
                // Sets the rain flag
                Hose[HOSE_IN].rainFlag++;
                // Then the rain timer
                Hose[HOSE_IN].rainTimer = millis();
                // Turns off the hose to wait for the precip prob to take affect
                Hose[HOSE_IN].status = WATER_OFF;
            }
            // If it has been more than 36 hours since the rain timer was set...
            else if (Timer(HOURS_36, Hose[HOSE_IN].rainTimer)) {
                // ...Go ahead and turn on the water
                Hose[HOSE_IN].status = WATER_ON;
                // Also resets the rain Flag
                Hose[HOSE_IN].rainFlag = 0;
            }
        }
    }
    // Turn off the hose if the sensors indicate it is not dry enough to water
    else {
      Hose[HOSE_IN].status = WATER_OFF;
    }
    printf("Hose %d Water Delivery:\nHose Status: %d;  Prev State = %d\n\n", HOSE_IN, Hose[HOSE_IN].status, prevstatus);
    // Now we actually turn on or off the Hose
    if (prevstatus != Hose[HOSE_IN].status) {
        // If statements to control terminal printing
        if (Hose[HOSE_IN].status == WATER_ON) {
            printf("Turning ON hose...\n");
        }
        else {
            printf("Turning OFF hose...\n");
        }
        // Call the state machine to open the solenoid valve
        while (!WaterDeliverySM(Hose[HOSE_IN].status, FET_DELAY, PULSE_DURATION));
        // More if statments to control terminal printing
        if (Hose[HOSE_IN].status == WATER_ON) {
            printf("Hose successfully turned ON\n\n");
        }
        else {
            printf("Hose successfully turned OFF\n\n");
        }
    }
    // Create a bit array of hose states to return
    uint8_t hose_status = Hose[2].status * 4 + Hose[1].status * 2 + Hose[0].status;

    return hose_status;
}




/* @name: LPMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void LPMOS_Set(uint8_t status){
  DEV_Digital_Write(LPMOS_Pin, status);
}

/* @name: RPMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void RPMOS_Set(uint8_t status){
  DEV_Digital_Write(RPMOS_Pin, status);
}

/* @name: LNMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void LNMOS_Set(uint8_t status){
  DEV_Digital_Write(LNMOS_Pin, status);
}

/* @name: RNMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void RNMOS_Set(uint8_t status){
  DEV_Digital_Write(RNMOS_Pin, status);
}



/* @name: recordPulses_FS
 * @param: i, determines which flow Sensor to Record 
   * Updates Flow Sensor Pulse Count  
   @return: return
*/
void recordPulses_FS(int i){
  
  if (i == 0){
  tach_fs[i] = DEV_Digital_Read(FLOW_SENSOR_1_Pin);
  } else if (i == 1){
    tach_fs[i] = DEV_Digital_Read(FLOW_SENSOR_2_Pin);
  } else if (i == 2){
    tach_fs[i] = DEV_Digital_Read(FLOW_SENSOR_3_Pin);
  } else{
    printf("Error, Improper Flow Sensor Recorded");
  }
  
  if (tach_fs[i] != prevTach_fs[i] && tach_fs[i] == 1){
    pulseCount_fs[i]+=1;
    prevTach_fs[i] = tach_fs[i];
    printf(" Pulse Trigger 1 \n ");
  }
  prevTach_fs[i] = tach_fs[i];
  
  return;
}

  

/* @name: convertPulse_Liters
   @param: pulseCount - var keeping track of fs pulses 
   @return: liters - var keeping track of fs liters
   * 
   @return: Liters as a float
*/
float convertPulse_Liters(int pulseCount){
  float liters = 0;
  
  if(pulseCount < 6500 ){
    // FS_CAL_A  = 46.2   FS_CAL_B = 40.8
    liters = pulseCount / (FS_CAL_A + (FS_CAL_B * log(pulseCount) ) ) ;
    
  } else {
    liters = pulseCount / FS_CAL_STEADY; //FS_CAL_STEADY = 404
  }
  
  return liters;
}

/* @name: convertLiters_Gals
   @param: liters - var keeping track of fs liters
   @param: gallons - var keeping track of fs gallons
   * 
   @return: Gallons as a float
*/

float convertLiters_Gals(float liters){
  float gallons = 0;

  gallons = liters * LITERS_TO_GAL; //LITERS_TO_GAL = 0.264172
  
  return gallons;
}



/* @name: WaterDeliverSM 
   @param: status - whether to turn on or off WD
   @param: delayP_N - delay time between turning ON/OFF PFET and NFET
   @param: pulseTime - Time for +/-5V Pulse, Delays time between ON and OFF 
   @return: 1/0 depending on whether drive was completed
*/
int WaterDeliverySM(uint8_t status, uint32_t delayP_N, uint32_t pulseTime){
  w_State nextState = waterState; //initialize var to current state
  int hoseSet = FALSE; // Set to TRUE(1) once done Driving 
  
  switch(waterState){
    case HOSE_IDLE:
        // If the hose is supposed to be turned on
      if (status == WATER_ON){
        nextState = HOSE_ON_S1;
        wTimer = bcm2835_millis();
        hoseSet = 0; 
        printf("Leaving Hose Idle: On  \n");
      } 
      // If the hose is supposed to be turned off
      else if (status == WATER_OFF){
        nextState = HOSE_OFF_S1;
        wTimer = bcm2835_millis();
        hoseSet = 0; 
        printf("Leaving Hose Idle: off  \n");
      }
      break;
      
    // Breaks down the function into two parts
    // This first part handles turning on the H-bridge
    case HOSE_ON_S1:
      LNMOS_Set(NMOS_ON);
      //LNMOS_Set(DEMUX_ON);
      // Waits for the P_N delay before moving to the next state
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S2;
        printf("Leaving Hose On S1 \n");
      }
      break;
    
    case HOSE_ON_S2:
      RPMOS_Set(PMOS_ON); 
      //RPMOS_Set(DEMUX_ON);
      // Waits for the pulse delay before moving to the next state
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_ON_S3;
        printf("Leaving Hose On S2 \n");
      }
      break;
      
    case HOSE_ON_S3:
      RPMOS_Set(PMOS_OFF); 
      //RPMOS_Set(DEMUX_OFF);
      // Waits for the P_N delay before moving to the next state
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        printf("Leaving Hose On S3 \n");
        LNMOS_Set(NMOS_OFF);
        //LNMOS_Set(DEMUX_OFF);
        nextState = HOSE_IDLE;
        hoseSet = 1;
        printf("Leaving Hose On S4 \n");
      }
      break;
    

    // This second part handles turning off the H-bridge
    case HOSE_OFF_S1:
      RNMOS_Set(NMOS_ON);
      //RNMOS_Set(DEMUX_ON);
      // Waits for the P_N delay before moving to the next state
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S2;
        printf("Leaving Hose Off S1 \n");
      }
      break;
    
    case HOSE_OFF_S2:
      LPMOS_Set(PMOS_ON); 
      //LPMOS_Set(DEMUX_ON);
      if (Timer(pulseTime, wTimer)){
        wTimer = bcm2835_millis();
        nextState = HOSE_OFF_S3;
        printf("Leaving Hose Off S2 \n");
      }
      break;
      
    case HOSE_OFF_S3:
      LPMOS_Set(PMOS_OFF); 
      //LPMOS_Set(DEMUX_OFF);
      // Waits for the P_N delay before moving to the next state
      if (Timer(delayP_N, wTimer)){
        wTimer = bcm2835_millis();
        printf("Leaving Hose Off S3 \n");
        RNMOS_Set(NMOS_OFF);
        //RNMOS_Set(DEMUX_OFF);
        nextState = HOSE_IDLE;
        hoseSet = 1;
        printf("Leaving Hose Off S4 \n");
      }
      break;
  }
  waterState = nextState;
  return hoseSet;  //1 if set, 0 if still in S1-4

}


/**
   @Function LCD_PrintArrow(int state)
   @param int x, int y Used to determine x,y position of arrow
   @return None
   @brief This function prints Arrow on OLED at x,y coordinates
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void OLED_PrintArrow(int x, int y) {
  print_String(x,y, (const uint8_t*)"<", FONT_5X8);
}

/* @name: OLED_SM 
   @param: Color of Page Text
   @return: void
*/

void OLED_SM(uint16_t color){
  int16_t temp_x,temp_y = 0;
  int i, j = 0;
  int element_Changed = FALSE;
  int arrowOptions = 0;

  
  
  Set_Color(color);
  
  if (Timer(sleepTime, oledSleepTimer)) { //If Sleep Timer Expires, return to SLEEP
    arrowState = 0;
    nextPage = SLEEP;
    Clear_Screen();
    oledSleepTimer = bcm2835_millis(); //reset sleep timer after transition to Sleep
  }
  
  oledState = nextPage; //Transition to next state
  prevArrowState = arrowState; //Save arrow State
  
  //Toggle Arrow
  if (DOWN_PRESSED) { //if down, increment arrowstate.
    if (arrowState >= 3) {
      arrowState = 0;
    } else {
      arrowState++;
    }
    oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
  } else if (UP_PRESSED){
    if (arrowState <= 0) { //otherwise, decrement arrowstate.
      arrowState = 3;
    } else {
      arrowState--;
    }
    oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
  }


  switch(oledState){
    case WELCOME_PAGE: 
      print_String(24,25, (const uint8_t*)"Welcome To ", FONT_8X16);  //Print Home Page
      print_String(30,55, (const uint8_t*)"Intuitive", FONT_8X16);
      print_String(8,85, (const uint8_t*)"Auto Irrigation", FONT_8X16);
      
      
      if (Timer(EIGHT_SECONDS, oledSleepTimer) || ENTER_PRESSED) { //If Sleep Timer Expires, return to SLEEP
        arrowState = 0; //Reset Arrow State
        nextPage = HOME_PAGE;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after transition to Sleep
      }
      break;
      
      
    case SLEEP:
      print_String(35,55, (const uint8_t*)"SLEEPING", FONT_8X16);
      print_String(35,85, (const uint8_t*)timeBuffer, FONT_8X16);
      
      if (ENTER_PRESSED){
        nextPage = HOME_PAGE;
        arrowState = 0; //Reset Arrow State
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
    
    case HOME_PAGE:
      if (prevArrowState != arrowState){  //Update Screen if arrowState changes
        Clear_Screen();
      }  
        
      print_String(0,0, (const uint8_t*)"Home Page", FONT_8X16);
      print_String(0,30, (const uint8_t*)"Sensor Data", FONT_5X8);
      print_String(0,45, (const uint8_t*)"Hose Configuration", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Settings", FONT_5X8);
      print_String(35,95, (const uint8_t*)timeBuffer, FONT_8X16);
      
      //Update Oled Printing
      if(arrowState == 0){
        OLED_PrintArrow(70, 30);
      } else if (arrowState == 1){
        OLED_PrintArrow(113, 45);
      } else {
        OLED_PrintArrow(55, 60);
      } 
      
      
      if (ENTER_PRESSED){ //Enter Page Corresponding to Arrow State
        if(arrowState == 0){
          nextPage = SENSORS_HOME;
        } else if(arrowState == 1){
          nextPage = HOSES_HOME;
        } else{
          nextPage = SETTINGS_HOME;
        }
        arrowState = 0; //Reset Arrow State
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
        
      } else if (BACK_PRESSED){
        nextPage = SLEEP;
        arrowState = 0;//Reset Arrow State
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
    
    case SENSORS_HOME:
      if (prevArrowState != arrowState){  //Update Screen if arrowState changes
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Sensors Home", FONT_8X16);
      
      print_String(0,30, (const uint8_t*)"Connected Sensors", FONT_5X8);
      print_String(0,45, (const uint8_t*)"Current Readings", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Plot Sensor Data", FONT_5X8);
      print_String(35,95, (const uint8_t*)timeBuffer, FONT_8X16);
      
      
      if(arrowState == 0){
        OLED_PrintArrow(110, 30);
      } else if(arrowState == 1){
        OLED_PrintArrow(110, 45);
      } else {
        OLED_PrintArrow(110, 60);
      } 
      
      if (ENTER_PRESSED){
        Clear_Screen();
        if(arrowState == 0){
          nextPage = SENSORS_LIST;
        } else if(arrowState == 1){
          nextPage = SENSORS_CURRENT;
        } else {
          nextPage = SENSORS_PLOT_START;
        } 
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = HOME_PAGE;
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
      
      case SENSORS_LIST:
      if (prevArrowState != arrowState){  //Update Screen if arrowState changes
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Sensors List", FONT_8X16);
    
      temp_x = 0;
      temp_y = 30;
      
      for (i = 0; i < mesh.addrListTop; i++) {  //Prints all Connected Sensors
            // Add sensor nodes to the list of sensors mapped to the hose
            sprintf(sensorIDBuffer,"Sensor Node %d", mesh.addrList[i].nodeID);
            print_String(temp_x,temp_y, (const uint8_t*)sensorIDBuffer, FONT_5X8); 
            temp_y += 15;
        }
      
      
      if(arrowState == 0){    //Update Arrow State 
        OLED_PrintArrow(110, 30);
      } else {
        OLED_PrintArrow(110, 45);
      } 
      
      if (ENTER_PRESSED){
        nextPage = SENSORS_CURRENT; //Go into Current Sensors Menu
        selected_Node = 2;  //Set selected Node for printing
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
        
      } else if (BACK_PRESSED){
        nextPage = SENSORS_HOME; 
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
      
      case SENSORS_CURRENT:
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      if (new_Data){
        Clear_Screen();
        new_Data = FALSE;
      }
      print_String(0,0, (const uint8_t*)"Current Data", FONT_8X16);
      print_String(0,30, (const uint8_t*)"Node ID:", FONT_5X8);
      print_String(0,45, (const uint8_t*)"Moisture(%):", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Light(%):", FONT_5X8);
      print_String(0,75, (const uint8_t*)"Temp(C):", FONT_5X8);
      
      
      if (selected_Node == 2){ //Store Struct Variables as Strings
        convertFloat_String(current_Dat_1.soilMoisture, currentBuffer1);
        convertFloat_String(current_Dat_1.lightLevel, currentBuffer2);
        sprintf(currentBuffer3, "%d", current_Dat_1.temp_C);
        sprintf(currentBuffer4, "%d", current_Dat_1.nodeID);
      } else if (selected_Node == 5){
        convertFloat_String(current_Dat_2.soilMoisture, currentBuffer1);
        convertFloat_String(current_Dat_2.lightLevel, currentBuffer2);
        sprintf(currentBuffer3, "%d", current_Dat_2.temp_C);
        sprintf(currentBuffer4, "%d", current_Dat_2.nodeID);
      } else if (selected_Node == 4){
        convertFloat_String(current_Dat_3.soilMoisture, currentBuffer1);
        convertFloat_String(current_Dat_3.lightLevel, currentBuffer2);
        sprintf(currentBuffer3, "%d", current_Dat_3.temp_C);
        sprintf(currentBuffer4, "%d", current_Dat_3.nodeID);
      }
      
      //Print Variable Strings
      print_String(55,30, (const uint8_t*)currentBuffer4, FONT_5X8);
      print_String(72,45, (const uint8_t*)currentBuffer1, FONT_5X8);
      print_String(60,60, (const uint8_t*)currentBuffer2, FONT_5X8);
      print_String(55,75, (const uint8_t*)currentBuffer3, FONT_5X8);

      if(arrowState == 0){
        OLED_PrintArrow(65, 30);
      } else if(arrowState == 1){
        OLED_PrintArrow(115, 45);
      } else if(arrowState == 2){
        OLED_PrintArrow(110, 60);
      } else{
        OLED_PrintArrow(70, 75);
      } 
      
      
      if (ENTER_PRESSED){
        if(arrowState == 0){
          if(selected_Node == 2){
            selected_Node = 5;
          } else if (selected_Node == 5){
            selected_Node = 4;
          } else {
            selected_Node = 2;
          }
          new_Data = TRUE;
        } else if(arrowState == 1){
          dataType = MOISTURE;
          nextPage = SENSORS_PLOT;
        } else if(arrowState == 2){
          dataType = SUNLIGHT;
          nextPage = SENSORS_PLOT;
        } else {
          dataType = TEMP;
          nextPage = SENSORS_PLOT;
        } 
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = SENSORS_HOME;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
      
      case SENSORS_PLOT_START:
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      print_String(0,0, (const uint8_t*)"Plot Sensor Data", FONT_8X16);
      print_String(0,30, (const uint8_t*)"Moisture", FONT_5X8);
      print_String(0,45, (const uint8_t*)"Sunlight", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Temperature", FONT_5X8);
      
      
      if(arrowState == 0){
        OLED_PrintArrow(110, 30);
      } else if (arrowState == 1){
        OLED_PrintArrow(110, 45);
      } else {
        OLED_PrintArrow(110, 60);
      }
      
      if (ENTER_PRESSED){
        Clear_Screen();
        if(arrowState == 2){
          dataType = TEMP;
        } else if (arrowState == 1){
          dataType = SUNLIGHT;
        } else {
          dataType = MOISTURE;
        }
        nextPage = SENSORS_PLOT;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
        
      } else if (BACK_PRESSED){
        nextPage = SENSORS_HOME;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
    

    case SENSORS_PLOT:
      
      //Set Up Grid for Printing
      printGrid(20,120,20,120,10,10);
      printAxesLabels(0,115);
      
      
      //Plot Corresponding to Sensor Node
      if(selected_Node == 2){
        plotSampleData(sensor1_data, dataType, MAX_ELEMENTS);
      } else if (selected_Node == 5){
        plotSampleData(sensor2_data, dataType, MAX_ELEMENTS);
      } else if (selected_Node == 4){
        plotSampleData(sensor3_data, dataType, MAX_ELEMENTS);
      } else {
        plotSampleData(sensor_data, dataType, MAX_ELEMENTS);
      }
    
      
      if(ENTER_PRESSED){  //Update Sensor Struct for Plotting
        if(selected_Node == 2){
          selected_Node = 5;
        } else if(selected_Node == 5){
          selected_Node = 4;
        } else {
          selected_Node = 2;
        }
        
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = SENSORS_PLOT_START;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      
      break;
      
   
    
    case HOSES_HOME: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Hoses", FONT_8X16);
      
      print_String(0,30, (const uint8_t*)"Current Hose Status", FONT_5X8);
      print_String(0,45, (const uint8_t*)"Hose Control", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Watering Log", FONT_5X8);
      print_String(0,75, (const uint8_t*)"Map Sensors ", FONT_5X8);
      print_String(35,95, (const uint8_t*)timeBuffer, FONT_8X16);
      
      if(arrowState == 0){  //Update Arrow
        OLED_PrintArrow(115, 30);
      } else if (arrowState == 1) {
        OLED_PrintArrow(80, 45);
      } else if (arrowState == 2) {
        OLED_PrintArrow(80, 60);
      } else {
        OLED_PrintArrow(85, 75);
      }
      
      if (ENTER_PRESSED){   //Menu traversal based on Arrowstate
        if(arrowState == 0){
          nextPage = HOSES_STATUS;
        } else if (arrowState == 1){
          nextPage = HOSES_CONTROL;
        } else if (arrowState == 2) {
          nextPage = HOSES_WATER;
        } else {
          nextPage = HOSES_MAP;
        }
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = HOME_PAGE;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
      
    case HOSES_STATUS: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
    
      if(hose_statuses != prev_hose_statuses) {
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Hoses Status", FONT_8X16);
      printHoseStatus(0,40,hose_statuses);  //Print OFF/ON for each hose
      prev_hose_statuses = hose_statuses; //Store statuses for OLED updating
      
    
      // Print Connected Sensors
      temp_x = 90;
      temp_y = 40;
      
      
      
      //Update Hose 0 Nodes
      for (i = 0; i < hose0_elements; i++){   //Iterate and print connected node IDs
        sprintf(intBuffer , "%d", Hose[HOSE0].sensors[hose0_elements]);
        print_String(temp_x,temp_y, (const uint8_t*)intBuffer, FONT_5X8);
        temp_x += 10;
      }
      
      temp_x = 90;
      temp_y = 55;
      
      //Update Hose 1 Nodes
      for (i = 0; i < hose1_elements; i++){   //Iterate and print connected node IDs
        sprintf(intBuffer , "%d", Hose[HOSE1].sensors[hose1_elements]);
        print_String(temp_x,temp_y, (const uint8_t*)intBuffer, FONT_5X8);
        temp_x += 10;
      }
      
      temp_x = 90;
      temp_y = 70;
      
      //Update Hose 2 Nodes
      for (i = 0; i < hose2_elements; i++){   //Iterate and print connected node IDs
        sprintf(intBuffer , "%d", Hose[HOSE2].sensors[hose2_elements]);
        print_String(temp_x,temp_y, (const uint8_t*)intBuffer, FONT_5X8);
        temp_x += 10;
      }
      

      if(arrowState == 0){
        OLED_PrintArrow(100, 40);
      } else if (arrowState == 1) {
        OLED_PrintArrow(100, 55);
      } else {
        OLED_PrintArrow(100, 70);
      }  
      
      if (ENTER_PRESSED){ 
        nextPage = HOSES_CONTROL;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = HOSES_HOME;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
    
    case HOSES_CONTROL: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      if (element_Changed == TRUE){ //Update screen only once change is made
        Clear_Screen(); 
        element_Changed = FALSE;
      }
    
      
      print_String(0,0, (const uint8_t*)"Hose Control", FONT_8X16);
    
      temp_x = 0;  //Initialize Starting Print Locations
      temp_y = 40;
      
      for (i = 0; i < NUM_HOSES; i++ ){ //Iterate and print hose control status
        
        if (Hose[i].control == AUTOMATIC){
          sprintf(hoseBuffer,"Hose %d: AUTO",(i+1));
          print_String(temp_x,temp_y, (const uint8_t*)hoseBuffer, FONT_5X8);
          
        } else if (Hose[i].control == ON){
          sprintf(hoseBuffer,"Hose %d: ON",(i+1));
          print_String(temp_x,temp_y, (const uint8_t*)hoseBuffer, FONT_5X8);
        } else {
          sprintf(hoseBuffer,"Hose %d: OFF",(i+1));
          print_String(temp_x,temp_y, (const uint8_t*)hoseBuffer, FONT_5X8);
        }
        
        temp_y += 15; //Increment 15 to move to next row
        
      }
      
      
      if(arrowState == 0){
        OLED_PrintArrow(100, 40);
      } else if (arrowState == 1) {
        OLED_PrintArrow(100, 55);
      } else {
        OLED_PrintArrow(100, 70);
      }
          
      i = 0; //initialize index variable      
      if (ENTER_PRESSED){
        if (arrowState <= 1){
          i = arrowState;
        } else {
          i = 2; //else store as last element
        }
        
        if (Hose[i].control == AUTOMATIC){
          Hose[i].control = OFF;
          
        } else if (Hose[i].control == OFF){
          Hose[i].control = ON;
        } else {
          Hose[i].control = AUTOMATIC;
        }
      
        
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = HOSES_HOME;
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;  
        
    case HOSES_WATER: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      
      for(i=0; i>3; i++){ //Update Screen if new data received
        if(prev_Liters_fs[i] != water_liters_fs[i]){
          Clear_Screen();
        }
      }
      
      print_String(0,0, (const uint8_t*)"Watering Log", FONT_8X16);
      
      //Convert Floats containing amount of water in liters into strings
      //tempVal = Hose0.waterLevel;
      convertFloat_String(water_liters_fs[0], hoseBuffer);
      print_String(0,40, (const uint8_t*)"Hose 1:", FONT_5X8);
      print_String(50,40, (const uint8_t*)hoseBuffer, FONT_5X8);
      print_String(90,40, (const uint8_t*)"L", FONT_5X8);
      
      //tempVal = Hose1.waterLevel;
      //tempVal = 3;
      convertFloat_String(water_liters_fs[1], hoseBuffer);
      print_String(0,55, (const uint8_t*)"Hose 2:", FONT_5X8);
      print_String(50,55, (const uint8_t*)hoseBuffer, FONT_5X8);
      print_String(90,55, (const uint8_t*)"L", FONT_5X8);
      
      //tempVal = 5; //used for testing
      // sprintf(hoseBuffer,"%d L", tempVal);
      
      convertFloat_String(water_liters_fs[2], hoseBuffer);
      print_String(0,70, (const uint8_t*)"Hose 3:", FONT_5X8);
      print_String(50,70, (const uint8_t*)hoseBuffer, FONT_5X8);
      print_String(90,70, (const uint8_t*)"L", FONT_5X8);
      
      //tempVal = Hose2.waterLevel + Hose1.waterLevel + Hose0.waterLevel;
      //tempVal = 8; //used for testing
      //sprintf(hoseBuffer,"%d L", tempVal);
      
      convertFloat_String((water_liters_fs[0]+water_liters_fs[1]+water_liters_fs[2]), hoseBuffer);
      print_String(0,85, (const uint8_t*)"Total:", FONT_5X8);
      print_String(50,85, (const uint8_t*)hoseBuffer, FONT_5X8);
      print_String(90,85, (const uint8_t*)"L", FONT_5X8);
      
      for(i=0; i<3; i++){
        prev_Liters_fs[i] = water_liters_fs[i]; //Save previous readings
      }
      
      if(arrowState == 0){    //Update Arrow State
        OLED_PrintArrow(100, 40);
      } else if (arrowState == 1) {
        OLED_PrintArrow(100, 55);
      } else {
        OLED_PrintArrow(100, 70);
      }
      
      if (ENTER_PRESSED){
        nextPage = SLEEP;
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = HOSES_HOME;
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
      
    case HOSES_MAP: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Map Sensors", FONT_8X16);
      
      temp_x = 0;
      temp_y = 30;
      

      arrowOptions = 0; //Initialize Arrow Options
      nodeUnmapped = FALSE; //Initialize flag
      j = 0; //Initialize Secondary Index
      
      for (i = 0; i < mesh.addrListTop; i++) {
            // Add sensor nodes to the list of sensors mapped to the hose
            if (mesh.addrList[i].nodeID != mappedSensors[i]){ //Check if mapped
              sprintf(sensorIDBuffer,"Sensor Node %d", mesh.addrList[i].nodeID);
              print_String(temp_x,temp_y, (const uint8_t*)sensorIDBuffer, FONT_5X8);
              unmappedSensors[j] = i; //Save Array Index for Selected Sensor
              arrowOptions += 1; // Increment amount of arrow options
              nodeUnmapped = TRUE; //Set Flag to true
              temp_y += 15; //Increment to new line
              j += 1; //Increment to next element
            }
        }
        
        if(!nodeUnmapped){ //if no Sensors left to map
          print_String(0,45, (const uint8_t*)"No Sensors to Map", FONT_5X8);
        }
        
      /*
      if (nodeMapState == 0){
        print_String(0,45, (const uint8_t*)"Sensor Node 2", FONT_5X8);
        print_String(0,60, (const uint8_t*)"Sensor Node 5", FONT_5X8);
        print_String(0,75, (const uint8_t*)"Sensor Node 4", FONT_5X8);
      } else if (nodeMapState == 1){
        print_String(0,45, (const uint8_t*)"Sensor Node 5", FONT_5X8);
        print_String(0,60, (const uint8_t*)"Sensor Node 4", FONT_5X8);
      } else if (nodeMapState == 2){
        print_String(0,45, (const uint8_t*)"Sensor Node 5", FONT_5X8);
      } else {
        print_String(0,60, (const uint8_t*)"Back", FONT_5X8);
        print_String(0,45, (const uint8_t*)"No Sensors to Map", FONT_5X8);
      }
      
      */
      
      if(arrowOptions == 4){  //Update Arrow State Corresponding to # of connected sensors
        if(arrowState == 0){
          OLED_PrintArrow(90, 45);
        } else if(arrowState == 1){
          OLED_PrintArrow(90, 60);
        } else if(arrowState == 2){
          OLED_PrintArrow(90, 75);
        } else {
          OLED_PrintArrow(50, 90);
        }
      } else if(arrowOptions == 3){
        if(arrowState == 0){
          OLED_PrintArrow(90, 45);
        } else if(arrowState == 1){
          OLED_PrintArrow(90, 60);
        } else {
          OLED_PrintArrow(50, 75);
        }
      } else if(arrowOptions == 2){
        if(arrowState == 0){
          OLED_PrintArrow(90, 45);
        } else if(arrowState == 1){
          OLED_PrintArrow(90, 60);
        } else {
          OLED_PrintArrow(50, 75);
        }
      } else if(arrowOptions == 3){
        if(arrowState == 0){
          OLED_PrintArrow(90, 45);
        } else if(arrowState == 1){
          OLED_PrintArrow(90, 60);
        } else {
          OLED_PrintArrow(50, 75);
        }
      } else if(arrowOptions == 3){
        if(arrowState == 0 || arrowState == 2){
          OLED_PrintArrow(90, 45);
        } else {
          OLED_PrintArrow(50, 60);
        }
      } else if(arrowOptions == 1){
          OLED_PrintArrow(90, 45);
      }
      
      if (ENTER_PRESSED){
        sensorToMap = unmappedSensors[arrowState]; //Transition to corresponding arrowState
        nextPage = HOSES_MAP_SELECT; //Transition to Map Select 
        
        if(!nodeUnmapped){ //If There are no more nodes to map
          nextPage = HOSES_HOME;
        } 
        
        arrowState = 0; //reset arrow State
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
        
      } else if (BACK_PRESSED){
        nextPage = SENSORS_HOME;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;   
    
    case HOSES_MAP_SELECT: //page allowing for selection of hoses to map sensor to
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Select Hose", FONT_8X16);
      
      print_String(0,45, (const uint8_t*)"Hose 1", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Hose 2", FONT_5X8);
      print_String(0,75, (const uint8_t*)"Hose 3", FONT_5X8);
      
      if(mesh.addrList[sensorToMap].nodeID == 2){
        print_String(45,30, (const uint8_t*)"(Node 2)", FONT_5X8);
      } else if(mesh.addrList[sensorToMap].nodeID == 2){
        print_String(45,30, (const uint8_t*)"(Node 4)", FONT_5X8);
      } else {
        print_String(45,30, (const uint8_t*)"(Node 5)", FONT_5X8);
      }
    
      if(arrowState == 0){
        OLED_PrintArrow(50, 45);
      } else if (arrowState == 1) {
        OLED_PrintArrow(50, 60);
      } else {
        OLED_PrintArrow(50, 75);
      } 
    
      if (ENTER_PRESSED){ //If Enter Pressed, Save Hose Sensors within Struct
        if(arrowState == 0){
          Hose[HOSE0].sensors[hose0_elements] = mesh.addrList[sensorToMap].nodeID;
          hose0_elements++;
        } else if (arrowState == 1) {
          Hose[HOSE1].sensors[hose1_elements] = mesh.addrList[sensorToMap].nodeID;
          hose1_elements++;
        } else {
          Hose[HOSE2].sensors[hose2_elements] = mesh.addrList[sensorToMap].nodeID;
          hose2_elements++;
        } 
        
        //Store List of Mapped Sensor Node ID's corresponding to Mesh Array Index
        mappedSensors[sensorToMap] = mesh.addrList[sensorToMap].nodeID;  
                                          
        arrowState = 0;
        nextPage = HOSES_MAP;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
        
      } else if (BACK_PRESSED){
        nextPage = HOSES_MAP;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      
      break;
    
    case SETTINGS_HOME:
    
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      //Print Settings Menu
      print_String(0,0, (const uint8_t*)"Settings", FONT_8X16);
      
      print_String(0,30, (const uint8_t*)"Adjust Sleep Timer", FONT_5X8);
      print_String(0,45, (const uint8_t*)"Calibrate Sensors", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Change Menu Color", FONT_5X8);
      print_String(0,75, (const uint8_t*)"Reset System", FONT_5X8);
      print_String(35,95, (const uint8_t*)timeBuffer, FONT_8X16);
      
       //Update Arrow Print statements depending on arrowState
      if(arrowState == 0){   
        OLED_PrintArrow(112, 30);
      } else if (arrowState == 1) {
        OLED_PrintArrow(105, 45);
      } else if(arrowState == 2){
        OLED_PrintArrow(112, 60);
      } else{
        OLED_PrintArrow(85, 75);
      }
      
      if (ENTER_PRESSED){ //Traversal to Page
        if(arrowState == 0){
          nextPage = SETTINGS_SLEEP;
        } else if (arrowState == 1){
          nextPage = SETTINGS_CAL;
        }  else if(arrowState == 2){
          nextPage = SETTINGS_COLOR;
        }  else {
          nextPage = SETTINGS_RESET;
        }
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){ // Return to Home
        nextPage = HOME_PAGE;
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;
      
     case SETTINGS_SLEEP: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      print_String(0,0, (const uint8_t*)"Sleep Settings", FONT_8X16);
      
      //Print out current Sleep TIme 
      print_String(0,30, (const uint8_t*)"Current:", FONT_5X8);
      if(sleepTime == 1){
        sprintf(intBuffer,"%d Min",(sleepTime/MIN_1));
      } else{
        sprintf(intBuffer,"%d Mins",(sleepTime/MIN_1));
      }
      
      //Sleep Menu Options
      print_String(70,30, (const uint8_t*)intBuffer, FONT_5X8);
      print_String(0,50, (const uint8_t*)"1 Minute", FONT_5X8);
      print_String(0,65, (const uint8_t*)"3 Minutes", FONT_5X8);
      print_String(0,80, (const uint8_t*)"5 Minutes", FONT_5X8);
      print_String(0,95, (const uint8_t*)"SLEEP", FONT_5X8);
      
      //Update Oled States 
      if(arrowState == 0){
        OLED_PrintArrow(65, 50);
      } else if (arrowState == 1) {
        OLED_PrintArrow(65, 65);
      } else if(arrowState == 2){
        OLED_PrintArrow(65, 80);
      } else{
        OLED_PrintArrow(50, 95);
      }
      
      if (ENTER_PRESSED){  //Set Timers corresponding to arrowState
        if(arrowState == 0){
          sleepTime = MIN_1;
        } else if (arrowState == 1){
          sleepTime = MIN_3;
        }  else if(arrowState == 2){
          sleepTime = MIN_5;
        }  else {
          nextPage = SLEEP;
          arrowState = 0;
        }
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = SETTINGS_HOME;
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;  
      
     case SETTINGS_CAL: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      if (new_Data){ //Updates each time a message is received 
        Clear_Screen();
        new_Data = FALSE;
      }
      
      //Menu Used for Calibration
      print_String(0,0, (const uint8_t*)"Sensor Recal", FONT_8X16);
      print_String(0,30, (const uint8_t*)"Node ID:", FONT_8X16);
      print_String(0,50, (const uint8_t*)"Moisture(%):", FONT_5X8);
      print_String(0,80, (const uint8_t*)"Prev Threshold(%):", FONT_5X8);
      print_String(0,100, (const uint8_t*)"Set as ", FONT_5X8);
      print_String(0,110, (const uint8_t*)"New Threshold ", FONT_5X8);
      
      
      //Check corresponding node Ids
      //Print current soil Moisture reading 
      if (selected_Node == 2){ //Store Struct Variables as Strings
        convertFloat_String(current_Dat_1.soilMoisture, currentBuffer1);
        sprintf(currentBuffer4, "%d", current_Dat_1.nodeID);
        convertFloat_String(moisture_s_thresh[0], currentBuffer5);
        
      } else if (selected_Node == 5){
        convertFloat_String(current_Dat_2.soilMoisture, currentBuffer1);
        sprintf(currentBuffer4, "%d", current_Dat_2.nodeID);
        convertFloat_String(moisture_s_thresh[1], currentBuffer5);
      } else if (selected_Node == 4){
        convertFloat_String(current_Dat_3.soilMoisture, currentBuffer1);
        sprintf(currentBuffer4, "%d", current_Dat_3.nodeID);
        convertFloat_String(moisture_s_thresh[2], currentBuffer5);
      }
      
      //Print Variable Strings
      print_String(70,30, (const uint8_t*)currentBuffer4, FONT_8X16);
      print_String(10,60, (const uint8_t*)currentBuffer1, FONT_5X8);
      print_String(10,90, (const uint8_t*)currentBuffer5, FONT_5X8);
      
      if(arrowState == 0 || arrowState == 2){
        OLED_PrintArrow(95, 33);
        
      } else {
        OLED_PrintArrow(85, 110);
        
        
      } 
      
      //Update Selected Node on Enter being pressed 
      if (ENTER_PRESSED){
        if(arrowState == 0 || arrowState == 2){
          if(selected_Node == 2){
            selected_Node = 5;
          } else if (selected_Node == 5){
            selected_Node = 4;
          } else {
            selected_Node = 2;
          }
          new_Data = TRUE;
          arrowState = 0;
        } else {
          if (selected_Node == 2){
            moisture_s_thresh[0] = current_Dat_1.soilMoisture;
          } else if (selected_Node == 5){
            moisture_s_thresh[1] = current_Dat_2.soilMoisture;
          } else {
            moisture_s_thresh[2] = current_Dat_3.soilMoisture;
          }
          new_Data = TRUE;
        } 
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = SETTINGS_HOME;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break; 
      
      case SETTINGS_COLOR: 
  
      if (prevArrowState != arrowState){
        Clear_Screen();
      }
      
      
      //Present Color Settings with available options 
      print_String(0,0, (const uint8_t*)"Color Settings", FONT_8X16);
      
      print_String(0,30, (const uint8_t*)"White", FONT_5X8);
      print_String(0,45, (const uint8_t*)"Blue", FONT_5X8);
      print_String(0,60, (const uint8_t*)"Green", FONT_5X8);
      print_String(0,75, (const uint8_t*)"Red", FONT_5X8);
      
      
      
      if(arrowState == 0){
        OLED_PrintArrow(40, 30);
      } else if (arrowState == 1) {
        OLED_PrintArrow(40, 45);
      } else if(arrowState == 2){
        OLED_PrintArrow(40, 60);
      } else{
        OLED_PrintArrow(40, 75);
      }
      
      //Change color corresponding to arrowState on Enter Press
      if (ENTER_PRESSED){
        if(arrowState == 0){
          oledColor = WHITE;
        } else if (arrowState == 1){
          oledColor = BLUE;
        }  else if(arrowState == 2){
          oledColor = GREEN;
        }  else {
          oledColor = RED;
        }
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = SETTINGS_HOME; //Return to Settings Page
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break; 
      
      case SETTINGS_RESET: 
      if (prevArrowState != arrowState){
        Clear_Screen();
      }  
      
      //Print Reset Statement 
      print_String(0,0, (const uint8_t*)"System Reset", FONT_8X16);
      
      print_String(0,30, (const uint8_t*)"This will reset all ", FONT_5X8);
      print_String(0,40, (const uint8_t*)"system settings to ", FONT_5X8);
      print_String(0,50, (const uint8_t*)"default", FONT_5X8);
      
      print_String(0,70, (const uint8_t*)"Are you sure?", FONT_5X8);
      print_String(80,70, (const uint8_t*)"No", FONT_5X8);
      print_String(80,90, (const uint8_t*)"Yes", FONT_5X8);
      
      if(arrowState == 0 || arrowState == 2){
        OLED_PrintArrow(95, 70);
      } else{
        OLED_PrintArrow(100, 90);
      }
      
      if (ENTER_PRESSED){
        if(arrowState == 0 || arrowState == 2){
          nextPage = SETTINGS_HOME;
        } else {
          Reset_System();  //Resets all global variables of system
          nextPage = WELCOME_PAGE;
        } 
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      } else if (BACK_PRESSED){
        nextPage = SETTINGS_HOME;
        arrowState = 0;
        Clear_Screen();
        oledSleepTimer = bcm2835_millis(); //reset sleep timer after each button press
      }
      break;    
      
    }
    
    //prevArrowState = arrowState;
    //oledState = nextPage;
    
    return;
}




/**
   @Function checkButtons(void)
   @param None
   @return None
   @brief This function uses debouncing to checksButtons and sets appropriate flag
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */

void checkButtons(void) {

  ENTER_PRESSED = FALSE;
  BACK_PRESSED = FALSE;
  DOWN_PRESSED = FALSE;
  UP_PRESSED = FALSE;

  enterButtonValue = DEV_Digital_Read(ENTER_Pin);
  downButtonValue = DEV_Digital_Read(DOWN_Pin);
  backButtonValue = DEV_Digital_Read(BACK_Pin);
  upButtonValue = DEV_Digital_Read(UP_Pin);

  DEV_Delay_ms(5);
  
  enterButtonValue2 = DEV_Digital_Read(ENTER_Pin);
  downButtonValue2 = DEV_Digital_Read(DOWN_Pin);
  backButtonValue2 = DEV_Digital_Read(BACK_Pin);
  upButtonValue2 = DEV_Digital_Read(UP_Pin);

  if (enterButtonValue == enterButtonValue2) {
    if (enterButtonValue != lastEnterButtonState) { //Change in State
      if (enterButtonValue == LOW) { //Flipped, Low is pressed
        ENTER_PRESSED = TRUE; //set flag TRUE
        printf("Enter Pressed \r \n ");
      } else {
        ENTER_PRESSED = FALSE; //set flag FALSE
      }
      lastEnterButtonState = enterButtonValue;
    }
  } if (downButtonValue == downButtonValue2) {
    if (downButtonValue != lastDownButtonState) { //Change in State
      if (downButtonValue == LOW) { //Flipped, Low is pressed
        DOWN_PRESSED = TRUE; //set flag TRUE
        printf("Down Pressed \r \n ");
      } else {
        DOWN_PRESSED = FALSE; //set flag FALSE
      }
      lastDownButtonState = downButtonValue;
    }
  } if (upButtonValue == upButtonValue2) {
    if (upButtonValue != lastUpButtonState) { //Change in State
      if (upButtonValue == LOW) { //Flipped, Low is pressed
        UP_PRESSED = TRUE; //set flag TRUE
        printf("Up Pressed \r \n ");
      } else {
        UP_PRESSED = FALSE; //set flag FALSE
      }
      lastUpButtonState = upButtonValue;
    }
  } if (upButtonValue == upButtonValue2) {
    if (backButtonValue != lastBackButtonState) { //Change in State
      if (backButtonValue == LOW) { //Flipped, Low is pressed
        BACK_PRESSED = TRUE; //set flag TRUE
        printf("Back Pressed \r \n ");
      } else {
        BACK_PRESSED = FALSE; //set flag FALSE
      }
      lastBackButtonState = backButtonValue;
    }
  }
}

/* @name: convertFloat_String
   @param: in - float to be converted to String
   @param: buffer - char variable used to store float string output
   @return: 0
*/

int convertFloat_String(float in, char buffer[100]){
    temp_wholeVal = in; //Store float into temporary float variable
  
    if (in < 0){
      temp_wholeVal = -in; //Store positive value if negative
    }                     //Minus sign taken care of in print statement 

    wholeVal = temp_wholeVal; //Store Whole Integer Value

    temp_decimalVal = temp_wholeVal - wholeVal; //Obtain remainder as float
  
    decimalVal = trunc(temp_decimalVal*10000); //Store decimal value as whole int
    
    //Store Int Values into string to format as a float value
    if (in < 0){
      sprintf(buffer,"-%d.%01d", wholeVal, decimalVal );
    } else {
      sprintf(buffer,"%d.%01d", wholeVal, decimalVal );
    }
    
    return 0;
} 


/* @name: printGrid
   @param: x0 - initial x position for grid
   @param: x1 - final x position for grid
   @param: y0 - initial y position for grid
   @param: y1 - final y position for grid
   @param: xtics - # of lines on x line
   @param: ytics - # of lines on y line
   @return: TRUE/FALSE depending if grid was successfully printed
*/

int printGrid(int16_t x0, int16_t x1, int16_t y0, int16_t y1, int16_t xtics, int16_t ytics){
  int i = 0;
  int xTic = 0;
  int yTic = 0;
  int incrementX =  ( x1 - x0 ) / xtics ;
  int incrementY =  ( y1 - y0 ) / ytics ;
  
  //printf("Xspaces: %d", incrementX); //Testing incrementX/Y
  //printf("Yspaces: %d", incrementY);
  
  //print x-axis
  Write_Line(x0, y1, x1, y1);
  
  xTic = x0 + incrementX;
  
  //Print Tic Marks on X Axis 
  for(i=0;i <= xtics-1; i++){
    Write_Line(xTic, y1-2, xTic, y1+2 );
    xTic += incrementX;
  }
  
  //print y-axis  
  Write_Line(x0, y0, x0, y1);
  
  yTic = y1 - incrementY; 
  
  //Print Tic Marks on Y Axis
  for(i=0;i <= ytics-1; i++){
    Write_Line(x0-2, yTic, x0+2, yTic );
    yTic -= incrementY;
  }
  
  return 0;
  
}

/* @name: printHoseStatus
   @param: x - initial x position for first Hose Print line
   @param: y - initial y position for first Hose Print line
   @param: status - current hose status 
   @return: void
*/

void printHoseStatus(int16_t x, int16_t y, uint8_t status){
  if(status & 0x01){
      print_String(x,y, (const uint8_t*)"Hose 1: ON", FONT_5X8);
  } else{
      print_String(x,y, (const uint8_t*)"Hose 1: OFF", FONT_5X8);
  } 
      
  if (status & 0x02){
      print_String(x,y+15, (const uint8_t*)"Hose 2: ON", FONT_5X8);
  } else {
      print_String(x,y+15, (const uint8_t*)"Hose 2: OFF", FONT_5X8);
  } 
      
  if (status & 0x04){
      print_String(x,y+30, (const uint8_t*)"Hose 3: ON", FONT_5X8);
  } else {
      print_String(x,y+30, (const uint8_t*)"Hose 3: OFF", FONT_5X8);
  }
      
}

/* @name: printAxelsLabels
   @param: x0 - initial x position for grid
   @param: x1 - final x position for grid
   @param: y0 - initial y position for grid
   @param: y1 - final y position for grid
   @param: xtics - # of lines on x line
   @param: ytics - # of lines on y line
   @return: TRUE/FALSE depending if grid was successfully printed
*/

int printAxesLabels(int16_t x0, int16_t y0){
  int x_Axis = 80;
  int y_Axis = 0;
  int temp_x = x0+5;  //Initialize Index variables
  int temp_y = y0;
  int i = 0;
    
  //Print X Value Axes Values
  for (i = 0; i < 6; i++ ){
    sprintf(intBuffer,"%d", y_Axis);
    print_String(temp_x,temp_y, (const uint8_t*)intBuffer, FONT_5X8);
    y_Axis += 20;
    temp_y -= 20;
  }

  temp_y = y0;    //Initialize Index variables 
  temp_x += x0+35;
  
  //Print Y Value Axes Values
  for (i = 0; i < 5; i++ ){
    sprintf(intBuffer,"%d", x_Axis);
    print_String(temp_x,temp_y, (const uint8_t*)intBuffer, FONT_5X8);
    x_Axis -= 20;
    temp_x += 20;
  }
  
  return 0;
}

/* @name: plotSampleData 
   @param: TestData - array of structs used for plotting
   @param: dataType - type of sensor data to display
   @param: size - # of elements in array
   
   @return: TRUE/FALSE depending if data was successfully printed
*/

int plotSampleData( D_Struct TestData[], uint8_t dataType, int16_t size){
  
  int gridPlotted = FALSE;
  int i = 0;
  int16_t x_Increment = 0;  
  int16_t mapped_y_Value = 0;
  int16_t x_Value = 0;
  int16_t mapped_x_Value = x_Value + 20; //start X at Left Side of Grid 
  
  //Print Corresponding to selecetd DataType 
  if( dataType == MOISTURE){  
    
    printf("Plotting Moisture \r\n ");
    
    Set_Color(RED);
    print_String(10,0, (const uint8_t*)"Moisture Level", FONT_5X8);
    print_String(10,10, (const uint8_t*)"(Node 2)", FONT_5X8);
    
    Set_Color(BLUE);
    print_String(0,60, (const uint8_t*)"Water%", FONT_5X8);
    print_String(55,120, (const uint8_t*)"Mins Ago", FONT_5X8);
    
    //Set number 
    x_Increment = 100 / MAX_ELEMENTS; //100 indicates the number of pixels
                                      //vertically on the OLED module
    
    for(i = 0; i <= (size-1) ; i++){ //Plot Data Points 
    
      mapped_y_Value = (int16_t)(110-TestData[i].soilMoisture); //110 is bottom of OLED
   
      //printf("Element: %d \r\n", i); //Testing Struct Elements
      
      //printf("Moisture Value: %f \r\n", TestData[i].soilMoisture);
      
      Draw_Pixel(mapped_x_Value, mapped_y_Value);
      mapped_x_Value += x_Increment;
      
    }
    
    gridPlotted = TRUE;
    
  } else if( dataType == SUNLIGHT){
    printf("Plotting Sunlight \r\n ");
    
    Set_Color(RED);
    print_String(10,0, (const uint8_t*)"Light Level", FONT_5X8);
    print_String(10,10, (const uint8_t*)"(Node 2)", FONT_5X8);
    
    
    Set_Color(YELLOW);
    print_String(0,60, (const uint8_t*)"Light%", FONT_5X8);
    print_String(55,120, (const uint8_t*)"Mins Ago", FONT_5X8);
    
    x_Increment = 100 / MAX_ELEMENTS; //Determine number points to increment in x-range
    
    for(i = 0; i <= (size-1) ; i++){ //Plot Data Points 
      
      mapped_y_Value = (int16_t)(110-TestData[i].lightLevel); //110 is bottom of OLED
   
      //printf("Element: %d \r\n", i);  //Testing Struct Elements
      
      //printf("Light Level Value: %f \r\n", TestData[i].lightLevel); 
      
      Draw_Pixel(mapped_x_Value, mapped_y_Value);
      mapped_x_Value += x_Increment;
      
    }
    
    gridPlotted = TRUE;
    
  } else if( dataType == TEMP){
    printf("Plotting Temperature \r\n ");
    
    Set_Color(RED);
    print_String(20,0, (const uint8_t*)"Temperature", FONT_5X8);
    print_String(20,10, (const uint8_t*)"Node ", FONT_5X8);
    
    
    Set_Color(RED);
    print_String(0,60, (const uint8_t*)"Deg(C)", FONT_5X8);
    print_String(55,120, (const uint8_t*)"Mins Ago", FONT_5X8);
    
    x_Increment = 100 / MAX_ELEMENTS; //Determine number points to increment in x-range
    
    for(i = 0; i <= size ; i++){ //Plot data points 
      
      mapped_y_Value = (int16_t)TestData[i].temp_C; //110 is bottom of OLED
   
      //printf("Element: %d \r\n", i); //Testing Struct Elements
      
      //printf("Temp Value: %d \r\n", TestData[i].temp_C);
      
      Draw_Pixel(mapped_x_Value, 110-mapped_y_Value);
      mapped_x_Value += x_Increment;
      
    }
    
    gridPlotted = TRUE;
    
  } else {
    printf(" No Plot Selected \r\n "); //Print error message if invalid dataType 
    
    Set_Color(RED);
    print_String(20,0, (const uint8_t*)"No Plot", FONT_5X8);
    print_String(20,10, (const uint8_t*)"Selected", FONT_5X8);
    
    gridPlotted = FALSE;
  }  

  return gridPlotted;
  
}

/* @name: plotSampleData 
   @param: TestData - array of structs used for plotting
   @param: dataType - type of sensor data to display
   @param: size - # of elements in array
   
   @return: TRUE/FALSE depending if data was successfully printed
*/
void Reset_System(void){
  int i;
  //Reset Hose Variables
  Hose[0] = Hose0; Hose[1] = Hose1; Hose[2] = Hose2;
  Hose[0].waterLevel = 1; Hose[1].waterLevel = 1; Hose[2].waterLevel = 1; 
  Hose[0].control = AUTOMATIC; Hose[1].control = OFF; Hose[2].control = OFF;
  Hose[0].status = WATER_ON; Hose[1].status = WATER_OFF; Hose[2].status = WATER_OFF;
  
  
  //Reset Flow Sensor Variables
  for(i=0; i<3; i++){
    pulseCount_fs[i] = 0;
    moisture_s_thresh[i] = 45.0;
  }
  
  oledColor = WHITE; //Set Oled Color to default
  sleepTime = MIN_3; //Reset Sleep Timer
  selected_Node = 2; //Reset Selected Node for Testing 

  return;
  
}

void Set_Select(uint8_t hose_selected){
  if(hose_selected == HOSE0){ //Set Select to 0x00
    DEV_Digital_Write(SEL_0_Pin, LOW);
    DEV_Digital_Write(SEL_1_Pin, LOW);
  } else if (hose_selected == HOSE1){ //Set Select to 0x01
    DEV_Digital_Write(SEL_0_Pin, HIGH);
    DEV_Digital_Write(SEL_1_Pin, LOW);
  } else {                            //Set Select to 0x10
    DEV_Digital_Write(SEL_0_Pin, LOW);
    DEV_Digital_Write(SEL_1_Pin, HIGH);
  }
  return;
}

