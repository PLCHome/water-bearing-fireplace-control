#ifndef GPIODIO_H_
#define GPIODIO_H_

#include <Arduino.h>

#include "Datatool.h"
#include "DataCare.h"
#include "dirtyp.h"

class gpioDio : public Datatool
{
private:
    dirtyp dir;
    int8_t pin;
    int8_t lo;
    int8_t pu;
public:
    bool init(DataCare *master) override;
    uint16_t getDOVals() override;
    uint16_t getDIVals() override;
    bool processDoValues() override;
    bool processDiValues() override;
};

#endif /* GPIODIO_H_ */