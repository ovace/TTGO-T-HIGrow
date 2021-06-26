/*
  @file read-sensors.cpp


*/
#include <Arduino.h>
#include "read-sensors.h"
#include "config.h"
#include "getCfg.h"

#define BAT_ADC             33
#define SALT_PIN            34
#define SOIL_PIN            32

// By doing this you will get the same readout for each unit. Replace the value below for the dry condition, and the 100% humid condition, and you are done.

// Soil defaults - change them to your calibration data
 static int soil_min = 1535;
 static int soil_max = 3300;
 static bool calibrate_soil = false;

// Salt/Fertilizer recommandation break points. You can change these according to your own calibration measurements.
 static int fertil_needed = 200;
 static int fertil_low = 201;
 static int fertil_opt = 251;
 static int fertil_high = 351;

// Give the sensor a plant name, change to true, upload sketch and then revert to false
 static const bool update_plant_name = false;
 static String plant_name = "Mogra";


espReadSensor::espReadSensor() { //Class constructor
};
espReadSensor::~espReadSensor() { //Class destructor
};

void espReadSensor::setupReadSensor(){

}; 
void espReadSensor::getReadSensorCfg(){
  
};

// READ Sensors

// READ Salt
// I am not quite sure how to read and use this number. I know that when put in water wich a DH value of 26, it gives a high number, but what it is and how to use ??????
uint32_t espReadSensor::readSalt()
{
  static uint8_t samples = 120;
  static uint32_t humi = 0;
  static uint16_t array[120];

  for (int i = 0; i < samples; i++) {
    array[i] = analogRead(SALT_PIN);
    //    Serial.print("Read salt pin : ");

    //    Serial.println(array[i]);
    delay(2);
  }
  std::sort(array, array + samples);
  for (int i = 0; i < samples; i++) {
    if (i == 0 || i == samples - 1)continue;
    humi += array[i];
  }
  humi /= samples - 2;
  return humi;
}

// READ Soil
uint16_t espReadSensor::readSoil()
{
  Serial.println(soil_max);
  uint16_t soil = analogRead(SOIL_PIN);
  Serial.print("Soil before map: ");
  Serial.println(soil);
  return map(soil, soil_min, soil_max, 100, 0);
}

// READ Battery
float espReadSensor::readBattery()
{
  static int vref = 1100;
  static uint16_t volt = analogRead(BAT_ADC);
  Serial.print("Volt direct ");
  Serial.println(volt);
  config.sensorcfg.batvolt = volt;
  
  float battery_voltage = ((float)volt / 4095.0) * 2.0 * 3.3 * (vref) / 1000;
  config.sensorcfg.batvoltage = battery_voltage;
  Serial.print("Battery Voltage: ");
  Serial.println(battery_voltage);
  battery_voltage = battery_voltage * 100;
  return map(battery_voltage, 416, 290, 100, 0);
}