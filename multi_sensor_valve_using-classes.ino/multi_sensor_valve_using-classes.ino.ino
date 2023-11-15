
#include "Adafruit_seesaw.h" //for moisture sensor
Adafruit_seesaw ss;
#include <Wire.h>


const int numSensors = 3;
Adafruit_seesaw sensors[numSensors]; //array that holds instances of soil sensor class


// Class representing a Valve
class Valve {
private:
    int pin; // Arduino GPIO pin connected to the valve

public:
   //Member functions
    Valve(int pin) : pin(pin) {
        pinMode(pin, OUTPUT);
        close(); // Ensure valves are closed initially
    }

    void open() {
        digitalWrite(pin, HIGH);
        Serial.print("Valve ");
        Serial.print(pin);
        Serial.println(" is OPEN");
    }

    void close() {
        digitalWrite(pin, LOW);
        Serial.print("Valve ");
        Serial.print(pin);
        Serial.println(" is CLOSED");
    }
};

// Class representing a Soil Moisture Sensor
class SoilMoistureSensor {
private:
    Valve& associatedValve;
    Adafruit_seesaw sensor;

    const int moistureThreshold = 300; // Adjust as needed

    const int stabilityThreshold = 3; //Adjust as needed
    int consecutiveStableReadings = 0;

public:
    SoilMoistureSensor(Valve& valve, uint8_t i2cAddress) : associatedValve(valve) {
        sensor.begin(i2cAddress);
    };

    // Method to check the soil moisture level and control the valve accordingly
    float checkMoistureLevel() {
        // Read moisture level from the soil sensor
        float moistureValue = sensor.touchRead(0);


        if(!isnan(moistureValue)){
        
          Serial.print("Moisture Level: ");
          Serial.print(moistureValue);
          Serial.print(" - ");

        // Assuming a moisture threshold 
        if (moistureValue < moistureThreshold) {
            // Increment consecutive stable readings
            consecutiveStableReadings++;
            
            if(consecutiveStableReadings >= stabilityThreshold){
              associatedValve.open();
            }
        } else {
          //Reset consecutive stable readings if moisture value is below the threshold
            consecutiveStableReadings = 0;
            associatedValve.close();
        }
        } else{
          Serial.println("Failed");
        } 
    }
};

const int numValves = 3;
Valve valves[numValves] = {
    Valve(2), // Specify the GPIO pin numbers for each valve
    Valve(3),
    Valve(4)
};

// Specify the I2C addresses for each sensor
uint8_t soilSensorAddresses[numValves] = {0x36, 0x37, 0x38};

SoilMoistureSensor moistureSensors[numValves] = {
    SoilMoistureSensor(valves[0], soilSensorAddresses[0]),
    SoilMoistureSensor(valves[1], soilSensorAddresses[1]),
    SoilMoistureSensor(valves[2], soilSensorAddresses[2])
};

void setup() { 
  Serial.begin(115200); // Initialize serial communication with baud rate 115200
  Wire.begin(); // Initialize I2C communication
  
  for (int i = 0; i < numSensors; ++i) {
    sensors[i].begin(0x36 + i);
  }
      //CHECK IF MOISTURE SENSOR IS THERE
  
}

void loop() {
    for (int i = 0; i < numSensors; ++i) {
      Serial.print("Reading from Sensor ");
      Serial.print(i + 1); //makes human readable

      float tempC = sensors[i].getTemp();
      uint16_t capread1 = sensors[i].touchRead(0);

      Serial.print("Temperature: ");
      Serial.println(tempC);
      Serial.print("Capacitive Reading: ");
      Serial.println(capread1);
      Serial.println();
    }
    
    // Check soil moisture levels using the Adafruit STEMMA Soil Sensors
    for (int i = 0; i < numValves; ++i) {
        //Get the current soil moisture sensor
        float moistureLevel = moistureSensors[i].checkMoistureLevel();
        SoilMoistureSensor currentSensor = moistureSensors[i];
        currentSensor.checkMoistureLevel();

        //Print the sensor index
        Serial.print("Sensor ");
        Serial.print(i + 1); //adding 1 to make the index human readable (starting from 1, not 0)
        Serial.print(": ");
        
        moistureSensors[i].checkMoistureLevel();
        //Call the checkMoistureLevel method for the current sensor
        
    }

    // Add a delay here if needed to control how often you check the soil moisture levels
    delay(1000);
}