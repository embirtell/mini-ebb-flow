#include "i2cHub.h"

class Channel {
  private:
    int sensor_addr;
    int valve_addr;
    char valve_num;
    bool useMux = false;
    char sensor_mux_port;
    char valve_mux_port;
    I2CHub *mux;

  public:
    Channel(int sensor_addr, int valve_addr, char valve_num);
    void setupMux(I2CHub mux, char sensor_mux_port, char valve_mux_port);
    void valveClose();
    void valveOpen();
    void readSensor();
};