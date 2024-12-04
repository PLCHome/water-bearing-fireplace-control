#ifndef MYTIMERWAKEUP_H
#define MYTIMERWAKEUP_H

#include <Arduino.h>

class myTimerWakeup {
public:
  unsigned long getWakeUpTime() const;
  unsigned long lastWakeUpTime;
  virtual void doWakeUp() = 0;

protected:
  unsigned long wakeUpTime = 0;
};

#endif
