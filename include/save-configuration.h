#ifndef _SAVE_CONFIG_H
#define _SAVE_CONFIG_H

  #include <Arduino.h>
  #include "getCfg.h"
  #include "read-sensors.h"

  // Allocate a  JsonDocument
  void saveConfiguration(const SensorData & sensordata);

#endif

