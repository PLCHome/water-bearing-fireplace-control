#include "beebDOut.h"

#include "../mySetup.h"

bool beebDOut::init(DataCare *master)
{
    Datatool::init(master);
    this->active = mysetup->getSectionValue<bool>("active", false);
    this->pin = mysetup->getSectionValue<int8_t>("pin", 2);
    if (this->active) {
        pinMode(this->pin, OUTPUT);
        digitalWrite(this->pin, LOW);
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
            digitalWrite(this->pin, relays[0]);
            memcpy(relayLast, relays, 1);
            changed=true;
        }
    }
    return changed;
}