#include "Pump.h"
#include <Arduino.h>

void Pump::setup(char pin){
    this->pin = pin;
    pinMode(pin, OUTPUT);
}

void Pump::blink(int seconds){
    digitalWrite(pin, HIGH);
    delay(seconds*1000);
    digitalWrite(pin, LOW);
}

void Pump::blink(){
    digitalWrite(pin, HIGH);
    delay(1000);
    digitalWrite(pin, LOW);
}

void Pump::open(){
    digitalWrite(pin, HIGH);
}

void Pump::close(){
    digitalWrite(pin, LOW);
}
