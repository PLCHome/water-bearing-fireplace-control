#ifndef MYLOGICPOINT_H_
#define MYLOGICPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "myData.h"
#include "myServer.h"
#include "myPoints.h"

enum logicPoint : int8_t
{
  LO_AND = 0,
  LO_OR = 1,
  LO_ANDN = 2,
  LO_ORN = 3,
  LO_ERR = -1
};

class myLogicPoint : public myPoint
{
  int ida = -1;
  int idb = -1;
  logicPoint logic = LO_ERR;

public:
  myLogicPoint(JsonVariant json, myPoint *next);
  void calcVal() override;
  String getJson() override;
};

#endif /* MYLOGICPOINT_H_ */
