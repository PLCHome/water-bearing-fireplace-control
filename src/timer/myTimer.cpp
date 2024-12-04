#include "myTimer.h"
#include "myCycleMessageTimer.h"
#include "myTimerCycle.h"
#include "myTimerWakeup.h"

myTimer mytimer = myTimer();
myCycleMessageTimer cyclemessagetimer = myCycleMessageTimer();

void myTimer::registerCycle(myTimerCycle *instance) {
  instance->lastCycleTime = millis();
  cycleTimer.push_back(instance);
}

void myTimer::unregisterCycle(myTimerCycle *instance) {
  auto it = std::find(cycleTimer.begin(), cycleTimer.end(), instance);
  if (it != cycleTimer.end()) {
    cycleTimer.erase(it);
  }
}

void myTimer::registerWakeUp(myTimerWakeup *instance) {
  instance->lastWakeUpTime = millis();
  wakeUpTimer.push_back(instance);
}

void myTimer::unregisterWakeUp(myTimerWakeup *instance) {
  auto it = std::find(wakeUpTimer.begin(), wakeUpTimer.end(), instance);
  if (it != wakeUpTimer.end()) {
    wakeUpTimer.erase(it);
  }
}


void myTimer::start() {
  for (auto &instance : this->cycleTimer) {
    instance->lastCycleTime = millis();
  }
  xTaskCreate(timerTask, "Timer Task", 2048, this, TMER_TASK_PRIO, NULL);

}

void myTimer::timerTask(void *pvParameters) {
  myTimer *timer = static_cast<myTimer *>(pvParameters);
  while (true) {
    unsigned long currentTime = millis();
    for (auto &cycle : timer->cycleTimer) {
      if (currentTime - cycle->lastCycleTime >= cycle->getCycleInterval()) {
        cycle->doCycleIntervall();
        cycle->lastCycleTime = millis();
      }
    }
    for (auto it = timer->wakeUpTimer.begin();
         it != timer->wakeUpTimer.end();) {
      auto &wakeup = *it;
      if (currentTime - wakeup->lastWakeUpTime >= wakeup->getWakeUpTime()) {
        wakeup->doWakeUp();
        it = timer->wakeUpTimer.erase(it);
      } else {
        ++it;
      }
    }
    vTaskDelay(1);
  }
}
