
#include "Adafruit_seesaw.h"
#include <Wire.h>

class SoilSensor {
private:
  Adafruit_seesaw sensor;
  int sensorIndex; //sensor index
public:
  //constructor
  SoilSensor(int index) : sensorIndex {}

  //Method to initialize the sensor with a specific I2C address
  void begin(uint8_t i2cAddress) {
    sensor.begin(i2cAddress);
  }
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
  }
  //Get temp and capacitive reading from sensor
  float tempC = getTemperature();
  uint16_t capRead = getCapacitiveReading(0);

  //Print the obtained data
  Serial.print("Temperature: ");
  Serial.println(tempC);
  Serial.print("Capacitive Reading: ");
  Serial.println(capread1);
  Serial.println();
  }
};

//Number of soil moisture sensors
const int numSensors = 3;

//Array of SoilSensor instances representing each sensor

SoilSensor sensors[numSensors] = {SoilSensor(0), SoilSensor(1), SoilSensor(2)};

void setup() {
  Serial.begin(115200); //Initialize serial communication
  Wire.begin(); //Initialize I2C communication
  
  // Initialize each soil moisture sensor with a unique I2C address
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].begin(0x36 + i);  // Assuming each sensor has a unique I2C address
  }
}

void loop() {
   for (int i = 0; i < numSensors; ++i) {
    Serial.print("Reading from Sensor ");
    Serial.println(i + 1);

    float tempC = sensors[i].getTemp();
    uint16_t capread1 = sensors[i].touchRead(0);
    
    Serial.print("Temperature: ");
    Serial.println(tempC);
    Serial.print("Capacitive Reading: ");
    Serial.println(capread1);
    Serial.println();
   }
  
  // Add a delay here if needed to control how often you read data from the sensors
  delay(1000);

   }

