#include "valve_sensor_pair.h"

void Channel::setup(int sensor_addr, int valve_addr, char valve_num, int numChannels, int sampleCount, Pump pump){
  this -> sensor_addr = sensor_addr;
  this -> valve_addr = valve_addr;
  this -> valve_num = valve_num;
  this -> numChannels = numChannels;
  this -> sampleCount = sampleCount;
  this -> pump = &pump;
  this -> valveStatus = false;

  relay.begin(0x11);

  //Testing if sensor is working
  if (!this -> sensor.begin(sensor_addr)) { //TODO why prints only finding the last channel
    		Serial.println("ERROR! seesaw not found");
    		//while(1) delay(1);
  	} else {
    		Serial.print("seesaw started! version: ");
    		Serial.println(this -> sensor.getVersion(), HEX);
  	}//if else sensor.begin

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
  delay(10);
  relay.turn_on_channel(valve_num);
  valveStatus = true;
}

void Channel::valveClose(){
  if (useMux && valve_mux_port != -1) {
    mux->setPort(valve_mux_port);
  }
  relay.channelCtrl(0);
  valveStatus = false;
}

bool Channel::getValveStatus(){
  return valveStatus;
}

uint16_t Channel::readSensor(int sampleNum){
  /*This was commented
  if (useMux && sensor_mux_port != -1) {
    mux->setPort(sensor_mux_port);
  }*/

  uint16_t capread = this->sensor.touchRead(0); //Reading the sensor
  if(capread < 0 || capread > 2000){ 
  		Serial.println("Sensor is not working!");
      this->sampleArrays[sampleNum] = -1;
      return -1;
  	}else{
  		///ThingSpeak.setField(i+1, int(capread));
  		Serial.print("Reading: ");
  		Serial.println(capread);
      this->sampleArrays[sampleNum] = capread;
      return capread;
  	}
}


float Channel::averageSamplesAndPublish(int channelNum){
  
  String myStatus = "";
  
  int summation = 0;
  int sampleRangeErr = 0;
  for(int i = 0; i < this->sampleCount; i++) {
    if(this->sampleArrays[i] < 0){// if data invalid, count sample as error
      sampleRangeErr++;
    } else {
      summation += this->sampleArrays[i];
      }//else
  }//for(int i = 0; i < sampleCount; i++){
  

  
  float avgSample = summation/(sampleCount - sampleRangeErr);

  ThingSpeak.setField(channelNum+1, avgSample);

  Serial.print("Average Sample: ");
  //Serial.print(channelNum+1);
  //Serial.print(" ");
  //Serial.print(" sampleRangeErr: ");
  //Serial.println(sampleRangeErr);
  Serial.println(avgSample);

  if(sampleRangeErr > 0){
    Serial.println("updating status with error");
    myStatus += String("Sensor ");
    myStatus += String(channelNum+1);
    myStatus += String(": ");
    myStatus += String(sampleRangeErr);
    myStatus += String(" readings out of range"); // \n is newline character
    ThingSpeak.setStatus(myStatus);
  }//if(sampleRangleErr > 0){
  return avgSample;
}//void averageSamples()
