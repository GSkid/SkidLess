/* THIS FILE IS USED TO STORE ALL RELEVANT #DEFINES FOR LOCAL_MASTER.CPP */
#ifndef LOCAL_MASTER_DEFINES
#define LOCAL_MASTER_DEFINES

#include <stdint.h>


/************************************* #DEFINES *************************************/

// Function
#define DEBUG_LOG(msg) DEBUG_ON ? fprintf(stdout,"%s.\n", msg) : return;

// Globals
#define LED RPI_BPLUS_GPIO_J8_07
#define pushButton RPI_BPLUS_GPIO_J8_29
#define SPI_SPEED_2MHZ 2000000
#define TRUE 1
#define FALSE 0
#define MAX_ELEMENTS 100
#define MAX_SENSORS 20
#define NUM_HOSES 3
#define MOISTURE 0
#define SUNLIGHT 1
#define TEMP 2

// Water Delivery
#define WATER_OFF 0
#define WATER_ON 1
#define PMOS_ON 0	//States for MOS gates
#define PMOS_OFF 1	//1 -> 5V, 0 -> 0V
#define NMOS_ON 1
#define NMOS_OFF 0
#define DEMUX_OFF 1	//Set to Low Enable
#define DEMUX_ON 0
#define LPMOS_Pin 6
#define LNMOS_Pin 13
#define RPMOS_Pin 19
#define RNMOS_Pin 26

// Buttons
#define ENTER_Pin 12
#define BACK_Pin 5
#define DOWN_Pin 20
#define UP_Pin 21

//Flow Sensor Pins 
#define FLOW_SENSOR_1_Pin 4
#define FLOW_SENSOR_2_Pin 3
#define FLOW_SENSOR_3_Pin 2

//Select Pins
#define SEL_1_Pin 17
#define SEL_0_Pin 27

//Flow Sensor Conversions
#define FS_CAL_A 46.2	//Variables used for Characterized FS Regression
#define FS_CAL_B 40.8
#define FS_CAL_STEADY 404	//Calibration factor used when FS signal is steady
#define LITERS_TO_GAL 0.264172

// Time
// These times are given in terms of milliseconds since the timer function
// uses the bcm2835 built-in millis() timer as reference
#define FORECAST_CALL 1800000
#define HOURS_36 129600000
#define MIN_10 600000
#define MIN_5 300000
#define MIN_3 180000
#define MIN_2 120000
#define MIN_1 60000
#define FIVE_SECONDS 5000
#define EIGHT_SECONDS 8000
#define ONE_SECOND 1000
#define PULSE_DURATION 3000
#define FET_DELAY 10
#define MUX_DELAY 50
#define HUNDRED_MILLI 100

// CSV Files
#define CSVFILENAME "Data_Log_to_db.csv"

// MISC
#define DEBUG_ON 1
#define DATA_PARAM_SIZE 10
#define DATA_PARAM_NUM 11

/*Avialable Colors
* Change these if you want to change colors on the OLED display
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
*/




/************************************* OBJECT TYPEDEFS *************************************/

// C_Struct stores relevant thresholds
typedef struct
{
    float sM_thresh;
    float sM_thresh_00;
    float lL_thresh;
    uint16_t tC_thresh;
    uint16_t time_thresh;
}
C_Struct;

// D_Struct stores the relevant sensor data
typedef struct
{
    float soilMoisture;
    float lightLevel;
    uint16_t temp_C;
    uint8_t digitalOut;
    uint8_t nodeID;
    uint8_t battLevel;
}
D_Struct;

typedef struct
{
    float precipProb;
    int temperature;
    int humidity;
    int pressure;
    int windSpeed;
    int windBearing;
}
Forecast;

typedef struct
{
    uint8_t status;
    uint8_t sensors[MAX_SENSORS];
    uint8_t waterLevel;
    uint8_t tally;
    uint8_t flowRate;
    uint8_t rainFlag;
    uint32_t rainTimer;
    uint8_t control;
}
Hoses;




/************************************* STATE MACHINE ENUMS *************************************/

//States for Water Delivery SM
typedef enum
{
    HOSE_IDLE,
    HOSE_ON_S1,
    HOSE_ON_S2,
    HOSE_ON_S3,
    HOSE_OFF_S1,
    HOSE_OFF_S2,
    HOSE_OFF_S3,
}
w_State;

//States for OLED SM
typedef enum
{
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
}
OLED_State;

// Enum for hose specification
typedef enum
{
    HOSE0,
    HOSE1,
    HOSE2,
}
HOSE_NUM;

// Enum for control types
enum
{
    OFF,
    ON,
    AUTOMATIC,
};


#endif // !LOCAL_MASTER_DEFINES