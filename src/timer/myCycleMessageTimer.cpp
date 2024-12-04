#include "timer/myCycleMessageTimer.h"
#include "MessageDispatcher.h"
#include "timer/myTimer.h"

myCycleMessageTimer::myCycleMessageTimer() {
  this->cycleInterval = CYCLEINTERVAL;
  mytimer.registerCycle(this);
}

void myCycleMessageTimer::doCycleIntervall() {
  messagedispatcher.notify(TIME_TRIGGER);
}
