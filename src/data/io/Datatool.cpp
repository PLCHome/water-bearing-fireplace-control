#include "data/io/Datatool.h"
#include "mySetup.h"

bool Datatool::init(DataCare *master) {
  Serial.println("Datatool init");
  this->master = master;
  this->active = mysetup->getArrayElementValue<bool>("active", false);
  return false;
}

uint16_t Datatool::getTempVals() { return 0; }

void Datatool::setTempValsStart(int16_t val) { this->tempValsStart = val; }

int16_t Datatool::getTempValsStart() { return this->tempValsStart; }

uint16_t Datatool::getDIVals() { return 0; }

void Datatool::setDIValsStart(int16_t val) { this->DIValsStart = val; }

int16_t Datatool::getDIValsStart() { return this->DIValsStart; }

uint16_t Datatool::getDOVals() { return 0; }

void Datatool::setDOValsStart(int16_t val) { this->DOValsStart = val; }

int16_t Datatool::getDOValsStart() { return this->DOValsStart; }

uint16_t Datatool::getLedVals() { return 0; }

void Datatool::setLedValsStart(int16_t val) { this->LedValsStart = val; }

int16_t Datatool::getLedValsStart() { return this->LedValsStart; }

bool Datatool::processTempValues() { return false; }

bool Datatool::processDoValues() { return false; }

bool Datatool::processDiValues() { return false; }

bool Datatool::processLedValues() { return false; }
