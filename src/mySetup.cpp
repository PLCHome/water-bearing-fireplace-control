#include "mySetup.h"


mySetup::mySetup(){
    if (!this->file) {
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
                this->setupOK=true;
                this->section=this->doc;
            }
        }
    }
}

mySetup::~mySetup(){
    this->close();
}

void mySetup::resetSection() {
    this->section=this->doc;
}

void mySetup::setNextSection(String path) {
    this->section=this->section[path];
}

template <typename ValueTyp>
ValueTyp mySetup::getSectionValue(String name, ValueTyp defaultval){
    if (setupOK && !this->section[name].isNull())
        return this->section[name].to<ValueTyp>;
    else 
        return defaultval;
}

void mySetup::close(){
    Serial.println("mySetup close called.");
    if (this->file) {
        this->file.close();
        this->file = (File) NULL;
        this->doc.clear();
        this->setupOK=false;
    }
}
