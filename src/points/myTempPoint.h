#ifndef MYTEMPPOINT_H_
#define MYTEMPPOINT_H_

#include "myPoint.h"
#include "myPoints.h"
#include "pointtyp.h"
#include <Arduino.h>
#include <ArduinoJson.h>

class myTempPoint : public myPoint {
  int tpos = -1;    /**< Position index in the temperature holding register */
  int16_t toff = 0; /**< Temperature threshold to turn "off" */
  int16_t ton = 0;  /**< Temperature threshold to turn "on" */

public:
  myTempPoint(JsonVariant json, pointTyp typ);
  void calcVal() override;
  void getJson(JsonObject &doc) override;
};

#endif /* MYTEMPPOINT_H_ */
