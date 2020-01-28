#include <Adafruit_BMP085.h>

#include  <Wire.h> //configures I2C (we are using nano so this is the pinout: SDA=A4 and SCL=A5)
#include <Adafruit_BMP085.h> 

#define LIGHT_PIN A0 //These are the only pins to change atm
#define MOISTURE_PIN A1


//FOR THE COFFEE CUP THIS IS THE PINOUT: 
//Red- 5V (Vin)
//Black- GND 
//Yellow- A0 (Analog light level)
//Blue- A1 (Moisture)
//Green- SDA (tie to pin A4 on nano)
//Purple- SCL (tie to pin A5 on nano)



Adafruit_BMP085 bmp;
float celsius;
float farenheit;
float analogMoisture;
float analogLight;
float lightVoltage;
float moistureVoltage;
float soilResistance;
float lightResistance;

  
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

    Serial.print("Soil Resistance = ");
    analogMoisture = analogRead(MOISTURE_PIN);
    moistureVoltage = (analogMoisture/1023)*5;
    soilResistance = ((5 - moistureVoltage) / moistureVoltage) * 10; //10k load
    Serial.print(soilResistance);
    Serial.print(" kOhms, ");
    Serial.print(moistureVoltage);
    Serial.println(" V analog reading");

    Serial.print("Photoresistor Resistance = ");
    analogLight = analogRead(LIGHT_PIN);
    lightVoltage = (analogLight/1023)*5;
    lightResistance = ((5 - lightVoltage) / lightVoltage) * 2.2; //2.2k load
    Serial.print(lightResistance);
    Serial.print(" kOhms, ");
    Serial.print(lightVoltage);
    Serial.println(" V analog reading");

    Serial.println();
    delay(3000);
}
