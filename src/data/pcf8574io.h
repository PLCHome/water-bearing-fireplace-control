#ifndef PCF8574IO_H_
#define PCF8574IO_H_

#include <Arduino.h>

#include "Datatool.h"
#include "DataCare.h"
#include "PCF8574.h"

class DataCare;

enum pcf8574dir : int8_t
{
    DIR_NA = 0,
    DIR_INPUT = 1,
    DIR_OUTPUT = 2
};

class pcf8574io : public Datatool
{
private:
    pcf8574dir dirb[8];
    PCF8574 *pcf8574;
    uint8_t input = 0;
    uint8_t output = 0;

public:
    bool init(DataCare *master) override;
    uint16_t getDOVals() override;
    uint16_t getDIVals() override;
    bool processDoValues() override;
    bool processDiValues() override;
};

#endif /* PCF8574IO_H_ */