#include  <Wire.h> //configures I2C (we are using nano so this is the pinout: SDA=A4 and SCL=A5)
#include <Adafruit_BMP085.h> 

Adafruit_BMP085 bmp;
float celsius;
float farenheit;

  
void setup() {
  Serial.begin(9600);
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
    
    Serial.println();
    delay(2000);
}
