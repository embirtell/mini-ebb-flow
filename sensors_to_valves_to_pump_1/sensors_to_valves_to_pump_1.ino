
#include "Adafruit_seesaw.h"
#include <Wire.h>
#include <Queue.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h"

//----------------Fill in your Wi-Fi / ThingSpeak Credentials-------
const String ssid = SECRET_SSID;                                 //The name of the Wi-Fi network you are connecting to
const String pass = SECRET_PASS;                             //Your WiFi network password

const long myChannelNumber = SECRET_CH_ID;                            //Your Thingspeak channel number
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;                 //Your ThingSpeak Write API Key
//------------------------------------------------------------------
//ThingSpeak Status
String myStatus = "";
String valveStatus = "";
WiFiClient client;

//timing for thingspeak updates
unsigned long elapsedTime = 0;
unsigned long lastUpdateTime = 0;

const int numSensors = 4;
Queue<int, numSensors> valveQueue;  //Queue to store valve pins
//Moisture threshold variable
int moistureThreshold = 400; //Adjust as needed


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
    bool isStable = abs(currentReading - average) < 5;
    //stability message for debugging
    if (!isStable) {
      displayStabilityWarning();
    }
    return isStable;
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
    displayData(capRead);

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
    //allValvesClosed = false;
  }
  //Method to close valve
  void closeValve() {
    digitalWrite(valvePin, LOW);
    delay(5000);//give the valve time to close
    valveOpen = false;

    //Set the flag to indicate that the valve is closed
    //allValvesClosed = true;
  }

  //Function to open the next valve in the queue
void openNextValve(int valvePin, SoilSensor &sensor, bool &isOpeningValve, Queue<int, numSensors> &valveQueue) {
  // Close any previously opened valve
  if (valveQueue.size() > 0) {
    int openValvePin = valveQueue.front();
    valveQueue.dequeue();
    closeOpenValve(openValvePin, sensor, isOpeningValve, valveQueue);
  }
  
  //Open the next valve
  sensor.openValve(isOpeningValve);

  Serial.print("Opening next valve in queue: ");
  Serial.println(valvePin - 1);
  myStatus = String("Valve " + String(valvePin - 1) + String(" is opening - 247.")); 
  ThingSpeak.setStatus(myStatus);
  
  unsigned long startTime = millis();

  //wait for the associated sensor to detect wet conditions
  while (sensor.getCapacitiveReading(0) > moistureThreshold) {
    delay(500);  // Adjust the delay based on your application's requirements
    if(millis() - startTime > 10000) { // Set a timeout (e.g., 10 seconds) to prevent blocking indefinitely
      Serial.println("Timeout: Valve didn't close within Window. Closing Valve.(257)");
      myStatus = String("Valve " + String(valvePin - 1) + String(" didn't close within Window. Closing Valve.(257)")); 
      ThingSpeak.setStatus(myStatus);
      break;
    }
  }
  
  sensor.closeValve();  // Close the valve after a delay
  allValvesClosed = true; // Update the flag after closing the valve

  Serial.print("Valve ");
  Serial.print(valvePin - 1);
  Serial.println(" is CLOSED (204)");
  myStatus = String("Valve " + String(valvePin - 1) + String(" is closed(205)")); 
  ThingSpeak.setStatus(myStatus); 
}
void closeOpenValve(int openValvePin, SoilSensor &sensor, bool &isOpeningValve, Queue<int, numSensors> &valveQueue) {
  // Check if there is any valve currently open and wait until the moisture level is above the threshold
  while (sensor.getCapacitiveReading(0) < moistureThreshold) {
    delay(500);
  }
   // Close the previously opened valve
  sensor.closeValve();
  isOpeningValve = false;
  Serial.print("Closing previously opened valve: ");
  Serial.println(openValvePin - 1);
  myStatus = String("Valve " + String(openValvePin - 1) + String(" is closed (282)")); 
  ThingSpeak.setStatus(myStatus);
}




private:
  void displayData(uint16_t capRead) {
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

    lcd.display();
  }

  //method to display a warning if cap readings unstable
  void displayStabilityWarning() {
    Serial.println("Capacitive readings not stable. Waiting for stability...");
  }
};
//Number of soil moisture sensors


unsigned int ms1_field = 1;
unsigned int ms2_field = 2;
unsigned int ms3_field = 3;
unsigned int ms4_field = 4;

