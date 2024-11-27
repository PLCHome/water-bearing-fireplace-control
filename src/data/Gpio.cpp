#include "Gpio.h"

#include "../KC868-A8S.h"
#include "../mySetup.h"

Gpio::Gpio() : Hardware(HW_GIPO) {
}

void Gpio::init()
{
    mysetup->resetSection();
    mysetup->setNextSection("gpio");
    Hardware::init();
}

