//V1.26
//History:
// added OTA via http server
// added debug mode for serial monitoring
#include <Arduino.h>

#ifdef ESP32
    #pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
    // #error message(Select ESP32 board)
    #include <WiFi.h>
    // #include <WiFiMulti.h>    
    #include <HTTPClient.h>
    #include <HTTPUpdate.h>
#elif defined(ESP8266)
    #pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
    //#error Select ESP8266 board
    #include <ESP8266HTTPClient.h>
    #include <ESP8266httpUpdate.h>

    /**
     * ESP8266 Pin Connections
     * 
     * GPIO0/SPI-CS2/_FLASH_ -                           Pull low for Flash download by UART
     * GPIO1/TXD0/SPI-CS1 -                              _LED_
     * GPIO2/TXD1/I2C-SDA/I2SO-WS -
     * GPIO3/RXD0/I2SO-DATA - 
     * GPIO4 -
     * GPIO5/IR-Rx -
     * GPIO6/SPI-CLK -                                  Flash CLK
     * GPIO7/SPI-MISO -                                 Flash DI
     * GPIO8/SPI-MOSI/RXD1 -                            Flash DO
     * GPIO9/SPI-HD -                                   Flash _HD_ 
     * GPIO10/SPI-WP -                                  Flash _WP_
     * GPIO11/SPI-CS0 -                                 Flash _CS_
     * GPIO12/MTDI/HSPI-MISO/I2SI-DATA/IR-Tx -
     * GPIO13/MTCK/CTS0/RXD2/HSPI-MOSI/I2S-BCK -
     * GPIO14/MTMS/HSPI-CLK/I2C-SCL/I2SI_WS -
     * GPIO15/MTDO/RTS0/TXD2/HSPI-CS/SD-BOOT/I2SO-BCK - Pull low for Flash boot
     * GPIO16/WAKE -
     * ADC -
     * EN -
     * RST -
     * GND -
     * VCC -
     */

#else
  #error Invalid platform
#endif //ESP version check

#define HTTP_OTA                      // Enable OTA updates from http server

#ifdef HTTP_OTA
  /* Over The Air automatic firmware update from a web server.  ESP8266 will contact the
   *  server on every boot and check for a firmware update.  If available, the update will
   *  be downloaded and installed.  Server can determine the appropriate firmware for this 
   *  device from any combination of HTTP_OTA_VERSION, MAC address, and firmware MD5 checksums.
   */
  
  #define HTTP_OTA_ADDRESS      F("HOMEMGMT")   // Address of OTA update server
  #define HTTP_OTA_PATH         F("/OTA/Update/") // Path to update firmware
  #define HTTP_OTA_PORT         1880                     // Port of update server
                                                         // Name of firmware
  #define HTTP_OTA_VERSION      String(__FILE__).substring(String(__FILE__).lastIndexOf('/')+1) + ".generic" 
#endif

//#define DEBUGMODE								  //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUGMODE //Macros are usually in all capital letters.
#define DPRINT(...) Serial.print(__VA_ARGS__) //DPRINT is a macro, debug print
#define DPRINTLN(...) Serial.println(__VA_ARGS__) //DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...) //now defines a blank line
#define DPRINTLN(...) //now defines a blank line
#endif

const char* fwBaseURL = "http://HOMEMGMT/fota/TTGO/"; // update with your link to the new firmware bin file.
const char* fwImageURL = "http://HOMEMGMT/fota/TTGO/firmware.bin"; // update with your link to the new firmware bin file.
const char* fwVersionURL = "http://HOMEMGMT/fota/TTGO/firmware.version"; // update with your link to a text file with new version (just a single line with a number)
// version is used to do OTA only one time, even if you let the firmware file available on the server.
// flashing will occur only if a greater number is available in the "firmware.version" text file.
// take care the number in text file is compared to "FW_VERSION" in the code => this const shall be incremented at each update.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  check_OTA() : check for some available new firmware on server?
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String getMAC()
{
  uint8_t mac[6];
  char result[14];

 snprintf( result, sizeof( result ), "%02x%02x%02x%02x%02x%02x", mac[ 0 ], mac[ 1 ], mac[ 2 ], mac[ 3 ], mac[ 4 ], mac[ 5 ] );

  return String( result );
} // end getMac()

void checkOTAUpdates() {

  String mac = getMAC();
  String fwURL = String( fwBaseURL );
  fwURL.concat( mac );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );

  Serial.println( "Checking for firmware updates." );
  Serial.print( "MAC address: " );
  Serial.println( mac );
  Serial.print( "Firmware version URL: " );
  Serial.println( fwVersionURL );

#ifdef ESP8266
  HTTPClient httpClient;
  httpClient.begin( fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware version: " );
    Serial.println( FW_VERSION );
    Serial.print( "Available firmware version: " );
    Serial.println( newFWVersion );

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      Serial.println( "Preparing to update" );

      String fwImageURL = fwURL;
      fwImageURL.concat( ".bin" );
      t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
      }
    }
    else {
      Serial.println( "Already on latest version" );
    }
  }
  else {
    Serial.print( "Firmware version check failed, got HTTP response code " );
    Serial.println( httpCode );
  }
  httpClient.end();

#elif defined(ESP32)

  Serial.println("Update SPIFFS...");

  WiFiClient client; 

  t_httpUpdate_return ret = httpUpdate.updateSpiffs(client, "http://server/spiffs.bin");
  if (ret == HTTP_UPDATE_OK) {
      Serial.println("Update sketch...");
      ret = httpUpdate.update(client, "http://server/file.bin");

      switch (ret) {
          case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
          break;

          case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;

          case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      }
  }
#endif //CheckOTA Update for ESP8266
}

// void check_OTA()
// {
//     // setup_wifi(); must be called before check_OTA();
//     DPRINT("Firmware:<");
//     DPRINT(FW_VERSION);
//     DPRINTLN(">");
//     DPRINTLN("Check for OTA");

//     EthernetClient hc;
//     HttpClient http(hc);

//     if (http.begin(client, fwVersionURL)) {
//         DPRINTLN("http begin");
//         int httpCode = http.GET();
//         DPRINT("httpCode:");
//         DPRINTLN(httpCode);
//         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//             String newFWVersion = http.getString();
//             DPRINT("Server  FWVersion: ");
//             DPRINT(newFWVersion);
//             DPRINT(" / ");
//             DPRINT("Current FWVersion: ");
//             DPRINTLN(FW_VERSION);
//             float newVersion = newFWVersion.toFloat();
//             if (newVersion > FW_VERSION) {
//                 DPRINTLN("start OTA !");
//                 http.end();
//                 delay(100);
//                 t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://HOMEMGMT/fota/TTGO/firmware.bin");
//                 switch (ret) {
//                 case HTTP_UPDATE_FAILED:
//                     DPRINT("HTTP_UPDATE_FAILD Error");
//                     DPRINT(ESPhttpUpdate.getLastError());
//                     DPRINT(": ");
//                     DPRINTLN(ESPhttpUpdate.getLastErrorString().c_str());
//                     //USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
//                     break;
//                 case HTTP_UPDATE_NO_UPDATES:
//                     DPRINTLN("HTTP_UPDATE_NO_UPDATES");
//                     break;
//                 case HTTP_UPDATE_OK:
//                     DPRINTLN("HTTP_UPDATE_OK");
//                     break;
//                 }
//             } else {
//                 DPRINTLN("no new version available");
//             }
//         }
//     } // end if http.begin
//     DPRINTLN("End of OTA");
// } // end check_OTA()