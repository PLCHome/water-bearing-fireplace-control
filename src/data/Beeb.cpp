#include "Beeb.h"

#include "../KC868-A8S.h"
#include "../mySetup.h"

Beeb::Beeb() : Hardware(HW_BEEB) {
}

void Beeb::init()
{
    mysetup->resetSection();
    mysetup->setNextSection("beeb");
    Hardware::init();
}

