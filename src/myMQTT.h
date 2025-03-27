#ifndef MYMQTT_H_
#define MYMQTT_H_

#include <Arduino.h>
#include <PubSubClient.h> // For MQTT
#include <WiFi.h> // For Wi-Fi, replace this with EthernetClient if using Ethernet
#include <vector>

#include "mySetup.h"

// Define the myMQTT broker server
#define MQTT_PORT 1883 // Standard MQTT Port without TLS

class myMQTTRegister {
public:
  virtual void processCallback(char *topic, byte *payload,
                               unsigned int length) = 0;
  virtual void onConnection(PubSubClient *client) = 0;
};

class myMQTT {
private:
  WiFiClient espClient; // Wi-Fi client, use EthernetClient if using Ethernet
  PubSubClient client;

  boolean active;
  const char *domain;
  uint16_t port;

  const char *clientId;
  const char *user;
  const char *pass;
  const char *willTopic;
  uint8_t willQos;
  boolean willRetain;
  const char *willMessage;

  const char *connectTopic;
  boolean connectRetain;
  const char *connectMessage;

  String publishTopic;
  boolean publishRetain;

  const char *subscribeTopic;
  uint8_t subscribeQos;

  unsigned long lastConnect = 0;
  unsigned long connectTime = 5000;

  // Callback function for received myMQTT messages
  static void callback(char *topic, byte *payload, unsigned int length);
  void onMessage(uint32_t change);

  SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

public:
  std::vector<myMQTTRegister *> mqttRegister;
  myMQTT();    // Constructor
  void init(); // myMQTT initialization function
  String cleanSubscribTopic(String topic);
  void docallback(char *topic, byte *payload, unsigned int length);
  void loop();      // myMQTT loop function to be called regularly
  void reconnect(); // Reconnect to the broker if disconnected
  String cleanPubTopic(String topic);
  void publish(String topic, String message); // Function to send messages
  void registerTopic(myMQTTRegister *instance);
  void unregisterTopic(myMQTTRegister *instance);
};

extern myMQTT *mymqtt;

#endif /* MYMQTT_H_ */
