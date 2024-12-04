#ifndef MESSAGEDISPATCHER_H_
#define MESSAGEDISPATCHER_H_

#include <Arduino.h>
#include <functional>
#include <vector>

#define CHANGE_TEMP (1 << 0)   // 0b0000'0000'0000'0001
#define CHANGE_DI (1 << 1)     // 0b0000'0000'0000'0010
#define CHANGE_DO (1 << 2)     // 0b0000'0000'0000'0100
#define CHANGE_LED (1 << 3)    // 0b0000'0000'0000'1000
#define CHANGE_POINTS (1 << 4) // 0b0000'0000'0001'0000
#define CHANGE_MQTT (1 << 5)   // 0b0000'0000'0010'0000
#define WSGET_DATA (1 << 6)    // 0b0000'0000'0100'0000
#define MQTTGET_DATA (1 << 7)  // 0b0000'0000'1000'0000
#define CHANGE_MIXER (1 << 8)  // 0b0000'0001'0000'0000
#define TIME_TRIGGER (1 << 9)  // 0b0000'0010'0000'0000

class MessageDispatcher {
public:
  MessageDispatcher();

  void startTask(const char *taskName, uint16_t stackSize,
                 UBaseType_t priority);
  void notify(uint32_t message); // Send a notification
  void registerCallback(std::function<void(uint32_t)> callback);

private:
  static void taskFunction(void *pvParameters); // Task to handle notifications

  TaskHandle_t taskHandle;    // Task handle for the dispatcher
  QueueHandle_t messageQueue; // Queue for notifications
  std::vector<std::function<void(uint32_t)>> callbacks; // Registered callbacks
};

extern MessageDispatcher messagedispatcher;

#endif // MESSAGEDISPATCHER_H_
