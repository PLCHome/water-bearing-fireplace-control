#ifndef MYTIMER_H
#define MYTIMER_H

#include <Arduino.h>
#include <vector>
#include "myTimerCycle.h"
#include "myTimerWakeup.h"

class myTimer {
public:
    
    void registerCycle(myTimerCycle* task); 
    void registerWakeUp(myTimerWakeup* task); 

    void start();
    
    void timerTask(void *pvParameters);
private:
    std::vector<myTimerCycle*> cycleTimer; 
    std::vector<myTimerWakeup*> wakeUpTimer; 
};

extern myTimer mytimer;

#endif
