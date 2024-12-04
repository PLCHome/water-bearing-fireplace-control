#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <Arduino.h>

enum HardwareTyp : int8_t {
  HW_I2C,
  HW_MODBUS,
  HW_BEEB,
  HW_WS2812,
  HW_GIPO,
  HW_DS18B20,
};

class Hardware {
protected:
  HardwareTyp type;
  bool active;

public:
  Hardware(HardwareTyp type);
  virtual void init();
  bool getType() const;
  virtual bool isActive() const;
};

#endif /* HARDWARE_H_ */
