#include <Arduino.h>

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

// Debug Level from 0 to 4
#define _ETHERNET_WEBSERVER_LOGLEVEL_ 3
#include "myServer.h"
#include "mySetup.h"
#include "myMQTT.h"
#include "data/DataCare.h"
#include "MessageDispatcher.h"
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

  datacare.init();
  WEBsetup();
  mymqtt->init();

  mypoints.init();
  
  messagedispatcher.startTask("DispatcherTask", 2048, 1);
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
