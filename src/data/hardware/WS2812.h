#ifndef WS2812_H_
#define WS2812_H_

#include "Hardware.h"
#include <Arduino.h>

class WS2812 : public Hardware {
private:
public:
  WS2812();
  void init() override;
};

#endif /* WS2812_H_ */
