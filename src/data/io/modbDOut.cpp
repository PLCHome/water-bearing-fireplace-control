#include "modbDOut.h"

#include "../../mySetup.h"

bool modbDOut::init(DataCare *master) {
  Datatool::init(master);
  this->typ = mysetup->getArrayElementValue<uint8_t>("typ", 0);
  this->id = mysetup->getArrayElementValue<uint8_t>("id", 2);
  this->adress = mysetup->getArrayElementValue<uint16_t>("adress", 0);
  this->values = mysetup->getArrayElementValue<uint16_t>("values", 4);
  return true;
}

uint16_t modbDOut::getDOVals() { return this->values; }

bool modbDOut::processDoValues() {
  bool result = false;
  Modbus *modbus = this->master->getModbus();
  uint8_t ModbusErr = MODBUS_RTU_MASTER_SUCCESS;
  bool *relays = master->getOutputs(this->DOValsStart);
  bool *relayLast = master->getLastOutputs(this->DOValsStart);
  if (memcmp(relayLast, relays, this->values) != 0) {
    if (this->active && modbus && modbus->isActive()) {
      Serial.println("MB Write: " + String(this->typ) + " id " +
                     String(this->id) + " adress " + String(this->adress));
      modbus->ModbusCleanup();
      ModbusErr = modbus->writeBoolValues(this->typ, this->id, this->adress,
                                          relays, this->values);
      if (ModbusErr == MODBUS_RTU_MASTER_SUCCESS) {
        memcpy(relayLast, relays, this->values);
        result = true;
      } else {
        Serial.println("Write relays modbus error: " + String(ModbusErr));
      }
    }
  }
  return result;
}
