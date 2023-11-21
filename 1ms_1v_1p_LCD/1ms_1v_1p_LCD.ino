#include "Adafruit_seesaw.h" //for moisture sensor

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

#define RELAY_PUMP    A5  // Arduino pin that controls the pump via relay
#define THRESHOLD 400   // => CHANGE YOUR THRESHOLD HERE

#define RELAY_VALVE1   A1 //Arduino pin that controls the relay for valve 1
boolean valve1_OPEN = false; //flag to indicate if valve 1 is open
//#define RELAY_VALVE2   A2 //Arduino pin that controls the relay for valve 2
//boolean valve2_OPEN = false; //flag to indicate if valve 2 is open
//#define RELAY_VALVE3   A3 //Arduino pin that controls the relay for valve 3
//#define RELAY_VALVE4   A4 //Arduino pin that controls the relay for valve 4

unsigned long startTime = 0; //Variable to store the Pump start time
unsigned long elapsedTime = 0; // Variable to store the elapsed Pump time
unsigned long totalRunTime = 0; //Var to store the total Pump run time
boolean pump_ON = false; // Flag to indicate if the pump is running

//trying to get an average value
const int numReadings = 10; //number of readings to average

int readings[numReadings]; //Array to store readings from input
int readIndex = 0; // the index of the current reading
int total = 0; // Variable to store the running total
int avgCap = 0; // the average

//trying to get the sensor stabilized before starting to make decisions based on value
const int SensorStabilityThresh = 5; // threshold for stabilization
const int stabilizationTime = 1000;

Adafruit_seesaw ss;

void setup() {
  Serial.begin(115200);
  
  // For averaging sensor values: Initialize all readings to 0:
  for(int this_reading = 0; this_reading < numReadings; this_reading++) {
    readings[this_reading] = 0;
  }
  //assigning pinMode
  pinMode(RELAY_PUMP, OUTPUT);
  digitalWrite(RELAY_PUMP, LOW); // Makes it so that pump is off at start up
  
  pinMode(RELAY_VALVE1, OUTPUT);
  digitalWrite(RELAY_VALVE1, LOW); // Makes it so that the valve is normally closed
  

  //pinMode(RELAY_VALVE2, OUTPUT);
  //digitalWrite(RELAY_VALVE2, LOW); // Makes it so that the valve is normally closed
  
  
  //CHECK IF MOISTURE SENSOR IS THERE
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  //startup LCD screen
  lcd.init(); //initialize the lcd
  lcd.backlight();
}

void loop() {


//set up LCD Screen
 


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
        Serial.println("Opening Valve 1");
        digitalWrite(RELAY_VALVE1, HIGH); //open valve1
        valve1_OPEN = true;
        if (valve1_OPEN){
          lcd.clear();
          lcd.print("Valve 1 = OPEN");
        } else {
          lcd.clear();
          lcd.print("Valve 1 = CLOSED");
          }

        delay(5000); //make sure valve is open before starting the pump
        Serial.print("The soil moisture is DRY => activate pump ");
        digitalWrite(RELAY_PUMP, HIGH); //turn on the pump
        if (!pump_ON) {
          startTime = millis(); //Record the start time
        pump_ON = true; //set flag to indicate that pump is running
        }
        
      } else {
        Serial.print("The soil moisture is WET=> deactivate the pump ");
        digitalWrite(RELAY_PUMP, LOW);// Turn the pump off
        delay(5000);//turn off the pump before closing the valve
        
        digitalWrite(RELAY_VALVE1, LOW);// Close Valve
        valve1_OPEN = false;
        if(pump_ON) {   
          unsigned long elapsedTime = millis() - startTime; //Calculate the elapsed time
          Serial.print("Pump ran for "); 
          Serial.print(elapsedTime / 1000); 
          Serial.println(" seconds.");
          totalRunTime += elapsedTime; //Accumulate the total run time
          pump_ON = false; //Reset the flag
          }
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


  if (valve1_OPEN){
    lcd.clear();
    lcd.setCursor(0,0); //move cursor to (0,0)
    lcd.print("Valve 1 = OPEN");
  } else {
    lcd.clear();
    lcd.setCursor(0,0); //move cursor to (0,0)
    lcd.print("Valve 1 = CLOSED");
  }
  

  Serial.print("Total Run Time: "); Serial.print(totalRunTime/1000); Serial.println(" seconds"); 
  Serial.print("Temperature: "); Serial.print(tempC); Serial.println("*C");
  Serial.print("Capacitive: "); Serial.println(capread);  
  Serial.print("Capacitive Avg: "); Serial.println(avgCap);
  delay(2000);
}
