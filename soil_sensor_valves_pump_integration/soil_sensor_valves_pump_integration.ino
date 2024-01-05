#include "valve_sensor_pair.h"
#include "Pump.h"
#include "wifiConnect.h"
#include "ThingSpeak.h" //always include thingspeak header file after other header files and custom macros



#define NUM_CHANNELS 4
#define SENSOR_THRESHOLD 400
#define SAMPLE_COUNT 30


Channel channels[NUM_CHANNELS];
I2CHub mux; //object for the bus
Pump pump; //creates object for the pump
wifiConnect wifi;
WiFiClient Client;



void setup() {
  Serial.begin(115200);

  wifi.initWifi();
  ThingSpeak.begin(Client);

  Wire.begin();

  mux.setup(0x70); 
  pump.setup(A0); //GPIO pin of pump
  
  for (int i = 0; i < NUM_CHANNELS; i++) {
    channels[i].setup(0x36 + i, 0x11, i, NUM_CHANNELS, SAMPLE_COUNT); //Runs through 0x36 up to 0x39, then adds 0x11 for all of them
    channels[i].setupMux(mux, i, -1);
  }
}

void loop() {
  wifi.wifiCheck();
  uint16_t reading = 0;
  int16_t sampleArrays[NUM_CHANNELS][SAMPLE_COUNT];
  for (uint8_t sampleNum = 0; sampleNum < SAMPLE_COUNT; sampleNum++){
    for (uint8_t i = 0; i < NUM_CHANNELS; i+=1) {
      //Serial.printf("Channel: %d\n", i+1);
      mux.setPort(i);
      channels[i].readSensor();
    }
  }
  
  for(int i=0; i < NUM_CHANNELS; i++){
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