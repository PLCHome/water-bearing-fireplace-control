#ifndef DS18B20TEMP_H_
#define DS18B20TEMP_H_

#include "data/DataCare.h"
#include "data/io/Datatool.h"
#include <Arduino.h>
#include <DallasTemperature.h>

class ds18b20Temp : public Datatool {
private:
  String id;
  DeviceAddress device;

public:
  bool init(DataCare *master) override;
  uint16_t getTempVals() override;
  bool processTempValues() override;
};

#endif /* DS18B20TEMP_H_ */
