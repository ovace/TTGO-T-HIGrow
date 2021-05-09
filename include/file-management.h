#ifndef _FILE_MGMT_H
#define  _FILE_MGMT_H

  #include <Arduino.h>
  #include "getCfg.h"  
  #include "FS.h"

  void writeFile(fs::FS & fs, const char * path, const char * message);

  void readFile(fs::FS & fs, const char * path);

  void listDir(fs::FS & fs, const char * dirname, uint8_t levels);

#endif
