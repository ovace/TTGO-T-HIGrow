#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
// #include <NTPClient.h>
#include <time.h>
#include <SPIFFS.h>

#include "config.h"
#include "getCfg.h"
// #include "espOTA.h"
#include "user-variables.h"
#include "module-parameter-management.h"

const String fw_rel = "4.0.4"; // Added OTA feature

// Reboot counters
RTC_DATA_ATTR int bootCount = 0;

#define I2C_SDA             25
#define I2C_SCL             26
#define POWER_CTRL          4
#define USER_BUTTON         35
#define DHT_PIN             16

BH1750 lightMeter(0x23); //0x23
Adafruit_BME280 bmp;     //0x77 Adafruit_BME280 is technically not used, but if removed the BH1750 will not work - Any suggestions why, would be appriciated.

DHT dht(DHT_PIN, DHT_TYPE);


long int nowTime;
long int initTime;

bool bme_found = false;

// Start Subroutines
#include <file-management.h>
#include <go-to-deep-sleep.h>
#include <get-string-value.h>
#include <read-sensors.h>
#include <save-configuration.h>
#include <connect-to-network.h>
#include "time-management.h"

 
// espOTA myota;
espCFG mycfg;
MPM mympm;
timeMGMT mytimemgmt;


#define USE_SERIAL Serial

void setup() {

  // cfgBegin(); 
  mycfg.loadConfiguration();  

  int _espbaud = config.serialcomcfg.espbaud;
  if(_espbaud <= 0){ _espbaud = 115200;};

  Serial.begin(_espbaud);

  if (_DEBUG_) {
    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    };
    Serial.println("Void Setup");

    Serial.print("ntpServer: ");
    Serial.println(config.ntpcfg.Server);
    Serial.print("TZoffset: ");
    Serial.println(config.ntpcfg.timezone);
    Serial.print("ntpRefresh: ");
    Serial.println(config.ntpcfg.refrehFreq);
  };

  // WiFiUDP ntpUDP;
  const char*  ntpServer = config.ntpcfg.Server.c_str();
  const int gmtOffset_sec = 60*60*config.ntpcfg.timezone;
  const int daylightOffset_sec = 0;
  const int ntpRefresh = config.ntpcfg.refrehFreq;
  // NTPClient timeClient(ntpUDP, ntpServer, TZoffset, ntpRefresh);

  // record the time this device started
  // Init and get the time
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  time_t initTime = time(nullptr);
  
  mympm.setup();

  if (_DEBUG_) {
    mycfg.printCFG();
  };

  // Start WiFi and update time
  connectToNetwork();
  Serial.println(" ");
  Serial.println("Connected to network");
  if (logging) {
    writeFile(SPIFFS, "/error.log", "Connected to network \n");
  }

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  mytimemgmt.setClock();

  
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // checkOTAUpdates(); // check for new firmware available on server

  // if (doOTA) {
  //   Serial.println(F("Enabling OTA"));
  //   myota.OTA_push();
  // }

  // if (doOTApull ) {
  //   Serial.println(F("Enabling OTA pull"));
  //   myota.OTA_pull();
  // }  
  
  Wire.begin(I2C_SDA, I2C_SCL);
  if (logging) {
    writeFile(SPIFFS, "/error.log", "Wire Begin OK! \n");
  }

  dht.begin();
  if (logging) {
    writeFile(SPIFFS, "/error.log", "DHT12 Begin OK! \n");
  }

  //! Sensor power control pin , use deteced must set high
  pinMode(POWER_CTRL, OUTPUT);
  digitalWrite(POWER_CTRL, 1);
  delay(1000);

  if (!bmp.begin()) {
    Serial.println(F("This check must be done, otherwise the BH1750 does not initiate!!!!?????"));
    bme_found = false;
  } else {
    bme_found = true;
  }

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }

  float luxRead = lightMeter.readLightLevel();
  Serial.print("lux ");
  Serial.println(luxRead);
  delay(2000);
  float t12 = dht.readTemperature(true); // Read temperature as Fahrenheit then dht.readTemperature(true)
  sensordata.temp = t12;
  delay(2000);
  float h12 = dht.readHumidity();
  sensordata.humid = h12;
  uint16_t soil = readSoil();
  sensordata.soil = soil;
  uint32_t salt = readSalt();
  sensordata.salt = salt;
  String advice;
  if (salt < 201) {
    advice = "needed";
  }
  else if (salt < 251) {
    advice = "low";
  }
  else if (salt < 351) {
    advice = "optimal";
  }
  else if (salt > 350) {
    advice = "too high";
  }
  Serial.println (advice);
  sensordata.saltadvice = advice;

  float bat = readBattery();
  sensordata.bat = bat;
  sensordata.batcharge = "";
  if (bat > 130) {
    sensordata.batcharge = "charging";
  }

  if (bat > 100) {
    sensordata.bat = 100;
  }
  
  sensordata.bootno = bootCount;

  luxRead = lightMeter.readLightLevel(); 
  Serial.print("lux ");
  Serial.println(luxRead);
  sensordata.lux = luxRead;
  sensordata.rel = fw_rel;
  
  // Get current time
  time_t  nowTime = time(nullptr);
  if (_DEBUG_) {
    Serial.println(nowTime);
    Serial.println(initTime);
  };

  sensordata.uptime = difftime(nowTime, initTime);

  // mytimemgmt.setClock();

  sensordata.date = mytimemgmt.getCurDate();
  sensordata.time = mytimemgmt.getCurTimestamp();
  sensordata.TZ = config.ntpcfg.timezone;
  sensordata.DST = mytimemgmt.chkDST();

  // Create JSON file
  Serial.println(F("Creating JSON document..."));
  if (logging) {
    writeFile(SPIFFS, "/error.log", "Creating JSON document...! \n");
  }
  saveConfiguration(sensordata);

  // Go to sleep
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Go to sleep now
  delay(1000);
  goToDeepSleep();
}

void loop() {

  // if (doOTA){
  //   // check for OTA update
  //   myota.OTA_push_handle();
  // };

  yield();
}
