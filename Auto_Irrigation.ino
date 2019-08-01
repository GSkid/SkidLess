typedef enum {
  Wet,
  Debounce,
  Dry
}planter_type;

const int pinIN = A0;   //Input pin read from planter
const int pinOUT = A1;  //Output pin for water valve node
const int OnVoltage = 2.5; //Max voltage for when plants are dry
const int OffVoltage = 0.5; //Min Voltage for when plants are wet
int ledPIN = 13;        //Can be replaced for water valve node
int nodeVoltage = 0;    //Mapped node voltage value
planter_type planter;        //Switch Case variable

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPIN, OUTPUT);
  planter = Wet;
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100000);
  nodeVoltage = analogRead(pinIN); //Reads the voltage at A0
  nodeVoltage = map(nodeVoltage, 0, 1023, 0, 5);
  switch(planter) {
    case Wet: //Plater is wet, water is off
      if (nodeVoltage >= OnVoltage) {
        planter = Debounce; //sets next state to debounce
        break;
      }
      else {
        break;  //Otherwise stays in the Wet (Off) state
      }
    case Debounce: //after one one hit of dry
      if (nodeVoltage >= OnVoltage) {
        planter = Dry;  //Moves to Dry (On) state
        break;
      }
      else {
        planter = Wet; //Returns to Wet (Off) state
        break;
      }
    case Dry: //planter is dry, water is on
      if (nodeVoltage <= OffVoltage) {
        analogWrite(pinOUT, 5);  //Turns on the irrigation
        //This analog write can be switched for a digital pin
        digitalWrite(ledPIN, HIGH); //Turns on indicator LED
        break;
      }
      else {
        planter = Wet;  //Returns to Wet (Off) state
        break;
      }
  }
}
