#include "valve_sensor_pair.h"
#include "Pump.h"
#include "wifiConnect.h"
#include "ThingSpeak.h" //always include thingspeak header file after other header files and custom macros



#define NUM_CHANNELS 4
#define SENSOR_THRESHOLD 400
#define SAMPLE_COUNT 30
#define THINGSPEAK_UPDATE_DELAY 15000


Channel channels[NUM_CHANNELS];
I2CHub mux; //object for the bus
Pump pump; //creates object for the pump
wifiConnect wifi; //wifConnect Object
WiFiClient Client; //WifiClient for Thingspeak



void setup() {
  Serial.begin(115200);

  wifi.initWifi();
  if (WiFi.status() == WL_CONNECT_FAILED) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  ThingSpeak.begin(Client);

  Wire.begin(); //For I2C, should only be begun once.

  mux.setup(0x70); 
  pump.setup(A0); //GPIO pin of pump
  
  for (int i = 0; i < NUM_CHANNELS; i++) {
    Serial.printf("Setting up channel %d", i+1);
    channels[i].setup(0x36 + i, 0x11, i, NUM_CHANNELS, SAMPLE_COUNT); //Runs through 0x36 up to 0x39, then adds 0x11 for all of them
    channels[i].setupMux(mux, i, -1);
  }
  //Serial.println("channels set up")
}

void loop() {
  wifi.wifiCheck(); //TODO Add Wifi Error Message

  //For ea
  for (uint8_t sampleNum = 0; sampleNum < SAMPLE_COUNT; sampleNum++){
    for (uint8_t i = 0; i < NUM_CHANNELS; i+=1) {
      Serial.printf("Channel: %d\n", i+1);
      mux.setPort(i);
      channels[i].readSensor(sampleNum);
    }
    delay(THINGSPEAK_UPDATE_DELAY/SAMPLE_COUNT);
  }
  
  for(int i=0; i < NUM_CHANNELS; i++){ //TODO add 15 second delay to send data, separate valves so doesnt mess with packet sending.
    float avgReading = channels[i].averageSamplesAndPublish(i);
    if(avgReading < SENSOR_THRESHOLD) {
      Serial.println("Opening");
      channels[i].valveOpen(); //FIX LATER TODO
      delay(1000);        //Adjust to 3-5 seconds
      pump.blink(3000);
      delay(1000);
      channels[i].valveClose(); //TODO
      Serial.println("Closing");
      delay(500);         //Adjust to 3-5 seconds
    }
    else{
      delay(1000);
    }
  }
  
}