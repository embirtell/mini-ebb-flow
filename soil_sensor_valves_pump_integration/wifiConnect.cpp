#include "wifiConnect.h"
#include<stdio.h>



//initiate wifi
void wifiConnect::initWifi() {
  int n = WiFi.scanNetworks();
  for(int i =0; i <n; i++){
    Serial.printf("Networks available: ", WiFi.SSID(i));
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }//while (WiFi.status() != WL_CONNECTED)
  Serial.println();
  Serial.println(WiFi.localIP());
} //void wifiConnect::initWifi

//connect or reconnect to wifi
void wifiConnect::wifiCheck() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } //while(WiFi.status() != WL_CONNECTED)
    Serial.println("\nConnected.");
  }//if(WiFi.status() != WL_CONNECTED)
}//void wifiConnect::wifiCheck()