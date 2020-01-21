#include <Adafruit_BMP085.h>

#include  <Wire.h> //configures I2C (we are using nano so this is the pinout: SDA=A4 and SCL=A5)
#include <Adafruit_BMP085.h> 

#define LIGHT_PIN A0
#define MOISTURE_PIN A1


Adafruit_BMP085 bmp;
float celsius;
float farenheit;
float analogMoisture;
float analogLight;
float lightPercent;
float moisturePercent;
  
void setup() {
  pinMode(LIGHT_PIN, INPUT);
  pinMode(MOISTURE_PIN, INPUT);
  
  Serial.begin(9600);
  while (! Serial); // Wait until Serial is ready 
  Serial.println("  Hello There \n");
  if (!bmp.begin()) {
    Serial.println("BMP 180 not found, check wiring -- Vin->5V, SCL->A5, SDA->A4");
    while (1);
  }
}


void loop() {
    celsius = bmp.readTemperature();
    farenheit = (celsius*9/5) +32;
    Serial.print("Temperature = ");
    Serial.print(celsius);
    Serial.print(" *C, ");
    Serial.print(farenheit);
    Serial.println(" *F");

    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp.readSealevelPressure());
    Serial.println(" Pa");

    //Looks like real altitude will take in actual sea level pressure in Pa 
    //for more accurate readings. Here it is equal to 101500 Pascals. 
    //Not sure if you can just pass in the bmp.readSealevelPressure() parameter
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101500));
    Serial.println(" meters");

    Serial.print("Relative Moisture Level = ");
    analogMoisture = analogRead(MOISTURE_PIN);
    moisturePercent =(analogMoisture/1023) *100; // create formula to scale to percentage
    Serial.print(moisturePercent);
    Serial.println("%");

    Serial.print("Relative Light Level = ");
    analogLight = analogRead(LIGHT_PIN);
    lightPercent = (analogLight/1023)*100;
    Serial.print(lightPercent);
    Serial.println("%");

    Serial.println();
    delay(2000);
}
