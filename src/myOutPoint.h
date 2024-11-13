#ifndef MYOUTPOINT_H_
#define MYOUTPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "myData.h"
#include "myServer.h"

enum outPoint : int8_t
{
    O_NORM = 0,
    O_NEG = 1
};

class myOutPoint : public myPoint
{
    int ida = -1;
    int opos = -1;
    outPoint op = O_NORM;

public:
    myOutPoint(JsonVariant json, myPoint* next);
    void calcVal() override; 
    String getJson() override;
};

#endif /* MYOUTPOINT_H_ */
