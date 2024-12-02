#include "Modbus.h"

#include "../../KC868-A8S.h"
#include "../../mySetup.h"

Modbus::Modbus() : Hardware(HW_MODBUS){

}

void Modbus::init()
{
    mysetup->resetSection();
    mysetup->setNextSection("modbus");
    Hardware::init();
    switch (mysetup->getSectionValue<uint8_t>("serial", 2))
    {
    case 0:
        this->modbus_serial = &Serial;
        break;
    case 1:
        this->modbus_serial = &Serial1;
        break;
    case 2:
        this->modbus_serial = &Serial2;
        break;

    default:
        this->active = false;
        break;
    }

    if (this->active)
    {
        int8_t rs485_default_de_pin = mysetup->getSectionValue<int8_t>("rs485_de_pin", RS485_DEFAULT_DE_PIN);
        this->modbusrtu = new ModbusRTUMaster(*(this->modbus_serial), rs485_default_de_pin);

        unsigned long modbus_buad = mysetup->getSectionValue<unsigned long>("buad", MODBUS_BUAD);
        uint32_t modbus_config = mysetup->getSectionValue<uint32_t>("config", MODBUS_CONFIG);
        int8_t rs485_default_rx_pin = mysetup->getSectionValue<int8_t>("rs485_rx_pin", RS485_DEFAULT_RX_PIN);
        int8_t rs485_default_tx_pin = mysetup->getSectionValue<int8_t>("rs485_tx_pin", RS485_DEFAULT_TX_PIN);

        modbus_serial->begin(modbus_buad, modbus_config, rs485_default_rx_pin, rs485_default_tx_pin);
        modbusrtu->begin(modbus_buad, modbus_config);
        // modbus->setTimeout(5000);
    }
}

void Modbus::ModbusCleanup()
{
    if (this->isActive())
    {
        // Clear the Modbus serial buffer.
        while (modbus_serial->available() > 0)
        {
            String(modbus_serial->read());
        }
    }
}

bool Modbus::isActive() const
{
    return this->active && this->modbusrtu;
}

uint8_t Modbus::readIntValues(uint8_t typ, uint8_t id, uint16_t startAddress, uint16_t buf[], uint16_t quantity)
{
    if (this->isActive())
        if (typ == 0)
        {
            return (uint8_t)this->modbusrtu->readHoldingRegisters(id, startAddress, buf, quantity);
        }
        else
        {
            return (uint8_t)this->modbusrtu->readInputRegisters(id, startAddress, buf, quantity);
        }
    else
        return 100;
}

uint8_t Modbus::writeBoolValues(uint8_t typ, uint8_t id, uint16_t startAddress, bool buf[], uint16_t quantity)
{
    if (this->isActive())
        if (typ == 0)
        {
            return (uint8_t)this->modbusrtu->writeMultipleCoils(id, startAddress, buf, quantity);
        }
        else
            return 100;
    return 101;
}
