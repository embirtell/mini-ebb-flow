#include "i2cHub.h"
#include <Wire.h>

void I2CHub::setup(char addr) {
  this -> addr = addr;
  currentPort = 0;
  
  //Wire.begin();
  //This was commented before
  /*Wire.beginTransmission(this -> addr);
  Wire.write(0b11111111);
  Wire.endTransmission();*/
  
  //setPort(0);
  //Serial.printf("I2C Hub Address: %d\n", addr);
}

void I2CHub::setPort(char port) {
  if (port > 7) return;
  //Serial.printf("Set Port: %d\n", port)
  
  Wire.beginTransmission(this -> addr);
  Wire.write(1 << port);
  Wire.endTransmission();

  currentPort = port;
  delay(100);
}

char I2CHub::getCurrentPort() {
  return currentPort;
}