#include <Arduino.h>
#include "MessageDispatcher.h"
#include "data/DataCare.h"
#include "myMQTT.h"
#include "myServer.h"
#include "mySetup.h"
#include "points/myPoints.h"
#include "timer/myTimer.h"
#define TASK_MONITOR
#ifdef TASK_MONITOR
#include <MycilaTaskMonitor.h>
#endif

void setup() {
  if (!SPIFFS.begin(true)) {
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

  messagedispatcher.startTask("DispatcherTask", 3096, 1);
  mytimer.start();
  delete mysetup;
  mysetup = nullptr;

#ifdef TASK_MONITOR
  Mycila::TaskMonitor.begin(5);
  Mycila::TaskMonitor.addTask("loopTask");
  Mycila::TaskMonitor.addTask("async_tcp");
  Mycila::TaskMonitor.addTask("Timer Task");
  Mycila::TaskMonitor.addTask("DATAloop");
  Mycila::TaskMonitor.addTask("DispatcherTask");
#endif
}

#ifdef TASK_MONITOR
unsigned long last = 0;
#endif
void loop() {
  // WiFiClient client = server.available();   // Listen for incoming clients
#ifdef TASK_MONITOR
  if (millis() - last > 5000) {
    last = millis();
    Mycila::TaskMonitor.log();
  }
#endif
  mymqtt->loop();
  WEBloop();
  delay(1);
}
