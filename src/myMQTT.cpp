#include "myMQTT.h"
#include "MessageDispatcher.h"
#include "data/DataCare.h"
#include "points/myPoints.h"

// #define DEBUG_MQTTPUB

myMQTT *mymqtt = new myMQTT();

// Constructor
myMQTT::myMQTT() : client(espClient) {}

// Callback for received messages
void myMQTT::callback(char *topic, byte *payload, unsigned int length) {
  mymqtt->docallback(topic, payload, length);
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  for (auto &mqttreg : mymqtt->mqttRegister) {
    mqttreg->processCallback(topic, payload, length);
  }
}

// myMQTT Initialization function
void myMQTT::init() {
  this->client.setBufferSize(1024);
  mysetup->resetSection();
  mysetup->setNextSection("mqtt");

  this->active = mysetup->getSectionValue<bool>("active", "");
  this->domain =
      mysetup->cstrPersists(mysetup->getSectionValue<String>("server", ""));
  this->port = mysetup->getSectionValue<uint16_t>("port", MQTT_PORT);
  this->connectTime =
      1000 * mysetup->getSectionValue<uint16_t>("connecttime", 5);
  this->clientId = mysetup->cstrPersists(
      mysetup->getSectionValue<String>("clientid", "myOven"));
  this->user =
      mysetup->cstrPersistsNull(mysetup->getSectionValue<String>("user", ""));
  this->pass = mysetup->cstrPersistsNull(
      mysetup->getSectionValue<String>("password", ""));
  this->willTopic = mysetup->cstrPersistsNull(
      mysetup->getSectionValue<String>("lastwilltop", ""));
  this->willQos = mysetup->getSectionValue<uint8_t>("lastwillqos", willQos);
  this->willRetain =
      mysetup->getSectionValue<bool>("lastwillretain", willRetain);
  this->willMessage = mysetup->cstrPersistsNull(
      mysetup->getSectionValue<String>("lastwillmes", "off"));
  this->connectTopic = mysetup->cstrPersistsNull(
      mysetup->getSectionValue<String>("connecttop", ""));
  this->connectRetain =
      mysetup->getSectionValue<bool>("connectretain", willRetain);
  this->connectMessage = mysetup->cstrPersistsNull(
      mysetup->getSectionValue<String>("connectmes", "on"));
  this->publishTopic =
      cleanPubTopic(mysetup->getSectionValue<String>("puplishtopic", ""));
  this->publishRetain =
      mysetup->getSectionValue<bool>("puplishretain", willRetain);
  this->subscribeTopic = mysetup->cstrPersistsNull(cleanSubscribTopic(
      mysetup->getSectionValue<String>("subscribetopic", "")));
  this->subscribeQos =
      mysetup->getSectionValue<uint8_t>("subscribeqos", willQos);

  // Set the myMQTT server and port
  client.setServer(this->domain, this->port);
  client.setCallback(callback); // Set the callback for received messages

  messagedispatcher.registerCallback(
      [this](uint32_t message) { this->onMessage(message); });
}

String myMQTT::cleanSubscribTopic(String topic) {
  if (topic.isEmpty() || topic.endsWith("#")) {
    return topic;
  } else if (topic.endsWith("/")) {
    return topic + "#";
  } else {
    return topic + "/#";
  }
}

String myMQTT::cleanPubTopic(String topic) {
  if (topic.isEmpty() || topic.endsWith("/")) {
    return topic;
  } else {
    return topic + "/";
  }
}

// Reconnect to the myMQTT broker
void myMQTT::reconnect() {
  // Keep trying to reconnect while not connected
  if (this->client.connect(this->clientId, this->user, this->pass,
                          this->willTopic, this->willQos, this->willRetain,
                          this->willMessage)) {
    Serial.println("MQTT Connected!");
      if (this->connectTopic) {
        this->client.publish(this->connectTopic, this->connectMessage,
                            this->connectRetain);
      }
      if (this->subscribeTopic) {
        this->client.subscribe(this->subscribeTopic, this->subscribeQos);
      }
      for (auto &mqttreg : mymqtt->mqttRegister) {
        mqttreg->onConnection(&this->client);
      }
    messagedispatcher.notify(MQTTGET_DATA);
  } else {
    Serial.print("Failed, rc=");
    Serial.print(this->client.state());
    Serial.println(" - Trying again later...");
  }
}

