
#include "Adafruit_seesaw.h"
#include <Wire.h>

//class representing valves
class Valve {
private:
  int relayPin; //Arduino GPIO pin connected to the relay

public:
  Valve(int pin) : relayPin(pin) {
    pinMode(relayPin, OUTPUT); // Set the relay pin as an output
    close(); // Ensure valves are closed initially
    Serial.print("Valve initialized on pin ");
    Serial.println(relayPin);
  }

  void open() {
    digitalWrite(relayPin, HIGH); // Activate the relay to open the valve
    Serial.print("Valve on pin ");
    Serial.print(relayPin);
    Serial.println(" is OPEN");
  }

  void close(){
    digitalWrite(relayPin, LOW); // Activate the relay to close the valve
    Serial.print("Valve on pin ");
    Serial.print(relayPin);
    Serial.println(" is CLOSED");
  }
};


//class representing soil sensors
class SoilSensor {
private:
  Adafruit_seesaw sensor;
  Valve& associatedValve; //Each sensor is associated with a valve
  uint16_t moistureThreshold; //Moisture level threshold for the sensor
  int sensorIndex; //sensor index/identifier of sensor

public:
  //constructor
  SoilSensor(Valve& valve, uint16_t threshold, int index) : associatedValve(valve), moistureThreshold(threshold), sensorIndex(index){}

  //Method to initialize the sensor with a specific I2C address
  void begin(uint8_t i2cAddress) {
    sensor.begin(i2cAddress);
    Serial.print("Sensor ");
    Serial.print(sensorIndex + 1);
    Serial.println(" initialized");
  }
  //Method to get temp from sensor
  float getTemperature() {
    return sensor.getTemp();
  }
  //Method to get the capacitive reading from a sensor
  uint16_t getCapacitiveReading(int pin) {
    return sensor.touchRead(pin);
  }
  //Method to check soil moisture level and control valve
  void checkMoistureLevel(){
    uint16_t capRead = getCapacitiveReading(0); // Read capacitive value from the sensor

    Serial.print("Reading from Sensor ");
    Serial.print(sensorIndex + 1);
    Serial.print("Capacitive Reading: ");
    Serial.println(capRead);

  //Open the valve if the capacitive reading is below threshold
  if(capRead < moistureThreshold) {
    Serial.println("Moisture level is low. Opening the valve.");
    associatedValve.open();
  } else {
    Serial.println("Moisture level is sufficient. Closing the valve.");
    associatedValve.close();
  }
}
  //Method to read and print data from the sensor
  void readData() {
     //Get temp and capacitive reading from sensor
    float tempC = getTemperature();
    uint16_t capRead = getCapacitiveReading(0);
    //Print the obtained data    
    Serial.print("Reading from Sensor ");
    Serial.println(sensorIndex + 1);
  
    Serial.print("Temperature: ");
    Serial.println(tempC);
  
    Serial.print("Capacitive Reading: ");
    Serial.println(capRead);
    Serial.println();
  }
};

const int numSensors = 3; //number of soil moisture sensors
Valve valves[numSensors] = {Valve(2), Valve(3), Valve(4)}; //declare valves
//Array of SoilSensor instances representing each sensor
SoilSensor sensors[numSensors] = {SoilSensor(valves[0], 300, 0), SoilSensor(valves[1], 300, 1), SoilSensor(valves[2], 300, 2)};


void setup() {
  Serial.begin(115200); //Initialize serial communication
  Wire.begin(); //Initialize I2C communication
  
  // Initialize each soil moisture sensor with a unique I2C address
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].begin(0x36 + i);  // Assuming each sensor has a unique I2C address
  }
}

void loop() {
   //Read and report sensor data
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].readData(); // Read and print additional data from the sensor
   }
  // Add a delay here if needed to control how often you read data from the sensors
  delay(1000);
  
  for(int i = 0; i < numSensors; ++i){
    sensors[i].checkMoistureLevel(); // Check soil moisture level and control the valve accordingly
  }
  delay(1000);
}

