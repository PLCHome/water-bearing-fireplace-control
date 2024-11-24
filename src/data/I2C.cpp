#include "I2C.h"

#include "../KC868-A8S.h"
#include "../mySetup.h"

void I2C::init()
{
    mysetup->resetSection();
    mysetup->setNextSection("i2c");
    this->sda = mysetup->getSectionValue<int>("sda", I2C_SDA);
    this->sdl = mysetup->getSectionValue<int>("sdl", I2C_SDL);

    this->active = mysetup->getSectionValue<bool>("active", "");
    if (this->active)
    {
        this->I2Cone = new TwoWire(0);
    }
}

bool I2C::isActive()
{
    return this->active && this->I2Cone;
}

int I2C::getSDA()
{
    return this->sda;
}
int I2C::getSDL()
{
    return this->sdl;
}

TwoWire *I2C::getTwoWire()
{
    return this->I2Cone;
}
