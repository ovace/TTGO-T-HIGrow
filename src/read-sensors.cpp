#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <Adafruit_BME280.h>


#include "driver/adc.h"
#include "getCfg.h"
#include <esp_wifi.h>
#include <esp_bt.h>



#include "read-sensors.h"

//const int led = 13;  Red indicator LED -- disable to save battery power


#define BAT_ADC             33
#define SALT_PIN            34
#define SOIL_PIN            32
#define BOOT_PIN            0


// READ Sensors

SensorData sensordata;

// READ Salt
// I am not quite sure how to read and use this number. I know that when put in water wich a DH value of 26, it gives a high number, but what it is and how to use ??????
uint32_t readSalt()
{
  uint8_t samples = 120;
  uint32_t humi = 0;
  uint16_t array[120];

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
uint16_t readSoil()
{
  Serial.println(soil_max);
  uint16_t soil = analogRead(SOIL_PIN);
  Serial.print("Soil before map: ");
  Serial.println(soil);
  return map(soil, soil_min, soil_max, 100, 0);
}

// READ Battery
float readBattery()
{
  int vref = 1100;
  uint16_t volt = analogRead(BAT_ADC);
  Serial.print("Volt direct ");
  Serial.println(volt);
  sensordata.batvolt = volt;
  float battery_voltage = ((float)volt / 4095.0) * 2.0 * 3.3 * (vref) / 1000;
  sensordata.batvoltage = battery_voltage;
  Serial.print("Battery Voltage: ");
  Serial.println(battery_voltage);
  battery_voltage = battery_voltage * 100;
  return map(battery_voltage, 416, 290, 100, 0);
}