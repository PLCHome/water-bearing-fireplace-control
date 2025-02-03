#ifndef MYMIXERTPOINT_H_
#define MYMIXERTPOINT_H_

#include "../timer/myTimerCycle.h"
#include "../timer/myTimerWakeup.h"
#include "myPoint.h"
#include "pointtyp.h"
#include <Arduino.h>
#include <ArduinoJson.h>

class myMixerTPoint : public myPoint, myTimerCycle, myTimerWakeup {
private:
  int16_t checkInterval = 10;
  int16_t impulseTime = 5;
  int16_t pulsesToOpen = 40;
  bool offClosed = true;

  int16_t currentPulse = 0;

  int16_t lastTemperature;
  int16_t hysteresis = 100;
  int16_t delta = 30;

  int8_t prozent;

  int16_t closing =
      0; // closing >0 : closing; closing =0 : off mode; closing =-1 : on mode;

  int idon = -1;
  int tpos = -1;
  int tposTarget = -1;
  int opclose = -1;
  int opopen = -1;

public:
  myMixerTPoint(JsonVariant json, pointTyp typ);
  ~myMixerTPoint();
  void calcVal() override;
  void getJson(JsonObject &doc) override;
  void doWakeUp() override;
  bool doOpen();
  bool doClose();
  void doCycleIntervall() override;
  void setcycleInterval(bool reset);
};

#endif // MYMIXERTPOINT_H_
