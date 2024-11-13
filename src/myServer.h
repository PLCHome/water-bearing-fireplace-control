#ifndef MYSERVER_H_
#define MYSERVER_H_

#include "KC868-A8S.h"
#include "myData.h"

#define HEARTBEAT_INTERVAL 10000L

#include <FS.h>
#include <SPIFFS.h>
#define FileFS SPIFFS

void WEBsetup();
void WEBloop();
void notifyClients(String sensorReadings);

#endif /* MYSERVER_H_ */
