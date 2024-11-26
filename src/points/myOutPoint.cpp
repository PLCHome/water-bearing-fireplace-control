#include "myOutPoint.h"  // Einbinden der Header-Datei
#include "myPoints.h"
#include "pointtyp.h"
#include "../data/DataCare.h"

myOutPoint::myOutPoint(JsonVariant json, pointTyp type) : myPoint(json, type)
{
    if (json["ida"].is<int>())
    {
        this->ida = json["ida"].as<int>();
    }
    if (json["op"].is<outPoint>())
    {
        this->op = json["op"].as<outPoint>();
    }
    if (json["opos"].is<int>())
    {
        this->opos = json["opos"].as<int>();
    }
    if (this->opos < 0 || this->opos >= datacare.getLenOutputs()){
        this->opos = -1;
    }
}

void myOutPoint::calcVal()
{
    ergPoint calc = TP_ERR;
    if (this->opos >= 0)
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
            datacare.getOutputs()[this->opos] = (this->on == TP_ON);
        }

        mypoints.setChanged();
    }
}

JsonVariant myOutPoint::getJson()
{
    JsonVariant doc = JsonVariant();
    doc["id"] = this->id;
    doc["name"] = this->name;
    doc["val"] = this->on;
    doc["type"] = this->type;
    return doc;
}
