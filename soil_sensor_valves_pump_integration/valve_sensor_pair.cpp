#include "valve_sensor_pair.h"

void Channel::setup(int sensor_addr, int valve_addr, char valve_num){
  this -> sensor_addr = sensor_addr;
  this -> valve_addr = valve_addr;
  this -> valve_num = valve_num;
  relay.begin(0x11);
  this -> sensor.begin(sensor_addr);
  //Serial.printf("Sensor Address: %d\nValve Address: %dValve number: %d\n", sensor_addr, valve_addr, valve_num);
}

void Channel::setupMux(I2CHub mux, char sensor_mux_port, char valve_mux_port) {
  this -> useMux = true;
  this -> mux = &mux;
  if(sensor_mux_port <= 7 && sensor_mux_port >= -1) {
    this -> sensor_mux_port = sensor_mux_port;
  }
  else {
    this -> sensor_mux_port = -1;
  }

  if(valve_mux_port <= 7 && valve_mux_port >= -1) {
    this -> valve_mux_port = valve_mux_port;
  }
  else {
    valve_mux_port = -1;
  }

  //Serial.printf("Sensor Mux Port: %d\nValve Mux Port: %d\n", sensor_mux_port, valve_mux_port);
}

void Channel::valveOpen(){
  if (useMux && valve_mux_port != -1) {
    mux->setPort(valve_mux_port);
  }
  relay.channelCtrl(0);
  relay.turn_on_channel(valve_num);
}

void Channel::valveClose(){
  if (useMux && valve_mux_port != -1) {
    mux->setPort(valve_mux_port);
  }
  relay.channelCtrl(0);
}

uint16_t Channel::readSensor(){
  /*if (useMux && sensor_mux_port != -1) {
    mux->setPort(sensor_mux_port);
  }*/

  return this->sensor.touchRead(0);
}
