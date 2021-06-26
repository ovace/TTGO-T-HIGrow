/*
  @file file-management.cpp


*/
#include <Arduino.h>

#if defined(ESP8266)
   #include <LITTLEFS.h>   
#elif defined(ESP32)
   #include "SPIFFS.h"   
#else
  #error Invalid platform
#endif 
#include "file-management.h"
#include "config.h"
#include "getCfg.h"

// static String readString;

// #include "user-variables.h"

espFileMgmt::espFileMgmt() { //Class constructor
};
espFileMgmt::~espFileMgmt() { //Class destructor
};

void espFileMgmt::setupespFileMgmt() {

};

void espFileMgmt::getFileMgmtCfg() {

};

void espFileMgmt::writeFile(fs::FS & fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
}

void espFileMgmt::readFile(fs::FS & fs, const char * path) {
  //  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  //  Serial.println("- read from file:");
  while (file.available()) {
    delay(2);  //delay to allow byte to arrive in input buffer
    char c = file.read();
    readString += c;
  }
  //  Serial.println(readString);
  file.close();
}

void espFileMgmt::listDir(fs::FS & fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        espFileMgmt::listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
