/*
  Made on 18 may 2021
  Home
  based on Arduino Library
*/
//#include "TCA9548A.h"

#include <Wire.h>
#define TCA9548A 0x70 
#define WIRE Wire


void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCA9548A);
  Wire.write(1 << i);
  Wire.endTransmission();  
}



void setup() {
    
  

  Serial.begin(115200);
  while (!Serial); // Leonardo: wait for serial monitor
  
  Wire.begin();
  
  Serial.println("\nI2C Scanner");
  
  // for (uint8_t t=0; t<8; t++) {
  //     tcaselect(t);
  //     Serial.print("TCA Port #"); Serial.println(t);

  //     for (uint8_t addr = 0; addr<=127; addr++) {
  //       if (addr == TCA9548A) continue;

  //       Wire.beginTransmission(addr);
  //       if (!Wire.endTransmission()) {
  //         Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
  //       }
  //     }
  //   }
  //   Serial.println("\ndone");





}

void loop() {
  for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      Serial.print("TCA Port #"); Serial.println(t);

      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCA9548A) continue;

        Wire.beginTransmission(addr);
        if (!Wire.endTransmission()) {
          Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");
    delay(3000);



  // int nDevices = 0;

  // Serial.println("Scanning...");

  // for (byte address = 1; address < 127; ++address) {
  //   // The i2c_scanner uses the return value of
  //   // the Write.endTransmisstion to see if
  //   // a device did acknowledge to the address.
  //   Wire.beginTransmission(address);
  //   byte error = Wire.endTransmission();

  //   if (error == 0) {
  //     Serial.print("I2C device found at address 0x");
  //     if (address < 16) {
  //       Serial.print("0");
  //     }
  //     Serial.print(address, HEX);
  //     Serial.println("  !");

  //     ++nDevices;
  //   } else if (error == 4) {
  //     Serial.print("Unknown error at address 0x");
  //     if (address < 16) {
  //       Serial.print("0");
  //     }
  //     Serial.println(address, HEX);
  //   }
  // }
  // if (nDevices == 0) {
  //   Serial.println("No I2C devices found\n");
  // } else {
  //   Serial.println("done\n");
  // }
  // delay(5000); // Wait 5 seconds for next scan
 }

