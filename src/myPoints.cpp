#include "myPoints.h"
#include <SPIFFS.h>

myPoints mypoints = myPoints();

myPoint *myPoints::getPoint(int id)
{
    if (this->first != NULL)
    {
        return this->first->getPoint(id);
    }
    else
    {
        return NULL;
    }
}
myPoint *myPoints::getPoint(String name)
{
    if (this->first != NULL)
    {
        return this->first->getPoint(name);
    }
    else
    {
        return NULL;
    }
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

void myPoints::calcVal()
{
    myPoint *p = this->first;
    if (p != NULL)
    {
        p->unsetCalculated();
    }
    while (p != NULL)
    {
        p->getVal();
        p = p->getNext();
    }
}

void myPoints::cleanUp()
{
    if (this->first != NULL)
    {
        myPoint *del = this->first;
        this->first = this->first->getNext();
        delete del;
    }
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
                JsonArray array = doc.to<JsonArray>();
                for (auto value : array)
                {
                    myPoint *next = NULL;
                    if (value["type"].is<pointTyp>())
                    {
                        pointTyp typ = value["type"].as<pointTyp>();
                        switch (typ)
                        {
                        case PT_TEMP:
                            next = new myTempPoint(value, this->first);
                            break;
                        case PT_TEMPT:
                            next = new myTempTPoint(value, this->first);
                            break;
                        case PT_LOGIC:
                            next = new myLogicPoint(value, this->first);
                            break;
                        case PT_OUT:
                            next = new myOutPoint(value, this->first);
                            break;
                        }
                    }
                    if (next != NULL)
                    {
                        this->first = next;
                    }
                }
            }
            file.close();
        }
    }
}