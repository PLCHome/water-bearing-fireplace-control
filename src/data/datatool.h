#ifndef DATATOOL_H_
#define DATATOOL_H_

#include <Arduino.h>
#include "DataCare.h"

class DataCare;
class Datatool
{
protected:
    int16_t tempValsStart = -1;
    int16_t DIValsStart = -1;
    int16_t DOValsStart = -1;
    int16_t BeebValsStart = -1;
    int16_t LedValsStart = -1;
    DataCare* master;
public:
    virtual bool init(DataCare *master);
    virtual uint16_t getTempVals();
    virtual void setTempValsStart(int16_t);
    virtual int16_t getTempValsStart();
    virtual uint16_t getDIVals();
    virtual void setDIValsStart(int16_t);
    virtual int16_t getDIValsStart();
    virtual uint16_t getDOVals();
    virtual void setDOValsStart(int16_t);
    virtual int16_t getDOValsStart();
    virtual uint16_t getLedVals();
    virtual void setLedValsStart(int16_t);
    virtual int16_t getLedValsStart();
    virtual bool processTempValues();
    virtual bool processDoValues();
    virtual bool processDiValues();
    virtual bool processLedValues();
};

#endif /* DATATOOL_H_ */