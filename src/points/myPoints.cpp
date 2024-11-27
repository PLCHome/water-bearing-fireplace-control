#include "myPoints.h"
#include "myTempPoint.h"
#include "myTempTPoint.h"
#include "myLogicPoint.h"
#include "myOutPoint.h"
#include "myMixerPoint.h"

#include "MessageDispatcher.h"


myPoints mypoints = myPoints();

myPoint *myPoints::getPoint(int id)
{
    for (auto* point : vmypoint) {
        myPoint* mypoint = point->getPoint(id);
        if (mypoint) return mypoint;
    }
    return NULL;
}

myPoint *myPoints::getPoint(String name)
{
    for (auto* point : vmypoint) {
        myPoint* mypoint = point->getPoint(name);
        if (mypoint) return mypoint;
    }
    return NULL;
}

ergPoint myPoints::getVal(int id)
{
    myPoint *p = this->getPoint(id);
    if (p != NULL)
    {
        return p->getVal();
    }
    else
    {
        return TP_ERR;
    }
}

ergPoint myPoints::getVal(String name)
{
    myPoint *p = this->getPoint(name);
    if (p != NULL)
    {
        return p->getVal();
    }
    else
    {
        return TP_ERR;
    }
}

void myPoints::calcVal(uint32_t change)
{
    if (change & (CHANGE_TEMP | CHANGE_DI) != 0)
    {
        for (auto* p : vmypoint) {
            this->changed = false;
            p->unsetCalculated();
        }
        for (auto* p : vmypoint) {
            p->getVal();
        }

        if (this->changed)
        {
            messagedispatcher.notify(CHANGE_POINTS);
        }
    }
}

void myPoints::cleanUp()
{
    auto temp = std::move(vmypoint);
    vmypoint.clear();

    for (auto* p : temp) {
        delete p;
    }
}

void myPoints::setChanged()
{
    this->changed = true;
}

String myPoints::getJSONValue(bool obj)
{
    JsonDocument doc;
    JsonObject data = (obj) ? doc["process"].to<JsonObject>() : doc.to<JsonObject>();

    for (auto* loop : vmypoint) {
        data[String(loop->getId())] = loop->getOn();
    }

    String out;
    serializeJson(doc, out);
    return out;
}

String myPoints::getJSONValueMixer()
{
    JsonDocument doc;
    JsonVariant data = doc.to<JsonVariant>();

    for (auto* loop : vmypoint) {
        if (loop->getTyp() == PT_MIXER) 
        {
            JsonObject idoc = data[String(loop->getId())].to<JsonObject>();
            ((myMixerPoint*)loop)->getJson(idoc);
        }
    }

    String out; 
    serializeJson(doc, out);
    return out;
}

void myPoints::init() {
    messagedispatcher.registerCallback([this](uint32_t message) {
        this->calcVal(message);
    });
    this->build();
}

void myPoints::build()
{
    this->cleanUp();
    if (!SPIFFS.begin())
    {
        Serial.println("Failed to mount SPIFFS");
    }
    else
    {
        File file = SPIFFS.open(POINTSFILENAME, FILE_READ);
        if (!file)
        {
            Serial.println("There was an error opening the file points.json");
            return;
        }
        else
        {
            Serial.println("File points.json opened!");
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, file);
            if (error)
            {
                Serial.println("deserializeJson points.json error...");
            }
            else
            {
                serializeJson(doc, Serial);
                Serial.println();
                JsonArray array = doc.as<JsonArray>();
                serializeJson(array, Serial);
                Serial.println();
                Serial.println(String("Prüfe Typ: ") + String(array[2]["type"].as<String>()));
                for (auto value : array)
                {
                    serializeJson(value, Serial);
                    Serial.println();
                    myPoint *next = NULL;
                    Serial.println(String("Prüfe Typ: ") + String(value["type"].is<pointTyp>()));
                    if (value["type"].is<pointTyp>())
                    {
                        pointTyp typ = value["type"].as<pointTyp>();
                        Serial.println(String("Erzeuge Typ: ") + String(typ));
                        switch (typ)
                        {
                        case PT_TEMP:
                            next = new myTempPoint(value,typ);
                            break;
                        case PT_TEMPT:
                            next = new myTempTPoint(value,typ);
                            break;
                        case PT_LOGIC:
                            next = new myLogicPoint(value,typ);
                            break;
                        case PT_OUT:
                            next = new myOutPoint(value,typ);
                            break;
                        case PT_MIXER:
                            next = new myMixerPoint(value,typ);
                            break;
                        }
                    }
                    if (next != NULL)
                    {
                        vmypoint.push_back(next); 
                    }
                }
            }
            file.close();
        }
    }
}
