#ifndef PCF8574IO_H_
#define PCF8574IO_H_

#include <Arduino.h>
#include <PCF8574.h>

#include "../DataCare.h"
#include "../dirtyp.h"
#include "Datatool.h"

class DataCare;

class pcf8574io : public Datatool {
private:
  dirtyp dirb[8];
  PCF8574 *pcf8574 = nullptr;
  uint8_t input = 0;
  uint8_t output = 0;
  uint8_t loinput = 0;
  uint8_t looutput = 0;

public:
  bool init(DataCare *master) override;
  uint16_t getDOVals() override;
  uint16_t getDIVals() override;
  bool processDoValues() override;
  bool processDiValues() override;
};

#endif /* PCF8574IO_H_ */
