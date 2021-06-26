#include "SPIFFS.h"
#include "get-string-value.h"
espSrtingVal getstringvalue;

#include "file-management.h"
  espFileMgmt espfilemgmt;
  
  
  // Initiate SPIFFS and Mount file system
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    if (logging) {
      espfilemgmt.writeFile(SPIFFS, "/error.log", "An Error has occurred while mounting SPIFFS \n");
    }
    return;
  }
  
   if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Void Setup \n");
  } 
  

  espfilemgmt.listDir(SPIFFS, "/", 0);

 
 if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "After listDir \n");
  } 
  

  if (readLogfile) {
    // Now we start reading the files..
    espfilemgmt.readFile(SPIFFS, "/error.log");
    Serial.println("Here comes the logging info:");
    Serial.println(readString);
  }

  if (deleteLogfile) {
    SPIFFS.remove("/error.log");
  }

  /*OAM  
  // Calibrate Soil figures save to file, if calibrate_soil == true
   if (calibrate_soil) {
    SPIFFS.remove("/soil.conf");
    String soil_write_str = String(soil_min) + ":" + String(soil_max);
    const char* soil_write = soil_write_str.c_str();
    espfilemgmt.writeFile(SPIFFS, "/soil.conf", soil_write);
  } else {
    espfilemgmt.readFile(SPIFFS, "/soil.conf");
    Serial.println("Here comes the calibration info:");
    Serial.println(readString);
    String xval = getstringvalue.getValue(readString, ':', 0);
    String yval = getstringvalue.getValue(readString, ':', 1);

    soil_min = xval.toInt();
    soil_max = yval.toInt();
    readString = "";
  }

  if (update_plant_name) {
    SPIFFS.remove("/name.conf");
    String name_write_str = plant_name;
    const char* name_write = name_write_str.c_str();
    espfilemgmt.writeFile(SPIFFS, "/name.conf", name_write);
  } else {
    espfilemgmt.readFile(SPIFFS, "/name.conf");
    Serial.println("Here comes the name info:");
    Serial.println(readString);
    plant_name = readString;
    readString = "";
  }  
  */
  

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  
  if (logging) {
    espfilemgmt.writeFile(SPIFFS, "/error.log", "Before Start WIFI \n");
  } 
