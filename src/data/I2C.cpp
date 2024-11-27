#include "I2C.h"

#include "../KC868-A8S.h"
#include "../mySetup.h"

I2C::I2C() : Hardware(HW_I2C)
{
}

void I2C::init()
{
    mysetup->resetSection();
    mysetup->setNextSection("i2c");
    Hardware::init();
    this->sda = mysetup->getSectionValue<int>("sda", I2C_SDA);
    this->sdl = mysetup->getSectionValue<int>("sdl", I2C_SDL);

    if (this->active)
    {
        this->I2Cone = new TwoWire(0);
    }
}

bool I2C::isActive() const
{
    return this->active && this->I2Cone;
}

int I2C::getSDA() const
{
    return this->sda;
}
int I2C::getSDL() const
{
    return this->sdl;
}

TwoWire *I2C::getTwoWire() const
{
    return this->I2Cone;
}