// myMQTT loop function (to be called regularly)
void myMQTT::loop() {
  if (this->active) {
    if (xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
      if (!client.connected()) {
        if (millis() - this->lastConnect > this->connectTime)
          reconnect(); // Reconnect if the connection is lost
      } else {
        client.loop(); // Update the myMQTT client to process incoming and
                      // outgoing messages
      }
      xSemaphoreGive(this->xMutex);
    }
  }
}

// Function to publish messages to a topic
void myMQTT::publish(String topic, String message) {
  if (this->active) {
    if (client.connected() && xSemaphoreTake(this->xMutex, portMAX_DELAY)) {
      String top = this->publishTopic + topic;
      boolean send =
          client.publish(top.c_str(), message.c_str(), this->publishRetain);
      xSemaphoreGive(this->xMutex);
#ifdef DEBUG_MQTTPUB
      Serial.print(top + " " + send + " sent: ");
      Serial.println(message);
#endif
    } else {
      Serial.println("Error: Not connected to the myMQTT server.");
    }
  }
}

void myMQTT::docallback(char *topic, byte *payload, unsigned int length) {
  u_int8_t len = strlen(this->subscribeTopic)-1;
  if (strncmp(this->subscribeTopic, topic, len) == 0) {
    if (strcmp("query", &topic[len]) == 0) {
      if (strncmp("all", (char *)payload, length) == 0) {
        //Serial.println("MQTTGET_DATA");
        messagedispatcher.notify(MQTTGET_DATA);
      } else if (strncmp("temps", (char *)payload, length) == 0) {
        //Serial.println("CHANGE_TEMP");
        messagedispatcher.notify(CHANGE_TEMP);
      } else if (strncmp("inputs", (char *)payload, length) == 0) {
        //Serial.println("CHANGE_DI");
        messagedispatcher.notify(CHANGE_DI);
      } else if (strncmp("outputs", (char *)payload, length) == 0) {
        //Serial.println("CHANGE_DO");
        messagedispatcher.notify(CHANGE_DO);
      } else if (strncmp("points", (char *)payload, length) == 0) {
        //Serial.println("CHANGE_POINTS");
        messagedispatcher.notify(CHANGE_POINTS);
      } else if (strncmp("mixer", (char *)payload, length) == 0) {
        //Serial.println("CHANGE_MIXER");
        messagedispatcher.notify(CHANGE_MIXER);
      }
    }
  }
}

void myMQTT::onMessage(uint32_t dataChange) {
  if ((dataChange & (CHANGE_TEMP + MQTTGET_DATA)) != 0) {
    this->publish("temps", datacare.jsonTemeratures(false));
    this->publish("tempsnoop", datacare.jsonNoTemeratures(false));
  }
  if ((dataChange & (CHANGE_DI + MQTTGET_DATA)) != 0) {
    this->publish("inputs", datacare.jsonDI(false));
  }
  if ((dataChange & (CHANGE_DO + MQTTGET_DATA)) != 0) {
    this->publish("outputs", datacare.jsonDO(false));
  }
  // if (dataChange & (CHANGE_LED|WSGET_DATA) != 0) {
  //     notifyClients(datacare.jsonLED());
  // }
  if ((dataChange & (CHANGE_POINTS + MQTTGET_DATA)) != 0) {
    this->publish("points", mypoints.getJSONValue(false));
  }
  if ((dataChange & (CHANGE_MIXER + MQTTGET_DATA)) != 0) {
    this->publish("mixer", mypoints.getJSONValueMixer());
  }
}

void myMQTT::registerTopic(myMQTTRegister *instance) {
  mqttRegister.push_back(instance);
  if (client.connected()) {
    instance->onConnection(&client);
  }
}

void myMQTT::unregisterTopic(myMQTTRegister *instance) {
  auto it = std::find(mqttRegister.begin(), mqttRegister.end(), instance);
  if (it != mqttRegister.end()) {
    mqttRegister.erase(it);
  }
}
