//Trying to get wifi working
#include <WiFiNINA.h>               //include wifi library 
#include "secrets.h"

#include <Adafruit_SCD30.h> // Basic demo for readings from Adafruit SCD30
#include "ThingSpeak.h"     //include thingspeak library - always include thingspeak header file after other header files and custom macros



//----------------Fill in your Wi-Fi / ThingSpeak Credentials-------
const String ssid = SECRET_SSID;                                 //The name of the Wi-Fi network you are connecting to
const String pass = SECRET_PASS;                             //Your WiFi network password

const long myChannelNumber = SECRET_CH_ID;                            //Your Thingspeak channel number
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;                 //Your ThingSpeak Write API Key
//------------------------------------------------------------------

Adafruit_SCD30  scd30;

const unsigned long postingInterval = 20L * 1000L; //time, converted to seconds
String myStatus = "";

//Initialize Thingspeak values; global variables, device specific
int temp_C = scd30.temperature;
int RH = scd30.relative_humidity;
int CO2_ppm = scd30.CO2;

unsigned long lastUpdate = 0;
WiFiClient client;


void setup() {
  Serial.begin(115200);
  // while (!Serial) {
  //   delay(10);     // will pause Zero, Leonardo, etc until serial console opens
  // }

  // Try to initialize sensor!
  if (!scd30.begin()) {
    Serial.println("Failed to find SCD30 chip");
    while (1) { 
      delay(10); 
      }
  }
  Serial.println("SCD30 Found!");

 
 //Slow down the readings to reduce power usage
  if (!scd30.setMeasurementInterval(10)) {
    Serial.println("Failed to set measurement interval");
    while(1) { 
      delay(10); //10 = 10 seconds
    } 
  }
 
  Serial.print("Measurement interval: ");
  Serial.print(scd30.getMeasurementInterval());
  Serial.println(" seconds");

  /*** Restart continuous measurement with a pressure offset from 700 to 1400 millibar.
   * Giving no argument or setting the offset to 0 will disable offset correction
   */
  // if (!scd30.startContinuousMeasurement(15)){
  //   Serial.println("Failed to set ambient pressure offset");
  //   while(1){ delay(10);}
  // }
  Serial.print("Ambient pressure offset: ");
  Serial.print(scd30.getAmbientPressureOffset());
  Serial.println(" mBar");

  /*** Set an altitude offset in meters above sea level.
   * Offset value stored in non-volatile memory of SCD30.
   * Setting an altitude offset will override any pressure offset.
   */
  // if (!scd30.setAltitudeOffset(110)){
  //   Serial.println("Failed to set altitude offset");
  //   while(1){ delay(10);}
  // }
  Serial.print("Altitude offset: ");
  Serial.print(scd30.getAltitudeOffset());
  Serial.println(" meters");

  /*** Set a temperature offset in hundredths of a degree celcius.
   * Offset value stored in non-volatile memory of SCD30.
   */
  // if (!scd30.setTemperatureOffset(1984)){ // 19.84 degrees celcius
  //   Serial.println("Failed to set temperature offset");
  //   while(1){ delay(10);}
  // }
  Serial.print("Temperature offset: ");
  Serial.print((float)scd30.getTemperatureOffset()/100.0);
  Serial.println(" degrees C");

  /*** Force the sensor to recalibrate with the given reference value
   * from 400-2000 ppm. Writing a recalibration reference will overwrite
   * any previous self calibration values.
   * Reference value stored in non-volatile memory of SCD30.
   */
  // if (!scd30.forceRecalibrationWithReference(400)){
  //   Serial.println("Failed to force recalibration with reference");
  //   while(1) { delay(10); }
  // }
  Serial.print("Forced Recalibration reference: ");
  Serial.print(scd30.getForcedCalibrationReference());
  Serial.println(" ppm");

  /*** Enable or disable automatic self calibration (ASC).
   * Parameter stored in non-volatile memory of SCD30.
   * Enabling self calibration will override any previously set
   * forced calibration value.
   * ASC needs continuous operation with at least 1 hour
   * 400ppm CO2 concentration daily.
   */
  // if (!scd30.selfCalibrationEnabled(true)){
  //   Serial.println("Failed to enable or disable self calibration");
  //   while(1) { delay(10); }
  // }
  if (scd30.selfCalibrationEnabled()) {
    Serial.print("Self calibration enabled");
  } else {
    Serial.print("Self calibration disabled");
  }

  Serial.println("\n\n");
  myStatus = String("\n\n");
  ThingSpeak.setStatus(myStatus);


  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
      myStatus = String("Communication with WiFi module failed!"); 
      ThingSpeak.setStatus(myStatus);
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
    
  ThingSpeak.begin(client);  //Initialize ThingSpeak
}

void loop() {


 // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(SECRET_SSID); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000); 
    } 
    Serial.println("\nConnected.");
    myStatus = String("Wifi \nConnected."); 
      ThingSpeak.setStatus(myStatus);
  }

  // figure out the status message
  //if(number1 > number2){
    //myStatus = String(""); 
 // }
  //else if(number1 < number2){
    //myStatus = String("field1 is less than field2");
  //}
  //else{
    //myStatus = String("field1 equals field2");
  //}
  
  
  // set the status
  //ThingSpeak.setStatus(myStatus);


  if (scd30.dataReady()){
    Serial.println("Data available!");
    myStatus = String("SCD30 Data available");
    ThingSpeak.setStatus(myStatus);
    if (!scd30.read()) { 
      Serial.println("Error reading sensor data"); 
      return; 
    }

    Serial.print("Temperature: ");
    Serial.print(scd30.temperature);
    Serial.println(" ºC");
    temp_C = scd30.temperature;
    
    Serial.print("Relative Humidity: ");
    Serial.print(scd30.relative_humidity);
    Serial.println(" %");
    RH = scd30.relative_humidity;
    
    Serial.print("CO2: ");
    Serial.print(scd30.CO2, 3);
    Serial.println(" ppm");
    Serial.println("");
    CO2_ppm = scd30.CO2;

  }


    // set the fields with the values
    ThingSpeak.setField(1, temp_C);
    ThingSpeak.setField(2, RH);
    ThingSpeak.setField(3, CO2_ppm);
    
    //write to thingspeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(x == 200) {
      Serial.println("data sent to thingspeak");
      myStatus = String("All is well");
      ThingSpeak.setStatus(myStatus);

    } else{
      Serial.println("Problem updating channel. HTTP error code" + String(x));
      myStatus = String("Problem updating channel. HTTP error code" + String(x)); 
      ThingSpeak.setStatus(myStatus);
    }
  }