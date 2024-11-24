#ifndef MODBUS_H_
#define MODBUS_H_

#include <Arduino.h>
#include <ModbusRTUMaster.h>


class Modbus
{
private:
    bool active = false;
    ModbusRTUMaster *modbusrtu;
    HardwareSerial *modbus_serial;

public:
    void init();
    void ModbusCleanup();
    bool isActive();
    uint8_t readIntValues(uint8_t typ, uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity);
    uint8_t writeBoolValues(uint8_t typ, uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity);
};

#endif /* MODBUS_H_ */