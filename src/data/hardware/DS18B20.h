#ifndef DS18B20_H_
#define DS18B20_H_

#include "Hardware.h"
#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

class DS18B20 : public Hardware {
private:
  OneWire *oneWire;
  DallasTemperature *sensors;
  int8_t pin;
  bool busi = false;

public:
  DS18B20();
  void init() override;
  String getJsonIDs(bool obj);
  DallasTemperature *getSensors() const;
  void requestTemperatures();
  void temperaturesFinished();
  bool stringToDeviceAddress(const String &str, DeviceAddress &address);
  int16_t getTemp(const DeviceAddress &address);
};

#endif /* DS18B20_H_ */
