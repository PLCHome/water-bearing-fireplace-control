#ifndef MODBDOUT_H_
#define MODBDOUT_H_

#include <Arduino.h>
#include "Datatool.h"
#include "../DataCare.h"


class modbDOut : public Datatool
{
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