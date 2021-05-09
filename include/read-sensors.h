#ifndef _READ_SENSOR_H
#define _READ_SENSOR_H

  #include <Arduino.h>
  #include "getCfg.h"
  #include "user-variables.h"

  //json construct setup
struct SensorData {
  String date;
  String time;
  int TZ = TZoffset;
  bool DST;
  long int uptime;
  int bootno;
  int sleep5no;
  float lux;
  float temp;
  float humid;
  float soil;
  float salt;
  String saltadvice;
  float bat;
  String batcharge;
  float batvolt;
  float batvoltage;
  String rel;
};
extern SensorData sensordata;

  // READ Salt
  uint32_t readSalt();

  // READ Soil
  uint16_t readSoil();

  // READ Battery
  float readBattery();

#endif