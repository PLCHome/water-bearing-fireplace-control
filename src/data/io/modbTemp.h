#ifndef MODBTEMP_H_
#define MODBTEMP_H_

#include "../DataCare.h"
#include "Datatool.h"
#include <Arduino.h>

class modbTemp : public Datatool {
private:
  uint8_t typ;
  uint8_t id;
  uint16_t adress;
  uint16_t values;
  uint8_t decimals;
  void calcTo2Dec(int16_t *buf, int16_t *out, bool *p);

public:
  bool init(DataCare *master) override;
  uint16_t getTempVals() override;
  bool processTempValues() override;
};

#endif /* MODBTEMP_H_ */
