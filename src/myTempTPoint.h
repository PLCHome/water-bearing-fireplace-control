#ifndef MYTEMPTPOINT_H_
#define MYTEMPTPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "myData.h"
#include "myServer.h"

class myTempTPoint : public myPoint
{
    int tpos = -1;
    int tpos2 = -1;
    int16_t t2minus = -1;
    int16_t t2plus = -1;

public:
    myTempTPoint(JsonVariant json, myPoint* next);
    void calcVal() override; 
    String getJson() override;
};

#endif /* MYTEMPTPOINT_H_ */
