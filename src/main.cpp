#include <Arduino.h>
#include "myServer.h"
#include "mySetup.h"
#include "myMQTT.h"
#include "data/DataCare.h"
#include "MessageDispatcher.h"
#include "points/myPoints.h"
#include "timer/myTimer.h"

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
  mytimer.start();
  delete mysetup;
  mysetup = nullptr;
}

void loop()
{
  // WiFiClient client = server.available();   // Listen for incoming clients
  mymqtt->loop();
  WEBloop();
}
