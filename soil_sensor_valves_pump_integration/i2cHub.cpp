#include "i2cHub.h"

I2CHub::I2CHub(char addr) {
  this -> addr = addr;
  currentPort = -1;
  //Add setup implementation
}

void I2CHub::setPort(char port) {
  //Implement port setting
}

char I2CHub::getCurrentPort() {
  return currentPort;
}