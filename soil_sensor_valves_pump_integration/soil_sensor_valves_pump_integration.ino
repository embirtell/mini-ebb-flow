#include "valve_sensor_pair.h"
#include "Pump.h"
#include "wifiConnect.h"
#include "ThingSpeak.h" //always include thingspeak header file after other header files and custom macros



#define NUM_CHANNELS 4
#define SENSOR_THRESHOLD 400

Channel channels[NUM_CHANNELS];
I2CHub mux; //object for the bus
Pump pump; //creates object for the pump

/*void setPort(uint8_t port) {
  Serial.println("Start of port");
  Serial.printf("Port: %d\n", port);
  if (port > 7) return;
  //Serial.printf("Set Port: %d\n", port)
  Serial.println("Passed return statement");
  
  Wire.beginTransmission(0x70);
  Serial.printf("Port Bits: %d\n", 1 << port);
  Wire.write(1 << port);
  Serial.println(Wire.endTransmission());
  Serial.println("End of port");
}*/

void setup() {
  Serial.begin(115200);

  initWifi();
  Wire.begin();

  mux.setup(0x70); 
  pump.setup(A0); //GPIO pin of pump
  
  for (int i = 0; i < NUM_CHANNELS; i++) {
    channels[i].setup(0x36 + i, 0x11, i); //Runs through 0x36 up to 0x39, then adds 0x11 for all of them
    channels[i].setupMux(mux, i, -1);
  }
}

void loop() {
  wifiCheck();
  mux.setPort(0);
  uint16_t reading = 0;
  for (uint8_t i = 0; i < NUM_CHANNELS; i+=1) {
    reading = channels[i].readSensor();
    Serial.printf("Channel: %d\n", i+1);
    mux.setPort(i+1);
    Serial.println(reading);
    

    if(reading < SENSOR_THRESHOLD) {
      Serial.println("Opening");
      channels[i].valveOpen();
      delay(1000);        //Adjust to 3-5 seconds
      pump.blink(3);
      delay(1000);
      channels[i].valveClose();
      Serial.println("Closing");
      delay(500);         //Adjust to 3-5 seconds
    }
    else{
      delay(1000);
    }
  }
}