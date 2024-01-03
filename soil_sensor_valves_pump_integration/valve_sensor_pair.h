#include "i2cHub.h"
#include "Adafruit_seesaw.h"
#include <multi_channel_relay.h>

class Channel {
  private:
    int sensor_addr;
    int valve_addr;
    char valve_num;
    bool useMux = false;
    char sensor_mux_port;
    char valve_mux_port;
    I2CHub *mux;
    Adafruit_seesaw sensor;
    Multi_Channel_Relay relay;


  public:
    void setup(int sensor_addr, int valve_addr, char valve_num);
    void setupMux(I2CHub mux, char sensor_mux_port, char valve_mux_port);
    void valveClose();
    void valveOpen();
    uint16_t readSensor();
};