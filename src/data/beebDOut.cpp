#include "beebDOut.h"

#include "../mySetup.h"

bool beebDOut::init(DataCare *master)
{
    Datatool::init(master);
    this->pin = mysetup->getArrayElementValue<int8_t>("pin", 2);
    this->lo = mysetup->getArrayElementValue<int8_t>("lo", 0);
    this->active &= this->pin > -1 && master->getBeeb()->isActive();
    if (this->active) {
        pinMode(this->pin, OUTPUT);
        digitalWrite(this->pin, this->lo ? HIGH : LOW);
    }
    return true;
}

uint16_t beebDOut::getDOVals()
{
    return 1;
}

bool beebDOut::processDoValues()
{
    bool changed = false;
    if (this->active)
    {
        bool *relayLast = master->getLastOutputs(this->DOValsStart);
        bool *relays = master->getOutputs(this->DOValsStart);
        if (memcmp(relayLast, relays, 1) != 0)
        {
            digitalWrite(this->pin, this->lo ?!relays[0]:relays[0]);
            memcpy(relayLast, relays, 1);
            changed=true;
        }
    }
    return changed;
}