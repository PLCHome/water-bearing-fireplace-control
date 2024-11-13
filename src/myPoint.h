#ifndef MYPOINT_H_
#define MYPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>

enum ergPoint : int8_t
{
  TP_OFF = 0,
  TP_ON = 1,
  TP_ERR = -1
};

class myPoint
{
protected:
  String name = "";
  int id = -1;
  ergPoint on = TP_ERR;
  bool calculated = false;
  myPoint *next = NULL;
public:
  myPoint(JsonVariant json, myPoint* next);
  myPoint* getPoint(int id);
  myPoint* getPoint(String name);
  myPoint* getNext();
  void unsetCalculated();
  ergPoint getVal();
  virtual void calcVal() = 0;
  virtual String getJson() = 0;
};

#endif /* MYPOINT_H_ */
