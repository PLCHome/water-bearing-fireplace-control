#include "WS2812.h"

#include "../../KC868-A8S.h"
#include "../../mySetup.h"

WS2812::WS2812() : Hardware(HW_WS2812) {
}

void WS2812::init()
{
    mysetup->resetSection();
    mysetup->setNextSection("ws2812led");
    Hardware::init();
}

