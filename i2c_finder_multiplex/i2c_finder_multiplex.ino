/*
  Made on 18 may 2021
  Home
  based on Arduino Library
*/
#include "TCA9548A.h"

#include <Wire.h>
TCA9548A TCA;
#define WIRE Wire
#define sdaPin 4
#define sclPin 5

void setup() {
  Serial.begin(115200);
  while (!Serial); // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
  
  Wire.begin();
  TCA.begin(sdaPin, sclPin);
  //defaut all channel was closed
  //TCA.openAll();
  //TCA.closeAll();
 
  // Select the channels you want to open. You can open as many channels as you want
  TCA.openChannel(TCA_CHANNEL_0);   //TCA.closeChannel(TCA_CHANNEL_0);
  TCA.openChannel(TCA_CHANNEL_1); //TCA.closeChannel(TCA_CHANNEL_1);
  TCA.openChannel(TCA_CHANNEL_2); //TCA.closeChannel(TCA_CHANNEL_2);
  TCA.openChannel(TCA_CHANNEL_3); //TCA.closeChannel(TCA_CHANNEL_3);
  TCA.openChannel(TCA_CHANNEL_4); //TCA.closeChannel(TCA_CHANNEL_4);
  TCA.openChannel(TCA_CHANNEL_5); //TCA.closeChannel(TCA_CHANNEL_5);
  TCA.openChannel(TCA_CHANNEL_6); //TCA.closeChannel(TCA_CHANNEL_6);
  TCA.openChannel(TCA_CHANNEL_7); //TCA.closeChannel(TCA_CHANNEL_7); 

}

void loop() {
  uint8_t error, i2cAddress, devCount, unCount;
  
  Serial.println("Scanning...");
  
   devCount = 0;
   unCount = 0;

  

  for (i2cAddress = 1; i2cAddress < 127; ++i2cAddress) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(i2cAddress);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (i2cAddress < 16) {
        Serial.print("0");
      }
      Serial.print(i2cAddress, HEX);
      Serial.println("  !");

      ++devCount;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (i2cAddress < 16) {
        Serial.print("0");
      }
      Serial.println(i2cAddress, HEX);
      unCount++;
    }
  }
  if (devCount + unCount == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    SERIAL.print(devCount);
    SERIAL.print(" device(s) found");
    if (unCount > 0) {
      SERIAL.print(", and unknown error in ");
      SERIAL.print(unCount);
      SERIAL.print(" address");
      Serial.println("done\n");
  }
  delay(5000); // Wait 5 seconds for next scan
 }

