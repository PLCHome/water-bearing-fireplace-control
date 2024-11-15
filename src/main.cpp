#include <Arduino.h>

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_ 3
#include "myServer.h"
#include "myData.h"
#include "myPoints.h"

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

  DATAsetup();
  WEBsetup();
  mypoints.build();
}

void loop()
{
  // WiFiClient client = server.available();   // Listen for incoming clients
  mypoints.loop();
  DATAloop();
  WEBloop();
}
