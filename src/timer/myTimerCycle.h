#ifndef MYTIMERCYCLE_H
#define MYTIMERCYCLE_H

#include <Arduino.h>

class myTimerCycle {
public:
    unsigned long getCycleInterval() const;
    unsigned long lastCycleTime;
    virtual void doCycleIntervall() = 0;
protected:
    unsigned long cycleInterval;
};

#endif
