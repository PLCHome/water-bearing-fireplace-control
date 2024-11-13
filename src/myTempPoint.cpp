#include "myTempPoint.h"

myTempPoint::myTempPoint(JsonVariant json, myPoint* next):myPoint(json, next)
{
    if (json["tpos"].is<int>())
    {
        this->tpos = json["tpos"].as<int>();
    }
    if (json["toff"].is<int16_t>())
    {
        this->toff = json["toff"].as<int16_t>();
    }
    if (json["ton"].is<int16_t>())
    {
        this->ton = json["ton"].as<int16_t>();
    }
}

void myTempPoint::calcVal()
{
    ergPoint calc = this->on;
    if (this->tpos >= 0 && this->tpos < TEMPHOLDINGREG)
    {
        uint16_t t = tempHoldingReg[this->tpos];
        if (this->toff <= this->ton)
        {
            if (this->on != TP_OFF && t <= this->toff)
            {
                calc = TP_OFF;
            }
            else if (this->on != TP_ON && t >= this->on)
            {
                calc = TP_ON;
            }
        }
        else
        {
            if (this->on != TP_OFF && t >= this->toff)
            {
                calc = TP_OFF;
            }
            if (this->on != TP_ON && t <= this->ton)
            {
                calc = TP_ON;
            }
        }
    }
    if (calc != this->on)
    {
        this->on = calc;
        String o = getJson();
        Serial.println(o);
        notifyClients(o);
    }
}

String myTempPoint::getJson()
{
    JsonDocument doc;
    doc["id"] = this->id;
    doc["name"] = this->name;
    doc["val"] = this->on;
    String out;
    serializeJson(doc, out);
    return out;
}