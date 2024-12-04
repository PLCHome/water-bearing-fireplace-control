#ifndef MYPOINT_H_
#define MYPOINT_H_

#include "pointtyp.h"
#include <Arduino.h>
#include <ArduinoJson.h>

enum ergPoint : int8_t { TP_OFF = 0, TP_ON = 1, TP_ERR = -1 };

class myPoint {
protected:
  String name = "";
  int id = -1;
  ergPoint on = TP_ERR;
  bool calculated = false;
  pointTyp type;

public:
  myPoint(JsonVariant json, pointTyp type);
  virtual ~myPoint();
  myPoint *getPoint(int id);
  myPoint *getPoint(String name);
  void unsetCalculated();
  ergPoint getVal();
  virtual void calcVal() = 0;
  virtual void getJson(JsonObject &doc) = 0;
  void loop();
  int getId() const;
  ergPoint getOn() const;
  pointTyp getTyp() const;
};

#endif /* MYPOINT_H_ */
