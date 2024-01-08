#include "Pump.h"
#include <Arduino.h>

void Pump::setup(char pin){
    this->pin = pin;
    pinMode(pin, OUTPUT);
    this -> running = false;
}

bool Pump::getRunning(){
  return this -> running;
}

void Pump::blink(int ms){
    running = true;
    digitalWrite(pin, HIGH);
    delay(ms);
    digitalWrite(pin, LOW);
    running = false;
}

void Pump::blink(){
    running = true;
    digitalWrite(pin, HIGH);
    delay(1000);
    digitalWrite(pin, LOW);
    running = false;
}

void Pump::open(){
    running = true;
    digitalWrite(pin, HIGH);
}

void Pump::close(){
    digitalWrite(pin, LOW);
    running = false;
}
