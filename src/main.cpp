#include <Arduino.h>

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_ 3
#include "myServer.h"
#include "mySetup.h"
#include "myData.h"
#include "myMQTT.h"
#include "points/myPoints.h"

void setup()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.begin(115200);
  while (!Serial)
    ;

  mysetup = new mySetup();

  DATAsetup();
  WEBsetup();
  mymqtt->init();
  mypoints.build();
  
  delete mysetup;
  mysetup = nullptr;
}

void loop()
{
  // WiFiClient client = server.available();   // Listen for incoming clients
  mymqtt->loop();
  mypoints.loop();
  //DATAloop();
  WEBloop();
}
