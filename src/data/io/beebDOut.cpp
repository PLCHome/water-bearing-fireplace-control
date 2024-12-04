#include "data/io/beebDOut.h"

#include "KC868-A8S.h"
#include "mySetup.h"

bool beebDOut::init(DataCare *master) {
  Datatool::init(master);
  cycleInterval = 250;
  this->pin = mysetup->getArrayElementValue<int8_t>("pin", BEEP_PIN);
  this->lo = mysetup->getArrayElementValue<int8_t>("lo", 0);
  this->active &= this->pin > -1 && master->getBeeb()->isActive();
  if (this->active) {
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, this->lo ? HIGH : LOW);
  }
  return true;
}

uint16_t beebDOut::getDOVals() { return 1; }

bool beebDOut::processDoValues() {
  bool changed = false;
  if (this->active) {
    bool *relayLast = master->getLastOutputs(this->DOValsStart);
    bool *relays = master->getOutputs(this->DOValsStart);
    if (memcmp(relayLast, relays, 1) != 0) {
      this->cycle = true;
      digitalWrite(this->pin, this->lo ? !relays[0] : relays[0]);
      memcpy(relayLast, relays, 1);
      changed = true;
    }
  }
  return changed;
}

void beebDOut::doCycleIntervall() {
  if (this->active) {
    bool *relays = master->getOutputs(this->DOValsStart);
    if (relays[0]) {
      this->cycle = !this->cycle;
      digitalWrite(this->pin,
                   (this->lo ? !relays[0] : relays[0]) && this->cycle);
    }
  }
};
