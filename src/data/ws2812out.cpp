#include "ws2812out.h"

#include "../mySetup.h"


bool ws2812out::init(DataCare *master)
{
    Datatool::init(master);
    //this->typ = mysetup->getArrayElementValue<uint8_t>("typ", 0);
    //this->id = mysetup->getArrayElementValue<uint8_t>("id", 1);
    //this->adress = mysetup->getArrayElementValue<uint16_t>("adress", 0);
    //this->values = mysetup->getArrayElementValue<uint16_t>("values", 24);
    //this->decimals = mysetup->getArrayElementValue<uint8_t>("decimals", 2);
    return true;
}