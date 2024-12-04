#ifndef MYPIDMIXERPOINT_H_
#define MYPIDMIXERPOINT_H_

#include "timer/myTimerCycle.h"
#include "timer/myTimerWakeup.h"
#include "points/myPoint.h"
#include "points/pointtyp.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PID_v1.h>

enum myPidMixerState {
  MIXER_IS_WORKING,
  MIXER_IS_OFF,
  MIXER_IS_RESETTING
};

class myPidMixerPoint : public myPoint, myTimerCycle, myTimerWakeup {
private:
  int16_t checkInterval = 10;
  unsigned long impulseTime = 5;
  unsigned long runTime = 120000;
  bool offClosed = true;

  PID *pid;
  double pidSetpoint = 0.0, pidInput = 0.0, pidOutput = 0.0;
  double Kp = 2.0, Ki = 5.0, Kd = 1.0;

  unsigned long aktPos = 0;

  int16_t targetTemperature;
  myPidMixerState aktState = MIXER_IS_OFF;

  int idon = -1;
  int tpos = -1;
  int opclose = -1;
  int opopen = -1;

public:
  myPidMixerPoint(JsonVariant json, pointTyp typ);
  ~myPidMixerPoint() override;
  void calcVal() override;
  void getJson(JsonObject &doc) override;
  void doWakeUp() override;
  void doReset();
  bool doOpen(long ms);
  bool doClose(long ms);
  void doCycleIntervall() override;
  void setcycleInterval(bool reset);
};

#endif // MYPIDMIXERPOINT_H_
