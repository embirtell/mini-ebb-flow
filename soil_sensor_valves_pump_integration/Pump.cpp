#include "Pump.h"
#include <Arduino.h>

void Pump::setup(char pin){
    this->pin = pin;
    pinMode(pin, OUTPUT);
}

void Pump::blink(int ms){
    digitalWrite(pin, HIGH);
    delay(ms);
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
