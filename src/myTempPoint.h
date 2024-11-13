#ifndef MYTEMPPOINT_H_
#define MYTEMPPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "myData.h"
#include "myServer.h"

class myTempPoint : public myPoint
{
    int tpos = -1;
    int16_t toff = 0;
    int16_t ton = 0;

public:
    myTempPoint(JsonVariant json, myPoint* next);
    void calcVal() override; 
    String getJson() override;
};

#endif /* MYTEMPPOINT_H_ */
