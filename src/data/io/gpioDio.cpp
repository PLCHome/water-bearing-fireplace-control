#include "gpioDio.h"

#include "../../mySetup.h"

bool gpioDio::init(DataCare *master) {
  Datatool::init(master);
  this->pin = mysetup->getArrayElementValue<int8_t>("pin", -1);
  this->dir = mysetup->getArrayElementValue<dirtyp>("dir", DIR_NA);
  this->lo = mysetup->getArrayElementValue<int8_t>("lo", 0);
  this->pu = mysetup->getArrayElementValue<int8_t>("pu", 2);
  this->active &= this->pin > -1 && master->getGpio()->isActive();
  if (this->active) {
    switch (this->dir) {
    case DIR_INPUT:
      pinMode(this->pin, (this->pu) ? INPUT_PULLDOWN : INPUT_PULLUP);
      break;
    case DIR_OUTPUT:
      pinMode(this->pin, OUTPUT);
      break;
    }
    digitalWrite(this->pin, lo ? HIGH : LOW);
  }
  return true;
}

uint16_t gpioDio::getDOVals() { return (this->dir == DIR_OUTPUT) ? 1 : 0; }

uint16_t gpioDio::getDIVals() { return (this->dir == DIR_INPUT) ? 1 : 0; }

bool gpioDio::processDoValues() {
  bool changed = false;
  if (this->active && (this->dir == DIR_OUTPUT) &&
      master->getGpio()->isActive()) {
    bool *relayLast = master->getLastOutputs(this->DOValsStart);
    bool *relays = master->getOutputs(this->DOValsStart);
    if (memcmp(relayLast, relays, 1) != 0) {
      digitalWrite(this->pin, lo ? !relays[0] : relays[0]);
      memcpy(relayLast, relays, 1);
      changed = true;
    }
  }
  return changed;
}

bool gpioDio::processDiValues() {
  bool changed = false;
  if (this->active && (this->dir == DIR_INPUT) &&
      master->getGpio()->isActive()) {
    TA_INPUT *inputs = master->getInputs(this->DIValsStart);
    int8_t pos = 0;
    if (digitalRead(this->pin) == lo ? LOW : HIGH) {
      if (inputs[0] != in_on) {
        if ((inputs[0] == in_come)) {
          changed = true;
          inputs[0] = in_on;
        } else if ((inputs[0] != in_come)) {
          changed = true;
          inputs[0] = in_come;
        }
      }
    } else {
      if (inputs[0] != in_off) {
        if ((inputs[0] == in_go)) {
          changed = true;
          inputs[0] = in_off;
        } else if ((inputs[0] != in_go)) {
          changed = true;
          inputs[0] = in_go;
        }
      }
    }
  }
  return changed;
}
