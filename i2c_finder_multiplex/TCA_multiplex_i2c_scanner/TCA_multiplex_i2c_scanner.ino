/**
 * TCA9548 I2CScanner.ino -- I2C bus scanner for Arduino
 *
 * Based on https://playground.arduino.cc/Main/I2cScanner/
 *
 */
 //Modified by Eva and Nicky, Sparks Lab
 //1-3-2024

#include "Wire.h"

#define TCA9548A 0x70

//Tells the breakout board (TCA chip) to open one port at a time
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCA9548A);
  Wire.write(1 << i);
  Wire.endTransmission();  
}


void setup()
{
    delay(1000);
    
    Wire.begin();   //Start i2c interface
    
    Serial.begin(115200);
    Serial.println("\nTCAScanner ready!");
    
    //Outer for loop - select one i2c breakout port at a time
    for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      Serial.print("TCA Port #"); Serial.println(t);

      //Inner for loop - search for all possible i2c addresses
      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCA9548A) continue;   //Skip the TCA/breakout chip address

        Wire.beginTransmission(addr);
        //If statement is true if something responded
        if (!Wire.endTransmission()) {
          Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");
}

void loop() 
{
//to get specific address later : tcaselect(2);
}