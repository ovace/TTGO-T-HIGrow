/*
  @file time-management.cpp


*/
#include <Arduino.h>
#include <NTPClient.h>

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

#include <time.h>

#include "config.h"
#include "getCfg.h"
extern espCFG mycfg;

#include "time-management.h"

/* Configuration of NTP */
#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   
const char* Timezone = "GMT0BST,M3.5.0/01,M10.5.0/02";       // UK

//Example time zones
//const char* Timezone = "GMT0BST,M3.5.0/01,M10.5.0/02";     // UK
//const char* Timezone = "MET-2METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
//const char* Timezone = "CET-1CEST,M3.5.0,M10.5.0/3";       // Central Europe
//const char* Timezone = "EST-2METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
//const char* Timezone = "EST5EDT,M3.2.0,M11.1.0";           // EST USA  
//const char* Timezone = "CST6CDT,M3.2.0,M11.1.0";           // CST USA
//const char* Timezone = "MST7MDT,M4.1.0,M10.5.0";           // MST USA
//const char* Timezone = "NZST-12NZDT,M9.5.0,M4.1.0/3";      // Auckland
//const char* Timezone = "EET-2EEST,M3.5.5/0,M10.5.5/0";     // Asia
//const char* Timezone = "ACST-9:30ACDT,M10.1.0,M4.1.0/3":   // Australia

String Date_str, Time_str, Time_format;


/* Globals */
time_t now;                         // this is the epoch
struct tm tminfo;                              // the structure tminfo holds time information in a more convient way



timeMgmt::timeMgmt() { //Class constructor
};
timeMgmt::~timeMgmt() { //Class destructor
};

void timeMgmt::setupespTimeMgmt(){
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  // setenv("TZ", Timezone, 1);
  Time_format = "M"; // or StartTime("I"); for Imperial 12:00 PM format and Date format MM-DD-CCYY e.g. 12:30PM 31-Mar-2019

};
void timeMgmt::getTimeMgmtCfg(){

};
void timeMgmt::getTime(){
  UpdateLocalTime(Time_format);
  Serial.println(Time_str);
  Serial.println();
};
void timeMgmt::getDate(){
  UpdateLocalTime(Time_format);
  Serial.println(Date_str);  
  Serial.println();

};
void timeMgmt::getTimeStamp(){

};

void timeMgmt::showTime() {
  // UpdateLocalTime(Time_format);
  time(&now);                       // read the current time
  localtime_r(&now, &tminfo);           // update the structure tminfo with the current time
  Serial.print("year:");
  Serial.print(tminfo.tm_year + 1900);  // years since 1900
  Serial.print("\tminfoonth:");
  Serial.print(tminfo.tm_mon + 1);      // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tminfo.tm_mday);         // day of month
  Serial.print("\thour:");
  Serial.print(tminfo.tm_hour);         // hours since midnight  0-23
  Serial.print("\tmin:");
  Serial.print(tminfo.tm_min);          // minutes after the hour  0-59
  Serial.print("\tsec:");
  Serial.print(tminfo.tm_sec);          // seconds after the minute  0-61*
  Serial.print("\twday");
  Serial.print(tminfo.tm_wday);         // days since Sunday 0-6
  if (tminfo.tm_isdst == 1)             // Daylight Saving Time flag
    Serial.print("\tDST");
  else
    Serial.print("\tstandard");
  Serial.println();
}

void timeMgmt::UpdateLocalTime(String Format){
  time_t now;
  time(&now);
  //See http://www.cplusplus.com/reference/ctime/strftime/
  char hour_output[30], day_output[30];
  if (Format == "M") {
    strftime(day_output, 30, "%a  %d-%m-%y", localtime(&now)); // Formats date as: Sat 24-Jun-17
    strftime(hour_output, 30, "%T", localtime(&now));    // Formats time as: 14:05:49
  }
  else {
    strftime(day_output, 30, "%a  %m-%d-%y", localtime(&now)); // Formats date as: Sat Jun-24-17
    strftime(hour_output, 30, "%r", localtime(&now));          // Formats time as: 2:05:49pm
  }
  Date_str = day_output;
  config.sensorcfg.date = Date_str;
  
  Time_str = hour_output;
  config.sensorcfg.time = Time_str;
}
