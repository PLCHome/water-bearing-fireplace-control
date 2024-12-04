#ifndef WS2812OUT_H_
#define WS2812OUT_H_

#include <Arduino.h>

#include "../DataCare.h"
#include "Datatool.h"

class ws2812out : public Datatool {
public:
  bool init(DataCare *master) override;
};

#endif /* WS2812OUT_H_ */
