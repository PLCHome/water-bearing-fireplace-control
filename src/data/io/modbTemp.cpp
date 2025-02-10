#include "modbTemp.h"
#include "../../mySetup.h"

bool modbTemp::init(DataCare *master) {
  Serial.println("modbTemp init");
  Datatool::init(master);
  this->typ = mysetup->getArrayElementValue<uint8_t>("typ", 0);
  this->id = mysetup->getArrayElementValue<uint8_t>("id", 1);
  this->adress = mysetup->getArrayElementValue<uint16_t>("adress", 0);
  this->values = mysetup->getArrayElementValue<uint16_t>("values", 24);
  this->decimals = mysetup->getArrayElementValue<uint8_t>("decimals", 2);
  return true;
}

uint16_t modbTemp::getTempVals() { return this->values; }

void modbTemp::calcTo2Dec(int16_t *buf, int16_t *out, bool *p) {
  int16_t mul = 1;
  switch (this->decimals) {
  case 0:
    mul = 100;
    break;
  case 1:
    mul = 10;
    break;
  }

  for (int i = 0; i < this->values; i++) {
    if (!p[i])
      out[i] = buf[i] * mul;
  }
}

bool modbTemp::processTempValues() {
  bool result = false;
  Modbus *modbus = this->master->getModbus();
  uint8_t ModbusErr = MODBUS_RTU_MASTER_SUCCESS;
  //int16_t *tempHoldingRegRead = master->getLastTemeratures(this->tempValsStart);
  int16_t *tempHoldingWork = master->getLastTemeratures(this->tempValsStart);
  int16_t *tempHoldingReg = master->getTemeratures(this->tempValsStart);
  if (this->active && modbus && modbus->isActive()) {
    modbus->ModbusCleanup();
    int16_t tempHoldingRegRead[this->values];
    ModbusErr =
        modbus->readIntValues(this->typ, this->id, this->adress,
                              (uint16_t *)tempHoldingRegRead, this->values);
    if (ModbusErr == MODBUS_RTU_MASTER_SUCCESS) {
      calcTo2Dec(tempHoldingRegRead, tempHoldingWork, this->master->getNoReadTemeratures(this->tempValsStart));
    } else {
      Serial.println("Read Temps modbus error: " + String(ModbusErr));
    }
  }
  if (memcmp(tempHoldingReg, tempHoldingWork, this->values * 2) != 0) {
    memcpy(tempHoldingReg, tempHoldingWork, this->values * 2);
    result = true;
  }
  return result;
}
