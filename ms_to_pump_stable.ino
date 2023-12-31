#include "Adafruit_seesaw.h"

#define RELAY_PUMP    A5  // Arduino pin that controls the pump via relay
#define THRESHOLD 500   // => CHANGE YOUR THRESHOLD HERE

unsigned long startTime = 0; //Variable to store the start time
unsigned long elapsedTime = 0; // Variable to store the elapsed time
unsigned long totalRunTime = 0; //Var to store the total run time
boolean pump_ON = false; // Flag to indicate if the pump is running

//trying to get an average value
const int numReadings = 10; //number of readings to average

int readings[numReadings]; //Array to store readings from input
int readIndex = 0; // the index of the current reading
int total = 0; // Variable to store the running total
int avgCap = 0; // the average

const int SensorStabilityThresh = 5; // threshold for stabilization
const int stabilizationTime = 1000;

Adafruit_seesaw ss;

void setup() {
  Serial.begin(115200);
  
  // Initialize all readings to 0:
  for(int this_reading = 0; this_reading < numReadings; this_reading++) {
    readings[this_reading] = 0;
  }

  pinMode(RELAY_PUMP, OUTPUT);
  digitalWrite(RELAY_PUMP, LOW); // Makes it so that the valve is normally closed
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
}

void loop() {

  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);

//subtract the last reading
  total = total - readings[readIndex];
//read from the sensor
  readings[readIndex] = ss.touchRead(0);
//add the reading to the total:
  total = total + readings[readIndex];
//advance to the next position in the array:
  readIndex = readIndex + 1;

//if we are at the end of the array
  if (readIndex >= numReadings){
  //wrap around to beginning:
    readIndex = 0;
  }

//calculate the average:
  avgCap = total / numReadings;
 
  static int previousAvgCap = 0; //Variable to store previous average
  static unsigned long stabilizationStartTime = 0; //Var to store time when stabilization starts

  //check if the absolute difference between current and previous readings is withing threshold
  if(abs(avgCap - previousAvgCap) <= SensorStabilityThresh) {
    // If the value has stabilized, check if it has been stabilized for the defined time
    if (millis() - stabilizationStartTime >= stabilizationTime) {
      // Sensor value has stabilized
      Serial.println("Sensor value has stabilized: " + String(avgCap));

      if (avgCap != 0 && avgCap < THRESHOLD) {
      delay(5000);
      if(avgCap !=0 && avgCap < THRESHOLD){
       Serial.print("The soil moisture is DRY => activate pump ");
        digitalWrite(RELAY_PUMP, HIGH); //turn on the pump
        if (!pump_ON) {
          startTime = millis(); //Record the start time
        pump_ON = true; //set flag to indicate that pump is running
        }
      }
      } else {
        Serial.print("The soil moisture is WET=> deactivate the pump ");
        digitalWrite(RELAY_PUMP, LOW);// Turn the pump off
        if(pump_ON) {   
          unsigned long elapsedTime = millis() - startTime; //Calculate the elapsed time
          Serial.print("Pump ran for "); 
          Serial.print(elapsedTime / 1000); 
          Serial.println(" seconds.");
          totalRunTime += elapsedTime; //Accumulate the total run time
          pump_ON = false; //Reset the flag
          }
        elapsedTime = 0;
        } 
        //Reset the stabilization start time for the next stabilization check
       stabilizationStartTime = millis();  
    }
  } else {
    //If the current reading is not within the threshold, reset the stabilization start time
    stabilizationStartTime = millis();    
  }
  //Update the previousAvgCap for the nex iteration
  previousAvgCap = avgCap;

  Serial.print("Total Run Time: "); Serial.print(totalRunTime/1000); Serial.println(" seconds"); 
  Serial.print("Temperature: "); Serial.print(tempC); Serial.println("*C");
  Serial.print("Capacitive: "); Serial.println(capread);  
  Serial.print("Capacitive Avg: "); Serial.println(avgCap);
  delay(3000);
}

