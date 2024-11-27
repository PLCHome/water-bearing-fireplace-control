#ifndef MYCYCLEMESSAGETIMER_H
#define MYCYCLEMESSAGETIMER_H

#include <Arduino.h>
#include "myTimerCycle.h"

#define CYCLEINTERVAL (1000); // every secund

class myCycleMessageTimer : public myTimerCycle {
public:
    myCycleMessageTimer();
    void doCycleIntervall() override;
protected:
};

#endif /* MYCYCLEMESSAGETIMER_H */
