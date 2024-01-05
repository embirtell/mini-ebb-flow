#include "WiFi.h"
#include "secrets.h"


char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password

void initWifi();
void wifiCheck();