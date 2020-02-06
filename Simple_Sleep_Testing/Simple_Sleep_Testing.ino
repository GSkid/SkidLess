#include "Arduino.h"
#include <avr/sleep.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  sleep_cpu();


}
