#ifndef BEEBDOUT_H_
#define BEEBDOUT_H_

#include <Arduino.h>

#include "Datatool.h"
#include "DataCare.h"

class beebDOut : public Datatool
{
private:
    int8_t pin;
    uint8_t lo;
public:
    bool init(DataCare *master) override;
    uint16_t getDOVals() override;
    bool processDoValues() override;
};

#endif /* BEEBDOUT_H_ */