#include "MessageDispatcher.h"

MessageDispatcher::MessageDispatcher()
    : taskHandle(nullptr), messageQueue(nullptr) {
  messageQueue =
      xQueueCreate(10, sizeof(uint32_t)); // Queue with capacity for 10 messages
}

void MessageDispatcher::startTask(const char *taskName, uint16_t stackSize,
                                  UBaseType_t priority) {
  xTaskCreate(taskFunction, // Static task function
              taskName,     // Name of the task
              stackSize,    // Stack size in bytes
              this,         // Pass the instance as the parameter
              priority,     // Task priority
              &taskHandle   // Save the task handle
  );
}

void MessageDispatcher::notify(uint32_t message) {
  if (messageQueue != nullptr) {
    // Enqueue the message
    xQueueSend(messageQueue, &message, 0);
  }
}

void MessageDispatcher::registerCallback(
    std::function<void(uint32_t)> callback) {
  // Register a callback
  callbacks.push_back(callback);
}

void MessageDispatcher::taskFunction(void *pvParameters) {
  // Recover the instance from the parameter
  MessageDispatcher *instance = static_cast<MessageDispatcher *>(pvParameters);

  uint32_t message;
  while (true) {
    // Wait for a message from the queue
    if (xQueueReceive(instance->messageQueue, &message, portMAX_DELAY)) {
      // Process the message
      for (const auto &callback : instance->callbacks) {
        callback(message);
      }
    }
  }
}

MessageDispatcher messagedispatcher = MessageDispatcher();
