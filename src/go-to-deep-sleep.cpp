/*
  @file go-to-deep-sleep.cpp


*/
#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>  
  #include <WiFiClient.h>
  #include <ESP8266mDNS.h>
  #include <LITTLEFS.h>
#elif defined(ESP32)
  #include <WiFi.h>  
  #include "SPIFFS.h"
#else
  #error Invalid platform
#endif 

#include "go-to-deep-sleep.h"


#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>
// #include "user-variables.h"

#include "file-management.h"
extern   espFileMgmt espfilemgmt; // defined in file included from src/main.cpp:92:0:

// Reboot counters
static RTC_DATA_ATTR int bootCount = 0;

static  RTC_DATA_ATTR int sleep5no = 0;

#include <getCfg.h>
#include <config.h>

// #include "user-variables.h"
#define uS_TO_S_FACTOR 1000000ULL //Conversion factor for micro seconds to seconds

go2Sleep::go2Sleep() { //Class constructor  
};
go2Sleep::~go2Sleep() { //Class destructor
};

void go2Sleep::setup2Sleep() {

};
void go2Sleep::get2SleepCfg() {

};
void go2Sleep::goToDeepSleep(int timeinsec)
{
  
  Serial.print("Going to sleep... ");
  Serial.print(timeinsec);
  Serial.println(" seconds");
  Serial.print(timeinsec*uS_TO_S_FACTOR);
  Serial.println(" uSec");
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Going to sleep for 3600 seconds \n");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();

  // // Configure the timer to wake us up!
  // esp_sleep_enable_timer_wakeup(config.timerscfg.TIME_TO_SLEEP * uS_TO_S_FACTOR);

  // // Go to sleep! Zzzz
  // esp_deep_sleep_start();
  esp_deep_sleep(timeinsec * uS_TO_S_FACTOR);
}
void go2Sleep::goToDeepSleepFiveMinutes()
{
  Serial.print("Going to sleep... ");
  Serial.print("300");
  Serial.println(" sekunder");
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Going to sleep for 300 seconds \n");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  btStop();

  adc_power_off();
  esp_wifi_stop();
  esp_bt_controller_disable();

  // Configure the timer to wake us up!
  ++sleep5no;
  esp_sleep_enable_timer_wakeup(300 * uS_TO_S_FACTOR);

  // Go to sleep! Zzzz
  esp_deep_sleep_start();
}
