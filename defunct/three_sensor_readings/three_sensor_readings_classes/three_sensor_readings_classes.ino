
#include "Adafruit_seesaw.h"
#include <Wire.h>

class SoilSensor {
private:
  Adafruit_seesaw sensor;
  int sensorIndex; //sensor index
  int valvePin; // Pin connected to teh corresponding valve
public:
  //constructor
  SoilSensor(int index, int valvePin) : sensorIndex(index), valvePin(valvePin) {}

  //Method to initialize the sensor with a specific I2C address
  void begin(uint8_t i2cAddress) {
    sensor.begin(i2cAddress);
  }
  //Method to get temp from sensor
  float getTemperature() {
    return sensor.getTemp();
  }
  //Method to get the capacitive reading from a sensor
  uint16_t getCapacitiveReading(int pin) {
    return sensor.touchRead(pin);
  }
  //Method to read and print data from the sensor
  void readData() {
    Serial.print("Reading from Sensor ");
    Serial.println(sensorIndex + 1);
  
    //Get temp and capacitive reading from sensor
    float tempC = getTemperature();
    uint16_t capRead = getCapacitiveReading(0);

    //Print the obtained data
    Serial.print("Temperature: ");
    Serial.println(tempC);
    Serial.print("Capacitive Reading: ");
    Serial.println(capRead);
    
    if(capRead < 400) {
      openValve();
    } else {
      closeValve();
    }

    Serial.println();
  }
  //Public method to get the valve pin
  int getValvePin() const {
    return valvePin;
  }
private: 
  //Private methods to open and close the valve
  void openValve() {
    digitalWrite(valvePin, HIGH);
    Serial.print("Valve ");
    Serial.print(valvePin);
    Serial.println(" is OPEN");
  }
  void closeValve() {
    digitalWrite(valvePin, LOW);
    Serial.print("Valve ");
    Serial.print(valvePin);
    Serial.println(" is CLOSED");
  }
};

//Number of soil moisture sensors
const int numSensors = 3;

//Array of SoilSensor instances representing each sensor and corresponding valve
SoilSensor sensors[numSensors] = {
  SoilSensor(0, 2), // Sensor 1 is connected to valve on pin 2
  SoilSensor(1, 3), // Sensor 2 is connected to valve on pin 3
  SoilSensor(2, 4), // Sensor 3 is connected to valve on pin 4
};



void setup() {
  Serial.begin(115200); //Initialize serial communication
  Wire.begin(); //Initialize I2C communication
  
  // Initialize each soil moisture sensor with a unique I2C address
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].begin(0x36 + i);  // Assuming each sensor has a unique I2C address
    pinMode(sensors[i].getValvePin(), OUTPUT);
    digitalWrite(sensors[i].getValvePin(), LOW); // Ensure valves are initially closed
  }
}

void loop() {
   for (int i = 0; i < numSensors; ++i) {
    sensors[i].readData();
   }
  // Add a delay here if needed to control how often you read data from the sensors
  delay(1000);

   }

