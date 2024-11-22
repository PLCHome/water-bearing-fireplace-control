#include "myMQTT.h"

myMQTT *mymqtt = new myMQTT();

// Constructor
myMQTT::myMQTT() : client(espClient) {}

// Callback for received messages
void myMQTT::callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// myMQTT Initialization function
void myMQTT::init()
{
    mysetup->resetSection();
    mysetup->setNextSection("mqtt");

    this->active = mysetup->getSectionValue<bool>("active", "");
    this->domain = mysetup->cstrPersists(mysetup->getSectionValue<String>("server", ""));
    this->port = mysetup->getSectionValue<uint16_t>("port", MQTT_PORT);
    this->connectTime = 1000 * mysetup->getSectionValue<uint16_t>("connecttime", 5);
    this->clientId = mysetup->cstrPersists(mysetup->getSectionValue<String>("clientid", "myOfen"));
    this->user = mysetup->cstrPersistsNull(mysetup->getSectionValue<String>("user", ""));
    this->pass = mysetup->cstrPersistsNull(mysetup->getSectionValue<String>("password", ""));
    this->willTopic = mysetup->cstrPersistsNull(mysetup->getSectionValue<String>("lastwilltop", ""));
    this->willQos = mysetup->getSectionValue<uint8_t>("lastwillqos", willQos);
    this->willRetain = mysetup->getSectionValue<bool>("lastwillretain", willRetain);
    this->willMessage = mysetup->cstrPersistsNull(mysetup->getSectionValue<String>("lastwillmes", "off"));
    this->connectTopic = mysetup->cstrPersistsNull(mysetup->getSectionValue<String>("connecttop", ""));
    this->connectRetain = mysetup->getSectionValue<bool>("connectretain", willRetain);
    this->connectMessage = mysetup->cstrPersistsNull(mysetup->getSectionValue<String>("connectmes", "on"));
    this->publishTopic = cleanPubTopic(mysetup->getSectionValue<String>("puplishtopic", ""));
    this->publishRetain = mysetup->getSectionValue<bool>("puplishretain", willRetain);
    this->subscribeTopic = mysetup->cstrPersistsNull(cleanSubscribTopic(mysetup->getSectionValue<String>("subscribetopic", "")));
    this->subscribeQos = mysetup->getSectionValue<uint8_t>("subscribeqos", willQos);

    // Set the myMQTT server and port
    client.setServer(this->domain, this->port);
    client.setCallback(callback); // Set the callback for received messages
}

String myMQTT::cleanSubscribTopic(String topic)
{
    if (topic.isEmpty() || topic.endsWith("#"))
    {
        return topic;
    }
    else if (topic.endsWith("/"))
    {
        return topic + "#";
    }
    else
    {
        return topic + "/#";
    }
}

String myMQTT::cleanPubTopic(String topic)
{
    if (topic.isEmpty() || topic.endsWith("/"))
    {
        return topic;
    }
    else
    {
        return topic + "/";
    }
}

// Reconnect to the myMQTT broker
void myMQTT::reconnect()
{
    // Keep trying to reconnect while not connected
    if (client.connect(this->clientId, this->user, this->pass, this->willTopic, this->willQos, this->willRetain, this->willMessage))
    {
        Serial.println("MQTT Connected!");
        if (this->connectTopic)
        {
            client.publish(this->connectTopic, this->connectMessage, this->connectRetain);
        }
        if (this->subscribeTopic)
        {
            client.subscribe(this->subscribeTopic, this->subscribeQos);
        }
    }
    else
    {
        Serial.print("Failed, rc=");
        Serial.print(client.state());
        Serial.println(" - Trying again later...");
    }
}

// myMQTT loop function (to be called regularly)
void myMQTT::loop()
{
    if (this->active) {
        if (!client.connected())
        {
            if (millis() - this->lastConnect > this->connectTime)
                reconnect(); // Reconnect if the connection is lost
        }
        else
            client.loop(); // Update the myMQTT client to process incoming and outgoing messages
    }
}

// Function to publish messages to a topic
void myMQTT::publish(String topic, String message)
{
    if (this->active) {
        if (client.connected())
        {
            String top = this->publishTopic+topic;
            client.publish(top.c_str(), message.c_str(),this->publishRetain);
            Serial.print(top+" sent: ");
            Serial.println(message);
        }
        else
        {
            Serial.println("Error: Not connected to the myMQTT server.");
        }
    }
}
