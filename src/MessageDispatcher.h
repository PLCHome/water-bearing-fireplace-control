#ifndef MESSAGEDISPATCHER_H_
#define MESSAGEDISPATCHER_H_

#include <Arduino.h>
#include <functional>
#include <vector>

#define CHANGE_TEMP 1
#define CHANGE_DI 2
#define CHANGE_DO 4
#define CHANGE_LED 8
#define CHANGE_POINTS 16
#define CHANGE_MQTT 32
#define WSGET_DATA 64
#define MQTTGET_DATA 128
#define CHANGE_MIXER 256

class MessageDispatcher {
public:
    MessageDispatcher();

    void startTask(const char *taskName, uint16_t stackSize, UBaseType_t priority);
    void notify(uint32_t message); // Send a notification
    void registerCallback(std::function<void(uint32_t)> callback);

private:
    static void taskFunction(void *pvParameters); // Task to handle notifications

    TaskHandle_t taskHandle; // Task handle for the dispatcher
    QueueHandle_t messageQueue; // Queue for notifications
    std::vector<std::function<void(uint32_t)>> callbacks; // Registered callbacks
};

extern MessageDispatcher messagedispatcher;

#endif // MESSAGEDISPATCHER_H_
