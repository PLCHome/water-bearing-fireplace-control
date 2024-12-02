#ifndef MODBTEMP_H_
#define MODBTEMP_H_

#include <Arduino.h>
#include "Datatool.h"
#include "../DataCare.h"


class modbTemp : public Datatool
{
private:
    uint8_t typ;
    uint8_t id;
    uint16_t adress;
    uint16_t values;
    uint8_t decimals;

public:
    bool init(DataCare *master) override;
    uint16_t getTempVals() override;
    void calcTo2Dec(int16_t *buf);
    bool processTempValues() override;
};

#endif /* MODBTEMP_H_ */