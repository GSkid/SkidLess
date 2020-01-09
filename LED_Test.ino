// ********** INCLUDES **********
#include <SPI.h>
#include "RF24.h"
#include <printf.h>

// ********** GLOBALS **********
/* Changes the radio number in the network
      -> used to change the reading/writing pipe addresses */

bool radioNumber = 1;

/* RF Radio setup on the SPI pins
      SPK - D13
      MOSI - D11
      MISO - D12
      ->RF24 radio(CE, CSN) */

RF24 radio(7, 8);

/* Address array stores the reading/writing pipes
      -> used for setup only */

byte addresses[][6] = {"1Nano", "2Nano"};

/* Radio_Mode to switch between sending and receiving */

#define Rx 0
#define Tx 1
int Radio_Mode = Tx;

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
  if (radioNumber) {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  } else {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }

  radio.startListening();
  radio.printDetails();

  // Test pin modes
  pinMode(PushButton, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  if ( Radio_Mode == Tx )  {

    radio.stopListening();                                    // First, stop listening so we can talk.

    uint16_t sensor_in = analogRead(PushButton);
    map(sensor_in, 0, 1023, 0, 33);
    if (sensor_in >= 10) {

      Serial.println(F("Now sending"));
      int mr_frog = 1;
      if (!radio.write( &mr_frog, sizeof(int) )) {
        Serial.println(F("radio.write failed"));
      }
      
      // Try again 1s later
      delay(1000);
    }
  }
  /****************** Pong Back Role ***************************/
  if ( Radio_Mode == Rx )
  {
    int mrs_frog;

    if ( radio.available()) {
      // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &mrs_frog, sizeof(int) );                       // Get the payload
      }
    }

    if (mrs_frog) {
      digitalWrite(PushButton, HIGH);
    } else {
      digitalWrite(PushButton, LOW);
    }
  }
  /****************** Change Roles via Serial Commands ***************************/
  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && Radio_Mode == Rx ) {
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      Radio_Mode = 1;                  // Become the primary transmitter (ping out)

    } else if ( c == 'R' && Radio_Mode == Tx ) {
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      Radio_Mode = 0;                // Become the primary receiver (pong back)
      radio.startListening();

    }
  }
} // Loop
