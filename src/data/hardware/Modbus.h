#ifndef MODBUS_H_
#define MODBUS_H_

#include "data/hardware/Hardware.h"
#include <Arduino.h>
#include <ModbusRTUMaster.h>

class Modbus : public Hardware {
private:
  ModbusRTUMaster *modbusrtu;
  HardwareSerial *modbus_serial;

public:
  Modbus();
  void init() override;
  void ModbusCleanup();
  bool isActive() const override;
  uint8_t readIntValues(uint8_t typ, uint8_t id, uint16_t startAddress,
                        uint16_t buf[], uint16_t quantity);
  uint8_t writeBoolValues(uint8_t typ, uint8_t id, uint16_t startAddress,
                          bool buf[], uint16_t quantity);
};

#endif /* MODBUS_H_ */
