#include "WiFi.h"
#include "secrets.h"

class wifiConnect{
  private:
    char ssid[20] = SECRET_SSID;   // your network SSID (name) 
    char pass[20] = SECRET_PASS;   // your network password

  public:
    void initWifi();
    void wifiCheck();

};