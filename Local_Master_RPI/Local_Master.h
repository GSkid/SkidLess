#ifndef LOCAL_MASTER

#include <stdint.h>
#include "Local_Master_Defines.h"
#include <sqlite3.h>

/****Helper Fxn Prototypes ****/

// Meta functions

/*@name: Timer
   @param: delayThresh - timer duration
   @param: prevDelay - time in millis() when the timer started
   @return: digital high/low depending if timer elapsed or not
   This is a non-blocking timer that handles uint32_t overflow,
   it works off the internal bcm2835 function millis() as reference
*/
int Timer(const uint32_t&, uint32_t&);

/*@name: Setup
   Sets-up the entire IAI system, including RF and OLED objects
*/
void setup(void);

/*@name: Reset_System
   Resets the entire IAI system, including OLED
*/
void Reset_System(void);

// Oled Functions

/**
   @Function checkButtons(void)
   @param None
   @return None
   @brief This function checksButtons and sets appropriate flag
   @note
   @author Brian Naranjo, 1/25/20
   @editor   */
void checkButtons(void);

/*@name: printHoseStatus
   @param: x - initial x position for first Hose Print line
   @param: y - initial y position for first Hose Print line
   @param: status - current hose status
   @return: void
*/
void printHoseStatus(int16_t x, int16_t y, uint8_t status);

/*@name: printGrid
   @param: x0 - initial x position for grid
   @param: x1 - final x position for grid
   @param: y0 - initial y position for grid
   @param: y1 - final y position for grid
   @param: xtics - # of lines on x line
   @param: ytics - # of lines on y line
   @return: TRUE/FALSE depending if grid was successfully printed
*/
int printGrid(int16_t x0, int16_t x1, int16_t y0, int16_t y1, int16_t xtics, int16_t ytics);

/*@name: printAxelsLabels
   @param: x0 - initial x position for grid
   @param: x1 - final x position for grid
   @param: y0 - initial y position for grid
   @param: y1 - final y position for grid
   @param: xtics - # of lines on x line
   @param: ytics - # of lines on y line
   @return: TRUE/FALSE depending if grid was successfully printed
*/
int printAxesLabels(int16_t x0, int16_t y0);

/*@name: plotSampleData
   @param: TestData - array of structs used for plotting
   @param: dataType - type of sensor data to display
   @param: size - # of elements in array
   @return: TRUE/FALSE depending if data was successfully printed
*/
int plotSampleData(D_Struct data[], uint8_t dataType, int16_t size);

/*@name: convertFloat_String
   @param: in - float to be converted to String
   @param: buffer - char variable used to store float string output
   @return: 0
*/
int convertFloat_String(float in, char buffer[100]);

/*
   @Function OLED_PrintArrow(int state)
   @param int x, int y Used to determine x,y position of arrow
   @return None
   @brief This function prints Arrow on OLED at x,y coordinates
   @note
   @author Brian Naranjo, 1/25/20
   @editor
 */
void OLED_PrintArrow(int x, int y);

/*@name: OLED_SM
   @param: Color of Page Text
   @return: void
*/
void OLED_SM(uint16_t color);

// Water Delivery Functions

/*@name: LPMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void LPMOS_Set(uint8_t status);

/*@name: RPMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void RPMOS_Set(uint8_t status);

/*@name: LNMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void LNMOS_Set(uint8_t status);

/*@name: RNMOS_Set
   @param: status - whether to turn off or on MOSFET
   @return:void
*/
void RNMOS_Set(uint8_t status);

/* @name: recordPulses_FS
 * @param: i, determines which flow Sensor to Record
 * Updates Flow Sensor Pulse Count
   @return: return
*/
void recordPulses_FS(int i);

/*@name: convertPulse_Liters
   @param: pulseCount - var keeping track of fs pulses
   @return: liters - var keeping track of fs liters
   *
   @return: Liters as a float
*/
float convertPulse_Liters(int pulseCount);

/*@name: convertLiters_Gals
   @param: liters - var keeping track of fs liters
   @param: gallons - var keeping track of fs gallons
   *
   @return: Gallons as a float
*/
float convertLiters_Gals(float liters);


void Set_Select(uint8_t hose_selected);

/*@name: WaterDelivery
   @param: HOSE_NUM - an enum that specifies which hose to evaluate
   @return: uint8_t - a bit array of values that indicate which hoses are on/off
   This function determines if a hose needs to be turned on or off based on sensor data.
   The function also handles the control of the water delivery SM to turn on/off the H-bridge
 */
uint8_t WaterDelivery(HOSE_NUM);

/*@name: WaterDeliverSM
   @param: status - whether to turn on or off WD
   @param: delayP_N - delay time between turning ON/OFF PFET and NFET
   @param: pulseTime - Time for +/-5V Pulse, Delays time between ON and OFF
   @return: 1/0 depending on whether drive was completed
*/
int WaterDeliverySM(uint8_t status, uint32_t delayP_N, uint32_t pulseTime);

/*
	@name: insert_into_database
	@desc: This function takes in the tokenized values from .csv file, binds
		each one to a prepare statement, and executes every statement.
	@param: *mDb - pointer to the sqlite3 database
	@param: soil_moisture - tokenized soil moisture value from the .csv file
	@param: light - tokenized ambient light value from the .csv file
	@param: temp - tokenized ambient temperature value from the .csv file
	@param: pressure - tokenized barometric pressure value from the .csv file
	@param: precip_prob - tokenized rain probability value from the .csv file
	@param: output - tokenized watering algorithm value from the .csv file
	@param: nodeID - tokenized nodeID value from the .csv file
	@param: battery_lvl - tokenized node battery level value from the .csv file
	@param: hose1 - tokenized first hose output value from the .csv file
	@param: hose2 - tokenized second hose output value from the .csv file
	@param: hose3 - tokenized third hose output value from the .csv file
*/
void insert_into_database(sqlite3* mDb, double soil_moisture, int light,
	int temp, double pressure, double precip_prob, int output, int nodeID,
	double battery_lvl, int hose1, int hose2, int hose3);

/*
	@name: processCSV
	@desc: This function opens and reads the .csv file, tokenizes the values
		from line 2 inside the csv file, changes the datatypes of the values to
		their proper type, and calls the insert_into_database function, and
		closes the file.
	@param: *db - the sqlite3 database where data is inserted into
*/
void processCSV(sqlite3* db);

/*
	@name: createTable
	@desc: Creates the table in the database in which the data will be stored
	@param: *db - the database where the table is created
*/
int createTable(sqlite3* db);

/*
	@name: callback
	@desc: calls for each row after returning from execute statement
	@param:
	@return:
*/
static int callback(void* NotUsed, int argc, char** argv, char** azColName);



#endif // !LOCAL_MASTER
