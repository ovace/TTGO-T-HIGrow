  
#include "time-management.h"
#include "user-variables.h"
#include "save-configuration.h"

#include <Arduino.h>
#include <time.h>
#include "config.h"
#include "getCfg.h"

String formattedDate;
String dayStamp;
String timeStamp1;

const int gmtOffset = 0;
const long int gmtOffset_sec = 60*60*gmtOffset;
const char* ntpSvr = config.ntpcfg.Server.c_str();
int ntpRefreshFreq = config.ntpcfg.refrehFreq;
int dstOffset = 0;
int dstOffset_sec = 60*60*dstOffset;

timeMGMT::timeMGMT() { //Class constructor
};

timeMGMT::~timeMGMT() { //Class destructor
};

void timeMGMT::setup() {
  
};

bool timeMGMT::chkDST() {
  // configTime(gmtOffset_sec, dstOffset_sec, config.ntpcfg.Server.c_str(), "time.nist.gov");  // CT  

  time_t now = time(nullptr);
  struct tm* timeinfo;

  timeinfo = localtime(&now);

  
  // variables needed for DST test
  int thisHour = ((timeinfo->tm_hour)+(gmtOffset)+(dstOffset))%24; //returns an int number with the current hour (0 to 23) in 24 hour format;
  int thisDay = timeinfo->tm_mday;
  int thisMonth = timeinfo->tm_mon;
  int thisWeekday = timeinfo->tm_wday; //returns an int number that corresponds to the the week day (0 to 6) starting on Sunday;
  bool dst = timeinfo->tm_isdst;

  if (_DEBUG_) {
    Serial.print(F("DST Status: "));
    Serial.println(dst);
  };

  

  // Test for DST active //2 a.m.local time on the second Sunday in March to 2 a.m. on the First Sunday of November.
  if (thisMonth == dstconf.DSTStartMonth) {   // is it March
    if (thisDay > 7 && thisDay > 14  &&      // in second week
        thisWeekday == dstconf.DSTStartDay && // Sunday 
        thisHour >= dstconf.DSTStartHour ) {   // after 2 AM   
      dst = true;
    }
    if (thisDay > 7  &&     // after first week
        thisWeekday >= dstconf.DSTStartDay ) { //  past Sunday 
      dst = true;
    };
  };

  if (thisMonth > dstconf.DSTStartMonth && thisMonth < dstconf.DSTEndMonth)  { // If date falls between March and Nov
    dst = true;
  }

  if (thisMonth == dstconf.DSTEndMonth &&  //If its first  Sunday of November and before 2 AM
  //<<to-do>> first week of Nov before Sunday
      thisDay < 7  && 
      thisWeekday == dstconf.DSTEndDay  && 
      thisHour <= dstconf.DSTEndHour )  
      { 
    dst = true;
  }

  // if (thisMonth < 10 && thisMonth > 3) {
  //   dst = true;
  // }

  // if (thisMonth == 3) {
  //   dst = true;
  //   if (thisDay < 25) {
  //     dst = false;
  //   }
  //   else
  //     // thisDay > 25
  //   {
  //     if (thisWeekday == 7 && thisHour < 2)      {
  //       dst = false;
  //     }
  //     else {
  //       if (thisWeekday == 7) {
  //         dst = true;
  //       }
  //       else {
  //         if (thisWeekday < 7) {
  //           int checkSum = thisDay - thisWeekday + 7;
  //           if (checkSum > 31) {
  //             dst = true;
  //           }
  //           else {
  //             dst = false;
  //           }
  //         }
  //       }
  //     }
  //   }
  // }
  sensordata.DST = dst;
  if (dst) {
    Serial.println("IN SUMMERTIME");   
  } else {
    Serial.println("IN WINTERTIME");
  }

  return dst;
}

String timeMGMT::getCurTimestamp () {
  
  if (_DEBUG_) {
    Serial.println(gmtOffset);
  };

  // if (timeMGMT::chkDST() ) {
  //   dstOffset = 60*60;    
  // } else {
  //   dstOffset = 0;
  // };

  configTime(gmtOffset_sec, dstOffset_sec, config.ntpcfg.Server.c_str(), "time.nist.gov");  // CT  

  time_t now = time(nullptr);
  struct tm* timeinfo;

  timeinfo = localtime(&now);

  int currentHour = (timeinfo->tm_hour)%24;  
  int currentMinute = timeinfo ->tm_min;     
  int currentSecond = timeinfo ->tm_sec;
  
  String formattedTime = String(currentHour) + ":" + String(currentMinute) +  ":" + String(currentSecond);

  if (_DEBUG_) {
    Serial.print(F("Formatted Time: "));
    Serial.println(formattedTime);
  };

  return formattedTime;

  }

String timeMGMT::getCurDate () {
    
  if (_DEBUG_) {
    Serial.println(gmtOffset);
  };

  // if (timeMGMT::chkDST() ) {
  //   dstOffset = -1;    
  // } else {
  //   dstOffset = 0;
  // };

  // configTime(gmtOffset_sec, dstOffset_sec, config.ntpcfg.Server.c_str(), "time.nist.gov");  // CT  

  time_t now = time(nullptr);
  struct tm* timeinfo;

  timeinfo = localtime(&now);

  int currDay = timeinfo ->tm_mday;  
  int currMonth = timeinfo ->tm_mon;     
  int currYear = (timeinfo ->tm_year) + 1900;
  
  String formattedDate = String(currDay) + "-" + String(currMonth) +  "-" + String(currYear);
  
  if (_DEBUG_) {
    Serial.print(F("Formatted Date: "));
    Serial.println(formattedDate);
  };
  

  // //Get a time structure
  // struct tm *ptm = gmtime ((time_t *)&epochTime); 

  // int monthDay = ptm->tm_mday;
  
  // int currentMonth = ptm->tm_mon+1;
  
  // int currentYear = ptm->tm_year+1900;
  

  // //Print complete date:
  // String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  
  return formattedDate;
}

// Set time via NTP, as required for x.509 validation
void timeMGMT::setClock() {

  if (_DEBUG_) {
    Serial.print(F("NTP Server: "));
    Serial.println(config.ntpcfg.Server.c_str());
  };
  
  configTime(gmtOffset_sec, dstOffset_sec, config.ntpcfg.Server.c_str(), "time.nist.gov");  // CT  

  Serial.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  if (_DEBUG_) {
    Serial.print(F("Now: "));
    Serial.println(now);
  };
  // while (now < 8 * 3600 * 2) {
  //   yield();
  //   delay(500);
  //   Serial.print(F("."));
  //   now = time(nullptr);
  // }

  Serial.println(F(""));
  struct tm * timeinfo;
  char buffer [80];

  timeinfo = localtime(&now);

  strftime (buffer,80,"Now it's %d-%m-%Y  %I:%M:%S %p TZ:%z.",timeinfo);
  Serial.println(buffer);
  
  return;  

  // Serial.print(F("Waiting for NTP time sync: "));
  // time_t now = time(nullptr);
  // while (now < 8 * 3600 * 2) {
  //   yield();
  //   delay(500);
  //   Serial.print(F("."));
  //   now = time(nullptr);
  // }

  // Serial.println(F(""));
  // struct tm timeinfo;

  // gmtime_r(&now, &timeinfo);
  // Serial.print(F("Current time: "));
  // Serial.print(asctime(&timeinfo));
}
