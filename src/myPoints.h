#ifndef MYPOINTS_H_
#define MYPOINTS_H_

#include <Arduino.h>
#include "myPoint.h"
#include "myTempPoint.h"
#include "myLogicPoint.h"
#include "myTempPoint.h"
#include "myTempTPoint.h"
#include "myOutPoint.h"

#define POINTSFILENAME "/points.json"

enum pointTyp : int8_t
{
  PT_TEMP = 0,
  PT_TEMPT = 1,
  PT_LOGIC = 2,
  PT_OUT = 3
};

class myPoints
{
  myPoint *first = NULL;

public:
  void cleanUp();
  void build();
  myPoint *getPoint(int id);
  myPoint *getPoint(String name);
  ergPoint getVal(int id);
  ergPoint getVal(String name);
  void calcVal();
};

extern myPoints mypoints;

#endif /* MYPOINTS_H_ */
