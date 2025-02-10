#include "mqttIoTemp.h"
#include "../../mySetup.h"

mqttIoTemp::~mqttIoTemp() { mymqtt->unregisterTopic(this); }

bool mqttIoTemp::init(DataCare *master) {
  Datatool::init(master);
  this->topic = mysetup->getArrayElementValue<String>("topic", "");
  this->qos = mysetup->getArrayElementValue<uint8_t>("qos", 0);
  this->defval = mysetup->getArrayElementValue<int16_t>("def", 2000);
  this->valtyp = mysetup->getArrayElementValue<int16_t>("valtyp", 100);
  this->json = mysetup->getArrayElementValue<String>("json", "");
  this->maxval = mysetup->getArrayElementValue<int16_t>("maxval", 5000);
  this->maxcut = mysetup->getArrayElementValue<uint8_t>("maxcut", 0);
  this->minval = mysetup->getArrayElementValue<int16_t>("minval", -5000);
  this->mincut = mysetup->getArrayElementValue<uint8_t>("mincut", 0);
  mymqtt->registerTopic(this);
  return true;
}

void mqttIoTemp::start() {
    this->master->getLastTemeratures(this->tempValsStart)[0] = this->defval;
}

uint16_t mqttIoTemp::getTempVals() { return 1; }

bool mqttIoTemp::processTempValues() {
  bool result = false;
  int16_t *tempHoldingRegRead =
      this->master->getLastTemeratures(this->tempValsStart);
  int16_t *tempHoldingReg = this->master->getTemeratures(this->tempValsStart);

  if (memcmp(tempHoldingReg, tempHoldingRegRead, 2) != 0) {
    memcpy(tempHoldingReg, tempHoldingRegRead, 2);
    result = true;
  }
  return result;
}

int16_t mqttIoTemp::cut(uint8_t cuttype, int16_t aktval, int16_t cutval) {
  switch (cuttype) {
  case 0:
    return aktval;
    break;
  case 1:
    return this->master->getLastTemeratures(this->tempValsStart)[0];
    break;
  case 2:
    return this->defval;
    break;
  case 3:
  default:
    return cutval;
    break;
  }
}

void mqttIoTemp::processCallback(char *topic, byte *payload,
                                 unsigned int length) {
  if (this->active && !(this->master->getNoReadTemeratures(this->tempValsStart)[0]) && strcmp(this->topic.c_str(), topic) == 0) {
    JsonDocument doc;
    deserializeJson(doc, payload, length);

    String jsonPath = this->json; // z. B. "brauchwassersolltemp.value"

    // Pfad dynamisch parsen, ohne festen Puffer
    int start = 0;
    int delimiterIndex;
    JsonVariant current = doc.as<JsonVariant>();

    while (start < jsonPath.length() && !current.isNull()) {
      delimiterIndex = jsonPath.indexOf('.', start);
      if (delimiterIndex == -1)
        delimiterIndex = jsonPath.length();

      String key = jsonPath.substring(start, delimiterIndex);
      current = current[key.c_str()];
      start = delimiterIndex + 1;
    }

    if (!current.isNull()) {
      Serial.print("Wert für ");
      Serial.print(this->json);
      Serial.print(": ");
      int16_t newval = round(current.as<float>() * valtyp);
      Serial.println(newval);
      if (newval < this->minval) {
        newval = cut(this->mincut, newval, this->minval);
      } else if (newval > this->maxval) {
        newval = cut(this->maxcut, newval, this->maxval);
      }
      this->master->getLastTemeratures(this->tempValsStart)[0] = newval;
    } else {
      Serial.print("Pfad nicht gefunden: ");
      Serial.println(this->json);
    }

    String out;
    serializeJson(doc, out);
    doc.clear();
    Serial.println(out);
  }
};

void mqttIoTemp::onConnection(PubSubClient *client) {
  if (this->active && topic.length() > 0) {
    Serial.print("register: ");
    Serial.print(this->topic.c_str());
    Serial.print(" ");
    Serial.println(client->subscribe(this->topic.c_str(), this->qos));
  }
};
