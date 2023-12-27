#include <WiFiNINA.h>
#include "secrets.h"
#include "Adafruit_seesaw.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name)
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
Adafruit_seesaw sensor_array[numSensors]; //holds the sensor objects
const int sampleCount = 30; //you decide on how many datapoints should be taken in between updates to thingspeak
int16_t sampleArrays[numSensors][sampleCount]; //holds data for averaging
const int samplingDelay = thingspeakUpdateDelay/sampleCount; //the delay between each sample taken should be equal to the total delay between updates divided by the number of samples taken
long lastSampleTime = 0;
int sampleIndex = 0;

void seesawReadData(){
  for(int i = 0; i < numSensors;i++){
  	//set field (thingspeak fields index start at 1) with each sensor in array.touchRead(0) (returns uint16, should be acceptable to thingspeak)
  	uint16_t capread = sensor_array[i].touchRead(0);
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

void averageSamplesAndPublish(){
  myStatus = "";
  for(int j = 0; j <numSensors;j++){
    int summation = 0;
    int sampleRangeErr = 0;
    for(int i = 0; i < sampleCount; i++){
      if(sampleArrays[j][i] < 0){// if data invalid, count sample as error
        sampleRangeErr++;
        }else{summation += sampleArrays[j][i];}
    }//for(int i = 0; i < sampleCount; i++){
    ThingSpeak.setField(j+1,summation/(sampleCount - sampleRangeErr));
    Serial.print("sampleRangeErr: ");Serial.println(sampleRangeErr);
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

void setup() {
  Serial.begin(115200);  // Initialize serial

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }


  //each sensor is started up. Given and I2C address and activated in the sensor_array[]
  ThingSpeak.begin(client);  //Initialize ThingSpeak
  for(int i = 0; i < numSensors;i++){
  	int i2cAddress = 0x36 + i;
  	if (!sensor_array[i].begin(i2cAddress)) {
    		Serial.println("ERROR! seesaw not found");
    		while(1) delay(1);
  	} else {
    		Serial.print("seesaw started! version: ");
    		Serial.println(sensor_array[i].getVersion(), HEX);
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
      delay(5000);
    }
    Serial.println("\nConnected.");
  }//if wifi

  // time for a new data sample
  if(millis() - lastSampleTime >= samplingDelay ){
    seesawReadData();
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
