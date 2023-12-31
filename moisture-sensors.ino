////From https://arduinogetstarted.com/tutorials/arduino-soil-moisture-sensor-pump
////Alternative: https://arduinogetstarted.com/tutorials/arduino-automatic-irrigation-system
////Moisture sensor + Pump

#define RELAY_PIN       2   // Arduino pin that controls the pump via relay
#define MOISTURE_PIN    A0  // Arduino pin that connects to the AOUT pin of the moisture sensor

#define THRESHOLD       500 // CHANGE THRESHOLD HERE


void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);

}

void loop() {
  int value = analogRead(MOISTURE_PIN); // read analog value from sensor
  
  if(value <THRESHOLD) {
    Serial.print ("The soil moisture is DRY => activate pump");
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    Serial.print("The soil moisture is WET => deactivate the pump");
    digitalWrite(RELAY_PIN, LOW);
  }

Serial.print (" (");
Serial.print(value);
Serial.println(")");

delay(1000);

}



//Just code for reading moisture sensor
//#define AOUT_PIN A0 // Arduino pin that connects to AOUT pin of moisture sensor

//void setup() {
//  Serial.begin(9600);
//}

//void loop() {
  //int value = analogRead(AOUT_PIN); // read the analog value from sensor

  //Serial.print("Moisture: ");
  //Serial.println(value);

  //delay(500);
//}

//#define AOUT_PIN A0 // Arduino pin that connects to AOUT pin of moisture sensor
//#define THRESHOLD 100 // CHANGE YOUR THRESHOLD HERE

//void setup() {
//  Serial.begin(9600);
//}

//void loop() {
  //int value = analogRead(AOUT_PIN); // read the analog value from sensor

  //if (value < THRESHOLD)
    //Serial.print("The soil is DRY (");
  //else
    //Serial.print("The soil is WET (");

  //Serial.print(value);
 // Serial.println(")");


  //delay(500);
//}

