#include "mySetup.h"

mySetup *mysetup = nullptr;

mySetup::mySetup()
{
    if (!this->file)
    {
        this->file = SPIFFS.open(CONFIGFILENAME, FILE_READ);
        if (!this->file)
        {
            Serial.println("There was an error opening the Setup file");
            return;
        }
        else
        {
            DeserializationError error = deserializeJson(this->doc, this->file);
            if (error)
            {
                Serial.println("deserializeJson Setup error...");
            }
            else
            {
                this->setupOK = true;
                this->resetSection();
                serializeJson(this->section, Serial);
            }
        }
    }
}

mySetup::~mySetup()
{
    this->close();
}

void mySetup::resetSection()
{
    this->section = this->doc.as<JsonVariant>();
}

void mySetup::setNextSection(String path)
{
    this->section = this->section[path];
}

bool mySetup::hasSectionValue(String name)
{
    return (setupOK && !this->section[name].isNull());
}

const char *mySetup::cstrPersists(String val)
{

    const char *cstr = (const char *)malloc(val.length() + 1);
    strcpy((char *)cstr, val.c_str());
    return cstr;
}

const char *mySetup::cstrPersistsNull(String val)
{
    if (val.isEmpty())
        return nullptr;
    else
        return this->cstrPersists(val);
}

void mySetup::close()
{
    Serial.println("mySetup close called.");
    if (this->file)
    {
        this->file.close();
        this->file = (File)NULL;
        this->doc.clear();
        this->setupOK = false;
    }
}
