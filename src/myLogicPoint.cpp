#include "myLogicPoint.h"

myLogicPoint::myLogicPoint(JsonVariant json, myPoint* next):myPoint(json, next)
{
    if (json["ida"].is<int>())
    {
        this->ida = json["ida"].as<int>();
    }
    if (json["logic"].is<int8_t>())
    {
        this->logic = json["logic"].as<logicPoint>();
    }
    if (json["idb"].is<int>())
    {
        this->idb = json["idb"].as<int>();
    }
}

void myLogicPoint::calcVal()
{
    ergPoint calc = this->on;
    ergPoint pointA = mypoints.getVal(this->ida);
    ergPoint pointB = mypoints.getVal(this->idb);

    if (pointA != TP_ERR || pointB != TP_ERR)
    {
        switch (logic)
        {
        case LO_AND:
            calc = (pointA == TP_ON) && (pointB == TP_ON) ? TP_ON : TP_OFF;
            break;
        case LO_OR:
            calc = (pointA == TP_ON) || (pointB == TP_ON) ? TP_ON : TP_OFF;
            break;
        case LO_ANDN:
            calc = (pointA == TP_ON) && (pointB == TP_OFF) ? TP_ON : TP_OFF;
            break;
        case LO_ORN:
            calc = (pointA == TP_ON) || (pointB == TP_OFF) ? TP_ON : TP_OFF;
            break;
        default:
            calc = TP_ERR;
        }
    }
    else
    {
        calc = TP_ERR;
    }
    if (calc != this->on)
    {
        this->on = calc;
        String o = getJson();
        Serial.println(o);
        notifyClients(o);
    }
}

String myLogicPoint::getJson()
{
    JsonDocument doc;
    doc["id"] = this->id;
    doc["name"] = this->name;
    doc["val"] = this->on;
    String out;
    serializeJson(doc, out);
    return out;
}

void calcVal(){

}

String getJson(){
    return "";
}
