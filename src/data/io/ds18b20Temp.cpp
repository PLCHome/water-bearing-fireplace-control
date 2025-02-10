#include "ds18b20Temp.h"
#include "../../mySetup.h"

bool ds18b20Temp::init(DataCare *master) {
  Datatool::init(master);
  this->id = mysetup->getArrayElementValue<String>("id", "");
  this->active &=
      master->getDs18b20()->stringToDeviceAddress(this->id, device) &&
      master->getDs18b20()->isActive();
  Serial.printf("DS18B20 %s acrive %d\n", this->id.c_str(), this->active);
  return true;
}

uint16_t ds18b20Temp::getTempVals() { return 1; }

bool ds18b20Temp::processTempValues() {
  bool result = false;

    int16_t *tempHoldingRegRead =
        master->getLastTemeratures(this->tempValsStart);
    int16_t *tempHoldingReg = master->getTemeratures(this->tempValsStart);

    if (this->active && !(this->master->getNoReadTemeratures(this->tempValsStart)[0])) {
      tempHoldingRegRead[0] = master->getDs18b20()->getTemp(this->device);
    }
    if (memcmp(tempHoldingReg, tempHoldingRegRead, 2) != 0) {
      memcpy(tempHoldingReg, tempHoldingRegRead, 2);
      result = true;
    }
  return result;
}
