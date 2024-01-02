//#include <WiFiNINA.h>
#include "WiFi.h"
#include <Wire.h>
#include "secrets.h"
#include "Adafruit_seesaw.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

<<<<<<< HEAD


char ssid[] = SECRET_SSID;   // your network SSID (name) 
=======
char ssid[] = SECRET_SSID;   // your network SSID (name)
>>>>>>> a7befe8ae179af21083e0ebbacea1d11c1488783
char pass[] = SECRET_PASS;   // your network password

WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
//IOT configuration variables
const int thingspeakUpdateDelay = 15000;
long lastThingspeakUpdateTime = 0;

// Initialize our values

String myStatus = "";
//system configuration varaibles
const int numSensors = 4;
<<<<<<< HEAD
const int numGroveSensors = 1;
Adafruit_seesaw sensorArray[numSensors];
const int sampleCount = 30; 
int16_t sampleArrays[numSensors][sampleCount]; //holds data for averaging
const int samplingDelay = thingspeakUpdateDelay/sampleCount; 
=======
Adafruit_seesaw sensor_array[numSensors]; //holds the sensor objects
const int sampleCount = 30; //you decide on how many datapoints should be taken in between updates to thingspeak
int16_t sampleArrays[numSensors][sampleCount]; //holds data for averaging
const int samplingDelay = thingspeakUpdateDelay/sampleCount; //the delay between each sample taken should be equal to the total delay between updates divided by the number of samples taken
>>>>>>> a7befe8ae179af21083e0ebbacea1d11c1488783
long lastSampleTime = 0;
int sampleIndex = 0;

//// Initialize I2C buses using TCA9548A I2C Multiplexer https://randomnerdtutorials.com/tca9548a-i2c-multiplexer-esp32-esp8266-arduino/
void tcaSelect(uint8_t bus){
  //if (bus > 7) return;

  Wire.beginTransmission(0x70); // TCA9548A address is 0x70
  Wire.write(1 << bus); // send byte to select bus
  Wire.endTransmission();
  
  //Serial.print(bus);
}//void tcaselect(uint8_t i2cBus)


void seesawReadData() {
  for(int i = 0; i < numSensors; i++){
  	tcaSelect(i);
    //set field (thingspeak fields index start at 1) with each sensor in array.touchRead(0) (returns uint16, should be acceptable to thingspeak)
  	uint16_t capread = sensorArray[i].touchRead(0);
  	if(capread < 0 || capread > 2000){
  		Serial.print("Sensor: ");
  		Serial.print(i+1);
  		Serial.println(" is not working!");
      sampleArrays[i][sampleIndex] = -1;
  	}else{
  		// ThingSpeak.setField(i+1, int(capread));
        sampleArrays[i][sampleIndex] = capread;
  		Serial.print("Sensor ");
  		Serial.print(i+1);
  		Serial.print(": ");
  		Serial.println(capread);
  	}//if else capread < 0
  }//for(int i = 0; i < numSensors;i++
  sampleIndex++;
}//void seesawReadData()

void groveReadData(){
  for(int i = 0; i < numGroveSensors; i++) {
  //read input on analog pin 0:
  int groveReading = analogRead(A0);
  Serial.print("Sensor 5(G)");
  Serial.print(": ");
  Serial.println(groveReading);

  ThingSpeak.setField(5, groveReading);
  }//for(int i = 0; i < numGroveSensors; i++)
}//void groveReadData()



void averageSamplesAndPublish(){
  
  myStatus = "";
  
  for(int j = 0; j <numSensors;j++) {
    int summation = 0;
    int sampleRangeErr = 0;
    for(int i = 0; i < sampleCount; i++) {
      if(sampleArrays[j][i] < 0){// if data invalid, count sample as error
        sampleRangeErr++;
      } else {
        summation += sampleArrays[j][i];
        }//else
    }//for(int i = 0; i < sampleCount; i++){
    
    float avgSample = summation/(sampleCount - sampleRangeErr);

    ThingSpeak.setField(j+1, avgSample);
  
    Serial.print("Sensor ");
    Serial.print(j+1);
    Serial.print(" sampleRangeErr: ");
    Serial.println(sampleRangeErr);
  
    if(sampleRangeErr > 0){
      Serial.println("updating status with error");
      myStatus += String("Sensor ");
      myStatus += String(j+1);
      myStatus += String(": ");
      myStatus += String(sampleRangeErr);
      myStatus += String(" readings out of range"); // \n is newline character
      ThingSpeak.setStatus(myStatus);
    }//if(sampleRangleErr > 0){

  }//for(int j = 0; j <numSensors;j++){

  sampleIndex = 0;

}//void averageSamples()



//initiate wifi
void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }//while (WiFi.status() != WL_CONNECTED)
  Serial.println();
  Serial.println(WiFi.localIP());
}//void initWifi



void setup() {
  Serial.begin(115200);  // Initialize serial
<<<<<<< HEAD
  initWifi();
=======

>>>>>>> a7befe8ae179af21083e0ebbacea1d11c1488783
  // check for the WiFi module:
  if (WiFi.status() == WL_CONNECT_FAILED) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }


  //each sensor is started up. Given and I2C address and activated in the sensor_array[]
  ThingSpeak.begin(client);  //Initialize ThingSpeak
  
  for(int i = 0; i < numSensors; i++){
  	//initialize sensors 
    tcaSelect(i);
    int i2cAddress = 0x36 + i;
  	if (!sensorArray[i].begin(i2cAddress)) {
    		Serial.println("ERROR! seesaw not found");
    		while(1) delay(1);
  	} else {
    		Serial.print("seesaw started! version: ");
    		Serial.println(sensorArray[i].getVersion(), HEX);
  	}//if else sensor.begin
  }//(int i = 0; i < 4;i++)
}//void setup

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
<<<<<<< HEAD
      delay(5000);     
    } //while(WiFi.status() != WL_CONNECTED)
    Serial.println("\nConnected.");
  }//if(WiFi.status() != WL_CONNECTED)
	
  
  // set the fields with the values
=======
      delay(5000);
    }
    Serial.println("\nConnected.");
  }//if wifi

  // time for a new data sample
>>>>>>> a7befe8ae179af21083e0ebbacea1d11c1488783
  if(millis() - lastSampleTime >= samplingDelay ){
    seesawReadData();
    groveReadData();
    lastSampleTime = millis();
  }//if(millis() - lastSampleTime >= samplingDelay )

  //time for a new thingspeak update
  if(millis() - lastThingspeakUpdateTime >= thingspeakUpdateDelay ){
    // write to the ThingSpeak channel
    averageSamplesAndPublish();
    ThingSpeak.setStatus(myStatus);
    
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(x == 200){
      Serial.println("Channel update successful.");
      lastThingspeakUpdateTime = millis();
    }else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      myStatus += String("Problem updating channel. HTTP error code " + String(x));
      delay(5000);
    }//if else for thingspeak
  }//if millis() delay for thingspeak

}//void loop
