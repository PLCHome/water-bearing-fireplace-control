#ifndef GPIO_H_
#define GPIO_H_

#include "Hardware.h"
#include <Arduino.h>

class Gpio : public Hardware {
private:
public:
  Gpio();
  void init() override;
};

#endif /* GPIO_H_ */
