#ifndef MODBDOUT_H_
#define MODBDOUT_H_

#include "data/DataCare.h"
#include "data/io/Datatool.h"
#include <Arduino.h>

class modbDOut : public Datatool {
private:
  uint8_t typ;
  uint8_t id;
  uint16_t adress;
  uint16_t values;

public:
  bool init(DataCare *master) override;
  uint16_t getDOVals() override;
  bool processDoValues() override;
};

#endif /* MODBDOUT_H_ */
