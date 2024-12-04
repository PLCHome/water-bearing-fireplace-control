#ifndef MYPOINTS_H_
#define MYPOINTS_H_

#include "points/myPoint.h"
#include "points/pointtyp.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <vector>

#define POINTSFILENAME "/config/points.json"

class myPoint;

class myPoints {
  std::vector<myPoint *> vmypoint;
  bool changed = false;

public:
  void cleanUp();
  void init();
  void build();
  void loop();
  void calcVal(uint32_t change);
  void setChanged();
  String getJSONValue(bool obj);
  String getJSONValueMixer();
  myPoint *getPoint(int id);
  myPoint *getPoint(String name);
  ergPoint getVal(int id);
  ergPoint getVal(String name);
};

extern myPoints mypoints;

#endif /* MYPOINTS_H_ */
