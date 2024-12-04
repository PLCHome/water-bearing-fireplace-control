#ifndef MYCYCLEMESSAGETIMER_H
#define MYCYCLEMESSAGETIMER_H

#include "timer/myTimerCycle.h"
#include <Arduino.h>

#define CYCLEINTERVAL (1000); // every secund

class myCycleMessageTimer : public myTimerCycle {
public:
  myCycleMessageTimer();
  void doCycleIntervall() override;

protected:
};

#endif /* MYCYCLEMESSAGETIMER_H */
