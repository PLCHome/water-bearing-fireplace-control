#ifndef MYTIMER_H
#define MYTIMER_H

#include "myTimerCycle.h"
#include "myTimerWakeup.h"
#include <Arduino.h>
#include <vector>

class myTimer {
public:
  void registerCycle(myTimerCycle *task);
  void unregisterCycle(myTimerCycle *task);
  void registerWakeUp(myTimerWakeup *task);

  void start();

  void timerTask(void *pvParameters);

private:
  std::vector<myTimerCycle *> cycleTimer;
  std::vector<myTimerWakeup *> wakeUpTimer;
};

extern myTimer mytimer;

#endif
