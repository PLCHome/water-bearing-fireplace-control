#ifndef MYTIMER_H
#define MYTIMER_H

#include "timer/myTimerCycle.h"
#include "timer/myTimerWakeup.h"
#include <Arduino.h>
#include <vector>

#define TMER_TASK_PRIO 11

class myTimer {
public:
  void registerCycle(myTimerCycle *task);
  void unregisterCycle(myTimerCycle *task);
  void registerWakeUp(myTimerWakeup *task);
  void unregisterWakeUp(myTimerWakeup *task);

  void start();

  static void timerTask(void *pvParameters);

private:
  std::vector<myTimerCycle *> cycleTimer;
  std::vector<myTimerWakeup *> wakeUpTimer;
};

extern myTimer mytimer;

#endif
