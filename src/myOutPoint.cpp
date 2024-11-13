#include "myOutPoint.h"
#include "myPoints.h"

myOutPoint::myOutPoint(JsonVariant json, myPoint* next) : myPoint(json, next)
{
    if (json["ida"].is<int>())
    {
        this->ida = json["ida"].as<int>();
    }
    if (json["opos"].is<int>())
    {
        this->opos = json["opos"].as<int>();
    }
    if (json["op"].is<outPoint>())
    {
        this->op = json["op"].as<outPoint>();
    }
}

void myOutPoint::calcVal()
{
    ergPoint calc = TP_ERR;
    if (this->opos >= 0 && this->opos < RELAYS)
    {
        ergPoint pointA = mypoints.getVal(this->ida);
        if (pointA != TP_ERR)
        {
            if (this->op == O_NEG)
            {
                calc = (pointA == TP_ON) ? TP_OFF : TP_ON;
            }
            else
            {
                calc = pointA;
            }
        }
    }
    if (calc != this->on)
    {
        this->on = calc;
        if (this->on != TP_ERR)
        {
            if (this->on != TP_ERR)
                relay[this->opos] = (this->on == TP_ON);
        }
        String o = getJson();
        Serial.println(o);
        notifyClients(o);
    }
}

String myOutPoint::getJson()
{
    JsonDocument doc;
    doc["id"] = this->id;
    doc["name"] = this->name;
    doc["val"] = this->on;
    String out;
    serializeJson(doc, out);
    return out;
}