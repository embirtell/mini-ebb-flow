#include <WiFiNINA.h>
#include "secrets.h"
#include "Adafruit_seesaw.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values

String myStatus = "";
const int numSensors = 4;
Adafruit_seesaw sensor_array[numSensors];

void setup() {
  Serial.begin(115200);  // Initialize serial
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
    
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
}

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
  }
	
	//insert soil moisture reading here 
  
  
  // set the fields with the values
  for(int i = 0; i < numSensors;i++){
  	//set field (thingspeak fields index start at 1) with each sensor in array.touchRead(0) (returns uint16, should be acceptable to thingspeak)
  	uint16_t capread = sensor_array[i].touchRead(0);
  	if(capread < 0 || capread > 2000){
  		Serial.print("Sensor: ");
  		Serial.print(i+1);
  		Serial.println("is not working!");
  		myStatus = String("An error has occured");
  	}else{
  		ThingSpeak.setField(i+1, capread);
  		Serial.print("Sensor ");
  		Serial.print(i+1);
  		Serial.print(": ");
  		Serial.println(capread);
  	}//if else capread < 0
  }//for(int i = 0; i < numSensors;i++
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(15000); //delay required for thingspeak 
  
}
