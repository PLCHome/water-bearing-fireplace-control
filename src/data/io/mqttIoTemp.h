#ifndef MQTTIOTEMP_H_
#define MQTTIOTEMP_H_

#include "../../myMQTT.h"
#include "../DataCare.h"
#include "Datatool.h"
#include <Arduino.h>

class mqttIoTemp : public Datatool, myMQTTRegister {
private:
  String topic;
  uint8_t qos;
  int16_t val;
  int16_t valtyp;
  int16_t defval;
  String json;
  int16_t maxval;
  uint8_t maxcut;
  int16_t minval;
  uint8_t mincut;
  int16_t cut(uint8_t cuttype, int16_t val, int16_t cutval);
public:
  ~mqttIoTemp();
  bool init(DataCare *master) override;
  uint16_t getTempVals() override;
  bool processTempValues() override;

  void processCallback(char *topic, byte *payload,
                       unsigned int length) override;
  void onConnection(PubSubClient *client) override;
};

#endif /* MQTTIOTEMP_H_ */
