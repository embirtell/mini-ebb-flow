/*  12_Channel_Keypad.ino
/*
 *  
 * Copyright (c) 2019 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Create Time: February 2019
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
//#include "SoftwareSerial.h"
#include <HardwareSerial.h>
#include <multi_channel_relay.h>


// #if defined(ARDUINO_ARCH_AVR)
#define SERIAL Serial
HardwareSerial mySerial(0); 
// #define TRANS_SERIAL  mySerial
// #elif defined(ARDUINO_ARCH_SAMD)
// #define SERIAL SerialUSB
// #define TRANS_SERIAL  Serial
// #else
// #endif

//for screen
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 
#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];
U8X8LOG u8x8log;

//for 2 channel relay
#define Two_channel_relay_1 A0

//for 4-channel relay

  /** 
   *  channel: 4 3 2 1
   *  state: 0b0000 -> 0x00  (all off)
   *  state: 0b1111 -> 0x0f   (all on)
  */ 

Multi_Channel_Relay relay;



void setup() {
	Wire.begin();
  Serial.begin(9600);  // start serial for output
  
  pinMode(Two_channel_relay_1, OUTPUT); //set 2 channel relay pin as OUTPUT
  
  // Keypad - Configure MySerial0 on pins TX=D6 and RX=D7 (-1, -1 means use the default)
  mySerial.begin(9600, SERIAL_8N1, -1, -1);
  //MySerial0.print("MySerial0");

    //For OLED Screen
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setFlipMode(3); //1-3

  u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
  u8x8log.setRedrawMode(1);		// 0: Update screen with newline, 1: Update screen for every char  
  //relay
  relay.begin(0x11);
  //turn all off to start
  relay.channelCtrl(0);
  DEBUG_PRINT.print("Turn off all channels, State: ");
  DEBUG_PRINT.println(relay.getChannelState(), BIN);
  
  delay(2000);
  
}

void loop() {
  
  printData();
}

/*
* data mapping:E1---1；E2---2；E3---3；E4---4；E5---5；E6---6；
*              E7---7；E8---8；E9---9；EA---*；EB---0；EC---#；
*/
void printData() {
    while(mySerial.available()) {
        uint8_t data = mySerial.read();
        switch(data) {
                case 0xE1 :
                    relay.channelCtrl(0);
                    // u8x8log.print("Valve: ");
                    // u8x8log.print(relay.getChannelState(), BIN);
                    // u8x8log.print("\n");
                    Serial.println("Valve 1 Opens");
                    relay.turn_on_channel(1);
                    u8x8log.print("V1 Open: ");
                    u8x8log.print(relay.getChannelState(), BIN);
                    u8x8log.print("\n");
                    delay(500);
                    break;
                    //Serial.println("1 pressed");
                    //u8x8log.print("1");
                case 0xE2 :
                    relay.channelCtrl(0);
                    // u8x8log.print("Valve: ");
                    // u8x8log.print(relay.getChannelState(), BIN);
                    // u8x8log.print("\n");
                    Serial.println("Valve 2 Opens");
                    relay.turn_on_channel(2);
                    u8x8log.print("V2 Open: ");
                    u8x8log.print(relay.getChannelState(), BIN);
                    u8x8log.print("\n");
                    delay(500);
                    //Serial.println("2 pressed");
                    //u8x8log.print("2");
                    //u8x8log.print("\n");
                    break;
                case 0xE3 :
                    relay.channelCtrl(0);
                    // u8x8log.print("Valve: ");
                    // u8x8log.print(relay.getChannelState(), BIN);
                    // u8x8log.print("\n");
                    Serial.println("Valve 3 Opens");
                    relay.turn_on_channel(3);
                    u8x8log.print("V3 Open: ");
                    u8x8log.print(relay.getChannelState(), BIN);
                    u8x8log.print("\n");
                    delay(500);
                    
                    //Serial.println("3 pressed");
                    //u8x8log.print("3");
                    //u8x8log.print("\n");
                    break;
                case 0xE4 :
                    relay.channelCtrl(0);
                    Serial.println("Valve 4 Opens");
                    relay.turn_on_channel(4);
                    u8x8log.print("V4 Open: ");
                    u8x8log.print(relay.getChannelState(), BIN);
                    u8x8log.print("\n");
                    delay(500);


                    
                    
                    // Serial.println("4 pressed");
                    // u8x8log.print("4");
                    //u8x8log.print("\n");
                    break;
                case 0xE5 :
                    Serial.println("5 pressed");
                    u8x8log.print("5");
                    //u8x8log.print("\n");
                    break;
                case 0xE6 :
                    Serial.println("6 pressed");
                    u8x8log.print("6");
                    //u8x8log.print("\n");
                    break;
                case 0xE7 :
                    Serial.println("7 pressed");
                    u8x8log.print("7");
                    //u8x8log.print("\n");
                    break;
                case 0xE8 :
                    Serial.println("8 pressed");
                    u8x8log.print("8");
                    //u8x8log.print("\n");
                    break;
                case 0xE9 :
                    relay.channelCtrl(0);
                    Serial.println("9 pressed");
                    u8x8log.print("Pump Relay Blink");
                    u8x8log.print("\n");
                    digitalWrite(Two_channel_relay_1, HIGH);
                    delay(1000);
                    digitalWrite(Two_channel_relay_1, LOW);
                    break;
                case 0xEA :
                    Serial.println("* pressed");
                    //u8x8log.print("*");
                    relay.channelCtrl(CHANNLE1_BIT | 
                                      CHANNLE2_BIT | 
                                      CHANNLE3_BIT | 
                                      CHANNLE4_BIT);
                    Serial.print("Turn all channels on, State: ");
                    Serial.print(relay.getChannelState(), BIN);
                    u8x8log.print("All Open");
                    u8x8log.print("\n");
                    break;
                   
                    
                case 0xEB :
                    Serial.println("0 pressed");
                    u8x8log.print("0");
                    //u8x8log.print("\n");

                  
                case 0xEC :
                    relay.channelCtrl(0);
                    Serial.println("# pressed");
                    u8x8log.print("All Closed");
                    //u8x8log.print("#");
                    u8x8log.print("\n");
                    break;
                default:
                    break;
            }
    }
  delay(200);
}



