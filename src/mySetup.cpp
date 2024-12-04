#include "mySetup.h"

mySetup *mysetup = nullptr;

mySetup::mySetup() {
  if (!this->file) {
    this->file = SPIFFS.open(CONFIGFILENAME, FILE_READ);
    if (!this->file) {
      Serial.println("There was an error opening the Setup file");
      return;
    } else {
      DeserializationError error = deserializeJson(this->doc, this->file);
      if (error) {
        Serial.println("deserializeJson Setup error...");
      } else {
        this->setupOK = true;
        this->resetSection();
        serializeJson(this->section, Serial);
        Serial.println();
      }
    }
  }
}

mySetup::~mySetup() { this->close(); }

void mySetup::resetSection() {
  this->section = this->doc.as<JsonVariant>();
  this->arrayelement.clear();
}

void mySetup::setNextSection(String path) {
  this->section = this->section[path];
  this->arrayelement.clear();
}

bool mySetup::hasSectionValue(String name) {
  return (setupOK && !this->section[name].isNull());
}

bool mySetup::isArrySection() {
  return (setupOK && this->section.is<JsonArray>());
}

uint16_t mySetup::getArrayLength() {
  if (isArrySection()) {
    return this->section.as<JsonArray>().size();
  } else {
    return 0;
  }
}

bool mySetup::setArrayElement(int16_t pos) {
  if (isArrySection()) {
    JsonArray array = this->section.as<JsonArray>();
    if (pos >= 0 && pos < array.size()) {
      this->arrayelement = array[pos].as<JsonVariant>();
      Serial.println("Arrayset " + String(pos));
      return true;
    }
  }
  this->arrayelement.clear();
  return false;
}

bool mySetup::hasArrayElementValue(String name) {
  return (setupOK && !this->section[name].isNull());
}

const char *mySetup::cstrPersists(String val) {

  const char *cstr = (const char *)malloc(val.length() + 1);
  strcpy((char *)cstr, val.c_str());
  return cstr;
}

const char *mySetup::cstrPersistsNull(String val) {
  if (val.isEmpty())
    return nullptr;
  else
    return this->cstrPersists(val);
}

void mySetup::close() {
  Serial.println("mySetup close called.");
  if (this->file) {
    this->file.close();
    this->file = (File)NULL;
    this->doc.clear();
    this->setupOK = false;
  }
}
