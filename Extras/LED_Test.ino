// ********** INCLUDES **********
#include <SPI.h>
#include "RF24.h"
#include <printf.h>

// ********** GLOBALS **********
/* Changes the radio number in the network
      -> used to change the reading/writing pipe addresses */

bool radioNumber = 0;

/* RF Radio setup on the SPI pins
      SPK - D13
      MOSI - D11
      MISO - D12
      ->RF24 radio(CE, CSN) */

RF24 radio(7, 8);

/* Address array stores the reading/writing pipes
      -> used for setup only */

byte addresses[][6] = {"1Node", "2Node"};

/* Radio_Mode to switch between sending and receiving */

#define Rx 0
#define Tx 1
bool Radio_Mode = Rx;

/* IO Pins */

int PushButton = A0;
int LED = 2;


void setup() {
  Serial.begin(115200);
  printf_begin();
  Serial.println(F("RF24 Point-to-Point LED Test"));
  Serial.println(F("*** PRESS 'R' to begin receiving from the other node"));

  radio.begin();

  // Used to set the RF24 power level
  //    -> choose between MAX, HIGH, LOW, MIN
  radio.setPALevel(RF24_PA_MIN);
  
  // Switch the reading/writing pipes based on the radio number
  Serial.print(radioNumber);
  Serial.println("\r\n");
  if (radioNumber) {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
    radio.stopListening();
  } else {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
    radio.startListening();
  }

  radio.startListening();
//  radio.printDetails();

  // Test pin modes
  pinMode(PushButton, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  /****************** Tx Role ***************************/
  if ( Radio_Mode == Tx )  {

    //Reads the button pressed located at A0
    uint16_t sensor_in = analogRead(PushButton);
    sensor_in = map(sensor_in, 0, 1023, 0, 33);
    Serial.print(sensor_in);
    Serial.print("\r\n");
    uint16_t mr_frog;

    if (sensor_in >= 10) {
      //if the button is pressed, mr_frog goes high
      mr_frog = 1;
    } else {
      //otherwise mr_frog goes low
      mr_frog = 0;
    }

    // Now sends the data of mr_frog to Rx
    Serial.println(F("Now sending"));
    if (!radio.write( &mr_frog, sizeof(uint16_t) )) {
      Serial.println(F("radio.write failed"));
    }

    // Try again 1s later
    delay(250);
  }
  /****************** Rx Role ***************************/
  if ( Radio_Mode == Rx )
  {
    // Stores the variable incoming from Tx
    uint16_t mrs_frog;

    if ( radio.available()) {
      while (radio.available()) {                                   // While there is data ready
        radio.read( &mrs_frog, sizeof(uint16_t) );                       // Get the payload
      }

      // Takes the mrs_frog var and turns on or off the LED
      Serial.print(mrs_frog);
      Serial.println("\r\n");
      if (mrs_frog) {
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(LED, LOW);
      }
    }
    
    // Try again later
    delay(250);
  }
  /****************** Change Roles via Serial Commands ***************************/
  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && Radio_Mode == Rx ) {
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      Radio_Mode = 1;                  // Become the primary transmitter (ping out)
      radio.stopListening();

    } else if ( c == 'R' && Radio_Mode == Tx ) {
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      Radio_Mode = 0;                // Become the primary receiver (pong back)
      radio.startListening();

    }
  }
} // Loop
