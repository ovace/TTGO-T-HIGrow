#include <Arduino.h>
#include "time.h"

#if defined(ESP8266)
  #include <ESP8266WiFi.h>  
  #include <LITTLEFS.h> 
#elif defined(ESP32)
  #include <WiFi.h>   
  #include "SPIFFS.h" 
#else
  #error Invalid platform
#endif

#include "config.h"
#include "time-management.h"
timeMgmt mytimemgmt;

/* Configuration of NTP */
// #define ntpServer1 config.ntpcfg.Server.c_str()
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";  

// const char* Timezone = config.ntpcfg.timeZone.c_str();       // UK
// #define Timezone config.ntpcfg.timeZone       // UK

//Example time zones
//const char* Timezone = "GMT0BST,M3.5.0/01,M10.5.0/02";     // UK
//const char* Timezone = "MET-2METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
//const char* Timezone = "CET-1CEST,M3.5.0,M10.5.0/3";       // Central Europe
//const char* Timezone = "EST-2METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
//const char* Timezone = "EST5EDT,M3.2.0,M11.1.0";           // EST USA  
const char* Timezone = "CST6CDT,M3.2.0,M11.1.0";           // CST USA
//const char* Timezone = "MST7MDT,M4.1.0,M10.5.0";           // MST USA
//const char* Timezone = "NZST-12NZDT,M9.5.0,M4.1.0/3";      // Auckland
//const char* Timezone = "EET-2EEST,M3.5.5/0,M10.5.5/0";     // Asia
//const char* Timezone = "ACST-9:30ACDT,M10.1.0,M4.1.0/3":   // Australia

String Date_str, Time_str;

// String Time_format = "M"; // or StartTime("I"); for Imperial 12:00 PM format and Date format MM-DD-CCYY e.g. 12:30PM 31-Mar-2019 

/* Globals */
time_t now;                         // this is the epoch
struct tm tminfo;                   // the structure tminfo holds time information in a more convient way

timeMgmt::timeMgmt() { //Class constructor  
  
};
timeMgmt::~timeMgmt() { //Class destructor
};

void timeMgmt::getTimeMgmtCfg(){

};

void timeMgmt::printTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void timeMgmt::setupNTP()
{
  Serial.println(ntpServer1);
  Serial.println(Timezone);

  //init and get the time
  configTime(0, 0, ntpServer1, ntpServer2); // UTC 
  mytimemgmt.updateTime(Timezone);
  printTime();
};

tmStruct timeMgmt::updateTime(const char* tz){

  #if defined(ESP8266)
    setTZ(tz);
  #elif defined(ESP32)
    setenv("TZ", tz, 1);
    tzset(); // Assign the local timezone from setenv
  #endif

  tmStruct s;
  time_t now;
  time(&now);

  int WLcount = 0;
  bool breakLoop = false;
  
  #if defined(ESP8266)
    //   Serial.println("Failed to obtain time");
    //   return false;
    // }
#elif defined(ESP32)
  while (!getLocalTime(&tminfo, 5000)) { // Wait for 5-sec for time to synchronise

    Serial.println("Failed to obtain time");
    s.curTime = now;
    s.status = false;

    ++WLcount;
    if (WLcount > 5) {
      Serial.println("we should break");
      breakLoop = true;
      break;
    }
  }  
#endif
   
  return s;
};

void timeMgmt::showTime()
{
  mytimemgmt.setupNTP();
  delay(1000);
  mytimemgmt.printTime();
};

String timeMgmt::getUTCtime(String Format) {
  now = mytimemgmt.updateTime("UTC0").curTime;
  char   time_output[30], update_time[30];

  // int CurrentHour = tminfo.tm_hour;
  // int CurrentMin  = tminfo.tm_min;
  // int CurrentSec  = tminfo.tm_sec;
  //See http://www.cplusplus.com/reference/ctime/strftime/
  
  strftime(update_time, sizeof(update_time), "%r", &tminfo);        // Creates: '02:05:49pm'
  sprintf(time_output, "%s", update_time);
    
  Time_str = time_output;
  // config.sensorcfg.time = Time_str;

  Serial.println(Time_str);  
  Serial.println();
  return Time_str;  
};

String timeMgmt::getUTCdate(String Format){
  now = mytimemgmt.updateTime("UTC0").curTime;

  //See http://www.cplusplus.com/reference/ctime/strftime/
  char day_output[30];
  
  strftime(day_output, sizeof(day_output), "%a %b-%d-%Y", &tminfo); // Creates  'Sat May-31-2019'

  Date_str = day_output;
  // config.sensorcfg.date = Date_str;

  Serial.println(Date_str);  

  return Date_str;
};

String timeMgmt::getLocalTzTime(String Format){
  now = mytimemgmt.updateTime(Timezone).curTime;
  char   time_output[30], update_time[30];

  //See http://www.cplusplus.com/reference/ctime/strftime/
  
  strftime(update_time, sizeof(update_time), "%r", &tminfo);        // Creates: '02:05:49pm'
  sprintf(time_output, "%s", update_time);
    
  Time_str = time_output;
  
  Serial.println(Time_str);  
  return Time_str;  
};

String timeMgmt::getLocalTzDate(String Format){
now = mytimemgmt.updateTime(Timezone).curTime;

  //See http://www.cplusplus.com/reference/ctime/strftime/
  char day_output[30];
  
  strftime(day_output, sizeof(day_output), "%a %b-%d-%Y", &tminfo); // Creates  'Sat May-31-2019'

  Date_str = day_output;
  // config.sensorcfg.date = Date_str;

  Serial.println(Date_str);  
  Serial.println();

  return Date_str;
};
unsigned long timeMgmt::getEpochTime() {
  now = mytimemgmt.updateTime(Timezone).curTime;  
  Serial.println("Epoch Time:" + now);
  return now;
};

String timeMgmt::getTimeStamp(){
  now = mytimemgmt.updateTime("UTC0").curTime;
  Serial.println(Time_str);  
  Serial.println();
  return Time_str;

};

void timeMgmt::showTimeComponents() {
  // read the current time
  now = mytimemgmt.updateTime(Timezone).curTime;  
  
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


