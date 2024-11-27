#include "myCycleMessageTimer.h"
#include "myTimer.h"
#include "../MessageDispatcher.h"

myCycleMessageTimer::myCycleMessageTimer() {
    this->cycleInterval = CYCLEINTERVAL;
    mytimer.registerCycle(this);
}

void myCycleMessageTimer::doCycleIntervall(){
    messagedispatcher.notify(TIME_TRIGGER);
}
