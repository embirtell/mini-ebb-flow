
#include "Adafruit_seesaw.h"
#include <Wire.h>
#include <Queue.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

bool allValvesClosed = true; // Global flag to indicate whether all valves are closed

class SoilSensor {
private:
  Adafruit_seesaw sensor;
  int sensorIndex;              //sensor index
  int valvePin;                 // Pin connected to teh corresponding valve
  uint16_t lastCapReadings[3];  //Array to store the last 5 cap readings
  int readingsIndex;            //Index to keep track of the current position in the array
  bool valveOpen;               //Flag to track if the valve is open
  

public:
  //constructor
  SoilSensor(int index, int valvePin)
    : sensorIndex(index), valvePin(valvePin), readingsIndex(0), valveOpen(false) {}

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
  // Method to check if the capacitive readings are stable
  bool areCapReadingsStable(uint16_t currentReading) {
    // Store the current reading in the array
    lastCapReadings[readingsIndex] = currentReading;

    // Increment the index and wrap around if necessary
    readingsIndex = (readingsIndex + 1) % 3;

    //Calculate the average of the last 5 readings
    uint16_t average = 0;
    for (int i = 0; i < 3; ++i){
      average += lastCapReadings[i];
    }
    average /= 3;

    //Check stability based on the difference between the current reading
    return abs(currentReading - average) < 10;
  }

  //Method to read and print data from the sensor
  void readData(int moistureThreshold, int nextValveInQueue, bool &isOpeningValve) {
    
    //Get temp and capacitive reading from sensor
    float tempC = getTemperature();
    uint16_t capRead = getCapacitiveReading(0);

    //Check if the capcitive readings are stable before initiating valves
    if(areCapReadingsStable(capRead)){
      if (capRead < moistureThreshold) {
        if (!valveOpen && !isOpeningValve && allValvesClosed) {
          openValve(isOpeningValve);
        }
      } else {
        if (valveOpen) {
          closeValve();
        } 
      }
    }
    
    //Display data on LCD Screen
    displayData(capRead, nextValveInQueue);

    //Print Data to Serial for debugging
    Serial.println();
    Serial.print("Sensor ");
    Serial.print(sensorIndex + 1);
    Serial.print(" - Moisture Level: ");
    Serial.print(capRead);
    Serial.print(" | Valve ");
    Serial.print(sensorIndex + 1);
    Serial.print(" Status: ");
    Serial.println(valveOpen ? "OPEN" : "CLOSED");
    Serial.println();
    Serial.print(nextValveInQueue);
  }

  //Public method to get the valve pin
  int getValvePin() const {
    return valvePin;
  }

  //Public method to check if the valve is open
  bool isValveOpen() const {
    return valveOpen;
  }
  //Method to open valve
  void openValve(bool &isOpeningValve) {
    digitalWrite(valvePin, HIGH);
    delay(5000);//give the valve time to open
    isOpeningValve = true;
    valveOpen = true;
    allValvesClosed = false;
  }
  //Method to close valve
  void closeValve() {
    digitalWrite(valvePin, LOW);
    delay(5000);//give the valve time to close
    valveOpen = false;

    //Set the flag to indicate that the valve is closed
    allValvesClosed = true;
  }


private:
  void displayData(uint16_t capRead, int nextValveInQueue) {
    //set the cursor at the beginning of the LCD
    lcd.setCursor(0, 0);
    // Display moisture level and valve status on the LCD
    lcd.print("MS");
    lcd.print(sensorIndex + 1);
    lcd.print(":");
    lcd.print(capRead); //reading 100x as high as serial

    lcd.print(" | V");
    lcd.print(valvePin - 1);
    lcd.print(":");
    lcd.print(valveOpen ? "O" : "C");

    lcd.setCursor(0,1);
    lcd.print("Next Valve: ");
    lcd.print(nextValveInQueue);
  }

  //method to display a warning if cap readings unstable
  void displayStabilityWarning() {
    Serial.println("Capacitive readings not stable. Waiting for stability...");
  }
};
//Number of soil moisture sensors
const int numSensors = 4;