//Array of SoilSensor instances representing each sensor and corresponding valve
SoilSensor sensors[numSensors] = {
  SoilSensor(0, 2),  // Sensor 1 is connected to valve on pin 2
  SoilSensor(1, 3),  // Sensor 2 is connected to valve on pin 3
  SoilSensor(2, 4),   // Sensor 3 is connected to valve on pin 4
  SoilSensor(3, 5) // Sensor 4 is connected to valve on pin 5
};





void setup() {
  Serial.begin(115200);  //Initialize serial communication
  Wire.begin();          //Initialize I2C communication

  //initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  delay(2000);

  //Initialize Wifi and thingspeak
  connectToWiFi();
// check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
      myStatus = String("Communication with WiFi module failed!"); 
      ThingSpeak.setStatus(myStatus);
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
    
  ThingSpeak.begin(client);  //Initialize ThingSpeak

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


void connectToWiFi(){
 // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(SECRET_SSID); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000); 
    } 
    Serial.println("\nConnected.");
    myStatus = String("Wifi \nConnected."); 
    ThingSpeak.setStatus(myStatus);
  }
} 
void printValveStatus() {
  String valveStatus = "Valve Status: ";
  for (int i = 0; i < numSensors; ++i) {
    valveStatus += "MS" + String(i+1) + ": V" + String(sensors[i].getValvePin()-1) + "- " + (sensors[i].isValveOpen() ? "O" : "X");

    if (i < numSensors - 1) {
      valveStatus += ", ";
    }
  }
  Serial.println(valveStatus);
}




void loop() {
  // Connect or reconnect to WiFi
  if (WiFi.status()==WL_CONNECTED) { 
    unsigned long currentTime = millis();
    elapsedTime = currentTime - lastUpdateTime; 

    // Loop through each soil moisture sensor and read data one at a time
    //Check if all valves are closed
    if(allValvesClosed) {
      bool isOpeningValve = false;   
      
      for (int i = 0; i < numSensors; ++i) {
        int nextValveInQueue = -1; //default if queue is empty
        if(valveQueue.size() > 0) {
          nextValveInQueue = valveQueue.front();
        }

        if (!sensors[i].isValveOpen() && valveQueue.size() > 0 && allValvesClosed && !isOpeningValve) {
          int nextValvePin = valveQueue.front();
          valveQueue.dequeue();
          sensors[i].openValve(isOpeningValve);
        
          Serial.print("Valves in Queue: ");
          printQueue(valveQueue);
          //openNextValve(nextValvePin, sensors[i], isOpeningValve); // Pass the isOpeningValve flag   
      } else {
        sensors[i].readData(moistureThreshold, nextValveInQueue, isOpeningValve); 
      }
      delay(1000);
    
    //Reset the flag after teh loop completes
    isOpeningValve = false;

    printValveStatus(); 
    
    int ms1 = sensors[0].getCapacitiveReading(0);
    int ms2 = sensors[1].getCapacitiveReading(0);
    int ms3 = sensors[2].getCapacitiveReading(0);
    int ms4 = sensors[3].getCapacitiveReading(0);

    delay(20000);   
      
    ThingSpeak.setField(ms1_field, ms1);
    ThingSpeak.setField(ms2_field, ms2);
    ThingSpeak.setField(ms3_field, ms3);
    ThingSpeak.setField(ms4_field, ms4);

    if(ms1 > 0) {
    //write to thingspeak channel
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200) {
        Serial.println("data sent to thingspeak");
        Serial.print("Elapsed Time: "); 
        Serial.print(elapsedTime/1000); 
        Serial.println(" seconds");
        myStatus = String(valveStatus);
        ThingSpeak.setStatus(myStatus);
        lastUpdateTime = currentTime;
      } else {
        Serial.println("Problem updating channel. HTTP error code" + String(x));
        myStatus = String("Problem updating channel. HTTP error code" + String(x)); 
        ThingSpeak.setStatus(myStatus);
        delay(5000);
        }
    } else {
      Serial.println("Sensor Reading Invalid. MS cannot < 0");
      myStatus = String("Sensor Reading Invalid. TMS cannot < 0"); 
      ThingSpeak.setStatus(myStatus);
      delay(5000);
      }
    }
  } else {
    //If wifi is not connected, attempt to reconnect
    connectToWiFi();
  }
}}
    // Print the valves in the queue after opening a valve
     // Serial.print("Valves in Queue: ");
      //printQueue(valveQueue);
    //}

    
