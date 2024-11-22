#ifndef MYSETUP_H_
#define MYSETUP_H_

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define CONFIGFILENAME "config/setup.json"

class mySetup
{
private:
    File file = (File) NULL;
    JsonDocument doc;
    JsonDocument section;
    bool setupOK = false;
public:
    mySetup();
    ~mySetup();
    void resetSection();
    void setNextSection(String);
    template <typename ValueTyp>
    ValueTyp getSectionValue(String name, ValueTyp defaultval);
    void close();
};

mySetup mysetup;

#endif /* MYSETUP_H_ */