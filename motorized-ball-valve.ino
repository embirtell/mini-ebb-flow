///Code is written for Arduino Edge and uses latching relays. Not sure exactly how it will need to be modified for non-latching relays
///Code is also for 3-wire US Solid Motorised Ball Valve (9 - 24 V) and uses a 12V power supply
///https://docs.arduino.cc/tutorials/edge-control/motorized-ball-valve
//Functional - worked with Joe, opened 1 valve based on MS 
#include "Adafruit_seesaw.h"

#define THRESHOLD   800 // Change Threshold here



Adafruit_seesaw ss;

void setup() {
  Serial.begin(115200);

  Serial.println("seesaw Soil Sensor example!");
  
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW); // Makes it so that the valve is normally closed
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while(1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
}

void loop() {
  unsigned long currentTime = millis();
  
  
  
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);


  if(capread <THRESHOLD) {
    Serial.print ("The soil moisture is DRY => activate valve");
    digitalWrite(5, HIGH);
  } else {
    Serial.print("The soil moisture is WET => deactivate the valve");
    digitalWrite(5, LOW);
  }
  Serial.print("Temperature: "); Serial.print(tempC); Serial.println("*C");
  Serial.print("Capacitive: "); Serial.println(capread);
  delay(100);
  
  }

}

