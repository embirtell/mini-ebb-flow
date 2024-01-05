#include "i2cHub.h"
#include "Adafruit_seesaw.h"
#include <multi_channel_relay.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

class Channel {
  private:
    int sensor_addr;
    int valve_addr;
    char valve_num;
    bool useMux = false;
    char sensor_mux_port;
    char valve_mux_port;
    int numChannels;
    int sampleCount;

    I2CHub *mux;
    Adafruit_seesaw sensor;
    Multi_Channel_Relay relay;


  public:
    int16_t sampleArrays[30]; //holds data for averaging (30 samples)
    void setup(int sensor_addr, int valve_addr, char valve_num, int numChannels, int sampleCount);
    void setupMux(I2CHub mux, char sensor_mux_port, char valve_mux_port);
    void valveClose();
    void valveOpen();
    uint16_t readSensor(int SampleNum);
    float averageSamplesAndPublish(int channelNum);
};