#ifndef _USER_VARS_H
#define  _USER_VARS_H
// *******************************************************************************************************************************
// START userdefined data
// *******************************************************************************************************************************
#include <Arduino.h>
#include "getCfg.h"
#include "config.h"

// Turn logging on/off - turn read logfile on/off, turn delete logfile on/off ---> default is false for all 3, otherwise it can cause battery drainage.
static const bool  logging = false;
static const bool  readLogfile = false;
static const bool  deleteLogfile = false;
static String readString; // do not change this variable

// Select DHT type on the module - supported are DHT11, DHT12, DHT22
#define DHT_TYPE DHT11
//#define DHT_TYPE DHT12
//#define DHT_TYPE DHT22

// It is a really good thing to calibrate each unit for soil, first note the number when unit is on the table, the soil number is for zero humidity. Then place the unit up to the electronics into a glass of water, the number now is the 100% humidity.
// By doing this you will get the same readout for each unit. Replace the value below for the dry condition, and the 100% humid condition, and you are done.

// Soil defaults - change them to your calibration data
static int soil_min = 1500;
static int soil_max = 3100;
static bool calibrate_soil = false;

// Salt/Fertilizer recommandation break points. You can change these according to your own calibration measurements.
static int fertil_needed = 200;
static int fertil_low = 201;
static int fertil_opt = 251;
static int fertil_high = 351;

// Give the sensor a plant name, change to true, upload sketch and then revert to false
static const bool update_plant_name = true;
static String plant_name = config.devicecfg.location;


// define your SSID's, and remember to fill out variable ssidArrNo with the number of your SSID's
static String ssidArr[] = {config.wirelesscfg.ssid.c_str() };
static int ssidArrNo = 1;

static const char* ssid = config.wirelesscfg.ssid.c_str(); // no need to fill in
static const char* password = config.wirelesscfg.psk;
static const char* ntpServer = config.ntpcfg.Server.c_str();
static const int   ntpRefresh = config.ntpcfg.refrehFreq;

// Off-sets for time, and summertime. each hour is 3.600 seconds.
// static const long  gmtOffset_sec = 3600;

// Device configuration and name setting
static const String device_name = config.devicecfg.name; // Can be changed, but not necessary, as it will give no added value.

#define uS_TO_S_FACTOR 1000000ULL //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  config.timerscfg.TIME_TO_SLEEP       //Time ESP32 will go to sleep (in seconds)

static const char* broker = config.mqttcfg.server.c_str();
static int port = config.mqttcfg.port;
static const char* mqttuser = config.mqttcfg.user.c_str(); //add eventual mqtt username
static const char* mqttpass = config.mqttcfg.pas.c_str(); //add eventual mqtt password

//Time Zone configuration for NTP
static int TZoffset = config.ntpcfg.timezone;

struct DSTconf {
  int DSTStartMonth = 3; //March
  int DSTStartWeek = 2; //Second week of March
  int DSTStartDay = 0;  //second Sunday of March - Sunday = 0
  int DSTStartHour = 2; // 2 AM 
  int DSTEndMonth = 11; //November
  int DSTEndWeek = 1; //First week of November
  int DSTEndDay = 0;  //First Sunday of March Sunday = 0
  int DSTEndHour = 2; // 2 AM 
};
static DSTconf dstconf;

// *******************************************************************************************************************************
// END userdefined data
// *******************************************************************************************************************************
#endif