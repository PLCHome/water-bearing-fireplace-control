#ifndef BEEB_H_
#define BEEB_H_

#include <Arduino.h>
#include "Hardware.h"

class Beeb : public Hardware
{
private:

public:
    Beeb();
    void init() override;
};

#endif /* BEEB_H_ */