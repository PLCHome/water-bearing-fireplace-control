#include "pcf8574io.h"
#include "../../mySetup.h"
#include "../hardware/I2C.h"
#include <Wire.h>

bool pcf8574io::init(DataCare *master) {
  Datatool::init(master);
  uint8_t adress = mysetup->getArrayElementValue<uint8_t>("adress", 34);
  loinput = mysetup->getArrayElementValue<uint8_t>("loin", 0);
  looutput = mysetup->getArrayElementValue<uint8_t>("loout", 0);

  I2C *i2c = master->getI2C();
  this->active &= adress > 0 && i2c->isActive();
  if (this->active) {
    this->pcf8574 =
        new PCF8574(i2c->getTwoWire(), adress, i2c->getSDA(), i2c->getSDL());
  }
  for (int i = 0; i < 8; i++) {
    this->dirb[i] =
        mysetup->getArrayElementValue<dirtyp>("dirb" + String(i), DIR_NA);
    switch (this->dirb[i]) {
    case DIR_INPUT:
      this->input++;
      if (this->pcf8574) {
        this->pcf8574->pinMode(i, INPUT);
      }
      break;
    case DIR_OUTPUT:
      this->output++;
      if (this->pcf8574) {
        this->pcf8574->pinMode(i, OUTPUT);
      }
      break;
    }
  }
  if (this->active) {
    this->pcf8574->begin();
    for (int i = 0; i < 8; i++) {
      if (this->dirb[i] == DIR_OUTPUT) {
        this->pcf8574->digitalWrite(i, looutput ? HIGH : LOW);
      }
    }
  }
  return true;
}

uint16_t pcf8574io::getDOVals() { return this->output; }

uint16_t pcf8574io::getDIVals() { return this->input; }

bool pcf8574io::processDoValues() {
  bool changed = false;
  PCF8574::DigitalInput digitalInput;
  uint8_t *val = (uint8_t *) &digitalInput;
  if (this->output > 0) {
    bool *relayLast = master->getLastOutputs(this->DOValsStart);
    bool *relays = master->getOutputs(this->DOValsStart);
    int8_t pos = 0;
    for (int i = 0; i < 8; i++) {
      if (this->dirb[i] == DIR_OUTPUT) {
        bool r = relays[pos];
        if (relayLast[pos] != r) {
          changed = true;
          relayLast[pos] = r;
        }
        val[i] = r ? looutput ? LOW : HIGH
                      : looutput  ? HIGH
                                  : LOW;
        pos++;
      }
    }
    if (changed && this->active) {
      this->pcf8574->digitalWriteAll(digitalInput);
    }
  }
  return changed;
}

bool pcf8574io::processDiValues() {
  bool changed = false;
  if (this->input > 0 && this->active) {
    TA_INPUT *inputs = master->getInputs(this->DIValsStart);
    int8_t pos = 0;
    PCF8574::DigitalInput digitalInput = this->pcf8574->digitalReadAll();
    uint8_t *val = (uint8_t *) &digitalInput;
    for (int i = 0; i < 8; i++) {
      if (this->dirb[i] == DIR_INPUT) {
        if (val[i] == looutput ? LOW : HIGH) {
          if (inputs[pos] != in_on) {
            if ((inputs[pos] == in_come)) {
              changed = true;
              inputs[pos] = in_on;
            } else if ((inputs[pos] != in_come)) {
              changed = true;
              inputs[pos] = in_come;
            }
          }
        } else {
          if (inputs[pos] != in_off) {
            if ((inputs[pos] == in_go)) {
              changed = true;
              inputs[pos] = in_off;
            } else if ((inputs[pos] != in_go)) {
              changed = true;
              inputs[pos] = in_go;
            }
          }
        }
        pos++;
      }
    }
  }
  return changed;
}
