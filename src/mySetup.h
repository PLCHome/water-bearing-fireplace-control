#ifndef MYSETUP_H_
#define MYSETUP_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#define CONFIGFILENAME "/config/setup.json"

class mySetup {
private:
  File file = (File)NULL;
  JsonDocument doc;
  JsonVariant section;
  JsonVariant arrayelement;
  bool setupOK = false;

public:
  mySetup();
  ~mySetup();
  void resetSection();
  void setNextSection(String);
  bool hasSectionValue(String name);
  bool isArrySection();
  uint16_t getArrayLength();
  bool setArrayElement(int16_t);
  bool hasArrayElementValue(String name);
  const char *cstrPersists(String val);
  const char *cstrPersistsNull(String val);
  void close();

  template <typename ValueTyp>
  ValueTyp getSectionValue(String name, ValueTyp defaultval) {
    if (setupOK && !this->section[name].isNull())
      return this->section[name].as<ValueTyp>();
    else
      return defaultval;
  }
  template <typename ValueTyp>
  ValueTyp getArrayElementValue(String name, ValueTyp defaultval) {
    if (setupOK && !this->arrayelement[name].isNull())
      return this->arrayelement[name].as<ValueTyp>();
    else
      return defaultval;
  }
};
extern mySetup *mysetup;

#endif /* MYSETUP_H_ */
