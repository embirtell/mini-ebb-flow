//code to control ball valves and pump - just to verify functional

#include <U8x8lib.h>
#include <ezOutput.h>
#include <ezButton.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 	

//constants var don't change
ezButton button(D1);
ezButton buttonB(D6);
ezButton buttonR(D3);
ezButton buttonG(D0);

//ezButton buttonW(D2);


ezOutput valve1(D7);
ezOutput valve2(D8);
ezOutput valve3(D9);
ezOutput valve4(D10);
//ezOutput pump(D4);


//var that will change
int buttonState = 0;
int buttonStateG = 0;
int buttonStateB = 0;
int buttonStateR = 0;

void setup() {
  Serial.begin(9600);
  // pinMode(button, INPUT_PULLUP);
  // pinMode(buttonG, INPUT_PULLUP);
  // pinMode(buttonR, INPUT_PULLUP);


  button.setDebounceTime(50); //in milliseconds
  buttonG.setDebounceTime(50);
  buttonB.setDebounceTime(50);
  buttonR.setDebounceTime(50);
  //buttonW.setDebounceTime(50);
  //initialize ouputs
  //pump.low(); //turn pump off
  // valve1.low(); //close valves
  // valve2.low();
  // valve3.low();
  // valve4.low();

   
  u8x8.begin();
  //u8x8.setPowerSave(0);
  //u8x8.setFlipMode(3);//# from 1-3, screen work will rotate
  u8x8.setFlipMode(1);
}

void loop() {
  //pump.loop();
  valve1.loop();
  valve2.loop();
  valve3.loop();
  valve4.loop();
  button.loop();
  buttonB.loop();
  buttonG.loop();
  buttonR.loop();
  //buttonW.loop();
  
  
  //set font
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,1);
  u8x8.print("v1");
  u8x8.setCursor(0,2);
  u8x8.print("v2");
  u8x8.setCursor(0,3);
  u8x8.print("v3");
  u8x8.setCursor(0,4);
  u8x8.print("v4");
 

  // // read the state of the pushbutton value:
  // buttonState = digitalRead(button);
  // Serial.println(buttonState);
  // buttonStateG = digitalRead(buttonG);
  // Serial.println(buttonStateG);
  // buttonStateR = digitalRead(buttonR);
  // Serial.println(buttonStateR);
  // int btnState = button.getState();
  // Serial.print("Pump Button state: ");
  // Serial.println(btnState);
  // int btnGState = buttonG.getState();
  // Serial.print("Green Button state: ");
  // Serial.println(btnGState);
  // int btnRState = buttonR.getState();
  // Serial.print("Red Button state: ");
  // Serial.println(btnRState);

  // read the state of the push button values
  // if (button.isPressed()) {
  //   Serial.println("Pump Button Pressed");
  //   u8x8.clearLine(0);
  //   u8x8.drawString(0,0,"pump is started");
  //   //pump.low();
  //   //pump.pulse(3000); //turns on pump for [time], and afterwards, pump will be turned off by pump.loop() fxn
  // } else if (button.isReleased()) {
  //   //pump.low();
  //   u8x8.clearLine(0);
  //   u8x8.setCursor(0,0);
  //   u8x8.print("pump is off");
  // }

  // check if green button is pressed. If it is, the buttonState is HIGH:
   //else if (buttonB.isReleased()){
  //   //close valves
  //   valve1.high();
  //   valve2.high();
  //   u8x8.clearLine(2);
  //   u8x8.setCursor(0,2);
  //   u8x8.print("v 3 & 4 Closed");
  //   delay(5000);
  // }
  if(button.isPressed()) {
    Serial.println("Button pressed");
    //open valves    
    valve1.toggle();
    buttonG.getState();
    buttonB.getState();
    Serial.println(button.getState());
    
    //give time for valves to open
    u8x8.clearLine(1);
    u8x8.setCursor(0,1);
    u8x8.print("v1: ");   
    u8x8.print(valve1.getState());
  }
  if(buttonB.isPressed()) {
    Serial.println("Blue Button Pressed");
    //open valves    
    valve2.toggle();
    buttonB.getState();
    Serial.println(buttonB.getState());
    
    //give time for valves to open
    u8x8.clearLine(2);
    u8x8.setCursor(0,2);
    u8x8.print("v2: ");   
    u8x8.print(valve2.getState());
  } 

  if(buttonR.isPressed()) {
    Serial.println("Red Button Pressed");
    //open valves    
    valve3.toggle();
    buttonR.getState();
    Serial.println(buttonR.getState());
    
    //give time for valves to open
    u8x8.clearLine(3);
    u8x8.setCursor(0,3);
    u8x8.print("v3: ");   
    u8x8.print(valve3.getState());
  }
  if(buttonG.isPressed()) {
    Serial.println("Green Button Pressed");
    //open valves    
    valve4.toggle();
    buttonG.getState();
    Serial.println(buttonG.getState());
    
    //give time for valves to open
    u8x8.clearLine(4);
    u8x8.setCursor(0,4);
    u8x8.print("v4: ");   
    u8x8.print(valve4.getState());
  }

  
}