#ifndef BEEBDOUT_H_
#define BEEBDOUT_H_

#include <Arduino.h>

#include "timer/myTimer.h"
#include "data/DataCare.h"
#include "Datatool.h"

class beebDOut : public Datatool, myTimerCycle {
private:
  int8_t pin;
  uint8_t lo;
  bool cycle;

public:
  bool init(DataCare *master) override;
  uint16_t getDOVals() override;
  bool processDoValues() override;
  void doCycleIntervall() override;
};

#endif /* BEEBDOUT_H_ */
