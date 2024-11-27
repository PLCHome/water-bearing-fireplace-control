#ifndef myMixerPoint_H_
#define myMixerPoint_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "pointtyp.h"
#include "../timer/myTimerCycle.h"
#include "../timer/myTimerWakeup.h"


class myMixerPoint : public myPoint, myTimerCycle, myTimerWakeup
{
private:
    int16_t checkInterval = 10;
    int16_t impulseTime = 5;
    int16_t pulsesToOpen = 40;

    int16_t currentPulse = 0;

    int16_t lastTemperature;
    int16_t hysteresis = 100;
    int16_t delta = 30;
    int16_t targetTemperature;

    int8_t prozent;
    
    int16_t closing = 0; // closing >0 : closing; closing =0 : off mode; closing =-1 : on mode;
    
    int idon = -1;
    int tpos = -1; 
    int opclose = -1; 
    int opopen = -1;

public:
    myMixerPoint(JsonVariant json, pointTyp typ);
    void calcVal() override;
    void getJson(JsonObject &doc) override;
    void doWakeUp() override;
    bool doOpen();
    bool doClose();
    void doCycleIntervall() override;
};

#endif // MISCHER_H
