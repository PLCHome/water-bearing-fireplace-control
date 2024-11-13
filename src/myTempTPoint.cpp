#include "myTempTPoint.h"

myTempTPoint::myTempTPoint(JsonVariant json, myPoint* next):myPoint(json, next)
{
    if (json["tpos"].is<int>())
    {
        this->tpos = json["tpos"].as<int>();
    }
    if (json["t2plus"].is<int16_t>())
    {
        this->t2plus = json["t2plus"].as<int16_t>();
    }
    if (json["t2minus"].is<int16_t>())
    {
        this->t2minus = json["t2minus"].as<int16_t>();
    }
    if (json["tpos2"].is<int>())
    {
        this->tpos2 = json["tpos2"].as<int>();
    }
}

void myTempTPoint::calcVal()
{
    ergPoint calc = this->on;
    if (this->tpos >= 0 && this->tpos < TEMPHOLDINGREG && this->tpos2 >= 0 && this->tpos2 < TEMPHOLDINGREG)
    {
        int16_t t = tempHoldingReg[this->tpos];
        int16_t t2 = tempHoldingReg[this->tpos2];

        int16_t t2p = t2 + this->t2plus;
        int16_t t2m = t2 + this->t2minus;

        if ((t2p) >= (t2m))
        {
            if (this->on != TP_OFF && t >= (t2p))
            {
                calc = TP_OFF;
            }
            else if (this->on != TP_ON && t <= (t2m))
            {
                calc = TP_ON;
            }
        }
        else
        {
            if (this->on != TP_OFF && t <= (t2p))
            {
                calc = TP_OFF;
            }
            else if (this->on != TP_ON && t >= (t2m))
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

String myTempTPoint::getJson()
{
    JsonDocument doc;
    doc["id"] = this->id;
    doc["name"] = this->name;
    doc["val"] = this->on;
    String out;
    serializeJson(doc, out);
    return out;
}