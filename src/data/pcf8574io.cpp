#include "pcf8574io.h"
#include "../mySetup.h"
#include "I2C.h"
#include <Wire.h>

bool pcf8574io::init(DataCare *master)
{
    Datatool::init(master);
    uint8_t adress = mysetup->getArrayElementValue<uint8_t>("adress", 34);
    I2C *i2c = master->getI2C();
    if (adress > 0 && i2c->isActive())
    {
        this->pcf8574 = new PCF8574(i2c->getTwoWire(), adress, i2c->getSDA(), i2c->getSDL());
        for (int i = 0; i < 8; i++)
        {
            this->dirb[i] = mysetup->getArrayElementValue<pcf8574dir>("dirb" + String(i), DIR_NA);
            switch (this->dirb[i])
            {
            case DIR_INPUT:
                this->input++;
                this->pcf8574->pinMode(i, INPUT);
                break;
            case DIR_OUTPUT:
                this->output++;
                this->pcf8574->pinMode(i, OUTPUT);
                break;
            }
        }
        this->pcf8574->begin();
        for (int i = 0; i < 8; i++)
        {
            if (this->dirb[i] == DIR_OUTPUT)
            {
                this->pcf8574->digitalWrite(i, HIGH);
            }
        }
        return true;
    }
    return false;
}

uint16_t pcf8574io::getDOVals()
{
    return this->output;
}

uint16_t pcf8574io::getDIVals()
{
    return this->input;
}

bool pcf8574io::processDoValues()
{
    bool changed = false;
    if (this->output > 0)
    {
        bool *relayLast = master->getLastOutputs(this->DOValsStart);
        bool *relays = master->getOutputs(this->DOValsStart);
        int8_t pos = 0;
        for (int i = 0; i < 8; i++)
        {
            if (this->dirb[i] == DIR_OUTPUT)
            {
                if (relayLast[pos] != relays[pos])
                {
                    this->pcf8574->digitalWrite(i, relays[pos] ? LOW : HIGH);
                    changed = true;
                    relayLast[pos] = relays[pos];
                }
                pos++;
            }
        }
    }
    return changed;
}

bool pcf8574io::processDiValues()
{
    bool changed = false;
    if (this->input > 0)
    {
        TA_INPUT *inputs = master->getInputs(this->DIValsStart);
        int8_t pos = 0;
        for (int i = 0; i < 8; i++)
        {
            if (this->dirb[i] == DIR_INPUT)
            {
                if (this->pcf8574->digitalRead(i) == LOW)
                {
                    if (inputs[pos] != in_on)
                    {
                        if ((inputs[pos] == in_come))
                        {
                            changed = true;
                            inputs[pos] = in_on;
                        }
                        else if ((inputs[pos] != in_come))
                        {
                            changed = true;
                            inputs[pos] = in_come;
                        }
                    }
                }
                else
                {
                    if (inputs[i] != in_off)
                    {
                        if ((inputs[pos] == in_go))
                        {
                            changed = true;
                            inputs[pos] = in_off;
                        }
                        else if ((inputs[pos] != in_go))
                        {
                            changed = true;
                            inputs[pos] = in_go;
                        }
                    }
                }
                pos++;
            }
        }
    }
    return changed;
}