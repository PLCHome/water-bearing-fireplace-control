#ifndef BEEB_H_
#define BEEB_H_

#include "data/hardware/Hardware.h"
#include <Arduino.h>

class Beeb : public Hardware {
private:
public:
  Beeb();
  void init() override;
};

#endif /* BEEB_H_ */
