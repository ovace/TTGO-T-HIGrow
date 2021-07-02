/*
  @file connect-to-network.cpp


*/
#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>  
  #include <WiFiClient.h>
  #include <ESP8266mDNS.h>
  #include <LITTLEFS.h>
#elif defined(ESP32)
  #include <WiFi.h>   
  #include <mdns.h>
  #include "SPIFFS.h"   
#else
  #error Invalid platform
#endif 

#include "config.h"
#include "getCfg.h"

#include "connect-to-network.h"
#include "go-to-deep-sleep.h"
#include "file-management.h"
// #include "user-variables.h"

extern espFileMgmt espfilemgmt;

extern go2Sleep go2sleep;

espNWconn::espNWconn() { //Class constructor
};
espNWconn::~espNWconn() { //Class destructor
};

void espNWconn::setupNWconn(){

}; 
void espNWconn::getNWconnCfg(){
  
};

void espNWconn::connectToNetwork() {
  Serial.print("Size of SSID ");
  Serial.println(config.wirelesscfg.ssid.c_str());
  // Serial.print("Size of PSK ");
  // Serial.println(config.wirelesscfg.psk);

  static String HOST_NAME = config.wirelesscfg.hostname;

  WiFi.mode(WIFI_STA);
  // WiFi.begin(config.wirelesscfg.ssid.c_str(), config.wirelesscfg.psk);
  const char* ssid = config.wirelesscfg.ssid.c_str(); // no need to fill in
  const char* password = config.wirelesscfg.psk;
  WiFi.begin(ssid, password);
  Serial.println("");
  bool breakLoop = false;
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Connecting to Network: \n");
  }
  /* 
  for (int i = 0; i < ssidArrNo; i++) {
    ssid = ssidArr[i].c_str();
    Serial.print("SSID name: ");
    Serial.print(ssidArr[i]);
  */
  {
    while ( WiFi.status() !=  WL_CONNECTED )
    {
      // wifi down, reconnect here
      WiFi.begin(ssid, password);
      int WLcount = 0;
      int UpCount = 0;
      while (WiFi.status() != WL_CONNECTED )
      {
        delay( 100 );
        Serial.printf(".");
        if (UpCount >= 60)  // just keep terminal from scrolling sideways
        {
          UpCount = 0;
          Serial.printf("\n");
        }
        ++UpCount;
        ++WLcount;
        if (WLcount > 200) {
          Serial.println("we should break");
          breakLoop = true;
          break;
        }
      }
      if (breakLoop) {
        breakLoop = false;
        break;
      }
    }
  }
  if (WiFi.status() !=  WL_CONNECTED) {
    go2sleep.goToDeepSleepFiveMinutes();
  }
#ifdef ESP32
  mdns_hostname_set(HOST_NAME.c_str());
#endif
  
}