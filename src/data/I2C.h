#ifndef I2C_H_
#define I2C_H_

#include <Arduino.h>
#include <Wire.h>

class I2C
{
private:
    bool active = false;
    int sda;
    int sdl;
    TwoWire *I2Cone;

public:
    void init();
    bool isActive();
    int getSDA();
    int getSDL();
    TwoWire *getTwoWire();
};

#endif /* I2C_ */