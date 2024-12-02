#ifndef GPIO_H_
#define GPIO_H_

#include <Arduino.h>
#include "Hardware.h"

class Gpio : public Hardware
{
private:

public:
    Gpio();
    void init() override;
};

#endif /* GPIO_H_ */