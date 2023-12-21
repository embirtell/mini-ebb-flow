#include <ezOutput.h>
#include <ezButton.h>


ezOutput pump(A5);
ezButton pumpButton(12);

void setup() {
  Serial.begin(9600);

  pumpButton.setDebounceTime(50); 
  pump.low();
  Serial.println("controlling pump");
}

void loop() {
  pump.loop();
  pumpButton.loop();

// read the state of the push button values
  if (pumpButton.isPressed()) {
    Serial.println("Pump is started");
    pump.low();
    pump.pulse(2000); //turns on pump for [time], and afterwards, pump will be turned off by pump.loop() fxn
  // } else if (pumpButton.isReleased()) {
   // pump.low();
    
  }

}
