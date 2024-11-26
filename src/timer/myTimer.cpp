#include "myTimer.h"
#include "myTimerCycle.h"
#include "myTimerWakeup.h"

myTimer mytimer = myTimer();

void myTimer::registerCycle(myTimerCycle *instance)
{
    instance->lastCycleTime = millis();
    cycleTimer.push_back(instance);
}

void myTimer::registerWakeUp(myTimerWakeup *instance)
{
    instance->lastWakeUpTime = millis();
    wakeUpTimer.push_back(instance);
}

void testTimerTask(void *pvParameters)
{
    mytimer.timerTask(pvParameters);
}

void myTimer::start()
{
    for (auto &instance : this->cycleTimer)
    {
        instance->lastCycleTime = millis();
    }
    xTaskCreate(testTimerTask, "Timer Task", 2048, this, 1, NULL);
}

void myTimer::timerTask(void *pvParameters)
{
    myTimer *timer = static_cast<myTimer *>(pvParameters);
    while (true)
    {
        unsigned long currentTime = millis();
        for (auto &cycle : this->cycleTimer)
        {
            if (currentTime - cycle->lastCycleTime >= cycle->getCycleInterval())
            {
                cycle->doCycleIntervall();
                cycle->lastCycleTime = millis();
            }
        }
        for (auto it = timer->wakeUpTimer.begin(); it != timer->wakeUpTimer.end();)
        {
            auto &wakeup = *it;
            if (currentTime - wakeup->lastWakeUpTime >= wakeup->getWakeUpTime())
            {
                wakeup->doWakeUp();
                it = timer->wakeUpTimer.erase(it);
            }
            else
            {
                ++it;
            }
        }
        vTaskDelay(1);
    }
}