//Array of SoilSensor instances representing each sensor and corresponding valve
SoilSensor sensors[numSensors] = {
  SoilSensor(0, 2),  // Sensor 1 is connected to valve on pin 2
  SoilSensor(1, 3),  // Sensor 2 is connected to valve on pin 3
  SoilSensor(2, 4),   // Sensor 3 is connected to valve on pin 4
  SoilSensor(3, 5) // Sensor 4 is connected to valve on pin 5
};

Queue<int, numSensors> valveQueue;  //Queue to store valve pins

//Moisture threshold variable
int moistureThreshold = 400; //Adjust as needed

void setup() {
  Serial.begin(115200);  //Initialize serial communication
  Wire.begin();          //Initialize I2C communication

  //initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  
  // Initialize each soil moisture sensor with a unique I2C address
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].begin(0x36 + i);  // Assuming each sensor has a unique I2C address
    pinMode(sensors[i].getValvePin(), OUTPUT);
    digitalWrite(sensors[i].getValvePin(), LOW);  // Ensure valves are initially closed
  }
}

bool isOpeningValve = false; //initialize a flag


//Function to print the contents of a queue
void printQueue(Queue<int, numSensors> &q){
   Queue<int, numSensors> tempQueue(q); // Create a temporary queue to avoid modifying the original queue
  while (!tempQueue.isEmpty()) {
    Serial.println(tempQueue.front());
    tempQueue.dequeue();
    if (!tempQueue.isEmpty()) {
      Serial.print(", ");
    }
  }
  Serial.println();
}

//Function to open the next valve in the queue
void openNextValve(int valvePin, SoilSensor &sensor, bool &isOpeningValve) {
  // Close any previously opened valve
  if (valveQueue.size() > 0) {
    int openValvePin = valveQueue.front();
    valveQueue.dequeue();
    closeOpenValve(openValvePin, sensor);
  }
  
  //Open the next valve
  sensor.openValve(isOpeningValve);

  Serial.print("Opening next valve in queue: ");
  Serial.println(valvePin - 1);
  
  unsigned long startTime = millis();

  //wait for the associated sensor to detect wet conditions
  while (sensor.getCapacitiveReading(0) > moistureThreshold) {
    delay(500);  // Adjust the delay based on your application's requirements
    if(millis() - startTime > 10000) { // Set a timeout (e.g., 10 seconds) to prevent blocking indefinitely
      Serial.println("Timeout: Valve didn't close within Window. Closing Valve.");
      break;
    }
  }
  
  sensor.closeValve();  // Close the valve after a delay
 
  Serial.print("Valve ");
  Serial.print(valvePin - 1);
  Serial.println(" is CLOSED"); 
}

void closeOpenValve(int openValvePin, SoilSensor &sensor) {
  // Check if there is any valve currently open and wait until the moisture level is above the threshold
  while (sensor.getCapacitiveReading(0) < moistureThreshold) {
    delay(500);
  }
   // Close the previously opened valve
  sensor.closeValve();
  isOpeningValve = false;
  Serial.print("Closing previously opened valve: ");
  Serial.println(openValvePin - 1);
}


void loop() {
  
  
  // Loop through each soil moisture sensor and read data one at a time
  //Check if all valves are closed


    for (int i = 0; i < numSensors; ++i) {
      int nextValveInQueue = -1; //default if queue is empty
      if(valveQueue.size() > 0) {
        nextValveInQueue = valveQueue.front();
    }

    sensors[i].readData(moistureThreshold, nextValveInQueue, isOpeningValve);
    
    if(allValvesClosed) {
      bool isOpeningValve = false;   
      
      sensors[i].readData(moistureThreshold, nextValveInQueue, isOpeningValve);
      
      if (!sensors[i].isValveOpen() && valveQueue.size() > 0 && allValvesClosed && !isOpeningValve) {
        int nextValvePin = valveQueue.front();
        valveQueue.dequeue();
        sensors[i].openValve(isOpeningValve);
      //openNextValve(nextValvePin, sensors[i], isOpeningValve); // Pass the isOpeningValve flag    
        }
    } 
    //Check if all valves are closed
    //if(!sensors[i].isValveOpen()) {
      //allValvesClosed = true;
      
      delay(1000);
    }  
}
    // Print the valves in the queue after opening a valve
     // Serial.print("Valves in Queue: ");
      //printQueue(valveQueue);
    //}

    
