#include "myCycleMessageTimer.h"
#include "../MessageDispatcher.h"
#include "myTimer.h"

myCycleMessageTimer::myCycleMessageTimer() {
  this->cycleInterval = CYCLEINTERVAL;
  mytimer.registerCycle(this);
}

void myCycleMessageTimer::doCycleIntervall() {
  messagedispatcher.notify(TIME_TRIGGER);
}
