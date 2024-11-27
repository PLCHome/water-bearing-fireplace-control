#include "Hardware.h"
#include <mySetup.h>

Hardware::Hardware(HardwareTyp type) : type(type)
{
}

void Hardware::init()
{
    this->active = mysetup->getSectionValue<bool>("active", false);
}

bool Hardware::isActive() const
{
    return this->active;
}

bool Hardware::getType() const
{
    return this->type;
}