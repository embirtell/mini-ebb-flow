
#include "Adafruit_seesaw.h"
#include <Wire.h>


class SoilSensor {
private:
  Adafruit_seesaw sensor;
  int sensorIndex;             //sensor index
  int valvePin;                // Pin connected to teh corresponding valve
  uint16_t lastCapReadings[3];  //Array to store previous 5 capacitive readings
  int readingsIndex;           //Index to keep track of current position in the array
  bool valveOpen;       //Variable to track the state of the valve
  static bool queueRequested; // Flag to indicate if a valve opening is requested when another valve is already open

public:
  //constructor
  SoilSensor(int index, int valvePin) : sensorIndex(index), valvePin(valvePin), readingsIndex(0), valveOpen(false) {}

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
  //Method to check if the capacitive reading is stable
  bool areCapReadingsStable(uint16_t currentReading) {
    //Add stability criteria here - for example - if the change is less than a threshold
    lastCapReadings[readingsIndex] = currentReading;  //store the current reading in the array
    //Increment the index and wrap around if necessary
    readingsIndex = (readingsIndex + 1) % 3;
     //Check stability based on the last 3 readings
    for (int i = 0; i < 2; ++i) {
      if (abs(lastCapReadings[i] - lastCapReadings[(i + 1) % 3]) > 10) {
        return false;  //not stable if any consecutive readings differ by more than 10
      }
    }
    return true;
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

  // Check if the capacitive readings are stable before initiating valve
    bool stable = areCapReadingsStable(capRead);

    //Open of close the corresponding valve based on the capacitive reading and vavlve status
    if (capRead < 400) {
      requestOpenValve(stable); //Request to open valve
    } else {
      requestCloseValve(stable); //Request to close valve
    }

    //Process the valve queue
    processQueue();

    Serial.println();
  }

  //Public method to get the valve pin
  int getValvePin() const {
    return valvePin;
  }

  //Public method to check if the valve is currently open
  bool isValveOpen() const {
    return valveOpen;
  }
  //Public method to process the valve queue
  static void processQueue() {
    if(queueRequested) {
     for(int i = 0; i < numSensors)
  }

private:
  //private methods to request opening and closing of the valve
  void requestOpenValve(bool stable) {
    if (!valveOpen) {
      openValve();
    } else {
      //Queue the request if the valve is already open
      queueRequested = true;
    }
    //print valve status even if readings not stable
    printValveStatus(stable);
  }
  
  void requestCloseValve(bool stable) {
    if(valveOpen) {
      closeValve();
    }
    printValveStatus(stable);
  }
  
  //Private methods to open and close the valve
  void openValve() {
    digitalWrite(valvePin, HIGH);
    Serial.print("Valve ");
    Serial.print(valvePin);
    Serial.println(" is OPEN");
    valveOpen = true;
  }
  void closeValve() {
    digitalWrite(valvePin, LOW);
    Serial.print("Valve ");
    Serial.print(valvePin);
    Serial.println(" is CLOSED");
    valveOpen = false;
    
    //Process the queue after closing the valve
    processQueue();
  }

  // Private method to print valve status
  void printValveStatus(bool stable) {
    Serial.print("Valve ");
    Serial.print(valvePin);
    Serial.print(" Status: ");
    Serial.println(valveOpen ? "OPEN" : "CLOSED");

    if(!stable) {
      Serial.println("Note: Capacitive readings not stable. Waiting for stability...");
    }
  }
};

//Number of soil moisture sensors
const int numSensors = 3;

//Array of SoilSensor instances representing each sensor and corresponding valve
SoilSensor sensors[numSensors] = {
  SoilSensor(0, 2),  // Sensor 1 is connected to valve on pin 2
  SoilSensor(1, 3),  // Sensor 2 is connected to valve on pin 3
  SoilSensor(2, 4),  // Sensor 3 is connected to valve on pin 4
};

void setup() {
  Serial.begin(115200);  //Initialize serial communication
  Wire.begin();          //Initialize I2C communication

  // Initialize each soil moisture sensor with a unique I2C address
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].begin(0x36 + i);  // Assuming each sensor has a unique I2C address
    pinMode(sensors[i].getValvePin(), OUTPUT);
    digitalWrite(sensors[i].getValvePin(), LOW);  // Ensure valves are initially closed
  }
}

void loop() {
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].readData();
    delay(2000);  // should have sensors read one at a time
  }
}
