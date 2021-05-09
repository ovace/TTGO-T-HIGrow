/*
  @file espWebSvr.cpp


*/
#include <Arduino.h>
#include <string>

/* 
#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif 
*/

#if defined(ESP8266)  
  #include <ESP8266WebServer.h>  
#elif defined(ESP32)  
  #include <WebServer.h>
#else
  #error Invalid platform
#endif 

// #include <Ticker.h>

#include "index.html"  //include the index.html page
#include "config.h"
#include "getCfg.h"
#include "espWebSvr.h"

//Initialize Webserver
ESP8266WebServer server(wsport);

espSwitch myswitch;

espWebSrv::espWebSrv() { //Class constructor
};

espSwitch::espSwitch() { //Class constructor
};

void espWebSrv::setupWebSvr(const int port) {
  // espWebSrv::initWebSvr(port);
  server.begin();
  Serial.printf("starting HTTP Serer on port: %i, %i \n", wsport, port);
  espWebSrv::restRouter();
};

void espWebSrv::restRouter(){
   server.on("/", HTTP_GET, espWebSrv::handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  
  server.on("/Switch-ON", HTTP_GET, espSwitch::handleSwitchON); 
  
  server.on("/Switch-OFF", HTTP_GET, espSwitch::handleSwitchOFF);
    
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

};

void espWebSrv::initWebSvr(const int port) {
  // //Initialize Webserver
  // ::ESP8266WebServer server(port);
};

void espWebSrv::WSStatus() {

};

int espWebSrv::getWSPort() {
  Serial.printf("Ws Port: %i \n", config.websvrcfg.port );
  return config.websvrcfg.port;
};


void espWebSrv::handleRoot() {
  String s = String(index_html); //Read HTML contents
  server.send(200, "text/html", s); 
  Serial.println("Webpage request received");
  //server.send(200, "text/html", "get Temperature \n get humidity \n");
};

void espWebSrv::handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  };
  server.send(404, "text/plain", message);  
};

void espWebSrv::handleClient(){
  server.handleClient();
};

void espSwitch::handleSwitchOFF() {  
  bool SW_State = espSwitch::relayOpen();
  server.send(200, "text/plain", String(SW_State));
};

void espSwitch::handleSwitchON() {  
  bool SW_State = espSwitch::relayClose();
  server.send(200, "text/plain", String(SW_State));
    
};


bool espSwitch::relayClose() {
  Serial.write("\xa0\x01\x01\xa2"); // CLOSE RELAY 
  // switchdata.SW_State = 1;

  // sendMsg();

  return 1;
}

bool espSwitch::relayOpen() {
  Serial.write("\xa0\x01"); // OPEN RELAY
  Serial.write(0x00); // null terminates a string so it has to be sent on its own
  Serial.write(0xa1);

  // switchdata.SW_State = 0;

  // sendMsg();

  return 0;
}
